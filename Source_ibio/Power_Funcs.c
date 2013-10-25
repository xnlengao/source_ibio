/*************************************************************************** 
 *	Module Name:	Power_Funcs
 *
 *	Abstract:	电源板通讯处理函数
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2013-5-7 13:36:45
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "General_Funcs.h"
#include "Serial_Funcs.h"


//串口接收缓冲
#define RECVBUFF_MAX_POWER		128

//模块对应的串口设备
int gfdSerial_Power = -1;
//串口写操作的同步信号量
sem_t semSend_Power;
//串口名称
extern unsigned char *gSerialsName[];
extern HWND ghWndMain;
BOOL AUTO_Shutdown=FALSE;
/*
	发送函数
*/
static int Power_Send(const unsigned char *datas, int datalen)
{
	int len = 0;
	int i;
	
	if(gfdSerial_Power <=0 || datalen <=0 || datas ==NULL) return -1;
		
	sem_wait(&semSend_Power);
	if(B_PRINTF) printf("port_send: ");
	for(i=0; i< datalen; i++){
		len = write(gfdSerial_Power, datas+i, 1);	//实际写入的长度
		if(B_PRINTF) printf("%.2X ", *(datas+i));
		if(len != 1){
			tcflush(gfdSerial_Power, TCOFLUSH);
			sem_post(&semSend_Power);
			return -1;
		}
// 		usleep(5000);
	}
	
	if(B_PRINTF) printf(" End.\n");
	sem_post(&semSend_Power);

	return datalen;
}

/*
	接收函数
*/
static int Power_Recv(unsigned  char *data, int datalen, int baudrate)
{
	int readlen, fs_sel;
	
	fd_set	fs_read;
	struct timeval tv_timeout;

	if(gfdSerial_Power<=0 || datalen <=0 || data ==NULL) return -1;

	FD_ZERO(&fs_read);
	FD_SET(gfdSerial_Power, &fs_read);
	tv_timeout.tv_sec =0;//TIMEOUT_SEC(datalen, baudrate);// 
	tv_timeout.tv_usec = TIMEOUT_USEC;
	
	fs_sel = select(gfdSerial_Power+1, &fs_read, NULL, NULL, &tv_timeout);
//	printf("fs_sel=%d\n",fs_sel);
	if(fs_sel)	{
		readlen = read(gfdSerial_Power, data, datalen);
	//	if(B_PRINTF) printf("%s:%d RECV Data %d.\n", __FILE__, __LINE__, readlen);
		return(readlen);
	}
	else{
		return(-1);
	}

	return (readlen);
}
#define POWER_OFF 				0
#define POWER_CLN_COMM 		1
#define POWER_CLN_FORCE  		2
#define POWER_CLN_ABNORMAL	3
#define POWER_CLN_KEY			4

int CMD_Power(BYTE info)
{
	unsigned char packet=0;
	packet |= 0x01<<7;
	switch(info){
		case 0x00:{ //power off cmd
			packet  |= 0x01;
		}break;
		case 0x01:{//Clear Communication Erro
			packet |= 0x01 << 1;
		}break;
		case 0x02:{//Clear Power Board Force Shutdown Error.
			packet |= 0x01 << 2;
		}break;
		case 0x03:{//Clear Power Board Abnormal Shutdown Error.
			packet  |= 0x01 << 3;
		}break;
		case 0x04:{//Clear Power Board Key Pressed status.
			packet  |= 0x01 << 4;
		}break;
		default:{
		}break;
	}
	packet |= (packet&0x01) << 5;
	
	Power_Send(&packet,1);
	
	return 0;
}

static int Power_Analyze(unsigned char*data,int len)
{
	int i;
	int PowerMsg;
	int batvol;
	BOOL debug=TRUE;
	static int show_power_flag=TRUE;
	//analyze -HEAD
	if(data[0]&0x01<<5) ///1:z支持两块电池, 0=支持一块电池
		if(B_PRINTF&&debug)	printf("HEAD[5]------Support 2 Batteries Info\n");
	else
		if(B_PRINTF&&debug)	printf("HEAD[5]------Support 1 Batteries Info\n");

	if(data[0]&0x01<<4){ ///1=Err-Note:电源板上次异常关闭
		if(B_PRINTF&&debug)	printf("HEAD[4]------Power Board Abnormal Shutdown At Last Time\n");
	//	CMD_Power(POWER_CLN_ABNORMAL);
	}
	if(data[0]&0x01<<3){ ///1=Err-Note:电源板上次强制关闭
		if(B_PRINTF&&debug)	printf("HEAD[3]------Power Board Force  Shutdown At Last Time\n");
	//	CMD_Power(POWER_CLN_FORCE);
	}
	if(data[0]&0x01<<2){ ///1=Err-Note:上次通讯错误关闭
		if(B_PRINTF&&debug)	printf("HEAD[2]------Communication Error Shutdown At Last time\n");
	//	CMD_Power(POWER_CLN_COMM);
	}

	if(data[0]&0x07<<2){
		IsAlm_Power(data,FALSE);
		
	}
	//printf("-----%d,%d\n",data[0]&0x01<<1,show_power_flag);
	if(data[0]&0x01<<1&&show_power_flag){ ///0=Power Key Pressed, 1=Power Key Release
		if(B_PRINTF&&debug)	printf("HEAD[1]------Power Key Release\n");
		CMD_Power(POWER_CLN_KEY);
		PostMessage(ghWndMain, MSG_CHAR, (WPARAM)'o', (LPARAM)0);
		show_power_flag=FALSE;
	}else{
		if(B_PRINTF&&debug)	printf("HEAD[1]------Power Key Pressed\n");
		if((data[0]&0x01<<1)==0)
			show_power_flag=TRUE;
	}
	
	if(data[0]&0x01<<0){ ///0=AC-Power Not Connect, 1=AC-Power Connected
		gPowerStatus.bPowerType=POWER_AC;
		if(B_PRINTF&&debug)	printf("HEAD[0]------AC-Power Connected\n");

	}else{
		gPowerStatus.bPowerType=POWER_DC;
		if(B_PRINTF&&debug)	printf("HEAD[0]------AC-Power Not Connect\n");
	}
	
	//analyze -BAT
	if(data[1]&0x01<<6) ///0=Battery1, 1=Battery2
		if(B_PRINTF&&debug)	printf("BAT[6]*******Battery2\n");
	else
		if(B_PRINTF&&debug)	printf("BAT[6]*******Battery1\n");

//	if(data[1]&0x01<<5) ///0=Ignore BAT Status(Bit4~Bit0).
//		printf("BAT Status(Bit4~Bit0)n");
//	else
//		printf("Ignore BAT Status(Bit4~Bit0).\n");

	if(data[1]&0x01<<4){ ///0=BAT Not Install, 1=BAT Installed
		if(B_PRINTF&&debug)	printf("BAT[4]*******1=BAT Installed\n");
		gPowerStatus.bHaveBat=POWER_HAVEBAT;
	}else{
		if(B_PRINTF&&debug)	printf("BAT[4]*******0=BAT Not Install\n");
		gPowerStatus.bHaveBat=POWER_NOBAT;
	}
	if(data[1]&0x01<<3) {///0=BAT Not Charge, 1=BAT Charged
		if(B_PRINTF&&debug)	printf("BAT[3]*******1=BAT Charged\n");
		gPowerStatus.bCharge=POWER_CHARGING;
	}else{
		if(B_PRINTF&&debug)	printf("BAT[3]*******0=BAT Not Charge\n");
		gPowerStatus.bCharge=POWER_NOT_CHARGE;
	}
	
	batvol=data[1]&0x07 ;///Bit0~Bit2 Battery Volume Indicate. 0 is lowest 7 is Highest
	if(B_PRINTF&&debug)	printf("BAT[2~0]*******batvol=%d\n",batvol);
	
	gPowerStatus.bBatteryStatus=batvol-1;
	if(batvol==0){
		gPowerStatus.bBatteryStatus=0;
		AUTO_Shutdown=TRUE;
	}
	//判断是否有报警
	IsLow_Power(&gPowerStatus);
	
	//analyze -REV
	static int soft_rev,hard_rev;
	static BYTE rev_flag;
	if(data[2]&0x01<<6) // 0=Hardware Revision 1=Software Revision
	{
		if(((data[2]&0x03<<4)>>4) == 0x03){
			soft_rev |= (data[2]&0x0f )<<12;	
			rev_flag |= 0x08;
		}else if(((data[2]&0x03<<4)>>4) == 0x02){
			soft_rev |= (data[2]&0x0f )<<8;	
			rev_flag |= 0x04;
		}else if(((data[2]&0x03<<4))>>4 == 0x01){//high bit
			soft_rev |= (data[2]&0x0f) <<4;
			rev_flag |= 0x02;
		}else{//low bit
			soft_rev |= data[2]&0x0f; 
			rev_flag |= 0x01;
		}
		if((rev_flag&0x0f)==0x0f){
			SystemStatus.wPowerSV=soft_rev;
			if(B_PRINTF&&debug)	printf("REV+++++++soft_rev=%.4x   \n",soft_rev);	
		}
		
	}else{
	
		if(((data[2]&0x03<<4)>>4) == 0x03){
			hard_rev |= (data[2]&0x0f) <<12;	
			rev_flag |= 0x80;
		}else if(((data[2]&0x03<<4)>>4) == 0x02){
			hard_rev |= (data[2]&0x0f )<<8;	
			rev_flag |= 0x40;
		}else if(((data[2]&0x03<<4))>>4 == 0x01){//high bit
			hard_rev |= (data[2]&0x0f) <<4;
			rev_flag |= 0x20;
		}else{//low bit
			hard_rev |= data[2]&0x0f; 
			rev_flag |= 0x10;
		}
		if((rev_flag&0xf0)==0xf0){
			SystemStatus.wPowerHV=hard_rev;
			if(B_PRINTF&&debug)	printf("REV+++++++hard_rev=%.4x    \n",hard_rev);	
			
		}
	}

	
	return 0;
}

static int Power_Repack(int length, const unsigned char *buff)
{
	static unsigned char data[10]={0};          //存放从buff里拆分出来的数据
	static int num=0;
	int i=0;
	int sum;
	
	while(i<length){
		if(buff[i]&0x01<<7){//head
                   //     if(B_PRINTF) printf("Debug data start%d.\n",length);
			for(num=0;num<4;num++){			
				data[num] = buff[i];
				i++;
			}
			if(num==4){
				sum=(data[0]+data[1]+data[2])&0x7f;
				if(sum==data[3])
					Power_Analyze( data,num);
			}
			
		}else{
			i++;
			
		}
	}
	
	return 0;
}



/*
	接收线程
*/
int Proc_Recv_Power()
{
	int iLen; 
	static int power_timeout;
	unsigned char buf;
 	unsigned char bBuff[RECVBUFF_MAX_POWER];	
	
	iLen = Power_Recv(bBuff, RECVBUFF_MAX_POWER, 9600);

	if(iLen>0){
			Power_Repack(iLen,bBuff);
			power_timeout=0;
		//	Power_Analyze(bBuff,iLen);
	}
	else{
		power_timeout++;
		if(power_timeout > 3){
			buf  |= 0x01<<2;
			IsAlm_Power(&buf,FALSE);
			power_timeout=0;
		}
		
		if(B_PRINTF) printf("(%s:%d) %s ID:%d receive error,,%d.\n\n", __FILE__, __LINE__, gSerialsName[DEV_SERIAL3], gfdSerial_Power,power_timeout);
		
	}

	return 0;
}


//初始化电源模块
int Init_Prower()
{
	int res;
	//初始化信号量
	res = sem_init(&semSend_Power, 0, 1);	

	//打开串口设备
	gfdSerial_Power = port_open(gSerialsName[DEV_SERIAL3], "9600", 8, 0, 1, 0);
	
	if(gfdSerial_Power<0){
		if(B_PRINTF) printf("%s:%d Serial_Power ERROR!\n", __FILE__, __LINE__);
		return -1;
	}else{
		if(B_PRINTF) printf("%s:%d Serial_Power=%d OK!\n", __FILE__, __LINE__,gfdSerial_Power);
		
	}
	
	return 0;
}

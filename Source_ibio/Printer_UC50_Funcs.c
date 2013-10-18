/*************************************************************************** 
 *	Module Name:   UC50	Printer_Funcs
 *
 *	Abstract:	打印机接口函数
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2013-4-27 13:36:45
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "General_Funcs.h"
#include "AlarmManage.h"
#include "Serial_Funcs.h"
#include "Printer_Funcs.h"


//print cmd define
#define HEAD_CMD_SET				0xFE			//设置命令包头
#define END_CMD_SET				0xFD			//设置命令包尾
#define HEAD_CMD_WAVE				0xFA			//波形打印包头
#define HEAD_CMD_WORD				0xFB			//字符打印包头
#define HEAD_CMD_IMAGE				0xFC			//图像打印包头

#define START_PRINT  				0x01 			//开始打印
#define STOP_PRINT  					0x02 			//停止打印
#define SET_MODE  					0x03 			//打印模式设置命令 
#define SET_WAVE_INTERPOLATION  	0x04			//曲线插值命令
#define SET_WAVE					0x05			//曲线设置命令 
#define SET_WAVE_SPEED  			0x06			//曲线速度设置命令 
#define SET_WAVE_GRID 				0x07			//打印栅格命令
#define SET_WORD_WIDTH 			0x08			//文字拉宽命令
#define SET_WORD_VERTICAL 			0x09			//打印纵向文字命令 
#define MOVE_PAPER  				0x0A			//走纸命令 
#define SET_IMAGE  					0x0C			//图形模式分辨率设置命令
#define RESET_PRINTER  				0x0D			//复位命令

//串口接收缓冲
#define RECVBUFF_MAX_PRINTER		128
#define PRINT_BUFF_MAX				246 			//能同时被1 2 3整除的数，最大为250

//打印文字缓冲区设定
#define MAX_PRT_ROW_COUNT		14  			//最多打印字串的行数
#define MAX_PRT_TEXT_LENGTH		200			//字串的长度

//打印波形缓冲器设定
#define MAX_PRT_REALWAVE_COUNT	3			//波形数量
#define MAX_PRT_REALWAVE_LENGTH	20000		//最多存储长度

//打印设置波形模式
#define PRN_MODE_TEST		0			//自检模式
#define PRN_MODE_WAVE		1			//波形模式
#define PRN_MODE_WORD		2			//文字模式
#define PRN_MODE_IMAGE		3			//图像模式



//打印机ID
//TODO:现主板只有一个TTL 接口，和现在参数板接口冲突，暂时不可用
int gSerialIndex_Printer = DEV_SERIAL1;		//TTL 电平
//串口名称
extern unsigned char *gSerialsName[];
//模块对应的串口设备
int gfdSerial_Printer = -1;

//串口写操作的同步信号量
sem_t semSend_Printer;

//串口接收线程ID
pthread_t ptID_iRecv_Printer;

//实时波形打印的同步信号量
sem_t semReal_Print;

//实时波形打印线程ID
pthread_t ptID_Real_Print;

extern ALARMDATA gAlmDataForReview;

/*
	发送函数
*/
static int Printer_Send(const unsigned char *datas, int datalen)
{
	int len = 0;
	int i;
	
	if(gfdSerial_Printer <=0 || datalen <=0 || datas ==NULL) return -1;
		
	sem_wait(&semSend_Printer);
	if(B_PRINTF) printf("port_send: ");
	for(i=0; i< datalen; i++){
		len = write(gfdSerial_Printer, datas+i, 1);	//实际写入的长度
		if(B_PRINTF) printf("%.2X ", *(datas+i));
		if(len != 1){
			tcflush(gfdSerial_Printer, TCOFLUSH);
			sem_post(&semSend_Printer);
			return -1;
		}
// 		usleep(5000);
	}
	
	if(B_PRINTF) printf(" End.\n");
	sem_post(&semSend_Printer);

	return datalen;
}



/*
	开始打印命令 
*/
static int CMD_Start_Print()
{
	unsigned char packet[10];
	
	packet[0]=HEAD_CMD_SET;
	
	packet[1]=START_PRINT;
	
	packet[2]=END_CMD_SET;
	
	
	Printer_Send(packet,3);
	
	if(B_PRINTF) printf("printer Send CMD_Start_Print\n");
	

	return 0;
}
/*
	结束打印命令 
*/
static int CMD_Stop_Print()
{
	unsigned char packet[10];
	
	packet[0]=HEAD_CMD_SET;


	packet[1]=STOP_PRINT;

	
	packet[2]=END_CMD_SET;
	Printer_Send(packet,3);
	
	if(B_PRINTF) printf("printer Send CMD_Stop_Print\n");
	
	return 0;
}

/*
	打印模式设置命令 
*/
int CMD_Set_Mode(BYTE Mode)
{
	unsigned char packet[10];
	
	packet[0]=HEAD_CMD_SET;
	packet[1]=SET_MODE;
	switch(Mode){
		case 0://自检模式
			packet[2]=0x00;
		break;
		case 1://曲线模式
			packet[2]=0x01;
		break;	
		case 2://文字模式
			packet[2]=0x02;
		break;	
		case 3://图形方式
			packet[2]=0x03;
		break;	
		default:
			packet[2]=0x00;
		break;
	}
	packet[3]=END_CMD_SET;
	Printer_Send(packet,4);
	printf("printer send cmd set mode\n");

	return 0;
}

/*
	曲线插值命令 
*/
static int CMD_Set_Wave_Interpolation(BYTE Num)
{
	unsigned char packet[10];
	
	packet[0]=HEAD_CMD_SET;

	packet[1]=SET_WAVE_INTERPOLATION;
	
	packet[2]=Num;
	switch(Num){
		case 0://插入0个点，曲线原样输出
			packet[2]=1;
		break;
		case 1://插入1个点，曲线拉宽为原来2倍 
			packet[2]=2;
		break;	
		case 2://插入2个点，曲线拉宽为原来3倍
			packet[2]=4;
		break;	
		default:
			packet[2]=1;
		break;
	}
	
	packet[3]=END_CMD_SET;
	
	Printer_Send(packet,4);
	
	if(B_PRINTF) printf("printer Send CMD_Set_Wave_Interpolation\n");
	
	
	return 0;
}

/*
	曲线设置命令 
*/
static int CMD_Set_Wave(BYTE WaveNum)
{
	int ilen;
	unsigned char packet[10]={0};
	
	packet[0]=HEAD_CMD_SET;

	packet[1]=SET_WAVE;

	if(WaveNum<1&&WaveNum>3) //波形数量不是1~3条
		WaveNum=1;
		
//	packet[2] |= 0x01<< 7; //最高位置1
	packet[2] |= WaveNum<<1; 
	printf("packet[2]=0x%.2x,WaveNum=%d\n",packet[2],WaveNum);
	
	switch(WaveNum){//设置基线位置//TODO 基线为中线或底线未确定
		case 1:
			packet[3] = 5;
			ilen=4;
		break;
		case 2:
			packet[3] = 25;
			packet[4] = 5;
			ilen=5;
		break;
		case 3:
			packet[3] = 24;
			packet[4] = 14;
			packet[5] = 4;
			ilen=6;
		break;
		default:
			packet[3] = 5;
			ilen=4;
		break;
	}
	
	
	packet[ilen]=END_CMD_SET;
	ilen++;
	
	Printer_Send(packet,ilen);
	
	if(B_PRINTF) printf("printer Send CMD_Set_Wave_Interpolation\n");
	

	return 0;
}
/*
	曲线速度设置命令 
*/
static int CMD_Set_Wave_Speed(BYTE speed)
{
	unsigned char packet[10];
	
	packet[0]=HEAD_CMD_SET;

	packet[1]=SET_WAVE_SPEED;
	switch(speed){
		case 0://12.5mm/s
			packet[2]=12;
		break;
		case 1://25mm/s
			packet[2]=25;
		break;
		case 2://50mm/s
			packet[2]=50;
		break;
		default:
			packet[2]=12;
		break;
	}
	
	packet[3]=END_CMD_SET;
	
	Printer_Send(packet,4);
	
	if(B_PRINTF) printf("printer Send CMD_Set_Wave_Speed\n");
	

	return 0;
}

/*
	打印栅格命令 
*/
static int CMD_Set_Wave_Grid()
{
	unsigned char packet[10];
	
	packet[0]=HEAD_CMD_SET;

	packet[1]=SET_WAVE_GRID;
	
	packet[2]=END_CMD_SET;
	
	Printer_Send(packet,3);
	
	if(B_PRINTF) printf("printer Send CMD_Set_Wave_Grid\n");
	

	return 0;
}

/*
	打印纵向文字命令 
*/
static int CMD_Set_Word_Vertical()
{

	unsigned char packet[10];
	
	packet[0]=HEAD_CMD_SET;

	packet[1]=SET_WORD_VERTICAL;
	
	packet[2]=END_CMD_SET;
	
	Printer_Send(packet,3);
	
	if(B_PRINTF) printf("printer Send CMD_Set_Word_Vertical\n");
	

	return 0;
}

/*
	 走纸命令 ，长度为length mm
*/
static int CMD_Move_Paper(BYTE Length)
{

	unsigned char packet[10];
	
	packet[0]=HEAD_CMD_SET;

	packet[1]=MOVE_PAPER;

	packet[2]=Length;
	
	packet[3]=END_CMD_SET;
	
	Printer_Send(packet,4);
	
	if(B_PRINTF) printf("printer Send CMD_Move_Paper\n");
	

	return 0;
}

/*
	图形模式分辨率设置命令 默认为8dot/mm,发送此命令成为16dot/mm
*/
static int CMD_Set_Image()
{
	
	unsigned char packet[10];
	
	packet[0]=HEAD_CMD_SET;

	packet[1]=SET_IMAGE;

	packet[2]=END_CMD_SET;
	
	Printer_Send(packet,3);
	
	if(B_PRINTF) printf("printer Send CMD_Set_Image\n");
	

	return 0;
}
/*
	复位命令 
*/
static int CMD_Reset_Printer()
{
	unsigned char packet[10];
	
	packet[0]=HEAD_CMD_SET;

	packet[1]=RESET_PRINTER;

	packet[2]=END_CMD_SET;
	
	Printer_Send(packet,3);
	
	if(B_PRINTF) printf("printer Send CMD_Reset_Printer\n");
	
	return 0;
}

/*
	复位命令 
*/
int Printer_Move_Paper(BYTE length)
{
	if(length>0)CMD_Move_Paper(length);
	
	return 0;
}

int Set_Printer_Speed()
{
	CMD_Set_Wave_Speed(gCfgPrinter.bSpeed);
	
	return 0;
}
int Stop_Printing()
{
	CMD_Stop_Print();
	
	return 0;
}
int Start_Printing()
{
	CMD_Start_Print();
	return 0;
}

int Set_Printer_Interpolation(BYTE num)
{
	
	CMD_Set_Wave_Interpolation(num);
	
	return 0;
}
int Set_Vertical_Word()
{
	CMD_Set_Word_Vertical();
	return 0;
}

int Set_Image_Print()
{
	CMD_Set_Image();
	return 0;
}
int Set_Mode(BYTE mode)
{
	if(mode>3)
		return -1;
	
	CMD_Set_Mode(mode);
	
	return 0;

}
int Reset_Printer()
{
	CMD_Reset_Printer();
	gPrinterWorkType = PRNWORK_NONE;
	gPrintingType = PRN_WORK_NONE;
	return 0;
}


static int  StringTransferNoFormat( PREC_STRING pdata, PREC_STRINGT  ptdata )
{
    REC_STRINGT    stringt1, stringt2;
    int	 i = 0, j = 0;
    int  len = 0, MaxLen = 0;

    memset(&stringt1, 0, sizeof(stringt1));
    memset(&stringt2, 0, sizeof(stringt2));

    stringt1.iRowCount = pdata->iRowCount;

    for(i = 0; i < stringt1.iRowCount; i++) {
	len = strlen(pdata->strText[i]);
	if(len > MaxLen)
	    MaxLen = len;
    }
    stringt1.iMaxLen = MaxLen;


    for(i = 0; i < stringt1.iRowCount; i++)
	for(j = 0; j < stringt1.iMaxLen; j++)  {
	    stringt1.strText[i][j] = pdata->strText[i][j];
	    if ( strlen(stringt1.strText[i]) <= j )
		stringt1.strText[i][j] = 0x20;
	}   

    for(i = stringt1.iRowCount; i < MAX_ROW_COUNT; i++)
	memset(stringt1.strText[i], 0x20, stringt1.iMaxLen);

    stringt2 = stringt1;
    stringt2.iRowCount = MAX_ROW_COUNT;
    for(i = 0; i < MAX_ROW_COUNT; i++)
	memcpy(stringt2.strText[i], stringt1.strText[MAX_ROW_COUNT - 1 - i], stringt1.iMaxLen);

  
    *ptdata =  stringt2;


    return 0 ;

}



static int Print_Word(BYTE *str_asc)
{
	int i,ilen;
	unsigned char packet[60];
	
	packet[0]=HEAD_CMD_WORD;

	packet[1]=MAX_PRT_ROW_COUNT;

	for(i=0;i<MAX_PRT_ROW_COUNT;i++){
		packet[2+i]=2+i*3;
	}
	
	for(i=0;i<MAX_PRT_ROW_COUNT;i++){
		packet[MAX_PRT_ROW_COUNT+2+i]=str_asc[i];
	}
	
	ilen = MAX_PRT_ROW_COUNT*2+2;
	
	Printer_Send(packet,ilen);
	
	return 0;
}

static int Print_Wave(const BYTE  *wave,int wave_ilen)
{
	int i,ilen;
	unsigned char packet[260];
	packet[0]=HEAD_CMD_WAVE;

	packet[1]=wave_ilen;

	for(i=0;i<wave_ilen;i++){
		packet[2+i]=wave[i];
	}

	ilen=wave_ilen+2;
	
	Printer_Send(packet,ilen);
	return 0;
}
int Printer_PrintString(const PREC_STRING pData)
{
	REC_STRINGT stringt1;
    	int	 i = 0;
	int icount=0;
	BYTE str_asc[MAX_PRT_ROW_COUNT];
	memset(&stringt1, 0, sizeof(stringt1));
	

	CMD_Set_Mode(PRN_MODE_WORD);
	
	StringTransferNoFormat( pData, &stringt1);
	
	/*
	for(j=0;j<stringt1.iMaxLen;j++){
		for(i=0;i<stringt1.iRowCount;i++){
			str_asc[i]=stringt1.strText[i][j];
		}	
		Print_Word(str_asc);
	}
	*/
	
	icount=stringt1.iMaxLen;
		
	while(icount>0&&gPrintingType!= PRN_WORK_NONE){
		if(gPrinterStatus != PRINT_STATUS_BUSY&&gPrinterStatus != PRINT_STATUS_ERR){
			printf("%s,%d,************print alarm data %d,%d\n",__FILE__,__LINE__,icount,stringt1.iRowCount);
			for(i=0;i<stringt1.iRowCount;i++)
				str_asc[i]=stringt1.strText[i][stringt1.iMaxLen-icount];
			
			Print_Word(str_asc);
			icount--;

		}
	usleep(10000);
	}

	return 0;
}

int Printer_PrintCopyWave(const PREC_COPYWAVE pData)
{
	
    	int i = 0, j = 0;
	int icount=0,istep=0;
	BYTE wave_value[MAX_PRT_REALWAVE_LENGTH]={0};
	BYTE wave[PRINT_BUFF_MAX]={0};
	

	CMD_Set_Mode(PRN_MODE_WAVE);
	CMD_Set_Wave(3);
	CMD_Set_Wave_Speed(1);
	CMD_Set_Wave_Grid();

	for(j =0; j < pData->iLength; j++){
		for(i = 0; i < pData->iCount ; i++){
			wave_value[icount] = pData->iWave[i][j];
			icount++;
		}	
	}
	
	printf("*icount=%d**Count=%d*iLength=%d**gPrinterWorkType=%d,*gPrinterStatus=%d**\n",icount,pData->iCount,pData->iLength,gPrinterWorkType,gPrinterStatus);

	while(icount > 0&&gPrinterWorkType!= PRNWORK_NONE){
		
		if(gPrinterStatus != PRINT_STATUS_BUSY&&gPrinterStatus != PRINT_STATUS_ERR){
			
			if(icount > PRINT_BUFF_MAX){
				
				printf("11icount=%d\n",icount);
				
				for(i=0;i<PRINT_BUFF_MAX;i++){
					wave[i]=wave_value[i+istep*PRINT_BUFF_MAX];
				}
				
				Print_Wave(wave,PRINT_BUFF_MAX);
				icount=icount-PRINT_BUFF_MAX;
				istep++;
			}else{
			
				for(i=0;i<icount;i++){
					wave[i]=wave_value[i+istep*PRINT_BUFF_MAX];
				}
				
				printf("22icount=%d\n",icount);
				
				Print_Wave(wave,icount);
				icount=0;
				istep=0;
			}
			
		}
		
	//	gPrinterStatus=PRINT_STATUS_ASK;
		
		usleep(20000);
		
	}
	
	Stop_Printing();
	
	return 0;
}
/*
	打印实时波形
*/
int Printer_PrintRealWave(const PREC_REALWAVE pData)
{
	
    	int i = 0, j = 0;
	int icount=0,istep=0;
	BYTE wave_value[MAX_PRT_REALWAVE_LENGTH]={0};
	BYTE wave[PRINT_BUFF_MAX]={0};
	

	CMD_Set_Mode(1);
	CMD_Set_Wave(3);
	CMD_Set_Wave_Speed(gCfgPrinter.bSpeed);
	CMD_Set_Wave_Interpolation(gCfgPrinter.bSpeed);
	CMD_Set_Wave_Grid();

	for(j =0; j < pData->iLength; j++){
		for(i = 0; i < pData->iCount ; i++){
			wave_value[icount] = pData->iWave[i][j];
			icount++;
		}	
	}
	
//	printf("real wave*icount=%d**Count=%d*iLength=%d**gCfgPrinter.bSpeed=%d***\n",icount,pData->iCount,pData->iLength,gCfgPrinter.bSpeed);

	while(icount > 0&&gPrinterWorkType != PRNWORK_NONE){
		
		if(gPrinterStatus != PRINT_STATUS_BUSY&&gPrinterStatus != PRINT_STATUS_ERR){
			
			if(icount > PRINT_BUFF_MAX){
				
				for(i=0;i<PRINT_BUFF_MAX;i++){
					wave[i]=wave_value[i+istep*PRINT_BUFF_MAX];
				}
				
				Print_Wave(wave,PRINT_BUFF_MAX);
				icount=icount-PRINT_BUFF_MAX;
				istep++;
			}else{
			
				for(i=0;i<icount;i++){
					wave[i]=wave_value[i+istep*PRINT_BUFF_MAX];
				}
				
				Print_Wave(wave,icount);
				icount=0;
				istep=0;
			}
			
		}
		
		
	}
	
	
	return 0;
}
/*

extern unsigned int DEMO_SPO2[152];
extern unsigned int DEMO_RESP[754];
extern unsigned int DEMO_ECG_I[376];

int   Printer_PrintCopyWave_test()
{
	int i = 0;
	int icount=0,istep=0;
	BYTE wave_value[MAX_PRT_REALWAVE_LENGTH]={0};
	BYTE wave[PRINT_BUFF_MAX]={0};

	
	CMD_Set_Mode(1);
	CMD_Set_Wave(3);
	CMD_Set_Wave_Speed(1);
	
	for(i=0;i<754;i++){
		
		
		wave_value[icount]=(DEMO_ECG_I[i%376]>>4);
		icount++;
		wave_value[icount]=DEMO_SPO2[i%152];
		icount++;
		wave_value[icount]=DEMO_RESP[i%754];
		icount++;
		
		
		
		wave_value[icount]=DEMO_SPO2[i%152];
		icount++;
		
		
	}

	while(icount > 0){
		
		if(gPrinterStatus != PRINT_STATUS_BUSY&&gPrinterStatus != PRINT_STATUS_ERR){
			
			if(icount > PRINT_BUFF_MAX){
				
				printf("11icount=%d\n",icount);
				
				for(i=0;i<PRINT_BUFF_MAX;i++){
					wave[i]=wave_value[i+istep*PRINT_BUFF_MAX];
				}
				
				Print_Wave(wave,PRINT_BUFF_MAX);
				icount=icount-PRINT_BUFF_MAX;
				istep++;
			}else{
			
				for(i=0;i<icount;i++){
					wave[i]=wave_value[i+istep*PRINT_BUFF_MAX];
				}
				printf("22icount=%d\n",icount);
				Print_Wave(wave,icount);
				icount=0;
			}
			gPrinterStatus=PRINT_STATUS_ASK;
		}
		
		usleep(10000);
		
	}
	
	Stop_Printing();

	return 0;
}
*/



/*
	接收函数
*/
static int Printer_Recv(unsigned  char *data, int datalen, int baudrate)
{
	int readlen, fs_sel;
	
	fd_set	fs_read;
	struct timeval tv_timeout;

	if(gfdSerial_Printer<=0 || datalen <=0 || data ==NULL) return -1;

	FD_ZERO(&fs_read);
	FD_SET(gfdSerial_Printer, &fs_read);
	tv_timeout.tv_sec = TIMEOUT_SEC(datalen, baudrate);
	tv_timeout.tv_usec = TIMEOUT_USEC;
	
	fs_sel = select(gfdSerial_Printer+1, &fs_read, NULL, NULL, &tv_timeout);
//	printf("fs_sel=%d\n",fs_sel);
	if(fs_sel)	{
		readlen = read(gfdSerial_Printer, data, datalen);
	//	if(B_PRINTF) printf("%s:%d RECV Data %d.\n", __FILE__, __LINE__, readlen);
		return(readlen);
	}
	else{
		return(-1);
	}

	return (readlen);
}


int Printer_Analyze(unsigned char*data,int len)
{
	int i;
	
	for(i=0;i<len;i++){
		gPrinterStatus=data[i];
		if(B_PRINTF)	printf("len=%d,gPrinterStatus=%d\n",len,gPrinterStatus);
	}
	
	return 0;
}


/*
	接收线程
*/
void  *ptProc_Recv_Printer(void *arg)
{
	int iLen; 
 	unsigned char bBuff[RECVBUFF_MAX_PRINTER];	
	
	if(B_PRINTF) printf("(%s:%d)create read thread for %s .\n", __FILE__, __LINE__, gSerialsName[gSerialIndex_Printer]);
	
	for(;;){

		iLen = Printer_Recv(bBuff, RECVBUFF_MAX_PRINTER, 115200);

		if(iLen>0){
		
 			Printer_Analyze(bBuff,iLen);
		}
		else{
		//	if(B_PRINTF) printf("(%s:%d) %s ID:%d receive error.\n\n", __FILE__, __LINE__, gSerialsName[gSerialIndex_iBio], gSerialIndex_iBio);
			
		}
	}
	
	if(B_PRINTF) printf("\nBye from %d thread.\n", gfdSerial_Printer);
	pthread_exit("Bye");
}

void  *ptProc_Real_Print(void *arg)
{

	for(;;){
		
	
		sem_wait(&semReal_Print);
		printf("fasdfasdfasdfasdfasdfa\n");
		switch(gPrintingType){
			case PRN_WORK_REAL://实时波形信息打印
				Print_Info_Real_Wave();
			break;
			case PRN_WORK_ALM://报警回顾打印
				Print_AlmReview_Data();
			break;
			case PRN_WORK_TREND://趋势列表打印
				PrintTrendTable();
			break;
			case PRN_WORK_NIBP://nibp列表打印
				PrintNIBPTable();
			break;
			case PRN_WORK_ALMPRINT://报警打印
				Print_Alm_Data();
			break;
			default://没有打印内容
			break;
		}
		
		gPrintingType=PRN_WORK_NONE;
		
	}
	
	pthread_exit("Bye");

}

/*
	初始化测量模块
*/
int Init_Printer_UC50()
{
	int res;
		
	if(B_PRINTF) printf("%s:%d Init Printer UC50 ...\n", __FILE__, __LINE__);
	//初始化信号量
	res = sem_init(&semSend_Printer, 0, 1);	


	//打开串口设备
	gfdSerial_Printer = port_open(gSerialsName[gSerialIndex_Printer], "115200", 8, 0, 1, 0);
	
	if(gfdSerial_Printer<0){
		if(B_PRINTF) printf("%s:%d Serial_Printer ERROR!\n", __FILE__, __LINE__);
		return -1;
	}else{
		if(B_PRINTF) printf("%s:%d Serial_Printer=%d OK!\n", __FILE__, __LINE__,gfdSerial_Printer);
		Reset_Printer();
	}


//------------打印机数据接收线程-------------------------------------------------	
		

	//注册接收线程 
	res = pthread_create(&ptID_iRecv_Printer, NULL, ptProc_Recv_Printer, NULL);
	if(res<0){
		if(B_PRINTF) printf("%s:%d Printer Recv pthread ERROR!\n", __FILE__, __LINE__);
		return -1;
	}else
		if(B_PRINTF) printf("%s:%d OK!\n", __FILE__, __LINE__);


//------------实时波形打印线程-------------------------------------------------
	//初始化信号量
	res = sem_init(&semReal_Print, 0, 1);	
		
	//注册接收线程 
	res = pthread_create(&ptID_Real_Print, NULL, ptProc_Real_Print, NULL);
	if(res<0){
		if(B_PRINTF) printf("%s:%d Printer Recv pthread ERROR!\n", __FILE__, __LINE__);
		return -1;
	}else
		if(B_PRINTF) printf("%s:%d OK!\n", __FILE__, __LINE__);

	
	return 0;

}



/*************************************************************************** 
 *	Module Name:	NewComiBio
 *
 *	Abstract:	新参数版iBio 模块通信程序
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *		hejp	2013-7-17 9:02:35
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Serial_Funcs.h"
#include "General_Funcs.h"
#include "Central.h"
#include "ComiBio.h"
#include "NewDrawWave.h"


#define DATAID 	0
#define DATAHEAD 	1

//包定义
#define ID_ECG_WAV1 			0x01
#define ID_ECG_WAV2			0x02
#define ID_RESP_WAV 		0x03
#define ID_RESP_STA 			0x04
#define ID_ST_TEMPLET 		0x05
#define ID_ECG_HR 	 		0x06
#define ID_ECG_PVC 	 		0x07
#define ID_ST_VALUE1  	 	0x08
#define ID_ST_VALUE2 	 	0x09
#define ID_ST_ISO_VAL  	 	0x0a
#define ID_ECG_ARR  	 		0x0b
#define ID_ECG_STATE 	 	0x0c
#define ID_SPO2_WAV 			0x10
#define ID_SPO2_DAT  			0x11
#define ID_TEMP_DAT 			0x12
#define ID_NIBP_DAT 			0X13
#define ID_NIBP_CUFF			0X14
#define ID_NIBP_CAL			0X15
#define ID_NIBP_FDB			0X16
#define ID_IBP12_WAV 		0X17
#define ID_IBP34_WAV			0X18
#define ID_IBP12_DAT			0X19
#define ID_IBP34_DAT			0X1a
#define ID_IBP_FDB 			0X1b
#define ID_IBP_TIM  			0X1c
#define ID_UNIT_INFOS  		0X1d
#define ID_PROD_SN   			0X1e

/**************NIBP*CMD**********************/
#define  CMD_PATIENT_INFO 		0x00
#define  CMD_ECG_LEAD  			0x01
#define  CMD_ECG_BW  			0x02
#define  CMD_ECG_CAL  			0x03
#define  CMD_PACE_DET  			0x04
#define  CMD_RESP_LEAD  		0x05
#define  CMD_ST_ISO  				0x06
#define  CMD_ST_TEMP  			0x07
#define  CMD_RESP_GAIN   			0x08
#define  CMD_ECG_LEARN  		0x09	
#define  CMD_SPO2_SEN			0x0a
#define  CMD_NIBP_START			0x0b
#define  CMD_NIBP_STOP			0x0c
#define  CMD_NIBP_RESET			0x0d
#define  CMD_NIBP_PRSURE		0x0e
#define  CMD_NIBP_PVT			0x0f
 #define CMD_IBP_MODE 			0x10
#define  CMD_IBP_ZERO 			0x11
#define  CMD_IBP_CAL 			0x12
#define  CMD_REQ_INFO 			0x13

/******************************************/
//串口名称
extern unsigned char *gSerialsName[];
//MemDC
extern HDC hdcWaveMem ;
//绘图环境
extern HDC hdcWave;
extern NWAVE_PANEL gNWavePanels[NWAVEPANEL_MAX_COUNT];
//模块对应的串口号，此处使用变量而不是宏，是为了以后可以动态分配串口

static int gSerialIndex_newiBio = DEV_SERIAL0;		//RS232
//int gSerialIndex_newiBio = DEV_SERIAL2;		//RSttf
//模块对应的串口设备
int gfdSerial_newiBio = -1;

//串口写操作的同步信号量
sem_t semSend_newiBio;

//串口接收缓冲
#define RECVBUFF_MAX_IBIO		128

//定义报文的最小长度
#define PACK_MIN_LENGTH_IBIO		15

//串口接收线程ID
pthread_t ptID_newiBioRecv;
extern BOOL gbSaveRealEcgData;

//NIBP过压保护
#define NIBP_PROTECT_ADULT			330
#define NIBP_PROTECT_ADULT_S		300
#define NIBP_PROTECT_CHILD			264
#define NIBP_PROTECT_CHILD_S		240
#define NIBP_PROTECT_BABY			165
#define NIBP_PROTECT_BABY_S		150

//过压保护计数器 根据采样间隔(20ms)计数
int giNibpProtectCounter = 0;
//15s的保护时间
#define NIBP_PROTECT_TIME		750		//50pack x 15s
//软件过压保护标志
BOOL gbNibpProtect = FALSE;
//NIBP校准标志位
BOOL gbNibpCal = FALSE;
//NIBP漏气检测标志位
BOOL gbNibpLeak = FALSE;


 int gbHaveBeep_QRS;
 int gST_templet[500];
 extern BOOL gbHavePack;
 extern BOOL gbHaveBeep_Pulse;
#define DEMO_ECG_COUNT 		188
#define DEMO_SPO2_COUNT 		82
#define DEMO_RESP_COUNT		130
static int Demo_Ecg_I[DEMO_ECG_COUNT]={
0x087c,0x0850,
0x0835,0x081b,0x0804,0x07f8,0x07f6,0x07f9,0x07fb,0x07fc,0x07fc,0x07fd,
0x07fd,0x07fc,0x07fc,0x07fc,0x07fc,0x07fb,0x07fc,0x07fd,0x07fd,0x07fd,
0x07fd,0x0800,0x0800,0x07ff,0x07ff,0x0801,0x0802,0x0801,0x0802,0x0803,
0x0804,0x0804,0x0805,0x0806,0x0806,0x0805,0x0806,0x0807,0x0808,0x0807,
0x0808,0x0808,0x0809,0x0809,0x080a,0x080a,0x080b,0x080a,0x080b,0x080c,
0x080e,0x080e,0x0810,0x080f,0x0810,0x0810,0x0810,0x080f,0x080e,0x080c,
0x080b,0x0808,0x0806,0x0803,0x0801,0x0800,0x07fd,0x07f8,0x07f6,0x07f8,
0x07f6,0x07f5,0x07f5,0x07f7,0x07f7,0x07f6,0x07f6,0x07f6,0x07f6,0x07f5,
0x07f6,0x07f7,0x07f7,0x07f6,0x07f6,0x07f7,0x07f7,0x07f6,0x07f6,0x07f6,
0x07f7,0x07f6,0x07f6,0x07f7,0x07f7,0x07f6,0x07f6,0x07f7,0x07f7,0x07f6,
0x07f6,0x07f7,0x07f7,0x07f6,0x07f6,0x07f8,0x07f9,0x07f8,0x07f8,0x07fe,
0x07ff,0x07ff,0x0800,0x07ff,0x07ff,0x07fe,0x07fe,0x07ff,0x07fe,0x07fd,
0x07fe,0x07ff,0x07ff,0x07fe,0x07fe,0x07ff,0x07ff,0x07fe,0x07fe,0x07ff,
0x07ff,0x07fe,0x07fe,0x07ff,0x0800,0x0801,0x0803,0x0806,0x0807,0x0808,
0x080a,0x080d,0x080e,0x080d,0x080d,0x080e,0x080d,0x080b,0x080a,0x0809,
0x0808,0x0805,0x0802,0x0801,0x07ff,0x07fe,0x07fe,0x07fe,0x07fe,0x07fd,
0x07fe,0x07ff,0x07ff,0x07fe,0x07fe,0x07ff,0x07ff,0x07fe,0x07fe,0x07ff,
0x07fe,0x07fe,0x07fe,0x07ff,0x07fd,0x07f8,0x07f3,0x07f0,0x07f6,0x0808,
0x0822,0x083e,0x085a,0x0872,0x0875,0x087e,
};
static int Demo_Ecg_II[DEMO_ECG_COUNT]={
0x08b3,0x089e,0x087b,0x0853,0x0833,
0x082a,0x0805,0x07ee,0x07e9,0x07ef,0x07f7,0x07fc,0x07fe,0x07fe,0x07fe,
0x07fe,0x07ff,0x07ff,0x07ff,0x07ff,0x07ff,0x07ff,0x0800,0x0800,0x0800,
0x0801,0x0802,0x0802,0x0803,0x0804,0x0805,0x0806,0x0807,0x0807,0x0808,
0x0809,0x080a,0x080a,0x080b,0x080c,0x080d,0x080e,0x080f,0x0810,0x0810,
0x0812,0x0811,0x0811,0x0812,0x0814,0x0813,0x0815,0x0817,0x0819,0x0818,
0x081a,0x081c,0x081d,0x081b,0x081c,0x081c,0x081b,0x0816,0x0814,0x0811,
0x080e,0x080b,0x0806,0x0800,0x07fa,0x07fd,0x07fa,0x07f7,0x07f6,0x07f3,
0x07f2,0x07f2,0x07f2,0x07f1,0x07f2,0x07f2,0x07f2,0x07f2,0x07f2,0x07f2,
0x07f2,0x07f2,0x07f2,0x07f1,0x07f2,0x07f2,0x07f2,0x07f2,0x07f2,0x07f1,
0x07f2,0x07f2,0x07f2,0x07f2,0x07f2,0x07f2,0x07f2,0x07f2,0x07f2,0x07f2,
0x07f2,0x07f2,0x07f2,0x07f2,0x07f2,0x07f8,0x07f9,0x07fa,0x07fb,0x07ff,
0x0800,0x0801,0x0802,0x07ff,0x07ff,0x07ff,0x0800,0x07ff,0x07ff,0x07ff,
0x07ff,0x07ff,0x07ff,0x07ff,0x07ff,0x07ff,0x07ff,0x07ff,0x07ff,0x07ff,
0x07ff,0x0800,0x0800,0x0802,0x0805,0x0809,0x080c,0x080f,0x0812,0x0814,
0x0816,0x0816,0x0816,0x0817,0x0817,0x0815,0x0813,0x0811,0x080e,0x080b,
0x0808,0x0804,0x0801,0x07ff,0x07ff,0x07ff,0x07ff,0x07ff,0x07ff,0x07ff,
0x07ff,0x07ff,0x07ff,0x07ff,0x07ff,0x0800,0x0800,0x07ff,0x07ff,0x07ff,
0x0800,0x0800,0x07fe,0x07fa,0x07f5,0x07ef,0x07f1,0x0805,0x0827,0x084d,
0x0874,0x089b,0x08b5,
};
static int Demo_Ecg_III[DEMO_ECG_COUNT]={
0x0835,
0x0830,0x0823,0x0815,0x0807,0x07fa,0x07f0,0x07ee,0x07f3,0x07f9,0x07fc,
0x07fd,0x07ff,0x07fe,0x07fe,0x07fd,0x07ff,0x0800,0x07ff,0x0800,0x0801,
0x07ff,0x07fe,0x07fe,0x07ff,0x07ff,0x07ff,0x07ff,0x0801,0x0801,0x0801,
0x0801,0x0802,0x0802,0x0801,0x0801,0x0802,0x0802,0x0801,0x0801,0x0803,
0x0803,0x0803,0x0803,0x0805,0x0805,0x0804,0x0804,0x0805,0x0805,0x0804,
0x0805,0x0807,0x0807,0x0807,0x0808,0x0809,0x0808,0x0808,0x0808,0x0809,
0x0809,0x0808,0x0807,0x0808,0x0805,0x0804,0x0802,0x07fc,0x07fc,0x07fd,
0x07fb,0x07fa,0x07fb,0x07fb,0x07fa,0x07fa,0x07fb,0x07fb,0x07f9,0x07f9,
0x07fa,0x07fb,0x07f9,0x07fa,0x07fb,0x07fb,0x07fa,0x07fa,0x07fb,0x07fb,
0x07f9,0x07fa,0x07fa,0x07fa,0x07fa,0x07fa,0x07fb,0x07fa,0x07f9,0x07f9,
0x07fb,0x07fb,0x07fa,0x07fa,0x07fb,0x07fb,0x07fb,0x07fa,0x07fc,0x07fe,
0x07fd,0x07fd,0x07fe,0x07ff,0x07fe,0x07fe,0x07ff,0x07ff,0x07fe,0x07fe,
0x07ff,0x07ff,0x07fe,0x07fe,0x07ff,0x07fe,0x07fe,0x07fe,0x07ff,0x07ff,
0x07ff,0x07fe,0x07fe,0x07fe,0x07ff,0x07ff,0x07ff,0x0800,0x0803,0x0804,
0x0804,0x0805,0x0807,0x0808,0x0807,0x0806,0x0808,0x0808,0x0806,0x0806,
0x0806,0x0805,0x0803,0x0802,0x0801,0x07ff,0x07fe,0x07fe,0x07ff,0x07ff,
0x07fe,0x07fe,0x07ff,0x07ff,0x07fe,0x07fe,0x07ff,0x07ff,0x07fe,0x07fd,
0x07ff,0x07ff,0x07fe,0x07ff,0x07fd,0x07fc,0x07fc,0x07fc,0x07fc,0x0800,
0x0809,0x0814,0x0820,0x082b,0x0830,0x0833,0x0833,
};
static int Demo_Ecg_V[DEMO_ECG_COUNT]={
0x0827,0x0825,0x0813,0x07f9,0x07dc,0x07bf,0x07a3,0x0799,
0x07a4,0x07be,0x07dc,0x07f3,0x07fd,0x0800,0x07ff,0x07ff,0x0800,0x07ff,
0x07ff,0x07ff,0x0800,0x0800,0x07ff,0x0800,0x0800,0x0800,0x07ff,0x0800,
0x0801,0x0801,0x0800,0x0802,0x0803,0x0802,0x0802,0x0802,0x0803,0x0802,
0x0803,0x0804,0x0804,0x0804,0x0804,0x0805,0x0805,0x0804,0x0804,0x0805,
0x0806,0x0807,0x0807,0x0808,0x0809,0x080a,0x0808,0x0809,0x080b,0x080b,
0x080b,0x080c,0x080c,0x080b,0x080a,0x080a,0x080a,0x0809,0x0807,0x0807,
0x0806,0x0804,0x0801,0x0800,0x07ff,0x07fd,0x07fc,0x07fc,0x07fc,0x07fb,
0x07fb,0x07fb,0x07fb,0x07fa,0x07fa,0x07fb,0x07fb,0x07fb,0x07fa,0x07fb,
0x07fc,0x07fb,0x07fb,0x07fb,0x07fc,0x07fb,0x07fb,0x07fb,0x07fc,0x07fb,
0x07fb,0x07fb,0x07fb,0x07fb,0x07fb,0x07fb,0x07fb,0x07fb,0x07fa,0x07fa,
0x07fb,0x07fa,0x07fb,0x07fc,0x07fd,0x07fd,0x07ff,0x07ff,0x0800,0x07ff,
0x07ff,0x0800,0x0800,0x07ff,0x07ff,0x07ff,0x07ff,0x07ff,0x07ff,0x07ff,
0x0800,0x07ff,0x07ff,0x07ff,0x0800,0x07ff,0x07ff,0x07ff,0x07ff,0x07ff,
0x07ff,0x0800,0x0803,0x0805,0x0807,0x080a,0x080c,0x080d,0x080e,0x080f,
0x0810,0x0810,0x080f,0x080e,0x080e,0x080c,0x0809,0x0808,0x0806,0x0803,
0x0800,0x0800,0x07ff,0x07ff,0x07fe,0x07ff,0x07ff,0x07fe,0x07fd,0x07fe,
0x07ff,0x07ff,0x07ff,0x07ff,0x07fe,0x07ff,0x07ff,0x07ff,0x07ff,0x07ff,
0x0800,0x07ff,0x0800,0x0804,0x080c,0x0813,0x081c,0x0820,0x0824,0x0829,
};
 static int Demo_Spo2[DEMO_SPO2_COUNT]={
0x00cc,0x00cc,0x00cc,0x00ca,
0x00c6,0x00c2,0x00bc,0x00b6,0x00b0,0x00aa,0x00a2,0x009c,0x0094,0x008e,
0x008a,0x0084,0x0082,0x007e,0x007e,0x007e,0x007e,0x007e,0x007e,0x007e,
0x007e,0x007e,0x007e,0x007e,0x007c,0x007a,0x007a,0x0078,0x0076,0x0074,
0x0072,0x0070,0x0070,0x006e,0x006c,0x006a,0x0068,0x0068,0x0066,0x0064,
0x0062,0x0062,0x0060,0x005e,0x005e,0x005c,0x005c,0x005a,0x0058,0x0058,
0x0056,0x0056,0x0054,0x0054,0x0052,0x0052,0x0050,0x0050,0x004e,0x004e,
0x0050,0x0054,0x005a,0x0060,0x0068,0x0070,0x007a,0x0082,0x008c,0x0096,
0x00a2,0x00ac,0x00b4,0x00bc,0x00c2,0x00c8,0x00ca,0x00cc,

};
static int Demo_Resp[DEMO_RESP_COUNT]={
0x00cf,0x00ce,0x00ce,0x00cd,0x00cd,0x00cc,0x00cc,0x00ca,0x00c8,0x00c6,
0x00c4,0x00c2,0x00c0,0x00be,0x00bc,0x00b9,0x00b6,0x00b3,0x00af,0x00ac,
0x00a9,0x00a5,0x00a1,0x009c,0x0097,0x0092,0x008e,0x008a,0x0085,0x0081,
0x007c,0x0078,0x0074,0x0072,0x0070,0x006e,0x006a,0x0066,0x0062,0x005e,
0x005b,0x0059,0x0056,0x0052,0x004e,0x0049,0x0044,0x0040,0x003c,0x0038,
0x0035,0x0033,0x0031,0x002e,0x002c,0x0029,0x0026,0x0023,0x0020,0x001e,
0x001b,0x0019,0x0018,0x0018,0x0018,0x0018,0x0018,0x0017,0x0017,0x0017,
0x0017,0x0019,0x001a,0x001a,0x001b,0x001c,0x001e,0x0022,0x0027,0x002c,
0x0032,0x0039,0x003d,0x0042,0x004a,0x004f,0x0055,0x005f,0x0063,0x0067,
0x006a,0x006d,0x0070,0x0073,0x0075,0x0078,0x007b,0x007f,0x0081,0x0085,
0x0089,0x008f,0x0094,0x0099,0x009d,0x009f,0x00a2,0x00a5,0x00a9,0x00ad,
0x00b0,0x00b3,0x00b6,0x00b8,0x00ba,0x00bc,0x00be,0x00c0,0x00c3,0x00c5,
0x00c8,0x00ca,0x00ca,0x00cc,0x00cd,0x00cd,0x00ce,0x00ce,0x00cf,0x00cf,
};

//供网络发送的波形缓冲区(1秒)和实际打包的长度
extern U8 gbNetWaveBuff_Ecg[ECG_WAVE_PER_SEC*4];			//200 x 4Leads
extern int giNetWaveLength_Ecg;
extern U8 gbNetWaveBuff_SpO2[SPO2_WAVE_PER_SEC];
extern int giNetWaveLength_SpO2;
extern U8 gbNetWaveBuff_Resp[RESP_WAVE_PER_SEC];
extern int giNetWaveLength_Resp;
extern U8 gbNetWaveBuff_CO2[CO2_WAVE_PER_SEC];
extern int giNetWaveLength_CO2;
extern U16 gbNetWaveBuff_IBP1[IBP_WAVE_PER_SEC];	
extern int giNetWaveLength_Ibp1;
extern U16 gbNetWaveBuff_IBP2[IBP_WAVE_PER_SEC];	
extern int giNetWaveLength_Ibp2;

/*
	发送函数
*/
static int iBio_Send(const unsigned char *datas, int datalen)
{
	int len = 0;
	int i;
	
	if(gfdSerial_newiBio <=0 || datalen <=0 || datas ==NULL) return -1;
		
	sem_wait(&semSend_newiBio);
	if(!B_PRINTF) printf("port_send: ");
	for(i=0; i< datalen; i++){
		len = write(gfdSerial_newiBio, datas+i, 1);	//实际写入的长度
		if(!B_PRINTF) printf("%.2X ", *(datas+i));
		if(len != 1){
			tcflush(gfdSerial_newiBio, TCOFLUSH);
			sem_post(&semSend_newiBio);
			return -1;
		}
// 		usleep(5000);
	}
	
	if(!B_PRINTF) printf(" End.\n");
	sem_post(&semSend_newiBio);

	return datalen;
}


/*
	接收函数
*/
static int iBio_Recv(char *data, int datalen, int baudrate)
{
	int readlen, fs_sel;
	
	fd_set	fs_read;
	struct timeval tv_timeout;

	if(gfdSerial_newiBio<=0 || datalen <=0 || data ==NULL) return -1;

	FD_ZERO(&fs_read);
	FD_SET(gfdSerial_newiBio, &fs_read);
	tv_timeout.tv_sec = TIMEOUT_SEC(datalen, baudrate);
	tv_timeout.tv_usec = 100;//TIMEOUT_USEC;
	
	fs_sel = select(gfdSerial_newiBio+1, &fs_read, NULL, NULL, &tv_timeout);
	
	if(fs_sel>0){
		readlen = read(gfdSerial_newiBio, data, datalen);
		//if(B_PRINTF) printf("%s:%d RECV Data %d.\n", __FILE__, __LINE__, readlen);
		return(readlen);
	}
	else{
		return(-1);
	}

	return (readlen);
}
#define DEMO_SPEED 50
static unsigned int gSaveWave[DEMO_SPEED*10];
/*
	保存波形数据到文件中，调试用
*/
static int SaveWaveValuetoFile()
{
	int i;
	static int savecount=0;
	
	if(savecount<=DEMO_SPEED*10){
	
		gSaveWave[savecount]=gValueResp.iWave;
	//	gSaveWave[savecount]=Ch1Value;
		
//		if(B_PRINTF) printf("%04x %d ",gSaveWave[savecount],savecount);
			if(savecount%DEMO_SPEED==0)
				if(!B_PRINTF) printf("**save %d s**\n",savecount/DEMO_SPEED);
		savecount++;
	}else{
			savecount=0;
			if(!B_PRINTF) printf("wave save---%d--------------------\n",savecount);
			FILE *fp;
			fp  = fopen("resp_wave.bin", "w");
			
			for(i=0;i<DEMO_SPEED*10;i++){

				if(i%10==0){
					fprintf(fp,"\n0x%04x,",gSaveWave[i]);
				}else{
					fprintf(fp,"0x%04x,",gSaveWave[i]);
				}
			}
			fclose(fp);
		}

	return 0;
}
static int Save_net_ecg_wave()
{
	static BOOL bSkip;
	
	if(gCfgEcg.bLeadType == ECGTYPE_5LEAD){
		// 打包网络数据	
		gbNetWaveBuff_Ecg[giNetWaveLength_Ecg] = (gValueEcg.iWaves[ECGWAVE_I]-2047)/3+128;
		gbNetWaveBuff_Ecg[giNetWaveLength_Ecg+1] =  (gValueEcg.iWaves[ECGWAVE_II]-2047)/3+128;
		gbNetWaveBuff_Ecg[giNetWaveLength_Ecg+2] =  (gValueEcg.iWaves[ECGWAVE_V]-2047)/3+128;
		
	}else{
		// 打包网络数据
		gbNetWaveBuff_Ecg[giNetWaveLength_Ecg] = (gValueEcg.iWaves[ECGWAVE_I]-2047)/3+128;
		gbNetWaveBuff_Ecg[giNetWaveLength_Ecg+1] = (gValueEcg.iWaves[ECGWAVE_II]-2047)/3+128;
		gbNetWaveBuff_Ecg[giNetWaveLength_Ecg+2] =(gValueEcg.iWaves[ECGWAVE_III]-2047)/3+128;
	}


	//由250个采样点降到200个采样点
	if((giNetWaveLength_Ecg==0)||((giNetWaveLength_Ecg/3)%5)!=0||bSkip){
		giNetWaveLength_Ecg =(giNetWaveLength_Ecg+3)%1000;
		bSkip=FALSE;
	}else {
		bSkip=TRUE;
	}
	
	return 0;
}

static int Save_net_spo2_wave()
{	
	static BOOL bSkip;
	
	gbNetWaveBuff_SpO2[giNetWaveLength_SpO2] = (gValueSpO2.bWave*100)/256;
	if(bSkip){
		giNetWaveLength_SpO2=(giNetWaveLength_SpO2+1)%1000;
	}
	bSkip=!bSkip;
	
}

static int Save_net_resp_wave()
{	
	
	gbNetWaveBuff_Resp[giNetWaveLength_Resp] = gValueResp.iWave/4;
	giNetWaveLength_Resp=(giNetWaveLength_Resp+1)%1000;
	
	
}
/*
	以ECG的采样为标准，收到200包的时候发送数据报文至中央机和广播报文  1s
	发送网络数据到中央机或者他床观察
*/
static int SendNetData()
{
	int res;
	S_ECG_VALUE ecgValue;
	int i;
	static int netsend=0;
	 
	if(giNetWaveLength_Ecg >=ECG_WAVE_PER_SEC*3){
//	printf("giNetWaveLength_Ibp1=%d,giNetWaveLength_Ibp2=%d\n",giNetWaveLength_Ibp1,giNetWaveLength_Ibp2);
		//如果spo2 resp 数量不足，填充完整
		if(giNetWaveLength_SpO2<SPO2_WAVE_PER_SEC){
			for(i=giNetWaveLength_SpO2;i<SPO2_WAVE_PER_SEC;i++){
				gbNetWaveBuff_SpO2[giNetWaveLength_SpO2]=gbNetWaveBuff_SpO2[giNetWaveLength_SpO2-1];
				giNetWaveLength_SpO2++;
				

			}
		}else{
			giNetWaveLength_SpO2=SPO2_WAVE_PER_SEC;
		}
		
		if(giNetWaveLength_Resp<RESP_WAVE_PER_SEC){
			for(i=giNetWaveLength_Resp;i<RESP_WAVE_PER_SEC;i++){
				gbNetWaveBuff_Resp[giNetWaveLength_Resp]=gbNetWaveBuff_Resp[giNetWaveLength_Resp-1];
				
				giNetWaveLength_Resp++;
				
			}
		}else{
			giNetWaveLength_Resp=RESP_WAVE_PER_SEC;
		}
	
		if(giNetWaveLength_Ibp1<IBP_WAVE_PER_SEC){
			
			for(i=giNetWaveLength_Ibp1;i<IBP_WAVE_PER_SEC;i++){
				gbNetWaveBuff_IBP1[giNetWaveLength_Ibp1]=gbNetWaveBuff_IBP1[giNetWaveLength_Ibp1-1];
				giNetWaveLength_Ibp1++;

			}
		}else{
			giNetWaveLength_Ibp1=IBP_WAVE_PER_SEC;
		}
		
		if(giNetWaveLength_Ibp2<IBP_WAVE_PER_SEC){
			for(i=giNetWaveLength_Ibp2;i<IBP_WAVE_PER_SEC;i++){
				gbNetWaveBuff_IBP2[giNetWaveLength_Ibp2]=gbNetWaveBuff_IBP2[giNetWaveLength_Ibp2-1];
				giNetWaveLength_Ibp2++;
			}
		}else{
			giNetWaveLength_Ibp2=IBP_WAVE_PER_SEC;
		}
		
		
		NetSend_Data();
	}
	
	
	return 0;
}

/*
	解析数据
*/
static int newiBio_Analyze(unsigned char*data,int len)
{

	int i;
//	if(data[0]==0x01) {
//		for(i=0;i<len;i++) printf("%x ",data[i]);
//		printf("\n");
//		}
	if(data==NULL||len<2)
		return -1;
	switch(data[DATAID]&0x7f){
		
		case ID_ECG_WAV1:{
			short wave1,wave2,wave3,waveV;
			
			if( data[2]&0x40){ //LA Off
				gValueEcg.bLAOff=TRUE;
			}else{
				gValueEcg.bLAOff=FALSE;
			}
			if( data[4]&0x40){  //LL Off
				gValueEcg.bLLOff=TRUE;
			}else{
				gValueEcg.bLLOff=FALSE;
				gValueResp.bOff=FALSE;
			}
			if( data[6]&0x40){//RA Off
				gValueEcg.bRAOff=TRUE;
			}else{
				gValueEcg.bRAOff=FALSE;
				gValueEcg.bConnected = TRUE;
			}
			if( data[8]&0x20){//RL Off
				gValueEcg.bRLOff=TRUE;
			}else{
				gValueEcg.bRLOff=FALSE;
			}
			if( data[8]&0x40){  //V1 Off
				gValueEcg.bVOff=TRUE;
			}else{
				gValueEcg.bVOff=FALSE;
			}
	static int DemoCountEcg=0;		
if(gbViewDemoData){
	
	gValueEcg.iWaves[ECGWAVE_I]=Demo_Ecg_I[DemoCountEcg];
	gValueEcg.iWaves[ECGWAVE_II]=Demo_Ecg_II[DemoCountEcg];
	gValueEcg.iWaves[ECGWAVE_III]=Demo_Ecg_III[DemoCountEcg];
	gValueEcg.iWaves[ECGWAVE_V]=Demo_Ecg_V[DemoCountEcg];
	
	if(DemoCountEcg==0){
		gbHaveBeep_QRS=TRUE;
	}

	DemoCountEcg+=1;
	DemoCountEcg=DemoCountEcg%DEMO_ECG_COUNT;
	
}else{			
	//pacemaker
			if( data[2]&0x20){
				gValueEcg.bPacingpulse=1;
			}else
				gValueEcg.bPacingpulse=0;
			
	//heart beat		
			if(data[4]&0x20)
				gbHaveBeep_QRS=TRUE;
			
			//wave I
			wave1 = data[1];	wave1 += (data[2]<<7);	wave1 &= 0xfff;
			gValueEcg.iWaves[ECGWAVE_I]=wave1;
			//wave II
			wave2 = data[3];	wave2 += (data[4]<<7);	wave2 &= 0xfff;
			gValueEcg.iWaves[ECGWAVE_II]=wave2;
			//wave III
			wave3 = data[5];	wave3 += (data[6]<<7);	wave3 &= 0xfff;
			gValueEcg.iWaves[ECGWAVE_III]=wave3;
			//wave V
			waveV = data[7];	waveV += (data[8]<<7);	waveV &= 0xfff;
			gValueEcg.iWaves[ECGWAVE_V]=waveV;
}

			if(gValueEcg.bLAOff || gValueEcg.bLLOff || gValueEcg.bRAOff || gCfgEcg.bLeadType==ECGTYPE_3LEAD){
				gValueEcg.iWaves[ECGWAVE_AVR] = BASELINE_ECG;
				gValueEcg.iWaves[ECGWAVE_AVL] = BASELINE_ECG;
				gValueEcg.iWaves[ECGWAVE_AVF] = BASELINE_ECG;

			}else{
				//aVR = -(I+II)/2
				gValueEcg.iWaves[ECGWAVE_AVR] = (gValueEcg.iWaves[ECGWAVE_I] + gValueEcg.iWaves[ECGWAVE_II])/2;
				//aVL=(I-III)/2
				gValueEcg.iWaves[ECGWAVE_AVL] = (gValueEcg.iWaves[ECGWAVE_I] - gValueEcg.iWaves[ECGWAVE_III])/2 +BASELINE_ECG;
				//aVF=(II+III)/2
				gValueEcg.iWaves[ECGWAVE_AVF] = (gValueEcg.iWaves[ECGWAVE_II] + gValueEcg.iWaves[ECGWAVE_III])/2;	

				
			}
			
			if((gValueEcg.iWaves[0]>2047+500)||(gValueEcg.iWaves[0]<2047-500)){
				
					for(i=0;i<7;i++)
						printf("%d ",gValueEcg.iWaves[i]);
					printf("\n");
					for(i=0;i<len;i++)
						printf("%02x ",data[i]);
					printf("\n");
					
					printf("LA off :%d,LL off :%d,RA off :%d\n ",gValueEcg.bLAOff,gValueEcg.bLLOff,gValueEcg.bRAOff);
					
				}
/*			
			
					

			for(i=0;i<ECGWAVE_COUNT;i++){
				gValueEcg.iWaves[i]=(gValueEcg.iWaves[i] -2047);
			//	gValueEcg.iWaves[i]=(gValueEcg.iWaves[i] -2048)/6+128;
				//printf(" %d \t",gValueEcg.iWaves[ECGWAVE_II] );
			}

		printf(" %d \n",gValueEcg.iWaves[ECGWAVE_II] );

*/
			//添加数据到实时存储数组中(原始1x数据)
			FillOneRealSaveData();	
			Save_net_ecg_wave();
			ReceivingData_Ecg1(&gValueEcg);		//添加到缓冲队列1中
			ReceivingData_Ecg2(&gValueEcg);

		}break;
		case ID_ECG_WAV2:{
			
		}break;
		case ID_RESP_WAV:{
			short resp_wave;
			static int demo_resp_count=0;
			//resp wave
			resp_wave=data[1]; resp_wave += (data[2]<<7);resp_wave &= 0x3fff;
			if(gbViewDemoData){
				gValueResp.iWave=Demo_Resp[demo_resp_count];
				demo_resp_count=(demo_resp_count+1)%DEMO_RESP_COUNT;
			}else
				gValueResp.iWave=(resp_wave-1023)+128;
			
			Save_net_resp_wave();
			ReceivingData_Resp(&gValueResp);
			
		//	printf("%d \t",gValueResp.iWave);
		}break;
		
		case ID_RESP_STA :{
			BYTE resp_rr;
			//rr
			resp_rr=data[1];
			resp_rr+=((data[2]<<1)&0x80);
			
			//resp lead off
			
			if(data[2]&=0x01)//lead off
				gValueResp.bOff =TRUE;
			else
				gValueResp.bOff =FALSE;
			
			if(data[2]&=0x01<<1)//RESP Apnea State
				gValueResp.bApneaAlm=TRUE;
			else
				gValueResp.bApneaAlm=FALSE;

			if(data[2]&=0x01<<2)//RESP Baseline Out of Range 
				gValueResp.bBaselineErr=TRUE;
			else
				gValueResp.bBaselineErr=FALSE;
			
			if(data[2]&=0x01<<3)//An artifact has been detected 
				gValueResp.bCVAAlm=TRUE;
			else
				gValueResp.bCVAAlm=FALSE;
			
			if(data[2]&=0x01<<4)// Similarity between HR and Breath Rate 心动干扰
				gValueResp.bCVAAlm=TRUE;
			else
				gValueResp.bCVAAlm=FALSE;
			
			if(gbViewDemoData){
				gValueResp.bOff =FALSE;
				gValueResp.wRR=20;
				
			}else{
				if(resp_rr<RR_MAX){
					gValueResp.wRR=resp_rr;	
					
				}else
					gValueResp.wRR=0;
			}
			
		IsAlm_RR(&gValueResp,&gCfgResp, FALSE);
		IsAlm_CVA(&gValueResp,&gCfgResp, FALSE);
	
	//		printf("ID_RESP_STA RR:%d\n",resp_rr);
		}break;
		case ID_ST_TEMPLET :{
			short st_val;
			BYTE bHave_Temp;
			BYTE st_count;
			static BYTE count_test;
			
			bHave_Temp=data[3]&0x40;
			
			/*	
			for(i=0;i<4;i++)
				printf(" %2x",data[i]);
			printf("**\n");
			*/
			
			if(bHave_Temp){
			//	static int st_test[250];
				st_count=data[2];
				st_count+=(data[3]&0x20)<<2;
					
				st_val=data[1]; 
				st_val+=(data[3]&0x1f)<<7;
				
				count_test++;
				
				if(st_count<250){
					gST_templet[st_count]=st_val;
				//	printf("%d:%d,,,,,%d\n",st_count,gST_templet[st_count],st_val);
				}else
					printf("st_count err %d\n",st_count);
				
			
				if(st_count==249){
						for(i=0;i<250;i++){
							if(i%10==0) printf("\n");
							printf("%d ",gST_templet[i]-2047);
						}
						printf("count_test=%d\n",st_count);
					}
				
				/*
				if(st_count==0){
						memset(gST_templet, 0, sizeof gST_templet);
						gST_templet[st_count]=st_val;
				}else{
					
				}
					
				*/
				
			}else{
				printf("no st templet\n");
			}
		//	printf("%d\n",count_test);
		}break;
		case ID_ECG_HR :{
			short HR;
			HR = data[1]+((data[2]&0x03)<<7);

			if(gbViewDemoData){
				gValueEcg.bLAOff=FALSE;
				gValueEcg.bLLOff=FALSE;
				gValueEcg.bRAOff=FALSE;
				gValueEcg.bVOff=FALSE;
				
				gValueEcg.wHR=80;
			}else{
				if(HR<=350)
					gValueEcg.wHR=HR;
				else
					gValueEcg.wHR=0;
			}
			IsOff_LL(&gValueEcg);
			IsOff_LA(&gValueEcg);
			IsOff_RA(&gValueEcg);
			IsOff_V(&gValueEcg);	
	
			IsAlm_HR(&gValueEcg,&gCfgEcg,FALSE);
		//	printf("HR=%d\n",HR);
			
		}break;
		case ID_ECG_PVC :{
			if(data[1]<=125)
				gValueEcg.bPVC = data[1];
			else
				gValueEcg.bPVC = 0;
			
		}break;
		case ID_ST_VALUE1:{
			short ST;
			ST = data[1]+((data[2]&0x0f)<<7);
			
			switch( (data[2]&0x70)>>4 )
			{
			case	0: //ST I
				gValueEcg.iST[ECGWAVE_I] = ST;
				break;
			case	1: //ST II
				gValueEcg.iST[ECGWAVE_II]= ST;
				break;
			case	2: //ST III
				gValueEcg.iST[ECGWAVE_III] = ST;
				break;
			case	3:  //ST V1
				gValueEcg.iST[ECGWAVE_V] = ST;
				break;
			default:break;
			}
		
			IsAlm_ST1(&gValueEcg,&gCfgEcg,FALSE);
			IsAlm_ST2(&gValueEcg,&gCfgEcg,FALSE);
		//	printf("***ST=%d\n",ST);
		}break;
		case ID_ST_VALUE2 :{
			
			
		}break;
		case ID_ST_ISO_VAL:{
			BYTE ST_val,ISO_val;
			ST_val=data[1];
			ISO_val=data[2];
			
			gCfgEcg.bST=ST_val;
			gCfgEcg.bISO=ISO_val;	

		//	printf("..........ST_val=%d,ISO_val=%d\n",gCfgEcg.bST,gCfgEcg.bISO);
			
		}break;
		case ID_ECG_ARR:{
			unsigned long arr;
			
			arr=data[1];
			arr |= data[2]<<7;
			arr |= data[3]<<14;
			arr |= data[4]<<21;			
			gValueEcg.lARR=arr;
			
			IsAlm_Arr(&gValueEcg, &gCfgEcg, FALSE);
			printf("*********ecg arr =%.6x\n",arr);
			
		}break;
		
		case ID_ECG_STATE:{
			BYTE Lead_sta,Cal_sta,Pace_sta,Bandwidth_sta;
			
		}break;

/*************************************************************************
*0x01: SPO2波形数据	
*	data[1],SPO2波形：8位无符号数，数据范围：0～255。
*	data[2],SPO2测量状态：
*			bit6:脉搏标志，上位机在该标志为1时可以进行脉搏声音提示
*			bit5: Photo Cell错误
*			bit4:Red-LED错误
*			bit3:IR-LED 
*			bit2~0: 0-正常1-没有接探头2-没有接手指3-探头ID错4-信号弱
*					5-没有波形6-运动干扰7-搜索脉搏
**************************************************************************/
		case ID_SPO2_WAV:{
			WORD wave;
			BYTE HavePluse;
			BYTE Err_PhotoCell,Err_RedLed,Err_IrLed;
			BYTE Info_Spo2;
			
			wave = data[1];
			HavePluse=data[2]&0x01<<6;
			Err_PhotoCell=data[2]&0x01<<5;
			Err_RedLed=data[2]&0x01<<4;
			Err_IrLed=data[2]&0x01<<3;
			Info_Spo2=data[2]&0x07;

			
			if(gbViewDemoData){
				Info_Spo2=0;
			}else{
				if(HavePluse)
					gbHaveBeep_Pulse=TRUE;
			}
			if(Info_Spo2==0x01)
				gValueSpO2.bNoSensor=TRUE;
			else
				gValueSpO2.bNoSensor=FALSE;

			if(Info_Spo2==0x02){
				gValueSpO2.bOff=TRUE;
				gValuePulse.bOff =TRUE;
				
			}else{
				gValueSpO2.bOff=FALSE;
				gValuePulse.bOff =FALSE;
				gValueSpO2.bConnected=TRUE;
			}

			if(Info_Spo2==0x03)
				gValueSpO2.bUnknownSensor=TRUE;
			else
				gValueSpO2.bUnknownSensor=FALSE;

			if(Info_Spo2==0x04)
				gValueSpO2.bErrCode |= SPO2_WEAKSIGNAL;
			else
				gValueSpO2.bErrCode &= (~SPO2_WEAKSIGNAL);
			
			if(Info_Spo2==0x06)
				gValueSpO2.bErrCode |= SPO2_INTERFERE;
			else
				gValueSpO2.bErrCode &= (~SPO2_INTERFERE);
			
			if(Info_Spo2==0x07)
				gValueSpO2.bErrCode |= SPO2_SEARCHPULSE;
			else
				gValueSpO2.bErrCode &= (~SPO2_SEARCHPULSE);

			//判断脱落报警 
			IsOff_SpO2(&gValueSpO2);
			
			//判断是否有测量错误
			isErr_SpO2(&gValueSpO2);
			
			static int spo2_demo_count=0;
			if(gbViewDemoData){
				gValueSpO2.bWave=Demo_Spo2[spo2_demo_count];
				spo2_demo_count=(spo2_demo_count+1)%DEMO_SPO2_COUNT;
				if(spo2_demo_count==0)gbHaveBeep_Pulse=TRUE;
			}else{
				gValueSpO2.bWave =wave*2;
			}
			Save_net_spo2_wave();
			ReceivingData_SpO2(&gValueSpO2);
			
		}break;
/*************************************************************************
*   0x02: SPO2数据
*	data[1]:Spo2数据:0~100,
*	data[2]:PR数值bit6~0
*	data[3]:Bit6~Bit2: IR 调制百分比 
*			Bit1~Bit0: PR数值bit8~7
**************************************************************************/	
		case ID_SPO2_DAT:{

			BYTE Spo2Value;
			WORD PR;
			BYTE IR_ModulationPercent;
			Spo2Value=data[1];
			PR = data[2];
			PR += (data[3]&0x03)<<7;
			IR_ModulationPercent = (data[3]>>2);
			
			if(gbViewDemoData){
				
				gValueSpO2.bSpO2=96;
				gValuePulse.wPR=75;
			}else{
				if(Spo2Value<=100)
				gValueSpO2.bSpO2 =Spo2Value;
			else
				gValueSpO2.bSpO2 = 0;	
			
				if(PR<=300)
					gValuePulse.wPR=PR;
				else
					gValuePulse.wPR=0;
			}
			
			IsAlm_SpO2(&gValueSpO2, &gCfgSpO2, FALSE);
			IsAlm_PR(&gValueSpO2, &gCfgSpO2, FALSE);
				
		//	printf("Spo2Value=%d,PR=%d\n",Spo2Value,PR);

		}break;
		
/*************************************************************************
* 0x03:体温数据
*	data[1]:temp1温度数据Bit6~Bit0
*	data[2]:temp2温度数据Bit6~Bit0
*	data[3]:Bit3~Bit2:    Temp2  Bit8~Bit7
*			Bit1~Bit2:    Temp1  Bit8~Bit7
*	data[4]:Bit6=1: 体温系统错误 
*			Bit5=1: Low-End Current Calibration Error 
*			Bit4=1: High-End Current Calibration Error   
*			Bit3=1: 体温2传感器类型错误
*			Bit2=1: 体温1传感器类型错误
*			Bit1=1: 体温探头2脱落
*			Bit0=1: 体温探头1脱落
**************************************************************************/
		case ID_TEMP_DAT:{
			WORD Temp1Val,Temp2Val;
			BYTE Err_TempSys,Err_LowEnd,Err_HighEnd;
			BYTE Err_T1SensorType,Err_T2SensorType,Err_T1Off,Err_T2Off;
			Temp1Val = data[1];
			Temp1Val += (data[3]&0x03)<<7;
			Temp2Val = data[2];
			Temp2Val += ((data[3]&0x0c)>>2)<<7;
			Err_T1Off=data[4]&0x01<<0;
			Err_T2Off=data[4]&0x01<<1;
			Err_T1SensorType=data[4]&0x01<<2;
			Err_T2SensorType=data[4]&0x01<<3;
			Err_HighEnd=data[4]&0x01<<4;
			Err_LowEnd=data[4]&0x01<<5;
			Err_TempSys=data[4]&0x01<<6;

			if(Err_T1Off) 
				gValueTemp.bOff_T1=TRUE;
			else
				gValueTemp.bOff_T1=FALSE;
			if(Err_T2Off) 
				gValueTemp.bOff_T2=TRUE;
			else
				gValueTemp.bOff_T2=FALSE;
			if(gbViewDemoData){
				gValueTemp.bOff_T1=FALSE;
				gValueTemp.bOff_T2=FALSE;	
				
				gValueTemp.wT1=396;
				gValueTemp.wT2=375;
			}else{
				if(Temp1Val<500)
					gValueTemp.wT1=Temp1Val;
				else
					gValueTemp.wT1=0;
				if(Temp2Val<500)
					gValueTemp.wT2=Temp2Val;
				else
					gValueTemp.wT2=0;
			}
			IsOff_T1(&gValueTemp);
			IsOff_T2(&gValueTemp);
			IsAlm_T1(&gValueTemp,&gCfgTemp,FALSE);
			IsAlm_T2(&gValueTemp,&gCfgTemp,FALSE);
			if(Temp1Val<500&&Temp2Val<500)
				IsAlm_TD(&gValueTemp,&gCfgTemp,FALSE);				
		//	printf("Temp1Val=%d,Temp2Val=%d\n",Temp1Val,Temp2Val);
			
		}break;
		
/********************************************************
*DAT[1]: SYS	BIT6-BIT0
*DAT[2]: MEA	BIT6-BIT0
*DAT[3]: DIA	BIT6-BIT0  
*DAT[4]: PULSE RATE BIT6-BIT0
*DAT[5]: NOT: UES BIT6
*		SYS:  BIT5-BIT4		(SYS BIT8-BIT7)
*		MEA:  BIT3-BIT2		(MEA BIT8-BIT7)
*		DIA :  BIT1-BIT0		(DIA BIT8-BIT7)
*DAT[6]: NOT USE:  BIT6-BIT2
*		NIBP-PR:  BIT1-BIT0	(PR  BIT8-BIT7)
*NOTE  :NIBP SYS[0-300]  INVALID >300   UNIT  MMHG
*		NIBP MEA [0~300]    Invalid>300
*		NIBP DIA [0~300]    Invalid>300
*		NIBP PR[0~300]    Invalid >300 
*********************************************************/
		case ID_NIBP_DAT:{
			WORD NibpSys,NibpDia,NibpMean,NibpPR;
			NibpSys= data[1];
			NibpSys+=((data[5]&0x30)<<2);
			NibpMean=data[2];
			NibpMean+=((data[5]&0x0c)<<5);
			NibpDia=data[3];
			NibpDia+=((data[5]&0x03)<<7);
			NibpPR=data[4];
			NibpPR+=((data[6]&0x03)<<7);

		if(NibpSys>0&&NibpDia>0&&NibpMean>0){		
			gValueNibp.wSys=NibpSys;
			gValueNibp.wMean=NibpMean;
			gValueNibp.wDia=NibpDia;
			gValueNibp.wPR=NibpPR;
			
		}

		IsAlm_NIBP(FALSE, FALSE);
		
		printf("ID_NIBP_DAT: %d / %d (%d),NibpPR:%d\n",NibpSys,NibpDia,NibpMean,NibpPR);	
		}break;
/********************************************************
*DAT1  NIBP Cuff Pressure Bit6~Bit0
*DAT2  		Bit6:    	1=NIBP system Running 
*           		   		0=NIBP system Idle 
*			Bit5:    	1 = Adult Cuff Type 
*           				0 = Pediatric/Neonate Cuff Type
*		Bit4~Bit2:     
*				       000=manual measurement 
*  					001=auto measurement 
* 					010=STAT measurement (5min continue) 
* 					011=Static Venipuncture 
*  					100=Leak test 
*  					101=Pump/Valve test 
*  					110=Over Pressure test 
*  					111= Pressure Calibration 
*		Bit1~Bit0:   	 NIBP Cuff Pressure Bit8~Bit7 
*Note:  Unit : mmHg 
*			NIBP Cuff Pressure [0~300]    Invalid >300  
********************************************************/
		case ID_NIBP_CUFF:{
				WORD	Pressure;
				BYTE	MeasureResult,SysStatus,Object;
				
				Pressure=data[1];
				Pressure+=((data[2]&0x03)<<7);
				MeasureResult=((data[2]&0x1c)>>2);
				SysStatus=((data[2]&0x40)>>6);
				Object=((data[2]&0x20)>>5);
				
				if(Pressure>0){
						gValueNibp.wPressure=Pressure;
					}
				
				switch(MeasureResult){
					case 0 :
						gValueNibp.bMeasureType=NIBP_MANUAL;
						break;
					case 1 :
						gValueNibp.bMeasureType=NIBP_AUTO;
						break;
					case 2 :
						gValueNibp.bMeasureType=NIBP_STAT;
						break;
					case 3 :
						gValueNibp.bMeasureType=NIBP_STATIC_VEN;
						break;
					case 4 :
						gValueNibp.bMeasureType=NIBP_LEAK_TEST;
						break;
					case 5 :
						gValueNibp.bMeasureType=NIBP_P_V_TEST;
						break;
					case 6 :
						gValueNibp.bMeasureType=NIBP_OVER_PRE_TESE;
						break;
					case 7 :
						gValueNibp.bMeasureType=NIBP_PRE_CAL;
						break;
					default :
						gValueNibp.bMeasureType=NIBP_MANUAL;
						break;
					}
				if(SysStatus){
					gValueNibp.bStatus=NIBP_SYSTEM_RUNNING;
					}
					else{
					gValueNibp.bStatus=NIBP_SYSTEM_IDLE;
						}
			
		//	printf("ID_NIBP_CUFF: Pressure=%d,MeasureResult=0x%x,SysStatus=0x%x,Object=%d\n",Pressure,MeasureResult,SysStatus,Object);			
			}break;

/********************************************************
*DAT1: 		Bit6~Bit0  NIBP Calibration Pressure Value Bit6~Bit0 
*DAT2: 		Bit1~Bit0  NIBP Calibration Pressure Value Bit8~Bit7 
*
*Note: 		Request  external  calibration  pressure  valu
*			Host  must  respond  this  command  
*			ID_NIBP_PRSURE  while  the  external  calib
*			pressure is same with this pressure value. 
********************************************************/
		case ID_NIBP_CAL:{
				WORD CalPreValue;
				CalPreValue=data[1];
				CalPreValue+=((data[2]&0x03)<<7);

				gValueNibp.wCalPreValue=CalPreValue;
				update_cal_info_1();
				
				printf("ID_NIBP_CAL: CalPreValue=%d\n",gValueNibp.wCalPreValue);	
				
			}break;
		
/********************************************************
*DATA0: Bit6~Bit0  NIBP Feedback state to Host 
*		0x01: manual measurement Start OK 
*		0x02: manual measurement Start Abort 
*		0x03: auto measurement Start OK 
*		0x04: auto measurement Start Abort 
*		0x05: STAT measurement Start OK 
*		0x06: STAT measurement Start Abort 
*		0x07: Static Venipuncture Start OK 
*		0x08: Static Venipuncture Start Abort 
*		0x09: Leak Test Start OK 
*		0x0a: Leak Test Start Abort 
*		0x0b: Pump Valve Test Start OK 
*		0x0c: Pump Valve Test Start Abort 
*		0x0d: Overpressure Test Start OK 
*		0x0e: Overpressure Test Start Abort 
*		0x0f: Calibration Start OK 
*		0x10: Calibration Start Abort 
*		0x11: External Calibration Pressure Value Error 
*		0x12: Safety System Init Overtim
********************************************************/
		
		case ID_NIBP_FDB:{
				BYTE	StatetoHost;
				StatetoHost=data[1];
				gValueNibp.wFDB=StatetoHost;
				
				//漏气检测返回值
				if((gValueNibp.wFDB==NBP_FDB_LEAK_START_OK)
					||(gValueNibp.wFDB==NBP_FDB_LEAK_START_ABT)
					||(gValueNibp.wFDB>=NBP_FDB_LEAK_YES
						&&gValueNibp.wFDB<=NBP_FDB_LEAK_PUMPERR))
				{
					update_Leaktest_info();
				}
				//气泵检测返回值
				if((gValueNibp.wFDB==NBP_FDB_PVT_START_OK)
					||(gValueNibp.wFDB==NBP_FDB_PVT_START_ABT)
					||(gValueNibp.wFDB>=NBP_FDB_PVT_PUMP_ON
						&&gValueNibp.wFDB<=NBP_FDB_PVT_OT))
				{
					update_pumptest_info();
				}
				//过压保护返回值
				if((gValueNibp.wFDB==NBP_FDB_OVP_START_OK)
					||(gValueNibp.wFDB==NBP_FDB_OVP_START_ABT)
					||(gValueNibp.wFDB>=NBP_FDB_OVP_NEO_OK
						&&gValueNibp.wFDB<=NBP_FDB_OVP_PNEU_ERR))
				{
					update_overpressuretest_info();
				}
				//nibp校准返回值
				if(gValueNibp.wFDB>=NBP_FDB_CAL_START_OK
						&&gValueNibp.wFDB<=NBP_FDB_CAL_DATA_ERR)
				{
					update_cal_info();
					
				}
				
				//isErr_iBioNibp(&gValueNibp);
				printf("\nID_NIBP_FDB : ***0x%02x***\n",StatetoHost);
			}

		case ID_IBP12_WAV:{
			
		}break;
		case ID_IBP34_WAV:{

		}break;
		case ID_IBP12_DAT:{

		}break;
		case ID_IBP34_DAT:{

		}break;
		case ID_IBP_FDB:{

		}break;
		case ID_IBP_TIM:{

		}break;
		/*
		参数板软 硬件版本号
		*/
		case ID_UNIT_INFOS:{
			unsigned int hard_version;
			unsigned int soft_version;
			unsigned long unit_info;
			BYTE unit_id;
		
			hard_version=data[2]+data[1]<<8;
			soft_version=data[4]+data[3]<<8;
			
			unit_info=data[5]+(data[6]<<7)+(data[7]<<14)+(data[8]<<21)+((data[9]&0xf)<<28);
			unit_id=(data[9]&0x70)>>4;
			if(unit_id==1){
				SystemStatus.wModuleSV=soft_version;
				SystemStatus.wModuleHV=hard_version;
				SystemStatus.lModuleUnitID=unit_info;
			}
			printf("++++++++hard_version=%.04x,soft_version=%.04x, unit_info=%lx,unit_id=%d\n",hard_version,soft_version,unit_info,unit_id);
			
		}break;
	
		/*
		参数板PN,SN码
		*/
		case ID_PROD_SN:{
			BYTE pn;
			unsigned long sn;
			
			pn=data[1];
			sn=data[2]+(data[3]<<7)+(data[4]<<15);
			SystemStatus.wModuleSN=sn;
			
			printf("--------pn =%.02x sn=%.08x\n",pn,sn);
			
		}break;
		default:{
		}break;

	}
	//以ECG的采样为标准，收到200包的时候发送数据报文至中央机和广播报文  200Hz
		
		SendNetData();
	
	return 0;
}

/********************************************************************
功能描述: 将命令发送到下位机。
参数: pack----要发送的命令缓冲区指针，内容为尚未组织好的命令：ID+DATA。
返回值: 无。
********************************************************************/
void newSendCommand (unsigned char* pack,BYTE len)
{
	unsigned char head;
	int i;
	unsigned short checksum=0;
	
	/* 如果包长度小于2，直接返回*/
	if (len < 2 )
		return;
	
	
	/* 数据头最高位置1 */
	*(pack) = *(pack)  | 0x80;
	
	for (i = 0; i<len-1; i++) {
		/* 计算校验和*/
		checksum += *(pack+i);
	}
	
	/* 保存校验和*/
	//*(pack+len-1) = checksum& 0x7f;
	*(pack+len-1) = ((checksum & 0x7f)+(checksum>>7))&0x7f;
	
	/* TODO: 用户可以向下位机串口发送命令*/
	/* uart.write(pack, len); */
	
	iBio_Send(pack,len);
}


static int UnChecksum(unsigned char * pack, int len)
{
	int i;
	unsigned short checksum;
	if ( len < 2 )
		return -1;
	/* 开始计算校验和*/
	checksum = *(pack);
	for (i = 1; i <= len - 2; i ++) {
		/* 计算校验和*/
		checksum += *(pack+i);
	}
	checksum= ((checksum & 0x7f)+(checksum>>7))&0x7f;
	if ( checksum != (*(pack+len-1)) ){
	//if ( (checksum & 0x7f) != (*(pack+len-1)) )
	/*
		printf("**************check err[%2x]\n",checksum);
		for(i=0;i<len;i++){
			printf("%.2x ",pack[i]);
		}
		printf("\n ");
*/
		
		return -2;	//Check Sum Error
	}
	return 0;	//OK
}
/********************************************************************
* 功能描述：从串口接收缓冲区中解包。
*参数：
*	data 串口缓冲区指针；
* 	len 要解包的串口缓冲区数据长度；
*返回值：
* 	0: 解包OK
* 	-1: 包长度错误
* 	-2: 校验和错误
*******************************************************************/
int newiBio_UnBind (unsigned char *data, int length)
{
	static unsigned char sCurrPacket[64];
	static unsigned char sPacketIdFound = 0;/*接受到包类型ID 的标志，1 表示接收到*/
	static short sCurrPacketLen=0;/*当前接受到的包长度*/

	static unsigned char sPkgSize[]={
		3,		//ID_UNIT_STA		0x80
		10,		//ID_ECG_WAV1	0x81
		12,		//ID_ECG_WAV2	0x82
		4,		//ID_RESP_WAV	0x83
		4,		//ID_RESP_STA 	0x84
		5,		//ID_ST_TEMPLET	0x85
		4,		//ID_ECG_HR		0x86
		3,		//ID_ECG_PVC		0x87
		4,		//ID_ST_VALUE1	0x88
		4,		//ID_ST_VALUE2	0x89
		4,		//ID_ST_ISO_VAL	0x8a
		6,		//ID_ECG_ARR		0x8b
		4,		//ID_ECG_STATE	0x8c
		0,		//				0x8d
		0,		//				0x8e
		0,		//				0x8f
		4,		//ID_SPO2_WS		0x90
		5,		//ID_SPO2_DAT	0x91
		6,		//ID_TEMP_DAT	0x92
		8,		//ID_NIBP_DAT		0x93
		4,		//ID_NIBP_CUFF	0x94
		4,		//ID_NIBP_CAL		0x95
		3,		//ID_NIBP_FDB		0x96
		5,		//ID_IBP12_WAV	0x97
		5,		//ID_IBP34_WAV	0x98
		6,		//ID_IBP12_DAT	0x99
		6,		//ID_IBP34_DAT	0x9a
		3,		//ID_IBP_FDB		0x9b
		5,		//ID_IBP_TIM		0x9c
		11,		//ID_UNIT_INFOS	0x9d
		7,		//ID_UNIT_SN		0x9e
		0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	};
	int result=0;
	BYTE st_count=0;
	/* 读取缓冲区的数据*/
	while (length > 0)
	{
		/* 接收到包类型字节*/
		if ( sPacketIdFound ){
			/* 当前数据是数据字节，有效的数据字节必须小于0x80 */
			if (*data < 0x80 ){
				// 保存数据
				sCurrPacket [sCurrPacketLen++] = *data;
				/* 接收到整个包的数据，则开始解包*/
				if (sCurrPacketLen == sPkgSize [sCurrPacket[0]&0x7f]) {
					
					result = UnChecksum(sCurrPacket, sCurrPacketLen );
					
					if ( 0 == result ){
						/* TODO：用户可以处理解包后的数据*/
						newiBio_Analyze(sCurrPacket, sCurrPacketLen);
					}
					sPacketIdFound = 0;
				}
			}else{
				/*当前数据如果大于0x80, 则必定是错误*/
				sPacketIdFound = 0;
				result = -1;
			}
		}else{
			/* 检查当前字节是否正确的PKG_ID */
			if (*data>=0x80 )
			{
				sCurrPacketLen = 1;
				sCurrPacket[0] = *data;
				sPacketIdFound = 1;
			
			}else {
				/* 当前数据不是正确的包类型*/
				result = -1;
			}
		}
		/* 指向下一个串口数据*/
		data ++;
		/* 数据长度减1 */
		length --;
	}
	return (result);
}

/********************************************************************************/
//static Send cmd

static int NewiBioCMD_PATIENT_INFO(BYTE age)
{
	unsigned char packet[10];
	packet[0]=CMD_PATIENT_INFO;
	packet[1]=age;
	
	newSendCommand(packet,3);
	return 0;
}
static int NewiBioCMD_ECG_LEAD(BOOL mul_lead,BYTE cal_channel)
{
	unsigned char packet[10];
	
	packet[0]=CMD_ECG_LEAD ;
	
	if(mul_lead)
		packet[1]=0x40+cal_channel;
	else
		packet[1]=cal_channel;
	
	newSendCommand(packet,3);
	return 0;

}
static int NewiBioCMD_ECG_BW(BYTE mode)
{
	unsigned char packet[10];
	packet[0]=CMD_ECG_BW ;
	packet[1]=mode;
	
	newSendCommand(packet,3);
	return 0;

}
static int NewiBioCMD_ECG_CAL(BYTE type)
{
	unsigned char packet[10];
	packet[0]=CMD_ECG_CAL;
	packet[1]=type;	
	newSendCommand(packet,3);
	return 0;
}

static int NewiBioCMD_PACE_DET(BYTE type)
{
	unsigned char packet[10];
	packet[0]=CMD_PACE_DET;
	packet[1]=type;
	newSendCommand(packet,3);
	return 0;

}
static int NewiBioCMD_RESP_LEAD(BYTE type)
{
	unsigned char packet[10];
	packet[0]=CMD_RESP_LEAD;
	packet[1]=type;
	
	newSendCommand(packet,3);
	return 0;

}
static int NewiBioCMD_ST_ISO(BYTE st_val,BYTE iso_val)
{
	unsigned char packet[10];
	packet[0]=CMD_ST_ISO;
	packet[1]=st_val;
	packet[2]=iso_val;	
	
	newSendCommand(packet,4);
	return 0;

}
static int NewiBioCMD_ST_TEMP()
{
	unsigned char packet[10];
	packet[0]=CMD_ST_TEMP;
	packet[1]=2;//II lead
	newSendCommand(packet,3);
	return 0;

}
static int NewiBioCMD_RESP_GAIN(BYTE gain)
{
	unsigned char packet[10];
	packet[0]=CMD_RESP_GAIN;
	packet[1]=gain;
	
	newSendCommand(packet,3);
	return 0;

}

static int NewiBioCMD_ECG_LEARN()
{
	unsigned char packet[10];
	packet[0]=CMD_ECG_LEARN;
	
	newSendCommand(packet,2);
	return 0;

}
static int NewiBioCMD_SPO2_SEN(int value)
{
	unsigned char packet[10];
	packet[0]=CMD_SPO2_SEN;
	switch(value){
		case 0x00:{//灵敏度高
			packet[1]=value;
		}break;
		case 0x01:{//灵敏度中
			packet[1]=value;
		}break;
		case 0x02:{//灵敏度低
			packet[1]=value;
		}break;
		default:{//灵敏度中
			packet[1]=0x01;
		}break;
	}
	newSendCommand(packet,3);
	if(B_PRINTF) printf("Send CMD_SPO2_SEN\n");
	return 0;
}

static int NewiBioCMD_NIBP_START(BYTE Ctyle,BYTE Update,BYTE Ptype )
{
	unsigned char packet[10];
	packet[0]=CMD_NIBP_START;
	packet[1]=(Ctyle<<4)+(Update<<2)+(Ptype);
	
	newSendCommand(packet,3);
	if(B_PRINTF) printf("Send CMD_NBP_START\n");
	return 0;
 }


static int NewiBioCMD_NIBP_STOP()
{
	unsigned char packet[10];
	packet[0]=CMD_NIBP_STOP;
	newSendCommand(packet,2);
	if(B_PRINTF) printf("Send CMD_NBP_STOP\n");
	return 0;
}

static int NewiBioCMD_NIBP_RST()
{
	unsigned char packet[10];
	packet[0]=CMD_NIBP_RESET;
	newSendCommand(packet,2);
	if(B_PRINTF) printf("Send CMD_NBP_REST\n");
	return 0;
}



static int NewiBioCMD_NIBP_PRSURE(WORD Pressure,BYTE Type)
{
	unsigned char packet[10];
	packet[0]=CMD_NIBP_PRSURE;
	packet[1]=(Pressure&0x7f);
	packet[2]=((Pressure>>7)&0x03);
	packet[2]+=(Type<<2);
	
	newSendCommand(packet,4);
	
	if(B_PRINTF) printf("Send CMD_NBP_PRSURE\n");
	
	return 0;
}

static int NewiBioCMD_NIBP_PVT(BYTE pump,BYTE dump,BYTE defalte)
{
	unsigned char packet[10];
	packet[0]=CMD_NIBP_PVT;
	packet[1]=pump;
	packet[1]+=dump<<1;
	packet[1]+=defalte<<2;
	newSendCommand(packet,3);
	if(B_PRINTF) printf("Send CMD_NBP_PVT\n");
	
	return 0;
}

static int NewiBioCMD_IBP_MODE()
{
	unsigned char packet[10];
	packet[0]=CMD_IBP_MODE;
	
	newSendCommand(packet,3);
	if(B_PRINTF) printf("Send CMD_NBP_PVT\n");
	return 0;
}
static int NewiBioCMD_IBP_ZERO()
{
	unsigned char packet[10];
	packet[0]=CMD_IBP_ZERO;

	newSendCommand(packet,3);
	if(B_PRINTF) printf("Send CMD_NBP_PVT\n");
	return 0;
}
static int NewiBioCMD_IBP_CAL()
{
	unsigned char packet[10];
	packet[0]=CMD_IBP_CAL;
	
	newSendCommand(packet,3);
	if(B_PRINTF) printf("Send CMD_NBP_PVT\n");
	
	return 0;
}
static int NewiBioCMD_REQ_INFO(BYTE unit)
{
	unsigned char packet[10];
	packet[0]=CMD_REQ_INFO;
	packet[1]=unit;
	
	newSendCommand(packet,3);
	if(B_PRINTF) printf("Send CMD_NBP_PVT\n");
	return 0;
}



/*************************************************************************************/
int Get_Board_info()
{
	 BYTE unit;
	for(unit=0;unit<4;unit++)
		NewiBioCMD_REQ_INFO(unit);

	return 0;
}

int Set_ECG_Relearn()
{
	NewiBioCMD_ECG_LEARN();
	return 0;
}

int Set_Patient_info()
{
	switch(gCfgPatient.bObject){
		case ADULT:{
			if(gCfgPatient.bAge>13)
				NewiBioCMD_PATIENT_INFO(gCfgPatient.bAge);
			else
				NewiBioCMD_PATIENT_INFO(15);
		}break;
		case CHILD:{
			if(gCfgPatient.bAge>3&&gCfgPatient.bAge<=13)
				NewiBioCMD_PATIENT_INFO(gCfgPatient.bAge);
			else
				NewiBioCMD_PATIENT_INFO(13);
		}break;
		case BABY:{
			if(gCfgPatient.bAge<3)
				NewiBioCMD_PATIENT_INFO(gCfgPatient.bAge);
			else
				NewiBioCMD_PATIENT_INFO(2);
		}break;
	}

}

int Set_Ecg_Lead()
{
	static BYTE leadtype=0xff,channel=0xff;
	if(leadtype!=gCfgEcg.bLeadType||channel !=gCfgEcg.bChannel1 ) {
		leadtype=gCfgEcg.bLeadType;
		channel=gCfgEcg.bChannel1 ;
		if(leadtype!=ECGTYPE_3LEAD)
			NewiBioCMD_ECG_LEAD(TRUE, channel);
		else
			NewiBioCMD_ECG_LEAD(FALSE, channel);
	}
	return 0;
}

int Set_Ecg_Mode()
{
	static BYTE mode=0xff;
	if(mode != gCfgEcg.bMode){
		mode = gCfgEcg.bMode;
		NewiBioCMD_ECG_BW(mode);
	}
	return 0;
}
extern BOOL gbiBioECGCaling;

int Start_Ecg_Cal(BYTE type)
{
	//是否处在定标状态
 	gbiBioECGCaling = TRUE;
	
	NewiBioCMD_ECG_CAL(type);

	return 0;
}

int Stop_Ecg_Cal()
{
	//是否处在定标状态
 	gbiBioECGCaling = FALSE;
	
	NewiBioCMD_ECG_CAL(gbiBioECGCaling);

	return 0;
}

int Set_Ecg_Pace_mode()
{
	static BYTE pacemaker;
	if(pacemaker != gCfgEcg.bPacemaker){
		pacemaker = gCfgEcg.bPacemaker;	
		NewiBioCMD_PACE_DET(pacemaker);
	}
	return 0;
}



int Get_ST_templet()
{
	NewiBioCMD_ST_TEMP();
	return 0;
}
int Set_Ecg_ST_ISO()
{
	static BYTE st=0xff,iso=0xff;
	if(st != gCfgEcg.bST || iso != gCfgEcg.bISO){
		st = gCfgEcg.bST; 
		iso = gCfgEcg.bISO;	
		
		NewiBioCMD_ST_ISO(st,iso);
	}
	return 0;
}
int Get_Ecg_ST_ISO()
{
	NewiBioCMD_ST_ISO(0,0);
	return 0;
}


//开始NIBP测量
int StartNibpMeasure(int type)
{

//	NewiBioCMD_NIBP_PRSURE(160,3);
	
	NewiBioCMD_NIBP_START(type,0,gCfgNibp.bObject+1);

	//复位计数器
	giNibpProtectCounter = 0;
	gbNibpProtect = FALSE;	
	gValueNibp.wErrCode=0;
	return 0;
}

//取消NIBP测量
int StopNibpMeasure()
{

	NewiBioCMD_NIBP_STOP();
	
	//复位计数器
	giNibpProtectCounter = 0;
	gbNibpProtect = FALSE;
	
	return 0;
}

int SetNibpVenipuncture()
{
	
	//NewiBioCMD_NIBP_START(NIBP_STATIC_VEN,)
	NewiBioCMD_NIBP_PRSURE(gCfgNibp.wVP,3);
	
	gbNibpProtect = FALSE;
	return 0;
}
int StartVenipuncture()
{
	
	NewiBioCMD_NIBP_START(NIBP_STATIC_VEN,0,0);
	return 0;
}
int StartNIBPLeakTest()
{
	NewiBioCMD_NIBP_START(NIBP_LEAK_TEST,0,0);
	return 0;
}
int StartNIBPPumpTest()
{
	NewiBioCMD_NIBP_START(NIBP_P_V_TEST,0,0);
	
	NewiBioCMD_NIBP_PVT(1,1,1);
	return 0;
}

int StartNIBPOverPressureTest(BYTE object)
{
	BYTE type;
	
	if(object==0) type=1;
	if(object==1) type=2;
	if(object==2) type=3;
	
	NewiBioCMD_NIBP_START(NIBP_OVER_PRE_TESE,0,type);
	return 0;
}
int Nibp_Reset()
{
	NewiBioCMD_NIBP_RST();
	return 0;
}	

int StartNibpCalTest()
{
	NewiBioCMD_NIBP_START(NIBP_PRE_CAL,0,0);
	return 0;
}

int NibpCalPressure()
{
	//printf("gValueNibp.wCalPreValue=%d\n",gValueNibp.wCalPreValue);
	NewiBioCMD_NIBP_PRSURE(gValueNibp.wCalPreValue,7);
	return 0;
}
int Set_NIBP_Init_pressure(BYTE type)
{
	
	NewiBioCMD_NIBP_PRSURE(gCfgNibp.wInitPres,type);
	
	return 0;
}
int Set_RespGain()
{
	static BYTE gain=0xff;
	if(gain != gCfgResp.bGain){
		gain = gCfgResp.bGain;
		NewiBioCMD_RESP_GAIN(gain);
	}
	return 0;
}
int Set_RespLead()
{
	static BYTE leadtype=0xff;
	if(leadtype != gCfgResp.bLeadType){
		leadtype = gCfgResp.bLeadType;
	
		NewiBioCMD_RESP_LEAD(leadtype);
	}
	return 0;
}
int Set_Spo2_Sen()
{
	static BYTE sen=0xff;

	if(sen != gCfgSpO2.bResponse){
		
		sen = gCfgSpO2.bResponse;
		
		NewiBioCMD_SPO2_SEN(sen);
	}
	
	return 0;
}


/*******send cmd end********************************************************************************/
/*
	接收线程
*/
void  *ptProc_newiBioRecv(void *arg)
{
	int iLen; 
	int i;
	unsigned char bBuff[RECVBUFF_MAX_IBIO];	
	
	if(B_PRINTF) printf("(%s:%d)create read thread for %s .\n", __FILE__, __LINE__, gSerialsName[gSerialIndex_newiBio]);
	
	for(;;){

		iLen = iBio_Recv(bBuff, RECVBUFF_MAX_IBIO, 115200);
		
		if(iLen>0){
			
 			newiBio_UnBind(bBuff,iLen);
			
		}
		else{
			
			if(B_PRINTF) printf("(%s:%d) %s ID:%d receive error.\n\n", __FILE__, __LINE__, gSerialsName[gSerialIndex_newiBio], gSerialIndex_newiBio);
			
		}
	}
	
	if(B_PRINTF) printf("\nBye from %d thread.\n", gfdSerial_newiBio);
	pthread_exit("Bye");
}

int Broad_init()
{

	Set_Ecg_Lead();
	
	Set_Ecg_Mode();
	
	Set_Ecg_ST_ISO();
	
	Set_RespGain();
	
	Set_RespLead();
	
	Set_Spo2_Sen();
	
	Set_NIBP_Init_pressure(2-gCfgNibp.bObject);
	
	return 0;
}

int Set_broad_para(){
	
	static int set_para_count=0;
	switch(set_para_count){
		case 0:{Set_Ecg_Lead();}break;
		case 1:{ Set_Ecg_Mode();}break;
		case 2:{Set_Ecg_ST_ISO(); }break;
		case 3:{Set_RespGain(); }break;
		case 4:{Set_RespLead(); }break;
		case 5:{Set_Spo2_Sen(); }break;
//		case 6:{ Set_NIBP_Init_pressure(2-gCfgNibp.bObject);}break;
		case 6:{ Set_Ecg_Pace_mode();}break;
		//case 8:{ }break;
		//case 9:{ }break;
		//case 10:{ }break;
		//case 11:{ }break;
		
		default:break;
			
	}
	set_para_count=(set_para_count+1)%7;
	return 0;
}


/*
	初始化测量模块
*/
int NewInitiBio()
{
	int res;
		
	if(B_PRINTF) printf("%s:%d Init new iBio board ...\n", __FILE__, __LINE__);
	//初始化信号量
	res = sem_init(&semSend_newiBio, 0, 1);	
	
	//打开串口设备
	gfdSerial_newiBio = port_open(gSerialsName[gSerialIndex_newiBio], "115200", 8, 0, 1, 0);
//	gfdSerial_newiBio = port_open(gSerialsName[gSerialIndex_newiBio], "120000", 8, 0, 1, 0);
	if(gfdSerial_newiBio<0){
		if(B_PRINTF) printf("%s:%d Serial_newiBio ERROR!\n", __FILE__, __LINE__);
		return -1;
	}else
		if(B_PRINTF) printf("%s:%d Serial_newiBio=%d OK!\n", __FILE__, __LINE__,gfdSerial_newiBio);
		
	//注册接收线程 
	res = pthread_create(&ptID_newiBioRecv, NULL, ptProc_newiBioRecv, NULL);
	if(res<0){
		if(B_PRINTF) printf("%s:%d newBioRecv pthread ERROR!\n", __FILE__, __LINE__);
		return -1;
	}else
		if(B_PRINTF) printf("%s:%d OK!\n", __FILE__, __LINE__);
		
	Get_Board_info();
//	Broad_init();
	return 0;
}

////////////////////////////////////////

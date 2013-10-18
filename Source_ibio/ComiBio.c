/*************************************************************************** 
 *	Module Name:	ComiBio
 *
 *	Abstract:	参数版iBio 模块通信程序
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			hejp	2012-12-25 20:25:35
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Serial_Funcs.h"
#include "General_Funcs.h"
#include "Central.h"
#include "ComiBio.h"
#include "NewDrawWave.h"

#define CHANNEL1  0x00
#define CHANNEL2  0x10
#define ECGLEAD3 	1
#define ECGLEAD5 	0

#define DATAID 	0
#define DATAHEAD 	1
//命令应答
#define CMDOK  		0
#define ERR_SUM  		1
#define ERR_LEN  		2
#define ERR_INVALCMD  	3
#define ERR_CMDPARA 		4
#define ERR_ACCEPT 		5

//test
#define ID_ECGWAVE_TRUE	 0
#define ID_RESPWAVE_TRUE	 0
#define ID_SPO2_WAVE_TRUE	 0

//串口名称
extern unsigned char *gSerialsName[];
//MemDC
extern HDC hdcWaveMem ;
//绘图环境
extern HDC hdcWave;
extern NWAVE_PANEL gNWavePanels[NWAVEPANEL_MAX_COUNT];
//模块对应的串口号，此处使用变量而不是宏，是为了以后可以动态分配串口
//int gSerialIndex_iBio = DEV_SERIAL0;		//RS232
int gSerialIndex_iBio = DEV_SERIAL2;		//RSttf
//模块对应的串口设备
int gfdSerial_iBio = -1;

//串口写操作的同步信号量
sem_t semSend_iBio;

//串口接收缓冲
#define RECVBUFF_MAX_IBIO		128

//定义报文的最小长度
#define PACK_MIN_LENGTH_IBIO		15

//串口接收线程ID
pthread_t ptID_iBioRecv;
extern BOOL gbSaveRealEcgData;

//心率声
int gbHaveBeep_QRS ;

//脉搏声
BOOL gbHaveBeep_Pulse ;
//ECG是否处在定标状态
BOOL gbiBioECGCaling = FALSE;
BOOL gSTStatus=FALSE;
BYTE gST1Wave[250],gST2Wave[250];

//NIBP过压保护
#define NIBP_PROTECT_ADULT			330
#define NIBP_PROTECT_ADULT_S		300
#define NIBP_PROTECT_CHILD			264
#define NIBP_PROTECT_CHILD_S		240
#define NIBP_PROTECT_BABY			165
#define NIBP_PROTECT_BABY_S		150

//过压保护计数器 根据采样间隔(20ms)计数
extern int giNibpProtectCounter ;
//15s的保护时间
#define NIBP_PROTECT_TIME		750		//50pack x 15s
//软件过压保护标志
extern BOOL gbNibpProtect ;
//NIBP校准标志位
extern BOOL gbNibpCal ;
//NIBP漏气检测标志位
extern BOOL gbNibpLeak ;
//NIBP 启动停止按钮
extern BOOL gbNibpKey_Start ;

//与中央机的连接状态
extern BOOL gbCentralStatus;
//与中央机的连接状态
extern BOOL gbOtherCentralStatus;

extern BOOL gbSaveWaveValuetofile;
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

BOOL gbHavePack=FALSE;
BOOL gSave=FALSE;
#define SAVETIME 125*10 //保存数据的长度
unsigned int gSaveWave[SAVETIME];
static int ccc=0;
char gPackLength[128] =
{
	0 , // 00: 保留
	2 , // 01: 系统复位信息
	5 , // 02: 系统状态
	9 , // 03: 系统自检
	5 , // 04: 命令应答
	8 , // 05: ECG波形数据
	5 , // 06: 导联连接和信号信息
	5 , // 07: 心率
	0 , // 08: 保留
	5 , // 09: 保留(********未定义***********)
	0 ,//6 , // 0a: 保留(********未定义***********)
	7 , // 0b: ST偏移
	9 , // 0c: ST段波形值
	0 , // 0d: 保留
	0 , // 0e: 保留
	0, // 0f: 保留
	4, // 10:呼吸波
	5, // 11: 呼吸率
	6, // 12:窒息报警
	4, // 13: CVA报警信息
	0, // 14:保留
	8, // 15:体温数据
	5, // 16: SPO2波形数据
	7, // 17: SPO2数据
	7, // 18: IBP波形数据
	4, // 19: IBP状态
	10, // 1a: IBP压力
	6, // 1b: IBP校零和校准信息
	7, // 1c: IBP校零和校准时间
	0, // 1d:保留
	0, // 1e: 保留
	0, // 1f: 保留
	7, // 20: NBP实时测量数据
	4, // 21: NBP测量结束包
	9, // 22: NBP测量结果1
	5, // 23: NBP测量结果2
	8, // 24: NBP
	0, // 25:保留
	0, // 26:保留
	0, // 27:保留
	0, // 28:保留
	0, // 29:保留
	0, // 2a: 保留
	0, // 2b:保留
	0, // 2c: 保留
	0, // 2d:保留
	0, // 2e: 保留
	0, // 2f: 保留
	0, // 30:保留
	0, // 31:保留
	0, // 32:保留
	0, // 33:保留
	0, // 34:保留
	0, // 35:保留
	0, // 36:保留
	0, // 37:保留
	0, // 38:保留
	0, // 39:保留
	0, // 3a: 保留
	0, // 3b:保留
	0, // 3c: 保留
	0, // 3d:保留
	0, // 3e: 保留
	0, // 3f: 保留
	2, // 40:读取自检结果
	4, // 41:接收数据选择
	4, // 42:病人信息设置
	0, // 43:保留
	0, // 44:保留
	4, // 45:导联模式选择:3,5导
	4, // 46:导联方式选择
	4, // 47:滤波方式选择
	4, // 48:心电增益
	4, // 49: 1mV校准信号设置
	4, // 4a: 工频抑制设置
	4, // 4b:起搏分析开关设置
	7, // 4c: ST测量的ISO 和ST点
	4, // 4d:设置心电分析通道
	2, // 4e: 心电自学习触发
	0, // 4f: 保留
	4, // 50:呼吸增益
	0,// 51: 保留
	4, // 52:窒息报警时间选择
	4, // 53:体温探头类型设置
	4, // 54: SPO2设置
	2, // 55: NBP开始一次手动/自动测量
	2, // 56: NBP结束测量
	4, // 57: NBP测量周期设置
	2, // 58: NBP开始校准
	2, // 59: NBP复位
	2, // 5a: NBP漏气检测
	2, // 5b: NBP查询状态
	5, // 5c: NBP设置初次充气压力
	2, // 5d: NBP开始STAT测量
	2, // 5e: NBP查询结果
	0, // 5f: 保留
	4, // 60: IBP设置压力名称
	4, // 61: IBP校零
	7, // 62: IBP1校准
	7, // 63: IBP2校准
	4, // 64:设置数字滤波模式
	5, // 65: IBP查询
	0, // 66:保留
	0,///4, // 67:保留(********未定义***********)
	0, // 68:保留
	0, // 69:保留
	0, // 6a: 保留
	0, // 6b:保留
	0, // 6c: 保留
	0, // 6d:保留
	0, // 6e: 保留
	0, // 6f: 保留
	0, // 70:保留
	0, // 71:保留
	0, // 72:保留
	0, // 73:保留
	0, // 74:保留
	0, // 75:保留
	0, // 76:保留
	0, // 77:保留
	0, // 78:保留
	0, // 79:保留
	0, // 7a: 保留
	0, // 7b:保留
	0, // 7c: 保留
	0, // 7d:保留
	0, // 7e: 保留
	0, // 7f: 保留
};
#define ECGCOUNT	376
#define RESPCOUNT	754
#define SPO2COUNT	152
#define IBP1COUNT	96
#define IBP2COUNT	96
unsigned int DEMO_ECG_II[ECGCOUNT]={
	0x0abb,0x0aa2,0x0a67,0x0a31,0x09f0,0x09a4,0x0950,0x08f6,0x0895,0x083c,0x07f2,0x07a6,0x0770,0x0763,
	0x0767,0x0764,0x0769,0x0775,0x0786,0x079f,0x07b4,0x07bd,0x07bd,0x07ba,0x07b7,0x07b4,0x07b9,0x07ba,0x07ba,0x07bd,
	0x07bb,0x07b4,0x07b1,0x07b4,0x07b9,0x07be,0x07c6,0x07c7,0x07c0,0x07bc,0x07b7,0x07b1,0x07b4,0x07bf,0x07c6,0x07cb,
	0x07ce,0x07c9,0x07c1,0x07ba,0x07ba,0x07bd,0x07c5,0x07cf,0x07d1,0x07d1,0x07d1,0x07ce,0x07c9,0x07ca,0x07d0,0x07d5,
	0x07db,0x07e2,0x07df,0x07df,0x07df,0x07dd,0x07de,0x07e3,0x07e5,0x07e5,0x07e6,0x07e8,0x07e9,0x07e9,0x07ec,0x07ec,
	0x07ee,0x07f2,0x07f3,0x07f1,0x07f1,0x07f4,0x07f7,0x07fc,0x0800,0x0800,0x07fe,0x07ff,0x07fe,0x07fe,0x0801,0x0804,
	0x0809,0x080b,0x080d,0x080f,0x0810,0x0812,0x0813,0x0817,0x081b,0x0823,0x0829,0x082a,0x0830,0x0832,0x0834,0x083a,
	0x083f,0x0840,0x0845,0x084a,0x084c,0x084c,0x084f,0x0852,0x0853,0x0851,0x0850,0x0852,0x0852,0x0850,0x084f,0x084c,
	0x084a,0x0846,0x0840,0x083d,0x083b,0x0834,0x082e,0x082b,0x0825,0x081b,0x0816,0x080f,0x0801,0x07fa,0x07f0,0x07e3,
	0x07d9,0x07d5,0x07d1,0x07ce,0x07ca,0x07c6,0x07c1,0x07bb,0x07ba,0x07b9,0x07b7,0x07bb,0x07ba,0x07b7,0x07b8,0x07b7,
	0x07b5,0x07b6,0x07b9,0x07b9,0x07b9,0x07ba,0x07b9,0x07b7,0x07b8,0x07b9,0x07b7,0x07b9,0x07bb,0x07b9,0x07b7,0x07b9,
	0x07b8,0x07b7,0x07ba,0x07bb,0x07b8,0x07b7,0x07b9,0x07ba,0x07b7,0x07bb,0x07bc,0x07ba,0x07b9,0x07bb,0x07ba,0x07ba,
	0x07bc,0x07b9,0x07b7,0x07bb,0x07b9,0x07b7,0x07ba,0x07ba,0x07b8,0x07b9,0x07b9,0x07b9,0x07bb,0x07ba,0x07b8,0x07b8,
	0x07b9,0x07ba,0x07b9,0x07ba,0x07ba,0x07ba,0x07b9,0x07bb,0x07bb,0x07ba,0x07bc,0x07bd,0x07bc,0x07bc,0x07ba,0x07b8,
	0x07b8,0x07bc,0x07ba,0x07b9,0x07be,0x07bd,0x07b9,0x07b9,0x07bc,0x07ba,0x07b7,0x07b9,0x07ba,0x07b9,0x07bb,0x07be,
	0x07ba,0x07b9,0x07ba,0x07ba,0x07b9,0x07ba,0x07ba,0x07bb,0x07bb,0x07ba,0x07ba,0x07bc,0x07ba,0x07b8,0x07ba,0x07bd,
	0x07bc,0x07bc,0x07bd,0x07bb,0x07b9,0x07b9,0x07bb,0x07bb,0x07b8,0x07ba,0x07bd,0x07bc,0x07bc,0x07bd,0x07bb,0x07ba,
	0x07bc,0x07bb,0x07b9,0x07bc,0x07bd,0x07bd,0x07c3,0x07ca,0x07cf,0x07d5,0x07db,0x07e2,0x07e9,0x07f3,0x07fa,0x07fe,
	0x0801,0x0806,0x080a,0x080d,0x0814,0x0815,0x0814,0x0818,0x0818,0x0818,0x081a,0x081b,0x0819,0x0818,0x0814,0x0810,
	0x0809,0x0807,0x0804,0x0800,0x07fe,0x07fa,0x07f0,0x07e9,0x07e5,0x07dc,0x07d3,0x07cc,0x07c7,0x07c2,0x07bf,0x07bd,
	0x07ba,0x07bb,0x07bd,0x07ba,0x07b8,0x07bd,0x07bf,0x07bb,0x07bd,0x07be,0x07bb,0x07ba,0x07ba,0x07b8,0x07b7,0x07bc,
	0x07bd,0x07bb,0x07bc,0x07be,0x07bc,0x07bc,0x07bc,0x07bb,0x07b9,0x07ba,0x07bd,0x07bc,0x07be,0x07bf,0x07bb,0x07bb,
	0x07bd,0x07ba,0x07bb,0x07bc,0x07bd,0x07ba,0x07b2,0x07a9,0x079d,0x0790,0x0785,0x0777,0x0769,0x076f,0x0799,0x07d8,
	0x0817,0x085d,0x08a7,0x08f9,0x0952,0x09a4,0x09f4,0x0a43,0x0a9b,0x0abb,

};
unsigned int DEMO_ECG_I[ECGCOUNT]={
	0x0a0f,0x09ea,0x09bb,0x09a2,0x0980,
	0x094d,0x0910,0x08d0,0x0890,0x0857,0x0825,0x07f8,0x07dd,0x07e2,0x07e2,0x07d5,0x07d2,0x07d4,0x07d8,0x07e5,0x07f6,
	0x07f6,0x07f0,0x07f1,0x07f0,0x07eb,0x07e7,0x07ed,0x07f3,0x07f5,0x07f4,0x07ec,0x07e5,0x07e8,0x07ec,0x07ed,0x07f2,
	0x07f8,0x07ef,0x07ea,0x07ee,0x07e6,0x07e2,0x07ee,0x07fd,0x07fe,0x07fb,0x07f9,0x07f0,0x07eb,0x07ee,0x07f0,0x07f3,
	0x0801,0x0804,0x07fe,0x07f8,0x07f5,0x07ee,0x07f1,0x0800,0x0803,0x0802,0x0803,0x0805,0x0800,0x07fb,0x07ff,0x0805,
	0x0808,0x0810,0x080e,0x0806,0x080a,0x080a,0x0808,0x0809,0x0811,0x0812,0x0811,0x0814,0x080d,0x0806,0x080c,0x0818,
	0x0819,0x081a,0x081c,0x0819,0x0814,0x0816,0x0819,0x081c,0x0826,0x082e,0x082b,0x0825,0x0824,0x0821,0x0821,0x082c,
	0x082f,0x082e,0x0833,0x0839,0x0835,0x0830,0x0835,0x083c,0x0841,0x0849,0x0849,0x0844,0x0849,0x084e,0x084c,0x084d,
	0x0856,0x0855,0x084f,0x0854,0x084f,0x0845,0x0849,0x0852,0x084f,0x0849,0x0849,0x0843,0x083b,0x0839,0x0837,0x0833,
	0x0835,0x0835,0x082c,0x0822,0x081a,0x0812,0x080b,0x080b,0x0808,0x0800,0x07fe,0x07ff,0x07f8,0x07f0,0x07f0,0x07f3,
	0x07f2,0x07f5,0x07f5,0x07ec,0x07eb,0x07ee,0x07ee,0x07ed,0x07f1,0x07f2,0x07ef,0x07f0,0x07ef,0x07e4,0x07e8,0x07f3,
	0x07f3,0x07ef,0x07ef,0x07ee,0x07ea,0x07ea,0x07ec,0x07eb,0x07f0,0x07f6,0x07f2,0x07ea,0x07eb,0x07eb,0x07e7,0x07ef,
	0x07f6,0x07ef,0x07ef,0x07f4,0x07ef,0x07e6,0x07e9,0x07ef,0x07f0,0x07f4,0x07f4,0x07ed,0x07eb,0x07ed,0x07ed,0x07ee,
	0x07f2,0x07f1,0x07ee,0x07f0,0x07f0,0x07e7,0x07ea,0x07f4,0x07f3,0x07ef,0x07f1,0x07f0,0x07e9,0x07ea,0x07ee,0x07ee,
	0x07f2,0x07f9,0x07f5,0x07ee,0x07ec,0x07ea,0x07e8,0x07f1,0x07f8,0x07f2,0x07f1,0x07f5,0x07ef,0x07e8,0x07ec,0x07f2,
	0x07f2,0x07f4,0x07f5,0x07ed,0x07eb,0x07ef,0x07ed,0x07ed,0x07f4,0x07f7,0x07f1,0x07f3,0x07f2,0x07ea,0x07ec,0x07f7,
	0x07f6,0x07f0,0x07f3,0x07f4,0x07ed,0x07ec,0x07ef,0x07ef,0x07f1,0x07f7,0x07f6,0x07ef,0x07ee,0x07ed,0x07ea,0x07f1,
	0x07f7,0x07f2,0x07f1,0x07f6,0x07f0,0x07e6,0x07ee,0x07f4,0x07f1,0x07f7,0x07f9,0x07ee,0x07eb,0x07f0,0x07f0,0x07f4,
	0x0800,0x0801,0x0800,0x0805,0x080b,0x0807,0x080b,0x081a,0x081e,0x081a,0x0820,0x0823,0x081f,0x0822,0x0827,0x0826,
	0x082d,0x0833,0x082c,0x0827,0x0828,0x0825,0x0821,0x082a,0x082e,0x0825,0x0820,0x0821,0x081a,0x080f,0x080d,0x0810,
	0x080b,0x080b,0x0809,0x07fe,0x07f8,0x07f6,0x07ee,0x07ed,0x07f4,0x07f6,0x07f0,0x07ee,0x07f1,0x07e9,0x07ea,0x07f5,
	0x07f4,0x07f1,0x07f5,0x07f3,0x07eb,0x07eb,0x07f1,0x07ef,0x07f3,0x07fa,0x07f6,0x07ef,0x07f0,0x07ee,0x07e7,0x07f0,
	0x07f9,0x07f2,0x07f3,0x07f5,0x07ee,0x07e7,0x07ed,0x07f2,0x07f0,0x07f6,0x07f7,0x07ef,0x07ef,0x07f2,0x07ea,0x07e4,
	0x07e5,0x07dc,0x07ca,0x07c4,0x07b9,0x07a7,0x07b1,0x07e4,0x0810,0x0839,0x0871,0x08a5,0x08d7,0x0916,0x0957,0x098d,
	0x09c6,0x0a00,0x0a0f,

};
unsigned int DEMO_ECG_V[ECGCOUNT]={
	0x062c,0x0640,0x0668,0x0695,0x06ca,0x0702,0x0749,
	0x0798,0x07cb,0x07e1,0x07ea,0x07ea,0x07e6,0x07e9,0x07e7,0x07e3,0x07e5,
	0x07e5,0x07de,0x07de,0x07e6,0x07e7,0x07e8,0x07f0,0x07f0,0x07e3,0x07e0,
	0x07de,0x07db,0x07e0,0x07eb,0x07ee,0x07ef,0x07f4,0x07f2,0x07e8,0x07e6,
	0x07e6,0x07e4,0x07ea,0x07f8,0x07f4,0x07ee,0x07f2,0x07f0,0x07e8,0x07ed,
	0x07f4,0x07ee,0x07f2,0x07fb,0x07f6,0x07f3,0x07f8,0x07f5,0x07f2,0x07f3,
	0x07f5,0x07ef,0x07f3,0x07fc,0x07f8,0x07f6,0x07fe,0x07fe,0x07f5,0x07f9,
	0x07fc,0x07f5,0x07f9,0x0800,0x07ff,0x07fd,0x0800,0x07fd,0x07f8,0x07fd,
	0x0801,0x07fd,0x0800,0x0808,0x0801,0x07ff,0x0807,0x0805,0x0801,0x0805,
	0x080b,0x0806,0x0809,0x080d,0x0808,0x0808,0x0814,0x0814,0x0813,0x0819,
	0x0818,0x0810,0x0815,0x081e,0x0819,0x081c,0x0827,0x0822,0x081c,0x081f,
	0x081f,0x0818,0x081c,0x0823,0x081b,0x0818,0x0820,0x0818,0x080f,0x0816,
	0x0819,0x0812,0x0814,0x081b,0x0811,0x080a,0x0811,0x080b,0x0803,0x0806,
	0x0805,0x07fb,0x07fa,0x07f8,0x07ea,0x07eb,0x07f4,0x07ef,0x07e9,0x07ee,
	0x07e8,0x07db,0x07e0,0x07e9,0x07e0,0x07e1,0x07ec,0x07ea,0x07e0,0x07e3,
	0x07e2,0x07dc,0x07e1,0x07e8,0x07e1,0x07e0,0x07e9,0x07e3,0x07dc,0x07e3,
	0x07e4,0x07de,0x07e6,0x07ec,0x07e0,0x07de,0x07e9,0x07e2,0x07dc,0x07e7,
	0x07ec,0x07e1,0x07e1,0x07e7,0x07dd,0x07de,0x07e9,0x07e5,0x07e0,0x07e9,
	0x07e8,0x07dd,0x07e1,0x07e5,0x07dd,0x07e0,0x07ed,0x07e7,0x07de,0x07e4,
	0x07e5,0x07dc,0x07e2,0x07eb,0x07e3,0x07e3,0x07ea,0x07e0,0x07db,0x07e6,
	0x07e7,0x07de,0x07e6,0x07ec,0x07df,0x07dd,0x07e9,0x07e3,0x07dc,0x07e7,
	0x07ea,0x07e3,0x07e5,0x07e6,0x07db,0x07dd,0x07ea,0x07e3,0x07e0,0x07e9,
	0x07e5,0x07db,0x07e2,0x07e5,0x07dd,0x07e1,0x07ec,0x07e6,0x07de,0x07e5,
	0x07e4,0x07dc,0x07e5,0x07e9,0x07e0,0x07e5,0x07ea,0x07e1,0x07db,0x07e6,
	0x07e4,0x07dd,0x07e8,0x07ec,0x07de,0x07df,0x07e9,0x07e1,0x07df,0x07e9,
	0x07e8,0x07e0,0x07e5,0x07e6,0x07dc,0x07df,0x07eb,0x07e4,0x07e3,0x07ef,
	0x07e6,0x07da,0x07e3,0x07e7,0x07db,0x07e1,0x07ef,0x07ea,0x07e6,0x07f2,
	0x07f6,0x07f2,0x07fe,0x0808,0x0804,0x080b,0x0815,0x080e,0x080b,0x0818,
	0x0819,0x0814,0x0821,0x0828,0x081c,0x081c,0x0826,0x081f,0x0819,0x0827,
	0x0826,0x0819,0x081c,0x081b,0x080c,0x080c,0x0816,0x080a,0x0805,0x080d,
	0x0803,0x07f6,0x07f5,0x07f1,0x07e6,0x07e9,0x07ee,0x07e6,0x07df,0x07e5,
	0x07e0,0x07da,0x07e4,0x07e7,0x07e1,0x07e5,0x07ea,0x07df,0x07db,0x07e6,
	0x07e3,0x07dc,0x07e4,0x07e8,0x07de,0x07df,0x07e7,0x07de,0x07db,0x07e7,
	0x07e6,0x07de,0x07e1,0x07e0,0x07d9,0x07e2,0x07e8,0x07de,0x07e0,0x07ec,
	0x07e3,0x07db,0x07e1,0x07e2,0x07dc,0x07e2,0x07ea,0x07e0,0x07dd,0x07e6,
	0x07e1,0x07db,0x07e1,0x07e4,0x07e4,0x07f4,0x0802,0x0803,0x080f,0x082b,
	0x083a,0x0848,0x0861,0x0871,0x087a,0x088c,0x0890,0x0866,0x0836,0x0815,
	0x07e3,0x07a4,0x076a,0x0728,0x06e3,0x06ab,0x0678,0x068c,0x06ac,
};
unsigned int DEMO_RESP[RESPCOUNT]={
	0x00c1,0x00c1,0x00c1,0x00c1,0x00c1,0x00c1,0x00c0,
	0x00c0,0x00c0,0x00bf,0x00bf,0x00bf,0x00be,0x00be,0x00bd,0x00bd,0x00bc,
	0x00bc,0x00bb,0x00bb,0x00bb,0x00bb,0x00ba,0x00ba,0x00b9,0x00b9,0x00b8,
	0x00b8,0x00b7,0x00b7,0x00b6,0x00b6,0x00b6,0x00b6,0x00b6,0x00b5,0x00b5,
	0x00b5,0x00b4,0x00b3,0x00b3,0x00b2,0x00b1,0x00b1,0x00b0,0x00af,0x00af,
	0x00ae,0x00ae,0x00ad,0x00ad,0x00ac,0x00ac,0x00ab,0x00aa,0x00a9,0x00a8,
	0x00a8,0x00a7,0x00a7,0x00a6,0x00a6,0x00a5,0x00a5,0x00a4,0x00a4,0x00a3,
	0x00a2,0x00a1,0x00a0,0x00a0,0x009f,0x009e,0x009d,0x009c,0x009b,0x009b,
	0x009a,0x0099,0x0098,0x0097,0x0096,0x0095,0x0094,0x0094,0x0093,0x0092,
	0x0091,0x0091,0x0090,0x008f,0x008e,0x008d,0x008d,0x008c,0x008b,0x008a,
	0x0089,0x0088,0x0087,0x0086,0x0085,0x0084,0x0083,0x0082,0x0081,0x0080,
	0x007f,0x007e,0x007e,0x007d,0x007c,0x007b,0x007a,0x0079,0x0078,0x0076,
	0x0075,0x0075,0x0074,0x0073,0x0072,0x0071,0x0070,0x006f,0x006e,0x006d,
	0x006c,0x006c,0x006b,0x006a,0x0069,0x0069,0x0068,0x0067,0x0066,0x0065,
	0x0064,0x0063,0x0062,0x0061,0x0061,0x0060,0x005f,0x005e,0x005d,0x005c,
	0x005b,0x005b,0x005b,0x005a,0x005a,0x0059,0x0058,0x0058,0x0057,0x0057,
	0x0056,0x0056,0x0055,0x0054,0x0054,0x0054,0x0054,0x0053,0x0053,0x0053,
	0x0052,0x0052,0x0052,0x0052,0x0052,0x0052,0x0051,0x0051,0x0050,0x004f,
	0x004f,0x004e,0x004e,0x004d,0x004d,0x004c,0x004c,0x004c,0x004c,0x004b,
	0x004b,0x004b,0x004a,0x004a,0x0049,0x0049,0x0049,0x0049,0x0049,0x0049,
	0x0049,0x0049,0x0049,0x0049,0x0049,0x0049,0x004a,0x004a,0x004a,0x004a,
	0x004a,0x004a,0x004a,0x004a,0x004a,0x004a,0x004a,0x004b,0x004b,0x004b,
	0x004b,0x004c,0x004c,0x004c,0x004d,0x004d,0x004d,0x004d,0x004e,0x004e,
	0x004f,0x004f,0x0050,0x0050,0x0050,0x0050,0x0051,0x0052,0x0052,0x0053,
	0x0054,0x0055,0x0056,0x0056,0x0057,0x0058,0x0059,0x005a,0x005a,0x005b,
	0x005c,0x005d,0x005e,0x005e,0x005f,0x0060,0x0060,0x0061,0x0061,0x0062,
	0x0063,0x0063,0x0064,0x0064,0x0065,0x0065,0x0066,0x0067,0x0068,0x0069,
	0x006a,0x006b,0x006b,0x006c,0x006d,0x006e,0x006e,0x006f,0x0070,0x0071,
	0x0072,0x0073,0x0074,0x0076,0x0077,0x0077,0x0078,0x0079,0x007a,0x007b,
	0x007c,0x007d,0x007e,0x007f,0x0080,0x0081,0x0082,0x0083,0x0085,0x0086,
	0x0087,0x0088,0x0089,0x008a,0x008b,0x008c,0x008d,0x008e,0x008e,0x008f,
	0x0090,0x0092,0x0093,0x0094,0x0096,0x0097,0x0098,0x0099,0x009a,0x009a,
	0x009b,0x009c,0x009d,0x009e,0x009e,0x009f,0x00a0,0x00a1,0x00a2,0x00a3,
	0x00a3,0x00a4,0x00a4,0x00a5,0x00a5,0x00a6,0x00a6,0x00a7,0x00a7,0x00a7,
	0x00a8,0x00a9,0x00a9,0x00aa,0x00ab,0x00ab,0x00ab,0x00ab,0x00ac,0x00ac,
	0x00ad,0x00ae,0x00ae,0x00ae,0x00af,0x00af,0x00b0,0x00b1,0x00b1,0x00b2,
	0x00b3,0x00b3,0x00b4,0x00b4,0x00b5,0x00b5,0x00b6,0x00b6,0x00b6,0x00b6,
	0x00b6,0x00b6,0x00b7,0x00b7,0x00b7,0x00b7,0x00b7,0x00b6,0x00b6,0x00b6,
	0x00b6,0x00b5,0x00b5,0x00b4,0x00b4,0x00b3,0x00b3,0x00b3,0x00b2,0x00b2,
	0x00b2,0x00b1,0x00b1,0x00b1,0x00b1,0x00b1,0x00b0,0x00b0,0x00b0,0x00af,
	0x00af,0x00ae,0x00ad,0x00ad,0x00ac,0x00ac,0x00ac,0x00ab,0x00ab,0x00ab,
	0x00aa,0x00aa,0x00a9,0x00a9,0x00a8,0x00a8,0x00a7,0x00a7,0x00a7,0x00a6,
	0x00a6,0x00a6,0x00a5,0x00a4,0x00a4,0x00a3,0x00a2,0x00a2,0x00a1,0x00a0,
	0x00a0,0x009f,0x009f,0x009e,0x009d,0x009d,0x009c,0x009b,0x009b,0x009a,
	0x0099,0x0098,0x0097,0x0096,0x0095,0x0094,0x0094,0x0093,0x0092,0x0091,
	0x0090,0x008f,0x008e,0x008d,0x008c,0x008b,0x008a,0x0088,0x0087,0x0087,
	0x0086,0x0085,0x0084,0x0083,0x0082,0x0082,0x0081,0x0081,0x0080,0x0080,
	0x007f,0x007e,0x007e,0x007d,0x007d,0x007c,0x007b,0x007a,0x0079,0x0078,
	0x0078,0x0078,0x0077,0x0077,0x0076,0x0075,0x0074,0x0073,0x0072,0x0071,
	0x0070,0x006f,0x006e,0x006d,0x006c,0x006b,0x006a,0x0069,0x0069,0x0068,
	0x0067,0x0066,0x0065,0x0064,0x0063,0x0062,0x0061,0x0060,0x005f,0x005e,
	0x005e,0x005d,0x005c,0x005b,0x005a,0x0059,0x0058,0x0058,0x0057,0x0057,
	0x0056,0x0056,0x0055,0x0055,0x0054,0x0053,0x0053,0x0052,0x0052,0x0051,
	0x0050,0x0050,0x004f,0x004e,0x004e,0x004d,0x004c,0x004c,0x004b,0x004b,
	0x004b,0x004b,0x004a,0x004a,0x004a,0x0049,0x0049,0x0048,0x0048,0x0048,
	0x0047,0x0047,0x0046,0x0045,0x0045,0x0044,0x0044,0x0044,0x0044,0x0043,
	0x0043,0x0042,0x0042,0x0042,0x0042,0x0042,0x0042,0x0041,0x0041,0x0041,
	0x0041,0x0041,0x0041,0x0042,0x0042,0x0042,0x0043,0x0043,0x0043,0x0044,
	0x0044,0x0044,0x0044,0x0044,0x0044,0x0045,0x0045,0x0045,0x0046,0x0046,
	0x0047,0x0047,0x0048,0x0049,0x004a,0x004a,0x004b,0x004b,0x004c,0x004c,
	0x004c,0x004d,0x004d,0x004e,0x004f,0x0050,0x0050,0x0051,0x0052,0x0053,
	0x0053,0x0054,0x0055,0x0055,0x0056,0x0056,0x0057,0x0058,0x0059,0x0059,
	0x005a,0x005b,0x005b,0x005c,0x005d,0x005e,0x005f,0x0060,0x0061,0x0061,
	0x0062,0x0063,0x0063,0x0064,0x0065,0x0066,0x0066,0x0067,0x0068,0x0068,
	0x0069,0x006a,0x006b,0x006c,0x006d,0x006e,0x006f,0x0070,0x0072,0x0073,
	0x0074,0x0075,0x0076,0x0077,0x0078,0x0079,0x007a,0x007b,0x007c,0x007d,
	0x007d,0x007e,0x007f,0x007f,0x0080,0x0081,0x0082,0x0083,0x0084,0x0086,
	0x0087,0x0088,0x0089,0x008b,0x008c,0x008d,0x008f,0x0090,0x0091,0x0092,
	0x0093,0x0094,0x0094,0x0095,0x0096,0x0097,0x0098,0x0099,0x009a,0x009b,
	0x009c,0x009d,0x009e,0x009e,0x009f,0x00a0,0x00a1,0x00a2,0x00a3,0x00a4,
	0x00a4,0x00a5,0x00a6,0x00a7,0x00a8,0x00a9,0x00aa,0x00aa,0x00ab,0x00ac,
	0x00ac,0x00ad,0x00ae,0x00af,0x00af,0x00b0,0x00b1,0x00b2,0x00b2,0x00b3,
	0x00b4,0x00b4,0x00b5,0x00b5,0x00b6,0x00b6,0x00b7,0x00b7,0x00b7,0x00b8,
	0x00b8,0x00b9,0x00ba,0x00ba,0x00bb,0x00bb,0x00bc,0x00bc,0x00bc,0x00bd,
	0x00bd,0x00be,0x00be,0x00be,0x00be,0x00bf,0x00bf,0x00c0,0x00c0,0x00c1,
	0x00c1,0x00c1,0x00c1,0x00c1,0x00c1,0x00c1,0x00c1,
};

unsigned int DEMO_SPO2[SPO2COUNT]={
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //1
   0,  1,  2,  3,  5,  7,  9, 12, 16, 19, //2
  23, 27, 31, 34, 37, 40, 43, 45, 48, 50, //3
  53, 55, 57, 58, 59, 60, 61, 62, 62, 62, //4
  63, 63, 63, 63, 62, 61, 61, 59, 58, 58, //5

  57, 56, 55, 53, 53, 52, 52, 51, 50, 49, //6
  46, 45, 44, 43, 42, 41, 40, 39, 39, 39, //7
  40, 40, 41, 42, 43, 43, 43, 43, 43, 43, //8
  43, 43, 43, 42, 42, 41, 40, 40, 39, 38, //9
  36, 35, 33, 32, 31, 30, 29, 28, 27, 26, //10
  
  25, 24, 23, 23, 23, 23, 21, 21, 20, 19, //11
  17, 17, 16, 16, 15, 15, 14, 14, 13, 13, //12
  13, 11, 11, 11, 11, 10, 10, 10,  9,  9, //13
   8,  7,  6,  5,  4,  3,  2,  1,  0,  0, //14
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, //15
   0,  0
};
unsigned int DEMO_IBP1[IBP1COUNT]={
0x0079,0x0079,0x0078,0x0078,0x0078,0x0077,0x0076,0x0076,0x0075,0x0074,
0x0072,0x0071,0x006f,0x006e,0x006c,0x006a,0x0069,0x0068,0x0067,0x0067,
0x0067,0x0067,0x0067,0x0067,0x0067,0x0067,0x0066,0x0065,0x0065,0x0064,
0x0062,0x0061,0x0060,0x005f,0x005e,0x005d,0x005d,0x005c,0x005c,0x005b,
0x005b,0x005a,0x0059,0x0059,0x0058,0x0058,0x0057,0x0056,0x0056,0x0055,
0x0055,0x0054,0x0054,0x0053,0x0053,0x0052,0x0052,0x0052,0x0051,0x0051,
0x0051,0x0051,0x0051,0x0051,0x0051,0x0051,0x0051,0x0051,0x0051,0x0051,
0x0051,0x0051,0x0051,0x0051,0x0052,0x0053,0x0054,0x0056,0x0058,0x005b,
0x005e,0x0061,0x0063,0x0066,0x0068,0x006b,0x006e,0x0071,0x0073,0x0076,
0x0077,0x0078,0x0079,0x0079,0x0079,0x0079,
   };
unsigned int DEMO_IBP2[IBP2COUNT]={
0x001a,0x001a,
0x0019,0x0019,0x0018,0x0016,0x0015,0x0013,0x0013,0x0012,0x0012,0x0012,
0x0012,0x0012,0x0012,0x0011,0x0011,0x0011,0x0010,0x000f,0x000f,0x000e,
0x000d,0x000d,0x000c,0x000b,0x000a,0x0009,0x0008,0x0007,0x0006,0x0005,
0x0004,0x0003,0x0002,0x0001,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
0x0000,0x0001,0x0001,0x0001,0x0001,0x0002,0x0002,0x0003,0x0003,0x0003,
0x0003,0x0004,0x0004,0x0004,0x0004,0x0004,0x0004,0x0004,0x0004,0x0004,
0x0005,0x0005,0x0005,0x0005,0x0005,0x0005,0x0005,0x0005,0x0006,0x0006,
0x0006,0x0006,0x0006,0x0006,0x0007,0x0007,0x0007,0x0008,0x0009,0x000a,
0x000b,0x000c,0x000f,0x0012,0x0014,0x0016,0x0018,0x0019,0x0019,0x001a,
0x001a,0x001a,0x001a,0x001a,
   };
//ECG 取拐点的函数
static int Filter_I(int iValue)
{
	static int a = 0;
	static int b = 0;
	static int iCount = 0;
	int c;
	int iRes;
	
//  	return iValue;
	
	// init
	if(iCount ==0){
		a = iValue;
		iCount ++;
		return iValue;
	}
	if(iCount ==1){
		b = iValue;
		iCount ++;
		return iValue;
	}

	//比较,取拐点
	c = iValue;
	iRes = c;
	if(b>a && b>c) iRes = b;
	if(b<a && b<c) iRes = b;
	if(b>a && b<c) iRes = c;
	if(b<a && b>c) iRes = c;
	
	//推值
	a = b; 
//  	b = iValue;
  	b = iRes;

	return iRes;
}

static int Filter_II(int iValue)
{
	static int a = 0;
	static int b = 0;
	static int iCount = 0;
	int c;
	int iRes;
	
//       return iValue;
	
	// init
	if(iCount ==0){
		a = iValue;
		iCount ++;
		return iValue;
	}
	if(iCount ==1){
		b = iValue;
		iCount ++;
		return iValue;
	}

	//比较,取拐点
	iRes = iValue;
	c = iValue;
	if(b>a && b>c){
//  		if(B_PRINTF) printf("---1 ");
		iRes = b;
	} 
	if(b<a && b<c) {
// 		if(B_PRINTF) printf("---2 ");
		iRes = b;
	}
	if(b>a && b<c){
// 		if(B_PRINTF) printf("---3 ");
		iRes = c;
	}
	if(b<a && b>c){
// 		if(B_PRINTF) printf("---4 ");
		iRes = c;
	}
// 	if(B_PRINTF) printf("\n");
	
	//推值
	a = b; 
// 	b = iValue;
  	b = iRes;

	return iRes;
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
	//printf("giNetWaveLength_Ibp1=%d,giNetWaveLength_Ibp2=%d\n",giNetWaveLength_Ibp1,giNetWaveLength_Ibp2);
		//如果spo2 resp 数量不足，填充完整
		if(giNetWaveLength_SpO2<SPO2_WAVE_PER_SEC){
			for(i=0;i<SPO2_WAVE_PER_SEC-giNetWaveLength_SpO2;i++){
				gbNetWaveBuff_SpO2[giNetWaveLength_SpO2]=gbNetWaveBuff_SpO2[giNetWaveLength_SpO2-1];
				giNetWaveLength_SpO2++;
			}
		}else{
			giNetWaveLength_SpO2=SPO2_WAVE_PER_SEC;
		}
		
		if(giNetWaveLength_Resp<RESP_WAVE_PER_SEC){
			for(i=0;i<RESP_WAVE_PER_SEC-giNetWaveLength_Resp;i++){
				gbNetWaveBuff_Resp[giNetWaveLength_Resp]=gbNetWaveBuff_Resp[giNetWaveLength_Resp-1];
				giNetWaveLength_Resp++;
			}
		}else{
			giNetWaveLength_Resp=RESP_WAVE_PER_SEC;
		}
	
		if(giNetWaveLength_Ibp1<IBP_WAVE_PER_SEC){
			
			for(i=0;i<IBP_WAVE_PER_SEC-giNetWaveLength_Ibp1;i++){
				gbNetWaveBuff_IBP1[giNetWaveLength_Ibp1]=gbNetWaveBuff_IBP1[giNetWaveLength_Ibp1-1];
				giNetWaveLength_Ibp1++;
			}
		}else{
			giNetWaveLength_Ibp1=IBP_WAVE_PER_SEC;
		}
		
		if(giNetWaveLength_Ibp2<IBP_WAVE_PER_SEC){
			for(i=0;i<IBP_WAVE_PER_SEC-giNetWaveLength_Ibp2;i++){
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
	实时打印
*/
int Real_Wave_Print()
{
	
	//设置实时打印数据  200=1s 
		if(giPrintRealWaveIndex>= 200){
			if((gPrinterStatus != PRINT_STATUS_NOPRN )
						 && (gPrinterStatus != PRINT_STATUS_ERR)
					//	 && (gPrinterStatus != PRINT_STATUS_BUSY)
						 && (gPrinterWorkType == PRNWORK_REAL)) {
						 
				//根据实时打印的方式，是连续还是固定时间，判断是否要继续送数
 				if(gCfgPrinter.iDuration == 0) {
					//连续打印，直到按下停止键
					printf("duration printing......\n");
				//	Printer_SetRealWave(&gPrintRealWaves);
					gPrintRealWaves.iCount=3;
					gPrintRealWaves.iLength=giPrintRealWaveIndex;
					Printer_PrintRealWave(&gPrintRealWaves);
				}
				else{
				//打印固定时间
					if(giPrintRealWaveCount < gCfgPrinter.iDuration){
					//	Printer_SetRealWave(&gPrintRealWaves);
						gPrintRealWaves.iCount=3;
						gPrintRealWaves.iLength=giPrintRealWaveIndex;
						if(B_PRINTF) printf("11%s:%d Print %d Countg CfgPrinter.iDuration=%d.\n", __FILE__, __LINE__, giPrintRealWaveCount,gCfgPrinter.iDuration);

						Printer_PrintRealWave(&gPrintRealWaves);
						giPrintRealWaveCount ++;
					}
					else{
						//停止实时打印
						Printer_StopPrintRealWave();

 						if(B_PRINTF) printf("22%s:%d Stop Print. %d Count.\n", __FILE__, __LINE__, giPrintRealWaveCount);
					}
				}
			}
			giPrintRealWaveIndex = 0;
		}
	
	
	return 0;
}

/*
	设置各个导联的波形值，当同时有1， 2导联，其他导联数据可计算出来
	无1， 2导联时，只赋值给相应导联
*/
static int SetECGWaveValue(unsigned int Ch1Value,unsigned int Ch2Value)
{
	int i;
	//if(B_PRINTF) printf("*****%d******%d*****\n",Ch1Value,Ch2Value);
	//界面为7导同屏时通道1 ,2
	if(gCfgSystem.bInterface==SCREEN_7LEADECG||gCfgSystem.bInterface==SCREEN_MULTIECG){
		gValueEcg.iWaves[ECGWAVE_I] = Ch1Value;
		gValueEcg.iWaves[ECGWAVE_II] = Ch2Value;

		//此处增加LA脱落的判断，如果脱落，其他计算导联为基线 2007-12-01 16:52:22
		if(!gValueEcg.bLAOff && !gValueEcg.bLLOff){
			//III=LL-LA=II-Is
			gValueEcg.iWaves[ECGWAVE_III] = gValueEcg.iWaves[ECGWAVE_II] - gValueEcg.iWaves[ECGWAVE_I] + 128;
			//aVR = -(I+II)/2
			gValueEcg.iWaves[ECGWAVE_AVR] = 256 - (gValueEcg.iWaves[ECGWAVE_I] + gValueEcg.iWaves[ECGWAVE_II])/2;
			//aVL=(I-III)/2
			gValueEcg.iWaves[ECGWAVE_AVL] = (gValueEcg.iWaves[ECGWAVE_I] - gValueEcg.iWaves[ECGWAVE_III])/2 + 128;
			//aVF=(II+III)/2
			gValueEcg.iWaves[ECGWAVE_AVF] = (gValueEcg.iWaves[ECGWAVE_II] + gValueEcg.iWaves[ECGWAVE_III])/2;	
		}
		else{
			gValueEcg.iWaves[ECGWAVE_III] = BASELINE_ECG;
			gValueEcg.iWaves[ECGWAVE_AVR] = BASELINE_ECG;
			gValueEcg.iWaves[ECGWAVE_AVL] = BASELINE_ECG;
			gValueEcg.iWaves[ECGWAVE_AVF] = BASELINE_ECG;
		}
		gValueEcg.iWaves[ECGWAVE_V] = BASELINE_ECG;
	}else{
		//当通道有v导时，需要发送命令修改导联，否则用1,2导联计算出来
		if( gCfgEcg.bChannel1==ECGWAVE_V ){
			for(i=ECGWAVE_I; i<=ECGWAVE_V; i++){
				if(i != gCfgEcg.bChannel1)
					gValueEcg.iWaves[i] = BASELINE_ECG;
		 	}
			gValueEcg.iWaves[ECGWAVE_V] = Ch1Value;
			switch(gCfgEcg.bChannel2){
				case 0:{//I
					gValueEcg.iWaves[ECGWAVE_I] = Ch2Value;
				}break;
				case 1:{//II
					gValueEcg.iWaves[ECGWAVE_II] = Ch2Value;
				}break;
				case 2:{//III
					gValueEcg.iWaves[ECGWAVE_III] = Ch2Value;
				}break;
				case 3:{//AVR
					gValueEcg.iWaves[ECGWAVE_AVR] = Ch2Value;
				}break;
				case 4:{//AVL
					gValueEcg.iWaves[ECGWAVE_AVL] = Ch2Value;
				}break;
				case 5:{//AVF
					gValueEcg.iWaves[ECGWAVE_AVF] = Ch2Value;
				}break;
				case 6:{//V
					gValueEcg.iWaves[ECGWAVE_V] = Ch2Value;
				}break;
			}
		}else if(gCfgEcg.bChannel2==ECGWAVE_V){
			for(i=ECGWAVE_I; i<=ECGWAVE_V; i++){
				if(i != gCfgEcg.bChannel1)
					gValueEcg.iWaves[i] = BASELINE_ECG;
		 	}
			gValueEcg.iWaves[ECGWAVE_V] = Ch2Value;
			switch(gCfgEcg.bChannel1){
				case 0:{//I
					gValueEcg.iWaves[ECGWAVE_I] = Ch1Value;
				}break;
				case 1:{//II
					gValueEcg.iWaves[ECGWAVE_II] = Ch1Value;
				}break;
				case 2:{//III
					gValueEcg.iWaves[ECGWAVE_III] = Ch1Value;
				}break;
				case 3:{//AVR
					gValueEcg.iWaves[ECGWAVE_AVR] = Ch1Value;
				}break;
				case 4:{//AVL
					gValueEcg.iWaves[ECGWAVE_AVL] = Ch1Value;
				}break;
				case 5:{//AVF
					gValueEcg.iWaves[ECGWAVE_AVF] = Ch1Value;
				}break;
				case 6:{//V
					gValueEcg.iWaves[ECGWAVE_V] = Ch1Value;
				}break;
			}
		}else{
			gValueEcg.iWaves[ECGWAVE_I] = Ch1Value;
			gValueEcg.iWaves[ECGWAVE_II] = Ch2Value;

			//此处增加LA脱落的判断，如果脱落，其他计算导联为基线 2007-12-01 16:52:22
			if(!gValueEcg.bLAOff && !gValueEcg.bLLOff){
				//III=LL-LA=II-Is
				gValueEcg.iWaves[ECGWAVE_III] = gValueEcg.iWaves[ECGWAVE_II] - gValueEcg.iWaves[ECGWAVE_I] + 128;
				//aVR = -(I+II)/2
				gValueEcg.iWaves[ECGWAVE_AVR] = 256 - (gValueEcg.iWaves[ECGWAVE_I] + gValueEcg.iWaves[ECGWAVE_II])/2;
				//aVL=(I-III)/2
				gValueEcg.iWaves[ECGWAVE_AVL] = (gValueEcg.iWaves[ECGWAVE_I] - gValueEcg.iWaves[ECGWAVE_III])/2 + 128;
				//aVF=(II+III)/2
				gValueEcg.iWaves[ECGWAVE_AVF] = (gValueEcg.iWaves[ECGWAVE_II] + gValueEcg.iWaves[ECGWAVE_III])/2;	
			}
			else{
				gValueEcg.iWaves[ECGWAVE_III] = BASELINE_ECG;
				gValueEcg.iWaves[ECGWAVE_AVR] = BASELINE_ECG;
				gValueEcg.iWaves[ECGWAVE_AVL] = BASELINE_ECG;
				gValueEcg.iWaves[ECGWAVE_AVF] = BASELINE_ECG;
			}
			gValueEcg.iWaves[ECGWAVE_V] = BASELINE_ECG;
		}
	
	}
	return 0;
}
/*
	保存波形数据到文件中，调试用
*/
static int SaveWaveValuetoFile()
{
	int i;
	static int savecount=0;
	if(savecount<=SAVETIME){
	
		gSaveWave[savecount]=gValueIbp1.iWave;
	//	gSaveWave[savecount]=Ch1Value;
		
//		if(B_PRINTF) printf("%04x %d ",gSaveWave[savecount],savecount);
			if(savecount%125==0)
				if(!B_PRINTF) printf("**save %d s**\n",savecount/125);
		savecount++;
	}else{
			savecount=0;
			if(!B_PRINTF) printf("wave save---%d--------------------\n",savecount);
			FILE *fp;
			fp  = fopen("/work/data/Moni/ibp_wave.bin", "w");
			
			for(i=0;i<SAVETIME;i++){

				if(i%10==0){
					fprintf(fp,"\n0x%04x,",gSaveWave[i]);
				}else{
					fprintf(fp,"0x%04x,",gSaveWave[i]);
				}
			}
			fclose(fp);
		}
	/*
	if(gbSaveWaveValuetofile){
		if(B_PRINTF) printf("wave save---%d--------------------\n",savecount);
			FILE *fp;
			fp  = fopen("ECGChannel1.bin", "w");
			
			for(i=0;i<500*10;i++){

				if(i%16==0){
					fprintf(fp,"\n0x%04x,",gSaveWave[i]);
				}else{
					fprintf(fp,"0x%04x,",gSaveWave[i]);
				}
			}
			fclose(fp);
		gbSaveWaveValuetofile=FALSE;
	}
	*/
	return 0;
}
#define REALTOPAPER 	3
#define REALTOPAPER_ECG 	1.6
int ECG_Value_To_Printer(int EcgValue)
{
	int value=0;
	if((EcgValue/REALTOPAPER_ECG)>80){
			value=80+(EcgValue/REALTOPAPER_ECG-80)*2.5;
			if(value>160) value=160;
	}else{
			value=80-(80-EcgValue/REALTOPAPER_ECG)*2.5;
			if(value<0) value=0;
	}
	return value;
}
int Save_net_ecg_wave()
{
	static BOOL bSkip;
	
	if(gCfgEcg.bLeadType == ECGTYPE_5LEAD){
		if(gCfgEcg.bChannel1==ECGWAVE_V||gCfgEcg.bChannel2==ECGWAVE_V){
			// 打包网络数据	
			gbNetWaveBuff_Ecg[giNetWaveLength_Ecg] = BASELINE_ECG;
			gbNetWaveBuff_Ecg[giNetWaveLength_Ecg+1] = BASELINE_ECG;
			gbNetWaveBuff_Ecg[giNetWaveLength_Ecg+2] = gValueEcg.iWaves[ECGWAVE_V];
			
		}else{
			// 打包网络数据	
			gbNetWaveBuff_Ecg[giNetWaveLength_Ecg] = gValueEcg.iWaves[ECGWAVE_I];
			gbNetWaveBuff_Ecg[giNetWaveLength_Ecg+1] = gValueEcg.iWaves[ECGWAVE_II];
			gbNetWaveBuff_Ecg[giNetWaveLength_Ecg+2] = gValueEcg.iWaves[ECGWAVE_V];
		
		}
		
	}else{
		// 打包网络数据
		gbNetWaveBuff_Ecg[giNetWaveLength_Ecg] = gValueEcg.iWaves[ECGWAVE_I];
		gbNetWaveBuff_Ecg[giNetWaveLength_Ecg+1] = gValueEcg.iWaves[ECGWAVE_II];
		gbNetWaveBuff_Ecg[giNetWaveLength_Ecg+2] = gValueEcg.iWaves[ECGWAVE_III];
	}

	//由250个采样点降到200个采样点
	if((giNetWaveLength_Ecg==0)||((giNetWaveLength_Ecg/3)%5)!=0||bSkip){
		giNetWaveLength_Ecg +=3;	
		bSkip=FALSE;
	}else {
		bSkip=TRUE;
	}

//	giNetWaveLength_Ecg +=3;	
	return 0;
}

int Save_print_wave_value()
{
	static BOOL bSkip;
	int test;
	int ecg1value,ecg2value;
		//以心电采样率为基准填充实时波形
//		printf("bWave1=%d,bWave2=%d,bWave3=%d\n",gCfgPrinter.bWave1,gCfgPrinter.bWave2,gCfgPrinter.bWave3);
		//Wave1
			
		ecg1value=ECG_Value_To_Printer(gValueEcg.iWaves[gCfgEcg.bChannel1]);
		ecg2value=ECG_Value_To_Printer(gValueEcg.iWaves[gCfgEcg.bChannel2]);
		
			
		switch(gCfgPrinter.bWave1){
			case 0: gPrintRealWaves.iWave[0][giPrintRealWaveIndex] =ecg1value; break;
			case 1: gPrintRealWaves.iWave[0][giPrintRealWaveIndex] = ecg2value; break;   
			//case WID_SPO2: gPrintRealWaves.iWave[0][giPrintRealWaveIndex] = gValueSpO2.bWave/REALTOPAPER; break;
		//	case WID_RESP: gPrintRealWaves.iWave[0][giPrintRealWaveIndex] = gValueResp.iWave/REALTOPAPER; break;
		//	case WID_IBP1: gPrintRealWaves.iWave[0][giPrintRealWaveIndex] = gValueIbp1.iWave; break;
		//	case WID_IBP2: gPrintRealWaves.iWave[0][giPrintRealWaveIndex] = gValueIbp2.iWave; break;
		//	case WID_CO2: gPrintRealWaves.iWave[0][giPrintRealWaveIndex] = gValueCO2.wWave*3; break;
			default: gPrintRealWaves.iWave[0][giPrintRealWaveIndex] = 0; break;
		}
	
		//Wave2
		switch(gCfgPrinter.bWave2){
		//	case WID_ECG1: gPrintRealWaves.iWave[1][giPrintRealWaveIndex] = ecg1value; break;
		//	case WID_ECG2: gPrintRealWaves.iWave[1][giPrintRealWaveIndex] = ecg2value; break;   
			case 1: gPrintRealWaves.iWave[1][giPrintRealWaveIndex] = gValueSpO2.bWave/REALTOPAPER; break;
			case 2: gPrintRealWaves.iWave[1][giPrintRealWaveIndex] = gValueResp.iWave/REALTOPAPER; break;
			case 3: gPrintRealWaves.iWave[1][giPrintRealWaveIndex] = gValueIbp1.iWave; break;
			case 4: gPrintRealWaves.iWave[1][giPrintRealWaveIndex] = gValueIbp2.iWave; break;
			case 5: gPrintRealWaves.iWave[1][giPrintRealWaveIndex] = gValueCO2.wWave*3; break;
			default: gPrintRealWaves.iWave[1][giPrintRealWaveIndex] =0; break;
		}
		//Wave3
		switch(gCfgPrinter.bWave3){
		//	case WID_ECG1: gPrintRealWaves.iWave[2][giPrintRealWaveIndex] = ecg1value; break;
		//	case WID_ECG2: gPrintRealWaves.iWave[2][giPrintRealWaveIndex] = ecg2value; break;   
			case 1: gPrintRealWaves.iWave[2][giPrintRealWaveIndex] = gValueSpO2.bWave/REALTOPAPER; break;
			case 2: gPrintRealWaves.iWave[2][giPrintRealWaveIndex] = gValueResp.iWave/REALTOPAPER; break;
			case 3: gPrintRealWaves.iWave[2][giPrintRealWaveIndex] = gValueIbp1.iWave; break;
			case 4: gPrintRealWaves.iWave[2][giPrintRealWaveIndex] = gValueIbp2.iWave; break;
			case 5: gPrintRealWaves.iWave[2][giPrintRealWaveIndex] = gValueCO2.wWave*3; break;
			default: gPrintRealWaves.iWave[2][giPrintRealWaveIndex] = 0; break;
		}
			
		//由250个采样点降到200个采样点
		if((giPrintRealWaveIndex==0)||(giPrintRealWaveIndex%5)!=0||bSkip){
			giPrintRealWaveIndex ++;
			bSkip=FALSE;
		}else {
			bSkip=TRUE;
		}
		
	return 0;
}
/*
	发送函数
*/
static int iBio_Send(const unsigned char *datas, int datalen)
{
	int len = 0;
	int i;
	
	if(gfdSerial_iBio <=0 || datalen <=0 || datas ==NULL) return -1;
		
	sem_wait(&semSend_iBio);
	if(B_PRINTF) printf("port_send: ");
	for(i=0; i< datalen; i++){
		len = write(gfdSerial_iBio, datas+i, 1);	//实际写入的长度
		if(B_PRINTF) printf("%.2X ", *(datas+i));
		if(len != 1){
			tcflush(gfdSerial_iBio, TCOFLUSH);
			sem_post(&semSend_iBio);
			return -1;
		}
// 		usleep(5000);
	}
	
	if(B_PRINTF) printf(" End.\n");
	sem_post(&semSend_iBio);

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

	if(gfdSerial_iBio<=0 || datalen <=0 || data ==NULL) return -1;

	FD_ZERO(&fs_read);
	FD_SET(gfdSerial_iBio, &fs_read);
	tv_timeout.tv_sec = TIMEOUT_SEC(datalen, baudrate);
	tv_timeout.tv_usec = 100;//TIMEOUT_USEC;
	
	fs_sel = select(gfdSerial_iBio+1, &fs_read, NULL, NULL, &tv_timeout);
	
	if(fs_sel>0){
		readlen = read(gfdSerial_iBio, data, datalen);
		//if(B_PRINTF) printf("%s:%d RECV Data %d.\n", __FILE__, __LINE__, readlen);
		return(readlen);
	}
	else{
		return(-1);
	}

	return (readlen);
}
/*
	解析数据
*/
int iBio_Analyze(unsigned char*data,int len)
{
	int i;
	
	if(data==NULL||len<2)
		return -1;
	
	switch(data[DATAID]){
/*************************************************************************
*  01: 系统复位信息
**************************************************************************/		
		case ID_RESET:{//01: 系统复位信息
			if(B_PRINTF) printf("ID_RESET\n");
			//重新读取系统配置发送到模块板
			ResetSysCfgToModule();
			iBioCMD_RESET();
		}break;
		
/*************************************************************************
*  02: 系统状态
*	data[2]:电压检测bit7~4:保留,bit3~2:3.3v电压检测bit1~0:5v电压检测
**************************************************************************/		
		case ID_COM_STS:{// 02: 系统状态
		
			switch(data[2]&0x0c){
				case 0x00: ///3.3v电压正常
					SystemStatus.bState33V=0;
				break;
				case 0x01: ///3.3v电压太高
					SystemStatus.bState33V=1;
				break;
				case 0x10: ///3.3v电压太低
					SystemStatus.bState33V=2;
				break;
				default:
				break;
			}
			switch(data[2]&0x03){
				case 0x00: ///5v电压正常
					SystemStatus.bState5V=0;
				break;
				case 0x01: ///5v电压太高
					SystemStatus.bState5V=1;
				break;
				case 0x10: ///5v电压太低
					SystemStatus.bState5V=2;
				break;
				default:
				break;
			}
			
			IsAlm_Voltage(&SystemStatus,FALSE);
//			if(B_PRINTF) printf("SystemStatus.bState33V=%d,SystemStatus.bState5V=%d\n",SystemStatus.bState33V,SystemStatus.bState5V);
		}break;
		
/*************************************************************************
*  03: 系统自检
*	data[2]:自检结果1,bit7~5:保留，4~0:Watchdog,A/D,RAM,ROM,CPU
*	data[3]:自检结果2,bit7~6:保留，5~0:IBP,NBP,SPO2,TEMP,RESP,ECG
*	data[4]:版本号,版本号×10，例如10代表1.0，20代表2.0。
*	data[5~7]:模块标识1~3
**************************************************************************/
		case ID_SELFTEST:{// 03: 系统自检
		//	if(B_PRINTF) printf("system state!");
			//自检结果1
			SystemStatus.bSelWatchdog=(((data[2]&0x7f)+((data[1]&0x01)<<7))&0x10);//watchdog 0-自检正确1-自检错误
			SystemStatus.bSelAD=(((data[2]&0x7f)+((data[1]&0x01)<<7))&0x08);
			SystemStatus.bSelRAM=(((data[2]&0x7f)+((data[1]&0x01)<<7))&0x04);
			SystemStatus.bSelROM=(((data[2]&0x7f)+((data[1]&0x01)<<7))&0x02);
			SystemStatus.bSelCPU=(((data[2]&0x7f)+((data[1]&0x01)<<7))&0x01);
			//自检结果2
			SystemStatus.bSelIBP=(((data[3]&0x7f)+((data[1]&0x02)<<6))&0x20);
			SystemStatus.bSelNBP=(((data[3]&0x7f)+((data[1]&0x02)<<6))&0x10);
			SystemStatus.bSelSPO2=(((data[3]&0x7f)+((data[1]&0x02)<<6))&0x08);
			SystemStatus.bSelTEMP=(((data[3]&0x7f)+((data[1]&0x02)<<6))&0x04);
			SystemStatus.bSelRESP=(((data[3]&0x7f)+((data[1]&0x02)<<6))&0x02);
			SystemStatus.bSelECG=(((data[3]&0x7f)+((data[1]&0x02)<<6))&0x01);
			//版本号
			SystemStatus.wVersion=((data[4]&0x7f)+((data[1]&0x04)<<5));
			//模块标示1
			SystemStatus.wModuleSta1=((data[5]&0x7f)+((data[1]&0x08)<<4));
			//模块标示2
			SystemStatus.wModuleSta2=((data[6]&0x7f)+((data[1]&0x10)<<3));
			//模块标示3
			SystemStatus.wModuleSta3=((data[7]&0x7f)+((data[1]&0x20)<<2));
			
			IsAlm_SelfTest(&SystemStatus,FALSE);
			if(B_PRINTF) printf("ID_SELFTEST bSelWatchdog:%d,bSelAD:%d,bSelRAM:%d,bSelROM:%d,bSelCPU:%d\n",
					SystemStatus.bSelWatchdog,SystemStatus.bSelAD,SystemStatus.bSelRAM,SystemStatus.bSelROM,SystemStatus.bSelCPU);
			if(B_PRINTF) printf(" ID_SELFTEST bSelIBP:%d,bSelNBP:%d,bSelSPO2:%d,bSelTEMP:%d,bSelRESP:%d,bSelECG:%d\n",
					SystemStatus.bSelIBP,SystemStatus.bSelNBP,SystemStatus.bSelSPO2,SystemStatus.bSelTEMP,SystemStatus.bSelRESP,SystemStatus.bSelECG);
			if(B_PRINTF) printf("ID_SELFTEST wVersion:%d ,wModuleSta1:%x,wModuleSta2:%x,wModuleSta3:%x \n",
					SystemStatus.wVersion,SystemStatus.wModuleSta1,SystemStatus.wModuleSta2,SystemStatus.wModuleSta3);
		}break;

/*************************************************************************
*   04: 命令应答
*	data[2]:应答的命令包类型ID。
*	data[3]:	应答消息
**************************************************************************/	
		case ID_CMD_ACK:{// 04: 命令应答
			int CmdId;
			int CmdSta;
			CmdId=((data[2]&0x7f)+((data[1]&0x01)<<7));
			CmdSta=((data[3]&0x7f)+((data[1]&0x02)<<6));
			
			switch(CmdSta){
				case	CMDOK://命令成功
					if(B_PRINTF) printf("CMDOK 0x%x\n",CmdId);
				break;
				case	ERR_SUM://校验和错误
					if(B_PRINTF) printf("ERR_SUM 0x%x\n",CmdId);
				break;
				case	ERR_LEN://命令包长度错误
					if(B_PRINTF) printf("ERR_LEN 0x%x\n",CmdId);
				break;
				case	ERR_INVALCMD://无效命令
					if(B_PRINTF) printf("ERR_INVALCMD 0x%x\n",CmdId);
				break;
				case	ERR_CMDPARA://命令参数数据错误
					if(B_PRINTF) printf("ERR_CMDPARA 0x%x\n",CmdId);
				break;
				case	ERR_ACCEPT://命令不接受
					if(B_PRINTF) printf("ERR_ACCEPT 0x%x\n",CmdId);
				break;
			}
			
		}break;
		
/*************************************************************************
*   05: ECG波形数据
*	data[2~3]:通道1波形数据
*	data[4~5]:通道2波形数据
*	data[6~7]:通道3波形数据(根据参数板不同，有些无)
*	data[8]:ECG状态STATUS bit7~2:保留bit1:是否包含起搏脉冲bit0:是否包含心跳
**************************************************************************/	
		case ID_ECGWAVE:{// 05: ECG波形数据
			
			unsigned int Ch1Value,Ch2Value,Ch3Value;//channel1~3通道1 2 3数据
			BOOL HavePacemaker,HaveHeartBeat;
			static int DemoCountEcg=0;
			static int beepcount=0,packcount=0;
			double fGain = 0.0;
			int j;
			Ch1Value=((data[2]&0x7f)+((data[1]&0x01)<<7))<<8;
			Ch1Value+=(((data[3]&0x7f)+((data[1]&0x02)<<6)));
			
			Ch2Value=((data[4]&0x7f)+((data[1]&0x04)<<5))<<8;
			Ch2Value+=(((data[5]&0x7f)+((data[1]&0x08)<<4)));
			
		//	Ch3Value=((data[6]&0x7f)+((data[1]&0x10)<<3))<<8;
		//	Ch3Value+=(((data[7]&0x7f)+((data[1]&0x20)<<2)));
			HavePacemaker=(((data[6]&0x7f)+((data[1]&0x10)<<3)))&(0x01<<1);
			HaveHeartBeat=(((data[6]&0x7f)+((data[1]&0x10)<<3)))&0x01;

			if(HaveHeartBeat&&!gbViewDemoData) {
				beepcount=(beepcount+1)%5;
				if(beepcount==0)
					gbHaveBeep_QRS=TRUE;
			}else{
				beepcount=0;
			}
			//把波形存入文本3s 
			//SaveWaveValuetoFile();
		
	
		 	Ch1Value >>=4;
			Ch2Value >>=4;
			Ch3Value=128;
				//获得起搏检测信号
				//TODO:检测起搏信号需要滤波处理
			if(HavePacemaker&&!gbViewDemoData){
				if(packcount>100){
					gbHavePack=TRUE;
					packcount=0;
				}
				gValueEcg.bPacingpulse = 1;
			}
			else{
				gValueEcg.bPacingpulse = 0;
			}
			packcount=(packcount+1)%500;
		//获取心电波形数据,
	 	if(gCfgEcg.bLeadType == ECGTYPE_5LEAD){

				/////////////////////////////////////////////////////
			if(gSave){
				//I & II滤波
				gValueEcg.iWaves[ECGWAVE_I] = Filter_I(gValueEcg.iWaves[ECGWAVE_I]);
				gValueEcg.iWaves[ECGWAVE_II] = Filter_II(gValueEcg.iWaves[ECGWAVE_II]);
				
			if(gbViewDemoData){
					gValueEcg.iWaves[ECGWAVE_I] = DEMO_ECG_I[DemoCountEcg]>>4;
					gValueEcg.iWaves[ECGWAVE_II] = DEMO_ECG_II[DemoCountEcg]>>4;
					//III=LL-LA=II-Is
					gValueEcg.iWaves[ECGWAVE_III] = gValueEcg.iWaves[ECGWAVE_II] - gValueEcg.iWaves[ECGWAVE_I] + 128;
					//aVR = -(I+II)/2
					gValueEcg.iWaves[ECGWAVE_AVR] = 256 - (gValueEcg.iWaves[ECGWAVE_I] + gValueEcg.iWaves[ECGWAVE_II])/2;
					//aVL=(I-III)/2
					gValueEcg.iWaves[ECGWAVE_AVL] = (gValueEcg.iWaves[ECGWAVE_I] - gValueEcg.iWaves[ECGWAVE_III])/2 + 128;
					//aVF=(II+III)/2
					gValueEcg.iWaves[ECGWAVE_AVF] = (gValueEcg.iWaves[ECGWAVE_II] + gValueEcg.iWaves[ECGWAVE_III])/2;	
					
					gValueEcg.iWaves[ECGWAVE_V] = DEMO_ECG_V[DemoCountEcg]>>4;
				if(DemoCountEcg==0){
					gbHaveBeep_QRS=TRUE;
					gValueEcg.bPacingpulse = 1;
					gbHavePack=TRUE;
				
				}else if(DemoCountEcg>10)
					gValueEcg.bPacingpulse = 0;

				DemoCountEcg+=2;
				DemoCountEcg=DemoCountEcg%ECGCOUNT;
			}else{
				//设置各个导联数值
				SetECGWaveValue(Ch1Value,Ch2Value);
			}
				//存储网络数据
				Save_net_ecg_wave();
				//存储打印波形数据
			//	Save_print_wave_value();
				//添加数据到实时存储数组中(原始1x数据)
				FillOneRealSaveData();	
				
				//添加到实时ECG存储数组
			//	FillOneRealEcgData();
	
			//	if(gbSaveRealEcgData){
			//		FillOneRealEcg_Data();
			//	}

			#if 0
				//缩放波形的公式
				//y = xy - (x-1)BASE	放大 
				//y = (y - BASE)*x + BASE  //微调定标
				//软件增益,硬件固定为1x增益
				switch(gCfgEcg.bGain){
					case ECGGAIN_025X:{
						//fGain = 0.25;
						fGain = 0.5;
					}break;
					case ECGGAIN_05X:{
						fGain = 0.5;
					}break;                                                                 
					case ECGGAIN_2X:{
						fGain = 2.0;
					}break;
					default:{//ECGGAIN_1X
						fGain = 1.0;
					}break;
				}
				//加上校准系数
				fGain = fGain * ((float)gCfgEcg.iAdjust / 100.0);
				//校准波形
				for(j=ECGWAVE_I; j<=ECGWAVE_V; j++){
					gValueEcg.iWaves[j] = (float)(gValueEcg.iWaves[j])*fGain - (fGain-1.0)*(float)BASELINE_ECG;
				}
		#endif
				//----------------------------------------------------
			
				ReceivingData_Ecg1(&gValueEcg);		//添加到缓冲队列1中
				ReceivingData_Ecg2(&gValueEcg);
				
			   }
			   gSave=!gSave;
			}
			else{///3导联
	 			for(i=ECGWAVE_I; i<=ECGWAVE_V; i++){
					if(i != gCfgEcg.bChannel1)
						gValueEcg.iWaves[i] = BASELINE_ECG;
	 			}
				if(gSave){
				//I & II滤波
				gValueEcg.iWaves[ECGWAVE_I] = Filter_I(gValueEcg.iWaves[ECGWAVE_I]);
				gValueEcg.iWaves[ECGWAVE_II] = Filter_II(gValueEcg.iWaves[ECGWAVE_II]);
				
				if(gbViewDemoData){
		 				gValueEcg.iWaves[ECGWAVE_I] = DEMO_ECG_I[DemoCountEcg]>>4;
		 				gValueEcg.iWaves[ECGWAVE_II] = DEMO_ECG_II[DemoCountEcg]>>4;
						//III=LL-LA=II-Is
						gValueEcg.iWaves[ECGWAVE_III] = gValueEcg.iWaves[ECGWAVE_II] - gValueEcg.iWaves[ECGWAVE_I] + 128;
						if(DemoCountEcg==10)gbHaveBeep_QRS=TRUE;

						DemoCountEcg+=2;
						DemoCountEcg=DemoCountEcg%ECGCOUNT;
				}else{
					switch(gCfgEcg.bChannel1){
						case 0:
							gValueEcg.iWaves[ECGWAVE_I] = Ch1Value;
						break;
						case 1:
							gValueEcg.iWaves[ECGWAVE_II] = Ch1Value;
						break;
						case 2:
							gValueEcg.iWaves[ECGWAVE_III] = Ch1Value;
						break;
						default:
							gValueEcg.iWaves[ECGWAVE_I] = Ch1Value;
						break;
					}
						/////////////////////////////////////////////////////
			
					
				}
					
				//添加数据到实时存储数组中(原始1x数据)
				
				FillOneRealSaveData();	
				
		
				//存储网络数据
				Save_net_ecg_wave();
				//存储打印波形数据
				Save_print_wave_value();

				
		#if 0		
				/////////////////////////////////////////////////////
				// 打包网络数据
 				gbNetWaveBuff_Ecg[giNetWaveLength_Ecg] = gValueEcg.iWaves[ECGWAVE_I];
 				gbNetWaveBuff_Ecg[giNetWaveLength_Ecg+1] = gValueEcg.iWaves[ECGWAVE_II];
 				gbNetWaveBuff_Ecg[giNetWaveLength_Ecg+2] = gValueEcg.iWaves[ECGWAVE_III];
			
				giNetWaveLength_Ecg +=3;
		#endif	
		#if 0
				//软件增益,硬件固定为1x增益
				
				switch(gCfgEcg.bGain){
					case ECGGAIN_025X:{
						fGain = 0.25;
					}break;
					case ECGGAIN_05X:{
						fGain = 0.5;
					}break;                                                                 
					case ECGGAIN_2X:{
						fGain = 2.0;
					}break;
					default:{//ECGGAIN_1X
						fGain = 1.0;
					}break;
				}
				for(j=ECGWAVE_I; j<=ECGWAVE_V; j++){
					gValueEcg.iWaves[j] = (float)(gValueEcg.iWaves[j])*fGain - (fGain-1.0)*(float)BASELINE_ECG;
//  					gValueEcg.iWaves[j] = (gbsEcgValue.wave[i]-BASELINE_ECG)*fGain+ BASELINE_ECG;
				}
				
				#endif
				//----------------------------------------------------

				ReceivingData_Ecg1(&gValueEcg);		//添加到缓冲队列1中
				ReceivingData_Ecg2(&gValueEcg);		//添加到缓冲队列2中
				}
				gSave=!gSave;
			}
	
		}break;

/*************************************************************************
*    06: 导联连接和信号信息
*	data[2]:导联信息bit7~4:保留bit3~0:V,RA,LA,LL导联连接信息（1：导联脱落，0：连接正常）
*	data[3]:过载报警bit7~3:保留bit2~0:通道3~1过载信息：（0：正常；1：过载）
**************************************************************************/	
		case ID_LEAD_STS:{// 06: 导联连接和信号信息
			BYTE MsgLead;//导联信息
			BYTE MsgOverload;//过载报警
			MsgLead=((data[2]&0x7f)+((data[1]&0x01)<<7));
			MsgOverload=((data[3]&0x7f)+((data[1]&0x02)<<6));
			if(gbViewDemoData){
				gValueEcg.bLLOff=FALSE;
				gValueEcg.bLAOff=FALSE;
				gValueEcg.bRAOff=FALSE;
				gValueEcg.bVOff=FALSE;
				gValueResp.bOff=FALSE;
			}else{
				//导联信息
				//LL导联连接信息（1：导联脱落，0：连接正常）
				if(MsgLead&0x01){
					gValueEcg.bLLOff=TRUE;
				}else{
					gValueEcg.bLLOff=FALSE;
					gValueResp.bOff=FALSE;
				}
				 
				//LA导联连接信息（1：导联脱落，0：连接正常
				if(MsgLead&0x02)
					gValueEcg.bLAOff=TRUE;
				else
					gValueEcg.bLAOff=FALSE;
				
				//RA导联连接信息（1：导联脱落，0：连接正常）
				if(MsgLead&0x04){
					gValueEcg.bRAOff=TRUE;
					
				}else{
					gValueEcg.bRAOff=FALSE;
					gValueEcg.bConnected = TRUE;
				}
				
				if(MsgLead&0x01||MsgLead&0x04)
					gValueResp.bOff=TRUE;
				else
					gValueResp.bOff=FALSE;
			//V导联连接信息（1：导联脱落，0：连接正常)
				if(gCfgEcg.bLeadType==ECGTYPE_5LEAD){
					if(MsgLead&0x08)
						gValueEcg.bVOff=TRUE;
					else
						gValueEcg.bVOff=FALSE;
				
				
				}
			}
			IsOff_LL(&gValueEcg);
			IsOff_LA(&gValueEcg);
			IsOff_RA(&gValueEcg);
			IsOff_V(&gValueEcg);		
		
			
			//过载报警
			if(MsgOverload&0x01)//ECG通道1过载信息：（0：正常；1：过载）
				gValueEcg.bOverLoad1=TRUE;
			else
				gValueEcg.bOverLoad1=FALSE;
			if(MsgOverload&0x02)//ECG通道2过载信息：（0：正常；1：过载）
				gValueEcg.bOverLoad2=TRUE;
			else
				gValueEcg.bOverLoad2=FALSE;
			if(MsgOverload&0x04)//ECG通道3过载信息：（0：正常；1：过载）
				gValueEcg.bOverLoad3=TRUE;
			else
				gValueEcg.bOverLoad3=FALSE;

			IsAlm_ECGOverload(&gValueEcg,&gCfgEcg,FALSE);
//			if(B_PRINTF) printf(" %x MsgLead LL:%d,LA:%d,RA:%d,V:%d\n",data[2],gValueEcg.bLLOff,gValueEcg.bLAOff,gValueEcg.bRAOff,gValueEcg.bVOff);
//			if(B_PRINTF) printf(" %x bOverLoad1:%d,bOverLoad1:%d,bOverLoad1:%d\n",data[3],gValueEcg.bOverLoad1,gValueEcg.bOverLoad2,gValueEcg.bOverLoad3);

	}break;
	
/*************************************************************************
*  07: 心率
*	data[2~3]:心率：16位有符号数，有效数据范围：成人/儿童0～300BPM，新生儿0～350BPM
**************************************************************************/	
		case ID_HR:{// 07: 心率
			short EcgHR;
			EcgHR=((data[2]&0x7f)+((data[1]&0x01)<<7))<<8;
			EcgHR+=(((data[3]&0x7f)+((data[1]&0x02)<<6)));
			
			
			if(gbViewDemoData){
				gValueEcg.bHRInval=FALSE;
				gValueEcg.wHR = 80;
			}else{
				if(EcgHR>0){
					gValueEcg.wHR = EcgHR;
					gValueEcg.bHRInval=FALSE;
				}else{
					gValueEcg.wHR=0;//无效值
					gValueEcg.bHRInval=TRUE;
				}	
			}
			IsAlm_HR(&gValueEcg, &gCfgEcg, FALSE);	
//		if(B_PRINTF) printf("HR:%d  %d\n",EcgHR,gValueEcg.wHR);

		}break;
/*************************************************************************
*  0b: ST偏移
*	data[2~3]:ST1偏移,ST偏移值为16位的有符号数,所有的值都扩大100倍.
*	data[4~5]:ST2偏移,	例:125代表1.25mv,-125代表-1.25mv.-10000 代表无效值.
*	data[6~7]:ST3偏移,看参数板有无
**************************************************************************/	
		case ID_ST:{// 0b: ST偏移
			short ST1,ST2,ST3;
			
			ST1=((data[2]&0x7f)+((data[1]&0x01)<<7))<<8;
			ST1+=(((data[3]&0x7f)+((data[1]&0x02)<<6)));
			
			ST2=((data[4]&0x7f)+((data[1]&0x04)<<5))<<8;
			ST2+=(((data[5]&0x7f)+((data[1]&0x08)<<4)));
			
		//	ST3=((data[6]&0x7f)+((data[1]&0x10)<<3))<<8;
		//	ST3+=(((data[7]&0x7f)+((data[1]&0x20)<<2)));
	
		//	if(B_PRINTF) printf("``````````````ST1:%d,ST2:%d\n",ST1,ST2);
		if(gbViewDemoData){
			gValueEcg.iST1=25;
			gValueEcg.iST2=-25;
		}else{
			if(ST1!=-10000) gValueEcg.iST1=ST1;
			else	gValueEcg.iST1 = 0;
			
			if(ST2!=-10000) gValueEcg.iST2 = ST2;
			else	gValueEcg.iST2 = 0;
			
		}
		
		IsAlm_ST1(&gValueEcg, &gCfgEcg, FALSE);	
		IsAlm_ST2(&gValueEcg, &gCfgEcg, FALSE);	
		
		}break;
/*************************************************************************
*  0c: ST段模板波形值
*	data[2]:顺序号,bit7:0-通道1,1-通道2,bit6~0顺序号:0～49,分50次传送，每次5个字节
*	data[3~7]:ST模板数据1~5	,8位无符号数,250个字节的ST模板波形数据组成长度为1秒钟的心	
*				电波形，波形基线为128，第125个数据为R波位置
**************************************************************************/	
		case ID_ST_PATTERN:{// 0c: ST段模板波形值
			BYTE bCount;BYTE bChannel;
			BYTE ST[5];
			int i;
			bChannel=(((data[2]&0x7f)+((data[1]&0x01)<<7))&0x80);
			bCount=(((data[2]&0x7f)+((data[1]&0x01)<<7))&0x7F);
			ST[0]=((data[3]&0x7f)+((data[1]&0x02)<<6));
			ST[1]=((data[4]&0x7f)+((data[1]&0x04)<<5));
			ST[2]=((data[5]&0x7f)+((data[1]&0x08)<<4));
			ST[3]=((data[6]&0x7f)+((data[1]&0x10)<<3));
			ST[4]=((data[7]&0x7f)+((data[1]&0x20)<<2));
			switch(bChannel){
			case 0://通道1
				if(bCount==0){
					for(i=0;i<5;i++){
						gST1Wave[0+i]=ST[i];
					}
				}else{
					
					for(i=0;i<5;i++){
						gST1Wave[bCount*5+i]=ST[i];
					}
					if(bCount==49)
						gSTStatus=TRUE;
				
				}
			break;
			case 0x80://通道2
				if(bCount==0){
						for(i=0;i<5;i++){
						gST1Wave[0+i]=ST[i];
					}
				}else{
					for(i=0;i<5;i++){
						gST1Wave[0+i]=ST[i];
					}
				}
			break;
			}
			
			
		}break;
		
/*************************************************************************
*  10:呼吸波
*	data[2]:呼吸波形数据,呼吸波形数据：8位无符号数，有效数据范围0～256，
*			当RA/LL导联脱落时波形数据为128。该数据包每秒发送125次。	
**************************************************************************/	
		case ID_RESPWAVE:{// 10:呼吸波
			unsigned char RRWaveValue;
			static int DemoCountRresp;
			static BOOL bSkip;
			RRWaveValue=((data[2]&0x7f)+((data[1]&0x01)<<7));
			

			if(gbViewDemoData){
				gValueResp.iWave=DEMO_RESP[DemoCountRresp];
				//TODO:otherbed demo
			//	NetReceivingData_Resp(&gValueResp);
				DemoCountRresp++;
				DemoCountRresp=DemoCountRresp%RESPCOUNT;
			}else{
				if(!gValueResp.bOff)
					gValueResp.iWave=RRWaveValue;	//  0~255
				else
					gValueResp.iWave = BASELINE_RESP;
			}
		//	ReceivingData_Resp(&gValueResp);
			ReceivingData_Resp(&gValueResp);
			NewFillOneCrgData(TRUE);
			static BOOL skip=TRUE;
			gbNetWaveBuff_Resp[giNetWaveLength_Resp] = gValueResp.iWave;
			
		
			//打包网络数据
			gbNetWaveBuff_Resp[giNetWaveLength_Resp] = (gValueResp.iWave*100)/256;
			
			//由125个采样点降到50 个采样点
			if((giNetWaveLength_Resp==0)||(giNetWaveLength_Resp%5)!=0||bSkip){
				if(skip)
					giNetWaveLength_Resp +=1;
				skip=!skip;
				bSkip=FALSE;
			}else {
				bSkip=TRUE;
			}
			
			
		//	giNetWaveLength_Resp +=1;
		
			
			//if(B_PRINTF) printf("RESPwave:%d\n",gValueResp.iWave);

		}break;
		
/*************************************************************************
*   11: 呼吸率
*	data[2~3]:16位有符号数，有效数据范围：成人6～120BrPM，儿童/新生儿6～150BrPM
*			代表无效值。窒息时呼吸率为0。
**************************************************************************/	
		case ID_RR:{// 11: 呼吸率
			short RRValue;
			RRValue=(((short)(data[1]&0x01)<<7)+(data[2]&0x7f))<<8;
			RRValue+=(((data[3]&0x7f)+((data[1]&0x02)<<6)));

		if(gbViewDemoData){
				gValueResp.bOff=FALSE;
				gValueResp.bInval=FALSE;	
				gValueResp.wRR = 20;
		}else{
			if(RRValue>0){
				//此处要判断RR来源
				switch(gCfgResp.bSource){
					case RR_SRC_ECG:{
						gValueResp.wRR=(short)RRValue;
					}break; 	
					case RR_SRC_CO2:{
						gValueResp.wRR = gValueCO2.wAwRR;
					}break;
				}
				gValueResp.bInval=FALSE;				
			}else{
				gValueResp.bInval=TRUE;
				gValueResp.wRR=0;
			}
			
		}
			IsAlm_RR(&gValueResp, &gCfgResp, FALSE);


		//		if(B_PRINTF) printf("RESPwRR:%d,%d\n",RRValue,gValueResp.wRR);
	
		}break;
		
/*************************************************************************
*  12:窒息报警
*	data[2]0：没有apnea报警信息；1：有apnea报警信息。
**************************************************************************/		
		case ID_APNEA:{ // 12:窒息报警
			unsigned char RR_APNEA;
			RR_APNEA=((data[2]&0x7f)+((data[1]&0x01)<<7));
			gValueResp.bApneaAlm=RR_APNEA;
			//放在NewDrawWave.c 1秒定时器中实现，
//			IsAlm_Apnea(&gValueResp, &gCfgResp, FALSE);
			//	if(B_PRINTF) printf("gValueResp.bApnea=%d\n",gValueResp.bApnea);
		}break;
		
/*************************************************************************
* 13: CVA报警信息
*	data[2]0：没有CVA报警信息；1：有CVA报警信息。
**************************************************************************/
		case ID_CVA:{ // 13: CVA报警信息
			unsigned char RR_CVA;
			RR_CVA=((data[2]&0x7f)+((data[1]&0x01)<<7));
			gValueResp.bCVAAlm=RR_CVA;
			IsAlm_CVA(&gValueResp,&gCfgResp, FALSE);
	//		printf("RR_CVA=%d\n",RR_CVA);
		}break;
		
/*************************************************************************
* 15:体温数据
*	data[2]探头状态status BIT7~2:保留,Bit1:体温通道2，Bit0:体温通道1(0:接上1:脱离)
*	data[3~6]体温数据：16位有符号数，有效数据范围：0～500。数据扩大10倍，单位是摄氏度。
*	例如，204代表20.4℃。-100代表无效数据。该数据包每秒发送1次。
**************************************************************************/
		case ID_TEMP:{ // 15:体温数据
			BYTE TempSta;//探头状态
			short TempValue1,TempValue2;//体温数据
			TempSta=((data[2]&0x7f)+((data[1]&0x01)<<7));
			TempValue1=((data[3]&0x7f)+((data[1]&0x02)<<6))<<8;
			TempValue1+=((data[4]&0x7f)+((data[1]&0x04)<<5));	
			TempValue2=((data[5]&0x7f)+((data[1]&0x08)<<4))<<8;
			TempValue2+=((data[6]&0x7f)+((data[1]&0x10)<<3));
			
		
			if(gbViewDemoData){
				gValueTemp.bOff_T1=FALSE;
				gValueTemp.bOff_T2=FALSE;
				
				gValueTemp.wT1=385;
				gValueTemp.wT2 =365;
				IsAlm_T1(&gValueTemp, &gCfgTemp, FALSE);
				IsAlm_T2(&gValueTemp, &gCfgTemp, FALSE);
				IsAlm_TD(&gValueTemp, &gCfgTemp, FALSE);
			}else{
			
				//TEMP1体温探头连接状态
				if(TempSta&0x01!=0){
					gValueTemp.bOff_T1=TRUE;
				}else{
					gValueTemp.bOff_T1=FALSE;
					gValueTemp.bConnected_T1=TRUE;
				}
				//TEMP2体温探头连接状态
				if((TempSta&(0x01<<1))!=0){
					gValueTemp.bOff_T2=TRUE;
				}else{
					gValueTemp.bOff_T2=FALSE;
					gValueTemp.bConnected_T2=TRUE;
				}
			
				//Temp1体温数据
				if(TempValue1>0){//-100无效值
					gValueTemp.wT1 = TempValue1 + gCfgTemp.iAdjust_T1;
				}else{
					gValueTemp.wT1=0;
				}
				IsAlm_T1(&gValueTemp, &gCfgTemp, FALSE);
				//Temp2体温数据
				if(TempValue2>0){//-100无效值s
					gValueTemp.wT2 = TempValue2  + gCfgTemp.iAdjust_T2;
					
				}else{
					gValueTemp.wT2=0;
				
				}
				IsAlm_T2(&gValueTemp, &gCfgTemp, FALSE);
				IsAlm_TD(&gValueTemp, &gCfgTemp, FALSE);//如果有T2数据，才有TD报警
			}
			
	 		IsOff_T1(&gValueTemp);
			IsOff_T2(&gValueTemp);

			
			
		//	if(B_PRINTF) printf("TempValue1:%d,TempValue2:%d\n",gValueTemp.wT1,	gValueTemp.wT2);
			
		}break;
		
/*************************************************************************
*16: SPO2波形数据	
*	data[2],SPO2波形：8位无符号数，数据范围：0～255。
*	data[3],SPO2测量状态：
*			bit7:SPO2探头手指脱落标志，1表示探头手指脱落
*			bit6:脉搏标志，上位机在该标志为1时可以进行脉搏声音提示
*			bit5:搜索脉搏标志：1表示正在搜索脉搏
*			bit4:SPO2探头脱落标志，1表示探头脱落
*			bit3~0:SPO2 棒图(表示脉搏跳动情况),0～15
**************************************************************************/
		case ID_SPO2_WAVE:{ // 16: SPO2波形数据
			unsigned int SPO2WaveValue;
			BYTE FingerOFF,HavePluse,SearchPluse,SensorOFF;
			BYTE Strength;
			static int DemoCountSpo2=0;
			static BOOL bSkip;
			SPO2WaveValue=((data[2]&0x7f)+((data[1]&0x01)<<7));
			SensorOFF=((data[3]&0x7f)+((data[1]&0x02)<<6))&0x10;
			SearchPluse=((data[3]&0x7f)+((data[1]&0x02)<<6))&0x20;
			HavePluse=((data[3]&0x7f)+((data[1]&0x02)<<6))&0x40;
			FingerOFF=((data[3]&0x7f)+((data[1]&0x02)<<6))&0x80;
			Strength=((data[3]&0x7f)+((data[1]&0x02)<<6))&0x0f;
	
			//SPO2探头手指脱落标志，如果是demo则标志位设为真
				if(gbViewDemoData){//demo
					gValueSpO2.bOff=FALSE;
					gValuePulse.bOff =FALSE;
				}else{//normal
					if(FingerOFF!=0){
						gValueSpO2.bOff=TRUE;
						gValuePulse.bOff =TRUE;
						gValueSpO2.bErrCode = 0;
					}else{
						gValueSpO2.bOff=FALSE;
						gValuePulse.bOff =FALSE;
						gValueSpO2.bConnected = TRUE;
						//搜索脉搏标志：1表示正在搜索脉搏
						if(SearchPluse!=0){
							gValueSpO2.bSearchPulse=TRUE;
							gValueSpO2.bErrCode |= SPO2_SEARCHPULSE;
						}
						else{
							gValueSpO2.bSearchPulse=FALSE;
							gValueSpO2.bErrCode &= (~SPO2_SEARCHPULSE);
						}
					}
				}
			//判断脱落报警 
			IsOff_SpO2(&gValueSpO2);
			//SPO2探头脱落标志，
			if(SensorOFF!=0){
				gValueSpO2.bNoSensor=TRUE;
			}else{
				gValueSpO2.bNoSensor=FALSE;
				
			}
			
			//脉搏标志，上位机在该标志为1时可以进行脉搏声音提示
			if(HavePluse!=0&&!gbViewDemoData) gbHaveBeep_Pulse=TRUE;
				
			
			//脉搏强度状态0~3弱
			
			gValuePulse.bStrength  = Strength;
			if(gValuePulse.bStrength >15) gValuePulse.bStrength = 15;
			if(gValueSpO2.bOff) gValuePulse.bStrength = 0;	
			
//			if(B_PRINTF) printf("%d,%d,%d,%d,%d\n",gValueSpO2.bOff,gValueSpO2.bNoSensor,gValueSpO2.bSearchPulse,gbHaveBeep_Pulse,gValueSpO2.bStrength);
			//判断是否有测量错误
			isErr_SpO2(&gValueSpO2);


			//获得SpO2波形, 0~255,无信号时数值为128
		//	ReceivingData_SpO2(&gValueSpO2);
	//		ReceivingData_SpO2(&gValueSpO2);
			if(gbViewDemoData){//demo
				gValueSpO2.bWave=(DEMO_SPO2[DemoCountSpo2])*2;
				if(DemoCountSpo2==0)gbHaveBeep_Pulse=TRUE;
				//TODO:otherbed demo
			//	NetReceivingData_SpO2(&gValueSpO2);
				DemoCountSpo2++;
				DemoCountSpo2=DemoCountSpo2%SPO2COUNT;
			}else{//normal
		 		if(!gValueSpO2.bOff){
					gValueSpO2.bWave=SPO2WaveValue;//TODO:测试一下，还要加上越界的判断
				}
		 		else{
					gValueSpO2.bWave =128;
				}
			}
			
			ReceivingData_SpO2(&gValueSpO2);
			ReceivingData_SpO2(&gValueSpO2);
			
			static BOOL skip=TRUE;
			gbNetWaveBuff_SpO2[giNetWaveLength_SpO2] = gValueSpO2.bWave;
			
			//打包网络数据
			gbNetWaveBuff_SpO2[giNetWaveLength_SpO2] = (gValueSpO2.bWave*100)/256;
			//由125个采样点降到50个采样点
			if((giNetWaveLength_SpO2==0)||(giNetWaveLength_SpO2%5)!=0||bSkip){
					if(skip)
						giNetWaveLength_SpO2 +=1;
					skip=!skip;
				bSkip=FALSE;
			}else {
				bSkip=TRUE;
			}
			
		//	giNetWaveLength_SpO2 +=1;

		//	if(B_PRINTF) printf("gValueSpO2.bWave:%d\n",SPO2WaveValue);
	
		}break;
		
/*************************************************************************
*   17: SPO2数据
*	data[2]:氧饱和度信息,bit7~6:保留,bit5:氧饱和度下降标志,bit4:搜索时间太长标志
*			bit3~0:信号强度(0～8，15 代表无效值)，表示脉搏搏动的强度
*	data[3~4]:16位有符号数,有效数据范围：0～255BPM，-100 代表无效值。
*	data[5]:氧饱和度(8位有符号数),有效数据范围：0～100%，-100 代表无效值。
**************************************************************************/	
		case ID_SPO2_PR:{ // 17: SPO2数据
			BYTE Signal,Searchtime,Spo2drop;
			short PR;
			char Spo2Value;
			Signal=((data[2]&0x7f)+((data[1]&0x01)<<7))&0x0f;
			Searchtime=((data[2]&0x7f)+((data[1]&0x01)<<7))&0x10;
			Spo2drop=((data[2]&0x7f)+((data[1]&0x01)<<7))&0x20;
			PR=((data[3]&0x7f)+((data[1]&0x02)<<6))<<8;
			PR+=((data[4]&0x7f)+((data[1]&0x04)<<5));
			Spo2Value=((data[5]&0x7f)+((data[1]&0x08)<<4));

			if(Searchtime!=0)
				gValueSpO2.bSearchTimeout=TRUE;
			else
				gValueSpO2.bSearchTimeout=FALSE;
			
			if(Spo2drop!=0)
				gValueSpO2.bSpo2drop=TRUE;
			else
				gValueSpO2.bSpo2drop=FALSE;
			
			if(gbViewDemoData) {
				gValuePulse.wPR = gValueEcg.wHR;
				gValuePulse.bInvalPR=FALSE;
			}else{
				if(PR>0){
					gValuePulse.wPR=PR;
					gValuePulse.bInvalPR=FALSE;
				}else{
					gValuePulse.wPR=0;
					gValuePulse.bInvalPR=TRUE;
				}
			}
			IsAlm_PR(&gValuePulse, &gCfgPulse, FALSE);
			//信号强度
			if(Signal!=15){
					gValueSpO2.bStrength=Signal;
				if(gValueSpO2.bStrength >8) gValueSpO2.bStrength = 8;
				if(gValueSpO2.bStrength <3 && !gValueSpO2.bOff){
					gValueSpO2.bErrCode |= SPO2_WEAKSIGNAL;
				}
				else{
					gValueSpO2.bErrCode &= (~SPO2_WEAKSIGNAL);
				}
			}else{
				gValueSpO2.bStrength=0;
			}
			
			//负数最高位为1 都是大于128的数	
			if(gbViewDemoData){
					gValueSpO2.bSpO2 =98;
					gValueSpO2.bInvalSpo2=FALSE;
			}else{ 
				if(!(Spo2Value&0x01<<7)){
					
					gValueSpO2.bSpO2 = Spo2Value ;
					
					if(gValueSpO2.bSpO2>100) gValueSpO2.bSpO2 = 100;
					
					gValueSpO2.bInvalSpo2=FALSE;
				}else{
					gValueSpO2.bSpO2=0;
					gValueSpO2.bInvalSpo2=TRUE;
				}
			}
		IsAlm_SpO2(&gValueSpO2, &gCfgSpO2, FALSE);
		IsAlmLowest_SpO2(&gValueSpO2, &gCfgSpO2, FALSE);	
		/*	//alarm debug---------
			gValueSpO2.bInvalSpo2=FALSE;
			gValueSpO2.bOff=FALSE;
			gValueSpO2.bSpO2 = 85 ;
			if(gValueSpO2.bSpO2>100) gValueSpO2.bSpO2 = 100;
			IsAlm_SpO2(&gValueSpO2, &gCfgSpO2, FALSE);
			IsAlmLowest_SpO2(&gValueSpO2, &gCfgSpO2, FALSE);
		
			//alarm debug---------*/
//			if(B_PRINTF) printf("bSearchTimeout:%d,bSpo2drop:%d,Spo2Value:0x%04x,bSpO2:%d,wPR:%d\n",
//				gValueSpO2.bSearchTimeout,gValueSpO2.bSpo2drop,Spo2Value,gValueSpO2.bSpO2,gValuePulse.wPR);
		
		}break;
		
/*************************************************************************
*   18: IBP波形数据
*	data[2~3]:IBP1波形值
*	data[4~5]:IBP2波形值,这里得到的IBP 数据是原始的采样值，
*				实际的压力值需要根据校零和校准结果计算而得。
*	实际IBP 的压力计算公式＝（波形数据– 校零值）*校准值/ 2 (mmHg)
**************************************************************************/	
		case ID_IBP_WAVE:{ // 18: IBP波形数据
			short WaveIbp1,WaveIbp2;
			WaveIbp1=((data[2]&0x7f)+((data[1]&0x01)<<7))<<8;
			WaveIbp1+=((data[3]&0x7f)+((data[1]&0x02)<<6));
			WaveIbp2=((data[4]&0x7f)+((data[1]&0x04)<<5))<<8;
			WaveIbp2+=((data[5]&0x7f)+((data[1]&0x08)<<4));
			static BOOL bSkip_ibp;
			static int DemoCountibp;
			
			//TODO:
			if(gbViewDemoData){//demo
				gValueIbp1.iWave=(DEMO_IBP1[DemoCountibp]);
				gValueIbp2.iWave=(DEMO_IBP2[DemoCountibp]);

				DemoCountibp++;
				DemoCountibp=DemoCountibp%IBP2COUNT;
			}else{//normal
				gValueIbp1.iWave=WaveIbp1/2;
				gValueIbp2.iWave=WaveIbp2/2;
			}
			
			ReceivingData_Ibp1(&gValueIbp1);
			ReceivingData_Ibp1(&gValueIbp1);
			
		//	gValueIbp2.iWave=(WaveIbp2-gValueIbp2.bZeroResult)*gValueIbp2.bCalResult/2;
			
			ReceivingData_Ibp2(&gValueIbp2);
			ReceivingData_Ibp2(&gValueIbp2);
			
			static BOOL skip_ibp=TRUE;
				//打包网络数据
			gbNetWaveBuff_IBP1[giNetWaveLength_Ibp1] = gValueIbp1.iWave;
			gbNetWaveBuff_IBP2[giNetWaveLength_Ibp2] = gValueIbp2.iWave;				
			//由125个采样点降到100个采样点
			if((giNetWaveLength_Ibp1==0)||(giNetWaveLength_Ibp1%5)!=0||bSkip_ibp){
					
						giNetWaveLength_Ibp1+=1;
						giNetWaveLength_Ibp2+=1;
						
				bSkip_ibp=FALSE;
			}else {
				bSkip_ibp=TRUE;
			}
		//	if(B_PRINTF) printf("WaveIbp1:%04x,WaveIbp2:%04x \n",WaveIbp1,WaveIbp2);
		}break;
		
/*************************************************************************
*    19: IBP状态
*	data[2]:IBP导联状态bit7~2:保留,1:IBP2导联状态,0:IBP1导联状态(0：导联正常，1：导联脱落)
**************************************************************************/	
		case ID_IBP_STS:{ // 19: IBP状态
			BYTE Ibp1Lead,Ibp2Lead;
			Ibp1Lead=((data[2]&0x7f)+((data[1]&0x01)<<7))&0x01;
			Ibp2Lead=((data[2]&0x7f)+((data[1]&0x01)<<7))&0x02;
			
			//TODO:
			
			
			if(gbViewDemoData){
				gValueIbp1.bOff=FALSE;
				gValueIbp2.bOff=FALSE;
			}else{
				if(Ibp1Lead!=0){
					gValueIbp1.bOff=TRUE;
					
				}else{
					gValueIbp1.bOff=FALSE;
					gValueIbp1.bConnected = TRUE;
				}
				if(Ibp2Lead!=0){
					gValueIbp2.bOff=TRUE;
					
				}else{
					gValueIbp2.bOff=FALSE;
					gValueIbp2.bConnected = TRUE;
				}	
			}
			IsOff_IBP1(&gValueIbp1);
			IsOff_IBP2(&gValueIbp2);
			
	//		if(!B_PRINTF) printf("--ibp off--%d %d----\n",(data[2]&0x7f)+((data[1]&0x01)<<7),gValueIbp2.bOff);
		}break;
		
/*************************************************************************
*  1a: IBP压力
*	data[2]:通道号：0：通道1；1：通道2
*	data[3~4]:IBP收缩压
*	data[5~6]:IBP平均压
*	data[7~8]:IBP舒张压IBP压力范围：-50～300mmHg，-100代表无效值。
**************************************************************************/	
		case ID_IBP_DATA:{ // 1a: IBP压力
			BYTE IbpLead;//导联0-IBP1,1-IBP2
			short IbpSys,IbpDia,IbpMean;
			
			IbpLead=(data[2]&0x7f)+((data[1]&0x01)<<7);
			IbpDia=((data[3]&0x7f)+((data[1]&0x02)<<6))<<8;
			IbpDia+=((data[4]&0x7f)+((data[1]&0x04)<<5));
			IbpMean=((data[5]&0x7f)+((data[1]&0x08)<<4))<<8;
			IbpMean+=((data[6]&0x7f)+((data[1]&0x10)<<3));
			IbpSys=((data[7]&0x7f)+((data[1]&0x20)<<2))<<8;
			IbpSys+=((data[8]&0x7f)+((data[1]&0x40)<<1));

			
			if(IbpLead==0){//ibp1
				if(gbViewDemoData){
					gValueIbp1.iSys=120;
					gValueIbp1.iDia=80;
					gValueIbp1.iMean=100;
				}else{
					gValueIbp1.iSys=IbpSys;
					gValueIbp1.iDia=IbpDia;
					gValueIbp1.iMean=IbpMean;
				}
				if(B_PRINTF) printf("+11+++IbpSys,IbpDia,IbpMean,%d %d %d++++\n",IbpSys,IbpDia,IbpMean);
			}else{//ibp2
				if(gbViewDemoData){
					gValueIbp2.iSys=25;
					gValueIbp2.iDia=0;
					gValueIbp2.iMean=13;
				}else{
					gValueIbp2.iSys=IbpSys;
					gValueIbp2.iDia=IbpDia;
					gValueIbp2.iMean=IbpMean;
				}
				if(B_PRINTF) printf("22++IbpSys,IbpDia,IbpMean%d %d %d++++\n",IbpSys,IbpDia,IbpMean);
			}
		
		}break;
		
/*************************************************************************
*  1b: IBP校零和校准信息
*	data[2]:IBP通道号 字节值：0， 通道1；1， 通道2
*	data[3]:校准结果,取值:0-校零成功,1 测量压力是脉动的,
*			2 校零压力超出测量范围,3 导联脱落（校零失败） 
*			4～9 保留10 没有校零(复位后模块发现没有校零，主动报告上位机)
*	data[4]:IBP校准信息,取值:0 校准成功 ,1 测量压力是脉动的；
*			2 压力超出测量范围,3 未校零（校准失败）
*			4 导联脱落（校准失败）5～9 保留
*			10 没有校准(复位后模块发现没有校准，主动报告上位机)
**************************************************************************/	
		case ID_IBP_CAL:{ // 1b: IBP校零和校准信息
			BYTE IbpLead;
			BYTE ZeroResult,CalResult;
			
			IbpLead=(data[2]&0x7f)+((data[1]&0x01)<<7);
			CalResult=(data[3]&0x7f)+((data[1]&0x02)<<6);
			ZeroResult=(data[4]&0x7f)+((data[1]&0x04)<<5);
			if(IbpLead==0){
				//ibp1
				if(gValueIbp1.Calviewmode==1)
					gValueIbp1.bCalResult=CalResult;
			//	if(gValueIbp1.Calviewmode==2)
					gValueIbp1.bZeroResult=ZeroResult;
				if(B_PRINTF) printf("11gValueIbp1.bZeroResult=%d wave=%d\n",gValueIbp1.bZeroResult,gValueIbp1.iWave);
				if(B_PRINTF) printf("111gValueIbp1.bCalResult=%d\n",gValueIbp1.bCalResult);
				IBP1Zeroinfo();
				IBP1Calinfo();
			}else{
				if(gValueIbp2.Calviewmode==1)
					gValueIbp2.bCalResult=CalResult;
			//	if(gValueIbp2.Calviewmode==2)
					gValueIbp2.bZeroResult=ZeroResult;
				if(B_PRINTF) printf("22gValueIbp2.bZeroResult=%d\n",gValueIbp2.bZeroResult);
				if(B_PRINTF) printf("222gValueIbp2.bCalResult=%d\n",gValueIbp2.bCalResult);
				IBP2Zeroinfo();
				IBP2Calinfo();
			}
			
			//TODO:
		//	if(B_PRINTF) printf("%d,%d,%d\n",IbpLead,ZeroResult,CalResult);
		}break;
		
/*************************************************************************
*   1c: IBP校零和校准时间
*	data[2]:IBP通道号 字节值：0， 通道1；1， 通道2
*	data[3]:校准时间－日1～31
*	data[4]:校准时间－月1～12；
*	data[5]:校准时间－年2000～2255；默认：从2000年开始，用一个字节表示后三位数
**************************************************************************/	
		case ID_IBP_CAL_TIME:{ // 1c: IBP校零和校准时间
			BYTE IbpLead;
			BYTE CalDay,CalMon,CalYear;//校准时间:天月年
			IbpLead=(data[2]&0x7f)+((data[1]&0x01)<<7);
			CalDay=(data[3]&0x7f)+((data[1]&0x02)<<6);
			CalMon=(data[4]&0x7f)+((data[1]&0x04)<<5);
			CalYear=(data[5]&0x7f)+((data[1]&0x08)<<4);
			
			gValueIbp1.sCalTime.bDay=CalDay;
			gValueIbp1.sCalTime.bMonth=CalMon;
			gValueIbp1.sCalTime.wYear=CalYear+2000;
			gValueIbp2.sCalTime.bDay=CalDay;
			gValueIbp2.sCalTime.bMonth=CalMon;
			gValueIbp2.sCalTime.wYear=CalYear+2000;
			
			if(IbpLead==0){
				IBP1time();
			}else{	
				IBP2time();
			}
				//TODO:
			if(B_PRINTF) printf("%d-%d-%d\n",CalDay,CalMon,CalYear);
			
		}break;
		
/*************************************************************************
*  20: NBP实时测量数据
*	data[2~3]:袖带压力,数据范围：0～300mmHg，-100代表无效值。
*	data[4]:袖带类型错标志,0：表示袖带使用正常 ；1：表示在成人/儿童模式下，检测到新生儿袖带。
*	data[5]:测量类型,数值1~5:手动/自动/STAT/校准/漏气检测
**************************************************************************/	
		case ID_NBP_CUFPRE:{ // 20: NBP实时测量数据
			short Pressure;//袖带压力
			BYTE CuffResult,MeasureResult;
			
			Pressure=((data[2]&0x7f)+((data[1]&0x01)<<7))<<8;
			Pressure+=((data[3]&0x7f)+((data[1]&0x02)<<6));

			CuffResult=((data[4]&0x7f)+((data[1]&0x04)<<5));
			MeasureResult=((data[5]&0x7f)+((data[1]&0x08)<<4));

			if(Pressure>0){
				gValueNibp.wPressure=Pressure;
				//根据袖带压力和模式进行过压保护
				switch(gCfgNibp.bObject){
					case NIBP_OBJECT_CHILD:{
						if(gValueNibp.wPressure > NIBP_PROTECT_CHILD){
							StopiBioNibpMeasure();
							gbNibpProtect = TRUE;
						}
					}break;
					case NIBP_OBJECT_BABY:{
						if(gValueNibp.wPressure > NIBP_PROTECT_BABY){
							StopiBioNibpMeasure();
							gbNibpProtect = TRUE;
						}
					}break;
					default:{//NIBP_OBJECT_ADULT
						if(gValueNibp.wPressure > NIBP_PROTECT_ADULT){
							StopiBioNibpMeasure();
							gbNibpProtect = TRUE;
						}
					}break;
				}
				gValueNibp.bInvalPre=FALSE;
				gValueNibp.bEndType=0;
				gValueNibp.bMeasureOK = FALSE;
				gbNibpKey_Start = FALSE;
				
			}else{
				gValueNibp.bInvalPre=TRUE;
			}
			
	//0：表示袖带使用正常 ；1：表示在成人/儿童模式下，检测到新生儿袖带。
	//		if(CuffResult!=0){
			if(CuffResult!=0){
				if(B_PRINTF) printf("#######%x#################\n",gValueNibp.wErrCode &NIBP_ERR_CUFF_MISTAKE);
				StopiBioNibpMeasure();
				if((gValueNibp.wErrCode &NIBP_ERR_CUFF_MISTAKE)==0)
					gValueNibp.wErrCode |= NIBP_ERR_CUFF_MISTAKE;
				//	isErr_iBioNibp(&gValueNibp);
					
			}else{
				gValueNibp.wErrCode&=~NIBP_ERR_CUFF_MISTAKE;
			}
			switch(MeasureResult){
			case 1:///1：在手动测量方式下 
				gValueNibp.bMeasureType=NIBP_STATUS_MANUAL;
				break;
			case 2:///2：在自动测量方式下
				gValueNibp.bMeasureType=NIBP_STATUS_AUTO;
				break;
			case 3:///3：在STAT测量方式下 
				gValueNibp.bMeasureType=NIBP_STATUS_SERIES;
				break;
			case 4:///4：在校准方式下
				gValueNibp.bMeasureType=NIBP_STATUS_CALIBRATION;
				break;
			case 5:///5：在漏气检测中
				gValueNibp.bMeasureType=NIBP_STATUS_DETECTION;
				break;
			}
			GetNibpPollState();
			if(B_PRINTF) printf("ID_NBP_CUFPRE 0x20 CuffResult:%d Type=%d .wPressure=%d\n",CuffResult,gValueNibp.bMeasureType,gValueNibp.wPressure);
		}break;
		
/*************************************************************************
*    21: NBP测量结束包
*	data[2]:测量类型,数值1~5:手动/自动/STAT/校准/漏气检测10-系统错误
**************************************************************************/	
		case ID_NBP_END:{  // 21: NBP测量结束包
			BYTE EndStatus;
			EndStatus=((data[2]&0x7f)+((data[1]&0x01)<<7));
			switch(EndStatus){
			case 1:///1：手动测量方式下测量结束
				gValueNibp.bEndType=NIBP_STATUS_MANUAL;
				break;
			case 2:///2：自动测量方式下测量结束
				gValueNibp.bEndType=NIBP_STATUS_AUTO;
				break;
			case 3:///3：STAT测量结束
				gValueNibp.bEndType=NIBP_STATUS_SERIES;
				break;
			case 4:///4：在校准方式下测量结束
				gbNibpCal=FALSE;
				gValueNibp.bEndType=NIBP_STATUS_CALIBRATION;
				break;
			case 5:///5：在漏气检测中测量结束
				gbNibpLeak=FALSE;
				gValueNibp.bEndType=NIBP_STATUS_DETECTION;
				break;
			case 10:///10：系统错误，具体错误信息见NBP状态包
				gValueNibp.bEndType=NIBP_STATUS_SYSERROR;
				break;
				
			}
			gValueNibp.bMeasureType=NIBP_STATUS_STOP;
			if(gValueNibp.bEndType!=0||gValueNibp.bEndType!=NIBP_STATUS_SYSERROR){
				//标记测量完成
				gValueNibp.bMeasureOK = TRUE;
				gbNibpKey_Start = TRUE;
				gValueNibp.wPressure=0;
			
				GetNibpResult();
			}
			
			if(B_PRINTF) printf("ID_NBP_END 0x21,gValueNibp.bEndType=%d\n",gValueNibp.bEndType);
		}break;
		
/*************************************************************************
*  22: NBP测量结果1
*	data[2~3]:收缩压:
*	data[4~5]:舒张压:
*	data[6~7]:平均压:16位有符号数，数据范围0～300mmHg，-100代表无效值。
**************************************************************************/	
		case ID_NBP_RESULT1:{ // 22: NBP测量结果1
			short NbpSys,NbpDia,NbpMean;
			NbpSys=((data[2]&0x7f)+((data[1]&0x01)<<7))<<8;
			NbpSys+=((data[3]&0x7f)+((data[1]&0x02)<<6));
			NbpDia=((data[4]&0x7f)+((data[1]&0x04)<<5))<<8;
			NbpDia+=((data[5]&0x7f)+((data[1]&0x08)<<4));
			NbpMean=((data[6]&0x7f)+((data[1]&0x10)<<3))<<8;
			NbpMean+=((data[7]&0x7f)+((data[1]&0x20)<<2));
			if(NbpSys>0&&NbpDia>0&&NbpMean>0){
				gValueNibp.wSys=NbpSys;
				gValueNibp.wDia=NbpDia;
				gValueNibp.wMean=NbpMean;
			}
			if(gValueNibp.bCanceled){
				gValueNibp.wSys=0;
				gValueNibp.wDia=0;
				gValueNibp.wMean=0;
				IsAlm_NIBP(FALSE, FALSE);
			}
			
			
			if(B_PRINTF) printf("ID_NBP_RESULT1 0x22.wSys=%d,.wDia=%d,.wMean=%d\n",gValueNibp.wSys,gValueNibp.wDia,gValueNibp.wMean);
		}break;
		
/*************************************************************************
*  23: NBP测量结果2 
*	data[2~3]:脉率：16位有符号数，-100代表无效值
**************************************************************************/	
		case ID_NBP_RESULT2:{ // 23: NBP测量结果2 wpr
			short NbpPR;
			NbpPR=((data[2]&0x7f)+((data[1]&0x01)<<7))<<8;
			NbpPR+=((data[3]&0x7f)+((data[1]&0x02)<<6));

			if(NbpPR>0){
				gValueNibp.wPR=NbpPR;
			}
			if(gValueNibp.bCanceled){
				gValueNibp.wPR=0;
			}
			if(gValueNibp.bMeasureOK && !gValueNibp.bCanceled){
				if(B_PRINTF) printf("NIBP OK. %d / %d   %d. Error Code is %d\n", 
						gValueNibp.wSys, gValueNibp.wDia, gValueNibp.wMean, gValueNibp.wErrCode);

				IsAlm_NIBP(FALSE, FALSE);
				
				GetSysTime(&(gValueNibp.sTime));
				
				//存储一条NIBP趋势, 测试
				RecOneTrendRecord(TRENDNIBP);
				gValueNibp.bMeasureOK=FALSE;
			}
			if(B_PRINTF) printf("ID_NBP_RESULT2 0x23,gValueNibp.wPR=%d\n",gValueNibp.wPR);
		}break;
		
/*************************************************************************
*   24: NBP状态
*	data[2]:NBP状态bit7~6:保留,bit5~4:病人信息00:表示成人模式01:表示儿童模式02:表示新生儿模式
*			bit3~0:NBP状态,
*	data[3]:测量周期,0-手动1~14:/1/2/3/4/5/10/15/30/60/90/120/480,15-STAT
*	data[4]:测量错误,0：无错误,1：袖带过松2：漏气 3：气压错误 
*			4：弱信号,5：超范围 6：过分运动7：过压8：信号饱和
*			9：漏气检测失败 10：系统错误 11：超时 
*	data[5]:NBP剩余时间,16位无符号数，单位：秒。
**************************************************************************/	
		case ID_NBP_STS:{ // 24: NBP
			BYTE NbpStatus,NbpObject,NbpPeriod,NbpError;
			unsigned int NbpRemTime;
			NbpObject=(((data[2]&0x7f)+((data[1]&0x01)<<7))&0x30)>>4;
			NbpStatus=((data[2]&0x7f)+((data[1]&0x01)<<7))&0x0f;
			NbpPeriod=((data[3]&0x7f)+((data[1]&0x02)<<6));
			NbpError=((data[4]&0x7f)+((data[1]&0x04)<<5));
			NbpRemTime=((data[5]&0x7f)+((data[1]&0x08)<<4))<<8;
			NbpRemTime+=((data[6]&0x7f)+((data[1]&0x10)<<3));
			
			//病人信息
			switch(NbpObject){
				case 0://成人模式
					gValueNibp.bObject=NIBP_OBJECT_ADULT;
					break;
				case 1://儿童模式
					gValueNibp.bObject=NIBP_OBJECT_CHILD;
					break;
				case 2://新生儿模式
					gValueNibp.bObject=NIBP_OBJECT_BABY;
					break;
			}
			//NBP状态
			switch(NbpStatus){
				case 0://NBP复位完成
					gValueNibp.bStatus=NIBP_STATUS_STOP;
					break;
				case 1://手动测量中
					gValueNibp.bStatus=NIBP_STATUS_MANUAL;
					break;
				case 2:// 自动测量中
					gValueNibp.bStatus=NIBP_STATUS_AUTO;
					break;
				case 3://STAT测量方式中
					gValueNibp.bStatus=NIBP_STATUS_SERIES;
					break;
				case 4:// 校准中
					gValueNibp.bStatus=NIBP_STATUS_CALIBRATION;
					break;
				case 5:// 漏气检测中
					gValueNibp.bStatus=NIBP_STATUS_DETECTION;
					break;
				case 6://NBP复位
					gValueNibp.bStatus=NIBP_STATUS_RESET;
					break;
				case 10://系统出错
					gValueNibp.bStatus=NIBP_STATUS_SYSERROR;
					break;
				default:
					gValueNibp.bStatus=NIBP_STATUS_STOP;
					break;
			}
			//NBP测量周期
			switch(NbpPeriod){
				case 0://在手动测量方式下
					gValueNibp.bPeriod=0;
					break;
				case 1://在自动测量方式下，对应周期为1分钟
					gValueNibp.bPeriod=1;
					break;
				case 2:// 周期为2分钟
					gValueNibp.bPeriod=2;
					break;
				case 3://周期为3分钟
					gValueNibp.bPeriod=3;
					break;
				case 4:// 周期为4分钟
					gValueNibp.bPeriod=4;
					break;
				case 5:// 周期为5分钟
					gValueNibp.bPeriod=5;
					break;
				case 6://周期为10分钟
					gValueNibp.bPeriod=10;
					break;
				case 7://周期为15分钟
					gValueNibp.bPeriod=15;
					break;
				case 8://周期为30分钟
					gValueNibp.bPeriod=30;
					break;
				case 9://周期为1小时60min
					gValueNibp.bPeriod=60;
					break;
				case 10:// 周期为1.5小时90smin
					gValueNibp.bPeriod=90;
					break;
				case 11://周期为2小时120min
					gValueNibp.bPeriod=120;
					break;
				case 12:// 周期为3小时180min
					gValueNibp.bPeriod=180;
					break;
				case 13://周期为4小时240min
					gValueNibp.bPeriod=240;
					break;
				case 14://周期为8小时480min
					gValueNibp.bPeriod=480;
					break;
				case 15://在STAT测量方式下
					gValueNibp.bPeriod=0;
					break;
			}
		//NBP测量错误：
			switch(NbpError){
				case 0://无错误
					if(!(gValueNibp.wErrCode&NIBP_ERR_CUFF_MISTAKE)){
						gValueNibp.wErrCode&=~0xffff;
					}
		//			if(B_PRINTF) printf("*******%x*********************************\n",gValueNibp.wErrCode);
					break;
				case 1:////袖带过松 
					gValueNibp.wErrCode=0;
					gValueNibp.wErrCode|=NIBP_ERR_CUFF_TOOLAX;
					break;
				case 2://漏气 
					gValueNibp.wErrCode=0;
					gValueNibp.wErrCode|=NIBP_ERR_CUFF_LEAK;
					break;
				case 3://气压错误 
					gValueNibp.wErrCode=0;
					gValueNibp.wErrCode|=NIBP_ERR_PRESSURE;
					break;
				case 4://微弱信号
					gValueNibp.wErrCode=0;
					gValueNibp.wErrCode|=NIBP_ERR_WEAK_SIGNAL;
					break;
				case 5://超范围
					gValueNibp.wErrCode=0;
					gValueNibp.wErrCode|=NIBP_ERR_OVER_RANGE;
					break;
				case 6://过分运动
					gValueNibp.wErrCode=0;
					gValueNibp.wErrCode|=NIBP_ERR_EXCESSIVE_MOTION;
					break;
				case 7://过压 
					gValueNibp.wErrCode=0;
					gValueNibp.wErrCode|=NIBP_ERR_OVERPRESSURE;
					break;
				case 8://信号饱和 
					gValueNibp.wErrCode=0;
					gValueNibp.wErrCode|=NIBP_ERR_SIGNAL_SATURATION;
					break;
				case 9://漏气检测失败
					gValueNibp.wErrCode=0;
					gbNibpLeak=FALSE;
					gValueNibp.wErrCode|=NIBP_ERR_CHECKLEAK;
					break;
				case 10://系统错误 
					gValueNibp.wErrCode=0;
					gValueNibp.wErrCode|=NIBP_ERR_MODULE_FAULT;
					break;
				case 11://超时 
					gValueNibp.wErrCode=0;
					gValueNibp.wErrCode|=NIBP_ERR_TIMEOUT;
					break;
				default:
					gValueNibp.wErrCode&=~0xffff;
					break;
			}
			if(gValueNibp.wErrCode!=0){
				gValueNibp.bMeasureOK = FALSE;
				//判断是否已经启动周期测量
			//	StopNibpAutoMeasure_Error();
			}
			isErr_iBioNibp(&gValueNibp);
			if(B_PRINTF) printf("ID_NBP_STS0x24,NbpObject=%d,NbpStatus=%d,NbpPeriod=%d,NbpError=%04x\n",NbpObject,NbpStatus,NbpPeriod,gValueNibp.wErrCode);
		}
	}

	//以ECG的采样为标准，收到200包的时候发送数据报文至中央机和广播报文  200Hz
		
			SendNetData();

		//实时打印
			Real_Wave_Print();
	
	return 0;
}

/********************************************************************
功能描述: 将命令发送到下位机。
参数: pack----要发送的命令缓冲区指针，内容为尚未组织好的命令：ID+DATA。
返回值: 无。
********************************************************************/
void SendCommand (unsigned char* pack)
{
	unsigned char head;
	int i;
	int len;
	unsigned char checksum;
	len =gPackLength [*pack];

	/* 如果包长度小于2，直接返回*/
	if (len < 2 )
		return;
	
	/* 开始 计算校验和*/
	checksum = *(pack);
	if (len > 2 ){
		head = 0;
		for (i = len-2; i > 1; i --) {
			head <<= 1;
			/* 最高位置1 */
			*(pack+i) = *(pack+i-1) | 0x80;
			/* 计算校验和*/
			checksum += *(pack+i);
			/* 计算数据头*/
			head |= (*(pack+i-1)&0x80) >> 7;
		}
		/* 数据头最高位置1 */
		*(pack+1) = head | 0x80;
		/* 校验和加上数据头*/
		checksum += *(pack+1);
	}
	/* 保存校验和*/
	*(pack+len-1) = checksum | 0x80;
	/* TODO: 用户可以向下位机串口发送命令*/
	/* uart.write(pack, len); */
	
	iBio_Send(pack,len);
}


/******************************************************************
* 功能描述：从缓冲区中恢复数据，解包后的数据仍然放在原缓冲区中，
*解包后数据头被删除。
*
*参数：
* 	pack 缓冲区指针，该指针必须指向包类型开始的数据缓冲区；
* 	len 所要解包的数据包长度(包括包类型和校验和)；
*
*返回值：
*	 0: 解包OK
* 	-1: 包长度错误
* 	-2: 校验和错误
*******************************************************************/
int iBio_UnPackData(unsigned char * pack, int len)
{
	unsigned char head;
	int i;
	unsigned char checksum;
	
	if ( len < 2 )
		return -1;
	
	/* 开始计算校验和*/
	checksum = *pack;
	for (i = 1; i < len - 1; i ++) 
		 checksum+=(*(pack+i)&0x7f);
	
	if ( (checksum&0x7f ) != (*(pack+len-1) &0x7f ))
	{
		//if(B_PRINTF) printf("check sum ERR pack[0]=%d#$#$#$#$#$$#$#$#$$#$#$\n",pack[0]);
		return -2;
	}
	
#if 0
	if(*pack==0x0a) {
		for(i=0;i<len;i++)
			if(B_PRINTF) printf("%02x  ",pack [i]);
		//if(B_PRINTF) printf("\n");
		if(B_PRINTF) printf("\n check sum=%x %x\n",checksum ,(*(pack+len-1)));
	}
#endif
	//解析数据
	iBio_Analyze(pack,len);
	
	return 0;
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
int iBio_UnBind (unsigned char *data, int length)
{
	static unsigned char sCurrPacket[10];
	static int sPacketIdFound =0;/*接受到包类型ID 的标志，1表示接收到*/
	static int sCurrPacketLen;/*当前接受到的包长度*/
	unsigned char current_data;
	int result =0;
	int i,res=0;
	/* 读取缓冲区的数据*/
	while (length > 0)
	{
		current_data = * data;
		/* 接收到包类型字节*/
		if ( sPacketIdFound ){
		/* 当前数据是数据字节，有效的数据字节必须大于等于0x80 */
			if (current_data >= 0x80 ){
				// 保存数据
				sCurrPacket [sCurrPacketLen++] = current_data;
					//if(B_PRINTF) printf("sCurrPacketLen=%d,gPackLength [sCurrPacket [0]]=%d\n",sCurrPacketLen,gPackLength [sCurrPacket [0]]);
				/* 接收到整个包的数据，则开始解包*/
				if (sCurrPacketLen == gPackLength [sCurrPacket [0]]) {
					//检查校验和以及分析数据
					res=iBio_UnPackData(sCurrPacket, sCurrPacketLen);
				//	if(res<0) if(B_PRINTF)printf("%s%d check sum error res=%d\n",__FILE__,__LINE__,res);
					
					sPacketIdFound = 0;
				}
				
			}else{
				/*当前数据如果小于0x80, 则必定是错误*/
				sPacketIdFound = 0;
				result = -1;
			}
		}else{
			/* 检查当前字节是否正确的包类型ID */
			if (current_data<0x80&&gPackLength [current_data] > 0 )
			{
				sCurrPacketLen = 1;
				sCurrPacket[0] = current_data;
				sPacketIdFound = 1;
				
			}else {
				/* 当前数据不是正确的包类型*/
				result = -1;
			//	if(B_PRINTF) printf("[3]#########[%x]\n",current_data);
			}
		
		}
		
				
		/* 指向下一个串口数据*/
		data ++;
		/* 数据长度减1 */
		length --;
	}
	return (result);
}

/*
	模块复位信息应答
*/
int iBioCMD_RESET()
{
	unsigned char packet[10];
	packet[0]=ID_RESET;
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD RESET\n");
	return 0;
}
/*
	读取自检结果
*/
int iBioCMD_GET_POST_RESULT()
{
	unsigned char packet[10];
	packet[0]=CMD_GET_POST_RESULT;
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_GET_POST_RESULT\n");
	return 0;
}
/*
	病人类型设置
*/
int iBioCMD_PAT_TYPE(BYTE PatType)
{
	unsigned char packet[10];
	packet[0]=CMD_PAT_TYPE;
	switch(PatType){
		case 0:{//成人
			packet[1]=0;
		}break;
		case 1:{//儿童
			packet[1]=1;
		}break;
		case 2:{//新生儿
			packet[1]=2;
		}break;
		default:{//成人
			packet[1]=0;
		}break;
	}
	if(B_PRINTF) printf("Send CMD_PAT_TYPEpacket[1]=%d\n",packet[1]);
	SendCommand(packet);
	//if(B_PRINTF) printf("Send CMD_PAT_TYPE\n");
	return 0;
}
/*
	导联设置3、5
*/
int iBioCMD_LEADSYSTEM(BYTE LeadSet)
{
	unsigned char packet[10];
	packet[0]=CMD_LEADSYSTEM;
	switch(LeadSet){
		case 0:{///5导联
			packet[1]=1;
		}break;
		case 1:{///3导联
			packet[1]=0;
		}break;
		default:{///5导联
			packet[1]=1;
		}break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("CMD_LEADSYSTEM\n");
	return 0;
}
/*
	导联方式设置
*/
int iBioCMD_LEADTYPE(BYTE Channel,BYTE Lead)
{
	unsigned char packet[10];
	packet[0]=CMD_LEADTYPE;
	switch(Channel){
		case 0x00:{//通道1
			packet[1]=Channel;
		}break;
		case 0x10:{//通道2
			packet[1]=Channel;
		}break;
		default:{//通道1
			packet[1]=0x00;
		}break;
	}
	switch(Lead){
		case 0x00:{//I导联
			packet[1]|=0x01;
		}break;
		case 0x01:{//II导联
			packet[1]|=0x02;
		}break;
		case 0x02:{//III导联
			packet[1]|=0x03;
		}break;
		case 0x03:{//AVL导联
			packet[1]|=0x04;
		}break;
		case 0x04:{//AVR导联
			packet[1]|=0x05;
		}break;
		case 0x05:{//AVF导联
			packet[1]|=0x06;
		}break;
		case 0x06:{//V导联
			packet[1]|=0x07;
		}break;
		default:{//II导联
			packet[1]|=0x02;
		}break;
	}
	
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_LEADTYPE\n");
	return 0;
}
/*
	心电滤波方式设置
*/
int iBioCMD_FILTER_MODE(BYTE FilterMode)
{
	unsigned char packet[10];
	packet[0]=CMD_FILTER_MODE;
	switch(FilterMode){
		case 0x00:{//诊断
			packet[1]=FilterMode;
		}break;
		case 0x01:{//监护
			packet[1]=FilterMode;
		}break;
		case 0x02:{//手术
			packet[1]=FilterMode;
		}break;
		default:{//诊断
			packet[1]=0x00;
		}break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_FILTER_MODE\n");
	return 0;
}

/*
	心电增益设置
*/
int iBioCMD_ECG_GAIN(BYTE Channel,BYTE EcgGain)
{
	unsigned char packet[10];
	packet[0]=CMD_ECG_GAIN;
	switch(Channel){
		case 0x00:{//通道1
			packet[1]=Channel;
		}break;
		case 0x10:{//通道2
			packet[1]=Channel;
		}break;
		case 0x20:{//通道3
			packet[1]=Channel;
		}break;
		default:{//通道1
			packet[1]=0x00;
		}break;
	}
	switch(EcgGain){
		case 0x00:{//x0.25
			packet[1]|=0x00;
		}break;
		case 0x01:{//x0.5
			packet[1]|=0x01;
		}break;
		case 0x02:{///x1
			packet[1]|=0x02;
		}break;
		case 0x03:{//x2
			packet[1]|=0x03;
		}break;
		default:{//x1
			packet[1]|=0x02;
		}break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_ECG_GAIN\n");
	return 0;
}

/*
	心电校准
*/
int iBioCMD_ECG_CAL(BYTE CalMode)
{
	unsigned char packet[10];
	packet[0]=CMD_ECG_CAL;
	switch(CalMode){
		case 0x00:{//关
			packet[1]=CalMode;
		}break;
		case 0x01:{//开
			packet[1]=CalMode;
		}break;
		default:{//关
			packet[1]=0x00;
		}break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_ECG_CAL\n");
	return 0;
}
/*
	工频干扰抑制开关
*/
int iBioCMD_ECG_TRAP(BYTE TrapMode)
{
	unsigned char packet[10];
	packet[0]=CMD_ECG_TRAP;
	switch(TrapMode){
		case 0x00:{//关
			packet[1]=TrapMode;
		}break;
		case 0x01:{//开
			packet[1]=TrapMode;
		}break;
		default:{//关
			packet[1]=0x00;
		}break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_ECG_TRAP\n");
	return 0;
}
/*
	起搏分析开关
*/
int iBioCMD_ID_PACE(BYTE Pace)
{
	unsigned char packet[10];
	packet[0]=CMD_ID_PACE;
	switch(Pace){
		case 0x00:{//关
			packet[1]=0;
		}break;
		case 0x01:{//开mode1
			packet[1]=1;
		}break;
		case 0x02:{//开mode2
			packet[1]=1;
		}break;
		default:{//关
			packet[1]=0;
		}break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_ID_PACE\n");
	return 0;
}
/*
	ST测量的ISO、ST点
*/
int iBioCMD_ST_ISO(int ISO,int ST)
{
	unsigned char packet[10];
	packet[0]=CMD_ST_ISO;
	packet[1]=(ISO>>8)&0xff; //ISO1
	packet[2]=ISO&0xff;	//ISO2
	packet[3]=(ST>>8)&0xff;//ST1
	packet[4]=ST&0xff;	//ST2
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_ST_ISO\n");
	return 0;
}

/*
	心率计算通道
*/
int iBioCMD_ECG_CHANNEL(BYTE EcgChannel)
{
	unsigned char packet[10];
	packet[0]=CMD_ECG_CHANNEL;
	switch(EcgChannel){
		case 0x00:{//通道1
			packet[1]=EcgChannel;
		}break;
		case 0x01:{//通道2
			packet[1]=EcgChannel;
		}break;
		case 0x02:{//通道3
			packet[1]=EcgChannel;
		}break;
		case 0x03:{//自动选择
			packet[1]=EcgChannel;
		}break;
		default:{//通道1
			packet[1]=0x00;
		}break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_ECG_CHANNEL\n");
	return 0;
}

/*
	心率重新计算
*/
int iBioCMD_ECG_LEARN()
{
	unsigned char packet[10];
	packet[0]=CMD_ECG_LEARN;
	
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_ECG_LEARN\n");
	return 0;
}

/*
	呼吸增益设置
*/
int iBioCMD_RESP_GAIN(BYTE RespGain)
{
	unsigned char packet[10];
	packet[0]=CMD_RESP_GAIN;
	switch(RespGain){
		case 0x00:{//x0.25
			packet[1]=RespGain;
		}break;
		case 0x01:{//x0.5
			packet[1]=RespGain;
		}break;
		case 0x02:{//x1
			packet[1]=RespGain;
		}break;
		case 0x03:{//x2
			packet[1]=RespGain;
		}break;
		case 0x04:{//x4
			packet[1]=RespGain;
		}break;
		default:{//x1
			packet[1]=0x02;
		}break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_RESP_GAIN\n");
	return 0;
}

/*
	呼吸I/II 导联设置
*/
int iBioCMD_RESP_TYPE(BYTE LeadSet)
{
	unsigned char packet[10];
	packet[0]=CMD_RESP_TYPE;
	switch(LeadSet){
		case 0x00:{//I导联
			packet[1]=0x00;
		}break;
		case 0x01:{//II导联
			packet[1]=0x01;
		}break;
		default:{//II导联
			packet[1]=0x01;
		}break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_RESP_TYPE\n");
	return 0;
}
/*
	窒息报警时间设置
*/
int iBioCMD_APNEA_ALM(BYTE Apnea)
{
	unsigned char packet[10];
	packet[0]=CMD_APNEA_ALM;
	switch(Apnea){
		case 0x00:{//不报警
			packet[1]=Apnea;
		}break;
		case 0x01:{//10s
			packet[1]=Apnea;
		}break;
		case 0x02:{//15s
			packet[1]=Apnea;
		}break;
		case 0x03:{//20s
			packet[1]=Apnea;
		}break;
		case 0x04:{//25s
			packet[1]=Apnea;
		}break;
		case 0x05:{//30s
			packet[1]=Apnea;
		}break;
		case 0x06:{//35s
			packet[1]=Apnea;
		}break;
		case 0x07:{//40s
			packet[1]=Apnea;
		}break;
		default:{//20s
			packet[1]=0x03;
		}break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_APNEA_ALM\n");
	return 0;
}
/*
	体温探头类型设置
*/
int iBioCMD_TEMP_TYPE(BYTE TempType)
{
	unsigned char packet[10];
	packet[0]=CMD_TEMP_TYPE;
	switch(TempType){
		case 0x00:{//YSI探头
			packet[1]=TempType;
		}break;
		case 0x01:{//CY-F探头
			packet[1]=TempType;
		}break;
		default:{//YSI探头
			packet[1]=0x00;
		}break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_TEMP_TYPE\n");
	return 0;
}
/*
	SPO2设置饱和度计算灵敏度
*/
int iBioCMD_SPO2(BYTE Spo2Sense)
{
	unsigned char packet[10];
	packet[0]=CMD_SPO2;
	switch(Spo2Sense){
		case 0x01:{//灵敏度高
			packet[1]=Spo2Sense;
		}break;
		case 0x02:{//灵敏度中
			packet[1]=Spo2Sense;
		}break;
		case 0x03:{//灵敏度低
			packet[1]=Spo2Sense;
		}break;
		default:{//灵敏度中
			packet[1]=0x02;
		}break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_SPO2\n");
	return 0;
}
/*
	NBP启动手动/自动测量
*/
int iBioCMD_NBP_START()
{
	unsigned char packet[10];
	packet[0]=CMD_NBP_START;

	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_NBP_START\n");
	return 0;
}
/*
	NBP中止测量
*/
int iBioCMD_NBP_END()
{
	unsigned char packet[10];
	packet[0]=CMD_NBP_END;

	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_NBP_END\n");
	return 0;
}
/*
	NBP测量周期设置
*/
int iBioCMD_NBP_PERIOD(BYTE NBPMode)
{
	unsigned char packet[10];
	packet[0]=CMD_NBP_PERIOD;
	switch(NBPMode){
		case 0://手动测量
		case 1:///1 分钟
		case 2:///2 分钟
		case 3:///3 分钟
		case 4:///4 分钟
		case 5:///5 分钟
		case 6:///10 分钟
		case 7:///15 分钟
		case 8:///30 分钟
		case 9:///60 分钟
		case 10:///90 分钟
		case 11:///120 分钟
		case 12:///180 分钟
		case 13:///240分钟
		case 14:///480 分钟
			packet[1]=NBPMode;
			break;
		default:{//手动测量
			packet[1]=0x00;
		}break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_NBP_PERIOD\n");
	return 0;
}
/*
	NBP校准
*/
int iBioCMD_NBP_CAL()
{
	unsigned char packet[10];
	packet[0]=CMD_NBP_CAL;
	
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_NBP_CAL\n");
	return 0;
}
/*
	NBP复位
*/
int iBioCMD_NBP_RESET()
{
	unsigned char packet[10];
	packet[0]=CMD_NBP_RESET;
	
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_NBP_RESET\n");
	return 0;
}
/*
	NBP漏气检测
*/
int iBioCMD_NBP_PNEUMATIC()
{
	unsigned char packet[10];
	packet[0]=CMD_NBP_PNEUMATIC;
	
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_NBP_PNEUMATIC\n");
	return 0;
}
/*
	NBP查询状态
*/
int iBioCMD_NBP_POLL_STATE()
{
	unsigned char packet[10];
	packet[0]=CMD_NBP_POLL_STATE;
	
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_NBP_POLL_STATE\n");
	return 0;
}
/*
	NBP设置初次充气压力
*/
int iBioCMD_NBP_PRE_PRESSURE(BYTE PatientType,unsigned int PresetPressure)
{
	unsigned char packet[10];
	packet[0]=CMD_NBP_PRE_PRESSURE;
	if(B_PRINTF) printf("PatientType=%d PresetPressure=%d \n",PatientType,PresetPressure);
	switch(PatientType){
		case 0://成人
		case 1://儿童
		case 2://新生儿
			packet[1]=PatientType;
			break;
		default:{//成人
			packet[1]=0;
		}break;
		if(B_PRINTF) printf("`````packet[1]=%d \n",packet[1]);

	}
	switch(PresetPressure){
		case 60://60 mmHg
		case 70://70 mmHg
		case 80://80 mmHg
		case 100://100 mmHg
		case 120://120 mmHg
		case 140://140 mmHg
		case 150://150 mmHg
		case 160://160 mmHg
		case 180://180 mmHg
		case 200://200 mmHg
		case 220://220 mmHg
		case 240://240 mmHg
			packet[2]=PresetPressure;
			break;
		default:{//160 mmHg
			packet[2]=160;
		}break;
		if(B_PRINTF) printf("`````packet[2]=%d \n",packet[2]);
	}
	
	if(B_PRINTF) printf("`````packet[1]=%d \n",packet[1]);
	if(B_PRINTF) printf("`````packet[2]=%d \n",packet[2]);
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_NBP_PRE_PRESSURE%d %d\n",packet[1],packet[2]);
	return 0;
}

/*
	NBP启动STAT测量
*/
int iBioCMD_NBP_CONTINUE()
{
	unsigned char packet[10];
	packet[0]=CMD_NBP_CONTINUE;
	
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_NBP_CONTINUE\n");
	return 0;
}

/*
	NBP查询测量结果
*/
int iBioCMD_NBP_GET_RESULT()
{
	unsigned char packet[10];
	packet[0]=CMD_NBP_GET_RESULT;
	
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_NBP_GET_RESULT\n");
	return 0;
}
/*
	IBP通道压力名称
*/
int iBioCMD_IBP_CHANNEL(BYTE Channel1,BYTE Channel2,BYTE Name1,BYTE Name2)
{
	unsigned char packet[10];
	packet[0]=CMD_IBP_CHANNEL;
	switch(Channel1){
		case 0://测量计算1(map)
		case 1://测量计算3(sys,map,dia)
			packet[1]|=3<<Channel1;
		break;
		default://测量计算3(sys,map,dia)
			packet[1]|=3<<1;
		break;
	}
	switch(Name1){
		case 0://ART
		case 1://PA
		case 2://CVP
		case 3://LAP
		case 4://RAP
		case 5://ICP
		case 6://P1
		case 7://P2
			packet[1]|=Name1;
		break;
		default:
			packet[1]|=0x00;
		break;
	}
	switch(Channel2){
		case 0://测量计算1(map)
		case 1://测量计算3(sys,map,dia)
			packet[1]|=7<<Channel2;
		break;
		default://测量计算3(sys,map,dia)
			packet[1]|=7<<1;
		break;
	}
	switch(Name2){
		case 0://ART
		case 1://PA
		case 2://CVP
		case 3://LAP
		case 4://RAP
		case 5://ICP
		case 6://P1
		case 7://P2
			packet[1]|=4<<Name2;
		break;
		default:
			packet[1]|=4<<0x02;
		break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_IBP_CHANNEL\n");
	return 0;
}
/*
	IBP校零
*/
int iBioCMD_IBP_ZERO(BYTE IbpChannel)
{
	unsigned char packet[10];
	packet[0]=CMD_IBP_ZERO;
	switch(IbpChannel){
		case 0://通道1
		case 1://通道2
			packet[1]=IbpChannel;
		break;
		default:
			packet[1]=0;
		break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_IBP_ZERO\n");
	return 0;
}
/*
	IBP通道1校准
*/
int iBioCMD_IBP_CAL1(BYTE Pressure,S_TIME *bstime)
{
	unsigned char packet[10];
	packet[0]=CMD_IBP_CAL1;
	packet[1]=Pressure;
	packet[2]=bstime->bDay;
	packet[3]=bstime->bMonth;
	packet[4]=bstime->wYear-2000;
	//printf("%d,%d,%d,%d\n",packet[1],packet[2],packet[3],packet[4]);

	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_IBP_CAL1\n");
	return 0;
}
/*
	IBP通道2校准
*/
int iBioCMD_IBP_CAL2(BYTE Pressure,S_TIME *bstime)
{
	unsigned char packet[10];
	packet[0]=CMD_IBP_CAL2;
	packet[1]=Pressure;
	packet[2]=bstime->bDay;
	packet[3]=bstime->bMonth;
	packet[4]=bstime->wYear;
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_IBP_CAL2\n");
	return 0;
}
/*
	IBP滤波模式设置
*/
int iBioCMD_IBP_FILTER(BYTE IbpFilterMode)
{
	unsigned char packet[10];
	packet[0]=CMD_IBP_FILTER;
	switch(IbpFilterMode){
		case 0://平滑
		case 1://正常
			packet[1]=IbpFilterMode;
		break;
		default:
			packet[1]=1;
		break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_IBP_FILTER\n");
	return 0;
}
/*
	IBP查询状态
*/
int iBioCMD_IBP_POLL_STATE(BYTE IbpChannel,BYTE IbpId)
{
	unsigned char packet[10];
	packet[0]=CMD_IBP_POLL_STATE;
	switch(IbpChannel){
		case 0://通道1
		case 1://通道2
			packet[1]=IbpChannel;
		break;
		default://通道1
			packet[1]=0;
		break;
	}
	switch(IbpId){
		case 0://IBP校零和校准信息
			packet[2]=0x1b;
		break;
		case 1://IBP校准时间
			packet[2]=0x1c;
		break;
		default://通道1
			packet[2]=0x1b;
		break;
	}
	SendCommand(packet);
	if(B_PRINTF) printf("Send CMD_IBP_POLL_STATE\n");
	return 0;
}
/*
	接收到模块复位信息，重新读取系统配置发送至模块
*/
int ResetSysCfgToModule()
{
	printf("*******reset**************\n");
	
	//病人信息设置object
	iBioCMD_PAT_TYPE(gCfgPatient.bObject);

	//导联设置3/5导联
	iBioCMD_LEADSYSTEM(gCfgEcg.bLeadType);
	
	//呼吸I/II导联设置
	iBioCMD_RESP_TYPE(gCfgResp.bLeadType);//new
	
	//导联方式选择
	ChangeECGLeadChannel();
	//滤波方式选择
	iBioCMD_FILTER_MODE(gCfgEcg.bMode);

	//心电增益选择
	iBioCMD_ECG_GAIN(gCfgEcg.bChannel1,gCfgEcg.bGain);
	//iBioCMD_ECG_GAIN(gCfgEcg.bChannel2,gCfgEcg.bGain);
	///1mv校准信号设置
	iBioCMD_ECG_CAL(gCfgEcg.b1mvCal);//new
		
	//工频抑制设置
	//iBioCMD_ECG_TRAP(gCfgEcg.bFilterAC);
	iBioCMD_ECG_TRAP(1);
	//起搏分析开关
	iBioCMD_ID_PACE(gCfgEcg.bPacemaker);
	//ST测量的ISO 和ST点
	iBioCMD_ST_ISO(gCfgEcg.bISO,gCfgEcg.bST);//new
	//呼吸增益选择
	iBioCMD_RESP_GAIN(gCfgResp.bGain);
	//窒息报警时间设置
	iBioCMD_APNEA_ALM(gCfgResp.bApnea);//new
	//体温探头类型设置
	iBioCMD_TEMP_TYPE(gCfgTemp.bTempType);//new
	//SPO2灵敏度设置
	iBioCMD_SPO2(gCfgSpO2.bResponse);
	//NBP手动/自动设置
	//iBioCMD_NBP_PERIOD(0);//设置为手动
	//NBP设置初次充气压力
	if(B_PRINTF) printf("gCfgNibp.bObject:%d ,gCfgNibp.wInitPres:%d\n",gCfgNibp.bObject,gCfgNibp.wInitPres);
	iBioCMD_NBP_PRE_PRESSURE(gCfgNibp.bObject,gCfgNibp.wInitPres);
	//IBP压力通道名称
	iBioCMD_IBP_CHANNEL(gCfgIbp1.bViewMode,gCfgIbp2.bViewMode,gCfgIbp1.bLabel,gCfgIbp2.bLabel);//new
	//设置IBP数字滤波带宽
	iBioCMD_IBP_FILTER(gCfgIbp1.bFilterMode);//new
	//获取系统自检结果
	iBioCMD_GET_POST_RESULT();

	return 0 ;
}
//设置病人类型
int SetiBioPatientType()
{
	iBioCMD_PAT_TYPE(gCfgPatient.bObject);
	return 0;
}
//设置NIBP病人类型
int SetiBioNIBPPatType()
{
	iBioCMD_PAT_TYPE(gCfgNibp.bObject);
	return 0;
}
//设置ECG增益
int SetiBioECGGain()
{
	iBioCMD_ECG_GAIN(0x00,gCfgEcg.bGain);
	iBioCMD_ECG_GAIN(0x10,gCfgEcg.bGain);
	return 0;	
}

//设置ECG导联类型和测量导联
int SetiBioEcgLeadType()
{
	//导联设置3/5导联
	iBioCMD_LEADSYSTEM(gCfgEcg.bLeadType);
		
	return 0;
}

///3导联方式选择
int SetiBioECG3LeadChannel()
{
	iBioCMD_LEADTYPE(CHANNEL1,gCfgEcg.bChannel1);///3导联只有通道1
	
	return 0;
}

///5导联方式选择
int SetiBioECG5LeadChannel()
{
	//5导联不用设使用默认通道
	iBioCMD_LEADTYPE(CHANNEL1,gCfgEcg.bChannel1);
	iBioCMD_LEADTYPE(CHANNEL2,gCfgEcg.bChannel2);
		
	return 0;
}
int ChangeECGLeadChannel()
{
	if(gbiBioECGCaling){
	
		return 0;	
	}
		
	//非7导同屏
	if(gCfgSystem.bInterface!=SCREEN_7LEADECG&&
		gCfgSystem.bInterface!=SCREEN_MULTIECG){
		//通道1,2 为v导
		if(gCfgEcg.bChannel1==ECGWAVE_V||gCfgEcg.bChannel2==ECGWAVE_V){
			iBioCMD_LEADTYPE(CHANNEL1,gCfgEcg.bChannel1);
			iBioCMD_LEADTYPE(CHANNEL2,gCfgEcg.bChannel2);
		}else{
			if(gCfgEcg.bLeadType==ECGLEAD5){///5导联
				iBioCMD_LEADTYPE(CHANNEL2,2);
				
				iBioCMD_LEADTYPE(CHANNEL1,0);
				iBioCMD_LEADTYPE(CHANNEL2,1);
				
			}else
				iBioCMD_LEADTYPE(CHANNEL1,gCfgEcg.bChannel1);
		}
	}else{
		if(gCfgEcg.bLeadType==ECGLEAD5){///5导联
				iBioCMD_LEADTYPE(CHANNEL2,2);
				
				iBioCMD_LEADTYPE(CHANNEL1,0);
				iBioCMD_LEADTYPE(CHANNEL2,1);

		}else
			iBioCMD_LEADTYPE(CHANNEL1,gCfgEcg.bChannel1);
	}
	
	return 0;
}
//设置ECG测量方式
int SetiBioECGMode()
{
	
	//滤波方式选择
	iBioCMD_FILTER_MODE(gCfgEcg.bMode);
	return 0;	
}
//设置起搏分析开关
int SetiBioEcgPace()
{
	//起搏分析开关
	iBioCMD_ID_PACE(gCfgEcg.bPacemaker);
	return 0;
}

//设置ECG交流滤波控制工频滤波
int SetiBioEcgACFilter()
{
	//工频抑制设置
	iBioCMD_ECG_TRAP(gCfgEcg.bFilterAC);
	return 0;
}

//开始ECG校准
int StartiBioCalibrateECG()
{
	ChangeECGLeadChannel();
	iBioCMD_ECG_CAL(1);
	
	//是否处在定标状态
 	gbiBioECGCaling = TRUE;
	return 0;
}

//停止ECG校准
int StopiBioCalibrateECG()
{
	iBioCMD_ECG_CAL(0);
	//是否处在定标状态
 	gbiBioECGCaling = FALSE;
	return 0;
}
//设置ISO ST 点
int SetiBioEcgISOandST()
{
	//ST测量的ISO 和ST点
	iBioCMD_ST_ISO(gCfgEcg.bISO,gCfgEcg.bST);//new
	
	return 0;
}
//设置RESP增益
int SetiBioRespGain()
{
	//呼吸增益选择
	iBioCMD_RESP_GAIN(gCfgResp.bGain);
	return 0;
}
//设置窒息报警时间
int SetiBioRespApnea()
{
	iBioCMD_APNEA_ALM(gCfgResp.bApnea);//new
	return 0;
}
//设置respI/II导联设置
int SetiBioRespType(BYTE leadtype)
{
	iBioCMD_RESP_TYPE(leadtype);
	return 0;
}
//SPO2灵敏度设置
int SetiBioSpO2Sense()
{
	
	iBioCMD_SPO2(gCfgSpO2.bResponse);
	return 0;
}

//开始NIBP测量
int StartiBioNibpMeasure()
{
	iBioCMD_NBP_START();
	//复位计数器
	giNibpProtectCounter = 0;
	gbNibpProtect = FALSE;	
	gValueNibp.wErrCode=0;
	return 0;
}

//取消NIBP测量
int StopiBioNibpMeasure()
{
	iBioCMD_NBP_END();
	
	//复位计数器
	giNibpProtectCounter = 0;
	gbNibpProtect = FALSE;

	return 0;
}
//血压校准
int CalibrateiBioNibp()
{
	iBioCMD_NBP_CAL();
	 gbNibpProtect = FALSE;
	 gbNibpCal=TRUE;
	return 0;
}
//漏气检测
//iPressure:充气压力
int iBioNibpLeakTest()
{
	iBioCMD_NBP_PNEUMATIC();
	 gbNibpLeak=TRUE;
	gbNibpProtect = FALSE;
	gValueNibp.wErrCode&=~0xffff;
	return 0;
}
//设置初始化充气压力
int SetiBioNibpInitPres()
{
	//NBP设置初次充气压力
	iBioCMD_NBP_PRE_PRESSURE(gCfgNibp.bObject,gCfgNibp.wInitPres);
	
	gbNibpProtect = FALSE;
	return 0;
}
//获取NIBP测量结果
int GetNibpResult()
{
	iBioCMD_NBP_GET_RESULT();
	return 0;
}
//获取NIBP当前状态
int GetNibpPollState()
{
	iBioCMD_NBP_POLL_STATE();
	return 0;
}
//设置NIBP测量周期
int SetIBioNibpPeriod(BYTE Period)
{
	iBioCMD_NBP_PERIOD(Period);
	return 0;
}
//体温探头类型设置
int SetIBioTempType()
{
	iBioCMD_TEMP_TYPE(gCfgTemp.bTempType);//new
	return 0;
}
	
/*
	接收线程
*/
void  *ptProc_iBioRecv(void *arg)
{
	int iLen; 
	int i;
	unsigned char bBuff[RECVBUFF_MAX_IBIO];	
	
	if(B_PRINTF) printf("(%s:%d)create read thread for %s .\n", __FILE__, __LINE__, gSerialsName[gSerialIndex_iBio]);
	
	for(;;){

		iLen = iBio_Recv(bBuff, RECVBUFF_MAX_IBIO, 115200);

		if(iLen>0){
			
 			iBio_UnBind(bBuff,iLen);
		}
		else{
		//	if(B_PRINTF) printf("(%s:%d) %s ID:%d receive error.\n\n", __FILE__, __LINE__, gSerialsName[gSerialIndex_iBio], gSerialIndex_iBio);
			
		}
	}
	
	if(B_PRINTF) printf("\nBye from %d thread.\n", gfdSerial_iBio);
	pthread_exit("Bye");
}



/*
	初始化测量模块
*/
int InitiBio()
{
	int res;
		
	if(B_PRINTF) printf("%s:%d Init iBio board ...\n", __FILE__, __LINE__);
	//初始化信号量
	res = sem_init(&semSend_iBio, 0, 1);	
	
	//打开串口设备
	gfdSerial_iBio = port_open(gSerialsName[gSerialIndex_iBio], "115200", 8, 0, 1, 0);
	if(gfdSerial_iBio<0){
		if(B_PRINTF) printf("%s:%d Serial_iBio ERROR!\n", __FILE__, __LINE__);
		return -1;
	}else
		if(B_PRINTF) printf("%s:%d Serial_iBio=%d OK!\n", __FILE__, __LINE__,gfdSerial_iBio);
		
	//注册接收线程 
	res = pthread_create(&ptID_iBioRecv, NULL, ptProc_iBioRecv, NULL);
	if(res<0){
		if(B_PRINTF) printf("%s:%d BioRecv pthread ERROR!\n", __FILE__, __LINE__);
		return -1;
	}else
		if(B_PRINTF) printf("%s:%d OK!\n", __FILE__, __LINE__);
//	iBioCMD_ECG_TRAP(1);
	return 0;
}

////////////////////////////////////////

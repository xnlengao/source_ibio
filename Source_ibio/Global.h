/*************************************************************************** 
 *	Module Name:	Global.h	
 *
 *	Abstract:	声明一些经常用到的全局变量
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *	
 ***************************************************************************/
#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "DataStruct.h"
#include "NewDrawWave.h"
//屏幕大小定义
//显示屏幕
// int SCREEN_LEFT;	
// int SCREEN_TOP;		
// int SCREEN_RIGHT;	
// int SCREEN_BOTTOM;	


//系统使用到的文件路径
#define MINIGUICFG			"cfg/MiniGUI.cfg"
//Language resource 
//#define LANPATH_OTHER		"language/Other.lan"
//#define LANPATH_ENGLISH		"language/English.lan"
#define LANPATH_ENGLISH		"language/English.lng"
#define LANPATH_CHINESE		"language/Chinese.lng"
#define LANPATH_CHT			"language/CHT.lng"
#define LANPATH_TURKISH		"language/Turkish.lng"
#define LANPATH_RUSSIAN		"language/Russian.lng"
#define LANPATH_ROMANIA		"language/Romania.lng"
#define LANPATH_SPANISH		"language/Spanish.lng"
#define LANPATH_ITALIAN		"language/Italian.lng"

//machine config
#define CFGPATH_MACHINE 	"cfg/machine.cfg"
#define CFGPATH_RECENT	 	"cfg/RecentSet.cfg"
#define CFGPATH_ADULT		"cfg/AdultSet.cfg"
#define CFGPATH_CHILD		"cfg/ChildSet.cfg"
#define CFGPATH_BABY		"cfg/BabySet.cfg"
#define CFGPATH_USER1		"cfg/User1Set.cfg"	
#define CFGPATH_USER2		"cfg/User2Set.cfg"	
#define CFGPATH_USER3		"cfg/User3Set.cfg"	
//Trend data
#define TRENDPATH				"trend/trend.ttt"
// #define TRENDPATH			"trend.ttt"
//Demo Files
#define DEMOPATH_PM9K		"demo/pm9k.demo"
#define DEMOPATH_IBP			"demo/ibp.demo"
#define DEMOPATH_CO2		"demo/co2.demo"

//Logo Picture
#define LOGO_PATH				"/mnt/data/logo.bmp"

//定义读取资源文件的索引
#define  	STR_SETTING_GENERAL				"GENERAL"
#define  	STR_SETTING_ACK	 				"ACK"
#define 	STR_SETTING_DLG_ECG	 	 		"DLG_ECG"
#define 	STR_SETTING_DLG_ECGSET	 		"DLG_ECGSET"
#define 	STR_SETTING_DLG_ECGALM	 		"DLG_ECGALM"
#define 	STR_SETTING_DLG_STADJUST 		"DLG_ST_ADJUST"
#define 	STR_SETTING_DLG_NIBP 				"DLG_NIBP"
#define 	STR_SETTING_DLG_NIBPALM 			"DLG_NIBPALM"
#define 	STR_SETTING_DLG_SPO2				"DLG_SPO2"
#define 	STR_SETTING_DLG_SPO2ALM			"DLG_SPO2ALM"
#define 	STR_SETTING_DLG_SPO2PRALM		"DLG_SPO2PRALM"
#define 	STR_SETTING_DLG_RESP				"DLG_RESP"
#define 	STR_SETTING_DLG_RESP_ALM		"DLG_RESPALM"
#define 	STR_SETTING_DLG_TEMP				"DLG_TEMP"
#define 	STR_SETTING_DLG_MAINMENU		"DLG_MAINMENU"
#define 	STR_SETTING_DLG_MONITOR			"DLG_MONITOR"
#define 	STR_SETTING_DLG_ALMSET			"DLG_ALMSET"
#define 	STR_SETTING_DLG_PATIENT			"DLG_PATIENT"
#define 	STR_SETTING_DLG_ALMCONTROL		"DLG_ALMCONTROL"
#define 	STR_SETTING_DLG_ALMSETUP		"DLG_ALMSETUP"
#define 	STR_SETTING_DLG_DRUG				"DLG_DRUG"
#define 	STR_SETTING_DLG_DRUGTITR			"DLG_DRUGTITR"
#define 	STR_SETTING_DLG_SHORTTREND		"DLG_SHORTTREND"
#define 	STR_SETTING_DLG_WAVEFORM		"DLG_WAVEFORM"
#define 	STR_SETTING_DLG_SYSTEMSET		"DLG_SYSTEMSET"
#define 	STR_SETTING_DLG_TIME				"DLG_TIME"
#define 	STR_SETTING_DLG_COLOR			"DLG_COLOR"
#define 	STR_SETTING_DLG_MODECONF		"DLG_MODECONF"
#define 	STR_SETTING_DLG_MACHINE			"DLG_MACHINE"
#define 	STR_SETTING_DLG_SYSMAINT			"DLG_SYSMAINT"
#define 	STR_SETTING_DLG_NET				"DLG_NET"
#define 	STR_SETTING_DLG_MODULEMAINT	"DLG_MODULEMAINT"
#define 	STR_SETTING_DLG_FACTORY			"DLG_FACTORY"
#define 	STR_SETTING_DLG_SELECTMODULE	"DLG_SELECTMODULE"
#define 	STR_SETTING_DLG_ALMREVIEW		"ALARM_REVIEW"
#define 	STR_SETTING_DLG_KEYBOARD 		"DLG_KEYBOARD"
#define 	STR_SETTING_DLG_SYSINFO			"DLG_SYSINFO"
#define 	STR_SETTING_DLG_PRINTER			"DLG_PRINTER"
#define 	STR_SETTING_DLG_POWEROFF		"DLG_POWEROFF"
#define 	STR_SETTING_DLG_ALMPRINT			"ALARM_PRINT"
#define 	STR_SETTING_DLG_NIBPMAINT		"DLG_NIBPMAINT"
#define 	STR_SETTING_DLG_IBP1				"IBP"
#define 	STR_SETTING_DLG_IBP1a				"IBP"
#define 	STR_SETTING_DLG_IBP2				"IBP"
#define 	STR_SETTING_DLG_IBP2a				"IBP"
#define 	STR_SETTING_DLG_ECGARR			"DLG_ECGARR"
/*
	根据实际情况控制键盘驱动的方向键应该什么样的键码
	KEY_LR:Left & Right
	KEY_TST:TAB & Shift+TAB
	KEY_UD:Up & Down
*/
#define KEY_LR		0
#define KEY_TAB		1
#define KEY_UD		2
#define KEY_AD		3				//left right
#define KEY_WS		4				//up down

//led灯闪烁频率
#define HIGH_ALARM_LED			25 	///2hz
#define MIDDLE_ALARM_LED		100	//0.5hz
#define LOW_ALARM_LED			0	//常亮

BYTE gbKeyType;

//系统电源状态
POWER_STATUS	gPowerStatus;
		
//自定义的定时器消息
#define B_MSG_TIMER     (MSG_USER + 1)  
//绘制波形的消息
#define B_MSG_WAVE     (MSG_USER + 2)  


//机器配置文件
unsigned char gFileMachineConfig[40];
//系统配置文件
unsigned char gFileSetup[40];

//语言资源文件
unsigned char gsLanguageRes[40];
//符合当前语言的字库
PLOGFONT gFontSystem, gFontSmall,gFontSystem_12;
//可以显示英文,数字的字库
PLOGFONT gFontEnglish_12, gFontEnglish_16, gFontEnglish_24;
//TTF字库，用来显示数字或者英文
PLOGFONT gFontTTF_20; 
PLOGFONT gFontTTF_30;
PLOGFONT gFontTTF_35;
PLOGFONT gFontTTF_40;
PLOGFONT gFontTTF_50;
PLOGFONT gFontTTF_20Bk;
PLOGFONT gFontTTF_30Bk;
PLOGFONT gFontTTF_35Bk;
PLOGFONT gFontTTF_40Bk;
PLOGFONT gFontTTF_50Bk;
PLOGFONT gFontTTF_90Bk;
PLOGFONT gFontTTF_60;
PLOGFONT gFontTTF_70;
PLOGFONT gFontTTF_75;
PLOGFONT gFontTTF_105;
PLOGFONT gFontTTF_120;

/*使用到的图标*/
//报警关闭
BITMAP gIconAlarmOff;
//QRS声音
BITMAP gIconAudio;
//QRS声音关闭
BITMAP gIconAudioOff;
//停止报警发声
BITMAP gIconAlmAudioOff;
//暂停报警发声
BITMAP gIconAlmAudioPause;
//打印机
BITMAP gIconPrinter;
//打印机故障
BITMAP gIconPrinterError;
//正在打印
BITMAP gIconPrinting;
//使用交流电
BITMAP gIconAC;
//使用交流电
BITMAP gIconACOff;
//充电电池电量
BITMAP gIconBatteryAC[6];
//电池电量
BITMAP gIconBattery[6];
//心率图标:0-big, 1-small
BITMAP gIconHR[2];
//NIBP周期测量图标
BITMAP gIconCycle;
//Demo
BITMAP gIconDemo;
//Net Status 
BITMAP gIconNetStatus[3];		//0-Connect, 1-Not Connect
//Next Page 
BITMAP gIconNextPage;



//机器模块配置信息
S_MODULES gsModules;
BOOL gbHaveNIBP;
BOOL gbHaveSpo2;
BOOL gbHaveResp;
BOOL gbHaveTemp;
//如果有CO2模块，显示方面由CO2取代Resp的位置
BOOL gbHaveCo2;		//是否有CO2模块
//如果有IBP模块，则需要增加两个显示模块IBP1和IBP2
BOOL gbHaveIbp;		//是否有IBP模块
//如果有Nellcor SpO2,则替换9k的SpO2
BOOL gbHaveNellSpO2;		//是否有Nellcor SpO2


//各模块的配置信息
S_ECG_CFG	gCfgEcg;
S_RESP_CFG	gCfgResp;
S_SPO2_CFG	gCfgSpO2;
S_PULSE_CFG	gCfgPulse;
S_TEMP_CFG	gCfgTemp;
S_NIBP_CFG	gCfgNibp;	
S_IBP_CFG	gCfgIbp1, gCfgIbp2;
S_CO2_CFG	gCfgCO2;	

//病人信息配置
S_PATIENT_CFG  gCfgPatient;
//系统配置信息
S_SYSTEM_CFG	 gCfgSystem;
//报警功能配置
S_ALARM_CFG  gCfgAlarm;


//模块参数数据（单个值）
S_ECG_VALUE		gValueEcg;	
S_RESP_VALUE	gValueResp;	
S_SPO2_VALUE	gValueSpO2;	
S_PULSE_VALUE	gValuePulse;	
S_TEMP_VALUE	gValueTemp;
S_NIBP_VALUE	gValueNibp;
S_IBP_VALUE		gValueIbp1;
S_IBP_VALUE		gValueIbp2;
S_CO2_VALUE		gValueCO2;

//是否显示DEMO数据
BOOL gbViewDemoData;

//-- Demo 演示数据 --
#define DEMOCOUNT_PM9K	500		//50Hz    
#define DEMOCOUNT_IBP2		500		//50Hz
#define DEMOCOUNT_CO2		1000	//100Hz
FILE            *gfileDemo_PM9K;		//文件标志
struct stat gDemoStat_PM9K;		//文件状态
BOOL gbSaveDemo_PM9K;			//是否保存DEMO文件
FILE            *gfileDemo_IBP;
struct stat gDemoStat_IBP;
BOOL gbSaveDemo_IBP;
FILE            *gfileDemo_CO2;
struct stat gDemoStat_CO2;
BOOL gbSaveDemo_CO2;

//波形冻结回放的控制变量
//显示波形的数量
#define WAVEREVIEW_COUNT		3
BYTE gbWaveReviewIDs[WAVEREVIEW_COUNT];


//打印配置
S_PRINT_CFG	gCfgPrinter;
		
//打印机工作状态
int gPrinterStatus;
//打印机正在进行的工作类型
int gPrinterWorkType;
int gPrintingType;

//实时打印的参数
REC_WAVEPARAM	gPrintRealWaveParam;
REC_REALWAVE 		gPrintRealWaves;
//发送计数
int giPrintRealWaveIndex;
//发送计时
int giPrintRealWaveCount;

//报警时刻的数据＋波形
ALARMDATA		gPrintAlarmData;

//增加一个写成功的标志，预防突然断电时，没有完全写入文件，导致文件损坏，无法开机  2008-04-16 17:56:44
BOOL gbTrendFileWrite_OK;

S_NET_CFG		gCfgNet;

//是否进行心电分析
BOOL gbEcgAnalysis;

//是否为PM41主板 
BOOL gbPMVer41;

//Sensor
BOOL gbSensor_Temp;		//0-503Et, 1-YSI-400
BOOL gbSensor_Ibp;		//0-ABBOTT, 1-BIO
BOOL gbSensor_SpO2;		//0-BCI, 1-Nellcor

//IBP 校准系数
double gfIbp1Check;
double gfIbp2Check;

//针对NIBP过压保护的不同企标
//0:0323-2007
///1:0341-2009
int giStandards;		

//测量对象
//0: Human
///1: Animal
BYTE gbMonitorObject;

//New Global set
//趋势存储间隔时间
int giTrendInterval;//0=OFF,1~10:1 2 3 4 5 10 15 20 25 30
BOOL gbNIBPStorage;
BOOL gbALMStorage;
BOOL gbWarnStorage;

//短趋势图标尺
//int giTimeScale;
int giHRScale;
int giSpo2Scale;
int giRespScale;
int giSTScale;
int giIBP1Scale;
int giIBP2Scale;
int giEtCO2Scale;
int giCIScale;

char gstrNextPage[10];

#endif	//__GLOBAL_H__

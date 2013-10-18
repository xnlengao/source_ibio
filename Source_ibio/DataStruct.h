/*************************************************************************** 
 *	Module Name:	DataStruct.h	
 *
 *	Abstract:	定义基本的数据结构
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-05-22 10:27:24
 ***************************************************************************/
#ifndef __DATASTRUCT_H__
#define __DATASTRUCT_H__

//定义屏幕类型
#define SCREEN_1024			0

#define SCREEN_640			0

//他床观察功能
#define OTHERBED_FUNC		1


//#define HAVE_PRINTER	1

//此处修改厂家标志
#if SCREEN_1024
	#define VERSION_SOFT		"L-2.00"
#elif SCREEN_640
	#define VERSION_SOFT		"S-2.00"
#else 
	#define VERSION_SOFT		"M-2.00"
#endif	


//调试输出
#define	DEBUGINFO()		printf("%s-%s:%d\n", __FILE__, __func__, __LINE__)

#define  B_PRINTF 	1

//显示屏幕
#if SCREEN_1024
#define SCREEN_LEFT		0
#define SCREEN_TOP		0
#define SCREEN_RIGHT		1024
#define SCREEN_BOTTOM		768
#elif  SCREEN_640
#define SCREEN_LEFT		0
#define SCREEN_TOP		0
#define SCREEN_RIGHT		640
#define SCREEN_BOTTOM		480
#else
#define SCREEN_LEFT		0
#define SCREEN_TOP		0
#define SCREEN_RIGHT		800
#define SCREEN_BOTTOM		600
#endif

//参数显示区域的左右边界（除大字符视图外）
#if SCREEN_1024	
#define PARAVIEW_TOP			(SCREEN_TOP + 35)
#define PARAVIEW_LEFT			(SCREEN_RIGHT - 290)	
#define PARAVIEW_RIGHT			(SCREEN_RIGHT - 2)
#define PARAVIEW_BOTTOM		(SCREEN_BOTTOM)
#elif  SCREEN_640
#define PARAVIEW_TOP			(SCREEN_TOP + 35)
#define PARAVIEW_LEFT			(SCREEN_RIGHT - 200)	
#define PARAVIEW_RIGHT			(SCREEN_RIGHT - 2)
#define PARAVIEW_BOTTOM		(SCREEN_BOTTOM )
#else
#define PARAVIEW_TOP			(SCREEN_TOP + 35)
#define PARAVIEW_LEFT			(SCREEN_RIGHT - 245)	
#define PARAVIEW_RIGHT			(SCREEN_RIGHT - 2)
#define PARAVIEW_BOTTOM		(SCREEN_BOTTOM )
#endif
//串口配置(索引)
#define DEV_SERIAL0		0
#define DEV_SERIAL1		1
#define DEV_SERIAL2		2
#define DEV_SERIAL3		3
#define DEV_SERIAL4		4
#define DEV_SERIAL5		5
#define DEV_SERIAL6		6
#define DEV_SERIAL7		7


//参数模块配置
#define MODULE_MAX_COUNT	10
#define MOD_NONE			0
#define MOD_ECG				1
#define MOD_NIBP			2
#define MOD_SPO2			3
#define MOD_PULSE			4
#define MOD_RESP			5
#define MOD_TEMP			6
#define MOD_IBP1				7
#define MOD_IBP2				8
#define MOD_CO2				9

//模块不使用	
#define MOD_DISABLE	0	
//模块在使用
#define MOD_ENABLE	1	
typedef struct {
	BYTE bID;			//模块ID: MOD_X
	BYTE bSerial;		//模块使用的串口: DEV_SERIALX	
	BYTE bStatus;		//模块状态: MOD_DISABLE Or MOD_ENABLE
}S_MODULE, *PS_MODULE;

typedef struct {
	BYTE bCount;		//现有(有效的)模块的数量
	S_MODULE s_Modules[MODULE_MAX_COUNT];	//所有模块信息
}S_MODULES, *PS_MODULES;

//系统时间结构                             
typedef struct {
	WORD wYear;
	BYTE bMonth;
	BYTE bDay;
	BYTE bHour;
	BYTE bMin;
	BYTE bSec;
}S_TIME, *PS_TIME;

//基线
#define BASELINE_ECG		2047
#define BASELINE_SPO2		50
#define BASELINE_RESP		1027

//-------------- 各模块的数据结构 ------------------
//音量
#define	VOLUME_OFF		0
#define	VOLUME_1		1
#define	VOLUME_2		2
#define	VOLUME_3		3
#define	VOLUME_4		4
#define	VOLUME_5		5
#define	VOLUME_6		6
#define	VOLUME_7		7
#define	VOLUME_8		8
#define	VOLUME_9		9
#define	VOLUME_10		10

//扫描速度
#define SCANSPEED_625		0
#define SCANSPEED_125		1
#define SCANSPEED_25		2
#define SCANSPEED_50		3
//交流频率
#define AC_50HZ			0
#define AC_60HZ			1
//报警控制
#define ALARM_OFF		0
#define ALARM_LOW		1
#define ALARM_MID		2
#define ALARM_HIGH		3
#define ALARM_INFO		4	//一般提示信息

//报警类型
#define ALARM_TECHNOLOGY   	0
#define ALARM_PHYSIOLOGICAL	1
//报警打印
#define ALARM_PRINT_OFF		0
#define ALARM_PRINT_ON			1
//报警回顾存储
#define ALARM_REVIEW_OFF		0
#define ALARM_REVIEW_ON		1
//开关状态
#define SWITCH_OFF		0
#define SWITCH_ON		1
//模块状态
#define MODULE_OFF		1
#define MODULE_ON		0
//模块通讯状态
#define COMMUNICATION_ERR	0
#define COMMUNICATION_OK	1
//波形描记方式
#define DRAWMODE_LINE		0
#define DRAWMODE_FILL		1

//---- ECG的相关定义 ----
//心率测量范围(bpm)
#define HR_MAX	350
#define HR_MIN	0

#define  ST_MAX		(100)
#define  ST_MIN		(-100)

//导联定义
#define ECGWAVE_COUNT		7	//7道波形
#define ECGWAVE_I		0
#define ECGWAVE_II		1
#define ECGWAVE_III		2
#define ECGWAVE_AVR		3
#define ECGWAVE_AVL		4
#define ECGWAVE_AVF		5
#define ECGWAVE_V		6
//12导联定义
#define ECG12WAVE_COUNT		12	//7道波形
#define ECG12WAVE_I		0
#define ECG12WAVE_II		1
#define ECG12WAVE_III		2
#define ECG12WAVE_AVR		3
#define ECG12WAVE_AVL		4
#define ECG12WAVE_AVF		5
#define ECG12WAVE_V1		6
#define ECG12WAVE_V2		7
#define ECG12WAVE_V3		8
#define ECG12WAVE_V4		9
#define ECG12WAVE_V5		10
#define ECG12WAVE_V6		11


//波形标示
#define WAVEFORM_OFF		0
#define WAVEFORM_ECG1		1
#define WAVEFORM_CASCADE	2
#define WAVEFORM_I  			3
#define WAVEFORM_II			4
#define WAVEFORM_III			5
#define WAVEFORM_AVL  		6
#define WAVEFORM_AVR		7
#define WAVEFORM_AVF 		8
#define WAVEFORM_V  			9
#define WAVEFORM_SPO2		10
#define WAVEFORM_RESP 		11
#define WAVEFORM_IBP1  		12
#define WAVEFORM_IBP2 		13
#define WAVEFORM_CO2  		14
#define WAVEFORM_MAX		15


//工作模式
#define ECGMODE_DIAG		0
#define ECGMODE_MON		1
#define ECGMODE_OPE		2
//导联类型
#define ECGTYPE_5LEAD	0
#define ECGTYPE_3LEAD	1
//波形增益
#define ECGGAIN_025X	0
#define ECGGAIN_05X	1
#define ECGGAIN_1X	2
#define ECGGAIN_2X	3
//报警来源(HR Or PR)
#define HR_ALM_SRC_AUTO	0
#define HR_ALM_SRC_HR	1
#define HR_ALM_SRC_PR	2
//交流滤波控制
#define ECGFILTER_AC_OFF	0
#define ECGFILTER_AC_ON		1
//基线漂移滤波
#define ECGFILTER_DRIFT_OFF	0
#define ECGFILTER_DRIFT_ON	1

//肌电滤波控制
#define ECGFILTER_HUM_OFF	0
#define ECGFILTER_HUM_ON	1

//起搏
#define PACEMAKER_OFF	0
#define PACEMAKER_MODE1	1
#define PACEMAKER_MODE2		1
//波形级联
#define CASCADE_OFF		0
#define CASCADE_ON		1
///1mv校准信号设置
#define CAL1MV_OFF		0
#define CAL1MV_ON		1
//ECG配置信息
typedef struct {
	BYTE bID;			//模块ID
	BYTE bSwitch;		//模块开关: SWITCH_OFF， SWITCH_ON
	int iColor;			//波形数据颜色
	BYTE bLeadType;		//导联类型: ECGTYPE_5LEAD or ECGTYPE_3LEAD
	BYTE bScanSpeed;	//扫描速度: SCANSPEED_125, SCANSPEED_25, SCANSPEED_50
	BYTE bPacemaker;	//是否起搏: PACEMAKER_OFF or PACEMAKER_ON, 与病人信息的配置要一致	
	BYTE bChannel1;		//显示通道1: ECGWAVE_X（不能关闭）
	BYTE bChannel2;		//显示通道2: ECGWAVE_X（三导联时无效,关闭显示）
	BOOL bCascade;		//波形级联: CASCADE_OFF, CASCADE_ON
	BYTE bMode;			//工作模式: ECGMODE_DIAG－诊断，ECGMODE_MON－监护，ECGMODE_OPE－手术
	BYTE bFilterAC;		//交流滤波: ECGFILTER_AC_OFF or ECGFILTER_AC_ON
	BYTE bFilterDrift;		//基线漂移滤波: ECGFILTER_DRIFT_OFF, ECGFILTER_DRIFT_ON
	BYTE bFilterHum;		//肌电滤波: OFF or ON
	BYTE bGain;			//波形增益: ECGGAIN_XX
	WORD wHigh_HR;		//心率上限
	WORD wLow_HR;		//心率下限
	BYTE bBeepVolume;	//心率音量: VOLUME_XX
	BYTE bAlmSource;	//心率报警来源: HR_ALM_SRC_AUTO, HR_ALM_SRC_HR, HR_ALM_SRC_PR
	BYTE bAlmControl;	//报警控制: ALARM_OFF, ALARM_LOW, ALARM_MID, ALARM_HIGH
	BYTE bPrnControl;	//报警触发打印: ALARM_PRINT_OFF, ALARM_PRINT_ON
	BYTE b1mvCal;	///1mv校准信号设置CAL1MV_OFF CAL1MV_ON
	int iAdjust;		//校准系数(百分比)
	BYTE bISO;			//ISO
	BYTE bST;			//ST
	BYTE bArrAnalyse;	//心律失常分析开关
	BYTE bPVCs;			//pvcs显示开关
	int iMax_HR;			//短趋势中的标尺最大值
	int iMin_HR;			//短趋势中的标尺最小值
	int iSt1_High;		//x100
	int iSt1_Low;		//x100
	int iSt2_High;		//x100
	int iSt2_Low;		//x100
}S_ECG_CFG, *PS_ECG_CFG;
//ECG参数信息
typedef struct {
	BYTE bID;			//模块ID
	BOOL bConnected;		//是否接过人体，FALSE－未接过，TRUE－已经接过
	BYTE bCommStatus;		//通讯状态，COMMUNICATION_OK, COMMUNICATION_ERR
	BOOL bAdjust;			//ECG校准，FALSE－正常测量，TRUE－正在校准 
	BYTE bLeadType;			//导联类型，ECGTYPE_5LEAD or ECGTYPE_3LEAD
	int wHR;			//心率
	BOOL bBeep;			//Beep，FALSE－无，TRUE－有
	BYTE bPacingpulse;		//起搏脉冲，0－无，1－有
	int iWaves[ECGWAVE_COUNT];	//波形
	BOOL bRAOff;			//导联RA脱落: TRUE-Off, FALSE-ON
	BOOL bRLOff;			//导联RL脱落: TRUE-Off, FALSE-ON
	BOOL bLAOff;			//导联LA脱落
	BOOL bLLOff;			//导联LL脱落
	BOOL bVOff;			//导联V脱落
	BOOL bHRInval;			//HR无效值标志位-100
	BOOL bOverLoad1;			//ECG通道1过载信息TRUE-过载，FALSE-正常
	BOOL bOverLoad2;			//ECG通道2过载信息
	BOOL bOverLoad3;			//ECG通道3过载信息
	BYTE bPVC;					//PVC
	unsigned long lARR;					//ARR	
	int iST1;			//ST1，10x
	int iST2;			//ST2，10x
	int iST[ECGWAVE_COUNT];		//ST I~V
}S_ECG_VALUE, *PS_ECG_VALUE;

//---- RESP的相关定义 ----
//呼吸率测量范围(rpm)
#define RR_MAX			180
#define RR_MIN			0
//呼吸波形增益
#define RESPGAIN_025X		0
#define RESPGAIN_05X		1
#define RESPGAIN_1X		2
#define RESPGAIN_2X		3
#define RESPGAIN_4X		4
#define RESPGAIN_8X		5
#define RESPGAIN_16X		6
#define RESPGAIN_32X		7
//窒息时间(Sec)
#define APNEATIME_OFF		0
#define APNEATIME_MAX		45
//RR来源
#define RR_SRC_ECG		0
#define RR_SRC_CO2		1
//呼吸I/II导联设置
#define RR_LEADTYPE_OFF	0
#define RR_LEADTYPE1		1
#define RR_LEADTYPE2		2
//RESP配置信息
typedef struct {
	BYTE bID;		//模块ID
	BYTE bSwitch;		//模块开关: SWITCH_OFF， SWITCH_ON
	int iColor;			//颜色
	BYTE bSource;		//呼吸率报警来源:RR_SRC_ECG, RR_SRC_CO2
	BYTE bGain;			//波形增益: RESPGAIN_05X, RESPGAIN_1X, RESPGAIN_2X, RESPGAIN_4X
	BYTE bScanSpeed;	//扫描速度: SCANSPEED_625, SCANSPEED_125, SCANSPEED_25
	WORD wHigh_RR;		//呼吸率上限
	WORD wLow_RR;		//呼吸率下限
	WORD wApneaTime;	//窒息报警时间0-Off	, x*10s(10s, 20s, .... 120s), 单位:10s//废弃
	BYTE bApnea;		//窒息报警时间0-Off	, 1~7  10s 15s 20s 25s 30s 35s 40s
	BYTE bAlmControl;	//报警控制: ALARM_OFF, ALARM_LOW, ALARM_MID, ALARM_HIGH
	BYTE bPrnControl;	//报警触发打印: ALARM_PRINT_OFF, ALARM_PRINT_ON
	BYTE bLeadType;		 //呼吸I/II导联设置RR_LEADTYPE1 RR_LEADTYPE2
	BYTE bMax_RR;		//短趋势中的标尺最大值
	BYTE bMin_RR;		//短趋势中的标尺最小值
}S_RESP_CFG, *PS_RESP_CFG;
//RESP参数信息
typedef struct {
	BYTE bID;			//模块ID
	BOOL bConnected;	//是否接过人体:FALSE－未接过，TRUE－已经接过
	BYTE bCommStatus;	//通讯状态，COMMUNICATION_OK, COMMUNICATION_ERR
	BOOL bApneaAlm;		//窒息报警
	BOOL bCVAAlm;		//心动干扰，心电信号叠加在呼吸波上的干扰
	int wRR;				//呼吸率
	int iWave;			//呼吸波形
	BOOL bInval;			// 无效值标志位
	BOOL bartifact;		//出现伪差
	BOOL bBaselineErr;	//RESP基线超出限值
	BOOL bOff;			//呼吸脱落: TRUE-Off, FALSE-ON
}S_RESP_VALUE, *PS_RESP_VALUE;


//---- SpO2的相关定义 ----
//血氧测量范围(%)
#define SPO2_MAX		100
#define SPO2_MIN		0
//响应灵敏度
#define SPO2_FAST		0
#define SPO2_NORMAL		1
#define SPO2_SLOW		2
//波形增益
#define SPO2GAIN_AUTO	0 
#define SPO2GAIN_05X	1
#define SPO2GAIN_1X		2
#define SPO2GAIN_2X		3
//滤波
#define SPO2FILTER_OFF	0
#define SPO2FILTER_ON	1
//SpO2配置信息
typedef struct {
	BYTE bID;			//模块ID
	BYTE bSwitch;		//模块开关: SWITCH_OFF， SWITCH_ON
	int iColor;			//颜色
	BYTE bResponse;		//响应速度: SPO2_FAST, SPO2_NORMAL, SPO2_SLOW
	BYTE bGain;			//波形增益: SPO2GAIN_AUTO, SPO2GAIN_05X, SPO2GAIN_1X, SPO2GAIN_2X
	BYTE bScanSpeed;	//扫描速度: SCANSPEED_125, SCANSPEED_25, SCANSPEED_50
	BYTE bFilter;		//波形滤波: SPO2FILTER_OFF, SPO2FILTER_ON
	BYTE bDrawMode;		//波形描计方式: DRAWMODE_LINE or DRAWMODE_FILL
	BYTE bHigh_SpO2;	//SpO2上限
	BYTE bLow_SpO2;		//SpO2下限
	BYTE bLowest_SpO2;	//低饱和度极限, 低于此极限后高级报警
	BYTE bAlmControl;	//报警控制
	BYTE bPrnControl;	//报警触发打印
	int iAdjust;		//校准系数(+-)
	BYTE bMax_SPO2;	//短趋势中的标尺最大值
	BYTE bMin_SPO2;		//短趋势中的标尺最小值
}S_SPO2_CFG, *PS_SPO2_CFG;
//SpO2参数信息
//要区分模块本身的错误和测量过程中出现的错误
#define SPO2_INTERFERE		BIT0
#define SPO2_SEARCHPULSE	BIT1
#define SPO2_HIGHLIGHT		BIT2
#define SPO2_WEAKSIGNAL		BIT3
#define SPO2_SIGNALBAD		BIT4
typedef struct {
	BYTE bID;			//模块ID
	BYTE bConnected;		//是否接过人体
	BYTE bSpO2;			//SpO2 value -100－无效值
	BOOL bInvalSpo2;		//SpO2 value  无效值标志
	BYTE bWave;			//SpO2 wave
	BOOL bBeep;			//Beep，FALSE－无脉搏声，TRUE－有脉搏声
	BYTE bStrength;			//信号强度
	BOOL bComError;			//通讯故障	
	BOOL bComStop;			//通讯停止
	BOOL bModuleInitError;		//模块初始化错误
	BOOL bModuleError;		//模块错误
	BOOL bLimitError;		//报警限错误
	BOOL bOverrange;		//测量超界
	BOOL bNoSensor;			//未接传感器
	BOOL bSensorError;		//传感器故障
	BOOL bUnknownSensor;		//未知传感器
	BOOL bIncompatible;		//不兼容的传感器
	BOOL bSearchPulse;		//搜索脉搏标志，1-表示正在搜索脉搏
	BOOL bSearchTimeout;			//搜索时间太长标志：1表示搜索脉搏的时间>15秒
	BOOL bSpo2drop;			//氧饱和度下降标志，1 表示氧饱和度下降
	BOOL bOff;			//探头脱落
	BYTE bErrCode;			//错误代码，0-OK, Bit0:Interfere, Bit1:Search Pulse, Bit2:HighLight, Bit3:Weak Signal, Bit4: Signal Bad
}S_SPO2_VALUE, *PS_SPO2_VALUE;


//---- Pulse的相关定义 ----
//脉率测量范围(bpm)
#define PR_MAX	300
#define PR_MIN	0
//Pulse配置信息
typedef struct {
	BYTE bID;			//模块ID
	BYTE bSwitch;			//模块开关: SWITCH_OFF， SWITCH_ON
	int iColor;			//颜色
	BYTE bBeepVolume;		//脉搏音量: VOLUME_XX
	WORD wHigh_PR;			//脉率上限
	WORD wLow_PR;			//脉率下限
	BYTE bAlmSource;		//心率报警来源: HR_ALM_SRC_AUTO, HR_ALM_SRC_HR, HR_ALM_SRC_PR
	BYTE bAlmControl;		//报警控制
	BYTE bPrnControl;		//报警触发打印
}S_PULSE_CFG, *PS_PULSE_CFG;
//Pulse参数信息
typedef struct {
	BYTE bID;			//模块ID
	BYTE bConnected;		//是否接过人体
	BYTE bCommStatus;		//通讯状态
	BOOL bOff;			//脱落状态: TRUE-Off, FALSE-On
	BOOL bInvalPR;		//PR无效值标志
	BYTE bStrength;		//SPO2 棒图(表示脉搏跳动情况),0～15
	WORD wPR;			//脉率	
}S_PULSE_VALUE, *PS_PULSE_VALUE;

//---- TEMP的相关定义 ----
//体温测量范围(C*10)
#define TEMP_MAX	500
#define TEMP_MIN	0
//显示单位
#define TEMP_UNIT_C	0
#define TEMP_UNIT_F		1
//temp sensor type
#define TEMP_YSI		0
#define TEMP_CYF		1
//TEMP配置信息
typedef struct {
	BYTE bID;			//模块ID
	BYTE bSwitch;			//模块开关: SWITCH_OFF， SWITCH_ON
	int iColor;			//颜色
	BYTE bUnit;			//显示单位: TEMP_UNIT_C, TEMP_UNIT_F
	WORD wHigh_T1;			//体温值上限，10x
	WORD wLow_T1;			//体温值下限，10x
	WORD wHigh_T2;			//体温值上限，10x
	WORD wLow_T2;			//体温值下限，10x
	WORD wHigh_TD;			//体温差高限
	BYTE bAlmControl;		//报警控制
	BYTE bPrnControl;		//报警触发打印
	BYTE bTempType;		//体温探头类型设置0-YSI 1-CY-F
	int iAdjust_T1;			//校准系数(+-)
	int iAdjust_T2;			//校准系数(+-)
	int iMax_TEMP;		//短趋势中的标尺最大值
	int iMin_TEMP;		//短趋势中的标尺最小值	
}S_TEMP_CFG, *PS_TEMP_CFG;

//TEMP参数信息
typedef struct {
	BYTE bID;			//模块ID
	BYTE bConnected_T1;		//是否接过人体, FALSE－未接过，TRUE－已经接过
	BYTE bCommStatus_T1;		//通讯状态
	BOOL bOverRange_T1;		//测量超界，FALSE－normal，TRUE－超界
	BOOL bOff_T1;			//传感器状态，FALSE－normal，TRUE－传感器脱落
	BYTE bConnected_T2;		//是否接过人体，FALSE－未接过，TRUE－已经接过
	BYTE bCommStatus_T2;		//通讯状态
	BOOL bOverRange_T2;		//测量超界，FALSE－normal，TRUE－超界
	BOOL bOff_T2;			//传感器状态，FALSE－normal，TRUE－传感器脱落

	WORD wT1;			//体温值，10x
	WORD wT2;			//体温值，10x
	WORD wTD;			//体温值，10x(温差绝对值)
}S_TEMP_VALUE, *PS_TEMP_VALUE;


//---- NIBP的相关定义 ----
//NIBP测量范围(mmHg)
#define NIBP_MAX	300
#define NIBP_MIN	0
//NIBP测量对象
#define NIBP_OBJECT_ADULT	0
#define NIBP_OBJECT_CHILD	1
#define NIBP_OBJECT_BABY	2
//NIBP显示单位
#define NIBP_UNIT_MMHG		0
#define NIBP_UNIT_KPA		1
//NIBP报警来源
#define NIBP_ALM_SRC_ALL	0
#define NIBP_ALM_SRC_S		1
#define NIBP_ALM_SRC_M		2
#define NIBP_ALM_SRC_D		3
#define NIBP_ALM_SRC_SM		4
#define NIBP_ALM_SRC_MD		5
#define NIBP_ALM_SRC_SD		6

/****************CUFF**************************/
#define NIBP_MANUAL				0
#define NIBP_AUTO				1
#define NIBP_STAT				2
#define NIBP_STATIC_VEN			3
#define NIBP_LEAK_TEST			4
#define NIBP_P_V_TEST			5
#define NIBP_OVER_PRE_TESE		6
#define NIBP_PRE_CAL			7

#define NIBP_UPDATE_PATIENT_INFO		1
#define NIBP_AUTO_DETECT_CUFF_TYPE	0
#define NIBP_ADULT_CUFF_TYPE			1
#define NIBP_PEDIATRIC_CUFF_TYPE		2
#define NIBP_NEONATE_CUFF_TYPE		3
/*****************************************/
#define  NIBP_SYSTEM_RUNNING		1
#define  NIBP_SYSTEM_IDLE			0
/******************************************/

/*******************FDB***********************/
#define NBP_FDB_MAN_START_OK		0x01	//
#define NBP_FDB_MAN_START_ABT	0x02	//
#define NBP_FDB_AUTO_START_OK	0x03	//
#define NBP_FDB_AUTO_START_ABT	0x04	//
#define NBP_FDB_STAT_START_OK		0x05	//
#define NBP_FDB_STAT_START_ABT	0x06	//
#define NBP_FDB_STC_START_OK		0x07	//
#define NBP_FDB_STC_START_ABT		0x08	//
#define NBP_FDB_LEAK_START_OK		0x09	//
#define NBP_FDB_LEAK_START_ABT	0x0a	//
#define NBP_FDB_PVT_START_OK		0x0b	//
#define NBP_FDB_PVT_START_ABT		0x0c	//
#define NBP_FDB_OVP_START_OK		0x0d	//
#define NBP_FDB_OVP_START_ABT		0x0e	//
#define NBP_FDB_CAL_START_OK		0x0f	//

#define NBP_FDB_CAL_START_ABT		0x10	
#define NBP_FDB_CAL_EXPRS_ERR	0x11	//NIBP calibration external pressure value Error
#define NBP_FDB_CAL_SMC_OT		0x12	//NIBP calibration Safety MCU init overtime
#define NBP_FDB_CAL_ZERO_OT		0x13	//NIBP calibration zeroing overtime
#define NBP_FDB_CAL_E200_OT		0x14	//NIBP calibration external 200mmHg overtime
#define NBP_FDB_CAL_SPAN_OT		0x15	//NIBP calibration calculate Span overtime
#define NBP_FDB_CAL_SAVE_OT		0x16	//NIBP calibration data save overtime
#define NBP_FDB_CAL_DONE_OK		0x17	//NIBP calibration finished
#define NBP_FDB_CAL_DATA_ERR		0x18	//NIBP calibration data error

#define NBP_FDB_TAG_PRS_ERR		0x20	//NIBP target inflate pressure set ERROR
#define NBP_FDB_PNEU_LEAK			0x21	//NIBP measurement pneumatic leak
#define NBP_FDB_INFL_FAIL			0x22	//NIBP measurement inflation fail ( cuff/hose not connected )
#define NBP_FDB_DEFLT_OT			0x23	//NIBP measurement deflate overtime
#define NBP_FDB_MEASURE_OT		0x24	//NIBP measurement  overtime
#define NBP_FDB_PULSE_WEAK		0x25	//NIBP measurement  pulse signal too weak
#define NBP_FDB_EXCS_PRS			0x26	//NIBP measurement pressure out of range

#define NBP_FDB_SIS_CTI_OT			0x30	//NIBP Static Venipuncture Cuff Type Inflation Overtime
#define NBP_FDB_SIS_CTC_OT			0x31	//NIBP Static Venipuncture Cuff Type Check Overtime
#define NBP_FDB_SIS_CTW_OT			0x32	//NIBP Static Venipuncture Cuff Type Wait Overtime
#define NBP_FDB_SIS_CTD_OT			0x33	//NIBP Static Venipuncture Cuff Type Deflation Overtime
#define NBP_FDB_SIS_CTR_OT			0x34	//NIBP Static Venipuncture Cuff Type Resolution Overtime
#define NBP_FDB_SIS_PRO_OT			0x35	//NIBP Static Venipuncture Process Overtime

#define NBP_FDB_LEAK_YES			0x40	//NIBP Pneumatic Leak
#define NBP_FDB_LEAK_NO			0x41	//NIBP Pneumatic No Leak
#define NBP_FDB_LEAK_PUMPERR		0x42	//NIBP Pneumatic pump error

#define NBP_FDB_OVP_NEO_OK		0x50	//NIBP Neonate /Pediactric Overpressue protection ok
#define NBP_FDB_OVP_NEO_FAIL		0x51	//NIBP Neonate /Pediactric Overpressue protection fail
#define NBP_FDB_OVP_ADU_OK		0x52	//NIBP Adult Overpressue protection ok
#define NBP_FDB_OVP_ADU_FAIL		0x53	//NIBP Adult Overpressue protection fail
#define NBP_FDB_OVP_PNEU_ERR		0x54	//NIBP Overpressue test fail, pneumatic error

#define NBP_FDB_PVT_PUMP_ON		0x60	//NIBP Pump On
#define NBP_FDB_PVT_PUMP_OFF		0x61	//NIBP Pump Off
#define NBP_FDB_PVT_DEF_OPEN		0x62	//NIBP Deflate[Fast] Valve Open
#define NBP_FDB_PVT_DEF_CLOSE		0x63	//NIBP Deflate[Fast] Valve Close
#define NBP_FDB_PVT_DUMP_OPEN	0x64	//NIBP Dump[Slow] Valve Open
#define NBP_FDB_PVT_DUMP_CLOSE	0x65	//NIBP Dump[Slow] Valve Close
#define NBP_FDB_PVT_OT				0x66	//NIBP Pump&Valve Test Overtime

#define NBP_FDB_FAIL_01			0x70
#define NBP_FDB_FAIL_02			0x71
#define NBP_FDB_FAIL_03			0x72
#define NBP_FDB_FAIL_04			0x73
#define NBP_FDB_FAIL_05			0x74
#define NBP_FDB_FAIL_06			0x75
#define NBP_FDB_FAIL_07			0x76
#define NBP_FDB_FAIL_08			0x77
#define NBP_FDB_FAIL_09			0x78
#define NBP_FDB_FAIL_10			0x79
#define NBP_FDB_FAIL_11			0x7a
#define NBP_FDB_FAIL_12			0x7b
#define NBP_FDB_FAIL_13			0x7c
#define NBP_FDB_FAIL_14			0x7d
#define NBP_FDB_FAIL_15			0x7e
#define NBP_FDB_FAIL_16			0x7f

/******************************************/

#define NIBP_STATUS_STOP				0	//未测或测量取消NBP复位完成
#define NIBP_STATUS_MANUAL			1	//正在测量手动测量
#define NIBP_STATUS_AUTO				2	//正在测量自动测量
#define NIBP_STATUS_SERIES				3	//正在测量连续测量
#define NIBP_STATUS_CALIBRATION		4		//正在标定
#define NIBP_STATUS_DETECTION		5		//正在检测(漏气..)
#define NIBP_STATUS_RESET			6		//NBP复位
#define NIBP_STATUS_SYSERROR		10		//系统错误
//NIBP测量类型
#define NIBP_TYPE_MANUAL				1	//在手动测量方式下
#define NIBP_TYPE_AUTO				2		//在自动测量方式下
#define NIBP_TYPE_STAT				3		//在STAT测量方式下
#define NIBP_TYPE_CALIBRATION		4		//在校准方式下
#define NIBP_TYPE_DETECTION			5		//在漏气检测中
#define NIBP_TYPE_SYSERROR			10		//系统错误，具体错误信息见NBP状态包

//NIBP周期测量间隔时间(min)
#define NIBPMODE_MANUAL				0	//手动测量模式 TODO:不用
#define NIBP_INTERVAL_MIN			1
#define NIBP_INTERVAL_MAX			480
//静脉穿刺压限值(初始压力)
#define NIBP_V_PRESSURE_MIN			20
#define NIBP_V_PRESSURE_MAX			120
//NIBP测量方式(NEW)
#define NIBP_MODE_MANUAL			0	//单次
#define NIBP_MODE_AUTO				1	//自动
#define NIBP_MODE_SERIES				2	//连续
#define NIBP_MODE_PUNCTURE			3	//穿刺
//连续测量的时间 秒 5分钟
#define NIBP_STAT_TIME		300	
//连续测量的间隔 秒
#define NIBP_STAT_INTER		5	
//静脉穿刺保持压力的时间 秒
#define NIBP_PUNCTURE_TIME_ADULT	170	
#define NIBP_PUNCTURE_TIME_BABY		80	
//NIBP配置信息
typedef struct {
	BYTE bID;			//模块ID
	BYTE bSwitch;			//模块开关: SWITCH_OFF， SWITCH_ON
	int iColor;				//颜色
	BYTE bUnit;				//显示单位: NIBP_UNIT_MMHG, NIBP_UNIT_KPA
	BYTE bObject;			//测量对象: NIBP_OBJECT_ADULT, NIBP_OBJECT_CHILD, NIBP_OBJECT_BABY
	BYTE bMode;				//测量方式: NIBP_MODE_XXX
	WORD wInterval;			//AUTO测量时的间隔min, NIBP_INTERVAL_MIN~NIBP_INTERVAL_MAX, 为0时手动
	WORD wInitPres;		//初始压力, 
	WORD wVP;			//静脉穿刺压力, NIBP_V_PRESSURE_MIN ~ NIBP_V_PRESSURE_MAX
	WORD wHigh_Sys;			//收缩压上限
	WORD wLow_Sys;			//收缩压下限
	WORD wHigh_Dia;			//舒张压上限
	WORD wLow_Dia;			//舒张压下限
	WORD wHigh_Mean;		//平均压上限
	WORD wLow_Mean;			//平均压下限
	WORD wVPressure;
	BYTE bAlmSource;		//报警来源: NIBP_ALM_SRC_X
	BYTE bAlmControl;		//报警控制
	BYTE bPrnControl;		//报警触发打印
	int iAdjust_Sys;		//校准系数(百分比)
	int iAdjust_Dia;		//校准系数(百分比)
	int iAdjust_Mean;		//校准系数(百分比)
	WORD wMax_NIBP;	//短趋势中的标尺最大值
	WORD wMin_NIBP;		//短趋势中的标尺最小值
}S_NIBP_CFG, *PS_NIBP_CFG;
//NIBP参数信息
//NIBP测量信息(包括出错) 
#define NIBP_INFO_EXCESSIVE_MOTION	BIT0		//运动过激
#define NIBP_INFO_CUFF_MISTAKE		BIT1		//袖带类型错
#define NIBP_INFO_CUFF_TOOLAX		BIT2		//袖带太松
#define NIBP_INFO_WEAK_SIGNAL		BIT3		//微弱信号
#define NIBP_INFO_INTERFERENCE		BIT4		//干扰太大
#define NIBP_INFO_OVER_RANGE		BIT5		//测量超出范围
#define NIBP_INFO_OVERPRESSURE		BIT6		//过压保护
#define NIBP_INFO_TIMEOUT			BIT7		//超时
#define NIBP_INFO_ERROR				BIT8		//测量出错
#define NIBP_INFO_SIGNAL_SATURATION	BIT9		//信号饱和
#define NIBP_INFO_MODULE_FAULT		BIT10		//模块故障
//(new)hejp
#define NIBP_ERR_CUFF_TOOLAX			BIT0		//袖带过松 
#define NIBP_ERR_CUFF_LEAK				BIT1		//漏气 
#define NIBP_ERR_PRESSURE				BIT2		//气压错误 
#define NIBP_ERR_WEAK_SIGNAL			BIT3		//微弱信号
#define NIBP_ERR_OVER_RANGE			BIT4		//超范围
#define NIBP_ERR_EXCESSIVE_MOTION	BIT5		//过分运动
#define NIBP_ERR_OVERPRESSURE			BIT6		//过压 
#define NIBP_ERR_SIGNAL_SATURATION	BIT7		//信号饱和 
#define NIBP_ERR_CHECKLEAK				BIT8		//漏气检测失败
#define NIBP_ERR_MODULE_FAULT			BIT9		//系统错误 
#define NIBP_ERR_TIMEOUT				BIT10	//超时 
#define NIBP_ERR_CUFF_MISTAKE			BIT11	//袖带类型错

typedef struct {
	BYTE bID;			//模块ID
	BYTE bConnected;	//是否接过人体
	BYTE bObject;		//病人信息00-成人，01-儿童，02-新生儿
	BYTE bStatus;		//工作状态: NIBP_STATUS_XX
	BYTE bEndType;		//测量类型结束:NIBP_STATUS_XX
	BYTE bMeasureType;		//测量类型:NIBP_STATUS_XX
	BOOL bMeasureOK;	//测量完成的标志, TRUE-OK
	BOOL bCanceled;		//是否被取消,TRUE:yes	
	WORD wSys;			//收缩压
	WORD wDia;			//舒张压
	WORD wMean;			//平均压
	WORD wPR;			//脉率
	WORD wPressure;		//袖带压
	BOOL bInvalPre; 	//袖带压无效值标志
	S_TIME sTime;		//测量时间
	WORD wRemTime;		//剩余时间:s秒
	WORD bPeriod;			//测量周期
	WORD wCalPreValue;
	//Error
	BOOL bComError;		//通讯故障	
	BOOL bComStop;		//通讯停止
	BOOL bModuleError;	//模块错误
	BOOL bLimitError_Sys;	//报警限错误
	BOOL bLimitError_Dia;	//报警限错误
	BOOL bLimitError_Mean;	//报警限错误
	BOOL bOverrange;		//测量超界
	unsigned int  wFDB;		//测量信息  NIBP_INFO_XX
	unsigned int  wErrCode;		//测量信息  NIBP_INFO_XX
}S_NIBP_VALUE, *PS_NIBP_VALUE;

//---- IBP的相关定义 ----
//测量范围(mmHg)
#define IBP_MAX		400
#define IBP_MIN		-100
//显示单位
#define IBPUNIT_MMHG	0
#define IBPUNIT_KPA	1
#define IBPUNIT_CMH2O	2
//标名
#define	IBPLAB_ART		0	//动脉血压
#define 	IBPLAB_PA		1	//肺泡压	
#define	IBPLAB_CVP		2	//中心静脉压
#define	IBPLAB_LAP		3	//左房压
#define	IBPLAB_RAP		4	//右房压 
#define	IBPLAB_ICP		5	//颅内压
#define	IBPLAB_P1		6
#define	IBPLAB_P2		7
#define 	IBPLAB_AO		8  	//主动脉压
#define	IBPLAB_UAP		9	//脐动脉压
#define	IBPLAB_BAP		10	//肱动脉压
#define	IBPLAB_FAP		11	//股动脉压
#define	IBPLAB_UVP		12	//脐静脉压

//参数显示方式
#define 	IBPVIEW_ALL	0
#define 	IBPVIEW_M	  	1
//报警源
#define IBP_ALM_SRC_ALL		0
#define IBP_ALM_SRC_S		1
#define IBP_ALM_SRC_M		2
#define IBP_ALM_SRC_D		3
#define IBP_ALM_SRC_SM		4
#define IBP_ALM_SRC_MD		5
#define IBP_ALM_SRC_SD		6
//测量状态
#define IBP_STATUS_NORMAL	0
#define IBP_STATUS_ZERO		1	//正在校准
//滤波模式
#define IBP_FILTER_SMOOTH	0	//平滑滤波
#define IBP_FILTER_NORMAL	1	//正常滤波

//IBP配置信息
typedef struct {
	BYTE bID;					//模块ID
	BYTE bSwitch;				//模块开关: SWITCH_OFF， SWITCH_ON
	int iColor;					//颜色
	BYTE bUnit;					//显示单位: IBPUNIT_MMHG, IBPUNIT_KPA, IBPUNIT_CMH2O
	BYTE bLabel;				//选择监护压力标名: IBPLAB_XX
	BYTE bViewMode;				//参数显示模式: IBPVIEW_ALL, IBPVIEW_M
	BYTE bScanSpeed;			//扫描速度: SCANSPEED_125, SCANSPEED_25, SCANSPEED_50
	int iScaleTop;				//压力波形显示刻度高限，高低限最少相差10mmHg
	int iScaleBottom;			//压力波形显示刻度低限	
	int iScaleMiddle;			//压力波形显示刻度中线
	int iHigh_Sys;				//收缩压上限
	int iLow_Sys;				//收缩压下限
	int iHigh_Dia;				//舒张压上限
	int iLow_Dia;				//舒张压下限
	int iHigh_Mean;				//平均压上限
	int iLow_Mean;				//平均压下限
	int iCheckValue;			//校准值，为相对值0， 从模块采集的数值减去该值，才是正确的值
	BYTE bAlmSource;			//报警源: IBP_ALM_SRC_XX, 当显示格式为Mean时，报警源亦固定为Mean
	BYTE bAlmControl;			//报警控制，0－关闭，1－LOW，2－MID，3－HIGH
	BYTE bPrnControl;			//报警触发打印，0－关闭，1－打开	
	BYTE bFilterMode;			//IBP滤波模式IBP_FILTER_SMOOTH，IBP_FILTER_NORMAL
}S_IBP_CFG, *PS_IBP_CFG;
//IBP参数信息
typedef struct {
	BYTE bID;		//模块ID
	BYTE bConnected;	//是否接过人体
	BYTE bCommStatus;	//通讯状态
	WORD wHR;		//心率值
	BOOL bOff;		//探头状态，FALSE－normal，TRUE－脱落
	BYTE bStatus;		//测量状态，IBP_STATUS_NORMAL, IBP_STATUS_ZERO
	BYTE bCalResult;	//校准结果
	BYTE bZeroResult;  	//校零结果
	S_TIME sCalTime;		//测量时间
	int iSys;		//收缩压
	int iDia;		//舒张压
	int iMean;		//平均压
	int iWave;		//压力波形
	BYTE Calviewmode; //改变面板显示的是校零还是校准
}S_IBP_VALUE, *PS_IBP_VALUE;

//---- CO2的相关定义 ----
//CO2测量范围(mmHg)
#define CO2_MAX		99
#define CO2_MIN		0
//显示单位
#define CO2UNIT_MMHG		0
#define CO2UNIT_PER		1
#define CO2UNIT_KPA		2
//采样流速
#define CO2RATE_50		0
#define CO2RATE_75		1
#define CO2RATE_100		2
#define CO2RATE_150		3
#define CO2RATE_200		4
//工作状态
#define CO2_STATUS_STOP		0	//待命
#define CO2_STATUS_RUN		1	//运行	
//波形显示幅度
#define CO2_RANGE_BIG		0	//大，0~50mmHg
#define CO2_RANGE_MIDDLE	1	//中，0~75mmHg
#define CO2_RANGE_SMALL		2	//小，0~CO2_MAX
//CO2配置信息
typedef struct {
	BYTE bID;		//模块ID
	BYTE bSwitch;		//模块开关: SWITCH_OFF， SWITCH_ON
	int iColor;		//颜色
	BYTE bUnit;		//显示单位: CO2UNIT_MMHG, CO2UNIT_PER, CO2UNIT_KPA
	BYTE bScanSpeed;	//扫描速度: SCANSPEED_625, SCANSPEED_125, SCANSPEED_25
	int iScaleTop;		//波形上限: 波形的上标尺(mmHg)
	BYTE bRate;		//气体采样流速: CO2RATE_XX
	BYTE bRange;		//波形显示范围: CO2_RANGE_BIG, CO2_RANGE_MIDDLE, CO2_RANGE_SMALL
	int iCaliValue;		//校准值
	WORD wHigh_EtCO2;	//EtCo2上限
	WORD wLow_EtCO2;	//EtCo2下限
	WORD wHigh_FiCO2;	//FiCo2上限
	WORD wLow_FiCO2;	//FiCo2下限	(不使用)
	WORD wHigh_awRR;	//awRR上限	(不使用)
	WORD wLow_awRR;		//awRR下限	(不使用)
	WORD wApneaTime;	//窒息报警时间1~60s	(不使用)
	BYTE bAlmControl;	//报警控制
	BYTE bPrnControl;	//报警触发打印
}S_CO2_CFG, *PS_CO2_CFG;
//CO2参数信息
typedef struct {
	BYTE bID;		//模块ID
	BYTE bConnected;	//是否接过人体
	BYTE bCommStatus;	//通讯状态
	BYTE bStatus;		//工作状态: CO2_STATUS_STOP, CO2_STATUS_RUN
	BOOL bOff;		//传感器状态，FALSE－normal，TRUE－脱落
	BYTE bRate;		//气体采样流速: CO2RATE_XX
	WORD wAwRR;		//气道呼吸率
	WORD wEtCO2;		//呼末二氧化碳
	WORD wFiCO2;		//吸入二氧化碳
	WORD wInsCO2;		//瞬时二氧化碳(校准时使用)
	WORD wWave;		//CO2波形
	WORD wBaro;		//大气压力   (CO2％ ＝ (CO2(mmHg)*100 / wBaro) )
}S_CO2_VALUE, *PS_CO2_VALUE;

//---- 病人的相关定义 ----
//姓名字串的范围
#define PATIENT_NAME_MAX	9
#define PATIENT_DATE_MAX	9
#define PATIENT_BED_MAX		PATIENT_NAME_MAX
#define PATIENT_ROOM_MAX	PATIENT_NAME_MAX
#define PATIENT_ID_MAX		PATIENT_NAME_MAX
#define PATIENT_BIRTH_MAX	9
//病人类型
#define ADULT	0
#define CHILD	1
#define BABY		2
//病人性别
#define MALE		0
#define FEMALE	1
//年龄
#define AGE_MIN		0
#define AGE_MAX		150
//身高(cm)
#define HEIGHT_MIN	10
#define HEIGHT_MAX	250
//体重(kg)
#define WEIGHT_MIN	1
#define WEIGHT_MAX	200
//血型
#define BLOOD_A			0
#define BLOOD_B			1
#define BLOOD_O			2
#define BLOOD_AB		3
#define BLOOD_UNKNOW	4

typedef struct {

	BYTE sRoom[PATIENT_ROOM_MAX];	//科室房间号
	BYTE sID[PATIENT_ID_MAX];			//病人ID
	BYTE sBed[PATIENT_BED_MAX];		//床位	
	BYTE sDoctor[PATIENT_NAME_MAX];	//医生	
	BYTE sName[PATIENT_NAME_MAX];		//姓名
	BYTE bSex;							//性别: MALE, FEMALE	
	BYTE bObject;						//病人类型: ADULT, CHILD, BABY
	BYTE sAdmit[PATIENT_BIRTH_MAX];	//入院日期
	BYTE sBirth[PATIENT_BIRTH_MAX];		//出生日
	BYTE bAge;							//年龄
	WORD wHeight;						//身高cm
	WORD wWeight;						//体重Kg
	BYTE bBlood;						//血型	0A, 1:B, 2:O, 3:AB, 4 :N/A
	BYTE bPacemaker;					//是否起搏: PACEMAKER_OFF or PACEMAKER_ON	
	
}S_PATIENT_CFG, *PS_PATIENT_CFG;

//---- 系统的相关定义 ----
//系统配置文件
#define SETUP_RECENT			0
#define SETUP_ADULT			1
#define SETUP_CHILD			2
#define SETUP_BABY			3
#define SETUP_USER1			4
#define SETUP_USER2			5
#define SETUP_USER3			6
//系统语言
#define LANGUAGE_ENGLISH		0
//#define LANGUAGE_OTHER		1
#define LANGUAGE_CHINESE		1
#define LANGUAGE_CHT			2
#define LANGUAGE_TURKISH		3
#define LANGUAGE_RUSSIAN		4
#define LANGUAGE_ROMANIA		5
#define LANGUAGE_SPANISH		6
#define LANGUAGE_ITALIAN		7



//工作界面
#define SCREEN_NORMAL			0
#define SCREEN_NIBPLIST			1
#define SCREEN_BIGCHAR			2
#define SCREEN_SHORTTREND		3
#define SCREEN_7LEADECG		4
#define SCREEN_MULTIECG		5
#define SCREEN_OXYCRG			6
#define SCREEN_OTHERBED		7
#define SCREEN_IBP				8
#define SCREEN_12LEAD			9

//键盘发声
#define KEYVOLUME_OFF	0
#define KEYVOLUME_ON	1
//ECG导联定义
#define ECGLEAD_AHA		0
#define ECGLEAD_IEC		1
//液晶类型
#define LCDTYPE_800_104		0	//800x600-10.4"
#define LCDTYPE_800_121		1	//800x600-12.1"
#define LCDTYPE_1024_121	2	//1024x768-12.1"
#define LCDTYPE_1024_15		3	//1024x768-15"
//网络类型
#define NETTYPE_ETH			0
#define NETTYPE_485			1
//功能键定义
#define FUNCBT_EXIT			0	//退回主屏幕
#define FUNCBT_SWITCHVIEW		1	//切换视图
//线段的绘制方式
#define LINE_WIDE			0	//宽线条(multi Pixels)
#define LINE_NORMAL			1	//正常线条(1Pixel)
#define LINE_ANTI			2	//反走样线条

//系统配置信息
typedef struct {
	BYTE bLanguage;			//系统语言: 0-English, 1-Other
	BYTE bInterface;		//工作界面
	BYTE bKeyVolume;		//键盘操作发声: KEYVOLUME_OFF, KEYVOLUME_ON
	BYTE bACFreq;			//交流频率: AC_50HZ, AC_60HZ
	BYTE bECGLeadDef;		//ECG导联定义: ECGLEAD_AHA, ECGLEAD_IEC
	BYTE bLCDType;			//液晶类型: LCDTYPE_121, LCDTYPE_104
	BYTE bNetType;			//网络类型: NETTYPE_ETH, NETTYPE_485
	BYTE bShortTrendScale;		//短趋势时间,0-30min, 1-60min, 2-120Min, other=120Min
	BYTE bOxyCrgScale;		//OxyCrg时间, 0-1Min, 1-2Min, 2-5Min, 3-10Min, 4-15Min, 5-30Min,other-30Min  
	BYTE bFuncButton;		//功能键定义
	BYTE bLineType;			//线段的绘制方式
	BYTE bWDTEnable;		//启用看门狗
}S_SYSTEM_CFG, *PS_SYSTEM_CFG;


#define ALARMPAUSE_1MIN		0
#define ALARMPAUSE_2MIN		1
#define ALARMPAUSE_3MIN		2
//报警功能配置
typedef struct {
	BYTE bVolume;			//0:Off, 1~10
	BYTE bPrint;			//SWITCH_OFF, SWITCH_ON
	BYTE bPauseTime;		//暂停时间:ALARMPAUSE_xMIN
	BOOL bViewLimit;		//是否显示报警上下限
	BYTE bPrintWaveLength;		//记录的波形长度， 4、8、16秒, 不超过 MAXALARMWAVELENGTH
}S_ALARM_CFG, *PS_ALARM_CFG;

//一屏可容纳的最多面板
#define WAVEPANEL_MAX_COUNT	10

//波形类型标识
#define WID_NONE			0
#define WID_ECG1			1
#define WID_ECG2			2
#define WID_SPO2				3
#define WID_RESP			4
#define WID_IBP1				5
#define WID_IBP2				6
#define WID_CO2				7
#define WID_MULTIECG		8
#define WID_MAX		WID_MULTIECG

//单个波形面板配置,用户配置
typedef struct {
	BYTE  bID;		//波形类型标识
	BOOL bSwitch;		//显示与否的开关 SWITCH_OFF, SWITHC_ON 
}WAVE_PANEL_CFG, *PWAVE_PANEL_CFG;

//-------------- 数据处理的数据结构 ------------------
/*
循环数组的结构
typedef struct {
	WORD wCount;	
	WORD wHeadIndex;	//循环数组首指针
	WORD wTailIndex;	//循环数组尾指针
	BOOL bLoop;		//数组翻转标记
	int iValue[MAXVALUECOUNT];
}CA_XX;
*/

//---- 趋势存储的定义 ----
//定义一些宏来表示趋势的信息，表示其中的某个模块的信息是否有效,如果某个模块脱落了，其信息也是无效的
//DWORD       dwHaveFlag;
#define HAVEHR		0x00000001
#define HAVESPO2	0x00000002
#define HAVENIBP	0x00000004
#define HAVEPR		0x00000008
#define HAVERESP	0x00000010
#define HAVET1		0x00000020
#define HAVET2		0x00000040
#define HAVEST		0x00000080
#define HAVECO2		0x00000100
#define HAVEIBP1		0x00000200
#define HAVEIBP2		0x00000400
#define HAVEAA		0x00000800

//定义趋势记录原因的标记 
//BYTE        bLogFlag
#define TRENDAUTO	0x01    //周期自动记录
#define TRENDNIBP	0x02    //NIBP测量记录
#define TRENDMARK	0x04    //用户标记的事件
#define TRENDALARM	0x08    //报警

//NIBP 趋势数据结构
typedef struct{
	WORD wSys;
	WORD wMean;
	WORD wDia;
}NIBPDATA, *PNIBPDATA;
//IBP 趋势数据结构
typedef struct{
	int iSys;
	int iMean;
	int iDia;
}IBPDATA, *PIBPDATA;
//CO2 趋势数据结构
typedef struct {
	WORD wEtCO2;	
	WORD wFiCO2;
	WORD wAwRR;	
}CO2DATA, *PCO2DATA;
//同类型参数的数量
#define ST_COUNT	2
#define TEMP_COUNT	2
#define IBP_COUNT	2

//单条趋势数据	
typedef struct {
	long int	liOffset;	//与趋势存储起始时间的偏移量,单位：秒, 用来修正存储时间
	S_TIME		sTime;		//存储趋势数据的时间(根据偏移量修正)
	BYTE      	bLogFlag; 	//记录原因 标记，NIBP测量，报警，周期
	DWORD		dwHaveFlag;		//记录趋势中是否有数  HR ST1 ST2 ST3 NIBP SPO2 PULSE T1 T2 CO2 IBP1 IBP2 
	int		wHR;
	int		wPR;
	int		wRR;    
	short int	iSTs[ST_COUNT];
	BYTE		bSpO2;
	NIBPDATA	sNIBP;
	IBPDATA		sIBPs[IBP_COUNT];
	WORD		wTemps[TEMP_COUNT]; 
	CO2DATA		sCO2;
	//TODO:此处以后还要添加AA等参数
}TRENDDATA, *PTRENDDATA;

//定义存储波形的类型, 考虑到Resp Spo2以及其他波形不同步, 所以设置如下标志
#define REALSAVE_ECG	0
#define REALSAVE_SPO2	1
#define REALSAVE_RESP	2
//欲存储的波形, 8位数据
typedef struct {
	//对于ECG波形,区分3/5导联,5导联时:Ecg1-I, Ecg2-II, Ecg3-V, 三导联时: Ecg1-I, Ecg2--II, Ecg3-III
	int bEcg1;
	int bEcg2;
	int bEcg3;
	BYTE bSpO2;
	BYTE bResp;
	BYTE bCO2;
	int iIbp1;
	int iIbp2;
}TRENDWAVE, *PTRENDWAVE;

//短趋势数据连续最大存储数量,单位:分钟
#define MAXSHORTTREND		120	///2小时	
//短趋势数据数组
typedef struct {
	WORD		wHead;	//循环数组首指针
	BOOL		bFull;		//数组翻转标记
	TRENDDATA	Datas[MAXSHORTTREND];		//短趋势数据
}CA_SHORTTREND, *PCA_SHORTTREND;


//趋势数据连续最大存储数量,单位:分钟
#define MAXTRENDCOUNT		7200	//5天	
//趋势数据数组
typedef struct {
	S_PATIENT_CFG	sPatientInfo;	//病人信息	
	S_TIME		sStartTime;	//开始存储趋势数据的时间
	WORD		wHeadIndex;	//循环数组首指针
	WORD		wTailIndex;	//循环数组尾指针
	BOOL		bLoop;		//数组翻转标记
	WORD		wCount;		//实际存储的数据
	TRENDDATA	Datas[MAXTRENDCOUNT];		//趋势数据
}CA_TRENDARRAY, *PCA_TRENDARRAY;


//实时存储的单条数据,包括波形,以1秒钟为单位
#define SAMPLING_RATE	200		//采样率
typedef struct {
	TRENDDATA	paras;
	TRENDWAVE	waves[SAMPLING_RATE];
}REALSAVEDATA, *PREALSAVEDATA;

//实时数据循环数组,单位 秒
#define MAXREALSAVECOUNT	60
typedef struct {
	S_TIME 			sStartTime;			//开始存储数据的时间
	WORD 			wHeadIndex;			//循环数组首指针
	WORD 			wTailIndex;			//循环数组尾指针
	BOOL 				bLoop;			//数组翻转标记
	WORD 			wCount;				//实际存储的数据
	REALSAVEDATA		Datas[MAXREALSAVECOUNT];	//趋势数据
}CA_REALSAVEARRAY, *PCA_REALSAVEARRAY;


//是否存储实时ECG数据
// #define SAVEREALECG				1

//实时ECG波形(只存一道,Channel1)
typedef struct {
	BYTE bLead;				//导联号
	BYTE bWave[SAMPLING_RATE];		//波形
}REALECGDATA, *PREALECGDATA;

//实时ECG波形存储, 单位: 秒
#define MAXREALECGCOUNT	3600
typedef struct {
	WORD 			wHeadIndex;				//循环数组首指针
	WORD 			wTailIndex;				//循环数组尾指针
	BOOL 			bLoop;					//数组翻转标记
	WORD 			wCount;					//实际存储的数据
	REALECGDATA		Datas[MAXREALECGCOUNT];
}CA_REALECGARRAY, *PCA_REALECGARRAY;

//报警数据实时存储缓冲 second
#define MAXREALALARMSAVECOUNT	33		
typedef struct {
	S_TIME 		sStartTime;				//开始存储数据的时间
	WORD 			wHeadIndex;			//循环数组首指针
	WORD 			wTailIndex;			//循环数组尾指针
	BOOL 			bLoop;				//数组翻转标记
	WORD 			wCount;				//实际存储的数据
	REALSAVEDATA		Datas[MAXREALALARMSAVECOUNT];	//趋势数据
}CA_REALALMSAVEARRAY, *PCA_REALALMSAVEARRAY;


//报警时波形的存储的时间 秒
#define MAXALARMWAVELENGTH	MAXREALALARMSAVECOUNT		//前后16秒
//单条报警数据(数据加波形)
typedef struct {
	BYTE 		bAlmType;				//报警类型, ALARM_TECHNOLOGY, ALARM_PHYSIOLOGICAL	
	BYTE 		bAlmID;					//报警ID, (定义在AlarmManger.h)
	BYTE 		bAlmClass;       			//报警级别, ALARM_OFF, ALARM_LOW, ALARM_MID, ALARM_HIGH, ALARM_INFO	
	TRENDDATA	paras;					//报警时刻的数据
	TRENDWAVE	waves[MAXALARMWAVELENGTH][SAMPLING_RATE];           //报警时刻前后X秒的波形
}ALARMDATA, *PALARMDATA;

//报警存储的条数, 单位:条
//#define MAXALARMDATACOUNT			150	//60
#define MAXALARMDATACOUNT			60
typedef struct {
	WORD 			wHeadIndex;			//循环数组首指针
	WORD 			wTailIndex;			//循环数组尾指针
	BOOL 			bLoop;				//数组翻转标记
	WORD 			wCount;				//实际存储的数据
	ALARMDATA		Datas[MAXALARMDATACOUNT];	//报警数据
}CA_ALMDATAARRAY, *PCA_ALMDATAARRAY;


//系统电源状态
#define POWER_SHUTDOWN		0
#define POWER_WORKING			1
#define POWER_DC				0
#define POWER_AC				1
#define POWER_NOT_CHARGE		0
#define POWER_CHARGING		1
#define POWER_NOBAT			0
#define POWER_HAVEBAT			1
#define BATTERY_EMPTY			0
#define BATTERY_Q1				1
#define BATTERY_Q2				2
#define BATTERY_Q3				3
#define BATTERY_Q4				4
#define BATTERY_FULL			5
typedef struct {
	BYTE  bShutDown;
	BYTE  bPowerType;
	BYTE  bHaveBat;
	BYTE  bCharge;
	BYTE  bBatteryStatus;
	int iSoft_Rev;
	int iHard_Rev;
}POWER_STATUS, *PPOWER_STATUS;


//---- 串口－>绘图缓冲区 ----
//串口数据缓冲队列长度
#define QUEUE_MAX_SERIAL	500	//200

//-- ECG 缓冲队列 --
typedef struct {
	int head;
	int tail;
	S_ECG_VALUE values[QUEUE_MAX_SERIAL+1];
}QUEUE_ECG;
//-- RESP 缓冲队列 --
typedef struct {
	int head;
	int tail;
	S_RESP_VALUE values[QUEUE_MAX_SERIAL+1];
}QUEUE_RESP;	
//-- Spo2 缓冲队列 --
typedef struct {
	int head;         
	int tail;
	S_SPO2_VALUE values[QUEUE_MAX_SERIAL+1];
}QUEUE_SPO2;		
//CO2 缓冲队列
typedef struct {
	int head;
	int tail;
	S_CO2_VALUE values[QUEUE_MAX_SERIAL+1];
}QUEUE_CO2;
//IBP 缓冲队列
typedef struct {
	int head;
	int tail;
	S_IBP_VALUE values[QUEUE_MAX_SERIAL+1];
}QUEUE_IBP;

//密码输入类型和密码
//演示功能
#define PASSTYPE_DEMO		0		
#define PASS_DEMO		2888
//用户维护                                 	
#define PASSTYPE_USER		1
#define PASS_USER		2899    
//厂家维护
#define PASSTYPE_FACTORY		2
#define PASS_FACTORY		125689
//机器维护
#define PASSTYPE_MACHINE		3
#define PASS_MACHINE		1789

//打印机的工作状态
#define PRNSTATUS_ERR			0		//没有打印机，或者故障
#define PRNSTATUS_IDLE			1		//打印机空闲
#define PRNSTATUS_PRINTING		2		//打印机正在打印
#define PRNSTATUS_NOPAPER		3		//打印机缺纸     (暂时没用)

//New-打印机状态
#define PRINT_STATUS_NOPRN		0		//没有打印机，或者故障
#define PRINT_STATUS_BUSY		1		//打印机忙碌，正在打印
#define PRINT_STATUS_ASK		2		//打印机空闲，可以打印
#define PRINT_STATUS_ERR		3		//打印机错误，过热或无纸
#define PRINT_STATUS_NOERR		4		//打印机状态正常，可以打印

//打印机正在进行的工作类型
#define PRNWORK_NONE		0		//没有打印任务
#define PRNWORK_REAL		1		//正在进行实时打印
#define PRNWORK_COPY		2		//正在进行拷屏打印
#define PRNWORK_OTHER		3		//正在进行记录打印


//new打印机打印线程中打印内容
#define PRN_WORK_NONE		0		//没有打印内容
#define PRN_WORK_REAL		1		//实时波形信息打印
#define PRN_WORK_ALM		2		//报警回顾打印
#define PRN_WORK_TREND	3		//趋势列表打印
#define PRN_WORK_NIBP		4		//nibp列表打印
#define PRN_WORK_ALMPRINT		5		//nibp列表打印
//打印速度
#define	PRINTSPEED_125		0
#define	PRINTSPEED_25		1
#define	PRINTSPEED_50		2

//define print recorder text print macro
#define MAX_ROW_COUNT				14  			//最多打印字串的行数
#define MAX_TEXT_LENGTH				200		//字串的长度

//最大持续时间 秒
#define REALPRINTTIME_MAX	60
//最大自动记录间隔 分
#define REALPRINTAUTO_MAX	480

//打印机配置
typedef struct {
	BYTE  bWave1;		//WID_xxx
	BYTE  bWave2;
	BYTE  bWave3;		
	BYTE bSpeed;		//PRINTSPEED_xx		
	int  iDuration;		//持续时间  0 - 连续， other-单位：秒
	int  iInter;		//自动实时记录的间隔时间, 0-关闭	 
}S_PRINT_CFG, *PS_PRINT_CFG;

//网络配置
typedef struct {
	WORD	wBedNO;
	unsigned char	cIP[16];
	unsigned char 	cMask[16];
	unsigned char  cGW[16];
}S_NET_CFG, *PS_NET_CFG;

//列表框选择波形
typedef struct {
	BYTE bID;	 //当前选中的ID号
	unsigned char *strIndex[15];//当前列表框显示的选项
	BYTE Count; //选项长度
}NWAVEFORM_COB, *PNWAVEFORM_COB;
NWAVEFORM_COB	WaveformCob[10];

//波形公共选择结构体
typedef struct {
	BYTE bID;	 
	unsigned char *strWaveform;//选择波形名称
	BOOL bSwitch; //是否选择
}NWAVEFORM_CFG, *PNWAVEFORM_CFG;
NWAVEFORM_CFG tmpWaveform[15];

//New 系统状态
typedef struct {
	BYTE bState33V;	 ///3.3v电压状态0-正常 1-太高 2-太低
	BYTE bState5V;	 	///5v电压状态0-正常 1-太高 2-太低
	BYTE bSelWatchdog;	//watchdog自检结果0-自检正确1-自检错误
	BYTE bSelAD;		//A/D自检结果0-自检正确1-自检错误
	BYTE bSelRAM;		//RAM自检结果0-自检正确1-自检错误
	BYTE bSelROM;		//ROM自检结果0-自检正确1-自检错误
	BYTE bSelCPU;		//CPU自检结果0-自检正确1-自检错误
	BYTE bSelIBP;		//IBP自检结果0-自检正确1-自检错误
	BYTE bSelNBP;		//NBP自检结果0-自检正确1-自检错误
	BYTE bSelSPO2;		//SPO2自检结果0-自检正确1-自检错误
	BYTE bSelTEMP;		//TEMP自检结果0-自检正确1-自检错误
	BYTE bSelRESP;		//RESP自检结果0-自检正确1-自检错误
	BYTE bSelECG;		//ECG自检结果0-自检正确1-自检错误
	
	WORD wVersion;		//版本号
	WORD wModuleSta1;		//模块标示符1
	WORD wModuleSta2;		//模块标示符2
	WORD wModuleSta3;		//模块标示符3
	
	WORD wModuleSV;
	WORD wModuleHV;
	unsigned long lModuleUnitID;
	unsigned long  wModuleSN;
	WORD wPowerSV;
	WORD wPowerHV;	
}SYS_STATUS, *PSYS_STATUS;
SYS_STATUS SystemStatus;

//趋势图配置
typedef struct {
	BYTE  bPara1;		//趋势图1
	BYTE  bPara2;
	BYTE  bPara3;		
	BYTE  bScale;		//间隔时间0~9:1h 2h 4h 6h 8h  10h 12h 24h 48h 72h	
	 
}TRENDGRAPH_CFG, *P_TRENDGRAPH_CFG;
TRENDGRAPH_CFG TrendGraphCfg;

#define MAX_OB_IP		 250

//它床数据
typedef struct {
	char IPaddr[16]; 	//IP地址
	BYTE BedNo;		//床号	
	BYTE MacNo;		//mac机器码	
	BOOL FLAG;		//广播包flag
	BYTE Status;		//连接状态flag
	int timeout;		//检查连接状态计数器
}S_OB_INFO, *PS_OB_INFO;

S_OB_INFO OB_Info[MAX_OB_IP];


#endif	//__DATASTRUCT_H__

//网络通信规约
#define NMACHINE_CENTER 		0x01 	//中央机
#define NMACHINE_MULTI_PARA 	0x02	//多参数监护仪
#define NMACHINE_MATERNAL 		0x03	//母婴监护仪
#define NMACHINE_FETAL 			0x04	//胎儿监护仪
#define NMACHINE_DEFIBRILLATE	0x05	//胎儿监护仪
#define NMACHINE_OTHER			0x06	//预留设备

#define NCMD_BROADCASE		0x00	//广播报文
#define NCMD_ASK_PACK			0x01	//请求数据报文主->从
#define NCMD_ANSWER_PACK		0x01	//应答请求数据报文
#define NCMD_DATA_PACK		0x02	//数据报文
#define NCMD_STOP_COMM		0x02	//终止通信
#define NCMD_ASK_RESEND		0x03	//请求数据重发
#define NCMD_NO_RESENDDATA	0x03	//要求重发的报文不存在 
#define NCMD_ASK_NIBP			0x04	//启动从机的血压测量
#define NCMD_ANSWER_NIBP		0x04	//应答主机的血压测量命令
#define NCMD_ASK_CFG			0x05	//请求从机的参数配置信息
#define NCMD_SET_PATINFO		0x05	//设置从机的病人信息
#define NCMD_SET_CFG			0x05	//设置从机的配置参数信息 
#define NCMD_UPLOAD_PATINFO	0x05	//上传病人信息至中央机 
#define NCMD_UPLOAD_CFG		0x05	//上传配置参数信息至中央机 
#define NCMD_CHANGE_MODE		0x06	//监护模式切换
#define NCMD_CHANGE_PAT		0x07	//更换病人
#define NCMD_ALARM				0x10	//报警

#define NCMD_OB_STOP_COMM	0x08	//它床停止通讯
#define NCMD_OB_ANSWER_PACK	0x09	//它床连接

#define NMODULE_ECG			0x01	//ECG 模块
#define NMODULE_SPO2			0x02	//SPO2 模块
#define NMODULE_RESP			0x03	//RESP 模块
#define NMODULE_NIBP			0x04	//NIBP 模块
#define NMODULE_TEMP1			0x05	//TEMP1 模块
#define NMODULE_TEMP2			0x06	//TEMP2模块
#define NMODULE_CO2			0x07	//CO2 模块
#define NMODULE_IBP1			0x08	//IBP1 模块
#define NMODULE_IBP2			0x09	//IBP2 模块
#define NMODULE_ARR			0x0A	//ARR 模块

//0x05 命令参数编号
#define NET_PAT_ALL 				0		//所有病人信息		
#define NET_PAT_DEPT 			1		//科室房间	
#define NET_PAT_ID	 			2		//病人编号
#define NET_PAT_BEDNO 			3		//病人床号
#define NET_PAT_DOCTOR 		4		//主治医生
#define NET_PAT_NAME			5		//病人姓名
#define NET_PAT_SEX 			6		//性别
#define NET_PAT_TYPE 			7		//类型
#define NET_PAT_ADMIT 			8		//入院接收日期
#define NET_PAT_BIRTH 			9		//出生日期
#define NET_PAT_HEIGHT 			10		//身高
#define NET_PAT_WEIGHT 			11		//体重
#define NET_PAT_BLOOD 			12		//血型

#define NET_ECG_ALL				100		//所有心电参数信息
#define NET_ECG_ALMLEV			101		//报警等级
#define NET_ECG_HR_HI			102		//HR 报警上限
#define NET_ECG_HR_LOW		103		//HR 报警下限
#define NET_ECG_LEADTYPE		104		//导联类型
#define NET_ECG_MONTYPE		105		//监护模式
		
#define NET_SPO2_ALL			200		//所有血氧参数信息
#define NET_SPO2_ALMLEV		201		//报警等级
#define NET_SPO2_HI				202		//spo2 报警上限
#define NET_SPO2_LOW			203		//spo2 报警下限
#define NET_SPO2_PR_HI			204		//PR 报警上限
#define NET_SPO2_PR_LOW		205		//PR 报警下限
	
#define NET_RESP_ALL			300		//所有呼吸参数
#define NET_RESP_ALMLEV		301		//报警等级
#define NET_RESP_RR_HI			302		//RR 报警上限
#define NET_RESP_RR_LOW		303		//RR 报警下限
#define NET_RESP_APNEA			304		//RR 报警下限

#define NET_NIBP_ALL			400		//所有无创血压参数
#define NET_NIBP_ALMLEV		401		//报警等级
#define NET_NIBP_SYS_HI			402		//收缩压上限
#define NET_NIBP_SYS_LOW		403		//收缩压下限
#define NET_NIBP_MAP_HI			404		//平均压上限
#define NET_NIBP_MAP_LOW		405		//平均压下限
#define NET_NIBP_DIA_HI			406		//舒张压上限
#define NET_NIBP_DIA_LOW		407		//舒张压下限
#define NET_NIBP_INTERVAL		408		//测量周期

#define NET_TEMP1_ALL			500		//所有体温1参数
#define NET_TEMP1_ALMLEV		501		//报警等级
#define NET_TEMP1_HI			502		//temp1 报警上限
#define NET_TEMP1_LOW			503		//temp1 报警下限

#define NET_TEMP2_ALL			600		//所有体温1参数
#define NET_TEMP2_ALMLEV		601		//报警等级
#define NET_TEMP2_HI			602		//temp1 报警上限
#define NET_TEMP2_LOW			603		//temp1 报警下限

#define NET_CO2_ALL				700		//所有co2参数
#define NET_CO2_ALMLEV			701		//报警等级
#define NET_CO2_ETCO2_HI		702		//ETCO2 报警上限
#define NET_CO2_ETCO2_LOW		703		//ETCO2 报警下限
#define NET_CO2_FICO2_HI		704		//FICO2 报警上限
#define NET_CO2_FICO2_LOW		705		//FICO2 报警下限
#define NET_CO2_AWRR_HI		706		//AWRR 报警上限
#define NET_CO2_AWRR_LOW		707		//AWRR 报警下限

#define NET_IBP1_ALL				800		//所有IBP2参数
#define NET_IBP1_ALMLEV			801		//报警等级
#define NET_IBP1_SYS_HI			802		//收缩压上限
#define NET_IBP1_SYS_LOW		803		//收缩压下限
#define NET_IBP1_MAP_HI			804		//平均压上限
#define NET_IBP1_MAP_LOW		805		//平均压下限
#define NET_IBP1_DIA_HI			806		//舒张压上限
#define NET_IBP1_DIA_LOW		807		//舒张压下限

#define NET_IBP2_ALL				900		//所有IBP2参数
#define NET_IBP2_ALMLEV			901		//报警等级
#define NET_IBP2_SYS_HI			902		//收缩压上限
#define NET_IBP2_SYS_LOW		903		//收缩压下限
#define NET_IBP2_MAP_HI			904		//平均压上限
#define NET_IBP2_MAP_LOW		905		//平均压下限
#define NET_IBP2_DIA_HI			906		//舒张压上限
#define NET_IBP2_DIA_LOW		907		//舒张压下限



















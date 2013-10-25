/*************************************************************************** 
 *	Module Name:	ViewData
 *
 *	Abstract:	显示参数主程序
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2007-06-18 19:43:40
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "General_Funcs.h"
#include "NewDrawWave.h"
//#include "Menu.h"


#define Draw_Mem 	1

//调试框架
static BOOL bDebugRect = FALSE;
//static BOOL bDebugRect = TRUE;

//扩展参数面板的数量
#define EXT_PARASPANEL_COUNT	6
//参数显示面板
#define ID_PARAPANEL		900
#define ID_PARAPANEL_EXT0	901
#define ID_PARAPANEL_EXT1	902
#define ID_PARAPANEL_EXT2	903
#define ID_PARAPANEL_EXT3	904
#define ID_PARAPANEL_EXT4	905
#define ID_PARAPANEL_EXT5	906
static HWND hWndParasPanel = (HWND)NULL;
static HWND hWndParasPanelExt[EXT_PARASPANEL_COUNT] = {
	(HWND)NULL,
	(HWND)NULL,
	(HWND)NULL,
	(HWND)NULL,
	(HWND)NULL,	
	(HWND)NULL,	
};

//参数显示区域
static RECT gRcParas;
static RECT gRcParasExt[EXT_PARASPANEL_COUNT];


//绘图环境
//MemDC
HDC hdcParaMem = (HDC)NULL;
//绘图环境
HDC hdcPara = (HDC)NULL;
//扩展参数，包括大字符屏时
HDC hdcParaMemExt[EXT_PARASPANEL_COUNT]={
	(HDC)NULL,
	(HDC)NULL,
	(HDC)NULL,
	(HDC)NULL,	
	(HDC)NULL,
	(HDC)NULL,	
};
HDC hdcParaExt[EXT_PARASPANEL_COUNT]={
	(HDC)NULL,
	(HDC)NULL,
	(HDC)NULL,
	(HDC)NULL,
	(HDC)NULL,
	(HDC)NULL,	
};

///////////// 注意不要互锁 /////////////
//是否可以显示参数
static BOOL gbCanViewParas = FALSE;



//参数面板的框架的颜色
#define PARACOL_FRAME			COLOR_darkgray
//参数面板背景颜色
#define PARACOL_BK				COLOR_black
//报警时参数面板的背景颜色
#define PARACOL_BK_ALARM		COLOR_red
//报警时参数面板的文字颜色
#define PARACOL_TEXT_ALARM		COLOR_yellow
//其他信息文字颜色
#define PARACOL_TEXT				COLOR_lightgray
//报警时参数闪烁的颜色(其他参数颜色设置不应该与此颜色冲突)				
#define ALARMFLASH_COLOR			COLOR_red
//报警上下限的颜色
#define PARACOL_LIMIT				COLOR_darkgray

//欲显示的信息,从资源文中提取相应的翻译
static unsigned char strInfo[50];

//---------- 定义参数显示区域 -----------
//----------Time
static RECT gRCTime = {701,2,798,32};

//----------Patient Info
static RECT gRCPatientInfo = {555,2,695,30};
BOOL gbUpdatePatientInfo = TRUE;

//----------System Info
static RECT gRCSysInfo = {5,570,180,599};
BOOL gbUpdateSysInfo = TRUE;
//Other Info
// static RECT gRCOtherInfo =  {185,573,305,598};
static RECT gRCOtherInfo =  {185,50,400,130};
BOOL gbUpdateOtherInfo = TRUE;

//----------ECG
RECT gRCEcg;
//ECG Label
static RECT gRCEcg_Label;
//ECG Data
static RECT gRCEcg_HR;
BOOL gbAlarmEcg_HR= FALSE;
BOOL gbAlarmEcg_ST= FALSE;

//ECG Alarm Icon
static RECT gRCEcg_AlmIcon;
//ECG Icon
RECT gRCEcg_Icon;
//ECG Unit
static RECT gRCEcg_Unit;
//ECG PACE
static RECT gRCEcg_Pace;
//ECG ST
static RECT gRCEcg_St;
//PVCs
static RECT gRCEcg_PVCs;
//ARR
static RECT gRCEcg_Arr;
//ECG Info (ARR)
static RECT gRCEcg_Info;
//ECG Alarm Limit
static RECT gRCEcg_Limit;

//---------------NIBP
 RECT gRCNibp;
//NIBP Label
static RECT gRCNibp_Label;
//Limit
static RECT gRCNibp_limit_Sys;
static RECT gRCNibp_limit_Dia;
static RECT gRCNibp_limit_Mean;
//label
static RECT gRCNibp_label_SysDia;
static RECT gRCNibp_label_Mean;
//Alarm Icon
static RECT gRCNibp_AlmIcon;
//Mode
static RECT gRCNibp_Mode;
//Object
static RECT gRCNibp_Object;
//NIBP Data -- Sys
static RECT gRCNibp_Sys;
BOOL gbAlarmNibp_Sys = FALSE;
//NIBP --/--
static RECT gRCNibp_Inter;
//NIBP Data -- Dia
static RECT gRCNibp_Dia;
BOOL gbAlarmNibp_Dia = FALSE;
//NIBP Data -- Mean
static RECT gRCNibp_Mean;
BOOL gbAlarmNibp_Mean = FALSE;
//Unit
static RECT gRCNibp_Unit;
//Pressure
RECT gRCNibp_Pressure;
//测量时间
static RECT gRCNibp_Time;
//测量信息,包括报警
static RECT gRCNibp_Info;
//Time,倒计时
RECT gRCNibp_InterTime;

//---------------SpO2 & PR
 RECT gRCSpO2AndPR;
//Label
static RECT gRCSpO2_Label;
//Alarm Limit
static RECT gRCSpO2_Limit;
//Alarm Icon
static RECT gRCSpO2_AlmIcon;
//Strength
static RECT gRCSpO2_Strength;
//Data
static RECT gRCSpO2_Data;
BOOL gbAlarmSpO2_Data = FALSE;
//Unit
static RECT gRCSpO2_Unit;
//Info
static RECT gRCSpO2_Info;
//Column
RECT gRCSpO2_Column;
//PR Label
static RECT gRCPr_Label;
//Alarm Icon
static RECT gRCPr_AlmIcon;
//PR Data
static RECT gRCPr_Data;
BOOL gbAlarmPr_Data = FALSE;
//Alarm Limit
static RECT gRCPr_Limit;

//---------------Resp
 RECT gRCResp;
//Label
static RECT gRCResp_Label;
//Limit
static RECT gRCResp_Limit;
//Alarm Icon
static RECT gRCResp_AlmIcon;
//Data
static RECT gRCResp_Data;
BOOL gbAlarmResp_Data = FALSE;
//Unit 
static RECT gRCResp_Unit;
//Switch
static RECT gRCResp_Info;

//---------------TEMP
 RECT gRCTemp;
//Label
static RECT gRCTemp_Label;
//Limit
static RECT gRCTemp_Limit_T1;
static RECT gRCTemp_Limit_T2;
static RECT gRCTemp_Limit_TD;
//Alarm Icon
static RECT gRCTemp_AlmIcon;
//T1
static RECT gRCTemp_T1;
BOOL gbAlarmTemp_T1 = FALSE;
//T2
static RECT gRCTemp_T2;
BOOL gbAlarmTemp_T2 = FALSE;
//TD
static RECT gRCTemp_TD;
BOOL gbAlarmTemp_TD = FALSE;
//Unit
static RECT gRCTemp_Unit;

//---------------CO2
 RECT gRCCo2;
//Label CO2
static RECT gRCCo2_Label_Co2;
//Limit
static RECT gRCCo2_Limit_EtCo2;
static RECT gRCCo2_Limit_FiCo2;
//Label RR
static RECT gRCCo2_Label_RR;
//Limit
static RECT gRCCo2_Limit_RR;
//Alarm Icon CO2
static RECT gRCCo2_AlmIcon_Co2;
//Alarm Icon RR
static RECT gRCCo2_AlmIcon_RR;
//Data Co2
static RECT gRCCo2_EtCo2;
BOOL gbAlarmCo2_EtCo2= FALSE;
static RECT gRCCo2_FiCo2;
BOOL gbAlarmCo2_FiCo2= FALSE;
//Data awRR
static RECT gRCCo2_RR;
BOOL gbAlarmCo2_RR= FALSE;
//Unit Co2
static RECT gRCCo2_Unit_Co2;
//Unit awRR
static RECT gRCCo2_Unit_RR;
//Switch
static RECT gRCCo2_Switch_RR;
//CO2 Info
static RECT gRCCo2_Info_Co2;
//RR Info
static RECT gRCCo2_Info_RR;
//CO2 Status
static RECT gRCCo2_Status;
//增益校准状态
BOOL gbCO2GainCal = FALSE;


//IBP 标名索引, 与DataStruct.h中的宏定义一致
unsigned char *gStrIbpLabelIndex[]={"art", "pa",  "cvp", "lap", "rap", "icp", "p1", "p2"};
//unsigned char *gStrIbpLabelIndex[]={"art", "pa", "ao", "uap", "bap", "fap", "cvp", "lap", "rap", "icp", "uvp", "p1", "p2"};
//--------------IBP1 
static RECT gRCIbp1;
//Label
static RECT gRCIbp1_Label;
//Alarm Icon
static RECT gRCIbp1_AlmIcon;
//Data
static RECT gRCIbp1_Sys;
BOOL gbAlarmIbp1_Sys = FALSE;
static RECT gRCIbp1_Inter;
static RECT gRCIbp1_Dia;
BOOL gbAlarmIbp1_Dia = FALSE;
static RECT gRCIbp1_Mean;
BOOL gbAlarmIbp1_Mean = FALSE;
//Unit
static RECT gRCIbp1_Unit;
//Info
static RECT gRCIbp1_Info;
//Limit
static RECT gRCIbp1_limit_Sys;
static RECT gRCIbp1_limit_Dia;
static RECT gRCIbp1_limit_Mean;
static RECT gRCIbp1_Zero;
static RECT gRCIbp1_Cal;
//--------------IBP2 
 RECT gRCIbp2;
//Label
static RECT gRCIbp2_Label;
//Alarm Icon
static RECT gRCIbp2_AlmIcon;
//Data
static RECT gRCIbp2_Sys;
BOOL gbAlarmIbp2_Sys = FALSE;
static RECT gRCIbp2_Inter;
static RECT gRCIbp2_Dia;
BOOL gbAlarmIbp2_Dia = FALSE;
static RECT gRCIbp2_Mean;
BOOL gbAlarmIbp2_Mean = FALSE;
//Unit
static RECT gRCIbp2_Unit;
//Info
static RECT gRCIbp2_Info;
//Limit
static RECT gRCIbp2_limit_Sys;
static RECT gRCIbp2_limit_Dia;
static RECT gRCIbp2_limit_Mean;
static RECT gRCIbp2_Zero;
static RECT gRCIbp2_Cal;
//---------------Gas
 RECT gRCGas;
//Label CO2
static RECT gRCGas_Label_CO2;
static RECT gRCGas_Label_N2O;
static RECT gRCGas_Label_O2;
static RECT gRCGas_Label_Gas;
static RECT gRCGas_Label_RR;
static RECT gRCGas_Label_MAC;
//Alarm Icon Gas
static RECT gRCGas_AlmIcon;
static RECT gRCGas_Status;
//Data
static RECT gRCGas_Unit_Co2;
static RECT gRCGas_EtCo2;
static RECT gRCGas_FiCo2;
static RECT gRCGas_awRR;
static RECT gRCGas_Unit_O2;
static RECT gRCGas_EtO2;
static RECT gRCGas_FiO2;
static RECT gRCGas_Unit_N2O;
static RECT gRCGas_EtN2O;
static RECT gRCGas_FiN2O;
static RECT gRCGas_Unit_Gas;
static RECT gRCGas_EtGas;
static RECT gRCGas_FiGas;


// //MemDC
// extern HDC hdcParaMem;
// //绘图环境
// extern HDC hdcPara;


//调试界面
int giLeftAdd = 35;
int giLeftDec = 34;


// BOOL gbExtendTest = TRUE;		//Debug IBP Screen "gbHaveIbp"
// BOOL gbHaveIbp = TRUE;		//Debug IBP Screen "gbHaveIbp"


////////////////////// NIBP List ////////////////
//欲绘制的NIBP条数
#define NLIST_COUNT		5
//行数
#define NLIST_ROW_COUNT	(NLIST_COUNT+1)
//列数(时间, NSYS, NDIA, NMEAN, HR)
#define NLIST_COL_COUNT 	5
//列表显示的文字的长度
#define ITEM_TXTLENGTH		25	
//列表内容
typedef struct {
	RECT rect;
	unsigned char *strValue[ITEM_TXTLENGTH];
}TABLE_ITEM, *PTABLE_ITEM;

//事件列表(包括表头) 
static TABLE_ITEM gTableEvent[NLIST_ROW_COUNT][NLIST_COL_COUNT];
//Nibp Trend List
static RECT gRCNibp_List;

//趋势数组
extern CA_TRENDARRAY gTrendDatas;
//////////////////////////////////////////////


		
//MemDC
extern HDC hdcWaveMem;
//绘图环境
extern HDC hdcWave;

extern HWND ghWndMain;

//是否检测到心率和脉率
extern BOOL gbHaveBeep_QRS;
extern BOOL gbHaveBeep_Pulse;

//配置参数显示区域
static int InitRectEcg();
static int InitRectNibp();
static int InitRectNibpList();
static int InitRectSpO2();
static int InitRectTemp();
static int InitRectResp();
static int InitRectCo2();
static int InitRectGas();
static int InitRectIbp1();
static int InitRectIbp2();
static int InitRectSys0();
static int InitRectSys1();

//左边参数显示区域的高度
static int iLeftParaRectHeight = 0;

//是否静音
extern BOOL gbSilence;


//是否正在绘制参数
BOOL gbParasViewing = FALSE;
BOOL Show_Para = FALSE;


//与中央机连接的状态 FALSE:未连接中央机，TRUE：已经连上中央机
extern BOOL gbCentralStatus;


//根据模块配置以及屏幕类型初始化参数显示区域
static int InitDataViewRect()
{
	
#if SCREEN_1024
	iLeftParaRectHeight = (PARAVIEW_BOTTOM-PARAVIEW_TOP)/5;
#elif	SCREEN_640
	iLeftParaRectHeight = (PARAVIEW_BOTTOM-PARAVIEW_TOP)/5;
#else
	iLeftParaRectHeight = (PARAVIEW_BOTTOM-PARAVIEW_TOP)/5;
#endif		

if(gCfgSystem.bInterface==SCREEN_BIGCHAR){
	iLeftParaRectHeight = (PARAVIEW_BOTTOM-PARAVIEW_TOP)/4;
//	EnableWindow(hWndParasPanelExt[3],TRUE);
//	ShowWindow(hWndParasPanelExt[3], SW_SHOW);
	
}
else{
	;
//	EnableWindow(hWndParasPanelExt[3],FALSE);
//	ShowWindow(hWndParasPanelExt[3], SW_HIDE);
}
//隐藏窗口,当界面不是bigchar 和nibp时
if(gCfgSystem.bInterface==SCREEN_BIGCHAR||gCfgSystem.bInterface==SCREEN_NIBPLIST){
	ShowWindow(hWndParasPanelExt[0], SW_SHOW);
	ShowWindow(hWndParasPanelExt[3], SW_SHOW);
}
else{
	ShowWindow(hWndParasPanelExt[0], SW_HIDE);
	ShowWindow(hWndParasPanelExt[3], SW_HIDE);
}
//隐藏窗口,当界面是ibp时
if(gCfgSystem.bInterface==SCREEN_IBP){
	ShowWindow(hWndParasPanelExt[4], SW_SHOW);
	ShowWindow(hWndParasPanelExt[5], SW_SHOW);
	
}
else{
	ShowWindow(hWndParasPanelExt[4], SW_HIDE);
	ShowWindow(hWndParasPanelExt[5], SW_HIDE);	
}

	InitRectEcg();
 	InitRectNibp();
	InitRectNibpList();
 	InitRectSpO2();
	InitRectTemp();
	InitRectResp();
	InitRectCo2();
//	InitRectGas();
	InitRectIbp1();
	InitRectIbp2();
	InitRectSys0();
	InitRectSys1();
Show_Para=TRUE;
	return 0;
}
	
//根据模块配置以及屏幕类型初始化参数显示区域
static int InitRectEcg()
{
		SetRect(&gRCEcg, 
			 gRcParas.left, 
			 gRcParas.top, 
			 gRcParas.right-1, 
			 gRcParas.top+iLeftParaRectHeight);

		//以下为相对坐标
		//ECG Label
		SetRect(&gRCEcg_Label, gRCEcg.left+1, gRCEcg.top+1, gRCEcg.left+70, gRCEcg.top+17);
		//ECG Unit
		SetRect(&gRCEcg_Unit, gRCEcg.left+1,gRCEcg.top+23,gRCEcg.left+38,gRCEcg.top+38);
		//ECG Limit
		SetRect(&gRCEcg_Limit, gRCEcg.left+1, gRCEcg.top+30, gRCEcg.left+30, gRCEcg.top+55);
		//ECG Data
		SetRect(&gRCEcg_HR, gRCEcg.left+30,gRCEcg.top+1,gRCEcg.right-30,gRCEcg.bottom-17);
		//ECG Alarm Icon
#if SCREEN_1024		
		SetRect(&gRCEcg_AlmIcon, gRCEcg.right-20,gRCEcg.top+1,gRCEcg.right, gRCEcg.top+20);
#elif SCREEN_640
		SetRect(&gRCEcg_AlmIcon, gRCEcg.right-20,gRCEcg.top+1,gRCEcg.right, gRCEcg.top+20);		
#else
		SetRect(&gRCEcg_AlmIcon, gRCEcg.right-20,gRCEcg.top+1,gRCEcg.right, gRCEcg.top+20);		
#endif		
		//ECG Icon
		SetRect(&gRCEcg_Icon, gRCEcg.right-30,gRCEcg.top+25,gRCEcg.right,gRCEcg.top+55);
		//ECG PACE
		SetRect(&gRCEcg_Pace, gRCEcg.left+170-giLeftDec,gRCEcg.top+11,gRCEcg.left + 241-giLeftDec,gRCEcg.top+27);
		//PVCs
		SetRect(&gRCEcg_PVCs, gRCEcg.left+170-giLeftDec,gRCEcg.top+51,gRCEcg.left + 241-giLeftDec,gRCEcg.top+65);
		//ARR
		SetRect(&gRCEcg_Arr, gRCEcg.left+170-giLeftDec,gRCEcg.top+69,gRCEcg.left + 241-giLeftDec,gRCEcg.top+85);
		//ECG Info (ARR)
		SetRect(&gRCEcg_Info, gRCEcg.left+1,gRCEcg.bottom-30,gRCEcg.left+RECTW(gRCEcg)/2,gRCEcg.bottom-1);
		//ECG ST
		SetRect(&gRCEcg_St, gRCEcg.right-RECTW(gRCEcg)/2,gRCEcg.bottom-16,gRCEcg.right,gRCEcg.bottom-1);
	
	
		return 0;
}


//Arr Type
unsigned char *strArrType[] = {
	"",							//data[1] bit0
	"",							//data[1] bit1
	"pacer_non_capturing",		//data[1] bit2
	"pacer_non_sensing",			//data[1] bit3
	"irregular_rhythm",			//data[1] bit4
	"atrial_fibrillation",				//data[1] bit5
	"",							//data[1] bit6
	"",							//data[2] bit0
	"",							//data[2] bit1
	"",							//data[2] bit2
	"supraventricular_bradycardia",	//data[2] bit3
	"supraventricular_tachycardia",	//data[2] bit4
	"",							//data[2] bit5
	"Isolated_PVC",				//data[2] bit6
	"trigeminy",					//data[3] bit0
	"pause_or_dropped_beat",		//data[3] bit1
	"accelerated_ventricular_rhythm",//data[3] bit2
	"bigeminy",					//data[3] bit3
	"couplet",					//data[3] bit4
	"ventricular_bradycardia",		//data[3] bit5
	"PVC_within_previous_QT",	//data[3] bit5
	"ventricular_tachycardia_short",	//data[4] bit0
	"ventricular_tachycardia_long",	//data[4] bit1
	"ventricular_fibrillation",		//data[4] bit2
	"asystole",					//data[4] bit3
	"ECG_Artifact",				//data[4] bit4
	"Not_found_R_Wave",			//data[4] bit5
	"ECG_Learning",				//data[4] bit6
};

static int ViewECG_ARR_info(HDC hdc)
{
	int i;
	static int arr_count;
	unsigned char str_arr_info[200]={0};
	if(gCfgEcg.bArrAnalyse==0) return 0;
	
	for(i=0;i<28;i++){
		if(gValueEcg.lARR&(0x01<<i)){
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGARR, strArrType[i], str_arr_info, sizeof str_arr_info,strArrType[i]);
			switch(arr_count){
				case 0:
					DrawText(hdc, str_arr_info, -1, &gRCEcg_Info, DT_NOCLIP | DT_BOTTOM | DT_LEFT | DT_SINGLELINE);	
				break;
				case 1:
					DrawText(hdc, str_arr_info, -1, &gRCEcg_Info, DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);	
				break;	
				default:break;
					
			}

			arr_count++;
		}
		
	}
	arr_count=0;
	return 0;

}

//显示ECG参数
static int ViewEcg(HDC hdc, BYTE bInterface, BYTE bMemCopy)
{
	unsigned char strHR[5]={0};	
	unsigned char strLabel[10]={0};
	static BOOL bFlashHR =FALSE;
	static BOOL bFlashST =FALSE;
	static int bShowST = 0;
	unsigned char strLimit[5]={0};
	unsigned char strST1[15]={0};
	unsigned char strST2[15]={0};
	unsigned char strST[4][15]={0};
	unsigned char strArrInfo[50]={0}; 
	
	//根据HR alarm source判断
	switch(gCfgEcg.bAlmSource){
		case HR_ALM_SRC_HR:{
			if((!gValueEcg.bRAOff)  && (gValueEcg.wHR !=0)){
			//if(!gValueEcg.bRAOff){
				snprintf(strHR, sizeof strHR, "%d", gValueEcg.wHR);
				if(gValueEcg.bHRInval)snprintf(strHR, sizeof strHR, "---");
			}
			else{
				snprintf(strHR, sizeof strHR, "---");
			}
			GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "hr", strLabel, sizeof strLabel);
		}break;
		case HR_ALM_SRC_PR:{
			if((!gValuePulse.bOff) && (gValuePulse.wPR !=0)){
				snprintf(strHR, sizeof strHR, "%d", gValuePulse.wPR);
			}
			else{
				snprintf(strHR, sizeof strHR, "---");
			}
			GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "pr", strLabel, sizeof strLabel);
		}break;
		default:{//Auto
			//ECG 优先
			if((!gValueEcg.bRAOff)  && (gValueEcg.wHR >0)){
			//if((!gValueEcg.bRAOff)){
				snprintf(strHR, sizeof strHR, "%d", gValueEcg.wHR);
				if(gValueEcg.bHRInval)snprintf(strHR, sizeof strHR, "---");
				GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "hr", strLabel, sizeof strLabel);	
			}
			else if((!gValuePulse.bOff) && (gValuePulse.wPR !=0) ){
				snprintf(strHR, sizeof strHR, "%d", gValuePulse.wPR);
				GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "pr", strLabel, sizeof strLabel);	
			}
			else{
				snprintf(strHR, sizeof strHR, "---");
				GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "hr", strLabel, sizeof strLabel);	
			}
		}break;
	}
	
  	SetTextColor(hdc, gCfgEcg.iColor);

	
	//显示 Alarm Icon
 	if(gCfgEcg.bAlmControl == ALARM_OFF)
		FillBoxWithBitmap(hdc, gRCEcg_AlmIcon.left+1,gRCEcg_AlmIcon.top+1, 16, 16, &gIconAlarmOff);

	//显示Label , Unit 
	SelectFont(hdc, gFontEnglish_16);
	memset(strInfo, 0, sizeof strInfo);
	if(gCfgSystem.bLanguage == LANGUAGE_SPANISH)
		snprintf(strInfo, sizeof strInfo, "%s (lpm)", strLabel);
	else
		snprintf(strInfo, sizeof strInfo, "%s (bpm)", strLabel);
    DrawText(hdc, strInfo, -1, &gRCEcg_Label, DT_NOCLIP | DT_VCENTER | DT_LEFT | DT_SINGLELINE);
//	DrawText(hdc, "(bpm)", -1, &gRCEcg_Unit, DT_NOCLIP | DT_TOP | DT_CENTER | DT_SINGLELINE);

	//显示Data
	if(gbAlarmEcg_HR && (gCfgEcg.bAlmControl != ALARM_OFF)){
		if(bFlashHR){
			SetTextColor(hdc, ALARMFLASH_COLOR);
		}
		else{
			SetTextColor(hdc, gCfgEcg.iColor);
		}
		bFlashHR = !bFlashHR;
	}
	else{
		SetTextColor(hdc, gCfgEcg.iColor);			
	}
	
	if(gbHaveIbp){

		
#if SCREEN_1024
		SelectFont(hdc, gFontTTF_90Bk);
#elif	SCREEN_640
 		SelectFont(hdc, gFontTTF_50Bk);
#else
 		SelectFont(hdc, gFontTTF_60);
#endif
		DrawText(hdc, strHR, -1, &gRCEcg_HR, DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE);


	}
	else{

		
#if SCREEN_1024
	SelectFont(hdc, gFontTTF_105);

//	TextOut(hdc, gRCEcg_HR.left-20, gRCEcg_HR.top-50, strHR);
	DrawText(hdc, strHR, -1, &gRCEcg_HR, DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE);

#elif SCREEN_640

	if(gCfgSystem.bInterface==SCREEN_BIGCHAR)
		SelectFont(hdc, gFontTTF_70);
	else
		SelectFont(hdc, gFontTTF_60);

	DrawText(hdc, strHR, -1, &gRCEcg_HR, DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE);

#else

	if(gCfgSystem.bInterface==SCREEN_BIGCHAR)
		SelectFont(hdc, gFontTTF_90Bk);
	else
		SelectFont(hdc, gFontTTF_75);

	DrawText(hdc, strHR, -1, &gRCEcg_HR, DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	
	
#endif
		//TextOut(hdc, gRCEcg_HR.left, gRCEcg_HR.top-50, strHR);

	}
	//绘制ST，轮询显示，如无数值则不显示
	int iST[5],i;
	unsigned char stname[5][10]={"ST I","ST II","ST III","ST V","PVC"};
	iST[0]= gValueEcg.iST[ECGWAVE_I];
	iST[1]= gValueEcg.iST[ECGWAVE_II];
	iST[2]= gValueEcg.iST[ECGWAVE_III];
	iST[3]=gValueEcg.iST[ECGWAVE_V];
	iST[4]=gValueEcg.bPVC;
	
	//加上脱落判断
	for(i=0;i<4;i++){
		if((iST[i] <2000) && (!gValueEcg.bRAOff))
			snprintf(strST[i], sizeof strST[i], "%s:%.2fmV",stname[i], (iST[i]-1000)/100.0);

	}
	snprintf(strST[4], sizeof strST[4], "%s:%dbpm",stname[4], iST[4]);


// 	SetTextColor(hdc, COLOR_lightgray);
	
	SelectFont(hdc, gFontSystem);

	if(gbAlarmEcg_ST && (gCfgEcg.bAlmControl != ALARM_OFF)){
		if(bFlashST){
			SetTextColor(hdc, ALARMFLASH_COLOR);
		}
		else{
			SetTextColor(hdc, gCfgEcg.iColor);
		}
		bFlashST = !bFlashST;
	}
	else{
		SetTextColor(hdc, gCfgEcg.iColor);			
	}
	
	
	DrawText(hdc, strST[bShowST], -1, &gRCEcg_St, DT_NOCLIP|  DT_BOTTOM  |  DT_RIGHT | DT_SINGLELINE);
	

	if(gCfgEcg.bPVCs==1)
		bShowST=(bShowST+1)%5;
	else
		bShowST=(bShowST+1)%4;
	
	//arr info
	ViewECG_ARR_info(hdc);
	
	if(gCfgAlarm.bViewLimit){
		SelectFont(hdc, gFontSmall);
		SetTextColor(hdc, PARACOL_LIMIT);
		memset(strLimit, 0, sizeof strLimit);
		snprintf(strLimit, sizeof strLimit, "%d", gCfgEcg.wHigh_HR);
		DrawText(hdc, strLimit, -1, &gRCEcg_Limit, DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);
		memset(strLimit, 0, sizeof strLimit);
		snprintf(strLimit, sizeof strLimit, "%d", gCfgEcg.wLow_HR);
		DrawText(hdc, strLimit, -1, &gRCEcg_Limit, DT_NOCLIP | DT_BOTTOM | DT_LEFT | DT_SINGLELINE);
	}
	
	//绘制框架
 	Rectangle(hdc, gRCEcg.left, gRCEcg.top, gRCEcg.right+5, gRCEcg.bottom);
	//调试框架
	if(bDebugRect){
//ECG Label
		SetPenColor(hdc, COLOR_red);
		Rectangle(hdc, gRCEcg_Label.left, gRCEcg_Label.top, gRCEcg_Label.right, gRCEcg_Label.bottom);
//ECG Data
		SetPenColor(hdc, COLOR_blue);
		Rectangle(hdc, gRCEcg_HR.left, gRCEcg_HR.top, gRCEcg_HR.right, gRCEcg_HR.bottom);
//ECG Alarm Icon
		SetPenColor(hdc, COLOR_darkgray);
		Rectangle(hdc, gRCEcg_AlmIcon.left, gRCEcg_AlmIcon.top, gRCEcg_AlmIcon.right, gRCEcg_AlmIcon.bottom);
//ECG Icon
		SetPenColor(hdc, COLOR_green);
		Rectangle(hdc, gRCEcg_Icon.left, gRCEcg_Icon.top, gRCEcg_Icon.right, gRCEcg_Icon.bottom);
//ECG Unit
	//SetPenColor(hdc, COLOR_yellow);
	//	Rectangle(hdc, gRCEcg_Unit.left, gRCEcg_Unit.top, gRCEcg_Unit.right, gRCEcg_Unit.bottom);
//ECG PACE
	SetPenColor(hdc, COLOR_lightwhite);
	//	Rectangle(hdc, gRCEcg_Pace.left, gRCEcg_Pace.top, gRCEcg_Pace.right, gRCEcg_Pace.bottom);
//PVCs
	//	Rectangle(hdc, gRCEcg_PVCs.left, gRCEcg_PVCs.top, gRCEcg_PVCs.right, gRCEcg_PVCs.bottom);
//ARR
//		Rectangle(hdc, gRCEcg_Arr.left, gRCEcg_Arr.top, gRCEcg_Arr.right, gRCEcg_Arr.bottom);
//ECG Info (ARR)
		Rectangle(hdc, gRCEcg_Info.left, gRCEcg_Info.top, gRCEcg_Info.right, gRCEcg_Info.bottom);
//ECG ST
		Rectangle(hdc, gRCEcg_St.left, gRCEcg_St.top, gRCEcg_St.right, gRCEcg_St.bottom);

//ECG Alarm Limit
		SetPenColor(hdc, COLOR_yellow);
		Rectangle(hdc, gRCEcg_Limit.left, gRCEcg_Limit.top, gRCEcg_Limit.right, gRCEcg_Limit.bottom);
	}
	
	//拷贝DC
	if(Draw_Mem){
		BitBlt(hdc, 
		 	gRCEcg.left, gRCEcg.top, RECTW(gRCEcg)+1, RECTH(gRCEcg)+1, 
	      	 	hdcPara, 
	       	gRCEcg.left, gRCEcg.top, 
	       	0);
	}

	return 0;
}


//根据模块配置以及屏幕类型初始化参数显示区域
static int InitRectNibp()
{
	//NIBP在有无IBP的情况下，大小是不同的
	//绝对坐标
if(gCfgSystem.bInterface!=SCREEN_BIGCHAR){
	SetRect(&gRCNibp, 
			 gRcParas.left, 
			 gRcParas.top+iLeftParaRectHeight, 
			 gRcParas.right-1, 
			 gRcParas.top+iLeftParaRectHeight*2);

}else{
	SetRect(&gRCNibp, 
			 gRcParasExt[3].left, 
			 gRcParasExt[3].top, 
			 gRcParasExt[3].right, 
			 gRcParasExt[3].bottom-1);
}

	//相对坐标
	//NIBP Label
	SetRect(&gRCNibp_Label, gRCNibp.left+1,  gRCNibp.top+1, gRCNibp.left+70, gRCNibp.top+17);
	//Unit
	SetRect(&gRCNibp_Unit, gRCNibp.left+1,  gRCNibp.top+23, gRCNibp.left+38, gRCNibp.top+38);

	//Mode
	SetRect(&gRCNibp_Mode, gRCNibp.right-70,  gRCNibp.top+1, gRCNibp.right-20, gRCNibp.top+17);
	//Object 
	SetRect(&gRCNibp_Object, gRCNibp.left+100,  gRCNibp.top+1, gRCNibp.left+140, gRCNibp.top+17);
	//Alarm Icon
	SetRect(&gRCNibp_AlmIcon, gRCNibp.right-20,  gRCNibp.top+1, gRCNibp.right, gRCNibp.top+20);

	//Limit Sys
	SetRect(&gRCNibp_limit_Sys, gRCNibp.left+1,  gRCNibp.top+30, gRCNibp.left+30, gRCNibp.top+55);
	//Limit Dia
	SetRect(&gRCNibp_limit_Dia, gRCNibp.right-30,  gRCNibp.top+30, gRCNibp.right-1, gRCNibp.top+55);
	
	//Limit Mean
	SetRect(&gRCNibp_limit_Mean, gRCNibp.left+1,  gRCNibp.bottom-50, gRCNibp.left+30, gRCNibp.bottom-25);

#if SCREEN_640
	
	if(gCfgSystem.bInterface==SCREEN_BIGCHAR){
		
		//Time,倒计时
		SetRect(&gRCNibp_InterTime, gRCNibp.right-150,  gRCNibp.top+3, gRCNibp.right-70, gRCNibp.top+17);
		//测量信息,包括报警
	// 	SetRect(&gRCNibp_Info, gRCNibp.left+77-giLeftDec,  gRCNibp.top+76, gRCNibp.left+195-giLeftDec, gRCNibp.top+91);
		SetRect(&gRCNibp_Info, gRCNibp.right-200,  gRCNibp.bottom-17, gRCNibp.right, gRCNibp.bottom);

		//NIBP Data -- Sys
		SetRect(&gRCNibp_Sys, gRCNibp.left+30,  gRCNibp.top, gRCNibp.left+180, gRCNibp.bottom-30);
		//NIBP Data -- Dia
		SetRect(&gRCNibp_Dia, gRCNibp.right-180,  gRCNibp.top, gRCNibp.right-30,gRCNibp.bottom-30);

		//NIBP --/--
		SetRect(&gRCNibp_Inter, gRCNibp.left+30,  gRCNibp.top, gRCNibp.right-30, gRCNibp.bottom-30);
		//NIBP Data -- Mean
 		SetRect(&gRCNibp_Mean, gRCNibp.left+130,  gRCNibp.top+85, gRCNibp.right-130, gRCNibp.bottom);
		//Label sys-dia
		SetRect(&gRCNibp_label_SysDia, gRCNibp_Inter.left , gRCNibp_Inter.top, gRCNibp_Inter.right, gRCNibp_Inter.top+15);
		//Label mean
		SetRect(&gRCNibp_label_Mean, gRCNibp_Mean.left,  gRCNibp_Mean.top-5, gRCNibp_Mean.right, gRCNibp_Mean.top+15);
	
	}else{
		//Limit Sys
		SetRect(&gRCNibp_limit_Sys, gRCNibp.left+1,  gRCNibp.top+23, gRCNibp.left+30, gRCNibp.top+43);
		//Limit Dia
		SetRect(&gRCNibp_limit_Dia, gRCNibp.right-30,  gRCNibp.top+30, gRCNibp.right-1, gRCNibp.top+55);
		//Limit Mean
		SetRect(&gRCNibp_limit_Mean, gRCNibp.left+1,  gRCNibp.bottom-43, gRCNibp.left+30, gRCNibp.bottom-23);

		//Time,倒计时
		SetRect(&gRCNibp_InterTime, gRCNibp.right-100,  gRCNibp.top+3, gRCNibp.right-20, gRCNibp.top+17);
		//测量信息,包括报警
	// 	SetRect(&gRCNibp_Info, gRCNibp.left+77-giLeftDec,  gRCNibp.top+76, gRCNibp.left+195-giLeftDec, gRCNibp.top+91);
		SetRect(&gRCNibp_Info, gRCNibp.left+40,  gRCNibp.bottom-17, gRCNibp.left+150, gRCNibp.bottom);

		//NIBP Data -- Sys
		SetRect(&gRCNibp_Sys, gRCNibp.left+30,  gRCNibp.top+17, gRCNibp.left+90, gRCNibp.bottom-28);
		//NIBP Data -- Dia
		SetRect(&gRCNibp_Dia, gRCNibp.right-90,  gRCNibp.top+17, gRCNibp.right-30,gRCNibp.bottom-28);

		//NIBP --/--
		SetRect(&gRCNibp_Inter, gRCNibp.left+30,  gRCNibp.top+17, gRCNibp.right-30, gRCNibp.bottom-28);
		//NIBP Data -- Mean
 		SetRect(&gRCNibp_Mean, gRCNibp.left+135,  gRCNibp.bottom-35, gRCNibp.right-1, gRCNibp.bottom);

		//Label sys-dia
		SetRect(&gRCNibp_label_SysDia, gRCNibp_Inter.left , gRCNibp_Inter.top-3, gRCNibp_Inter.right, gRCNibp_Inter.top+15);
		//Label mean
		SetRect(&gRCNibp_label_Mean, gRCNibp_Mean.left,  gRCNibp_Mean.top-3, gRCNibp_Mean.right, gRCNibp_Mean.top+15);
	
	}
#else

	if(gCfgSystem.bInterface==SCREEN_BIGCHAR){
		
		//Time,倒计时
		SetRect(&gRCNibp_InterTime, gRCNibp.right-180,  gRCNibp.top+3, gRCNibp.right-70, gRCNibp.top+17);
		//测量信息,包括报警
	// 	SetRect(&gRCNibp_Info, gRCNibp.left+77-giLeftDec,  gRCNibp.top+76, gRCNibp.left+195-giLeftDec, gRCNibp.top+91);
		SetRect(&gRCNibp_Info, gRCNibp.right-200,  gRCNibp.bottom-17, gRCNibp.right, gRCNibp.bottom);

		//NIBP Data -- Sys
		SetRect(&gRCNibp_Sys, gRCNibp.left+30,  gRCNibp.top, gRCNibp.left+230, gRCNibp.bottom-48);
		//NIBP Data -- Dia
		SetRect(&gRCNibp_Dia, gRCNibp.right-230,  gRCNibp.top, gRCNibp.right-30,gRCNibp.bottom-48);

		//NIBP --/--
		SetRect(&gRCNibp_Inter, gRCNibp.left+230,  gRCNibp.top, gRCNibp.right-230, gRCNibp.bottom-42);
		//NIBP Data -- Mean
 		SetRect(&gRCNibp_Mean, gRCNibp.left+130,  gRCNibp.top+100, gRCNibp.right-130, gRCNibp.bottom);
		//Label sys-dia
		SetRect(&gRCNibp_label_SysDia, gRCNibp_Inter.left , gRCNibp_Inter.top, gRCNibp_Inter.right, gRCNibp_Inter.top+15);
		//Label mean
		SetRect(&gRCNibp_label_Mean, gRCNibp_Mean.left,  gRCNibp_Mean.top-5, gRCNibp_Mean.right, gRCNibp_Mean.top+15);
	
	}else{
	
		//Time,倒计时
		SetRect(&gRCNibp_InterTime, gRCNibp.right-100,  gRCNibp.top+3, gRCNibp.right-20, gRCNibp.top+17);
		//测量信息,包括报警
	// 	SetRect(&gRCNibp_Info, gRCNibp.left+77-giLeftDec,  gRCNibp.top+76, gRCNibp.left+195-giLeftDec, gRCNibp.top+91);
		SetRect(&gRCNibp_Info, gRCNibp.left+40,  gRCNibp.bottom-17, gRCNibp.left+150, gRCNibp.bottom);

		//NIBP Data -- Sys
		SetRect(&gRCNibp_Sys, gRCNibp.left+30,  gRCNibp.top+17, gRCNibp.left+110, gRCNibp.bottom-40);
		//NIBP Data -- Dia
		SetRect(&gRCNibp_Dia, gRCNibp.right-110,  gRCNibp.top+17, gRCNibp.right-30,gRCNibp.bottom-40);

		//NIBP --/--
		SetRect(&gRCNibp_Inter, gRCNibp.left+110,  gRCNibp.top+17, gRCNibp.right-110, gRCNibp.bottom-40);
		//NIBP Data -- Mean
 		SetRect(&gRCNibp_Mean, gRCNibp.left+140,  gRCNibp.bottom-40, gRCNibp.right-1, gRCNibp.bottom);

		//Label sys-dia
		SetRect(&gRCNibp_label_SysDia, gRCNibp_Inter.left , gRCNibp_Inter.top-3, gRCNibp_Inter.right, gRCNibp_Inter.top+15);
		//Label mean
		SetRect(&gRCNibp_label_Mean, gRCNibp_Mean.left,  gRCNibp_Mean.top-3, gRCNibp_Mean.right, gRCNibp_Mean.top+15);
	
	}
#endif	
	
	//测量时间
// 	SetRect(&gRCNibp_Time, gRCNibp.left+35-giLeftDec,  gRCNibp.top+76, gRCNibp.left+75-giLeftDec, gRCNibp.top+91);
	SetRect(&gRCNibp_Time, gRCNibp.left+1,  gRCNibp.bottom-17, gRCNibp.left+40, gRCNibp.bottom);
	//袖带压力
	SetRect(&gRCNibp_Pressure, gRCNibp.left+40,  gRCNibp.bottom-17, gRCNibp.left+75, gRCNibp.bottom);
	
	return 0;
}

/*
	显示NIBP测量时间,格式: XX分钟(小时)以前
	1分钟更新一次
*/
static int ViewNibp_Time(HDC hdc)
{
	S_TIME sCurTime;
	int iInter;		//时间间隔,单位:分钟
	unsigned char strTmp1[10] = {0};
	unsigned char strTmp2[10] = {0};
	unsigned char strTime[30]={0};
	
	GetSysTime(&sCurTime);
	
	iInter = CompTimeWithMin(&sCurTime, &(gValueNibp.sTime));
	if(iInter <=0) iInter = 0;

	snprintf(strTime, sizeof strTime, "%.2d:%.2d", gValueNibp.sTime.bHour, gValueNibp.sTime.bMin);

// 	SetTextColor(hdcParaMem, gCfgNibp.iColor);	
//	if(bDebugRect) Rectangle(hdc, gRCNibp_Time.left, gRCNibp_Time.top, gRCNibp_Time.right, gRCNibp_Time.bottom);
	//正常测量完成时显示时间,错误时显示错误信息,测试时显示测试信息
	if(!gValueNibp.bCanceled){
// 		SelectFont(hdcParaMem, gFontSystem);
		DrawText(hdc, strTime, -1, &gRCNibp_Time, DT_NOCLIP | DT_VCENTER | DT_LEFT| DT_SINGLELINE);
	}

	return 0;
}

extern BOOL gbNibpProtect;
//显示NIBP信息
static int ViewNibp_Info(HDC hdc)
{
	unsigned char strErrCode[3]={0};
	
	memset(strInfo, 0, sizeof strInfo);

	switch(gValueNibp.wFDB){
		case NBP_FDB_TAG_PRS_ERR:
			GetStringFromResFiles(gsLanguageRes, "TEC_INFO", "nibp_fdb_tag_prs_err", strInfo, sizeof strInfo,"init pressure set error");
		break;
		case NBP_FDB_PNEU_LEAK:
			GetStringFromResFiles(gsLanguageRes, "TEC_INFO", "nibp_fdb_pneu_leak", strInfo, sizeof strInfo,"leak out");
		break;
		case NBP_FDB_INFL_FAIL:
			GetStringFromResFiles(gsLanguageRes, "TEC_INFO", "nibp_fdb_infl_fail", strInfo, sizeof strInfo,"cuff off");
		break;
		case NBP_FDB_DEFLT_OT:
			GetStringFromResFiles(gsLanguageRes, "TEC_INFO", "nibp_fdb_deflt_ot", strInfo, sizeof strInfo,"deflate overtime");
		break;
		case NBP_FDB_MEASURE_OT:
			GetStringFromResFiles(gsLanguageRes, "TEC_INFO", "nibp_fdb_measure_ot", strInfo, sizeof strInfo,"overtime");
		break;
		case NBP_FDB_PULSE_WEAK:
			GetStringFromResFiles(gsLanguageRes, "TEC_INFO", "nibp_fdb_pulse_weak", strInfo, sizeof strInfo,"pulse weak");
		break;
		case NBP_FDB_EXCS_PRS:
			GetStringFromResFiles(gsLanguageRes, "TEC_INFO", "nibp_fdb_excs_prs", strInfo, sizeof strInfo,"out of rang");
		break;
		case NBP_FDB_SIS_CTI_OT:
			GetStringFromResFiles(gsLanguageRes, "TEC_INFO", "nibp_fdb_sis_cti_ot", strInfo, sizeof strInfo,"Inflation Overtime");
		break;
		case NBP_FDB_SIS_CTC_OT:
			GetStringFromResFiles(gsLanguageRes, "TEC_INFO", "nibp_fdb_sis_ctc_ot", strInfo, sizeof strInfo,"Check Overtime");
		break;
		case NBP_FDB_SIS_CTW_OT:
			GetStringFromResFiles(gsLanguageRes, "TEC_INFO", "nibp_fdb_sis_ctw_ot", strInfo, sizeof strInfo,"Wait Overtime");
		break;
		case NBP_FDB_SIS_CTD_OT:
			GetStringFromResFiles(gsLanguageRes, "TEC_INFO", "nibp_fdb_sis_ctd_ot", strInfo, sizeof strInfo,"Deflation Overtime");
		break;
		case NBP_FDB_SIS_CTR_OT:
			GetStringFromResFiles(gsLanguageRes, "TEC_INFO", "nibp_fdb_ctr_ot", strInfo, sizeof strInfo,"Resolution Overtime");
		break;
		case NBP_FDB_SIS_PRO_OT:
			GetStringFromResFiles(gsLanguageRes, "TEC_INFO", "nibp_fdb_sis_pro_ot", strInfo, sizeof strInfo,"Process Overtime");
		break;
		case NBP_FDB_FAIL_01:
		case NBP_FDB_FAIL_02:
		case NBP_FDB_FAIL_03:
		case NBP_FDB_FAIL_04:
		case NBP_FDB_FAIL_05:
		case NBP_FDB_FAIL_06:
		case NBP_FDB_FAIL_07:
		case NBP_FDB_FAIL_08:
		case NBP_FDB_FAIL_09:
		case NBP_FDB_FAIL_10:	
		case NBP_FDB_FAIL_11:
		case NBP_FDB_FAIL_12:
		case NBP_FDB_FAIL_13:
		case NBP_FDB_FAIL_14:
		case NBP_FDB_FAIL_15:
		case NBP_FDB_FAIL_16:
			GetStringFromResFiles(gsLanguageRes, "TEC_INFO", "nibp_fdb_fail", strInfo, sizeof strInfo,"NIBP fail");
		break;	
		default:
			strcpy(strInfo,"");
		break;
		

	}
	
	//nibp测量错误信息
	if(((!gValueNibp.bCanceled)  || (gbNibpProtect) )&&gValueNibp.bStatus==NIBP_SYSTEM_IDLE){
		snprintf(strErrCode, sizeof strErrCode, "%d", gValueNibp.wErrCode);
		if(gValueNibp.wFDB>=NBP_FDB_FAIL_01&&gValueNibp.wFDB<=NBP_FDB_FAIL_16)
					sprintf(strInfo,"%s:%x",strInfo,gValueNibp.wFDB);
		
		if(gCfgSystem.bInterface==SCREEN_BIGCHAR)
			DrawText(hdc, strInfo, -1, &gRCNibp_Info, DT_NOCLIP | DT_VCENTER | DT_RIGHT | DT_SINGLELINE);	
		else
			DrawText(hdc, strInfo, -1, &gRCNibp_Info, DT_NOCLIP | DT_VCENTER | DT_LEFT | DT_SINGLELINE);	

	}

	
	return 0;
}


/**
	绘制倒计时棒图刻度和进度
	单位:秒
	bType: 0-Auto, 1-Stat, 2-Puncture
 */
static unsigned char *strAutoTime[]={"1min", "2min", "3min", "4min", "5min", "10min",
	"15min", "30min", "60min", "90min", "120min", "180min", "240min", "480min"};	
static int ViewNibpCycleGraph(HDC hdc, int iTime, int iMax, BYTE bType)
{
	int iStep;
	
// 	printf("iTime is %d.\n", iTime);
	
	iStep = (iTime * (gRCNibp_InterTime.right - gRCNibp_InterTime.left) )/ iMax;
	//左边为已过时间
	SetBrushColor(hdc, COLOR_darkblue);
	FillBox(hdc, gRCNibp_InterTime.left, gRCNibp_InterTime.top, iStep, gRCNibp_InterTime.bottom - gRCNibp_InterTime.top);
	//右边为剩余时间
	SetBrushColor(hdc, COLOR_black);
	FillBox(hdc, gRCNibp_InterTime.left+iStep, gRCNibp_InterTime.top, gRCNibp_InterTime.right - (gRCNibp_InterTime.left+iStep), gRCNibp_InterTime.bottom - gRCNibp_InterTime.top);
	
	//剩余时间
	SetTextColor(hdc, gCfgNibp.iColor);
	SetBkColor(hdc, COLOR_black);
// 	SelectFont(hdc, gFontSystem);
	if(bType ==1){
		//Stat
		DrawText(hdc, "5min", -1, &gRCNibp_InterTime, DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	}
	else if(bType ==2) {
		//Puncture
		if(gCfgNibp.bObject == NIBP_OBJECT_BABY)
			DrawText(hdc, "80s", -1, &gRCNibp_InterTime, DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE);	
		else
			DrawText(hdc, "170s", -1, &gRCNibp_InterTime, DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE);	
	}
	else{
		//Auto
		DrawText(hdc, strAutoTime[TransformTimeToIndex(gCfgNibp.wInterval)], -1, &gRCNibp_InterTime, DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	}

	//画刻度 : |__|__|__|__|
// 	SetPenColor(hdcParaMem, gCfgNibp.iColor);
	MoveTo(hdc, gRCNibp_InterTime.left, gRCNibp_InterTime.bottom);
	LineTo(hdc, gRCNibp_InterTime.right, gRCNibp_InterTime.bottom);
	
	MoveTo(hdc, gRCNibp_InterTime.left, gRCNibp_InterTime.bottom);
	LineTo(hdc, gRCNibp_InterTime.left, gRCNibp_InterTime.bottom - (gRCNibp_InterTime.bottom-gRCNibp_InterTime.top)/2);
	
	MoveTo(hdc, gRCNibp_InterTime.right, gRCNibp_InterTime.bottom);
	LineTo(hdc, gRCNibp_InterTime.right, gRCNibp_InterTime.bottom - (gRCNibp_InterTime.bottom-gRCNibp_InterTime.top)/2);
			
	//Center
	MoveTo(hdc, gRCNibp_InterTime.left + (gRCNibp_InterTime.right - gRCNibp_InterTime.left)/2, gRCNibp_InterTime.bottom);
	LineTo(hdc, gRCNibp_InterTime.left + (gRCNibp_InterTime.right - gRCNibp_InterTime.left)/2, gRCNibp_InterTime.top);
	
	MoveTo(hdc, gRCNibp_InterTime.left + (gRCNibp_InterTime.right - gRCNibp_InterTime.left)/4, gRCNibp_InterTime.bottom);
	LineTo(hdc, gRCNibp_InterTime.left + (gRCNibp_InterTime.right - gRCNibp_InterTime.left)/4, gRCNibp_InterTime.bottom - (gRCNibp_InterTime.bottom-gRCNibp_InterTime.top)/2);
	
	MoveTo(hdc, gRCNibp_InterTime.left + (gRCNibp_InterTime.right - gRCNibp_InterTime.left)/4*3, gRCNibp_InterTime.bottom);
	LineTo(hdc, gRCNibp_InterTime.left + (gRCNibp_InterTime.right - gRCNibp_InterTime.left)/4*3, gRCNibp_InterTime.bottom - (gRCNibp_InterTime.bottom-gRCNibp_InterTime.top)/2);
	
	Rectangle(hdc, gRCNibp_InterTime.left, gRCNibp_InterTime.top, gRCNibp_InterTime.right, gRCNibp_InterTime.bottom);

	return 0;
}

//计时器
extern int giNibpTimeCount_Auto;
extern int giNibpTimeCount_Stat;
extern int giNibpTimeCount_Punc;	
//报警暂停状态倒计时
extern int giSuspendTime;
extern BOOL gbSuspending;
/**
	显示NIBP 测量倒计时(自动,连续,穿刺)
	单位:秒
	bType: 0-Auto, 1-Stat, 2-Puncture
*/
static int ViewNibp_InterTime(HDC hdc)
{
	unsigned char strInterLabel[20]={0};
	unsigned char strInterTime[30]={0};
	int iTimeMax = 0;
	int iTimeCount = 0;
	
	switch(gCfgNibp.bMode){
		case NIBP_MODE_AUTO:{
			iTimeMax = gCfgNibp.wInterval*60;
			iTimeCount = giNibpTimeCount_Auto;
			//图形显示
			ViewNibpCycleGraph(hdc, iTimeCount,iTimeMax,0);
		}break;
		case NIBP_MODE_SERIES:{
			iTimeMax = NIBP_STAT_TIME;
			iTimeCount = giNibpTimeCount_Stat;
			ViewNibpCycleGraph(hdc, iTimeCount,iTimeMax,1);
		}break;	
		case NIBP_MODE_PUNCTURE:{
			if(gCfgNibp.bObject == NIBP_OBJECT_BABY){
				iTimeMax = NIBP_PUNCTURE_TIME_BABY;
			}
			else{
				iTimeMax = NIBP_PUNCTURE_TIME_ADULT;
			}
			iTimeCount = giNibpTimeCount_Punc;
			ViewNibpCycleGraph(hdc, iTimeCount,iTimeMax,2);
		}break;
		default:{
			return 0;
		}break;
	}
	
	
	//文字显示		
// 	GetStringFromResFile(gsLanguageRes, "GENERAL", "interval_time", strInterLabel, sizeof strInterLabel);	
// 	snprintf(strInterTime, sizeof strInterTime, "%s %d", strInterLabel, iTimeMax - iTimeCount);		
// 	SelectFont(hdcParaMem, gFontSystem);
//  	DrawText(hdcParaMem, strInterTime, -1, &gRCNibp_InterTime, DT_NOCLIP | DT_BOTTOM | DT_LEFT | DT_SINGLELINE);
		
	return 0;
}

//显示NIBP参数
static int ViewNibp(HDC hdc, BYTE bInterface, BOOL bMemCopy)
{
	unsigned char strLabel[10]={0};
	unsigned char strUnit[10]={0};
	int iSys, iDia, iMean;
	double fSys, fDia, fMean;
	unsigned char strSys[5]={0}, strDia[5]={0},strMean[8]={0}, strPress[5]={0};
	
	
	static BOOL bFlashData = FALSE;

	unsigned char strTime[10]={0};
	static BOOL bFlashSys = FALSE;
	static BOOL bFlashMean = FALSE;
	static BOOL bFlashDia = FALSE;
	unsigned char strMode[20]={0};
	unsigned char *strModeIndex[]={"mode_manual", "mode_auto", "mode_series", "mode_puncture"};
	unsigned char strObject[20]={0};
	unsigned char *strObjectIndex[]={"adult", "child", "baby"};
	unsigned char *strObjectIndex_ani[]={"big_animal", "middle_animal", "small_animal"};
	unsigned char strLimit_h[5]={0};
	unsigned char strLimit_l[5]={0};

	if(gbViewDemoData){
		iSys=115;
		iDia=75;
		iMean=95;
	}else{
		//mmHg
		iSys = gValueNibp.wSys;
		iDia = gValueNibp.wDia;
		iMean = gValueNibp.wMean;
	}
	//kPa
	fSys = (double)iSys / mmHgTokPa;
	fDia = (double)iDia / mmHgTokPa;
	fMean = (double)iMean / mmHgTokPa;
	
	if(iSys !=0 && iDia !=0 && iMean !=0){
		switch(gCfgNibp.bUnit){
			case 0:{//mmHg
				snprintf(strSys, sizeof strSys, "%d",iSys);
				snprintf(strDia, sizeof strDia, "%d",iDia);
				snprintf(strMean, sizeof strMean, "(%d)", iMean);
			}break;
			case 1:{//kPa
				snprintf(strSys, sizeof strSys, "%.1f",fSys);
				snprintf(strDia, sizeof strDia, "%.1f",fDia);
				snprintf(strMean, sizeof strMean, "(%.1f)", fMean);
			}break;
		}
	}
	else{
		switch(gCfgNibp.bUnit){
			case 0:{//mmHg
				snprintf(strSys, sizeof strSys, "---");
				snprintf(strDia, sizeof strDia, "---");
				snprintf(strMean, sizeof strMean, "(---)");
			}break;
			case 1:{//kPa
				snprintf(strSys, sizeof strSys, "--.-");
				snprintf(strDia, sizeof strDia, "--.-");
				snprintf(strMean, sizeof strMean, "(--.-)");
			}break;
		}
	}
	
 //	snprintf(strSys, sizeof strSys, "55.5");
 //	snprintf(strDia, sizeof strDia, "55.5");
// 	snprintf(strMean, sizeof strMean, "(55.5)"); 
	
	//设置DC属性
	SetTextColor(hdc, gCfgNibp.iColor);

  	SelectFont(hdc, gFontEnglish_16);
// 	SelectFont(hdc, gFontSystem);
	
	//Label
	memset(strLabel, 0, sizeof strLabel);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "nibp", strLabel, sizeof strLabel);	
	memset(strUnit, 0, sizeof strUnit);
	if(gCfgNibp.bUnit == NIBP_UNIT_MMHG)
		GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "unit_mmhg", strUnit, sizeof strUnit);	
	else
		GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "unit_kpa", strUnit, sizeof strUnit);	
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "NIBP (%s)", strUnit);
	DrawText(hdc, strInfo, -1, &gRCNibp_Label, DT_NOCLIP | DT_VCENTER | DT_LEFT | DT_SINGLELINE);
//	DrawText(hdc, strInfo, -1, &gRCNibp_Unit, DT_NOCLIP | DT_CENTER |  DT_SINGLELINE);
	SelectFont(hdc, gFontEnglish_16);
	//显示信息
	ViewNibp_Info(hdc);
	
	//显示测量时间
	ViewNibp_Time(hdc);
	
	//显示 Alarm Icon
 	if(gCfgNibp.bAlmControl == ALARM_OFF){
 		FillBoxWithBitmap(hdc, gRCNibp_AlmIcon.left+1,gRCNibp_AlmIcon.top+1, 16, 16, &gIconAlarmOff);
 	}

 	//显示Mode
// 	SelectFont(hdc, gFontSystem); 	
  	GetStringFromResFile(gsLanguageRes, "NIBP", strModeIndex[gCfgNibp.bMode], strMode, sizeof strMode);
  	DrawText(hdc, strMode, -1, &gRCNibp_Mode, DT_NOCLIP | DT_VCENTER | DT_CENTER| DT_SINGLELINE);

	//显示测量对象
	if(gbMonitorObject ==0)
		GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, strObjectIndex[gCfgNibp.bObject], strObject, sizeof strObject);
	else
		GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, strObjectIndex_ani[gCfgNibp.bObject], strObject, sizeof strObject);
	DrawText(hdc, strObject, -1, &gRCNibp_Object, DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	
	//显示倒计时
	ViewNibp_InterTime(hdc);
		
	//显示充气压力
	if(gValueNibp.bStatus!=NIBP_SYSTEM_IDLE){
		if(gCfgNibp.bUnit == NIBP_UNIT_MMHG) {
			snprintf(strPress, sizeof strPress, "%d", gValueNibp.wPressure);
		}
		else{
			snprintf(strPress, sizeof strPress, "%.1f", (float)gValueNibp.wPressure / mmHgTokPa);
		}
 		DrawText(hdc, strPress, -1, &gRCNibp_Pressure, DT_NOCLIP | DT_VCENTER | DT_RIGHT | DT_SINGLELINE);
	}

		
	//Sys/Dia Label
	if(gCfgSystem.bInterface==SCREEN_BIGCHAR){
		SelectFont(hdc, gFontEnglish_16);
		
		DrawText(hdc, "Sys/Dia", -1, &gRCNibp_label_SysDia, DT_NOCLIP | DT_TOP | DT_CENTER | DT_SINGLELINE);
				//Mean Label
	  	DrawText(hdc, "Mean", -1, &gRCNibp_label_Mean, DT_NOCLIP | DT_TOP | DT_CENTER | DT_SINGLELINE);
	
	}else{
		SelectFont(hdc, gFontSmall);
		
		DrawText(hdc, "Sys/Dia", -1, &gRCNibp_label_SysDia, DT_NOCLIP | DT_TOP | DT_CENTER | DT_SINGLELINE);
				//Mean Label
	  	DrawText(hdc, "Mean", -1, &gRCNibp_label_Mean, DT_NOCLIP | DT_TOP | DT_CENTER | DT_SINGLELINE);
	
	}
	//显示分割符号
	
#if SCREEN_640
	if(gCfgSystem.bInterface==SCREEN_BIGCHAR){
		if(gCfgNibp.bUnit == NIBP_UNIT_MMHG)
			SelectFont(hdc, gFontTTF_75);
		else
			SelectFont(hdc, gFontTTF_70);
		
	}else{
			SelectFont(hdc, gFontTTF_30Bk);
	}
#else
	if(gCfgSystem.bInterface==SCREEN_BIGCHAR){
		if(gCfgNibp.bUnit == NIBP_UNIT_MMHG)
			SelectFont(hdc, gFontTTF_90Bk);
		else
			SelectFont(hdc, gFontTTF_75);
		
	}else{
			SelectFont(hdc, gFontTTF_40Bk);
	}
#endif

	//显示/
	DrawText(hdc, "/", -1, &gRCNibp_Inter, DT_NOCLIP | DT_BOTTOM | DT_CENTER | DT_SINGLELINE);

	//显示 Sys
	if((gbAlarmNibp_Sys)&& (gCfgNibp.bAlmControl != ALARM_OFF)){
		if(bFlashSys){
			SetTextColor(hdc, ALARMFLASH_COLOR);
		}
		else{
			SetTextColor(hdc, gCfgNibp.iColor);		
		}
		bFlashSys = !bFlashSys;
	}
	else{
		SetTextColor(hdc, gCfgNibp.iColor);			
	}
//	DrawText(hdc, strSysDia, -1, &gRCNibp_Inter, DT_NOCLIP | DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
	DrawText(hdc, strSys, -1, &gRCNibp_Sys, DT_NOCLIP | DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
	//显示 Dia
	if((gbAlarmNibp_Dia)&& (gCfgNibp.bAlmControl != ALARM_OFF)){
		if(bFlashDia){
			SetTextColor(hdc, ALARMFLASH_COLOR);
		}
		else{
			SetTextColor(hdc, gCfgNibp.iColor);		
		}
		bFlashDia = !bFlashDia;
	}
	else{
		SetTextColor(hdc, gCfgNibp.iColor);			
	}
	DrawText(hdc, strDia, -1, &gRCNibp_Dia, DT_NOCLIP | DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
	

	
//Mean
#if SCREEN_640
	if(gCfgSystem.bInterface==SCREEN_BIGCHAR)
		SelectFont(hdc, gFontTTF_40Bk);
	else
		SelectFont(hdc, gFontTTF_20Bk);
#else
	if(gCfgSystem.bInterface==SCREEN_BIGCHAR)
		SelectFont(hdc, gFontTTF_50Bk);
	else
		SelectFont(hdc, gFontTTF_30Bk);
#endif

//显示mean
	if((gbAlarmNibp_Mean )&& (gCfgNibp.bAlmControl != ALARM_OFF)){
		if(bFlashMean){
			SetTextColor(hdc, ALARMFLASH_COLOR);
		}
		else{
			SetTextColor(hdc, gCfgNibp.iColor);		
		}
		bFlashMean = !bFlashMean;
	}
	else{
		SetTextColor(hdc, gCfgNibp.iColor);			
	}
 	DrawText(hdc, strMean, -1, &gRCNibp_Mean, DT_NOCLIP | DT_CENTER | DT_BOTTOM | DT_SINGLELINE);


	//Alarm Limit
	SelectFont(hdc, gFontSmall);
	SetTextColor(hdc, PARACOL_LIMIT);
	if(gCfgAlarm.bViewLimit){
		//Sys
		memset(strLimit_h, 0, sizeof strLimit_h);
		memset(strLimit_l, 0, sizeof strLimit_l);
		switch(gCfgNibp.bUnit){
			case 0:{//mmHg
				snprintf(strLimit_h, sizeof strLimit_h, "%d", gCfgNibp.wHigh_Sys);
				snprintf(strLimit_l, sizeof strLimit_l, "%d", gCfgNibp.wLow_Sys);
			}break;
			case 1:{//kPa
				snprintf(strLimit_h, sizeof strLimit_h, "%.1f", (gCfgNibp.wHigh_Sys)/mmHgTokPa);
				snprintf(strLimit_l, sizeof strLimit_l, "%.1f", (gCfgNibp.wLow_Sys)/mmHgTokPa);
			}break;
		}
		DrawText(hdc, strLimit_h, -1, &gRCNibp_limit_Sys, DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);
		DrawText(hdc, strLimit_l, -1, &gRCNibp_limit_Sys, DT_NOCLIP | DT_BOTTOM | DT_LEFT | DT_SINGLELINE);	
		//Dia
		memset(strLimit_h, 0, sizeof strLimit_h);
		memset(strLimit_l, 0, sizeof strLimit_l);
		switch(gCfgNibp.bUnit){
			case 0:{//mmHg
				snprintf(strLimit_h, sizeof strLimit_h, "%d", gCfgNibp.wHigh_Dia);
				snprintf(strLimit_l, sizeof strLimit_l, "%d", gCfgNibp.wLow_Dia);
			}break;
			case 1:{//kPa
				snprintf(strLimit_h, sizeof strLimit_h, "%.1f", (gCfgNibp.wHigh_Dia)/mmHgTokPa);
				snprintf(strLimit_l, sizeof strLimit_l, "%.1f", (gCfgNibp.wLow_Dia)/mmHgTokPa);
			}break;
		}
		DrawText(hdc, strLimit_h, -1, &gRCNibp_limit_Dia, DT_NOCLIP | DT_TOP | DT_RIGHT| DT_SINGLELINE);
		DrawText(hdc, strLimit_l, -1, &gRCNibp_limit_Dia, DT_NOCLIP | DT_BOTTOM | DT_RIGHT| DT_SINGLELINE);	
 		//Mean
 		memset(strLimit_h, 0, sizeof strLimit_h);
 		memset(strLimit_l, 0, sizeof strLimit_l);
 		switch(gCfgNibp.bUnit){
 			case 0:{//mmHg
 				snprintf(strLimit_h, sizeof strLimit_h, "%d", gCfgNibp.wHigh_Mean);
 				snprintf(strLimit_l, sizeof strLimit_l, "%d", gCfgNibp.wLow_Mean);
 			}break;
 			case 1:{//kPa
 				snprintf(strLimit_h, sizeof strLimit_h, "%.1f", (gCfgNibp.wHigh_Mean)/mmHgTokPa);
 				snprintf(strLimit_l, sizeof strLimit_l, "%.1f", (gCfgNibp.wLow_Mean)/mmHgTokPa);
 			}break;
 		}
 		DrawText(hdc, strLimit_h, -1, &gRCNibp_limit_Mean, DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);
 		DrawText(hdc, strLimit_l, -1, &gRCNibp_limit_Mean, DT_NOCLIP | DT_BOTTOM | DT_LEFT| DT_SINGLELINE);			
	}

	//绘制框架
	if(gCfgSystem.bInterface==SCREEN_BIGCHAR)
		Rectangle(hdc, gRCNibp.left, gRCNibp.top, gRCNibp.right, gRCNibp.bottom);
	else
		Rectangle(hdc, gRCNibp.left, gRCNibp.top, gRCNibp.right+5, gRCNibp.bottom);
	
	
	if(bDebugRect){
//NIBP Label
	SetPenColor(hdc, COLOR_yellow);
	Rectangle(hdc, gRCNibp_Label.left, gRCNibp_Label.top, gRCNibp_Label.right, gRCNibp_Label.bottom);
//Unit
//	Rectangle(hdc, gRCNibp_Unit.left, gRCNibp_Unit.top, gRCNibp_Unit.right, gRCNibp_Unit.bottom);
//Limit
	SetPenColor(hdc, COLOR_darkgreen);
	Rectangle(hdc, gRCNibp_limit_Sys.left, gRCNibp_limit_Sys.top, gRCNibp_limit_Sys.right, gRCNibp_limit_Sys.bottom);
	Rectangle(hdc, gRCNibp_limit_Dia.left, gRCNibp_limit_Dia.top, gRCNibp_limit_Dia.right, gRCNibp_limit_Dia.bottom);
	Rectangle(hdc, gRCNibp_limit_Mean.left, gRCNibp_limit_Mean.top, gRCNibp_limit_Mean.right, gRCNibp_limit_Mean.bottom);
//Alarm Icon
	SetPenColor(hdc, COLOR_darkcyan);
	Rectangle(hdc, gRCNibp_AlmIcon.left, gRCNibp_AlmIcon.top, gRCNibp_AlmIcon.right, gRCNibp_AlmIcon.bottom);
//Mode
	SetPenColor(hdc, COLOR_darkred);
	Rectangle(hdc, gRCNibp_Mode.left, gRCNibp_Mode.top, gRCNibp_Mode.right, gRCNibp_Mode.bottom);
//Object
	SetPenColor(hdc, COLOR_darkmagenta);
	Rectangle(hdc, gRCNibp_Object.left, gRCNibp_Object.top, gRCNibp_Object.right, gRCNibp_Object.bottom);

//NIBP --/--
	SetPenColor(hdc, COLOR_blue );
	Rectangle(hdc, gRCNibp_Inter.left, gRCNibp_Inter.top, gRCNibp_Inter.right, gRCNibp_Inter.bottom);
	Rectangle(hdc, gRCNibp_Sys.left, gRCNibp_Sys.top, gRCNibp_Sys.right, gRCNibp_Sys.bottom);
	Rectangle(hdc, gRCNibp_Dia.left, gRCNibp_Dia.top, gRCNibp_Dia.right, gRCNibp_Dia.bottom);

//NIBP Data -- Mean
	Rectangle(hdc, gRCNibp_Mean.left, gRCNibp_Mean.top, gRCNibp_Mean.right, gRCNibp_Mean.bottom);

	//lable sysdia
	Rectangle(hdc, gRCNibp_label_SysDia.left, gRCNibp_label_SysDia.top, gRCNibp_label_SysDia.right, gRCNibp_label_SysDia.bottom);
	//lable sysdia
	Rectangle(hdc, gRCNibp_label_Mean.left, gRCNibp_label_Mean.top, gRCNibp_label_Mean.right, gRCNibp_label_Mean.bottom);


//Pressure
	SetPenColor(hdc, COLOR_lightwhite );
	Rectangle(hdc, gRCNibp_Pressure.left, gRCNibp_Pressure.top, gRCNibp_Pressure.right, gRCNibp_Pressure.bottom);

//测量时间
	SetPenColor(hdc, COLOR_cyan);
	Rectangle(hdc, gRCNibp_Time.left, gRCNibp_Time.top, gRCNibp_Time.right, gRCNibp_Time.bottom);

//测量信息,包括报警
	SetPenColor(hdc, COLOR_darkgray);
	Rectangle(hdc, gRCNibp_Info.left, gRCNibp_Info.top, gRCNibp_Info.right, gRCNibp_Info.bottom);

//Time,倒计时
	SetPenColor(hdc, COLOR_darkyellow );
	Rectangle(hdc, gRCNibp_InterTime.left, gRCNibp_InterTime.top, gRCNibp_InterTime.right, gRCNibp_InterTime.bottom);

	} 

 	//拷贝DC
 	if(Draw_Mem){
		if(gCfgSystem.bInterface!=SCREEN_BIGCHAR){
 			BitBlt(hdc, 
	 		       gRCNibp.left, gRCNibp.top, RECTW(gRCNibp)+1, RECTH(gRCNibp)+1, 
	 		       hdcPara, 
	 		       gRCNibp.left, gRCNibp.top, 
	 		       0);
		}else{
			BitBlt(hdcParaMemExt[3], 
			       gRcParasExt[3].left, gRcParasExt[3].top, RECTW(gRcParasExt[3])+1, RECTH(gRcParasExt[3])+1, 
			       hdcParaExt[3], 
			       gRcParasExt[3].left, gRcParasExt[3].top, 
			       0);
		}
 	}

	return 0;
}

//填充列表内容
static int FillNListItem(int iRowIndex, const PTRENDDATA pTrendData)
{
	S_TIME sTmpTime;
	int i;
	
	if(pTrendData == NULL) return -1;
	
	//0是Label
	if(iRowIndex<1) iRowIndex = 1;
	
	//Time
	sTmpTime = pTrendData->sTime;
	snprintf((unsigned char *)(gTableEvent[iRowIndex][0].strValue), ITEM_TXTLENGTH, "%.2d/%.2d %.2d:%.2d", sTmpTime.bMonth, sTmpTime.bDay, sTmpTime.bHour, sTmpTime.bMin);
	
	//NIBP
	if(gCfgNibp.bUnit == NIBP_UNIT_MMHG){
		snprintf((unsigned char *)(gTableEvent[iRowIndex][1].strValue), ITEM_TXTLENGTH, "%d", pTrendData->sNIBP.wSys);
		snprintf((unsigned char *)(gTableEvent[iRowIndex][2].strValue), ITEM_TXTLENGTH, "%d", pTrendData->sNIBP.wDia);
		snprintf((unsigned char *)(gTableEvent[iRowIndex][3].strValue), ITEM_TXTLENGTH, "%d", pTrendData->sNIBP.wMean);
	}
	else{
		snprintf((unsigned char *)(gTableEvent[iRowIndex][1].strValue), ITEM_TXTLENGTH, "%.1f", (pTrendData->sNIBP.wSys) / mmHgTokPa);
		snprintf((unsigned char *)(gTableEvent[iRowIndex][2].strValue), ITEM_TXTLENGTH, "%.1f", (pTrendData->sNIBP.wDia) / mmHgTokPa);
		snprintf((unsigned char *)(gTableEvent[iRowIndex][3].strValue), ITEM_TXTLENGTH, "%.1f", (pTrendData->sNIBP.wMean) / mmHgTokPa);	
	}
	
	//HR
	if(pTrendData->dwHaveFlag & HAVEHR)
		snprintf((unsigned char *)(gTableEvent[iRowIndex][4].strValue), ITEM_TXTLENGTH, "%d", pTrendData->wHR);
	else
//		snprintf((unsigned char *)(gTableEvent[iRowIndex][4].strValue), ITEM_TXTLENGTH, "%d",gValueNibp.wPR);
		snprintf((unsigned char *)(gTableEvent[iRowIndex][4].strValue), ITEM_TXTLENGTH, "---");
	
	return 0;
}

/*
	思路:用趋势数组中最近的NLIST_COUNT次NIBP记录填充
*/
static int FillNListDatas()
{
	WORD wHead, wTail, wSize;
	BOOL bLoop;
	int i, j;
	int iCount;
		
	//初始化表格内容
	for(i=0; i<NLIST_ROW_COUNT; i++){
		for(j=0; j<NLIST_COL_COUNT; j++){
			memset(gTableEvent[i][j].strValue, 0, ITEM_TXTLENGTH);
			memset(gTableEvent[i][j].strValue, 0, ITEM_TXTLENGTH);
		}
	}

	//填写表头
	GetStringFromResFiles(gsLanguageRes, "GENERAL", "ntime", gTableEvent[0][0].strValue, ITEM_TXTLENGTH,"TIME");	
	GetStringFromResFiles(gsLanguageRes, "GENERAL", "nsys", gTableEvent[0][1].strValue, ITEM_TXTLENGTH,"SYS");	
	GetStringFromResFiles(gsLanguageRes, "GENERAL", "ndia", gTableEvent[0][2].strValue, ITEM_TXTLENGTH,"DIA");	
	GetStringFromResFiles(gsLanguageRes, "GENERAL", "nmean", gTableEvent[0][3].strValue, ITEM_TXTLENGTH,"MEAN");	
	GetStringFromResFiles(gsLanguageRes, "GENERAL", "nhr", gTableEvent[0][4].strValue, ITEM_TXTLENGTH,"PR");	

	if(gTrendDatas.wCount == 0){
// 		printf("%s:%d No Trend data.\n", __FILE__, __LINE__);
		return -1;	
	}

	//从趋势数组中取出前 NLIST_COUNT 条NIBP数据
	wHead = gTrendDatas.wHeadIndex;
	wTail = gTrendDatas.wTailIndex;
	bLoop = gTrendDatas.bLoop;
	wSize = MAXTRENDCOUNT;
		

	iCount =1;
	if(wHead < wTail){
		for(i=wHead; i>=0; i--){
			if(iCount < NLIST_ROW_COUNT){
				if((gTrendDatas.Datas[i].bLogFlag && TRENDNIBP) && (gTrendDatas.Datas[i].dwHaveFlag & HAVENIBP)){
					FillNListItem(iCount, &(gTrendDatas.Datas[i]));		
					iCount++;
				}
			}
			else{
				goto SEARCH_OVER;
			}
		}
		for(i=(wSize-1); i>=wTail; i--){
			if(iCount < NLIST_ROW_COUNT){
				if((gTrendDatas.Datas[i].bLogFlag && TRENDNIBP) && (gTrendDatas.Datas[i].dwHaveFlag & HAVENIBP)){
					FillNListItem(iCount, &(gTrendDatas.Datas[i]));		
					iCount++;
				}
			}
			else{
				goto SEARCH_OVER;
			}
		}
	}
	else if(bLoop){
		for(i=wHead; i>=wTail; i--){
			if(iCount < NLIST_ROW_COUNT){
				if((gTrendDatas.Datas[i].bLogFlag && TRENDNIBP) && (gTrendDatas.Datas[i].dwHaveFlag & HAVENIBP)){
// 					printf("----Found NIBP Trend index %d.\n", i);
					FillNListItem(iCount, &(gTrendDatas.Datas[i]));		
					iCount++;
				}
			}
			else{
				goto SEARCH_OVER;
			}
		}
	}
	else{
		for(i=wHead; i>wTail; i--){
			if(iCount < NLIST_ROW_COUNT){
				if((gTrendDatas.Datas[i].bLogFlag && TRENDNIBP) && (gTrendDatas.Datas[i].dwHaveFlag & HAVENIBP)){
// 					printf("----Found NIBP Trend index %d.\n", i);
					FillNListItem(iCount, &(gTrendDatas.Datas[i]));		
					iCount++;
				}
			}
			else{
				goto SEARCH_OVER;
			}
		}
	}
	
SEARCH_OVER:
	
		return 0;
}

/*
	绘制框架
*/
static int DrawFrame(HDC hdc)
{
	int i, j;
	
	if(hdc == (HDC)NULL) return -1;
	
	SetTextColor(hdc, gCfgNibp.iColor);
	
	SelectFont(hdc, gFontSystem);	
	
	Rectangle(hdc, gRCNibp_List.left, gRCNibp_List.top, gRCNibp_List.right, gRCNibp_List.bottom);
							
	//仅绘制标签行
	for(i=0; i<1; i++){	
		MoveTo(hdc, gTableEvent[0][i].rect.left, gTableEvent[0][i].rect.bottom);
		LineTo(hdc, gRCNibp_List.right, gTableEvent[0][i].rect.bottom);		
	}
	//列
	for(i=0; i<NLIST_COL_COUNT-1; i++){
		MoveTo(hdc, gTableEvent[0][i].rect.right, gTableEvent[0][i].rect.top);
		LineTo(hdc, gTableEvent[0][i].rect.right, gRCNibp_List.bottom);
	}
	

	/*
	//事件表格
	for(i=0; i<NLIST_ROW_COUNT; i++){
		for(j=0; j<NLIST_COL_COUNT; j++){
			Rectangle(hdc, 
				  gTableEvent[i][j].rect.left, 
				  gTableEvent[i][j].rect.top, 
				  gTableEvent[i][j].rect.right, 
				  gTableEvent[i][j].rect.bottom);
		}
	}
        */
	return 0;
}


static int InitRectNibpList()
{
	int i,j;
	int iWidth;
	int iHeight;
	//Nibp List 
	SetRect(&gRCNibp_List,
		 gRcParasExt[0].left, 
		 gRcParasExt[0].top,
		 gRcParasExt[0].right-1, 
		 gRcParasExt[0].bottom-1);
	
	//Time占两列
	iWidth = RECTW(gRCNibp_List) / (NLIST_COL_COUNT+1);
	iHeight = RECTH(gRCNibp_List) / NLIST_ROW_COUNT;
	
	for(i=0; i<NLIST_ROW_COUNT; i++){
		//Time
		SetRect(&(gTableEvent[i][0].rect), 
			  gRCNibp_List.left, 
			  gRCNibp_List.top+i*iHeight, 
			  gRCNibp_List.left+2*iWidth+10, 
			  gRCNibp_List.top+(i+1)*iHeight);
		//Other
		for(j=1; j<NLIST_COL_COUNT; j++){
			SetRect(&(gTableEvent[i][j].rect), 
				  gRCNibp_List.left+ (j+1)*iWidth+10, 
				  gRCNibp_List.top+i*iHeight, 
				  gRCNibp_List.left+(j+2)*iWidth+10, 
				  gRCNibp_List.top+(i+1)*iHeight);
		}
	}

	return 0;
}

//显示NIBP列表
static int ViewNibpList(HDC hdc, BYTE bInterface, BOOL bMemCopy)
{
	int i,j;
	
	//填充数据
	FillNListDatas();
	
	//表头
	SelectFont(hdc, gFontSystem);
	for(i=0; i<NLIST_COL_COUNT; i++){
		DrawText(hdc, (unsigned char *)(gTableEvent[0][i].strValue), -1, 
			 &(gTableEvent[0][i].rect), 
			 DT_NOCLIP | DT_SINGLELINE | DT_VCENTER | DT_CENTER);
	}

	//内容
	for(i=1; i<NLIST_ROW_COUNT; i++){
		for(j=0; j<NLIST_COL_COUNT; j++){
			DrawText(hdc, (unsigned char *)(gTableEvent[i][j].strValue), -1, 
				 &(gTableEvent[i][j].rect), 
				 DT_NOCLIP | DT_SINGLELINE | DT_VCENTER | DT_CENTER);
		}
	}
	
	//绘制框架
	DrawFrame(hdc);
			     
	//绘制框架
	Rectangle(hdc, gRCNibp_List.left, gRCNibp_List.top, gRCNibp_List.right, gRCNibp_List.bottom);
	
// 	//拷贝DC
// 	if(bMemCopy){
// 		BitBlt(hdc, 
// 		       gRCNibp.left, gRCNibp.top, RECTW(gRCNibp)+1, RECTH(gRCNibp)+1, 
// 		       hdcPara, 
// 		       gRCNibp.left, gRCNibp.top, 
// 		       0);
// 	}
	if(Draw_Mem){
		BitBlt(hdcParaMemExt[0], 
			       gRcParasExt[0].left, gRcParasExt[0].top, RECTW(gRcParasExt[0])+1, RECTH(gRcParasExt[0])+1, 
			       hdcParaExt[0], 
			       gRcParasExt[0].left, gRcParasExt[0].top, 
			       0);
	}
	return 0;
}
		
		
//根据模块配置以及屏幕类型初始化参数显示区域
static int InitRectSpO2()
{
if(gCfgSystem.bInterface!=SCREEN_BIGCHAR)
		SetRect(&gRCSpO2AndPR, 
			 gRcParas.left, 
			 gRcParas.top+iLeftParaRectHeight*2, 
			 gRcParas.right-1, 
			 gRcParas.top+iLeftParaRectHeight*3);
else
	SetRect(&gRCSpO2AndPR, 
			 gRcParas.left, 
			 gRcParas.top+iLeftParaRectHeight, 
			 gRcParas.right-1, 
			 gRcParas.top+iLeftParaRectHeight*2);
		//相对坐标
		//Label
		SetRect(&gRCSpO2_Label, gRCSpO2AndPR.left+1,gRCSpO2AndPR.top+1,gRCSpO2AndPR.left+70 , gRCSpO2AndPR.top+17);
		//Unit
		SetRect(&gRCSpO2_Unit, gRCSpO2AndPR.left+1,gRCSpO2AndPR.top+23,gRCSpO2AndPR.left +40, gRCSpO2AndPR.top+35);
		//Limit
		SetRect(&gRCSpO2_Limit, gRCSpO2AndPR.left+1, gRCSpO2AndPR.top+30, gRCSpO2AndPR.left+30, gRCSpO2AndPR.top+55);
		//Data
// 		SetRect(&gRCSpO2_Data, gRCSpO2AndPR.left+70-giLeftDec,gRCSpO2AndPR.top+20,gRCSpO2AndPR.right-35,gRCSpO2AndPR.bottom-20);
		SetRect(&gRCSpO2_Data, gRCSpO2AndPR.left+30,gRCSpO2AndPR.top+1, gRCSpO2AndPR.right-30,gRCSpO2AndPR.bottom-17);
		//Alarm Icon
		SetRect(&gRCSpO2_AlmIcon, gRCSpO2AndPR.right-20,gRCSpO2AndPR.top+1,gRCSpO2AndPR.right, gRCSpO2AndPR.top+20);
		//Strength
		SetRect(&gRCSpO2_Strength, gRCSpO2AndPR.right-45,gRCSpO2AndPR.top+1,gRCSpO2AndPR.right-20, gRCSpO2AndPR.top+17);
		//Info
		SetRect(&gRCSpO2_Info, gRCSpO2AndPR.left+35-giLeftDec,gRCSpO2AndPR.bottom-20,gRCSpO2AndPR.right,gRCSpO2AndPR.bottom);
		//PR Label
		SetRect(&gRCPr_Label, gRCSpO2AndPR.left + gRCSpO2AndPR.right/2, gRCSpO2AndPR.top+1,gRCSpO2AndPR.right,gRCSpO2AndPR.top+17);
		//PR Data
		SetRect(&gRCPr_Data, gRCSpO2AndPR.right-85, gRCSpO2AndPR.bottom-30,gRCSpO2AndPR.right-25,gRCSpO2AndPR.bottom);
		//PR Alarm Icon
		SetRect(&gRCPr_AlmIcon, gRCSpO2AndPR.right-20,gRCSpO2AndPR.top+20,gRCSpO2AndPR.right,gRCSpO2AndPR.top+40);
		//PR Alarm limit
		SetRect(&gRCPr_Limit,  gRCSpO2AndPR.right-25,gRCSpO2AndPR.bottom-25,gRCSpO2AndPR.right,gRCSpO2AndPR.bottom);
		
		//Column
		SetRect(&gRCSpO2_Column, gRCSpO2AndPR.left+225-giLeftDec,gRCSpO2AndPR.top+3,gRCSpO2AndPR.left+240-giLeftDec,gRCSpO2AndPR.top+103);
	
	return 0;
}

//显示SpO2 & PR参数
static int ViewSpO2(HDC hdc, BYTE bInterface, BOOL bMemCopy)
{
	unsigned char strLabel[10]={0};
	unsigned char strSpO2[4]={0};
	int i;
	static BOOL bFlashSpO2 = FALSE;
	unsigned char strPr[6]={0};
	static BOOL bFlashPr = FALSE;
	unsigned char strLimit[5]={0};
	//获得数据 SpO2
	if((!gValueSpO2.bOff) && (gValueSpO2.bSpO2 !=0)){
		snprintf(strSpO2, sizeof strSpO2, "%d", gValueSpO2.bSpO2);
	}
	else{
  		snprintf(strSpO2, sizeof strSpO2, "---");
	}
	if(gValueSpO2.bInvalSpo2)snprintf(strSpO2, sizeof strSpO2, "---");
//  	snprintf(strSpO2, sizeof strSpO2, "100");
	
	//获得数据 PR
	
	if((gValuePulse.bOff !=TRUE) && (gValuePulse.wPR !=0)){
		snprintf(strPr, sizeof strPr, "(%d)", gValuePulse.wPR);
	}
	else{
 		snprintf(strPr, sizeof strPr, "(---)");
	}
	if(gValuePulse.bInvalPR) snprintf(strPr, sizeof strPr, "(---)");
//  	snprintf(strPr, sizeof strPr, "255");

	//设置DC属性
	SetTextColor(hdc, gCfgSpO2.iColor);
	
	SelectFont(hdc, gFontEnglish_16);
	
	//Label
//	if(bDebugRect) Rectangle(hdc, gRCSpO2_Label.left, gRCSpO2_Label.top, gRCSpO2_Label.right, gRCSpO2_Label.bottom);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "spo2", strLabel, sizeof strLabel);	
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%s (%%)", strLabel);
	DrawText(hdc, strInfo, -1, &gRCSpO2_Label, DT_NOCLIP | DT_VCENTER | DT_LEFT | DT_SINGLELINE);
//	DrawText(hdc, "sáb", -1, &gRCSpO2_Label, DT_NOCLIP | DT_VCENTER | DT_LEFT | DT_SINGLELINE);

	//Alarm Icon
//	if(bDebugRect) Rectangle(hdc, gRCSpO2_AlmIcon.left, gRCSpO2_AlmIcon.top, gRCSpO2_AlmIcon.right, gRCSpO2_AlmIcon.bottom);
	if(gCfgSpO2.bAlmControl == ALARM_OFF){
		FillBoxWithBitmap(hdc, gRCSpO2_AlmIcon.left+1,gRCSpO2_AlmIcon.top+1, 16, 16, &gIconAlarmOff);
	}

	//SpO2 Info (Alarm)
//	if(bDebugRect) Rectangle(hdc, gRCSpO2_Info.left, gRCSpO2_Info.top, gRCSpO2_Info.right, gRCSpO2_Info.bottom);
	memset(strInfo, 0, sizeof strInfo);

	if(gValueSpO2.bErrCode & SPO2_INTERFERE){
		GetStringFromResFile(gsLanguageRes, "TEC_INFO", "spo2_error_interfere", strInfo, sizeof strInfo);	
	}else if(gValueSpO2.bErrCode & SPO2_SEARCHPULSE){
		GetStringFromResFile(gsLanguageRes, "TEC_INFO", "spo2_error_search", strInfo, sizeof strInfo);		
	}else if(gValueSpO2.bErrCode & SPO2_HIGHLIGHT){
		GetStringFromResFile(gsLanguageRes, "TEC_INFO", "spo2_error_highlight", strInfo, sizeof strInfo);		
	}else if(gValueSpO2.bErrCode & SPO2_WEAKSIGNAL){
		GetStringFromResFile(gsLanguageRes, "TEC_INFO", "spo2_error_weaksignal", strInfo, sizeof strInfo);
	}else if(gValueSpO2.bErrCode & SPO2_SIGNALBAD){
		GetStringFromResFile(gsLanguageRes, "TEC_INFO", "spo2_error_signal_bad", strInfo, sizeof strInfo);
	}
 	SelectFont(hdc, gFontSystem);
	DrawText(hdc, strInfo, -1, &gRCSpO2_Info, DT_NOCLIP | DT_VCENTER | DT_LEFT| DT_SINGLELINE);
		
	//Strength
//	if(bDebugRect) Rectangle(hdc, gRCSpO2_Strength.left, gRCSpO2_Strength.top, gRCSpO2_Strength.right, gRCSpO2_Strength.bottom);
	SetPenColor(hdc, PARACOL_FRAME);
	for(i=0; i<8; i++){
		MoveTo(hdc, gRCSpO2_Strength.left+i*3, gRCSpO2_Strength.bottom);
		LineTo(hdc, gRCSpO2_Strength.left+i*3, gRCSpO2_Strength.bottom - i);
	}
	SetPenColor(hdc, COLOR_red);
	for(i=0; i<gValueSpO2.bStrength; i++){
		MoveTo(hdc, gRCSpO2_Strength.left+i*3, gRCSpO2_Strength.bottom);
		LineTo(hdc, gRCSpO2_Strength.left+i*3, gRCSpO2_Strength.bottom - i);
	}
	SetPenColor(hdc, PARACOL_FRAME);
	
	//显示数据
	//SpO2
	if(gbAlarmSpO2_Data && (gCfgSpO2.bAlmControl != ALARM_OFF)){
		if(bFlashSpO2){
			SetTextColor(hdc, ALARMFLASH_COLOR);
		}
		else{
			SetTextColor(hdc, gCfgSpO2.iColor);		
		}
		bFlashSpO2 = !bFlashSpO2;
	}
	else{
		SetTextColor(hdc, gCfgSpO2.iColor);			
	}
//	if(bDebugRect) Rectangle(hdc, gRCSpO2_Data.left, gRCSpO2_Data.top, gRCSpO2_Data.right, gRCSpO2_Data.bottom);
	if(gbHaveIbp){
#if SCREEN_1024
		SelectFont(hdc, gFontTTF_90Bk);
#elif	SCREEN_640
		SelectFont(hdc, gFontTTF_60);
#else
		SelectFont(hdc, gFontTTF_60);
#endif
		//DrawText(hdc, strSpO2, -1, &gRCSpO2_Data, DT_NOCLIP | DT_VCENTER | DT_LEFT | DT_SINGLELINE);
		DrawText(hdc, strSpO2, -1, &gRCSpO2_Data, DT_NOCLIP | DT_TOP | DT_CENTER | DT_SINGLELINE);
	}                                         
	else{
#if SCREEN_1024
		SelectFont(hdc, gFontTTF_120);
#elif	SCREEN_640
		if(gCfgSystem.bInterface==SCREEN_BIGCHAR)
			SelectFont(hdc, gFontTTF_70);
		else
			SelectFont(hdc, gFontTTF_60);
#else
		if(gCfgSystem.bInterface==SCREEN_BIGCHAR)
			SelectFont(hdc, gFontTTF_90Bk);
		else
			SelectFont(hdc, gFontTTF_75);

#endif
		DrawText(hdc, strSpO2, -1, &gRCSpO2_Data, DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	}
	
	
	//Alarm Limit
//	if(bDebugRect) Rectangle(hdc, gRCSpO2_Limit.left, gRCSpO2_Limit.top, gRCSpO2_Limit.right, gRCSpO2_Limit.bottom);
	if(gCfgAlarm.bViewLimit){
		SelectFont(hdc, gFontSmall);
		SetTextColor(hdc, PARACOL_LIMIT);
		memset(strLimit, 0, sizeof strLimit);
		snprintf(strLimit, sizeof strLimit, "%d", gCfgSpO2.bHigh_SpO2);
		DrawText(hdc, strLimit, -1, &gRCSpO2_Limit, DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);
		memset(strLimit, 0, sizeof strLimit);
		snprintf(strLimit, sizeof strLimit, "%d", gCfgSpO2.bLow_SpO2);
		DrawText(hdc, strLimit, -1, &gRCSpO2_Limit, DT_NOCLIP | DT_BOTTOM | DT_LEFT | DT_SINGLELINE);
	}
	
	//当HR来源为ECG时，显示PR
// 	switch(gCfgEcg.bAlmSource){
// 		case HR_ALM_SRC_HR:{
	if(gCfgEcg.bAlmSource != HR_ALM_SRC_PR){
		SetTextColor(hdc, gCfgSpO2.iColor);
 	//	SelectFont(hdc, gFontEnglish_16);
		SelectFont(hdc, gFontSmall);
		//PR Label
//		if(bDebugRect) Rectangle(hdc, gRCPr_Label.left, gRCPr_Label.top, gRCPr_Label.right, gRCPr_Label.bottom);
		memset(strLabel, 0, sizeof strLabel);
		GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "pr", strLabel, sizeof strLabel);	
		/*
		memset(strInfo, 0, sizeof strInfo);
		
		if(gCfgSystem.bLanguage == LANGUAGE_SPANISH)
			snprintf(strInfo, sizeof strInfo, "%s (ppm)", strLabel);
		else
			snprintf(strInfo, sizeof strInfo, "%s (bpm)", strLabel);
		
 		DrawText(hdc, strInfo, -1, &gRCPr_Label, DT_NOCLIP | DT_VCENTER | DT_LEFT | DT_SINGLELINE);
	*/
	 	DrawText(hdc, strLabel, -1, &gRCPr_Data, DT_NOCLIP | DT_TOP| DT_RIGHT | DT_SINGLELINE);

//		if(bDebugRect) Rectangle(hdc, gRCPr_AlmIcon.left, gRCPr_AlmIcon.top, gRCPr_AlmIcon.right, gRCPr_AlmIcon.bottom);
		if(gCfgPulse.bAlmControl == ALARM_OFF){
			FillBoxWithBitmap(hdc, gRCPr_AlmIcon.left+1,gRCPr_AlmIcon.top+1, 16, 16, &gIconAlarmOff);
		}

		//Data
		if(gbAlarmPr_Data && (gCfgPulse.bAlmControl != ALARM_OFF)){
			if(bFlashPr){
				SetTextColor(hdc, ALARMFLASH_COLOR);
			}
			else{
				SetTextColor(hdc, gCfgPulse.iColor);		
			}
			bFlashPr = !bFlashPr;
		}
		else{
			SetTextColor(hdc, gCfgPulse.iColor);			
		}
		SelectFont(hdc, gFontTTF_20);
//		if(bDebugRect) Rectangle(hdc, gRCPr_Data.left, gRCPr_Data.top, gRCPr_Data.right, gRCPr_Data.bottom);
		DrawText(hdc, strPr, -1, &gRCPr_Data, DT_NOCLIP | DT_BOTTOM | DT_LEFT | DT_SINGLELINE);
		
		//Alarm Limit
//		if(bDebugRect) Rectangle(hdc, gRCPr_Limit.left, gRCPr_Limit.top, gRCPr_Limit.right, gRCPr_Limit.bottom);
		if(gCfgAlarm.bViewLimit){
			SelectFont(hdc, gFontSmall);
			SetTextColor(hdc, PARACOL_LIMIT);
			memset(strLimit, 0, sizeof strLimit);
			snprintf(strLimit, sizeof strLimit, "%d", gCfgPulse.wHigh_PR);
			DrawText(hdc, strLimit, -1, &gRCPr_Limit, DT_NOCLIP | DT_TOP | DT_RIGHT | DT_SINGLELINE);
			memset(strLimit, 0, sizeof strLimit);
			snprintf(strLimit, sizeof strLimit, "%d", gCfgPulse.wLow_PR);
			DrawText(hdc, strLimit, -1, &gRCPr_Limit, DT_NOCLIP | DT_BOTTOM | DT_RIGHT | DT_SINGLELINE);
		}
		
	}
	
	//绘制框架
 	Rectangle(hdc, gRCSpO2AndPR.left, gRCSpO2AndPR.top, gRCSpO2AndPR.right+5, gRCSpO2AndPR.bottom);

	if(bDebugRect){
	SetPenColor(hdc, COLOR_darkgreen);
	//Label
	 Rectangle(hdc, gRCSpO2_Label.left, gRCSpO2_Label.top, gRCSpO2_Label.right, gRCSpO2_Label.bottom);

	//Alarm Limit
	SetPenColor(hdc, COLOR_darkcyan);
	 Rectangle(hdc, gRCSpO2_Limit.left, gRCSpO2_Limit.top, gRCSpO2_Limit.right, gRCSpO2_Limit.bottom);

	//Alarm Icon
		SetPenColor(hdc, COLOR_darkred);

	 Rectangle(hdc, gRCSpO2_AlmIcon.left, gRCSpO2_AlmIcon.top, gRCSpO2_AlmIcon.right, gRCSpO2_AlmIcon.bottom);

	//Strength
		SetPenColor(hdc, COLOR_darkmagenta);

	 Rectangle(hdc, gRCSpO2_Strength.left, gRCSpO2_Strength.top, gRCSpO2_Strength.right, gRCSpO2_Strength.bottom);

	//Data
		SetPenColor(hdc, COLOR_blue );

	 Rectangle(hdc, gRCSpO2_Data.left, gRCSpO2_Data.top, gRCSpO2_Data.right, gRCSpO2_Data.bottom);

	//Unit
//		SetPenColor(hdc, COLOR_lightwhite );

//	 Rectangle(hdc, gRCSpO2_Unit.left, gRCSpO2_Unit.top, gRCSpO2_Unit.right, gRCSpO2_Unit.bottom);

	//Info
		SetPenColor(hdc, COLOR_cyan);

	 Rectangle(hdc, gRCSpO2_Info.left, gRCSpO2_Info.top, gRCSpO2_Info.right, gRCSpO2_Info.bottom);

	//Column
		SetPenColor(hdc, COLOR_darkgray);

	 Rectangle(hdc, gRCSpO2_Column.left, gRCSpO2_Column.top, gRCSpO2_Column.right, gRCSpO2_Column.bottom);

	//PR Label
		SetPenColor(hdc, COLOR_darkyellow );

	 Rectangle(hdc, gRCPr_Label.left, gRCPr_Label.top, gRCPr_Label.right, gRCPr_Label.bottom);

	//Alarm Icon
		SetPenColor(hdc, COLOR_green );

	 Rectangle(hdc, gRCPr_AlmIcon.left, gRCPr_AlmIcon.top, gRCPr_AlmIcon.right, gRCPr_AlmIcon.bottom);

	//PR Data
		SetPenColor(hdc, COLOR_green );
	 Rectangle(hdc, gRCPr_Data.left, gRCPr_Data.top, gRCPr_Data.right, gRCPr_Data.bottom);

	//Alarm Limit
	 Rectangle(hdc, gRCPr_Limit.left, gRCPr_Limit.top, gRCPr_Limit.right, gRCPr_Limit.bottom);

	}


	//拷贝DC
 	if(Draw_Mem){
 		BitBlt(hdcParaMem, 
 		       gRCSpO2AndPR.left, gRCSpO2AndPR.top, RECTW(gRCSpO2AndPR)+1, RECTH(gRCSpO2AndPR)+1, 
 		       hdcPara, 
 		       gRCSpO2AndPR.left, gRCSpO2AndPR.top, 
 		       0);
 	}	

	return 0;
}	

//根据模块配置以及屏幕类型初始化参数显示区域
static int InitRectResp()
{

if(gCfgSystem.bInterface!=SCREEN_BIGCHAR)
	SetRect(&gRCResp, 
			gRcParas.left, 
			gRcParas.top+iLeftParaRectHeight*3, 
			gRcParas.right-1, 
			gRcParas.top+iLeftParaRectHeight*4);
else
	SetRect(&gRCResp, 
			gRcParas.left, 
			gRcParas.top+iLeftParaRectHeight*2, 
			gRcParas.right-1, 
			gRcParas.top+iLeftParaRectHeight*3-18);

	//Label
	SetRect(&gRCResp_Label, gRCResp.left+1,gRCResp.top+1,gRCResp.left+70,gRCResp.top+17);
	//Unit 
	SetRect(&gRCResp_Unit, gRCResp.left+1,gRCResp.top+23,gRCResp.left+40,gRCResp.top+35);

	//Limit
	SetRect(&gRCResp_Limit, gRCResp.left+1, gRCResp.top+30, gRCResp.left+30, gRCResp.top+55);
	//Alarm Icon
// 	SetRect(&gRCResp_AlmIcon, gRCResp.right-35,gRCResp.top+90,gRCResp.right, gRCResp.top+110);
	SetRect(&gRCResp_AlmIcon, gRCResp.right-20,gRCResp.top+1,gRCResp.right, gRCResp.top+20);
	//Data
// 	SetRect(&gRCResp_Data, gRCResp.left+35-giLeftDec,gRCResp.top+27,gRCResp.left+125-giLeftDec,gRCResp.top+87);
#if SCREEN_1024	
	SetRect(&gRCResp_Data, gRCResp.left+50-giLeftDec,gRCResp.top+20,gRCResp.right-35,gRCResp.bottom);
#elif SCREEN_640
	SetRect(&gRCResp_Data, gRCResp.left+30,gRCResp.top+1,gRCResp.right-30,gRCResp.bottom);	
#else
	SetRect(&gRCResp_Data, gRCResp.left+30,gRCResp.top+1,gRCResp.right-30,gRCResp.bottom);	

#endif	
	
	//Switch
	SetRect(&gRCResp_Info, gRCResp.left+1,gRCResp.bottom-20,gRCResp.left+100,gRCResp.bottom);
	
	return 0;
}
//显示Resp参数
static int ViewResp(HDC hdc, BYTE bInterface, BOOL bMemCopy)
{
	unsigned char strLabel[10]={0};
	unsigned char strRr[4]={0};
	static BOOL bFlashRr = FALSE;
	unsigned char strLimit[5]={0};
	
	//获得数据
	if(gValueResp.bOff !=TRUE  && gCfgResp.bSwitch == SWITCH_ON&&gValueResp.wRR!=0){
		snprintf(strRr, sizeof strRr, "%d", gValueResp.wRR);
	}
	else{
		snprintf(strRr, sizeof strRr, "---");
	}
		
	//if(B_PRINTF) printf("gValueResp.wRR=%d\n",gValueResp.wRR);
	//设置DC属性
// 	SetPenColor(hdc, PARACOL_FRAME);
	SetTextColor(hdc, gCfgResp.iColor);	
// 	SetBkColor(hdc, PARACOL_BK);
// 	SetBrushColor(hdc, PARACOL_BK);
	
	//清屏
// 	FillBox(hdc, gRCResp.left, gRCResp.top, RECTW(gRCResp), RECTH(gRCResp));
		
	
	//显示 Label
	SelectFont(hdc, gFontEnglish_16);
//	if(bDebugRect) Rectangle(hdc, gRCResp_Label.left, gRCResp_Label.top, gRCResp_Label.right, gRCResp_Label.bottom);
	memset(strLabel, 0, sizeof strLabel);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "rr", strLabel, sizeof strLabel);	
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%s (rpm)", strLabel);
//	snprintf(strInfo, sizeof strInfo, "(rpm)");
	DrawText(hdc, strInfo, -1, &gRCResp_Label, DT_NOCLIP | DT_VCENTER | DT_LEFT | DT_SINGLELINE);

	//显示 Alarm Icon
//	if(bDebugRect) Rectangle(hdc, gRCResp_AlmIcon.left, gRCResp_AlmIcon.top, gRCResp_AlmIcon.right, gRCResp_AlmIcon.bottom);
	if(gCfgResp.bAlmControl == ALARM_OFF){
		FillBoxWithBitmap(hdc, gRCResp_AlmIcon.left+1,gRCResp_AlmIcon.top+1, 16, 16, &gIconAlarmOff);
	}
	
	//显示是否关闭功能
	memset(strInfo, 0, sizeof strInfo);
//	if(bDebugRect) Rectangle(hdc, gRCResp_Info.left, gRCResp_Info.top, gRCResp_Info.right, gRCResp_Info.bottom);
	if(gCfgResp.bSwitch == SWITCH_ON){
		GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "on", strInfo, sizeof strInfo);	
	}
	else{
		GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "off", strInfo, sizeof strInfo);	
		DrawText(hdc, strInfo, -1, &gRCResp_Info, DT_NOCLIP | DT_VCENTER | DT_LEFT| DT_SINGLELINE);
	}

	//显示数据
	if(gbAlarmResp_Data && (gCfgResp.bAlmControl != ALARM_OFF)){
		if(bFlashRr){
			SetTextColor(hdc, ALARMFLASH_COLOR);
		}
		else{
			SetTextColor(hdc, gCfgResp.iColor);		
		}
		bFlashRr = !bFlashRr;
	}
	else{
		SetTextColor(hdc, gCfgResp.iColor);			
	}
//	if(bDebugRect) Rectangle(hdc, gRCResp_Data.left, gRCResp_Data.top, gRCResp_Data.right, gRCResp_Data.bottom);
// 	SelectFont(hdc, gFontTTF_50);
#if SCREEN_640 
	if(gCfgSystem.bInterface==SCREEN_BIGCHAR)
		SelectFont(hdc, gFontTTF_70);
	else
		SelectFont(hdc, gFontTTF_60);
#else
	if(gCfgSystem.bInterface==SCREEN_BIGCHAR)
		SelectFont(hdc, gFontTTF_90Bk);
	else
		SelectFont(hdc, gFontTTF_75);
#endif
 	DrawText(hdc, strRr, -1, &gRCResp_Data, DT_NOCLIP | DT_CENTER | DT_VCENTER | DT_SINGLELINE);
//	DrawText(hdc, strRr, -1, &gRCResp_Data, DT_NOCLIP | DT_BOTTOM| DT_LEFT | DT_SINGLELINE);

	//Alarm Limit
//	if(bDebugRect) Rectangle(hdc, gRCResp_Limit.left, gRCResp_Limit.top, gRCResp_Limit.right, gRCResp_Limit.bottom);
	if(gCfgAlarm.bViewLimit){
		SelectFont(hdc, gFontSmall);
		SetTextColor(hdc, PARACOL_LIMIT);
		memset(strLimit, 0, sizeof strLimit);
		snprintf(strLimit, sizeof strLimit, "%d", gCfgResp.wHigh_RR);
		DrawText(hdc, strLimit, -1, &gRCResp_Limit, DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);
		memset(strLimit, 0, sizeof strLimit);
		snprintf(strLimit, sizeof strLimit, "%d", gCfgResp.wLow_RR);
		DrawText(hdc, strLimit, -1, &gRCResp_Limit, DT_NOCLIP | DT_BOTTOM | DT_LEFT | DT_SINGLELINE);
	}
	
	//绘制框架
	Rectangle(hdc, gRCResp.left, gRCResp.top, gRCResp.right+5, gRCResp.bottom);
	if(bDebugRect) {
	//Label
	Rectangle(hdc, gRCResp_Label.left, gRCResp_Label.top, gRCResp_Label.right, gRCResp_Label.bottom);

	//Limit
	Rectangle(hdc, gRCResp_Limit.left, gRCResp_Limit.top, gRCResp_Limit.right, gRCResp_Limit.bottom);

	//Alarm Icon
	Rectangle(hdc, gRCResp_AlmIcon.left, gRCResp_AlmIcon.top, gRCResp_AlmIcon.right, gRCResp_AlmIcon.bottom);

	//Data
	Rectangle(hdc, gRCResp_Data.left, gRCResp_Data.top, gRCResp_Data.right, gRCResp_Data.bottom);

	//Unit 
	Rectangle(hdc, gRCResp_Unit.left, gRCResp_Unit.top, gRCResp_Unit.right, gRCResp_Unit.bottom);

	//Switch
	Rectangle(hdc, gRCResp_Info.left, gRCResp_Info.top, gRCResp_Info.right, gRCResp_Info.bottom);

	
	}
	if(Draw_Mem){
		//拷贝DC
		BitBlt(hdcParaMem, 
		gRCResp.left, gRCResp.top, RECTW(gRCResp)+1, RECTH(gRCResp)+1, 
		hdcPara, 
		gRCResp.left, gRCResp.top, 
		0);
	}
		
	return 0;
}	



//根据模块配置以及屏幕类型初始化参数显示区域
static int InitRectCo2()
{
	/*
	if(gbHaveIbp){
		//绝对坐标
#if SCREEN_1024		
		SetRect(&gRCCo2, 
			 gRcParas.left, 
			 gRcParas.top+iLeftParaRectHeight*4+10, 
			 gRcParas.right-1, 
			 gRcParas.bottom-1);
#elif SCREEN_640
		SetRect(&gRCCo2, 
			 gRcParas.left, 
			 gRcParas.top+410, 
			 gRcParas.right-1, 
			 gRcParas.bottom-1);
#else
		SetRect(&gRCCo2, 
			 gRcParas.left, 
			 gRcParas.top+410, 
			 gRcParas.right-1, 
			 gRcParas.bottom-1);
#endif
					 
	}
	else{
		//绝对坐标
		SetRect(&gRCCo2, 
			 gRcParas.left, 
			 gRcParas.top+iLeftParaRectHeight*2, 
			 gRcParas.right-1, 
			 gRcParas.top+iLeftParaRectHeight*2+140);
	}	
	*/
	if(gCfgSystem.bInterface!=SCREEN_BIGCHAR)
	SetRect(&gRCCo2, 
			gRcParas.left, 
			gRcParas.top+iLeftParaRectHeight*3, 
			gRcParas.right-1, 
			gRcParas.top+iLeftParaRectHeight*4);
else
	SetRect(&gRCCo2, 
			gRcParas.left, 
			gRcParas.top+iLeftParaRectHeight*2, 
			gRcParas.right-1, 
			gRcParas.top+iLeftParaRectHeight*3-18);
	
	//相对坐标
	SetRect(&gRCCo2_Label_Co2, gRCCo2.left+35-giLeftDec,gRCCo2.top+1,gRCCo2.left+180-giLeftDec,gRCCo2.top+17);
	//Limit
	SetRect(&gRCCo2_Limit_EtCo2, gRCCo2.left+1,gRCCo2.top+30,gRCCo2.left+30,gRCCo2.top+55);
	SetRect(&gRCCo2_Limit_FiCo2, gRCCo2.left+1,gRCCo2.top+65,gRCCo2.left+30,gRCCo2.top+90);
	//Data CO2
	SetRect(&gRCCo2_EtCo2, gRCCo2.left+30,gRCCo2.top+17,gRCCo2.left+150,gRCCo2.top+RECTH(gRCCo2)/2+10);
	SetRect(&gRCCo2_FiCo2, gRCCo2.left+30,gRCCo2.top+RECTH(gRCCo2)/2+10,gRCCo2.left+150,gRCCo2.bottom);
	//Alarm Icon CO2
	SetRect(&gRCCo2_AlmIcon_Co2, gRCCo2.right-20,gRCCo2.top,gRCCo2.right,gRCCo2.top+20);
	//RR Label
	SetRect(&gRCCo2_Label_RR, gRCCo2.left+150,gRCCo2.top+17,gRCCo2.right,gRCCo2.top+30);
	//RR Limit
	SetRect(&gRCCo2_Limit_RR, gRCCo2.right-30,gRCCo2.top+40,gRCCo2.right,gRCCo2.top+65);
	//RR Data
	SetRect(&gRCCo2_RR, gRCCo2.left+120,gRCCo2.top+17,gRCCo2.right, gRCCo2.bottom);
	//Alarm Icon RR
	SetRect(&gRCCo2_AlmIcon_RR, gRCCo2.right-20,gRCCo2.top+20,gRCCo2.right, gRCCo2.top+40);
	
	//CO2 Status
	SetRect(&gRCCo2_Status, gRCCo2.left+35-giLeftDec, gRCCo2.bottom-20, gRCCo2.left+90-giLeftDec, gRCCo2.bottom);
	//Info CO2
	SetRect(&gRCCo2_Info_Co2, gRCCo2.left+120,gRCCo2.bottom-20,gRCCo2.right, gRCCo2.bottom);
		
	return 0;
}


//显示CO2参数
static int ViewCo2(HDC hdc, BYTE bInterface, BOOL bMemCopy)
{
	unsigned char strLabel[10]={0};
	unsigned char strUnit[10]={0};
	unsigned char strSource[10]={0};
	unsigned char strTmp[10]={0};
	unsigned char strRr[4]={0};
	unsigned char strEtCo2[6]={0};
	unsigned char strFiCo2[6]={0};
	unsigned char strInsCo2[6]={0};
	static BOOL bFlashEtCo2 = FALSE;
	static BOOL bFlashFiCo2 = FALSE;
	static BOOL bFlashRr = FALSE;
	unsigned char strLimit_h[5]={0};
	unsigned char strLimit_l[5]={0};
	unsigned char strLimit_fi[5]={0};
	
	//获得数据
	if(!gValueCO2.bOff){
		if(gCfgCO2.bUnit == CO2UNIT_MMHG){
			snprintf(strEtCo2, sizeof strEtCo2, "%d", gValueCO2.wEtCO2);
			snprintf(strFiCo2, sizeof strFiCo2, "%d", gValueCO2.wFiCO2);
			snprintf(strInsCo2, sizeof strInsCo2, "%d", gValueCO2.wInsCO2);
		}
		else if(gCfgCO2.bUnit == CO2UNIT_PER){
			snprintf(strEtCo2, sizeof strEtCo2, "%3.1f", (float)(gValueCO2.wEtCO2*100.0/gValueCO2.wBaro));
			snprintf(strFiCo2, sizeof strFiCo2, "%3.1f", (float)(gValueCO2.wFiCO2*100.0/gValueCO2.wBaro));
			snprintf(strInsCo2, sizeof strInsCo2, "%3.1f", (float)(gValueCO2.wInsCO2*100.0/gValueCO2.wBaro));
		}
	}
	else{
		if(gCfgCO2.bUnit == CO2UNIT_MMHG){
		//	snprintf(strEtCo2, sizeof strEtCo2, "--");
		//	snprintf(strFiCo2, sizeof strFiCo2, "--");

			snprintf(strEtCo2, sizeof strEtCo2, "45");
			snprintf(strFiCo2, sizeof strFiCo2, "20");
		}
		else if(gCfgCO2.bUnit == CO2UNIT_PER){
 		//	snprintf(strEtCo2, sizeof strEtCo2, "--.-");		
 		//	snprintf(strFiCo2, sizeof strFiCo2, "--.-");
			
			snprintf(strEtCo2, sizeof strEtCo2, "10.5");		
 			snprintf(strFiCo2, sizeof strFiCo2, "8.5");
		}
	}
	
// 	snprintf(strEtCo2, sizeof strEtCo2, "13.1");		
// 	snprintf(strFiCo2, sizeof strFiCo2, "13.1");

	//RR
	if(!gValueResp.bOff && gCfgResp.bSwitch == SWITCH_ON){
		snprintf(strRr, sizeof strRr, "%d", gValueResp.wRR);
	}
	else{
 		snprintf(strRr, sizeof strRr, "---");
	}
	
	//设置DC属性
	SetTextColor(hdc, gCfgCO2.iColor);	
	
	SelectFont(hdc, gFontEnglish_16);
	//显示 Label CO2
	memset(strLabel, 0, sizeof strLabel);
	if(!gbCO2GainCal){
		GetStringFromResFile(gsLanguageRes, "GENERAL", "co2", strLabel, sizeof strLabel);	
	}
	else{
		GetStringFromResFile(gsLanguageRes, "GENERAL", "insco2", strLabel, sizeof strLabel);	
	}
	memset(strUnit, 0, sizeof strUnit);
 	if(gCfgCO2.bUnit == CO2UNIT_MMHG){
 		GetStringFromResFile(gsLanguageRes, "GENERAL", "unit_mmhg", strUnit, sizeof strUnit);		
 	}
 	else{
 		GetStringFromResFile(gsLanguageRes, "GENERAL", "unit_per", strUnit, sizeof strUnit);			
 	}
	memset(strInfo, 0, sizeof strInfo);
	
	snprintf(strInfo, sizeof strInfo, "%s (%s)", strLabel, strUnit);
	DrawText(hdc, strInfo, -1, &gRCCo2_Label_Co2, DT_NOCLIP | DT_VCENTER | DT_LEFT | DT_SINGLELINE);
	DrawText(hdc, "ET", -1, &gRCCo2_EtCo2, DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);
	DrawText(hdc, "FI", -1, &gRCCo2_FiCo2, DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);
		
	//显示 Alarm Icon
   	if(gCfgCO2.bAlmControl == ALARM_OFF){
 		FillBoxWithBitmap(hdc, gRCCo2_AlmIcon_Co2.left+1,gRCCo2_AlmIcon_Co2.top+1, 16, 16, &gIconAlarmOff);
   	}

	//显示 Status 
 	if(gValueCO2.bStatus == CO2_STATUS_STOP){
 		GetStringFromResFile(gsLanguageRes, "CO2", "stop", strInfo, sizeof strInfo);	
 	}
 	else{
 		GetStringFromResFile(gsLanguageRes, "CO2", "run", strInfo, sizeof strInfo);	
 	}
 	SelectFont(hdc, gFontSystem);
 	DrawText(hdc, strInfo, -1, &gRCCo2_Status, DT_NOCLIP | DT_VCENTER | DT_LEFT| DT_SINGLELINE);
		
	//CO2 Info
 	if(gbCO2GainCal){
 		SelectFont(hdc, gFontSystem);
 		GetStringFromResFile(gsLanguageRes, "CO2", "info", strInfo, sizeof strInfo);	
 		DrawText(hdc, strInfo, -1, &gRCCo2_Info_Co2, DT_NOCLIP | DT_VCENTER | DT_RIGHT| DT_SINGLELINE);
 	}
	
	SelectFont(hdc, gFontEnglish_16);
	//显示 Label RR
	memset(strLabel, 0, sizeof strLabel);
	//来源不同，名称不同
	switch(gCfgResp.bSource){
		case RR_SRC_ECG:{
			GetStringFromResFile(gsLanguageRes, "GENERAL", "rr", strLabel, sizeof strLabel);	
		}break;
		default:{
			GetStringFromResFile(gsLanguageRes, "GENERAL", "awrr", strLabel, sizeof strLabel);	
		}break;
	}
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%s (rpm)", strLabel);
	DrawText(hdc, strInfo, -1, &gRCCo2_Label_RR, DT_NOCLIP | DT_VCENTER | DT_LEFT | DT_SINGLELINE);
	
	//显示 Alarm Icon
	if(gCfgResp.bAlmControl == ALARM_OFF){
		FillBoxWithBitmap(hdc, gRCCo2_AlmIcon_RR.left+1,gRCCo2_AlmIcon_RR.top+1, 16, 16, &gIconAlarmOff);
	}
	
	//显示数据 CO2
	//如果处在校准状态，仅显示瞬时的波形值
	if(!gbCO2GainCal){
		//EtCO2
		if(gbAlarmCo2_EtCo2 && (gCfgCO2.bAlmControl != ALARM_OFF)){
			if(bFlashEtCo2){
				SetTextColor(hdc, ALARMFLASH_COLOR);
			}
			else{
				SetTextColor(hdc, gCfgCO2.iColor);		
			}
			bFlashEtCo2 = !bFlashEtCo2;
		}
		else{
			SetTextColor(hdc, gCfgCO2.iColor);			
		}
		SelectFont(hdc, gFontTTF_40Bk);
		DrawText(hdc, strEtCo2, -1, &gRCCo2_EtCo2, DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE);

		//FiCO2
		if(gbAlarmCo2_FiCo2 && (gCfgCO2.bAlmControl != ALARM_OFF)){
			if(bFlashFiCo2){
				SetTextColor(hdc, ALARMFLASH_COLOR);
			}
			else{
				SetTextColor(hdc, gCfgCO2.iColor);		
			}
			bFlashFiCo2 = !bFlashFiCo2;
		}
		else{
			SetTextColor(hdcParaMem, gCfgCO2.iColor);			
		}
		DrawText(hdc, strFiCo2, -1, &gRCCo2_FiCo2, DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	
		//Alarm Limit
		if(gCfgAlarm.bViewLimit){
			SetTextColor(hdc, PARACOL_LIMIT);
			SelectFont(hdc, gFontSmall);
			memset(strLimit_h, 0, sizeof strLimit_h);
			memset(strLimit_l, 0, sizeof strLimit_l);
			memset(strLimit_fi, 0, sizeof strLimit_fi);
			switch(gCfgCO2.bUnit){
				case 0:{//mmHg
					snprintf(strLimit_h, sizeof strLimit_h, "%d", gCfgCO2.wHigh_EtCO2);
					snprintf(strLimit_l, sizeof strLimit_l, "%d", gCfgCO2.wLow_EtCO2);
					snprintf(strLimit_fi, sizeof strLimit_fi, "%d", gCfgCO2.wHigh_FiCO2);
				}break;
				case 1:{//kPa
					snprintf(strLimit_h, sizeof strLimit_h, "%.1f", (gCfgCO2.wHigh_EtCO2)/7.6);
					snprintf(strLimit_l, sizeof strLimit_l, "%.1f", (gCfgCO2.wLow_EtCO2)/7.6);
					snprintf(strLimit_fi, sizeof strLimit_fi, "%.1f", (gCfgCO2.wHigh_FiCO2)/7.6);
				}break;
			}
			DrawText(hdc, strLimit_h, -1, &gRCCo2_Limit_EtCo2, DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);
			DrawText(hdc, strLimit_l, -1, &gRCCo2_Limit_EtCo2, DT_NOCLIP | DT_BOTTOM | DT_LEFT| DT_SINGLELINE);	
			DrawText(hdc, strLimit_fi, -1, &gRCCo2_Limit_FiCo2, DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);	
		}
	}
	else{
		//InsCO2
		SetTextColor(hdc, gCfgCO2.iColor);
		SelectFont(hdc, gFontTTF_40);
		DrawText(hdc, strInsCo2, -1, &gRCCo2_EtCo2, DT_NOCLIP | DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
	}

	//显示数据 RR

	if(gbAlarmResp_Data && (gCfgResp.bAlmControl != ALARM_OFF)){
		if(bFlashRr){
			SetTextColor(hdc, ALARMFLASH_COLOR);
		}
		else{
			SetTextColor(hdc, gCfgCO2.iColor);		
		}
		bFlashRr = !bFlashRr;
	}
	else{
		SetTextColor(hdc, gCfgCO2.iColor);			
	}
	SelectFont(hdc, gFontTTF_40Bk);
	DrawText(hdc, strRr, -1, &gRCCo2_RR, DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE);

	if(gCfgAlarm.bViewLimit){
		SetTextColor(hdc, PARACOL_LIMIT);
		SelectFont(hdc, gFontSmall);
		memset(strLimit_h, 0, sizeof strLimit_h);
		memset(strLimit_l, 0, sizeof strLimit_l);
		snprintf(strLimit_h, sizeof strLimit_h, "%d", gCfgResp.wHigh_RR);
		snprintf(strLimit_l, sizeof strLimit_l, "%d", gCfgResp.wLow_RR);
		DrawText(hdc, strLimit_h, -1, &gRCCo2_Limit_RR, DT_NOCLIP | DT_TOP | DT_RIGHT | DT_SINGLELINE);
		DrawText(hdc, strLimit_l, -1, &gRCCo2_Limit_RR, DT_NOCLIP | DT_BOTTOM | DT_RIGHT | DT_SINGLELINE);	
	}
	
	//绘制框架
  	Rectangle(hdc, gRCCo2.left, gRCCo2.top, gRCCo2.right+5, gRCCo2.bottom);
	
if(bDebugRect) {
	//Limit
	Rectangle(hdc, gRCCo2_Limit_EtCo2.left, gRCCo2_Limit_EtCo2.top, gRCCo2_Limit_EtCo2.right, gRCCo2_Limit_EtCo2.bottom);
	Rectangle(hdc, gRCCo2_Limit_FiCo2.left, gRCCo2_Limit_FiCo2.top, gRCCo2_Limit_FiCo2.right, gRCCo2_Limit_FiCo2.bottom);
	//Data CO2
	Rectangle(hdc, gRCCo2_EtCo2.left, gRCCo2_EtCo2.top, gRCCo2_EtCo2.right, gRCCo2_EtCo2.bottom);
	Rectangle(hdc, gRCCo2_FiCo2.left, gRCCo2_FiCo2.top, gRCCo2_FiCo2.right, gRCCo2_FiCo2.bottom);
	//Alarm Icon CO2
	Rectangle(hdc, gRCCo2_AlmIcon_Co2.left, gRCCo2_AlmIcon_Co2.top, gRCCo2_AlmIcon_Co2.right, gRCCo2_AlmIcon_Co2.bottom);
	//RR Label
	Rectangle(hdc, gRCCo2_Label_RR.left, gRCCo2_Label_RR.top, gRCCo2_Label_RR.right, gRCCo2_Label_RR.bottom);
	//RR Limit
	Rectangle(hdc, gRCCo2_Limit_RR.left, gRCCo2_Limit_RR.top, gRCCo2_Limit_RR.right, gRCCo2_Limit_RR.bottom);
	//RR Data
	Rectangle(hdc, gRCCo2_RR.left, gRCCo2_RR.top, gRCCo2_RR.right, gRCCo2_RR.bottom);
	//Alarm Icon RR
	Rectangle(hdc, gRCCo2_AlmIcon_RR.left, gRCCo2_AlmIcon_RR.top, gRCCo2_AlmIcon_RR.right, gRCCo2_AlmIcon_RR.bottom);
	//CO2 Status
	Rectangle(hdc, gRCCo2_Status.left, gRCCo2_Status.top, gRCCo2_Status.right, gRCCo2_Status.bottom);
	//Info CO2
	Rectangle(hdc, gRCCo2_Info_Co2.left, gRCCo2_Info_Co2.top, gRCCo2_Info_Co2.right, gRCCo2_Info_Co2.bottom);

}

	//拷贝DC
 	if(Draw_Mem){
 		BitBlt(hdc, 
 		gRCCo2.left, gRCCo2.top, RECTW(gRCCo2)+1, RECTH(gRCCo2)+1, 
 		hdcPara, 
 		gRCCo2.left, gRCCo2.top, 
 		0);
 	}
		
	return 0;
}	


static int InitRectGas()
{
	
	if(gbHaveIbp){
		//绝对坐标
#if SCREEN_1024		
		SetRect(&gRCGas, 
				 gRcParas.left, 
	 			 gRcParas.top+iLeftParaRectHeight*4+10, 
  				 gRcParas.right-1, 
 				 gRcParas.bottom-1);
#elif	SCREEN_640
		SetRect(&gRCGas, 
				 gRcParas.left, 
	 			 gRcParas.top+410, 
  				 gRcParas.right-1, 
  				 gRcParas.bottom-1);
#else
		SetRect(&gRCGas, 
				 gRcParas.left, 
	 			 gRcParas.top+410, 
  				 gRcParas.right-1, 
  				 gRcParas.bottom-1);
#endif
	}
	else{
		//绝对坐标
		SetRect(&gRCGas,
				 gRcParas.left, 
				 gRcParas.top+iLeftParaRectHeight*2, 
  				 gRcParas.right-1, 
  				 gRcParas.top+iLeftParaRectHeight*2+140);
	}	

	//相对坐标
	SetRect(&gRCGas_Label_CO2, gRCGas.left+35-giLeftDec,gRCGas.top+1,gRCGas.left+180-giLeftDec,gRCGas.top+17);
	
	/*
	//相对坐标
	SetRect(&gRCCo2_Label_Co2, gRCCo2.left+35-giLeftDec,gRCCo2.top+1,gRCCo2.left+180-giLeftDec,gRCCo2.top+17);
	//Limit
	SetRect(&gRCCo2_Limit_EtCo2, gRCCo2.left+35-giLeftDec,gRCCo2.top+30,gRCCo2.left+80-giLeftDec,gRCCo2.top+60);
	SetRect(&gRCCo2_Limit_FiCo2, gRCCo2.left+35-giLeftDec,gRCCo2.top+80,gRCCo2.left+80-giLeftDec,gRCCo2.top+110);
	//Data CO2
	SetRect(&gRCCo2_EtCo2, gRCCo2.left+80-giLeftDec,gRCCo2.top+20,gRCCo2.left+150-giLeftDec,gRCCo2.top+70);
	SetRect(&gRCCo2_FiCo2, gRCCo2.left+80-giLeftDec,gRCCo2.top+70,gRCCo2.left+150-giLeftDec,gRCCo2.top+120);
	//Alarm Icon CO2
	SetRect(&gRCCo2_AlmIcon_Co2, gRCCo2.left+150-giLeftDec,gRCCo2.top+20,gRCCo2.left+170-giLeftDec,gRCCo2.top+40);
	//RR Label
	SetRect(&gRCCo2_Label_RR, gRCCo2.left+160-giLeftDec,gRCCo2.top+40,gRCCo2.right,gRCCo2.top+60);
	//RR Limit
	SetRect(&gRCCo2_Limit_RR, gRCCo2.left+150-giLeftDec,gRCCo2.top+60,gRCCo2.left+170-giLeftDec,gRCCo2.top+90);
	//RR Data
	SetRect(&gRCCo2_RR, gRCCo2.left+170-giLeftDec,gRCCo2.top+60,gRCCo2.right, gRCCo2.top+100);
	//Alarm Icon RR
	SetRect(&gRCCo2_AlmIcon_RR, gRCCo2.right-20,gRCCo2.top+100,gRCCo2.right, gRCCo2.top+120);
	
	//CO2 Status
	SetRect(&gRCCo2_Status, gRCCo2.left+35-giLeftDec, gRCCo2.bottom-20, gRCCo2.left+90-giLeftDec, gRCCo2.bottom);
	//Info CO2
	SetRect(&gRCCo2_Info_Co2, gRCCo2.left+92-giLeftDec,gRCCo2.bottom-20,gRCCo2.right, gRCCo2.bottom);
	*/	

	return 0;
}

//显示GAS
static int ViewGas(HDC hdc, BYTE bInterface, BOOL bMemCopy)
{
	
	//设置DC属性
// 	SetTextColor(hdc, gCfgCO2.iColor);	
	
	//CO2 & awRR
	SetTextColor(hdc, COLOR_yellow);	
	SelectFont(hdc, gFontEnglish_16);
	TextOut(hdc, gRCGas.left, gRCGas.top, "CO2");
	TextOut(hdc, gRCGas.left+85, gRCGas.top, "mmHg");
	TextOut(hdc, gRCGas.left+10, gRCGas.top+25, "Et");
	TextOut(hdc, gRCGas.left+10, gRCGas.top+50, "Fi");
	TextOut(hdc, gRCGas.left+85, gRCGas.top+20, "awRR");

 	SelectFont(hdc, gFontTTF_20);
 	TextOut(hdc, gRCGas.left+35, gRCGas.top+5, "38");
	TextOut(hdc, gRCGas.left+35, gRCGas.top+32, "2");
	TextOut(hdc, gRCGas.left+85, gRCGas.top+32, "20");
	
	//N2O
	SetTextColor(hdc, COLOR_cyan);	
	SelectFont(hdc, gFontEnglish_16);
	TextOut(hdc, gRCGas.left, gRCGas.top+RECTH(gRCGas)/2, "N2O");
	TextOut(hdc, gRCGas.left+85, gRCGas.top+RECTH(gRCGas)/2, "%");
	TextOut(hdc, gRCGas.left+10, gRCGas.top+RECTH(gRCGas)/2+25, "Et");
	TextOut(hdc, gRCGas.left+10, gRCGas.top+RECTH(gRCGas)/2+50, "Fi");
	SelectFont(hdc, gFontTTF_20);
	TextOut(hdc, gRCGas.left+35, gRCGas.top+RECTH(gRCGas)/2+5, "45");
	TextOut(hdc, gRCGas.left+35, gRCGas.top+RECTH(gRCGas)/2+32, "50");
	
	//O2
	SetTextColor(hdc, COLOR_green);	
	SelectFont(hdc, gFontEnglish_16);
	TextOut(hdc, gRCGas.left+RECTW(gRCGas)/2, gRCGas.top, "O2");
	TextOut(hdc, gRCGas.left+RECTW(gRCGas)/2+85, gRCGas.top, "mmHg");
	TextOut(hdc, gRCGas.left+RECTW(gRCGas)/2+10, gRCGas.top+25, "Et");
	TextOut(hdc, gRCGas.left+RECTW(gRCGas)/2+10, gRCGas.top+50, "Fi");
	SelectFont(hdc, gFontTTF_20);
	TextOut(hdc, gRCGas.left+RECTW(gRCGas)/2+35, gRCGas.top+5, "141");
	TextOut(hdc, gRCGas.left+RECTW(gRCGas)/2+35, gRCGas.top+32, "152");
		
	//GAS
	SetTextColor(hdc, COLOR_darkyellow);	
	SelectFont(hdc, gFontEnglish_16);
	TextOut(hdc, gRCGas.left+RECTW(gRCGas)/2, gRCGas.top+RECTH(gRCGas)/2, "Enf");
	TextOut(hdc, gRCGas.left+RECTW(gRCGas)/2+85, gRCGas.top+RECTH(gRCGas)/2, "%");
	TextOut(hdc, gRCGas.left+RECTW(gRCGas)/2+10, gRCGas.top+25+RECTH(gRCGas)/2, "Et");
	TextOut(hdc, gRCGas.left+RECTW(gRCGas)/2+10, gRCGas.top+50+RECTH(gRCGas)/2, "Fi");
	TextOut(hdc, gRCGas.left+RECTW(gRCGas)/2+85, gRCGas.top+20+RECTH(gRCGas)/2, "MAC");
	SelectFont(hdc, gFontTTF_20);
	TextOut(hdc, gRCGas.left+RECTW(gRCGas)/2+35, gRCGas.top+5+RECTH(gRCGas)/2, "1.39");
	TextOut(hdc, gRCGas.left+RECTW(gRCGas)/2+35, gRCGas.top+32+RECTH(gRCGas)/2, "1.69");
	TextOut(hdc, gRCGas.left+85+RECTW(gRCGas)/2, gRCGas.top+32+RECTH(gRCGas)/2, "1.28");
	
		//绘制框架
	Rectangle(hdc, gRCGas.left, gRCGas.top, gRCGas.right, gRCGas.bottom);
	
	return 0;
}



//根据模块配置以及屏幕类型初始化参数显示区域
static int InitRectIbp1()
{
	
//	if(gbHaveIbp){
		//绝对坐标
		if(gbHaveIbp){
		SetRect(&gRCIbp1, 
			 gRcParasExt[4].left, 
			 gRcParasExt[4].top, 
			 gRcParasExt[4].right, 
			 gRcParasExt[4].bottom-1);	
	}
	
	//Label
	SetRect(&gRCIbp1_Label, gRCIbp1.left+2, gRCIbp1.top+1, gRCIbp1.right, gRCIbp1.top+17);
 	//Alarm Icon
	SetRect(&gRCIbp1_AlmIcon, gRCIbp1.right-19,gRCIbp1.top,gRCIbp1.right, gRCIbp1.top+17 );
 	//Info
	SetRect(&gRCIbp1_Info, gRCIbp1.left+35-giLeftDec,gRCIbp1.bottom-20,gRCIbp1.right,gRCIbp1.bottom);

	
	SetRect(&gRCIbp1_Zero, gRCIbp1.right-70,1,gRCIbp1.right-40,20);
	SetRect(&gRCIbp1_Cal, gRCIbp1.right-120,1,gRCIbp1.right-100,20);

#if SCREEN_1024
	//显示模式不同，区域划分也不同
	if(gCfgIbp1.bViewMode == IBPVIEW_ALL){
		//Data Sys
		SetRect(&gRCIbp1_Sys, gRCIbp1.left+35-giLeftDec,gRCIbp1.top+21,gRCIbp1.left+120-giLeftDec,gRCIbp1.bottom-20);
		//Inter
		SetRect(&gRCIbp1_Inter, gRCIbp1.left+120-giLeftDec,gRCIbp1.top+21,gRCIbp1.left+140-giLeftDec,gRCIbp1.bottom-20);
		//Data Dia
		SetRect(&gRCIbp1_Dia, gRCIbp1.left+140-giLeftDec,gRCIbp1.top+21,gRCIbp1.right-60,gRCIbp1.bottom-20);
		//Data Mean
		SetRect(&gRCIbp1_Mean, gRCIbp1.right-57,gRCIbp1.top+21,gRCIbp1.right-1,gRCIbp1.top+75);
		//Limit
		SetRect(&gRCIbp1_limit_Sys, gRCIbp1_Sys.left,gRCIbp1_Sys.top, gRCIbp1_Sys.left+40,gRCIbp1_Sys.top+30);
		SetRect(&gRCIbp1_limit_Dia, gRCIbp1_Dia.right-40,gRCIbp1_Dia.top, gRCIbp1_Dia.right, gRCIbp1_Dia.top+30);
		SetRect(&gRCIbp1_limit_Mean, gRCIbp1_Mean.right-40,gRCIbp1_Mean.top, gRCIbp1_Mean.right, gRCIbp1_Mean.top+30);
	}
	else if(gCfgIbp1.bViewMode == IBPVIEW_M){
		//Data Mean
		SetRect(&gRCIbp1_Mean, gRCIbp1.left+35-giLeftDec, gRCIbp1.top+21, gRCIbp1.left+170-giLeftDec, gRCIbp1.bottom-20);
		//Data Sys
		SetRect(&gRCIbp1_Sys, gRCIbp1.left+175-giLeftDec, gRCIbp1.top+21, gRCIbp1.left+225-giLeftDec, gRCIbp1.top+70);
		//Inter
		SetRect(&gRCIbp1_Inter, gRCIbp1.left+225-giLeftDec, gRCIbp1.top+21, gRCIbp1.left+235-giLeftDec, gRCIbp1.top+70);
		//Data Dia
		SetRect(&gRCIbp1_Dia, gRCIbp1.left+235-giLeftDec, gRCIbp1.top+21, gRCIbp1.left+285-giLeftDec, gRCIbp1.top+70);
		//Limit
		SetRect(&gRCIbp1_limit_Sys, gRCIbp1_Sys.left,gRCIbp1_Sys.top, gRCIbp1_Sys.left+40,gRCIbp1_Sys.top+30);
		SetRect(&gRCIbp1_limit_Dia, gRCIbp1_Dia.right-40,gRCIbp1_Dia.top, gRCIbp1_Dia.right, gRCIbp1_Dia.top+30);
		SetRect(&gRCIbp1_limit_Mean, gRCIbp1_Mean.left, gRCIbp1_Mean.top, gRCIbp1_Mean.left+40, gRCIbp1_Mean.top+30);
	}
#elif	SCREEN_640
	//显示模式不同，区域划分也不同
	if(gCfgIbp1.bViewMode == IBPVIEW_ALL){
		//Data Sys
		SetRect(&gRCIbp1_Sys, gRCIbp1.left+35-giLeftDec+20,gRCIbp1.top+20,gRCIbp1.left+110-giLeftDec+15,gRCIbp1.bottom-10);
		//Inter
		SetRect(&gRCIbp1_Inter, gRCIbp1.left+110-giLeftDec+20,gRCIbp1.top+20,gRCIbp1.left+120-giLeftDec+20,gRCIbp1.bottom-5);
		//Data Dia
		SetRect(&gRCIbp1_Dia, gRCIbp1.left+120-giLeftDec+20,gRCIbp1.top+20,gRCIbp1.right-45,gRCIbp1.bottom-10);
		//Data Mean
		SetRect(&gRCIbp1_Mean, gRCIbp1.right-30,gRCIbp1.bottom-30,gRCIbp1.right-5,gRCIbp1.bottom-1);
		//Limit
		SetRect(&gRCIbp1_limit_Sys, gRCIbp1.left+35-giLeftDec+65,gRCIbp1.bottom-25, gRCIbp1.left+35-giLeftDec+90,gRCIbp1.bottom-1);
		SetRect(&gRCIbp1_limit_Dia, gRCIbp1.left+110-giLeftDec+25,gRCIbp1.bottom-25, gRCIbp1.left+110-giLeftDec+50, gRCIbp1.bottom-1);
		SetRect(&gRCIbp1_limit_Mean, gRCIbp1.right-75,gRCIbp1.bottom-25, gRCIbp1.right-45, gRCIbp1.bottom-1);
	}
	else if(gCfgIbp1.bViewMode == IBPVIEW_M){
		//Data Mean
		SetRect(&gRCIbp1_Mean, gRCIbp1.left+35-giLeftDec+15, gRCIbp1.top+20, gRCIbp1.left+125-giLeftDec+15, gRCIbp1.bottom-15);
		//Data Sys
		SetRect(&gRCIbp1_Sys, gRCIbp1.left+125-giLeftDec, gRCIbp1.top+20, gRCIbp1.left+175-giLeftDec, gRCIbp1.top+55);
		//Inter
		SetRect(&gRCIbp1_Inter, gRCIbp1.left+175-giLeftDec-3, gRCIbp1.top+20, gRCIbp1.left+185-giLeftDec-3, gRCIbp1.top+60);
		//Data Dia
		SetRect(&gRCIbp1_Dia, gRCIbp1.left+185-giLeftDec, gRCIbp1.top+20, gRCIbp1.right, gRCIbp1.top+55);
		//Limit
		SetRect(&gRCIbp1_limit_Sys, gRCIbp1.left+125-giLeftDec+25,gRCIbp1.bottom-25,gRCIbp1.left+125-giLeftDec+45 , gRCIbp1.bottom-1);
		SetRect(&gRCIbp1_limit_Dia,gRCIbp1.left+185-giLeftDec+10,gRCIbp1.bottom-25, gRCIbp1.left+185-giLeftDec+30,  gRCIbp1.bottom-1);
		SetRect(&gRCIbp1_limit_Mean,gRCIbp1.left+1 ,gRCIbp1.bottom-25 , gRCIbp1.left+20,  gRCIbp1.bottom-1);
	}
#else
	//显示模式不同，区域划分也不同
	if(gCfgIbp1.bViewMode == IBPVIEW_ALL){
		//Data Sys
		SetRect(&gRCIbp1_Sys, gRCIbp1.left+65,gRCIbp1.top+20,gRCIbp1.left+85,gRCIbp1.top+100);
		//Inter
		SetRect(&gRCIbp1_Inter, gRCIbp1.left+70,gRCIbp1.top+10,gRCIbp1.left+105,gRCIbp1.top+100);
		//Data Dia
		SetRect(&gRCIbp1_Dia, gRCIbp1.left+110,gRCIbp1.top+20,gRCIbp1.left+130,gRCIbp1.top+100);
		//Data Mean
		SetRect(&gRCIbp1_Mean, gRCIbp1.left+185,gRCIbp1.top+65,gRCIbp1.left+240,gRCIbp1.top+110);
		//Limit
		SetRect(&gRCIbp1_limit_Sys, gRCIbp1.left+55,90, gRCIbp1.left+85,gRCIbp1.bottom);
		SetRect(&gRCIbp1_limit_Dia, gRCIbp1.left+110,90, gRCIbp1.left+140,gRCIbp1.bottom);
		SetRect(&gRCIbp1_limit_Mean,gRCIbp1.left+235,90,gRCIbp1.right,gRCIbp1.bottom);
	}
	else if(gCfgIbp1.bViewMode == IBPVIEW_M){
		//Data Mean
		SetRect(&gRCIbp1_Mean, gRCIbp1.left+35-giLeftDec+30, gRCIbp1.top+21, gRCIbp1.left+125-giLeftDec+30, gRCIbp1.bottom-20);
		//Data Sys
		SetRect(&gRCIbp1_Sys, gRCIbp1.left+125-giLeftDec+45, gRCIbp1.top+21, gRCIbp1.left+175-giLeftDec+45, gRCIbp1.top+70);
		//Inter
		SetRect(&gRCIbp1_Inter, gRCIbp1.left+175-giLeftDec+45, gRCIbp1.top+21, gRCIbp1.left+185-giLeftDec+45, gRCIbp1.top+70);
		//Data Dia
		SetRect(&gRCIbp1_Dia, gRCIbp1.left+185-giLeftDec+45, gRCIbp1.top+21, gRCIbp1.right+45, gRCIbp1.top+70);
		//Limit
		SetRect(&gRCIbp1_limit_Sys, gRCIbp1.left+125-giLeftDec+70,gRCIbp1.top+85, gRCIbp1.left+125-giLeftDec+90,gRCIbp1.bottom);
		SetRect(&gRCIbp1_limit_Dia,  gRCIbp1.left+125-giLeftDec+105,gRCIbp1.top+85, gRCIbp1.left+125-giLeftDec+125, gRCIbp1.bottom);
		SetRect(&gRCIbp1_limit_Mean, 1 , gRCIbp1.top+85 , 30 , gRCIbp1.bottom );
	}
#endif		
	return 0;
}

//显示IBP1参数
static int ViewIbp1(HDC hdc, BYTE bInterface, BYTE bMemCopy)
{
	unsigned char strLabel[10]={0};
	unsigned char strUnit[10]={0};
	unsigned char strSys[6]={0};
	unsigned char strDia[6]={0};
	unsigned char strMean[6]={0};
	unsigned char strZero[20]={0};	
	static BOOL bFlashSys = FALSE;
	static BOOL bFlashDia = FALSE;
	static BOOL bFlashMean = FALSE;
	int iSys, iDia, iMean;
	unsigned char strLimitS_h[6]={0};
	unsigned char strLimitS_l[6]={0};
	unsigned char strLimitD_h[6]={0};
	unsigned char strLimitD_l[6]={0};
	unsigned char strLimitM_h[6]={0};
	unsigned char strLimitM_l[6]={0};
	
	iSys = gValueIbp1.iSys;
	iDia = gValueIbp1.iDia;
	iMean = gValueIbp1.iMean;
	
// 	iSys = 255; iDia = 255; iMean = 255;
	
// 	gValueIbp1.bOff = FALSE;
	
	switch(gCfgIbp1.bUnit){
		//case IBPUNIT_MMHG
		default :{
			if((!gValueIbp1.bOff)&&(iSys>=-50)&&(iDia>=-50)&&(iMean>=-50)){
				snprintf(strSys, sizeof strSys, "%d", iSys);
				snprintf(strDia, sizeof strDia, "%d", iDia);
				snprintf(strMean, sizeof strMean, "%d", iMean);
			}
			else{
				snprintf(strSys, sizeof strSys, "---");
				snprintf(strDia, sizeof strDia, "---");
				snprintf(strMean, sizeof strMean, "---");
			}
		}break;
		case IBPUNIT_KPA:{
			if((!gValueIbp1.bOff)&&(iSys>=-50)&&(iDia>=-50)&&(iMean>=-50)){
				snprintf(strSys, sizeof strSys, "%3.1f", (float)(iSys / 7.5));
				snprintf(strDia, sizeof strDia, "%3.1f", (float)(iDia/ 7.5));
				snprintf(strMean, sizeof strMean, "%3.1f", (float)(iMean/ 7.5));
			}
			else{
				snprintf(strSys, sizeof strSys, "--.-");
				snprintf(strDia, sizeof strDia, "--.-");
				snprintf(strMean, sizeof strMean, "--.-");
			}
		}break;
		/*case IBPUNIT_CMH2O:{
			if(!gValueIbp1.bOff){
				snprintf(strSys, sizeof strSys, "%d", (int)(iSys *1.36));
				snprintf(strDia, sizeof strDia, "%d", (int)(iDia *1.36));
				snprintf(strMean, sizeof strMean, "%d", (int)(iMean *1.36));
			}
			else{
				snprintf(strSys, sizeof strSys, "---");
				snprintf(strDia, sizeof strDia, "---");
				snprintf(strMean, sizeof strMean, "---");
			}
		}break;*/
	}
	
//  	snprintf(strHR, sizeof strHR, "255");
	SetTextColor(hdc, gCfgIbp1.iColor);
		
	//显示 Alarm Icon
	if(bDebugRect) Rectangle(hdc, gRCIbp1_AlmIcon.left, gRCIbp1_AlmIcon.top, gRCIbp1_AlmIcon.right, gRCIbp1_AlmIcon.bottom);
	if(gCfgIbp1.bAlmControl == ALARM_OFF){
		FillBoxWithBitmap(hdc, gRCIbp1_AlmIcon.left+1,gRCIbp1_AlmIcon.top+1, 16, 16, &gIconAlarmOff);
	}

	//显示Label , Unit 
	SelectFont(hdc, gFontEnglish_16);
	//Label
	if(bDebugRect) Rectangle(hdc, gRCIbp1_Label.left, gRCIbp1_Label.top, gRCIbp1_Label.right, gRCIbp1_Label.bottom);
	memset(strLabel, 0, sizeof strLabel);
	GetStringFromResFile(gsLanguageRes, "IBP", gStrIbpLabelIndex[gCfgIbp1.bLabel], strLabel, sizeof strLabel);	
	memset(strUnit, 0, sizeof strUnit);
	switch(gCfgIbp1.bUnit){
		default :{
			GetStringFromResFile(gsLanguageRes, "GENERAL", "unit_mmhg", strUnit, sizeof strUnit);		
		}break;
		case IBPUNIT_KPA :{
			GetStringFromResFile(gsLanguageRes, "GENERAL", "unit_kpa", strUnit, sizeof strUnit);		
		}break;
		/*case IBPUNIT_CMH2O:{
			GetStringFromResFile(gsLanguageRes, "GENERAL", "unit_cmh2o", strUnit, sizeof strUnit);		
		}break;*/
	}
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%s(%s)", strLabel, strUnit);
	DrawText(hdc, strInfo, -1, &gRCIbp1_Label, DT_NOCLIP | DT_VCENTER | DT_LEFT | DT_SINGLELINE);
	//显示校准校0信息
/*
	if(gValueIbp1.bZeroResult==0){
				SelectFont(hdc, gFontEnglish_16);
				memset(strZero, 0, sizeof strZero);
				GetStringFromResFile(gsLanguageRes, "IBP", "ok_set_zero", strZero, sizeof strZero);	
				DrawText(hdc, strZero, -1, &gRCIbp1_Zero, DT_NOCLIP | DT_VCENTER | DT_LEFT | DT_SINGLELINE);

		}
*/
	if(gValueIbp1.bZeroResult==10)	{
				SelectFont(hdc, gFontEnglish_16);
				memset(strZero, 0, sizeof strZero);
				GetStringFromResFiles(gsLanguageRes, "IBP", "no_set_zero", strZero, sizeof strZero,"No zero");	
				DrawText(hdc, strZero, -1, &gRCIbp1_Zero, DT_NOCLIP | DT_VCENTER | DT_LEFT | DT_SINGLELINE);

		}

	if(gValueIbp1.bZeroResult!=0&&gValueIbp1.bZeroResult!=10){
				SelectFont(hdc, gFontEnglish_16);
				memset(strZero, 0, sizeof strZero);
				GetStringFromResFiles(gsLanguageRes, "IBP", "no_set_zero_fail", strZero, sizeof strZero,"Zero fail");	
				DrawText(hdc, strZero, -1, &gRCIbp1_Zero, DT_NOCLIP | DT_VCENTER | DT_LEFT | DT_SINGLELINE);

		}
	/*
	if(gValueIbp1.bCalResult==0){
				SelectFont(hdc, gFontEnglish_16);
				memset(strZero, 0, sizeof strZero);
				GetStringFromResFile(gsLanguageRes, "IBP", "ok_set_cal", strZero, sizeof strZero);	
				DrawText(hdc, strZero, -1, &gRCIbp1_Cal, DT_NOCLIP | DT_VCENTER | DT_LEFT | DT_SINGLELINE);		

		}
	*/
	if(gValueIbp1.bCalResult==10){
				SelectFont(hdc, gFontEnglish_16);
				memset(strZero, 0, sizeof strZero);
				GetStringFromResFile(gsLanguageRes, "IBP", "no_set_cal", strZero, sizeof strZero);	
				DrawText(hdc, strZero, -1, &gRCIbp1_Cal, DT_NOCLIP | DT_VCENTER | DT_LEFT | DT_SINGLELINE);	

		}
	if(gValueIbp1.bCalResult!=0&&gValueIbp1.bCalResult!=10){
				SelectFont(hdc, gFontEnglish_16);
				memset(strZero, 0, sizeof strZero);
				GetStringFromResFile(gsLanguageRes, "IBP", "no_set_cal_fail", strZero, sizeof strZero);	
				DrawText(hdc, strZero, -1, &gRCIbp1_Cal, DT_NOCLIP | DT_VCENTER | DT_LEFT | DT_SINGLELINE);	

		}


	if(gCfgIbp1.bViewMode == IBPVIEW_ALL){
		//显示间隔
		InitRectIbp1();
		if(bDebugRect) Rectangle(hdcParaMemExt[4], gRCIbp1_Inter.left, gRCIbp1_Inter.top, gRCIbp1_Inter.right, gRCIbp1_Inter.bottom);
#if SCREEN_1024		
		SelectFont(hdcParaMemExt[4], gFontTTF_50);
#elif SCREEN_640
		SelectFont(hdcParaMemExt[4], gFontTTF_40);	
#else
		SelectFont(hdcParaMemExt[4], gFontTTF_50Bk);	

#endif
		DrawText(hdcParaMemExt[4], "/", -1, &gRCIbp1_Inter, DT_NOCLIP | DT_VCENTER| DT_RIGHT | DT_SINGLELINE);
		
		//显示数据 SYS
		if(gbAlarmIbp1_Sys && (gCfgIbp1.bAlmControl != ALARM_OFF)){
			if(bFlashSys){
				SetTextColor(hdcParaMemExt[4], ALARMFLASH_COLOR);
			}
			else{
				SetTextColor(hdcParaMemExt[4], gCfgIbp1.iColor);		
			}
			bFlashSys = !bFlashSys;
		}
		else{
			SetTextColor(hdcParaMemExt[4], gCfgIbp1.iColor);			
		}
		if(bDebugRect) Rectangle(hdcParaMemExt[4], gRCIbp1_Sys.left, gRCIbp1_Sys.top, gRCIbp1_Sys.right, gRCIbp1_Sys.bottom);
		//SelectFont(hdcParaMemExt[4], gFontEnglish_16);
#if SCREEN_1024
		SelectFont(hdcParaMemExt[4], gFontEnglish_24);
#elif SCREEN_640
		SelectFont(hdcParaMemExt[4], gFontSmall);	
#else
		SelectFont(hdcParaMemExt[4], gFontEnglish_16);	
#endif

		memset(strInfo, 0, sizeof strInfo);
		GetStringFromResFiles(gsLanguageRes, "GENERAL", "sys", strInfo, sizeof strInfo,"Sys");		
		DrawText(hdcParaMemExt[4], strInfo, -1, &gRCIbp1_Sys, DT_NOCLIP | DT_TOP | DT_RIGHT | DT_SINGLELINE);
#if SCREEN_1024		
		SelectFont(hdcParaMemExt[4], gFontTTF_50);
#elif	SCREEN_640
		SelectFont(hdcParaMemExt[4], gFontTTF_30);	
#else
		SelectFont(hdcParaMemExt[4], gFontTTF_30Bk);
#endif
		DrawText(hdcParaMemExt[4], strSys, -1, &gRCIbp1_Sys, DT_NOCLIP | DT_VCENTER | DT_RIGHT | DT_SINGLELINE);
		
    	//显示数据 Dia
 	   if(gbAlarmIbp1_Dia && (gCfgIbp1.bAlmControl != ALARM_OFF)){
			if(bFlashDia){
				SetTextColor(hdcParaMemExt[4], ALARMFLASH_COLOR);
			}
			else{
				SetTextColor(hdcParaMemExt[4], gCfgIbp1.iColor);		
			}
			bFlashDia = !bFlashDia;
		}
		else{
			SetTextColor(hdcParaMemExt[4], gCfgIbp1.iColor);			
		}
		if(bDebugRect) Rectangle(hdcParaMemExt[4], gRCIbp1_Dia.left, gRCIbp1_Dia.top, gRCIbp1_Dia.right, gRCIbp1_Dia.bottom);
		//SelectFont(hdcParaMemExt[4], gFontEnglish_16);
#if SCREEN_1024
		SelectFont(hdcParaMemExt[4], gFontEnglish_24);
#elif SCREEN_640
		SelectFont(hdcParaMemExt[4], gFontSmall);	
#else
		SelectFont(hdcParaMemExt[4], gFontEnglish_16);	
#endif

		memset(strInfo, 0, sizeof strInfo);
		GetStringFromResFiles(gsLanguageRes, "GENERAL", "dia", strInfo, sizeof strInfo,"Dia");		
		DrawText(hdcParaMemExt[4], strInfo, -1, &gRCIbp1_Dia, DT_NOCLIP | DT_TOP | DT_LEFT| DT_SINGLELINE);
#if SCREEN_1024		
		SelectFont(hdcParaMemExt[4], gFontTTF_50);
#elif SCREEN_640
		SelectFont(hdcParaMemExt[4], gFontTTF_30);	
#else
		SelectFont(hdcParaMemExt[4], gFontTTF_30Bk);	
#endif
		DrawText(hdcParaMemExt[4], strDia, -1, &gRCIbp1_Dia, DT_NOCLIP | DT_VCENTER| DT_LEFT| DT_SINGLELINE);

		//显示数据 Mean
		if(gbAlarmIbp1_Mean && (gCfgIbp1.bAlmControl != ALARM_OFF)){
			if(bFlashMean){
				SetTextColor(hdcParaMemExt[4], ALARMFLASH_COLOR);
			}
			else{
				SetTextColor(hdcParaMemExt[4], gCfgIbp1.iColor);		
			}
			bFlashMean = !bFlashMean;
		}
		else{
			SetTextColor(hdcParaMemExt[4], gCfgIbp1.iColor);			
		}
		if(bDebugRect) Rectangle(hdcParaMemExt[4], gRCIbp1_Mean.left, gRCIbp1_Mean.top, gRCIbp1_Mean.right, gRCIbp1_Mean.bottom);
		//SelectFont(hdcParaMemExt[4], gFontEnglish_12);
#if SCREEN_1024
		SelectFont(hdcParaMemExt[4], gFontEnglish_24);
#elif SCREEN_640
		SelectFont(hdcParaMemExt[4], gFontSmall);	
#else
		SelectFont(hdcParaMemExt[4], gFontEnglish_12);	
#endif

		memset(strInfo, 0, sizeof strInfo);
		GetStringFromResFiles(gsLanguageRes, "GENERAL", "mean", strInfo, sizeof strInfo,"Mean");		
		DrawText(hdcParaMemExt[4], strInfo, -1, &gRCIbp1_Mean, DT_NOCLIP | DT_TOP | DT_CENTER| DT_SINGLELINE);
		//SelectFont(hdcParaMemExt[4], gFontTTF_20);
#if SCREEN_1024
		SelectFont(hdcParaMemExt[4], gFontEnglish_24);
#elif SCREEN_640
		SelectFont(hdcParaMemExt[4], gFontSystem);	
#else
		SelectFont(hdcParaMemExt[4], gFontTTF_20);	
#endif


		DrawText(hdcParaMemExt[4], strMean, -1, &gRCIbp1_Mean, DT_NOCLIP | DT_BOTTOM | DT_CENTER| DT_SINGLELINE);
		//TextOut(hdcParaMemExt[4], gRCIbp1_Mean.left-20, gRCIbp1_Mean.top+5, strMean);
	}
	else if(gCfgIbp1.bViewMode == IBPVIEW_M){
		//显示间隔
		InitRectIbp1();
		if(bDebugRect) Rectangle(hdcParaMemExt[4], gRCIbp1_Inter.left, gRCIbp1_Inter.top, gRCIbp1_Inter.right, gRCIbp1_Inter.bottom);
		SelectFont(hdcParaMemExt[4], gFontTTF_20);
		DrawText(hdcParaMemExt[4], "/", -1, &gRCIbp1_Inter, DT_NOCLIP | DT_BOTTOM | DT_RIGHT | DT_SINGLELINE);

		//显示数据 SYS
		if(gbAlarmIbp1_Sys && (gCfgIbp1.bAlmControl != ALARM_OFF)){
			if(bFlashSys){
				SetTextColor(hdcParaMemExt[4], ALARMFLASH_COLOR);
			}
			else{
				SetTextColor(hdcParaMemExt[4], gCfgIbp1.iColor);		
			}
			bFlashSys = !bFlashSys;
		}
		else{
			SetTextColor(hdcParaMemExt[4], gCfgIbp1.iColor);			
		}
		if(bDebugRect) Rectangle(hdcParaMemExt[4], gRCIbp1_Sys.left, gRCIbp1_Sys.top, gRCIbp1_Sys.right, gRCIbp1_Sys.bottom);
		//SelectFont(hdcParaMemExt[4], gFontEnglish_16);
#if SCREEN_1024
		SelectFont(hdcParaMemExt[4], gFontEnglish_24);
#elif SCREEN_640
		SelectFont(hdcParaMemExt[4], gFontSmall);	
#else
		SelectFont(hdcParaMemExt[4], gFontEnglish_16);	
#endif

		memset(strInfo, 0, sizeof strInfo);
		GetStringFromResFile(gsLanguageRes, "GENERAL", "sys", strInfo, sizeof strInfo);		
		DrawText(hdcParaMemExt[4], strInfo, -1, &gRCIbp1_Sys, DT_NOCLIP | DT_TOP | DT_RIGHT | DT_SINGLELINE);
		//SelectFont(hdcParaMemExt[4], gFontTTF_20);
#if SCREEN_1024
		SelectFont(hdcParaMemExt[4], gFontTTF_90Bk);
#elif SCREEN_640
		SelectFont(hdcParaMemExt[4], gFontEnglish_24);	
#else
		SelectFont(hdcParaMemExt[4], gFontTTF_20);	
#endif

		DrawText(hdcParaMemExt[4], strSys, -1, &gRCIbp1_Sys, DT_NOCLIP | DT_BOTTOM | DT_RIGHT | DT_SINGLELINE);
		//显示数据 Dia
		if(gbAlarmIbp1_Dia && (gCfgIbp1.bAlmControl != ALARM_OFF)){
			if(bFlashDia){
				SetTextColor(hdcParaMemExt[4], ALARMFLASH_COLOR);
			}
			else{
				SetTextColor(hdcParaMemExt[4], gCfgIbp1.iColor);		
			}
			bFlashDia = !bFlashDia;
		}
		else{
			SetTextColor(hdcParaMemExt[4], gCfgIbp1.iColor);			
		}

		if(bDebugRect) Rectangle(hdcParaMemExt[4], gRCIbp1_Dia.left, gRCIbp1_Dia.top, gRCIbp1_Dia.right, gRCIbp1_Dia.bottom);
		//SelectFont(hdcParaMemExt[4], gFontEnglish_16);
#if SCREEN_1024
		SelectFont(hdcParaMemExt[4], gFontEnglish_24);
#elif SCREEN_640
		SelectFont(hdcParaMemExt[4], gFontSmall);	
#else
		SelectFont(hdcParaMemExt[4], gFontEnglish_16);	
#endif

		memset(strInfo, 0, sizeof strInfo);
		GetStringFromResFile(gsLanguageRes, "GENERAL", "dia", strInfo, sizeof strInfo);		
		DrawText(hdcParaMemExt[4], strInfo, -1, &gRCIbp1_Dia, DT_NOCLIP | DT_TOP | DT_LEFT| DT_SINGLELINE);
		//SelectFont(hdcParaMemExt[4], gFontTTF_20);
#if SCREEN_1024
		SelectFont(hdcParaMemExt[4], gFontTTF_90Bk);
#elif SCREEN_640
		SelectFont(hdcParaMemExt[4], gFontEnglish_24);	
#else
		SelectFont(hdcParaMemExt[4], gFontTTF_20);	
#endif

		DrawText(hdcParaMemExt[4], strDia, -1, &gRCIbp1_Dia, DT_NOCLIP | DT_BOTTOM | DT_LEFT| DT_SINGLELINE);

		//显示数据 Mean
		if(gbAlarmIbp1_Mean && (gCfgIbp1.bAlmControl != ALARM_OFF)){
			if(bFlashMean){
				SetTextColor(hdcParaMemExt[4], ALARMFLASH_COLOR);
			}
			else{
				SetTextColor(hdcParaMemExt[4], gCfgIbp1.iColor);		
			}
			bFlashMean = !bFlashMean;
		}
		else{
			SetTextColor(hdcParaMemExt[4], gCfgIbp1.iColor);			
		}
		if(bDebugRect) Rectangle(hdcParaMemExt[4], gRCIbp1_Mean.left, gRCIbp1_Mean.top, gRCIbp1_Mean.right, gRCIbp1_Mean.bottom);
		//SelectFont(hdcParaMemExt[4], gFontEnglish_16);
#if SCREEN_1024
		SelectFont(hdcParaMemExt[4], gFontEnglish_24);
#elif SCREEN_640
		SelectFont(hdcParaMemExt[4], gFontEnglish_12);	
#else
		SelectFont(hdcParaMemExt[4], gFontEnglish_12);	
#endif

		memset(strInfo, 0, sizeof strInfo);
		GetStringFromResFile(gsLanguageRes, "GENERAL", "mean", strInfo, sizeof strInfo);		
		DrawText(hdcParaMemExt[4], strInfo, -1, &gRCIbp1_Mean, DT_NOCLIP | DT_TOP | DT_CENTER| DT_SINGLELINE);
#if SCREEN_1024		
		SelectFont(hdcParaMemExt[4], gFontTTF_90Bk);
#elif SCREEN_640
		SelectFont(hdcParaMemExt[4], gFontTTF_30);
#else
		SelectFont(hdcParaMemExt[4], gFontTTF_40Bk);
#endif		
		DrawText(hdcParaMemExt[4], strMean, -1, &gRCIbp1_Mean, DT_NOCLIP | DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
	}
	
	//Alarm Limit
	if(gCfgAlarm.bViewLimit){
		if(bDebugRect) Rectangle(hdcParaMemExt[4], gRCIbp1_limit_Sys.left, gRCIbp1_limit_Sys.top, gRCIbp1_limit_Sys.right, gRCIbp1_limit_Sys.bottom);
		if(bDebugRect) Rectangle(hdcParaMemExt[4], gRCIbp1_limit_Dia.left, gRCIbp1_limit_Dia.top, gRCIbp1_limit_Dia.right, gRCIbp1_limit_Dia.bottom);
		if(bDebugRect) Rectangle(hdcParaMemExt[4], gRCIbp1_limit_Mean.left, gRCIbp1_limit_Mean.top, gRCIbp1_limit_Mean.right, gRCIbp1_limit_Mean.bottom);
		SetTextColor(hdcParaMemExt[4], PARACOL_LIMIT);
		SelectFont(hdcParaMemExt[4], gFontSmall);
		
		memset(strLimitS_h, 0, sizeof strLimitS_h);
		memset(strLimitS_l, 0, sizeof strLimitS_l);
		memset(strLimitD_h, 0, sizeof strLimitD_h);
		memset(strLimitD_l, 0, sizeof strLimitD_l);
		memset(strLimitM_h, 0, sizeof strLimitM_h);
		memset(strLimitM_l, 0, sizeof strLimitM_l);
		switch(gCfgIbp1.bUnit){
			//case IBPUNIT_MMHG
				default :{//mmHg
				snprintf(strLimitS_h, sizeof strLimitS_h, "%d", gCfgIbp1.iHigh_Sys);
				snprintf(strLimitS_l, sizeof strLimitS_l, "%d", gCfgIbp1.iLow_Sys);
				snprintf(strLimitD_h, sizeof strLimitD_h, "%d", gCfgIbp1.iHigh_Dia);
				snprintf(strLimitD_l, sizeof strLimitD_l, "%d", gCfgIbp1.iLow_Dia);
				snprintf(strLimitM_h, sizeof strLimitM_h, "%d", gCfgIbp1.iHigh_Mean);
				snprintf(strLimitM_l, sizeof strLimitM_l, "%d", gCfgIbp1.iLow_Mean);
			}break;
			case IBPUNIT_KPA:{//kPa
				snprintf(strLimitS_h, sizeof strLimitS_h, "%.1f", (gCfgIbp1.iHigh_Sys)/mmHgTokPa);
				snprintf(strLimitS_l, sizeof strLimitS_l, "%.1f", (gCfgIbp1.iLow_Sys)/mmHgTokPa);
				snprintf(strLimitD_h, sizeof strLimitD_h, "%.1f", (gCfgIbp1.iHigh_Dia)/mmHgTokPa);
				snprintf(strLimitD_l, sizeof strLimitD_l, "%.1f", (gCfgIbp1.iLow_Dia)/mmHgTokPa);
				snprintf(strLimitM_h, sizeof strLimitM_h, "%.1f", (gCfgIbp1.iHigh_Mean)/mmHgTokPa);
				snprintf(strLimitM_l, sizeof strLimitM_l, "%.1f", (gCfgIbp1.iLow_Mean)/mmHgTokPa);
			}break;
			/*case IBPUNIT_CMH2O:{
				snprintf(strLimitS_h, sizeof strLimitS_h, "%d", (int)((gCfgIbp1.iHigh_Sys)*1.36));
				snprintf(strLimitS_l, sizeof strLimitS_l, "%d", (int)((gCfgIbp1.iLow_Sys)*1.36));
				snprintf(strLimitD_h, sizeof strLimitD_h, "%d", (int)((gCfgIbp1.iHigh_Dia)*1.36));
				snprintf(strLimitD_l, sizeof strLimitD_l, "%d", (int)((gCfgIbp1.iLow_Dia)*1.36));
				snprintf(strLimitM_h, sizeof strLimitM_h, "%d", (int)((gCfgIbp1.iHigh_Mean)*1.36));
				snprintf(strLimitM_l, sizeof strLimitM_l, "%d", (int)((gCfgIbp1.iLow_Mean)*1.36));
			}break;*/
		}
		DrawText(hdcParaMemExt[4], strLimitS_h, -1, &gRCIbp1_limit_Sys, DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);
		DrawText(hdcParaMemExt[4], strLimitS_l, -1, &gRCIbp1_limit_Sys, DT_NOCLIP | DT_BOTTOM | DT_LEFT | DT_SINGLELINE);	
		DrawText(hdcParaMemExt[4], strLimitD_h, -1, &gRCIbp1_limit_Dia, DT_NOCLIP | DT_TOP | DT_RIGHT | DT_SINGLELINE);
		DrawText(hdcParaMemExt[4], strLimitD_l, -1, &gRCIbp1_limit_Dia, DT_NOCLIP | DT_BOTTOM | DT_RIGHT | DT_SINGLELINE);	
		if(gCfgIbp1.bViewMode == IBPVIEW_M){
			DrawText(hdcParaMemExt[4], strLimitM_h, -1, &gRCIbp1_limit_Mean, DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);
			DrawText(hdcParaMemExt[4], strLimitM_l, -1, &gRCIbp1_limit_Mean, DT_NOCLIP | DT_BOTTOM | DT_LEFT | DT_SINGLELINE);	 	
		}
		else{
			DrawText(hdcParaMemExt[4], strLimitM_h, -1, &gRCIbp1_limit_Mean, DT_NOCLIP | DT_TOP | DT_RIGHT | DT_SINGLELINE);
			DrawText(hdcParaMemExt[4], strLimitM_l, -1, &gRCIbp1_limit_Mean, DT_NOCLIP | DT_BOTTOM | DT_RIGHT | DT_SINGLELINE);	
		}
	}
		//绘制框架
	Rectangle(hdc, gRCIbp1.left, gRCIbp1.top, gRCIbp1.right, gRCIbp1.bottom);
		
	//Info
	if(bDebugRect) {
		//SetPenColor(hdcParaMemExt[4], gCfgIbp1.iColor);	
		//Label
		Rectangle(hdcParaMemExt[4], gRCIbp1_Label.left, gRCIbp1_Label.top, gRCIbp1_Label.right, gRCIbp1_Label.bottom);
		//SetPenColor(hdcParaMemExt[4], COLOR_yellow);
		//Alarm Icon
		Rectangle(hdcParaMemExt[4], gRCIbp1_AlmIcon.left, gRCIbp1_AlmIcon.top, gRCIbp1_AlmIcon.right, gRCIbp1_AlmIcon.bottom);
		//Info
		Rectangle(hdcParaMemExt[4], gRCIbp1_Info.left, gRCIbp1_Info.top, gRCIbp1_Info.right, gRCIbp1_Info.bottom);
		//Data Sys
		Rectangle(hdcParaMemExt[4], gRCIbp1_Sys.left, gRCIbp1_Sys.top, gRCIbp1_Sys.right, gRCIbp1_Sys.bottom);
		//Inter
		Rectangle(hdcParaMemExt[4], gRCIbp1_Inter.left, gRCIbp1_Inter.top, gRCIbp1_Inter.right, gRCIbp1_Inter.bottom);
		//Data Dia
		Rectangle(hdcParaMemExt[4], gRCIbp1_Dia.left, gRCIbp1_Dia.top, gRCIbp1_Dia.right, gRCIbp1_Dia.bottom);
		//Data Mean
		Rectangle(hdcParaMemExt[4], gRCIbp1_Mean.left, gRCIbp1_Mean.top, gRCIbp1_Mean.right, gRCIbp1_Mean.bottom);
		//Limit
		Rectangle(hdcParaMemExt[4], gRCIbp1_limit_Sys.left, gRCIbp1_limit_Sys.top, gRCIbp1_limit_Sys.right, gRCIbp1_limit_Sys.bottom);
		Rectangle(hdcParaMemExt[4], gRCIbp1_limit_Dia.left, gRCIbp1_limit_Dia.top, gRCIbp1_limit_Dia.right, gRCIbp1_limit_Dia.bottom);
		Rectangle(hdcParaMemExt[4], gRCIbp1_limit_Mean.left, gRCIbp1_limit_Mean.top, gRCIbp1_limit_Mean.right, gRCIbp1_limit_Mean.bottom);

		}
	

	
	//拷贝DC
 	if(Draw_Mem){
 		BitBlt(hdc, 
 		gRCIbp1.left, gRCIbp1.top, RECTW(gRCIbp1)+1, RECTH(gRCIbp1)+1, 
 		hdcParaExt[4], 
 		gRCIbp1.left, gRCIbp1.top, 
 		0);
 	}
	return 0;
}

//根据模块配置以及屏幕类型初始化参数显示区域
static int InitRectIbp2()
{
		
	if(1){
	SetRect(&gRCIbp2, 
			 gRcParasExt[5].left, 
			 gRcParasExt[5].top, 
			 gRcParasExt[5].right, 
			 gRcParasExt[5].bottom-1);	
	}
	SetRect(&gRCIbp2_Zero, 50,1,130,20);
	//gCfgIbp2.iColor=200;//橙色
	//Label
	SetRect(&gRCIbp2_Label, gRCIbp2.left+3, gRCIbp2.top+1, gRCIbp2.right, gRCIbp2.top+17);
 	//Alarm Icon
	SetRect(&gRCIbp2_AlmIcon, gRCIbp2.right-19,gRCIbp2.top,gRCIbp2.right, gRCIbp2.top+17 );
 	//Info
	SetRect(&gRCIbp2_Info, gRCIbp2.left+35-giLeftDec,gRCIbp2.bottom-20,gRCIbp2.right,gRCIbp2.bottom);
	SetRect(&gRCIbp2_Zero, gRCIbp2.right-67,1,gRCIbp2.right-40,20);	
	SetRect(&gRCIbp2_Cal, gRCIbp2.right-118,1,gRCIbp2.right-100,20);
#if SCREEN_1024
	//显示模式不同，区域划分也不同
	if(gCfgIbp2.bViewMode == IBPVIEW_ALL){
		//Data Sys
		SetRect(&gRCIbp2_Sys, gRCIbp2.left+35-giLeftDec,gRCIbp2.top+21,gRCIbp2.left+120-giLeftDec,gRCIbp2.bottom-20);
		//Inter
		SetRect(&gRCIbp2_Inter, gRCIbp2.left+120-giLeftDec,gRCIbp2.top+21,gRCIbp2.left+140-giLeftDec,gRCIbp2.bottom-20);
		//Data Dia
		SetRect(&gRCIbp2_Dia, gRCIbp2.left+140-giLeftDec,gRCIbp2.top+21,gRCIbp2.right-60,gRCIbp2.bottom-20);
		//Data Mean
		SetRect(&gRCIbp2_Mean, gRCIbp2.right-57,gRCIbp2.top+21,gRCIbp2.right-1,gRCIbp2.top+75);
		//Limit
		SetRect(&gRCIbp2_limit_Sys, gRCIbp2_Sys.left,gRCIbp2_Sys.top, gRCIbp2_Sys.left+40,gRCIbp2_Sys.top+30);
		SetRect(&gRCIbp2_limit_Dia, gRCIbp2_Dia.right-40,gRCIbp2_Dia.top, gRCIbp2_Dia.right, gRCIbp2_Dia.top+30);
		SetRect(&gRCIbp2_limit_Mean, gRCIbp2_Mean.right-50,gRCIbp2_Mean.top, gRCIbp2_Mean.right-10, gRCIbp2_Mean.top+30);
	}
	else if(gCfgIbp2.bViewMode == IBPVIEW_M){
		//Data Mean
		SetRect(&gRCIbp2_Mean, gRCIbp2.left+35-giLeftDec+10, gRCIbp2.top+21, gRCIbp2.left+170-giLeftDec+10, gRCIbp2.bottom-20);
		//Data Sys
		SetRect(&gRCIbp2_Sys, gRCIbp2.left+175-giLeftDec, gRCIbp2.top+21, gRCIbp2.left+225-giLeftDec, gRCIbp2.top+70);
		//Inter
		SetRect(&gRCIbp2_Inter, gRCIbp2.left+225-giLeftDec, gRCIbp2.top+21, gRCIbp2.left+235-giLeftDec, gRCIbp2.top+70);
		//Data Dia
		SetRect(&gRCIbp2_Dia, gRCIbp2.left+235-giLeftDec, gRCIbp2.top+21, gRCIbp2.left+285-giLeftDec, gRCIbp2.top+70);
		//Limit
		SetRect(&gRCIbp2_limit_Sys, gRCIbp2_Sys.left,gRCIbp2_Sys.top, gRCIbp2_Sys.left+40,gRCIbp2_Sys.top+30);
		SetRect(&gRCIbp2_limit_Dia, gRCIbp2_Dia.right-40,gRCIbp2_Dia.top, gRCIbp2_Dia.right, gRCIbp2_Dia.top+30);
		SetRect(&gRCIbp2_limit_Mean, gRCIbp2_Mean.left, gRCIbp2_Mean.top, gRCIbp2_Mean.left+40, gRCIbp2_Mean.top+30);
	}
#elif SCREEN_640
	//显示模式不同，区域划分也不同
	if(gCfgIbp2.bViewMode == IBPVIEW_ALL){
		//Data Sys
		SetRect(&gRCIbp2_Sys, gRCIbp2.left+35,gRCIbp2.top+20,gRCIbp2.left+85,gRCIbp2.top+75);
		//Inter
		SetRect(&gRCIbp2_Inter, gRCIbp2.left+85,gRCIbp2.top+20,gRCIbp2.left+95,gRCIbp2.top+80);
		//Data Dia
		SetRect(&gRCIbp2_Dia,  gRCIbp2.left+95,gRCIbp2.top+20, gRCIbp2.left+145,gRCIbp2.top+75);
		//Data Mean
		SetRect(&gRCIbp2_Mean,  gRCIbp2.right-35,gRCIbp2.bottom-30, gRCIbp2.right-10,gRCIbp2.bottom-1);
		//Limit
		SetRect(&gRCIbp2_limit_Sys,  gRCIbp2.left+45,gRCIbp2.bottom-25,  gRCIbp2.left+65,gRCIbp2.bottom-1);
		SetRect(&gRCIbp2_limit_Dia,  gRCIbp2.left+90,gRCIbp2.bottom-25,  gRCIbp2.left+110, gRCIbp2.bottom-1);
		SetRect(&gRCIbp2_limit_Mean,  gRCIbp2.right-70 ,gRCIbp2.bottom-25,  gRCIbp2.right-40, gRCIbp2.bottom-1);
	}
	else if(gCfgIbp2.bViewMode == IBPVIEW_M){
		//Data Mean
		SetRect(&gRCIbp2_Mean,  gRCIbp2.left+30, gRCIbp2.top+20,  gRCIbp2.left+70, gRCIbp2.top+75);
		//Data Sys
		SetRect(&gRCIbp2_Sys,  gRCIbp2.left+105, gRCIbp2.top+20,  gRCIbp2.left+125, gRCIbp2.top+55);
		//Inter
		SetRect(&gRCIbp2_Inter,  gRCIbp2.left+125, gRCIbp2.top+20, gRCIbp2.left+135, gRCIbp2.top+60);
		//Data Dia
		SetRect(&gRCIbp2_Dia,  gRCIbp2.left+140, gRCIbp2.top+20, gRCIbp2.left+160, gRCIbp2.top+55);
		//Limit
		SetRect(&gRCIbp2_limit_Sys,  gRCIbp2.left+100,gRCIbp2.bottom-25,  gRCIbp2.left+120,gRCIbp2.bottom-1);
		SetRect(&gRCIbp2_limit_Dia,  gRCIbp2.left+140,gRCIbp2.bottom-25,  gRCIbp2.left+160, gRCIbp2.bottom-1);
		SetRect(&gRCIbp2_limit_Mean,  gRCIbp2.left+1, gRCIbp2.bottom-25,  gRCIbp2.left+20, gRCIbp2.bottom-1);
	}
#else
	//显示模式不同，区域划分也不同
	if(gCfgIbp2.bViewMode == IBPVIEW_M){
		//Data Mean
		SetRect(&gRCIbp2_Mean, gRCIbp2.left+50, gRCIbp2.top+20, gRCIbp2.left+90, gRCIbp2.top+95);
		//Data Sys
		SetRect(&gRCIbp2_Sys, gRCIbp2.left+120-giLeftDec+40, gRCIbp2.top+21, gRCIbp2.left+170-giLeftDec+40, gRCIbp2.top+70);
		//Inter
		SetRect(&gRCIbp2_Inter, gRCIbp2.left+180-43-5+35, gRCIbp2.top+20, gRCIbp2.left+190-34-5+35, gRCIbp2.top+70);
		//Data Dia
		SetRect(&gRCIbp2_Dia, gRCIbp2.left+185-giLeftDec+35, gRCIbp2.top+21, gRCIbp2.left+185-giLeftDec+55, gRCIbp2.top+70);
		//Limit
		SetRect(&gRCIbp2_limit_Sys,gRCIbp2.left+120-giLeftDec+70 ,gRCIbp2.top+85,gRCIbp2.left+120-giLeftDec+100 ,gRCIbp2.bottom);
		SetRect(&gRCIbp2_limit_Dia, gRCIbp2.left+185-giLeftDec+25 ,gRCIbp2.top+85,  gRCIbp2.left+185-giLeftDec+65, gRCIbp2.bottom);
		SetRect(&gRCIbp2_limit_Mean, 1,gRCIbp2.top+85,30, gRCIbp2.bottom);

	}
	else if(gCfgIbp2.bViewMode == IBPVIEW_ALL){
				//Data Sys
		SetRect(&gRCIbp2_Sys, gRCIbp2.left+65,gRCIbp2.top+20,gRCIbp2.left+85,gRCIbp2.top+85);
		//Inter
		SetRect(&gRCIbp2_Inter, gRCIbp2.left+65,gRCIbp2.top+10,gRCIbp2.left+100,gRCIbp2.top+100);
		//Data Dia
		SetRect(&gRCIbp2_Dia, gRCIbp2.left+100,gRCIbp2.top+20,gRCIbp2.left+120,gRCIbp2.top+85);
		//Data Mean
		SetRect(&gRCIbp2_Mean, gRCIbp2.left+170,gRCIbp2.top+65,gRCIbp2.left+225,gRCIbp2.top+110);
		//Limit
		SetRect(&gRCIbp2_limit_Sys, gRCIbp2.left+55,90, gRCIbp2.left+80,gRCIbp2.bottom);
		SetRect(&gRCIbp2_limit_Dia, gRCIbp2.left+85,90, gRCIbp2.left+130,gRCIbp2.bottom);
		SetRect(&gRCIbp2_limit_Mean,gRCIbp2.left+235,90,gRCIbp2.right,gRCIbp2.bottom);
	}
	
#endif		
	return 0;
}

//显示IBP2参数
static int ViewIbp2(HDC hdc, BYTE bInterface, BYTE bMemCopy)
{
	unsigned char strLabel[10]={0};
	unsigned char strUnit[10]={0};
	unsigned char strSys[6]={0};
	unsigned char strDia[6]={0};
	unsigned char strMean[6]={0};
	unsigned char strZero[20]={0};
	static BOOL bFlashSys = FALSE;
	static BOOL bFlashDia = FALSE;
	static BOOL bFlashMean = FALSE;
	int iSys, iDia, iMean;
	unsigned char strLimitS_h[6]={0};
	unsigned char strLimitS_l[6]={0};
	unsigned char strLimitD_h[6]={0};
	unsigned char strLimitD_l[6]={0};
	unsigned char strLimitM_h[6]={0};
	unsigned char strLimitM_l[6]={0};
	
	iSys = gValueIbp2.iSys;
	iDia = gValueIbp2.iDia;
	iMean = gValueIbp2.iMean;
	
// 	iSys = 255; iDia = 255; iMean = 255;
	
// 	gValueIbp2.bOff = FALSE;
	
	switch(gCfgIbp2.bUnit){
		//case IBPUNIT_MMHG
		default :{
			if((!gValueIbp2.bOff)&&(iSys>=-50)&&(iDia>=-50)&&(iMean>=-50)){
				snprintf(strSys, sizeof strSys, "%d", iSys);
				snprintf(strDia, sizeof strDia, "%d", iDia);
				snprintf(strMean, sizeof strMean, "%d", iMean);
			}
			else{
				snprintf(strSys, sizeof strSys, "---");
				snprintf(strDia, sizeof strDia, "---");
				snprintf(strMean, sizeof strMean, "---");
			}
		}break;
		case IBPUNIT_KPA:{
			if((!gValueIbp2.bOff)&&(iSys>=-50)&&(iDia>=-50)&&(iMean>=-50)){
				snprintf(strSys, sizeof strSys, "%3.1f", (float)(iSys / 7.5));
				snprintf(strDia, sizeof strDia, "%3.1f", (float)(iDia/ 7.5));
				snprintf(strMean, sizeof strMean, "%3.1f", (float)(iMean/ 7.5));
			}
			else{
				snprintf(strSys, sizeof strSys, "--.-");
				snprintf(strDia, sizeof strDia, "--.-");
				snprintf(strMean, sizeof strMean, "--.-");
			}
		}break;
		/*case IBPUNIT_CMH2O:{
			if(!gValueIbp2.bOff){
				snprintf(strSys, sizeof strSys, "%d", (int)(iSys *1.36));
				snprintf(strDia, sizeof strDia, "%d", (int)(iDia *1.36));
				snprintf(strMean, sizeof strMean, "%d", (int)(iMean *1.36));
			}
			else{
				snprintf(strSys, sizeof strSys, "---");
				snprintf(strDia, sizeof strDia, "---");
				snprintf(strMean, sizeof strMean, "---");
			}
		}break;*/
	}
	
//  	snprintf(strHR, sizeof strHR, "255");
	SetTextColor(hdc, gCfgIbp2.iColor);
		
	//显示 Alarm Icon
	if(bDebugRect) Rectangle(hdc, gRCIbp2_AlmIcon.left, gRCIbp2_AlmIcon.top, gRCIbp2_AlmIcon.right, gRCIbp2_AlmIcon.bottom);
	if(gCfgIbp2.bAlmControl == ALARM_OFF){
		FillBoxWithBitmap(hdc, gRCIbp2_AlmIcon.left+1,gRCIbp2_AlmIcon.top+1, 16, 16, &gIconAlarmOff);
	}

	//显示Label , Unit 
	SelectFont(hdc, gFontEnglish_16);
	//Label
	if(bDebugRect) Rectangle(hdc, gRCIbp2_Label.left, gRCIbp2_Label.top, gRCIbp2_Label.right, gRCIbp2_Label.bottom);
	memset(strLabel, 0, sizeof strLabel);
	GetStringFromResFile(gsLanguageRes, "IBP", gStrIbpLabelIndex[gCfgIbp2.bLabel], strLabel, sizeof strLabel);	
	memset(strUnit, 0, sizeof strUnit);
	switch(gCfgIbp2.bUnit){
		default:{
			GetStringFromResFile(gsLanguageRes, "GENERAL", "unit_mmhg", strUnit, sizeof strUnit);		
		}break;
		case IBPUNIT_KPA :{
			GetStringFromResFile(gsLanguageRes, "GENERAL", "unit_kpa", strUnit, sizeof strUnit);		
		}break;
		/*case IBPUNIT_CMH2O:{
			GetStringFromResFile(gsLanguageRes, "GENERAL", "unit_cmh2o", strUnit, sizeof strUnit);		
		}break;*/
	}
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%s(%s)", strLabel, strUnit);
	DrawText(hdc, strInfo, -1, &gRCIbp2_Label, DT_NOCLIP | DT_VCENTER | DT_LEFT | DT_SINGLELINE);
	//显示ibp2校准校0信息
	/*
	if(gValueIbp2.bZeroResult==0){
				SelectFont(hdc, gFontEnglish_16);
				memset(strZero, 0, sizeof strZero);
				GetStringFromResFile(gsLanguageRes, "IBP", "ok_set_zero", strZero, sizeof strZero);	
				DrawText(hdc, strZero, -1, &gRCIbp2_Zero, DT_NOCLIP | DT_VCENTER | DT_LEFT | DT_SINGLELINE);	
		}
		*/
	if(gValueIbp2.bZeroResult==10)	{
				SelectFont(hdc, gFontEnglish_16);
				memset(strZero, 0, sizeof strZero);
				GetStringFromResFile(gsLanguageRes, "IBP", "no_set_zero", strZero, sizeof strZero);	
				DrawText(hdc, strZero, -1, &gRCIbp2_Zero, DT_NOCLIP | DT_VCENTER | DT_LEFT | DT_SINGLELINE);				
		}
	
	if(gValueIbp2.bZeroResult!=0&&gValueIbp2.bZeroResult!=10){
				SelectFont(hdc, gFontEnglish_16);
				memset(strZero, 0, sizeof strZero);
				GetStringFromResFile(gsLanguageRes, "IBP", "no_set_zero_fail", strZero, sizeof strZero);	
				DrawText(hdc, strZero, -1, &gRCIbp2_Zero, DT_NOCLIP | DT_VCENTER | DT_LEFT | DT_SINGLELINE);
		}
/*
	if(gValueIbp2.bCalResult==0){
				SelectFont(hdc, gFontEnglish_16);
				memset(strZero, 0, sizeof strZero);
				GetStringFromResFile(gsLanguageRes, "IBP", "ok_set_cal", strZero, sizeof strZero);	
				DrawText(hdc, strZero, -1, &gRCIbp2_Cal, DT_NOCLIP | DT_VCENTER | DT_LEFT | DT_SINGLELINE);		
		}
*/	
	if(gValueIbp2.bCalResult==10){
				SelectFont(hdc, gFontEnglish_16);
				memset(strZero, 0, sizeof strZero);
				GetStringFromResFile(gsLanguageRes, "IBP", "no_set_cal", strZero, sizeof strZero);	
				DrawText(hdc, strZero, -1, &gRCIbp2_Cal, DT_NOCLIP | DT_VCENTER | DT_LEFT | DT_SINGLELINE);	
		}
	if(gValueIbp2.bCalResult!=0&&gValueIbp2.bCalResult!=10){
				SelectFont(hdc, gFontEnglish_16);
				memset(strZero, 0, sizeof strZero);
				GetStringFromResFile(gsLanguageRes, "IBP", "no_set_cal_fail", strZero, sizeof strZero);	
				DrawText(hdc, strZero, -1, &gRCIbp2_Cal, DT_NOCLIP | DT_VCENTER | DT_LEFT | DT_SINGLELINE);	
		}



	
	if(gCfgIbp2.bViewMode == IBPVIEW_ALL){
		InitRectIbp2();
		//显示间隔                                                             
		if(bDebugRect) Rectangle(hdcParaMemExt[5], gRCIbp2_Inter.left, gRCIbp2_Inter.top, gRCIbp2_Inter.right, gRCIbp2_Inter.bottom);
#if SCREEN_1024
		SelectFont(hdcParaMemExt[5], gFontTTF_50);
#elif	SCREEN_640
		SelectFont(hdcParaMemExt[5], gFontTTF_40);	
#else
		SelectFont(hdcParaMemExt[5], gFontTTF_50Bk);	
#endif
		DrawText(hdcParaMemExt[5], "/", -1, &gRCIbp2_Inter, DT_NOCLIP | DT_VCENTER| DT_RIGHT | DT_SINGLELINE);
		
		//显示数据 SYS
		if(gbAlarmIbp2_Sys && (gCfgIbp2.bAlmControl != ALARM_OFF)){
			if(bFlashSys){
				SetTextColor(hdcParaMemExt[5], ALARMFLASH_COLOR);
			}
			else{
				SetTextColor(hdcParaMemExt[5], gCfgIbp2.iColor);		
			}
			bFlashSys = !bFlashSys;
		}
		else{
			SetTextColor(hdcParaMemExt[5], gCfgIbp2.iColor);			
		}
		if(bDebugRect) Rectangle(hdcParaMemExt[5], gRCIbp2_Sys.left, gRCIbp2_Sys.top, gRCIbp2_Sys.right, gRCIbp2_Sys.bottom);
		//SelectFont(hdcParaMemExt[5], gFontEnglish_16);
#if SCREEN_1024
		SelectFont(hdcParaMemExt[5], gFontEnglish_24);
#elif SCREEN_640
		SelectFont(hdcParaMemExt[5], gFontSmall);	
#else
		SelectFont(hdcParaMemExt[5], gFontEnglish_16);	
#endif

		memset(strInfo, 0, sizeof strInfo);
		GetStringFromResFile(gsLanguageRes, "GENERAL", "sys", strInfo, sizeof strInfo);		
		DrawText(hdcParaMemExt[5], strInfo, -1, &gRCIbp2_Sys, DT_NOCLIP | DT_TOP | DT_RIGHT | DT_SINGLELINE);
#if SCREEN_1024
		SelectFont(hdcParaMemExt[5], gFontTTF_50);
#elif	SCREEN_640
		SelectFont(hdcParaMemExt[5], gFontTTF_30);	
#else
		SelectFont(hdcParaMemExt[5], gFontTTF_30Bk);	

#endif
		DrawText(hdcParaMemExt[5], strSys, -1, &gRCIbp2_Sys, DT_NOCLIP | DT_VCENTER | DT_RIGHT | DT_SINGLELINE);

		//显示数据 Dia
		if(gbAlarmIbp2_Dia && (gCfgIbp2.bAlmControl != ALARM_OFF)){
			if(bFlashDia){
				SetTextColor(hdcParaMemExt[5], ALARMFLASH_COLOR);
			}
			else{
				SetTextColor(hdcParaMemExt[5], gCfgIbp2.iColor);		
			}
			bFlashDia = !bFlashDia;
		}
		else{
			SetTextColor(hdcParaMemExt[5], gCfgIbp2.iColor);			
		}
		if(bDebugRect) Rectangle(hdcParaMemExt[5], gRCIbp2_Dia.left, gRCIbp2_Dia.top, gRCIbp2_Dia.right, gRCIbp2_Dia.bottom);
		//SelectFont(hdcParaMemExt[5], gFontEnglish_16);
#if SCREEN_1024
		SelectFont(hdcParaMemExt[5], gFontEnglish_24);
#elif SCREEN_640
		SelectFont(hdcParaMemExt[5], gFontSmall);	
#else
		SelectFont(hdcParaMemExt[5], gFontEnglish_16);	
#endif

		memset(strInfo, 0, sizeof strInfo);
		GetStringFromResFile(gsLanguageRes, "GENERAL", "dia", strInfo, sizeof strInfo);		
		DrawText(hdcParaMemExt[5], strInfo, -1, &gRCIbp2_Dia, DT_NOCLIP | DT_TOP | DT_LEFT| DT_SINGLELINE);
#if SCREEN_1024
		SelectFont(hdcParaMemExt[5], gFontTTF_50);
#elif SCREEN_640
		SelectFont(hdcParaMemExt[5], gFontTTF_30);	
#else
		SelectFont(hdcParaMemExt[5], gFontTTF_30Bk);	

#endif
		DrawText(hdcParaMemExt[5], strDia, -1, &gRCIbp2_Dia, DT_NOCLIP | DT_VCENTER| DT_LEFT| DT_SINGLELINE);

		//显示数据 Mean
		if(gbAlarmIbp2_Mean && (gCfgIbp2.bAlmControl != ALARM_OFF)){
			if(bFlashMean){
				SetTextColor(hdcParaMemExt[5], ALARMFLASH_COLOR);
			}
			else{
				SetTextColor(hdcParaMemExt[5], gCfgIbp2.iColor);		
			}
			bFlashMean = !bFlashMean;
		}
		else{
			SetTextColor(hdcParaMemExt[5], gCfgIbp2.iColor);			
		}
		if(bDebugRect) Rectangle(hdcParaMemExt[5], gRCIbp2_Mean.left, gRCIbp2_Mean.top, gRCIbp2_Mean.right, gRCIbp2_Mean.bottom);
		//SelectFont(hdcParaMemExt[5], gFontEnglish_24);
#if SCREEN_1024
		SelectFont(hdcParaMemExt[5], gFontTTF_50);
#elif SCREEN_640
		SelectFont(hdcParaMemExt[5], gFontSmall);	
#else
		SelectFont(hdcParaMemExt[5], gFontEnglish_24);	

#endif

		memset(strInfo, 0, sizeof strInfo);
		GetStringFromResFile(gsLanguageRes, "GENERAL", "mean", strInfo, sizeof strInfo);		
		DrawText(hdcParaMemExt[5], strInfo, -1, &gRCIbp2_Mean, DT_NOCLIP | DT_TOP | DT_CENTER| DT_SINGLELINE);
		//SelectFont(hdcParaMemExt[5], gFontTTF_20);
#if SCREEN_1024
		SelectFont(hdcParaMemExt[5], gFontTTF_50);
#elif SCREEN_640
		SelectFont(hdcParaMemExt[5], gFontSystem);	
#else
		SelectFont(hdcParaMemExt[5], gFontTTF_20);	

#endif
		DrawText(hdcParaMemExt[5], strMean, -1, &gRCIbp2_Mean, DT_NOCLIP | DT_BOTTOM | DT_LEFT| DT_SINGLELINE);
		//TextOut(hdcParaMemExt[5], gRCIbp2_Mean.left-20, gRCIbp2_Mean.top+23, strMean);
	}
	else if(gCfgIbp2.bViewMode == IBPVIEW_M){
		InitRectIbp2();
		//显示间隔
		if(bDebugRect) Rectangle(hdcParaMemExt[5], gRCIbp2_Inter.left, gRCIbp2_Inter.top, gRCIbp2_Inter.right, gRCIbp2_Inter.bottom);
		SelectFont(hdcParaMemExt[5], gFontTTF_20);
		DrawText(hdcParaMemExt[5], "/", -1, &gRCIbp2_Inter, DT_NOCLIP | DT_BOTTOM | DT_RIGHT | DT_SINGLELINE);
	
		//显示数据 SYS
		if(gbAlarmIbp2_Sys && (gCfgIbp2.bAlmControl != ALARM_OFF)){
			if(bFlashSys){
				SetTextColor(hdcParaMemExt[5], ALARMFLASH_COLOR);
			}
			else{
				SetTextColor(hdcParaMemExt[5], gCfgIbp2.iColor);		
			}
			bFlashSys = !bFlashSys;
		}
		else{
			SetTextColor(hdcParaMemExt[5], gCfgIbp2.iColor);			
		}
		

		
		if(bDebugRect) Rectangle(hdcParaMemExt[5], gRCIbp2_Sys.left, gRCIbp2_Sys.top, gRCIbp2_Sys.right, gRCIbp2_Sys.bottom);
		//SelectFont(hdcParaMemExt[5], gFontEnglish_16);
#if SCREEN_1024
		SelectFont(hdcParaMemExt[5], gFontEnglish_24);
#elif SCREEN_640
		SelectFont(hdcParaMemExt[5], gFontSmall);	
#else
		SelectFont(hdcParaMemExt[5], gFontEnglish_16);	
#endif

		memset(strInfo, 0, sizeof strInfo);
		GetStringFromResFile(gsLanguageRes, "GENERAL", "sys", strInfo, sizeof strInfo);		
		DrawText(hdcParaMemExt[5], strInfo, -1, &gRCIbp2_Sys, DT_NOCLIP | DT_TOP | DT_RIGHT | DT_SINGLELINE);
		//SelectFont(hdcParaMemExt[5], gFontTTF_20);
#if SCREEN_1024
		SelectFont(hdcParaMemExt[5], gFontTTF_90Bk);
#elif SCREEN_640
		SelectFont(hdcParaMemExt[5], gFontEnglish_24);	
#else
		SelectFont(hdcParaMemExt[5], gFontTTF_20);	
#endif

		DrawText(hdcParaMemExt[5], strSys, -1, &gRCIbp2_Sys, DT_NOCLIP | DT_BOTTOM | DT_RIGHT | DT_SINGLELINE);
		//TextOut(hdcParaMemExt[5], gRCIbp2_Sys.left-50, gRCIbp2_Sys.top-30,strMean);
		//显示数据 Dia
		if(gbAlarmIbp2_Dia && (gCfgIbp2.bAlmControl != ALARM_OFF)){
			if(bFlashDia){
				SetTextColor(hdcParaMemExt[5], ALARMFLASH_COLOR);
			}
			else{
				SetTextColor(hdcParaMemExt[5], gCfgIbp2.iColor);		
			}
			bFlashDia = !bFlashDia;
		}
		else{
			SetTextColor(hdcParaMemExt[5], gCfgIbp2.iColor);			
		}
		
		if(bDebugRect) Rectangle(hdcParaMemExt[5], gRCIbp2_Dia.left, gRCIbp2_Dia.top, gRCIbp2_Dia.right, gRCIbp2_Dia.bottom);
		//SelectFont(hdcParaMemExt[5], gFontEnglish_16);
#if SCREEN_1024
		SelectFont(hdcParaMemExt[5], gFontEnglish_24);
#elif SCREEN_640
		SelectFont(hdcParaMemExt[5], gFontSmall);	
#else
		SelectFont(hdcParaMemExt[5], gFontEnglish_16);	
#endif

		memset(strInfo, 0, sizeof strInfo);
		GetStringFromResFile(gsLanguageRes, "GENERAL", "dia", strInfo, sizeof strInfo);		
		DrawText(hdcParaMemExt[5], strInfo, -1, &gRCIbp2_Dia, DT_NOCLIP | DT_TOP | DT_LEFT| DT_SINGLELINE);
		//SelectFont(hdcParaMemExt[5], gFontTTF_20);
#if SCREEN_1024
		SelectFont(hdcParaMemExt[5], gFontTTF_90Bk);
#elif SCREEN_640
		SelectFont(hdcParaMemExt[5], gFontEnglish_24);	
#else
		SelectFont(hdcParaMemExt[5], gFontTTF_20);	
#endif
		DrawText(hdcParaMemExt[5], strDia, -1, &gRCIbp2_Dia, DT_NOCLIP | DT_BOTTOM | DT_LEFT| DT_SINGLELINE);
		//TextOut(hdcParaMemExt[5], gRCIbp2_Dia.left-5, gRCIbp2_Dia.top-30,strMean);
		//显示数据 Mean
		if(gbAlarmIbp2_Mean && (gCfgIbp2.bAlmControl != ALARM_OFF)){
			if(bFlashMean){
				SetTextColor(hdcParaMemExt[5], ALARMFLASH_COLOR);
			}
			else{
				SetTextColor(hdcParaMemExt[5], gCfgIbp2.iColor);		
			}
			bFlashMean = !bFlashMean;
		}
		else{
			SetTextColor(hdcParaMemExt[5], gCfgIbp2.iColor);			
		}
		if(bDebugRect) Rectangle(hdcParaMemExt[5], gRCIbp2_Mean.left, gRCIbp2_Mean.top, gRCIbp2_Mean.right, gRCIbp2_Mean.bottom);
		SelectFont(hdcParaMemExt[5], gFontEnglish_16);
		memset(strInfo, 0, sizeof strInfo);
		GetStringFromResFile(gsLanguageRes, "GENERAL", "mean", strInfo, sizeof strInfo);		
		DrawText(hdcParaMemExt[5], strInfo, -1, &gRCIbp2_Mean, DT_NOCLIP | DT_TOP | DT_CENTER | DT_SINGLELINE);
#if SCREEN_1024
		SelectFont(hdcParaMemExt[5], gFontTTF_90Bk);
#elif SCREEN_640
		SelectFont(hdcParaMemExt[5], gFontTTF_30);	
#else
		SelectFont(hdcParaMemExt[5], gFontTTF_40Bk);	
#endif
		DrawText(hdcParaMemExt[5], strMean, -1, &gRCIbp2_Mean, DT_NOCLIP | DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
	}

	//Alarm Limit
	if(gCfgAlarm.bViewLimit){
		if(bDebugRect) Rectangle(hdcParaMemExt[5], gRCIbp2_limit_Sys.left, gRCIbp2_limit_Sys.top, gRCIbp2_limit_Sys.right, gRCIbp2_limit_Sys.bottom);
		if(bDebugRect) Rectangle(hdcParaMemExt[5], gRCIbp2_limit_Dia.left, gRCIbp2_limit_Dia.top, gRCIbp2_limit_Dia.right, gRCIbp2_limit_Dia.bottom);
		if(bDebugRect) Rectangle(hdcParaMemExt[5], gRCIbp2_limit_Mean.left, gRCIbp2_limit_Mean.top, gRCIbp2_limit_Mean.right, gRCIbp2_limit_Mean.bottom);
		SetTextColor(hdcParaMemExt[5], PARACOL_LIMIT);
		SelectFont(hdcParaMemExt[5], gFontSmall);

		memset(strLimitS_h, 0, sizeof strLimitS_h);
		memset(strLimitS_l, 0, sizeof strLimitS_l);
		memset(strLimitD_h, 0, sizeof strLimitD_h);
		memset(strLimitD_l, 0, sizeof strLimitD_l);
		memset(strLimitM_h, 0, sizeof strLimitM_h);
		memset(strLimitM_l, 0, sizeof strLimitM_l);
		switch(gCfgIbp2.bUnit){
			//case IBPUNIT_MMHG
			default	:{//mmHg
				snprintf(strLimitS_h, sizeof strLimitS_h, "%d", gCfgIbp2.iHigh_Sys);
				snprintf(strLimitS_l, sizeof strLimitS_l, "%d", gCfgIbp2.iLow_Sys);
				snprintf(strLimitD_h, sizeof strLimitD_h, "%d", gCfgIbp2.iHigh_Dia);
				snprintf(strLimitD_l, sizeof strLimitD_l, "%d", gCfgIbp2.iLow_Dia);
				snprintf(strLimitM_h, sizeof strLimitM_h, "%d", gCfgIbp2.iHigh_Mean);
				snprintf(strLimitM_l, sizeof strLimitM_l, "%d", gCfgIbp2.iLow_Mean);
			}break;
			case IBPUNIT_KPA:{//kPa
				snprintf(strLimitS_h, sizeof strLimitS_h, "%.1f", (gCfgIbp2.iHigh_Sys)/mmHgTokPa);
				snprintf(strLimitS_l, sizeof strLimitS_l, "%.1f", (gCfgIbp2.iLow_Sys)/mmHgTokPa);
				snprintf(strLimitD_h, sizeof strLimitD_h, "%.1f", (gCfgIbp2.iHigh_Dia)/mmHgTokPa);
				snprintf(strLimitD_l, sizeof strLimitD_l, "%.1f", (gCfgIbp2.iLow_Dia)/mmHgTokPa);
				snprintf(strLimitM_h, sizeof strLimitM_h, "%.1f", (gCfgIbp2.iHigh_Mean)/mmHgTokPa);
				snprintf(strLimitM_l, sizeof strLimitM_l, "%.1f", (gCfgIbp2.iLow_Mean)/mmHgTokPa);
			}break;
			/*case IBPUNIT_CMH2O:{
				snprintf(strLimitS_h, sizeof strLimitS_h, "%d", (int)((gCfgIbp2.iHigh_Sys)*1.36));
				snprintf(strLimitS_l, sizeof strLimitS_l, "%d", (int)((gCfgIbp2.iLow_Sys)*1.36));
				snprintf(strLimitD_h, sizeof strLimitD_h, "%d", (int)((gCfgIbp2.iHigh_Dia)*1.36));
				snprintf(strLimitD_l, sizeof strLimitD_l, "%d", (int)((gCfgIbp2.iLow_Dia)*1.36));
				snprintf(strLimitM_h, sizeof strLimitM_h, "%d", (int)((gCfgIbp2.iHigh_Mean)*1.36));
				snprintf(strLimitM_l, sizeof strLimitM_l, "%d", (int)((gCfgIbp2.iLow_Mean)*1.36));
			}break;*/
		}
		DrawText(hdcParaMemExt[5], strLimitS_h, -1, &gRCIbp2_limit_Sys, DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);
		DrawText(hdcParaMemExt[5], strLimitS_l, -1, &gRCIbp2_limit_Sys, DT_NOCLIP | DT_BOTTOM | DT_LEFT | DT_SINGLELINE);	
		DrawText(hdcParaMemExt[5], strLimitD_h, -1, &gRCIbp2_limit_Dia, DT_NOCLIP | DT_TOP | DT_RIGHT | DT_SINGLELINE);
		DrawText(hdcParaMemExt[5], strLimitD_l, -1, &gRCIbp2_limit_Dia, DT_NOCLIP | DT_BOTTOM | DT_RIGHT | DT_SINGLELINE);	
		if(gCfgIbp2.bViewMode == IBPVIEW_M){
			DrawText(hdcParaMemExt[5], strLimitM_h, -1, &gRCIbp2_limit_Mean, DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);
			DrawText(hdcParaMemExt[5], strLimitM_l, -1, &gRCIbp2_limit_Mean, DT_NOCLIP | DT_BOTTOM | DT_LEFT | DT_SINGLELINE);	 	
		}
		else{
			DrawText(hdcParaMemExt[5], strLimitM_h, -1, &gRCIbp2_limit_Mean, DT_NOCLIP | DT_TOP | DT_RIGHT | DT_SINGLELINE);
			DrawText(hdcParaMemExt[5], strLimitM_l, -1, &gRCIbp2_limit_Mean, DT_NOCLIP | DT_BOTTOM | DT_RIGHT | DT_SINGLELINE);	
		}
	}
	
	//Info
	if(bDebugRect) Rectangle(hdc, gRCIbp2_Info.left, gRCIbp2_Info.top, gRCIbp2_Info.right, gRCIbp2_Info.bottom);
	
	//绘制框架
	Rectangle(hdc, gRCIbp2.left, gRCIbp2.top, gRCIbp2.right, gRCIbp2.bottom);
	//Info
	if(bDebugRect) {
		Rectangle(hdcParaMemExt[5], gRCIbp2_Label.left, gRCIbp2_Label.top, gRCIbp2_Label.right, gRCIbp2_Label.bottom);
		//SetPenColor(hdcParaMemExt[4], COLOR_yellow);
		//Alarm Icon
		Rectangle(hdcParaMemExt[5], gRCIbp2_AlmIcon.left, gRCIbp2_AlmIcon.top, gRCIbp2_AlmIcon.right, gRCIbp2_AlmIcon.bottom);
		//Info
		Rectangle(hdcParaMemExt[5], gRCIbp2_Info.left, gRCIbp2_Info.top, gRCIbp2_Info.right, gRCIbp2_Info.bottom);
		//Data Sys
		Rectangle(hdcParaMemExt[5], gRCIbp2_Sys.left, gRCIbp2_Sys.top, gRCIbp2_Sys.right, gRCIbp2_Sys.bottom);
		//Inter
		Rectangle(hdcParaMemExt[5], gRCIbp2_Inter.left, gRCIbp2_Inter.top, gRCIbp2_Inter.right, gRCIbp2_Inter.bottom);
		//Data Dia
		Rectangle(hdcParaMemExt[5], gRCIbp2_Dia.left, gRCIbp2_Dia.top, gRCIbp2_Dia.right, gRCIbp2_Dia.bottom);
		//Data Mean
		Rectangle(hdcParaMemExt[5], gRCIbp2_Mean.left, gRCIbp2_Mean.top, gRCIbp2_Mean.right, gRCIbp2_Mean.bottom);
		//Limit
		Rectangle(hdcParaMemExt[5], gRCIbp2_limit_Sys.left, gRCIbp2_limit_Sys.top, gRCIbp2_limit_Sys.right, gRCIbp2_limit_Sys.bottom);
		Rectangle(hdcParaMemExt[5], gRCIbp2_limit_Dia.left, gRCIbp2_limit_Dia.top, gRCIbp2_limit_Dia.right, gRCIbp2_limit_Dia.bottom);
		Rectangle(hdcParaMemExt[5], gRCIbp2_limit_Mean.left, gRCIbp2_limit_Mean.top, gRCIbp2_limit_Mean.right, gRCIbp2_limit_Mean.bottom);

	}
	
		//拷贝DC
 	if(Draw_Mem){
 		BitBlt(hdc, 
 		gRCIbp2.left, gRCIbp2.top, RECTW(gRCIbp2)+1, RECTH(gRCIbp2)+1, 
 		hdcParaExt[5], 
 		gRCIbp2.left, gRCIbp2.top, 
 		0);
 	}
	return 0;
}


//根据模块配置以及屏幕类型初始化参数显示区域
static int InitRectTemp()
{
if(gCfgSystem.bInterface!=SCREEN_BIGCHAR)
	SetRect(&gRCTemp, 
			 gRcParas.left, 
			 gRcParas.top+iLeftParaRectHeight*4, 
			 gRcParas.right-1, 
			 gRcParas.bottom-1);
else
	SetRect(&gRCTemp, 
			 gRcParas.left, 
			 gRcParas.top+iLeftParaRectHeight*3-18, 
			 gRcParas.right-1, 
			 gRcParas.bottom-1);
	
	//相对坐标
	//Label
	SetRect(&gRCTemp_Label, gRCTemp.left+1,gRCTemp.top+1,gRCTemp.left+70,gRCTemp.top+17);
	//Unit
	SetRect(&gRCTemp_Unit, gRCTemp.left+1,gRCTemp.top+23,gRCTemp.left+40,gRCTemp.top+35);
	//Limit
	SetRect(&gRCTemp_Limit_T1, gRCTemp.left+1,gRCTemp.top+35,gRCTemp.left+30,gRCTemp.top+60);
	SetRect(&gRCTemp_Limit_T2, gRCTemp.left+1,gRCTemp.bottom-30,gRCTemp.left+30,gRCTemp.bottom-5);
	SetRect(&gRCTemp_Limit_TD, gRCTemp.right-30,gRCTemp.top+30,gRCTemp.right,gRCTemp.top+40);
 	//Data
#if SCREEN_1024
	SetRect(&gRCTemp_T1, gRCTemp.left+90-giLeftDec,gRCTemp.top+25,gRCTemp.left+180-giLeftDec,gRCTemp.top+70);
	SetRect(&gRCTemp_T2, gRCTemp.left+90-giLeftDec,gRCTemp.top+70,gRCTemp.left+180-giLeftDec,gRCTemp.top+120);
#elif SCREEN_640
 	SetRect(&gRCTemp_T1, gRCTemp.left+30,gRCTemp.top+17,gRCTemp.right-60,gRCTemp.top+RECTH(gRCTemp)/2+10);
 	SetRect(&gRCTemp_T2, gRCTemp.left+30,gRCTemp.top+RECTH(gRCTemp)/2+10,gRCTemp.right-60,gRCTemp.bottom);
#else
 	SetRect(&gRCTemp_T1, gRCTemp.left+30,gRCTemp.top+17,gRCTemp.right-80,gRCTemp.top+RECTH(gRCTemp)/2+10);
 	SetRect(&gRCTemp_T2, gRCTemp.left+30,gRCTemp.top+RECTH(gRCTemp)/2+10,gRCTemp.right-80,gRCTemp.bottom);
#endif
	//TD
// 	SetRect(&gRCTemp_TD, gRCTemp.left+195-giLeftDec,gRCTemp.top+23,gRCTemp.left+240-giLeftDec,gRCTemp.top+75);
	SetRect(&gRCTemp_TD, gRCTemp.right-80,gRCTemp.top+40,gRCTemp.right,gRCTemp.bottom-5);
	//Alarm Icon
// 	SetRect(&gRCTemp_AlmIcon, gRCTemp.left+180-giLeftDec,gRCTemp.bottom-25,gRCTemp.left+210-giLeftDec,gRCTemp.bottom);
	SetRect(&gRCTemp_AlmIcon, gRCTemp.right-20,gRCTemp.top+1,gRCTemp.right, gRCTemp.top+20);
	
	
	return 0;
}

//显示Temp参数
static int ViewTemp(HDC hdc, BYTE bInterface, BOOL bMemCopy)
{
	unsigned char strLabel[10]={0};
// 	unsigned char strUnit[10]={0};
	unsigned char strUnit[8]={0};
	double fT1, fT2, fTD, fValue;
	int iValue;
	unsigned char strT1[6]={0}, strT2[6]={0}, strTD[6]={0};	
	unsigned char strStatus[20]={0};
	static BOOL bFlashT1 = FALSE;
	static BOOL bFlashT2 = FALSE;
	static BOOL bFlashTD = FALSE;
	unsigned char strLimit_h[6]={0};
	unsigned char strLimit_l[6]={0};
//	printf(".wT1=%d,wT2=%d\n",gValueTemp.wT1,gValueTemp.wT2);
	if(gCfgTemp.bUnit == TEMP_UNIT_C){
		if((!gValueTemp.bOff_T1)&&(gValueTemp.wT1!=0)){
			snprintf(strT1, sizeof strT1, "%.1f", (double)((double)gValueTemp.wT1 /10.0));
		}
		else{
			snprintf(strT1, sizeof strT1, "--.-");
		}
		
		if((!gValueTemp.bOff_T2)&&(gValueTemp.wT2!=0)){
			snprintf(strT2, sizeof strT2, "%.1f", (double)((double)gValueTemp.wT2 /10.0));
		}
		else{
			snprintf(strT2, sizeof strT2, "--.-");
		}
		
		if((!gValueTemp.bOff_T1) && (!gValueTemp.bOff_T2)
			&&(gValueTemp.wT1!=0)&&(gValueTemp.wT2!=0)){
			iValue = abs(gValueTemp.wT2 - gValueTemp.wT1);
			snprintf(strTD, sizeof strTD, "%.1f", (double)((double)iValue/10.0));
		}
		else{
			snprintf(strTD, sizeof strTD, "-.-");
		}	
	}
	else{
		if((!gValueTemp.bOff_T1)&&(gValueTemp.wT1!=0)){
			fT1 = (double)gValueTemp.wT1 / 10.0 * 1.8 +32.0;
			snprintf(strT1, sizeof strT1, "%.1f", fT1);
		}
		else{
			snprintf(strT1, sizeof strT1, "--.-");
		}
	
		if((!gValueTemp.bOff_T2)&&(gValueTemp.wT2!=0)){
			fT2 = (double)gValueTemp.wT2 / 10.0 *1.8 +32.0;
			snprintf(strT2, sizeof strT2, "%.1f", fT2);
		}
		else{
			snprintf(strT2, sizeof strT2, "--.-");
		}
	
	if((!gValueTemp.bOff_T1) && (!gValueTemp.bOff_T2)
			&&(gValueTemp.wT1!=0)&&(gValueTemp.wT2!=0)){
			fTD = fabs(fT2 - fT1);
			snprintf(strTD, sizeof strTD, "%.1f", fTD);
		}
		else{
			snprintf(strTD, sizeof strTD, "-.-");
		}	
	}
	
	//设置DC属性
	SetTextColor(hdc, gCfgTemp.iColor);	
		
	//显示 Label
//	if(bDebugRect) Rectangle(hdc, gRCTemp_Label.left, gRCTemp_Label.top, gRCTemp_Label.right, gRCTemp_Label.bottom);
	SelectFont(hdc, gFontEnglish_16);
	memset(strLabel, 0, sizeof strLabel);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "temp", strLabel, sizeof strLabel);	
	memset(strUnit, 0, sizeof strUnit);
 	if(gCfgTemp.bUnit == TEMP_UNIT_C)
  		GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "unit_c", strUnit, sizeof strUnit);
 	else
 		GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "unit_f", strUnit, sizeof strUnit);
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "TEMP (%s)", strUnit);
//	DrawText(hdc, strUnit, -1, &gRCTemp_Unit, DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE);

	DrawText(hdc, strInfo, -1, &gRCTemp_Label, DT_NOCLIP | DT_VCENTER | DT_LEFT | DT_SINGLELINE);
	SelectFont(hdc, gFontEnglish_16);
	DrawText(hdc, "T1", -1, &gRCTemp_T1, DT_NOCLIP | DT_TOP | DT_LEFT| DT_SINGLELINE);	
	DrawText(hdc, "T2", -1, &gRCTemp_T2, DT_NOCLIP | DT_TOP | DT_LEFT| DT_SINGLELINE);	
	if(gCfgSystem.bLanguage == LANGUAGE_SPANISH)
		DrawText(hdc, "DT", -1, &gRCTemp_TD, DT_NOCLIP | DT_TOP | DT_LEFT| DT_SINGLELINE);	
	else
		DrawText(hdc, "TD", -1, &gRCTemp_TD, DT_NOCLIP | DT_TOP | DT_CENTER| DT_SINGLELINE);	
	

// 	//Unit
// 	if(bDebugRect) Rectangle(hdc, gRCTemp_Unit.left, gRCTemp_Unit.top, gRCTemp_Unit.right, gRCTemp_Unit.bottom);
// 	SelectFont(hdc, gFontEnglish_16);

// 	DrawText(hdc, strInfo, -1, &gRCTemp_Unit, DT_NOCLIP | DT_BOTTOM | DT_RIGHT | DT_SINGLELINE);
// 	
	//Alarm Icon
//	if(bDebugRect) Rectangle(hdc, gRCTemp_AlmIcon.left, gRCTemp_AlmIcon.top, gRCTemp_AlmIcon.right, gRCTemp_AlmIcon.bottom);
	if(gCfgTemp.bAlmControl == ALARM_OFF){
		FillBoxWithBitmap(hdc, gRCTemp_AlmIcon.left+1,gRCTemp_AlmIcon.top+1, 16, 16, &gIconAlarmOff);
	}
		
	//显示T1
	if(gbAlarmTemp_T1 && (gCfgTemp.bAlmControl != ALARM_OFF)){
		if(bFlashT1){
			SetTextColor(hdc, ALARMFLASH_COLOR);
		}
		else{
			SetTextColor(hdc, gCfgTemp.iColor);		
		}
		bFlashT1 = !bFlashT1;
	}
	else{
		SetTextColor(hdc, gCfgTemp.iColor);			
	}
#if SCREEN_640
	if(gCfgSystem.bInterface==SCREEN_BIGCHAR)
		SelectFont(hdc, gFontTTF_40Bk);
	else
		SelectFont(hdc, gFontTTF_30Bk);
#else
	if(gCfgSystem.bInterface==SCREEN_BIGCHAR)
		SelectFont(hdc, gFontTTF_40Bk);
	else
		SelectFont(hdc, gFontTTF_35Bk);
#endif
 	DrawText(hdc, strT1, -1, &gRCTemp_T1, DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE);

	//Alarm Limit
	if(gCfgAlarm.bViewLimit){
		SetTextColor(hdc, PARACOL_LIMIT);
		SelectFont(hdc, gFontSmall);
		memset(strLimit_h, 0, sizeof strLimit_h);
		memset(strLimit_l, 0, sizeof strLimit_l);
		switch(gCfgTemp.bUnit){
			case TEMP_UNIT_C:{
				snprintf(strLimit_h, sizeof strLimit_h, "%.1f", (gCfgTemp.wHigh_T1)/10.0);
				snprintf(strLimit_l, sizeof strLimit_l, "%.1f", (gCfgTemp.wLow_T1)/10.0);
			}break;
			case TEMP_UNIT_F:{
				snprintf(strLimit_h, sizeof strLimit_h, "%.1f", (float)((gCfgTemp.wHigh_T1)/10.0*1.8+32.0));
				snprintf(strLimit_l, sizeof strLimit_l, "%.1f",(float)((gCfgTemp.wLow_T1)/10.0*1.8+32.0));
			}break;
		}
		DrawText(hdc, strLimit_h, -1, &gRCTemp_Limit_T1, DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);
		DrawText(hdc, strLimit_l, -1, &gRCTemp_Limit_T1, DT_NOCLIP | DT_BOTTOM | DT_LEFT | DT_SINGLELINE);	
	}
	
	
	//显示T2
	if(gbAlarmTemp_T2 && (gCfgTemp.bAlmControl != ALARM_OFF)){
		if(bFlashT2){
			SetTextColor(hdc, ALARMFLASH_COLOR);
		}
		else{
			SetTextColor(hdc, gCfgTemp.iColor);		
		}
		bFlashT2 = !bFlashT2;
	}
	else{
		SetTextColor(hdc, gCfgTemp.iColor);			
	}
#if SCREEN_640
	if(gCfgSystem.bInterface==SCREEN_BIGCHAR)
		SelectFont(hdc, gFontTTF_40Bk);
	else
		SelectFont(hdc, gFontTTF_30Bk);
#else
	if(gCfgSystem.bInterface==SCREEN_BIGCHAR)
		SelectFont(hdc, gFontTTF_40Bk);
	else
		SelectFont(hdc, gFontTTF_35Bk);
#endif
	
 	DrawText(hdc, strT2, -1, &gRCTemp_T2, DT_NOCLIP | DT_VCENTER | DT_CENTER  | DT_SINGLELINE);

	//Alarm Limit
	if(gCfgAlarm.bViewLimit){
		SetTextColor(hdc, PARACOL_LIMIT);
		SelectFont(hdc, gFontSmall);
		memset(strLimit_h, 0, sizeof strLimit_h);
		memset(strLimit_l, 0, sizeof strLimit_l);
		switch(gCfgTemp.bUnit){
			case TEMP_UNIT_C:{
				snprintf(strLimit_h, sizeof strLimit_h, "%.1f", (gCfgTemp.wHigh_T2)/10.0);
				snprintf(strLimit_l, sizeof strLimit_l, "%.1f", (gCfgTemp.wLow_T2)/10.0);
			}break;
			case TEMP_UNIT_F:{
				snprintf(strLimit_h, sizeof strLimit_h, "%.1f", (float)((gCfgTemp.wHigh_T2)/10.0*1.8+32.0));
				snprintf(strLimit_l, sizeof strLimit_l, "%.1f",(float)((gCfgTemp.wLow_T2)/10.0*1.8+32.0));
			}break;
		}
		DrawText(hdc, strLimit_h, -1, &gRCTemp_Limit_T2, DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);
		DrawText(hdc, strLimit_l, -1, &gRCTemp_Limit_T2, DT_NOCLIP | DT_BOTTOM | DT_LEFT | DT_SINGLELINE);	
	}
	
	//显示TD
	if(gbAlarmTemp_TD && (gCfgTemp.bAlmControl != ALARM_OFF)){
		if(bFlashTD){
			SetTextColor(hdc, ALARMFLASH_COLOR);
		}
		else{
			SetTextColor(hdc, gCfgTemp.iColor);		
		}
		bFlashTD = !bFlashTD;
	}
	else{
		SetTextColor(hdc, gCfgTemp.iColor);			
	}
#if SCREEN_640
	if(gCfgSystem.bInterface==SCREEN_BIGCHAR)
		SelectFont(hdc, gFontTTF_30Bk);
	else
		SelectFont(hdc, gFontTTF_20Bk);
#else
	if(gCfgSystem.bInterface==SCREEN_BIGCHAR)
		SelectFont(hdc, gFontTTF_30Bk);
	else
		SelectFont(hdc, gFontTTF_30Bk);
#endif
	
	DrawText(hdc, strTD, -1, &gRCTemp_TD, DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE);

	//Alarm Limit
//	if(bDebugRect) Rectangle(hdc, gRCTemp_Limit_TD.left, gRCTemp_Limit_TD.top, gRCTemp_Limit_TD.right, gRCTemp_Limit_TD.bottom);
	if(gCfgAlarm.bViewLimit){
		SetTextColor(hdc, PARACOL_LIMIT);
		SelectFont(hdc, gFontSmall);
		memset(strLimit_h, 0, sizeof strLimit_h);
		switch(gCfgTemp.bUnit){
			case TEMP_UNIT_C:{
				snprintf(strLimit_h, sizeof strLimit_h, "%.1f", (gCfgTemp.wHigh_TD)/10.0);
			}break;
			case TEMP_UNIT_F:{
				snprintf(strLimit_h, sizeof strLimit_h, "%.1f", (float)((gCfgTemp.wHigh_TD)/10.0*1.8+32.0));
			}break;
		}
		DrawText(hdc, strLimit_h, -1, &gRCTemp_Limit_TD, DT_NOCLIP | DT_VCENTER | DT_RIGHT | DT_SINGLELINE);
	}
	
	//绘制框架
	Rectangle(hdc, gRCTemp.left, gRCTemp.top, gRCTemp.right+5, gRCTemp.bottom+5);
	if(bDebugRect) {

		//Label
		Rectangle(hdc, gRCTemp_Label.left, gRCTemp_Label.top, gRCTemp_Label.right, gRCTemp_Label.bottom);

		//Limit
			Rectangle(hdc, gRCTemp_Limit_T1.left, gRCTemp_Limit_T1.top, gRCTemp_Limit_T1.right, gRCTemp_Limit_T1.bottom);

			Rectangle(hdc, gRCTemp_Limit_T2.left, gRCTemp_Limit_T2.top, gRCTemp_Limit_T2.right, gRCTemp_Limit_T2.bottom);

			Rectangle(hdc, gRCTemp_Limit_TD.left, gRCTemp_Limit_TD.top, gRCTemp_Limit_TD.right, gRCTemp_Limit_TD.bottom);

		//Alarm Icon
			Rectangle(hdc, gRCTemp_AlmIcon.left, gRCTemp_AlmIcon.top, gRCTemp_AlmIcon.right, gRCTemp_AlmIcon.bottom);

		//T1
			Rectangle(hdc, gRCTemp_T1.left, gRCTemp_T1.top, gRCTemp_T1.right, gRCTemp_T1.bottom);

		//T2
			Rectangle(hdc, gRCTemp_T2.left, gRCTemp_T2.top, gRCTemp_T2.right, gRCTemp_T2.bottom);

		//TD
			Rectangle(hdc, gRCTemp_TD.left, gRCTemp_TD.top, gRCTemp_TD.right, gRCTemp_TD.bottom);

		//Unit
	//		Rectangle(hdc, gRCTemp_Unit.left, gRCTemp_Unit.top, gRCTemp_Unit.right, gRCTemp_Unit.bottom);

		}

 	//拷贝DC
 	if(Draw_Mem){
 		BitBlt(hdcParaMem, 
 		gRCTemp.left, gRCTemp.top, RECTW(gRCTemp)+1, RECTH(gRCTemp)+1, 
 		hdcPara, 
 		gRCTemp.left, gRCTemp.top, 
 		0);
 	}

		
	return 0;
}

//初始化系统状态区域 0
static int InitRectSys0()
{

	//Patient Info
	SetRect(&gRCSysInfo, 
		 gRcParasExt[1].left, 
		 gRcParasExt[1].top+7, 
		 gRcParasExt[1].left+120, 
		 gRcParasExt[1].bottom-1);
	
	//Time
	SetRect(&gRCTime, 
		 gRcParasExt[1].left+120, 
		 gRcParasExt[1].top, 
		 gRcParasExt[1].right-1, 
		 gRcParasExt[1].bottom-1);
	

	return 0;
}


/*
	显示系统状态0 patient info & time
*/
static int ViewSysInfo0_Time(HDC hdc)
{
	S_TIME 	sTime;
	unsigned char strTime[10]={0};	
	unsigned char strDate[20]={0};	
	static int iCount = 0;
 	SetTextColor(hdc, PARACOL_TEXT);
	SelectFont(hdc, gFontTTF_20Bk);

	//Time 
	GetSysTime(&sTime);
	if(bDebugRect) Rectangle(hdc, gRCTime.left, gRCTime.top, gRCTime.right, gRCTime.bottom);
	snprintf(strTime, sizeof strTime, "%.2d:%.2d:%.2d", sTime.bHour, sTime.bMin, sTime.bSec);
	DrawText(hdc, strTime, -1, &gRCTime, DT_NOCLIP | DT_SINGLELINE | DT_TOP | DT_CENTER);

	SelectFont(hdc, gFontSmall);
	snprintf(strDate, sizeof strDate, "%.4d-%.2d-%.2d", sTime.wYear, sTime.bMonth, sTime.bDay);
	DrawText(hdc, strDate, -1, &gRCTime, DT_NOCLIP | DT_SINGLELINE | DT_BOTTOM | DT_CENTER);

	if(Draw_Mem){
	BitBlt(hdcParaMemExt[1], 
			       gRCTime.left, gRCTime.top, RECTW(gRCTime)+1, RECTH(gRCTime)+1, 
			       hdcParaExt[1], 
			       gRCTime.left, gRCTime.top, 
			       0);
	}
	return 0;
}

/*
	显示系统状态0 patient info & time
*/
static int ViewSysInfo0_Icon(HDC hdc)
{
	S_TIME 	sTime;
	unsigned char strTime[10]={0};	
	unsigned char strDate[20]={0};	
	static int iCount = 0;
 	SetTextColor(hdc, PARACOL_TEXT);
	SelectFont(hdc, gFontTTF_20Bk);


	//Power
	if(gPowerStatus.bPowerType == POWER_AC){
		FillBoxWithBitmap(hdc, gRCSysInfo.left+1,gRCSysInfo.top, 30, 30, &gIconAC);
	}else{
		FillBoxWithBitmap(hdc, gRCSysInfo.left+1,gRCSysInfo.top, 30, 30, &gIconACOff);
	}
	
	//Battery
	//交流电的情况下,如果电池未满,动态显示充电图标
	if(gPowerStatus.bPowerType == POWER_AC 
		&& gPowerStatus.bCharge!=POWER_NOT_CHARGE
			&&gPowerStatus.bHaveBat != POWER_NOBAT){
		if(iCount >BATTERY_FULL) iCount = BATTERY_EMPTY;	
		FillBoxWithBitmap(hdc, gRCSysInfo.left+38,gRCSysInfo.top, 18, 32, &(gIconBatteryAC[iCount]));
		iCount ++;	
	}
	else{
		iCount = gPowerStatus.bBatteryStatus;
// 		printf("Battery : %d.\n", iCount);
		FillBoxWithBitmap(hdc, gRCSysInfo.left+38,gRCSysInfo.top, 18, 32, &(gIconBattery[iCount]));
	}
		
	//Printer Info
	if(gPrinterStatus == PRNSTATUS_ERR || gPrinterStatus == PRNSTATUS_NOPAPER)
		FillBoxWithBitmap(hdc, gRCSysInfo.left+60,gRCSysInfo.top, 30, 30, &gIconPrinterError);
	else if(gPrinterStatus == PRNSTATUS_PRINTING)
		FillBoxWithBitmap(hdc, gRCSysInfo.left+60,gRCSysInfo.top, 30, 30, &gIconPrinting);
	else
		FillBoxWithBitmap(hdc, gRCSysInfo.left+60,gRCSysInfo.top, 30, 30, &gIconPrinter);
	
	//NetStatus 
// 	printf("NetStatus %d \n", gbCentralStatus);
	if(gbCentralStatus)
		FillBoxWithBitmap(hdc, gRCSysInfo.left+90,gRCSysInfo.top, 30, 30, &gIconNetStatus[1]);
	else
		FillBoxWithBitmap(hdc, gRCSysInfo.left+90,gRCSysInfo.top, 30, 30, &gIconNetStatus[0]);
			
	
/*
	//Time 
	GetSysTime(&sTime);
	if(bDebugRect) Rectangle(hdc, gRCTime.left, gRCTime.top, gRCTime.right, gRCTime.bottom);
	snprintf(strTime, sizeof strTime, "%.2d:%.2d:%.2d", sTime.bHour, sTime.bMin, sTime.bSec);
	DrawText(hdc, strTime, -1, &gRCTime, DT_NOCLIP | DT_SINGLELINE | DT_TOP | DT_CENTER);

	SelectFont(hdc, gFontSmall);
	snprintf(strDate, sizeof strDate, "%.4d-%.2d-%.2d", sTime.wYear, sTime.bMonth, sTime.bDay);
	DrawText(hdc, strDate, -1, &gRCTime, DT_NOCLIP | DT_SINGLELINE | DT_BOTTOM | DT_CENTER);
*/
	if(Draw_Mem){
	BitBlt(hdcParaMemExt[1], 
			       gRCSysInfo.left, gRCSysInfo.top, RECTW(gRCSysInfo)+1, RECTH(gRCSysInfo)+1, 
			       hdcParaExt[1], 
			       gRCSysInfo.left, gRCSysInfo.top, 
			       0);
	}
	return 0;
}


//初始化系统状态区域 1
static int InitRectSys1()
{
/*
	//Icon
	SetRect(&gRCSysInfo, 
		 gRcParasExt[2].left, 
		 gRcParasExt[2].top, 
		 gRcParasExt[2].left+120, 
		 gRcParasExt[2].bottom-1);
*/	
	//Other
	SetRect(&gRCOtherInfo, 
		 gRcParasExt[2].left, 
		 gRcParasExt[2].top, 
		 gRcParasExt[2].right-1, 
		 gRcParasExt[2].bottom-1);
	
	return 0;
}
	
/*
	显示系统状态0 System & other info(DEMO)
*/
static int ViewSysInfo1(HDC hdc)
{

	char strInfo[20]={0};
	static BYTE bFlash=0;
 //	if(bDebugRect) 
//		Rectangle(hdc, gRCSysInfo.left, gRCSysInfo.top, gRCSysInfo.right, gRCSysInfo.bottom);
 	if(bDebugRect) 
		Rectangle(hdc, gRCOtherInfo.left, gRCOtherInfo.top, gRCOtherInfo.right, gRCOtherInfo.bottom);
	
	//Audio Info
	
	if(gbSuspending){
		FillBoxWithBitmap(hdc, gRCOtherInfo.left+5,gRCOtherInfo.top+2, 30, 30, &gIconAlmAudioPause);
		SelectFont(hdc, gFontTTF_20Bk);
		memset(strInfo, 0, sizeof strInfo);
		snprintf(strInfo, sizeof strInfo, "%d:%d", (int)(giSuspendTime/60),giSuspendTime%60);
		DrawText(hdc, strInfo, -1, &gRCOtherInfo, DT_NOCLIP | DT_SINGLELINE| DT_RIGHT | DT_VCENTER);
	}else if(gbSilence){
		FillBoxWithBitmap(hdc, gRCOtherInfo.left+5,gRCOtherInfo.top+2, 30, 30, &gIconAudioOff);
	}else{
		//Bed NO.
		unsigned char strBed[20]={0};
		SetTextColor(hdc, COLOR_lightgray);
		SelectFont(hdc, gFontSystem);
		if(gCfgSystem.bLanguage == LANGUAGE_SPANISH)
			snprintf(strBed, sizeof strBed, "CAMA:%d", gCfgNet.wBedNO);
		else
			snprintf(strBed, sizeof strBed, "Bed:%d", gCfgNet.wBedNO);
		DrawText(hdc, strBed, -1, &gRCOtherInfo, DT_NOCLIP | DT_VCENTER | DT_LEFT | DT_SINGLELINE);


		//Object

		if(gbMonitorObject==0){
			memset(strInfo, 0, sizeof strInfo);
			if(gCfgPatient.bObject == ADULT)
				GetStringFromResFile(gsLanguageRes, "GENERAL", "adult", strInfo, sizeof strInfo);
			else if(gCfgPatient.bObject == CHILD)
				GetStringFromResFile(gsLanguageRes, "GENERAL", "child", strInfo, sizeof strInfo);
			else GetStringFromResFile(gsLanguageRes, "GENERAL", "baby", strInfo, sizeof strInfo);
		}
		else{
			memset(strInfo, 0, sizeof strInfo);
			if(gCfgPatient.bObject == ADULT)
				GetStringFromResFile(gsLanguageRes, "GENERAL", "big_animal", strInfo, sizeof strInfo);
			else if(gCfgPatient.bObject == CHILD)
				GetStringFromResFile(gsLanguageRes, "GENERAL", "middle_animal", strInfo, sizeof strInfo);
			else GetStringFromResFile(gsLanguageRes, "GENERAL", "small_animal", strInfo, sizeof strInfo);
		}
	

		SelectFont(hdc, gFontEnglish_16);
		DrawText(hdc, strInfo, -1, &gRCOtherInfo, DT_NOCLIP | DT_VCENTER | DT_RIGHT| DT_SINGLELINE);
	}
	
 	if(gbViewDemoData){
 		SetTextColor(hdc, COLOR_red);
 		SelectFont(hdc, gFontTTF_20Bk);
		if(bFlash<3)
 		DrawText(hdc, "DEMO", -1, &gRCOtherInfo, DT_NOCLIP | DT_BOTTOM | DT_RIGHT | DT_SINGLELINE);
 		//恢复字体
 		SelectFont(hdc, gFontSystem);
		bFlash=(bFlash+1)%5;
 	}
			
	
if(Draw_Mem){
	BitBlt(hdcParaMemExt[2], 
			       gRcParasExt[2].left, gRcParasExt[2].top, RECTW(gRcParasExt[2])+1, RECTH(gRcParasExt[2])+1, 
			       hdcParaExt[2], 
			       gRcParasExt[2].left, gRcParasExt[2].top, 
			       0);
	}
 	
	/*
	unsigned char *gStrPrinterStatus[]={"打印机故障!", "无打印任务.", "正在打印 ... ", "打印机缺纸!"};
	if(gPrinterStatus >=PRNSTATUS_ERR && gPrinterStatus <=PRNSTATUS_NOPAPER){
		SetTextColor(hdc, COLOR_lightgray);
		SelectFont(hdc, gFontSystem);
		DrawText(hdc, gStrPrinterStatus[gPrinterStatus], -1, &gRCOtherInfo, DT_NOCLIP | DT_VCENTER | DT_LEFT | DT_SINGLELINE);	
	}
	           */
	
	return 0;
}

int NewViewParas_bak()
{
	int i;
	static int para_count;
	int iHrState =0;	//状态 0：开始状态 1：图标显示状态 2：图标关闭状态
	int iHrCount =0;
	
// 	HDC hdcTmp;
 	BOOL bMemCopy;	//是否进行内存拷贝
// 	HDC hdcTmpExt[EXT_PARASPANEL_COUNT];
 	BOOL bMemCopyExt[EXT_PARASPANEL_COUNT];	
	
	struct timeval start_tv;
	struct timeval end_tv;
	
//	gettimeofday(&start_tv, NULL);
	
	gbParasViewing  = TRUE;
	
	if(!gbCanViewParas){
		printf("----------------------------------------------------------------Cann't view paras.\n");
		return -1;
	} 
	
     	FillBox(hdcParaMem, gRcParas.left, gRcParas.top, RECTW(gRcParas), RECTH(gRcParas));
	for(i=0; i<EXT_PARASPANEL_COUNT; i++){
		if(hdcParaMemExt[i] != (HDC)NULL){
			FillBox(hdcParaMemExt[i], gRcParasExt[i].left, gRcParasExt[i].top, RECTW(gRcParasExt[i]), RECTH(gRcParasExt[i]));
		}
	}
	
if(para_count>3||Show_Para){
	para_count = 0;
	Show_Para=FALSE;
	
	//显示参数
 	ViewEcg(hdcParaMem, gCfgSystem.bInterface, bMemCopy);
 	ViewSpO2(hdcParaMem, gCfgSystem.bInterface, bMemCopy);
	if(gbHaveIbp){
 		ViewIbp1(hdcParaMemExt[4], gCfgSystem.bInterface, bMemCopy);
		ViewIbp2(hdcParaMemExt[5], gCfgSystem.bInterface, bMemCopy);
	}

 	if(!gbHaveCo2){
 		ViewResp(hdcParaMem, gCfgSystem.bInterface, bMemCopy);
 	}
 	else{
  		ViewCo2(hdcParaMem, gCfgSystem.bInterface, bMemCopy);
// 		ViewGas(hdcParaMem, gCfgSystem.bInterface, bMemCopy);
 	}

	 ViewTemp(hdcParaMem, gCfgSystem.bInterface, bMemCopy);	
	
	//NIBP 在扩展面板3
	if(gCfgSystem.bInterface!=SCREEN_BIGCHAR){
		ViewNibp(hdcParaMem, gCfgSystem.bInterface, bMemCopy);
	}else{
	 	ViewNibp(hdcParaMemExt[3], gCfgSystem.bInterface, bMemCopy);
	}

	//NIBPList 在扩展面板0	
	if(gCfgSystem.bInterface==SCREEN_NIBPLIST)
		ViewNibpList(hdcParaMemExt[0], gCfgSystem.bInterface, bMemCopy);
	
	ViewSysInfo0_Icon(hdcParaMemExt[1]);


}

	para_count  = (para_count+1)%20;
	//扩展面板1 System info 0 time
	ViewSysInfo0_Time(hdcParaMemExt[1]);
	//扩展面板2 System info 1
	ViewSysInfo1(hdcParaMemExt[2]);
	gbParasViewing  = FALSE;
	
	//gettimeofday(&end_tv, NULL);
	
// 	if(abs(start_tv.tv_usec-end_tv.tv_usec)>200000)
// 		printf("%s:%d (usec)Start time: %ld End time:%ld, inter:%ld\n", __FILE__, __LINE__, 
// 		       start_tv.tv_usec, end_tv.tv_usec, abs(start_tv.tv_usec-end_tv.tv_usec));
	
	return 0;
}


int NewViewParas()
{
	int i;
	static int para_count;
	int iHrState =0;	//状态 0：开始状态 1：图标显示状态 2：图标关闭状态
	int iHrCount =0;
	
// 	HDC hdcTmp;
 	BOOL bMemCopy;	//是否进行内存拷贝
// 	HDC hdcTmpExt[EXT_PARASPANEL_COUNT];
 	BOOL bMemCopyExt[EXT_PARASPANEL_COUNT];	
	
	struct timeval start_tv;
	struct timeval end_tv;
	
	gettimeofday(&start_tv, NULL);
	
	gbParasViewing  = TRUE;
	
	if(!gbCanViewParas){
		printf("----------------------------------------------------------------Cann't view paras.\n");
		return -1;
	} 
		
     	FillBox(hdcParaMem, gRcParas.left, gRcParas.top, RECTW(gRcParas), RECTH(gRcParas));
	for(i=0; i<EXT_PARASPANEL_COUNT; i++){
		if(hdcParaMemExt[i] != (HDC)NULL){
			FillBox(hdcParaMemExt[i], gRcParasExt[i].left, gRcParasExt[i].top, RECTW(gRcParasExt[i]), RECTH(gRcParasExt[i]));
		}
	}
	
switch(para_count){
	case 0:
		//显示参数
	 	ViewEcg(hdcParaMem, gCfgSystem.bInterface, bMemCopy);
	break;
	case 1:
	 	ViewSpO2(hdcParaMem, gCfgSystem.bInterface, bMemCopy);
	break;
	case 2:
 		ViewResp(hdcParaMem, gCfgSystem.bInterface, bMemCopy);
	break;
	case 3: 
	 	ViewTemp(hdcParaMem, gCfgSystem.bInterface, bMemCopy);	
	break;
	case 4:
		//NIBP 在扩展面板3
		if(gCfgSystem.bInterface!=SCREEN_BIGCHAR){
			ViewNibp(hdcParaMem, gCfgSystem.bInterface, bMemCopy);
		}else{
		 	ViewNibp(hdcParaMemExt[3], gCfgSystem.bInterface, bMemCopy);
		}
		//NIBPList 在扩展面板0	
		if(gCfgSystem.bInterface==SCREEN_NIBPLIST)
			ViewNibpList(hdcParaMemExt[0], gCfgSystem.bInterface, bMemCopy);
	break;
	case 5:
		//扩展面板2 System info 1
		ViewSysInfo1(hdcParaMemExt[2]);
	break;
	default:
		
	break;
}

para_count = (para_count+1)%6;
//扩展面板1 System info 0
//ViewSysInfo0(hdcParaMemExt[1]);

#if 0	
	//拷贝DC

// 	if(bMemCopy){
		BitBlt(hdcParaMem, 
		       gRcParas.left, gRcParas.top, RECTW(gRcParas)+1, RECTH(gRcParas)+1, 
		       hdcPara, 
		       gRcParas.left, gRcParas.top, 
		       0);
// 	}
		
	if(gCfgSystem.bInterface==SCREEN_NIBPLIST)
		BitBlt(hdcParaMemExt[0], 
			       gRcParasExt[0].left, gRcParasExt[0].top, RECTW(gRcParasExt[0])+1, RECTH(gRcParasExt[0])+1, 
			       hdcParaExt[0], 
			       gRcParasExt[0].left, gRcParasExt[0].top, 
			       0);
	if(gCfgSystem.bInterface==SCREEN_BIGCHAR)
		BitBlt(hdcParaMemExt[3], 
			       gRcParasExt[3].left, gRcParasExt[3].top, RECTW(gRcParasExt[3])+1, RECTH(gRcParasExt[3])+1, 
			       hdcParaExt[3], 
			       gRcParasExt[3].left, gRcParasExt[3].top, 
			       0);

	for(i=1; i<3; i++){
// 		if(bMemCopyExt[i]){
			BitBlt(hdcParaMemExt[i], 
			       gRcParasExt[i].left, gRcParasExt[i].top, RECTW(gRcParasExt[i])+1, RECTH(gRcParasExt[i])+1, 
			       hdcParaExt[i], 
			       gRcParasExt[i].left, gRcParasExt[i].top, 
			       0);
//  			printf("%s:%d Copy Extend (%d) memdc\n", __FILE__, __LINE__, i);
// 		}
	}

	#endif
	gbParasViewing  = FALSE;
	
	gettimeofday(&end_tv, NULL);
	
// 	if(abs(start_tv.tv_usec-end_tv.tv_usec)>200000)
// 		printf("%s:%d (usec)Start time: %ld End time:%ld, inter:%ld\n", __FILE__, __LINE__, 
// 		       start_tv.tv_usec, end_tv.tv_usec, abs(start_tv.tv_usec-end_tv.tv_usec));
	
	return 0;
}




//绘图线程
void *NThreadProcDrawParas(void *arg)
{
		
	//等待绘图信号量，然后绘图
	for(;;){
		
		//sem_wait(&semNDrawWave);
	
		if(!gbParasViewing){
			
		}
	}
	
}

	
/*
	创建显示参数的线程
	
*/
int CreateViewParasProc()
{
	int res;
	
	//创建绘图线程
 	//res = pthread_create(&ptNDrawWave, NULL, NThreadProcDrawParas, NULL);
	
	return res;
}


/*
	初始化参数显示
	1、建立参数显示面板,当显示为大字符屏，或者有扩展参数IBP时，建立两个面板
	2、初始化参数显示区域
	bInterface:当前屏幕类型
*/
int InitParasView(BYTE bInterface)
{
	int i;

	//不允许进行画图
	gbCanViewParas = FALSE;

	//如果面板存在，则先进行销毁
	if(hWndParasPanel !=(HWND)NULL){
		DestroyWindow(hWndParasPanel);	
		hWndParasPanel = (HWND)NULL;	
	}
	
	for(i=0; i<EXT_PARASPANEL_COUNT; i++){
		if(hWndParasPanelExt[i] !=(HWND)NULL){
			DestroyWindow(hWndParasPanelExt[i]);	
			hWndParasPanelExt[i] = (HWND)NULL;
		}
		
	}

	//创建参数显示面板
	hWndParasPanel = CreateWindow("static", "", WS_CHILD | WS_VISIBLE | SS_BLACKRECT,
					ID_PARAPANEL, 
					PARAVIEW_LEFT, 
					PARAVIEW_TOP, 
					PARAVIEW_RIGHT - PARAVIEW_LEFT, 
					PARAVIEW_BOTTOM - PARAVIEW_TOP, 
					ghWndMain, 0);	
	GetClientRect(hWndParasPanel, &gRcParas);
	
	
	//扩展参数面板 (NIBP  List)
		//NIBP  List
		hWndParasPanelExt[0] = CreateWindow("static", "", WS_CHILD | WS_VISIBLE | SS_BLACKRECT,
				ID_PARAPANEL_EXT0, 
#if SCREEN_1024			
			SCREEN_LEFT+5,
 			RC_NIBPLIST_BOTTOM+10,
 			RC_NIBPLIST_RIGHT-5,
 			SCREEN_BOTTOM-RC_NIBPLIST_BOTTOM-10,
#elif 	SCREEN_640
 			SCREEN_LEFT+5,
 			RC_NIBPLIST_BOTTOM+10,
 			RC_NIBPLIST_RIGHT-5,
 			SCREEN_BOTTOM-RC_NIBPLIST_BOTTOM-10,
#else			
 			SCREEN_LEFT+5,
 			RC_NIBPLIST_BOTTOM+10,
 			RC_NIBPLIST_RIGHT-5,
 			SCREEN_BOTTOM-RC_NIBPLIST_BOTTOM-10,
 			
#endif			
			ghWndMain, 0);	
 		GetClientRect(hWndParasPanelExt[0], &gRcParasExt[0]);


	//System info0: Patient Info & Time
	hWndParasPanelExt[1] = CreateWindow("static", "", WS_CHILD | WS_VISIBLE | SS_BLACKRECT,
			ID_PARAPANEL_EXT1, 
			SCREEN_RIGHT-105-120, 
			SCREEN_TOP-7, 
			105+120, 
			42, 
			ghWndMain, 0);	
	GetClientRect(hWndParasPanelExt[1], &gRcParasExt[1]);

	//System Info1: Icon & Info etc.
#if SCREEN_1024
	hWndParasPanelExt[2] = CreateWindow("static", "", WS_CHILD | WS_VISIBLE | SS_BLACKRECT,
					     ID_PARAPANEL_EXT2, 
					    SCREEN_LEFT, 
					    SCREEN_TOP, 
					 100, 
					    30, 
					    ghWndMain, 0);
#elif	SCREEN_640
	hWndParasPanelExt[2] = CreateWindow("static", "", WS_CHILD | WS_VISIBLE | SS_BLACKRECT,
					    ID_PARAPANEL_EXT2, 
					    SCREEN_LEFT, 
					    SCREEN_TOP, 
					  100, 
					    30, 
					    ghWndMain, 0);
#else
	hWndParasPanelExt[2] = CreateWindow("static", "", WS_CHILD | WS_VISIBLE | SS_BLACKRECT,
					    ID_PARAPANEL_EXT2, 
					    SCREEN_LEFT, 
					    SCREEN_TOP, 
					  100, 
					    30, 
					    ghWndMain, 0);
#endif	
	GetClientRect(hWndParasPanelExt[2], &gRcParasExt[2]);

//nibp大字体菜单时面板
	hWndParasPanelExt[3] = CreateWindow("static", "", WS_CHILD | WS_VISIBLE | SS_BLACKRECT,
				ID_PARAPANEL_EXT3, 
#if SCREEN_1024		
			SCREEN_LEFT+21,
 			RC_BIGCHAR_BOTTOM+5,
 			PARAVIEW_LEFT-23-19,
 			SCREEN_BOTTOM-RC_BIGCHAR_BOTTOM-2,
 			
#elif	SCREEN_640

 			SCREEN_LEFT+21,
 			RC_BIGCHAR_BOTTOM+5,
 			PARAVIEW_LEFT-23-19,
 			SCREEN_BOTTOM-RC_BIGCHAR_BOTTOM-2,
#else			
 			SCREEN_LEFT+21,
 			RC_BIGCHAR_BOTTOM+5,
 			PARAVIEW_LEFT-23-19,
 			SCREEN_BOTTOM-RC_BIGCHAR_BOTTOM-2,
 			
#endif			
			ghWndMain, 0);	
 	GetClientRect(hWndParasPanelExt[3], &gRcParasExt[3]);

	//IBp1
	hWndParasPanelExt[4] = CreateWindow("static", "", WS_CHILD | WS_VISIBLE | SS_BLACKRECT,
			ID_PARAPANEL_EXT4, 
			SCREEN_LEFT+21,
 			RC_IBP_BOTTOM+12,
 			PARAVIEW_LEFT/2-22,
 			SCREEN_BOTTOM-RC_IBP_BOTTOM-11,
 			 
			ghWndMain, 0);	
	GetClientRect(hWndParasPanelExt[4], &gRcParasExt[4]);
	
	//IBp2
	hWndParasPanelExt[5] = CreateWindow("static", "", WS_CHILD | WS_VISIBLE | SS_BLACKRECT,
			ID_PARAPANEL_EXT5, 
			PARAVIEW_LEFT/2+20, 
 			RC_IBP_BOTTOM+12,
			PARAVIEW_LEFT/2-20-20, 
			SCREEN_BOTTOM-RC_IBP_BOTTOM-11,
			ghWndMain, 0);	
	GetClientRect(hWndParasPanelExt[5], &gRcParasExt[5]);

	//初始化参数显示区域
	InitDataViewRect();

	//允许画图
	gbCanViewParas = TRUE;
	
	//得到绘图环境	
	if(hdcPara == (HDC)NULL){
		hdcPara = GetClientDC(hWndParasPanel);
	
		//设置DC属性
		SetPenColor(hdcPara, PARACOL_FRAME);
		SetBkColor(hdcPara, PARACOL_BK);
		SetBrushColor(hdcPara, PARACOL_BK);
//		SetBkColor(hdcPara, COLOR_red);
//		SetBrushColor(hdcPara, COLOR_red);
		SetBkMode(hdcPara, BM_TRANSPARENT);
 		
		hdcParaMem = CreateCompatibleDC(hdcPara);	

		//扩展面板
		for(i=0; i<EXT_PARASPANEL_COUNT; i++){
			if(hWndParasPanelExt[i] != (HWND)NULL){
				hdcParaExt[i] = GetClientDC(hWndParasPanelExt[i]);	
				SetPenColor(hdcParaExt[i], PARACOL_FRAME);
				SetBkColor(hdcParaExt[i], PARACOL_BK);
				SetBrushColor(hdcParaExt[i], PARACOL_BK);
 				SetBkMode(hdcParaExt[i], BM_TRANSPARENT);
				if(hdcParaExt[i] != (HDC)NULL) {
					hdcParaMemExt[i] = CreateCompatibleDC(hdcParaExt[i]);
				}
			}
		}
			
	}	

	return 0;
}

/*
	改变IBP1显示模式
*/
int ChangeIbp1ViewMode()
{
	InitRectIbp1();
	return 0;
}


/*
	改变IBP2显示模式
*/
int ChangeIbp2ViewMode()
{
	InitRectIbp2();
	return 0;
}

/*
	切换参数屏幕
*/
int SwitchParasView(BYTE bInterface)
{
	//隐藏窗口,当界面不是bigchar 和nibp 和ibp 时
	if(gCfgSystem.bInterface==SCREEN_BIGCHAR||gCfgSystem.bInterface==SCREEN_NIBPLIST){
		ShowWindow(hWndParasPanelExt[0], SW_SHOW);
		ShowWindow(hWndParasPanelExt[3], SW_SHOW);
		
		
	}
	else{
		ShowWindow(hWndParasPanelExt[0], SW_HIDE);
		ShowWindow(hWndParasPanelExt[3], SW_HIDE);
	}
	if(gCfgSystem.bInterface==SCREEN_IBP){
		ShowWindow(hWndParasPanelExt[4], SW_SHOW);
		ShowWindow(hWndParasPanelExt[5], SW_SHOW);

		
		
	}
	else{
		ShowWindow(hWndParasPanelExt[4], SW_HIDE);
		ShowWindow(hWndParasPanelExt[5], SW_HIDE);

		
	}
	//只有当切换入/出大字符时
	if(gCfgSystem.bInterface == SCREEN_BIGCHAR || bInterface == SCREEN_BIGCHAR 
		||gCfgSystem.bInterface == SCREEN_OTHERBED|| bInterface == SCREEN_OTHERBED
		||gCfgSystem.bInterface==SCREEN_IBP|| bInterface == SCREEN_IBP){
		//不允许进行画图
		gbCanViewParas = FALSE;
		
		//切换参数显示
 	//	InitParasView(gCfgSystem.bInterface);
		//初始化参数显示区域
		InitDataViewRect();
		//切换参数按钮
		SwitchParasMenu();
		//允许进行画图
		gbCanViewParas = TRUE;
	}
	bInterface = gCfgSystem.bInterface;	
	
	return 0;
}


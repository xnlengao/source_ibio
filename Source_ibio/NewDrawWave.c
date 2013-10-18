/*************************************************************************** 
 *	Module Name:	NewDrawWave
 *
 *	Abstract:	新的绘图主程序
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2008-01-18 16:25:48
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "NewDrawWave.h"
#include "DrawWave.h"


//用户对波形面板的配置, 适用于:  Normal, Short Trend, Nibp List
WAVE_PANEL_CFG gWaveCfg_User[WAVEPANEL_MAX_COUNT];


//视图Normal的波形配置
NWAVE_CFG gNWaveCfg_Normal[NWAVEPANEL_MAX_COUNT];
//视图 ShortTrend 波形配置
NWAVE_CFG gNWaveCfg_ShortTrend[NWAVEPANEL_MAX_COUNT];
//视图 Multi ECG 波形配置
NWAVE_CFG gNWaveCfg_MultiEcg[NWAVEPANEL_MAX_COUNT];
//视图7lead ECG 波形配置
NWAVE_CFG gNWaveCfg_7LeadEcg[NWAVEPANEL_MAX_COUNT];
//视图 Big Character 波形配置
NWAVE_CFG gNWaveCfg_BigChar[NWAVEPANEL_MAX_COUNT];
//视图 oxyCrg 波形配置
NWAVE_CFG gNWaveCfg_OxyCrg[NWAVEPANEL_MAX_COUNT];
//视图 NIBP List 波形配置
NWAVE_CFG gNWaveCfg_NibpList[NWAVEPANEL_MAX_COUNT];
//视图 12lead 波形配置
NWAVE_CFG gNWaveCfg_12Lead[NWAVEPANEL_MAX_COUNT];
//视图 other bed 波形配置
NWAVE_CFG gNWaveCfg_OhterBed[NWAVEPANEL_MAX_COUNT];
//视图 IBP 波形配置
NWAVE_CFG gNWaveCfg_IBP[NWAVEPANEL_MAX_COUNT];

//波形面板
NWAVE_PANEL gNWavePanels[NWAVEPANEL_MAX_COUNT];
//波形面板的数量
int giPanelCount = 0;
//波形面板的配置(根据用户配置重新生成的配置,据此以序创建面板) 
WAVE_PANEL gWavePanels[WAVEPANEL_MAX_COUNT];

//波形的显示区域(相对于参数，信息等显示区域, 在该区域内，分配各面板的显示区域)
RECT gNRcWaves;
RECT gNRcWavesAll;	//包括Short Trend, oxyCRG, etc.

//是否检测到心率和脉率
extern BOOL gbHaveBeep_QRS;
extern BOOL gbHaveBeep_Pulse;
//心率图标显示区域
// RECT gNRCEcg_Icon={760, 36, 800, 52};
RECT gNRCEcg_Icon;

///1mm对应像素的点数
float gfNMMToPixel = 3.2;

//控制波形绘制的信号量
sem_t	semNDrawWave;

//波形复位控制
BOOL gbResetWaves = TRUE;

//控制是否可以进行绘制的变量
BOOL gbNCanDrawWave = FALSE;
//控制是否可以进行绘制的变量
BOOL gbNCanDrawWave_OB = FALSE;

//绘制第几个参数标志位
extern int gbViewPara;
//定时器线程
static pthread_t 	ptNewDrawTimer;	
BOOL gbHasCreatedTimer = FALSE;

//是否进入报警暂停状态
extern BOOL gbSuspending;
//报警暂停状态倒计时
extern int giSuspendTime;
//绘图线程切换互斥量
pthread_mutex_t 	mtDrawWave;	

//ECG定标
float gfMMToPixel=3.2;				//1mm=3.2pixel
float gfLSB_Ecg = 0.0131579;		//ECG AD采样的最低有效位 1/gSamplesPerMV, (暂未使用)
// static float gSamplesPerMV = 76;			//1mv对应的采样点数
float gSamplesPerMV = 62;		///1mv对应的采样点数

//根据配置文件初始化用户的波形配置 gNWaveCfg_Normal
static int InitUsrWaveConfig(const unsigned char *pCfgFiles)
{
	int iCfgValue;
	int res;
	int i;
	
	if(pCfgFiles == NULL) return -1;
		
	for(i = 0; i<NWAVEPANEL_MAX_COUNT; i++){
		gNWaveCfg_Normal[i].bID = NWID_NONE;
		gNWaveCfg_Normal[i].bSwitch = SWITCH_OFF;
	}
	
	//Wave 0	 ECG1
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave0_id", &iCfgValue);
	if(iCfgValue <NWID_NONE || iCfgValue >NWID_MAX) iCfgValue = NWID_NONE;	
	gNWaveCfg_Normal[0].bID = iCfgValue;
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave0_switch", &iCfgValue);
	if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_OFF;	
	gNWaveCfg_Normal[0].bSwitch = iCfgValue;
	//Wave 1 ECG 2
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave1_id", &iCfgValue);
	if(iCfgValue <NWID_NONE || iCfgValue >NWID_MAX) iCfgValue = NWID_NONE;	
	gNWaveCfg_Normal[1].bID = iCfgValue;
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave1_switch", &iCfgValue);
	if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_OFF;	
	gNWaveCfg_Normal[1].bSwitch = iCfgValue;
	//Wave 2 Pleth
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave2_id", &iCfgValue);
	if(iCfgValue <NWID_NONE || iCfgValue >NWID_MAX) iCfgValue = NWID_NONE;	
	gNWaveCfg_Normal[2].bID = iCfgValue;
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave2_switch", &iCfgValue);
	if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_OFF;	
	gNWaveCfg_Normal[2].bSwitch = iCfgValue;
	
	//Wave 3 RESP
	if(gbHaveCo2){
		res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave6_id", &iCfgValue);
		if(iCfgValue <NWID_NONE || iCfgValue >NWID_MAX) iCfgValue = NWID_NONE;	
		gNWaveCfg_Normal[3].bID = iCfgValue;
		res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave6_switch", &iCfgValue);
		if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_OFF;	
		gNWaveCfg_Normal[3].bSwitch = iCfgValue;

	} else{
		res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave3_id", &iCfgValue);
		if(iCfgValue <NWID_NONE || iCfgValue >NWID_MAX) iCfgValue = NWID_NONE;	
		gNWaveCfg_Normal[3].bID = iCfgValue;
		res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave3_switch", &iCfgValue);
		if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_OFF;	
		gNWaveCfg_Normal[3].bSwitch = iCfgValue;
	}
	
	/*
	//Wave 4 IBP1
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave4_id", &iCfgValue);
	if(iCfgValue <NWID_NONE || iCfgValue >NWID_MAX) iCfgValue = NWID_NONE;	
	gNWaveCfg_Normal[4].bID = iCfgValue;
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave4_switch", &iCfgValue);
	if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_OFF;	
	gNWaveCfg_Normal[4].bSwitch = iCfgValue;
	if(!gbHaveIbp) gNWaveCfg_Normal[4].bSwitch = SWITCH_OFF;

	//Wave 5 IBP2
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave5_id", &iCfgValue);
	if(iCfgValue <NWID_NONE || iCfgValue >NWID_MAX) iCfgValue = NWID_NONE;	
	gNWaveCfg_Normal[5].bID = iCfgValue;
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave5_switch", &iCfgValue);
	if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_OFF;	
	gNWaveCfg_Normal[5].bSwitch = iCfgValue;
	if(!gbHaveIbp) gNWaveCfg_Normal[5].bSwitch = SWITCH_OFF;
	/*
	//Wave 6 CO2
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave6_id", &iCfgValue);
	if(iCfgValue <NWID_NONE || iCfgValue >NWID_MAX) iCfgValue = NWID_NONE;	
	gNWaveCfg_Normal[6].bID = iCfgValue;
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave6_switch", &iCfgValue);
	if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_OFF;	
	gNWaveCfg_Normal[6].bSwitch = iCfgValue;
	if(!gbHaveCo2) gNWaveCfg_Normal[6].bSwitch = SWITCH_OFF;
	*/
	return 0;
}

/*
	初始化波形配置信息
	Normal: 根据用户的配置而定
	ShortTrend: 同Normal
	MultiECG: ECG + SpO2 + Resp/CO2
	NIBP List: ECG1 + SpO2 + Resp / CO2
	oxyCRG: ECG1 + SpO2 + Resp / CO2
	BigChar: ECG1 + SpO2 
*/
static int InitWaveConfig()
{
	int i;
		
	// Normal，根据用户配置而定
	InitUsrWaveConfig(gFileSetup);
	
	//ShortTrend 同Normal
	for(i = 0; i<NWAVEPANEL_MAX_COUNT; i++){
		gNWaveCfg_ShortTrend[i].bID = gNWaveCfg_Normal[i].bID;
		gNWaveCfg_ShortTrend[i].bSwitch = gNWaveCfg_Normal[i].bSwitch;
	}
	
	//MultiECG:  ECG  + SpO2 + Resp / CO2
	for(i = 0; i<NWAVEPANEL_MAX_COUNT; i++){
		gNWaveCfg_MultiEcg[i].bID = NWID_NONE;
		gNWaveCfg_MultiEcg[i].bSwitch = SWITCH_OFF;
	}
	gNWaveCfg_MultiEcg[0].bID = NWID_MULTIECG;
	gNWaveCfg_MultiEcg[0].bSwitch = SWITCH_ON;
	gNWaveCfg_MultiEcg[1].bID = NWID_SPO2;
 	gNWaveCfg_MultiEcg[1].bSwitch = SWITCH_ON;		
	if(gbHaveCo2) {
		gNWaveCfg_MultiEcg[2].bID = NWID_CO2;
		gNWaveCfg_MultiEcg[2].bSwitch = SWITCH_ON;
	}
	else{
		gNWaveCfg_MultiEcg[2].bID = NWID_RESP;
		gNWaveCfg_MultiEcg[2].bSwitch = SWITCH_ON;
	}
	
	//7LeadECG:  ECG 
	for(i = 0; i<NWAVEPANEL_MAX_COUNT; i++){
		gNWaveCfg_7LeadEcg[i].bID = NWID_NONE;
		gNWaveCfg_7LeadEcg[i].bSwitch = SWITCH_OFF;
	}
	gNWaveCfg_7LeadEcg[0].bID = NWID_MULTIECG;
	gNWaveCfg_7LeadEcg[0].bSwitch = SWITCH_ON;
 	
	//NIBP List: ECG1 + SpO2 + Resp / CO2
	for(i = 0; i<NWAVEPANEL_MAX_COUNT; i++){
		gNWaveCfg_NibpList[i].bID = NWID_NONE;
		gNWaveCfg_NibpList[i].bSwitch = SWITCH_OFF;
	}
	//NIBP List 同Normal
	for(i = 0; i<NWAVEPANEL_MAX_COUNT; i++){
		gNWaveCfg_NibpList[i].bID = gNWaveCfg_Normal[i].bID;
		gNWaveCfg_NibpList[i].bSwitch = gNWaveCfg_Normal[i].bSwitch;
	}
	
	//oxyCRG: ECG1 + SpO2 + Resp / CO2
	for(i = 0; i<NWAVEPANEL_MAX_COUNT; i++){
		gNWaveCfg_OxyCrg[i].bID = NWID_NONE;
		gNWaveCfg_OxyCrg[i].bSwitch = SWITCH_OFF;
	}
		// oxyCRG 同Normal
	for(i = 0; i<NWAVEPANEL_MAX_COUNT; i++){
		gNWaveCfg_OxyCrg[i].bID = gNWaveCfg_Normal[i].bID;
		gNWaveCfg_OxyCrg[i].bSwitch = gNWaveCfg_Normal[i].bSwitch;
	}
	/*
	gNWaveCfg_OxyCrg[0].bID = NWID_ECG1;
	gNWaveCfg_OxyCrg[0].bSwitch = SWITCH_ON;
	gNWaveCfg_OxyCrg[1].bID = NWID_SPO2;
	gNWaveCfg_OxyCrg[1].bSwitch = SWITCH_ON;		
	if(gbHaveCo2) {
		gNWaveCfg_OxyCrg[2].bID = NWID_CO2;
		gNWaveCfg_OxyCrg[2].bSwitch = SWITCH_ON;
	}
	else{
		gNWaveCfg_OxyCrg[2].bID = NWID_RESP;
		gNWaveCfg_OxyCrg[2].bSwitch = SWITCH_ON;
	}
	*/
	//BigChar: ECG1 + SpO2 
	for(i = 0; i<NWAVEPANEL_MAX_COUNT; i++){
		gNWaveCfg_BigChar[i].bID = NWID_NONE;
		gNWaveCfg_BigChar[i].bSwitch = SWITCH_OFF;
	}
		// BigChar 同Normal
	for(i = 0; i<NWAVEPANEL_MAX_COUNT; i++){
		gNWaveCfg_BigChar[i].bID = gNWaveCfg_Normal[i].bID;
		gNWaveCfg_BigChar[i].bSwitch = gNWaveCfg_Normal[i].bSwitch;
	}
	
		//IBP: ECG1 + SpO2 +RESP+IBP1+IBP2
	for(i = 0; i<NWAVEPANEL_MAX_COUNT; i++){
		gNWaveCfg_IBP[i].bID = NWID_NONE;
		gNWaveCfg_IBP[i].bSwitch = SWITCH_OFF;
	}
	gNWaveCfg_IBP[0].bID = NWID_ECG1;
	gNWaveCfg_IBP[0].bSwitch = SWITCH_ON;
	gNWaveCfg_IBP[1].bID = NWID_SPO2;
	gNWaveCfg_IBP[1].bSwitch = SWITCH_ON;	
	
	if(gbHaveCo2) {
		gNWaveCfg_IBP[2].bID = NWID_CO2;
		gNWaveCfg_IBP[2].bSwitch = SWITCH_ON;
	}
	else{
		gNWaveCfg_IBP[2].bID = NWID_RESP;
		gNWaveCfg_IBP[2].bSwitch = SWITCH_ON;
	}
	gNWaveCfg_IBP[3].bID = NWID_IBP1;
	gNWaveCfg_IBP[3].bSwitch = SWITCH_ON;
	gNWaveCfg_IBP[4].bID = NWID_IBP2;
	gNWaveCfg_IBP[4].bSwitch = SWITCH_ON;
	
	/*
	gNWaveCfg_BigChar[0].bID = NWID_ECG1;
	gNWaveCfg_BigChar[0].bSwitch = SWITCH_ON;
	gNWaveCfg_BigChar[1].bID = NWID_SPO2;
	gNWaveCfg_BigChar[1].bSwitch = SWITCH_ON;		
	if(gbHaveCo2) {
		gNWaveCfg_BigChar[2].bID = NWID_CO2;
		gNWaveCfg_BigChar[2].bSwitch = SWITCH_ON;
	}
	else{
		gNWaveCfg_BigChar[2].bID = NWID_RESP;
		gNWaveCfg_BigChar[2].bSwitch = SWITCH_ON;
	}
	*/
	//12Lead:  12 ECG waves
	for(i = 0; i<NWAVEPANEL_MAX_COUNT; i++){
		gNWaveCfg_12Lead[i].bID = NWID_NONE;
		gNWaveCfg_12Lead[i].bSwitch = SWITCH_OFF;
	}
	gNWaveCfg_12Lead[0].bID = NWID_12LEAD;
	gNWaveCfg_12Lead[0].bSwitch = SWITCH_ON;
 	
	// Otherbed 同Normal
	for(i = 0; i<NWAVEPANEL_MAX_COUNT; i++){
		gNWaveCfg_OhterBed[i].bID = gNWaveCfg_Normal[i].bID;
		gNWaveCfg_OhterBed[i].bSwitch = gNWaveCfg_Normal[i].bSwitch;
	}
	return 0;
}
/*
	通过波形的范围计算波形面板的增益系数(Y轴上的比例系数)
*/
static int SetWaveY(PWAVE_PANEL pPanel)
{
	int iValueMax;
	int iValueMin;
	float fYAxis=0.0;		//y轴比例系数
	RECT rcPanel;		//波形面板的大小
	
	if(pPanel == NULL) return -1;
	
	//取得并设置波形的范围
	switch(pPanel->bID){
		case WID_ECG1:{ 
			iValueMax = NWAVEMAX_ECG;	
			iValueMin = 0; 
		}break; 
		case WID_ECG2:{ 
			iValueMax = NWAVEMAX_ECG;	
			iValueMin = 0; 
		}break; 
		case WID_MULTIECG:{
			iValueMax = NWAVEMAX_ECG;	
			iValueMin = 0; 
		}break;
		case WID_SPO2:{
			iValueMax = NWAVEMAX_SPO2;
			iValueMin = 0; 
		}break;
		case WID_RESP:{
			iValueMax = NWAVEMAX_RESP;
			iValueMin = 0; 
		}break;
		case WID_CO2:{
			switch(gCfgCO2.bRange){
				case CO2_RANGE_MIDDLE:{
					iValueMax = 75;			
				}break;
				case CO2_RANGE_SMALL:{
					iValueMax = 99;
				}break;
				default:{
					iValueMax = 50;
				}break;
			}
			iValueMin = 0; 
		}break;
		case WID_IBP1:{
			iValueMax = gCfgIbp1.iScaleTop;
			iValueMin = gCfgIbp1.iScaleBottom; 
		}break;
		case WID_IBP2:{
			iValueMax = gCfgIbp2.iScaleTop;
			iValueMin = gCfgIbp2.iScaleBottom; 
		}break;
		
	}
	pPanel->iYMax = iValueMax;
	pPanel->iYMin = iValueMin;

	//根据波形的范围和面板的实际大小，计算波形在Y轴上的缩放系数
	if(pPanel->hWnd == (HWND)NULL ) return -1;
	GetClientRect(pPanel->hWnd, &rcPanel);
		
	fYAxis = (float)((float)(iValueMax-iValueMin) / (float)((float)(rcPanel.bottom-rcPanel.top)/(float)pPanel->bWaveCount));	
	pPanel->fYAxis = fYAxis;
			
 	printf("(%s:%d)MAX: %d  MIN:%d    RECT: H-%d    Y Axis: %f  \n", __FILE__, __LINE__,
 	       pPanel->iYMax, pPanel->iYMin,  (rcPanel.bottom-rcPanel.top)/pPanel->bWaveCount, pPanel->fYAxis);
	
	pPanel->bReset = TRUE;
	
	return 0;
}

/*
    ==================================================
	Description:	对坐标系进行缩放
			
   	Parameters:								method of calling:(r/w/rw/other)
   		hdc : 绘图环境									rw
		ViewOrg_X, ViewOrg_Y: 视图原点;                   r
		ViewExt_X, ViewExt_Y:  视图范围;                   r
		WindOrg_X, WindOrg_Y: 窗口原点;                 r
		WindExt_X, WindExt_Y: 窗口范围                   r
				
	return:0-OK, -1-Error
    ===================================================
*/
int ChangeCoord(HDC hdc, int ViewOrg_X, int ViewOrg_Y, int ViewExt_X, int ViewExt_Y, int WindOrg_X, int WindOrg_Y, int WindExt_X, int WindExt_Y)
{
	POINT Pt_ViewOrg;
	POINT Pt_ViewExt;
	POINT Pt_WindExt;
	POINT Pt_WindOrg;

	Pt_ViewOrg.x = ViewOrg_X;
	Pt_ViewOrg.y = ViewOrg_Y;
	Pt_ViewExt.x = ViewExt_X;
	Pt_ViewExt.y = ViewExt_Y;
	Pt_WindOrg.x = WindOrg_X;
	Pt_WindOrg.y = WindOrg_Y;
	Pt_WindExt.x = WindExt_X;
	Pt_WindExt.y = WindExt_Y;
	
	SetMapMode(hdc, MM_ANISOTROPIC);
	SetViewportOrg(hdc, &Pt_ViewOrg);
	SetViewportExt(hdc, &Pt_ViewExt);         
	SetWindowExt(hdc, &Pt_WindExt);
	SetWindowOrg(hdc, &Pt_WindOrg );
	
	return 0;
}

/*
	根据面板的设置（ID和区域）设置X，Y轴 的系数
*/
int SetWavePanelXY(PNWAVE_PANEL pWavePanel)
{
	//X
	float ftmpX;
	float fSpeeds[4] = {6.25, 12.5, 25.0, 50.0};
	float ftmpSpeed;
	//Y
	int iMaxY, iMinY;
	float ftmpY;
	
	if(pWavePanel == NULL) return -1;
	
	//根据采样频率和速度计算X轴系数
	switch(pWavePanel->bWaveID){
		case NWID_ECG1:
		case NWID_ECG2:
		case NWID_MULTIECG:{
			ftmpSpeed = fSpeeds[gCfgEcg.bScanSpeed];
			iMaxY =  NWAVEMAX_ECG;
			iMinY = 0;
		}break;
		case NWID_SPO2:{
			ftmpSpeed = fSpeeds[gCfgSpO2.bScanSpeed];	
			iMaxY =  NWAVEMAX_SPO2;
			iMinY = 0;
		}break;
		case NWID_RESP:{
			ftmpSpeed = fSpeeds[gCfgResp.bScanSpeed];	
			iMaxY =  NWAVEMAX_RESP;
			iMinY = 0;
		}break;
		case NWID_CO2:{
			ftmpSpeed = fSpeeds[gCfgCO2.bScanSpeed];	
			switch(gCfgCO2.bRange){
				case CO2_RANGE_MIDDLE:{iMaxY = 75;}break;
				case CO2_RANGE_SMALL:{iMaxY = 100;}break;
				default:{iMaxY = 50;}break;
			}
			iMinY = 0;
		}break;			
		case NWID_IBP1:{
			ftmpSpeed = fSpeeds[gCfgIbp1.bScanSpeed];	
			iMaxY = gCfgIbp1.iScaleTop;
			iMinY = gCfgIbp1.iScaleBottom; 
		}break;
		case NWID_IBP2:{
			ftmpSpeed = fSpeeds[gCfgIbp2.bScanSpeed];	
			iMaxY = gCfgIbp2.iScaleTop;
			iMinY = gCfgIbp2.iScaleBottom; 
		}break;
		default:{
			ftmpSpeed = 25.0;
			iMaxY = RECTH(pWavePanel->rcWave);
			iMinY = 0; 
		}break;
	}
	
	//X系数：Hz/mm ---  SampleRate / (speed * mmToPixel)
	ftmpX =(float)( (float)NSAMPLERATE / (ftmpSpeed * gfNMMToPixel));
	
	//Y系数：(Max-Min) / (RECTH / WaveCount)
	ftmpY = (float)((float)(iMaxY - iMinY) / (float)((float)(RECTH(pWavePanel->rcWave))/(float)pWavePanel->bWaveCount));
	/*
	if(B_PRINTF) printf("%s:%d Panel ID : %d, Speed: %.1f, gfNMMToPixel:%f,X_Axis: %.1f.\n", __FILE__, __LINE__, 
	       pWavePanel->bWaveID, ftmpSpeed,gfNMMToPixel, ftmpX);
	
	if(B_PRINTF) printf("YMax: %d, YMin: %d, RECTH: %d,bWaveCount: %1f, Y_Axis: %.1f.\n", 
	       iMaxY, iMinY, RECTH(pWavePanel->rcWave), pWavePanel->bWaveCount,ftmpY);
	*/	
	//设置XY系数
	pWavePanel->fXAxis = ftmpX;
	pWavePanel->fYAxis = ftmpY;
	
	return 0;
}

/*
	根据配置设置波形绘制面板 绘图区域 
*/
static int SetWavePanelsArea(int iPanelCount, RECT rcWaveArea)
{
	int iWaveCount;		//波形的数量	
	int i;
	
	//面板的顶部坐标
	int iPanelTop, iPanelLeft,iPanelRight;
	//面板宽高
	int iPanelWidth, iPanelHeight;	

	//统计波形的数量(包括级联)
  	iWaveCount = 0;
  	for(i=0; i<iPanelCount; i++){
 		if(gNWavePanels[i].bWaveID != NWID_NONE){
 			iWaveCount += gNWavePanels[i].bWaveCount;
 		}
 	}
 	printf("%s:%d Total %d waves.\n", __FILE__, __LINE__, iWaveCount);
	
	//根据波形数量和绘图区域，确定个面板的绘图区域
	//得到面板的宽高
	iPanelWidth = RECTW(rcWaveArea);
	//根据波形数量（包括级联）
	iPanelHeight = (RECTH(rcWaveArea)) / iWaveCount - PANELAREA_INTER;	
	
	//设置各个面板的区域
	iPanelTop = rcWaveArea.top;
	iPanelLeft = rcWaveArea.left;
	iPanelRight=rcWaveArea.right;
	for(i=0; i<iPanelCount; i++){
		//Para
		SetRect(&(gNWavePanels[i].rcPara), 
			  iPanelRight-PARAOFWAVE_WIDTH, 
			  iPanelTop, 
			  iPanelRight, 
			  iPanelTop + iPanelHeight*(gNWavePanels[i].bWaveCount));
		//Wave
		SetRect(&(gNWavePanels[i].rcWave), 
			  iPanelLeft,  
			  iPanelTop, 
			  iPanelLeft + iPanelWidth-5, 
			  iPanelTop + iPanelHeight*(gNWavePanels[i].bWaveCount));
		iPanelTop += (iPanelHeight*(gNWavePanels[i].bWaveCount)+PANELAREA_INTER);
	}
	
//  	FillWaveArea(gNRcWavesAll,  iPanelCount);
	
	return iPanelCount;
}

/*
	设置波形面板
	返回实际绘制的波形面板数量
*/
static int SetWavePanels(const PNWAVE_CFG pCfg, RECT rcWaveArea)
{
	int  iPanelCount;		//面板的数量
	int i;
	
	if(pCfg == NULL) return -1;
	
	//初始化面板
	for(i=0; i<NWAVEPANEL_MAX_COUNT; i++){
		gNWavePanels[i].bWaveID = NWID_NONE;
		gNWavePanels[i].bCascade = CASCADE_OFF;
		gNWavePanels[i].bWaveCount = 1;
	}
	
	//获得配置,并生成系统配置	
	iPanelCount = 0;
	for(i=0; i<NWAVEPANEL_MAX_COUNT; i++){
		if(pCfg[i].bID != NWID_NONE && pCfg[i].bSwitch != SWITCH_OFF){
			gNWavePanels[iPanelCount].bWaveID = pCfg[i].bID;
			//判断是否需要级联(ECG)
			if(pCfg[i].bID == NWID_ECG1 ){
				gNWavePanels[iPanelCount].bCascade = gCfgEcg.bCascade;
				if(gNWavePanels[iPanelCount].bCascade == CASCADE_ON) gNWavePanels[iPanelCount].bWaveCount = 2;
			}
			else{
				//其他波形不需要级联
				gNWavePanels[iPanelCount].bCascade = CASCADE_OFF;
				//7心电
				//由于参数板只支持两通道不能7导联同屏，暂时改为6导联同屏

				if(pCfg[i].bID == NWID_MULTIECG) gNWavePanels[iPanelCount].bWaveCount = 7;
				if(pCfg[i].bID == NWID_12LEAD) gNWavePanels[iPanelCount].bWaveCount = 12;
			}
			iPanelCount ++;
		}
	}
	
	if(B_PRINTF) printf("%s:%d Total %d waveform panel:\n", __FILE__, __LINE__, iPanelCount);
	
	//没有波形，退出
	if(iPanelCount<=0) return -1;

	//设置面板区域
 	SetWavePanelsArea(iPanelCount, rcWaveArea);
	
	//根据面板的设置（ID和区域）设置X，Y轴 的系数
	for(i=0; i<iPanelCount; i++){
		SetWavePanelXY(&(gNWavePanels[i]));
	}
	
	return iPanelCount;
}

/*
	初始化波形绘制面板
	返回实际绘制的波形面板数量	
*/
static int InitWavePanels()
{
	int i;
	int iPanelCount = 0;
	NWAVE_CFG tmpWaveCfg[NWAVEPANEL_MAX_COUNT];
	
	SetRect(&gNRcWavesAll, 
		 RC_NORMAL_LEFT, 
		 RC_NORMAL_TOP, 
		 RC_NORMAL_RIGHT+3, 
		 RC_NORMAL_BOTTOM+2);
	
	//根据视图类型确定当前的波形配置,和绘图区域
	switch(gCfgSystem.bInterface){
		case SCREEN_SHORTTREND:{
			for(i= 0; i<NWAVEPANEL_MAX_COUNT; i++){
				tmpWaveCfg[i].bID = gNWaveCfg_ShortTrend[i].bID;
				tmpWaveCfg[i].bSwitch = gNWaveCfg_ShortTrend[i].bSwitch;
			}
			//确定绘图区域
			SetRect(&gNRcWaves, 
				 RC_SHORTTREND_LEFT, 
				 RC_SHORTTREND_TOP, 
				 RC_SHORTTREND_RIGHT, 
				 RC_SHORTTREND_BOTTOM);
			
		}break;
		case SCREEN_BIGCHAR:{
			for(i= 0; i<NWAVEPANEL_MAX_COUNT; i++){
				tmpWaveCfg[i].bID = gNWaveCfg_BigChar[i].bID;
				tmpWaveCfg[i].bSwitch = gNWaveCfg_BigChar[i].bSwitch;
			}
			//确定绘图区域
			SetRect(&gNRcWaves, 
				 RC_BIGCHAR_LEFT, 
				 RC_BIGCHAR_TOP, 
				 RC_BIGCHAR_RIGHT, 
				 RC_BIGCHAR_BOTTOM);
		}break;
		case SCREEN_NIBPLIST:{
			for(i= 0; i<NWAVEPANEL_MAX_COUNT; i++){
				tmpWaveCfg[i].bID = gNWaveCfg_NibpList[i].bID;
				tmpWaveCfg[i].bSwitch = gNWaveCfg_NibpList[i].bSwitch;
			}
			//确定绘图区域
			SetRect(&gNRcWaves, 
				 RC_NIBPLIST_LEFT, 
				 RC_NIBPLIST_TOP, 
				 RC_NIBPLIST_RIGHT, 
				 RC_NIBPLIST_BOTTOM);
		}break;
		case SCREEN_MULTIECG:{
			for(i= 0; i<NWAVEPANEL_MAX_COUNT; i++){
				tmpWaveCfg[i].bID = gNWaveCfg_MultiEcg[i].bID;
				tmpWaveCfg[i].bSwitch = gNWaveCfg_MultiEcg[i].bSwitch;
			}
			//确定绘图区域
			SetRect(&gNRcWaves, 
				 RC_MECG_LEFT, 
				 RC_MECG_TOP, 
				 RC_MECG_RIGHT, 
				 RC_MECG_BOTTOM);
		}break;
		case SCREEN_7LEADECG:{
			for(i= 0; i<NWAVEPANEL_MAX_COUNT; i++){
				tmpWaveCfg[i].bID = gNWaveCfg_7LeadEcg[i].bID;
				tmpWaveCfg[i].bSwitch = gNWaveCfg_7LeadEcg[i].bSwitch;
			}
			//确定绘图区域
			SetRect(&gNRcWaves, 
				 RC_MECG_LEFT, 
				 RC_MECG_TOP, 
				 RC_MECG_RIGHT, 
				 RC_MECG_BOTTOM);
		}break;
		case SCREEN_OXYCRG:{
			for(i= 0; i<NWAVEPANEL_MAX_COUNT; i++){
				tmpWaveCfg[i].bID = gNWaveCfg_OxyCrg[i].bID;
				tmpWaveCfg[i].bSwitch = gNWaveCfg_OxyCrg[i].bSwitch;
			}
			//确定绘图区域
			SetRect(&gNRcWaves, 
				 RC_OXYCRG_LEFT, 
				 RC_OXYCRG_TOP, 
				 RC_OXYCRG_RIGHT, 
				 RC_OXYCRG_BOTTOM);
		}break;
		case SCREEN_12LEAD:{
			for(i= 0; i<NWAVEPANEL_MAX_COUNT; i++){
				tmpWaveCfg[i].bID = gNWaveCfg_12Lead[i].bID;
				tmpWaveCfg[i].bSwitch = gNWaveCfg_12Lead[i].bSwitch;
			}
			//确定绘图区域
			SetRect(&gNRcWaves, 
				 RC_12LEAD_LEFT, 
				 RC_12LEAD_TOP, 
				 RC_12LEAD_RIGHT, 
				 RC_12LEAD_BOTTOM);
		}break;
		case SCREEN_OTHERBED:{
			for(i= 0; i<NWAVEPANEL_MAX_COUNT; i++){
				tmpWaveCfg[i].bID = gNWaveCfg_OhterBed[i].bID;
				tmpWaveCfg[i].bSwitch = gNWaveCfg_OhterBed[i].bSwitch;
			}
			//确定绘图区域
			SetRect(&gNRcWaves, 
				 RC_OTHERBED_LEFT, 
				 RC_OTHERBED_TOP, 
				 RC_OTHERBED_RIGHT, 
				 RC_OTHERBED_BOTTOM);
		}break;
		case SCREEN_IBP:{
			for(i= 0; i<NWAVEPANEL_MAX_COUNT; i++){
				tmpWaveCfg[i].bID = gNWaveCfg_IBP[i].bID;
				tmpWaveCfg[i].bSwitch = gNWaveCfg_IBP[i].bSwitch;
			}
			//确定绘图区域
			SetRect(&gNRcWaves, 
				 RC_IBP_LEFT, 
				 RC_IBP_TOP, 
				 RC_IBP_RIGHT, 
				 RC_IBP_BOTTOM);
		}break;
		default:{//Normal
			for(i= 0; i<NWAVEPANEL_MAX_COUNT; i++){
				tmpWaveCfg[i].bID = gNWaveCfg_Normal[i].bID;
				tmpWaveCfg[i].bSwitch = gNWaveCfg_Normal[i].bSwitch;
			}
			//确定绘图区域
			SetRect(&gNRcWaves, 
				 RC_NORMAL_LEFT, 
				 RC_NORMAL_TOP, 
				 RC_NORMAL_RIGHT, 
				 RC_NORMAL_BOTTOM);
		}break;
	}
			
	//根据配置初始化波形绘制面板
 	iPanelCount = SetWavePanels((PNWAVE_CFG)&tmpWaveCfg, gNRcWaves);
	
	//出错返回
	if(iPanelCount <=0) return -1;
	
	return iPanelCount;
}

/*
	显示心率图标
*/
extern HWND ghWndPanel_Heart;
int HRIconDisplay(BOOL bDisplay)
{

	HDC hdc = (HDC)NULL;
	RECT rc; 
	
	hdc = GetClientDC(ghWndPanel_Heart);
	GetClientRect(ghWndPanel_Heart, &gNRCEcg_Icon);
	if(hdc != (HDC)NULL){
		if(bDisplay){ 
			FillBoxWithBitmap(hdc, gNRCEcg_Icon.left,gNRCEcg_Icon.top, 24, 24, &gIconHR[0]);     
		}
		else{ //隐藏图标                               
			SetBrushColor(hdc, COLOR_black);
			FillBox(hdc, gNRCEcg_Icon.left, gNRCEcg_Icon.top, 24, 24); 
		} 
		
		ReleaseDC(hdc);
		hdc = (HDC)NULL;
	}
	
	
	return 0;
}
 int DisplayHeartIcon()
{
	static int iHrState =0;	//状态 0：开始状态 1：图标显示状态 2：图标关闭状态
	static int iHrCount =0;
	
	switch(iHrState){
		case 1:{
			//iHrCount=(iHrCount+1)%10;
			iHrCount=(iHrCount+1)%2;
			if(iHrCount==0)
				iHrState = 2;
		
		}break;	
		case 2:{
			iHrCount=(iHrCount+1)%2;
			if(iHrCount==0){
				HRIconDisplay(FALSE);
				iHrState = 0;
			}
		}break;
		default:{
			//TODO:此处要辨别心率发声还是脉率发声
			switch(gCfgEcg.bAlmSource){
				case HR_ALM_SRC_HR:{
					//按照ECG发声
					if(gbHaveBeep_QRS){
						Sound_Beep();
						HRIconDisplay(TRUE);
						gbHaveBeep_QRS = FALSE;
						iHrCount = 0;
						iHrState = 1;
					}	
				}break;
				case HR_ALM_SRC_PR:{
					
					//按照SpO2发声	
					if(gbHaveBeep_Pulse){
						Sound_Beep();
						HRIconDisplay(TRUE);
						gbHaveBeep_Pulse = FALSE;
						iHrCount = 0;
						iHrState = 1;
					}	
					
				}break;
				default:{
					//Auto
					//首先判断ECG是否脱落
					if(gValueEcg.bRAOff){
						//按照SpO2发声	
						if(gbHaveBeep_Pulse){
							Sound_Beep();
							HRIconDisplay(TRUE);
							gbHaveBeep_Pulse = FALSE;
							iHrCount = 0;
							iHrState = 1;
						}	
					}
					else{
						//按照ECG发声
						if(gbHaveBeep_QRS){
							Sound_Beep();
							HRIconDisplay(TRUE);
							gbHaveBeep_QRS = FALSE;
							iHrCount = 0;
							iHrState = 1;
						}
					}
				}break;
			}
			
		}break;
	}
	return 0;
}


//是否正在绘制参数
extern BOOL gbParasViewing;

extern BOOL gbApneaTimeChanged;
/////////////////////////定时器 ///////////////////////
/*
	定时器线程
*/
extern BOOL gbTestPowerDown;	//调试关机
static void *NewThreadProcDrawTimer(void *arg)
{
	struct timeval	timeout;
	int delay;
	
	//时间计数器
	int iTimer_1s = 0;
	int iTimer_500ms = 0;
	int iTimer_1m = 0;
	
	int iHrState =0;	//状态 0：开始状态 1：图标显示状态 2：图标关闭状态
	int iHrCount =0;
		
	for(;;)
	{
		//-- 利用Select延时 --
		timeout.tv_sec = 0;
		timeout.tv_usec= TIMER_DRAW;

		delay = select(FD_SETSIZE, NULL, NULL, NULL, &timeout);

		//喂狗，因为绘图线程优先级较高
		WDT_Feed();
		
		//电源正在关闭时，不显示
		if(gPowerStatus.bShutDown == POWER_SHUTDOWN){
			break;
		}

		//功能键轮询处理
		DealFunKey();
		//更新绘图
		UpdateDrawWave();
		//更新他床绘图
		if(gCfgSystem.bInterface==SCREEN_OTHERBED){
			NewUpdateOtherBedWave();
		}
		DisplayHeartIcon();
		//更新参数显示
		
		
		

		
		//计数器1
		if(iTimer_1s<((TIMER_1S/TIMER_DRAW)-1)){    
			
			iTimer_1s ++;   
		}
		else{
			iTimer_1s =0;
			
			//更新周期测量倒计时
			UpdateNibpAutoTimer(FALSE);
			//更新连续测量倒计时
			UpdateNibpStatTimer();
			//判断是否有窒息报警
			if(gbApneaTimeChanged)
				IsAlm_Apnea(&gValueResp, &gCfgResp, TRUE);
			else
				IsAlm_Apnea(&gValueResp, &gCfgResp, FALSE);

			
			//更新报警发声
			UpdateAlarmSound();
			//如果处在报警暂停状态下,则倒计时
			if(gbSuspending){
				if(giSuspendTime >0){
					giSuspendTime --;
				}
				else{
					giSuspendTime = gCfgAlarm.bPauseTime*10;
					gbSuspending = FALSE;
				}
			} 
			
			//--- 获得电源状态 ---old
		//	GetPowerStatus();
			//send_net_data_test();	
			
		}
		
		//0.5s的定时器
		if(iTimer_500ms <((TIMER_500MS/TIMER_DRAW)-1)){    
			iTimer_500ms ++;   
		}
		else{
			iTimer_500ms =0;
			
			//更新报警灯光
			UpdateAlarmLight();
 	//		if(gbViewDemoData && gbNCanDrawWave)
 	//		DrawDemo(NULL, NULL);
		}
		
	}
}

/*
	创建绘图定时器
	return: 0-OK, -1:Failure
*/
static int NewCreateDrawWaveTimer()
{
	int res;
	
	//初始化绘图定时器信号量
	res = sem_init(&semNDrawWave, 0, 0);
	
	//建立绘图定时器线程
	res = pthread_create(&ptNewDrawTimer, NULL, NewThreadProcDrawTimer, NULL);
		
	gbHasCreatedTimer = TRUE;
	return res;
}

//清空波形缓冲区
static int ClearWaveBuffer()
{
	ClearData_Ecg1();
	ClearData_Ecg2();
	ClearData_SpO2();
	ClearData_Resp();
	ClearData_CO2();
	ClearData_Ibp1();
	ClearData_Ibp2();
	printf("%s:%d Clear wave buffer.\n", __FILE__, __LINE__);
	return 0;
}

//////////////////////////////////// interface ///////////////////
/*
	初始化波形绘制
*/
int NewInitWaveDraw()
{
	int res;
	int i;
	
	//禁止绘图
	gbNCanDrawWave = FALSE;
	
//	usleep(200000);
	
#if SCREEN_1024
	//15" 1024
	gfNMMToPixel  = 3.33;	
#elif SCREEN_640
	//12.1" 800x600
	gfNMMToPixel  = 3.2;	
#else
	//12.1" 800x600
	gfNMMToPixel  = 3.2;		
#endif
	
	FillWaveArea(gNRcWavesAll);
	
	//初始化波形配置
	res = InitWaveConfig();
		
	//根据视图和波形配置初始化波形绘制面板
	giPanelCount  = InitWavePanels();
	printf("%s:%d Waveform Config:\n", __FILE__, __LINE__, res);
	for(i=0; i<NWAVEPANEL_MAX_COUNT;  i++){
		printf("Index: %d, WaevID: %d\n", i, gNWavePanels[i].bWaveID);
	}
			
 	gbResetWaves = TRUE;
		
	if(!gbHasCreatedTimer)
		NewCreateDrawWaveTimer();
	
	//清空波形缓冲区
	ClearWaveBuffer();
			
	if(gCfgSystem.bInterface == SCREEN_SHORTTREND)
		NewInitShortTrend();
	else if(gCfgSystem.bInterface == SCREEN_OXYCRG)
		NewInitCrg();
	else if(gCfgSystem.bInterface == SCREEN_OTHERBED)
		NewInitOtherBed();
	
	FillWaveArea(gNRcWavesAll);
	usleep(200000);
	//允许绘图
	gbNCanDrawWave = TRUE;
	
	if(gCfgSystem.bInterface == SCREEN_OTHERBED)
		OtherBedShowBtn();
	if(gCfgSystem.bInterface == SCREEN_IBP)
		UpdateIbp();
	
	return 0;
}

/*
	更新波形绘制
*/
int UpdateDrawWave()
{
	if(gbNCanDrawWave){
		sem_post(&semNDrawWave);
	} 
	else{
		return -1;
	} 
	
	return 0;
}

/*
	改变波形区域的左边界（当有对话框弹出时，向右移动波形）
	prcDlg : NULL－没有对话框，other－对话框所占的区域 
*/
int ChangeWavePanelsLeft(PRECT prcDlg)
{
 	return 0;
	
	static int iLeft = 0;
	
	//禁止绘图
	gbNCanDrawWave = FALSE;
	
// 	FillWaveArea(gNRcWavesAll, giPanelCount);
	FillWaveArea(gNRcWavesAll);
		
	if(prcDlg !=NULL){
		gNRcWaves.left = prcDlg->right +10;
	}
	else{
		if(gCfgSystem.bInterface != SCREEN_SHORTTREND)
			gNRcWaves.left =  RC_NORMAL_LEFT;
		else
			gNRcWaves.left =  RC_SHORTTREND_LEFT;
	}
		
 	SetWavePanelsArea(giPanelCount,gNRcWaves);
	gbResetWaves = TRUE;
		
	//允许绘图
	gbNCanDrawWave = TRUE;
	
	//更新oxyCRG
	if(gCfgSystem.bInterface == SCREEN_OXYCRG){
		NewUpdateCrg();
	}	
	
	return 0;
}

//////////////////一些操作函数/////////////
/*
	改变ECG扫描速度
*/
int ChangeWaveSpeed_Ecg()
{
	int i;
		
	for(i=0; i<giPanelCount; i++){
		if(gNWavePanels[i].bWaveID == NWID_ECG1 
				 ||gNWavePanels[i].bWaveID == NWID_ECG2 
				 ||gNWavePanels[i].bWaveID == NWID_MULTIECG)
			SetWavePanelXY(&(gNWavePanels[i]));
	}
	gbResetWaves = TRUE;
}

/*
	改变SpO2扫描速度
*/
int ChangeWaveSpeed_SpO2()
{
	int i;
		
	for(i=0; i<giPanelCount; i++){
		if(gNWavePanels[i].bWaveID == NWID_SPO2)
			SetWavePanelXY(&(gNWavePanels[i]));
	}
	gbResetWaves = TRUE;
	
}

/*
	改变Resp扫描速度
*/
int ChangeWaveSpeed_Resp()
{
	int i;
		
	for(i=0; i<giPanelCount; i++){
		if(gNWavePanels[i].bWaveID == NWID_RESP)
			SetWavePanelXY(&(gNWavePanels[i]));
	}
	gbResetWaves = TRUE;
}

/*
	改变CO2扫描速度
*/
int ChangeWaveSpeed_CO2()
{
	int i;
		
	for(i=0; i<giPanelCount; i++){
		if(gNWavePanels[i].bWaveID == NWID_CO2)
			SetWavePanelXY(&(gNWavePanels[i]));
	}
	gbResetWaves = TRUE;
}

/*
	改变CO2扫描速度
*/
int ChangeWaveGain_CO2()
{
	int i;
		
	for(i=0; i<giPanelCount; i++){
		if(gNWavePanels[i].bWaveID == NWID_CO2)
			SetWavePanelXY(&(gNWavePanels[i]));
	}
	gbResetWaves = TRUE;
}

int ChangeWaveGain_Ibp1()
{
	int i;
		
	for(i=0; i<giPanelCount; i++){
		if(gNWavePanels[i].bWaveID == NWID_IBP1)
			SetWavePanelXY(&(gNWavePanels[i]));
	}
	gbResetWaves = TRUE;
}

int ChangeWaveGain_Ibp2()
{
	int i;
		
	for(i=0; i<giPanelCount; i++){
		if(gNWavePanels[i].bWaveID == NWID_IBP2)
			SetWavePanelXY(&(gNWavePanels[i]));
	}
	gbResetWaves = TRUE;
}

int ChangeWaveSpeed_Ibp1()
{
	int i;
		
	for(i=0; i<giPanelCount; i++){
		if(gNWavePanels[i].bWaveID == NWID_IBP1)
			SetWavePanelXY(&(gNWavePanels[i]));
	}
	gbResetWaves = TRUE;
}

int ChangeWaveSpeed_Ibp2()
{
	int i;
		
	for(i=0; i<giPanelCount; i++){
		if(gNWavePanels[i].bWaveID == NWID_IBP2)
			SetWavePanelXY(&(gNWavePanels[i]));
	}
	gbResetWaves = TRUE;
}
int ChangeFilter_Ibp1()
{
	iBioCMD_IBP_FILTER(gCfgIbp1.bFilterMode);
	gbResetWaves = TRUE;
}

int ChangeFilter_Ibp2()
{
	iBioCMD_IBP_FILTER(gCfgIbp2.bFilterMode);
	gbResetWaves = TRUE;
}


/*
	切换波形显示
*/
int SwitchView()
{
	static BYTE bOldInterface;
	ReturnMainView();
	//清屏
	SetBrushColor(HDC_SCREEN, COLOR_black);
	FillBox(HDC_SCREEN, 
		gNRcWaves.left, 
		gNRcWaves.top, 
		RECTW(gNRcWaves), 
		RECTH(gNRcWaves));
	bOldInterface=gCfgSystem.bInterface;
	
	gCfgSystem.bInterface=(gCfgSystem.bInterface+1)%SCREEN_12LEAD;
	
	
	SetIntValueToResFile(gFileSetup, "SystemSetup", "interface", gCfgSystem.bInterface, 1);	
	printf("gCfgSystem.bInterface=%d\n",gCfgSystem.bInterface);
							
	//新的视图
	NewDestroyShortTrend();
	NewDestroyCrg();
	NewDestroyOtherBed();
	SwitchParasView(bOldInterface);
	NewInitWaveDraw();
	
	ChangeECGLeadChannel();
	bOldInterface=gCfgSystem.bInterface;
	
	return 0;
}

//冻结波形
int FreezeWaves()
{
	gbNCanDrawWave = FALSE;
	
	//启动屏幕回放
	StartScreenReview();
	
	return 0;
}

//解冻波形
int UnfreezeWaves()
{
	//回到主界面
	ReturnMainView();
	
	gbNCanDrawWave = TRUE;
	
	return 0;
}
/*
	改变Y轴上的系数,如增益
*/
int ChangeWaveY()
{
	int i;
		
	for(i=0; i<WAVEPANEL_MAX_COUNT; i++){
		if(gWavePanels[i].hWnd != (HWND)NULL){
			SetWaveY(&gWavePanels[i]);
		}
	}	
	
	return 0;
}


/*
	改变Ibp1的波形范围
*/
int ChangeWaveRange_Ibp1()
{
	int i;
		
	for(i=0; i<WAVEPANEL_MAX_COUNT; i++){
		if(gWavePanels[i].hWnd != (HWND)NULL 
				 && gWavePanels[i].bID == WID_IBP1){
			SetWaveY(&gWavePanels[i]);
		 }
	}	
	
	return 0;
}

/*
	改变Ibp2的波形范围
*/
int ChangeWaveRange_Ibp2()
{
	int i;
		
	for(i=0; i<WAVEPANEL_MAX_COUNT; i++){
		if(gWavePanels[i].hWnd != (HWND)NULL 
				 && gWavePanels[i].bID == WID_IBP2){
			SetWaveY(&gWavePanels[i]);
				 }
	}	
	
	return 0;
}


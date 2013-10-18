/*************************************************************************** 
 *	Module Name:	Init_Funcs
 *
 *	Abstract:	程序初始化时调用的函数
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-05-23 10:46:39
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Init_Funcs.h"

//用户对波形面板的配置, 适用于:  Normal, Short Trend, Nibp List
extern WAVE_PANEL_CFG gWaveCfg_User[WAVEPANEL_MAX_COUNT];

/*
	加载机器和系统配置文件
*/
static int LoadConfigFiles()
{
	int res;
	
	//TODO:此处要对文件的存在与完整性进行检查
	
	//加载机器配置文件	
	snprintf(gFileMachineConfig, sizeof gFileMachineConfig, CFGPATH_MACHINE);
	
	//根据机器的软件配置，加载相应的系统配置文件
	GetIntValueFromEtcFile(gFileMachineConfig, "SOFTWARE", "setup_file", &res);
	
	switch(res){
		case SETUP_ADULT:{
			snprintf(gFileSetup, sizeof gFileSetup, CFGPATH_ADULT);
		}break;
		case SETUP_CHILD:{
			snprintf(gFileSetup, sizeof gFileSetup, CFGPATH_CHILD);
		}break;
		case SETUP_BABY:{
			snprintf(gFileSetup, sizeof gFileSetup, CFGPATH_BABY);
		}break;
		case SETUP_USER1:{
			snprintf(gFileSetup, sizeof gFileSetup, CFGPATH_USER1);
		}break;
		case SETUP_USER2:{
			snprintf(gFileSetup, sizeof gFileSetup, CFGPATH_USER2);
		}break;
		case SETUP_USER3:{
			snprintf(gFileSetup, sizeof gFileSetup, CFGPATH_USER3);
		}break;
		default:{//SETUP_RECENT
			snprintf(gFileSetup, sizeof gFileSetup, CFGPATH_RECENT);
		}break;
	}
	
	if(B_PRINTF)printf("(%s:%d)Config File is %d -- %s.\n", __FILE__, __LINE__, res, gFileSetup);	
	
	return 0;
}                            


/*
	初始化机器模块配置信息
	输入:                                              
		CfgFile : 机器配置文件
		psModules: 模块配置信息
	输出:
		0 : OK,  -1 : Failure	
*/
static int InitModulesSet(const unsigned char * CfgFile, S_MODULES *psModules)
{
	int iCfgValue=0;
	int iCount=0;
	
	GetIntValueFromEtcFile(CfgFile, "HARDWARE", "ecg", &iCfgValue);
	printf("(%s:%d)ECG module is %d.\n", __FILE__, __LINE__, iCfgValue);	
	psModules->s_Modules[0].bID = MOD_ECG;
	psModules->s_Modules[0].bStatus = iCfgValue;
	if(iCfgValue == 1) iCount += 1;
	
	GetIntValueFromEtcFile(CfgFile, "HARDWARE", "nibp", &iCfgValue);
	printf("(%s:%d)NIBP module is %d.\n", __FILE__, __LINE__, iCfgValue);	
	psModules->s_Modules[1].bID = MOD_NIBP;
	psModules->s_Modules[1].bStatus = iCfgValue;
	if(iCfgValue == 1) iCount += 1;
	
	GetIntValueFromEtcFile(CfgFile, "HARDWARE", "spo2", &iCfgValue);
	printf("(%s:%d)SpO2 module is %d.\n", __FILE__, __LINE__, iCfgValue);	
	psModules->s_Modules[2].bID = MOD_SPO2;
	psModules->s_Modules[2].bStatus = iCfgValue;
	if(iCfgValue == 1) iCount += 1;
	
	GetIntValueFromEtcFile(CfgFile, "HARDWARE", "pulse", &iCfgValue);
	printf("(%s:%d)PULSE module is %d.\n", __FILE__, __LINE__, iCfgValue);	
	psModules->s_Modules[3].bID = MOD_PULSE;
	psModules->s_Modules[3].bStatus = iCfgValue;
	if(iCfgValue == 1) iCount += 1;
	
	GetIntValueFromEtcFile(CfgFile, "HARDWARE", "resp", &iCfgValue);
	printf("(%s:%d)RESP module is %d.\n", __FILE__, __LINE__, iCfgValue);	
	psModules->s_Modules[4].bID = MOD_RESP;
	psModules->s_Modules[4].bStatus = iCfgValue;
	if(iCfgValue == 1) iCount += 1;
	
	GetIntValueFromEtcFile(CfgFile, "HARDWARE", "temp", &iCfgValue);
	printf("(%s:%d)TEMP1 module is %d.\n", __FILE__, __LINE__, iCfgValue);	
	psModules->s_Modules[5].bID = MOD_TEMP;
	psModules->s_Modules[5].bStatus = iCfgValue;
	if(iCfgValue == 1) iCount += 1;
	
	GetIntValueFromEtcFile(CfgFile, "HARDWARE", "ibp1", &iCfgValue);
	printf("(%s:%d)IBP1 module is %d.\n", __FILE__, __LINE__, iCfgValue);	
	psModules->s_Modules[7].bID = MOD_IBP1;
	psModules->s_Modules[7].bStatus = iCfgValue;
	if(iCfgValue == 1) iCount += 1;
	
	GetIntValueFromEtcFile(CfgFile, "HARDWARE", "ibp2", &iCfgValue);
	printf("(%s:%d)IBP2 module is %d.\n", __FILE__, __LINE__, iCfgValue);	
	psModules->s_Modules[8].bID = MOD_IBP2;
	psModules->s_Modules[8].bStatus = iCfgValue;
	if(iCfgValue == 1) iCount += 1;

	
	//判断是否有NIBP模块
	if(psModules->s_Modules[1].bStatus == 1){
		gbHaveNIBP = TRUE;
		gCfgNibp.bSwitch=0x3;
	}
	else{
		gbHaveNIBP = FALSE;
		gCfgNibp.bSwitch |=0x0;
	}
	//判断是否有Spo2模块
	if(psModules->s_Modules[2].bStatus == 1){
		gbHaveSpo2= TRUE;
		gCfgSpO2.bSwitch=0x3;
		
	}
	else{
		gbHaveSpo2 = FALSE;
		gCfgSpO2.bSwitch |=0x0;
	}
	//判断是否有Resp模块
	if(psModules->s_Modules[4].bStatus == 1){
		gbHaveResp= TRUE;
		gCfgResp.bSwitch=0x3;
	}
	else{
		gbHaveResp = FALSE;
		gCfgResp.bSwitch |=0x0;
	}
		//判断是否有Temp模块
	if(psModules->s_Modules[5].bStatus == 1){
		gbHaveTemp= TRUE;
		gCfgTemp.bSwitch=0x3;
	}
	else{
		gbHaveTemp = FALSE;
		gCfgTemp.bSwitch |=0x0;
	}
	//判断是否有IBP模块
	if(psModules->s_Modules[7].bStatus == 1 
		  ||psModules->s_Modules[8].bStatus ==1){
		gbHaveIbp = TRUE;	
		gCfgIbp1.bSwitch |=0x3;//bit0=1 模块有bit1=1波形有
		gCfgIbp2.bSwitch |=0x3;
	}
	else{
		gbHaveIbp = FALSE;
		gCfgIbp1.bSwitch |=0x0;
		gCfgIbp2.bSwitch |=0x0;
	}
	
	GetIntValueFromEtcFile(CfgFile, "HARDWARE", "co2", &iCfgValue);
	printf("(%s:%d)CO2 module is %d.\n", __FILE__, __LINE__, iCfgValue);	
	psModules->s_Modules[9].bID = MOD_CO2;
	psModules->s_Modules[9].bStatus = iCfgValue;
	
	//判断是否有CO2模块
	if(psModules->s_Modules[9].bStatus == 1){
		gbHaveCo2= TRUE;	
		gCfgCO2.bSwitch |=0x3;
	}
	else{
		gbHaveCo2= FALSE;
		gCfgCO2.bSwitch |=0x0;
	}
	
	//判断是否有Nellcor SpO2
	GetIntValueFromEtcFile(CfgFile, "HARDWARE", "nellcor", &iCfgValue);
	printf("(%s:%d)Nellcor  module is %d.\n", __FILE__, __LINE__, iCfgValue);
	if(1==iCfgValue) gbHaveNellSpO2 = TRUE;
	else gbHaveNellSpO2 = FALSE;
	
	
	psModules->bCount = iCount;
	printf("(%s:%d)used module is %d.\n", __FILE__, __LINE__, iCount);	
	
	return 0;
}


/*******************************************
*	函数名:		InitCfg_xxx()
*	输入:		
*		pCfgFiles : 配置文件
		pConfig : 配置信息
*	输出:	
		-1: Error
		0: OK
*	功能描述：	初始化模块配置信息
*	作者:		
*	日期:		
********************************************/
static int InitCfg_ECG(const unsigned char *pCfgFiles, PS_ECG_CFG  pConfig)
{
	int iCfgValue;
	int res;        
	
	if((pCfgFiles == NULL) || (pConfig ==NULL) ) return -1;
	
	pConfig->bID = MOD_ECG;
	
	//Switch
	res = GetIntValueFromEtcFile(pCfgFiles, "ECGSetup", "switch", &iCfgValue);
	if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_ON;	
	pConfig->bSwitch = iCfgValue;
	//Color
	res = GetIntValueFromEtcFile(pCfgFiles, "ECGSetup", "color", &iCfgValue);
	if(iCfgValue <0 || iCfgValue >255) iCfgValue = COLOR_green;	
	pConfig->iColor = iCfgValue;	
	//导联类型
	res = GetIntValueFromEtcFile(pCfgFiles, "ECGSetup", "lead_type", &iCfgValue);
	if(iCfgValue <ECGTYPE_5LEAD || iCfgValue > ECGTYPE_3LEAD) iCfgValue = ECGTYPE_5LEAD;		
	pConfig->bLeadType= iCfgValue;
	//扫描速度
	res = GetIntValueFromEtcFile(pCfgFiles, "ECGSetup", "scan_speed", &iCfgValue);
	if(iCfgValue <SCANSPEED_625 || iCfgValue > SCANSPEED_50) iCfgValue = SCANSPEED_25;		
	pConfig->bScanSpeed= iCfgValue;
	
	//Pacemaker TODO:此处要判断是否和病人信息中的设置一致, 以病人信息为准
	res = GetIntValueFromEtcFile(pCfgFiles, "PatientSetup", "pacemaker", &iCfgValue);
	if(iCfgValue == PACEMAKER_OFF){
		pConfig->bPacemaker = PACEMAKER_OFF;
	} 
	else{
		//如果是起搏病人,起搏检测功能可选
		res = GetIntValueFromEtcFile(pCfgFiles, "ECGSetup", "pacemaker", &iCfgValue);
		if(iCfgValue <PACEMAKER_OFF || iCfgValue > PACEMAKER_MODE2) 
			iCfgValue = PACEMAKER_OFF;		
		pConfig->bPacemaker = iCfgValue;
	}

	//显示通道1
	res = GetIntValueFromEtcFile(pCfgFiles, "ECGSetup", "channel1", &iCfgValue);
	if(iCfgValue <ECGWAVE_I || iCfgValue >ECGWAVE_V) iCfgValue = ECGWAVE_II;		
	pConfig->bChannel1= iCfgValue;
	//显示通道2
	res = GetIntValueFromEtcFile(pCfgFiles, "ECGSetup", "channel2", &iCfgValue);
	if(iCfgValue <ECGWAVE_I || iCfgValue >ECGWAVE_V) iCfgValue = ECGWAVE_I;		
	pConfig->bChannel2= iCfgValue;
	//波形级联
	res = GetIntValueFromEtcFile(pCfgFiles, "ECGSetup", "cascade", &iCfgValue);
	if(iCfgValue <CASCADE_OFF || iCfgValue >CASCADE_ON) iCfgValue = CASCADE_ON;		
	pConfig->bCascade= iCfgValue;
	//工作模式
	res = GetIntValueFromEtcFile(pCfgFiles, "ECGSetup", "mode", &iCfgValue);
	if(iCfgValue <ECGMODE_DIAG || iCfgValue >ECGMODE_OPE) iCfgValue = ECGMODE_DIAG;		
	pConfig->bMode= iCfgValue;
	//交流滤波
	res = GetIntValueFromEtcFile(pCfgFiles, "ECGSetup", "filter_ac", &iCfgValue);
	if(iCfgValue <ECGFILTER_AC_OFF || iCfgValue >ECGFILTER_AC_ON) iCfgValue = ECGFILTER_AC_OFF;		
	pConfig->bFilterAC = iCfgValue;
	//基线漂移滤波
	res = GetIntValueFromEtcFile(pCfgFiles, "ECGSetup", "filter_drift", &iCfgValue);
	if(iCfgValue <ECGFILTER_DRIFT_OFF || iCfgValue >ECGFILTER_DRIFT_ON) iCfgValue = ECGFILTER_DRIFT_OFF;		
	pConfig->bFilterDrift= iCfgValue;
	//肌电滤波
	res = GetIntValueFromEtcFile(pCfgFiles, "ECGSetup", "filter_hum", &iCfgValue);
	if(iCfgValue <ECGFILTER_HUM_OFF || iCfgValue >ECGFILTER_HUM_ON) iCfgValue = ECGFILTER_HUM_OFF;		
	pConfig->bFilterHum= iCfgValue;	
	
	//波形增益
	res = GetIntValueFromEtcFile(pCfgFiles, "ECGSetup", "gain", &iCfgValue);
	if(iCfgValue <ECGGAIN_025X || iCfgValue >ECGGAIN_2X) iCfgValue = ECGGAIN_1X;		
	pConfig->bGain = iCfgValue;
	//心率高限
	res = GetIntValueFromEtcFile(pCfgFiles, "ECGSetup", "hr_high", &iCfgValue);
	if(iCfgValue <HR_MIN || iCfgValue >HR_MAX) iCfgValue = 120;		
	pConfig->wHigh_HR = iCfgValue;
	//心率低限
	res = GetIntValueFromEtcFile(pCfgFiles, "ECGSetup", "hr_low", &iCfgValue);
	if(iCfgValue <HR_MIN || iCfgValue >HR_MAX) iCfgValue = 50;		
	pConfig->wLow_HR = iCfgValue;
	//Beep音量	                            
	res = GetIntValueFromEtcFile(pCfgFiles, "ECGSetup", "beep_volume", &iCfgValue);
	if(iCfgValue <VOLUME_OFF || iCfgValue >VOLUME_10) iCfgValue = VOLUME_2;		
	pConfig->bBeepVolume = iCfgValue;
	//心率报警来源                                   
	res = GetIntValueFromEtcFile(pCfgFiles, "ECGSetup", "alarm_source", &iCfgValue);
	if(iCfgValue <HR_ALM_SRC_AUTO || iCfgValue >HR_ALM_SRC_PR) iCfgValue = HR_ALM_SRC_AUTO;		
	pConfig->bAlmSource = iCfgValue;
	//心率报警控制 
	res = GetIntValueFromEtcFile(pCfgFiles, "ECGSetup", "alarm_control", &iCfgValue);
	if(iCfgValue <ALARM_OFF || iCfgValue >ALARM_HIGH) iCfgValue = ALARM_MID;	
	pConfig->bAlmControl = iCfgValue;
	//心率报警打印
	res = GetIntValueFromEtcFile(pCfgFiles, "ECGSetup", "alarm_print", &iCfgValue);
	if(iCfgValue <ALARM_PRINT_OFF || iCfgValue >ALARM_PRINT_ON) iCfgValue = ALARM_PRINT_OFF;	
	pConfig->bPrnControl = iCfgValue;
// 	//校准系数
// 	res = GetIntValueFromEtcFile(pCfgFiles, "ECGSetup", "adjust", &iCfgValue);
// 	pConfig->iAdjust = iCfgValue;

	//ARR analyse 开关
	res = GetIntValueFromEtcFile(pCfgFiles, "ECGSetup", "arr_analyse", &iCfgValue);
	if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_OFF;	
	pConfig->bArrAnalyse= iCfgValue;
	
	//PVCs 开关
	res = GetIntValueFromEtcFile(pCfgFiles, "ECGSetup", "pvcs", &iCfgValue);
	if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_OFF;	
	pConfig->bPVCs= iCfgValue;

	res = GetIntValueFromEtcFile(pCfgFiles, "ECGSetup", "st1_high", &iCfgValue);
	if(iCfgValue <ST_MIN || iCfgValue >ST_MAX) iCfgValue = 50;		
	pConfig->iSt1_High = iCfgValue;
	
	res = GetIntValueFromEtcFile(pCfgFiles, "ECGSetup", "st1_low", &iCfgValue);
	if(iCfgValue <ST_MIN || iCfgValue >ST_MAX) iCfgValue = -50;		
	pConfig->iSt1_Low= iCfgValue;
	
	res = GetIntValueFromEtcFile(pCfgFiles, "ECGSetup", "st2_high", &iCfgValue);
	if(iCfgValue <ST_MIN || iCfgValue >ST_MAX) iCfgValue = 50;		
	pConfig->iSt2_High = iCfgValue;
	
	res = GetIntValueFromEtcFile(pCfgFiles, "ECGSetup", "st2_low", &iCfgValue);
	if(iCfgValue <ST_MIN || iCfgValue >ST_MAX) iCfgValue = -50;		
	pConfig->iSt2_Low= iCfgValue;
	
	res = GetIntValueFromEtcFile(pCfgFiles, "ECGSetup", "ios_value", &iCfgValue);
	if(iCfgValue <0 || iCfgValue >400) iCfgValue = 50;		
	pConfig->bISO= iCfgValue;
	
	res = GetIntValueFromEtcFile(pCfgFiles, "ECGSetup", "st_value", &iCfgValue);
	if(iCfgValue <0 || iCfgValue >400) iCfgValue = -50;		
	pConfig->bST= iCfgValue;
	
	res = GetIntValueFromEtcFile(pCfgFiles, "ECGSetup", "max_hr", &iCfgValue);
	if(iCfgValue <ST_MIN || iCfgValue >300) iCfgValue = 300;		
	pConfig->iMax_HR= iCfgValue;
	
	res = GetIntValueFromEtcFile(pCfgFiles, "ECGSetup", "min_hr", &iCfgValue);
	if(iCfgValue <ST_MIN || iCfgValue >160) iCfgValue = 0;		
	pConfig->iMin_HR= iCfgValue;
	
	return 0;
}

static int InitValue_ECG(PS_ECG_VALUE pValue)
{
	int i;
	
	if(pValue == NULL) return -1;
	
	pValue->bID = MOD_ECG;
	pValue->bConnected = FALSE;
	pValue->bCommStatus = COMMUNICATION_OK;
	pValue->bAdjust = FALSE;
	pValue->bLeadType = ECGTYPE_5LEAD;
	pValue->wHR = 0;
	pValue->bBeep = FALSE;
	pValue->bPacingpulse = 0;
	for(i=0; i<ECGWAVE_COUNT; i++){
		pValue->iWaves[i]= 0;
	}
	pValue->bRAOff = TRUE;
	pValue->bLAOff = TRUE;
	pValue->bLLOff = TRUE;
	pValue->bVOff = TRUE;
	pValue->iST1 = 0;
	pValue->iST2 = 0;
			
	return 0;
}
		
static int InitCfg_Resp(const unsigned char *pCfgFiles, PS_RESP_CFG  pConfig)
{
	int iCfgValue;
	int res;
	
	if((pCfgFiles == NULL) || (pConfig ==NULL) ) return -1;
	
	pConfig->bID = MOD_RESP;
	
	//Switch
	res = GetIntValueFromEtcFile(pCfgFiles, "RESPSetup", "switch", &iCfgValue);
	if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_ON;	
	pConfig->bSwitch = iCfgValue;
	//Color
	res = GetIntValueFromEtcFile(pCfgFiles, "RESPSetup", "color", &iCfgValue);
	if(iCfgValue <0 || iCfgValue >255) iCfgValue = COLOR_lightgray;	
	pConfig->iColor = iCfgValue;	
	//Gain
	res = GetIntValueFromEtcFile(pCfgFiles, "RESPSetup", "gain", &iCfgValue);
	if(iCfgValue <RESPGAIN_025X || iCfgValue >RESPGAIN_32X) iCfgValue = RESPGAIN_1X;	
	pConfig->bGain = iCfgValue;	
	//Scan Speed 
	res = GetIntValueFromEtcFile(pCfgFiles, "RESPSetup", "scan_speed", &iCfgValue);
	if(iCfgValue <SCANSPEED_625|| iCfgValue >SCANSPEED_25) iCfgValue = SCANSPEED_125;	
	pConfig->bScanSpeed = iCfgValue;	
	//RR High
	res = GetIntValueFromEtcFile(pCfgFiles, "RESPSetup", "rr_high", &iCfgValue);
	if(iCfgValue <RR_MIN|| iCfgValue >RR_MAX) iCfgValue = 30;	
	pConfig->wHigh_RR = iCfgValue;	
	//RR Low
	res = GetIntValueFromEtcFile(pCfgFiles, "RESPSetup", "rr_low", &iCfgValue);
	if(iCfgValue <RR_MIN|| iCfgValue >RR_MAX) iCfgValue = 8;	
	pConfig->wLow_RR = iCfgValue;	
	//short trend max_rr
	res = GetIntValueFromEtcFile(pCfgFiles, "RESPSetup", "max_rr", &iCfgValue);
	if(iCfgValue <RR_MIN|| iCfgValue >RR_MAX) iCfgValue = 100;	
	pConfig->bMax_RR= iCfgValue;	
	//short trend min_rr
	res = GetIntValueFromEtcFile(pCfgFiles, "RESPSetup", "min_rr", &iCfgValue);
	if(iCfgValue <RR_MIN|| iCfgValue >RR_MAX) iCfgValue = 0;	
	pConfig->bMin_RR= iCfgValue;	
	
	//Apnea Time
	res = GetIntValueFromEtcFile(pCfgFiles, "RESPSetup", "apnea_time", &iCfgValue);
	if(iCfgValue <APNEATIME_OFF|| iCfgValue >APNEATIME_MAX) iCfgValue = APNEATIME_OFF;	
	pConfig->wApneaTime = iCfgValue;	
	
	//Apnea
	res = GetIntValueFromEtcFile(pCfgFiles, "RESPSetup", "apnea", &iCfgValue);
	if(iCfgValue <APNEATIME_OFF|| iCfgValue >7) iCfgValue = APNEATIME_OFF;	
	pConfig->bApnea= iCfgValue;
	
	//Alarm Control
	res = GetIntValueFromEtcFile(pCfgFiles, "RESPSetup", "alarm_control", &iCfgValue);
	if(iCfgValue <ALARM_OFF|| iCfgValue >ALARM_HIGH) iCfgValue = ALARM_MID;	
	pConfig->bAlmControl = iCfgValue;	
	//Alarm Print Control
	res = GetIntValueFromEtcFile(pCfgFiles, "RESPSetup", "alarm_print", &iCfgValue);
	if(iCfgValue < ALARM_PRINT_OFF|| iCfgValue > ALARM_PRINT_ON) iCfgValue = ALARM_PRINT_OFF;	
	pConfig->bPrnControl = iCfgValue;
	//Lead type
	res = GetIntValueFromEtcFile(pCfgFiles, "RESPSetup", "leadtype", &iCfgValue);
	if(iCfgValue < RR_LEADTYPE_OFF|| iCfgValue > RR_LEADTYPE2) iCfgValue = RR_LEADTYPE2;	
	pConfig->bLeadType= iCfgValue;
	//Source
	res = GetIntValueFromEtcFile(pCfgFiles, "RESPSetup", "source", &iCfgValue);
	if(!gbHaveCo2){
		pConfig->bSource= RR_SRC_ECG;	
	}
	else{
		pConfig->bSource = iCfgValue;	
	}
	

	return 0;    
}

static int InitValue_Resp(PS_RESP_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	pValue->bID = MOD_RESP;
	pValue->bConnected = FALSE;
	pValue->bCommStatus = COMMUNICATION_OK;
	pValue->wRR = 0; 
	pValue->iWave = 0;
	pValue->bOff = TRUE;

	return 0;
}

static int InitCfg_SpO2(const unsigned char *pCfgFiles, PS_SPO2_CFG  pConfig)
{
	int iCfgValue;
	int res;
	
	if((pCfgFiles == NULL) || (pConfig ==NULL) ) return -1;
	
	pConfig->bID = MOD_SPO2;
	
	//Switch
	res = GetIntValueFromEtcFile(pCfgFiles, "SPO2Setup", "switch", &iCfgValue);
	if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_ON;	
	pConfig->bSwitch = iCfgValue;
	//Color
	res = GetIntValueFromEtcFile(pCfgFiles, "SPO2Setup", "color", &iCfgValue);
	if(iCfgValue <0 || iCfgValue >255) iCfgValue = COLOR_cyan;	
	pConfig->iColor = iCfgValue;	
	//Response
	res = GetIntValueFromEtcFile(pCfgFiles, "SPO2Setup", "response", &iCfgValue);
	if(iCfgValue <SPO2_FAST || iCfgValue >SPO2_SLOW) iCfgValue = SPO2_NORMAL;	
	pConfig->bResponse = iCfgValue;	
	//Gain
	res = GetIntValueFromEtcFile(pCfgFiles, "SPO2Setup", "gain", &iCfgValue);
	if(iCfgValue <SPO2GAIN_AUTO || iCfgValue >SPO2GAIN_2X) iCfgValue = SPO2GAIN_AUTO;	
	pConfig->bGain = iCfgValue;	
	//Scan Speed
	res = GetIntValueFromEtcFile(pCfgFiles, "SPO2Setup", "scan_speed", &iCfgValue);
	if(iCfgValue <SCANSPEED_125 || iCfgValue >SCANSPEED_50) iCfgValue = SCANSPEED_25;	
	pConfig->bScanSpeed = iCfgValue;	
	//Filter 
	res = GetIntValueFromEtcFile(pCfgFiles, "SPO2Setup", "filter", &iCfgValue);
	if(iCfgValue <SPO2FILTER_OFF || iCfgValue >SPO2FILTER_ON) iCfgValue = SPO2FILTER_ON;	
	pConfig->bFilter = iCfgValue;	
	//Draw Mode
	res = GetIntValueFromEtcFile(pCfgFiles, "SPO2Setup", "draw_mode", &iCfgValue);
	if(iCfgValue < DRAWMODE_LINE || iCfgValue >DRAWMODE_FILL) iCfgValue = DRAWMODE_LINE;	
	pConfig->bDrawMode = iCfgValue;	
	//SpO2 High
	res = GetIntValueFromEtcFile(pCfgFiles, "SPO2Setup", "spo2_high", &iCfgValue);
	if(iCfgValue <SPO2_MIN|| iCfgValue >SPO2_MAX) iCfgValue = 100;	
	pConfig->bHigh_SpO2 = iCfgValue;	
	//SpO2 Low
	res = GetIntValueFromEtcFile(pCfgFiles, "SPO2Setup", "spo2_low", &iCfgValue);
	if(iCfgValue <SPO2_MIN|| iCfgValue >SPO2_MAX) iCfgValue = 90;	
	pConfig->bLow_SpO2 = iCfgValue;
	//SpO2 Lowest
	res = GetIntValueFromEtcFile(pCfgFiles, "SPO2Setup", "spo2_lowest", &iCfgValue);
	if(iCfgValue <SPO2_MIN|| iCfgValue >SPO2_MAX) iCfgValue = 80;	
	pConfig->bLowest_SpO2 = iCfgValue;
	//Alarm Control
	res = GetIntValueFromEtcFile(pCfgFiles, "SPO2Setup", "alarm_control", &iCfgValue);
	if(iCfgValue <ALARM_OFF || iCfgValue >ALARM_HIGH) iCfgValue = ALARM_MID;	
	pConfig->bAlmControl = iCfgValue;
	//Alarm Print Control
	res = GetIntValueFromEtcFile(pCfgFiles, "SPO2Setup", "alarm_print", &iCfgValue);
	if(iCfgValue <ALARM_PRINT_OFF || iCfgValue >ALARM_PRINT_ON) iCfgValue = ALARM_PRINT_OFF;	
	pConfig->bPrnControl = iCfgValue;
	//short trend max spo2
	res = GetIntValueFromEtcFile(pCfgFiles, "SPO2Setup", "max_spo2", &iCfgValue);
	if(iCfgValue <SPO2_MIN || iCfgValue >SPO2_MAX) iCfgValue = 100;	
	pConfig->bMax_SPO2= iCfgValue;
	//short trend minspo2
	res = GetIntValueFromEtcFile(pCfgFiles, "SPO2Setup", "min_spo2", &iCfgValue);
	if(iCfgValue <SPO2_MIN || iCfgValue >SPO2_MAX) iCfgValue = 0;	
	pConfig->bMin_SPO2= iCfgValue;
	//校准系数
// 	res = GetIntValueFromEtcFile(pCfgFiles, "SPO2Setup", "adjust", &iCfgValue);
// 	pConfig->iAdjust = iCfgValue;
	
	return 0;
}

static int InitValue_SpO2(PS_SPO2_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	pValue->bID = MOD_SPO2;
	pValue->bConnected = FALSE;
	pValue->bSpO2 = 127;	//无效值 
	pValue->bWave = 0;
	pValue->bBeep = FALSE;
	pValue->bStrength = 0;
	pValue->bComError = FALSE;
	pValue->bComStop = FALSE;
	pValue->bModuleInitError = FALSE;
	pValue->bModuleError = FALSE;
	pValue->bLimitError = FALSE;
	pValue->bOverrange = FALSE;
	pValue->bNoSensor = FALSE;
	pValue->bSensorError = FALSE;
	pValue->bUnknownSensor = FALSE;
	pValue->bIncompatible = FALSE;
	pValue->bOff = TRUE;
	pValue->bErrCode = 0;	
	
	return 0;
}

static int InitCfg_Pulse(const unsigned char *pCfgFiles, PS_PULSE_CFG  pConfig)
{
	int iCfgValue;
	int res;
	
	if((pCfgFiles == NULL) || (pConfig ==NULL) ) return -1;
	
	pConfig->bID = MOD_PULSE;
	
	//Switch
	res = GetIntValueFromEtcFile(pCfgFiles, "PULSESetup", "switch", &iCfgValue);
	if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_ON;	
	pConfig->bSwitch = iCfgValue;
	//Color 
	res = GetIntValueFromEtcFile(pCfgFiles, "PULSESetup", "color", &iCfgValue);
	if(iCfgValue <0 || iCfgValue >255) iCfgValue = COLOR_cyan;	
	pConfig->iColor = iCfgValue;	
	//Beep Volume
	res = GetIntValueFromEtcFile(pCfgFiles, "PULSESetup", "beep_volume", &iCfgValue);
	if(iCfgValue <VOLUME_OFF || iCfgValue >VOLUME_10) iCfgValue = VOLUME_2;	
	pConfig->bBeepVolume = iCfgValue;	
	//PR High
	res = GetIntValueFromEtcFile(pCfgFiles, "PULSESetup", "pr_high", &iCfgValue);
	if(iCfgValue <PR_MIN|| iCfgValue >PR_MAX) iCfgValue = 120;	
	pConfig->wHigh_PR = iCfgValue;	
	//PR Low
	res = GetIntValueFromEtcFile(pCfgFiles, "PULSESetup", "pr_low", &iCfgValue);
	if(iCfgValue <PR_MIN|| iCfgValue >PR_MAX) iCfgValue = 50;	
	pConfig->wLow_PR = iCfgValue;	
	//Alarm Source: 与ECG的报警源同步
	res = GetIntValueFromEtcFile(pCfgFiles, "ECGSetup", "alarm_source", &iCfgValue);
	if(iCfgValue <HR_ALM_SRC_AUTO || iCfgValue >HR_ALM_SRC_PR) iCfgValue = HR_ALM_SRC_AUTO;	
	pConfig->bAlmSource = iCfgValue;	
	//Alarm Control
	res = GetIntValueFromEtcFile(pCfgFiles, "PULSESetup", "alarm_control", &iCfgValue);
	if(iCfgValue <ALARM_OFF || iCfgValue >ALARM_HIGH) iCfgValue = ALARM_MID;	
	pConfig->bAlmControl = iCfgValue;	
	//Alarm Print Control
	res = GetIntValueFromEtcFile(pCfgFiles, "PULSESetup", "alarm_print", &iCfgValue);
	if(iCfgValue <ALARM_PRINT_OFF || iCfgValue >ALARM_PRINT_ON) iCfgValue = ALARM_PRINT_OFF;	
	pConfig->bPrnControl = iCfgValue;	

	return 0;
}

static int InitValue_Pulse(PS_PULSE_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	pValue->bID = MOD_PULSE;
	pValue->bConnected = FALSE;
	pValue->bCommStatus = COMMUNICATION_OK;
	pValue->wPR = 0;
	pValue->bOff = TRUE;

	return 0;
}


static int InitCfg_Temp(const unsigned char *pCfgFiles, PS_TEMP_CFG pConfig)
{
	int iCfgValue;
	int res;
	
	if((pCfgFiles == NULL) || (pConfig ==NULL) ) return -1;
	
	pConfig->bID = MOD_PULSE;
	
	//Switch
	res = GetIntValueFromEtcFile(pCfgFiles, "TEMPSetup", "switch", &iCfgValue);
	if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_ON;	
	pConfig->bSwitch = iCfgValue;
	//Color 
	res = GetIntValueFromEtcFile(pCfgFiles, "TEMPSetup", "color", &iCfgValue);
	if(iCfgValue <0 || iCfgValue >255) iCfgValue = COLOR_yellow;	
	pConfig->iColor = iCfgValue;	
	//Unit
	res = GetIntValueFromEtcFile(pCfgFiles, "TEMPSetup", "unit", &iCfgValue);
	if(iCfgValue <TEMP_UNIT_C || iCfgValue >TEMP_UNIT_F) iCfgValue = TEMP_UNIT_C;	
	pConfig->bUnit = iCfgValue;	
	//T1 High
	res = GetIntValueFromEtcFile(pCfgFiles, "TEMPSetup", "t1_high", &iCfgValue);
// 	if(iCfgValue <TEMP_MIN || iCfgValue >TEMP_MAX) iCfgValue = 390;	
	pConfig->wHigh_T1= iCfgValue;	
	//T1 Low
	res = GetIntValueFromEtcFile(pCfgFiles, "TEMPSetup", "t1_low", &iCfgValue);
	if(iCfgValue <TEMP_MIN || iCfgValue >TEMP_MAX) iCfgValue = 360;	
	pConfig->wLow_T1= iCfgValue;	
	//T2 High
	res = GetIntValueFromEtcFile(pCfgFiles, "TEMPSetup", "t2_high", &iCfgValue);
// 	if(iCfgValue <TEMP_MIN || iCfgValue >TEMP_MAX) iCfgValue = 390;	
	pConfig->wHigh_T2= iCfgValue;	
	//T2 Low
	res = GetIntValueFromEtcFile(pCfgFiles, "TEMPSetup", "t2_low", &iCfgValue);
// 	if(iCfgValue <TEMP_MIN || iCfgValue >TEMP_MAX) iCfgValue = 360;	
	pConfig->wLow_T2= iCfgValue;	
	//TD High
	res = GetIntValueFromEtcFile(pCfgFiles, "TEMPSetup", "td_high", &iCfgValue);
// 	if(iCfgValue <TEMP_MIN || iCfgValue >TEMP_MAX) iCfgValue = 390;	
	pConfig->wHigh_TD= iCfgValue;	
	//Alarm Control
	res = GetIntValueFromEtcFile(pCfgFiles, "TEMPSetup", "alarm_control", &iCfgValue);
	if(iCfgValue <ALARM_OFF || iCfgValue >ALARM_HIGH) iCfgValue = ALARM_MID;	
	pConfig->bAlmControl = iCfgValue;	
	//Alarm Print Control
	res = GetIntValueFromEtcFile(pCfgFiles, "TEMPSetup", "alarm_print", &iCfgValue);
	if(iCfgValue <ALARM_PRINT_OFF || iCfgValue >ALARM_PRINT_ON) iCfgValue = ALARM_PRINT_OFF;	
	pConfig->bPrnControl = iCfgValue;	
	//temp sensor 
	res = GetIntValueFromEtcFile(pCfgFiles, "TEMPSetup", "temptype", &iCfgValue);
	if(iCfgValue <TEMP_YSI || iCfgValue >TEMP_CYF) iCfgValue = TEMP_YSI;	
	pConfig->bTempType= iCfgValue;	
	//short trend max temp
	res = GetIntValueFromEtcFile(pCfgFiles, "TEMPSetup", "max_temp", &iCfgValue);
	if(iCfgValue <TEMP_MIN || iCfgValue >TEMP_MAX) iCfgValue = 390;	
	pConfig->iMax_TEMP= iCfgValue;
	//short trend min temp
	res = GetIntValueFromEtcFile(pCfgFiles, "TEMPSetup", "min_temp", &iCfgValue);
	if(iCfgValue <TEMP_MIN || iCfgValue >TEMP_MAX) iCfgValue = 0;	
	pConfig->iMin_TEMP= iCfgValue;
// 	//校准系数
// 	res = GetIntValueFromEtcFile(pCfgFiles, "TEMPSetup", "adjust_t1", &iCfgValue);
// 	pConfig->iAdjust_T1 = iCfgValue;
// 	res = GetIntValueFromEtcFile(pCfgFiles, "TEMPSetup", "adjust_t2", &iCfgValue);
// 	pConfig->iAdjust_T2 = iCfgValue;
	
	return 0;
}

static int InitValue_Temp(PS_TEMP_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	pValue->bID = MOD_TEMP;
	pValue->bConnected_T1 = FALSE;
	pValue->bCommStatus_T1 = COMMUNICATION_OK;
	pValue->bOverRange_T1 = FALSE;
	pValue->bOff_T1 = TRUE;
	pValue->bConnected_T2 = FALSE;
	pValue->bCommStatus_T2 = COMMUNICATION_OK;
	pValue->bOverRange_T2 = FALSE;
	pValue->bOff_T2 = TRUE;
	pValue->wT1 = 0;
	pValue->wT2 = 0;
	pValue->wTD = 0;

	return 0;
}

static int InitCfg_NIBP(const unsigned char *pCfgFiles, PS_NIBP_CFG pConfig)
{
	int iCfgValue;
	int res;
	
	if((pCfgFiles == NULL) || (pConfig ==NULL) ) return -1;
	
	pConfig->bID = MOD_NIBP;
	
	//Switch
	res = GetIntValueFromEtcFile(pCfgFiles, "NIBPSetup", "switch", &iCfgValue);
	if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_ON;	
	pConfig->bSwitch = iCfgValue;
	//Color 
	res = GetIntValueFromEtcFile(pCfgFiles, "NIBPSetup", "color", &iCfgValue);
	if(iCfgValue <0 || iCfgValue >255) iCfgValue = COLOR_lightgray;	
	pConfig->iColor = iCfgValue;	
	//Unit
	res = GetIntValueFromEtcFile(pCfgFiles, "NIBPSetup", "unit", &iCfgValue);
	if(iCfgValue <NIBP_UNIT_MMHG || iCfgValue >NIBP_UNIT_KPA) iCfgValue = NIBP_UNIT_MMHG;	
	pConfig->bUnit = iCfgValue;	
	//Object
	res = GetIntValueFromEtcFile(pCfgFiles, "NIBPSetup", "object", &iCfgValue);
	if(iCfgValue <NIBP_OBJECT_ADULT || iCfgValue >NIBP_OBJECT_BABY) iCfgValue = NIBP_OBJECT_ADULT;	
	pConfig->bObject = iCfgValue;		
	//Mode
	res = GetIntValueFromEtcFile(pCfgFiles, "NIBPSetup", "mode", &iCfgValue);
	if(iCfgValue <NIBP_MODE_MANUAL || iCfgValue >NIBP_MODE_PUNCTURE) iCfgValue = NIBP_MODE_MANUAL;	
	pConfig->bMode = iCfgValue;		
	//Initial pressure
	res = GetIntValueFromEtcFile(pCfgFiles, "NIBPSetup", "init_pres", &iCfgValue);
 	if(iCfgValue <NIBP_MIN || iCfgValue >NIBP_MAX) iCfgValue = 100;	
	pConfig->wInitPres = iCfgValue;		
	//venipuncture
	res = GetIntValueFromEtcFile(pCfgFiles, "NIBPSetup", "venipuncture", &iCfgValue);
 	if(iCfgValue <NIBP_V_PRESSURE_MIN || iCfgValue >NIBP_V_PRESSURE_MAX) iCfgValue = 80;	
	pConfig->wVP = iCfgValue;
	//Interval
	res = GetIntValueFromEtcFile(pCfgFiles, "NIBPSetup", "interval", &iCfgValue);
	if(iCfgValue <NIBP_INTERVAL_MIN || iCfgValue >NIBP_INTERVAL_MAX) iCfgValue = NIBP_INTERVAL_MIN;	
	pConfig->wInterval = iCfgValue;		
	//Sys High
	res = GetIntValueFromEtcFile(pCfgFiles, "NIBPSetup", "sys_high", &iCfgValue);
	if(iCfgValue <NIBP_MIN || iCfgValue >NIBP_MAX) iCfgValue = 160;	
	pConfig->wHigh_Sys= iCfgValue;		
	//Sys Low
	res = GetIntValueFromEtcFile(pCfgFiles, "NIBPSetup", "sys_low", &iCfgValue);
	if(iCfgValue <NIBP_MIN || iCfgValue >NIBP_MAX) iCfgValue = 90;	
	pConfig->wLow_Sys= iCfgValue;		
	//Dia High
	res = GetIntValueFromEtcFile(pCfgFiles, "NIBPSetup", "dia_high", &iCfgValue);
	if(iCfgValue <NIBP_MIN || iCfgValue >NIBP_MAX) iCfgValue = 90;	
	pConfig->wHigh_Dia= iCfgValue;		
	//Dia Low
	res = GetIntValueFromEtcFile(pCfgFiles, "NIBPSetup", "dia_low", &iCfgValue);
	if(iCfgValue <NIBP_MIN || iCfgValue >NIBP_MAX) iCfgValue = 50;	
	pConfig->wLow_Dia= iCfgValue;		
	//Mean High
	res = GetIntValueFromEtcFile(pCfgFiles, "NIBPSetup", "mean_high", &iCfgValue);
	if(iCfgValue <NIBP_MIN || iCfgValue >NIBP_MAX) iCfgValue = 110;	
	pConfig->wHigh_Mean= iCfgValue;		
	//Mean Low
	res = GetIntValueFromEtcFile(pCfgFiles, "NIBPSetup", "mean_low", &iCfgValue);
	if(iCfgValue <NIBP_MIN || iCfgValue >NIBP_MAX) iCfgValue = 60;	
	pConfig->wLow_Mean = iCfgValue;		
	//Alarm Source
	res = GetIntValueFromEtcFile(pCfgFiles, "NIBPSetup", "alarm_source", &iCfgValue);
	if(iCfgValue <NIBP_ALM_SRC_ALL || iCfgValue >NIBP_ALM_SRC_SD) iCfgValue = NIBP_ALM_SRC_ALL;	
	pConfig->bAlmSource = iCfgValue;	
	//Alarm Control
	res = GetIntValueFromEtcFile(pCfgFiles, "NIBPSetup", "alarm_control", &iCfgValue);
	if(iCfgValue <ALARM_OFF || iCfgValue >ALARM_HIGH) iCfgValue = ALARM_MID;	
	pConfig->bAlmControl = iCfgValue;	
	//Alarm Print Control
	res = GetIntValueFromEtcFile(pCfgFiles, "NIBPSetup", "alarm_print", &iCfgValue);
	if(iCfgValue <ALARM_PRINT_OFF || iCfgValue >ALARM_PRINT_ON) iCfgValue = ALARM_PRINT_OFF;	
	pConfig->bPrnControl = iCfgValue;	
	//short trend max NIBP
	res = GetIntValueFromEtcFile(pCfgFiles, "NIBPSetup", "max_nibp", &iCfgValue);
	if(iCfgValue <NIBP_MIN || iCfgValue >NIBP_MAX) iCfgValue = 300;	
	pConfig->wMax_NIBP= iCfgValue;
	//short trend min	NIBP
	res = GetIntValueFromEtcFile(pCfgFiles, "NIBPSetup", "min_nibp", &iCfgValue);
	if(iCfgValue <NIBP_MIN || iCfgValue >NIBP_MAX) iCfgValue = 0;	
	pConfig->wMin_NIBP= iCfgValue;
	
// 	//校准系数
// 	res = GetIntValueFromEtcFile(pCfgFiles, "NIBPSetup", "adjust_sys", &iCfgValue);
// 	pConfig->iAdjust_Sys= iCfgValue;
// 	res = GetIntValueFromEtcFile(pCfgFiles, "NIBPSetup", "adjust_dia", &iCfgValue);
// 	pConfig->iAdjust_Dia= iCfgValue;
// 	res = GetIntValueFromEtcFile(pCfgFiles, "NIBPSetup", "adjust_mean", &iCfgValue);
// 	pConfig->iAdjust_Mean= iCfgValue;
	
	return 0;
}

static int InitValue_NIBP(PS_NIBP_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	pValue->bID = MOD_NIBP;
	pValue->bConnected = FALSE;
	pValue->bStatus = NIBP_STATUS_STOP;
	pValue->bMeasureOK = FALSE;
	pValue->bCanceled = TRUE;
	pValue->wSys = pValue->wMean = pValue->wDia = 0;
	pValue->wPR = 0;
	pValue->wPressure = 0;
	pValue->bComError = FALSE;
	pValue->bComStop = FALSE;
	pValue->bModuleError = FALSE;
	pValue->bLimitError_Sys = FALSE;
	pValue->bLimitError_Dia = FALSE;
	pValue->bLimitError_Mean = FALSE;
	pValue->wErrCode = 0;
	return 0;
}

static int InitCfg_IBP1(const unsigned char *pCfgFiles, PS_IBP_CFG pConfig)
{
	int iCfgValue;
	int res;
	
	if((pCfgFiles == NULL) || (pConfig ==NULL) ) return -1;
	
	pConfig->bID = MOD_IBP1;
	
	//Switch
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP1Setup", "switch", &iCfgValue);
	if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_ON;	
	pConfig->bSwitch = iCfgValue;
	//Color 
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP1Setup", "color", &iCfgValue);
	if(iCfgValue <0 || iCfgValue >255) iCfgValue = COLOR_lightgray;	
	pConfig->iColor = iCfgValue;	
	//Unit
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP1Setup", "unit", &iCfgValue);
	if(iCfgValue <IBPUNIT_MMHG || iCfgValue >IBPUNIT_CMH2O) iCfgValue = IBPUNIT_MMHG;	
	pConfig->bUnit = iCfgValue;	
	//Label
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP1Setup", "label", &iCfgValue);
	if(iCfgValue <IBPLAB_ART || iCfgValue >IBPLAB_P2) iCfgValue = IBPLAB_ART;	
	pConfig->bLabel = iCfgValue;	
	//View Mode
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP1Setup", "view_mode", &iCfgValue);
	if(iCfgValue <IBPVIEW_ALL || iCfgValue >IBPVIEW_M) iCfgValue = IBPVIEW_ALL;	
	pConfig->bViewMode = iCfgValue;	
	//Scan Speed
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP1Setup", "scan_speed", &iCfgValue);
	if(iCfgValue <SCANSPEED_125 || iCfgValue >SCANSPEED_50) iCfgValue = SCANSPEED_25;	
	pConfig->bScanSpeed = iCfgValue;	
	//Scale Top
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP1Setup", "scale_top", &iCfgValue);
	if(iCfgValue <IBP_MIN || iCfgValue >IBP_MAX) iCfgValue = 150;	
	pConfig->iScaleTop = iCfgValue;	
	//Scale Bottom
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP1Setup", "scale_bottom", &iCfgValue);
	if(iCfgValue <IBP_MIN || iCfgValue >IBP_MAX) iCfgValue = 0;	
	pConfig->iScaleBottom = iCfgValue;	
	//Scale Middle
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP1Setup", "scale_middle", &iCfgValue);
	if(iCfgValue <IBP_MIN || iCfgValue >IBP_MAX) iCfgValue = 75;	
	pConfig->iScaleMiddle = iCfgValue;	
	//Sys High
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP1Setup", "sys_high", &iCfgValue);
	if(iCfgValue <IBP_MIN || iCfgValue >IBP_MAX) iCfgValue = 160;	
	pConfig->iHigh_Sys = iCfgValue;	
	//Sys Low
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP1Setup", "sys_low", &iCfgValue);
	if(iCfgValue <IBP_MIN || iCfgValue >IBP_MAX) iCfgValue = 90;	
	pConfig->iLow_Sys = iCfgValue;	
	//Dia High
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP1Setup", "dia_high", &iCfgValue);
	if(iCfgValue <IBP_MIN || iCfgValue >IBP_MAX) iCfgValue = 90;	
	pConfig->iHigh_Dia = iCfgValue;	
	//Dia Low
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP1Setup", "dia_low", &iCfgValue);
	if(iCfgValue <IBP_MIN || iCfgValue >IBP_MAX) iCfgValue = 50;	
	pConfig->iLow_Dia = iCfgValue;	
	//Mean High
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP1Setup", "mean_high", &iCfgValue);
	if(iCfgValue <IBP_MIN || iCfgValue >IBP_MAX) iCfgValue = 110;	
	pConfig->iHigh_Mean = iCfgValue;	
	//Mean Low
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP1Setup", "mean_low", &iCfgValue);
	if(iCfgValue <IBP_MIN || iCfgValue >IBP_MAX) iCfgValue = 70;	
	pConfig->iLow_Mean = iCfgValue;	
	//Alarm Source
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP1Setup", "alarm_source", &iCfgValue);
	if(iCfgValue <IBP_ALM_SRC_ALL || iCfgValue >IBP_ALM_SRC_SD) iCfgValue = IBP_ALM_SRC_ALL;	
	pConfig->bAlmSource = iCfgValue;	
	//Alarm Control
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP1Setup", "alarm_control", &iCfgValue);
	if(iCfgValue <ALARM_OFF || iCfgValue >ALARM_HIGH) iCfgValue = ALARM_MID;	
	pConfig->bAlmControl = iCfgValue;	
	//Alarm Print Control
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP1Setup", "alarm_print", &iCfgValue);
	if(iCfgValue <ALARM_PRINT_OFF || iCfgValue >ALARM_PRINT_ON) iCfgValue = ALARM_PRINT_OFF;	
	pConfig->bPrnControl = iCfgValue;	
	//Check Value
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP1Setup", "check_value", &iCfgValue);
	pConfig->iCheckValue = iCfgValue;	
	
	return 0;
}

static int InitValue_IBP1(PS_IBP_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	pValue->bID = MOD_IBP1;
	pValue->bConnected = FALSE;
	pValue->bCommStatus = COMMUNICATION_OK;
	pValue->bStatus = IBP_STATUS_NORMAL; 
	pValue->iSys = pValue->iMean = pValue->iDia = 0;
	pValue->iWave = 0;
	pValue->wHR = 0;
	pValue->bOff = TRUE;

	return 0;
}


static int InitCfg_IBP2(const unsigned char *pCfgFiles, PS_IBP_CFG pConfig)
{
	int iCfgValue;
	int res; 
	
	if((pCfgFiles == NULL) || (pConfig ==NULL) ) return -1;
	
	pConfig->bID = MOD_IBP2;
	
	//Switch
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP2Setup", "switch", &iCfgValue);
	if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_ON;	
	pConfig->bSwitch = iCfgValue;
	//Color 
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP2Setup", "color", &iCfgValue);
	if(iCfgValue <0 || iCfgValue >255) iCfgValue = COLOR_cyan;	
	pConfig->iColor = iCfgValue;	
	//Unit
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP2Setup", "unit", &iCfgValue);
	//if(iCfgValue <IBPUNIT_MMHG || iCfgValue >IBPUNIT_CMH2O) iCfgValue = IBPUNIT_CMH2O;	
	if(iCfgValue <IBPUNIT_MMHG || iCfgValue >IBPUNIT_CMH2O) iCfgValue = IBPUNIT_MMHG;	
	pConfig->bUnit = iCfgValue;	
	//Label
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP2Setup", "label", &iCfgValue);
	if(iCfgValue <IBPLAB_ART || iCfgValue >IBPLAB_P2) iCfgValue = IBPLAB_CVP;	
	pConfig->bLabel = iCfgValue;	
	//View Mode
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP2Setup", "view_mode", &iCfgValue);
	if(iCfgValue <IBPVIEW_ALL || iCfgValue >IBPVIEW_M) iCfgValue = IBPVIEW_M;	
	pConfig->bViewMode = iCfgValue;	
	//Scan Speed
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP2Setup", "scan_speed", &iCfgValue);
	if(iCfgValue <SCANSPEED_125 || iCfgValue >SCANSPEED_50) iCfgValue = SCANSPEED_25;	
	pConfig->bScanSpeed = iCfgValue;	
	//Scale Top
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP2Setup", "scale_top", &iCfgValue);
	if(iCfgValue <IBP_MIN || iCfgValue >IBP_MAX) iCfgValue = 40;	
	pConfig->iScaleTop = iCfgValue;	
	//Scale Bottom
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP2Setup", "scale_bottom", &iCfgValue);
	if(iCfgValue <IBP_MIN || iCfgValue >IBP_MAX) iCfgValue = 0;	
	pConfig->iScaleBottom = iCfgValue;	
	//Scale Middle
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP2Setup", "scale_middle", &iCfgValue);
	if(iCfgValue <IBP_MIN || iCfgValue >IBP_MAX) iCfgValue = 20;	
	pConfig->iScaleMiddle = iCfgValue;	
	//Sys High
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP2Setup", "sys_high", &iCfgValue);
	if(iCfgValue <IBP_MIN || iCfgValue >IBP_MAX) iCfgValue = 160;	
	pConfig->iHigh_Sys = iCfgValue;	
	//Sys Low
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP2Setup", "sys_low", &iCfgValue);
	if(iCfgValue <IBP_MIN || iCfgValue >IBP_MAX) iCfgValue = 90;	
	pConfig->iLow_Sys = iCfgValue;	
	//Dia High
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP2Setup", "dia_high", &iCfgValue);
	if(iCfgValue <IBP_MIN || iCfgValue >IBP_MAX) iCfgValue = 90;	
	pConfig->iHigh_Dia = iCfgValue;	
	//Dia Low
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP2Setup", "dia_low", &iCfgValue);
	if(iCfgValue <IBP_MIN || iCfgValue >IBP_MAX) iCfgValue = 50;	
	pConfig->iLow_Dia = iCfgValue;	
	//Mean High
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP2Setup", "mean_high", &iCfgValue);
	if(iCfgValue <IBP_MIN || iCfgValue >IBP_MAX) iCfgValue = 10;	
	pConfig->iHigh_Mean = iCfgValue;	
	//Mean Low
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP2Setup", "mean_low", &iCfgValue);
	if(iCfgValue <IBP_MIN || iCfgValue >IBP_MAX) iCfgValue = 0;	
	pConfig->iLow_Mean = iCfgValue;	
	//Alarm Source
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP2Setup", "alarm_source", &iCfgValue);
	if(iCfgValue <IBP_ALM_SRC_ALL || iCfgValue >IBP_ALM_SRC_SD) iCfgValue = IBP_ALM_SRC_M;	
	pConfig->bAlmSource = iCfgValue;	
	//Alarm Control
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP2Setup", "alarm_control", &iCfgValue);
	if(iCfgValue <ALARM_OFF || iCfgValue >ALARM_HIGH) iCfgValue = ALARM_MID;	
	pConfig->bAlmControl = iCfgValue;	
	//Alarm Print Control
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP2Setup", "alarm_print", &iCfgValue);
	if(iCfgValue <ALARM_PRINT_OFF || iCfgValue >ALARM_PRINT_ON) iCfgValue = ALARM_PRINT_OFF;	
	pConfig->bPrnControl = iCfgValue;	
        //Check Value
	res = GetIntValueFromEtcFile(pCfgFiles, "IBP2Setup", "check_value", &iCfgValue);
	pConfig->iCheckValue = iCfgValue;	
	
	return 0;
}

static int InitValue_IBP2(PS_IBP_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	pValue->bID = MOD_IBP2;
	pValue->bConnected = FALSE;
	pValue->bCommStatus = COMMUNICATION_OK;
	pValue->bStatus = IBP_STATUS_NORMAL; 
	pValue->iSys = pValue->iMean = pValue->iDia = 0;
	pValue->iWave = 0;
	pValue->wHR = 0;
	pValue->bOff = TRUE;

	return 0;
}

static int InitCfg_CO2(const unsigned char *pCfgFiles, PS_CO2_CFG pConfig)
{
	int iCfgValue;
	int res;
	
	if((pCfgFiles == NULL) || (pConfig ==NULL) ) return -1;
	
	pConfig->bID = MOD_CO2;
	
	//Switch
	res = GetIntValueFromEtcFile(pCfgFiles, "CO2Setup", "switch", &iCfgValue);
	if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_ON;	
	pConfig->bSwitch = iCfgValue;
	//Color 
	res = GetIntValueFromEtcFile(pCfgFiles, "CO2Setup", "color", &iCfgValue);
	if(iCfgValue <0 || iCfgValue >255) iCfgValue = COLOR_yellow;	
	pConfig->iColor = iCfgValue;	
	//Unit
	res = GetIntValueFromEtcFile(pCfgFiles, "CO2Setup", "unit", &iCfgValue);
	if(iCfgValue <CO2UNIT_MMHG || iCfgValue >CO2UNIT_KPA) iCfgValue = CO2UNIT_MMHG;	
	pConfig->bUnit = iCfgValue;	
	//Scan Speed
	res = GetIntValueFromEtcFile(pCfgFiles, "CO2Setup", "scan_speed", &iCfgValue);
	if(iCfgValue <SCANSPEED_625 || iCfgValue >SCANSPEED_25) iCfgValue = SCANSPEED_125;	
	pConfig->bScanSpeed = iCfgValue;	
	//Scale Top
	res = GetIntValueFromEtcFile(pCfgFiles, "CO2Setup", "scale_top", &iCfgValue);
	if(iCfgValue <CO2_MIN || iCfgValue >CO2_MAX) iCfgValue = 50;	
	pConfig->iScaleTop = iCfgValue;	
	//Range 波形显示幅度
	res = GetIntValueFromEtcFile(pCfgFiles, "CO2Setup", "range", &iCfgValue);
	if(iCfgValue <CO2_RANGE_BIG || iCfgValue >CO2_RANGE_SMALL) iCfgValue = CO2_RANGE_BIG;	
	pConfig->bRange = iCfgValue;	
	//Rate(气体采样流速) 
	res = GetIntValueFromEtcFile(pCfgFiles, "CO2Setup", "rate", &iCfgValue);
	if(iCfgValue < CO2RATE_50 || iCfgValue >CO2RATE_200) iCfgValue = CO2RATE_100;	
	pConfig->bRate = iCfgValue;	
	//Calibration values
	res = GetIntValueFromEtcFile(pCfgFiles, "CO2Setup", "cali_value", &iCfgValue);
	if(iCfgValue <CO2_MIN || iCfgValue >CO2_MAX) iCfgValue = 20;	
	pConfig->iCaliValue = iCfgValue;	
	//EtCO2 High
	res = GetIntValueFromEtcFile(pCfgFiles, "CO2Setup", "etco2_high", &iCfgValue);
	if(iCfgValue <CO2_MIN || iCfgValue >CO2_MAX) iCfgValue = 50;	
	pConfig->wHigh_EtCO2 = iCfgValue;	
	//EtCO2 Low
	res = GetIntValueFromEtcFile(pCfgFiles, "CO2Setup", "etco2_low", &iCfgValue);
	if(iCfgValue <CO2_MIN || iCfgValue >CO2_MAX) iCfgValue = 15;	
	pConfig->wLow_EtCO2 = iCfgValue;	
	//FiCO2 High
	res = GetIntValueFromEtcFile(pCfgFiles, "CO2Setup", "fico2_high", &iCfgValue);
	if(iCfgValue <CO2_MIN || iCfgValue >CO2_MAX) iCfgValue = 4;	
	pConfig->wHigh_FiCO2 = iCfgValue;	
	//FiCO2 Low
	res = GetIntValueFromEtcFile(pCfgFiles, "CO2Setup", "fico2_low", &iCfgValue);
	if(iCfgValue <CO2_MIN || iCfgValue >CO2_MAX) iCfgValue = 0;	
	pConfig->wLow_FiCO2 = iCfgValue;	
	//awRR High
	res = GetIntValueFromEtcFile(pCfgFiles, "CO2Setup", "awrr_high", &iCfgValue);
	if(iCfgValue <RR_MIN || iCfgValue >RR_MAX) iCfgValue = 30;	
	pConfig->wHigh_awRR= iCfgValue;	
	//awRR Low
	res = GetIntValueFromEtcFile(pCfgFiles, "CO2Setup", "awrr_low", &iCfgValue);
	if(iCfgValue <RR_MIN || iCfgValue >RR_MAX) iCfgValue = 8;	
	pConfig->wLow_awRR = iCfgValue;	
	//Apnea Time
	res = GetIntValueFromEtcFile(pCfgFiles, "CO2Setup", "apnea_time", &iCfgValue);
	if(iCfgValue <APNEATIME_OFF || iCfgValue >APNEATIME_MAX) iCfgValue = 20;	
	pConfig->wApneaTime = iCfgValue;	
	//Alarm Control
	res = GetIntValueFromEtcFile(pCfgFiles, "CO2Setup", "alarm_control", &iCfgValue);
	if(iCfgValue <ALARM_OFF || iCfgValue >ALARM_HIGH) iCfgValue = ALARM_MID;	
	pConfig->bAlmControl = iCfgValue;	
	//Alarm Print Control
	res = GetIntValueFromEtcFile(pCfgFiles, "CO2Setup", "alarm_print", &iCfgValue);
	if(iCfgValue <ALARM_PRINT_OFF || iCfgValue >ALARM_PRINT_ON) iCfgValue = ALARM_PRINT_OFF;	
	pConfig->bPrnControl = iCfgValue;	

	return 0;
}

static int InitValue_CO2(PS_CO2_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	pValue->bID = MOD_IBP1;
	pValue->bConnected = FALSE;
	pValue->bCommStatus = COMMUNICATION_OK;
	pValue->bStatus = CO2_STATUS_STOP;
	pValue->bOff = TRUE;
	pValue->bRate = CO2RATE_100;
	pValue->wAwRR = 0;
	pValue->wEtCO2 = 0;
	pValue->wFiCO2 = 0;
	pValue->wInsCO2 = 0;
	pValue->wWave = 0;
	pValue->wBaro = 0;

	return 0;
}

static int InitCfg_Patient(const unsigned char *pCfgFiles, PS_PATIENT_CFG  pConfig)
{
	int iCfgValue;
	int res;
// 	unsigned char strMenu[PATIENT_NAME_MAX]={0};
	char strMenu[100]={0};
	
	if((pCfgFiles == NULL) || (pConfig ==NULL) ) return -1;

	//科室Room
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(pCfgFiles, "PatientSetup", "room", strMenu, sizeof strMenu);	
	snprintf((BYTE *)(pConfig->sRoom), PATIENT_ROOM_MAX, "%s", strMenu);

	//病人ID
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(pCfgFiles, "PatientSetup", "id", strMenu, sizeof strMenu);	
	snprintf((BYTE *)pConfig->sID, PATIENT_ID_MAX, "%s", strMenu);

	//床号Bed
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(pCfgFiles, "PatientSetup", "bed", strMenu, sizeof strMenu);	
	snprintf((BYTE *)(pConfig->sBed), PATIENT_BED_MAX, "%s", strMenu);
	
	//医生
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(pCfgFiles, "PatientSetup", "doctor", strMenu, sizeof strMenu);	
	snprintf((BYTE *)(pConfig->sDoctor), PATIENT_NAME_MAX, "%s", strMenu);
	
	//姓名
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(pCfgFiles, "PatientSetup", "name", strMenu, sizeof strMenu);	
	snprintf((BYTE *)pConfig->sName, PATIENT_NAME_MAX, "%s", strMenu);

	//性别
	res = GetIntValueFromEtcFile(pCfgFiles, "PatientSetup", "sex", &iCfgValue);
	if(iCfgValue <MALE || iCfgValue >FEMALE) iCfgValue = MALE;	
	pConfig->bSex = iCfgValue;
	
	//类型
	res = GetIntValueFromEtcFile(pCfgFiles, "PatientSetup", "object", &iCfgValue);
	if(iCfgValue <ADULT || iCfgValue >BABY) iCfgValue = ADULT;	
	pConfig->bObject = iCfgValue;
	
	//入院日期
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(pCfgFiles, "PatientSetup", "admit", strMenu, sizeof strMenu);	
	snprintf((BYTE *)pConfig->sAdmit, PATIENT_DATE_MAX, "%s", strMenu);
	
	//生日日期
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(pCfgFiles, "PatientSetup", "birth", strMenu, sizeof strMenu);	
	snprintf((BYTE *)pConfig->sBirth, PATIENT_DATE_MAX, "%s", strMenu);

	//身高
	res = GetIntValueFromEtcFile(pCfgFiles, "PatientSetup", "height", &iCfgValue);
	if(iCfgValue <HEIGHT_MIN || iCfgValue >HEIGHT_MAX) iCfgValue = 178;	
	pConfig->wHeight= iCfgValue;
	
	//体重
	res = GetIntValueFromEtcFile(pCfgFiles, "PatientSetup", "weight", &iCfgValue);
	if(iCfgValue <WEIGHT_MIN || iCfgValue >WEIGHT_MAX) iCfgValue = 80;	
	pConfig->wWeight = iCfgValue;
	
	//血型
	res = GetIntValueFromEtcFile(pCfgFiles, "PatientSetup", "blood", &iCfgValue);
	if(iCfgValue <BLOOD_A || iCfgValue >BLOOD_UNKNOW) iCfgValue = BLOOD_UNKNOW;	
	pConfig->bBlood= iCfgValue;

	//年龄
	res = GetIntValueFromEtcFile(pCfgFiles, "PatientSetup", "age", &iCfgValue);
	if(iCfgValue <AGE_MIN || iCfgValue >AGE_MAX) iCfgValue = 28;	
	pConfig->bAge = iCfgValue;
	
	//Pacemaker
	res = GetIntValueFromEtcFile(pCfgFiles, "PatientSetup", "pacemaker", &iCfgValue);
	if(iCfgValue <PACEMAKER_OFF || iCfgValue >PACEMAKER_MODE2) iCfgValue = PACEMAKER_OFF;	
	pConfig->bPacemaker = iCfgValue;
	
	
	return 0;
}

static int InitCfg_System(const unsigned char *pCfgFiles, PS_SYSTEM_CFG  pConfig)
{
	int iCfgValue;
	int res;
	
	if((pCfgFiles == NULL) || (pConfig ==NULL) ) return -1;
	
	//Language
	res = GetIntValueFromEtcFile(pCfgFiles, "SystemSetup", "language", &iCfgValue);
	if(iCfgValue <LANGUAGE_ENGLISH || iCfgValue >20) iCfgValue = LANGUAGE_ENGLISH;	
	pConfig->bLanguage = iCfgValue;
	//检查OEM信息，如果只是配置了英语，则屏蔽其他语言
	//GetIntValueFromEtcFile(gFileMachineConfig, "OEM_INFO", "language", &iCfgValue);
	//if(iCfgValue == 0) pConfig->bLanguage = 0;
	
	//Interface
	res = GetIntValueFromEtcFile(pCfgFiles, "SystemSetup", "interface", &iCfgValue);
//  	if(iCfgValue <SCREEN_NORMAL || iCfgValue >SCREEN_OXYCRG) iCfgValue = SCREEN_NORMAL;	
 	if(iCfgValue <SCREEN_NORMAL || iCfgValue >SCREEN_12LEAD) iCfgValue = SCREEN_NORMAL;
	
	pConfig->bInterface = iCfgValue;
	//如果有IBP选项，则开机先开启IBP界面
	if(gbHaveIbp)
 		pConfig->bInterface = SCREEN_IBP;
	
	
	//Key Volume
	res = GetIntValueFromEtcFile(pCfgFiles, "SystemSetup", "key_volume", &iCfgValue);
	if(iCfgValue <KEYVOLUME_OFF || iCfgValue >KEYVOLUME_ON) iCfgValue = KEYVOLUME_OFF;	
	pConfig->bKeyVolume = iCfgValue;
	//AC Freq
	res = GetIntValueFromEtcFile(pCfgFiles, "SystemSetup", "ac_freq", &iCfgValue);
	if(iCfgValue <AC_50HZ || iCfgValue >AC_60HZ) iCfgValue = AC_50HZ;	
	pConfig->bACFreq = iCfgValue;
	//LCD Type
	res = GetIntValueFromEtcFile(pCfgFiles, "SystemSetup", "lcd_type", &iCfgValue);
	if(iCfgValue <LCDTYPE_800_104  || iCfgValue >LCDTYPE_1024_15) iCfgValue = LCDTYPE_800_104;	
	pConfig->bLCDType= iCfgValue;
	//Network Type
	res = GetIntValueFromEtcFile(pCfgFiles, "SystemSetup", "net_type", &iCfgValue);
	if(iCfgValue <NETTYPE_ETH || iCfgValue >NETTYPE_485) iCfgValue = NETTYPE_ETH;	
	pConfig->bNetType= iCfgValue;
	//Short Trend Scale
	res = GetIntValueFromEtcFile(pCfgFiles, "SystemSetup", "shorttrend_scale", &iCfgValue);
	if(iCfgValue <0 || iCfgValue >7) iCfgValue = 0;	
	pConfig->bShortTrendScale= iCfgValue;
	//OxyCrg Scale
	res = GetIntValueFromEtcFile(pCfgFiles, "SystemSetup", "oxycrg_scale", &iCfgValue);
	if(iCfgValue <0 || iCfgValue>5) iCfgValue = 0;	
	pConfig->bOxyCrgScale = iCfgValue;
	//功能键定义
	res = GetIntValueFromEtcFile(pCfgFiles, "SystemSetup", "func_button", &iCfgValue);
	if(iCfgValue <FUNCBT_SWITCHVIEW || iCfgValue>FUNCBT_EXIT) iCfgValue = FUNCBT_SWITCHVIEW;	
	pConfig->bFuncButton= iCfgValue;
	//线段的绘制方式
	res = GetIntValueFromEtcFile(pCfgFiles, "SystemSetup", "line_type", &iCfgValue);
	if(iCfgValue <LINE_WIDE || iCfgValue>LINE_NORMAL) iCfgValue = LINE_WIDE;	
 	pConfig->bLineType = iCfgValue;
// 	pConfig->bLineType = LINE_ANTI;
	
	//看门狗
	res = GetIntValueFromEtcFile(pCfgFiles, "SystemSetup", "watchdog", &iCfgValue);
	if(iCfgValue <0 || iCfgValue>1) iCfgValue = 0;	
	pConfig->bWDTEnable = iCfgValue;
	
	return 0;
}

static int InitCfg_Alarm(const unsigned char *pCfgFiles, PS_ALARM_CFG  pConfig)
{
	int iCfgValue;
	int res;
	
	if((pCfgFiles == NULL) || (pConfig ==NULL) ) return -1;
	
	//Volume
	res = GetIntValueFromEtcFile(pCfgFiles, "AlarmSetup", "volume", &iCfgValue);
	if(iCfgValue <VOLUME_OFF || iCfgValue >VOLUME_10 ) iCfgValue = VOLUME_5;	
	pConfig->bVolume = iCfgValue;
	//pause time
	res = GetIntValueFromEtcFile(pCfgFiles, "AlarmSetup", "pause_time", &iCfgValue);
	if(iCfgValue <ALARMPAUSE_1MIN || iCfgValue >ALARMPAUSE_3MIN) iCfgValue = ALARMPAUSE_2MIN;	
	pConfig->bPauseTime = iCfgValue;
	//Print
	res = GetIntValueFromEtcFile(pCfgFiles, "AlarmSetup", "print", &iCfgValue);
	if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_OFF;	
	pConfig->bPrint = iCfgValue;
	//View Limit
	res = GetIntValueFromEtcFile(pCfgFiles, "AlarmSetup", "view_limit", &iCfgValue);
	if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_OFF;	
	pConfig->bViewLimit = iCfgValue;
	//Wave Length 
	res = GetIntValueFromEtcFile(pCfgFiles, "AlarmSetup", "wave_length", &iCfgValue);
	if(iCfgValue <4 || iCfgValue >16) iCfgValue = 4;	
 	pConfig->bPrintWaveLength = iCfgValue;

	return 0;
}

static int InitCfg_Printer(const unsigned char *pCfgFiles, PS_PRINT_CFG pConfig)
{
	int iCfgValue;
	int res;
	
	if((pCfgFiles == NULL) || (pConfig ==NULL) ) return -1;
	
	//Wave1
	res = GetIntValueFromEtcFile(pCfgFiles, "PrintSetup", "wave1", &iCfgValue);
 	if(iCfgValue <WID_NONE|| iCfgValue >WID_MAX) iCfgValue = WID_ECG1;	
 	pConfig->bWave1 = iCfgValue;
	
	//Wave2
	res = GetIntValueFromEtcFile(pCfgFiles, "PrintSetup", "wave2", &iCfgValue);
	if(iCfgValue <WID_NONE|| iCfgValue >WID_MAX) iCfgValue = WID_SPO2;	
	pConfig->bWave2 = iCfgValue;
	
	//Wave3
	res = GetIntValueFromEtcFile(pCfgFiles, "PrintSetup", "wave3", &iCfgValue);
	if(iCfgValue <WID_NONE|| iCfgValue >WID_MAX) iCfgValue = WID_RESP;	
	pConfig->bWave3 = iCfgValue;
	
	//Speed
	res = GetIntValueFromEtcFile(pCfgFiles, "PrintSetup", "speed", &iCfgValue);
	//暂时不支持50mm/s
	if(iCfgValue <PRINTSPEED_125 || iCfgValue >PRINTSPEED_25) iCfgValue = PRINTSPEED_25;	
	pConfig->bSpeed = iCfgValue;
	
	//Duration
	res = GetIntValueFromEtcFile(pCfgFiles, "PrintSetup", "duration", &iCfgValue);
	if(iCfgValue <0 || iCfgValue >REALPRINTTIME_MAX) iCfgValue = 0;	
	//强制为10秒
	pConfig->iDuration = 10;
	
	//Auto
	res = GetIntValueFromEtcFile(pCfgFiles, "PrintSetup", "inter", &iCfgValue);
	if(iCfgValue <0 || iCfgValue >REALPRINTAUTO_MAX) iCfgValue = 0;	
	//强制为关闭
	pConfig->iInter = 0;
		
	return 0;
}

//初始化波形配置
static int InitCfg_WaveSetup(const unsigned char *pCfgFiles)
{
	int iCfgValue;
	int res;
	
	if(pCfgFiles == NULL) return -1;
		
	//Wave 0	 ECG1
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave0_id", &iCfgValue);
	if(iCfgValue <WID_NONE || iCfgValue >WID_MAX) iCfgValue = WID_NONE;	
	gWaveCfg_User[0].bID = iCfgValue;
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave0_switch", &iCfgValue);
	if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_OFF;	
	gWaveCfg_User[0].bSwitch = iCfgValue;
	//Wave 1 ECG 2
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave1_id", &iCfgValue);
	if(iCfgValue <WID_NONE || iCfgValue >WID_MAX) iCfgValue = WID_NONE;	
	gWaveCfg_User[1].bID = iCfgValue;
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave1_switch", &iCfgValue);
	if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_OFF;	
	gWaveCfg_User[1].bSwitch = iCfgValue;
	//Wave 2 Pleth
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave2_id", &iCfgValue);
	if(iCfgValue <WID_NONE || iCfgValue >WID_MAX) iCfgValue = WID_NONE;	
	gWaveCfg_User[2].bID = iCfgValue;
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave2_switch", &iCfgValue);
	if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_OFF;	
	gWaveCfg_User[2].bSwitch = iCfgValue;
	//Wave 3 RESP
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave3_id", &iCfgValue);
	if(iCfgValue <WID_NONE || iCfgValue >WID_MAX) iCfgValue = WID_NONE;	
	gWaveCfg_User[3].bID = iCfgValue;
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave3_switch", &iCfgValue);
	if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_OFF;	
	gWaveCfg_User[3].bSwitch = iCfgValue;
// 	//有CO2的时候，屏蔽呼吸波形
// 	if(gbHaveCo2) gWaveCfg_User[3].bSwitch = SWITCH_OFF;
	
	//Wave 4 IBP1
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave4_id", &iCfgValue);
	if(iCfgValue <WID_NONE || iCfgValue >WID_MAX) iCfgValue = WID_NONE;	
	gWaveCfg_User[4].bID = iCfgValue;
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave4_switch", &iCfgValue);
	if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_OFF;	
	gWaveCfg_User[4].bSwitch = iCfgValue;
	if(!gbHaveIbp) gWaveCfg_User[4].bSwitch = SWITCH_OFF;
	
	//Wave 5 IBP2
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave5_id", &iCfgValue);
	if(iCfgValue <WID_NONE || iCfgValue >WID_MAX) iCfgValue = WID_NONE;	
	gWaveCfg_User[5].bID = iCfgValue;
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave5_switch", &iCfgValue);
	if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_OFF;	
	gWaveCfg_User[5].bSwitch = iCfgValue;
	if(!gbHaveIbp) gWaveCfg_User[5].bSwitch = SWITCH_OFF;
	
	//Wave 6 CO2
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave6_id", &iCfgValue);
	if(iCfgValue <WID_NONE || iCfgValue >WID_MAX) iCfgValue = WID_NONE;	
	gWaveCfg_User[6].bID = iCfgValue;
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveSetup", "wave6_switch", &iCfgValue);
	if(iCfgValue <SWITCH_OFF || iCfgValue >SWITCH_ON) iCfgValue = SWITCH_OFF;	
	gWaveCfg_User[6].bSwitch = iCfgValue;
 	if(!gbHaveCo2) gWaveCfg_User[6].bSwitch = SWITCH_OFF;
	
	return 0;
}

//初始化趋势图配置
static int InitCfg_TrendGraph(const unsigned char *pCfgFiles)
{
	int iCfgValue;
	int res;
	
	if(pCfgFiles == NULL) return -1;
		
	//Para1
	res = GetIntValueFromEtcFile(pCfgFiles, "TrendGraphSetup", "Para1", &iCfgValue);
	if(iCfgValue <WID_NONE || iCfgValue >8) iCfgValue = WID_NONE;	
	TrendGraphCfg.bPara1= iCfgValue;
	//Para2
	res = GetIntValueFromEtcFile(pCfgFiles, "TrendGraphSetup", "Para2", &iCfgValue);
	if(iCfgValue <WID_NONE || iCfgValue >8) iCfgValue = WID_NONE;	
	TrendGraphCfg.bPara2 = iCfgValue;
	//Para3
	res = GetIntValueFromEtcFile(pCfgFiles, "TrendGraphSetup", "Para3", &iCfgValue);
	if(iCfgValue <WID_NONE || iCfgValue >8) iCfgValue = WID_NONE;	
	TrendGraphCfg.bPara3 = iCfgValue;
	//Scale
	res = GetIntValueFromEtcFile(pCfgFiles, "TrendGraphSetup", "Scale", &iCfgValue);
	if(iCfgValue <WID_NONE || iCfgValue >9) iCfgValue = WID_NONE;	
	TrendGraphCfg.bScale= iCfgValue;

	return 0;
}
/*
	初始化全局变量 
*/
static int InitGlobalPara()
{
	int iCfgValue;
	
	//键盘默认发送左右键
	gbKeyType = KEY_LR;
	
	//不显示Demo数据
	gbViewDemoData = FALSE;
	
	//初始化波形回放的全局变量
	gbWaveReviewIDs[0] = WID_ECG1;
	gbWaveReviewIDs[1] = WID_SPO2;
	if(gbHaveCo2){
		gbWaveReviewIDs[2] = WID_CO2;
	}
	else{
		gbWaveReviewIDs[2] = WID_RESP;
	}
	
				
	//默认不进行心电分析
//	gbEcgAnalysis = TRUE;
	gbEcgAnalysis = FALSE;

	
	//Select Sensor
	iCfgValue = 0;
	GetIntValueFromEtcFile(gFileMachineConfig, "SENSOR", "sensor_ibp", &iCfgValue);
	if(iCfgValue>1 || iCfgValue<0) iCfgValue = 0;
	gbSensor_Ibp = iCfgValue;
	
	iCfgValue = 0;
	GetIntValueFromEtcFile(gFileMachineConfig, "SENSOR", "sensor_temp", &iCfgValue);
	if(iCfgValue>1 || iCfgValue<0) iCfgValue = 0;
	gbSensor_Temp = iCfgValue;

	
	iCfgValue = 0;
	GetIntValueFromEtcFile(gFileMachineConfig, "SENSOR", "sensor_spo2", &iCfgValue);
	if(iCfgValue>1 || iCfgValue<0) iCfgValue = 0;
	gbSensor_SpO2 = iCfgValue;

	
	//监护对象：人类 or  动物
	iCfgValue = 0;
	GetIntValueFromEtcFile(gFileMachineConfig, "OBJECT", "object", &iCfgValue);
	if(iCfgValue>1 || iCfgValue<0) iCfgValue = 0;
	gbMonitorObject = iCfgValue;


	
	
	return 0;
}


//初始化波形配置
static int InitCfg_WaveformSetup(const unsigned char *pCfgFiles)
{
	int iCfgValue;
	int res;
	int i;
	if(pCfgFiles == NULL) return -1;
		
	//Wave 0	
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveformSetup", "wave0_id", &iCfgValue);
	if(iCfgValue <WAVEFORM_OFF|| iCfgValue >WAVEFORM_MAX) iCfgValue = WAVEFORM_ECG1;	
	WaveformCob[0].bID = iCfgValue;
	
	//Wave 1 
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveformSetup", "waveform1_id", &iCfgValue);
	if(iCfgValue <WAVEFORM_OFF || iCfgValue >WAVEFORM_MAX) iCfgValue = WAVEFORM_OFF;	
	WaveformCob[1].bID = iCfgValue;
	
	//Wave 2 
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveformSetup", "waveform2_id", &iCfgValue);
	if(iCfgValue <WAVEFORM_OFF || iCfgValue >WAVEFORM_MAX) iCfgValue = WAVEFORM_OFF;	
	WaveformCob[2].bID = iCfgValue;
	//Wave 3 
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveformSetup", "waveform3_id", &iCfgValue);
	if(iCfgValue <WAVEFORM_OFF || iCfgValue >WAVEFORM_MAX) iCfgValue = WAVEFORM_OFF;	
	WaveformCob[3].bID = iCfgValue;
	
	//Wave 4 
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveformSetup", "waveform4_id", &iCfgValue);
	if(iCfgValue <WAVEFORM_OFF || iCfgValue >WAVEFORM_MAX) iCfgValue = WAVEFORM_OFF;	
	WaveformCob[4].bID = iCfgValue;
	
	//Wave 5 
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveformSetup", "waveform5_id", &iCfgValue);
	if(iCfgValue <WAVEFORM_OFF || iCfgValue >WAVEFORM_MAX) iCfgValue = WAVEFORM_OFF;	
	WaveformCob[5].bID = iCfgValue;

	//Wave 6 
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveformSetup", "waveform6_id", &iCfgValue);
	if(iCfgValue <WAVEFORM_OFF || iCfgValue >WAVEFORM_MAX) iCfgValue = WAVEFORM_OFF;	
	WaveformCob[6].bID = iCfgValue;
 
	//Wave 7 
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveformSetup", "waveform7_id", &iCfgValue);
	if(iCfgValue <WAVEFORM_OFF || iCfgValue >WAVEFORM_MAX) iCfgValue = WAVEFORM_OFF;	
	WaveformCob[7].bID = iCfgValue;
 	
	//Wave 8 
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveformSetup", "waveform8_id", &iCfgValue);
	if(iCfgValue <WAVEFORM_OFF || iCfgValue >WID_MAX) iCfgValue = WAVEFORM_OFF;	
	WaveformCob[8].bID = iCfgValue;
 	
	//Wave 9 
	res = GetIntValueFromEtcFile(pCfgFiles, "WaveformSetup", "waveform9_id", &iCfgValue);
	if(iCfgValue <WAVEFORM_OFF || iCfgValue >WID_MAX) iCfgValue = WAVEFORM_OFF;	
	WaveformCob[9].bID = iCfgValue;
 	for(i=0;i<10;i++)
 		{	
 			//判断模块
 			if(WaveformCob[i].bID==WAVEFORM_IBP1)
				if(!gbHaveIbp) WaveformCob[i].bID = WAVEFORM_OFF;
			if(WaveformCob[i].bID==WAVEFORM_IBP2)
				if(!gbHaveIbp) WaveformCob[i].bID = WAVEFORM_OFF;
			if(WaveformCob[i].bID==WAVEFORM_CO2)
				if(!gbHaveCo2) WaveformCob[i].bID = WAVEFORM_OFF;
			//防止读取出错
			if(i==0&&WaveformCob[0].bID!=WAVEFORM_ECG1)
				WaveformCob[0].bID=WAVEFORM_ECG1;
			if(i==1&&WaveformCob[1].bID==WAVEFORM_ECG1)
				WaveformCob[1].bID=WAVEFORM_OFF;
			if(i>1&&(WaveformCob[i].bID==WAVEFORM_ECG1 ||WaveformCob[i].bID==WAVEFORM_CASCADE))
			WaveformCob[i].bID=WAVEFORM_OFF;
 		}
	
	
	return 0;
}


static int InitCfg_GlobalSetup(const unsigned char *pCfgFiles)
{
	int iCfgValue;
	int res;
	int i;
	if(pCfgFiles == NULL) return -1;
//短趋势存储设置
	//New trendinterval
	iCfgValue = 0;
	GetIntValueFromEtcFile(gFileSetup, "GlobalSetup", "TrendInterval", &iCfgValue);
	if(iCfgValue>11 || iCfgValue<0) iCfgValue = 0;
	giTrendInterval= iCfgValue;

	//NIBP storage
	iCfgValue = 0;
	GetIntValueFromEtcFile(gFileSetup, "GlobalSetup", "NIBPStorage", &iCfgValue);
	if(iCfgValue>1 || iCfgValue<0) iCfgValue = 0;
	gbNIBPStorage= iCfgValue;

	//ALM storage
	iCfgValue = 0;
	GetIntValueFromEtcFile(gFileSetup, "GlobalSetup", "ALMStorage", &iCfgValue);
	if(iCfgValue>1 || iCfgValue<0) iCfgValue = 0;
	gbALMStorage= iCfgValue;

	//Warn storage
	iCfgValue = 0;
	GetIntValueFromEtcFile(gFileSetup, "GlobalSetup", "WarnStorage", &iCfgValue);
	if(iCfgValue>1 || iCfgValue<0) iCfgValue = 0;
	gbWarnStorage= iCfgValue;


//短趋势图标尺
/*
	//Time Scale 
	iCfgValue = 0;
	GetIntValueFromEtcFile(gFileSetup, "GlobalSetup", "TimeScale", &iCfgValue);
	if(iCfgValue>7 || iCfgValue<0) iCfgValue = 0;
	giTimeScale= iCfgValue;
*/

//HR Scale
	iCfgValue = 0;
	GetIntValueFromEtcFile(gFileSetup, "GlobalSetup", "HRScale", &iCfgValue);
	if(iCfgValue>1 || iCfgValue<0) iCfgValue = 0;
	giHRScale= iCfgValue;


//Spo2 Scale
	iCfgValue = 0;
	GetIntValueFromEtcFile(gFileSetup, "GlobalSetup", "Spo2Scale", &iCfgValue);
	if(iCfgValue>2 || iCfgValue<0) iCfgValue = 0;
	giSpo2Scale= iCfgValue;


//Resp Scale
	iCfgValue = 0;
	GetIntValueFromEtcFile(gFileSetup, "GlobalSetup", "RespScale", &iCfgValue);
	if(iCfgValue>3 || iCfgValue<0) iCfgValue = 0;
	giRespScale= iCfgValue;


//ST Scale
	iCfgValue = 0;
	GetIntValueFromEtcFile(gFileSetup, "GlobalSetup", "STScale", &iCfgValue);
	if(iCfgValue>2 || iCfgValue<0) iCfgValue = 0;
	giSTScale= iCfgValue;

//IBP1 Scale
	iCfgValue = 0;
	GetIntValueFromEtcFile(gFileSetup, "GlobalSetup", "IBP1Scale", &iCfgValue);
	if(iCfgValue>2 || iCfgValue<0) iCfgValue = 0;
	giIBP1Scale= iCfgValue;

//IBP2 Scale
	iCfgValue = 0;
	GetIntValueFromEtcFile(gFileSetup, "GlobalSetup", "IBP2Scale", &iCfgValue);
	if(iCfgValue>2 || iCfgValue<0) iCfgValue = 0;
	giIBP2Scale= iCfgValue;

//EtCO2 Scale
	iCfgValue = 0;
	GetIntValueFromEtcFile(gFileSetup, "GlobalSetup", "EtCO2Scale", &iCfgValue);
	if(iCfgValue>2 || iCfgValue<0) iCfgValue = 0;
	giEtCO2Scale= iCfgValue;

//CI Scale
	iCfgValue = 0;
	GetIntValueFromEtcFile(gFileSetup, "GlobalSetup", "CIScale", &iCfgValue);
	if(iCfgValue>2 || iCfgValue<0) iCfgValue = 0;
	giCIScale= iCfgValue;


	
	return 0;
}



/*
	初始化DEMO文件ID
*/
static int  InitDemoFiles()
{
	gfileDemo_PM9K = NULL;
	gbSaveDemo_PM9K = FALSE;
	gfileDemo_IBP = NULL;
	gbSaveDemo_IBP = FALSE;
	gfileDemo_CO2 = NULL;
	gbSaveDemo_CO2 = FALSE;
	
	return 0;
}

//加载校准系数
static int LoadAdjust()
{
	int res;
	int iCfgValue;
	
	//校准系数
	//ECG
	res = GetIntValueFromEtcFile(gFileMachineConfig, "ECG", "adjust", &iCfgValue);
	gCfgEcg.iAdjust = iCfgValue;
	//SpO2
	res = GetIntValueFromEtcFile(gFileMachineConfig, "SPO2", "adjust", &iCfgValue);
	gCfgSpO2.iAdjust = iCfgValue;
	//NIBP
	res = GetIntValueFromEtcFile(gFileMachineConfig, "NIBP", "adjust_sys", &iCfgValue);
	gCfgNibp.iAdjust_Sys = iCfgValue;
	res = GetIntValueFromEtcFile(gFileMachineConfig, "NIBP", "adjust_dia", &iCfgValue);
	gCfgNibp.iAdjust_Dia = iCfgValue;
	res = GetIntValueFromEtcFile(gFileMachineConfig, "NIBP", "adjust_mean", &iCfgValue);
	gCfgNibp.iAdjust_Mean = iCfgValue;
	//Temp
	res = GetIntValueFromEtcFile(gFileMachineConfig, "TEMP", "adjust_t1", &iCfgValue);
	gCfgTemp.iAdjust_T1 = iCfgValue;
	res = GetIntValueFromEtcFile(gFileMachineConfig, "TEMP", "adjust_t2", &iCfgValue);
	gCfgTemp.iAdjust_T2 = iCfgValue;
	
	//IBP
	res = GetIntValueFromEtcFile(gFileMachineConfig, "IBP1", "check", &iCfgValue);
	if(iCfgValue<=0 || iCfgValue>1000)
		gfIbp1Check = 1.00;
	else
		gfIbp1Check = (float)((float)iCfgValue /100.00);

	res = GetIntValueFromEtcFile(gFileMachineConfig, "IBP2", "check", &iCfgValue);
	if(iCfgValue<=0 || iCfgValue>1000)
		gfIbp2Check = 1.00;
	else
		gfIbp2Check = (float)((float)iCfgValue /100.00);

	return 0;
}



//------------------ interface -------------------
/*
	初始化系统
*/
int InitSystem()
{
	
	//默认使用非41的主板
	gbPMVer41 = FALSE;
	
	//加载配置文件	
	LoadConfigFiles();	
	
	//初始化DEMO文件
//	InitDemoFiles();
			
	//初始化机器模块配置信息
	InitModulesSet(gFileMachineConfig, &gsModules);
	
	//初始化各模块的数据和配置信息
	InitValue_ECG(&gValueEcg);
	InitCfg_ECG(gFileSetup, &gCfgEcg);
	InitValue_Resp(&gValueResp);
	InitCfg_Resp(gFileSetup, &gCfgResp);
	InitValue_SpO2(&gValueSpO2);
	InitCfg_SpO2(gFileSetup, &gCfgSpO2);
	InitValue_Pulse(&gValuePulse);
	InitCfg_Pulse(gFileSetup, &gCfgPulse);
	InitValue_Temp(&gValueTemp);
	InitCfg_Temp(gFileSetup, &gCfgTemp);
	InitValue_NIBP(&gValueNibp);
	InitCfg_NIBP(gFileSetup, &gCfgNibp);
	InitValue_IBP1(&gValueIbp1);
	InitCfg_IBP1(gFileSetup, &gCfgIbp1);
	InitValue_IBP2(&gValueIbp2);
	InitCfg_IBP2(gFileSetup, &gCfgIbp2);
	InitValue_CO2(&gValueCO2);
	InitCfg_CO2(gFileSetup, &gCfgCO2);

	//读取波形配置文件-NEW--hejp
	InitCfg_WaveformSetup(gFileSetup);
	//初始化一些全局变量-NEW--hejp
	InitCfg_GlobalSetup(gFileSetup);

	//初始化病人信息
	InitCfg_Patient(gFileSetup, &gCfgPatient);
		
	//初始化系统配置信息
	InitCfg_System(gFileSetup, &gCfgSystem);
		
	//初始化报警功能配置
	InitCfg_Alarm(gFileSetup, &gCfgAlarm);
	
	//初始化打印机
 	InitCfg_Printer(gFileSetup, &gCfgPrinter);
	
	//初始化全局变量
	InitGlobalPara();
	
	//初始化波形配置
	InitCfg_WaveSetup(gFileSetup);

	//初始化趋势图配置
	InitCfg_TrendGraph(gFileSetup);
	
	//加载校准系数
	LoadAdjust();

	//通过配置文件获得网络配置
	LoadNetSetFromCfg(gFileMachineConfig, &gCfgNet);

	return 0;
}



/*
	加载语言资源文件和字库
*/
int LoadLanguageRes()
{
	//根据系统设置，加载相应的语言资源文件
	switch(gCfgSystem.bLanguage){
		//case LANGUAGE_OTHER:{
		//	snprintf(gsLanguageRes, sizeof gsLanguageRes, LANPATH_OTHER);
		//}break;
		case LANGUAGE_CHINESE:{
			snprintf(gsLanguageRes, sizeof gsLanguageRes, LANPATH_CHINESE);
		}break; 	
		/*
		case LANGUAGE_CHT:{
			snprintf(gsLanguageRes, sizeof gsLanguageRes, LANPATH_CHT);
		}break; 
		case LANGUAGE_TURKISH:{
			snprintf(gsLanguageRes, sizeof gsLanguageRes, LANPATH_TURKISH);
		}break;
		case LANGUAGE_RUSSIAN:{
			snprintf(gsLanguageRes, sizeof gsLanguageRes, LANPATH_RUSSIAN);
		}break;
		case LANGUAGE_ROMANIA:{
			snprintf(gsLanguageRes, sizeof gsLanguageRes, LANPATH_ROMANIA);
		}break;
		case LANGUAGE_SPANISH:{
			snprintf(gsLanguageRes, sizeof gsLanguageRes, LANPATH_SPANISH);
		}break;
		case LANGUAGE_ITALIAN:{
			snprintf(gsLanguageRes, sizeof gsLanguageRes, LANPATH_ITALIAN);
		}break;		
		*/
		default:{
			snprintf(gsLanguageRes, sizeof gsLanguageRes, LANPATH_ENGLISH);
		}break;
	}
	
	printf("------------------%s:%d System language is %d- %s.\n", __FILE__, __LINE__, gCfgSystem.bLanguage, gsLanguageRes);
	return 0;
}

//加载相应的字库和字体
int LoadFonts()
{	
	int iRes = 0;

	gFontSmall = CreateLogFontByName("rbf-fixed-rrncnn-6-12-ISO8859-1");
	
	//gFontEnglish_16=CreateLogFontByName("rbf-fixed-rrncnn-8-16-ISO8859-1");
	
	gFontSystem =CreateLogFont(NULL,
							"unifont",//"wx",
							"UTF-8",//"UTF-8",
							FONT_WEIGHT_REGULAR,
		
			                           FONT_SLANT_ROMAN,
			                           FONT_SETWIDTH_NORMAL,
			                           FONT_SPACING_CHARCELL,
			                           FONT_UNDERLINE_NONE,
			                           FONT_STRUCKOUT_NONE,
			                           12,
			                           0);
	gFontEnglish_16  = gFontSystem;
	
	//加载TTF字体
	gFontTTF_20 = CreateLogFont ("ttf", "arial", "ISO8859-1", 
				     FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
				     FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 
 				     20, 0);
	gFontTTF_30 = CreateLogFont ("ttf", "arial", "ISO8859-1", 
				     FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
				     FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 
				     34, 0);
// 				     32, 0);
//	gFontTTF_35 = CreateLogFont ("ttf", "arial", "ISO8859-1", 
//				     FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
//				     FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 
//				     36, 0);
	gFontTTF_40 = CreateLogFont ("ttf", "arial", "ISO8859-1", 
				     FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
				     FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 
				     40, 0);
	
	gFontTTF_50 = CreateLogFont ("ttf", "arial", "ISO8859-1", 
				     FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
				     FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 
				     45, 0);
	gFontTTF_20Bk= CreateLogFont ("ttf", "arialbk", "ISO8859-1", 
				     FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
				     FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 
 				     22, 0);
	gFontTTF_30Bk = CreateLogFont ("ttf", "arialbk", "ISO8859-1", 
				     FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
				     FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 
				     30, 0);
// 				     32, 0);
	gFontTTF_35Bk = CreateLogFont ("ttf", "arialbk", "ISO8859-1", 
				     FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
				     FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 
				     36, 0);
	gFontTTF_40Bk = CreateLogFont ("ttf", "arialbk", "ISO8859-1", 
				     FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
				     FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 
				     38, 0);
	
	gFontTTF_50Bk = CreateLogFont ("ttf", "arialbk", "ISO8859-1", 
				     FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
				     FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 
				     50, 0);
	gFontTTF_60 = CreateLogFont ("ttf", "arialbk", "ISO8859-1", 
				     FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
				     FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 
				     60, 0);
	gFontTTF_70 =CreateLogFont ("ttf", "arialbk", "ISO8859-1", 
				     FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
				     FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 
			     	     70, 0);
	gFontTTF_75 = CreateLogFont ("ttf", "arialbk", "ISO8859-1", 
				     FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
				     FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 
			     	     80, 0);
	gFontTTF_90Bk = CreateLogFont ("ttf", "arialbk", "ISO8859-1", 
				     FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
				     FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 
			     	     90, 0);		     
	gFontTTF_105 = CreateLogFont ("ttf", "arialbk", "ISO8859-1", 
				      FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
	  				FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 
   					105, 0);
	gFontTTF_120 = CreateLogFont ("ttf", "arialbk", "ISO8859-1", 
				      FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
				      FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 
				      135, 0);

	printf("%s:%d Load fonts OK!\n", __FILE__, __LINE__);
	return 0;
}

/*加载系统图标*/
int LoadSystemIcons()
{
	LoadBitmap(HDC_SCREEN, &gIconAlarmOff, "Icon/AlarmOff.png"); 
	LoadBitmap(HDC_SCREEN, &gIconAudioOff, "Icon/Silence.png"); 
	LoadBitmap(HDC_SCREEN, &gIconAlmAudioPause, "Icon/Pause.png"); 
	LoadBitmap(HDC_SCREEN, &gIconPrinter, "Icon/Printer-on.png"); 
	LoadBitmap(HDC_SCREEN, &gIconPrinterError, "Icon/Printer-off.png"); 
	LoadBitmap(HDC_SCREEN, &gIconPrinting, "Icon/Printing.png"); 
	LoadBitmap(HDC_SCREEN, &gIconAC, "Icon/ac-on.png"); 
	LoadBitmap(HDC_SCREEN, &gIconACOff, "Icon/ac-off.png"); 
	LoadBitmap(HDC_SCREEN, &gIconBattery[0], "Icon/bat/bat-0.png"); 
	LoadBitmap(HDC_SCREEN, &gIconBattery[1], "Icon/bat/bat-20.png"); 
	LoadBitmap(HDC_SCREEN, &gIconBattery[2], "Icon/bat/bat-40.png"); 
	LoadBitmap(HDC_SCREEN, &gIconBattery[3], "Icon/bat/bat-60.png"); 
	LoadBitmap(HDC_SCREEN, &gIconBattery[4], "Icon/bat/bat-80.png"); 
	LoadBitmap(HDC_SCREEN, &gIconBattery[5], "Icon/bat/bat-100.png"); 
	
	LoadBitmap(HDC_SCREEN, &gIconBatteryAC[0], "Icon/bat-ac/bat-0.png"); 
	LoadBitmap(HDC_SCREEN, &gIconBatteryAC[1], "Icon/bat-ac/bat-20.png"); 
	LoadBitmap(HDC_SCREEN, &gIconBatteryAC[2], "Icon/bat-ac/bat-40.png"); 
	LoadBitmap(HDC_SCREEN, &gIconBatteryAC[3], "Icon/bat-ac/bat-60.png"); 
	LoadBitmap(HDC_SCREEN, &gIconBatteryAC[4], "Icon/bat-ac/bat-80.png"); 
	LoadBitmap(HDC_SCREEN, &gIconBatteryAC[5], "Icon/bat-ac/bat-100.png"); 
	
	LoadBitmap(HDC_SCREEN, &gIconHR[0], "Icon/Heart_Big.png"); 
	LoadBitmap(HDC_SCREEN, &gIconHR[1], "Icon/Heart_Small.png"); 
	LoadBitmap(HDC_SCREEN, &gIconDemo, "Icon/demo.png"); 
	LoadBitmap(HDC_SCREEN, &gIconNetStatus[0], "Icon/Net-Error.png");
	LoadBitmap(HDC_SCREEN, &gIconNetStatus[1], "Icon/Net-Online.png");  

	return 0;
}

//加载系统默认设置
int LoadSetup(BYTE bSetType)
{
	unsigned char SetupFile[40];
	unsigned char strCommand[200];
	int iTmp;
	
	printf("bSetType is %d.\n", bSetType);
	switch(bSetType){
		case ADULT:{
			snprintf(SetupFile, sizeof SetupFile, CFGPATH_ADULT);
			gCfgPatient.bObject = ADULT;
		}break;
		case CHILD:{
			snprintf(SetupFile, sizeof SetupFile, CFGPATH_CHILD);
			gCfgPatient.bObject = CHILD;
		}break;
		case BABY:{
			snprintf(SetupFile, sizeof SetupFile, CFGPATH_BABY);
			gCfgPatient.bObject = BABY;
		}break;
		default:{
			return 0;
		}break;
	}
	
	//初始化各模块的数据和配置信息
	InitCfg_ECG(SetupFile, &gCfgEcg);
	InitCfg_Resp(SetupFile, &gCfgResp);
	InitCfg_SpO2(SetupFile, &gCfgSpO2);
	InitCfg_Pulse(SetupFile, &gCfgPulse);
	InitCfg_Temp(SetupFile, &gCfgTemp);
	InitCfg_NIBP(SetupFile, &gCfgNibp);
	InitCfg_IBP1(SetupFile, &gCfgIbp1);
	InitCfg_IBP2(SetupFile, &gCfgIbp2);
	InitCfg_CO2(SetupFile, &gCfgCO2);
	
	//初始化测量模块 2009-12-29 09:19:26
//	InitModule_Ecg();
//	InitModule_Resp();
//	InitModule_SpO2();
//	InitModule_Nibp();
	
	//拷贝该配置为当前配置
 	snprintf(strCommand, sizeof strCommand, "cp %s %s", SetupFile, gFileSetup);
 	printf("Command is %s\n", strCommand);
 	system(strCommand);


// 		
	//保持一些系统选项不变
	//Language
	SetIntValueToResFile(gFileSetup, "SystemSetup", "language",  gCfgSystem.bLanguage, 1);
	//AC Freq
	SetIntValueToResFile(gFileSetup, "SystemSetup", "ac_freq",  gCfgSystem.bACFreq, 1);
	//Line Type 
	SetIntValueToResFile(gFileSetup, "SystemSetup", "line_type",  gCfgSystem.bLineType, 1);

	return 0;
}

//加载用户设置
int LoadResentSetup()
{
	unsigned char SetupFile[40];
	unsigned char strCommand[200];
	int iTmp;
	snprintf(SetupFile, sizeof SetupFile, CFGPATH_RECENT);
	
	//初始化各模块的数据和配置信息
	InitCfg_ECG(SetupFile, &gCfgEcg);
	InitCfg_Resp(SetupFile, &gCfgResp);
	InitCfg_SpO2(SetupFile, &gCfgSpO2);
	InitCfg_Pulse(SetupFile, &gCfgPulse);
	InitCfg_Temp(SetupFile, &gCfgTemp);
	InitCfg_NIBP(SetupFile, &gCfgNibp);
	InitCfg_IBP1(SetupFile, &gCfgIbp1);
	InitCfg_IBP2(SetupFile, &gCfgIbp2);
	InitCfg_CO2(SetupFile, &gCfgCO2);
	
	//初始化测量模块 2009-12-29 09:19:26
//	InitModule_Ecg();
//	InitModule_Resp();
//	InitModule_SpO2();
//	InitModule_Nibp();
	
// 		
	//保持一些系统选项不变
	//Language
	SetIntValueToResFile(gFileSetup, "SystemSetup", "language",  gCfgSystem.bLanguage, 1);
	//AC Freq
	SetIntValueToResFile(gFileSetup, "SystemSetup", "ac_freq",  gCfgSystem.bACFreq, 1);
	//Line Type 
	SetIntValueToResFile(gFileSetup, "SystemSetup", "line_type",  gCfgSystem.bLineType, 1);

	return 0;
}
 
/*
	切换语言
*/
int SwitchLanguage()
{
	LoadLanguageRes();	
	SetParaBtnText();
	//加载报警信息文字
	ReadAlmInfoFromFile();
}

 

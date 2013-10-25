/*************************************************************************** 
 *	Module Name:	AlarmJudgment
 *
 *	Abstract:		报警判断
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-07-03 09:50:53
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "General_Funcs.h"
#include "AlarmManage.h"
//#include "arr.h"

//定义超限报警状态
#define NORMAL		0
#define LOWER		1
#define HIGHER		2
#define LOWEST		3	

//定义脱落报警的状态
#define SENSOR_ON	0
#define SENSOR_OFF	1

//定义测量错误报警状态
#define MEASURE_OK	0
#define MEASURE_ERROR	1

//定义电量报警状态
#define POWER_LOWEST	BATTERY_EMPTY
#define POWER_NORMAL	BATTERY_Q1

//参数报警与参数显示之间的联系变量
extern BOOL gbAlarmNibp_Sys;
extern BOOL gbAlarmNibp_Dia;
extern BOOL gbAlarmNibp_Mean;
extern BOOL gbAlarmTemp_T1;
extern BOOL gbAlarmTemp_T2;
extern BOOL gbAlarmTemp_TD;
extern BOOL gbAlarmEcg_HR;
extern BOOL gbAlarmEcg_ST;
extern BOOL gbAlarmSpO2_Data;
extern BOOL gbAlarmPr_Data;
extern BOOL gbAlarmResp_Data;
extern BOOL gbAlarmCo2_EtCo2;
extern BOOL gbAlarmCo2_FiCo2;
extern BOOL gbAlarmIbp1_Sys;
extern BOOL gbAlarmIbp1_Dia;
extern BOOL gbAlarmIbp1_Mean;
extern BOOL gbAlarmIbp2_Sys;
extern BOOL gbAlarmIbp2_Dia;
extern BOOL gbAlarmIbp2_Mean;

//是否进入报警暂停状态
extern BOOL gbSuspending;

//-------------------------------- 判断体温报警 ------------------------------
//判断体温参数报警
int IsAlm_T1(const PS_TEMP_VALUE pValue, const PS_TEMP_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->wT1;
	
	//判断参数值是否越限
	if(pValue->bOff_T1 || iValue ==0){
		iAlmStatus = NORMAL;
		gbAlarmTemp_T1 = FALSE;
	}
	else{
		if(iValue < (pCfg->wLow_T1)){
			iAlmStatus = LOWER;
			gbAlarmTemp_T1 = TRUE;
			
		}
		else if(iValue > (pCfg->wHigh_T1)){
			iAlmStatus = HIGHER;
			gbAlarmTemp_T1 = TRUE;
		
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmTemp_T1 = FALSE;
		}
	}
	//如果于之前的状态相同，则直接返回；否则根据不同状态判断
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//首先解除原先的状态
		sAlmInfo.bAlmID = AP_T1_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_T1_LOW;
		AlarmLifted_Phy(sAlmInfo);	

		//增加新的状态
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_T1_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_Temp1_High();	
// 					if(B_PRINTF) printf("Add High Alarm   --- T1.\n");
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_T1_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
					NetSend_Alm_Temp1_Low();
// 					if(B_PRINTF) printf("Add Low Alarm   --- T1.\n");
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

int IsAlm_T2(const PS_TEMP_VALUE pValue, const PS_TEMP_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->wT2;

// 	if(B_PRINTF) printf("T1 value is %d, High linit is %d, Low Limit is %d.\n", iValue, pCfg->wHigh_T1, pCfg->wLow_T1);
	
	//判断参数值是否越限
	if(pValue->bOff_T2 || iValue ==0){
		iAlmStatus = NORMAL;
		gbAlarmTemp_T2 = FALSE;
	}
	else{
		if(iValue < (pCfg->wLow_T2)){
			iAlmStatus = LOWER;
			gbAlarmTemp_T2 = TRUE;
			
		}
		else if(iValue > (pCfg->wHigh_T2)){
			iAlmStatus = HIGHER;
			gbAlarmTemp_T2 = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmTemp_T2 = FALSE;
		}
	}
	
	//如果于之前的状态相同，则直接返回；否则根据不同状态判断
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//首先解除原先的状态
		sAlmInfo.bAlmID = AP_T2_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_T2_LOW;
		AlarmLifted_Phy(sAlmInfo);	
		
		//增加新的状态
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_T2_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_Temp2_High();
// 					if(B_PRINTF) printf("Add High Alarm   --- T2.\n");
				}break;                                     
				case LOWER:{
					sAlmInfo.bAlmID = AP_T2_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
					NetSend_Alm_Temp2_Low();
// 					if(B_PRINTF) printf("Add Low Alarm   --- T2.\n");
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

//判断体温报警
int IsAlm_TD(const PS_TEMP_VALUE pValue, const PS_TEMP_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue1, iValue2, iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue1 = pValue->wT1;
	iValue2 = pValue->wT2;	
	iValue = abs(iValue1 - iValue2);
	
	//判断参数值是否越限
	if(pValue->bOff_T2 || pValue->bOff_T1){
		iAlmStatus = NORMAL;
		gbAlarmTemp_TD = FALSE;
	}
	else{
		if(iValue > (pCfg->wHigh_TD)){
			iAlmStatus = HIGHER;
			gbAlarmTemp_TD = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmTemp_TD = FALSE;
		}
	}
	
// 	if(B_PRINTF) printf("TD value is %d, High linit is %d. iPreAlmStatus is %d, iAlmStatus is %d\n", iValue, pCfg->wHigh_TD, 
// 	      iPreAlmStatus, iAlmStatus);
	
	//如果于之前的状态相同，则直接返回；否则根据不同状态判断
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//首先解除原先的状态
		sAlmInfo.bAlmID = AP_TD_HIGH;
		AlarmLifted_Phy(sAlmInfo);	

		//增加新的状态
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_TD_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_TD_High();
// 					if(B_PRINTF) printf("Add High Alarm   --- TD.\n");
				}break;                                     
				default:{//NORMAL
				}break;
			}		
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

//判断体温技术报警
int IsOff_T1(const PS_TEMP_VALUE pValue)
{
	if(pValue == NULL) return -1;
	//前一次的报警状态
	static int iPreAlmStatus = SENSOR_OFF;
	//当前的状态
	int iAlmStatus=SENSOR_OFF;
	B_ALM_INFO sAlmInfo;
	
	//如果报警暂停，则取消脱落报警
	if(gbSuspending) pValue->bConnected_T1 = FALSE;
	//得到当前的报警状态
  	if(pValue->bOff_T1&& pValue->bConnected_T1){
//  	if(pValue->bOff_T1){
		iAlmStatus = SENSOR_OFF;
	}
	else{
		iAlmStatus = SENSOR_ON;
	}
	//与先前的状态比较,如果相同,则直接返回,
	//否则添加 or 解除报警
	if(iPreAlmStatus == iAlmStatus){
// 	if(iPreAlmStatus == iAlmStatus && pValue->bConnected_T1){
		return -1;	
	}
	else{
		sAlmInfo.bAlmID = AT_TEMP_T1SENSOROFF;
		sAlmInfo.bStatus = ALMSTATUS_ALARM;
		sAlmInfo.bClass = ALARM_LOW;
// 		if(pValue->bConnected_T1)
// 			sAlmInfo.bClass = ALARM_LOW;
// 		else
// 			sAlmInfo.bClass = ALARM_INFO;
		if(iAlmStatus == SENSOR_OFF){
			//Add	
			AddAlarm_Tec(sAlmInfo);
			NetSend_Alm_Temp1_Off();
		}
		else{
			//Lift
			AlarmLifted_Tec(sAlmInfo);
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	return 0;
}

int IsOff_T2(const PS_TEMP_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = SENSOR_OFF;
	//当前的状态
	int iAlmStatus=SENSOR_OFF;
	B_ALM_INFO sAlmInfo;
	
	//如果报警暂停，则取消脱落报警
	if(gbSuspending) pValue->bConnected_T2 = FALSE;
		
	//得到当前的报警状态
 	if(pValue->bOff_T2 && pValue->bConnected_T2){
// 	if(pValue->bOff_T2){
		iAlmStatus = SENSOR_OFF;
	}
	else{
		iAlmStatus = SENSOR_ON;
	}
	
	//与先前的状态比较,如果相同,则直接返回,
	//否则添加 or 解除报警
	if(iPreAlmStatus == iAlmStatus){
// 	if(iPreAlmStatus == iAlmStatus && pValue->bConnected_T2){
		return -1;	
	}
	else{
		sAlmInfo.bAlmID = AT_TEMP_T2SENSOROFF;
		sAlmInfo.bStatus = ALMSTATUS_ALARM;
// 		if(pValue->bConnected_T2)
			sAlmInfo.bClass = ALARM_LOW;
// 		else
// 			sAlmInfo.bClass = ALARM_INFO;
		
		if(iAlmStatus == SENSOR_OFF){
			//Add	
			AddAlarm_Tec(sAlmInfo);
			NetSend_Alm_Temp2_Off();
		}
		else{
			//Lift
			AlarmLifted_Tec(sAlmInfo);
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	return 0;
}

//------------------------   
//判断SpO2参数报警
int IsAlm_SpO2(const PS_SPO2_VALUE pValue, const PS_SPO2_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->bSpO2;

	//判断参数值是否越限
	if(pValue->bOff || iValue ==0){
		iAlmStatus = NORMAL;
		gbAlarmSpO2_Data = FALSE;
	}
	else{
		if(iValue < (pCfg->bLow_SpO2)){
			iAlmStatus = LOWER;
			gbAlarmSpO2_Data = TRUE;
			
		}
		else if(iValue > (pCfg->bHigh_SpO2)){
			iAlmStatus = HIGHER;
			gbAlarmSpO2_Data = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmSpO2_Data = FALSE;
		}
	}
	
	//如果于之前的状态相同，则直接返回；否则根据不同状态判断
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//首先解除原先的状态
		sAlmInfo.bAlmID = AP_SPO2_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_SPO2_LOW;
		AlarmLifted_Phy(sAlmInfo);	
		
		//增加新的状态
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_SPO2_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_SpO2_High();
// 					if(B_PRINTF) printf("Add High Alarm   --- SpO2.\n");
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_SPO2_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
					NetSend_Alm_SpO2_Low();
// 					if(B_PRINTF) printf("Add Low Alarm   --- SpO2.\n");
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

//判断SpO2参数报警
int IsAlmLowest_SpO2(const PS_SPO2_VALUE pValue, const PS_SPO2_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->bSpO2;

	//判断参数值是否越限
	if(pValue->bOff || iValue ==0){
		iAlmStatus = NORMAL;
	}
	else{
		if(iValue < (pCfg->bLowest_SpO2)){
			iAlmStatus = LOWEST;
		}
		else{
			iAlmStatus = NORMAL;
		}
	}
	
	//如果于之前的状态相同，则直接返回；否则根据不同状态判断
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//首先解除原先的状态
		sAlmInfo.bAlmID = AP_SPO2_LOWEST;
		AlarmLifted_Phy(sAlmInfo);	
	
		//增加新的状态
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case LOWEST:{
					sAlmInfo.bAlmID = AP_SPO2_LOWEST;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = ALARM_HIGH;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
// 					if(B_PRINTF) printf("Add Lowest Alarm   --- SpO2.\n");
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

//判断SpO2是否脱落
int IsOff_SpO2(const PS_SPO2_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = SENSOR_OFF;
	//当前的状态
	int iAlmStatus=SENSOR_OFF;
	B_ALM_INFO sAlmInfo;
	
	//如果报警暂停，则取消脱落报警
	if(gbSuspending) pValue->bConnected = FALSE;
	
	//得到当前的报警状态
 	if(pValue->bOff && pValue->bConnected){
// 	if(pValue->bOff){
		iAlmStatus = SENSOR_OFF;
	}
	else{
		iAlmStatus = SENSOR_ON;
	}
	
	//与先前的状态比较,如果相同,则直接返回,
	//否则添加 or 解除报警
	if(iPreAlmStatus == iAlmStatus){
// 	if(iPreAlmStatus == iAlmStatus && pValue->bConnected){
		return -1;	
	}
	else{
		sAlmInfo.bAlmID = AT_SPO2_SENSOROFF;
		sAlmInfo.bStatus = ALMSTATUS_ALARM;
		
// 		if( pValue->bConnected)
			sAlmInfo.bClass = ALARM_LOW;
// 		else
// 			sAlmInfo.bClass = ALARM_INFO;
		
		if(iAlmStatus == SENSOR_OFF){
			//Add	
			AddAlarm_Tec(sAlmInfo);
			NetSend_Alm_SpO2_Off();
		}
		else{
			//Lift
			AlarmLifted_Tec(sAlmInfo);
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	return 0;
}
		
//判断SpO2是否出现测量错误
int isErr_SpO2(const PS_SPO2_VALUE pValue)
{
	
	if(pValue == NULL) return -1;
	

	//前一次的报警状态
	static int iPreAlmStatus = MEASURE_OK;
	//当前的状态
	int iAlmStatus=MEASURE_OK;
	B_ALM_INFO sAlmInfo;
		
	//得到当前的报警状态
	if(pValue->bOff || pValue->bErrCode ==0){
		iAlmStatus = MEASURE_OK;
	}
	else{//根据错误代码判断是否出现测量错误
		iAlmStatus = MEASURE_ERROR;	
	}
	
	if(gbHaveNellSpO2) iAlmStatus = MEASURE_OK;
	
	//与先前的状态比较,如果相同,则直接返回,
	//否则添加 or 解除报警
	if(iPreAlmStatus == iAlmStatus){
		return -1;	
	}
	else{
		sAlmInfo.bAlmID = AT_SPO2_ERROR;
		sAlmInfo.bStatus = ALMSTATUS_ALARM;
		//根据不同的测量错误，定义报警级别
		if((pValue->bErrCode) & SPO2_INTERFERE){
			sAlmInfo.bClass = ALARM_LOW;		
// 			if(B_PRINTF) printf("%s:%d SpO2 Alarm Interfere.\n", __FILE__, __LINE__);
		}else if((pValue->bErrCode) & SPO2_SEARCHPULSE){
			sAlmInfo.bClass = ALARM_LOW;		
// 			if(B_PRINTF) printf("%s:%d SpO2 Alarm Search Pulse.\n", __FILE__, __LINE__);
		}else if((pValue->bErrCode) & SPO2_HIGHLIGHT){
			sAlmInfo.bClass = ALARM_LOW;		
// 			if(B_PRINTF) printf("%s:%d SpO2 Alarm High Light.\n", __FILE__, __LINE__);
		}else if((pValue->bErrCode) & SPO2_WEAKSIGNAL){
			sAlmInfo.bClass = ALARM_LOW;	
// 			if(B_PRINTF) printf("%s:%d SpO2 Alarm Weak Signal.\n", __FILE__, __LINE__);	
		}else if((pValue->bErrCode) & SPO2_SIGNALBAD){
			sAlmInfo.bClass = ALARM_LOW;		
// 			if(B_PRINTF) printf("%s:%d SpO2 Alarm Signal Bad.\n", __FILE__, __LINE__);
		}else{
			sAlmInfo.bClass = ALARM_INFO;		
		}

		if(iAlmStatus == MEASURE_ERROR){
			//Add	
			AddAlarm_Tec(sAlmInfo);
		
		}
		else{
			//Lift
			AlarmLifted_Tec(sAlmInfo);
		}
	}

	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

//判断NIBP参数报警
static int IsAlm_NSys(const PS_NIBP_VALUE pValue, const PS_NIBP_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->wSys;

	//判断参数值是否越限
	if(pValue->bCanceled || iValue ==0){
		iAlmStatus = NORMAL;
		gbAlarmNibp_Sys = FALSE;
	}
	else{
		if(iValue < (pCfg->wLow_Sys)){
			iAlmStatus = LOWER;
			gbAlarmNibp_Sys = TRUE;
			
		}
		else if(iValue > (pCfg->wHigh_Sys)){
			iAlmStatus = HIGHER;
			gbAlarmNibp_Sys = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmNibp_Sys = FALSE;
		}
	}
	
	//如果于之前的状态相同，则直接返回；否则根据不同状态判断
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//首先解除原先的状态
		sAlmInfo.bAlmID = AP_NSYS_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_NSYS_LOW;
		AlarmLifted_Phy(sAlmInfo);	
		
		//增加新的状态
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_NSYS_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_NSys_High();
// 					if(B_PRINTF) printf("Add High Alarm   --- NIBP Sys.\n");
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_NSYS_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_NSys_Low();
// 					if(B_PRINTF) printf("Add Low Alarm   --- NIBP Sys.\n");
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

//判断NIBP参数报警
static int IsAlm_NDia(const PS_NIBP_VALUE pValue, const PS_NIBP_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->wDia;

	//判断参数值是否越限
	if(pValue->bCanceled || iValue ==0){
		iAlmStatus = NORMAL;
		gbAlarmNibp_Dia = FALSE;
	}
	else{
		if(iValue < (pCfg->wLow_Dia)){
			iAlmStatus = LOWER;
			gbAlarmNibp_Dia = TRUE;
			
		}
		else if(iValue > (pCfg->wHigh_Dia)){
			iAlmStatus = HIGHER;
			gbAlarmNibp_Dia = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmNibp_Dia = FALSE;
		}
	}
	
	//如果于之前的状态相同，则直接返回；否则根据不同状态判断
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//首先解除原先的状态
		sAlmInfo.bAlmID = AP_NDIA_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_NDIA_LOW;
		AlarmLifted_Phy(sAlmInfo);	
		
		//增加新的状态
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_NDIA_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_NDia_High();
// 					if(B_PRINTF) printf("Add High Alarm   --- NIBP Dia.\n");
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_NDIA_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
					NetSend_Alm_NDia_Low();
// 					if(B_PRINTF) printf("Add Low Alarm   --- NIBP Dia.\n");
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

//判断NIBP参数报警
static int IsAlm_NMean(const PS_NIBP_VALUE pValue, const PS_NIBP_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->wMean;

	//判断参数值是否越限
	if(pValue->bCanceled || iValue ==0){
		iAlmStatus = NORMAL;
		gbAlarmNibp_Mean = FALSE;
	}
	else{
		if(iValue < (pCfg->wLow_Mean)){
			iAlmStatus = LOWER;
			gbAlarmNibp_Mean = TRUE;
			
		}
		else if(iValue > (pCfg->wHigh_Mean)){
			iAlmStatus = HIGHER;
			gbAlarmNibp_Mean = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmNibp_Mean = FALSE;
		}
	}
	
// 	if(B_PRINTF) printf("NIBP Mean is %d. High is %d, Low is %d.\n", iValue, pCfg->wHigh_Mean, pCfg->wLow_Mean);
	
	//如果于之前的状态相同，则直接返回；否则根据不同状态判断
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//首先解除原先的状态
		sAlmInfo.bAlmID = AP_NMEAN_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_NMEAN_LOW;
		AlarmLifted_Phy(sAlmInfo);	
		
		//增加新的状态
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_NMEAN_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_NMean_High();
// 					if(B_PRINTF) printf("Add High Alarm   --- NIBP Mean.\n");
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_NMEAN_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
					NetSend_Alm_NMean_Low();
// 					if(B_PRINTF) printf("Add Low Alarm   --- NIBP Mean.\n");
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

//当改变报警源时使用
static int ClearNibpAlarm()
{
	B_ALM_INFO sAlmInfo;
	
	//解除原先的状态
	sAlmInfo.bAlmID = AP_NSYS_HIGH;
	AlarmLifted_Phy(sAlmInfo);	
	sAlmInfo.bAlmID = AP_NSYS_LOW;
	AlarmLifted_Phy(sAlmInfo);	
	gbAlarmNibp_Sys = FALSE;
	sAlmInfo.bAlmID = AP_NDIA_HIGH;
	AlarmLifted_Phy(sAlmInfo);	
	sAlmInfo.bAlmID = AP_NDIA_LOW;
	AlarmLifted_Phy(sAlmInfo);	
	gbAlarmNibp_Dia = FALSE;
	sAlmInfo.bAlmID = AP_NMEAN_HIGH;
	AlarmLifted_Phy(sAlmInfo);	
	sAlmInfo.bAlmID = AP_NMEAN_LOW;
	AlarmLifted_Phy(sAlmInfo);	
	gbAlarmNibp_Mean = FALSE;

	return 0;
}

/*
	判断NIBP参数是否报警
*/
int IsAlm_NIBP(BOOL bChangedSource, BOOL bChangedClass)
{
	BOOL bReset = bChangedSource;	
	BOOL bChanged = bChangedClass;
	
	if(bReset){
		ClearNibpAlarm();
		bChanged = TRUE;
	}

	//根据不同的报警源进行判断               
	if(B_PRINTF) printf("%s:%d NIBP Alarm Source is %d.\n", __FILE__, __LINE__, gCfgNibp.bAlmSource);
	switch(gCfgNibp.bAlmSource){
		case NIBP_ALM_SRC_S:{
			IsAlm_NSys(&gValueNibp, &gCfgNibp, bChanged);
		}break;
		case NIBP_ALM_SRC_M:{
			IsAlm_NMean(&gValueNibp, &gCfgNibp, bChanged);
		}break;
		case NIBP_ALM_SRC_D:{
			IsAlm_NDia(&gValueNibp, &gCfgNibp, bChanged);
		}break;
		case NIBP_ALM_SRC_SM:{
			IsAlm_NSys(&gValueNibp, &gCfgNibp, bChanged);
			IsAlm_NMean(&gValueNibp, &gCfgNibp, bChanged);
		}break;
		case NIBP_ALM_SRC_MD:{
			IsAlm_NMean(&gValueNibp, &gCfgNibp, bChanged);
			IsAlm_NDia(&gValueNibp, &gCfgNibp, bChanged);
		}break;
		case NIBP_ALM_SRC_SD:{
			IsAlm_NSys(&gValueNibp, &gCfgNibp, bChanged);
			IsAlm_NDia(&gValueNibp, &gCfgNibp, bChanged);
		}break;
		default:{//NIBP_ALM_SRC_ALL
			IsAlm_NSys(&gValueNibp, &gCfgNibp, bChanged);
			IsAlm_NDia(&gValueNibp, &gCfgNibp, bChanged);
			IsAlm_NMean(&gValueNibp, &gCfgNibp, bChanged);
		}break;
	}
	
	return 0;
}

extern BOOL gbNibpProtect;
//判断NIBP是否出现测量错误
int isErr_Nibp(const PS_NIBP_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = MEASURE_OK;
	//当前的状态
	int iAlmStatus=MEASURE_OK;
	B_ALM_INFO sAlmInfo;
		
	//得到当前的报警状态
	if((pValue->bCanceled || pValue->wErrCode ==0) && (!gbNibpProtect)){
		iAlmStatus = MEASURE_OK;
	}
	else{//根据错误代码判断是否出现测量错误
		iAlmStatus = MEASURE_ERROR;	
	}
	
	if(B_PRINTF) printf("%s:%d NIBP Errcode is 0x%.2X gbNibpProtect:%d.\n", __FILE__, __LINE__, pValue->wErrCode, gbNibpProtect);
	
	//与先前的状态比较,如果相同,则直接返回,
	//否则添加 or 解除报警
	if(iPreAlmStatus == iAlmStatus){
		if(B_PRINTF) printf("%s:%d NIBP Errcode is 0x%.2X   status:%d.\n", __FILE__, __LINE__, pValue->wErrCode, iAlmStatus);
		return -1;	
	}
	else{
		sAlmInfo.bAlmID = AT_NIBP_ERROR;
		sAlmInfo.bStatus = ALMSTATUS_ALARM;
		//根据不同的测量错误，定义报警级别
		if((pValue->wErrCode) & NIBP_INFO_EXCESSIVE_MOTION){
			sAlmInfo.bClass = ALARM_LOW;		
		}else if((pValue->wErrCode) & NIBP_INFO_CUFF_MISTAKE){
			sAlmInfo.bClass = ALARM_LOW;		
		}else if((pValue->wErrCode) & NIBP_INFO_CUFF_TOOLAX){
			sAlmInfo.bClass = ALARM_LOW;		
		}else if((pValue->wErrCode) & NIBP_INFO_WEAK_SIGNAL){
			sAlmInfo.bClass = ALARM_LOW;	
		}else if((pValue->wErrCode) & NIBP_INFO_INTERFERENCE){
			sAlmInfo.bClass = ALARM_LOW;		
		}else if((pValue->wErrCode) & NIBP_INFO_OVER_RANGE){
			sAlmInfo.bClass = ALARM_HIGH;		
		}else if((pValue->wErrCode) & NIBP_INFO_OVERPRESSURE){
			sAlmInfo.bClass = ALARM_HIGH;		
		}else if((pValue->wErrCode) & NIBP_INFO_TIMEOUT){
			sAlmInfo.bClass = ALARM_HIGH;		
		}else if((pValue->wErrCode) & NIBP_INFO_ERROR){
			sAlmInfo.bClass = ALARM_LOW;		
		}else if((pValue->wErrCode) & NIBP_INFO_SIGNAL_SATURATION){
			sAlmInfo.bClass = ALARM_LOW;		
		}else if((pValue->wErrCode) & NIBP_INFO_MODULE_FAULT){
			sAlmInfo.bClass = ALARM_HIGH;		
		}
		else{
			sAlmInfo.bClass = ALARM_INFO;		
		}

		if(iAlmStatus == MEASURE_ERROR){
			//Add	
			AddAlarm_Tec(sAlmInfo);
		}
		else{
			//Lift
			AlarmLifted_Tec(sAlmInfo);
		}
	}

	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

//判断NIBP是否出现测量错误
int isErr_iBioNibp(const PS_NIBP_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = MEASURE_OK;
	//当前的状态
	int iAlmStatus=MEASURE_OK;
	B_ALM_INFO sAlmInfo;

	switch(pValue->wFDB){
		case NBP_FDB_MAN_START_ABT:
		case NBP_FDB_AUTO_START_ABT:
		case NBP_FDB_STAT_START_ABT:
		case NBP_FDB_STC_START_ABT:
		case NBP_FDB_LEAK_START_ABT:
		case NBP_FDB_PVT_START_ABT:
		case NBP_FDB_OVP_START_ABT:
		case NBP_FDB_CAL_START_ABT:
		break;	
			
	}
		
	//得到当前的报警状态
	if((pValue->bCanceled || pValue->wFDB==0) && (!gbNibpProtect)){
		iAlmStatus = MEASURE_OK;
	}
	else{//根据错误代码判断是否出现测量错误
		iAlmStatus = MEASURE_ERROR;	
	}
	
	//if(B_PRINTF) printf("%s:%d NIBP Errcode is 0x%.2X gbNibpProtect:%d.\n", __FILE__, __LINE__, pValue->wErrCode, gbNibpProtect);
	
	//与先前的状态比较,如果相同,则直接返回,
	//否则添加 or 解除报警
	if(iPreAlmStatus == iAlmStatus){
//		if(B_PRINTF) printf("%s:%d NIBP Errcode is 0x%.2X   status:%d.\n", __FILE__, __LINE__, pValue->wErrCode, iAlmStatus);
		return -1;	
	}
	else{
		sAlmInfo.bAlmID = AT_NIBP_ERROR;
		sAlmInfo.bStatus = ALMSTATUS_ALARM;
		
		
		if(iAlmStatus == MEASURE_ERROR){
			//Add	
			AddAlarm_Tec(sAlmInfo);
		}
		else{
			//Lift
			AlarmLifted_Tec(sAlmInfo);
		}
	}

	iPreAlmStatus = iAlmStatus;
	
	return 0;
}
//判断PR是否报警
int IsAlm_PR(const PS_PULSE_VALUE pValue, const PS_PULSE_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->wPR;
	//判断参数值是否越限
	if(pValue->bOff || iValue ==0){
		iAlmStatus = NORMAL;
		gbAlarmPr_Data = FALSE;
	}
	else{
		if(iValue < (pCfg->wLow_PR)){
			iAlmStatus = LOWER;
			gbAlarmPr_Data = TRUE;
			
		}
		else if(iValue > (pCfg->wHigh_PR)){
			iAlmStatus = HIGHER;
			gbAlarmPr_Data = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmPr_Data = FALSE;
		}
	}
	
	//如果于之前的状态相同，则直接返回；否则根据不同状态判断
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//首先解除原先的状态
		sAlmInfo.bAlmID = AP_PR_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_PR_LOW;
		AlarmLifted_Phy(sAlmInfo);	
		
		//增加新的状态
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_PR_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_PR_High();
// 					if(B_PRINTF) printf("----------------------Add High Alarm   --- PR.\n");
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_PR_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
					NetSend_Alm_PR_Low();
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

//判断ECG测量参数是否报警
int IsAlm_HR(const PS_ECG_VALUE pValue, const PS_ECG_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->wHR;

// 	if(B_PRINTF) printf("-----------------HR: iPreAlmStatus is %d, iAlmStatus is %d.\n", iPreAlmStatus, iAlmStatus);
	//判断参数值是否越限
  	if(pValue->bRAOff || iValue<=0){
//	if(pValue->bRAOff){
		iAlmStatus = NORMAL;
		gbAlarmEcg_HR = FALSE;
	}
	else{
		if(iValue < (pCfg->wLow_HR)){
			iAlmStatus = LOWER;
			gbAlarmEcg_HR = TRUE;
			
		}
		else if(iValue > (pCfg->wHigh_HR)){
			iAlmStatus = HIGHER;
			gbAlarmEcg_HR = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmEcg_HR = FALSE;
		}
	}
	
// 	if(B_PRINTF) printf("HR: iPreAlmStatus is %d, iAlmStatus is %d.\n", iPreAlmStatus, iAlmStatus);
	
	//如果于之前的状态相同，则直接返回；否则根据不同状态判断
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//首先解除原先的状态
		sAlmInfo.bAlmID = AP_HR_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_HR_LOW;
		AlarmLifted_Phy(sAlmInfo);	
		
		//增加新的状态
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_HR_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_HR_High();
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_HR_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
					NetSend_Alm_HR_Low();	
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

//判断呼吸参数报警
int IsAlm_RR(const PS_RESP_VALUE pValue, const PS_RESP_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->wRR;
	
	//判断参数值是否越限
	if(pValue->bOff || gCfgResp.bSwitch == SWITCH_OFF){
		iAlmStatus = NORMAL;
		gbAlarmResp_Data = FALSE;
	}
	else{
		if(iValue < (pCfg->wLow_RR)){
			iAlmStatus = LOWER;
			gbAlarmResp_Data = TRUE;
			
		}
		else if(iValue > (pCfg->wHigh_RR)){
			iAlmStatus = HIGHER;
			gbAlarmResp_Data = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmResp_Data = FALSE;
		}
	}
	
	//如果于之前的状态相同，则直接返回；否则根据不同状态判断
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//首先解除原先的状态
		sAlmInfo.bAlmID = AP_RR_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_RR_LOW;
		AlarmLifted_Phy(sAlmInfo);	

		//增加新的状态
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_RR_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_RR_High();
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_RR_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
					NetSend_Alm_RR_Low();
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}


//判断ECG脱落报警
int IsOff_LA(const PS_ECG_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = SENSOR_OFF;
	//当前的状态
	int iAlmStatus=SENSOR_OFF;
	B_ALM_INFO sAlmInfo;
	
	//如果报警暂停，则取消脱落报警
	if(gbSuspending) pValue->bConnected = FALSE;
	
	//得到当前的报警状态
 	if(pValue->bLAOff && pValue->bConnected){
// 	if(pValue->bLAOff){
		iAlmStatus = SENSOR_OFF;
	}
	else{
		iAlmStatus = SENSOR_ON;
	}
	
	//与先前的状态比较,如果相同,则直接返回,
	//否则添加 or 解除报警
	if(iPreAlmStatus == iAlmStatus){
// 	if(iPreAlmStatus == iAlmStatus && pValue->bConnected){
		return -1;	
	}
	else{
		sAlmInfo.bAlmID = AT_ECGOFF_LA;
		sAlmInfo.bStatus = ALMSTATUS_ALARM;
// 		if(pValue->bConnected)
			sAlmInfo.bClass = ALARM_LOW;
// 		else
// 			sAlmInfo.bClass = ALARM_INFO;
		if(iAlmStatus == SENSOR_OFF){
			//Add	
			AddAlarm_Tec(sAlmInfo);
			NetSend_Alm_LA_Off();
// 			if(B_PRINTF) printf("Add Tec Alarm   --- LA Off.\n");
		}
		else{
			//Lift
			AlarmLifted_Tec(sAlmInfo);
// 			if(B_PRINTF) printf("Lift Tec Alarm   --- LA Off.\n");
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	return 0;
}

int IsOff_LL(const PS_ECG_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = SENSOR_OFF;
	//当前的状态
	int iAlmStatus=SENSOR_OFF;
	B_ALM_INFO sAlmInfo;
	
	//得到当前的报警状态
 	if(pValue->bLLOff && pValue->bConnected){
// 	if(pValue->bLLOff){
		iAlmStatus = SENSOR_OFF;
	}
	else{
		iAlmStatus = SENSOR_ON;
	}
	
	//与先前的状态比较,如果相同,则直接返回,
	//否则添加 or 解除报警
	if(iPreAlmStatus == iAlmStatus){
// 	if(iPreAlmStatus == iAlmStatus && pValue->bConnected){
		return -1;	
	}
	else{
		sAlmInfo.bAlmID = AT_ECGOFF_LL;
		sAlmInfo.bStatus = ALMSTATUS_ALARM;
// 		if(pValue->bConnected)
			sAlmInfo.bClass = ALARM_LOW;
// 		else
// 			sAlmInfo.bClass = ALARM_INFO;
		
		if(iAlmStatus == SENSOR_OFF){
			//Add	
			AddAlarm_Tec(sAlmInfo);
			NetSend_Alm_LL_Off();
// 			if(B_PRINTF) printf("Add Tec Alarm   --- LL Off.\n");
		}
		else{
			//Lift
			AlarmLifted_Tec(sAlmInfo);
// 			if(B_PRINTF) printf("Lift Tec Alarm   --- LL Off.\n");
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	return 0;
}

int IsOff_RA(const PS_ECG_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = SENSOR_OFF;
	//当前的状态
	int iAlmStatus=SENSOR_OFF;
	B_ALM_INFO sAlmInfo;
	
	//如果报警暂停，则取消脱落报警
	if(gbSuspending) pValue->bConnected = FALSE;
	
	//得到当前的报警状态
 	if(pValue->bRAOff && pValue->bConnected){
// 	if(pValue->bRAOff){
		iAlmStatus = SENSOR_OFF;
	}
	else{
		iAlmStatus = SENSOR_ON;
	}
	
	//与先前的状态比较,如果相同,则直接返回,
	//否则添加 or 解除报警
	if(iPreAlmStatus == iAlmStatus){
// 	if(iPreAlmStatus == iAlmStatus && pValue->bConnected){
		return -1;	
	}
	else{
		sAlmInfo.bAlmID = AT_ECGOFF_RA;
		sAlmInfo.bStatus = ALMSTATUS_ALARM;
// 		if(pValue->bConnected)
			sAlmInfo.bClass = ALARM_LOW;
// 		else
// 			sAlmInfo.bClass = ALARM_INFO;
		if(iAlmStatus == SENSOR_OFF){
			//Add	
			AddAlarm_Tec(sAlmInfo);
			NetSend_Alm_RA_Off();
// 			if(B_PRINTF) printf("Add Tec Alarm   --- RA Off.\n");
		}
		else{
			//Lift
			AlarmLifted_Tec(sAlmInfo);
// 			if(B_PRINTF) printf("Lift Tec Alarm   --- RA Off.\n");
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	return 0;
}

int IsOff_V(const PS_ECG_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = SENSOR_OFF;
	//当前的状态
	int iAlmStatus=SENSOR_OFF;
	B_ALM_INFO sAlmInfo;
	
	//如果报警暂停，则取消脱落报警
	if(gbSuspending) pValue->bConnected = FALSE;
	
	//得到当前的报警状态
 	if(pValue->bVOff && pValue->bConnected){
// 	if(pValue->bVOff ){
		iAlmStatus = SENSOR_OFF;
	}
	else{
		iAlmStatus = SENSOR_ON;
	}
	
	//与先前的状态比较,如果相同,则直接返回,
	//否则添加 or 解除报警
	if(iPreAlmStatus == iAlmStatus){
// 	if(iPreAlmStatus == iAlmStatus && pValue->bConnected){
		return -1;	
	}
	else{
		sAlmInfo.bAlmID = AT_ECGOFF_V;
		sAlmInfo.bStatus = ALMSTATUS_ALARM;
// 		if(pValue->bConnected)
			sAlmInfo.bClass = ALARM_LOW;
// 		else
// 			sAlmInfo.bClass = ALARM_INFO;
		if(iAlmStatus == SENSOR_OFF){
			//Add	
			AddAlarm_Tec(sAlmInfo);
			NetSend_Alm_V_Off();
// 			if(B_PRINTF) printf("Add Tec Alarm   --- V Off.\n");
		}
		else{
			//Lift
			AlarmLifted_Tec(sAlmInfo);
// 			if(B_PRINTF) printf("Lift Tec Alarm   --- V Off.\n");
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	return 0;
}

extern BOOL gbApneaTimeChanged;
//判断窒息报警,在1s的定时器调用
int IsAlm_Apnea(const PS_RESP_VALUE pValue, const PS_RESP_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	//窒息计时器
	static int iTimerCount = 0;
	
	iValue = pValue->wRR;
//   	if(B_PRINTF) printf("(%s:%d) bOff:%d,,bchanged:%d,,apneatime=%d,,switch=%d.\n", __FILE__, __LINE__,pValue->bOff,bChanged,gCfgResp.wApneaTime,gCfgResp.bSwitch );
	//如果呼吸脱落,或者改变时间, 或者关闭窒息报警(时间为0),则复位计时器,如果此前是报警状态,则取消报警状态
	if(pValue->bOff 
		  || bChanged 
		  || gCfgResp.bApnea ==0 
		  || gCfgResp.bSwitch == SWITCH_OFF){

		if(bChanged) gbApneaTimeChanged = FALSE;
		
		iTimerCount = 0;
		if(iPreAlmStatus != NORMAL){
			sAlmInfo.bAlmID = AP_APNEA;
			AlarmLifted_Phy(sAlmInfo);	
			iPreAlmStatus = NORMAL;
// 			if(B_PRINTF) printf("%s:%d Lifted Apnea.\n", __FILE__, __LINE__);
		}
	}
	else if(iValue>0){
		//如果呼吸率不为0,则复位计时器,如果此前是报警状态,则取消报警状态
		iTimerCount = 0;
		if(iPreAlmStatus != NORMAL){
			sAlmInfo.bAlmID = AP_APNEA;
			AlarmLifted_Phy(sAlmInfo);	
			iPreAlmStatus = NORMAL;
// 			if(B_PRINTF) printf("%s:%d Lifted Apnea.\n", __FILE__, __LINE__);
		}
	}
	else{
 //		if(B_PRINTF) printf("%s:%d Debug Apnea iTimerCount:%d,,%d.\n", __FILE__, __LINE__, iTimerCount,,gValueResp.bApneaAlm);
		//导联未脱落, 且呼吸率为0的情况
		//计时器到时,如果此前不是报警状态,则添加报警状态
		if(iTimerCount >=gCfgResp.wApneaTime&&gValueResp.bApneaAlm!=0){
			if(iPreAlmStatus == NORMAL){
				sAlmInfo.bAlmID = AP_APNEA;
				sAlmInfo.bStatus = ALMSTATUS_ALARM;
				//TODO:此处可能要单独处理窒息报警的级别
				sAlmInfo.bClass = pCfg->bAlmControl;
				sAlmInfo.bPrint = pCfg->bPrnControl;
				AddAlarm_Phy(sAlmInfo);	
 //				if(B_PRINTF) printf("%s:%d Add Apnea.\n", __FILE__, __LINE__);
				iPreAlmStatus = LOWEST;
				NetSend_Alm_RR_Apnea();
			}
		}
		else{
			iTimerCount =(iTimerCount +1)%100;
		}
	}
	
	return 0;
}
//判断是否CVA报警,是心电信号叠加在呼吸波上的干扰，1:有心电干扰
int IsAlm_CVA(const PS_RESP_VALUE pValue, const PS_RESP_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	//窒息计时器
	static int iTimerCount = 0;
	
	iValue = pValue->bCVAAlm;
//CVA报警时呼吸率为无效值：-100。
	if(!pValue->bOff&&pValue->bInval&&iValue>0){
		iAlmStatus = LOWER;
	}else{
		iAlmStatus = NORMAL;
	}

	//与先前的状态比较,如果相同,则直接返回,
	//否则添加 or 解除报警
	if(iPreAlmStatus == iAlmStatus){
// 	if(iPreAlmStatus == iAlmStatus && pValue->bConnected){
		return -1;	
	}
	else{
		sAlmInfo.bAlmID = AT_CVA;
		sAlmInfo.bStatus = ALMSTATUS_ALARM;
		sAlmInfo.bClass = ALARM_MID;
		if(iAlmStatus == LOWER){
			//Add	
			AddAlarm_Tec(sAlmInfo);
 			if(B_PRINTF) printf("Add Tec Alarm   --- RESP CVA.\n");
		}
		else{
			//Lift
			AlarmLifted_Tec(sAlmInfo);
 			if(B_PRINTF) printf("Lift Tec Alarm   --- LA Off.\n");
		}
	
	}
	
	iPreAlmStatus = iAlmStatus;

	return 0;
}


//判断低电量技术报警
int IsLow_Power(const PPOWER_STATUS pValue)
{
	//前一次的报警状态
	static int iPreAlmStatus = POWER_LOWEST;
	//当前的状态
	int iAlmStatus=POWER_LOWEST;
	B_ALM_INFO sAlmInfo;
	
	//得到当前的报警状态
	//如果正在使用交流电，则忽略低电量报警
	if(pValue->bBatteryStatus == BATTERY_EMPTY && pValue->bPowerType == POWER_DC){
		iAlmStatus = POWER_LOWEST;
	}
	else{
		iAlmStatus = POWER_NORMAL;
	}
	
	//与先前的状态比较,如果相同,则直接返回,
	//否则添加 or 解除报警
	if(iPreAlmStatus == iAlmStatus){
		return -1;	
	}
	else{
		sAlmInfo.bAlmID = AT_BATTERY_LOW;
		sAlmInfo.bStatus = ALMSTATUS_ALARM;
		sAlmInfo.bClass = ALARM_HIGH;
		if(iAlmStatus == POWER_LOWEST){
			//Add	
			AddAlarm_Tec(sAlmInfo);
			NetSend_Alm_Battery_Low();
// 			if(B_PRINTF) printf("%s:%d Add Power Lowest Alarm.\n", __FILE__, __LINE__);
		}
		else{
			//Lift
			AlarmLifted_Tec(sAlmInfo);
// 			if(B_PRINTF) printf("%s:%d Lift Power Lowest Alarm.\n", __FILE__, __LINE__);
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	return 0;
}

int IsAlm_EtCO2(const PS_CO2_VALUE pValue, const PS_CO2_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->wEtCO2;
	
	//判断参数值是否越限
	if(pValue->bOff || iValue ==0){
		iAlmStatus = NORMAL;
		gbAlarmCo2_EtCo2 = FALSE;
	}
	else{
		if(iValue < (pCfg->wLow_EtCO2)){
			iAlmStatus = LOWER;
			gbAlarmCo2_EtCo2 = TRUE;
			
		}
		else if(iValue > (pCfg->wHigh_EtCO2)){
			iAlmStatus = HIGHER;
			gbAlarmCo2_EtCo2 = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmCo2_EtCo2 = FALSE;
		}
	}
	
	//如果于之前的状态相同，则直接返回；否则根据不同状态判断
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//首先解除原先的状态
		sAlmInfo.bAlmID = AP_ETCO2_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_ETCO2_LOW;
		AlarmLifted_Phy(sAlmInfo);	

		//增加新的状态
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_ETCO2_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_EtCO2_High();
					if(B_PRINTF) printf("Add High Alarm   --- EtCO2.\n");
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_ETCO2_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
					NetSend_Alm_EtCO2_Low();
					if(B_PRINTF) printf("Add Low Alarm   --- EtCO2.\n");
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

int IsAlm_FiCO2(const PS_CO2_VALUE pValue, const PS_CO2_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->wFiCO2;
	
	//判断参数值是否越限
	if(pValue->bOff){
		iAlmStatus = NORMAL;
		gbAlarmCo2_FiCo2 = FALSE;
	}
	else{
		if(iValue > (pCfg->wHigh_FiCO2)){
			iAlmStatus = HIGHER;
			gbAlarmCo2_FiCo2 = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmCo2_FiCo2 = FALSE;
		}
	}
	
	//如果于之前的状态相同，则直接返回；否则根据不同状态判断
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//首先解除原先的状态
		sAlmInfo.bAlmID = AP_FICO2_HIGH;
		AlarmLifted_Phy(sAlmInfo);	

		//增加新的状态
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_FICO2_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_FiCO2_High();
 					if(B_PRINTF) printf("Add High Alarm   --- FiCO2.\n");
				}break;                                     
				default:{//NORMAL
				}break;
			}		
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

//判断IBP参数报警
static int IsAlm_ISys1(const PS_IBP_VALUE pValue, const PS_IBP_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->iSys;

	//判断参数值是否越限
	if(pValue->bOff){
		iAlmStatus = NORMAL;
		gbAlarmIbp1_Sys = FALSE;
	}
	else{
		if(iValue < (pCfg->iLow_Sys)){
			iAlmStatus = LOWER;
			gbAlarmIbp1_Sys = TRUE;
			
		}
		else if(iValue > (pCfg->iHigh_Sys)){
			iAlmStatus = HIGHER;
			gbAlarmIbp1_Sys = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmIbp1_Sys = FALSE;
		}
	}
	
	//如果于之前的状态相同，则直接返回；否则根据不同状态判断
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//首先解除原先的状态
		sAlmInfo.bAlmID = AP_ISYS1_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_ISYS1_LOW;
		AlarmLifted_Phy(sAlmInfo);	
		
		//增加新的状态
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_ISYS1_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_ISys1_High();
 					if(B_PRINTF) printf("Add High Alarm   --- IBP1 Sys.\n");
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_ISYS1_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
					NetSend_Alm_ISys1_Low();
 					if(B_PRINTF) printf("Add Low Alarm   --- IBP1 Sys.\n");
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}

	iPreAlmStatus = iAlmStatus;
	return 0;
}

static int IsAlm_IDia1(const PS_IBP_VALUE pValue, const PS_IBP_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->iDia;

	//判断参数值是否越限
	if(pValue->bOff){
		iAlmStatus = NORMAL;
		gbAlarmIbp1_Dia = FALSE;
	}
	else{
		if(iValue < (pCfg->iLow_Dia)){
			iAlmStatus = LOWER;
			gbAlarmIbp1_Dia = TRUE;
			
		}
		else if(iValue > (pCfg->iHigh_Dia)){
			iAlmStatus = HIGHER;
			gbAlarmIbp1_Dia = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmIbp1_Dia = FALSE;
		}
	}
	
	//如果于之前的状态相同，则直接返回；否则根据不同状态判断
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//首先解除原先的状态
		sAlmInfo.bAlmID = AP_IDIA1_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_IDIA1_LOW;
		AlarmLifted_Phy(sAlmInfo);	
		
		//增加新的状态
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_IDIA1_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_IDia1_High();
					if(B_PRINTF) printf("Add High Alarm   --- IBP1 Dia.\n");
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_IDIA1_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
					NetSend_Alm_IDia1_Low();
					if(B_PRINTF) printf("Add Low Alarm   --- IBP1 Dia.\n");
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}

	iPreAlmStatus = iAlmStatus;
	return 0;
}

static int IsAlm_IMean1(const PS_IBP_VALUE pValue, const PS_IBP_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->iMean;

	//判断参数值是否越限
	if(pValue->bOff){
		iAlmStatus = NORMAL;
		gbAlarmIbp1_Mean = FALSE;
	}
	else{
		if(iValue < (pCfg->iLow_Mean)){
			iAlmStatus = LOWER;
			gbAlarmIbp1_Mean = TRUE;
			
		}
		else if(iValue > (pCfg->iHigh_Mean)){
			iAlmStatus = HIGHER;
			gbAlarmIbp1_Mean = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmIbp1_Mean = FALSE;
		}
	}
	
	//如果于之前的状态相同，则直接返回；否则根据不同状态判断
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//首先解除原先的状态
		sAlmInfo.bAlmID = AP_IMEAN1_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_IMEAN1_LOW;
		AlarmLifted_Phy(sAlmInfo);	
		
		//增加新的状态
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_IMEAN1_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_IMean1_High();
					if(B_PRINTF) printf("Add High Alarm   --- IBP1 Mean.\n");
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_IMEAN1_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
					NetSend_Alm_IMean1_Low();
					if(B_PRINTF) printf("Add Low Alarm   --- IBP1 Mean.\n");
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}

	iPreAlmStatus = iAlmStatus;
	return 0;
}

//当改变报警源时使用
static int ClearIbp1Alarm()
{
	B_ALM_INFO sAlmInfo;
	
	//解除原先的状态
	sAlmInfo.bAlmID = AP_ISYS1_HIGH;
	AlarmLifted_Phy(sAlmInfo);	
	sAlmInfo.bAlmID = AP_ISYS1_LOW;
	AlarmLifted_Phy(sAlmInfo);	
	gbAlarmIbp1_Sys = FALSE;
	sAlmInfo.bAlmID = AP_IDIA1_HIGH;
	AlarmLifted_Phy(sAlmInfo);	
	sAlmInfo.bAlmID = AP_IDIA1_LOW;
	AlarmLifted_Phy(sAlmInfo);	
	gbAlarmIbp1_Dia = FALSE;
	sAlmInfo.bAlmID = AP_IMEAN1_HIGH;
	AlarmLifted_Phy(sAlmInfo);	
	sAlmInfo.bAlmID = AP_IMEAN1_LOW;
	AlarmLifted_Phy(sAlmInfo);	
	gbAlarmIbp1_Mean = FALSE;

	return 0;
}

/*
	判断IBP1参数是否报警
*/
int IsAlm_IBP1(BOOL bChangedSource, BOOL bChangedClass)
{
	BOOL bReset = bChangedSource;	
	BOOL bChanged = bChangedClass;
	
	if(bReset){
		ClearIbp1Alarm();
		bChanged = TRUE;
	}

	//根据不同的报警源进行判断               
// 	if(B_PRINTF) printf("%s:%d IBP Alarm Source is %d.\n", __FILE__, __LINE__, gCfgIbp1.bAlmSource);
	switch(gCfgIbp1.bAlmSource){
		case IBP_ALM_SRC_S:{
			IsAlm_ISys1(&gValueIbp1, &gCfgIbp1, bChanged);
		}break;
		case IBP_ALM_SRC_M:{
			IsAlm_IMean1(&gValueIbp1, &gCfgIbp1, bChanged);
		}break;
		case IBP_ALM_SRC_D:{
			IsAlm_IDia1(&gValueIbp1, &gCfgIbp1, bChanged);
		}break;
		case IBP_ALM_SRC_SM:{
			IsAlm_ISys1(&gValueIbp1, &gCfgIbp1, bChanged);
			IsAlm_IMean1(&gValueIbp1, &gCfgIbp1, bChanged);
		}break;
		case IBP_ALM_SRC_MD:{
			IsAlm_IMean1(&gValueIbp1, &gCfgIbp1, bChanged);
			IsAlm_IDia1(&gValueIbp1, &gCfgIbp1, bChanged);
		}break;
		case IBP_ALM_SRC_SD:{
			IsAlm_ISys1(&gValueIbp1, &gCfgIbp1, bChanged);
			IsAlm_IDia1(&gValueIbp1, &gCfgIbp1, bChanged);
		}break;
		default:{//IBP_ALM_SRC_ALL
			IsAlm_ISys1(&gValueIbp1, &gCfgIbp1, bChanged);
			IsAlm_IDia1(&gValueIbp1, &gCfgIbp1, bChanged);
			IsAlm_IMean1(&gValueIbp1, &gCfgIbp1, bChanged);
		}break;
	}
	
	return 0;
}

//判断IBP参数报警
static int IsAlm_ISys2(const PS_IBP_VALUE pValue, const PS_IBP_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->iSys;

	//判断参数值是否越限
	if(pValue->bOff){
		iAlmStatus = NORMAL;
		gbAlarmIbp2_Sys = FALSE;
	}
	else{
		if(iValue < (pCfg->iLow_Sys)){
			iAlmStatus = LOWER;
			gbAlarmIbp2_Sys = TRUE;
			
		}
		else if(iValue > (pCfg->iHigh_Sys)){
			iAlmStatus = HIGHER;
			gbAlarmIbp2_Sys = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmIbp2_Sys = FALSE;
		}
	}
	
	//如果于之前的状态相同，则直接返回；否则根据不同状态判断
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//首先解除原先的状态
		sAlmInfo.bAlmID = AP_ISYS2_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_ISYS2_LOW;
		AlarmLifted_Phy(sAlmInfo);	
		
		//增加新的状态
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_ISYS2_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_ISys2_High();
					if(B_PRINTF) printf("Add High Alarm   --- IBP2 Sys.\n");
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_ISYS2_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
					NetSend_Alm_ISys2_Low();
					if(B_PRINTF) printf("Add Low Alarm   --- IBP2 Sys.\n");
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}

	iPreAlmStatus = iAlmStatus;
	return 0;
}

static int IsAlm_IDia2(const PS_IBP_VALUE pValue, const PS_IBP_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->iDia;

	//判断参数值是否越限
	if(pValue->bOff){
		iAlmStatus = NORMAL;
		gbAlarmIbp2_Dia = FALSE;
	}
	else{
		if(iValue < (pCfg->iLow_Dia)){
			iAlmStatus = LOWER;
			gbAlarmIbp2_Dia = TRUE;
			
		}
		else if(iValue > (pCfg->iHigh_Dia)){
			iAlmStatus = HIGHER;
			gbAlarmIbp2_Dia = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmIbp2_Dia = FALSE;
		}
	}
	
	//如果于之前的状态相同，则直接返回；否则根据不同状态判断
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//首先解除原先的状态
		sAlmInfo.bAlmID = AP_IDIA2_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_IDIA2_LOW;
		AlarmLifted_Phy(sAlmInfo);	
		
		//增加新的状态
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_IDIA2_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_IDia2_High();
					if(B_PRINTF) printf("Add High Alarm   --- IBP2 Dia.\n");
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_IDIA2_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
					NetSend_Alm_IDia2_Low();
					if(B_PRINTF) printf("Add Low Alarm   --- IBP2 Dia.\n");
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}

	iPreAlmStatus = iAlmStatus;
	return 0;
}

static int IsAlm_IMean2(const PS_IBP_VALUE pValue, const PS_IBP_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->iMean;

	//判断参数值是否越限
	if(pValue->bOff){
		iAlmStatus = NORMAL;
		gbAlarmIbp2_Mean = FALSE;
	}
	else{
		if(iValue < (pCfg->iLow_Mean)){
			iAlmStatus = LOWER;
			gbAlarmIbp2_Mean = TRUE;
			
		}
		else if(iValue > (pCfg->iHigh_Mean)){
			iAlmStatus = HIGHER;
			gbAlarmIbp2_Mean = TRUE;
			
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmIbp2_Mean = FALSE;
		}
	}
	
	//如果于之前的状态相同，则直接返回；否则根据不同状态判断
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//首先解除原先的状态
		sAlmInfo.bAlmID = AP_IMEAN2_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_IMEAN2_LOW;
		AlarmLifted_Phy(sAlmInfo);	
			
		//增加新的状态
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_IMEAN2_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
					NetSend_Alm_IMean2_High();
					if(B_PRINTF) printf("Add High Alarm   --- IBP2 Mean.\n");
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_IMEAN2_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
					NetSend_Alm_IMean2_Low();
					if(B_PRINTF) printf("Add Low Alarm   --- IBP2 Mean.\n");
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}

	iPreAlmStatus = iAlmStatus;
	return 0;
}

//当改变报警源时使用
static int ClearIbp2Alarm()
{
	B_ALM_INFO sAlmInfo;
	
	//解除原先的状态
	sAlmInfo.bAlmID = AP_ISYS2_HIGH;
	AlarmLifted_Phy(sAlmInfo);	
	sAlmInfo.bAlmID = AP_ISYS2_LOW;
	AlarmLifted_Phy(sAlmInfo);	
	gbAlarmIbp2_Sys = FALSE;
	sAlmInfo.bAlmID = AP_IDIA2_HIGH;
	AlarmLifted_Phy(sAlmInfo);	
	sAlmInfo.bAlmID = AP_IDIA2_LOW;
	AlarmLifted_Phy(sAlmInfo);	
	gbAlarmIbp2_Dia = FALSE;
	sAlmInfo.bAlmID = AP_IMEAN2_HIGH;
	AlarmLifted_Phy(sAlmInfo);	
	sAlmInfo.bAlmID = AP_IMEAN2_LOW;
	AlarmLifted_Phy(sAlmInfo);	
	gbAlarmIbp2_Mean = FALSE;

	return 0;
}

/*
	判断IBP2参数是否报警
*/
int IsAlm_IBP2(BOOL bChangedSource, BOOL bChangedClass)
{
	BOOL bReset = bChangedSource;	
	BOOL bChanged = bChangedClass;
	
	if(bReset){
		ClearIbp2Alarm();
		bChanged = TRUE;
	}

	//根据不同的报警源进行判断               
// 	if(B_PRINTF) printf("%s:%d IBP Alarm Source is %d.\n", __FILE__, __LINE__, gCfgIbp1.bAlmSource);
	switch(gCfgIbp2.bAlmSource){
		case IBP_ALM_SRC_S:{
			IsAlm_ISys2(&gValueIbp2, &gCfgIbp2, bChanged);
		}break;
		case IBP_ALM_SRC_M:{
			IsAlm_IMean2(&gValueIbp2, &gCfgIbp2, bChanged);
		}break;
		case IBP_ALM_SRC_D:{
			IsAlm_IDia2(&gValueIbp2, &gCfgIbp2, bChanged);
		}break;
		case IBP_ALM_SRC_SM:{
			IsAlm_ISys2(&gValueIbp2, &gCfgIbp2, bChanged);
			IsAlm_IMean2(&gValueIbp2, &gCfgIbp2, bChanged);
		}break;
		case IBP_ALM_SRC_MD:{
			IsAlm_IMean2(&gValueIbp2, &gCfgIbp2, bChanged);
			IsAlm_IDia2(&gValueIbp2, &gCfgIbp2, bChanged);
		}break;
		case IBP_ALM_SRC_SD:{
			IsAlm_ISys2(&gValueIbp2, &gCfgIbp2, bChanged);
			IsAlm_IDia2(&gValueIbp2, &gCfgIbp2, bChanged);
		}break;
		default:{//IBP_ALM_SRC_ALL
			IsAlm_ISys2(&gValueIbp2, &gCfgIbp2, bChanged);
			IsAlm_IDia2(&gValueIbp2, &gCfgIbp2, bChanged);
			IsAlm_IMean2(&gValueIbp2, &gCfgIbp2, bChanged);
		}break;
	}

	return 0;
}

int IsOff_IBP1(const PS_IBP_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = SENSOR_OFF;
	//当前的状态
	int iAlmStatus=SENSOR_OFF;
	B_ALM_INFO sAlmInfo;
	
	//如果报警暂停，则取消脱落报警
	if(gbSuspending) pValue->bConnected = FALSE;
	
	//得到当前的报警状态
	if(pValue->bOff && pValue->bConnected){
		iAlmStatus = SENSOR_OFF;
	}
	else{
		iAlmStatus = SENSOR_ON;
	}
	
	//与先前的状态比较,如果相同,则直接返回,
	//否则添加 or 解除报警
	if(iPreAlmStatus == iAlmStatus){
		return -1;	
	}
	else{
		sAlmInfo.bAlmID = AT_IBP1_SENSOROFF;
		sAlmInfo.bStatus = ALMSTATUS_ALARM;
		sAlmInfo.bClass = ALARM_LOW;
		if(iAlmStatus == SENSOR_OFF){
			//Add	
			AddAlarm_Tec(sAlmInfo);
		}
		else{
			//Lift
			AlarmLifted_Tec(sAlmInfo);
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	return 0;
}

int IsOff_IBP2(const PS_IBP_VALUE pValue)
{
	if(pValue == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = SENSOR_OFF;
	//当前的状态
	int iAlmStatus=SENSOR_OFF;
	B_ALM_INFO sAlmInfo;
	
	//如果报警暂停，则取消脱落报警
	if(gbSuspending) pValue->bConnected = FALSE;

	//得到当前的报警状态
	if(pValue->bOff && pValue->bConnected){
		iAlmStatus = SENSOR_OFF;
	}
	else{
		iAlmStatus = SENSOR_ON;
	}
	
	//与先前的状态比较,如果相同,则直接返回,
	//否则添加 or 解除报警
	if(iPreAlmStatus == iAlmStatus){
		return -1;	
	}
	else{
		sAlmInfo.bAlmID = AT_IBP2_SENSOROFF;
		sAlmInfo.bStatus = ALMSTATUS_ALARM;
		sAlmInfo.bClass = ALARM_LOW;
		if(iAlmStatus == SENSOR_OFF){
			//Add	
			AddAlarm_Tec(sAlmInfo);
		}
		else{
			//Lift
			AlarmLifted_Tec(sAlmInfo);
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	return 0;
}



//判断ECG测量参数是否报警
int IsAlm_ST1(const PS_ECG_VALUE pValue, const PS_ECG_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->iST1;

 	//if(B_PRINTF) printf("-----------------ST1 : iPreAlmStatus is %d, iAlmStatus is %d.\n", iPreAlmStatus, iAlmStatus);
	//判断参数值是否越限
// 	if(pValue->bRAOff || iValue ==0){
	if(pValue->bRAOff){
		iAlmStatus = NORMAL;
		gbAlarmEcg_ST=FALSE;
	}
	else{
		if(iValue < (pCfg->iSt1_Low)){
			iAlmStatus = LOWER;
			gbAlarmEcg_ST=TRUE;
			//NetSend_Alm_HR_Low();	
		}
		else if(iValue > (pCfg->iSt1_High)){
			iAlmStatus = HIGHER;
			gbAlarmEcg_ST=TRUE;
			//NetSend_Alm_HR_High();
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmEcg_ST=FALSE;
		}
	}
	
// 	if(B_PRINTF) printf("HR: iPreAlmStatus is %d, iAlmStatus is %d.\n", iPreAlmStatus, iAlmStatus);
	
	//如果于之前的状态相同，则直接返回；否则根据不同状态判断
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//首先解除原先的状态
		sAlmInfo.bAlmID = AP_ST1_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_ST1_LOW;
		AlarmLifted_Phy(sAlmInfo);	
		
		//增加新的状态
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_ST1_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_ST1_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}


int IsAlm_ST2(const PS_ECG_VALUE pValue, const PS_ECG_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = NORMAL;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->iST2;

 	//if(B_PRINTF) printf("-----------------ST1 : iPreAlmStatus is %d, iAlmStatus is %d.\n", iPreAlmStatus, iAlmStatus);
	//判断参数值是否越限
// 	if(pValue->bRAOff || iValue ==0){
	if(pValue->bRAOff){
		iAlmStatus = NORMAL;
		gbAlarmEcg_ST=FALSE;
	}
	else{
		if(iValue < (pCfg->iSt2_Low)){
			iAlmStatus = LOWER;
			gbAlarmEcg_ST=TRUE;
			//NetSend_Alm_HR_Low();	
		}
		else if(iValue > (pCfg->iSt2_High)){
			iAlmStatus = HIGHER;
			gbAlarmEcg_ST=TRUE;
			//NetSend_Alm_HR_High();
		}
		else{
			iAlmStatus = NORMAL;
			gbAlarmEcg_ST=FALSE;
		}
	}
	
// 	if(B_PRINTF) printf("HR: iPreAlmStatus is %d, iAlmStatus is %d.\n", iPreAlmStatus, iAlmStatus);
	
	//如果于之前的状态相同，则直接返回；否则根据不同状态判断
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//首先解除原先的状态
		sAlmInfo.bAlmID = AP_ST2_HIGH;
		AlarmLifted_Phy(sAlmInfo);	
	
		sAlmInfo.bAlmID = AP_ST2_LOW;
		AlarmLifted_Phy(sAlmInfo);	
		
		//增加新的状态
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_ST2_HIGH;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
				}break;
				case LOWER:{
					sAlmInfo.bAlmID = AP_ST2_LOW;                                          
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 	
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

int IsAlm_Arr(const PS_ECG_VALUE pValue, const PS_ECG_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL || pCfg == NULL) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = NORMAL,iold_arr_value;
	int iAlmStatus=NORMAL;
	int iValue;
	B_ALM_INFO sAlmInfo;
	
	iValue = pValue->lARR;

 	//if(B_PRINTF) printf("-----------------ST1 : iPreAlmStatus is %d, iAlmStatus is %d.\n", iPreAlmStatus, iAlmStatus);
	//判断参数值是否越限
 	if(iValue ==0||iValue ==0x8000000||(gCfgEcg.bArrAnalyse==0)){
//	if(pValue->bRAOff ){
		iAlmStatus = NORMAL;
	}
	else{
		if(iValue > 0){
			iAlmStatus = HIGHER;
		}	
		else{
			iAlmStatus = NORMAL;
		}
	}
	
// 	if(B_PRINTF) printf("HR: iPreAlmStatus is %d, iAlmStatus is %d.\n", iPreAlmStatus, iAlmStatus);
	
	//如果于之前的状态相同，则直接返回；否则根据不同状态判断
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//首先解除原先的状态
		sAlmInfo.bAlmID = AP_ARR;
		AlarmLifted_Phy(sAlmInfo);	
		
		//增加新的状态
		if(pCfg->bAlmControl != ALARM_OFF){
			switch(iAlmStatus){
				case HIGHER:{
					sAlmInfo.bAlmID = AP_ARR;
					sAlmInfo.bStatus = ALMSTATUS_ALARM;
					sAlmInfo.bClass = pCfg->bAlmControl;
					sAlmInfo.bPrint = pCfg->bPrnControl;
					AddAlarm_Phy(sAlmInfo); 
				}break;
				default:{//NORMAL
				}break;
			}		
		}
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

//系统状态参数板电压3.3v 以及5.5v是否正常
int IsAlm_Power(const unsigned char *pValue,BOOL bChanged)
{

	if(pValue == NULL ) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = MEASURE_OK;
	int iAlmStatus=MEASURE_OK;
	int abnormal,force,comm;
	B_ALM_INFO sAlmInfo;

	abnormal = *pValue&0x01<<4;
	force = *pValue&0x01<<3;
	comm = *pValue&0x01<<2;

	if(*pValue&0x07<<2){
	//	if(B_PRINTF)printf("1=============\n");
		iAlmStatus = MEASURE_ERROR;
	}
	else{
	//	if(B_PRINTF)printf("2~~~~~~~~~~~~~~\n");
		iAlmStatus = MEASURE_OK;
	}
	
	if(gbSuspending){
		if(abnormal){
	//		if(B_PRINTF)printf("11111111111111\n");
			sAlmInfo.bAlmID = AT_POWER_ABNORMAL;
			AlarmLifted_Tec(sAlmInfo);
			
			CMD_Power(3);
			
		}
		if(force){
	//		if(B_PRINTF)printf("2222222222222\n");
			sAlmInfo.bAlmID = AT_POWER_FORCE;
			AlarmLifted_Tec(sAlmInfo);
			
			CMD_Power(2);
		}
		if(comm){
		//	if(B_PRINTF)printf("33333333\n");
			sAlmInfo.bAlmID = AT_POWER_COMM;
			AlarmLifted_Tec(sAlmInfo);
			
			CMD_Power(1);
		}
		return 0;
	}
	
	//如果于之前的状态相同，则直接返回；否则根据不同状态判断
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
//	if(B_PRINTF)	printf("-----return \n");
		return -1;
	}
	else{
		if(abnormal){
			sAlmInfo.bAlmID = AT_POWER_ABNORMAL;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_LOW;
			//Add	
			AddAlarm_Tec(sAlmInfo);
		}
		if(force){
			sAlmInfo.bAlmID = AT_POWER_FORCE;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_LOW;
			//Add	
			AddAlarm_Tec(sAlmInfo);
		}
		if(comm){
			sAlmInfo.bAlmID = AT_POWER_COMM;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_LOW;
			//Add	
			AddAlarm_Tec(sAlmInfo);
		}
		
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}
//系统状态参数板电压3.3v 以及5.5v是否正常
int IsAlm_Voltage(const PSYS_STATUS pValue,BOOL bChanged)
{
	if(pValue == NULL ) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = MEASURE_OK;
	int iAlmStatus=MEASURE_OK;
	int iValue33,iValue5;
	B_ALM_INFO sAlmInfo;
	
	iValue33 = pValue->bState33V;
	iValue5 = pValue->bState5V;

	if(pValue->bState33V>0||pValue->bState5V>0){
		iAlmStatus = MEASURE_ERROR;
	}
	else{
		iAlmStatus = MEASURE_OK;
	}
	
	//如果于之前的状态相同，则直接返回；否则根据不同状态判断
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		switch(iValue33){
			case 0:
				sAlmInfo.bAlmID = AT_BOARD33V_HIGH;
				AlarmLifted_Tec(sAlmInfo);
				sAlmInfo.bAlmID = AT_BOARD33V_LOW;
				AlarmLifted_Tec(sAlmInfo);
			break;
			case 1:
				sAlmInfo.bAlmID = AT_BOARD33V_HIGH;
				sAlmInfo.bStatus = ALMSTATUS_ALARM;
				sAlmInfo.bClass = ALARM_HIGH;
					//Add	
				AddAlarm_Tec(sAlmInfo);
			break;
			case 2:
				sAlmInfo.bAlmID = AT_BOARD33V_LOW;
				sAlmInfo.bStatus = ALMSTATUS_ALARM;
				sAlmInfo.bClass = ALARM_HIGH;
					//Add	
				AddAlarm_Tec(sAlmInfo);
			break;
			default:
			break;
			}
			switch(iValue5){
			case 0:
				sAlmInfo.bAlmID = AT_BOARD5V_HIGH;
				AlarmLifted_Tec(sAlmInfo);
				sAlmInfo.bAlmID = AT_BOARD5V_LOW;
				AlarmLifted_Tec(sAlmInfo);
			break;
			case 1:
				sAlmInfo.bAlmID = AT_BOARD5V_HIGH;
				sAlmInfo.bStatus = ALMSTATUS_ALARM;
				sAlmInfo.bClass = ALARM_HIGH;
					//Add	
				AddAlarm_Tec(sAlmInfo);
			break;
			case 2:
				sAlmInfo.bAlmID = AT_BOARD5V_LOW;
				sAlmInfo.bStatus = ALMSTATUS_ALARM;
				sAlmInfo.bClass = ALARM_HIGH;
					//Add	
				AddAlarm_Tec(sAlmInfo);
			break;
			default:
			break;
			}
		
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}

//系统自检结果报警
int IsAlm_SelfTest(const PSYS_STATUS pValue,BOOL bChanged)
{
	if(pValue == NULL ) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = MEASURE_OK;
	int iAlmStatus=MEASURE_OK;
	B_ALM_INFO sAlmInfo;
	

	if(pValue->bSelWatchdog>0||pValue->bSelAD>0||pValue->bSelRAM>0
		||pValue->bSelROM>0||pValue->bSelCPU>0||pValue->bSelIBP>0
		||pValue->bSelNBP>0||pValue->bSelSPO2>0||pValue->bSelTEMP>0
		||pValue->bSelRESP>0||pValue->bSelECG>0){
		iAlmStatus = MEASURE_ERROR;
	}
	else{
		iAlmStatus = MEASURE_OK;
	}
	
	//如果于之前的状态相同，则直接返回；否则根据不同状态判断
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//watchdog
		if(pValue->bSelWatchdog>0){
			sAlmInfo.bAlmID = AT_SELFTEST_WATCHDOG;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_HIGH;
				//Add	
			AddAlarm_Tec(sAlmInfo);
		}else{
			sAlmInfo.bAlmID = AT_SELFTEST_WATCHDOG;
				//delete
			AlarmLifted_Tec(sAlmInfo);
		}
		//AD
		if(pValue->bSelWatchdog>0){
			sAlmInfo.bAlmID = AT_SELFTEST_AD;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_HIGH;
				//Add	
			AddAlarm_Tec(sAlmInfo);
		}else{
			sAlmInfo.bAlmID = AT_SELFTEST_AD;
				//delete
			AlarmLifted_Tec(sAlmInfo);
		}
		//RAM
		if(pValue->bSelWatchdog>0){
			sAlmInfo.bAlmID = AT_SELFTEST_RAM;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_HIGH;
				//Add	
			AddAlarm_Tec(sAlmInfo);
		}else{
			sAlmInfo.bAlmID = AT_SELFTEST_RAM;
				//delete
			AlarmLifted_Tec(sAlmInfo);
		}
		//ROM
		if(pValue->bSelWatchdog>0){
			sAlmInfo.bAlmID = AT_SELFTEST_ROM;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_HIGH;
				//Add	
			AddAlarm_Tec(sAlmInfo);
		}else{
			sAlmInfo.bAlmID = AT_SELFTEST_ROM;
				//delete
			AlarmLifted_Tec(sAlmInfo);
		}
		//CPU
		if(pValue->bSelWatchdog>0){
			sAlmInfo.bAlmID = AT_SELFTEST_CPU;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_HIGH;
				//Add	
			AddAlarm_Tec(sAlmInfo);
		}else{
			sAlmInfo.bAlmID = AT_SELFTEST_CPU;
				//delete
			AlarmLifted_Tec(sAlmInfo);
		}
		//IBP
		if(pValue->bSelWatchdog>0){
			sAlmInfo.bAlmID = AT_SELFTEST_IBP;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_HIGH;
				//Add	
			AddAlarm_Tec(sAlmInfo);
		}else{
			sAlmInfo.bAlmID = AT_SELFTEST_IBP;
				//delete
			AlarmLifted_Tec(sAlmInfo);
		}
		//NIBP
		if(pValue->bSelWatchdog>0){
			sAlmInfo.bAlmID = AT_SELFTEST_NBP;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_HIGH;
				//Add	
			AddAlarm_Tec(sAlmInfo);
		}else{
			sAlmInfo.bAlmID = AT_SELFTEST_NBP;
				//delete
			AlarmLifted_Tec(sAlmInfo);
		}
		//SPO2
		if(pValue->bSelWatchdog>0){
			sAlmInfo.bAlmID = AT_SELFTEST_SPO2;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_HIGH;
				//Add	
			AddAlarm_Tec(sAlmInfo);
		}else{
			sAlmInfo.bAlmID = AT_SELFTEST_SPO2;
				//delete
			AlarmLifted_Tec(sAlmInfo);
		}
		//TEMP
		if(pValue->bSelWatchdog>0){
			sAlmInfo.bAlmID = AT_SELFTEST_TEMP;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_HIGH;
				//Add	
			AddAlarm_Tec(sAlmInfo);
		}else{
			sAlmInfo.bAlmID = AT_SELFTEST_TEMP;
				//delete
			AlarmLifted_Tec(sAlmInfo);
		}
		//RESP
		if(pValue->bSelWatchdog>0){
			sAlmInfo.bAlmID = AT_SELFTEST_RESP;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_HIGH;
				//Add	
			AddAlarm_Tec(sAlmInfo);
		}else{
			sAlmInfo.bAlmID = AT_SELFTEST_RESP;
				//delete
			AlarmLifted_Tec(sAlmInfo);
		}
		//ECG
		if(pValue->bSelWatchdog>0){
			sAlmInfo.bAlmID = AT_SELFTEST_ECG;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_HIGH;
				//Add	
			AddAlarm_Tec(sAlmInfo);
		}else{
			sAlmInfo.bAlmID = AT_SELFTEST_ECG;
				//delete
			AlarmLifted_Tec(sAlmInfo);
		}
		
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}


//Ecg通道过载报警
int IsAlm_ECGOverload(const PS_ECG_VALUE pValue, const PS_ECG_CFG pCfg, BOOL bChanged)
{
	if(pValue == NULL ) return -1;
	
	//前一次的报警状态
	static int iPreAlmStatus = MEASURE_OK;
	int iAlmStatus=MEASURE_OK;
	B_ALM_INFO sAlmInfo;
	

	if(pValue->bOverLoad1||pValue->bOverLoad2||pValue->bOverLoad3){
		iAlmStatus = MEASURE_ERROR;
	}
	else{
		iAlmStatus = MEASURE_OK;
	}
	
	//如果于之前的状态相同，则直接返回；否则根据不同状态判断
	if(iPreAlmStatus == iAlmStatus && (!bChanged)){
		return -1;
	}
	else{
		//通道1过载
		if(pValue->bOverLoad1){
			sAlmInfo.bAlmID = AT_ECG_OVERLOAD1;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_HIGH;
				//Add	
			AddAlarm_Tec(sAlmInfo);
		}else{
			sAlmInfo.bAlmID = AT_ECG_OVERLOAD1;
				//delete
			AlarmLifted_Tec(sAlmInfo);
		}
		//通道2过载
		if(pValue->bOverLoad2){
			sAlmInfo.bAlmID = AT_ECG_OVERLOAD2;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_HIGH;
				//Add	
			AddAlarm_Tec(sAlmInfo);
		}else{
			sAlmInfo.bAlmID = AT_ECG_OVERLOAD2;
				//delete
			AlarmLifted_Tec(sAlmInfo);
		}
		//通道3过载
		if(pValue->bOverLoad3){
			sAlmInfo.bAlmID = AT_ECG_OVERLOAD3;
			sAlmInfo.bStatus = ALMSTATUS_ALARM;
			sAlmInfo.bClass = ALARM_HIGH;
				//Add	
			AddAlarm_Tec(sAlmInfo);
		}else{
			sAlmInfo.bAlmID = AT_ECG_OVERLOAD3;
				//delete
			AlarmLifted_Tec(sAlmInfo);
		}
		
	}
	
	iPreAlmStatus = iAlmStatus;
	
	return 0;
}



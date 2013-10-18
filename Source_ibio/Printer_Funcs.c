/*************************************************************************** 
 *	Module Name:	Printer_Funcs
 *
 *	Abstract:	打印机接口函数
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-12-27 10:36:45
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "General_Funcs.h"
#include "AlarmManage.h"
#include "Printer_Funcs.h"

//打印机ID
#define PRINTER_ID		GT_S2_PRINTER

//空白走纸宽度(列)
#define BLANKPAPER_WIDTH		15

//开始实时打印的时间
S_TIME	sTime_StartPrint;
//停止打印的时间
S_TIME	sTime_StopPrint;
extern sem_t semReal_Print;

//此处的排列顺序应与头文件定义的AlarmID相对应
unsigned char *gStrPhyAlmInfoPrintTxt[] = {
	"NONE", 
	"HR HIGH", "HR LOW",
	"ECG WEAKSIGNAL",
	"RR HIGH", "RR LOW",
	"APNEA",
	"SpO2 HIGH", "SpO2 LOW","SpO2 LOWEST",
	"PR HIGH", "PR LOW",
	"T1 HIGH", "T1 LOW", 
	"T2 HIGH", "T2 LOW", 
	"TD HIGH",
	"NSYS HIGH", "NSYS LOW", 
	"NDIA HIGH", "NDIA LOW",
	"NMAP HIGH", "NMAP LOW",
	"EtCO2 HIGH","EtCO2 LOW",
	"FiCO2 HIGH",
	"ISYS1 HIGH", "ISYS1 LOW", 
	"IDIA1 HIGH", "IDIA1 LOW",
	"IMAP1 HIGH", "IMAP1 LOW",
	"ISYS2 HIGH", "ISYS2 LOW", 
	"IDIA2 HIGH", "IDIA2 LOW",
	"IMAP2 HIGH", "IMAP2 LOW"
};	


static BOOL bDebugPrintInfo = FALSE;
//得到打印机状态的回调函数
int  RecorderGetStatus( int *status )
{
	switch (*status) {
		case GT_RECORDER_STATUS_UNKNOWN:{
			if(bDebugPrintInfo)
				printf("(%s:%d)Printer STATUS_UNKNOWN !\n", __FILE__, __LINE__);
			gPrinterStatus = PRNSTATUS_ERR;
		}break;
		case GT_RECORDER_STATUS_PRINTING:{
			if(bDebugPrintInfo)
	 			printf("(%s:%d)Printer STATUS_PRINTING !\n", __FILE__, __LINE__);
			gPrinterStatus = PRNSTATUS_PRINTING;
		}break;
		case GT_RECORDER_STATUS_REQUEST_REPEAT:{
			if(bDebugPrintInfo)
	 			printf("(%s:%d)Printer STATUS_REQUEST_REPEAT !\n", __FILE__, __LINE__);
		}break;
		case  GT_RECORDER_STATUS_NO_FIND:{
			if(bDebugPrintInfo)
	 			printf("(%s:%d)Printer STATUS_NO_FIND !\n", __FILE__, __LINE__);
			gPrinterStatus = PRNSTATUS_ERR;
		}break;
		case  GT_RECORDER_STATUS_NO_PAPER:{
			if(bDebugPrintInfo)
	 			printf("(%s:%d)Printer STATUS_NO_PAPER !\n", __FILE__, __LINE__);
			//停止打印
 	//		RecorderStopPrint();
			gPrinterStatus = PRNSTATUS_NOPAPER;
		}break;
		case  GT_RECORDER_STATUS_BUSY:{
			if(bDebugPrintInfo)
	 			printf("(%s:%d)Printer STATUS_BUSY !\n", __FILE__, __LINE__);
		}break;
		case  GT_RECORDER_STATUS_IDLE:{
			if(bDebugPrintInfo)
	 			printf("(%s:%d)Printer STATUS_IDLE !\n", __FILE__, __LINE__);
			gPrinterStatus = PRNSTATUS_IDLE;
		}break;
		default:break;
	}

	return 0;
}

/*
	打开打印机
	return:
		0:OK
		-1:Failure
*/
int Printer_Open()
{
	int res;
	
// 	return 0;
	
//	res = RecorderOpen(PRINTER_ID);
	
	if(res !=0){
		gPrinterStatus = PRNSTATUS_ERR;
		printf("%s:%d Printer is error.\n", __FILE__, __LINE__);
		return -1;
	}
	else{
		gPrinterStatus = PRNSTATUS_IDLE;
		printf("%s:%d Printer is idle.\n", __FILE__, __LINE__);
	}
	
	//工作状态
	gPrinterWorkType = PRNWORK_NONE;
	return 0;
}

/*
	关闭打印机
	return:
		0:OK
		-1:Failure
*/
int Printer_Close()
{
	int res;
	
//	res = RecorderClose();
	
	return res;
}
	
/*
	打印文字
	return:
		0:OK
		-1:Failure
*/
int Printer_PrintStrings(const PREC_STRING pData)
{
	int res;
	
//	res = RecorderPrintString(pData);
	
	return res; 	
}

/*
	空白走纸
	iCol: 列数 > 3
	return:
		0:OK
		-1:Failure
*/
int Printer_MovePaper(int iCol)
{
	int res;
	
//	res = RecorderMovePaper(iCol);
	
	return res;
}

/*
	开始实时波形打印
	return:
		0:OK
		-1:Failure
*/
int Printer_StartPrintRealWave()
{
	int i, j;
	int res;
		
	unsigned char bSpeed; 					//打印速度	12.5, 25. 50mm/s
	unsigned char bFreq;   					//采样率	100, 200, 300,... ... 1000Hz 	 
	unsigned char bGrid;   					//是否打印网格
	int baseline[GT_PRINT_MAX_CHANNEL];			
	
	//检查打印机是否处在空闲状态 ,否则不响应打印任务
	if(gPrinterStatus == PRINT_STATUS_BUSY){
		printf("%s:%d Printer is busy. Status : \n", __FILE__, __LINE__, gPrinterStatus);
		return -1;
	}

	//Param
//   	gPrintRealWaveParam.bSpeed = GT_PRINT_SPEED_12;
  	gPrintRealWaveParam.bSpeed = gCfgPrinter.bSpeed+1;
	gPrintRealWaveParam.bFreq = GT_FREQUENCE_200;
	gPrintRealWaveParam.bGrid = 0;
	//波形的基线位置
	gPrintRealWaveParam.baseline[0] = 280;
	gPrintRealWaveParam.baseline[1] = 150;
	gPrintRealWaveParam.baseline[2] = 64;
			
	//Wave
	gPrintRealWaves.iCount =3;
// 	gPrintRealWaves.iLength = 200;		//1s
	gPrintRealWaves.iLength = 20;		// 1s
  	for(i=0; i<gPrintRealWaves.iCount; i++){
 		memset(gPrintRealWaves.iWave[i], 0, MAX_REALWAVE_LENGTH);
  	}
	
	
//	res = RecorderStartPrintRealWave(NULL, &gPrintRealWaveParam);		
	res=Start_Printing();

	if(res ==0){
		//打包计数
		giPrintRealWaveIndex = 0;
		//计时
		giPrintRealWaveCount = 0;
		//得到开始打印的时间
		GetSysTime(&sTime_StartPrint);
		//状态
		gPrinterWorkType = PRNWORK_REAL;
		
		printf("%s:%d Start real print. \n", __FILE__, __LINE__);
		return 0;
	}
	else{
		printf("%s:%d Start real print   error. \n", __FILE__, __LINE__);
		return -1;	
	}
}

/*
	设置实时打印波形数据
	pData:波形数据
	return:
		0:OK
		-1:Failure
*/
int Printer_SetRealWave(const PREC_REALWAVE pData)
{
	int res;
	
//	res = RecorderSetPrintRealWave(pData);
	
	if(res !=0) {
		printf("%s:%d real print error. \n", __FILE__, __LINE__);
	}
	else{
// 		printf("%s:%d Set Real Wave. Count is %d.\n", __FILE__, __LINE__, giPrintRealWaveIndex);
// 		printf("%d\n", pData->iWave[0][0]);
	}
	
	return res;
}


/////////////////////////////具体的打印功能函数 //////////////////////
/*
	打印病人信息
	bType: 

	return 
		0:OK
		-1:Failure
*/
int PrintPatientInfo(BYTE bType)
{
	REC_STRING  sInfo;
	int res;
	
	sInfo.iRowCount = 11;
			
	//清空
	memset(sInfo.strText, 0, MAX_ROW_COUNT*MAX_TEXT_LENGTH);
	//标签
	snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, "DATE:");
	snprintf(sInfo.strText[1], MAX_TEXT_LENGTH, "TIME:");
	snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "SPEED:");
	snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, "ID:");
	snprintf(sInfo.strText[5], MAX_TEXT_LENGTH, "NAME:");
	snprintf(sInfo.strText[6], MAX_TEXT_LENGTH, "SEX:");
	snprintf(sInfo.strText[7], MAX_TEXT_LENGTH, "PACE:");
	snprintf(sInfo.strText[8], MAX_TEXT_LENGTH, "AGE:");
	snprintf(sInfo.strText[9], MAX_TEXT_LENGTH, "HEIGHT:");
	snprintf(sInfo.strText[10], MAX_TEXT_LENGTH, "WEIGHT:");
	//打印标签
	res = Printer_PrintString(&sInfo);
	if(res <0) return -1;
	//清空
	memset(sInfo.strText, 0, MAX_ROW_COUNT*MAX_TEXT_LENGTH);
	//Date
	snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, "%.4d/%.2d/%.2d", 
		 sTime_StartPrint.wYear, sTime_StartPrint.bMonth, sTime_StartPrint.bDay);
	//Time
	snprintf(sInfo.strText[1], MAX_TEXT_LENGTH, "%.2d:%.2d:%.2d", 
		 sTime_StartPrint.bHour, sTime_StartPrint.bMin, sTime_StartPrint.bSec);
	snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "%.2d:%.2d:%.2d", 
		 sTime_StopPrint.bHour, sTime_StopPrint.bMin, sTime_StopPrint.bSec);
	//Speed:
	switch(gCfgPrinter.bSpeed){
		case PRINTSPEED_125:{
			snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "12.5mm/s");	
		}break;
		case PRINTSPEED_50:{
			snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "50mm/s");	
		}break;
		default:{
			snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "25mm/s");	
		}break;
	}
	//ID
	snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, "%s", gCfgPatient.sID);
	//Name
	snprintf(sInfo.strText[5], MAX_TEXT_LENGTH, "%s", gCfgPatient.sName);
	//Sex
	if(gCfgPatient.bSex == MALE)
		snprintf(sInfo.strText[6], MAX_TEXT_LENGTH, "Male");
	else
		snprintf(sInfo.strText[6], MAX_TEXT_LENGTH, "Female");
	//Pace
	if(gCfgPatient.bPacemaker == PACEMAKER_OFF)
		snprintf(sInfo.strText[7], MAX_TEXT_LENGTH, "Off");	
	else
		snprintf(sInfo.strText[7], MAX_TEXT_LENGTH, "On");	
	//Age
	snprintf(sInfo.strText[8], MAX_TEXT_LENGTH, "%d", gCfgPatient.bAge);
	//Height
	snprintf(sInfo.strText[9], MAX_TEXT_LENGTH, "%d cm", gCfgPatient.wHeight);
	//Weight
	snprintf(sInfo.strText[10], MAX_TEXT_LENGTH, "%d kg", gCfgPatient.wWeight);
	//打印信息
	res = Printer_PrintString(&sInfo);

	//空白走纸
	Printer_MovePaper(BLANKPAPER_WIDTH);
	
	return 0;
}

/*
	打印ECG信息
*/
int PrintEcgInfo()
{
	REC_STRING  sInfo;
	int res;
	unsigned char *strLeadLabel[]={"I", "II", "III", "aVR", "aVF", "aVL", "V-"};
	sInfo.iRowCount = 10;
			
	//清空
	memset(sInfo.strText, 0, MAX_ROW_COUNT*MAX_TEXT_LENGTH);
	//标签
	snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, "ECG");
	snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "GAIN:");
	snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "MODE:");
	snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, "LEAD:");
	snprintf(sInfo.strText[5], MAX_TEXT_LENGTH, "HR:");
	snprintf(sInfo.strText[6], MAX_TEXT_LENGTH, "ST:");
	snprintf(sInfo.strText[7], MAX_TEXT_LENGTH, "ARR:");
	//打印标签
	res = Printer_PrintString(&sInfo);
	
	if(res <0) return -1;
	
	//清空
	memset(sInfo.strText, 0, MAX_ROW_COUNT*MAX_TEXT_LENGTH);
	
	snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, "   ");
	
	//Gain
	if(gCfgEcg.bGain  == ECGGAIN_025X)
		snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "0.25mm/mv");
	else if(gCfgEcg.bGain  == ECGGAIN_05X)
		snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "0.5mm/mv");
	else if(gCfgEcg.bGain  == ECGGAIN_1X)
		snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "10mm/mv");
	else if(gCfgEcg.bGain  == ECGGAIN_2X)
		snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "20mm/mv");
	else 
		snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "Auto");
	//Mode
	if(gCfgEcg.bMode == ECGMODE_DIAG)
		snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "Diag");
	else if(gCfgEcg.bMode == ECGMODE_MON)
		snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "Moni");
	else 
		snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "Oper");
	//TODO:根据配置来定
	snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, "%s", strLeadLabel[gCfgEcg.bChannel1]);	
	//HR TODO:此处脱落判断要区分导联类型
	if(!gValueEcg.bRAOff)
		snprintf(sInfo.strText[5], MAX_TEXT_LENGTH, "%d", gValueEcg.wHR);
	else
		snprintf(sInfo.strText[5], MAX_TEXT_LENGTH, "---");
	//ST
	snprintf(sInfo.strText[6], MAX_TEXT_LENGTH, "-.-");
	//Arr
// 	snprintf(sInfo.strText[8], MAX_TEXT_LENGTH, "%d", gCfgPatient.wWeight);
	//打印信息
	res = Printer_PrintString(&sInfo);
	if(res <0) printf("=======================  HR is %d\n", gValueEcg.wHR);
	
	//空白走纸
	Printer_MovePaper(BLANKPAPER_WIDTH);
	
	return 0;
}

/*
	打印SpO2信息
*/
int PrintSpO2Info()
{
	REC_STRING  sInfo;
	int res;
	
	sInfo.iRowCount = 10;
			
	//清空
	memset(sInfo.strText, 0, MAX_ROW_COUNT*MAX_TEXT_LENGTH);
	//标签
	snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, "SpO2");
	snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "GAIN:");
	snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "FILTER:");
	snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, "SpO2:");
	snprintf(sInfo.strText[5], MAX_TEXT_LENGTH, "PR:");
	//打印标签
	res = Printer_PrintString(&sInfo);
	if(res <0) return -1;
	//清空
	memset(sInfo.strText, 0, MAX_ROW_COUNT*MAX_TEXT_LENGTH);
	snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, "   ");
	//Gain
	if(gCfgSpO2.bGain  == SPO2GAIN_05X)
		snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "x5");
	else if(gCfgSpO2.bGain  == SPO2GAIN_1X)
		snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "x1");
	else if(gCfgSpO2.bGain  == SPO2GAIN_2X)
		snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "x2");
	else 
		snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "Auto");
	//Filter
	if(gCfgSpO2.bFilter == SPO2FILTER_OFF)
		snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "Off");
	else 
		snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "On");
	//SpO2
	if(!gValueSpO2.bOff)
		snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, "%d", gValueSpO2.bSpO2);
	else
		snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, "---");
	//Pr
	if(!gValuePulse.bOff)
		snprintf(sInfo.strText[5], MAX_TEXT_LENGTH, "%d", gValuePulse.wPR);
	else
		snprintf(sInfo.strText[5], MAX_TEXT_LENGTH, "---");
	//打印信息
	res = Printer_PrintString(&sInfo);
	
	//空白走纸
	Printer_MovePaper(BLANKPAPER_WIDTH);
	
	return 0;
}

/*
	打印Nibp信息
*/
int PrintNibpInfo()
{
	REC_STRING  sInfo;
	int res;
	
	sInfo.iRowCount = 10;
			
	//清空
	memset(sInfo.strText, 0, MAX_ROW_COUNT*MAX_TEXT_LENGTH);
	//标签
	snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, "NIBP");
	snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "UNIT:");
	snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "OBJECT:");
	snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, "DATE:");
	snprintf(sInfo.strText[5], MAX_TEXT_LENGTH, "TIME:");
	snprintf(sInfo.strText[6], MAX_TEXT_LENGTH, "SYS:");
	snprintf(sInfo.strText[7], MAX_TEXT_LENGTH, "MAP:");
	snprintf(sInfo.strText[8], MAX_TEXT_LENGTH, "DIA:");
	//打印标签
	res = Printer_PrintString(&sInfo);
	if(res <0) return -1;
	//清空
	memset(sInfo.strText, 0, MAX_ROW_COUNT*MAX_TEXT_LENGTH);
	snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, "   ");
	//Unit	
	if(gCfgNibp.bUnit == NIBP_UNIT_MMHG)
		snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "mmHg");
	else
		snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "kPa");
	//Object
	if(gCfgNibp.bObject == NIBP_OBJECT_ADULT)
		snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "Adult");
	else if(gCfgNibp.bObject == NIBP_OBJECT_CHILD)
		snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "Child");
	else if(gCfgNibp.bObject == NIBP_OBJECT_BABY)
		snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "Neo");
	//如果有测量值
	if(gValueNibp.wSys !=0){
		//Date
		snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, "%.4d/%.2d/%.2d", 
			 gValueNibp.sTime.wYear, gValueNibp.sTime.bMonth, gValueNibp.sTime.bDay);
		//Time
		snprintf(sInfo.strText[5], MAX_TEXT_LENGTH, "%.2d:%.2d:%.2d", 
			 gValueNibp.sTime.bHour, gValueNibp.sTime.bMin, gValueNibp.sTime.bSec);
		
		if(gCfgNibp.bUnit == NIBP_UNIT_MMHG){
			//Sys
			snprintf(sInfo.strText[6], MAX_TEXT_LENGTH, "%d",gValueNibp.wSys);
			//Map
			snprintf(sInfo.strText[7], MAX_TEXT_LENGTH, "%d",gValueNibp.wMean);
			//Dia
			snprintf(sInfo.strText[8], MAX_TEXT_LENGTH, "%d",gValueNibp.wDia);
		}
		else {
			//Sys
			snprintf(sInfo.strText[6], MAX_TEXT_LENGTH, "%.1f",gValueNibp.wSys/mmHgTokPa);
			//Map
			snprintf(sInfo.strText[7], MAX_TEXT_LENGTH, "%.1f",gValueNibp.wMean/mmHgTokPa);
			//Dia
			snprintf(sInfo.strText[8], MAX_TEXT_LENGTH, "%.1f",gValueNibp.wDia/mmHgTokPa);
		}
	}
	else{
		//Date
		snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, "----/--/--");
		//Time
		snprintf(sInfo.strText[5], MAX_TEXT_LENGTH, "--:--:--");		
		if(gCfgNibp.bUnit == NIBP_UNIT_MMHG){
			//Sys
			snprintf(sInfo.strText[6], MAX_TEXT_LENGTH, "---");
			//Map
			snprintf(sInfo.strText[7], MAX_TEXT_LENGTH, "---");
			//Dia
			snprintf(sInfo.strText[8], MAX_TEXT_LENGTH, "---");
		}
		else {
			//Sys
			snprintf(sInfo.strText[6], MAX_TEXT_LENGTH, "--.-");
			//Map
			snprintf(sInfo.strText[7], MAX_TEXT_LENGTH, "--.-");
			//Dia
			snprintf(sInfo.strText[8], MAX_TEXT_LENGTH, "--.-");
		}
	}
	//打印信息
	res = Printer_PrintString(&sInfo);
	
	//空白走纸
	Printer_MovePaper(BLANKPAPER_WIDTH);
	
	return 0;
}

/*
	打印Temp信息
*/
int PrintTempInfo()
{
	REC_STRING  sInfo;
	int res;
	
	sInfo.iRowCount = 10;
			
	//清空
	memset(sInfo.strText, 0, MAX_ROW_COUNT*MAX_TEXT_LENGTH);
	//标签
	snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, "TEMP");
	snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "UNIT:");
	snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "T1:");
	snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, "T2:");
	snprintf(sInfo.strText[5], MAX_TEXT_LENGTH, "TD:");
	//打印标签
	res = Printer_PrintString(&sInfo);
	if(res <0) return -1;
	//清空
	memset(sInfo.strText, 0, MAX_ROW_COUNT*MAX_TEXT_LENGTH);
	snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, "   ");
	//Unit	
	if(gCfgTemp.bUnit == TEMP_UNIT_C)
// 		snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "°C");
		snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "'C");
	else
		snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "°F");
	
	//T1
	if(!gValueTemp.bOff_T1){
		if(gCfgTemp.bUnit == TEMP_UNIT_C){
			snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "%.1f",gValueTemp.wT1/10.0);
		}
		else {
			snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "%.1f",gValueTemp.wT1/10.0* 1.8 +32.0);
		}
	}
	else{
		snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "--.-");
	}
	
	//T2
	if(!gValueTemp.bOff_T2){
		if(gCfgTemp.bUnit == TEMP_UNIT_C){
			snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, "%.1f",gValueTemp.wT2/10.0);
		}
		else {
			snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, "%.1f",gValueTemp.wT2/10.0* 1.8 +32.0);
		}
	}
	else{
		snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, "--.-");
	}
	
	//TD
	if(!gValueTemp.bOff_T1 && !gValueTemp.bOff_T2){
		if(gCfgTemp.bUnit == TEMP_UNIT_C){
			snprintf(sInfo.strText[5], MAX_TEXT_LENGTH, "%.1f",
				 fabs(gValueTemp.wT1/10.0 - gValueTemp.wT2/10.0));
		}
		else {
			snprintf(sInfo.strText[5], MAX_TEXT_LENGTH, "%.1f",
				 fabs(gValueTemp.wT1/10.0* 1.8- gValueTemp.wT2/10.0* 1.8));
		}
	}
	else{
		snprintf(sInfo.strText[5], MAX_TEXT_LENGTH, "--.-");
	}
	
	//打印信息
	res = Printer_PrintString(&sInfo);
	
	//空白走纸
	Printer_MovePaper(BLANKPAPER_WIDTH);
	
	return 0;
}

/*
	打印呼吸信息
*/
int PrintRespInfo()
{
	REC_STRING  sInfo;
	int res;
	
	sInfo.iRowCount = 10;
			
	//清空
	memset(sInfo.strText, 0, MAX_ROW_COUNT*MAX_TEXT_LENGTH);
	//标签
	snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, "RESP");
	snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "GAIN:");
	snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "SRC:");
	snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, "RR:");
	//打印标签
	res = Printer_PrintString(&sInfo);
	if(res <0) return -1;
	//清空
	memset(sInfo.strText, 0, MAX_ROW_COUNT*MAX_TEXT_LENGTH);
	snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, "   ");
	//Gain
	switch(gCfgResp.bGain){
		case RESPGAIN_025X:
			snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "x0.25");
		break;
		case RESPGAIN_05X:
			snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "x0.5");
		break;
		case RESPGAIN_1X:
			snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "x1");
		break;
		case RESPGAIN_2X:
			snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "x2");
		break;
		case RESPGAIN_4X:
			snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "x4");
		break;
		case RESPGAIN_8X:
			snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "x8");
		break;
		case RESPGAIN_16X:
			snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "x16");
		break;
		case RESPGAIN_32X:
			snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "x32");
		break;

	}
	
	//Source
	if(gCfgResp.bSource == RR_SRC_ECG)
		snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "ECG");
	else 
		snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "CO2");
	//RR
	if(!gValueResp.bOff)
		snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, "%d", gValueResp.wRR);
	else
		snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, "---");
	//打印信息
	res = Printer_PrintString(&sInfo);
		
	//空白走纸
	Printer_MovePaper(BLANKPAPER_WIDTH);
	
	return 0;
}

/*
	打印CO2信息
*/
int PrintCO2Info()
{
	REC_STRING  sInfo;
	int res;
	
	sInfo.iRowCount = 10;
			
	//清空
	memset(sInfo.strText, 0, MAX_ROW_COUNT*MAX_TEXT_LENGTH);
	//标签
	snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, "CO2");
	snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "UNIT:");
	snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "EtCO2:");
	snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, "FiCO2:");
	snprintf(sInfo.strText[5], MAX_TEXT_LENGTH, "awRR:");
	//打印标签
	res = Printer_PrintString(&sInfo);
	if(res <0) return -1;
	//清空
	memset(sInfo.strText, 0, MAX_ROW_COUNT*MAX_TEXT_LENGTH);
	snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, "   ");
	//Unit	
	if(gCfgCO2.bUnit == CO2UNIT_MMHG)
		snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "mmHg");
	else
		snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "%%");
	
	//EtCO2
	if(!gValueCO2.bOff){
		if(gCfgCO2.bUnit == CO2UNIT_MMHG){
			snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "%d",gValueCO2.wEtCO2);
			snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, "%d",gValueCO2.wFiCO2);
		}
		else {
			snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "%.1f",(float)(gValueCO2.wEtCO2*100.0/gValueCO2.wBaro));
			snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, "%.1f",(float)(gValueCO2.wFiCO2*100.0/gValueCO2.wBaro));
		}
		snprintf(sInfo.strText[5], MAX_TEXT_LENGTH, "%d",gValueCO2.wAwRR);
	}
	else{
		if(gCfgCO2.bUnit == CO2UNIT_MMHG){
			snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "--");
			snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, "--");
		}
		else {
			snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "--.-");
			snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, "--.-");
		}	
		snprintf(sInfo.strText[5], MAX_TEXT_LENGTH, "---");
	}

	//打印信息
	res = Printer_PrintString(&sInfo);
	
	//空白走纸
	Printer_MovePaper(BLANKPAPER_WIDTH);
	
	return 0;
}

/*
	打印IBP信息
*/
int PrintIbp1Info()
{
	REC_STRING  sInfo;
	int res;
	
	sInfo.iRowCount = 10;
			
	//清空
	memset(sInfo.strText, 0, MAX_ROW_COUNT*MAX_TEXT_LENGTH);
	//标签
	snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, "IBP1");
	snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "UNIT:");
	snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "SYS:");
	snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, "DIA:");
	snprintf(sInfo.strText[5], MAX_TEXT_LENGTH, "MEAN:");
	//打印标签
	res = Printer_PrintString(&sInfo);
	if(res <0) return -1;
	//清空
	memset(sInfo.strText, 0, MAX_ROW_COUNT*MAX_TEXT_LENGTH);
	snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, "   ");
	//Unit	
	if(gCfgIbp1.bUnit == IBPUNIT_MMHG)
		snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "mmHg");
	else if(gCfgIbp1.bUnit == IBPUNIT_KPA)
		snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "kPa");
	else
		snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "cmH2O");
	
	//SYS
	if(!gValueIbp1.bOff){
		if(gCfgIbp1.bUnit == IBPUNIT_MMHG){
			snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "%d",gValueIbp1.iSys);
			snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, "%d",gValueIbp1.iDia);
			snprintf(sInfo.strText[5], MAX_TEXT_LENGTH, "%d",gValueIbp1.iMean);
		}
		else if(gCfgIbp1.bUnit == IBPUNIT_KPA) {
			snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "%.1f",(float)(gValueIbp1.iSys/ 7.5));
			snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, "%.1f",(float)(gValueIbp1.iDia/ 7.5));
			snprintf(sInfo.strText[5], MAX_TEXT_LENGTH, "%.1f",(float)(gValueIbp1.iMean/ 7.5));
		}
		else {
			snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "%d",(int)(gValueIbp1.iSys*1.36));
			snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, "%d",(int)(gValueIbp1.iDia*1.36));
			snprintf(sInfo.strText[5], MAX_TEXT_LENGTH, "%d",(int)(gValueIbp1.iMean*1.36));
		}
	}
	else{
		snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "--");
		snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, "--");
		snprintf(sInfo.strText[5], MAX_TEXT_LENGTH, "--");
	}
	
	//打印信息
	res = Printer_PrintString(&sInfo);
	
	//空白走纸
	Printer_MovePaper(BLANKPAPER_WIDTH);
	
	return 0;
}

int PrintIbp2Info()
{
	REC_STRING  sInfo;
	int res;
	
	sInfo.iRowCount = 10;
			
	//清空
	memset(sInfo.strText, 0, MAX_ROW_COUNT*MAX_TEXT_LENGTH);
	//标签
	snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, "IBP2");
	snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "UNIT:");
	snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "SYS:");
	snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, "DIA:");
	snprintf(sInfo.strText[5], MAX_TEXT_LENGTH, "MEAN:");
	//打印标签
	res = Printer_PrintString(&sInfo);
	if(res <0) return -1;
	//清空
	memset(sInfo.strText, 0, MAX_ROW_COUNT*MAX_TEXT_LENGTH);
	snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, "   ");
	//Unit	
	if(gCfgIbp2.bUnit == IBPUNIT_MMHG)
		snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "mmHg");
	else if(gCfgIbp2.bUnit == IBPUNIT_KPA)
		snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "kPa");
	else
		snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, "cmH2O");
	
	//SYS
	if(!gValueIbp2.bOff){
		if(gCfgIbp1.bUnit == IBPUNIT_MMHG){
			snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "%d",gValueIbp2.iSys);
			snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, "%d",gValueIbp2.iDia);
			snprintf(sInfo.strText[5], MAX_TEXT_LENGTH, "%d",gValueIbp2.iMean);
		}
		else if(gCfgIbp2.bUnit == IBPUNIT_KPA) {
			snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "%.1f",(float)(gValueIbp2.iSys/ 7.5));
			snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, "%.1f",(float)(gValueIbp2.iDia/ 7.5));
			snprintf(sInfo.strText[5], MAX_TEXT_LENGTH, "%.1f",(float)(gValueIbp2.iMean/ 7.5));
		}
		else {
			snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "%d",(int)(gValueIbp2.iSys*1.36));
			snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, "%d",(int)(gValueIbp2.iDia*1.36));
			snprintf(sInfo.strText[5], MAX_TEXT_LENGTH, "%d",(int)(gValueIbp2.iMean*1.36));
		}
	}
	else{
		snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, "--");
		snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, "--");
		snprintf(sInfo.strText[5], MAX_TEXT_LENGTH, "--");
	}
	
	//打印信息
	res = Printer_PrintString(&sInfo);
	
	//空白走纸
	Printer_MovePaper(BLANKPAPER_WIDTH);
	
	return 0;
}


/*
	停止当前的打印任务
	return:
		0:OK
		-1:Failure
*/
int Printer_StopCurWork()
{
	
// 	if(gPrinterStatus ==PRNSTATUS_ERR)  return -1;
	printf("%s :%d  stop current  print.\n", __FILE__, __LINE__);
	
	//停止打印
	//RecorderStopPrint();
	
	Stop_Printing();
	
	//工作状态 无
	gPrinterWorkType = PRNWORK_NONE;
	gPrintingType = PRN_WORK_NONE;
	return 0;
}

/*
	停止实时打印
	return:
		0:OK
		-1:Failure
*/
int Printer_StopPrintRealWave()
{
	
	printf("%s :%d  stop realtime print.\n", __FILE__, __LINE__);
	
	//停止打印
//	RecorderStopPrint();
	Stop_Printing();
	
	//空白走纸
	Printer_Move_Paper(BLANKPAPER_WIDTH);
	
	//打印其他信息
	if(gPrinterWorkType ==PRNWORK_REAL){
		gPrintingType=PRN_WORK_REAL;
		sem_post(&semReal_Print);
	}

     gPrinterWorkType =PRNWORK_NONE;
}
int Print_Info_Real_Wave()
{
	//得到结束打印的时间
	GetSysTime(&sTime_StopPrint);
	
	//打印病人信息
	PrintPatientInfo(1);
	//打印参数信息

	PrintEcgInfo();
	PrintSpO2Info();
	PrintNibpInfo();
	PrintTempInfo();
	PrintRespInfo();
	if(gbHaveCo2){
		PrintCO2Info();
	}
	
	if(gbHaveIbp){
		PrintIbp1Info();
		PrintIbp2Info();
	}
	
	Printer_Move_Paper(BLANKPAPER_WIDTH);
	
	return 0;
}

/*
	打印报警数据
*/
static unsigned char *strEcgLeadName[]={"I", "II", "III", "aVR", "aVL", "aVF", "V"};
int Printer_PrintAlarmData(const PALARMDATA pData)
{
	int i, j, k,index, res;
	int iTmp1, iTmp2, iTmp3;
	//拷屏打印参数
	REC_WAVEPARAM	PrintAlarmParam;
	REC_COPYWAVE 	PrintAlarmWaves;
	//波形信息
	REC_STRING  sInfo;
	//波形组合类型, 
	//0-NONE,  1- ECG 1+ Pleth + Resp,  2 - ECG1 + Pleth + CO2,  3-ECG1 + Ibp1 + IBP2 
	BYTE bWavesType =0;		
	
	
	if(pData  == NULL) return  -1;	
		
	//如果打印机故障，或者缺纸，则不执行打印任务
	if(gPrinterStatus == PRNSTATUS_ERR || gPrinterStatus == PRNSTATUS_NOPAPER){
		printf("%s:%d Printer may be error. \n", __FILE__, __LINE__);
		return -1;
	} 
	
	//如果有打印任务，则终止当前打印任务
	if(gPrinterWorkType != PRNWORK_NONE) {
		if(B_PRINTF) printf("%s:%d Printer is busy . \n", __FILE__, __LINE__);
		return -1;
// 		Printer_StopCurWork();
	}
	printf("%s:%d Printing Alarm Data. \n", __FILE__, __LINE__);
	
	gPrinterWorkType = PRNWORK_COPY;
	//设置打印参数
	//Param
	PrintAlarmParam.bSpeed = GT_PRINT_SPEED_25;
	PrintAlarmParam.bFreq = GT_FREQUENCE_200;
	PrintAlarmParam.bGrid = 0;
	//波形的基线位置
// 	PrintAlarmParam.baseline[0] = 70;
// 	PrintAlarmParam.baseline[1] = 120;
// 	PrintAlarmParam.baseline[2] = 20;
	PrintAlarmParam.baseline[0] = 280;
	PrintAlarmParam.baseline[1] = 150;
	PrintAlarmParam.baseline[2] = 64;
	
	//Wave
	PrintAlarmWaves.iCount = 3;
	PrintAlarmWaves.iLength = (gCfgAlarm.bPrintWaveLength)*SAMPLING_RATE;		
	for(i=0; i<PrintAlarmWaves.iCount; i++){
		memset(PrintAlarmWaves.iWave[i], 0, MAXALARMWAVELENGTH);
	}
		
	//设置文字信息
	sInfo.iRowCount = MAX_ROW_COUNT;
	//清空文字
	memset(sInfo.strText, 0, MAX_ROW_COUNT*MAX_TEXT_LENGTH);
			
	//报警信息
	switch(pData->bAlmClass){
		case ALARM_LOW:{
			snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, " *%s", 
				 gStrPhyAlmInfoPrintTxt[pData->bAlmID]); 
		}break;
		case ALARM_MID:{
			snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, " **%s", 
				 gStrPhyAlmInfoPrintTxt[pData->bAlmID]); 
		}break;
		case ALARM_HIGH:{
			snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, " ***%s", 
				 gStrPhyAlmInfoPrintTxt[pData->bAlmID]); 
		}break;
		default:{
			snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, " %s", 
				 gStrPhyAlmInfoPrintTxt[pData->bAlmID]); 
		}break;
	}
	
	//时间信息
	snprintf(sInfo.strText[1], MAX_TEXT_LENGTH, " %.2d:%.2d:%.2d ", 
		 pData->paras.sTime.bHour,
		 pData->paras.sTime.bMin,
		 pData->paras.sTime.bSec);
	snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, " %.4d/%.2d/%.2d ", 
 		 pData->paras.sTime.wYear, 
		 pData->paras.sTime.bMonth, 
		 pData->paras.sTime.bDay);
	
	//参数信息 
	if(pData->paras.wHR !=0)
		snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, " HR: %3d bpm ",  pData->paras.wHR); 
	else
		snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, " HR: --- bpm "); 
	if(pData->paras.bSpO2 !=0)
		snprintf(sInfo.strText[5], MAX_TEXT_LENGTH, " SpO2: %3d %% ",  pData->paras.bSpO2); 
	else
		snprintf(sInfo.strText[5], MAX_TEXT_LENGTH, " SpO2: -- %% "); 
	if(pData->paras.wPR !=0)
		snprintf(sInfo.strText[6], MAX_TEXT_LENGTH, " PR: %3d bpm ",  pData->paras.wPR); 
	else
		snprintf(sInfo.strText[6], MAX_TEXT_LENGTH, " PR: --- bpm "); 
	if(!gValueResp.bOff)
		snprintf(sInfo.strText[7], MAX_TEXT_LENGTH, " RR: %3d rpm ",  pData->paras.wRR); 
	else
		snprintf(sInfo.strText[7], MAX_TEXT_LENGTH, " RR: --- rpm "); 
	if(gCfgNibp.bUnit == NIBP_UNIT_MMHG){
		if(pData->paras.sNIBP.wSys !=0)
			snprintf(sInfo.strText[8], MAX_TEXT_LENGTH, " NIBP: %3d/%3d (%3d) mmHg ",  
				 pData->paras.sNIBP.wSys, pData->paras.sNIBP.wDia, pData->paras.sNIBP.wMean); 
		else
			snprintf(sInfo.strText[8], MAX_TEXT_LENGTH, " NIBP: ---/--- (---) mmHg ");  
	}else{
		if(pData->paras.sNIBP.wSys !=0)
			snprintf(sInfo.strText[8], MAX_TEXT_LENGTH, " NIBP: %.1f/%.1f (%.1f) kPa ",  
				 pData->paras.sNIBP.wSys/mmHgTokPa, pData->paras.sNIBP.wDia/mmHgTokPa, pData->paras.sNIBP.wMean/mmHgTokPa); 
		else
			snprintf(sInfo.strText[8], MAX_TEXT_LENGTH, " NIBP: --.-/--.- (--.-) kPa");  
	}
				
	if(gCfgTemp.bUnit == TEMP_UNIT_C){
		if(pData->paras.wTemps[0] !=0)
			snprintf(sInfo.strText[9], MAX_TEXT_LENGTH, " T1: %.1f 'C ",  pData->paras.wTemps[0]/10.0); 
		else	
			snprintf(sInfo.strText[9], MAX_TEXT_LENGTH, " T1:--.- 'C "); 
		if(pData->paras.wTemps[1] !=0)
			snprintf(sInfo.strText[10], MAX_TEXT_LENGTH, " T2: %.1f 'C ",  pData->paras.wTemps[1]/10.0); 
		else	
			snprintf(sInfo.strText[10], MAX_TEXT_LENGTH, " T2:--.- 'C "); 
		if(pData->paras.wTemps[0] !=0 && pData->paras.wTemps[1] !=0)
			snprintf(sInfo.strText[11], MAX_TEXT_LENGTH, " TD: %.1f 'C ", fabs(pData->paras.wTemps[0]/10.0- pData->paras.wTemps[1]/10.0));
		else	
			snprintf(sInfo.strText[11], MAX_TEXT_LENGTH, " TD:--.- 'C "); 
	}
	else{
		if(pData->paras.wTemps[0] !=0)
			snprintf(sInfo.strText[9], MAX_TEXT_LENGTH, " T1: %.1f 'F ",  pData->paras.wTemps[0]/10.0* 1.8 +32.0); 
		else	
			snprintf(sInfo.strText[9], MAX_TEXT_LENGTH, " T1:--.- 'F "); 
		if(pData->paras.wTemps[1] !=0)
			snprintf(sInfo.strText[10], MAX_TEXT_LENGTH, " T2: %.1f 'F ",  pData->paras.wTemps[1]/10.0* 1.8 +32.0); 
		else	
			snprintf(sInfo.strText[10], MAX_TEXT_LENGTH, " T2:--.- 'F "); 
		if(pData->paras.wTemps[0] !=0 && pData->paras.wTemps[1] !=0)
			snprintf(sInfo.strText[11], MAX_TEXT_LENGTH, " TD: %.1f 'F ", fabs(pData->paras.wTemps[0]/10.0* 1.8- pData->paras.wTemps[1]/10.0* 1.8));
		else	
			snprintf(sInfo.strText[11], MAX_TEXT_LENGTH, " TD:--.- 'F "); 
		
	}
	
	if(gbHaveCo2){
		if(gCfgCO2.bUnit == CO2UNIT_MMHG){
			if(pData->paras.sCO2.wEtCO2 !=0)
				snprintf(sInfo.strText[12], MAX_TEXT_LENGTH, " etCO2: %3d mmHg ",  pData->paras.sCO2.wEtCO2); 
			else	
				snprintf(sInfo.strText[12], MAX_TEXT_LENGTH, " etCO2: -- mmHg "); 
		}
		else{
			if(pData->paras.sCO2.wEtCO2 !=0)
				snprintf(sInfo.strText[12], MAX_TEXT_LENGTH, " etCO2: %.1f %%",  (float)(pData->paras.sCO2.wEtCO2*100.0/gValueCO2.wBaro)); 
			else	
				snprintf(sInfo.strText[12], MAX_TEXT_LENGTH, " etCO2: --.- %% "); 
		}			
	}
		
	if(gbHaveIbp){
		//另外打印一列
	}
	
	//打印参数信息
	res = Printer_PrintString(&sInfo);
	
		
	//根据不同的报警显示不同的波形
	switch(pData->bAlmID){
		case AP_HR_HIGH:	
		case AP_HR_LOW:
		case AP_ECG_WEAKSIGNAL:			
		case AP_SPO2_HIGH:
		case AP_SPO2_LOW:
		case AP_SPO2_LOWEST:
		case AP_PR_HIGH:			
		case AP_PR_LOW:	
		case AP_RR_HIGH:
		case AP_RR_LOW:
		case AP_APNEA:{
			//Wave: ECG1 + Pleth + Resp
			bWavesType = 1;
		}break;	
		case AP_NSYS_HIGH:
		case AP_NSYS_LOW:
		case AP_NDIA_HIGH:
		case AP_NDIA_LOW:
		case AP_NMEAN_HIGH:
		case AP_NMEAN_LOW:
		case AP_T1_HIGH:
		case AP_T1_LOW:
		case AP_T2_HIGH:
		case AP_T2_LOW:		
		case AP_TD_HIGH:{
			//Waves : None
			bWavesType = 0;			
		}break;
		case AP_ETCO2_HIGH:
		case AP_ETCO2_LOW:				
		case AP_FICO2_HIGH:{
			//Waves : ECG1 + Pleth + Co2
			bWavesType = 2;	
		}break;
		case AP_ISYS1_HIGH:
		case AP_ISYS1_LOW:
		case AP_IDIA1_HIGH:
		case AP_IDIA1_LOW:
		case AP_IMEAN1_HIGH:
		case AP_IMEAN1_LOW:
		case AP_ISYS2_HIGH:
		case AP_ISYS2_LOW:
		case AP_IDIA2_HIGH:
		case AP_IDIA2_LOW:
		case AP_IMEAN2_HIGH:
		case AP_IMEAN2_LOW:{
			//Waves : ECG1 + IBP1 + IBP2 	
			bWavesType = 3;
		}break;
		default: bWavesType = 0;
	}

	
	//清空文字
	memset(sInfo.strText, 0, MAX_ROW_COUNT*MAX_TEXT_LENGTH);
	/*
	switch(gCfgPrinter.bSpeed){
		case 0://12.5mms
			snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, "12.5mm/s");
		break;
		case 1://25mms
			snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, "25mm/s");
		break;
		case 2://50mms
			snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, "50mm/s");
		break;
		default://50mms
			snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, "12.5mm/s");
		break;
	}
	*/
		snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, "12.5mm/s");
	//波形信息
	switch(bWavesType){
		case 1:{
			//Wave: ECG1 + Pleth + Resp
			snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, " %s", strEcgLeadName[gCfgEcg.bChannel1]);
			snprintf(sInfo.strText[6], MAX_TEXT_LENGTH, " Pleth");
			snprintf(sInfo.strText[9], MAX_TEXT_LENGTH, " Resp");
		}break;
		case 2:{
			//Waves : ECG1 + Pleth + Co2
			snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, " %s", strEcgLeadName[gCfgEcg.bChannel1]);
			snprintf(sInfo.strText[6], MAX_TEXT_LENGTH, " Pleth");
			snprintf(sInfo.strText[9], MAX_TEXT_LENGTH, " CO2");
		}break;
		case 3:{
			//Waves : ECG1 + IBP1 + IBP2 	
			snprintf(sInfo.strText[3], MAX_TEXT_LENGTH, " %s", strEcgLeadName[gCfgEcg.bChannel1]);
			//TODO:此处要显示标名
			snprintf(sInfo.strText[6], MAX_TEXT_LENGTH, " IBP1");
			snprintf(sInfo.strText[9], MAX_TEXT_LENGTH, " IBP2");
		}break;
		default:{
			
			Printer_MovePaper(BLANKPAPER_WIDTH);
			return 0;
		}break;
	}
	
	//打印波形信息
	res = Printer_PrintString(&sInfo);
	

	//填充波形
	//0-ECG 1
	k = 0 ;
	for(i=0; i<gCfgAlarm.bPrintWaveLength; i++){
		for(j=SAMPLING_RATE-1; j>=0; j--){
			if(gCfgEcg.bChannel1 == ECGWAVE_I){
				//Lead I
				iTmp1 =pData->waves[i][j].bEcg1;
			}                       
			else if(gCfgEcg.bChannel1 == ECGWAVE_II){
				//Lead II
				iTmp1 = pData->waves[i][j].bEcg2;	
			}
			else{
				
				if(gCfgEcg.bLeadType == ECGTYPE_3LEAD){
					iTmp1 = pData->waves[i][j].bEcg3;	
				}
				else{
					//Lead III = II - I
					iTmp1 = pData->waves[i][j].bEcg2 - pData->waves[i][j].bEcg1 ;
				}
			}
			PrintAlarmWaves.iWave[0][k] = ECG_Value_To_Printer(iTmp1);;
			k++;
		}
	}
	
	switch(bWavesType){
		case 1:{
			//Wave: ECG1 + Pleth + Resp
			k = 0 ;
			for(i=0; i<gCfgAlarm.bPrintWaveLength; i++){
				for(j=SAMPLING_RATE-1; j>=0; j--){
					//SpO2
					PrintAlarmWaves.iWave[1][k] = pData->waves[i][j].bSpO2/3;
					//Resp
					PrintAlarmWaves.iWave[2][k] = pData->waves[i][j].bResp/3;
					k++;
				}
			}
		}break;
		case 2:{
			//Waves : ECG1 + Pleth + Co2
			k = 0 ;
			for(i=0; i<gCfgAlarm.bPrintWaveLength; i++){
				for(j=SAMPLING_RATE-1; j>=0; j--){
					//SpO2
					PrintAlarmWaves.iWave[1][k] = pData->waves[i][j].bSpO2*2;
					//CO2
					PrintAlarmWaves.iWave[2][k] = pData->waves[i][j].bCO2;
					k++;
				}
			}
		}break;
		case 3:{
			//Waves : ECG1 + IBP1 + IBP2 	
			k = 0 ;
			for(i=0; i<gCfgAlarm.bPrintWaveLength; i++){
				for(j=SAMPLING_RATE-1; j>=0; j--){
					//IBP1
					PrintAlarmWaves.iWave[1][k] = pData->waves[i][j].iIbp1;
					//IBP2
					PrintAlarmWaves.iWave[2][k] = pData->waves[i][j].iIbp2;
					k++;
				}
			}
		}break;
		default:{
			
			Printer_MovePaper(BLANKPAPER_WIDTH);
			return 0;
		}break;
	}
		//打印波形
  	//	res = RecorderStartPrintCopyWave(&PrintAlarmWaves, &PrintAlarmParam);	
	sleep(1);
	Printer_PrintCopyWave(&PrintAlarmWaves);
	
//	Printer_StopCurWork();	
	
		
//	Printer_MovePaper(15);
	Printer_Move_Paper(15);

	gPrinterWorkType = PRNWORK_NONE;
	
	return 0;
}

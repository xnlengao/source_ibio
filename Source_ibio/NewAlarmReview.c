/*************************************************************************** 
 *	Module Name:	AlarmReview
 *
 *	Abstract:	报警回顾
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-08-06 16:22:42
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "General_Funcs.h"
#include "AlarmManage.h"
#include "Global.h"
#include "Dialog.h"
#include "DrawWave.h"

//DID_ALARMREVIEW
#define DID_ALMREV_HELP		DID_ALARMREVIEW
#define DID_ALMREV_OK			DID_ALARMREVIEW+1
#define DID_ALMREV_PREV		DID_ALARMREVIEW+2
#define DID_ALMREV_NEXT		DID_ALARMREVIEW+3
#define DID_ALMREV_MOVE		DID_ALARMREVIEW+4
#define DID_ALMREV_BROWSE		DID_ALARMREVIEW+5
#define DID_ALMREV_PARAS		DID_ALARMREVIEW+6
#define DID_ALMREV_WAVE		DID_ALARMREVIEW+7
#define DID_ALMREV_PAGE		DID_ALARMREVIEW+8
#define DID_ALMREV_PRINT		DID_ALARMREVIEW+9
#define DID_ALMREV_CAPTION		DID_ALARMREVIEW+10
//显示参数标志
#define ALMPARA_ECG 	0
#define ALMPARA_SPO2 	1
#define ALMPARA_RESP 	2
#define ALMPARA_CO2 	3
#define ALMPARA_NIBP 	4
#define ALMPARA_TEMP	5
#if SCREEN_640
//一屏幕有多少秒波形
#define SEC_EACHSCREEN	5
//波形区域的宽度
#define WAVE_LENGHT	315
//波形区域的x轴起始点
#define X_WAVEDRAW		100

#else
//一屏幕有多少秒波形
#define SEC_EACHSCREEN	7
//波形区域的宽度
#define WAVE_LENGHT	440
//波形区域的x轴起始点
#define X_WAVEDRAW		90
#endif
static HWND editHelp;
static HWND stcCaption;
static HWND btnOK;
static HWND btnPrev;
static HWND btnNext;
static HWND btnMove;
static HWND btnBrowse;
static HWND btnPage;
static HWND btnPrint;
static HWND staParas = (HWND)NULL;		//参数显示面板

static WNDPROC  OldBtnProc;
static WNDPROC  OldSta0Proc;
static WNDPROC  OldSta1Proc;
static WNDPROC  OldSta2Proc;
static WNDPROC  OldSta3Proc;
static WNDPROC  OldEditProcHelp;

static DLGTEMPLATE DlgSet = {
	WS_BORDER | WS_VISIBLE ,
	WS_EX_NOCLOSEBOX,   
//    	5, 175, 545, 397,    
#if SCREEN_640
//	50, 175, 535, 335-50,   
	50, 175, 535-115, 335-50,    
#else
	50, 175, 535, 335-50,   
#endif
	"",
	0, 0,
	1,      
	NULL,
	0
};

static CTRLDATA CtrlSet[] = {
	{
		//"mledit",
		"static",
		WS_CHILD | WS_VISIBLE ,
		5, 230-50, X_WAVEDRAW-10,95,
		DID_ALMREV_HELP,
		"",
		0
	}
};

//游标框颜色
#define CURSOUR_COLOR		COLOR_darkred

//一页显示的事件数量
#define EVENT_COUNT_ONEPAGE	10
//行数(包括表头)
#define ROW_COUNT 	(EVENT_COUNT_ONEPAGE+1)
//列数(事件,时间)
#define COL_COUNT 	2

//列表显示的文字的长度
#define ITEM_TXTLENGTH		25	
//Alarm Review Struct
typedef struct {
	RECT rect;
	unsigned char *strValue[ITEM_TXTLENGTH];
}TABLE_ITEM, *PTABLE_ITEM;

//事件列表(包括表头) TODO:装载所有的数据 
// static TABLE_ITEM gTableEvent[ROW_COUNT][COL_COUNT];
//#define ALMDATA_COUNT		MAXREALSAVECOUNT+1 
#define ALMDATA_COUNT		MAXALARMDATACOUNT+1   //16/08/2011 15:32:27
static TABLE_ITEM gTableEvent[ALMDATA_COUNT][COL_COUNT];

//页数
static int giPageCount = 0;
//当前页
static int giCurPage = 0;
//当前事件
static int giCurEvent = 0;
static HWND hWndSelf = (HWND)NULL;

//事件框架区域
static RECT gRcEventForm;

//状态显示区域
static RECT gRcStatus;

//波形回顾时间区域
static RECT gRcWaveTime;

//绘制游标的索引
static int CursorIndex = 1;

//显示波形的数量
#define WAVE_COUNT		3
//波形显示面板
static WAVE_PANEL	wp[WAVE_COUNT];

//是否处在浏览记录的状态
static BOOL bBrowseData = FALSE;
//是否处在浏览波形的状态
static BOOL bBrowseWave = FALSE;
//是否处在翻页的状态
static BOOL bBrowsePage = FALSE;

//排序好的报警数据存储
CA_ALMDATAARRAY	gAlmDataArraySorted;


//数组数量
static int gAlmDataCount = 0;
static int giHeadIndex = 0;
static int giTailIndex = 0;

//每页显示的索引限值
static int giPageIndexStart = 0;
static int giPageIndexEnd = 0;

//当前回放的报警信息索引
static int giIndexOfReviewData = 0;
//取出的单条报警信息
 ALARMDATA gAlmDataForReview;

//回放波形的索引,暂定为32秒 0~32
int gWaveIndexStart = 12;	//-4s
int gWaveIndexEnd = 16;		//0s		
		
//报警数据存储数组
extern CA_ALMDATAARRAY	gAlmDataArray;
//报警信息字符串
extern unsigned char strPhyAlmInfo[ALM_PHY_INFOMAX][ALM_PHY_INFOLEN];

//ECG定标
extern float gfNMMToPixel;				///1mm=3.2pixel
extern float gSamplesPerMV;	

//报警打印信息字符串
extern unsigned char *gStrPhyAlmInfoPrintTxt[] ;

extern sem_t semReal_Print;
//---------- 函数声明 ----------
//初始化区域
static int InitRects();
//填充列表内容
static int FillDatas(int iStartIndex);
//绘制边框
static int DrawForm(HWND hWnd);
//绘制表格内容
static int DrawDatas(HWND hWnd, int iPage);
//绘制游标框,供浏览用
static int DrawCursour(HWND hWnd, RECT rc);
//绘制状态
static int DrawStatus(HWND hWnd);
//绘制波形时间
static int DrawWaveTime(HWND hWnd);
//初始化波形绘图环境的属性(与绘图相同)
static int InitWavePanels();
//销毁波形绘制面板
static int DestroyWavePanels();
//得到一条报警数据
static int GetOneAlarmData(int iIndex);
//绘制参数
static int DrawParas(HWND hWnd);
//绘制波形
static int DrawWave_Ecg(PWAVE_PANEL pPanel, int iStartIndex, int iEndIndex);
static int DrawWave_SpO2(PWAVE_PANEL pPanel, int iStartIndex, int iEndIndex);
static int DrawWave_Resp(PWAVE_PANEL pPanel, int iStartIndex, int iEndIndex);
static int DrawWave_CO2(PWAVE_PANEL pPanel, int iStartIndex, int iEndIndex);
static int DrawWave();
//翻页时更新一页
static int UpdatePage(HWND hWnd, int iPage);

/*
	帮助控件回调函数
*/
static int EditProcHelp(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	char strHelp[200] = {0};
	//设置帮助栏背景框颜色和字体颜色
	SetWindowBkColor(hWnd,COLOR_black);
#ifdef FONTCOLOR	
	SetWindowElementColorEx( hWnd, FGC_CONTROL_NORMAL,COLOR_yellow);
#endif
	switch(message){
		case MSG_PAINT:{
			id = lParam;
			switch(id){
				case DID_ALMREV_OK:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "help_backtomain", strHelp, sizeof strHelp,"Back to Main.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALMREV_BROWSE:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMREVIEW, "help_browse", strHelp, sizeof strHelp,"Scroll record.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALMREV_PRINT:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMREVIEW, "help_print", strHelp, sizeof strHelp,"Print alarm review.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALMREV_MOVE:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMREVIEW, "help_move", strHelp, sizeof strHelp,"Scroll review time.");
					SetWindowText(hWnd, strHelp);
				}break;
				default:{
					SetWindowText(hWnd, "");
				}break;
			}break;
		}break;	
	}

	temp = (*OldEditProcHelp)(hWnd, message, wParam, lParam);
	return(temp);
}

//改变参数的报警级别
static int ChangeCO2AlarmClass()
{
	IsAlm_EtCO2(&gValueCO2, &gCfgCO2, TRUE);
	IsAlm_FiCO2(&gValueCO2, &gCfgCO2, TRUE);
	
	return 0;
}


		
//复制并且排序当前的实时数据数组
//按照由早(Tail=0)到晚(Head=New)的顺序取出数据
static BOOL CopyAlmData()
{
	WORD wHead, wTail, wSize;
	BOOL bLoop;
	int i;
	int iCount = 0;
	BOOL bHave;

	if(gAlmDataArray.wCount ==0){
		gAlmDataCount = 0;
		giHeadIndex = 0;
		giTailIndex = 0;
		return FALSE;
	}
			
	wHead = gAlmDataArray.wHeadIndex;
	wTail = gAlmDataArray.wTailIndex;
	bLoop = gAlmDataArray.bLoop;
	wSize = MAXALARMDATACOUNT;
	

	
	//初始化排序数组
	memset(gAlmDataArraySorted.Datas, 0, sizeof gAlmDataArraySorted.Datas);
	gAlmDataArraySorted.wHeadIndex = 0;
	gAlmDataArraySorted.wTailIndex = 0;
	gAlmDataArraySorted.wCount = 0;
	gAlmDataArraySorted.bLoop = FALSE;
	
	if(B_PRINTF) printf("%s:%d Source Array: Count is %d, Head:%d Tail:%d.\n", __FILE__, __LINE__,
	       gAlmDataArray.wCount, 
	       gAlmDataArray.wHeadIndex, 
	       gAlmDataArray.wTailIndex);
	
	

	//如果已经存满，则翻转进行存储
	if(wHead < wTail){
		if(B_PRINTF) printf(" Rollback \n");
		if(B_PRINTF) printf("%s:%d  wHead:%d wTail:%d.\n", __FILE__, __LINE__,wHead,wTail);
		for(i=wTail; i<=(wSize-1); i++){
			gAlmDataArraySorted.Datas[iCount] = gAlmDataArray.Datas[i];
			iCount ++;
			if(B_PRINTF) printf(" %d ", i);
			if(i%10==0)
				{
					if(B_PRINTF) printf("\n");
				}
		}
		if(B_PRINTF) printf("\n");
		for(i=0; i<=wHead; i++){
			gAlmDataArraySorted.Datas[iCount] = gAlmDataArray.Datas[i];
			iCount ++;
			if(B_PRINTF) printf(" %d ", i);
			if(i%10==0)
				{
					if(B_PRINTF) printf("\n");
				}
		}
	}
	else if(bLoop){
 		if(B_PRINTF) printf(" Spacial \n");
		if(B_PRINTF) printf("%s:%d  wHead:%d wTail:%d.\n", __FILE__, __LINE__,wHead,wTail);
		for(i=wTail; i<=wHead; i++){
			gAlmDataArraySorted.Datas[iCount] = gAlmDataArray.Datas[i];
			iCount ++;
			if(B_PRINTF) printf(" %d ", i);
			if(i%10==0)
				{
					if(B_PRINTF) printf("\n");
				}
		}
	}
	else{
		if(B_PRINTF) printf(" Normal \n");
		if(B_PRINTF) printf("%s:%d  wHead:%d wTail:%d.\n", __FILE__, __LINE__,wHead,wTail);
		for(i=(wTail+1); i<=wHead; i++){
			gAlmDataArraySorted.Datas[iCount] = gAlmDataArray.Datas[i];
			iCount ++;
			if(B_PRINTF) printf(" %d ", i);
			if(i%10==0)
				{
					if(B_PRINTF) printf("\n");
				}
		}
	}
	if(B_PRINTF) printf("\n");

	gAlmDataArraySorted.wHeadIndex = iCount-1;
	gAlmDataArraySorted.wCount = iCount;
	
	gAlmDataCount = gAlmDataArraySorted.wCount;
	giHeadIndex = gAlmDataArraySorted.wHeadIndex;
	giTailIndex = gAlmDataArraySorted.wTailIndex;
	
	if(B_PRINTF) printf("%s:%d Sorted Array: Count is %d, Head:%d Tail:%d.\n", __FILE__, __LINE__,
	       gAlmDataArraySorted.wCount, 
	       gAlmDataArraySorted.wHeadIndex, 
	       gAlmDataArraySorted.wTailIndex);
	
	return TRUE;
}

static  int PrintAlarmData(const PALARMDATA pData)
{
	static unsigned char *strEcgLeadName[]={"I", "II", "III", "aVR", "aVL", "aVF", "V"};
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
		if(B_PRINTF) printf("%s:%d Printer may be error. \n", __FILE__, __LINE__);
		return -1;
	} 
	
	//如果有打印任务，则终止当前打印任务
	if(gPrinterWorkType != PRNWORK_NONE) {
		if(B_PRINTF) printf("%s:%d Printer is busy . \n", __FILE__, __LINE__);
		return -1;
// 		Printer_StopCurWork();
	}
	
	gPrinterWorkType = PRNWORK_COPY;

	
	//Wave
	PrintAlarmWaves.iCount = 3;
	PrintAlarmWaves.iLength = (MAXREALALARMSAVECOUNT)*SAMPLING_RATE;		
	for(i=0; i<PrintAlarmWaves.iCount; i++){
		memset(PrintAlarmWaves.iWave[i], 0, MAXALARMWAVELENGTH);
	}
		
	//设置文字信息
	sInfo.iRowCount = MAX_ROW_COUNT;
	//清空文字
	memset(sInfo.strText, 0, MAX_ROW_COUNT*MAX_TEXT_LENGTH);
	
	//报警信息
	switch(gAlmDataForReview.bAlmClass){
		case ALARM_LOW:{
			snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, " *%s", 
				 gStrPhyAlmInfoPrintTxt[gAlmDataForReview.bAlmID]); 
		}break;
		case ALARM_MID:{
			snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, " **%s", 
				 gStrPhyAlmInfoPrintTxt[gAlmDataForReview.bAlmID]); 
		}break;
		case ALARM_HIGH:{
			snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, " ***%s", 
				 gStrPhyAlmInfoPrintTxt[gAlmDataForReview.bAlmID]); 
		}break;
		default:{
			snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, " %s", 
				 gStrPhyAlmInfoPrintTxt[gAlmDataForReview.bAlmID]); 
		}break;
	}
	
	//时间信息
	snprintf(sInfo.strText[1], MAX_TEXT_LENGTH, " %.2d:%.2d:%.2d ", 
		 gAlmDataForReview.paras.sTime.bHour,
		 gAlmDataForReview.paras.sTime.bMin,
		 gAlmDataForReview.paras.sTime.bSec);
	snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, " %.4d/%.2d/%.2d ", 
		 gAlmDataForReview.paras.sTime.wYear, 
		 gAlmDataForReview.paras.sTime.bMonth, 
		 gAlmDataForReview.paras.sTime.bDay);
	
	//参数信息 
	if(gAlmDataForReview.paras.wHR !=0)
		snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, " HR: %3d bpm ",  gAlmDataForReview.paras.wHR); 
	else
		snprintf(sInfo.strText[4], MAX_TEXT_LENGTH, " HR: --- bpm "); 
	if(gAlmDataForReview.paras.bSpO2 !=0)
		snprintf(sInfo.strText[5], MAX_TEXT_LENGTH, " SpO2: %3d %% ",  gAlmDataForReview.paras.bSpO2); 
	else
		snprintf(sInfo.strText[5], MAX_TEXT_LENGTH, " SpO2: -- %% "); 
	if(gAlmDataForReview.paras.wPR !=0)
		snprintf(sInfo.strText[6], MAX_TEXT_LENGTH, " PR: %3d bpm ",  gAlmDataForReview.paras.wPR); 
	else
		snprintf(sInfo.strText[6], MAX_TEXT_LENGTH, " PR: --- bpm "); 
	if(!gValueResp.bOff)
		snprintf(sInfo.strText[7], MAX_TEXT_LENGTH, " RR: %3d rpm ",  gAlmDataForReview.paras.wRR); 
	else
		snprintf(sInfo.strText[7], MAX_TEXT_LENGTH, " RR: --- rpm "); 
	if(gCfgNibp.bUnit == NIBP_UNIT_MMHG){
		if(gAlmDataForReview.paras.sNIBP.wSys !=0)
			snprintf(sInfo.strText[8], MAX_TEXT_LENGTH, " NIBP: %3d/%3d(%3d) mmHg ",  
				 gAlmDataForReview.paras.sNIBP.wSys, gAlmDataForReview.paras.sNIBP.wDia, gAlmDataForReview.paras.sNIBP.wMean); 
		else
			snprintf(sInfo.strText[8], MAX_TEXT_LENGTH, " NIBP: ---/---(---)mmHg ");  
	}else{
		if(gAlmDataForReview.paras.sNIBP.wSys !=0)
			snprintf(sInfo.strText[8], MAX_TEXT_LENGTH, " NIBP: %.1f/%.1f(%.1f) kPa ",  
				 gAlmDataForReview.paras.sNIBP.wSys/mmHgTokPa, gAlmDataForReview.paras.sNIBP.wDia/mmHgTokPa, gAlmDataForReview.paras.sNIBP.wMean/mmHgTokPa); 
		else
			snprintf(sInfo.strText[8], MAX_TEXT_LENGTH, " NIBP: --.-/--.-(--.-)kPa");  
	}
				
	if(gCfgTemp.bUnit == TEMP_UNIT_C){
		if(gAlmDataForReview.paras.wTemps[0] !=0)
			snprintf(sInfo.strText[9], MAX_TEXT_LENGTH, " T1: %.1f 'C ",  gAlmDataForReview.paras.wTemps[0]/10.0); 
		else	
			snprintf(sInfo.strText[9], MAX_TEXT_LENGTH, " T1:--.- 'C "); 
		if(gAlmDataForReview.paras.wTemps[1] !=0)
			snprintf(sInfo.strText[10], MAX_TEXT_LENGTH, " T2: %.1f 'C ",  gAlmDataForReview.paras.wTemps[1]/10.0); 
		else	
			snprintf(sInfo.strText[10], MAX_TEXT_LENGTH, " T2:--.- 'C "); 
		if(gAlmDataForReview.paras.wTemps[0] !=0 && gAlmDataForReview.paras.wTemps[1] !=0)
			snprintf(sInfo.strText[11], MAX_TEXT_LENGTH, " TD: %.1f 'C ", fabs(gAlmDataForReview.paras.wTemps[0]/10.0- gAlmDataForReview.paras.wTemps[1]/10.0));
		else	
			snprintf(sInfo.strText[11], MAX_TEXT_LENGTH, " TD:--.- 'C "); 
	}
	else{
		if(gAlmDataForReview.paras.wTemps[0] !=0)
			snprintf(sInfo.strText[9], MAX_TEXT_LENGTH, " T1: %.1f 'F ",  gAlmDataForReview.paras.wTemps[0]/10.0* 1.8 +32.0); 
		else	
			snprintf(sInfo.strText[9], MAX_TEXT_LENGTH, " T1:--.- 'F "); 
		if(gAlmDataForReview.paras.wTemps[1] !=0)
			snprintf(sInfo.strText[10], MAX_TEXT_LENGTH, " T2: %.1f 'F ",  gAlmDataForReview.paras.wTemps[1]/10.0* 1.8 +32.0); 
		else	
			snprintf(sInfo.strText[10], MAX_TEXT_LENGTH, " T2:--.- 'F "); 
		if(gAlmDataForReview.paras.wTemps[0] !=0 && gAlmDataForReview.paras.wTemps[1] !=0)
			snprintf(sInfo.strText[11], MAX_TEXT_LENGTH, " TD: %.1f 'F ", fabs(gAlmDataForReview.paras.wTemps[0]/10.0* 1.8- gAlmDataForReview.paras.wTemps[1]/10.0* 1.8));
		else	
			snprintf(sInfo.strText[11], MAX_TEXT_LENGTH, " TD:--.- 'F "); 
		
	}
	
	if(gbHaveCo2){
		if(gCfgCO2.bUnit == CO2UNIT_MMHG){
			if(gAlmDataForReview.paras.sCO2.wEtCO2 !=0)
				snprintf(sInfo.strText[12], MAX_TEXT_LENGTH, " etCO2: %3d mmHg ",  gAlmDataForReview.paras.sCO2.wEtCO2); 
			else	
				snprintf(sInfo.strText[12], MAX_TEXT_LENGTH, " etCO2: -- mmHg "); 
		}
		else{
			if(gAlmDataForReview.paras.sCO2.wEtCO2 !=0)
				snprintf(sInfo.strText[12], MAX_TEXT_LENGTH, " etCO2: %.1f %%",  (float)(gAlmDataForReview.paras.sCO2.wEtCO2*100.0/gValueCO2.wBaro)); 
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
	switch(gAlmDataForReview.bAlmID){
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
	//速度
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
			//Printer_MovePaper(15);
			Printer_Move_Paper(15);
			return 0;
		}break;
	}
	
	//打印波形信息
	res = Printer_PrintString(&sInfo);
	
	
	//填充波形
	//0-ECG 1
	k = 0 ;
	for(i=0; i<MAXREALALARMSAVECOUNT; i++){
		if(B_PRINTF) printf("Print %d.\n", i);
		for(j=SAMPLING_RATE-1; j>=0; j--){
			if(gCfgEcg.bChannel1 == ECGWAVE_I){
				//Lead I
				iTmp1 =gAlmDataForReview.waves[i][j].bEcg1;
			}                       
			else if(gCfgEcg.bChannel1 == ECGWAVE_II){
				//Lead II
				iTmp1 = gAlmDataForReview.waves[i][j].bEcg2;	
			}
			else{
				if(gCfgEcg.bLeadType == ECGTYPE_3LEAD){
					iTmp1 = gAlmDataForReview.waves[i][j].bEcg3;	
				}
				else{
					//Lead III = II - I
					iTmp1 = gAlmDataForReview.waves[i][j].bEcg2 - gAlmDataForReview.waves[i][j].bEcg1 ;
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
			for(i=0; i<MAXREALALARMSAVECOUNT; i++){
				for(j=SAMPLING_RATE-1; j>=0; j--){
					//SpO2
					PrintAlarmWaves.iWave[1][k] = gAlmDataForReview.waves[i][j].bSpO2/3;
					//Resp
					PrintAlarmWaves.iWave[2][k] = gAlmDataForReview.waves[i][j].bResp/3;
					k++;
				}
			}
		}break;
		case 2:{
			//Waves : ECG1 + Pleth + Co2
			k = 0 ;
			for(i=0; i<MAXREALALARMSAVECOUNT; i++){
				for(j=SAMPLING_RATE-1; j>=0; j--){
					//SpO2
					PrintAlarmWaves.iWave[1][k] = gAlmDataForReview.waves[i][j].bSpO2;
					//CO2
					PrintAlarmWaves.iWave[2][k] = gAlmDataForReview.waves[i][j].bCO2;
					k++;
				}
			}
		}break;
		case 3:{
			//Waves : ECG1 + IBP1 + IBP2 	
			k = 0 ;
			for(i=0; i<MAXREALALARMSAVECOUNT/2; i++){
				for(j=SAMPLING_RATE-1; j>=0; j--){
					//IBP1
					PrintAlarmWaves.iWave[1][k] = gAlmDataForReview.waves[i][j].iIbp1;
					//IBP2
					PrintAlarmWaves.iWave[2][k] = gAlmDataForReview.waves[i][j].iIbp2;
					k++;
				}
			}
		}break;
		default:{
		//	Printer_MovePaper(15);
			Printer_Move_Paper(15);
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
int Print_AlmReview_Data()
{
	PrintAlarmData(&gAlmDataForReview);
//	Printer_PrintAlarmData(&gAlmDataForReview);
	return 0;
}		
		
static  int BtnProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	
	id = GetDlgCtrlID(hWnd);
		
	temp = (*OldBtnProc)(hWnd, message, wParam, lParam);
	
	switch(message){
		case MSG_SETFOCUS:{
			if(bBrowseData || bBrowseWave || bBrowsePage){
				SetWindowBkColor(hWnd, BROWSE_COLOR);	
			}
			else{
				SetWindowBkColor(hWnd, SETFOCUS_COLOR);	
			}
			SendMessage(editHelp, MSG_PAINT, 0, (LPARAM)GetDlgCtrlID(hWnd));
		}break;
		case MSG_KILLFOCUS:{
			SetWindowBkColor(hWnd, KILLFOCUS_COLOR);
		}break;
		case MSG_KEYUP:{
			switch(wParam){
				case SCANCODE_ENTER:{
					switch(id){
						case DID_ALMREV_BROWSE:{
							bBrowseData = ! bBrowseData;
							if(bBrowseData){
								gbKeyType = KEY_AD;
								SetWindowBkColor(hWnd, BROWSE_COLOR);
							} 
							else{
								gbKeyType = KEY_TAB;
								SetWindowBkColor(hWnd, SETFOCUS_COLOR);	
							}
						}break;		
						case DID_ALMREV_MOVE:{
							bBrowseWave = !bBrowseWave;
							if(bBrowseWave){
								gbKeyType = KEY_AD;
								SetWindowBkColor(hWnd, BROWSE_COLOR);
							} 
							else{
								gbKeyType = KEY_TAB;
								SetWindowBkColor(hWnd, SETFOCUS_COLOR);	
							}
						}break;		
						case DID_ALMREV_PAGE:{
							bBrowsePage = !bBrowsePage;
							if(bBrowsePage){
								gbKeyType = KEY_AD;
								SetWindowBkColor(hWnd, BROWSE_COLOR);
							} 
							else{
								gbKeyType = KEY_TAB;
								SetWindowBkColor(hWnd, SETFOCUS_COLOR);	
							}
						}break;		
					}//end switch(id)
				}break;
			}//end switch(wParam)
		}break;
	}
	
// 	temp = (*OldBtnProc)(hWnd, message, wParam, lParam);
	return(temp);
}
     
static int Sta0Proc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	
	id = GetDlgCtrlID(hWnd);
		
	temp = (*OldSta0Proc)(hWnd, message, wParam, lParam);
	
	switch(message){
		case MSG_PAINT:{
			if(gAlmDataArray.wCount == 0){
				if(B_PRINTF) printf("%s:%d No Alarm data.\n", __FILE__, __LINE__);
				return -1;	
			} 

			DrawParas(hWnd);
			return TRUE;
		}break;
	}
	
	return(temp);
}
     
     
static int Sta1Proc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	
	id = GetDlgCtrlID(hWnd);
		
	temp = (*OldSta1Proc)(hWnd, message, wParam, lParam);
	
	switch(message){
		case MSG_PAINT:{
			if(gAlmDataArray.wCount == 0){
				if(B_PRINTF) printf("%s:%d No Alarm data.\n", __FILE__, __LINE__);
				return -1;	
			} 
 	
			DrawWave_Ecg(&wp[0], gWaveIndexStart, gWaveIndexEnd);
			return TRUE;
		}break;
	}
	
	return(temp);
}

static int Sta2Proc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	
	id = GetDlgCtrlID(hWnd);
		
	temp = (*OldSta2Proc)(hWnd, message, wParam, lParam);
	
	switch(message){
		case MSG_PAINT:{
			if(gAlmDataArray.wCount == 0){
				if(B_PRINTF) printf("%s:%d No Alarm data.\n", __FILE__, __LINE__);
				return -1;	
			} 
			DrawWave_SpO2(&wp[1], gWaveIndexStart, gWaveIndexEnd);
			return TRUE;
		}break;
	}
	
	return(temp);
}

static int Sta3Proc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	
	id = GetDlgCtrlID(hWnd);
		
	temp = (*OldSta3Proc)(hWnd, message, wParam, lParam);
	
	switch(message){
		case MSG_PAINT:{
			if(gAlmDataArray.wCount == 0){
				if(B_PRINTF) printf("%s:%d No Alarm data.\n", __FILE__, __LINE__);
				return -1;	
			} 
			if(gbHaveCo2){
				DrawWave_CO2(&wp[2], gWaveIndexStart, gWaveIndexEnd);
			}
			else{
				DrawWave_Resp(&wp[2], gWaveIndexStart, gWaveIndexEnd);
			}
			
			return TRUE;
		}break;
	}
	
	return(temp);
}

static int DlgProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	char strMenu[100];
	int res; 
	
	switch(message){
		case MSG_INITDIALOG:{
 			gbKeyType = KEY_TAB;
						
			hWndSelf = hDlg;
			
			//绘制游标的索引
			CursorIndex = 1;

			//波形索引
// 			gWaveIndexStart = 12;	//-4s
// 			gWaveIndexEnd = 16;	//0s
			gWaveIndexStart = 16;	//0s
			gWaveIndexEnd = 16+SEC_EACHSCREEN;	
			
			//是否处在浏览记录的状态
			bBrowseData = FALSE;
			//是否处在浏览波形的状态
			bBrowseWave = FALSE;
			//是否处在翻页状态
			bBrowsePage = FALSE;
		
			editHelp = GetDlgItem(hDlg, DID_ALMREV_HELP);
			OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp); 

			//caption
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMREVIEW, "caption", strMenu, sizeof strMenu,"ALARM REVIEW");	
			stcCaption = CreateWindow("static", strMenu, WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE, 
					     DID_ALMREV_CAPTION, 0, 5, X_WAVEDRAW-10, 25, hDlg, 0);
			
			//Print
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMREVIEW, "print", strMenu, sizeof strMenu,"Print");	
			btnPrint = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					       DID_ALMREV_PRINT, 5, 30, X_WAVEDRAW-10, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnPrint, BtnProc);
			if(gPrinterStatus == PRNSTATUS_ERR) 
				EnableWindow(btnPrint, FALSE);
			
	/*
			//Page翻页
			memset(strMenu, 0, sizeof strMenu);
 			GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "page", strMenu, sizeof strMenu);	
 			btnPage = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
 					       DID_ALMREV_PAGE, 5, 40, 80, 26, hDlg, 0);
 			OldBtnProc = SetWindowCallbackProc(btnPage, BtnProc);
		*/	
		
			//Brwose浏览事件
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMREVIEW, "browse", strMenu, sizeof strMenu,"Browse");	
			btnBrowse = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					       DID_ALMREV_BROWSE, 5, 60, X_WAVEDRAW-10, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnBrowse, BtnProc);
			
			//Move浏览波形
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMREVIEW, "move", strMenu, sizeof strMenu,"Move");	
			btnMove = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					       DID_ALMREV_MOVE, 5, 90, X_WAVEDRAW-10, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnMove, BtnProc);
					
			//退出
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "exit", strMenu, sizeof strMenu,"Exit");	
			btnOK = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					     DID_ALMREV_OK, 5, 120, X_WAVEDRAW-10, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnOK, BtnProc);
			
			SetFocus(btnOK);
			
	
			//参数显示面板
			staParas = CreateWindow("static", "", WS_CHILD | SS_BLACKRECT,
					DID_ALMREV_PARAS,X_WAVEDRAW, 0, WAVE_LENGHT, 70, hDlg, 0);	
			OldSta0Proc = SetWindowCallbackProc(staParas, Sta0Proc);
			ShowWindow(staParas, SW_SHOW);
		
			//波形显示面板
			wp[0].hWnd = (HWND)NULL;
 			wp[0].hWnd = CreateWindow("static", "", WS_CHILD | SS_BLACKRECT,
 					DID_ALMREV_WAVE,X_WAVEDRAW,71, WAVE_LENGHT, 80, hDlg, 0);	
  			OldSta1Proc = SetWindowCallbackProc(wp[0].hWnd, Sta1Proc);
			wp[0].bID = WID_ECG1;
			ShowWindow(wp[0].hWnd, SW_SHOW);
					
			wp[1].hWnd = (HWND)NULL;
			wp[1].hWnd = CreateWindow("static", "", WS_CHILD | SS_BLACKRECT,
					DID_ALMREV_WAVE,X_WAVEDRAW,152, WAVE_LENGHT, 50, hDlg, 0);	
 			OldSta2Proc = SetWindowCallbackProc(wp[1].hWnd, Sta2Proc);
			wp[1].bID = WID_SPO2;
			ShowWindow(wp[1].hWnd, SW_SHOW);
			
			wp[2].hWnd = (HWND)NULL;
			wp[2].hWnd= CreateWindow("static", "", WS_CHILD | SS_BLACKRECT,
					DID_ALMREV_WAVE,X_WAVEDRAW,203, WAVE_LENGHT, 50, hDlg, 0);	
 			OldSta3Proc = SetWindowCallbackProc(wp[2].hWnd, Sta3Proc);
			if(gbHaveCo2){
				wp[2].bID = WID_CO2;
			}
			else{
				wp[2].bID = WID_RESP;
			}
			
			ShowWindow(wp[2].hWnd, SW_SHOW);
			
			//初始化波形回顾时间
//			SetRect(&gRcWaveTime, X_WAVEDRAW, 304-50, WAVE_LENGHT+X_WAVEDRAW-20, 331-50);	
				
			//初始化区域
			InitRects();
			
			//复制并且排序当前的报警数据数组
			CopyAlmData();
			//New-20130114
			//初始化波形绘图环境
 			InitWavePanels();
			//确定索引范围start :giHeadIndex end:giTailIndex
			giCurEvent=1;
			giIndexOfReviewData = giHeadIndex;   
		
			//得到一条报警记录
 			GetOneAlarmData(giIndexOfReviewData);
			
		
			if (gAlmDataCount == 0) {
					giCurEvent=0;
			}
		

		}break;
		case MSG_PAINT:{
			//DrawDatas(hDlg, giCurPage);
			//DrawForm(hDlg);
			
			DrawStatus(hDlg);
			DrawWaveTime(hDlg);
			//光标定在每页的第一条记录
	//		DrawCursour(hWndSelf, gTableEvent[1][0].rect);
			if(gAlmDataCount ==0) break;
 		//	DrawWave();
		//	DrawParas(staParas);
		}break;
		case MSG_COMMAND:{
			int	id   = LOWORD(wParam);

		switch(id){
				case DID_ALMREV_OK:{
					PostMessage(hDlg, MSG_CLOSE, 0, 0L);
				}break;
				case DID_ALMREV_PRINT:{
					if(gAlmDataCount ==0) break;
					//PrintAlarmData(&gAlmDataForReview);
					gPrintingType=PRN_WORK_ALM;
					sem_post(&semReal_Print);
					
				}break;
			}
		}break;
		case MSG_KEYUP:{
			switch(wParam){
				case SCANCODE_ESCAPE:{
					PostMessage(hDlg, MSG_CLOSE, 0, 0L);
				}break;
				//case SCANCODE_CURSORBLOCKLEFT:{
				case SCANCODE_A:{
					if(bBrowsePage){
						SetFocus(btnPage);
						if(giCurPage >1){
							giCurPage --;
							UpdatePage(hDlg, giCurPage);
						}
					}
					else if(bBrowseWave){
						SetFocus(btnMove);

						if(gWaveIndexStart>=1){
							gWaveIndexStart -=1;
							gWaveIndexEnd -=1;
							DrawWave();
						}
						return TRUE;	
					}
					else if(bBrowseData){
						
						//New-20130114
						SetFocus(btnBrowse);
						//if(B_PRINTF) printf("aaaaaaaaaaaaaagiCurEvent:%d,giIndexOfReviewData:%d\n",giCurEvent,giIndexOfReviewData);
						if(giIndexOfReviewData<giHeadIndex){
							if(giCurEvent>1){
								giCurEvent--;
							}
							giIndexOfReviewData ++;    
						//得到一条报警记录
							GetOneAlarmData(giIndexOfReviewData);
							DrawWave();
							DrawParas(staParas);
							DrawStatus(hDlg);
						//	if(B_PRINTF) printf("giHeadIndex:%d,giCurEvent:%d,giIndexOfReviewData:%d\n",giHeadIndex,giCurEvent,giIndexOfReviewData);
						}
						/*
						//此处要考虑每页最多显示的数据量
						SetFocus(btnBrowse);
						
						if(giIndexOfReviewData<giPageIndexStart){
							//移动游标
							if(CursorIndex>1){
								DrawCursour(hWndSelf, gTableEvent[CursorIndex][0].rect);
								CursorIndex --;
								DrawCursour(hWndSelf, gTableEvent[CursorIndex][0].rect);
							}
						
							giIndexOfReviewData ++;    
							//得到一条报警记录
							GetOneAlarmData(giIndexOfReviewData);
							DrawWave();
							DrawParas(staParas);
						
						}
						*/
						return TRUE;
					}
				}break;
				//case SCANCODE_CURSORBLOCKRIGHT:{
				case SCANCODE_D:{
					if(bBrowsePage){
						SetFocus(btnPage);
						if((giCurPage <giPageCount) && (giPageIndexEnd >=giTailIndex)){
							giCurPage ++;
							UpdatePage(hDlg, giCurPage);
						}
					}
					else if(bBrowseWave){
						SetFocus(btnMove);

						if(gWaveIndexEnd<=31){
							gWaveIndexStart +=1;
							gWaveIndexEnd +=1;
							DrawWave();
						}
						return TRUE;	
					}
					else if(bBrowseData){
							//New-20130114
						SetFocus(btnBrowse);
						//	if(B_PRINTF) printf("dddddddddddddddgiCurEvent:%d,giIndexOfReviewData:%d\n",giCurEvent,giIndexOfReviewData);
						if(giIndexOfReviewData>giTailIndex){
							if(giCurEvent<=giHeadIndex+1){
								giCurEvent++;
							}
							giIndexOfReviewData --;    
						//得到一条报警记录
							GetOneAlarmData(giIndexOfReviewData);
							DrawWave();
							DrawParas(staParas);
							DrawStatus(hDlg);
						//	if(B_PRINTF) printf("giHeadIndex:%d,giCurEvent:%d,giIndexOfReviewData:%d\n",giHeadIndex,giCurEvent,giIndexOfReviewData);
						}
						
						/*
						//此处要考虑每页最多显示的数据量
						if(giIndexOfReviewData>MAX(giPageIndexEnd, giTailIndex)){
							//移动游标
							if(CursorIndex<ROW_COUNT-1){
								DrawCursour(hWndSelf, gTableEvent[CursorIndex][0].rect);
								CursorIndex ++;
								DrawCursour(hWndSelf, gTableEvent[CursorIndex][0].rect);
							}
						
							giIndexOfReviewData --;    
							//得到一条报警记录
							GetOneAlarmData(giIndexOfReviewData);
							DrawWave();
							DrawParas(staParas);
						}
		*/
						return TRUE;
					}
				}break;
			}
		}break;
		case MSG_CLOSE:{
			gbKeyType = KEY_TAB;
			
			hWndSelf  = (HWND)NULL;
 			staParas = (HWND)NULL;
			DestroyWavePanels();
			EndDialog(hDlg, wParam);
		}break;
	}
	
	temp = DefaultDialogProc(hDlg, message, wParam, lParam);
	return temp;
}     

//初始化区域
static int InitRects()
{
	int i;
	int iWidth;
	int iHeight;
	
	
	//初始化事件列表框架
	SetRect(&gRcEventForm, 5, 5, 210, 350);
	//初始化状态显示框架
	SetRect(&gRcStatus, 20, 200-50, 140, 230-50);
	//初始化波形回顾时间
//	SetRect(&gRcWaveTime, 90, 304-50, 550, 331-50);
	SetRect(&gRcWaveTime, X_WAVEDRAW, 304-50, WAVE_LENGHT+X_WAVEDRAW, 331-50);	
	
	//初始化事件列表区域
	iWidth = RECTW(gRcEventForm) / 3;
	iHeight = RECTH(gRcEventForm) / ROW_COUNT;
	
	for(i=0; i<ROW_COUNT; i++){
		SetRect(&(gTableEvent[i][0].rect), 
			  gRcEventForm.left, 
			  gRcEventForm.top+i*iHeight, 
			  gRcEventForm.left+iWidth*2, 
			  gRcEventForm.top+(i+1)*iHeight);
		SetRect(&(gTableEvent[i][1].rect), 
			  gRcEventForm.left+iWidth*2, 
			  gRcEventForm.top+i*iHeight, 
			  gRcEventForm.right, 
			  gRcEventForm.top+(i+1)*iHeight);
	}

	return 0;

}

//填充列表内容
//iStartIndex, 读取数据的起始索引
//iCount, 读取的数量
//TODO:装载所有的数据 
static int FillDatas(int iStartIndex)
{
	WORD wHead, wTail, wSize;
	int i;
	int iCount=0;
	
	//初始化表格内容
	for(i=0; i<ALMDATA_COUNT; i++){
		memset(gTableEvent[i][0].strValue, 0, ITEM_TXTLENGTH);
		memset(gTableEvent[i][1].strValue, 0, ITEM_TXTLENGTH);
	}
	
	//填写表头
	GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "event", gTableEvent[iCount][0].strValue, ITEM_TXTLENGTH);	
	GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "time", gTableEvent[iCount][1].strValue, ITEM_TXTLENGTH);	
	
	if(gAlmDataCount == 0){
		if(B_PRINTF) printf("%s:%d No Alarm data.\n", __FILE__, __LINE__);
		return -1;	
	} 
	//从报警存储数组里读取数据(Head-->Tail),并填写列表
	wHead = iStartIndex;
	wTail = giTailIndex;
	wSize = MAXALARMDATACOUNT;
		
// 	if(B_PRINTF) printf(" Sorted ");
	iCount+=1;
	for(i=wHead; i>=wTail; i--, iCount++){
// 		if(B_PRINTF) printf("  %d iCount is %d   ", gAlmDataArraySorted.Datas[i].bAlmID, iCount);
		//Value
		memcpy(&(gTableEvent[iCount][0].strValue), 
				strPhyAlmInfo[gAlmDataArraySorted.Datas[i].bAlmID], ITEM_TXTLENGTH);
		//Time
// 		snprintf((unsigned char *)(gTableEvent[iCount][1].strValue), ITEM_TXTLENGTH, "%.4d/%.2d/%.2d\n%.2d:%.2d:%.2d", 
		snprintf((unsigned char *)(gTableEvent[iCount][1].strValue), ITEM_TXTLENGTH, "%.2d/%.2d\n%.2d:%.2d:%.2d", 
// 				gAlmDataArraySorted.Datas[i].paras.sTime.wYear, 
				gAlmDataArraySorted.Datas[i].paras.sTime.bMonth,
				gAlmDataArraySorted.Datas[i].paras.sTime.bDay,
				gAlmDataArraySorted.Datas[i].paras.sTime.bHour,
				gAlmDataArraySorted.Datas[i].paras.sTime.bMin,
				gAlmDataArraySorted.Datas[i].paras.sTime.bSec);

	}
// 	if(B_PRINTF) printf("\n");
	
	return 0;
}

//绘制边框
static int DrawForm(HWND hWnd)
{
	HDC hdc;
	int i;
			
	if(hWnd == (HWND)NULL) return -1;
	
	hdc = GetClientDC(hWnd);
	if(hdc == (HDC)NULL) return -1;
	SetPenColor(hdc, COLOR_black);
	
	//事件框架
//  	Rectangle(hdc, gRcEventForm.left, gRcEventForm.top, gRcEventForm.right, gRcEventForm.bottom);

	//事件表格
	for(i=0; i<ROW_COUNT; i++){
		Rectangle(hdc, 
			  gTableEvent[i][0].rect.left, 
			  gTableEvent[i][0].rect.top, 
			  gTableEvent[i][0].rect.right, 
			  gTableEvent[i][0].rect.bottom);
		Rectangle(hdc, 
			  gTableEvent[i][1].rect.left, 
			  gTableEvent[i][1].rect.top, 
			  gTableEvent[i][1].rect.right, 
			  gTableEvent[i][1].rect.bottom);
	}
	
		
	ReleaseDC(hdc);
	return 0;
}

//绘制表格内容
static int DrawDatas(HWND hWnd, int iPage)
{
	HDC hdc;
	int i;
	int iPageOffset = 0;
	
	if(hWnd == (HWND)NULL) return -1;
	
	hdc = GetClientDC(hWnd);
	if(hdc == (HDC)NULL) return -1;
	SetBkColor(hdc, COLOR_lightgray);
	SetTextColor(hdc, COLOR_black);
	SelectFont(hdc, gFontSystem);
	
	//清空显示区域
	SetBrushColor(hdc, COLOR_lightgray);
	FillBox(hdc, gRcEventForm.left, gRcEventForm.top, RECTW(gRcEventForm), RECTH(gRcEventForm));
	
	//表头
	DrawText(hdc, (unsigned char *)(gTableEvent[0][0].strValue), -1, 
		 &(gTableEvent[0][0].rect), 
		 DT_NOCLIP | DT_SINGLELINE | DT_VCENTER | DT_CENTER);
	DrawText(hdc, (unsigned char *)(gTableEvent[0][1].strValue), -1, 
		 &(gTableEvent[0][1].rect), 
		 DT_NOCLIP | DT_SINGLELINE | DT_VCENTER | DT_CENTER);
	
	if(gAlmDataArray.wCount != 0) {
		//内容(根据page改变)
		iPageOffset =  (iPage-1)*EVENT_COUNT_ONEPAGE;
		for(i=1; i<ROW_COUNT; i++){
			DrawText(hdc, (unsigned char *)(gTableEvent[i+iPageOffset][0].strValue), -1, 
				 &(gTableEvent[i][0].rect), 
				 DT_NOCLIP | DT_SINGLELINE | DT_VCENTER | DT_CENTER);
			DrawText(hdc, (unsigned char *)(gTableEvent[i+iPageOffset][1].strValue), -1, 
				 &(gTableEvent[i][1].rect), 
				 DT_NOCLIP | DT_CENTER);
		}
	}
	ReleaseDC(hdc);
	return 0;
}


//绘制游标框,供浏览用
static int DrawCursour(HWND hWnd, RECT rc)
{
	HDC hdc;
	int nType;
		
	if(hWnd == (HWND)NULL) return -1;
	hdc = GetClientDC(hWnd);
	if(hdc == (HDC)NULL) return -1;
	nType = GetRasterOperation(hdc);
	SetBkMode(hdc, BM_TRANSPARENT);
	SetRasterOperation(hdc, ROP_XOR);
	SetPenColor(hdc, CURSOUR_COLOR);
	SetBrushColor(hdc, CURSOUR_COLOR);

// 	Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
	FillBox(hdc, rc.left, rc.top, RECTW(rc), RECTH(rc));
	
	SetRasterOperation(hdc, nType);
	ReleaseDC(hdc);
	return 0;
}
    
//绘制状态
static int DrawStatus(HWND hWnd)
{
	HDC hdc;
	unsigned char strStatus[50]={0};
				
	if(hWnd == (HWND)NULL) return -1;
	hdc = GetClientDC(hWnd);
	if(hdc == (HDC)NULL) return -1;
	SetBkColor(hdc, COLOR_lightgray);

	SetTextColor(hdc, COLOR_black);
	SelectFont(hdc, gFontSystem);
	SetPenColor(hdc, COLOR_red);
// 	Rectangle(hdc, gRcStatus.left, gRcStatus.top, gRcStatus.right, gRcStatus.bottom);
	
	if(gCfgSystem.bLanguage == LANGUAGE_SPANISH)
		snprintf(strStatus, sizeof strStatus, " %d / %d", giCurPage, giPageCount);
	else
		snprintf(strStatus, sizeof strStatus, " %d / %d", giCurEvent, gAlmDataCount);
	
	DrawText(hdc, strStatus, -1,&gRcStatus,  DT_NOCLIP | DT_SINGLELINE | DT_VCENTER | DT_LEFT);
	
	ReleaseDC(hdc);
	return 0;
}
    
//绘制波形时间
static int DrawWaveTime(HWND hWnd)
{
	
	HDC hdc;
	int i=0;
	unsigned char strTime[5] = {0};	
	
	if(hWnd == (HWND)NULL) return -1;
	hdc = GetClientDC(hWnd);
	if(hdc == (HDC)NULL) return -1;
	SetBkColor(hdc, COLOR_black);
	SetTextColor(hdc, COLOR_lightgray);
	SelectFont(hdc, gFontSystem);
	SetBrushColor(hdc, COLOR_black);

	//清屏
	FillBox(hdc, gRcWaveTime.left, gRcWaveTime.top, RECTW(gRcWaveTime), RECTH(gRcWaveTime));
	SetPenColor(hdc, COLOR_lightgray);
	//绘制时间低横线
	MoveTo(hdc, gRcWaveTime.left, gRcWaveTime.top+8);
	LineTo(hdc, gRcWaveTime.right, gRcWaveTime.top+8);
	//绘制时间刻度条
	for(i=0;i<(SEC_EACHSCREEN+1);i++){
		
		MoveTo(hdc, gRcWaveTime.left+RECTW(gRcWaveTime)/SEC_EACHSCREEN*i, gRcWaveTime.top+8);
		LineTo(hdc, gRcWaveTime.left+RECTW(gRcWaveTime)/SEC_EACHSCREEN*i, gRcWaveTime.top+2);
		snprintf(strTime, sizeof strTime, "%ds", gWaveIndexStart - 16+i);
		if(i<=SEC_EACHSCREEN-1)
			TextOut(hdc, gRcWaveTime.left+RECTW(gRcWaveTime)/SEC_EACHSCREEN*i, gRcWaveTime.top+10, strTime);
		else
			TextOut(hdc, gRcWaveTime.right-20, gRcWaveTime.top+10, strTime);
			//DrawText(hdc, strTime, -1, &gRcWaveTime, DT_NOCLIP | DT_SINGLELINE | DT_VCENTER | DT_RIGHT);

	}
		
	/*
	MoveTo(hdc, gRcWaveTime.left, gRcWaveTime.top+8);
	LineTo(hdc, gRcWaveTime.left, gRcWaveTime.top+2);
	snprintf(strTime, sizeof strTime, "%ds", gWaveIndexStart - 16);
	TextOut(hdc, gRcWaveTime.left, gRcWaveTime.top+10, strTime);
	
	MoveTo(hdc, gRcWaveTime.left+RECTW(gRcWaveTime)/4, gRcWaveTime.top+8);
	LineTo(hdc, gRcWaveTime.left+RECTW(gRcWaveTime)/4, gRcWaveTime.top+2);
	snprintf(strTime, sizeof strTime, "%ds", gWaveIndexStart - 16+1);
	TextOut(hdc, gRcWaveTime.left+RECTW(gRcWaveTime)/4, gRcWaveTime.top+10, strTime);
	
	MoveTo(hdc, gRcWaveTime.left+RECTW(gRcWaveTime)/2, gRcWaveTime.top+8);
	LineTo(hdc, gRcWaveTime.left+RECTW(gRcWaveTime)/2, gRcWaveTime.top+2);
	snprintf(strTime, sizeof strTime, "%ds", gWaveIndexStart - 16+2);
	TextOut(hdc, gRcWaveTime.left+RECTW(gRcWaveTime)/2, gRcWaveTime.top+10, strTime);
	
	MoveTo(hdc, gRcWaveTime.left+RECTW(gRcWaveTime)/4*3, gRcWaveTime.top+8);
	LineTo(hdc, gRcWaveTime.left+RECTW(gRcWaveTime)/4*3, gRcWaveTime.top+2);
	snprintf(strTime, sizeof strTime, "%ds", gWaveIndexStart - 16+3);
	TextOut(hdc, gRcWaveTime.left+RECTW(gRcWaveTime)/4*3, gRcWaveTime.top+10, strTime);
	
	MoveTo(hdc, gRcWaveTime.right, gRcWaveTime.top+8);
	LineTo(hdc, gRcWaveTime.right, gRcWaveTime.top+2);
	snprintf(strTime, sizeof strTime, "%ds", gWaveIndexEnd- 16);
	DrawText(hdc, strTime, -1, &gRcWaveTime, DT_NOCLIP | DT_SINGLELINE | DT_BOTTOM | DT_RIGHT);
	*/
	ReleaseDC(hdc);
	return 0;
}
     
//初始化波形绘图环境的属性(与绘图相同)
static int InitWavePanels()
{
	int iValueMax;
	int iValueMin;
	float fYAxis=0.0;	//y轴比例系数
	RECT rcPanel;		//波形面板的大小
	int i;
		
	for(i=0; i<WAVE_COUNT; i++){
	///1个面板绘制的波形数量
		wp[i].bWaveCount = 1;
		//X轴缩放系数(25mm/s)
 //		wp[i].fXAxis = 4.86;
 //		wp[i].fXAxis = 2.43;
		 wp[i].fXAxis = 3.2;
		//X轴步进值
		wp[i].iXStep = 1;
		//Y轴缩放系数(ECG 1x)
		//取得并设置波形的范围
		switch(wp[i].bID){
			case WID_ECG1:{ 
				iValueMax = WAVEMAX_ECG;	
				iValueMin = 0; 
			}break; 
			case WID_ECG2:{ 
				iValueMax = WAVEMAX_ECG;	
				iValueMin = 0; 
			}break; 
			case WID_MULTIECG:{
				iValueMax = WAVEMAX_ECG;	
				iValueMin = 0; 
			}break;
			case WID_SPO2:{
				iValueMax = WAVEMAX_SPO2;
				iValueMin = 0; 
			}break;
			case WID_RESP:{
				iValueMax = WAVEMAX_RESP;
				iValueMin = 0; 
			}break;
			case WID_CO2:{
				iValueMax = 50;
				iValueMin = 0; 
			}break;
		}
		wp[i].iYMax = iValueMax;
		wp[i].iYMin = iValueMin;
	
		//根据波形的范围和面板的实际大小，计算波形在Y轴上的缩放系数
		if(wp[i].hWnd == (HWND)NULL ) return -1;
		GetClientRect(wp[i].hWnd, &rcPanel);
				
		fYAxis = (float)((float)(iValueMax-iValueMin) / (float)((float)(rcPanel.bottom-rcPanel.top)/(float)(wp[i].bWaveCount)));	
		wp[i].fYAxis = fYAxis;
		//ECG定标
		wp[i].fSampleToPixel = (float)(((float)(gSamplesPerMV))/((float)10.0*gfNMMToPixel));
	}
	
	return 0;
}

//销毁波形绘制面板
static int DestroyWavePanels()
{
	int i;
	
	for(i=0; i<WAVE_COUNT; i++){
// 		if(wp[i].hdc != (HDC)NULL){
// 			ReleaseDC(wp[i].hdc);
// 			wp[i].hdc = (HDC)NULL;
// 		}
		if(wp[i].hWnd != (HWND)NULL){
			DestroyWindow(wp[i].hWnd);
			wp[i].hWnd = (HWND)NULL;
		}
	}
	
	return 0;
}

static int GetOneAlarmData(int iIndex)
{
	if(gAlmDataArray.wCount == 0){
		if(B_PRINTF) printf("%s:%d No Alarm data.\n", __FILE__, __LINE__);
		return -1;	
	} 
	memcpy(&gAlmDataForReview,&(gAlmDataArraySorted.Datas[iIndex]), sizeof(ALARMDATA));
	return 0;
}

extern float gfEcgSampleToPixel;
//绘制波形
static int DrawWave_Ecg(PWAVE_PANEL pPanel, int iStartIndex, int iEndIndex)
{
	int i, j;
	static int x=0, y=BASELINE_ECG;
	HDC hdc;
	RECT rcPanel;
	int iTemp;
	
	GetClientRect(pPanel->hWnd, &rcPanel);

	hdc = GetClientDC(pPanel->hWnd);
	if(hdc == (HDC)NULL){
		if(B_PRINTF) printf("HDC is NULL!\n");
	}	
	
	SetTextColor(hdc, gCfgEcg.iColor);
	SetBkColor(hdc, COLOR_black);
	
	SetBrushColor(hdc, COLOR_black);
	FillBox(hdc, rcPanel.left, rcPanel.top, RECTW(rcPanel), RECTH(rcPanel));
	
	//设定坐标系
	//缩放坐标系(Y轴不进行缩放)
	ChangeCoord(hdc, rcPanel.left, rcPanel.top, rcPanel.right, rcPanel.bottom,
		    rcPanel.left, rcPanel.top, (int)((double)rcPanel.right*(double)pPanel->fXAxis), rcPanel.bottom);
 		
	
	TextOut(hdc, 5, 5, "ECG1");
	
	SetPenColor(hdc, gCfgEcg.iColor);
	x = (int)((double)rcPanel.right*(double)pPanel->fXAxis);
	y = (4095-BASELINE_ECG )/ pPanel->fYAxis;

	MoveTo(hdc, x, y);
	//绘制4秒钟的波形
// 	for(i=iStartIndex; i<iEndIndex; i++){
	for(i=iEndIndex; i>iStartIndex; i--){
		for(j=0; j<SAMPLING_RATE; j++){
			if(gCfgEcg.bChannel1 == ECGWAVE_I){
				//Lead I
				iTemp = gAlmDataForReview.waves[i][j].bEcg1;
			}                       
			else if(gCfgEcg.bChannel1 == ECGWAVE_II){
				//Lead II
				iTemp = gAlmDataForReview.waves[i][j].bEcg2;	
			}
			else{
				if(gCfgEcg.bLeadType == ECGTYPE_3LEAD){
					iTemp = gAlmDataForReview.waves[i][j].bEcg3;	
				}
				else{
			//		Lead III = II - I
					iTemp = gAlmDataForReview.waves[i][j].bEcg2 - gAlmDataForReview.waves[i][j].bEcg1 + BASELINE_ECG;
				}
			}
			
//   			y = (WAVEMAX_ECG - iTemp)/pPanel->fYAxis;	
//			y = (WAVEMAX_ECG-iTemp)/pPanel->fSampleToPixel-BASELINE_ECG/2 / pPanel->fYAxis;

//			y = (4095-iTemp)-BASELINE_ECG)/ pPanel->fYAxis;


			y = rcPanel.top 
					+  RECTH(rcPanel) - iTemp/(gfEcgSampleToPixel) 
					+ (BASELINE_ECG/gfEcgSampleToPixel - RECTH(rcPanel)/2);
	
	
			if(y>rcPanel.bottom-1) y = rcPanel.bottom-1;
			if(y<rcPanel.top) y = rcPanel.top;
			
			LineTo(hdc, x, y);
			x-=pPanel->iXStep;
		}
	}

	x = 0;
	
	ReleaseDC(hdc);
	
	return 0;
}

static int DrawWave_SpO2(PWAVE_PANEL pPanel, int iStartIndex, int iEndIndex)
{
	int i, j;
	int x=0, y=BASELINE_SPO2;
	HDC hdc;
	RECT rcPanel;
	
	GetClientRect(pPanel->hWnd, &rcPanel);

	hdc = GetClientDC(pPanel->hWnd);
	if(hdc == (HDC)NULL){
		if(B_PRINTF) printf("HDC is NULL!\n");
	}	
	
	SetTextColor(hdc, gCfgSpO2.iColor);
	SetBkColor(hdc, COLOR_black);
	
	SetBrushColor(hdc, COLOR_black);
	FillBox(hdc, rcPanel.left, rcPanel.top, RECTW(rcPanel), RECTH(rcPanel));
	
	//设定坐标系
	//缩放坐标系(Y轴不进行缩放)
   	ChangeCoord(hdc, rcPanel.left, rcPanel.top, rcPanel.right, rcPanel.bottom,
		    rcPanel.left, rcPanel.top, (int)((double)rcPanel.right*(double)pPanel->fXAxis), rcPanel.bottom);
 		
	
	TextOut(hdc, 5, 5, "Pleth");
	
 	SetPenColor(hdc, gCfgSpO2.iColor);
	x = (int)((double)rcPanel.right*(double)pPanel->fXAxis);
   	y = BASELINE_SPO2 / pPanel->fYAxis;
	
	MoveTo(hdc, x, y);
	//绘制4秒钟的波形
// 	for(i=iStartIndex; i<iEndIndex; i++){
	for(i=iEndIndex; i>iStartIndex; i--){
		for(j=0; j<SAMPLING_RATE; j++){
// 			y = BASELINE_SPO2/ pPanel->fYAxis;
  			y = (WAVEMAX_SPO2 - gAlmDataForReview.waves[i][j].bSpO2)/pPanel->fYAxis;
			if(y>rcPanel.bottom-1) y = rcPanel.bottom-1;
			if(y<rcPanel.top) y = rcPanel.top;
			LineTo(hdc, x, y);
			x-=pPanel->iXStep;
		}
	}

	ReleaseDC(hdc);
	
	return 0;
}
		
static int DrawWave_Resp(PWAVE_PANEL pPanel, int iStartIndex, int iEndIndex)
{
	int i, j;
	int x=0, y=BASELINE_RESP;
	HDC hdc;
	RECT rcPanel;
	
	GetClientRect(pPanel->hWnd, &rcPanel);

	hdc = GetClientDC(pPanel->hWnd);
	if(hdc == (HDC)NULL){
		if(B_PRINTF) printf("HDC is NULL!\n");
	}	
	
	SetTextColor(hdc, gCfgResp.iColor);
	SetBkColor(hdc, COLOR_black);
	
	SetBrushColor(hdc, COLOR_black);
	FillBox(hdc, rcPanel.left, rcPanel.top, RECTW(rcPanel), RECTH(rcPanel));
	
	//设定坐标系
	//缩放坐标系(Y轴不进行缩放)
	ChangeCoord(hdc, rcPanel.left, rcPanel.top, rcPanel.right, rcPanel.bottom,
		    rcPanel.left, rcPanel.top, (int)((double)rcPanel.right*(double)pPanel->fXAxis), rcPanel.bottom);
 		
	
	TextOut(hdc, 5, 5, "Resp");
	
	SetPenColor(hdc, gCfgResp.iColor);
	x = (int)((double)rcPanel.right*(double)pPanel->fXAxis);
	y = BASELINE_RESP / pPanel->fYAxis;
	
	MoveTo(hdc, x, y);
	//绘制4秒钟的波形
// 	for(i=iStartIndex; i<iEndIndex; i++){
	for(i=iEndIndex; i>iStartIndex; i--){
		for(j=0; j<SAMPLING_RATE; j++){
// 			y = BASELINE_SPO2/ pPanel->fYAxis;
			y = (WAVEMAX_RESP - gAlmDataForReview.waves[i][j].bResp)/pPanel->fYAxis;
			if(y>rcPanel.bottom-1) y = rcPanel.bottom-1;
			if(y<rcPanel.top) y = rcPanel.top;
			LineTo(hdc, x, y);
			x-=pPanel->iXStep;
		}
	}

	ReleaseDC(hdc);
	
	return 0;
}

static int DrawWave_CO2(PWAVE_PANEL pPanel, int iStartIndex, int iEndIndex)
{
	int i, j;
	int x=0, y=0;
	HDC hdc;
	RECT rcPanel;
	
	GetClientRect(pPanel->hWnd, &rcPanel);

	hdc = GetClientDC(pPanel->hWnd);
	if(hdc == (HDC)NULL){
		if(B_PRINTF) printf("HDC is NULL!\n");
	}	
	
	SetTextColor(hdc, gCfgCO2.iColor);
	SetBkColor(hdc, COLOR_black);
	
	SetBrushColor(hdc, COLOR_black);
	FillBox(hdc, rcPanel.left, rcPanel.top, RECTW(rcPanel), RECTH(rcPanel));
	
	//设定坐标系
	//缩放坐标系(Y轴不进行缩放)
	ChangeCoord(hdc, rcPanel.left, rcPanel.top, rcPanel.right, rcPanel.bottom,
		    rcPanel.left, rcPanel.top, (int)((double)rcPanel.right*(double)pPanel->fXAxis), rcPanel.bottom);
 		
	
	TextOut(hdc, 5, 5, "CO2");
	
	SetPenColor(hdc, gCfgCO2.iColor);
	x = (int)((double)rcPanel.right*(double)pPanel->fXAxis);
	y = 0;
	
	MoveTo(hdc, x, y);
	//绘制4秒钟的波形
// 	for(i=iStartIndex; i<iEndIndex; i++){
	for(i=iEndIndex; i>iStartIndex; i--){
		for(j=0; j<SAMPLING_RATE; j++){
// 			y = BASELINE_SPO2/ pPanel->fYAxis;
			y = (50 - gAlmDataForReview.waves[i][j].bCO2)/pPanel->fYAxis;
			if(y>rcPanel.bottom-1) y = rcPanel.bottom-1;
			if(y<rcPanel.top) y = rcPanel.top;
			LineTo(hdc, x, y);
			x-=pPanel->iXStep;
		}
	}

	ReleaseDC(hdc);
	
	return 0;
}


static int DrawWave()
{
 	//波形存储的索引
	
  	if(gAlmDataArray.wCount == 0){
  		if(B_PRINTF) printf("%s:%d No Alarm data.\n", __FILE__, __LINE__);
  		return -1;	
  	} 
 	
// 	if(B_PRINTF) printf("Draw Wave From %d to %d.\n", gWaveIndexStart, gWaveIndexEnd);
	DrawWave_Ecg(&wp[0], gWaveIndexStart, gWaveIndexEnd);
	DrawWave_SpO2(&wp[1], gWaveIndexStart, gWaveIndexEnd);
	if(gbHaveCo2){
		DrawWave_CO2(&wp[2], gWaveIndexStart, gWaveIndexEnd);
	}
	else{
		DrawWave_Resp(&wp[2], gWaveIndexStart, gWaveIndexEnd);
	}

	DrawWaveTime(hWndSelf);
	
	return 0;
}
//绘制参数数据
static int DrawParasText(HWND hWnd)
{
	int i;
	HDC hdc;
	RECT rc;
	unsigned char strAlm[ITEM_TXTLENGTH]={0};
	unsigned char strLabel[15] = {0};
	unsigned char strValue[15] = {0};
	
	return 0;
}
//绘制参数
static int DrawParas(HWND hWnd)
{
	HDC hdc;
	RECT rc;
	int * para[4]={0};
	unsigned char strAlm[ITEM_TXTLENGTH]={0};
	unsigned char strLabel[15] = {0};
	unsigned char strValue[15] = {0};
	
	if(hWnd == (HWND)NULL) return -1;
	hdc = GetClientDC(hWnd);
	if(hdc == (HDC)NULL) return -1;
	GetClientRect(hWnd, &rc);
	SelectFont(hdc, gFontSystem);
	SetBkColor(hdc, COLOR_black);
	//清屏	
	SetBrushColor(hdc, COLOR_black);
	FillBox(hdc, rc.left, rc.top, RECTW(rc), RECTH(rc));
	
	//报警信息文字
	switch(gAlmDataForReview.bAlmClass){
		case ALARM_HIGH:{
			SetTextColor(hdc, HIGHALM_BKCOLOR);	
		}break;
		case ALARM_MID:{
			SetTextColor(hdc, MIDALM_BKCOLOR);
		}break;
		default:{
			SetTextColor(hdc, LOWALM_BKCOLOR);
		}
	}
	memset(strAlm, 0, sizeof strAlm);
	memcpy(&strAlm, strPhyAlmInfo[gAlmDataForReview.bAlmID], ITEM_TXTLENGTH);
	TextOut(hdc,150, 2, strAlm);
	//TODO:显示的参数根据报警的类型，可有所选择的显示
	//参数标签和数值
 	SetTextColor(hdc, COLOR_lightgray);	

	//时间
	memset(strAlm, 0, sizeof strAlm);
	snprintf(strAlm, ITEM_TXTLENGTH, "%.2d/%.2d %.2d:%.2d:%.2d", 
				gAlmDataForReview.paras.sTime.bMonth,
				gAlmDataForReview.paras.sTime.bDay,
				gAlmDataForReview.paras.sTime.bHour,
				gAlmDataForReview.paras.sTime.bMin,
				gAlmDataForReview.paras.sTime.bSec);
	TextOut(hdc, 1, 2, strAlm);
	
	memset(strAlm, 0, sizeof strAlm);
	snprintf(strAlm, ITEM_TXTLENGTH, "Event:%d", giCurEvent);
//	TextOut(hdc, 390, 1, strAlm);
	DrawText(hdc, strAlm, -1, &rc, DT_NOCLIP | DT_SINGLELINE | DT_TOP | DT_RIGHT);
	
	switch(gAlmDataForReview.bAlmID){
		case AP_HR_HIGH:
		case AP_HR_LOW:
		case AP_ECG_WEAKSIGNAL:			
		case AP_SPO2_HIGH:
		case AP_SPO2_LOW:
		case AP_SPO2_LOWEST:
		case AP_PR_HIGH:			
		case AP_PR_LOW:{
			//显示ECG, SpO2, Pr, Resp
			
			//ECG
			memset(strLabel, 0, sizeof strLabel);
			GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "ecg_label", strLabel, sizeof strLabel);	
			TextOut(hdc, 1, 20, strLabel);
			memset(strLabel, 0, sizeof strLabel);
			GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "hr", strLabel, sizeof strLabel);	
			TextOut(hdc, 1, 35, strLabel);
			if(gAlmDataForReview.paras.dwHaveFlag & HAVEHR){
				memset(strValue, 0, sizeof strValue);
				snprintf(strValue, sizeof strValue, "%d", gAlmDataForReview.paras.wHR);
				TextOut(hdc, 25, 35, strValue);
			}
			//SpO2
			memset(strLabel, 0, sizeof strLabel);
			GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "spo2_label", strLabel, sizeof strLabel);	
			TextOut(hdc, 60, 20, strLabel);
			memset(strLabel, 0, sizeof strLabel);
			GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "spo2", strLabel, sizeof strLabel);	
			TextOut(hdc, 60, 35, strLabel);
			if(gAlmDataForReview.paras.dwHaveFlag & HAVESPO2){
				memset(strValue, 0, sizeof strValue);
				snprintf(strValue, sizeof strValue, "%d", gAlmDataForReview.paras.bSpO2);
				TextOut(hdc, 95, 35, strValue);
			}
			else{
					if(B_PRINTF) printf("------------------------08-808798685786576576 gfu  \n");
			}
			memset(strLabel, 0, sizeof strLabel);
			GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "pr", strLabel, sizeof strLabel);	
			TextOut(hdc, 60, 50, strLabel);
			if(gAlmDataForReview.paras.dwHaveFlag & HAVEPR){
				memset(strValue, 0, sizeof strValue);
				snprintf(strValue, sizeof strValue, "%d", gAlmDataForReview.paras.wPR);
				TextOut(hdc, 85, 50, strValue);
			}
			//Resp Or CO2
			memset(strLabel, 0, sizeof strLabel);
			GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "resp_label", strLabel, sizeof strLabel);	
			TextOut(hdc, 120, 20, strLabel);
			memset(strLabel, 0, sizeof strLabel);
			GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "rr", strLabel, sizeof strLabel);	
			TextOut(hdc, 120, 35, strLabel);
			if(gAlmDataForReview.paras.dwHaveFlag & HAVERESP){
				memset(strValue, 0, sizeof strValue);
				snprintf(strValue, sizeof strValue, "%d", gAlmDataForReview.paras.wRR);
				TextOut(hdc, 145, 35, strValue);
			}
			if(gbHaveCo2){
				memset(strLabel, 0, sizeof strLabel);
				GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "co2_label", strLabel, sizeof strLabel);	
				TextOut(hdc, 180, 20, strLabel);
				memset(strLabel, 0, sizeof strLabel);
				GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "etco2", strLabel, sizeof strLabel);	
				TextOut(hdc, 180, 35, strLabel);			
				memset(strLabel, 0, sizeof strLabel);
				GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "fico2", strLabel, sizeof strLabel);	
				TextOut(hdc, 180, 50, strLabel);	
 				if(gAlmDataForReview.paras.dwHaveFlag & HAVECO2){
					memset(strValue, 0, sizeof strValue);
 					snprintf(strValue, sizeof strValue, "%d", gAlmDataForReview.paras.sCO2.wEtCO2);
					TextOut(hdc, 235, 35, strValue);
					memset(strValue, 0, sizeof strValue);
					snprintf(strValue, sizeof strValue, "%d", gAlmDataForReview.paras.sCO2.wFiCO2);
					TextOut(hdc, 235, 50, strValue);
 				}	
			}
		}break;
		case AP_RR_HIGH:
		case AP_RR_LOW:
		case AP_APNEA:
		case AP_ETCO2_HIGH:
		case AP_ETCO2_LOW:				
		case AP_FICO2_HIGH:{
			//显示Resp, CO2, ECg, SpO2 
		
			//Resp Or CO2
			
			if(!gbHaveCo2){
				memset(strLabel, 0, sizeof strLabel);
				GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "resp_label", strLabel, sizeof strLabel);	
				TextOut(hdc, 1, 20, strLabel);
				memset(strLabel, 0, sizeof strLabel);
				GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "rr", strLabel, sizeof strLabel);	
				TextOut(hdc, 1, 35, strLabel);
				if(gAlmDataForReview.paras.dwHaveFlag & HAVERESP){
					memset(strValue, 0, sizeof strValue);
					snprintf(strValue, sizeof strValue, "%d", gAlmDataForReview.paras.wRR);
					TextOut(hdc, 25, 35, strValue);
				}
			}
			else{
				memset(strLabel, 0, sizeof strLabel);
				GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "co2_label", strLabel, sizeof strLabel);	
				TextOut(hdc, 1, 20, strLabel);
				memset(strLabel, 0, sizeof strLabel);
				GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "etco2", strLabel, sizeof strLabel);	
				TextOut(hdc, 1, 35, strLabel);			
				memset(strLabel, 0, sizeof strLabel);
				GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "fico2", strLabel, sizeof strLabel);	
				TextOut(hdc, 1, 50, strLabel);	
				memset(strLabel, 0, sizeof strLabel);
				GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "rr", strLabel, sizeof strLabel);	
				TextOut(hdc, 1, 65, strLabel);	
 				if(gAlmDataForReview.paras.dwHaveFlag & HAVECO2){
					memset(strValue, 0, sizeof strValue);
					snprintf(strValue, sizeof strValue, "%d", gAlmDataForReview.paras.sCO2.wEtCO2);
					TextOut(hdc, 55, 35, strValue);
					memset(strValue, 0, sizeof strValue);
					snprintf(strValue, sizeof strValue, "%d", gAlmDataForReview.paras.sCO2.wFiCO2);
					TextOut(hdc, 55, 50, strValue);
 				}	
				if(gAlmDataForReview.paras.dwHaveFlag & HAVERESP){	
					memset(strValue, 0, sizeof strValue);
					snprintf(strValue, sizeof strValue, "%d", gAlmDataForReview.paras.wRR);
					TextOut(hdc, 55, 65, strValue);	
				}
			}
			//ECG
			memset(strLabel, 0, sizeof strLabel);
			GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "ecg_label", strLabel, sizeof strLabel);	
			TextOut(hdc, 90, 20, strLabel);
			memset(strLabel, 0, sizeof strLabel);
			GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "hr", strLabel, sizeof strLabel);	
			TextOut(hdc, 90, 35, strLabel);
			if(gAlmDataForReview.paras.dwHaveFlag & HAVEHR){
				memset(strValue, 0, sizeof strValue);
				snprintf(strValue, sizeof strValue, "%d", gAlmDataForReview.paras.wHR);
				TextOut(hdc, 120, 35, strValue);
			}
			//SpO2
			memset(strLabel, 0, sizeof strLabel);
			GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "spo2_label", strLabel, sizeof strLabel);	
			TextOut(hdc, 180, 20, strLabel);
			memset(strLabel, 0, sizeof strLabel);
			GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "spo2", strLabel, sizeof strLabel);	
			TextOut(hdc, 180, 35, strLabel);
			if(gAlmDataForReview.paras.dwHaveFlag & HAVESPO2){
				memset(strValue, 0, sizeof strValue);
				snprintf(strValue, sizeof strValue, "%d", gAlmDataForReview.paras.bSpO2);
				TextOut(hdc, 220, 35, strValue);
			}
			memset(strLabel, 0, sizeof strLabel);
			GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "pr", strLabel, sizeof strLabel);	
			TextOut(hdc, 180, 50, strLabel);
			if(gAlmDataForReview.paras.dwHaveFlag & HAVEPR){
				memset(strValue, 0, sizeof strValue);
				snprintf(strValue, sizeof strValue, "%d", gAlmDataForReview.paras.wPR);
				TextOut(hdc, 210, 50, strValue);
			}
		}break;
		case AP_T1_HIGH:
		case AP_T1_LOW:
		case AP_T2_HIGH:
		case AP_T2_LOW:		
		case AP_TD_HIGH:{
			//显示Temp, ECG， SpO2，Resp or CO2
			//Temp
			memset(strLabel, 0, sizeof strLabel);
			GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "temp_label", strLabel, sizeof strLabel);	
			TextOut(hdc, 1, 20, strLabel);
			memset(strLabel, 0, sizeof strLabel);
			GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "t1", strLabel, sizeof strLabel);	
			TextOut(hdc, 1, 35, strLabel);
			if(gAlmDataForReview.paras.dwHaveFlag & HAVET1){
				memset(strValue, 0, sizeof strValue);
				snprintf(strValue, sizeof strValue, "%3.1f", (float)(gAlmDataForReview.paras.wTemps[0])/10.0);
				TextOut(hdc, 25, 35, strValue);
			}
			memset(strLabel, 0, sizeof strLabel);
			GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "t2", strLabel, sizeof strLabel);	
			TextOut(hdc, 1, 50, strLabel);
			if(gAlmDataForReview.paras.dwHaveFlag & HAVET2){
				memset(strValue, 0, sizeof strValue);
				snprintf(strValue, sizeof strValue, "%3.1f", (float)(gAlmDataForReview.paras.wTemps[1])/10.0);
				TextOut(hdc, 25,50, strValue);
				/*
				//TD
				memset(strLabel, 0, sizeof strLabel);
				GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "td", strLabel, sizeof strLabel);	
				TextOut(hdc, 1, 65, strLabel);
				memset(strValue, 0, sizeof strValue);
				snprintf(strValue, sizeof strValue, "%3.1f", 
					 (float)(abs(gAlmDataForReview.paras.wTemps[1] -gAlmDataForReview.paras.wTemps[0]))/10.0);
				TextOut(hdc, 25,65, strValue);
				*/
			}
			
// 			//ECG
// 			memset(strLabel, 0, sizeof strLabel);
// 			GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "ecg_label", strLabel, sizeof strLabel);	
// 			TextOut(hdc, 60, 20, strLabel);
// 			memset(strLabel, 0, sizeof strLabel);
// 			GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "hr", strLabel, sizeof strLabel);	
// 			TextOut(hdc, 60, 35, strLabel);
// 			if(gAlmDataForReview.paras.dwHaveFlag & HAVEHR){
// 				memset(strValue, 0, sizeof strValue);
// 				snprintf(strValue, sizeof strValue, "%d", gAlmDataForReview.paras.wHR);
// 				TextOut(hdc, 80, 35, strValue);
// 			}
// 			//SpO2
// 			memset(strLabel, 0, sizeof strLabel);
// 		 	GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "spo2_label", strLabel, sizeof strLabel);	
// 			TextOut(hdc, 120, 20, strLabel);
// 			memset(strLabel, 0, sizeof strLabel);
// 			GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "spo2", strLabel, sizeof strLabel);	
// 			TextOut(hdc, 120, 35, strLabel);
// 			if(gAlmDataForReview.paras.dwHaveFlag & HAVESPO2){
// 				memset(strValue, 0, sizeof strValue);
// 				snprintf(strValue, sizeof strValue, "%d", gAlmDataForReview.paras.bSpO2);
// 				TextOut(hdc, 150, 35, strValue);
// 			}
// 			memset(strLabel, 0, sizeof strLabel);
// 			GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "pr", strLabel, sizeof strLabel);	
// 			TextOut(hdc, 120, 50, strLabel);
// 			if(gAlmDataForReview.paras.dwHaveFlag & HAVEPR){
// 				memset(strValue, 0, sizeof strValue);
// 				snprintf(strValue, sizeof strValue, "%d", gAlmDataForReview.paras.wPR);
// 				TextOut(hdc, 140, 50, strValue);
// 			}
// 			//Resp Or CO2
// 			memset(strLabel, 0, sizeof strLabel);
// 			GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "resp_label", strLabel, sizeof strLabel);	
// 			TextOut(hdc, 180, 20, strLabel);
// 			memset(strLabel, 0, sizeof strLabel);
// 			GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "rr", strLabel, sizeof strLabel);	
// 			TextOut(hdc, 180, 35, strLabel);
// 			if(gAlmDataForReview.paras.dwHaveFlag & HAVERESP){
// 				memset(strValue, 0, sizeof strValue);
// 				snprintf(strValue, sizeof strValue, "%d", gAlmDataForReview.paras.wRR);
// 				TextOut(hdc, 200, 35, strValue);
// 			}
			
		}break;
		case AP_NSYS_HIGH:
		case AP_NSYS_LOW:
		case AP_NDIA_HIGH:
		case AP_NDIA_LOW:
		case AP_NMEAN_HIGH:
		case AP_NMEAN_LOW:{
			//显示NIBP， ECG， SpO2，Resp or CO2
			memset(strLabel, 0, sizeof strLabel);
			GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "nibp_label", strLabel, sizeof strLabel);	
			TextOut(hdc, 10, 20, strLabel);
			if(B_PRINTF) printf("111111111111111%d/%d  (%d) =dwHaveFlag=%ld\n", 
					gAlmDataForReview.paras.sNIBP.wSys, 
					gAlmDataForReview.paras.sNIBP.wDia, 
					gAlmDataForReview.paras.sNIBP.wMean,
					gAlmDataForReview.paras.dwHaveFlag);
			if(gAlmDataForReview.paras.dwHaveFlag & HAVENIBP){
				if(B_PRINTF) printf("222222222222222%d/%d  (%d)\n", 
					gAlmDataForReview.paras.sNIBP.wSys, 
					gAlmDataForReview.paras.sNIBP.wDia, 
					gAlmDataForReview.paras.sNIBP.wMean);
				memset(strValue, 0, sizeof strValue);
				snprintf(strValue, sizeof strValue, "%d/%d  (%d)", 
					gAlmDataForReview.paras.sNIBP.wSys, 
					gAlmDataForReview.paras.sNIBP.wDia, 
					gAlmDataForReview.paras.sNIBP.wMean);
				TextOut(hdc, 10,35, strValue);	
			}
			
			//ECG
			memset(strLabel, 0, sizeof strLabel);
			GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "ecg_label", strLabel, sizeof strLabel);	
			TextOut(hdc, 100, 20, strLabel);
			memset(strLabel, 0, sizeof strLabel);
			GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "hr", strLabel, sizeof strLabel);	
			TextOut(hdc, 100, 35, strLabel);
			if(gAlmDataForReview.paras.dwHaveFlag & HAVEHR){
				memset(strValue, 0, sizeof strValue);
				snprintf(strValue, sizeof strValue, "%d", gAlmDataForReview.paras.wHR);
				TextOut(hdc, 120, 35, strValue);
			}
			//SpO2
			memset(strLabel, 0, sizeof strLabel);
			GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "spo2_label", strLabel, sizeof strLabel);	
			TextOut(hdc, 160, 20, strLabel);
			memset(strLabel, 0, sizeof strLabel);
			GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "spo2", strLabel, sizeof strLabel);	
			TextOut(hdc, 160, 35, strLabel);
			if(gAlmDataForReview.paras.dwHaveFlag & HAVESPO2){
				memset(strValue, 0, sizeof strValue);
				snprintf(strValue, sizeof strValue, "%d", gAlmDataForReview.paras.bSpO2);
				TextOut(hdc, 195, 35, strValue);
			}
			memset(strLabel, 0, sizeof strLabel);
			GetStringFromResFile(gsLanguageRes, "ALARM_REVIEW", "pr", strLabel, sizeof strLabel);	
			TextOut(hdc, 160, 50, strLabel);
			if(gAlmDataForReview.paras.dwHaveFlag & HAVEPR){
				memset(strValue, 0, sizeof strValue);
				snprintf(strValue, sizeof strValue, "%d", gAlmDataForReview.paras.wPR);
				TextOut(hdc, 180, 50, strValue);
			}
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
			//显示IBP1, IBP2, ECG, SpO2
			 
		}break;			
	
	}
	
	ReleaseDC(hdc);
	return 0;
}
     
//翻页时更新一页
static int UpdatePage(HWND hWnd, int iPage)
{
	//根据当前页足确定该页的索引范围
	giPageIndexStart = MIN((giHeadIndex-(iPage-1)*EVENT_COUNT_ONEPAGE), giHeadIndex);
// 	giPageIndexEnd = MAX((giPageIndexStart - (EVENT_COUNT_ONEPAGE-1)), 0);
	giPageIndexEnd = giPageIndexStart - (EVENT_COUNT_ONEPAGE-1);
	if(B_PRINTF) printf("%s:%d  Head is %d, Tail is %d  Page %d: From (Start)%d to (End)%d.\n", __FILE__, __LINE__, 
	       giHeadIndex, giTailIndex, iPage, giPageIndexStart, giPageIndexEnd);
	
	if(giPageIndexStart <giTailIndex) {
		return 0;
	}
	
	//更新状态
	DrawStatus(hWnd);
	
	//绘制表格内容
//	DrawDatas(hWnd, iPage);
//	DrawForm(hWnd);
	
	//光标定在每页的第一条记录
	//绘制游标的索引
//	CursorIndex = 1;
//	DrawCursour(hWnd, gTableEvent[CursorIndex][0].rect);
		
	
	giIndexOfReviewData = giPageIndexStart;
	
	//得到一条报警记录
	GetOneAlarmData(giIndexOfReviewData);
	
	//绘制波形
	DrawWave();
	//绘制参数
	DrawParas(staParas);
	
	
	return 0;
}    
     
//-------------------------------------------------- interface ----------------------------------------------------
/**
	创建报警回顾对话框
 */
void CreateAlarmReview(HWND hWnd)
{
	int High_Dlg;
if(gCfgSystem.bInterface==SCREEN_NORMAL||gCfgSystem.bInterface==SCREEN_7LEADECG
		||gCfgSystem.bInterface==SCREEN_MULTIECG||gCfgSystem.bInterface==SCREEN_SHORTTREND){
		High_Dlg=0;
	}else
		High_Dlg=160;
#if SCREEN_1024
	DlgSet.x = SCREEN_LEFT+100,
	DlgSet.y = SCREEN_TOP+180, 
#elif SCREEN_640
	DlgSet.x = SCREEN_LEFT,
	DlgSet.y = SCREEN_BOTTOM-285, 	
#else
	DlgSet.x = SCREEN_LEFT,
	DlgSet.y = SCREEN_BOTTOM-285-High_Dlg, 		
#endif
	
	DlgSet.controls = CtrlSet;

	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);    
}

/*

*/

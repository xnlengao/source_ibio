/*************************************************************************** 
 *	Module Name:	WaveReview
 *
 *	Abstract:	波形回放
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-09-01 08:51:18
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "General_Funcs.h"
#include "AlarmManage.h"
#include "Global.h"
#include "Dialog.h"
#include "DrawWave.h"

//DID_WR
#define DID_WR	DID_WAVEREVIEW 
#define DID_WR_HELP			DID_WR
#define DID_WR_OK			DID_WR+1
#define DID_WR_PREV			DID_WR+2
#define DID_WR_NEXT			DID_WR+3
#define DID_WR_PRINT		DID_WR+4
#define DID_WR_WAVE		DID_WR+5
#define DID_WR_WAVE0		DID_WR+6
#define DID_WR_WAVE0_SEL	DID_WR+7
#define DID_WR_WAVE1		DID_WR+8
#define DID_WR_WAVE1_SEL	DID_WR+9
#define DID_WR_WAVE2		DID_WR+10
#define DID_WR_WAVE2_SEL	DID_WR+11

static HWND editHelp;
static HWND btnOK;
static HWND btnPrev;
static HWND btnNext;         
static HWND btnPrint;
static HWND btnWave0Sel, cobWave0;
static HWND btnWave1Sel, cobWave1;
static HWND btnWave2Sel, cobWave2;

static WNDPROC  OldBtnProc;
static WNDPROC OldCobProc;

static DLGTEMPLATE DlgSet = {
	WS_BORDER | WS_VISIBLE | WS_CAPTION,
	WS_EX_NOCLOSEBOX,   
 #if SCREEN_1024		
   	5, 35, 590, 420,    
 #else
	5, 35, 545, 420,    
 #endif	
	"",
	0, 0,
	1,      
	NULL,
	0
};

static CTRLDATA CtrlSet[] = {
	{
		"mledit",
		WS_CHILD,
		450, 303, 80, 1,
		DID_WR_HELP,
		"",
		0
	}
};

//绘图面板的回调函数，数量由绘图面板的数量而定
static WNDPROC  OldStaProc;
static WNDPROC  OldSta0Proc;
static WNDPROC  OldSta1Proc;
static WNDPROC  OldSta2Proc;
static WNDPROC  OldSta3Proc;
//波形显示面板
static WAVE_PANEL	wp[WAVEREVIEW_COUNT];

static HWND hWndSelf = (HWND)NULL;

//波形面板显示区域
static RECT rcWaves;

//参数显示区域
static RECT rcParas;

//时间游标显示区域
static RECT rcTime;

//实时数据数组
extern CA_REALSAVEARRAY gRealDataArray;
//实时数据数组的Copy(排序后的数组 0---之前x秒)
static REALSAVEDATA gRealDataArraySorted[MAXREALSAVECOUNT];
//数组数量
static int gRealDataCount = 0;
//数据是否有效
static BOOL bHaveValidData = FALSE;

//回放波形的索引,暂定为60秒 60
static int gWaveIndexStart;	
static int gWaveIndexEnd;	
	
//一屏显示的数据量 秒
static int giTimeInter = 5;		//5S
	

//冻结和解冻波形
extern BOOL gbFreezeKey;
	       
//是否处在浏览波形的状态
static BOOL bBrowseWave = FALSE;

//ECG定标
extern float gfNMMToPixel;				//1mm=3.2pixel
extern float gSamplesPerMV;	

//拷屏打印参数
static REC_WAVEPARAM	gPrintCopyWaveParam;
static REC_COPYWAVE 	gPrintCopyWaves;


//复制并且排序当前的实时数据数组
static BOOL CopyRealData()
{
	WORD wHead, wTail, wSize;
	BOOL bLoop;
	int i;
	int iCount = 0;
	
// 	printf("%s:%d RealData Count is %d.\n", __FILE__, __LINE__, gRealDataArray.wCount);
	if(gRealDataArray.wCount ==0){
		return FALSE;
	}
	
	//From Head to Tail
	gRealDataCount = gRealDataArray.wCount;
		
	//按照由晚到早的顺序取出数据
	wHead = gRealDataArray.wHeadIndex;
	wTail = gRealDataArray.wTailIndex;
	bLoop = gRealDataArray.bLoop;
	wSize = MAXREALSAVECOUNT;
	
	memset(gRealDataArraySorted, 0, sizeof gRealDataArraySorted);
	
//  	printf("--------%s:%d Get RealData (Down)head %d tail %d : \n", __FILE__, __LINE__, wHead, wTail);
	if(wHead < wTail){
//  		printf(" Rollback \n");
		for(i=wHead; i>=0; i--){
			gRealDataArraySorted[iCount] = gRealDataArray.Datas[i];
//   			 printf("count %d:  %d:%d:%d \n", iCount,
//  			 gRealDataArraySorted[iCount].paras.sTime.bHour, 
//  			 gRealDataArraySorted[iCount].paras.sTime.bMin,
//  			 gRealDataArraySorted[iCount].paras.sTime.bSec);
			iCount ++;
		}
//  		printf("\n");
		for(i=(wSize-1); i>=wTail; i--){
			gRealDataArraySorted[iCount] = gRealDataArray.Datas[i];
//  			printf("count %d:  %d:%d:%d \n", iCount,
//  			       gRealDataArraySorted[iCount].paras.sTime.bHour, 
//  			       gRealDataArraySorted[iCount].paras.sTime.bMin,
//  			       gRealDataArraySorted[iCount].paras.sTime.bSec);
			iCount ++;
		}
	}
	else if(bLoop){
// 		printf(" Spacial \n");
		for(i=wHead; i>=wTail; i--){
			gRealDataArraySorted[iCount] = gRealDataArray.Datas[i];
// 			printf("count %d:  %d:%d:%d \n", iCount,
// 			       gRealDataArraySorted[iCount].paras.sTime.bHour, 
// 			       gRealDataArraySorted[iCount].paras.sTime.bMin,
// 			       gRealDataArraySorted[iCount].paras.sTime.bSec);
			iCount ++;
		}
	}
	else{
//  		printf(" Normal \n");
		for(i=wHead; i>wTail; i--){
			gRealDataArraySorted[iCount] = gRealDataArray.Datas[i];
//  			printf("count %d:  %d:%d:%d \n", iCount,
//  			       gRealDataArraySorted[iCount].paras.sTime.bHour, 
//  			       gRealDataArraySorted[iCount].paras.sTime.bMin,
//  			       gRealDataArraySorted[iCount].paras.sTime.bSec);
			iCount ++;
		}
	}

	
	return TRUE;
}



//初始化显示区域
static int InitRects(HWND hWnd)
{
	RECT rc;
		
	if(hWnd == (HWND)NULL) return -1;
	
	GetClientRect(hWnd, &rc);
	
#if SCREEN_1024	
 	SetRect(&rcWaves, rc.left+168, rc.top+5, rc.right-5, rc.bottom-80);
 	SetRect(&rcParas, rc.left+5, rc.top+95, rc.left+130, rc.bottom-35);
	SetRect(&rcTime, rc.left+168, rc.bottom-82, rc.right-5, rc.bottom-35);
#else	
	SetRect(&rcWaves, rc.left+136, rc.top+5, rc.right-5, rc.bottom-80);
	SetRect(&rcParas, rc.left+5, rc.top+95, rc.left+130, rc.bottom-35);
	SetRect(&rcTime, rc.left+136, rc.bottom-82, rc.right-5, rc.bottom-35);
#endif	
	return 0;
}

//绘制区域
static int DrawRects(HWND hWnd)
{
	HDC hdc = (HDC)NULL;

	if(hWnd == (HWND)NULL) return -1;
	
	hdc = GetClientDC(hWnd);
	
	if(hdc == (HDC)NULL) return -1;
	
	SetPenColor(hdc, COLOR_red);
// 	Rectangle(hdc, rcWaves.left, rcWaves.top, rcWaves.right, rcWaves.bottom);
// 	Rectangle(hdc, rcParas.left, rcParas.top, rcParas.right, rcParas.bottom);
// 	Rectangle(hdc, rcTime.left, rcTime.top, rcTime.right, rcTime.bottom);

	ReleaseDC(hdc);
	
	return 0;
}

//绘制波形时间
static int DrawWaveTime(HWND hWnd)
{
	
	HDC hdc = (HDC)NULL;
	unsigned char strWaveTime[20] = {0};	
	unsigned char strTime[5] = {0};	
	int i;
	int iStart;
	
	iStart = -(gWaveIndexStart);
	
	if(hWnd == (HWND)NULL) return -1;
	hdc = GetClientDC(hWnd);
	if(hdc == (HDC)NULL) return -1;
	SetBkColor(hdc, COLOR_black);
	SetTextColor(hdc, COLOR_lightgray);
	SelectFont(hdc, gFontSystem);
	SetBrushColor(hdc, COLOR_black);

	//清屏
	FillBox(hdc, rcTime.left, rcTime.top, RECTW(rcTime), RECTH(rcTime));
	SetPenColor(hdc, COLOR_lightgray);

	MoveTo(hdc, rcTime.left, rcTime.top+8);
	LineTo(hdc, rcTime.right, rcTime.top+8);
	
	MoveTo(hdc, rcTime.left, rcTime.top+8);
	LineTo(hdc, rcTime.left, rcTime.top+2);
	snprintf(strTime, sizeof strTime, "%ds", iStart);
// 	TextOut(hdc, rcTime.left, rcTime.top+13, strTime);
	DrawText(hdc, strTime, -1, &rcTime, DT_NOCLIP | DT_SINGLELINE | DT_VCENTER | DT_RIGHT);
	
	MoveTo(hdc, rcTime.left+RECTW(rcTime)/6*5, rcTime.top+8);
	LineTo(hdc, rcTime.left+RECTW(rcTime)/6*5, rcTime.top+2);
	snprintf(strTime, sizeof strTime, "%ds", iStart-1);
	TextOut(hdc, rcTime.left+RECTW(rcTime)/6*5-10, rcTime.top+13, strTime);	

	MoveTo(hdc, rcTime.left+RECTW(rcTime)/6*4, rcTime.top+8);
	LineTo(hdc, rcTime.left+RECTW(rcTime)/6*4, rcTime.top+2);
	snprintf(strTime, sizeof strTime, "%ds", iStart-2);
	TextOut(hdc, rcTime.left+RECTW(rcTime)/6*4-10, rcTime.top+13, strTime);
	
	MoveTo(hdc, rcTime.left+RECTW(rcTime)/6*3, rcTime.top+8);
	LineTo(hdc, rcTime.left+RECTW(rcTime)/6*3, rcTime.top+2);
	snprintf(strTime, sizeof strTime, "%ds", iStart-3);
	TextOut(hdc, rcTime.left+RECTW(rcTime)/6*3-10, rcTime.top+13, strTime);
	
	MoveTo(hdc, rcTime.left+RECTW(rcTime)/6*2, rcTime.top+8);
	LineTo(hdc, rcTime.left+RECTW(rcTime)/6*2, rcTime.top+2);
	snprintf(strTime, sizeof strTime, "%ds", iStart-4);
	TextOut(hdc, rcTime.left+RECTW(rcTime)/6*2-10, rcTime.top+13, strTime);
	
	MoveTo(hdc, rcTime.left+RECTW(rcTime)/6, rcTime.top+8);
	LineTo(hdc, rcTime.left+RECTW(rcTime)/6, rcTime.top+2);
	snprintf(strTime, sizeof strTime, "%ds", iStart-5);
	TextOut(hdc, rcTime.left+RECTW(rcTime)/6-10, rcTime.top+13, strTime);
	
	MoveTo(hdc, rcTime.left, rcTime.top+8);
	LineTo(hdc, rcTime.left, rcTime.top+2);
	snprintf(strTime, sizeof strTime, "%ds", iStart-6);
	TextOut(hdc, rcTime.left, rcTime.top+13, strTime);

	//波形回放的起始时间
	if(bHaveValidData){
		if(gCfgSystem.bLanguage != LANGUAGE_SPANISH)
			snprintf(strWaveTime, sizeof strWaveTime, "%.4d/%.2d/%.2d %.2d:%.2d:%.2d", 
				gRealDataArraySorted[0].paras.sTime.wYear, 
				gRealDataArraySorted[0].paras.sTime.bMonth, 
				gRealDataArraySorted[0].paras.sTime.bDay,
				gRealDataArraySorted[0].paras.sTime.bHour,
				gRealDataArraySorted[0].paras.sTime.bMin,
				gRealDataArraySorted[0].paras.sTime.bSec);
		else
			snprintf(strWaveTime, sizeof strWaveTime, "%.2d-%.2d-%.4d %.2d:%.2d:%.2d", 
				gRealDataArraySorted[0].paras.sTime.bDay,
				gRealDataArraySorted[0].paras.sTime.bMonth, 
				gRealDataArraySorted[0].paras.sTime.wYear, 
				gRealDataArraySorted[0].paras.sTime.bHour,
				gRealDataArraySorted[0].paras.sTime.bMin,
				gRealDataArraySorted[0].paras.sTime.bSec);
		DrawText(hdc, strWaveTime, -1, &rcTime, DT_NOCLIP | DT_SINGLELINE | DT_CENTER | DT_BOTTOM);
	}
	
	ReleaseDC(hdc);
	return 0;
}


//初始化波形绘图环境的属性(与绘图相同)
static int InitWavePanel(PWAVE_PANEL pPanel)
{
	int iValueMax;
	int iValueMin;
	float fYAxis=0.0;	//y轴比例系数
	RECT rcPanel;		//波形面板的大小
	
	///1个面板绘制的波形数量
	pPanel->bWaveCount = 1;
	//X轴缩放系数(25mm/s)
  	pPanel->fXAxis = 2.43;
// 	pPanel->fXAxis = 4.86;
	//X轴步进值
	pPanel->iXStep = 1;
	//Y轴缩放系数(ECG 1x)
	//取得并设置波形的范围
	switch(pPanel->bID){
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
		case WID_IBP1:{
			iValueMax = WAVEMAX_IBP;
			iValueMin = 0; 
		}break;
		case WID_IBP2:{
			iValueMax = WAVEMAX_IBP;
			iValueMin = 0; 
		}break;
	}
	pPanel->iYMax = iValueMax;
	pPanel->iYMin = iValueMin;
	
	//根据波形的范围和面板的实际大小，计算波形在Y轴上的缩放系数
	if(pPanel->hWnd == (HWND)NULL ) return -1;
	GetClientRect(pPanel->hWnd, &rcPanel);
				
	fYAxis = (float)((float)(iValueMax-iValueMin) / (float)((float)(rcPanel.bottom-rcPanel.top)/(float)(pPanel->bWaveCount)));	

	pPanel->fYAxis = fYAxis;

	pPanel->fSampleToPixel = (float)(((float)(gSamplesPerMV))/((float)10.0*gfNMMToPixel));
	
	return 0;
}

static int InitWavePanels()
{
	int i;
	
	for(i=0; i<WAVEREVIEW_COUNT; i++){
		InitWavePanel(&wp[i]);
	}
	
	return 0;
}

static unsigned char *strEcgLeadName[]={"I", "II", "III", "aVR", "aVL", "aVF", "V"};

//绘制波形
static int DrawWave_Ecg1(PWAVE_PANEL pPanel, int iStartIndex, int iEndIndex)
{
	
	int i, j;
	int x=0, y=BASELINE_ECG;
	HDC hdc = (HDC)NULL;
	RECT rcPanel;
	int iTemp;
	
	GetClientRect(pPanel->hWnd, &rcPanel);

	hdc = GetClientDC(pPanel->hWnd);
	if(hdc == (HDC)NULL){
		printf("HDC is NULL!\n");
		return -1;
	}	
	
	SetTextColor(hdc, gCfgEcg.iColor);
	SetBkColor(hdc, COLOR_black);

	SetBrushColor(hdc, COLOR_black);
	FillBox(hdc, rcPanel.left, rcPanel.top, RECTW(rcPanel), RECTH(rcPanel));
	
	//设定坐标系
	//缩放坐标系(Y轴不进行缩放)
    	ChangeCoord(hdc, rcPanel.left, rcPanel.top, rcPanel.right, rcPanel.bottom,
    		    rcPanel.left, rcPanel.top, (int)((double)rcPanel.right*(double)pPanel->fXAxis), rcPanel.bottom);
	
// 	TextOut(hdc, 5, 5, "ECG1");
	TextOut(hdc, 5, 5, strEcgLeadName[gCfgEcg.bChannel1]);
	
	SetPenColor(hdc, gCfgEcg.iColor);
  	x = (int)((double)rcPanel.right*(double)pPanel->fXAxis);
	y = BASELINE_ECG / pPanel->fYAxis;
	
//  	MoveTo(hdc, 0, y);
//  	LineTo(hdc, 390, y);
	
	MoveTo(hdc, x, y);
	for(i=iStartIndex; i<iEndIndex; i++){
		for(j=0; j<SAMPLING_RATE; j++){
			if(gCfgEcg.bChannel1 == ECGWAVE_I){
				//Lead I
				iTemp = gRealDataArraySorted[i].waves[j].bEcg1;
			}                       
			else if(gCfgEcg.bChannel1 == ECGWAVE_II){
				//Lead II
				iTemp = gRealDataArraySorted[i].waves[j].bEcg2;	
			}
			else{
				if(gCfgEcg.bLeadType == ECGTYPE_3LEAD){
					iTemp = gRealDataArraySorted[i].waves[j].bEcg3;	
				}
				else{
			//		Lead III = II - I
					iTemp = gRealDataArraySorted[i].waves[j].bEcg2 - gRealDataArraySorted[i].waves[j].bEcg1 + 128;
				}
			}
			
//   			y = (WAVEMAX_ECG - iTemp)/pPanel->fYAxis;	
			y = (WAVEMAX_ECG-iTemp)/pPanel->fSampleToPixel;

			if(y>rcPanel.bottom-1) y = rcPanel.bottom-1;
			if(y<rcPanel.top) y = rcPanel.top;
			LineTo(hdc, x, y);
 			x-=pPanel->iXStep;
		}
	}


	ReleaseDC(hdc);
	
	return 0;
}


static int DrawWave_Ecg2(PWAVE_PANEL pPanel, int iStartIndex, int iEndIndex)
{
	int i, j;
	int x=0, y=BASELINE_ECG;
	HDC hdc = (HDC)NULL;
	RECT rcPanel;
	int iTemp;
	
	GetClientRect(pPanel->hWnd, &rcPanel);

	hdc = GetClientDC(pPanel->hWnd);
	if(hdc == (HDC)NULL){
		printf("HDC is NULL!\n");
		return -1;
	}	
	
	SetTextColor(hdc, gCfgEcg.iColor);
	SetBkColor(hdc, COLOR_black);
	
	SetBrushColor(hdc, COLOR_black);
	FillBox(hdc, rcPanel.left, rcPanel.top, RECTW(rcPanel), RECTH(rcPanel));
	
	//设定坐标系
	//缩放坐标系(Y轴不进行缩放)
	ChangeCoord(hdc, rcPanel.left, rcPanel.top, rcPanel.right, rcPanel.bottom,
		    rcPanel.left, rcPanel.top, (int)((double)rcPanel.right*(double)pPanel->fXAxis), rcPanel.bottom);
	
// 	TextOut(hdc, 5, 5, "ECG2");
	TextOut(hdc, 5, 5, strEcgLeadName[gCfgEcg.bChannel2]);
	
	//三导联时不进行回放
	if(gCfgEcg.bLeadType == ECGTYPE_3LEAD){
		ReleaseDC(hdc);
		return -1;	
	}

	SetPenColor(hdc, gCfgEcg.iColor);
	x = (int)((double)rcPanel.right*(double)pPanel->fXAxis);
	y = BASELINE_ECG / pPanel->fYAxis;
	
// 	MoveTo(hdc, x, y);
// 	LineTo(hdc, x+100, y);
	
	MoveTo(hdc, x, y);
	for(i=iStartIndex; i<iEndIndex; i++){
		for(j=0; j<SAMPLING_RATE; j++){
			if(gCfgEcg.bChannel2 == ECGWAVE_I){
				//Lead I
				iTemp = gRealDataArraySorted[i].waves[j].bEcg1;
			}                       
			else if(gCfgEcg.bChannel2 == ECGWAVE_II){
				//Lead II
				iTemp = gRealDataArraySorted[i].waves[j].bEcg2;	
			}
			else if(gCfgEcg.bChannel2 == ECGWAVE_V){
				//Lead II
				iTemp = gRealDataArraySorted[i].waves[j].bEcg3;	
			}
			else{
				switch(gCfgEcg.bChannel2){
					case ECGWAVE_III:{
						//Lead III = II - I
						iTemp = gRealDataArraySorted[i].waves[j].bEcg2 - gRealDataArraySorted[i].waves[j].bEcg1 + 128;
					}break;
					case ECGWAVE_AVR:{
						//aVR = -(I+II)/2
						iTemp = 256 - (gRealDataArraySorted[i].waves[j].bEcg1 + gRealDataArraySorted[i].waves[j].bEcg2)/2;
					}break;
					case ECGWAVE_AVL:{
						//aVL=(I-III)/2
						iTemp = gRealDataArraySorted[i].waves[j].bEcg2 - gRealDataArraySorted[i].waves[j].bEcg1 + 128;
						iTemp = (gRealDataArraySorted[i].waves[j].bEcg1 -iTemp)/2 + 128;;
					}break;
					case ECGWAVE_AVF:{
						//aVF=(II+III)/2
						iTemp = gRealDataArraySorted[i].waves[j].bEcg2 - gRealDataArraySorted[i].waves[j].bEcg1 + 128;
						iTemp = (gRealDataArraySorted[i].waves[j].bEcg1 + iTemp)/2;
					}break;
				}
						
			}
			
// 			y = (WAVEMAX_ECG - iTemp)/pPanel->fYAxis;	
			y = (WAVEMAX_ECG-iTemp)/pPanel->fSampleToPixel;
			if(y>rcPanel.bottom-1) y = rcPanel.bottom-1;
			if(y<rcPanel.top) y = rcPanel.top;
			LineTo(hdc, x, y);
			x-=pPanel->iXStep;
		}
	}

	ReleaseDC(hdc);
	
	return 0;
}

static int DrawWave_SpO2(PWAVE_PANEL pPanel, int iStartIndex, int iEndIndex)
{
	int i, j;
	int x=0, y=BASELINE_SPO2;
	HDC hdc = (HDC)NULL;
	RECT rcPanel;
	
	GetClientRect(pPanel->hWnd, &rcPanel);

	hdc = GetClientDC(pPanel->hWnd);
	if(hdc == (HDC)NULL){
		printf("HDC is NULL!\n");
		return -1;
	}	
	
	SetTextColor(hdc, gCfgSpO2.iColor);
	SetBkColor(hdc, COLOR_black);
	
	SetBrushColor(hdc, COLOR_black);
	FillBox(hdc, rcPanel.left, rcPanel.top, RECTW(rcPanel), RECTH(rcPanel));
	
	//设定坐标系
	//缩放坐标系(Y轴不进行缩放)
	ChangeCoord(hdc, rcPanel.left, rcPanel.top, rcPanel.right, rcPanel.bottom,
		    rcPanel.left, rcPanel.top, (int)((double)rcPanel.right*(double)pPanel->fXAxis), rcPanel.bottom);
	
	if(gCfgSystem.bLanguage == LANGUAGE_SPANISH)
		TextOut(hdc, 5, 5, "Pletis");
	else
		TextOut(hdc, 5, 5, "Pleth");
	
	SetPenColor(hdc, gCfgSpO2.iColor);
	x = (int)((double)rcPanel.right*(double)pPanel->fXAxis);
	y = BASELINE_SPO2 / pPanel->fYAxis;
	
	MoveTo(hdc, x, y);
	//绘制4秒钟的波形
	for(i=iStartIndex; i<iEndIndex; i++){
		for(j=0; j<SAMPLING_RATE; j++){
// 			y = BASELINE_SPO2/ pPanel->fYAxis;
 			y = (WAVEMAX_SPO2 - gRealDataArraySorted[i].waves[j].bSpO2)/pPanel->fYAxis;
			
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
	HDC hdc = (HDC)NULL;
	RECT rcPanel;
	
	GetClientRect(pPanel->hWnd, &rcPanel);

	hdc = GetClientDC(pPanel->hWnd);
	if(hdc == (HDC)NULL){
		printf("HDC is NULL!\n");
		return -1;
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
	for(i=iStartIndex; i<iEndIndex; i++){
		for(j=0; j<SAMPLING_RATE; j++){
			y = (WAVEMAX_RESP - gRealDataArraySorted[i].waves[j].bResp)/pPanel->fYAxis;
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
	HDC hdc = (HDC)NULL;
	RECT rcPanel;
	
	GetClientRect(pPanel->hWnd, &rcPanel);

	hdc = GetClientDC(pPanel->hWnd);
	if(hdc == (HDC)NULL){
		printf("HDC is NULL!\n");
		return -1;
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
// 	y = BASELINE_RESP / pPanel->fYAxis;
	y = 0;
		
	MoveTo(hdc, x, y);
	//绘制4秒钟的波形
	for(i=iStartIndex; i<iEndIndex; i++){
		for(j=0; j<SAMPLING_RATE; j++){
			y = (50 - gRealDataArraySorted[i].waves[j].bCO2)/pPanel->fYAxis;
			if(y>rcPanel.bottom-1) y = rcPanel.bottom-1;
			if(y<rcPanel.top) y = rcPanel.top;
			LineTo(hdc, x, y);
			x-=pPanel->iXStep;
		}
	}

	ReleaseDC(hdc);
	
	return 0;
}

static int DrawWave_Ibp2(PWAVE_PANEL pPanel, int iStartIndex, int iEndIndex)
{
	int i, j;
	int x=0, y=0;
	HDC hdc = (HDC)NULL;
	RECT rcPanel;
	
	GetClientRect(pPanel->hWnd, &rcPanel);

	hdc = GetClientDC(pPanel->hWnd);
	if(hdc == (HDC)NULL){
		printf("HDC is NULL!\n");
		return -1;
	}	
	
	SetTextColor(hdc, gCfgIbp2.iColor);
	SetBkColor(hdc, COLOR_black);
	
	SetBrushColor(hdc, COLOR_black);
	FillBox(hdc, rcPanel.left, rcPanel.top, RECTW(rcPanel), RECTH(rcPanel));
	
	//设定坐标系
	//缩放坐标系(Y轴不进行缩放)
	ChangeCoord(hdc, rcPanel.left, rcPanel.top, rcPanel.right, rcPanel.bottom,
		    rcPanel.left, rcPanel.top, (int)((double)rcPanel.right*(double)pPanel->fXAxis), rcPanel.bottom);
	
	TextOut(hdc, 5, 5, "IBP2");
	
	SetPenColor(hdc, gCfgIbp2.iColor);
	x = (int)((double)rcPanel.right*(double)pPanel->fXAxis);
// 	y = BASELINE_SPO2 / pPanel->fYAxis;
	y = 0;
	
	MoveTo(hdc, x, y);

			
	//绘制4秒钟的波形
	for(i=iStartIndex; i<iEndIndex; i++){
		for(j=0; j<SAMPLING_RATE; j++){
// 			y = BASELINE_SPO2/ pPanel->fYAxis;
			y = (WAVEMAX_IBP - gRealDataArraySorted[i].waves[j].iIbp2)/pPanel->fYAxis;
						
			if(y>rcPanel.bottom-1) y = rcPanel.bottom-1;
			if(y<rcPanel.top) y = rcPanel.top;
			
			LineTo(hdc, x, y);
			x-=pPanel->iXStep;
		}
	}

	
	ReleaseDC(hdc);

	return 0;
}

static int DrawWave_Ibp1(PWAVE_PANEL pPanel, int iStartIndex, int iEndIndex)
{
	int i, j;
	int x=0, y=0;
	HDC hdc = (HDC)NULL;
	RECT rcPanel;
	
	GetClientRect(pPanel->hWnd, &rcPanel);

	hdc = GetClientDC(pPanel->hWnd);
	if(hdc == (HDC)NULL){
		printf("HDC is NULL!\n");
		return -1;
	}	
	
	SetTextColor(hdc, gCfgIbp1.iColor);
	SetBkColor(hdc, COLOR_black);
	
	SetBrushColor(hdc, COLOR_black);
	FillBox(hdc, rcPanel.left, rcPanel.top, RECTW(rcPanel), RECTH(rcPanel));
	
	//设定坐标系
	//缩放坐标系(Y轴不进行缩放)
	ChangeCoord(hdc, rcPanel.left, rcPanel.top, rcPanel.right, rcPanel.bottom,
		    rcPanel.left, rcPanel.top, (int)((double)rcPanel.right*(double)pPanel->fXAxis), rcPanel.bottom);
	
	TextOut(hdc, 5, 5, "IBP1");
	
	SetPenColor(hdc, gCfgIbp1.iColor);
	x = (int)((double)rcPanel.right*(double)pPanel->fXAxis);
// 	y = BASELINE_SPO2 / pPanel->fYAxis;
	y = 0;
	
	MoveTo(hdc, x, y);

			
	//绘制4秒钟的波形
	for(i=iStartIndex; i<iEndIndex; i++){
		for(j=0; j<SAMPLING_RATE; j++){
// 			y = BASELINE_SPO2/ pPanel->fYAxis;
			y = (WAVEMAX_IBP - gRealDataArraySorted[i].waves[j].iIbp1)/pPanel->fYAxis;
						
			if(y>rcPanel.bottom-1) y = rcPanel.bottom-1;
			if(y<rcPanel.top) y = rcPanel.top;
			
			LineTo(hdc, x, y);
			x-=pPanel->iXStep;
		}
	}

	
	ReleaseDC(hdc);

	return 0;
}

static int DrawWave(PWAVE_PANEL pPanel)
{
 	//波形存储的索引
	
	if(gRealDataCount == 0){
		printf("%s:%d No data.\n", __FILE__, __LINE__);
		return -1;	
	} 
	
	switch(pPanel->bID){
		case WID_ECG1:{
			DrawWave_Ecg1(pPanel, gWaveIndexStart, gWaveIndexEnd);	
		}break;
		case WID_ECG2:{
			DrawWave_Ecg2(pPanel, gWaveIndexStart, gWaveIndexEnd);	
		}break;
		case WID_SPO2:{
			DrawWave_SpO2(pPanel, gWaveIndexStart, gWaveIndexEnd);	
		}break;
		case WID_RESP:{
			DrawWave_Resp(pPanel, gWaveIndexStart, gWaveIndexEnd);	
		}break;
		case WID_CO2:{
			DrawWave_CO2(pPanel, gWaveIndexStart, gWaveIndexEnd);	
		}break;
		case WID_IBP1:{
			DrawWave_Ibp1(pPanel, gWaveIndexStart, gWaveIndexEnd);	
		}break;
		case WID_IBP2:{
			DrawWave_Ibp2(pPanel, gWaveIndexStart, gWaveIndexEnd);	
		}break;
	}

	return 0;
}

//打印波形
static int PrintWave()
{
	int index, i, j, k;
	int res;
	int iTemp;
	//波形信息
	REC_STRING  sInfo;
	
	if(gRealDataCount == 0){
		printf("%s:%d No data.\n", __FILE__, __LINE__);
		return -1;	
	} 
	
	//检查打印机是否处在空闲状态 ,否则不响应打印任务
	if(gPrinterStatus != PRNSTATUS_IDLE){
		printf("%s:%d Printer is busy. Status : \n", __FILE__, __LINE__, gPrinterStatus);
		return -1;
	}
	
	gPrinterWorkType = PRNWORK_COPY;
	
	//设置打印参数
	//Param
	gPrintCopyWaveParam.bSpeed = GT_PRINT_SPEED_25;
	gPrintCopyWaveParam.bFreq = GT_FREQUENCE_200;
	gPrintCopyWaveParam.bGrid = 0;
	
	//波形的基线位置
	gPrintCopyWaveParam.baseline[0] = 70;
	gPrintCopyWaveParam.baseline[1] = 120;
	gPrintCopyWaveParam.baseline[2] = 20;
	
	//清空波形
	//Wave
	gPrintCopyWaves.iCount = 1;
	gPrintCopyWaves.iLength = (gWaveIndexEnd - gWaveIndexStart)*SAMPLING_RATE;		
	for(i=0; i<gPrintCopyWaves.iCount; i++){
		memset(gPrintCopyWaves.iWave[i], 0, MAX_COPYWAVE_LENGTH);
	}
		
	//设置文字信息
	sInfo.iRowCount = 3;
	//清空文字
	memset(sInfo.strText, 0, MAX_ROW_COUNT*MAX_TEXT_LENGTH);
		
	//时间信息
	snprintf(sInfo.strText[1], MAX_TEXT_LENGTH, " %.4d/%.2d/%.2d ", 
		 gRealDataArraySorted[0].paras.sTime.wYear, 
		 gRealDataArraySorted[0].paras.sTime.bMonth, 
		 gRealDataArraySorted[0].paras.sTime.bDay);
	snprintf(sInfo.strText[2], MAX_TEXT_LENGTH, " %.2d:%.2d:%.2d ", 
		 gRealDataArraySorted[0].paras.sTime.bHour,
		 gRealDataArraySorted[0].paras.sTime.bMin,
		 gRealDataArraySorted[0].paras.sTime.bSec);

// 	for(index=0; index<1;index++){
 	for(index=0; index<WAVEREVIEW_COUNT;index++){
		//填充波形
		switch(wp[index].bID){
			case WID_ECG1:{
				//打印波形标名
				snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, " ECG %s", strEcgLeadName[gCfgEcg.bChannel1]);
				res = Printer_PrintString(&sInfo);
											
				//wave
				k =gPrintCopyWaves.iLength-1;
				for(i=gWaveIndexStart; i<gWaveIndexEnd; i++){
					for(j=0; j<SAMPLING_RATE; j++){
						if(gCfgEcg.bChannel1 == ECGWAVE_I){
							//Lead I
							iTemp = gRealDataArraySorted[i].waves[j].bEcg1;
						}                       
						else if(gCfgEcg.bChannel1 == ECGWAVE_II){
							//Lead II
							iTemp = gRealDataArraySorted[i].waves[j].bEcg2;	
						}
						else{
							if(gCfgEcg.bLeadType == ECGTYPE_3LEAD){
								iTemp = gRealDataArraySorted[i].waves[j].bEcg3;	
							}
							else{
								//Lead III = II - I
								iTemp = gRealDataArraySorted[i].waves[j].bEcg2 - gRealDataArraySorted[i].waves[j].bEcg1 + 128;
							}
						}
						gPrintCopyWaves.iWave[0][k] = iTemp*1.25;
						k--;
					}
				}
				//打印波形
			//	res = RecorderStartPrintCopyWave(&gPrintCopyWaves, &gPrintCopyWaveParam);	
			}break;
			case WID_ECG2:{
				//打印波形标名
				snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, " ECG %s", strEcgLeadName[gCfgEcg.bChannel2]);
				res = Printer_PrintString(&sInfo);
				
				//wave
				k =gPrintCopyWaves.iLength-1;
				for(i=gWaveIndexStart; i<gWaveIndexEnd; i++){
					for(j=0; j<SAMPLING_RATE; j++){
						if(gCfgEcg.bChannel2 == ECGWAVE_I){
							//Lead I
							iTemp = gRealDataArraySorted[i].waves[j].bEcg1;
						}                       
						else if(gCfgEcg.bChannel2 == ECGWAVE_II){
							//Lead II
							iTemp = gRealDataArraySorted[i].waves[j].bEcg2;	
						}
						else if(gCfgEcg.bChannel2 == ECGWAVE_V){
							//Lead II
							iTemp = gRealDataArraySorted[i].waves[j].bEcg3;	
						}
						else{
							if(gCfgEcg.bLeadType == ECGTYPE_3LEAD){
								iTemp = gRealDataArraySorted[i].waves[j].bEcg3;	
							}
							else{
								switch(gCfgEcg.bChannel2){
									case ECGWAVE_III:{
										//Lead III = II - I
										iTemp = gRealDataArraySorted[i].waves[j].bEcg2 - gRealDataArraySorted[i].waves[j].bEcg1 + 128;
									}break;
									case ECGWAVE_AVR:{
										//aVR = -(I+II)/2
										iTemp = 256 - (gRealDataArraySorted[i].waves[j].bEcg1 + gRealDataArraySorted[i].waves[j].bEcg2)/2;
									}break;
									case ECGWAVE_AVL:{
										//aVL=(I-III)/2
										iTemp = gRealDataArraySorted[i].waves[j].bEcg2 - gRealDataArraySorted[i].waves[j].bEcg1 + 128;
										iTemp = (gRealDataArraySorted[i].waves[j].bEcg1 -iTemp)/2 + 128;;
									}break;
									case ECGWAVE_AVF:{
										//aVF=(II+III)/2
										iTemp = gRealDataArraySorted[i].waves[j].bEcg2 - gRealDataArraySorted[i].waves[j].bEcg1 + 128;
										iTemp = (gRealDataArraySorted[i].waves[j].bEcg1 + iTemp)/2;
									}break;
								}
							}
						}
						gPrintCopyWaves.iWave[0][k] = iTemp*1.25;
						k--;
					}
				}
				//打印波形
			//	res = RecorderStartPrintCopyWave(&gPrintCopyWaves, &gPrintCopyWaveParam);	
			}break;
			case WID_SPO2:{
				//打印波形标名
				snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, " Pleth ");
				res = Printer_PrintString(&sInfo);
												
				//wave
				k =gPrintCopyWaves.iLength-1;
				for(i=gWaveIndexStart; i<gWaveIndexEnd; i++){
					for(j=0; j<SAMPLING_RATE; j++){
						gPrintCopyWaves.iWave[0][k] = gRealDataArraySorted[i].waves[j].bSpO2*3;
						k--;
					}
				}
				//打印波形
			//	res = RecorderStartPrintCopyWave(&gPrintCopyWaves, &gPrintCopyWaveParam);				
			}break;
			case WID_RESP:{
				//打印波形标名
				snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, " Resp ");
				res = Printer_PrintString(&sInfo);
				
				//wave
				k =gPrintCopyWaves.iLength-1;
				for(i=gWaveIndexStart; i<gWaveIndexEnd; i++){
					for(j=0; j<SAMPLING_RATE; j++){
						//存储时放大了4倍
						gPrintCopyWaves.iWave[0][k] = gRealDataArraySorted[i].waves[j].bResp-50;
						k--;
					}
				}
				
				
				//打印波形
			//	res = RecorderStartPrintCopyWave(&gPrintCopyWaves, &gPrintCopyWaveParam);	
			}break;
			case WID_CO2:{
				//打印波形标名
				snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, " CO2 ");
				res = Printer_PrintString(&sInfo);
										
				//wave
				k =gPrintCopyWaves.iLength-1;
				for(i=gWaveIndexStart; i<gWaveIndexEnd; i++){
					for(j=0; j<SAMPLING_RATE; j++){
						gPrintCopyWaves.iWave[0][k] = gRealDataArraySorted[i].waves[j].bCO2*4;
						k--;
					}
				}			
				//打印波形
			//	res = RecorderStartPrintCopyWave(&gPrintCopyWaves, &gPrintCopyWaveParam);	
			}break;
			case WID_IBP1:{
				//打印波形标名
				snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, " IBP1 ");
				res = Printer_PrintString(&sInfo);

				//wave
				k =gPrintCopyWaves.iLength-1;
				for(i=gWaveIndexStart; i<gWaveIndexEnd; i++){
					for(j=0; j<SAMPLING_RATE; j++){
						gPrintCopyWaves.iWave[0][k] = gRealDataArraySorted[i].waves[j].iIbp1;
						k--;
					}
				}			

				//打印波形
			//	res = RecorderStartPrintCopyWave(&gPrintCopyWaves, &gPrintCopyWaveParam);
			}break;
			case WID_IBP2:{
				//打印波形标名
				snprintf(sInfo.strText[0], MAX_TEXT_LENGTH, " IBP2 ");
				res = Printer_PrintString(&sInfo);

				//wave
				k =gPrintCopyWaves.iLength-1;
				for(i=gWaveIndexStart; i<gWaveIndexEnd; i++){
					for(j=0; j<SAMPLING_RATE; j++){
						gPrintCopyWaves.iWave[0][k] = gRealDataArraySorted[i].waves[j].iIbp2;
						k--;
					}
				}			

				//打印波形
			//	res = RecorderStartPrintCopyWave(&gPrintCopyWaves, &gPrintCopyWaveParam);
			}break;
			default:{
			
			}break;
		}
		
		Printer_StopCurWork();	
	}
	
	//空白走纸
	Printer_MovePaper(15);
	
	gPrinterWorkType = PRNWORK_NONE;
	
	return 0;
}
		


//销毁波形绘制面板
static int DestroyWavePanels()
{
	int i;
	
	for(i=0; i<WAVEREVIEW_COUNT; i++){
		if(wp[i].hWnd != (HWND)NULL){
			DestroyWindow(wp[i].hWnd);
			wp[i].hWnd = (HWND)NULL;
		}
	}
	
	return 0;
}

//切换波形类型
static int ChangeReviewWave(PWAVE_PANEL pPanel, BYTE bWaveID)
{
	//设置波形ID
	pPanel->bID = bWaveID;
	
	//初始化绘图区域
	InitWavePanel(pPanel);
	
	//绘图
	DrawWave(pPanel);
	
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
  			if(bBrowseWave){
  				SetWindowBkColor(hWnd, BROWSE_COLOR);	
  			}
  			else{
				SetWindowBkColor(hWnd, SETFOCUS_COLOR);	
  			}
		}break;
		case MSG_KILLFOCUS:{
			SetWindowBkColor(hWnd, KILLFOCUS_COLOR);
		}break;
		case MSG_KEYUP:{
			switch(wParam){
				case SCANCODE_ENTER:{
					switch(id){
						case DID_WR_PREV:{
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
					}//end switch(id)
				}break;
			}
		}break;
	}
	
	return(temp);
}
     
/*
	列表框消息回调函数
*/
static int CobProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	int index;
	RECT rc;
	HDC hdc= (HDC)NULL;
	
	GetWindowRect(hWnd, &rc);	
	switch(message){
		case MSG_SETFOCUS:{
			hdc = GetClientDC(GetParent(hWnd));
			if(hdc != (HDC)NULL){
				SetPenColor(hdc, SETFOCUS_COLOR);
				Rectangle(hdc, rc.left-1, rc.top-1, rc.right+1, rc.bottom+1);
				Rectangle(hdc, rc.left-2, rc.top-2, rc.right+2, rc.bottom+2);
				ReleaseDC(hdc);
			}
		}break;
		case MSG_KILLFOCUS:{
			hdc = GetClientDC(GetParent(hWnd));
			if(hdc !=(HDC)NULL){
				SetPenColor(hdc, KILLFOCUS_COLOR);
				Rectangle(hdc, rc.left-1, rc.top-1, rc.right+1, rc.bottom+1);
				Rectangle(hdc, rc.left-2, rc.top-2, rc.right+2, rc.bottom+2);
				ReleaseDC(hdc);
			}
		}break;
	case MSG_COMMAND:{
		case MSG_KEYUP:{
			if(wParam==SCANCODE_ENTER||CBN_EDITCHANGE==HIWORD(wParam)){
					id = GetDlgCtrlID(hWnd);
					index = SendMessage(hWnd, CB_GETCURSEL, 0, 0);
						
					switch(id){
 						case DID_WR_WAVE0:{
							gbWaveReviewIDs[0] = index +1;
							int Count = SendMessage(hWnd, CB_GETCOUNT, 0, 0);
							if(gbWaveReviewIDs[0] == gbWaveReviewIDs[1])
								{
									gbWaveReviewIDs[0]++;
									if(gbWaveReviewIDs[0] > Count)
									{
											gbWaveReviewIDs[0]=1;
									}
									SendMessage(cobWave0, CB_SETCURSEL, gbWaveReviewIDs[0]-1, 0);
									ChangeReviewWave(&wp[0], gbWaveReviewIDs[0]);
								}
							
							if(gbWaveReviewIDs[0] == gbWaveReviewIDs[2])
								{
									gbWaveReviewIDs[0]++;
									if(gbWaveReviewIDs[0]> Count)
									{
											gbWaveReviewIDs[0]=1;
									}
									SendMessage(cobWave0, CB_SETCURSEL,gbWaveReviewIDs[0]-1, 0);
									ChangeReviewWave(&wp[0], gbWaveReviewIDs[0]);
								}
							if(gbWaveReviewIDs[0] == gbWaveReviewIDs[1])
								{
									gbWaveReviewIDs[0]++;
									if(gbWaveReviewIDs[0]> Count)
									{
											gbWaveReviewIDs[0]=1;
									}
									SendMessage(cobWave0, CB_SETCURSEL,gbWaveReviewIDs[0]-1, 0);
									ChangeReviewWave(&wp[0], gbWaveReviewIDs[0]);
								}
							
							if(gbWaveReviewIDs[0] == gbWaveReviewIDs[2])
								{
									gbWaveReviewIDs[0]++;
									if(gbWaveReviewIDs[0] > Count)
									{
											gbWaveReviewIDs[0]=1;
									}
									SendMessage(cobWave0, CB_SETCURSEL, gbWaveReviewIDs[0]-1, 0);
									ChangeReviewWave(&wp[0], gbWaveReviewIDs[0]);
								}
					//		printf("%s :%d index=%d\n",__FILE__,__LINE__,gbWaveReviewIDs[0]);
							ChangeReviewWave(&wp[0], gbWaveReviewIDs[0]);
 							SetFocus(btnWave0Sel);
 						}break;
						case DID_WR_WAVE1:{
							gbWaveReviewIDs[1] = index+1 ;
							int Count = SendMessage(hWnd, CB_GETCOUNT, 0, 0);

								if(gbWaveReviewIDs[1] == gbWaveReviewIDs[0])
								{
									gbWaveReviewIDs[1]++;
									if(gbWaveReviewIDs[1] > Count)
									{
											gbWaveReviewIDs[1]=1;
									}
									SendMessage(cobWave1, CB_SETCURSEL, gbWaveReviewIDs[1]-1, 0);
									ChangeReviewWave(&wp[1], gbWaveReviewIDs[1]);
								}
							
							if(gbWaveReviewIDs[1] == gbWaveReviewIDs[2])
								{
									gbWaveReviewIDs[1]++;
									if(gbWaveReviewIDs[1]> Count)
									{
											gbWaveReviewIDs[1]=1;
									}
									SendMessage(cobWave1, CB_SETCURSEL,gbWaveReviewIDs[1]-1, 0);
									ChangeReviewWave(&wp[1], gbWaveReviewIDs[1]);
								}
							if(gbWaveReviewIDs[1] == gbWaveReviewIDs[0])
								{
									gbWaveReviewIDs[1]++;
									if(gbWaveReviewIDs[1]> Count)
									{
											gbWaveReviewIDs[1]=1;
									}
									SendMessage(cobWave1, CB_SETCURSEL,gbWaveReviewIDs[1]-1, 0);
									ChangeReviewWave(&wp[1], gbWaveReviewIDs[1]);
								}
							
							if(gbWaveReviewIDs[1] == gbWaveReviewIDs[2])
								{
									gbWaveReviewIDs[1]++;
									if(gbWaveReviewIDs[1] > Count)
									{
											gbWaveReviewIDs[1]=1;
									}
									SendMessage(cobWave1, CB_SETCURSEL, gbWaveReviewIDs[1]-1, 0);
									ChangeReviewWave(&wp[1], gbWaveReviewIDs[1]);
								}
						//	printf("%s :%d index=%d\n",__FILE__,__LINE__,gbWaveReviewIDs[1]);
							ChangeReviewWave(&wp[1], gbWaveReviewIDs[1]);
							SetFocus(btnWave1Sel);
						}break;
						case DID_WR_WAVE2:{
							gbWaveReviewIDs[2] = index+1;
							int Count = SendMessage(hWnd, CB_GETCOUNT, 0, 0);

							if(gbWaveReviewIDs[2] == gbWaveReviewIDs[0])
								{
									gbWaveReviewIDs[2]++;
									if(gbWaveReviewIDs[2] > Count)
									{
											gbWaveReviewIDs[2]=1;
									}
									SendMessage(cobWave2, CB_SETCURSEL, gbWaveReviewIDs[2]-1, 0);
									ChangeReviewWave(&wp[2], gbWaveReviewIDs[2]);
								}
							
							if(gbWaveReviewIDs[2] == gbWaveReviewIDs[1])
								{
									gbWaveReviewIDs[2]++;
									if(gbWaveReviewIDs[2]> Count)
									{
											gbWaveReviewIDs[2]=1;
									}
									SendMessage(cobWave2, CB_SETCURSEL,gbWaveReviewIDs[2]-1, 0);
									ChangeReviewWave(&wp[2], gbWaveReviewIDs[2]);
								}
							if(gbWaveReviewIDs[2] == gbWaveReviewIDs[0])
								{
									gbWaveReviewIDs[2]++;
									if(gbWaveReviewIDs[2] > Count)
									{
											gbWaveReviewIDs[2]=1;
									}
									SendMessage(cobWave2, CB_SETCURSEL, gbWaveReviewIDs[2]-1, 0);
									ChangeReviewWave(&wp[2], gbWaveReviewIDs[2]);
								}
							
							if(gbWaveReviewIDs[2] == gbWaveReviewIDs[1])
								{
									gbWaveReviewIDs[2]++;
									if(gbWaveReviewIDs[2]> Count)
									{
											gbWaveReviewIDs[2]=1;
									}
									SendMessage(cobWave2, CB_SETCURSEL,gbWaveReviewIDs[2]-1, 0);
									ChangeReviewWave(&wp[2], gbWaveReviewIDs[2]);
								}
						//	printf("%s :%d index=%d\n",__FILE__,__LINE__,gbWaveReviewIDs[2]);
							ChangeReviewWave(&wp[2], gbWaveReviewIDs[2]);
							SetFocus(btnWave2Sel);
						}break;
					}
					gbKeyType = KEY_TAB;
				}break;
			}
			return 0;
		}break;	
	}	
	temp = (*OldCobProc)(hWnd, message, wParam, lParam);
	return(temp);
}

static int StaProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	
	id = GetDlgCtrlID(hWnd);
		
	temp = (*OldStaProc)(hWnd, message, wParam, lParam);
	
	switch(message){
		case MSG_PAINT:{
			
			return TRUE;
		}break;
	}
	
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
			DrawWave(&wp[0]);
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
			DrawWave(&wp[1]);
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
			DrawWave(&wp[2]);
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
	RECT rc;
	
	//temp = DefaultDialogProc(hDlg, message, wParam, lParam);
	
	switch(message){
		case MSG_CREATE:{
			gbKeyType = KEY_TAB;
						
			//获取实时数据
			bHaveValidData = CopyRealData();
			
			//回放波形的索引
			gWaveIndexStart = 0;					//0s
			gWaveIndexEnd = BMIN(giTimeInter, gRealDataCount);	//-5s
			
			
			hWndSelf = hDlg;

			GetClientRect(hDlg, &rc);
			
			InitRects(hWndSelf);

			bBrowseWave = FALSE;
			
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "WAVE_REVIEW", "caption", strMenu, sizeof strMenu);	
			SetWindowCaption(hDlg, strMenu);
				
			//Select Wave0-2
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "WAVE_REVIEW", "wave0", strMenu, sizeof strMenu);	
#if SCREEN_1024		
						btnWave0Sel = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
								DID_WR_WAVE0_SEL, rc.left+5, rc.top+5, 70, 26, hDlg, 0);
						OldBtnProc = SetWindowCallbackProc(btnWave0Sel, BtnProc);
			
						cobWave0 = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
								DID_WR_WAVE0, rc.left+85, rc.top+7, 75, 26, hDlg, 0);
						OldCobProc = SetWindowCallbackProc(cobWave0, CobProc);
						
#else
			
						btnWave0Sel = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
								DID_WR_WAVE0_SEL, rc.left+5, rc.top+5, 60, 26, hDlg, 0);
						OldBtnProc = SetWindowCallbackProc(btnWave0Sel, BtnProc);
			
						cobWave0 = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
								DID_WR_WAVE0, rc.left+70, rc.top+7, 65, 26, hDlg, 0);
						OldCobProc = SetWindowCallbackProc(cobWave0, CobProc);
						
#endif
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "WAVE", "ecg1", strMenu, sizeof strMenu);	
			SendMessage(cobWave0, CB_ADDSTRING, 0, (LPARAM)strMenu);
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "WAVE", "ecg2", strMenu, sizeof strMenu);	
			SendMessage(cobWave0, CB_ADDSTRING, 0, (LPARAM)strMenu);
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "WAVE", "spo2", strMenu, sizeof strMenu);	
			SendMessage(cobWave0, CB_ADDSTRING, 0, (LPARAM)strMenu);
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "WAVE", "resp", strMenu, sizeof strMenu);	
			SendMessage(cobWave0, CB_ADDSTRING, 0, (LPARAM)strMenu);
			
			if(gbHaveIbp){
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "WAVE", "ibp1", strMenu, sizeof strMenu);	
			SendMessage(cobWave0, CB_ADDSTRING, 0, (LPARAM)strMenu);
			
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "WAVE", "ibp2", strMenu, sizeof strMenu);	
			SendMessage(cobWave0, CB_ADDSTRING, 0, (LPARAM)strMenu);
			}
			
			if(gbHaveCo2){
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "WAVE", "co2", strMenu, sizeof strMenu);	
			SendMessage(cobWave0, CB_ADDSTRING, 0, (LPARAM)strMenu);
				}
 			SendMessage(cobWave0, CB_SETCURSEL, gbWaveReviewIDs[0]-1, 0);
			
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "WAVE_REVIEW", "wave1", strMenu, sizeof strMenu);	
#if SCREEN_1024	
			
			btnWave1Sel = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_WR_WAVE1_SEL, rc.left+5, rc.top+65, 70, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnWave1Sel, BtnProc);
			
			cobWave1 = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_WR_WAVE1, rc.left+85, rc.top+67, 75, 26, hDlg, 0);
			OldCobProc = SetWindowCallbackProc(cobWave1, CobProc);
#else
			btnWave1Sel = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_WR_WAVE1_SEL, rc.left+5, rc.top+65, 60, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnWave1Sel, BtnProc);
			
			cobWave1 = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_WR_WAVE1, rc.left+70, rc.top+67, 65, 26, hDlg, 0);
			OldCobProc = SetWindowCallbackProc(cobWave1, CobProc);
			
#endif

			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "WAVE", "ecg1", strMenu, sizeof strMenu);	
			SendMessage(cobWave1, CB_ADDSTRING, 0, (LPARAM)strMenu);
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "WAVE", "ecg2", strMenu, sizeof strMenu);	
			SendMessage(cobWave1, CB_ADDSTRING, 0, (LPARAM)strMenu);
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "WAVE", "spo2", strMenu, sizeof strMenu);	
			SendMessage(cobWave1, CB_ADDSTRING, 0, (LPARAM)strMenu);
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "WAVE", "resp", strMenu, sizeof strMenu);	
			SendMessage(cobWave1, CB_ADDSTRING, 0, (LPARAM)strMenu);
			if(gbHaveIbp){
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "WAVE", "ibp1", strMenu, sizeof strMenu);	
			SendMessage(cobWave1, CB_ADDSTRING, 0, (LPARAM)strMenu);
			
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "WAVE", "ibp2", strMenu, sizeof strMenu);	
			SendMessage(cobWave1, CB_ADDSTRING, 0, (LPARAM)strMenu);
			}
			
			if(gbHaveCo2){
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "WAVE", "co2", strMenu, sizeof strMenu);	
			SendMessage(cobWave1, CB_ADDSTRING, 0, (LPARAM)strMenu);
				}
			SendMessage(cobWave1, CB_SETCURSEL, gbWaveReviewIDs[1]-1, 0);
			
			
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "WAVE_REVIEW", "wave2", strMenu, sizeof strMenu);	
#if SCREEN_1024	
			
			btnWave2Sel = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_WR_WAVE2_SEL, rc.left+5, rc.top+125, 70, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnWave2Sel, BtnProc);

			cobWave2 = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_WR_WAVE2, rc.left+85, rc.top+127, 75, 26, hDlg, 0);
			OldCobProc = SetWindowCallbackProc(cobWave2, CobProc);
			
#else

			btnWave2Sel = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
				DID_WR_WAVE2_SEL, rc.left+5, rc.top+125, 60, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnWave2Sel, BtnProc);

			cobWave2 = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
				DID_WR_WAVE2, rc.left+70, rc.top+127, 65, 26, hDlg, 0);
			OldCobProc = SetWindowCallbackProc(cobWave2, CobProc);

			
#endif

			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "WAVE", "ecg1", strMenu, sizeof strMenu);	
			SendMessage(cobWave2, CB_ADDSTRING, 0, (LPARAM)strMenu);
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "WAVE", "ecg2", strMenu, sizeof strMenu);	
			SendMessage(cobWave2, CB_ADDSTRING, 0, (LPARAM)strMenu);
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "WAVE", "spo2", strMenu, sizeof strMenu);	
			SendMessage(cobWave2, CB_ADDSTRING, 0, (LPARAM)strMenu);
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "WAVE", "resp", strMenu, sizeof strMenu);	
			SendMessage(cobWave2, CB_ADDSTRING, 0, (LPARAM)strMenu);
			if(gbHaveIbp){
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "WAVE", "ibp1", strMenu, sizeof strMenu);	
			SendMessage(cobWave2, CB_ADDSTRING, 0, (LPARAM)strMenu);
			
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "WAVE", "ibp2", strMenu, sizeof strMenu);	
			SendMessage(cobWave2, CB_ADDSTRING, 0, (LPARAM)strMenu);
			}
			
			if(gbHaveCo2){
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "WAVE", "co2", strMenu, sizeof strMenu);	
			SendMessage(cobWave2, CB_ADDSTRING, 0, (LPARAM)strMenu);
				}
			SendMessage(cobWave2, CB_SETCURSEL, gbWaveReviewIDs[2]-1, 0);			
			
			//Print
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "WAVE_REVIEW", "print", strMenu, sizeof strMenu);	
			btnPrint = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_WR_PRINT, rc.right-225, rc.bottom-30, 70, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnPrint, BtnProc);
			if(gPrinterStatus == PRNSTATUS_ERR)
				EnableWindow(btnPrint, FALSE);
			
			//Previous
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "WAVE_REVIEW", "browse", strMenu, sizeof strMenu);	
			btnPrev = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					       DID_WR_PREV, rc.right-150, rc.bottom-30, 70, 26, hDlg, 0);
			
			OldBtnProc = SetWindowCallbackProc(btnPrev, BtnProc);
			
// 			//Next				
// 			memset(strMenu, 0, sizeof strMenu);
// 			GetStringFromResFile(gsLanguageRes, "WAVE_REVIEW", "next", strMenu, sizeof strMenu);	
// 			btnNext = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
// 					       DID_WR_NEXT, 390, 370, 70, 25, hDlg, 0);
// 			OldBtnProc = SetWindowCallbackProc(btnNext, BtnProc);
			
			//退出
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "GENERAL", "exit", strMenu, sizeof strMenu);	
			btnOK = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					     DID_WR_OK, rc.right-75, rc.bottom-30, 70, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnOK, BtnProc);
			SetFocus(btnOK);
			//根据波形显示区域和数量计算波形面板的位置
			int iHeight = 0;
			int i;
			
			iHeight = RECTH(rcWaves) / WAVEREVIEW_COUNT;
			
// 			printf("RECT height is %d, Wave Height is %d.Width is %d.\n", RECTH(rcWaves), iHeight, RECTW(rcWaves));
						
			//波形显示面板
			for(i=0; i<WAVEREVIEW_COUNT; i++){
				wp[i].hWnd = (HWND)NULL;
				wp[i].hWnd = CreateWindow("static", "", WS_CHILD | SS_BLACKRECT,
						DID_WR_WAVE,rcWaves.left,rcWaves.top+i*iHeight, RECTW(rcWaves), iHeight-1, hDlg, 0);	
// 				OldStaProc = SetWindowCallbackProc(wp[i].hWnd, StaProc);	
			}
			printf("%s:%d,%d\n",__FILE__,__LINE__,rcWaves.left);
			wp[0].bID = gbWaveReviewIDs[0] ;
			OldSta0Proc = SetWindowCallbackProc(wp[0].hWnd, Sta0Proc);	
			wp[1].bID = gbWaveReviewIDs[1] ;
			OldSta1Proc = SetWindowCallbackProc(wp[1].hWnd, Sta1Proc);	
			wp[2].bID = gbWaveReviewIDs[2] ;
			OldSta2Proc = SetWindowCallbackProc(wp[2].hWnd, Sta2Proc);	
												
			//初始化波形绘图环境
			InitWavePanels();
			
			for(i=0; i<WAVEREVIEW_COUNT; i++){
				ShowWindow(wp[i].hWnd, SW_SHOW);
		
			}
			
		}break;
		case MSG_PAINT:{
			DrawRects(hWndSelf);
 			DrawWaveTime(hWndSelf);
		}break;
		case MSG_COMMAND:{
			int	id   = LOWORD(wParam);

			switch(id){
				case DID_WR_WAVE0_SEL:{
					gbKeyType = KEY_UD;
					SetFocus(cobWave0);	
				}break;
				case DID_WR_WAVE1_SEL:{
					gbKeyType = KEY_UD;
					SetFocus(cobWave1);	
				}break;
				case DID_WR_WAVE2_SEL:{
					gbKeyType = KEY_UD;
					SetFocus(cobWave2);	
				}break;
				case DID_WR_PRINT:{
					PrintWave();
				}break;
				case DID_WR_OK:{
					PostMessage(hDlg, MSG_CLOSE, 0, 0L);
				}break;
			}
		}break;
		case  MSG_KEYDOWN:{
				switch(wParam){
					case SCANCODE_CURSORBLOCKLEFT:{
						printf("left key down.\n");
						return TRUE;
					}break;
					case SCANCODE_CURSORBLOCKRIGHT:{
						printf("right key down.\n");
						return TRUE;
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
					//printf("a key up.\n");
					if(bBrowseWave){
						SetFocus(btnPrev);

						if(gWaveIndexEnd<= gRealDataCount-2){
							gWaveIndexStart +=2;
							gWaveIndexEnd +=2;
							DrawWave(&wp[0]);
							DrawWave(&wp[1]);
							DrawWave(&wp[2]);
							DrawWaveTime(hWndSelf);
						}
						
						return TRUE;	                               
					}
				}break;
				//case SCANCODE_CURSORBLOCKRIGHT:{
				case SCANCODE_D:{
					//printf("d key up.\n");
					if(bBrowseWave){
						SetFocus(btnPrev);

// 						printf("Start %d, End %d, Count %d.\n", gWaveIndexStart, gWaveIndexEnd, gRealDataCount);
						
						if(gWaveIndexStart>=2){
							gWaveIndexStart -=2;
							gWaveIndexEnd -=2;
							DrawWave(&wp[0]);
							DrawWave(&wp[1]);
							DrawWave(&wp[2]);
							DrawWaveTime(hWndSelf);
						}	

						return TRUE;	
					}
				}break;
				
			}
		}break;
		case MSG_CLOSE:{
			gbKeyType = KEY_LR;
			
			hWndSelf  = (HWND)NULL;
 			DestroyWavePanels();
			//解除冻结状态
			UnfreezeWaves();
			gbFreezeKey = FALSE;
			
			//恢复波形位置
			ChangeWavePanelsLeft((PRECT)NULL);
			
			NewInitWaveDraw();
			
			//回到主界面
			ReturnMainView();
			
 			EndDialog(hDlg, wParam);
		}break;
	}
	
	temp = DefaultDialogProc(hDlg, message, wParam, lParam);
	return temp;
}     


//-------------------------------------------------- interface ----------------------------------------------------
/**
	创建对话框
 */
void CreateWaveReview(HWND hWnd)
{
#if SCREEN_1024
	DlgSet.x = SCREEN_LEFT+100,
	DlgSet.y = SCREEN_TOP+170,
#endif	
	DlgSet.controls = CtrlSet;
	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);
}

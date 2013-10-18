/*************************************************************************** 
 *	Module Name:	NibpList
 *
 *	Abstract:	NIBP列表视图
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-08-15 14:32:04
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "General_Funcs.h"
#include "DrawWave.h"

//颜色
#define NLIST_BKCOLOR		COLOR_blue
#define NLIST_TEXTCOLOR	COLOR_lightgray
#define NLIST_PENCOLOR		COLOR_lightgray

//NibpList显示面板
#define NLISTPANEL		4000
#define NLISTLABEL		4001
static HWND gPanel = (HWND)NULL;
static HWND gLabel = (HWND)NULL;
static WNDPROC  OldProcPanel;
static WNDPROC  OldProcLabel;

//面板所在的区域
static RECT rcLabel = {5, 380, 540, 400};
static RECT rcPanel= {5, 403, 540, 565};

//欲绘制的NIBP条数
#define NLIST_COUNT	7
//行数
#define NLIST_ROW_COUNT	(NLIST_COUNT+1)
//列数(时间, NSYS, NDIA, NMEAN, HR, SpO2, PR)
#define NLIST_COL_COUNT 	7
//列表显示的文字的长度
#define ITEM_TXTLENGTH		25	
//列表内容
typedef struct {
	RECT rect;
	unsigned char *strValue[ITEM_TXTLENGTH];
}TABLE_ITEM, *PTABLE_ITEM;
//事件列表(包括表头) 
static TABLE_ITEM gTableEvent[NLIST_ROW_COUNT][NLIST_COL_COUNT];

//事件框架区域
static RECT gRcForm;

//趋势数组
extern CA_TRENDARRAY gTrendDatas;

//填充列表内容
static int FillItem(int iRowIndex, const PTRENDDATA pTrendData)
{
// 	unsigned char strValue[ITEM_TXTLENGTH] = {0};
	S_TIME sTmpTime;
	int i;
	
	if(pTrendData == NULL) return -1;
	
	//0是Label
	if(iRowIndex<1) iRowIndex = 1;
	
	//Time
	sTmpTime = pTrendData->sTime;
	snprintf((unsigned char *)(gTableEvent[iRowIndex][0].strValue), ITEM_TXTLENGTH, "%.2d/%.2d %.2d:%.2d", sTmpTime.bMonth, sTmpTime.bDay, sTmpTime.bHour, sTmpTime.bMin);
	
	//NSYS
	snprintf((unsigned char *)(gTableEvent[iRowIndex][1].strValue), ITEM_TXTLENGTH, "%d", pTrendData->sNIBP.wSys);
	
	//NDIA
	snprintf((unsigned char *)(gTableEvent[iRowIndex][2].strValue), ITEM_TXTLENGTH, "%d", pTrendData->sNIBP.wDia);
	
	//NMEAN
	snprintf((unsigned char *)(gTableEvent[iRowIndex][3].strValue), ITEM_TXTLENGTH, "%d", pTrendData->sNIBP.wMean);
	
	//HR
	if(pTrendData->dwHaveFlag & HAVEHR)
		snprintf((unsigned char *)(gTableEvent[iRowIndex][4].strValue), ITEM_TXTLENGTH, "%d", pTrendData->wHR);
	else
		snprintf((unsigned char *)(gTableEvent[iRowIndex][4].strValue), ITEM_TXTLENGTH, "---");
	
	//SpO2
	if(pTrendData->dwHaveFlag & HAVESPO2)
		snprintf((unsigned char *)(gTableEvent[iRowIndex][5].strValue), ITEM_TXTLENGTH, "%d", pTrendData->bSpO2);
	else
		snprintf((unsigned char *)(gTableEvent[iRowIndex][5].strValue), ITEM_TXTLENGTH, "--");
	
	//RR
	if(pTrendData->dwHaveFlag & HAVERESP)
		snprintf((unsigned char *)(gTableEvent[iRowIndex][6].strValue), ITEM_TXTLENGTH, "%d", pTrendData->wRR);
	else
		snprintf((unsigned char *)(gTableEvent[iRowIndex][6].strValue), ITEM_TXTLENGTH, "---");
		
	
	return 0;
}

/*
	思路:用趋势数组中最近的NLIST_COUNT次NIBP记录填充
*/
static int FillDatas()
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
	GetStringFromResFile(gsLanguageRes, "GENERAL", "time", gTableEvent[0][0].strValue, ITEM_TXTLENGTH);	
	GetStringFromResFile(gsLanguageRes, "GENERAL", "nsys", gTableEvent[0][1].strValue, ITEM_TXTLENGTH);	
	GetStringFromResFile(gsLanguageRes, "GENERAL", "ndia", gTableEvent[0][2].strValue, ITEM_TXTLENGTH);	
	GetStringFromResFile(gsLanguageRes, "GENERAL", "nmean", gTableEvent[0][3].strValue, ITEM_TXTLENGTH);	
	GetStringFromResFile(gsLanguageRes, "GENERAL", "hr", gTableEvent[0][4].strValue, ITEM_TXTLENGTH);	
	GetStringFromResFile(gsLanguageRes, "GENERAL", "spo2", gTableEvent[0][5].strValue, ITEM_TXTLENGTH);	
	GetStringFromResFile(gsLanguageRes, "GENERAL", "rr", gTableEvent[0][6].strValue, ITEM_TXTLENGTH);	
		
	if(gTrendDatas.wCount == 0){
		if(B_PRINTF) printf("%s:%d No Trend data.\n", __FILE__, __LINE__);
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
					FillItem(iCount, &(gTrendDatas.Datas[i]));		
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
					FillItem(iCount, &(gTrendDatas.Datas[i]));		
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
// 					if(B_PRINTF) printf("----Found NIBP Trend index %d.\n", i);
					FillItem(iCount, &(gTrendDatas.Datas[i]));		
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
// 					if(B_PRINTF) printf("----Found NIBP Trend index %d.\n", i);
					FillItem(iCount, &(gTrendDatas.Datas[i]));		
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

//绘制标签
static int DrawLabel(HWND hWnd)
{
	HDC hdc;
	RECT rc;
	unsigned char strLabel[50]={0};
	
	if(hWnd == (HWND)NULL) return -1;
	hdc = GetClientDC(hWnd);
	if(hdc == (HDC)NULL) return -1;
	GetClientRect(hWnd, &rc);
	
	//清屏
// 	SetBrushColor(hdc, COLOR_darkblue);
	SetBrushColor(hdc, COLOR_lightgray);
	FillBox(hdc, rc.left, rc.top, RECTW(rc), RECTH(rc));
	
	//绘制标签
// 	SetBkColor(hdc, COLOR_darkblue);
// 	SetTextColor(hdc, NLIST_TEXTCOLOR);	
	SetBkColor(hdc, COLOR_lightgray);
	SetTextColor(hdc, COLOR_black);	
	SelectFont(hdc, gFontSystem);
	memset(strLabel, 0, sizeof strLabel);
	GetStringFromResFile(gsLanguageRes, "GENERAL", "nibplist", strLabel, sizeof strLabel);				
	DrawText(hdc, strLabel, -1, &rc, DT_NOCLIP | DT_SINGLELINE | DT_CENTER | DT_TOP | DT_VCENTER);
	
	ReleaseDC(hdc);
	
	return 0;
}

/*
	绘制框架
*/
static int DrawFrame(HDC hdc)
{                                                                                                     
	int i, j;
	
	if(hdc == (HDC)NULL) return -1;
	
	SetPenColor(hdc, NLIST_PENCOLOR);
	SetBkColor(hdc, NLIST_BKCOLOR);
	SetTextColor(hdc, NLIST_TEXTCOLOR);
	SelectFont(hdc, gFontSystem);	
	
	Rectangle(hdc, gRcForm.left, gRcForm.top, gRcForm.right, gRcForm.bottom);
							
	//仅绘制标签行
	for(i=0; i<1; i++){	
		MoveTo(hdc, gTableEvent[0][i].rect.left, gTableEvent[0][i].rect.bottom);
		LineTo(hdc, gRcForm.right, gTableEvent[0][i].rect.bottom);		
	}
	//列
	for(i=0; i<NLIST_COL_COUNT-1; i++){
		MoveTo(hdc, gTableEvent[0][i].rect.right, gTableEvent[0][i].rect.top);
		LineTo(hdc, gTableEvent[0][i].rect.right, gRcForm.bottom);
	}
	

// 	//事件表格
// 	for(i=0; i<NLIST_ROW_COUNT; i++){
// 		for(j=0; j<NLIST_COL_COUNT; j++){
// 			Rectangle(hdc, 
// 				  gTableEvent[i][j].rect.left, 
// 				  gTableEvent[i][j].rect.top, 
// 				  gTableEvent[i][j].rect.right, 
// 				  gTableEvent[i][j].rect.bottom);
// 		}
// 	}

	return 0;
}

//绘制数据内容
static int DrawDatas(HWND hWnd)
{
	HDC hdc;
	RECT rc;
// 	unsigned char strLabel[50]={0};
	int i, j;
	
	if(hWnd == (HWND)NULL) return -1;
	hdc = GetClientDC(hWnd);
	if(hdc == (HDC)NULL) return -1;
	GetClientRect(hWnd, &rc);
	SetBkColor(hdc, NLIST_BKCOLOR);
	SetTextColor(hdc, NLIST_TEXTCOLOR);
	SelectFont(hdc, gFontEnglish_16);
	
	//填充数据
	FillDatas();
	
	//清屏
	SetBrushColor(hdc, NLIST_BKCOLOR);
	FillBox(hdc, rc.left, rc.top, RECTW(rc), RECTH(rc));
		
	//表头
	for(i=0; i<NLIST_COL_COUNT; i++){
		DrawText(hdc, (unsigned char *)(gTableEvent[0][i].strValue), -1, 
			 &(gTableEvent[0][i].rect), 
			 DT_NOCLIP | DT_SINGLELINE | DT_VCENTER | DT_CENTER);
	}

// 	SelectFont(hdc, gFontSystem);
	//内容
	for(i=1; i<NLIST_ROW_COUNT; i++){
		for(j=0; j<NLIST_COL_COUNT; j++){
			DrawText(hdc, (unsigned char *)(gTableEvent[i][j].strValue), -1, 
				 &(gTableEvent[i][j].rect), 
				 DT_NOCLIP | DT_SINGLELINE | DT_VCENTER | DT_CENTER);
		}
	}
	
	DrawFrame(hdc);
			
	ReleaseDC(hdc);
	
	return 0;
}

/**
	趋势面板的回调函数
 */
static int ProcPanel(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;		
	HDC hdc;
                                                                                 
	temp = (*OldProcPanel)(hWnd, message, wParam, lParam);
	switch(message){
		case MSG_PAINT:{
			DrawDatas(hWnd);
		}break;
	}
		
	return(temp);
}

static int ProcLabel(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;		
	
	temp = (*OldProcLabel)(hWnd, message, wParam, lParam);
	switch(message){
		case MSG_PAINT:{
 			DrawLabel(hWnd);
		}break;
	}

	return(temp);
}

		
static int CreateNList(HWND hParentWnd)
{
	RECT rc;
	int i,j;
	int iWidth;
	int iHeight;
	
	if(hParentWnd == (HWND)NULL) return -1;
	
	//面板所在的区域
// 	static RECT rcLabel = {5, 380, 540, 400};
// 	static RECT rcPanel= {5, 403, 540, 565};
	
	if(gbHaveIbp){
		SetRect(&rcLabel, 5, VW_NIBPLIST_BOTTOM_EXT+10, 540, VW_NIBPLIST_BOTTOM_EXT+30);
		SetRect(&rcPanel, 5, VW_NIBPLIST_BOTTOM_EXT+30+2, 540, VW_NORMAL_BOTTOM_EXT+10);	
	}
	else{
		SetRect(&rcLabel, 5, 380, 540, 400);
		SetRect(&rcPanel, 5, 403, 540, 565);	
	}
	
	gPanel =  CreateWindow("static", "", WS_CHILD | SS_BLACKRECT,
			       NLISTPANEL, rcPanel.left, rcPanel.top, RECTW(rcPanel), RECTH(rcPanel), hParentWnd, 0L);   	
	OldProcPanel = SetWindowCallbackProc(gPanel, ProcPanel);
	
	gLabel =  CreateWindow("static", "", WS_CHILD | SS_BLACKRECT,
				NLISTLABEL, rcLabel.left, rcLabel.top, RECTW(rcLabel), RECTH(rcLabel), hParentWnd, 0L);   	
	OldProcLabel = SetWindowCallbackProc(gLabel, ProcLabel);
	
	//初始化列表内容结构
	GetClientRect(gPanel, &rc);
	SetRect(&gRcForm, rc.left+3, rc.top+1, rc.right-3, rc.bottom-1);
	
	//Time占两列
	iWidth = RECTW(gRcForm) / (NLIST_COL_COUNT+1);
	iHeight = RECTH(gRcForm) / NLIST_ROW_COUNT;

	
	for(i=0; i<NLIST_ROW_COUNT; i++){
		//Time
		SetRect(&(gTableEvent[i][0].rect), 
			  gRcForm.left, 
			  gRcForm.top+i*iHeight, 
			  gRcForm.left+2*iWidth, 
			  gRcForm.top+(i+1)*iHeight);
		//Other
		for(j=1; j<NLIST_COL_COUNT; j++){
			SetRect(&(gTableEvent[i][j].rect), 
				  gRcForm.left+ (j+1)*iWidth, 
				  gRcForm.top+i*iHeight, 
				  gRcForm.left+(j+2)*iWidth, 
				  gRcForm.top+(i+1)*iHeight);
		}
	}
	
	
	return 0;
}

/**
	销毁趋势面板
 */
static int DestroyPanel(HWND *phWnd)
{
	
	if(*phWnd == (HWND)NULL) return -1;
	
	DestroyWindow(*phWnd);
	*phWnd = (HWND)NULL;		
	
	return 0;	
}

//------------------------- interface ------------------------------
/**
	初始化短趋势	
	rcShortTrend : 短趋势面板区域
 */
int InitNibpList(HWND hWnd)
{
	int res;
	if(hWnd == (HWND)NULL) return -1;
				

	CreateNList(hWnd);
	//显示面板
	ShowWindow(gPanel, SW_SHOW);
	ShowWindow(gLabel, SW_SHOW);
	
	return 0;
}

/**
	销毁短趋势
 */
int DestroyNibpList()
{
	int res;
	
	//销毁趋势面板
	res = DestroyPanel(&gPanel);
	res = DestroyPanel(&gLabel);

	return res;
}

/*
	更新NIBP列表
*/
int UpdateNibpList()
{
	if(gPanel != (HWND)NULL){
		DrawDatas(gPanel);
	}	
	return 0;
}


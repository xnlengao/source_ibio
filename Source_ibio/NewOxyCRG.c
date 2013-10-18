/*************************************************************************** 
 *	Module Name:	NewOxyCRG
 *
 *	Abstract:	OxyCRG
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2008-02-13 09:04:14
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "General_Funcs.h"
#include "NewDrawWave.h"

//呼吸氧合图类型
#define CRG_NONE	0
#define CRG_HR	1
#define CRG_SPO2	2
#define CRG_RR	3	//呼吸率
#define CRG_RESP	4	//呼吸波形

//绘制线段的宽度
#define CRG_LINE_WIDTH		1

//显示范围
//HR
#define CRG_HR_MIN			HR_MIN
#define CRG_HR_MAX		HR_MAX
//SpO2
#define CRG_SPO2_MIN		80
#define CRG_SPO2_MAX		SPO2_MAX
//RR
#define CRG_RR_MIN			RR_MIN
#define CRG_RR_MAX			RR_MAX
//RESP TODO:暂定,有待进一步确认
#define CRG_RESP_MIN		0
#define CRG_RESP_MAX		64

//趋势数量, HR, SpO2, RR or Resp
#define CRG_COUNT			3

//颜色
#define CRG_BKCOLOR		COLOR_black
#define CRG_TEXTCOLOR		COLOR_darkgray
#define CRG_PENCOLOR		COLOR_darkgray

//区域定义
#define CRG_LEFT		RC_OXYCRG_LEFT
#define CRG_TOP			RC_OXYCRG_BOTTOM+20
#define CRG_RIGHT		RC_NORMAL_RIGHT
#define CRG_BOTTOM		RC_NORMAL_BOTTOM


//CRG显示区域数据结构
typedef struct {
	BYTE bID;			
	RECT rcView;		//显示区域
	int xScale;		//x值的范围(秒),即在横向要显示指定时间内的全部数据
	int yMin;			//y轴最小值，测量参数趋势的最小值
	int yMax;			//y轴最大值，测量参数趋势的最大值
	double fRatio_x;       //X轴的绘图比例
	double fRatio_x_wave;       //X轴的绘图比例
	double fRatio_y	;	//Y轴的绘图比例	
	int iColor;			//趋势图绘制颜色
}CRG_TREND, *PCRG_TREND;

//显示区域
RECT gRCCrg;
RECT gRCCrgValue;
RECT gRCCrgLabel;
RECT gRCCrgTime;
static CRG_TREND gCrgPanels[CRG_COUNT];
//趋势数量
static int giCrgCount;	

//短趋势X轴显示刻度即时间范围, 单位:秒
static int giCrg_XScale = 120;

static unsigned char strInfo[20]={0};
static unsigned char strXCale[20]={0};
static unsigned char strUnit[10]={0};

//HDC
static HDC hdc_oxyCRG = (HDC)NULL;
//MDC
static HDC mdc_oxyCRG = (HDC)NULL;

extern HWND ghWndPanel_Crg;

//绘制线程
static pthread_t ptCrg;
static sem_t semCrg;	
static BOOL gbThreadHasCreated_Crg = FALSE;
static BOOL gbDrawCrgFinished  = TRUE;


//------------------------数据处理(存储,读取)-----------------------------------
/*
	思路:以1s的呼吸波形为基准,循环存储8分钟的数据. 显示的时候显示当前时间之前指定范围内的数据
*/
//CRG数据内容
typedef struct {
	WORD wHR;
	WORD wRR;
	BYTE bSpO2;
	BYTE bResp[200];		//200 pack/sec
}CRG_DATA, *PCRG_DATA;

//最大存储Crg的数据时间,单位:秒
#define CRG_DATA_MAX_COUNT	1800
//循环数组
typedef struct {
	WORD 			wHeadIndex;					//循环数组首指针
	WORD 			wTailIndex;					//循环数组尾指针
	BOOL 			bLoop;						//数组翻转标记
	WORD 			wCount;						//实际存储的数据数量
	CRG_DATA		Datas[CRG_DATA_MAX_COUNT];
}CRG_DATAARRAY, *PCRG_DATAARRAY;

CRG_DATA    		gCrgData;	
CRG_DATAARRAY	gCrgDataArray;


/**
	添加一条数据到数组,在HeadIndex处添加最新的数据
	注意:按此法存储,未翻转前,数组索引[0]处无数据,读取数据时要注意
 */
static int CrgArray_Add(const PCRG_DATA pData)
{
	WORD wHead, wTail, wSize, wCount;
	BOOL bLoop;
	S_TIME sTime;
	if(pData == NULL) return -1;
	
	wHead = gCrgDataArray.wHeadIndex;
	wTail = gCrgDataArray.wTailIndex;
	bLoop = gCrgDataArray.bLoop;
	wSize = CRG_DATA_MAX_COUNT;

	if(wHead < (wSize-1)){
		wHead ++;
		if(bLoop){
			//如果是翻转的情况
			if(wTail < (wSize-1)){
				wTail ++;
			}
			else{
				wTail = 0;
			}
		}
	}
	else{
		//数据已经存满,翻转
		wHead = 0;
		wTail = wHead +1;
		bLoop = TRUE;
	}
	gCrgDataArray.wHeadIndex = wHead;
	gCrgDataArray.wTailIndex = wTail;
	gCrgDataArray.bLoop = bLoop;
	gCrgDataArray.Datas[gCrgDataArray.wHeadIndex] = *pData;
	//计算实际存储的数量
	if(gCrgDataArray.wHeadIndex < gCrgDataArray.wTailIndex){
		//数据已经存满,并翻转
		gCrgDataArray.wCount = CRG_DATA_MAX_COUNT;
	}
	else{
		gCrgDataArray.wCount +=1;
	}

	return wHead;
}

/**
	填充单条实时数据 
	@param:bFinish: 波形数据是否存完
 */
int NewFillOneCrgData(BOOL bFinish)
{
	static int iCount = 0;
	static int iPackCount = 0;
	int i;
	
	if(iCount < 125){
		//继续添加波形
		//Resp
		gCrgData.bResp[iCount] = gValueResp.iWave;	
		iCount ++;
	}
	else{
		iCount = 0;
		iPackCount = 0;

		//添加参数信息
		gCrgData.wHR = gValueEcg.wHR;
		gCrgData.wRR = gValueResp.wRR;
		gCrgData.bSpO2 = gValueSpO2.bSpO2;
		
		//将记录添加到数组中
		CrgArray_Add(&gCrgData);
  		
//		printf("%s:%d CrgSave. Total %d records. Head %d, Tail %d. SpO2 is %d\n", __FILE__, __LINE__, 
 // 		       gCrgDataArray.wCount, gCrgDataArray.wHeadIndex, gCrgDataArray.wTailIndex, 
//    		       gValueSpO2.bSpO2);
	}
	
	return 0;
}

//////////////////////////////////

//配置短趋势属性
static int InitCrgAttr()
{
	int iTop = 35;
	int iInter = 106;
	int i;
	
	//分配ID
	for(i = 0; i < CRG_COUNT; i++){
		gCrgPanels[0].bID = CRG_NONE;
	}
	gCrgPanels[0].bID = CRG_HR;
	gCrgPanels[1].bID = CRG_SPO2;
	gCrgPanels[2].bID = CRG_RR;
	giCrgCount = 3;

	GetClientRect(ghWndPanel_Crg, &gRCCrg);
	//区域
	SetRect(&gRCCrgValue, gRCCrg.left, gRCCrg.top+20, gRCCrg.right-1, gRCCrg.bottom);
	SetRect(&gRCCrgLabel, gRCCrg.left, gRCCrg.top, gRCCrg.right-1, gRCCrg.top+19);
	//SetRect(&gRCCrgTime, gRCCrg.left, gRCCrg.bottom-15, gRCCrg.right-1, gRCCrg.bottom-1);
	iInter = RECTH(gRCCrgValue)/giCrgCount;
	for(i = 0; i < giCrgCount; i++){
		SetRect(&(gCrgPanels[i].rcView), 
			  gRCCrgValue.left,
			  gRCCrgValue.top + i*iInter,
			  gRCCrgValue.right,
			  gRCCrgValue.top + (i+1)*iInter);
	}
	
	//根据区域和ID计算系数
	for(i = 0; i < giCrgCount; i++){
		//时间刻度统一
		gCrgPanels[i].xScale = giCrg_XScale;		
		switch(gCrgPanels[i].bID){
			case CRG_HR:{
				gCrgPanels[i].yMin = CRG_HR_MIN;
				gCrgPanels[i].yMax = CRG_HR_MAX;
				gCrgPanels[i].iColor = gCfgEcg.iColor;
			}break;
			case CRG_SPO2:{
				gCrgPanels[i].yMin = CRG_SPO2_MIN;
				gCrgPanels[i].yMax = CRG_SPO2_MAX;
				gCrgPanels[i].iColor = gCfgSpO2.iColor;
			}break;
			case CRG_RR:{
				gCrgPanels[i].yMin = CRG_RR_MIN;
				gCrgPanels[i].yMax = CRG_RR_MAX;
				gCrgPanels[i].iColor = gCfgResp.iColor;	
			}break;
			case CRG_RESP:{
				gCrgPanels[i].yMin = CRG_RESP_MIN;
				gCrgPanels[i].yMax = CRG_RESP_MAX;
				gCrgPanels[i].iColor = gCfgResp.iColor;
			}break;
		}
		
		//根据时间刻度计算时间与像素的比列
		gCrgPanels[i].fRatio_x = (double)((double)(RECTW(gCrgPanels[i].rcView))/(double)(gCrgPanels[i].xScale));
		//根据参数的限值计算数据值与像素的比例
		gCrgPanels[i].fRatio_y = (double)((double)(RECTH(gCrgPanels[i].rcView)) / (double)(gCrgPanels[i].yMax - gCrgPanels[i].yMin));
	}

	return 0;
}

static int DrawCrg_HR(HDC hdc, PCRG_TREND pPanel)
{
	SetTextColor(hdc, CRG_TEXTCOLOR);
	
	//Label
	if(gCfgSystem.bLanguage == LANGUAGE_SPANISH)
		DrawText(hdc, "FC", -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_CENTER | DT_SINGLELINE);
	else
		DrawText(hdc, "HR", -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%d", pPanel->yMin);
	DrawText(hdc, strInfo, -1, &(pPanel->rcView), DT_NOCLIP | DT_BOTTOM | DT_RIGHT | DT_SINGLELINE);
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%d", pPanel->yMax);
	DrawText(hdc, strInfo, -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_RIGHT | DT_SINGLELINE);
// 	//XCale
// 	DrawText(hdc, strXCale, -1, &(pPanel->rcView), DT_NOCLIP | DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
	
	//Grid
	DrawHDotLine(hdc, 
		     pPanel->rcView.left, 
		     pPanel->rcView.top+RECTH(pPanel->rcView)/2,
		     RECTW(pPanel->rcView));
	
	Rectangle(hdc, 
		  pPanel->rcView.left, 
		  pPanel->rcView.top, 
		  pPanel->rcView.right, 
		  pPanel->rcView.bottom);
	
	return 0;
}

static int DrawCrg_SpO2(HDC hdc, PCRG_TREND pPanel)
{
	SetTextColor(hdc, CRG_TEXTCOLOR);
	
	//Label
	DrawText(hdc, "SpO2", -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%d", pPanel->yMin);
	DrawText(hdc, strInfo, -1, &(pPanel->rcView), DT_NOCLIP | DT_BOTTOM | DT_RIGHT | DT_SINGLELINE);
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%d", pPanel->yMax);
	DrawText(hdc, strInfo, -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_RIGHT | DT_SINGLELINE);
	//XCale
// 	DrawText(hdc, strXCale, -1, &(pPanel->rcView), DT_NOCLIP | DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
	
	//Grid
	DrawHDotLine(hdc, 
		     pPanel->rcView.left, 
		     pPanel->rcView.top+RECTH(pPanel->rcView)/2,
		     RECTW(pPanel->rcView));
	
	Rectangle(hdc, 
		  pPanel->rcView.left, 
		  pPanel->rcView.top, 
		  pPanel->rcView.right, 
		  pPanel->rcView.bottom);
	
	return 0;
}

static int DrawCrg_RR(HDC hdc, PCRG_TREND pPanel)
{
	SetTextColor(hdc, CRG_TEXTCOLOR);
	
	//Label
	if(gCfgSystem.bLanguage == LANGUAGE_SPANISH)
		DrawText(hdc, "FR", -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);
	else
		DrawText(hdc, "RR", -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%d", pPanel->yMin);
	DrawText(hdc, strInfo, -1, &(pPanel->rcView), DT_NOCLIP | DT_BOTTOM | DT_RIGHT | DT_SINGLELINE);
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%d", pPanel->yMax);
	DrawText(hdc, strInfo, -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_RIGHT | DT_SINGLELINE);
	//XCale
// 	DrawText(hdc, strXCale, -1, &(pPanel->rcView), DT_NOCLIP | DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
	
	//Grid
	DrawHDotLine(hdc, 
		     pPanel->rcView.left, 
		     pPanel->rcView.top+RECTH(pPanel->rcView)/2,
		     RECTW(pPanel->rcView));
	
	Rectangle(hdc, 
		  pPanel->rcView.left, 
		  pPanel->rcView.top, 
		  pPanel->rcView.right, 
		  pPanel->rcView.bottom);
	
	return 0;
}

static int DrawCrg_Resp(HDC hdc, PCRG_TREND pPanel)
{
	SetTextColor(hdc, CRG_TEXTCOLOR);
	
	//Label
	DrawText(hdc, "Resp", -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%d", pPanel->yMin);
	DrawText(hdc, strInfo, -1, &(pPanel->rcView), DT_NOCLIP | DT_BOTTOM | DT_RIGHT | DT_SINGLELINE);
	memset(strInfo, 0, sizeof strInfo);
	snprintf(strInfo, sizeof strInfo, "%d", pPanel->yMax);
	DrawText(hdc, strInfo, -1, &(pPanel->rcView), DT_NOCLIP | DT_TOP | DT_RIGHT | DT_SINGLELINE);
	//XCale
 	DrawText(hdc, strXCale, -1, &(pPanel->rcView), DT_NOCLIP | DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
	
	//Grid
	DrawHDotLine(hdc, 
		     pPanel->rcView.left, 
		     pPanel->rcView.top+RECTH(pPanel->rcView)/2,
		     RECTW(pPanel->rcView));
	
	Rectangle(hdc, 
		  pPanel->rcView.left, 
		  pPanel->rcView.top, 
		  pPanel->rcView.right, 
		  pPanel->rcView.bottom);
	
	return 0;
}

static int DrawCrg_Label(HDC hdc, RECT rc)
{
	SetTextColor(hdc, CRG_TEXTCOLOR);
	SelectFont(hdc, gFontSystem);
	//Label
	memset(strInfo, 0, sizeof strInfo);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "oxycrg", strInfo, sizeof strInfo,"oxyCRG");				
	DrawText(hdc, strInfo, -1, &(rc), DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	
	Rectangle(hdc, 
		  rc.left, 
		  rc.top, 
		  rc.right, 
		  rc.bottom);
	SelectFont(hdc, gFontSmall);
	return 0;
}

static int DrawCrg_Time(HDC hdc, RECT rc)
{
	SetTextColor(hdc, CRG_TEXTCOLOR);
	
	DrawText(hdc, strXCale, -1, &(rc), DT_NOCLIP | DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
	
	Rectangle(hdc, 
		  rc.left, 
		  rc.top, 
		  rc.right, 
		  rc.bottom);
	
	return 0;
}

/*
	绘制数据
	绘制时,根据时间与像素之间的比例,按时间绘制
	bClean: TRUE:清空波形
*/
static int DrawValue(HDC hdc, PCRG_DATA pData, int iCount)
{
	int iLeft = 0, iTop = 0, iRight = 0, iBottom = 0, iWidth = 0, iHeight = 0;
	static int x1=0, y1=0; 
	static int x2=0, y2=0;
	int i, j;
		
	for(i=0; i<CRG_COUNT; i++){
		//取得区域坐标
		iLeft = gCrgPanels[i].rcView.left;
		iTop = gCrgPanels[i].rcView.top;
		iRight = gCrgPanels[i].rcView.right;
		iBottom = gCrgPanels[i].rcView.bottom;
		iHeight = iBottom - iTop;
		iWidth = iRight - iLeft;	
		
		switch(gCrgPanels[i].bID){
			case CRG_HR:{
				//以点为中心,画短棒图
				SetPenColor(hdc, gCfgEcg.iColor);
				x1 = iRight - iCount *gCrgPanels[i].fRatio_x;
				y1 = pData->wHR;
				if(y1 !=0){
					if(y1 > gCrgPanels[i].yMax) y1 = gCrgPanels[i].yMax;
					if(y1 < gCrgPanels[i].yMin) y1 = gCrgPanels[i].yMin;
					y1 = iBottom - (y1 - gCrgPanels[i].yMin)*gCrgPanels[i].fRatio_y;
					DrawWideLine(hdc, REALWIDELINE, 1, x1, y1-1, x1, y1+1, WIDELINE_RECT);
				}
			}break;
			case CRG_SPO2:{
				//从上往下画棒图
				SetPenColor(hdc, gCfgSpO2.iColor);
				x1 = iRight - iCount*gCrgPanels[i].fRatio_x;
				y1 = pData->bSpO2;
				if(y1 !=0){
					if(y1 > gCrgPanels[i].yMax) y1 = gCrgPanels[i].yMax;
					if(y1 < gCrgPanels[i].yMin) y1 = gCrgPanels[i].yMin;
					y1 = iBottom - (y1 - gCrgPanels[i].yMin)*gCrgPanels[i].fRatio_y;
// 					DrawWideLine(hdc, REALWIDELINE, 1, x1, iTop, x1, y1, WIDELINE_RECT);
					DrawWideLine(hdc, REALWIDELINE, 1, x1, y1-1, x1, y1+1, WIDELINE_RECT); ////以点为中心,画短棒图
				}
			}break;
			case CRG_RR:{
				//以点为中心,画短棒图
				SetPenColor(hdc, gCfgResp.iColor);
				x1 = iRight - iCount *gCrgPanels[i].fRatio_x;
				y1 = pData->wRR;
				if(y1 !=0){
					if(y1 > gCrgPanels[i].yMax) y1 = gCrgPanels[i].yMax;
					if(y1 < gCrgPanels[i].yMin) y1 = gCrgPanels[i].yMin;
					y1 = iBottom - (y1 - gCrgPanels[i].yMin)*gCrgPanels[i].fRatio_y;
					DrawWideLine(hdc, REALWIDELINE, 1, x1, y1-1, x1, y1+1, WIDELINE_RECT);
				}
 			}break;
// 			case CRG_RESP:{
// 				//压缩呼吸波形
// 				SetPenColor(hdc, gCfgResp.iColor);
// 				x1 = iRight - iCount *gCrgPanels[i].fRatio_x;
//
// 				MoveTo(hdc, x1, y2);
// 				for(j=0; j<200; j++){
// 					x2 = x1 + j*gCrgPanels[i].fRatio_x_wave;
// 					y2 = pData->bResp[j]; 
// 					if(y2 > gCrgPanels[i].yMax) y2 = gCrgPanels[i].yMax;
// 					if(y2 < gCrgPanels[i].yMin) y2 = gCrgPanels[i].yMin;
// 					y2 = iBottom - (y2 - gCrgPanels[i].yMin)*gCrgPanels[i].fRatio_y;
// 					LineTo(hdc, x2, y2);
// 				}
// 			}break;
		}
		
	}
	
	return 0;
}

//绘制图形
static int DrawCrgValue(HDC hdc)
{
	int nType;
	
	WORD wHead, wTail, wSize;
	BOOL bLoop;	
	CRG_DATA  tmpCrgData[CRG_DATA_MAX_COUNT];
	int i;
	int iCount = 0;
	int iCRG_Count=0;
	//获取趋势数据的信息
	wHead = gCrgDataArray.wHeadIndex;
	wTail = gCrgDataArray.wTailIndex;
	bLoop = gCrgDataArray.bLoop;
	wSize = CRG_DATA_MAX_COUNT;

	iCRG_Count=giCrg_XScale;
	//判断趋势数据是否为空
	if(wHead == wTail) return -1;
	//printf("wsize=%d\n",wSize);

	
	//获取段趋势需要画图的点
	if(bLoop){//数据已经存满情况下
		for(i=0;i<giCrg_XScale;i++){
			tmpCrgData[i]=gCrgDataArray.Datas[(wHead-i+CRG_DATA_MAX_COUNT)%CRG_DATA_MAX_COUNT];
		}
	}else{//数据未存满
		if(wHead < giCrg_XScale ){//数据未超出短趋势时间.只循环
			for(i=0;i<wHead;i++){
				tmpCrgData[i]=gCrgDataArray.Datas[wHead-i];
			}
			iCRG_Count=wHead;
		}else{//数据超出时间，
			for(i=0;i<giCrg_XScale;i++){
				tmpCrgData[i]=gCrgDataArray.Datas[wHead-i];
			}
		
		}
	}
for(i=0;i<iCRG_Count;i++){
	//	printf("st[%d].hr=%d\n",i,tmpTrendData[i].wHR);
		DrawValue(hdc,  &tmpCrgData[i], i);
	}

#if 0	
	//从最新数据(Head)往前(Tail)取指定时间内的数据, 边取边画
	if(wHead < wTail){
		for(i=wHead; i>=0; i--, iCount++){
			//绘制数据
			if(iCount < giCrg_XScale){
				tmpCrgData = gCrgDataArray.Datas[i];
				DrawValue(hdc, &tmpCrgData, iCount);
			}
		}
		for(i=(wSize-1); i>=wTail; i--, iCount++){
                        //绘制数据
			if(iCount < giCrg_XScale){
				tmpCrgData = gCrgDataArray.Datas[i];
				DrawValue(hdc, &tmpCrgData, iCount);
			}
		}
	}
	else if(bLoop){
		for(i=wHead; i>=wTail; i--, iCount++){
			//绘制数据
			if(iCount < giCrg_XScale){
				tmpCrgData = gCrgDataArray.Datas[i];
				DrawValue(hdc, &tmpCrgData, iCount);
			}
		}
	}
	else{
		for(i=wHead; i>wTail; i--, iCount++){
			//绘制数据
			if(iCount < giCrg_XScale){
				
				tmpCrgData = gCrgDataArray.Datas[i];
				DrawValue(hdc, &tmpCrgData, iCount);
			}
		}
	}	
#endif
	return 0;
}

//绘制趋势图
static int DrawCrg()
{
	int i;

	if((gCfgSystem.bInterface == SCREEN_OXYCRG)  && (hdc_oxyCRG != (HDC)NULL) && (mdc_oxyCRG != (HDC)NULL)) {
		//清屏
		FillBox(mdc_oxyCRG, 
			gRCCrg.left, 
			gRCCrg.top, 
			RECTW(gRCCrg)+1, 
			RECTH(gRCCrg)+1);

		//绘制趋势数据
		DrawCrgValue(mdc_oxyCRG);
	
		//绘制框架
		SetPenColor(mdc_oxyCRG, CRG_PENCOLOR);
		for(i=0; i<giCrgCount; i++){
			switch(gCrgPanels[i].bID){
				case CRG_HR:{
					DrawCrg_HR(mdc_oxyCRG, &(gCrgPanels[i]));
				}break;
				case CRG_SPO2:{
					DrawCrg_SpO2(mdc_oxyCRG, &(gCrgPanels[i]));
				}break;
				case CRG_RR:{
					DrawCrg_RR(mdc_oxyCRG, &(gCrgPanels[i]));
				}break;
				case CRG_RESP:{
					DrawCrg_Resp(mdc_oxyCRG, &(gCrgPanels[i]));
				}break;
			}
		}
	
		//Label
		DrawCrg_Label(mdc_oxyCRG, gRCCrgLabel);
		//Time
		DrawCrg_Time(mdc_oxyCRG, gRCCrgValue);
		
		//拷贝DC
		if((gCfgSystem.bInterface == SCREEN_OXYCRG)  && (hdc_oxyCRG != (HDC)NULL)) 
			BitBlt(mdc_oxyCRG, 
				gRCCrg.left, gRCCrg.top, RECTW(gRCCrg)+1, RECTH(gRCCrg)+1, 
				hdc_oxyCRG, 
				gRCCrg.left, gRCCrg.top, 
				0);
		else
			if(B_PRINTF) printf("-----------------------------------------------%s:%d  No crg hdc, bitblt failed.\n", __FILE__, __LINE__);
	}	
	else
		if(B_PRINTF) printf("-----------------------------------------------%s:%d  No crg hdc, bitblt failed.\n", __FILE__, __LINE__);
	
	
	
	return 0;
}

static int DrawCrg_hdc()
{
	int i;

	//清屏
	FillBox(hdc_oxyCRG, 
		gRCCrg.left, 
		gRCCrg.top, 
		RECTW(gRCCrg)+1, 
		RECTH(gRCCrg)+1);

	//绘制趋势数据
	DrawCrgValue(hdc_oxyCRG);
	
	//绘制框架
	SetPenColor(hdc_oxyCRG, CRG_PENCOLOR);
	for(i=0; i<giCrgCount; i++){
		switch(gCrgPanels[i].bID){
			case CRG_HR:{
				DrawCrg_HR(hdc_oxyCRG, &(gCrgPanels[i]));
			}break;
			case CRG_SPO2:{
				DrawCrg_SpO2(hdc_oxyCRG, &(gCrgPanels[i]));
			}break;
			case CRG_RR:{
				DrawCrg_RR(hdc_oxyCRG, &(gCrgPanels[i]));
			}break;
			case CRG_RESP:{
				DrawCrg_Resp(hdc_oxyCRG, &(gCrgPanels[i]));
			}break;
		}
	}
	
	//Label
	DrawCrg_Label(hdc_oxyCRG, gRCCrgLabel);
	//Time
	DrawCrg_Time(hdc_oxyCRG, gRCCrgValue);
	
	return 0;
}

//绘图线程
static void *ThreadProcCrg(void *arg)
{
	
	for(;;)
	{
		sem_wait(&semCrg);
	//	if((gCfgSystem.bInterface == SCREEN_OXYCRG)){
			gbDrawCrgFinished  = FALSE;
			
			DrawCrg();
			gbDrawCrgFinished  = TRUE;
		//}
			
			//DrawCrg_hdc();
	}

	if(B_PRINTF) printf("%s:%d Cancel OxyCrg thread.\n", __FILE__, __LINE__);
	pthread_exit("Cancel OxyCrg thread.\n");
}

//销毁HDC
int NewDestroyCrg()
{
	while(!gbDrawCrgFinished) {
		if(B_PRINTF) printf("------------------------------------------------------%s:%d CRG not draw finished.\n", __FILE__, __LINE__);
		sleep(1);
	}
	
		if(mdc_oxyCRG != (HDC)NULL){
			DeleteCompatibleDC(mdc_oxyCRG);
			mdc_oxyCRG = (HDC)NULL;
			if(B_PRINTF) printf("--------------------Destroy oxyCRG mdc.\n");
		}
		
		if(hdc_oxyCRG != (HDC)NULL){
			ReleaseDC(hdc_oxyCRG);
			hdc_oxyCRG = (HDC)NULL;
			if(B_PRINTF) printf("--------------------Destroy oxyCRG hdc.\n");
		}
	
	gbDrawCrgFinished = TRUE;

	return 0;
}

//初始化短趋势
int NewInitCrg()
{
	//显示面板
	ShowWindow(ghWndPanel_Crg, SW_SHOW);
	
	hdc_oxyCRG = mdc_oxyCRG = (HDC)NULL;
	//建立绘图memDC
	if(hdc_oxyCRG == (HDC)NULL){
  		hdc_oxyCRG = GetClientDC(ghWndPanel_Crg);
		
		if(hdc_oxyCRG == (HDC)NULL) return -1;
		SetPenColor(hdc_oxyCRG, CRG_PENCOLOR);
		SetBkMode(hdc_oxyCRG, BM_TRANSPARENT);
		SetBkColor(hdc_oxyCRG, CRG_BKCOLOR);
		SetBrushColor(hdc_oxyCRG, CRG_BKCOLOR);
		SetTextColor(hdc_oxyCRG, CRG_TEXTCOLOR);
		SelectFont(hdc_oxyCRG, gFontSmall);
 		mdc_oxyCRG = CreateCompatibleDC(hdc_oxyCRG);
		
		if(B_PRINTF) printf("%s:%d create hdc of oxyCRG OK!\n", __FILE__, __LINE__);
	}else{
		if(B_PRINTF) printf("%s:%d Hdc of oxyCRG had created.\n", __FILE__, __LINE__);
	}
	
	//获得X刻度
	//获得X刻度
	switch (gCfgSystem.bOxyCrgScale){
		case 1:{
			giCrg_XScale= 120;
		}break;
		case 2:{
 			giCrg_XScale = 300;
//			giCrg_XScale = 240;
		}break;
		case 3:{
			giCrg_XScale = 600;
		}break;
		case 4:{
			giCrg_XScale = 900;
		}break;
		case 5:{
			giCrg_XScale = 1800;
		}break;
		default:{
			giCrg_XScale = 60;
		}break;
	}
	
	//获得时间单位
	memset(strUnit, 0, sizeof strUnit);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "oxycrg_unit", strUnit, sizeof strUnit,"S");		
	memset(strXCale, 0, sizeof strXCale);
	snprintf(strXCale, sizeof strXCale, "%d %s", giCrg_XScale, strUnit);
	
	//初始化区域
	InitCrgAttr();

	//建立绘图线程
	if(!gbThreadHasCreated_Crg){
		sem_init(&semCrg, 0, 0);
		pthread_create(&ptCrg, NULL, ThreadProcCrg, NULL);	
		if(B_PRINTF) printf("%s:%d create thread of drawing oxyCRG OK!\n", __FILE__, __LINE__);
		usleep(10000);
		gbThreadHasCreated_Crg = TRUE;
	}
	else{
		if(B_PRINTF) printf("%s:%d Thread of drawing oxyCRG has created!\n", __FILE__, __LINE__);
	}

	return 0;
}


int NewChangeCrgScale()
{
	//获得X刻度
	switch (gCfgSystem.bOxyCrgScale){
		case 1:{
			giCrg_XScale = 120;
		}break;
		case 2:{
 			giCrg_XScale = 300;
//			giCrg_XScale = 240;
		}break;
		case 3:{
			giCrg_XScale = 600;
		}break;
		case 4:{
			giCrg_XScale = 900;
		}break;
		case 5:{
			giCrg_XScale = 1800;
		}break;
		default:{
			giCrg_XScale = 60;
		}break;
	}

	//获得时间单位
	memset(strUnit, 0, sizeof strUnit);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "oxycrg_unit", strUnit, sizeof strUnit);		
	memset(strXCale, 0, sizeof strXCale);
	snprintf(strXCale, sizeof strXCale, "%d %s", giCrg_XScale, strUnit);
	
	//初始化区域
	InitCrgAttr();

	return 0;
}

int NewUpdateCrg()
{
//	if((gCfgSystem.bInterface == SCREEN_OXYCRG))
		sem_post(&semCrg);
			
	return 0;
	
}


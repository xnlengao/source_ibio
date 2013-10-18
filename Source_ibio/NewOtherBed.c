/*************************************************************************** 
 *	Module Name:	NewOtherBed
 *
 *	Abstract:	OtherBed
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2013-02-4 09:04:14
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "General_Funcs.h"
#include "NewDrawWave.h"
#include "Dialog.h"
#include "Central.h"
//区域定义
#define BED_LEFT		RC_OXYCRG_LEFT
#define BED_TOP			RC_OXYCRG_BOTTOM+20
#define BED_RIGHT		RC_NORMAL_RIGHT
#define BED_BOTTOM		RC_NORMAL_BOTTOM
//参数定义
#define PARA_HR		0
#define PARA_SPO2	1
#define PARA_PR		2	
#define PARA_RR		3	
#define PARA_T1		4
#define PARA_T2		5
#define PARA_NIBP	6	


extern HWND btnBedNO;
extern HWND btnWave;
extern HWND cobBedNO;
extern HWND cobWave;
 
static WNDPROC  OldBtnProc;
static WNDPROC  OldCobProc;
//绘制线段的宽度
#define CRG_LINE_WIDTH		1


//参数数量, HR, SpO2,PR, RR ,Temp1,temp2,NIBP
#define PARA_COUNT			7

//颜色
#define BED_BKCOLOR		COLOR_black
#define BED_TEXTCOLOR		COLOR_darkgray
#define BED_PENCOLOR		COLOR_darkgray

//显示区域
RECT gRCOtherBed;
RECT gRCWave;
RECT gRCParaValue;
RECT gRCbtn;

//参数显示区域结构体
typedef struct {
	BYTE bID;		//参数ID	
	RECT rcView;		//显示区域
	int iColor;			//参数绘制颜色
}OB_PARA, *POB_PARA;

static OB_PARA gParaPanels[PARA_COUNT];

//趋势数量
static int giCrgCount;	

//短趋势X轴显示刻度即时间范围, 单位:秒
static int giCrg_XScale = 120;

static unsigned char strInfo[20]={0};
static unsigned char strXCale[20]={0};
static unsigned char strUnit[10]={0};

//HDC
static HDC hdc_OtherBed = (HDC)NULL;
//MDC
static HDC mdc_OtherBed= (HDC)NULL;
NWAVE_PANEL gNWavePanelsOtherBed;
extern HWND ghWndPanel_OtherBed;
extern HWND	ghWndMain;
//绘制参数线程
static pthread_t ptOtherBed;
static sem_t semOtherBed;	
static BOOL gbThreadHasCreated_OtherBed_Para = FALSE;
static BOOL gbDrawOtherBedFinished  = TRUE;
//绘制波形线程
static pthread_t ptOtherBedWave;
static sem_t semOtherBedWave;	
static BOOL gbThreadHasCreated_OtherBed_Wave = FALSE;
static BOOL gbDrawWaveOtherBedFinished  = TRUE;
//控制是否可以进行绘制的变量
BOOL gbNCanDrawWaveOtherBed = FALSE;
extern BOOL gbNCanDrawWave;
//他床数据
extern CENTERDATA gOtherBedData;
//需要数据的他床床号  (255表示当前没有需要请求数据)
extern BYTE  gbOtherBedIndex;
//改变床边机
extern BOOL gbChangeBedIndex;
//他床的通讯状态  0-offline  1-online
extern BYTE gbOtherBedStatus[240];
//他床波形选择
extern BYTE gbOtherBedWaveIndex;
//波形复位控制
BOOL gbOtherBedResetWaves=FALSE;
extern BOOL gbResetWaves;
//波形数据
static S_ECG_VALUE	NetEcgWave;
static S_SPO2_VALUE	NetSpO2Wave;
static S_RESP_VALUE	NetRespWave;

//ECG采样点对应屏幕像素的比例系数
extern float gfEcgSampleToPixel;
///1mv对应的采样点数
extern float gEcgSamplesPerMV ;
extern int giEcgBaseline ;
extern BOOL gbNCanDrawWave_OB;

/*
	按钮消息回调函数
*/
static int BtnProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	
	temp = (*OldBtnProc)(hWnd, message, wParam, lParam);
		
	switch(message){
 		case MSG_SETFOCUS:{
 			SetWindowBkColor(hWnd, SETFOCUS_COLOR);
 		}break;
 		case MSG_KILLFOCUS:{
 			SetWindowBkColor(hWnd, KILLFOCUS_COLOR);
 		}break;
		default: break;
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
					
						case 0:{
							
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
/*
	根据面板的设置（ID和区域）设置X，Y轴 的系数
*/
int SetWavePanelXY_OB(PNWAVE_PANEL pWavePanel)
{
	//X
	float ftmpX;
	float fSpeeds[4] = {6.25, 12.5, 25.0, 50.0};
	float ftmpSpeed;
	//Y
	int iMaxY, iMinY;
	float ftmpY;
	float gfNMMToPixel=3.2;
	
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
	ftmpX =(float)( (float)250 / (ftmpSpeed * gfNMMToPixel));
	
	//Y系数：(Max-Min) / (RECTH / WaveCount)
	ftmpY = (float)((float)(iMaxY - iMinY) / (float)((float)(RECTH(pWavePanel->rcWave))/(float)pWavePanel->bWaveCount));
	
	if(B_PRINTF) printf("%s:%d Panel ID : %d, Speed: %.1f, gfNMMToPixel:%f,X_Axis: %.1f.\n", __FILE__, __LINE__, 
	       pWavePanel->bWaveID, ftmpSpeed,gfNMMToPixel, ftmpX);
	
	if(B_PRINTF) printf("YMax: %d, YMin: %d, RECTH: %d,bWaveCount: %d, Y_Axis: %.1f.\n", 
	       iMaxY, iMinY, RECTH(pWavePanel->rcWave), pWavePanel->bWaveCount,ftmpY);
		
	//设置XY系数
	pWavePanel->fXAxis = ftmpX;
	pWavePanel->fYAxis = ftmpY;
	
	return 0;
}

//绘制边框线条
static int DrawRectangle()
{
	int i;
	//绘制框架
	SetPenColor(mdc_OtherBed, COLOR_darkgray);
	//整体框图
	Rectangle(mdc_OtherBed, 
			gRCOtherBed.left, 
			gRCOtherBed.top, 
			gRCOtherBed.right-2,
			gRCOtherBed.bottom-1);

	//按钮区边框
	Rectangle(mdc_OtherBed, 
			gRCbtn.left, 
			gRCbtn.top, 
			gRCbtn.right-2,
			gRCbtn.bottom);
	//参数区边框
	for(i=0;i<PARA_COUNT;i++){
	Rectangle(mdc_OtherBed, 
			gParaPanels[i].rcView.left, 
			gParaPanels[i].rcView.top, 
			gParaPanels[i].rcView.right,
			gParaPanels[i].rcView.bottom);
	}
//	SetPenColor(mdc_OtherBed, COLOR_yellow);
	//波形区边框
//	Rectangle(mdc_OtherBed, 
//			gRCWave.left, 
//			gRCWave.top, 
//			gRCWave.right-2,
//			gRCWave.bottom);
	//拷贝DC
	if((gCfgSystem.bInterface == SCREEN_OTHERBED)  && (hdc_OtherBed!= (HDC)NULL)) 
		BitBlt(mdc_OtherBed, 
			gRCOtherBed.left, gRCOtherBed.top, RECTW(gRCOtherBed)+1, RECTH(gRCOtherBed)+1, 
			hdc_OtherBed, 
			gRCOtherBed.left, gRCOtherBed.top, 
			0);
	else
		if(B_PRINTF) printf("-----------------------------------------------%s:%d  No OtherBed hdc, bitblt failed.\n", __FILE__, __LINE__);
	return 0;
}
static int DrawPara_HR(HDC hdc)
{
	static char strOldValue[30]={0};
	unsigned char strValue[30]={0};
	
	SelectFont(hdc, gFontSystem);
	SetTextColor(hdc, COLOR_black);
	DrawText(hdc,strOldValue, -1, &(gParaPanels[PARA_HR].rcView), DT_NOCLIP  | DT_VCENTER | DT_CENTER | DT_SINGLELINE);

	SetTextColor(hdc, gParaPanels[PARA_HR].iColor);
	if(gOtherBedData.ECG_para_HR <350 && gOtherBedData.ECG_para_HR >0 )
		snprintf(strValue, sizeof strValue, "%d", gOtherBedData.ECG_para_HR);
	else
		snprintf(strValue, sizeof strValue, "---");
	
	//label
	DrawText(hdc, "HR", -1, &(gParaPanels[PARA_HR].rcView), DT_NOCLIP | DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
	
	DrawText(hdc,strValue, -1, &(gParaPanels[PARA_HR].rcView), DT_NOCLIP  | DT_VCENTER | DT_CENTER | DT_SINGLELINE);
//	TextOut(hdc, gParaPanels[PARA_HR].rcView.left+30,  gParaPanels[PARA_HR].rcView.top+10, strValue);

	memset(strOldValue, 0, sizeof strOldValue);
	strcpy(strOldValue, strValue);
	return 0;
}
static int DrawPara_SPO2(HDC hdc)
{
	static char strOldValue[30]={0};
	unsigned char strValue[30]={0};
	
	SelectFont(hdc, gFontSystem);
	
	SetTextColor(hdc, COLOR_black);
	DrawText(hdc,strOldValue, -1, &(gParaPanels[PARA_SPO2].rcView), DT_NOCLIP  | DT_VCENTER | DT_CENTER | DT_SINGLELINE);

	SetTextColor(hdc, gParaPanels[PARA_SPO2].iColor);
	DrawText(hdc, "SPO2", -1, &(gParaPanels[PARA_SPO2].rcView), DT_NOCLIP | DT_BOTTOM | DT_CENTER | DT_SINGLELINE);

	if(gOtherBedData.SPO2_para_spo2 <=100 && gOtherBedData.SPO2_para_spo2>30 )
		snprintf(strValue, sizeof strValue, "%d", gOtherBedData.SPO2_para_spo2);
	else
		snprintf(strValue, sizeof strValue, "---");
	DrawText(hdc, strValue, -1, &(gParaPanels[PARA_SPO2].rcView), DT_NOCLIP  | DT_VCENTER | DT_CENTER | DT_SINGLELINE);

	
	memset(strOldValue, 0, sizeof strOldValue);
	strcpy(strOldValue, strValue);
	
	return 0;
}

static int DrawPara_PR(HDC hdc)
{
	static char strOldValue[30]={0};
	unsigned char strValue[30]={0};
	
	SelectFont(hdc, gFontSystem);
	SetTextColor(hdc, COLOR_black);
	DrawText(hdc,strOldValue, -1, &(gParaPanels[PARA_PR].rcView), DT_NOCLIP  | DT_VCENTER | DT_CENTER | DT_SINGLELINE);

	SetTextColor(hdc, gParaPanels[PARA_PR].iColor);
	DrawText(hdc, "PR", -1, &(gParaPanels[PARA_PR].rcView), DT_NOCLIP | DT_BOTTOM | DT_CENTER | DT_SINGLELINE);

	if(gOtherBedData.SPO2_para_PR <=350&& gOtherBedData.SPO2_para_PR>0 )
		snprintf(strValue, sizeof strValue, "%d", gOtherBedData.SPO2_para_PR);
	else
		snprintf(strValue, sizeof strValue, "---");
	DrawText(hdc, strValue, -1, &(gParaPanels[PARA_PR].rcView), DT_NOCLIP  | DT_VCENTER | DT_CENTER | DT_SINGLELINE);

	
	memset(strOldValue, 0, sizeof strOldValue);
	strcpy(strOldValue, strValue);	
	return 0;
}
static int DrawPara_RR(HDC hdc)
{
	static char strOldValue[30]={0};
	unsigned char strValue[30]={0};
	
	SelectFont(hdc, gFontSystem);
	
	SetTextColor(hdc, COLOR_black);
	DrawText(hdc,strOldValue, -1, &(gParaPanels[PARA_RR].rcView), DT_NOCLIP  | DT_VCENTER | DT_CENTER | DT_SINGLELINE);

	SetTextColor(hdc, gParaPanels[PARA_RR].iColor);
	DrawText(hdc, "RR", -1, &(gParaPanels[PARA_RR].rcView), DT_NOCLIP | DT_BOTTOM | DT_CENTER | DT_SINGLELINE);

	if(gOtherBedData.RESP_para_RR <=250 && gOtherBedData.RESP_para_RR>0 )
		snprintf(strValue, sizeof strValue, "%d" , gOtherBedData.RESP_para_RR);
	else
		snprintf(strValue, sizeof strValue, "---");
	DrawText(hdc, strValue, -1, &(gParaPanels[PARA_RR].rcView), DT_NOCLIP  | DT_VCENTER | DT_CENTER | DT_SINGLELINE);

	
	memset(strOldValue, 0, sizeof strOldValue);
	strcpy(strOldValue, strValue);	
	return 0;
}

static int DrawPara_T1(HDC hdc)
{
	static char strOldValue[30]={0};
	unsigned char strValue[30]={0};
	
	SelectFont(hdc, gFontSystem);
	SetTextColor(hdc, COLOR_black);
	DrawText(hdc,strOldValue, -1, &(gParaPanels[PARA_T1].rcView), DT_NOCLIP  | DT_VCENTER | DT_CENTER | DT_SINGLELINE);

	SetTextColor(hdc, gParaPanels[PARA_T1].iColor);
	DrawText(hdc, "TEMP1", -1, &(gParaPanels[PARA_T1].rcView), DT_NOCLIP | DT_BOTTOM | DT_CENTER | DT_SINGLELINE);

	if(gOtherBedData.TEMP1_para_temp <=500&& gOtherBedData.TEMP1_para_temp>0 )
		snprintf(strValue, sizeof strValue, "%.1f" , (float)(gOtherBedData.TEMP1_para_temp/10.0));
	else
		snprintf(strValue, sizeof strValue, "--.-");
	DrawText(hdc, strValue, -1, &(gParaPanels[PARA_T1].rcView), DT_NOCLIP  | DT_VCENTER | DT_CENTER | DT_SINGLELINE);

	
	memset(strOldValue, 0, sizeof strOldValue);
	strcpy(strOldValue, strValue);
	return 0;
}

static int DrawPara_T2(HDC hdc)
{
	static char strOldValue[30]={0};
	unsigned char strValue[30]={0};
	SelectFont(hdc, gFontSystem);
	SetTextColor(hdc, COLOR_black);
	DrawText(hdc,strOldValue, -1, &(gParaPanels[PARA_T2].rcView), DT_NOCLIP  | DT_VCENTER | DT_CENTER | DT_SINGLELINE);

	SetTextColor(hdc, gParaPanels[PARA_T2].iColor);
	DrawText(hdc, "TEMP2", -1, &(gParaPanels[PARA_T2].rcView), DT_NOCLIP | DT_BOTTOM | DT_CENTER | DT_SINGLELINE);

	if(gOtherBedData.TEMP2_para_temp <=500&& gOtherBedData.TEMP2_para_temp>0 )
		snprintf(strValue, sizeof strValue, "%.1f" , (float)(gOtherBedData.TEMP2_para_temp/10.0));
	else
		snprintf(strValue, sizeof strValue, "--.-");
	DrawText(hdc, strValue, -1, &(gParaPanels[PARA_T2].rcView), DT_NOCLIP  | DT_VCENTER | DT_CENTER | DT_SINGLELINE);

	
	memset(strOldValue, 0, sizeof strOldValue);
	strcpy(strOldValue, strValue);
	return 0;
}

static int DrawPara_NIBP(HDC hdc)
{
	static char strOldValue1[30]={0};
	static char strOldValue2[30]={0};
	unsigned char strSYSDIA[30]={0};
	unsigned char strMEAN[30]={0};
	
	SelectFont(hdc, gFontSystem);
	
	SetTextColor(hdc, COLOR_black);
	DrawText(hdc, strOldValue1, -1, &(gParaPanels[PARA_NIBP].rcView), DT_NOCLIP  | DT_TOP | DT_CENTER | DT_SINGLELINE);
	DrawText(hdc, strOldValue2, -1, &(gParaPanels[PARA_NIBP].rcView), DT_NOCLIP  | DT_VCENTER | DT_CENTER | DT_SINGLELINE);

	SetTextColor(hdc, gParaPanels[PARA_NIBP].iColor);
	DrawText(hdc, "NIBP", -1, &(gParaPanels[PARA_NIBP].rcView), DT_NOCLIP | DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
//		SetTextColor(hdc, gParaPanels[PARA_NIBP].iColor);
	if((gOtherBedData.NIBP_para_SYS <=400 && gOtherBedData.NIBP_para_SYS>0)
		||(gOtherBedData.NIBP_para_DIA <=400 && gOtherBedData.NIBP_para_DIA>0)
		||(gOtherBedData.NIBP_para_MEAN <=400 && gOtherBedData.NIBP_para_MEAN>0)){
			snprintf(strSYSDIA, sizeof strSYSDIA, "%d/%d", gOtherBedData.NIBP_para_SYS,gOtherBedData.NIBP_para_DIA);
			snprintf(strMEAN, sizeof strMEAN, "(%d)", gOtherBedData.NIBP_para_MEAN);
	}
	else{
			snprintf(strSYSDIA, sizeof strSYSDIA, "---/---");
			snprintf(strMEAN, sizeof strMEAN, "(---)");
		}
	
//	if(B_PRINTF) printf("```````````other bed sys:%d dai:%d mean:%d\n",
//		gOtherBedData.NIBP_para_SYS,gOtherBedData.NIBP_para_DIA,gOtherBedData.NIBP_para_MEAN);
	DrawText(hdc, strSYSDIA, -1, &(gParaPanels[PARA_NIBP].rcView), DT_NOCLIP  | DT_TOP | DT_CENTER | DT_SINGLELINE);
	DrawText(hdc, strMEAN, -1, &(gParaPanels[PARA_NIBP].rcView), DT_NOCLIP  | DT_VCENTER | DT_CENTER | DT_SINGLELINE);

	memset(strOldValue1, 0, sizeof strOldValue1);
	memset(strOldValue2, 0, sizeof strOldValue2);
	strcpy(strOldValue1, strSYSDIA);
	strcpy(strOldValue2, strMEAN);
	return 0;
}

//绘制Ecg 波形
static int DrawWave_Ecg(HDC hdc, HDC mdc, const PNWAVE_PANEL pPanel, int iPointCount)
{
	static int x = 0;
	static int y = 0;
	static int iWaveCount = 0;
	static int iValue;
	BOOL res;
	int i, j;
	int x1, y1;
	static BOOL bNext = FALSE;	//级联的波形
	int iLinkMove = 0;			//级联波形需要移动距离
	
	//开始绘制标志 ，初始化X，Y坐标 
	if(gbOtherBedResetWaves){
//	if(gbResetWaves){
		x = pPanel->rcWave.left;
		y = pPanel->rcWave.top + RECTH(pPanel->rcWave)/2;
		iWaveCount = 0;
		FillBox(hdc, 
			pPanel->rcWave.left, 
			pPanel->rcWave.top, 
			RECTW(pPanel->rcWave)-2, 
			RECTH(pPanel->rcWave));
		FillBox(mdc, 
			pPanel->rcWave.left, 
			pPanel->rcWave.top, 
			RECTW(pPanel->rcWave)-2, 
			RECTH(pPanel->rcWave));
		//清空绘图缓冲
		NetClearData_Ecg1();
	}
	
	//波形以及文字颜色根据配置而定
	SetPenColor(hdc, gCfgEcg.iColor);	
	SetPenColor(mdc, gCfgEcg.iColor);	

		x1 =x;
		y1 =y;
		MoveTo(hdc, x, y);
		MoveTo(mdc, x, y);
		for(j=0; j<iPointCount; j++){
			//获得波形值
			res = NetRetrieveData_Ecg1(&NetEcgWave);
	//		res = RetrieveData_Ecg2(&NetEcgWave);
			
			if(res){
				iValue = NetEcgWave.iWaves[gbOtherBedWaveIndex];
				y = pPanel->rcWave.top 
						+  RECTH(pPanel->rcWave) - iValue/(gfEcgSampleToPixel) 
						+ (giEcgBaseline - RECTH(pPanel->rcWave)/2);
			}else{
				//printf("res exg \n");
				//当取不到数据时，跳出本次画波形，
				goto BREAKDRAW;
			}
			
			//Y
			if(y > pPanel->rcWave.bottom-2) y = pPanel->rcWave.bottom-2;
			if(y<pPanel->rcWave.top) y = pPanel->rcWave.top;
		
			//X
			if(x < pPanel->rcWave.right-14){
				x = pPanel->rcWave.left + (float)(iWaveCount) / (float)(pPanel->fXAxis);
				iWaveCount ++;
				
			}
			else{
				x = pPanel->rcWave.left;
				iWaveCount = 0;
				MoveTo(hdc, x, y);
				MoveTo(mdc, x, y);
				x1 = x;
				y1 = y;
				//画扫描棒
				FillBox(hdc, 
					x, 
					pPanel->rcWave.top, 
					iPointCount+4, 
					RECTH(pPanel->rcWave));
				FillBox(mdc, 
					x, 
					pPanel->rcWave.top, 
					iPointCount+4, 
					RECTH(pPanel->rcWave));
			}

			if(x <=  pPanel->rcWave.left+1) {
				MoveTo(hdc, x, y);
				MoveTo(mdc, x, y);
				continue;
			}
			
			LineTo(hdc, x, y);
			LineTo(mdc, x, y);

	
		//画扫描棒
		FillBox(hdc, 
			x+iPointCount, 
			pPanel->rcWave.top, 
			iPointCount, 
			RECTH(pPanel->rcWave));
		FillBox(mdc, 
			x+iPointCount, 
			pPanel->rcWave.top, 
			iPointCount, 
			RECTH(pPanel->rcWave));
	//	DrawScanBar(hdc,mdc,pPanel,x,iPointCount);
	BREAKDRAW:
	;
	}

	
	return 0;
}

//绘制SpO2波形
static int DrawWave_SpO2(HDC hdc, HDC mdc, const PNWAVE_PANEL pPanel, int iPointCount)
{
	static int x = 0;
	static int y = 0;
	static int iWaveCount = 0;
	static int iValue;
	BOOL res;
	int i, j;
	int x1, y1;
	
	//开始绘制标志 ，初始化X，Y坐标 
	if(gbOtherBedResetWaves){
		x = pPanel->rcWave.left;
 		y = pPanel->rcWave.top + RECTH(pPanel->rcWave)/2;
		iWaveCount = 0;
		FillBox(hdc, 
			pPanel->rcWave.left, 
			pPanel->rcWave.top, 
			RECTW(pPanel->rcWave)+1, 
			RECTH(pPanel->rcWave));
		if(mdc != (HDC)NULL)
			FillBox(mdc, 
				pPanel->rcWave.left, 
				pPanel->rcWave.top, 
				RECTW(pPanel->rcWave)+1, 
				RECTH(pPanel->rcWave));
		NetClearData_SpO2();
	}
	
	//波形以及文字颜色根据配置而定
	SetPenColor(hdc, gCfgSpO2.iColor);	
	if(mdc != (HDC)NULL)
		SetPenColor(mdc, gCfgSpO2.iColor);	
	
	x1 =x;
	y1 =y;
	MoveTo(hdc, x, y);
	if(mdc != (HDC)NULL)
		MoveTo(mdc, x, y);
	for(j=0; j<iPointCount; j++){
		
		//获得SpO2波形值
//		res = RetrieveData_SpO2(&NetSpO2Wave);
		res = NetRetrieveData_SpO2(&NetSpO2Wave);

		//Nellcor SpO2  (波形不判断状态)
		if( res){
			iValue = NetSpO2Wave.bWave;
			//if(B_PRINTF) printf("SpO2Wave.bWave:%d\n",iValue);
			y = pPanel->rcWave.top +  RECTH(pPanel->rcWave) - iValue/(pPanel->fYAxis);

		}else{
		
			goto BREAKDRAW;
			
		}
		
		//Y
// 		y = pPanel->rcWave.top +  (NWAVEMAX_SPO2-iValue)/pPanel->fYAxis;
		if(y > pPanel->rcWave.bottom-2) y = pPanel->rcWave.bottom-2;
		if(y<pPanel->rcWave.top) y = pPanel->rcWave.top;
		
		//X
		if(x < pPanel->rcWave.right-14){
			x = pPanel->rcWave.left + (float)(iWaveCount) / (float)(pPanel->fXAxis);
			iWaveCount ++;
		}
		else{
			x = pPanel->rcWave.left;
			iWaveCount = 0;
			MoveTo(hdc, x, y);
			if(mdc != (HDC)NULL)
				MoveTo(mdc, x, y);
			x1 = x;
			y1 = y;
			//画扫描棒
			FillBox(hdc, x, pPanel->rcWave.top, iPointCount+4, RECTH(pPanel->rcWave));
			if(mdc != (HDC)NULL)
			FillBox(mdc, x, pPanel->rcWave.top, iPointCount+4, RECTH(pPanel->rcWave));
		}

		if(x <=  pPanel->rcWave.left+1) {
			MoveTo(hdc, x, y);
			if(mdc != (HDC)NULL)
				MoveTo(mdc, x, y);
			continue;
		}
			
	
		LineTo(hdc, x, y);
		if(mdc != (HDC)NULL)
			LineTo(mdc, x, y);

		BREAKDRAW:
	;
	}
	
 
 	//画扫描棒
		FillBox(hdc, 
			x+iPointCount, 
			pPanel->rcWave.top, 
			iPointCount, 
			RECTH(pPanel->rcWave));
		FillBox(mdc, 
			x+iPointCount, 
			pPanel->rcWave.top, 
			iPointCount, 
			RECTH(pPanel->rcWave));
 	//DrawScanBar(hdc,mdc,pPanel,x,iPointCount);
		
	
	return 0;
}


//绘制Resp波形
static int DrawWave_Resp(HDC hdc, HDC mdc, const PNWAVE_PANEL pPanel, int iPointCount)
{
	static int x = 0;
	static int y = 0;
	static int iWaveCount = 0;
	static int iValue;
	BOOL res;
	int i, j;
	int x1, y1;
	
	//开始绘制标志 ，初始化X，Y坐标 
	if(gbOtherBedResetWaves){
		x = pPanel->rcWave.left;
		y = pPanel->rcWave.top + RECTH(pPanel->rcWave)/2;
		iWaveCount = 0;
		FillBox(hdc, 
			pPanel->rcWave.left, 
			pPanel->rcWave.top, 
			RECTW(pPanel->rcWave)+1, 
			RECTH(pPanel->rcWave));
		FillBox(mdc, 
			pPanel->rcWave.left, 
			pPanel->rcWave.top, 
			RECTW(pPanel->rcWave)+1, 
			RECTH(pPanel->rcWave));
		NetClearData_Resp();
	}
	
	//波形以及文字颜色根据配置而定
	SetPenColor(hdc, gCfgResp.iColor);	
	SetPenColor(mdc, gCfgResp.iColor);	
	
	x1 =x;
	y1 =y;
	MoveTo(hdc, x, y);
	MoveTo(mdc, x, y);
	
	for(j=0; j<iPointCount; j++){
		//获得波形值
	//	res = RetrieveData_Resp(&NetRespWave);
		res = NetRetrieveData_Resp(&NetRespWave);
//		if(B_PRINTF) printf(" RespWave.iWave=%d\n", RespWave.iWave);
		if(res){
			iValue = NetRespWave.iWave;
	//		if(B_PRINTF) printf("%d*****%f*****\n",iValue,pPanel->fYAxis);
			y = pPanel->rcWave.top +  RECTH(pPanel->rcWave) - iValue/(pPanel->fYAxis);
		}else{
			goto BREAKDRAW;
		}
		
		
		//Y
// 		y = pPanel->rcWave.top +  (NWAVEMAX_SPO2-iValue)/pPanel->fYAxis;
		if(y > pPanel->rcWave.bottom-2) y = pPanel->rcWave.bottom-2;
		if(y<pPanel->rcWave.top) y = pPanel->rcWave.top;
		
		//X
		if(x < pPanel->rcWave.right-14){
			x = pPanel->rcWave.left + (float)(iWaveCount) / (float)(pPanel->fXAxis);
			iWaveCount ++;
		}
		else{
			x = pPanel->rcWave.left;
			iWaveCount = 0;
			MoveTo(hdc, x, y);
			MoveTo(mdc, x, y);
			x1 = x;
			y1 = y;
			//画扫描棒
			FillBox(hdc, x, pPanel->rcWave.top, iPointCount+4, RECTH(pPanel->rcWave));
			FillBox(mdc, x, pPanel->rcWave.top, iPointCount+4, RECTH(pPanel->rcWave));
		}

		if(x <=  pPanel->rcWave.left+1) {
			MoveTo(hdc, x, y);
			MoveTo(mdc, x, y);
			continue;
		}
		
		
			LineTo(hdc, x, y);
			LineTo(mdc, x, y);

		BREAKDRAW:
	;
	}
		
	//画扫描棒
		FillBox(hdc, 
			x+iPointCount, 
			pPanel->rcWave.top, 
			iPointCount, 
			RECTH(pPanel->rcWave));
		FillBox(mdc, 
			x+iPointCount, 
			pPanel->rcWave.top, 
			iPointCount, 
			RECTH(pPanel->rcWave));
 //	DrawScanBar(hdc,mdc,pPanel,x,iPointCount);

	return 0;
}

//更新参数
static int UpdateParaValue(HDC hdc)
{
	int i;
	for(i=0;i<PARA_COUNT;i++){
		switch(gParaPanels[i].bID){
			case PARA_HR:{
				DrawPara_HR(hdc);
			}break;
			case PARA_SPO2:{
				DrawPara_SPO2(hdc);
			}break;
			case PARA_PR:{
				DrawPara_PR(hdc);
			}break;
			case PARA_RR:{
				DrawPara_RR(hdc);
			}break;
			case PARA_T1:{
				DrawPara_T1(hdc);
			}break;
			case PARA_T2:{
				DrawPara_T2(hdc);
			}break;
			case PARA_NIBP:{
				DrawPara_NIBP(hdc);		
			}break;
		}
	}
	return 0;
}
//////////////////////////////////

static int InitOtherBedAttr()
{
	int i;
	int ParaLength;
	GetClientRect(ghWndPanel_OtherBed, &gRCOtherBed);

	//按键区域
//	SetRect(&gRCbtn, gRCOtherBed.left-1, gRCOtherBed.top, gRCOtherBed.right+5, gRCOtherBed.top+30);
	SetRect(&gRCWave, gRCOtherBed.left+2, gRCOtherBed.top, gRCOtherBed.right-2, gRCOtherBed.bottom-50);

	SetRect(&gRCParaValue, gRCOtherBed.left-1, gRCOtherBed.bottom-50, gRCOtherBed.right+5, gRCOtherBed.bottom);
	ParaLength=(gRCParaValue.right-gRCParaValue.left)/PARA_COUNT;
	for(i=0;i<PARA_COUNT;i++){
		gParaPanels[i].bID=i;
		SetRect(&gParaPanels[i].rcView, gRCParaValue.left+ParaLength*i, gRCParaValue.top, gRCParaValue.left+ParaLength*(i+1), gRCParaValue.bottom);
	}
	gParaPanels[0].iColor=gCfgEcg.iColor;
	gParaPanels[1].iColor=gCfgSpO2.iColor;
	gParaPanels[2].iColor=gCfgSpO2.iColor;
	gParaPanels[3].iColor=gCfgResp.iColor;
	gParaPanels[4].iColor=gCfgTemp.iColor;
	gParaPanels[5].iColor=gCfgTemp.iColor;
	gParaPanels[6].iColor=gCfgNibp.iColor;
	
	return 0;
}
static int InitOtherBedWave()
{

	//绘图区域同上面波形区域
	SetRect(&(gNWavePanelsOtherBed.rcWave), 
			  gRCWave.left,  
			  gRCWave.top+2, 
			  gRCWave.right, 
			 gRCWave.bottom);	//参数区域

	SetWavePanelXY_OB(&gNWavePanelsOtherBed);	
	gbOtherBedResetWaves=TRUE;
	gbNCanDrawWave_OB=TRUE;
	return 0;
}
//绘制趋势图
 int DrawOtherBed()
{
	int i;
	int ParaLength;
	
	if((gCfgSystem.bInterface == SCREEN_OTHERBED)  && (hdc_OtherBed!= (HDC)NULL) && (mdc_OtherBed!= (HDC)NULL)) {
		
		//绘制参数
		UpdateParaValue(mdc_OtherBed);
		
		//拷贝DC
		if((gCfgSystem.bInterface == SCREEN_OTHERBED)  && (hdc_OtherBed!= (HDC)NULL)) 
			BitBlt(mdc_OtherBed, 
				gRCOtherBed.left, gRCOtherBed.top, RECTW(gRCOtherBed)+1, RECTH(gRCOtherBed)+1, 
				hdc_OtherBed, 
				gRCOtherBed.left, gRCOtherBed.top, 
				0);
		else
			if(B_PRINTF) printf("-----------------------------------------------%s:%d  No OtherBed hdc, bitblt failed.\n", __FILE__, __LINE__);

	}	
	else
		if(B_PRINTF) printf("-----------------------------------------------%s:%d  No OtherBed hdc, bitblt failed.\n", __FILE__, __LINE__);
	
	
	return 0;
}

static int DrawOtherBedWave()
{
	static int iCount_OB=0;
	static BOOL Flash=TRUE;
//	if(B_PRINTF) printf("gbOtherBedWaveIndex=%d\n",gbOtherBedWaveIndex);
	
	
 	switch(gbOtherBedWaveIndex){
		case 0://ecg I
		case 1://ecg II
		case 2://ecg III
		case 3://ecg AVR
		case 4://ecg AVL
		case 5://ecg AVF
		case 6:{//ecg V
		if(iCount_OB%6)
			DrawWave_Ecg(hdc_OtherBed, mdc_OtherBed, &gNWavePanelsOtherBed, DRAWWAVENUM_200HZ+1);
		else
			DrawWave_Ecg(hdc_OtherBed, mdc_OtherBed, &gNWavePanelsOtherBed, DRAWWAVENUM_200HZ+2);
	
//		DrawWave_Ecg(hdc_OtherBed, mdc_OtherBed, &gNWavePanelsOtherBed, 4);
		}break;
		case 7:{//ecg SPO2
		if(iCount_OB%6)
			DrawWave_SpO2(hdc_OtherBed, mdc_OtherBed, &gNWavePanelsOtherBed, DRAWWAVENUM_200HZ+1);
		else
			DrawWave_SpO2(hdc_OtherBed, mdc_OtherBed, &gNWavePanelsOtherBed, DRAWWAVENUM_200HZ+2);

		}break;
		case 8:{//ecg RESP
		if(iCount_OB%6)
			DrawWave_Resp(hdc_OtherBed, mdc_OtherBed, &gNWavePanelsOtherBed, DRAWWAVENUM_200HZ+1);
		else
			DrawWave_Resp(hdc_OtherBed, mdc_OtherBed, &gNWavePanelsOtherBed, DRAWWAVENUM_200HZ+2);
		}break;
		default:{//ecg I
		if(iCount_OB%6)
			DrawWave_Ecg(hdc_OtherBed, mdc_OtherBed, &gNWavePanelsOtherBed, DRAWWAVENUM_200HZ+1);
		else
			DrawWave_Ecg(hdc_OtherBed, mdc_OtherBed, &gNWavePanelsOtherBed, DRAWWAVENUM_200HZ+2);
		
		}break;

	}
	Flash=!Flash;
	//画边框
	iCount_OB=(iCount_OB+1)%50;
	if(iCount_OB==0)
		DrawRectangle();
	
	gbOtherBedResetWaves = FALSE;
//		gbResetWaves=FALSE;
	return 0;
}
//绘图线程
static void *ThreadProcOtherBed(void *arg)
{
	
	for(;;)
	{
		sem_wait(&semOtherBed);
			gbDrawOtherBedFinished  = FALSE;
			
			;//DrawOtherBed();
			gbDrawOtherBedFinished  = TRUE;
			
	}

	if(B_PRINTF) printf("%s:%d Cancel OtherBed thread.\n", __FILE__, __LINE__);
	pthread_exit("Cancel OtherBed thread.\n");
}

//绘图波形线程
static void *ThreadProcOtherBedWave(void *arg)
{
	
	for(;;)
	{
		sem_wait(&semOtherBedWave);
		gbDrawWaveOtherBedFinished  = FALSE;
		if(gbNCanDrawWave){
			//绘制波形
			DrawOtherBedWave();
		}
		gbDrawWaveOtherBedFinished  = TRUE;
	}

	if(B_PRINTF) printf("%s:%d Cancel OtherBed Wave thread.\n", __FILE__, __LINE__);
	pthread_exit("Cancel OtherBed Wave thread.\n");
}

//销毁HDC
int NewDestroyOtherBed()
{
	
//	while(!gbDrawOtherBedFinished||!gbDrawWaveOtherBedFinished) {
	while(!gbDrawWaveOtherBedFinished) {
		if(B_PRINTF) printf("------------------------------------------------------%s:%d OtherBed not draw finished.\n", __FILE__, __LINE__);
		sleep(1);
	}
		gbNCanDrawWave_OB=FALSE;
		if(mdc_OtherBed != (HDC)NULL){
			DeleteCompatibleDC(mdc_OtherBed);
			mdc_OtherBed= (HDC)NULL;
			if(B_PRINTF) printf("--------------------Destroy OtherBed mdc.\n");
		}
		
		if(hdc_OtherBed!= (HDC)NULL){
			ReleaseDC(hdc_OtherBed);
			hdc_OtherBed= (HDC)NULL;
			if(B_PRINTF) printf("--------------------Destroy OtherBed hdc.\n");
		}
	
	gbDrawOtherBedFinished = TRUE;

	return 0;
}
int OtherBedShowBtn()
{
	
	if(btnBedNO!= (HWND)NULL){	
		UpdateWindow(btnBedNO,TRUE);
	}
	if(btnWave!= (HWND)NULL){	
		UpdateWindow(btnWave,TRUE);
	}
	if(cobBedNO!= (HWND)NULL){
		UpdateWindow(cobBedNO,TRUE);
	}
	if(cobWave!= (HWND)NULL){
		UpdateWindow(cobWave,TRUE);
	}
	
	return 0;
}
//复位
int OB_ResetDraw()
{
	//清空绘图缓冲
	NetClearData_Ecg1();
	NetClearData_Resp();
	NetClearData_SpO2();

	memset(&gOtherBedData, 0, sizeof gOtherBedData);
	
	gbOtherBedResetWaves=TRUE;
	
	return 0;
}

//初始化面板
int NewInitOtherBed()
{
	//显示面板
	ShowWindow(ghWndPanel_OtherBed, SW_SHOW);
	
	hdc_OtherBed = mdc_OtherBed = (HDC)NULL;
	//建立绘图memDC
	if(hdc_OtherBed == (HDC)NULL){
  		hdc_OtherBed = GetClientDC(ghWndPanel_OtherBed);
		
		if(hdc_OtherBed == (HDC)NULL) return -1;
		SetPenColor(hdc_OtherBed, BED_PENCOLOR);
		SetBkMode(hdc_OtherBed, BM_TRANSPARENT);
		SetBkColor(hdc_OtherBed, BED_BKCOLOR);
		SetBrushColor(hdc_OtherBed, BED_BKCOLOR);
		SetTextColor(hdc_OtherBed, BED_TEXTCOLOR);
 		mdc_OtherBed = CreateCompatibleDC(hdc_OtherBed);
		
		if(B_PRINTF) printf("%s:%d create hdc of OtherBed OK!\n", __FILE__, __LINE__);
	}else{
		if(B_PRINTF) printf("%s:%d Hdc of OtherBed had created.\n", __FILE__, __LINE__);
	}

	//初始化区域
	InitOtherBedAttr();
	
	//清空绘图缓冲
	NetClearData_Ecg1();
	NetClearData_Resp();
	NetClearData_SpO2();
	
	InitOtherBedWave();
	
	/*
	//建立参数绘图线程
	if(!gbThreadHasCreated_OtherBed_Para){
		sem_init(&semOtherBed, 0, 0);
		pthread_create(&ptOtherBed, NULL, ThreadProcOtherBed, NULL);	
		if(B_PRINTF) printf("%s:%d create thread of drawing OtherBed OK!\n", __FILE__, __LINE__);
		usleep(10000);
		gbThreadHasCreated_OtherBed_Para = TRUE;
	}
	else{
		if(B_PRINTF) printf("%s:%d Thread of drawing OtherBed has created!\n", __FILE__, __LINE__);
	}
	*/
	
	//建立波形绘图线程
	if(!gbThreadHasCreated_OtherBed_Wave){
		sem_init(&semOtherBedWave, 0, 0);
		pthread_create(&ptOtherBedWave, NULL, ThreadProcOtherBedWave, NULL);	
		if(B_PRINTF) printf("%s:%d create thread of drawing OtherBed Wave OK!\n", __FILE__, __LINE__);
		usleep(10000);
		gbThreadHasCreated_OtherBed_Wave = TRUE;
	}
	else{
		if(B_PRINTF) printf("%s:%d Thread of drawing OtherBed Wave has created!\n", __FILE__, __LINE__);
	}
	return 0;
}

int NewUpdateOtherBedWave()
{

	if(gbNCanDrawWave&&gbNCanDrawWave_OB){
		sem_post(&semOtherBedWave);
	} 
	else{
		return -1;
	} 
	
	return 0;
}

int NewUpdateOtherBed()
{

	sem_post(&semOtherBed);
			
	return 0;
	
}

 

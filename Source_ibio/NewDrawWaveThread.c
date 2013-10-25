/*************************************************************************** 
 *	Module Name:	NewDrawWaveThread
 *
 *	Abstract:	新的绘图主程序
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2008-01-22 09:05:27
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "General_Funcs.h"
#include "NewDrawWave.h"

static BOOL gbDebugRect = FALSE;


//波形区域的背景颜色
#define	WAVE_BKCOLOR	COLOR_black	

#define	PARA_TEXTCOLOR		COLOR_darkgray	
//ECG标尺宽度pixel
#define NECG_CAL_WIDTH		3

/*//基线
#define BASELINE_ECG		127
#define BASELINE_SPO2	50
#define BASELINE_RESP	127
//#define BASELINE_CO2	(WAVEMAX_CO2/2)
*/

//MemDC
HDC hdcWaveMem = (HDC)NULL;
//绘图环境
HDC hdcWave = (HDC)NULL;

//绘制波形线程
static pthread_t 	ptNDrawWave;				
//在创建和销毁面板时，控制绘图与否
BOOL gbCanDrawWave = FALSE;	
//线宽
static BYTE gbLineWidth = 1;	///1+xPixels

//ECG标尺的长度(像素, 供绘图时使用)
static int giEcgRulers[4] = {0, 0, 0, 0};
//ECG采样点对应屏幕像素的比例系数
 float gfEcgSampleToPixel = 0.0;
///1mv对应的采样点数
 float gEcgSamplesPerMV = 200;//50;
//ECG波形基线
 int giEcgBaseline = 0;


//波形数据
static S_ECG_VALUE		EcgWave1;
static S_ECG_VALUE		EcgWave2;
static S_SPO2_VALUE		SpO2Wave;
static S_RESP_VALUE		RespWave;
static S_CO2_VALUE		CO2Wave;
static S_IBP_VALUE		Ibp1Wave;
static S_IBP_VALUE		Ibp2Wave;


static unsigned char  strLabel[20]={0};
static unsigned char *strSpeed[]={"6.25mm/s", "12.5mm/s", "25mm/s", "50mm/s"};
//ECG
static unsigned char *strEcgLabel={"ECG"};
static unsigned char *strEcgGain[]={"1mv", "1mv", "1mV", "1mV", "1mV"};
static unsigned char *strEcgGainLb[]={"X0.25", "X0.5", "X1", "X2", "X4"};
static unsigned char *strEcgMode[]={"Diag", "Moni", "Oper"};
static unsigned char *strEcgLeads[]={"  I", " II", "III", "aVR", "aVL", "aVF", "V1", "V2", "V3", "V4", "V5", "V6"};
//SpO2
static unsigned char *strSpO2Label={"Pleth"};
static unsigned char *strSpO2Gain[]={"AUTO", "X0.5", "X1", "X2"};
//Resp
static unsigned char *strRespLabel={"Resp"};
static unsigned char *strRespGain[]={"X0.25","X0.5", "X1", "X2", "X4", "X8", "X16", "X32"};
//CO2
static unsigned char *strCO2Label={"CO2"};
static unsigned char *strCO2Unit[]={"mmHg", "%"};
static unsigned char strCO2High[10][10][10]={
	{"50 ", "75 ", "99 "}, 
	{"6.6 ", "9.9 ", "13.0 "}
};
static unsigned char strCO2Low[10][10][10]={
	{"0 ", "0 ", "0 "}, 
	{"0.0 ", "0.0 ", "0.0 "}
};
//IBP
static unsigned char *strIbpLabel[]={"ART", "PA",  "CVP", "LAP", "RAP", "ICP",  "P1", "P2"};
static unsigned char *strIbpUnit[]={"mmHg", "kPa", "cmH2O"};
unsigned char  strIbpTop[6]={0};
unsigned char  strIbpBottom[6]={0};
static unsigned char  strIbp1tmp[6]={0},strIbp2tmp[6]={0},strIbp1tmp2[6]={0},strIbp2tmp2[6]={0};

//控制是否冻结波形
static BOOL gbFreeze = FALSE;

//起搏信号的显示标志
static unsigned char *strPaceSignal = "v";

//刷新参数标志位
static int iOldChannel1=0,iOldChannel2=0,iOldMode=0,iGain=0,iECG2OldGain=0;
static int iMulOldGain=0,iMulOldMode=0,iSpo2Gain=0,iRROldGain=0;
static int Ibp1unit=0,Ibp2unit=0,Ibp1bLabel,Ibp2bLabel;
///////////////////// extern /////////////////
//主窗口句柄
extern HWND ghWndMain;

///1mm对应像素的点数
extern float gfNMMToPixel;

//控制波形绘制的信号量
extern sem_t	semNDrawWave;

//控制是否可以进行绘制的变量
extern BOOL gbNCanDrawWave;

//波形复位控制
extern BOOL gbResetWaves;

//波形面板
extern NWAVE_PANEL gNWavePanels[NWAVEPANEL_MAX_COUNT];

//波形面板的数量
extern int giPanelCount;

//波形的显示区域(相对于参数，信息等显示区域, 在该区域内，分配各面板的显示区域)
extern RECT gNRcWaves;

extern BOOL gbHavePack;
////////////////////////////////////////////

/*
	计算ECG标尺的长度(像素)
	线程开始时调用
*/
 int CalcEcgRuler()
{
	
	//0.25x
	giEcgRulers[0] = (int)(2.5 * gfNMMToPixel);
	//0.5x
	giEcgRulers[1] = (int)(5.0 * gfNMMToPixel);
	///1x
	giEcgRulers[2] = (int)(10.0 * gfNMMToPixel);
	///2x
	giEcgRulers[3] = (int)(20.0 * gfNMMToPixel);
	
	//Y轴系数
//	gfEcgSampleToPixel = (float)(((float)(gEcgSamplesPerMV))/((float)20.0*gfNMMToPixel));
	gfEcgSampleToPixel = (float)(((float)(gEcgSamplesPerMV))/(float)giEcgRulers[gCfgEcg.bGain]);
			
			
	//计算ECG基线
	giEcgBaseline  = BASELINE_ECG / gfEcgSampleToPixel;
	
	
	return 0;
}

//画Ecg标尺
static int DrawRuler(const PNWAVE_PANEL pPanel,int iRulerLen,int i)
{
	
	Rectangle(hdcWaveMem, 
			  pPanel->rcPara.left+5+30, 
			  pPanel->rcPara.top+(RECTH(pPanel->rcPara)/(pPanel->bWaveCount*2))*(i*2+1)-iRulerLen/2,
			  pPanel->rcPara.left+10+30, 
			  pPanel->rcPara.top+(RECTH(pPanel->rcPara)/(pPanel->bWaveCount*2))*(i*2+1)-iRulerLen/2+iRulerLen);
	Rectangle(hdcWaveMem, 
			  pPanel->rcPara.left+30, 
			  pPanel->rcPara.top+(RECTH(pPanel->rcPara)/(pPanel->bWaveCount*2))*(i*2+1)-iRulerLen/2+iRulerLen,
			  pPanel->rcPara.left+15+30, 
			  pPanel->rcPara.top+(RECTH(pPanel->rcPara)/(pPanel->bWaveCount*2))*(i*2+1)-iRulerLen/2+iRulerLen);

	
	return 0;
}
//绘制Demo字样
int DrawDemo(HDC hdc, HDC mdc)
{
 //	gIconDemo.bmAlpha =  0;		
	FillBoxWithBitmap(hdcWaveMem, 100,45, 175, 42, &gIconDemo);
	FillBoxWithBitmap(hdcWave, 100,45, 175, 42, &gIconDemo);
	return 0;
}

// int FillWaveArea(HDC hdc, HDC mdc, RECT rc)
int FillWaveArea(RECT rc)
{
	int i;

 	SetBrushColor(hdcWaveMem, COLOR_black);
	SetBrushColor(hdcWave, COLOR_black);
	FillBox(hdcWaveMem, 
		rc.left, 
		rc.top, 
		RECTW(rc)+1, 
		RECTH(rc)+1);
	FillBox(hdcWave, 
		rc.left, 
		rc.top, 
		RECTW(rc)+1, 
		RECTH(rc)+1);
	
	BitBlt(hdcWaveMem, 
	       rc.left, rc.top, RECTW(rc)+1, RECTH(rc)+1, 
	       hdcWave, 
	       rc.left, rc.top, 
	       0);
	
	if(B_PRINTF) printf("---------------Fill %d %d %d %d \n", rc.left, rc.top, rc.right, rc.bottom);
	
	return 0;
}


//绘制起搏信号
static int DrawPaceSignal(HDC hdc, int x, int y)
{
	if(gCfgEcg.bPacemaker != PACEMAKER_OFF && hdc != (HDC)NULL){
	//	SetTextColor(hdc, COLOR_darkgreen);
	//	TextOut(hdc, x, y, strPaceSignal);
	//	SetTextColor(hdc, gCfgEcg.iColor);
 //		printf("TextOut Pace signal\n");
	 SetPenColor(hdc, ~gCfgEcg.iColor);
 		MoveTo(hdc, x, y);
		LineTo(hdc,x,y+5);
	SetPenColor(hdc, gCfgEcg.iColor);	
	
		return 0;
	}
	return -1;
}
int DrawScanBar(HDC hdc,HDC mdc,PNWAVE_PANEL pPanel,int x,int iPointCount)
{
	
	if(x <pPanel->rcWave.right-iPointCount){
		FillBox(hdc, x+iPointCount, pPanel->rcWave.top, iPointCount, RECTH(pPanel->rcWave));
		FillBox(mdc, x+iPointCount, pPanel->rcWave.top, iPointCount, RECTH(pPanel->rcWave));
	}else{
		FillBox(hdc, x, pPanel->rcWave.top, iPointCount, RECTH(pPanel->rcWave));
		FillBox(mdc, x, pPanel->rcWave.top,iPointCount, RECTH(pPanel->rcWave));
	}
	return 0;
}
static int DrawRuler_Ecg(const PNWAVE_PANEL pPanel)
{
	int iRulerLen = 0;	
	int i;
	static int EcgGain=0;
	if(EcgGain!=gCfgEcg.bGain){
		SetPenColor(hdcWaveMem, COLOR_black);
		iRulerLen = giEcgRulers[EcgGain];
		for(i=0; i<pPanel->bWaveCount; i++){
			DrawRuler(pPanel, iRulerLen,i);
		}	
	}else{
		SetPenColor(hdcWaveMem, PARATEXTCOLOR);
		iRulerLen = giEcgRulers[gCfgEcg.bGain];
		for(i=0; i<pPanel->bWaveCount; i++){
			DrawRuler(pPanel, iRulerLen,i);
		}
	}
	EcgGain=gCfgEcg.bGain;
	return 0;
}

static int DrawRuler_Ecg2(const PNWAVE_PANEL pPanel)
{
	int iRulerLen = 0;	
	int i;
	static int EcgGain=0;
	if(EcgGain!=gCfgEcg.bGain){
		SetPenColor(hdcWaveMem, COLOR_black);
		iRulerLen = giEcgRulers[EcgGain];
		for(i=0; i<pPanel->bWaveCount; i++){
			DrawRuler(pPanel, iRulerLen,i);
		}	
	}else{
		SetPenColor(hdcWaveMem, PARATEXTCOLOR);
		iRulerLen = giEcgRulers[gCfgEcg.bGain];
		for(i=0; i<pPanel->bWaveCount; i++){
			DrawRuler(pPanel, iRulerLen,i);
		}
	}
	EcgGain=gCfgEcg.bGain;
	return 0;
}

//ECG1 Paras
static int DrawParas_Ecg1(const PNWAVE_PANEL pPanel)
{
	int iRulerLen = 0;		//标尺长度	
	int i=0;
/*
	//Clear Area
	FillBox(hdcWaveMem, 
		pPanel->rcPara.left, 
		pPanel->rcPara.top, 
		RECTW(pPanel->rcPara)+1, 
		RECTH(pPanel->rcPara)+1);
*/
	SetTextColor(hdcWaveMem, COLOR_black);
//	SetPenColor(hdcWaveMem, COLOR_black);
	if(iOldChannel1!=gCfgEcg.bChannel1)
		TextOut(hdcWaveMem, pPanel->rcPara.left, pPanel->rcPara.top+1, strEcgLeads[iOldChannel1]);
	if(iOldMode!=gCfgEcg.bMode)
		TextOut(hdcWaveMem, pPanel->rcPara.left+(PARAOFWAVE_WIDTH/5)*2, pPanel->rcPara.top+1, strEcgMode[iOldMode]);
	if(iGain!=gCfgEcg.bGain){
		TextOut(hdcWaveMem, pPanel->rcPara.left+(PARAOFWAVE_WIDTH/5)*4, pPanel->rcPara.top+1, strEcgGainLb[iGain]);
		//TextOut(hdcWaveMem, pPanel->rcPara.left+15, pPanel->rcPara.top+10,strEcgGain[iGain]);
	iRulerLen = giEcgRulers[iGain];
	TextOut(hdcWaveMem, pPanel->rcPara.left+15+30,  pPanel->rcPara.top+(RECTH(pPanel->rcPara)/(pPanel->bWaveCount*2))*(i*2+1)-iRulerLen/2,strEcgGain[iGain]);

		/*
	
		for(i=0; i<pPanel->bWaveCount; i++){
			//标尺
		DrawRuler(pPanel, iRulerLen,i);
		TextOut(hdcWaveMem, pPanel->rcPara.left+15+30,  pPanel->rcPara.top+(RECTH(pPanel->rcPara)/(pPanel->bWaveCount*2))*(i*2+1)-iRulerLen/2-10,strEcgGain[iGain]);
		}	
		*/

	}

	

	//SetTextColor(hdcWaveMem, gCfgEcg.iColor); 
	
//	if(B_PRINTF) printf("----------color [%d]-----------\n",MyColor);
	SetTextColor(hdcWaveMem,  PARATEXTCOLOR); 

	//显示文字
	//Label
 	//TextOut(hdcWaveMem, pPanel->rcPara.left+4, pPanel->rcPara.top+1, strEcgLabel);
	//Lead
	TextOut(hdcWaveMem, pPanel->rcPara.left, pPanel->rcPara.top+1, strEcgLeads[gCfgEcg.bChannel1]);
	//Mode
	TextOut(hdcWaveMem, pPanel->rcPara.left+(PARAOFWAVE_WIDTH/5)*2, pPanel->rcPara.top+1, strEcgMode[gCfgEcg.bMode]);
	//Gain
	TextOut(hdcWaveMem, pPanel->rcPara.left+(PARAOFWAVE_WIDTH/5)*4, pPanel->rcPara.top+1, strEcgGainLb[gCfgEcg.bGain]);
	
	//TextOut(hdcWaveMem, pPanel->rcPara.left+15, pPanel->rcPara.top+10,strEcgGain[gCfgEcg.bGain]);

	iRulerLen = giEcgRulers[gCfgEcg.bGain];
	TextOut(hdcWaveMem, pPanel->rcPara.left+15+30,  pPanel->rcPara.top+(RECTH(pPanel->rcPara)/(pPanel->bWaveCount*2))*(i*2+1)-iRulerLen/2,strEcgGain[gCfgEcg.bGain]);
/*
	//计算标尺 
	
	
	iRulerLen = giEcgRulers[gCfgEcg.bGain];
	for(i=0; i<pPanel->bWaveCount; i++){
		DrawRuler(pPanel, iRulerLen,i);
		TextOut(hdcWaveMem, pPanel->rcPara.left+15+30,  pPanel->rcPara.top+(RECTH(pPanel->rcPara)/(pPanel->bWaveCount*2))*(i*2+1)-iRulerLen/2-10,strEcgGain[gCfgEcg.bGain]);
	}	
	*/
// 	SetBrushColor(hdcWaveMem, WAVE_BKCOLOR);

	if(gbDebugRect) 
		Rectangle(hdcWaveMem, 
			  pPanel->rcPara.left, 
			  pPanel->rcPara.top, 
			  pPanel->rcPara.right, 
			  pPanel->rcPara.bottom);

	
	BitBlt(hdcWaveMem, 
	       pPanel->rcPara.left, pPanel->rcPara.top, RECTW(pPanel->rcPara)+1, RECTH(pPanel->rcPara)+1, 
	       hdcWave, 
	       pPanel->rcPara.left, pPanel->rcPara.top, 
	       0);
	
	 iOldChannel1=gCfgEcg.bChannel1;
	 iOldMode=gCfgEcg.bMode;
	 iGain=gCfgEcg.bGain;
	
	return 0;
}

//绘制Ecg1波形
// DrawWave_Ecg1(hdcWave, hdcWaveMem, &(gNWavePanels[i]), DRAWWAVENUM_200HZ);
extern int gbHaveBeep_QRS;
static int DrawWave_Ecg1(HDC hdc, HDC mdc, const PNWAVE_PANEL pPanel, int iPointCount)
{	
	static int x = 0;
	static int y = 0;
	static int iWaveCount = 0;
	static int iValue;
	BOOL res;
	BOOL havepace=FALSE;
	int i, j;
	int x1, y1;
	static BOOL bNext = FALSE;	//级联的波形
	int iLinkMove = 0;				//级联波形需要移动距离
	
	//开始绘制标志 ，初始化X，Y坐标 
	if(gbResetWaves){
		x = pPanel->rcWave.left;
		y = pPanel->rcWave.top + RECTH(pPanel->rcWave)/2;
		iWaveCount = 0;
		FillBox(hdc, 
			pPanel->rcWave.left, 
			pPanel->rcWave.top, 
			RECTW(pPanel->rcWave)+1, 
			RECTH(pPanel->rcWave)+1);
		FillBox(mdc, 
			pPanel->rcWave.left, 
			pPanel->rcWave.top, 
			RECTW(pPanel->rcWave)+1, 
			RECTH(pPanel->rcWave)+1);
		//??????í??o3?
		ClearData_Ecg1();
	}
	
	//波形以及文字颜色根据配置而定
	SetPenColor(hdc, gCfgEcg.iColor);	
	SetPenColor(mdc, gCfgEcg.iColor);	

	if(pPanel->bCascade == CASCADE_ON){
		//波形级联
		for(j=0; j<iPointCount; j++){
			//画主波形和其级联波形
			x1 = x;
			y1 = y;	
			MoveTo(hdc, x, y);
			MoveTo(mdc, x, y);
			
			if(!bNext){
//     				SetBrushColor(hdcWave, COLOR_cyan);
				iLinkMove = 0;
			}
			else{
//     				SetBrushColor(hdcWave, COLOR_yellow);
				iLinkMove = RECTH(pPanel->rcWave)/pPanel->bWaveCount;
			}
			/////如果没有数据上来时 ，绘制中线
			y = pPanel->rcWave.top 
						+  (RECTH(pPanel->rcWave)/2 - BASELINE_ECG/(gfEcgSampleToPixel)) 
						+ (giEcgBaseline - RECTH(pPanel->rcWave)/(pPanel->bWaveCount)/2)
 						+ iLinkMove;
			///////		
			res = RetrieveData_Ecg1(&EcgWave1);
			
			if(res){
				iValue = EcgWave1.iWaves[gCfgEcg.bChannel1];
				if(EcgWave1.bPacingpulse==1)havepace=TRUE;
				
				//0.25增益下，缩小2/1
			//	if(gCfgEcg.bGain == ECGGAIN_025X){
			//		iValue = (iValue - BASELINE_ECG)/2 + BASELINE_ECG;
			//	}
				y = pPanel->rcWave.top 
						+  (RECTH(pPanel->rcWave)/2 - iValue/(gfEcgSampleToPixel)) 
						+ (giEcgBaseline - RECTH(pPanel->rcWave)/(pPanel->bWaveCount)/2)
 						+ iLinkMove;
				
			}
			
			if(!bNext){
 				if(y>pPanel->rcWave.top+RECTH(pPanel->rcWave)/pPanel->bWaveCount-2) 
					y = pPanel->rcWave.top+RECTH(pPanel->rcWave)/pPanel->bWaveCount-2;
 				if(y<pPanel->rcWave.top+1) y = pPanel->rcWave.top+1;
 			}
 			else{
 				if(y<pPanel->rcWave.top+RECTH(pPanel->rcWave)/pPanel->bWaveCount+1) 
					y = pPanel->rcWave.top+RECTH(pPanel->rcWave)/pPanel->bWaveCount + 1;
 				if(y>pPanel->rcWave.bottom-1) y = pPanel->rcWave.bottom-1;
 			}
			
			//X
			if(x < pPanel->rcWave.right){
				x = pPanel->rcWave.left + (float)(iWaveCount) / (float)(pPanel->fXAxis);
				iWaveCount ++;

				if(gCfgSystem.bLineType == LINE_NORMAL){
					if(x <=  pPanel->rcWave.left+1) {
						MoveTo(hdc, x, y);
						MoveTo(mdc, x, y);
						continue;
					}
					LineTo(hdc, x, y);
					LineTo(mdc, x, y);
				}
			}
			else{
				x = pPanel->rcWave.left;
				iWaveCount = 0;
				MoveTo(hdc, x, y);
				MoveTo(mdc, x, y);
				x1 = x;
//  				y1 = 0;
// 				if(bNext){
				y1 = pPanel->rcWave.top + RECTH(pPanel->rcWave)/2;
// 				}
				
				//画扫描棒
				FillBox(hdc, 
					pPanel->rcWave.left, 
					pPanel->rcWave.top, 
					FILLWAVE_WIDTH, 
					RECTH(pPanel->rcWave)+1);
				FillBox(mdc, 
					pPanel->rcWave.left, 
					pPanel->rcWave.top, 
					FILLWAVE_WIDTH, 
					RECTH(pPanel->rcWave)+1);
				
				bNext = !bNext;
			}
			
			if(x <=  pPanel->rcWave.left+1) {
				MoveTo(hdc, x, y);
				MoveTo(mdc, x, y);
				continue;
			}
			
			if(gCfgSystem.bLineType != LINE_NORMAL){
				DrawWideLine(hdc, REALWIDELINE, gbLineWidth, x1, y1, x, y, WIDELINE_DOT);
				DrawWideLine(mdc, REALWIDELINE, gbLineWidth, x1, y1, x, y, WIDELINE_DOT);
			}
		}
		
		//扫描棒
 		if(x <= pPanel->rcWave.left+iPointCount){
			//扫描棒
			FillBox(hdc, 
				pPanel->rcWave.left, 
				pPanel->rcWave.top, 
				FILLWAVE_WIDTH, 
				RECTH(pPanel->rcWave)+1);
			FillBox(mdc, 
				pPanel->rcWave.left, 
				pPanel->rcWave.top, 
				FILLWAVE_WIDTH, 
				RECTH(pPanel->rcWave)+1);
		}
		FillBox(hdc, 
			x+iPointCount, 
			pPanel->rcWave.top+iLinkMove, 
			FILLWAVE_WIDTH, 
			RECTH(pPanel->rcWave)/pPanel->bWaveCount+1);
		FillBox(mdc, 
			x+iPointCount, 
			pPanel->rcWave.top+iLinkMove, 
			FILLWAVE_WIDTH, 
			RECTH(pPanel->rcWave)/pPanel->bWaveCount+1);
		
		//绘制起搏信号
//		if(EcgWave1.bPacingpulse == 1){

	if(havepace){
			if(!bNext){
				DrawPaceSignal(hdc, x, pPanel->rcWave.top) ;
				DrawPaceSignal(mdc, x, pPanel->rcWave.top) ;
			}
			else{
				DrawPaceSignal(hdc, x, pPanel->rcWave.top+RECTH(pPanel->rcWave)/2) ;
				DrawPaceSignal(mdc, x, pPanel->rcWave.top+RECTH(pPanel->rcWave)/2) ;
			}
		//	gbHavePack=FALSE;
		}
	}
	else{
		x1 =x;
		y1 =y;
		MoveTo(hdc, x, y);
		MoveTo(mdc, x, y);
		
		for(j=0; j<iPointCount; j++){
			//获得波形值
			res = RetrieveData_Ecg1(&EcgWave1); 
			if(res){

				iValue = EcgWave1.iWaves[gCfgEcg.bChannel1]; 
				if(EcgWave1.bPacingpulse==1)havepace=TRUE;
			
			
				//0.25增益下，缩小2/1
		//		if(gCfgEcg.bGain == ECGGAIN_025X){
		//			iValue = (iValue - BASELINE_ECG)/2 + BASELINE_ECG;
		//		}
 				y = pPanel->rcWave.top 
						+  RECTH(pPanel->rcWave) - iValue/(gfEcgSampleToPixel) 
						+ (giEcgBaseline - RECTH(pPanel->rcWave)/2);
			}
			
			//Y
			if(y > pPanel->rcWave.bottom-2) y = pPanel->rcWave.bottom-2;
			if(y<pPanel->rcWave.top) y = pPanel->rcWave.top;
		
			//X
			if(x < pPanel->rcWave.right){
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
					iPointCount, 
					RECTH(pPanel->rcWave)+1);
				FillBox(mdc, 
					x, 
					pPanel->rcWave.top, 
					iPointCount, 
					RECTH(pPanel->rcWave)+1);
			}

			if(x <=  pPanel->rcWave.left+1) {
				MoveTo(hdc, x, y);
				MoveTo(mdc, x, y);
				continue;
			}
			
			if(gCfgSystem.bLineType == LINE_ANTI){
				DrawWuLine(hdc, x1, y1, x, y, COLOR_magenta);
				DrawWuLine(mdc, x1, y1, x, y, COLOR_magenta);
			}else if(gCfgSystem.bLineType == LINE_WIDE ){
 				DrawWideLine(hdc, REALWIDELINE, gbLineWidth, x1, y1, x, y, WIDELINE_DOT);
				DrawWideLine(mdc, REALWIDELINE, gbLineWidth, x1, y1, x, y, WIDELINE_DOT);
			}else{
				LineTo(hdc, x, y);
				LineTo(mdc, x, y);
			}
		}

		//画扫描棒
		FillBox(hdc, 
			x+iPointCount, 
			pPanel->rcWave.top, 
			FILLWAVE_WIDTH, 
			RECTH(pPanel->rcWave)+1);
		FillBox(mdc, 
			x+iPointCount, 
			pPanel->rcWave.top, 
			FILLWAVE_WIDTH, 
			RECTH(pPanel->rcWave)+1);
		//绘制起搏信号
	//	if(EcgWave1.bPacingpulse == 1){
	//	if(gbHavePack){
		if(havepace){
			printf("EcgWave1.bPacingpulse=%d\n",EcgWave1.bPacingpulse);	
		
			DrawPaceSignal(hdc, x, pPanel->rcWave.top) ;
			DrawPaceSignal(mdc, x, pPanel->rcWave.top) ;
		//	gbHavePack=FALSE;
		//	EcgWave1.bPacingpulse = 0;
			
		}
	}
	
//	DrawParas_Ecg1(pPanel);		
	DrawRuler_Ecg(pPanel);
	return 0;
}


//ECG2 Paras
static int DrawParas_Ecg2(const PNWAVE_PANEL pPanel)
{
	int iRulerLen = 0;		//标尺长度	
	int i;
	
	/*
	//Clear Area
	FillBox(hdcWaveMem, 
		pPanel->rcPara.left, 
		pPanel->rcPara.top, 
		RECTW(pPanel->rcPara)+1, 
		RECTH(pPanel->rcPara)+1);
	*/
	SetTextColor(hdcWaveMem, COLOR_black);
	if(iOldChannel2!=gCfgEcg.bChannel2)
		TextOut(hdcWaveMem, pPanel->rcPara.left, pPanel->rcPara.top+1, strEcgLeads[iOldChannel2]);

	
	SetTextColor(hdcWaveMem,PARATEXTCOLOR);
	
	//显示文字
	//Label
	//TextOut(hdcWaveMem, pPanel->rcPara.left+4, pPanel->rcPara.top+1, strEcgLabel);
	//Lead
	TextOut(hdcWaveMem, pPanel->rcPara.left, pPanel->rcPara.top+1, strEcgLeads[gCfgEcg.bChannel2]);
	
	//计算标尺 
/*
	if(iECG2OldGain!=gCfgEcg.bGain){
		iRulerLen = giEcgRulers[iECG2OldGain];
		SetPenColor(hdcWaveMem, COLOR_black);
		for(i=0; i<pPanel->bWaveCount; i++){
			DrawRuler(pPanel, iRulerLen,i);
		}	
	}
	
	iRulerLen = giEcgRulers[gCfgEcg.bGain];
	SetPenColor(hdcWaveMem, PARATEXTCOLOR);
	for(i=0; i<pPanel->bWaveCount; i++){
			DrawRuler(pPanel, iRulerLen,i);
		}	
*/
// 	SetBrushColor(hdcWaveMem, WAVE_BKCOLOR);
	
	if(gbDebugRect) 
		Rectangle(hdcWaveMem, 
			  pPanel->rcPara.left, 
			  pPanel->rcPara.top, 
			  pPanel->rcPara.right, 
			  pPanel->rcPara.bottom);
	
	//拷贝DC
	BitBlt(hdcWaveMem, 
	       pPanel->rcPara.left, pPanel->rcPara.top, RECTW(pPanel->rcPara)+1, RECTH(pPanel->rcPara)+1, 
	       hdcWave, 
	       pPanel->rcPara.left, pPanel->rcPara.top, 
	       0);
	
	iOldChannel2=gCfgEcg.bChannel2;
	iECG2OldGain=gCfgEcg.bGain;
	return 0;
}

//绘制Ecg2波形
static int DrawWave_Ecg2(HDC hdc, HDC mdc, const PNWAVE_PANEL pPanel, int iPointCount)
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
	if(gbResetWaves){
		x = pPanel->rcWave.left;
		y = pPanel->rcWave.top + RECTH(pPanel->rcWave)/2;
		iWaveCount = 0;
		FillBox(hdc, 
			pPanel->rcWave.left, 
			pPanel->rcWave.top, 
			RECTW(pPanel->rcWave)+1, 
			RECTH(pPanel->rcWave)+1);
		FillBox(mdc, 
			pPanel->rcWave.left, 
			pPanel->rcWave.top, 
			RECTW(pPanel->rcWave)+1, 
			RECTH(pPanel->rcWave)+1);
		//??????í??o3?
		ClearData_Ecg2();
	}
	
	//波形以及文字颜色根据配置而定
	SetPenColor(hdc, gCfgEcg.iColor);	
	SetPenColor(mdc, gCfgEcg.iColor);	

	if(pPanel->bCascade == CASCADE_ON){
		//波形级联
		for(j=0; j<iPointCount; j++){
			//画主波形和其级联波形
			MoveTo(hdc, x, y);
			MoveTo(mdc, x, y);
			x1 = x;
			y1 = y;	

			if(!bNext){
//     				SetBrushColor(hdcWave, COLOR_cyan);
				iLinkMove = 0;
			}
			else{
//     				SetBrushColor(hdcWave, COLOR_yellow);
				iLinkMove = RECTH(pPanel->rcWave)/pPanel->bWaveCount;
			}
						
			res = RetrieveData_Ecg2(&EcgWave2);
			if(res){
				iValue = EcgWave2.iWaves[gCfgEcg.bChannel2];
				//0.25增益下，缩小2/1
			//	if(gCfgEcg.bGain == ECGGAIN_025X){
			//		iValue = (iValue - BASELINE_ECG)/2 + BASELINE_ECG;
			//	}
				y = pPanel->rcWave.top 
						+  (RECTH(pPanel->rcWave)/2 - iValue/(gfEcgSampleToPixel)) 
						+ (giEcgBaseline - RECTH(pPanel->rcWave)/(pPanel->bWaveCount)/2)
						+ iLinkMove;
			}
			
			if(!bNext){
				if(y>pPanel->rcWave.top+RECTH(pPanel->rcWave)/pPanel->bWaveCount-2) 
					y = pPanel->rcWave.top+RECTH(pPanel->rcWave)/pPanel->bWaveCount-2;
				if(y<pPanel->rcWave.top+1) y = pPanel->rcWave.top+1;
			}
			else{
				if(y<pPanel->rcWave.top+RECTH(pPanel->rcWave)/pPanel->bWaveCount+1) 
					y = pPanel->rcWave.top+RECTH(pPanel->rcWave)/pPanel->bWaveCount + 1;
				if(y>pPanel->rcWave.bottom-1) y = pPanel->rcWave.bottom-1;
			}
			
			//X
			if(x < pPanel->rcWave.right){
				x = pPanel->rcWave.left + (float)(iWaveCount) / (float)(pPanel->fXAxis);
				iWaveCount ++;
				if(gCfgSystem.bLineType == LINE_NORMAL){
					LineTo(hdc, x, y);
					LineTo(mdc, x, y);
				}
			}
			else{
				x = pPanel->rcWave.left;
				iWaveCount = 0;
				MoveTo(hdc, x, y);
				MoveTo(mdc, x, y);
				x1 = x;
// 				y1 = 0;
				y1 = pPanel->rcWave.top + RECTH(pPanel->rcWave)/2;
				//画扫描棒
				FillBox(hdc, 
					pPanel->rcWave.left, 
					pPanel->rcWave.top, 
					FILLWAVE_WIDTH, 
					RECTH(pPanel->rcWave)+1);
				FillBox(mdc, 
					pPanel->rcWave.left, 
					pPanel->rcWave.top, 
					FILLWAVE_WIDTH, 
					RECTH(pPanel->rcWave)+1);
				
				bNext = !bNext;
			}
			if(gCfgSystem.bLineType != LINE_NORMAL){
				DrawWideLine(hdc, REALWIDELINE, gbLineWidth, x1, y1, x, y, WIDELINE_DOT);
				DrawWideLine(mdc, REALWIDELINE, gbLineWidth, x1, y1, x, y, WIDELINE_DOT);
			}
		}
		
		//扫描棒
		if(x <= pPanel->rcWave.left+iPointCount){
			//扫描棒
			FillBox(hdc, 
				pPanel->rcWave.left, 
				pPanel->rcWave.top, 
				FILLWAVE_WIDTH, 
				RECTH(pPanel->rcWave)+1);
			FillBox(mdc, 
				pPanel->rcWave.left, 
				pPanel->rcWave.top, 
				FILLWAVE_WIDTH, 
				RECTH(pPanel->rcWave)+1);
		}
		FillBox(hdc, 
			x+iPointCount, 
			pPanel->rcWave.top+iLinkMove, 
			FILLWAVE_WIDTH, 
			RECTH(pPanel->rcWave)/pPanel->bWaveCount+1);
		
		FillBox(mdc, 
			x+iPointCount, 
			pPanel->rcWave.top+iLinkMove, 
			FILLWAVE_WIDTH, 
			RECTH(pPanel->rcWave)/pPanel->bWaveCount+1);
	}
	else{
		x1 =x;
		y1 =y;
		MoveTo(hdc, x, y);
		MoveTo(mdc, x, y);
			
		for(j=0; j<iPointCount; j++){
			//获得波形值
			res = RetrieveData_Ecg2(&EcgWave2);
			if(res){
				iValue = EcgWave2.iWaves[gCfgEcg.bChannel2];
				//0.25增益下，缩小2/1
		//		if(gCfgEcg.bGain == ECGGAIN_025X){
		//			iValue = (iValue - BASELINE_ECG)/2 + BASELINE_ECG;
		//		}
				y = pPanel->rcWave.top 
						+  RECTH(pPanel->rcWave) - iValue/(gfEcgSampleToPixel) 
						+ (giEcgBaseline - RECTH(pPanel->rcWave)/2);
			}
			
			//Y
			if(y > pPanel->rcWave.bottom-2) y = pPanel->rcWave.bottom-2;
			if(y<pPanel->rcWave.top) y = pPanel->rcWave.top;
		
			//X
			if(x < pPanel->rcWave.right){
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
					iPointCount, 
					RECTH(pPanel->rcWave)+1);
				FillBox(mdc, 
					x, 
					pPanel->rcWave.top, 
					iPointCount, 
					RECTH(pPanel->rcWave)+1);
			}

			if(x <=  pPanel->rcWave.left+1) {
				MoveTo(hdc, x, y);
				MoveTo(mdc, x, y);
				continue;
			}
			
			if(gCfgSystem.bLineType == LINE_ANTI){
				DrawWuLine(hdc, x1, y1, x, y, COLOR_magenta);
				DrawWuLine(mdc, x1, y1, x, y, COLOR_magenta);
			}else if(gCfgSystem.bLineType == LINE_WIDE ){
				DrawWideLine(hdc, REALWIDELINE, gbLineWidth, x1, y1, x, y, WIDELINE_DOT);
				DrawWideLine(mdc, REALWIDELINE, gbLineWidth, x1, y1, x, y, WIDELINE_DOT);
			}else{
				LineTo(hdc, x, y);
				LineTo(mdc, x, y);
			}
		}
		//画扫描棒
		FillBox(hdc, x+iPointCount, pPanel->rcWave.top, FILLWAVE_WIDTH, RECTH(pPanel->rcWave)+1);
		FillBox(mdc, x+iPointCount, pPanel->rcWave.top, FILLWAVE_WIDTH, RECTH(pPanel->rcWave)+1);
	}
//	DrawParas_Ecg2(pPanel);
	DrawRuler_Ecg2(pPanel);
	return 0;
}


//Multi ECG Paras
static int DrawParas_MultiEcg(const PNWAVE_PANEL pPanel)
{
	int iRulerLen = 0;		//标尺长度	
	int i;
	static int icount=0;
	
	/*
	//Clear Area
	FillBox(hdcWaveMem, 
		pPanel->rcPara.left, 
		pPanel->rcPara.top, 
		RECTW(pPanel->rcPara)+1, 
		RECTH(pPanel->rcPara)+1);
	*/
	SetTextColor(hdcWaveMem, COLOR_black);
	SetPenColor(hdcWaveMem, COLOR_black);
	SelectFont(hdcWaveMem, gFontSmall);
	if(iMulOldMode!=gCfgEcg.bMode)
		TextOut(hdcWaveMem, pPanel->rcPara.left+(PARAOFWAVE_WIDTH/5)*2, pPanel->rcPara.top+1, strEcgMode[iMulOldMode]);
	if(iMulOldGain!=gCfgEcg.bGain){
		TextOut(hdcWaveMem, pPanel->rcPara.left+(PARAOFWAVE_WIDTH/5)*4, pPanel->rcPara.top+1, strEcgGainLb[iMulOldGain]);
	
	iRulerLen = giEcgRulers[iMulOldGain];
	for(i=0; i<pPanel->bWaveCount; i++){
	//	DrawRuler(pPanel, iRulerLen,i);

		TextOut(hdcWaveMem, pPanel->rcPara.left, 
			pPanel->rcPara.top+(RECTH(pPanel->rcPara)/(pPanel->bWaveCount*2))*(i*2+1)-30, 
			strEcgLeads[i]);
		if(i==0)
		TextOut(hdcWaveMem, pPanel->rcPara.left+15+30,  pPanel->rcPara.top+(RECTH(pPanel->rcPara)/(pPanel->bWaveCount*2))*(i*2+1)-iRulerLen/2-5,strEcgGain[iMulOldGain]);
	
		}	
	}
	
	SetTextColor(hdcWaveMem, PARATEXTCOLOR);
	//显示文字
	//Label
//	TextOut(hdcWaveMem, pPanel->rcPara.left+4, pPanel->rcPara.top+1, strEcgLabel);
	//Mode
	TextOut(hdcWaveMem, pPanel->rcPara.left+(PARAOFWAVE_WIDTH/5)*2, pPanel->rcPara.top+1, strEcgMode[gCfgEcg.bMode]);
	//Gain
	TextOut(hdcWaveMem, pPanel->rcPara.left+(PARAOFWAVE_WIDTH/5)*4, pPanel->rcPara.top+1, strEcgGainLb[gCfgEcg.bGain]);
	//Speed
// 	TextOut(hdcWave, pPanel->rcPara.left+4, pPanel->rcPara.top+46, strSpeed[gCfgEcg.bScanSpeed]);
	
	
	//计算标尺 
	iRulerLen = giEcgRulers[gCfgEcg.bGain];
//	SetPenColor(hdcWaveMem, PARATEXTCOLOR);
	
//	DrawRuler(pPanel, iRulerLen,0);
	
	for(i=0; i<pPanel->bWaveCount; i++){
		TextOut(hdcWaveMem, pPanel->rcPara.left, 
			pPanel->rcPara.top+(RECTH(pPanel->rcPara)/(pPanel->bWaveCount*2))*(i*2+1)-30, 
			strEcgLeads[i]);
		if(i==0)
		TextOut(hdcWaveMem, pPanel->rcPara.left+15+30,  pPanel->rcPara.top+(RECTH(pPanel->rcPara)/(pPanel->bWaveCount*2))*(i*2+1)-iRulerLen/2-5,strEcgGain[gCfgEcg.bGain]);
	
	}	
// 	SetBrushColor(hdcWaveMem, WAVE_BKCOLOR);
	
	if(gbDebugRect) 
		Rectangle(hdcWaveMem, 
			  pPanel->rcPara.left, 
			  pPanel->rcPara.top, 
			  pPanel->rcPara.right, 
			  pPanel->rcPara.bottom);
	
	//拷贝DC
		BitBlt(hdcWaveMem, 
			       pPanel->rcPara.left, pPanel->rcPara.top, 30, RECTH(pPanel->rcPara)+1, 
			       hdcWave, 
			       pPanel->rcPara.left, pPanel->rcPara.top, 
			       0);
		BitBlt(hdcWaveMem, 
			       pPanel->rcPara.left+40, pPanel->rcPara.top, 150, 17, 
			       hdcWave, 
			       pPanel->rcPara.left+40, pPanel->rcPara.top, 
			       0);
		BitBlt(hdcWaveMem, 
			       pPanel->rcPara.left+30, pPanel->rcPara.top, 40, RECTH(pPanel->rcPara)+1, 
			       hdcWave, 
			       pPanel->rcPara.left+30, pPanel->rcPara.top, 
			       0);
	
	iMulOldMode=gCfgEcg.bMode;
	iMulOldGain=gCfgEcg.bGain;
	return 0;
}

//绘制MultiEcg波形
static int DrawWave_Multi(HDC hdc, HDC mdc, const PNWAVE_PANEL pPanel, int iPointCount)
{
	static int x[ECGWAVE_COUNT] = {0};
	static int y[ECGWAVE_COUNT] = {0};
	static int x1[ECGWAVE_COUNT] = {0};
	static int y1[ECGWAVE_COUNT] = {0};
	static int iValue[ECGWAVE_COUNT] = {0};
	static int iWaveCount[ECGWAVE_COUNT] = {0};
	BOOL res;
	int i, j;
	
	//开始绘制标志 ，初始化X，Y坐标 
	if(gbResetWaves){
		for(i=0; i<pPanel->bWaveCount; i++){
			x[i]=pPanel->rcWave.left;
			y[i]=pPanel->rcWave.top +(i*2+1)*(RECTH(pPanel->rcWave)/pPanel->bWaveCount/2);
			iWaveCount[i] = 0;
		} 
		FillBox(hdc, 
			pPanel->rcWave.left, 
			pPanel->rcWave.top, 
			RECTW(pPanel->rcWave)+1, 
			RECTH(pPanel->rcWave)+1);
		if(mdc != (HDC)NULL)
			FillBox(mdc, 
				pPanel->rcWave.left, 
				pPanel->rcWave.top, 
				RECTW(pPanel->rcWave)+1, 
				RECTH(pPanel->rcWave)+1);
		//??????í??o3?
		ClearData_Ecg1();
	}
	
	//波形以及文字颜色根据配置而定
	SetPenColor(hdc, gCfgEcg.iColor);	
	if(mdc != (HDC)NULL)
		SetPenColor(mdc, gCfgEcg.iColor);	
	
	for(j=0; j<iPointCount; j++){
		//获得波形值
		res = RetrieveData_Ecg1(&EcgWave1);
		if(res){
			iValue[ECGWAVE_I] = EcgWave1.iWaves[ECGWAVE_I];	
			iValue[ECGWAVE_II] = EcgWave1.iWaves[ECGWAVE_II];	
			iValue[ECGWAVE_III] = EcgWave1.iWaves[ECGWAVE_III];	
			iValue[ECGWAVE_AVR] = EcgWave1.iWaves[ECGWAVE_AVR];	
			iValue[ECGWAVE_AVL] = EcgWave1.iWaves[ECGWAVE_AVL];	
			iValue[ECGWAVE_AVF] = EcgWave1.iWaves[ECGWAVE_AVF];	
			iValue[ECGWAVE_V] = EcgWave1.iWaves[ECGWAVE_V];	
			
			//0.25增益下，缩小2/1
			if(gCfgEcg.bGain == ECGGAIN_025X){
				for(i=0; i<ECGWAVE_COUNT; i++){
					iValue[i] = (iValue[i] - BASELINE_ECG)/2 + BASELINE_ECG;
				}
			}
		}else{
			for(i=0;i<ECGWAVE_COUNT;i++)
				iValue[i]=BASELINE_ECG;
		}
		
  		for(i=0; i<pPanel->bWaveCount; i++){
			if(gCfgSystem.bLineType == LINE_NORMAL){
				MoveTo(hdc, x[i], y[i]);	
				if(mdc != (HDC)NULL) MoveTo(mdc, x[i], y[i]);	
			}
			else{
				x1[i] = x[i];
				y1[i] = y[i];	
			}

			//Y
			y[i] = pPanel->rcWave.top 
				+  (RECTH(pPanel->rcWave)/pPanel->bWaveCount - iValue[i]/(gfEcgSampleToPixel)) 
				+ (giEcgBaseline - RECTH(pPanel->rcWave)/pPanel->bWaveCount/2)
				+  (i*2)*(RECTH(pPanel->rcWave)/pPanel->bWaveCount/2);
/*
			y[i] = pPanel->rcWave.top-68
				+  (RECTH(pPanel->rcWave)/pPanel->bWaveCount- iValue[i]/(gfEcgSampleToPixel)) 
				+ (giEcgBaseline - RECTH(pPanel->rcWave)/(pPanel->bWaveCount)/2)
				+ i*2*(RECTH(pPanel->rcWave)/pPanel->bWaveCount/2);
*/

		
			if(y[i] > pPanel->rcWave.bottom-2) y[i] = pPanel->rcWave.bottom-2;
 			if(y[i] < pPanel->rcWave.top+1) y[i] = pPanel->rcWave.top +1;

			//X
			if(x[i] < pPanel->rcWave.right){
				x[i] = pPanel->rcWave.left + (float)(iWaveCount[i]) / (float)(pPanel->fXAxis);
				iWaveCount[i] ++;
			}
			else{
				x[i] = pPanel->rcWave.left;
				iWaveCount[i] = 0;
				MoveTo(hdc, x[i], y[i]);
				if(mdc != (HDC)NULL) MoveTo(mdc, x[i], y[i]);
				x1[i] = x[i];
				y1[i] = y[i];
				
				//画扫描棒
				FillBox(hdc, 
					x[0], 
					pPanel->rcWave.top, 
					FILLWAVE_WIDTH, 
					RECTH(pPanel->rcWave)+1);
				if(mdc != (HDC)NULL)
					FillBox(mdc, 
						x[0], 
						pPanel->rcWave.top, 
						FILLWAVE_WIDTH, 
						RECTH(pPanel->rcWave)+1);
			}
						
			if(gCfgSystem.bLineType == LINE_ANTI){
				DrawWuLine(hdc, x1[i], y1[i], x[i], y[i], COLOR_green);
				if(mdc != (HDC)NULL)
					DrawWuLine(mdc, x1[i], y1[i], x[i], y[i], COLOR_green);
			}else if(gCfgSystem.bLineType == LINE_WIDE){
				DrawWideLine(hdc, REALWIDELINE, gbLineWidth, x1[i], y1[i], x[i], y[i], WIDELINE_DOT);
				if(mdc != (HDC)NULL)
					DrawWideLine(mdc, REALWIDELINE, gbLineWidth, x1[i], y1[i], x[i], y[i], WIDELINE_DOT);
			}else{
				LineTo(hdc, x[i], y[i]);
				if(mdc != (HDC)NULL)
					LineTo(mdc, x[i], y[i]);
			}
 		}
	}
		
	//画一个扫描棒，处理波形叠加问题
	FillBox(hdc, 
		x[0]+iPointCount, 
		pPanel->rcWave.top, 
		FILLWAVE_WIDTH, 
		RECTH(pPanel->rcWave)+1);
	if(mdc != (HDC)NULL)
		FillBox(mdc, 
			x[0]+iPointCount, 
			pPanel->rcWave.top, 
			FILLWAVE_WIDTH, 
			RECTH(pPanel->rcWave)+1);
	
//	DrawParas_MultiEcg(pPanel);
	DrawRuler_Ecg(pPanel);
	return 0;
}

//Multi ECG Paras
static int DrawParas_12LEADEcg(const PNWAVE_PANEL pPanel)
{
	int iRulerLen = 0;		//标尺长度	
	int i;
	
	//Clear Area
	FillBox(hdcWaveMem, 
		pPanel->rcPara.left, 
		pPanel->rcPara.top, 
		RECTW(pPanel->rcPara)+1, 
		RECTH(pPanel->rcPara)+1);
	
	SetTextColor(hdcWaveMem, gCfgEcg.iColor);
	
	//计算标尺 
	iRulerLen = giEcgRulers[gCfgEcg.bGain];
	SetPenColor(hdcWaveMem, COLOR_darkgreen);
	for(i=0; i<pPanel->bWaveCount; i++){
		//标尺
/*		SetBrushColor(hdcWaveMem, COLOR_darkgreen);
		FillBox(hdcWaveMem, 
			pPanel->rcPara.right-NECG_CAL_WIDTH, 
			pPanel->rcPara.top+(RECTH(pPanel->rcPara)/(pPanel->bWaveCount*2))*(i*2+1)-iRulerLen/2, 
			NECG_CAL_WIDTH, 
			iRulerLen);*/
		
		Rectangle(hdcWaveMem, 
			  pPanel->rcPara.right-2, 
			  pPanel->rcPara.top+(RECTH(pPanel->rcPara)/(pPanel->bWaveCount*2))*(i*2+1)-iRulerLen/2,
			  pPanel->rcPara.right-1, 
			  pPanel->rcPara.top+(RECTH(pPanel->rcPara)/(pPanel->bWaveCount*2))*(i*2+1)-iRulerLen/2+iRulerLen);
		
		TextOut(hdcWaveMem, pPanel->rcPara.right-30, 
			pPanel->rcPara.top+(RECTH(pPanel->rcPara)/(pPanel->bWaveCount*2))*(i*2+1)-iRulerLen/2+10, 
			strEcgLeads[i]);
	}	
// 	SetBrushColor(hdcWaveMem, WAVE_BKCOLOR);
	
	//显示文字
	//Label
	TextOut(hdcWaveMem, pPanel->rcPara.left+4, pPanel->rcPara.top+1, strEcgLabel);
	//Mode
	TextOut(hdcWaveMem, pPanel->rcPara.left+4, pPanel->rcPara.top+16, strEcgMode[gCfgEcg.bMode]);
	//Gain
	TextOut(hdcWaveMem, pPanel->rcPara.left+4, pPanel->rcPara.top+31, strEcgGain[gCfgEcg.bGain]);
	//Speed
// 	TextOut(hdcWave, pPanel->rcPara.left+4, pPanel->rcPara.top+46, strSpeed[gCfgEcg.bScanSpeed]);
	
	if(gbDebugRect) 
		Rectangle(hdcWaveMem, 
			  pPanel->rcPara.left, 
			  pPanel->rcPara.top, 
			  pPanel->rcPara.right, 
			  pPanel->rcPara.bottom);
	
	//拷贝DC
	BitBlt(hdcWaveMem, 
	       pPanel->rcPara.left, pPanel->rcPara.top, RECTW(pPanel->rcPara)+1, RECTH(pPanel->rcPara)+1, 
	       hdcWave, 
	       pPanel->rcPara.left, pPanel->rcPara.top, 
	       0);
	
	return 0;
}

//绘制12LeadEcg波形
static int DrawWave_12LEAD(HDC hdc, HDC mdc, const PNWAVE_PANEL pPanel, int iPointCount)
{
	static int x[ECG12WAVE_COUNT] = {0};
	static int y[ECG12WAVE_COUNT] = {0};
	static int x1[ECG12WAVE_COUNT] = {0};
	static int y1[ECG12WAVE_COUNT] = {0};
	static int iValue[ECG12WAVE_COUNT] = {0};
	static int iWaveCount[ECG12WAVE_COUNT] = {0};
	BOOL res;
	int i, j;
	
	//开始绘制标志 ，初始化X，Y坐标 
	if(gbResetWaves){
		for(i=0; i<pPanel->bWaveCount; i++){
			x[i]=pPanel->rcWave.left;
			y[i]=pPanel->rcWave.top +(i*2+1)*(RECTH(pPanel->rcWave)/pPanel->bWaveCount/2);
			iWaveCount[i] = 0;
		} 
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
		//清空绘图缓冲
		ClearData_Ecg1();
	}
	
	//波形以及文字颜色根据配置而定
	SetPenColor(hdc, gCfgEcg.iColor);	
	if(mdc != (HDC)NULL)
		SetPenColor(mdc, gCfgEcg.iColor);	
	
	for(j=0; j<iPointCount; j++){
		//获得波形值
		res = RetrieveData_Ecg1(&EcgWave1);
		if(res){
			iValue[ECG12WAVE_I] = EcgWave1.iWaves[ECG12WAVE_I];	
			iValue[ECG12WAVE_II] = EcgWave1.iWaves[ECG12WAVE_II];	
			iValue[ECG12WAVE_III] = EcgWave1.iWaves[ECG12WAVE_III];	
			iValue[ECG12WAVE_AVR] = EcgWave1.iWaves[ECG12WAVE_AVR];	
			iValue[ECG12WAVE_AVL] = EcgWave1.iWaves[ECG12WAVE_AVL];	
			iValue[ECG12WAVE_AVF] = EcgWave1.iWaves[ECG12WAVE_AVF];	
			iValue[ECG12WAVE_V1] = EcgWave1.iWaves[ECG12WAVE_V1];	
			iValue[ECG12WAVE_V2] = EcgWave1.iWaves[ECG12WAVE_V2];	
			iValue[ECG12WAVE_V3] = EcgWave1.iWaves[ECG12WAVE_V3];	
			iValue[ECG12WAVE_V4] = EcgWave1.iWaves[ECG12WAVE_V4];	
			iValue[ECG12WAVE_V5] = EcgWave1.iWaves[ECG12WAVE_V5];	
			iValue[ECG12WAVE_V6] = EcgWave1.iWaves[ECG12WAVE_V6];	
			//0.25增益下，缩小2/1
			if(gCfgEcg.bGain == ECGGAIN_025X){
				for(i=0; i<ECG12WAVE_COUNT; i++){
					iValue[i] = (iValue[i] - BASELINE_ECG)/2 + BASELINE_ECG;
				}
			}
		}
		
  		for(i=0; i<pPanel->bWaveCount; i++){
			if(gCfgSystem.bLineType == LINE_NORMAL){
				MoveTo(hdc, x[i], y[i]);	
				if(mdc != (HDC)NULL) MoveTo(mdc, x[i], y[i]);	
			}
			else{
				x1[i] = x[i];
				y1[i] = y[i];	
			}
/*
			//Y
			y[i] = pPanel->rcWave.top 
				+  (RECTH(pPanel->rcWave)/pPanel->bWaveCount - iValue[i]/(gfEcgSampleToPixel)) 
				+ (giEcgBaseline - RECTH(pPanel->rcWave)/pPanel->bWaveCount/2)
				+  (i*2)*(RECTH(pPanel->rcWave)/pPanel->bWaveCount/2);
	*/		
			y[i] = pPanel->rcWave.top-69
				+  (RECTH(pPanel->rcWave)/pPanel->bWaveCount- iValue[i]/(gfEcgSampleToPixel)) 
				+ (giEcgBaseline - RECTH(pPanel->rcWave)/(pPanel->bWaveCount)/2)
				+ i*2*(RECTH(pPanel->rcWave)/pPanel->bWaveCount/2);


		
			if(y[i] > pPanel->rcWave.bottom-2) y[i] = pPanel->rcWave.bottom-2;
 			if(y[i] < pPanel->rcWave.top+1) y[i] = pPanel->rcWave.top +1;

			//X
			if(x[i] < pPanel->rcWave.right){
				x[i] = pPanel->rcWave.left + (float)(iWaveCount[i]) / (float)(pPanel->fXAxis);
				iWaveCount[i] ++;
			}
			else{
				x[i] = pPanel->rcWave.left;
				iWaveCount[i] = 0;
				MoveTo(hdc, x[i], y[i]);
				if(mdc != (HDC)NULL) MoveTo(mdc, x[i], y[i]);
				x1[i] = x[i];
				y1[i] = y[i];
				
				//画扫描棒
				FillBox(hdc, 
					x[0], 
					pPanel->rcWave.top, 
					FILLWAVE_WIDTH, 
					RECTH(pPanel->rcWave));
				if(mdc != (HDC)NULL)
					FillBox(mdc, 
						x[0], 
						pPanel->rcWave.top, 
						FILLWAVE_WIDTH, 
						RECTH(pPanel->rcWave));
			}
						
			if(gCfgSystem.bLineType == LINE_ANTI){
				DrawWuLine(hdc, x1[i], y1[i], x[i], y[i], COLOR_green);
				if(mdc != (HDC)NULL)
					DrawWuLine(mdc, x1[i], y1[i], x[i], y[i], COLOR_green);
			}else if(gCfgSystem.bLineType == LINE_WIDE){
				DrawWideLine(hdc, REALWIDELINE, gbLineWidth, x1[i], y1[i], x[i], y[i], WIDELINE_DOT);
				if(mdc != (HDC)NULL)
					DrawWideLine(mdc, REALWIDELINE, gbLineWidth, x1[i], y1[i], x[i], y[i], WIDELINE_DOT);
			}else{
				LineTo(hdc, x[i], y[i]);
				if(mdc != (HDC)NULL)
					LineTo(mdc, x[i], y[i]);
			}
 		}
	}
		
	//画一个扫描棒，处理波形叠加问题
	FillBox(hdc, 
		x[0]+iPointCount, 
		pPanel->rcWave.top, 
		FILLWAVE_WIDTH, 
		RECTH(pPanel->rcWave));
	if(mdc != (HDC)NULL)
		FillBox(mdc, 
			x[0]+iPointCount, 
			pPanel->rcWave.top, 
			FILLWAVE_WIDTH, 
			RECTH(pPanel->rcWave));

	return 0;
}

/*
	绘制SpO2波形参数
*/
static int DrawParas_SpO2(const PNWAVE_PANEL pPanel)
{
	/*
	//Clear Area
 	FillBox(hdcWaveMem, 
		pPanel->rcPara.left, 
		pPanel->rcPara.top, 
		RECTW(pPanel->rcPara)+1, 
		RECTH(pPanel->rcPara)+1);
	*/
	SetTextColor(hdcWaveMem, COLOR_black);
	if(iSpo2Gain!=gCfgSpO2.bGain)
			TextOut(hdcWaveMem, pPanel->rcPara.left+(PARAOFWAVE_WIDTH/5)*2, pPanel->rcPara.top+1, strSpO2Gain[iSpo2Gain]);


	SetTextColor(hdcWaveMem,PARATEXTCOLOR);
 	//显示信息文字
	if(gCfgSystem.bLanguage == LANGUAGE_SPANISH)
		DrawText(hdcWaveMem, "Pletis", -1, &(pPanel->rcPara), DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);
	else
		DrawText(hdcWaveMem, strSpO2Label, -1, &(pPanel->rcPara), DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);
 	//DrawText(hdcWaveMem, strSpO2Gain[gCfgSpO2.bGain], -1, &(pPanel->rcPara), DT_NOCLIP | DT_TOP | DT_CENTER | DT_SINGLELINE);
	TextOut(hdcWaveMem, pPanel->rcPara.left+(PARAOFWAVE_WIDTH/5)*2, pPanel->rcPara.top+1, strSpO2Gain[gCfgSpO2.bGain]);
		
	/*
 	//显示标尺
	SetPenColor(hdcWaveMem, gCfgSpO2.iColor);
	Rectangle(hdcWaveMem, 
		  pPanel->rcPara.right-2, 
		  pPanel->rcPara.top+1, 
		  pPanel->rcPara.right-1, 
		  pPanel->rcPara.bottom-1);
// 	MoveTo(hdcWaveMem, pPanel->rcPara.right-16, pPanel->rcPara.top+1);
// 	LineTo(hdcWaveMem, pPanel->rcPara.right-1, pPanel->rcPara.top+1);
// 	LineTo(hdcWaveMem, pPanel->rcPara.right-1, pPanel->rcPara.bottom-1);
// 	LineTo(hdcWaveMem, pPanel->rcPara.right-16, pPanel->rcPara.bottom-1);
	*/
	if(gbDebugRect) 
		Rectangle(hdcWaveMem, 
			  pPanel->rcPara.left, 
			  pPanel->rcPara.top, 
			  pPanel->rcPara.right, 
			  pPanel->rcPara.bottom);
	
	//拷贝DC
	BitBlt(hdcWaveMem, 
	       pPanel->rcPara.left, pPanel->rcPara.top, RECTW(pPanel->rcPara)+1, RECTH(pPanel->rcPara)+1, 
	       hdcWave, 
	       pPanel->rcPara.left, pPanel->rcPara.top, 
	       0);
	iSpo2Gain=gCfgSpO2.bGain;
	return 0;
}

//绘制SpO2波形
//DrawWave_SpO2(hdcWave, hdcWaveMem, &(gNWavePanels[i]), DRAWWAVENUM_200HZ);
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
	if(gbResetWaves){
		x = pPanel->rcWave.left;//左上端
 		y = pPanel->rcWave.top + RECTH(pPanel->rcWave)/2;//右下端
		iWaveCount = 0;
		FillBox(hdc, 
			pPanel->rcWave.left, 
			pPanel->rcWave.top, 
			RECTW(pPanel->rcWave)+1, 
			RECTH(pPanel->rcWave)+1);
		if(mdc != (HDC)NULL)
			FillBox(mdc, 
				pPanel->rcWave.left, 
				pPanel->rcWave.top, 
				RECTW(pPanel->rcWave)+1, 
				RECTH(pPanel->rcWave)+1);
		ClearData_SpO2();
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
		res = RetrieveData_SpO2(&SpO2Wave);
		//Nellcor SpO2  (波形不判断状态)
		if(!gbHaveNellSpO2){
			if((!gValueSpO2.bOff) && (res)){
				iValue = SpO2Wave.bWave;
				y = pPanel->rcWave.top +  RECTH(pPanel->rcWave) - iValue/(pPanel->fYAxis);
			}
			else if(gValueSpO2.bOff){
				if(gCfgSpO2.bDrawMode == DRAWMODE_LINE){
					iValue = RECTH(pPanel->rcWave)/2;//
					//iValue = RECTH(pPanel->rcWave);//
					y = pPanel->rcWave.top + iValue;
				}
				else{
					iValue = 0;
					y = pPanel->rcWave.top +  RECTH(pPanel->rcWave) - iValue/(pPanel->fYAxis);
				}
			}
		}
		else{
			if(res){
				iValue = SpO2Wave.bWave;
				y = pPanel->rcWave.top +  RECTH(pPanel->rcWave) - iValue/(pPanel->fYAxis);
			}
			else if(gValueSpO2.bOff){
				if(gCfgSpO2.bDrawMode == DRAWMODE_LINE){
					iValue = RECTH(pPanel->rcWave)/2;
					y = pPanel->rcWave.top + iValue;
				}
				else{
					iValue = 0;
					y = pPanel->rcWave.top +  RECTH(pPanel->rcWave) - iValue/(pPanel->fYAxis);
				}
			}
		}
		//Y
// 		y = pPanel->rcWave.top +  (NWAVEMAX_SPO2-iValue)/pPanel->fYAxis;
		if(y > pPanel->rcWave.bottom-2) y = pPanel->rcWave.bottom-2;
		if(y<pPanel->rcWave.top) y = pPanel->rcWave.top;
		
		//X
		if(x < pPanel->rcWave.right){
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
			FillBox(hdc, x, pPanel->rcWave.top, iPointCount, RECTH(pPanel->rcWave)+1);
			if(mdc != (HDC)NULL)
			FillBox(mdc, x, pPanel->rcWave.top, iPointCount, RECTH(pPanel->rcWave)+1);
		}

		if(x <=  pPanel->rcWave.left+1) {
			MoveTo(hdc, x, y);
			if(mdc != (HDC)NULL)
				MoveTo(mdc, x, y);
			continue;
		}
			
		if(gCfgSystem.bLineType == LINE_ANTI && gCfgSpO2.bDrawMode != DRAWMODE_FILL){
			DrawWuLine(hdc, x1, y1, x, y, COLOR_magenta);
			if(mdc != (HDC)NULL)
				DrawWuLine(mdc, x1, y1, x, y, COLOR_magenta);
		}else if(gCfgSystem.bLineType == LINE_WIDE && gCfgSpO2.bDrawMode != DRAWMODE_FILL){
			DrawWideLine(hdc, REALWIDELINE, gbLineWidth, x1, y1, x, y, WIDELINE_DOT);
			if(mdc != (HDC)NULL)
				DrawWideLine(mdc, REALWIDELINE, gbLineWidth, x1, y1, x, y, WIDELINE_DOT);
		}else{
			if(gCfgSpO2.bDrawMode == DRAWMODE_FILL){
				MoveTo(hdc, x, pPanel->rcWave.bottom-1);
				if(mdc != (HDC)NULL)
					MoveTo(mdc, x, pPanel->rcWave.bottom-1);
			}	
			LineTo(hdc, x, y);
			if(mdc != (HDC)NULL)
				LineTo(mdc, x, y);
		}
	}
		
 	//画扫描棒
 	FillBox(hdc, x+iPointCount, pPanel->rcWave.top, FILLWAVE_WIDTH, RECTH(pPanel->rcWave)+1);
	if(mdc != (HDC)NULL)
		FillBox(mdc, x+iPointCount, pPanel->rcWave.top, FILLWAVE_WIDTH, RECTH(pPanel->rcWave)+1);

	return 0;
}

/*
	绘制Resp波形参数
*/
static int DrawParas_Resp(const PNWAVE_PANEL pPanel)
{
/*
	//Clear Area
	FillBox(hdcWaveMem, 
		pPanel->rcPara.left, 
		pPanel->rcPara.top, 
		RECTW(pPanel->rcPara)+1, 
		RECTH(pPanel->rcPara)+1);
*/	
	SetTextColor(hdcWaveMem, COLOR_black);
	if(iRROldGain!=gCfgResp.bGain)
			TextOut(hdcWaveMem, pPanel->rcPara.left+(PARAOFWAVE_WIDTH/5)*2, pPanel->rcPara.top+1, strRespGain[iRROldGain]);


	SetTextColor(hdcWaveMem, PARATEXTCOLOR);
 	//显示信息文字
	DrawText(hdcWaveMem, strRespLabel, -1, &(pPanel->rcPara), DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);
//	DrawText(hdcWaveMem, strRespGain[gCfgResp.bGain], -1, &(pPanel->rcPara), DT_NOCLIP | DT_TOP | DT_CENTER | DT_SINGLELINE);
// 	DrawText(hdcWave, strSpeed[gCfgResp.bScanSpeed], -1, &(pPanel->rcPara), DT_NOCLIP | DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
	TextOut(hdcWaveMem, pPanel->rcPara.left+(PARAOFWAVE_WIDTH/5)*2, pPanel->rcPara.top+1, strRespGain[gCfgResp.bGain]);

/*
 	//显示标尺
	SetPenColor(hdcWaveMem, gCfgResp.iColor);
	Rectangle(hdcWaveMem, 
		  pPanel->rcPara.right-2, 
		  pPanel->rcPara.top+1, 
		  pPanel->rcPara.right-1, 
		  pPanel->rcPara.bottom-1);
	MoveTo(hdcWaveMem, pPanel->rcPara.right-16, pPanel->rcPara.top+1);
	LineTo(hdcWaveMem, pPanel->rcPara.right-1, pPanel->rcPara.top+1);
	LineTo(hdcWaveMem, pPanel->rcPara.right-1, pPanel->rcPara.bottom-1);
	LineTo(hdcWaveMem, pPanel->rcPara.right-16, pPanel->rcPara.bottom-1);
*/	
	if(gbDebugRect) 
		Rectangle(hdcWaveMem, 
			  pPanel->rcPara.left, 
			  pPanel->rcPara.top, 
			  pPanel->rcPara.right, 
			  pPanel->rcPara.bottom);
	
	//拷贝DC
	BitBlt(hdcWaveMem, 
	       pPanel->rcPara.left, pPanel->rcPara.top, RECTW(pPanel->rcPara)+1, RECTH(pPanel->rcPara)+1, 
	       hdcWave, 
	       pPanel->rcPara.left, pPanel->rcPara.top, 
	       0);
	iRROldGain=gCfgResp.bGain;
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
	if(gbResetWaves){
		x = pPanel->rcWave.left;
		y = pPanel->rcWave.top + RECTH(pPanel->rcWave)/2;
		iWaveCount = 0;
		FillBox(hdc, 
			pPanel->rcWave.left, 
			pPanel->rcWave.top, 
			RECTW(pPanel->rcWave)+1, 
			RECTH(pPanel->rcWave)+1);
		FillBox(mdc, 
			pPanel->rcWave.left, 
			pPanel->rcWave.top, 
			RECTW(pPanel->rcWave)+1, 
			RECTH(pPanel->rcWave)+1);
		ClearData_Resp();
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
		res = RetrieveData_Resp(&RespWave);
		if((!gValueResp.bOff) && (res) &&(gCfgResp.bSwitch == SWITCH_ON)){
			iValue = RespWave.iWave;
			y = pPanel->rcWave.top +  RECTH(pPanel->rcWave) - iValue/(pPanel->fYAxis);
		}
		else if(gValueResp.bOff || gCfgResp.bSwitch == SWITCH_OFF){
			iValue = RECTH(pPanel->rcWave)/2;
			y = pPanel->rcWave.top + iValue;
		}
		
		//Y
// 		y = pPanel->rcWave.top +  (NWAVEMAX_SPO2-iValue)/pPanel->fYAxis;
		if(y > pPanel->rcWave.bottom-2) y = pPanel->rcWave.bottom-2;
		if(y<pPanel->rcWave.top) y = pPanel->rcWave.top;
		
		//X
		if(x < pPanel->rcWave.right){
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
			FillBox(hdc, x, pPanel->rcWave.top, iPointCount, RECTH(pPanel->rcWave)+1);
			FillBox(mdc, x, pPanel->rcWave.top, iPointCount, RECTH(pPanel->rcWave)+1);
		}

		if(x <=  pPanel->rcWave.left+1) {
			MoveTo(hdc, x, y);
			MoveTo(mdc, x, y);
			continue;
		}
		
		if(gCfgSystem.bLineType == LINE_ANTI){
			DrawWuLine(hdc, x1, y1, x, y, COLOR_magenta);
			DrawWuLine(mdc, x1, y1, x, y, COLOR_magenta);
		}else if(gCfgSystem.bLineType == LINE_WIDE ){
			DrawWideLine(hdc, REALWIDELINE, gbLineWidth, x1, y1, x, y, WIDELINE_DOT);
			DrawWideLine(mdc, REALWIDELINE, gbLineWidth, x1, y1, x, y, WIDELINE_DOT);
		}else{
			LineTo(hdc, x, y);
			LineTo(mdc, x, y);
		}
	}
		
	//画扫描棒
	FillBox(hdc, x+iPointCount, pPanel->rcWave.top, FILLWAVE_WIDTH, RECTH(pPanel->rcWave)+1);
	FillBox(mdc, x+iPointCount, pPanel->rcWave.top, FILLWAVE_WIDTH, RECTH(pPanel->rcWave)+1);
	
	return 0;
}

/*
	绘制CO2波形参数
*/
static int DrawParas_CO2(const PNWAVE_PANEL pPanel)
{
	//Clear Area
	/*
	FillBox(hdcWaveMem, 
		pPanel->rcPara.left, 
		pPanel->rcPara.top, 
		RECTW(pPanel->rcPara)+1, 
		RECTH(pPanel->rcPara)+1);
	*/
	
	SetTextColor(hdcWaveMem, PARATEXTCOLOR);
 	//显示信息文字
	DrawText(hdcWaveMem, strCO2Label, -1, &(pPanel->rcPara), DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);
//	DrawText(hdcWaveMem, strCO2Unit[gCfgCO2.bUnit], -1, &(pPanel->rcPara), DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE);
// 	DrawText(hdcWave, strSpeed[gCfgCO2.bScanSpeed], -1, &(pPanel->rcPara), DT_NOCLIP | DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
/*
 	//显示标尺
	DrawText(hdcWaveMem, strCO2High[gCfgCO2.bUnit][gCfgCO2.bRange], -1, &(pPanel->rcPara), DT_NOCLIP | DT_TOP | DT_RIGHT| DT_SINGLELINE);
	DrawText(hdcWaveMem, strCO2Low[gCfgCO2.bUnit][gCfgCO2.bRange], -1, &(pPanel->rcPara), DT_NOCLIP | DT_BOTTOM | DT_RIGHT| DT_SINGLELINE);

	SetPenColor(hdcWaveMem, gCfgCO2.iColor);
	Rectangle(hdcWaveMem, 
		  pPanel->rcPara.right-2, 
		  pPanel->rcPara.top+1, 
		  pPanel->rcPara.right-1, 
		  pPanel->rcPara.bottom-1);
*/	

// 	MoveTo(hdcWaveMem, pPanel->rcPara.right-16, pPanel->rcPara.top+1);
// 	LineTo(hdcWaveMem, pPanel->rcPara.right-1, pPanel->rcPara.top+1);
// 	LineTo(hdcWaveMem, pPanel->rcPara.right-1, pPanel->rcPara.bottom-1);
// 	LineTo(hdcWaveMem, pPanel->rcPara.right-16, pPanel->rcPara.bottom-1);
	

	if(gbDebugRect) 
		Rectangle(hdcWaveMem, 
			  pPanel->rcPara.left, 
			  pPanel->rcPara.top, 
			  pPanel->rcPara.right, 
			  pPanel->rcPara.bottom);
	
	//拷贝DC
	BitBlt(hdcWaveMem, 
	       pPanel->rcPara.left, pPanel->rcPara.top, RECTW(pPanel->rcPara)+1, RECTH(pPanel->rcPara)+1, 
	       hdcWave, 
	       pPanel->rcPara.left, pPanel->rcPara.top, 
	       0);
	
	return 0;
}

//绘制CO2波形
static int DrawWave_CO2(HDC hdc, HDC mdc, const PNWAVE_PANEL pPanel, int iPointCount)
{
	static int x = 0;
	static int y = 0;
	static int iWaveCount = 0;
	static int iValue;
	BOOL res;
	int i, j;
	int x1, y1;
		
	//开始绘制标志 ，初始化X，Y坐标 
	if(gbResetWaves){
		x = pPanel->rcWave.left;
		y = pPanel->rcWave.top + RECTH(pPanel->rcWave)/2;
		iWaveCount = 0;
		FillBox(hdc, 
			pPanel->rcWave.left, 
			pPanel->rcWave.top, 
			RECTW(pPanel->rcWave)+1, 
			RECTH(pPanel->rcWave)+1);
		if(mdc != (HDC)NULL)
			FillBox(mdc, 
				pPanel->rcWave.left, 
				pPanel->rcWave.top, 
				RECTW(pPanel->rcWave)+1, 
				RECTH(pPanel->rcWave)+1);
		ClearData_CO2();
	}
	
	//波形以及文字颜色根据配置而定
	SetPenColor(hdc, gCfgCO2.iColor);	
	if(mdc != (HDC)NULL)
		SetPenColor(mdc, gCfgCO2.iColor);	
	
	x1 =x;
	y1 =y;
	MoveTo(hdc, x, y);
	if(mdc != (HDC)NULL)
		MoveTo(mdc, x, y);
	
	for(j=0; j<iPointCount; j++){
		//获得波形值
		res = RetrieveData_CO2(&CO2Wave);
		if((!gValueCO2.bOff) && (res)){
			iValue = CO2Wave.wWave;
			y = pPanel->rcWave.top +  RECTH(pPanel->rcWave) - iValue/(pPanel->fYAxis);
		}
		else if(gValueCO2.bOff){
			iValue = RECTH(pPanel->rcWave)/2;
			y = pPanel->rcWave.top + iValue;
		}
		
		//Y
		if(y > pPanel->rcWave.bottom-2) y = pPanel->rcWave.bottom-2;
		if(y<pPanel->rcWave.top) y = pPanel->rcWave.top;
		
		//X
		if(x < pPanel->rcWave.right){
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
			FillBox(hdc, x, pPanel->rcWave.top, FILLWAVE_WIDTH, RECTH(pPanel->rcWave)+1);
			if(mdc != (HDC)NULL)
				FillBox(mdc, x, pPanel->rcWave.top, FILLWAVE_WIDTH, RECTH(pPanel->rcWave)+1);
		}

		if(x <=  pPanel->rcWave.left+1) {
			MoveTo(hdc, x, y);
			if(mdc != (HDC)NULL)
				MoveTo(mdc, x, y);
			continue;
		}
		
		if(gCfgSystem.bLineType == LINE_ANTI){
			DrawWuLine(hdc, x1, y1, x, y, COLOR_magenta);
			if(mdc != (HDC)NULL)
				DrawWuLine(mdc, x1, y1, x, y, COLOR_magenta);
		}else if(gCfgSystem.bLineType == LINE_WIDE ){
			DrawWideLine(hdc, REALWIDELINE, gbLineWidth, x1, y1, x, y, WIDELINE_DOT);
			if(mdc != (HDC)NULL)
				DrawWideLine(mdc, REALWIDELINE, gbLineWidth, x1, y1, x, y, WIDELINE_DOT);
		}else{
			LineTo(hdc, x, y);
			if(mdc != (HDC)NULL)
				LineTo(mdc, x, y);
		}
	}
		
	//画扫描棒
	FillBox(hdc, x+iPointCount, pPanel->rcWave.top, FILLWAVE_WIDTH, RECTH(pPanel->rcWave)+1);
	if(mdc != (HDC)NULL)
		FillBox(mdc, x+iPointCount, pPanel->rcWave.top, FILLWAVE_WIDTH, RECTH(pPanel->rcWave)+1);
	
	return 0;
}

/*
	绘制IBP1波形参数
*/
static int DrawParas_Ibp1(const PNWAVE_PANEL pPanel)
{
	//Clear Area
	/*
	FillBox(hdcWaveMem, 
		pPanel->rcPara.left, 
		pPanel->rcPara.top, 
		RECTW(pPanel->rcPara)+1, 
		RECTH(pPanel->rcPara)+1);
	*/
	SetTextColor(hdcWaveMem, COLOR_black);
	if(gCfgIbp1.bUnit !=Ibp1unit){
		DrawText(hdcWaveMem, strIbp1tmp, -1, &(pPanel->rcPara), DT_NOCLIP | DT_TOP | DT_RIGHT| DT_SINGLELINE);
		DrawText(hdcWaveMem, strIbpUnit[Ibp1unit], -1, &(pPanel->rcPara), DT_NOCLIP | DT_CENTER | DT_TOP | DT_SINGLELINE);
		DrawText(hdcWaveMem, strIbp1tmp2, -1, &(pPanel->rcPara), DT_NOCLIP | DT_BOTTOM | DT_RIGHT| DT_SINGLELINE);
		}
	if(gCfgIbp1.bLabel!=Ibp1bLabel){
		DrawText(hdcWaveMem, strIbpLabel[Ibp1bLabel], -1, &(pPanel->rcPara), DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);
		DrawText(hdcWaveMem, strIbpUnit[Ibp1unit], -1, &(pPanel->rcPara), DT_NOCLIP | DT_CENTER | DT_TOP | DT_SINGLELINE);
		DrawText(hdcWaveMem, strIbp1tmp2, -1, &(pPanel->rcPara), DT_NOCLIP | DT_BOTTOM | DT_RIGHT| DT_SINGLELINE);		
		DrawText(hdcWaveMem, strIbp1tmp, -1, &(pPanel->rcPara), DT_NOCLIP | DT_TOP | DT_RIGHT| DT_SINGLELINE);
		}

	SetTextColor(hdcWaveMem, COLOR_darkgray);
 	//显示信息文字
	DrawText(hdcWaveMem, strIbpLabel[gCfgIbp1.bLabel], -1, &(pPanel->rcPara), DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);
	DrawText(hdcWaveMem, strIbpUnit[gCfgIbp1.bUnit], -1, &(pPanel->rcPara), DT_NOCLIP | DT_CENTER | DT_TOP | DT_SINGLELINE);
	
 	//显示标尺
	memset(strIbpTop, 0, sizeof strIbpTop);
	memset(strIbpBottom, 0, sizeof strIbpBottom);
	if(gCfgIbp1.bUnit == IBPUNIT_MMHG){
		snprintf(strIbpTop, sizeof strIbpTop, "%d ", gCfgIbp1.iScaleTop);
		snprintf(strIbpBottom, sizeof strIbpBottom, "%d ", gCfgIbp1.iScaleBottom);
	}
	else if(gCfgIbp1.bUnit == IBPUNIT_KPA){
		snprintf(strIbpTop, sizeof strIbpTop, "%3.1f ", (float)(gCfgIbp1.iScaleTop / 7.5));
		snprintf(strIbpBottom, sizeof strIbpBottom, "%3.1f ", (float)(gCfgIbp1.iScaleBottom / 7.5));		
	}
	/*else  if(gCfgIbp1.bUnit == IBPUNIT_CMH2O){
		snprintf(strIbpTop, sizeof strIbpTop, "%d ", (int)(gCfgIbp1.iScaleTop *1.36));
		snprintf(strIbpBottom, sizeof strIbpBottom, "%d ", (int)(gCfgIbp1.iScaleBottom *1.36));		
	}*/
	

	DrawText(hdcWaveMem, strIbpTop, -1, &(pPanel->rcPara), DT_NOCLIP | DT_TOP | DT_RIGHT| DT_SINGLELINE);
	DrawText(hdcWaveMem, strIbpBottom, -1, &(pPanel->rcPara), DT_NOCLIP | DT_BOTTOM | DT_RIGHT| DT_SINGLELINE);


	//拷贝DC
	BitBlt(hdcWaveMem, 
	       pPanel->rcPara.left, pPanel->rcPara.top, RECTW(pPanel->rcPara)+1, RECTH(pPanel->rcPara)+1, 
	       hdcWave, 
	       pPanel->rcPara.left, pPanel->rcPara.top, 
	       0);
	Ibp1unit=gCfgIbp1.bUnit ;
	Ibp1bLabel=gCfgIbp1.bLabel;
	unsigned char i;
	for(i=0;i<7;i++)
	strIbp1tmp[i]=strIbpTop[i];
	for(i=0;i<7;i++)
	strIbp1tmp2[i]=strIbpBottom[i];
	return 0;
}

/*
	绘制IBP1
*/
/*
**DrawWave_Ibp1(hdcWave, hdcWaveMem, &(gNWavePanels[i]), DRAWWAVENUM_200HZ);
*/
static int DrawWave_Ibp1(HDC hdc, HDC mdc, const PNWAVE_PANEL pPanel, int iPointCount)
{
	static int x = 0;
	static int y = 0;
	static int iWaveCount = 0;
	static int iValue;
	BOOL res;
	int i, j;
	int x1, y1;
	
	//开始绘制标志 ，初始化X，Y坐标 
	if(gbResetWaves){
		x = pPanel->rcWave.left;
		y = pPanel->rcWave.top + RECTH(pPanel->rcWave)/2;
		iWaveCount = 0;
		FillBox(hdc, 
			pPanel->rcWave.left, 
			pPanel->rcWave.top, 
			RECTW(pPanel->rcWave)+1, 
			RECTH(pPanel->rcWave)+1);
		if(mdc != (HDC)NULL)
			FillBox(mdc, 
				pPanel->rcWave.left, 
				pPanel->rcWave.top, 
				RECTW(pPanel->rcWave)+1, 
				RECTH(pPanel->rcWave)+1);
		ClearData_Ibp1();
	}
	
	//波形以及文字颜色根据配置而定
	SetPenColor(hdc, gCfgIbp1.iColor);	
	if(mdc != (HDC)NULL)
		SetPenColor(mdc, gCfgIbp1.iColor);	
	
	x1 =x;
	y1 =y;
	MoveTo(hdc, x, y);
	if(mdc != (HDC)NULL)
		MoveTo(mdc, x, y);
	
	for(j=0; j<iPointCount; j++){
		//获得波形值
		res = RetrieveData_Ibp1(&Ibp1Wave);
		if((!gValueIbp1.bOff) && (res)){
			iValue = Ibp1Wave.iWave;
			y = pPanel->rcWave.top +  RECTH(pPanel->rcWave) - (iValue-gCfgIbp1.iScaleBottom)/(pPanel->fYAxis);	
		}
		else if(gValueIbp1.bOff){
			iValue = RECTH(pPanel->rcWave)/2;
 			y = pPanel->rcWave.top + iValue;
		}
		
		/*
		//测试波形
		static int iCount = 0;
 		if(iCount ==10){
//   			if(iValue < gCfgIbp2.iScaleTop ) iValue +=1;
//   			else  iValue = gCfgIbp2.iScaleBottom;
 			if(iValue < 7 ) iValue +=2;
 			else  iValue = -7;
  			iCount =0;
 		}
 		else{
 			iCount ++;
 		}
		
// 		iValue = 0;
	
 		y = pPanel->rcWave.top +  RECTH(pPanel->rcWave) - (iValue-gCfgIbp1.iScaleBottom)/(pPanel->fYAxis);	
		*/
		
		//Y
		if(y > pPanel->rcWave.bottom-2) y = pPanel->rcWave.bottom-2;
		if(y<pPanel->rcWave.top) y = pPanel->rcWave.top;
		
		//X
		if(x < pPanel->rcWave.right){
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
			FillBox(hdc, x, pPanel->rcWave.top, FILLWAVE_WIDTH, RECTH(pPanel->rcWave)+1);
			if(mdc != (HDC)NULL)
				FillBox(mdc, x, pPanel->rcWave.top, FILLWAVE_WIDTH, RECTH(pPanel->rcWave)+1);
		}

		if(x <=  pPanel->rcWave.left+1) {
			MoveTo(hdc, x, y);
			if(mdc != (HDC)NULL)
				MoveTo(mdc, x, y);
			continue;
		}
		
		if(gCfgSystem.bLineType == LINE_ANTI){
			DrawWuLine(hdc, x1, y1, x, y, COLOR_magenta);
			if(mdc != (HDC)NULL)
				DrawWuLine(mdc, x1, y1, x, y, COLOR_magenta);
		}else if(gCfgSystem.bLineType == LINE_WIDE ){
			DrawWideLine(hdc, REALWIDELINE, gbLineWidth, x1, y1, x, y, WIDELINE_DOT);
			if(mdc != (HDC)NULL)
				DrawWideLine(mdc, REALWIDELINE, gbLineWidth, x1, y1, x, y, WIDELINE_DOT);
		}else{
			LineTo(hdc, x, y);
			if(mdc != (HDC)NULL)
				LineTo(mdc, x, y);
		}
	}
		
	//画扫描棒
	FillBox(hdc, x+iPointCount, pPanel->rcWave.top, FILLWAVE_WIDTH, RECTH(pPanel->rcWave)+1);
	if(mdc != (HDC)NULL)
		FillBox(mdc, x+iPointCount, pPanel->rcWave.top, FILLWAVE_WIDTH, RECTH(pPanel->rcWave)+1);
	
	SetPenColor(hdcWaveMem,COLOR_darkgray);
	Rectangle(hdcWaveMem, 
		  pPanel->rcPara.right-165,
		  pPanel->rcPara.top+1, 
		  pPanel->rcPara.right-170,
		  pPanel->rcPara.bottom-1);
	Rectangle(hdcWaveMem, 
		  pPanel->rcPara.right-160,
		  pPanel->rcPara.bottom-1, 
		  pPanel->rcPara.right-175,
		  pPanel->rcPara.bottom-1);
	
	if(gbDebugRect) {
		Rectangle(hdcWaveMem, 
			  pPanel->rcPara.right-165,
			  pPanel->rcPara.top+1, 
			  pPanel->rcPara.right-170,
			  pPanel->rcPara.bottom-1);
		Rectangle(hdcWaveMem, 
			  pPanel->rcPara.right-160,
			  pPanel->rcPara.bottom-1, 
			  pPanel->rcPara.right-175,
			  pPanel->rcPara.bottom-1);
		}





	return 0;
}


/*
	绘制IBP2波形参数
*/
static int DrawParas_Ibp2(const PNWAVE_PANEL pPanel)
{
	/*
	//Clear Area
	FillBox(hdcWaveMem, 
		pPanel->rcPara.left, 
		pPanel->rcPara.top, 
		RECTW(pPanel->rcPara)+1, 
		RECTH(pPanel->rcPara)+1);
	*/
	SetTextColor(hdcWaveMem, COLOR_black);
	if(gCfgIbp2.bUnit !=Ibp2unit){
		DrawText(hdcWaveMem, strIbp2tmp, -1, &(pPanel->rcPara), DT_NOCLIP | DT_TOP | DT_RIGHT| DT_SINGLELINE);
		DrawText(hdcWaveMem, strIbpUnit[Ibp2unit], -1, &(pPanel->rcPara), DT_NOCLIP | DT_CENTER | DT_TOP | DT_SINGLELINE);
		DrawText(hdcWaveMem, strIbp2tmp2, -1, &(pPanel->rcPara), DT_NOCLIP | DT_BOTTOM | DT_RIGHT| DT_SINGLELINE);		
		}
	if(gCfgIbp2.bLabel!=Ibp2bLabel){
		DrawText(hdcWaveMem, strIbpLabel[Ibp2bLabel], -1, &(pPanel->rcPara), DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);
		DrawText(hdcWaveMem, strIbp2tmp, -1, &(pPanel->rcPara), DT_NOCLIP | DT_TOP | DT_RIGHT| DT_SINGLELINE);
		DrawText(hdcWaveMem, strIbp2tmp2, -1, &(pPanel->rcPara), DT_NOCLIP | DT_BOTTOM | DT_RIGHT| DT_SINGLELINE);		
		}
	SetTextColor(hdcWaveMem, COLOR_darkgray);
 	//显示信息文字
	DrawText(hdcWaveMem, strIbpLabel[gCfgIbp2.bLabel], -1, &(pPanel->rcPara), DT_NOCLIP | DT_TOP | DT_LEFT | DT_SINGLELINE);
	DrawText(hdcWaveMem, strIbpUnit[gCfgIbp2.bUnit], -1, &(pPanel->rcPara), DT_NOCLIP | DT_TOP | DT_CENTER | DT_SINGLELINE);

 	//显示标尺
	memset(strIbpTop, 0, sizeof strIbpTop);
	memset(strIbpBottom, 0, sizeof strIbpBottom);
	if(gCfgIbp2.bUnit == IBPUNIT_MMHG){
		snprintf(strIbpTop, sizeof strIbpTop, "%d ", gCfgIbp2.iScaleTop);
		snprintf(strIbpBottom, sizeof strIbpBottom, "%d ", gCfgIbp2.iScaleBottom);
	}
	else if(gCfgIbp2.bUnit == IBPUNIT_KPA){
		snprintf(strIbpTop, sizeof strIbpTop, "%3.1f ", (float)(gCfgIbp2.iScaleTop / 7.5));
		snprintf(strIbpBottom, sizeof strIbpBottom, "%3.1f ", (float)(gCfgIbp2.iScaleBottom / 7.5));		
	}
	/*else  if(gCfgIbp2.bUnit == IBPUNIT_CMH2O){
		snprintf(strIbpTop, sizeof strIbpTop, "%d ", (int)(gCfgIbp2.iScaleTop *1.36));
		snprintf(strIbpBottom, sizeof strIbpBottom, "%d ", (int)(gCfgIbp2.iScaleBottom *1.36));		
	}*/
	DrawText(hdcWaveMem, strIbpTop, -1, &(pPanel->rcPara), DT_NOCLIP | DT_TOP | DT_RIGHT| DT_SINGLELINE);
	DrawText(hdcWaveMem, strIbpBottom, -1, &(pPanel->rcPara), DT_NOCLIP | DT_BOTTOM | DT_RIGHT| DT_SINGLELINE);


	
	//拷贝DC
	BitBlt(hdcWaveMem, 
	       pPanel->rcPara.left, pPanel->rcPara.top, RECTW(pPanel->rcPara)+1, RECTH(pPanel->rcPara)+1, 
	       hdcWave, 
	       pPanel->rcPara.left, pPanel->rcPara.top, 
	       0);
	Ibp2unit=gCfgIbp2.bUnit ;
	Ibp2bLabel=gCfgIbp2.bLabel;
	unsigned char i;
	for(i=0;i<7;i++)
	strIbp2tmp[i]=strIbpTop[i];
	for(i=0;i<7;i++)
	strIbp2tmp2[i]=strIbpBottom[i];
	return 0;
}
/*
	绘制IBP2
*/
static int DrawWave_Ibp2(HDC hdc, HDC mdc, const PNWAVE_PANEL pPanel, int iPointCount)
{
	static int x = 0;
	static int y = 0;
	static int iWaveCount = 0;
	static int iValue;
	BOOL res;
	int i, j;
	int x1, y1;
	
	//开始绘制标志 ，初始化X，Y坐标 
	if(gbResetWaves){
		x = pPanel->rcWave.left;
		y = pPanel->rcWave.top + RECTH(pPanel->rcWave)/2;
		iWaveCount = 0;
		FillBox(hdc, 
			pPanel->rcWave.left, 
			pPanel->rcWave.top, 
			RECTW(pPanel->rcWave)+1, 
			RECTH(pPanel->rcWave)+1);
		if(mdc != (HDC)NULL)
			FillBox(mdc, 
				pPanel->rcWave.left, 
				pPanel->rcWave.top, 
				RECTW(pPanel->rcWave)+1, 
				RECTH(pPanel->rcWave)+1);
		ClearData_Ibp2();
	}
	
	//波形以及文字颜色根据配置而定
	SetPenColor(hdc, gCfgIbp2.iColor);	
	if(mdc != (HDC)NULL)
		SetPenColor(mdc, gCfgIbp2.iColor);	
	
	x1 =x;
	y1 =y;
	MoveTo(hdc, x, y);
	if(mdc != (HDC)NULL)
		MoveTo(mdc, x, y);
	
	for(j=0; j<iPointCount; j++){
		//获得波形值
		res = RetrieveData_Ibp2(&Ibp2Wave);
		if((!gValueIbp2.bOff) && (res)){
			iValue = Ibp2Wave.iWave;
			y = pPanel->rcWave.top +  RECTH(pPanel->rcWave) - (iValue-gCfgIbp2.iScaleBottom)/(pPanel->fYAxis);	
		}
		else if(gValueIbp2.bOff){
			iValue = RECTH(pPanel->rcWave)/2;
			y = pPanel->rcWave.top + iValue;
		}
		
		/*
		//测试波形
		static int iCount = 0;
		if(iCount ==10){
//   			if(iValue < gCfgIbp2.iScaleTop ) iValue +=1;
//   			else  iValue = gCfgIbp2.iScaleBottom;
		if(iValue < 7 ) iValue +=2;
		else  iValue = -7;
		iCount =0;
	}
		else{
		iCount ++;
	}
		
// 		iValue = 0;
	
		y = pPanel->rcWave.top +  RECTH(pPanel->rcWave) - (iValue-gCfgIbp1.iScaleBottom)/(pPanel->fYAxis);	
		*/
		//Y
		if(y > pPanel->rcWave.bottom-2) y = pPanel->rcWave.bottom-2;
		if(y<pPanel->rcWave.top) y = pPanel->rcWave.top;
		
		//X
		if(x < pPanel->rcWave.right){
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
			FillBox(hdc, x, pPanel->rcWave.top, FILLWAVE_WIDTH, RECTH(pPanel->rcWave)+1);
			if(mdc != (HDC)NULL)
				FillBox(mdc, x, pPanel->rcWave.top, FILLWAVE_WIDTH, RECTH(pPanel->rcWave)+1);
		}

		if(x <=  pPanel->rcWave.left+1) {
			MoveTo(hdc, x, y);
			if(mdc != (HDC)NULL)
				MoveTo(mdc, x, y);
			continue;
		}
		
		if(gCfgSystem.bLineType == LINE_ANTI){
			DrawWuLine(hdc, x1, y1, x, y, COLOR_magenta);
			if(mdc != (HDC)NULL)
				DrawWuLine(mdc, x1, y1, x, y, COLOR_magenta);
		}else if(gCfgSystem.bLineType == LINE_WIDE ){
			DrawWideLine(hdc, REALWIDELINE, gbLineWidth, x1, y1, x, y, WIDELINE_DOT);
			if(mdc != (HDC)NULL)
				DrawWideLine(mdc, REALWIDELINE, gbLineWidth, x1, y1, x, y, WIDELINE_DOT);
		}else{
			LineTo(hdc, x, y);
			if(mdc != (HDC)NULL)
				LineTo(mdc, x, y);
		}
	}
		
	//画扫描棒
	FillBox(hdc, x+iPointCount, pPanel->rcWave.top, FILLWAVE_WIDTH, RECTH(pPanel->rcWave)+1);
	if(mdc != (HDC)NULL)
		FillBox(mdc, x+iPointCount, pPanel->rcWave.top, FILLWAVE_WIDTH, RECTH(pPanel->rcWave)+1);
	
	SetPenColor(hdcWaveMem, COLOR_darkgray);
	Rectangle(hdcWaveMem, 
		  pPanel->rcPara.right-165,
		  pPanel->rcPara.top+1, 
		  pPanel->rcPara.right-170,
		  pPanel->rcPara.bottom-1);
	Rectangle(hdcWaveMem, 
		  pPanel->rcPara.right-160,
		  pPanel->rcPara.bottom-1, 
		  pPanel->rcPara.right-175,
		  pPanel->rcPara.bottom-1);
	
	if(gbDebugRect) {
		Rectangle(hdcWaveMem, 
			  pPanel->rcPara.right-165, 
			  pPanel->rcPara.top+1, 
			  pPanel->rcPara.right-170,
			  pPanel->rcPara.bottom-1);
		Rectangle(hdcWaveMem, 
			  pPanel->rcPara.right-160,
			  pPanel->rcPara.bottom-1, 
			  pPanel->rcPara.right-175,
			  pPanel->rcPara.bottom-1);

		}
	return 0;
}


//绘制波形参数
int DrawWaveParas()
{
	int i;

	if(giPanelCount >0){
		for(i=0; i<giPanelCount; i++){
			switch(gNWavePanels[i].bWaveID){
				case NWID_ECG1:{
 					DrawParas_Ecg1(&(gNWavePanels[i]));
				}break;
				case NWID_ECG2:{
					DrawParas_Ecg2(&(gNWavePanels[i]));
				}break;
				case NWID_MULTIECG:{
					DrawParas_MultiEcg(&(gNWavePanels[i]));
				}break;
				case NWID_SPO2:{
					DrawParas_SpO2(&(gNWavePanels[i]));
				}break;
				case NWID_RESP:{
					DrawParas_Resp(&(gNWavePanels[i]));
				}break;
				case NWID_CO2:{
					DrawParas_CO2(&(gNWavePanels[i]));
				}break;
				case NWID_IBP1:{
					DrawParas_Ibp1(&(gNWavePanels[i]));
				}break;
				case NWID_IBP2:{
					DrawParas_Ibp2(&(gNWavePanels[i]));
				}break;
				case NWID_12LEAD:{
					DrawParas_12LEADEcg(&(gNWavePanels[i]));
				}break;
			}		
		}
	}

	return 0;
}

/*
	绘制波形
	hdc:绘图DC
	bMem: TRUE-MemDC
		
*/
static int DrawWave()
{
	int i;
	
	if(giPanelCount >0){
		for(i=0; i<giPanelCount; i++){
			switch(gNWavePanels[i].bWaveID){
				case NWID_ECG1:{
   					DrawWave_Ecg1(hdcWave, hdcWaveMem, &(gNWavePanels[i]), DRAWWAVENUM_200HZ);
				}break;
				case NWID_ECG2:{
   					DrawWave_Ecg2(hdcWave, hdcWaveMem, &(gNWavePanels[i]), DRAWWAVENUM_200HZ);
				}break;
				case NWID_MULTIECG:{
					DrawWave_Multi(hdcWave, hdcWaveMem, &(gNWavePanels[i]), DRAWWAVENUM_200HZ);
				}break;
				case NWID_SPO2:{
					DrawWave_SpO2(hdcWave, hdcWaveMem, &(gNWavePanels[i]), DRAWWAVENUM_200HZ);
				}break;
				case NWID_RESP:{
					DrawWave_Resp(hdcWave, hdcWaveMem, &(gNWavePanels[i]), DRAWWAVENUM_200HZ);
				}break;
				case NWID_CO2:{
					DrawWave_CO2(hdcWave, hdcWaveMem, &(gNWavePanels[i]), DRAWWAVENUM_200HZ);
				}break;
				case NWID_IBP1:{
					DrawWave_Ibp1(hdcWave, hdcWaveMem, &(gNWavePanels[i]), DRAWWAVENUM_200HZ);
				}break;
				case NWID_IBP2:{
					DrawWave_Ibp2(hdcWave, hdcWaveMem, &(gNWavePanels[i]), DRAWWAVENUM_200HZ);
				}break;
				case NWID_12LEAD:{
					DrawWave_12LEAD(hdcWave, hdcWaveMem, &(gNWavePanels[i]), DRAWWAVENUM_200HZ);
				}break;
			}		
		}
		gbResetWaves = FALSE;
	}
				
	return 0;
}

/*
	拷贝memDC
*/
int RestorWave()
{
	
//  	return 0;
	
	usleep(10000);
	
	BitBlt(hdcWaveMem, 
		gNRcWaves.left, gNRcWaves.top, RECTW(gNRcWaves)+1, RECTH(gNRcWaves)+1, 
		hdcWave, 
		gNRcWaves.left, gNRcWaves.top, 
		0);
	return 0;
	
	usleep(10000);
	//拷贝两次，防止拷贝失败
	BitBlt(hdcWaveMem, 
	       gNRcWaves.left, gNRcWaves.top, RECTW(gNRcWaves)+1, RECTH(gNRcWaves)+1, 
	       hdcWave, 
	       gNRcWaves.left, gNRcWaves.top, 
	       0);
		
	return 0;
}

/*
	初始化绘图环境的一些通用属性
*/
static int InitWaveDC()
{
	if(hdcWave == (HDC)NULL || hdcWaveMem == (HDC)NULL) return -1;
	
	SetBkMode(hdcWave, BM_TRANSPARENT);
	SetBkMode(hdcWaveMem, BM_TRANSPARENT);
	//背景色
	SetBkColor(hdcWave, WAVE_BKCOLOR);
 	SetBrushColor(hdcWave, WAVE_BKCOLOR);
	
	SetBkColor(hdcWaveMem, WAVE_BKCOLOR);
	SetBrushColor(hdcWaveMem, WAVE_BKCOLOR);
	//字体
	SelectFont(hdcWave, gFontSystem);
	SelectFont(hdcWaveMem, gFontSystem);
	
//	SetRasterOperation(hdcWave,ROP_OR);
//	SetRasterOperation(hdcWaveMem,ROP_OR);
//	printf("!!!!!!GetRasterOperation=%d\n",GetRasterOperation(hdcWave));
	return 0;
}


//绘图线程
void *NThreadProcDrawWave(void *arg)
{
	//获得绘图环境
 	hdcWave = GetClientDC(ghWndMain);	
	//建立屏幕DC拷贝
	hdcWaveMem = CreateCompatibleDC(hdcWave);
	
	if(hdcWaveMem == (HDC)NULL){
		printf("%s:%d Create MemDC for drawing waveform failure!\n", __FILE__, __LINE__);
// 		return ;
	}
	
	if(hdcWave == (HDC)NULL){
		printf("%s:%d Create HDC for drawing waveform failure!\n", __FILE__, __LINE__);
		return ;
	}
	
	
	//初始化绘图环境的一些通用属性
	InitWaveDC();
	
	//计算ECG标尺的长度
	CalcEcgRuler();
			
	
	//等待绘图信号量，然后绘图
	for(;;){
// 		int semValue;
// 		sem_getvalue(&semNDrawWave, &semValue);
// 		printf("sem value %d.\n", semValue);
		sem_wait(&semNDrawWave);
	
		if(gbNCanDrawWave){
			//绘制波形
			DrawWave();
// 			if(gbViewDemoData)
// 				DrawDemo(hdcWave, hdcWaveMem);
		}
	}
}


/////////////////// interface /////////////////
int NewCreateDrawWaveProc()
{
	int res;
	
	//创建绘图线程
 	res = pthread_create(&ptNDrawWave, NULL, NThreadProcDrawWave, NULL);
	
	
	return res;
}





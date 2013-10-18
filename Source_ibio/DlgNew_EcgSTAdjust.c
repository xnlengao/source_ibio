/*************************************************************************** 
 *	Module Name:	DlgNew_EcgSTAdjust
 *
 *	Abstract:Ecg设置中的报警设置选项
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2013-1-17 	16:14:18
 *	
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"
#include "DrawWave.h"
//DID_NEWECGST
#define DID_NEWECGST_HELP			DID_NEWECGST
#define DID_NEWECGST_OK			DID_NEWECGST+1
#define DID_NEWECGST_CAPTION		DID_NEWECGST+2
#define DID_NEWECGST_ISO			DID_NEWECGST+3
#define DID_NEWECGST_ST			DID_NEWECGST+4
#define DID_NEWECGST_STWP		DID_NEWECGST+5

//ST模板框架颜色
#define COLOR_STFRAME	COLOR_darkgray	
//ST模板的背景颜色
#define COLOR_STBK		COLOR_black
//ST模板IOS 线条颜色
#define COLOR_IOSLine		COLOR_blue
//ST模板ST线条颜色
#define COLOR_STLine		COLOR_yellow
//ST模板R波线条颜色
#define COLOR_RWaveLine		COLOR_red

//帮助对话框
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;

//其他控件 
static HWND btnOk;
static HWND btnISO;
static HWND btnST;

//控件回调函数
static WNDPROC OldBtnProc;
static WNDPROC OldCobProc;
static WNDPROC OldSTwpProc;

//ST波形绘制区域
static RECT gRcSTFrame;	
//ST IOS ST参数绘制区域
static RECT gRcSTPara;

BYTE ISO_X,ST_X;
short ISO_Value,ST_Value; 

//ECG定标
extern float gfNMMToPixel;				///1mm=3.2pixel
extern float gSamplesPerMV;	

extern float gSTStatus;

extern BYTE gST1Wave[250];
extern BYTE gST2Wave[250];
extern int gST_templet[250];
static int TempST1Wave[250];
static BYTE TempST2Wave[250];
//波形显示面板
static WAVE_PANEL	STwp;

//光标移动按钮是否按下
static BOOL gbBtnISO = FALSE;
//页面按钮是否按下
static BOOL gbBtnST = FALSE;
//绘制波形
static int DrawWave_Ecg(PWAVE_PANEL pPanel);

//对话框属性
static DLGTEMPLATE DlgSet= {
	//WS_VISIBLE | WS_CAPTION | WS_BORDER,
	WS_VISIBLE | WS_BORDER,
	WS_EX_NOCLOSEBOX,	
	DLG_X, DLG_Y, DLG_LENGHT, DLG_HIGH,	
	"",
	0, 0,
	1,		
	NULL,
	0
};

static CTRLDATA CtrlSet[] = {
	{
		"static",
		WS_CHILD | WS_VISIBLE | SS_LEFT ,
		//WS_CHILD | WS_VISIBLE | SS_LEFT|SS_WHITERECT ,
		5,DLG_HIGH-50, DLG_LENGHT-15, 40,
		DID_NEWECGST_HELP,
		"",
		0
	}
};


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
				case DID_NEWECGST_ISO:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_STADJUST, "help_iso", strHelp, sizeof strHelp,"Set ISO point.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWECGST_ST:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_STADJUST, "help_st", strHelp, sizeof strHelp,"Set ST point.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWECGST_OK:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "help_previous", strHelp, sizeof strHelp,"Return to the previous menu.");
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
	int interface;
	
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
						case DID_NEWECGST_OK:{
							
						}break;
					}	
					gbKeyType = KEY_TAB;
				}break;
			}
			return 0;
		}break;	
	}	
	temp = (*OldCobProc)(hWnd, message, wParam, lParam);
	return(temp);}

/*
	按钮消息回调函数
*/
static int BtnProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{

	int temp;
	int id;
	id = GetDlgCtrlID(hWnd);
	switch(message){
		case MSG_SETFOCUS:{
			if(gbBtnISO || gbBtnST){
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
						case DID_NEWECGST_ISO:{
							gbBtnISO = !gbBtnISO;
							if(gbBtnISO){
								//gbKeyType = KEY_LR;
								gbKeyType = KEY_AD;
								SetWindowBkColor(hWnd, BROWSE_COLOR);
							} 
							else{
								gbKeyType = KEY_TAB;
								SetWindowBkColor(hWnd, SETFOCUS_COLOR);	
								gCfgEcg.bISO= ISO_Value/4;
								SetIntValueToResFile(gFileSetup, "ECGSetup", "ios_value",  gCfgEcg.bISO, 3);
							//	SetiBioEcgISOandST();
								Set_Ecg_ST_ISO();
							}
						}break;		
						case DID_NEWECGST_ST:{
							gbBtnST = !gbBtnST;
							if(gbBtnST){
								//gbKeyType = KEY_LR;
								gbKeyType = KEY_AD;
								SetWindowBkColor(hWnd, BROWSE_COLOR);
							} 
							else{
								gbKeyType = KEY_TAB;
								SetWindowBkColor(hWnd, SETFOCUS_COLOR);	
								gCfgEcg.bST= ST_Value/4;
								
								SetIntValueToResFile(gFileSetup, "ECGSetup", "st_value",  gCfgEcg.bST, 3);
							//	SetiBioEcgISOandST();
								Set_Ecg_ST_ISO();
							}
						}break;	
					
						
					}//end switch(id)
				}break;
				//case SCANCODE_CURSORBLOCKLEFT:{
				case SCANCODE_A:{
					if(gbBtnISO){
						SetFocus(btnISO);
						if(ISO_Value<400)
							ISO_Value+=4;
						ISO_X=ISO_Value/4;
						UpdateWindow(STwp.hWnd,TRUE);
					//	GraphMoveCursor(hWndSelf, CURSOR_LEFT);
						return TRUE;
 					} 
					if(gbBtnST){
						SetFocus(btnST);
						if(ST_Value<400)
							ST_Value+=4;
						ST_X=ST_Value/4;
					
						UpdateWindow(STwp.hWnd,TRUE);
						return TRUE;
					} 
				}break;
				//case SCANCODE_CURSORBLOCKRIGHT:{
				case SCANCODE_D:{
					if(gbBtnISO){
						SetFocus(btnISO);
						if(ISO_Value>4)
							ISO_Value-=4;
						ISO_X=ISO_Value/4;
						UpdateWindow(STwp.hWnd,TRUE);
						return TRUE;
					} 
					if(gbBtnST){
						SetFocus(btnST);
						if(ST_Value>4)
							ST_Value-=4;
						ST_X=ST_Value/4;
						UpdateWindow(STwp.hWnd,TRUE);
						return TRUE;
					} 
 					
				}break;
			}//end switch(wParam)
		}//end MSG_KEYUP
	}
	temp = (*OldBtnProc)(hWnd, message, wParam, lParam);
	return(temp);
}
static int STwpProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	
	id = GetDlgCtrlID(hWnd);
		
	temp = (*OldSTwpProc)(hWnd, message, wParam, lParam);
	
	switch(message){
		case MSG_PAINT:{
			DrawWave_Ecg(&STwp);
			return TRUE;
		}break;
	}
	
	return(temp);
}

//初始化波形绘图环境的属性(与绘图相同)
static int InitWavePanels()
{
	
	int iValueMax;
	int iValueMin;
	float fYAxis=0.0;	//y轴比例系数
	RECT rcPanel;		//波形面板的大小
	int i;

	///1个面板绘制的波形数量
		STwp.bWaveCount = 1;
		//X轴缩放系数(25mm/s)
		STwp.fXAxis = 1.22;
// 		STwp.fXAxis = 2.43;
		//X轴步进值
		STwp.iXStep = 1;
		//Y轴缩放系数(ECG 1x)
		//取得并设置波形的范围
		
		switch(STwp.bID){
			case WID_ECG1:{ 
				iValueMax = 4095;	
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
		STwp.iYMax = iValueMax;
		STwp.iYMin = iValueMin;
		
		//根据波形的范围和面板的实际大小，计算波形在Y轴上的缩放系数
		if(STwp.hWnd == (HWND)NULL ) return -1;
		GetClientRect(STwp.hWnd, &rcPanel);
				
		fYAxis = (float)((float)(iValueMax-iValueMin) / (float)((float)(rcPanel.bottom-rcPanel.top)/(float)(STwp.bWaveCount)));	
		//STwp.fYAxis = fYAxis;
		STwp.fYAxis = fYAxis;
		//ECG定标

		STwp.fSampleToPixel = (float)(((float)(gSamplesPerMV))/((float)10.0*gfNMMToPixel));
	//	if(B_PRINTF) printf("STwp.fSampleToPixel :%f fYAxis :%f \n",STwp.fSampleToPixel ,STwp.fYAxis);
	
	
	return 0;
}
extern float gfEcgSampleToPixel;
//绘制波形
static int DrawWave_Ecg(PWAVE_PANEL pPanel)
{
	int i, j;
	unsigned char strISO[20]={0};
	unsigned char strST[20]={0};
	static int x=0, y=BASELINE_ECG;
	HDC hdc;
	RECT rcPanel;
	int iTemp;
	int RWaveX;
	GetClientRect(pPanel->hWnd, &rcPanel);

	hdc = GetClientDC(pPanel->hWnd);
	if(hdc == (HDC)NULL){
		if(B_PRINTF) printf("HDC is NULL!\n");
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


	
	
		
	//TextOut(hdc, 5, 5, "ECG1");
	/*
	SetPenColor(hdc, COLOR_yellow);
	x=20+RECTW(rcPanel)/2;
	y=0;
	MoveTo(hdc, x, y);
	LineTo(hdc, x,y+150);
	*/
//	x = (int)((double)rcPanel.right*(double)pPanel->fXAxis);
	x=0;
	y =(4095-TempST1Wave[0]) / pPanel->fYAxis;
	
	SetPenColor(hdc, gCfgEcg.iColor);
	
	
	MoveTo(hdc, x, y);
	//绘制1秒钟的波形
	
		for(j=0; j<250; j++){
		//	iTemp = gST1Wave[j];
			iTemp =TempST1Wave[j];                 
	
			//y =  (4095-iTemp) / pPanel->fYAxis;

			y = rcPanel.top 
					+  RECTH(rcPanel) - iTemp/(gfEcgSampleToPixel) 
					+ (BASELINE_ECG/gfEcgSampleToPixel - RECTH(rcPanel)/2);

		
			//y轴增益
		
			if(y>rcPanel.bottom-1) y = rcPanel.bottom-1;
			if(y<rcPanel.top) y = rcPanel.top;
			/*
			if(B_PRINTF) printf("%d ",y);
			if((j+1)%10==0)
					if(B_PRINTF) printf("\n%d :",(j+1));
			*/
			LineTo(hdc, x, y);
			x+=pPanel->iXStep;
			if(j==125)RWaveX=x;
		}


	x = 0;

	
	//R波位置
	SetPenColor(hdc, COLOR_RWaveLine);
	SetTextColor(hdc, COLOR_RWaveLine);
	
	MoveTo(hdc, rcPanel.left+RWaveX, rcPanel.top);
	LineTo(hdc,  rcPanel.left+RWaveX, rcPanel.bottom);

	//ISO 位置线
	SetPenColor(hdc, COLOR_IOSLine);
	SetTextColor(hdc, COLOR_IOSLine);
	snprintf(strISO, sizeof strISO, "ISO %d ms", ISO_Value);
	MoveTo(hdc, rcPanel.left+RWaveX-ISO_X, rcPanel.top);
	LineTo(hdc,  rcPanel.left+RWaveX-ISO_X, rcPanel.bottom-15);
	TextOut(hdc, rcPanel.left +30, rcPanel.bottom-15, strISO);

	//ST 位置线
	SetPenColor(hdc, COLOR_STLine);
	SetTextColor(hdc, COLOR_STLine);
	
	snprintf(strST, sizeof strST, "ST %d ms", ST_Value);
	MoveTo(hdc, rcPanel.left+RWaveX+ST_X, rcPanel.top);
	LineTo(hdc,  rcPanel.left+RWaveX+ST_X, rcPanel.bottom-15);
	TextOut(hdc, rcPanel.right -50, rcPanel.bottom-15, strST);
	
	ReleaseDC(hdc);
	
	return 0;
}

//----------------- ST校准绘制函数 -------------------
/**
	初始化ST校准区域,对话框建立时调用
	@param	hWnd: 绘图的窗体
*/
static int InitSTRects(HWND hWnd)
{
	RECT rc;
	
	GetClientRect(hWnd, &rc);
	
	//绘图区域
	SetRect(&gRcSTFrame, rc.left+10, rc.top+40, rc.right-10, rc.top +150);
	
	//IOS ST参数区
	SetRect(&gRcSTPara,rc.left+10, rc.top+150, rc.right-10, rc.top +170);
	

	return 0;
}
/**
	绘制ST模板框架
 */
static int DrawSTFrame(HWND hWnd)
{
	int i;
	HDC hdc;
	
	hdc = GetClientDC(hWnd);
	if(hdc == (HDC)NULL) return -1;
		
	SetBrushColor(hdc, COLOR_black);
	FillBox(hdc, 
		gRcSTFrame.left,
		gRcSTFrame.top, 
		gRcSTFrame.right - gRcSTFrame.left,
		gRcSTFrame.bottom - gRcSTFrame.top+20);

	SetPenColor(hdc, COLOR_red);
	
	//STWave
	Rectangle(hdc, 
			  gRcSTFrame.left, 
			  gRcSTFrame.top, 
			  gRcSTFrame.right, 
			  gRcSTFrame.bottom);

	//STPara
	Rectangle(hdc, 
		  gRcSTPara.left, 
		  gRcSTPara.top, 
		  gRcSTPara.right, 
		  gRcSTPara.bottom);

	ReleaseDC(hdc);
	
	return 0;
}

/**
	初始化ST校准图像
 */
static int InitSTAdjust(HWND hWnd)
{
	HDC hdc;
	long int res;
	
	if(hWnd == (HWND)NULL) return -1;
	InitSTRects(hWnd);
	
	//获得绘图DC
	hdc = GetClientDC(hWnd);
	if(hdc == (HDC)NULL) return -1;
	
	//DrawSTFrame(hWnd);
	
//	DrawIOSSTLine(STwp.hWnd);
	
	ReleaseDC(hdc);
			
	return 0;
}

/*
	创建控件函数
*/
static int CreateCtlProc(HWND hDlg)
{
	editCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE,
					     DID_NEWECGST_CAPTION, 0, 5, 240, 25, hDlg, 0);
	editHelp = GetDlgItem(hDlg, DID_NEWECGST_HELP);

	btnISO = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECGST_ISO, 10, 190, 125, 26, hDlg, 0);
	btnST = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECGST_ST, 10, 220, 125, 26, hDlg, 0);
	
	btnOk = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECGST_OK, 10, 250, 125, 26, hDlg, 0);

	//参数显示面板
	STwp.hWnd= CreateWindow("static", "", WS_CHILD | SS_BLACKRECT,
					DID_NEWECGST_STWP,20, 40, DLG_LENGHT-45+5, 120, hDlg, 0);	
	STwp.bID = WID_ECG1;
	ShowWindow(STwp.hWnd, SW_SHOW);
		gbBtnISO = FALSE;
			gbBtnST = FALSE;
	return 0;
}
static int InitCtlName(HWND hDlg)
{
	char strMenu[100];

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_STADJUST, "caption", strMenu, sizeof strMenu,"ST Adjust");
	SetWindowText(editCaption, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "previous", strMenu, sizeof strMenu,"Previous");	
	SetWindowText(btnOk, strMenu);
	
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_STADJUST, "iso", strMenu, sizeof strMenu,"<< ISO >>");	
	SetWindowText(btnISO, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_STADJUST, "st", strMenu, sizeof strMenu,"<< ST >>");	
	SetWindowText(btnST, strMenu);
	
	return 0;
}


static int InitCtlCallBackProc(HWND hDlg)
{

	OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnISO, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnST, BtnProc);
	
	OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp);
	OldSTwpProc = SetWindowCallbackProc(STwp.hWnd, STwpProc);
	return 0;
}

static int InitCtlStatus(HWND hDlg)
{
	
	
	return 0;
}

/*
	对话框回调函数
*/
static int DlgProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	int temp;
	char strMenu[100];
	int i;
	RECT rcDlg;
	RECT rcCaption;
	switch(message){
		case MSG_INITDIALOG:{
			ISO_X=gCfgEcg.bISO;
			ST_X=gCfgEcg.bST;
			ISO_Value=ISO_X*4;
			ST_Value=ST_X*4;
			if(!B_PRINTF) printf("ISO_Value:%d ST_Value:%d\n",ISO_Value,ST_Value);
			for(i=0;i<250;i++){
				//	TempST1Wave[i]=2047;
					TempST1Wave[i]=gST_templet[i];
				if(i%10==0)printf("\n");
				printf("%4d ",TempST1Wave[i]-2047);
				
			}
			memset(gST_templet,0,sizeof(gST_templet));
			/*
			if(!gSTStatus){
				for(i=0;i<250;i++)
					TempST1Wave[i]=128;
			}
			*/
			//创建控件
			CreateCtlProc(hDlg);
			//设置控件字符
			InitCtlName(hDlg);
			//设置控件回调函数
			InitCtlCallBackProc(hDlg);
			//设置控件状态
			InitCtlStatus(hDlg);
			//初始化波形绘图环境的属性(与绘图相同)
	 		InitWavePanels();
			
			
			
			/*
			for(i=0;i<250;i++){
				if(B_PRINTF) printf("%d ",gST1Wave[i]);
				if((i+1)%10==0)
					if(B_PRINTF) printf("\n%d :",(i+1));
			}
			*/
			
			//InitSTAdjust(hDlg);
			gbKeyType = KEY_TAB;
			SetFocus(btnOk);
		}break;
		case MSG_PAINT:{
		
			DrawWave_Ecg(&STwp);
			
		}break;
		case MSG_COMMAND:{
			int	id  = LOWORD(wParam);
			switch(id){
				case DID_NEWECGST_OK:{
					PostMessage(hDlg, MSG_CLOSE, 0, 0L);
					}break;
				}
		}break;	
		case MSG_KEYUP:{
			switch(wParam){
				case SCANCODE_ESCAPE:{
					PostMessage(hDlg, MSG_CLOSE, 0, 0L);
				}break;
			}//end switch
		}break;
		case MSG_CLOSE:{
			gbKeyType = KEY_TAB;	
			//恢复波形位置
			Get_ST_templet();
			Get_Ecg_ST_ISO();
			EndDialog(hDlg, wParam);	
		}break;
	}
	
	temp = DefaultDialogProc(hDlg, message, wParam, lParam);
	return(temp);
}


/*
	建立对话框
*/
void CreateDlgNewECGSTAdjust(HWND hWnd)
{
 	DlgSet.controls = CtrlSet;
 	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}


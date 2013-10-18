/*************************************************************************** 
 *	Module Name:	DlgSet_Color
 *
 *	Abstract:	设置Color信息的主对话框
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2007-06-15 17:24:17
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"

//DID_SETCOLOR
#define DID_COLOR_HELP			DID_SETCOLOR
#define DID_COLOR_OK			DID_SETCOLOR+1
#define DID_COLOR_SET_ECG		DID_SETCOLOR+2
#define DID_COLOR_SET_SPO2		DID_SETCOLOR+3
#define DID_COLOR_SET_RESP		DID_SETCOLOR+4
#define DID_COLOR_SET_NIBP		DID_SETCOLOR+5
#define DID_COLOR_SET_TEMP		DID_SETCOLOR+6
#define DID_COLOR_SET_IBP1		DID_SETCOLOR+7
#define DID_COLOR_SET_IBP2		DID_SETCOLOR+8
#define DID_COLOR_SET_CO2		DID_SETCOLOR+9
#define DID_COLOR_SET_CAPTION		DID_SETCOLOR+10
#define DID_COLOR_SET_DEFAULT		DID_SETCOLOR+11


//帮助对话框
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;
//其他控件
static HWND btnOk;
static HWND btnSetEcg;
static HWND btnSetSpO2;
static HWND btnSetResp;
static HWND btnSetNibp;
static HWND btnSetTemp;
static HWND btnSetIbp1;
static HWND btnSetIbp2;
static HWND btnSetCo2;
static HWND btnColorDefault;

static RECT rcColorEcg;
static RECT rcColorSpO2;
static RECT rcColorResp;
static RECT rcColorNibp;
static RECT rcColorTemp;
static RECT rcColorIbp1;
static RECT rcColorIbp2;
static RECT rcColorCo2;
static int icolor=0;


//控件回调函数
static WNDPROC OldBtnProc;
static WNDPROC OldBtnProcColor;

//对话框属性
static DLGTEMPLATE DlgSet= {
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
		5,DLG_HIGH-50, DLG_LENGHT-15, 40,
		DID_COLOR_HELP,
		"",
		0
		
	}
};

static HWND hWndSelf = (HWND)NULL;

//选择的颜色
static int iSelectedColor = 0;

/*
	在指定区域显示颜色
*/
static int ViewColor(HWND hWnd,  RECT rc, int iColor)
{
	if(hWnd == (HWND)NULL) return -1;
	
	HDC hdc;
// 	RECT rc;

	hdc = GetClientDC(hWnd);
	if(hdc == (HDC)NULL) return -1;
// 	GetClientRect(hWnd, &rc);
	SetBrushColor(hdc, iColor);
	FillBox(hdc, rc.left, rc.top, RECTW(rc), RECTH(rc));
	
	
	ReleaseDC(hdc);
	
//  	SetWindowBkColor(hWnd, iColor);
	
	return 0;
}

/*
	更新颜色配置
*/
static int UpdateColors()
{
	ViewColor(hWndSelf, rcColorEcg, gCfgEcg.iColor);
	ViewColor(hWndSelf, rcColorSpO2, gCfgSpO2.iColor);
	ViewColor(hWndSelf, rcColorNibp, gCfgNibp.iColor);
	ViewColor(hWndSelf, rcColorResp, gCfgResp.iColor);
	ViewColor(hWndSelf, rcColorTemp, gCfgTemp.iColor);
	ViewColor(hWndSelf, rcColorIbp1, gCfgIbp1.iColor);
	ViewColor(hWndSelf, rcColorIbp2, gCfgIbp2.iColor);
	ViewColor(hWndSelf, rcColorCo2, gCfgCO2.iColor);
	
	return 0;
}

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
				case DID_COLOR_SET_ECG:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_COLOR, "help_ecg", strHelp, sizeof strHelp,"Set display color for ECG.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_COLOR_SET_SPO2:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_COLOR, "help_spo2", strHelp, sizeof strHelp,"Set display color for Spo2.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_COLOR_SET_RESP:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_COLOR, "help_resp", strHelp, sizeof strHelp,"Set display color for Resp.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_COLOR_SET_NIBP:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_COLOR, "help_nibp", strHelp, sizeof strHelp,"Set display color for NIBP.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_COLOR_SET_TEMP:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_COLOR, "help_temp", strHelp, sizeof strHelp,"Set display color for TEMP.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_COLOR_SET_IBP1:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_COLOR, "help_ibp1", strHelp, sizeof strHelp,"Set display color for IBP1.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_COLOR_SET_IBP2:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_COLOR, "help_ibp2", strHelp, sizeof strHelp,"Set display color for IBP2.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_COLOR_SET_CO2:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_COLOR, "help_co2", strHelp, sizeof strHelp,"Set display color for CO2");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_COLOR_SET_DEFAULT:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_COLOR, "help_default", strHelp, sizeof strHelp,"Set default display color.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_COLOR_OK:{
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
	按钮消息回调函数
*/
static int BtnProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	
	switch(message){
		case MSG_SETFOCUS:{
			SetWindowBkColor(hWnd, SETFOCUS_COLOR);
			SendMessage(editHelp, MSG_PAINT, 0, (LPARAM)GetDlgCtrlID(hWnd));
		}break;
		case MSG_KILLFOCUS:{
			SetWindowBkColor(hWnd, KILLFOCUS_COLOR);
		}break;	
	}
	
	temp = (*OldBtnProc)(hWnd, message, wParam, lParam);
	return(temp);
}

static int BtnProcColor(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	
	temp = (*OldBtnProcColor)(hWnd, message, wParam, lParam);
	switch(message){
		case MSG_PAINT:{
			return TRUE;
		}break;
	}
	
	
	return(temp);
}

static int SetDefualtColor(HWND hDlg)
{

	gCfgEcg.iColor = COLOR_green;
	gCfgSpO2.iColor = COLOR_cyan;
	gCfgPulse.iColor = COLOR_cyan;
	gCfgResp.iColor = COLOR_yellow;
	gCfgNibp.iColor = COLOR_lightwhite;
	gCfgTemp.iColor = COLOR_magenta;
	gCfgIbp1.iColor = 243; //浅粉色
	gCfgIbp2.iColor = COLOR_darkred;
	gCfgCO2.iColor = COLOR_yellow;
	
	SetIntValueToResFile(gFileSetup, "ECGSetup", "color",  gCfgEcg.iColor, 3);
	SetIntValueToResFile(gFileSetup, "SPO2Setup", "color",  gCfgSpO2.iColor, 3);
	SetIntValueToResFile(gFileSetup, "PULSESetup", "color",  gCfgPulse.iColor, 3);
	SetIntValueToResFile(gFileSetup, "RESPSetup", "color",  gCfgResp.iColor, 3);
	SetIntValueToResFile(gFileSetup, "NIBPSetup", "color",  gCfgNibp.iColor, 3);
	SetIntValueToResFile(gFileSetup, "TEMPSetup", "color",  gCfgTemp.iColor, 3);
	SetIntValueToResFile(gFileSetup, "IBP1Setup", "color",  gCfgIbp1.iColor, 3);
	SetIntValueToResFile(gFileSetup, "IBP2Setup", "color",  gCfgIbp2.iColor, 3);
	SetIntValueToResFile(gFileSetup, "CO2Setup", "color",  gCfgCO2.iColor, 3);
	
	return 0;
}

/*
	对话框回调函数
*/
static int DlgProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	char strMenu[100];
	int i;
	RECT rcDlg;
		
	switch(message){
		case MSG_INITDIALOG:{
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_COLOR, "caption", strMenu, sizeof strMenu,"Set Color");	
			editCaption = CreateWindow("static", strMenu, WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE, 
					     DID_COLOR_SET_CAPTION, 0, 5, 240, 25, hDlg, 0);


			gbKeyType = KEY_TAB;
			
			hWndSelf = hDlg;
			
			//Set ECG
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "ecg", strMenu, sizeof strMenu,"ECG");	
			btnSetEcg  = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_COLOR_SET_ECG, 10, 30+29*0, 125, 25, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnSetEcg, BtnProc);
			
			SetRect(&rcColorEcg, 140, 30+29*0, 230, 60+29*0-5);
			
			//Set SpO2
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "spo2", strMenu, sizeof strMenu,"Spo2");	
			btnSetSpO2 = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_COLOR_SET_SPO2, 10, 30+29*1, 125, 25, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnSetSpO2, BtnProc);
			
			SetRect(&rcColorSpO2, 140, 30+29*1,230, 60+29*1-5);
			
			
			//Set NIBP
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "nibp", strMenu, sizeof strMenu,"NIBP");	
			btnSetNibp = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_COLOR_SET_NIBP, 10, 30+29*2, 125, 25, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnSetNibp, BtnProc);
			
			SetRect(&rcColorNibp, 140, 30+29*2, 230, 60+29*2-5);
			
			//Set Resp
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "resp", strMenu, sizeof strMenu,"Resp");	
			btnSetResp = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_COLOR_SET_RESP, 10, 30+29*3, 125, 25, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnSetResp, BtnProc);

			SetRect(&rcColorResp, 140, 30+29*3, 230, 60+29*3-5);
		
			//Set Temp
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "temp", strMenu, sizeof strMenu,"Temp");	
			btnSetTemp = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_COLOR_SET_TEMP, 10, 30+29*4, 125, 25, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnSetTemp, BtnProc);
			
			SetRect(&rcColorTemp, 140, 30+29*4,230, 60+29*4-5);
			
			//Set Ibp1
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "ibp1", strMenu, sizeof strMenu,"IBP1");	
			btnSetIbp1 = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_COLOR_SET_IBP1, 10, 30+29*5, 125, 25, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnSetIbp1, BtnProc);
			if(!gbHaveIbp){
				EnableWindow(btnSetIbp1, FALSE);
			}
			SetRect(&rcColorIbp1, 140, 30+29*5, 230, 60+29*5-5);
			
			//Set Ibp2
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "ibp2", strMenu, sizeof strMenu,"IBP2");	
			btnSetIbp2 = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_COLOR_SET_IBP2, 10, 30+29*6, 125, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnSetIbp2, BtnProc);
			if(!gbHaveIbp){
				EnableWindow(btnSetIbp2, FALSE);
			}
			SetRect(&rcColorIbp2, 140, 30+29*6,230, 60+29*6-5);

			//Set CO2
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "co2", strMenu, sizeof strMenu,"CO2");	
			btnSetCo2 = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_COLOR_SET_CO2, 10, 30+29*7, 125, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnSetCo2, BtnProc);
			if(!gbHaveCo2){
				EnableWindow(btnSetCo2, FALSE);
			}
			SetRect(&rcColorCo2, 140, 30+29*7, 230, 60+29*7-5);

			//default
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_COLOR, "default", strMenu, sizeof strMenu,"Default");	
			btnColorDefault = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_COLOR_SET_DEFAULT, 10, 30+29*8, 125, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnColorDefault, BtnProc);
			
			//Exit
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "previous", strMenu, sizeof strMenu,"Previous");	
			btnOk = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_COLOR_OK, 10, 30+29*9, 125, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);
				
			editHelp = GetDlgItem(hDlg, DID_COLOR_HELP);
			OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp); 
			
			SetFocus(btnOk);
		}break;
		case MSG_PAINT:{
			
			UpdateColors(hWndSelf);
		}break;
		case MSG_COMMAND:{
			int	id  = LOWORD(wParam);
			
			switch(id){
				case DID_COLOR_SET_ECG:{
					GetWindowRect(btnSetEcg, &gRC_Input);
					iSelectedColor = ColorSelect(hDlg, gRC_Input, gCfgEcg.iColor);
					gCfgEcg.iColor = iSelectedColor;
					SetIntValueToResFile(gFileSetup, "ECGSetup", "color",  gCfgEcg.iColor, 3);
				}break;
				case DID_COLOR_SET_SPO2:{
					GetWindowRect(btnSetSpO2, &gRC_Input);
					iSelectedColor = ColorSelect(hDlg, gRC_Input, gCfgSpO2.iColor);
					gCfgSpO2.iColor = iSelectedColor;
					gCfgPulse.iColor = iSelectedColor;
					SetIntValueToResFile(gFileSetup, "SPO2Setup", "color",  gCfgSpO2.iColor, 3);
					SetIntValueToResFile(gFileSetup, "PULSESetup", "color",  gCfgPulse.iColor, 3);
				}break;
				case DID_COLOR_SET_RESP:{
					GetWindowRect(btnSetResp, &gRC_Input);
					iSelectedColor = ColorSelect(hDlg, gRC_Input, gCfgResp.iColor);
					gCfgResp.iColor = iSelectedColor;
					SetIntValueToResFile(gFileSetup, "RESPSetup", "color",  gCfgResp.iColor, 3);
				}break;
				case DID_COLOR_SET_NIBP:{
					GetWindowRect(btnSetNibp, &gRC_Input);
					iSelectedColor = ColorSelect(hDlg, gRC_Input, gCfgNibp.iColor);
					gCfgNibp.iColor = iSelectedColor;
					SetIntValueToResFile(gFileSetup, "NIBPSetup", "color",  gCfgNibp.iColor, 3);
				}break;
				case DID_COLOR_SET_TEMP:{
					GetWindowRect(btnSetTemp, &gRC_Input);
					iSelectedColor = ColorSelect(hDlg, gRC_Input, gCfgTemp.iColor);
					gCfgTemp.iColor = iSelectedColor;
					SetIntValueToResFile(gFileSetup, "TEMPSetup", "color",  gCfgTemp.iColor, 3);
				}break;
				case DID_COLOR_SET_IBP1:{
					GetWindowRect(btnSetIbp1, &gRC_Input);
					iSelectedColor = ColorSelect(hDlg, gRC_Input, gCfgIbp1.iColor);
					gCfgIbp1.iColor = iSelectedColor;
					SetIntValueToResFile(gFileSetup, "IBP1Setup", "color",  gCfgIbp1.iColor, 3);
				}break;
				case DID_COLOR_SET_IBP2:{
					GetWindowRect(btnSetIbp2, &gRC_Input);
					iSelectedColor = ColorSelect(hDlg, gRC_Input, gCfgIbp2.iColor);
					gCfgIbp2.iColor = iSelectedColor;
					SetIntValueToResFile(gFileSetup, "IBP2Setup", "color",  gCfgIbp2.iColor, 3);
				}break;
				case DID_COLOR_SET_CO2:{
					GetWindowRect(btnSetCo2, &gRC_Input);
					iSelectedColor = ColorSelect(hDlg, gRC_Input, gCfgCO2.iColor);
					gCfgCO2.iColor = iSelectedColor;
					SetIntValueToResFile(gFileSetup, "CO2Setup", "color",  gCfgCO2.iColor, 3);
				}break;
				case DID_COLOR_SET_DEFAULT:{
					BOOL res = FALSE;
					char strACKInfo[200];
					memset(strACKInfo, 0, sizeof strACKInfo);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_COLOR, "ackinfo_update", strACKInfo, sizeof strACKInfo,"Set default display color?");	
					res = ACKDialog(hDlg, strACKInfo, ACK_ATTENTION);
					
					if(res){
						SetDefualtColor(hDlg);
						UpdateColors(hWndSelf);
					}
					
					
				}break;
				case DID_COLOR_OK:{
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
			
			EndDialog(hDlg, wParam);	
		}break;
	}
	
	temp = DefaultDialogProc(hDlg, message, wParam, lParam);
	return(temp);
}

/*
	建立对话框
*/
void CreateDlgColor(HWND hWnd)
{
	DlgSet.controls = CtrlSet;
	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}


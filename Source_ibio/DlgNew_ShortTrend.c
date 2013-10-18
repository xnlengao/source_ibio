/*************************************************************************** 
 *	Module Name:	DlgNew_ShortTrend
 *
 *	Abstract:短趋势图标尺设置
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2012-12-6 16:14:18
 *	
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"

//DID_MENU
#define DID_SHORTTREND_HELP			DID_SHORTTRENDSET
#define DID_SHORTTREND_OK			DID_SHORTTRENDSET+1
#define DID_SHORTTREND_CAPTION		DID_SHORTTRENDSET+2
#define DID_SHORTTREND_TIME			DID_SHORTTRENDSET+3
#define DID_SHORTTREND_HR			DID_SHORTTRENDSET+4
#define DID_SHORTTREND_SPO2			DID_SHORTTRENDSET+5
#define DID_SHORTTREND_RESP			DID_SHORTTRENDSET+6
#define DID_SHORTTREND_ST			DID_SHORTTRENDSET+7
#define DID_SHORTTREND_IBP1			DID_SHORTTRENDSET+8
#define DID_SHORTTREND_IBP2			DID_SHORTTRENDSET+9
#define DID_SHORTTREND_ETCO2		DID_SHORTTRENDSET+10
#define DID_SHORTTREND_CI			DID_SHORTTRENDSET+11
#define DID_SHORTTREND_TIME_COB		DID_SHORTTRENDSET+12
#define DID_SHORTTREND_HR_COB		DID_SHORTTRENDSET+13
#define DID_SHORTTREND_SPO2_COB	DID_SHORTTRENDSET+14
#define DID_SHORTTREND_RESP_COB		DID_SHORTTRENDSET+15
#define DID_SHORTTREND_ST_COB		DID_SHORTTRENDSET+16
#define DID_SHORTTREND_IBP1_COB		DID_SHORTTRENDSET+17
#define DID_SHORTTREND_IBP2_COB		DID_SHORTTRENDSET+18
#define DID_SHORTTREND_ETCO2_COB	DID_SHORTTRENDSET+19
#define DID_SHORTTREND_CI_COB		DID_SHORTTRENDSET+20


//帮助对话框
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;

//其他控件 
static HWND btnOk;
static HWND btnTimeScale;
static HWND btnHRScale;
static HWND btnSpo2Scale;
static HWND btnRespScale;
static HWND btnSTScale;
static HWND btnIBP1Scale;
static HWND btnIBP2Scale;
static HWND btnEtCO2Scale;
static HWND btnCIScale;
static HWND cobTimeScale;
static HWND cobHRScale;
static HWND cobSpo2Scale;
static HWND cobRespScale;
static HWND cobSTScale;
static HWND cobIBP1Scale;
static HWND cobIBP2Scale;
static HWND cobEtCO2Scale;
static HWND cobCIScale;

//控件回调函数
static WNDPROC OldBtnProc;
static WNDPROC OldCobProc;

static unsigned char *strCtlName[40];

extern HWND ghWndPanel_ShortTrend;
	
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
		5,DLG_HIGH-50, DLG_LENGHT-15, 40,
		DID_SHORTTREND_HELP,
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
				case DID_SHORTTREND_TIME:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SHORTTREND, "help_timescale", strHelp, sizeof strHelp,"Set time scale for short trend.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_SHORTTREND_HR:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SHORTTREND, "help_hrscale", strHelp, sizeof strHelp,"Set HR scale for short trend.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_SHORTTREND_SPO2:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SHORTTREND, "help_spo2scale", strHelp, sizeof strHelp,"Set Spo2 scale for short trend.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_SHORTTREND_RESP:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SHORTTREND, "help_respscale", strHelp, sizeof strHelp,"Set Resp scale for short trend.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_SHORTTREND_ST:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SHORTTREND, "help_stscale", strHelp, sizeof strHelp,"Set ST scale for short trend.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_SHORTTREND_IBP1:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SHORTTREND, "help_ibp1scale", strHelp, sizeof strHelp,"Set IBP1 scale for short trend.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_SHORTTREND_IBP2:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SHORTTREND, "help_ibp2scale", strHelp, sizeof strHelp,"Set IBP2 scale for short trend.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_SHORTTREND_ETCO2:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SHORTTREND, "help_etco2scale", strHelp, sizeof strHelp,"Set EtCO2 scale for short trend.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_SHORTTREND_CI:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SHORTTREND, "help_ciscale", strHelp, sizeof strHelp,"Set CI scale for short trend.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_SHORTTREND_OK:{
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
						case DID_SHORTTREND_TIME_COB:{
							gCfgSystem.bShortTrendScale=index;
							SetIntValueToResFile(gFileSetup, "SystemSetup", "shorttrend_scale",  gCfgSystem.bShortTrendScale, 2);
							
							SetFocus(btnTimeScale);	
						}break;
						case DID_SHORTTREND_HR_COB:{
							giHRScale= index;
							SetIntValueToResFile(gFileSetup, "GlobalSetup", "HRScale",  giHRScale, 2);
							switch(giHRScale){
								case 0:
									gCfgEcg.iMax_HR=160;
									gCfgEcg.iMin_HR=0;
								break;
								case 1:
									gCfgEcg.iMax_HR=300;
									gCfgEcg.iMin_HR=0;
								break;
							}
							SetIntValueToResFile(gFileSetup, "ECGSetup", "max_hr",  gCfgEcg.iMax_HR, 4);
							SetIntValueToResFile(gFileSetup, "ECGSetup", "min_hr",  gCfgEcg.iMin_HR, 4);
							SetFocus(btnHRScale);	
						}break;
						case DID_SHORTTREND_SPO2_COB:{
							giSpo2Scale= index;
							SetIntValueToResFile(gFileSetup, "GlobalSetup", "Spo2Scale",  giSpo2Scale, 2);
							switch(giSpo2Scale){
								case 0:
									gCfgSpO2.bMax_SPO2=100;
									gCfgSpO2.bMin_SPO2=40;
								break;
								case 1:
									gCfgSpO2.bMax_SPO2=100;
									gCfgSpO2.bMin_SPO2=60;
								break;
								case 2:
									gCfgSpO2.bMax_SPO2=100;
									gCfgSpO2.bMin_SPO2=80;
								break;
							}
							SetIntValueToResFile(gFileSetup, "SPO2Setup", "max_spo2", gCfgSpO2.bMax_SPO2, 4);
							SetIntValueToResFile(gFileSetup, "SPO2Setup", "min_spo2",  gCfgSpO2.bMin_SPO2, 4);
							
							SetFocus(btnSpo2Scale);	
						}break;
						case DID_SHORTTREND_RESP_COB:{
							giRespScale= index;
							SetIntValueToResFile(gFileSetup, "GlobalSetup", "RespScale",  giRespScale, 2);
							switch(giRespScale){
								case 0:
									gCfgResp.bMax_RR=8;
									gCfgResp.bMin_RR=0;
								break;
								case 1:
									gCfgResp.bMax_RR=24;
									gCfgResp.bMin_RR=0;
								break;
								case 2:
									gCfgResp.bMax_RR=50;
									gCfgResp.bMin_RR=0;
								break;
								case 3:
									gCfgResp.bMax_RR=100;
									gCfgResp.bMin_RR=0;
								break;
							}
							SetIntValueToResFile(gFileSetup, "RESPSetup", "max_rr", gCfgResp.bMax_RR, 4);
							SetIntValueToResFile(gFileSetup, "RESPSetup", "min_rr",  gCfgResp.bMin_RR, 4);
							
							SetFocus(btnRespScale);	
						}break;
						case DID_SHORTTREND_ST_COB:{
							giSTScale= index;
							SetIntValueToResFile(gFileSetup, "GlobalSetup", "STScale",  giSTScale, 2);
							SetFocus(btnSTScale);	
						}break;
						case DID_SHORTTREND_IBP1_COB:{
							giIBP1Scale= index;
							SetIntValueToResFile(gFileSetup, "GlobalSetup", "IBP1Scale",  giIBP1Scale, 2);
							SetFocus(btnIBP1Scale);	
						}break;
						case DID_SHORTTREND_IBP2_COB:{
							giIBP2Scale= index;
							SetIntValueToResFile(gFileSetup, "GlobalSetup", "IBP2Scale",  giIBP2Scale, 2);
							SetFocus(btnIBP2Scale);	
						}break;
						case DID_SHORTTREND_ETCO2_COB:{
							giEtCO2Scale= index;
							SetIntValueToResFile(gFileSetup, "GlobalSetup", "EtCO2Scale",  giEtCO2Scale, 2);
							SetFocus(btnEtCO2Scale);	
						}break;
						case DID_SHORTTREND_CI_COB:{
							giCIScale= index;
							SetIntValueToResFile(gFileSetup, "GlobalSetup", "CIScale",  giCIScale, 2);
							SetFocus(btnCIScale);	
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


/*
	创建控件函数
*/
static int CreateCtlProc(HWND hDlg)
{
	editCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE, 
					     DID_SHORTTREND_CAPTION, 0, 2, 240, 20, hDlg, 0);
	editHelp = GetDlgItem(hDlg, DID_SHORTTREND_HELP);
	//button
	btnTimeScale = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON |BS_NOBORDER | BS_FLAT,
					DID_SHORTTREND_TIME, 10, 30+29*0, 125, 26, hDlg, 0);
	btnHRScale = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON |BS_NOBORDER | BS_FLAT,
					DID_SHORTTREND_HR, 10, 30+29*1, 125, 26, hDlg, 0);
	btnSpo2Scale = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON |BS_NOBORDER | BS_FLAT,
					DID_SHORTTREND_SPO2, 10, 30+29*2, 125, 26, hDlg, 0);
	btnRespScale = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON |BS_NOBORDER | BS_FLAT,
					DID_SHORTTREND_RESP, 10, 30+29*3, 125, 26, hDlg, 0);
//	btnSTScale = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON |BS_NOBORDER | BS_FLAT,
//					DID_SHORTTREND_ST, 10, 30+29*4, 125, 26, hDlg, 0);
	btnIBP1Scale = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON |BS_NOBORDER | BS_FLAT,
					DID_SHORTTREND_IBP1, 10, 30+29*4, 125, 26, hDlg, 0);
	btnIBP2Scale = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON |BS_NOBORDER | BS_FLAT,
					DID_SHORTTREND_IBP2, 10, 30+29*5, 125, 26, hDlg, 0);
	btnEtCO2Scale = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON |BS_NOBORDER | BS_FLAT,
					DID_SHORTTREND_ETCO2, 10, 30+29*6, 125, 26, hDlg, 0);
	btnCIScale = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON |BS_NOBORDER | BS_FLAT,
					DID_SHORTTREND_CI, 10, 30+29*7, 125, 26, hDlg, 0);
	btnOk = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_SHORTTREND_OK, 10, 30+29*8, 125, 26, hDlg, 0);

	//combobox
	cobTimeScale = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_SHORTTREND_TIME_COB, 140, 30+29*0, 90, 26, hDlg, 0);
	cobHRScale = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_SHORTTREND_HR_COB, 140, 30+29*1, 90, 26, hDlg, 0);
	cobSpo2Scale = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_SHORTTREND_SPO2_COB, 140, 30+29*2, 90, 26, hDlg, 0);
	cobRespScale = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_SHORTTREND_RESP_COB, 140, 30+29*3, 90, 26, hDlg, 0);
//	cobSTScale = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
//					DID_SHORTTREND_ST_COB, 140, 30+29*4, 90, 26, hDlg, 0);
	cobIBP1Scale = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_SHORTTREND_IBP1_COB, 140, 30+29*4, 90, 26, hDlg, 0);
	cobIBP2Scale = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_SHORTTREND_IBP2_COB, 140, 30+29*5, 90, 26, hDlg, 0);
	cobEtCO2Scale = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_SHORTTREND_ETCO2_COB, 140, 30+29*6, 90, 26, hDlg, 0);
	cobCIScale = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_SHORTTREND_CI_COB, 140, 30+29*7, 90, 26, hDlg, 0);
	
	
	return 0;
}

static int InitCtlName(HWND hDlg)
{
	char strMenu[100];

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SHORTTREND, "caption", strMenu, sizeof strMenu,"Short Trend Setup");
	SetWindowText(editCaption, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SHORTTREND, "time_scale", strMenu, sizeof strMenu,"Time Scale");	
	SetWindowText(btnTimeScale, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SHORTTREND, "hr_scale", strMenu, sizeof strMenu,"HR Scale");	
	SetWindowText(btnHRScale, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SHORTTREND, "spo2_scale", strMenu, sizeof strMenu,"Spo2 Scale");	
	SetWindowText(btnSpo2Scale, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SHORTTREND, "resp_scale", strMenu, sizeof strMenu,"Resp Scale");	
	SetWindowText(btnRespScale, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SHORTTREND, "st_scale", strMenu, sizeof strMenu,"ST Scale");	
	SetWindowText(btnSTScale, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SHORTTREND, "ibp1_scale", strMenu, sizeof strMenu,"IBP1 Scale");	
	SetWindowText(btnIBP1Scale, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SHORTTREND, "ibp2_scale", strMenu, sizeof strMenu,"IBP2 Scale");	
	SetWindowText(btnIBP2Scale, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SHORTTREND, "etco2_scale", strMenu, sizeof strMenu,"EtCo2 Scale");	
	SetWindowText(btnEtCO2Scale, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SHORTTREND, "ci_scale", strMenu, sizeof strMenu,"C.I. Scale");	
	SetWindowText(btnCIScale, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "previous", strMenu, sizeof strMenu,"Previous");	
	SetWindowText(btnOk, strMenu);
	
	return 0;
}

static int InitCobList(HWND hDlg)
{
	int i;
	unsigned char *strTimeScale[]={"5 min", "10 min", "15 min", "20 min", "30 min", "1 h", "2 h"};
	unsigned char *strHRScale[]={"0~160/min", "0~300/min"};
	unsigned char *strSpo2Scale[]={"40~100%", "60~100%","80~100%"};
	unsigned char *strRespScale[]={"0~8/min", "0~24/min","0~50/min","0~100/min"};
	unsigned char *strSTScale[]={"-2~+2 mm", "-6~+6 mm","-9~+9 mm"};
	unsigned char *strIBP1Scale[]={"0~80 mmHg","0~100 mmHg","0~300 mmHg"};
	unsigned char *strIBP2Scale[]={"0~80 mmHg","0~100 mmHg","0~300 mmHg"};
	unsigned char *strEtCO2Scale[]={"0~40mmHg","0~60mmHg","0~100 mmHg"};
	unsigned char *strCIScale[]={"2~6L/min/m2"};

	for(i=0; i<7; i++){
			SendMessage(cobTimeScale, CB_ADDSTRING, 0, (LPARAM)strTimeScale[i]);
		}
	SendMessage(cobTimeScale, CB_SETCURSEL,gCfgSystem.bShortTrendScale, 0);
	for(i=0; i<2; i++){
			SendMessage(cobHRScale, CB_ADDSTRING, 0, (LPARAM)strHRScale[i]);
		}
	SendMessage(cobHRScale, CB_SETCURSEL,giHRScale, 0);
	for(i=0; i<3; i++){
			SendMessage(cobSpo2Scale, CB_ADDSTRING, 0, (LPARAM)strSpo2Scale[i]);
		}
	SendMessage(cobSpo2Scale, CB_SETCURSEL,giSpo2Scale, 0);
	for(i=0; i<4; i++){
			SendMessage(cobRespScale, CB_ADDSTRING, 0, (LPARAM)strRespScale[i]);
		}
	SendMessage(cobRespScale, CB_SETCURSEL,giRespScale, 0);
	for(i=0; i<3; i++){
			SendMessage(cobSTScale, CB_ADDSTRING, 0, (LPARAM)strSTScale[i]);
		}
	SendMessage(cobSTScale, CB_SETCURSEL,giSTScale, 0);
	for(i=0; i<3; i++){
			SendMessage(cobIBP1Scale, CB_ADDSTRING, 0, (LPARAM)strIBP1Scale[i]);
		}
	SendMessage(cobIBP1Scale, CB_SETCURSEL,giIBP1Scale, 0);
	for(i=0; i<3; i++){
			SendMessage(cobIBP2Scale, CB_ADDSTRING, 0, (LPARAM)strIBP2Scale[i]);
		}
	SendMessage(cobIBP2Scale, CB_SETCURSEL,giIBP2Scale, 0);
	for(i=0; i<3; i++){
			SendMessage(cobEtCO2Scale, CB_ADDSTRING, 0, (LPARAM)strEtCO2Scale[i]);
		}
	SendMessage(cobEtCO2Scale, CB_SETCURSEL,giEtCO2Scale, 0);
	for(i=0; i<1; i++){
			SendMessage(cobCIScale, CB_ADDSTRING, 0, (LPARAM)strCIScale[i]);
		}
	SendMessage(cobCIScale, CB_SETCURSEL,giCIScale, 0);
	
	
	return 0;
}
static int InitCtlCallBackProc(HWND hDlg)
{

	OldBtnProc = SetWindowCallbackProc(btnTimeScale, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnHRScale, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSpo2Scale, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnRespScale, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSTScale, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnIBP1Scale, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnIBP2Scale, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnEtCO2Scale, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnCIScale, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);

	OldCobProc = SetWindowCallbackProc(cobTimeScale, CobProc);
	OldCobProc = SetWindowCallbackProc(cobHRScale, CobProc);
	OldCobProc = SetWindowCallbackProc(cobSpo2Scale, CobProc);
	OldCobProc = SetWindowCallbackProc(cobRespScale, CobProc);
	OldCobProc = SetWindowCallbackProc(cobSTScale, CobProc);
	OldCobProc = SetWindowCallbackProc(cobIBP1Scale, CobProc);
	OldCobProc = SetWindowCallbackProc(cobIBP2Scale, CobProc);
	OldCobProc = SetWindowCallbackProc(cobEtCO2Scale, CobProc);
	OldCobProc = SetWindowCallbackProc(cobCIScale, CobProc);
	
	OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp);
	
	return 0;
}

static int InitCtlStatus(HWND hDlg)
{
	if(!gbHaveIbp){
		EnableWindow(btnIBP1Scale, FALSE);
		EnableWindow(cobIBP1Scale, FALSE);
		EnableWindow(btnIBP2Scale, FALSE);
		EnableWindow(cobIBP2Scale, FALSE);
	}
	if(!gbHaveCo2){
		EnableWindow(btnEtCO2Scale, FALSE);
		EnableWindow(cobEtCO2Scale, FALSE);
	}
	//暂定不能选择
	EnableWindow(btnCIScale, FALSE);
	EnableWindow(cobCIScale, FALSE);
	
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
			//创建控件
			CreateCtlProc(hDlg);
			//设置控件字符
			InitCtlName(hDlg);
			InitCobList(hDlg);
			//设置控件回调函数
			InitCtlCallBackProc(hDlg);
			//设置控件状态
			InitCtlStatus(hDlg);
			gbKeyType = KEY_TAB;
			SetFocus(btnOk);
		}break;
		case MSG_PAINT:{
			

		}break;
		case MSG_COMMAND:{
			int	id  = LOWORD(wParam);
			switch(id){
				case DID_SHORTTREND_TIME:{
					gbKeyType = KEY_UD;
					SetFocus(cobTimeScale);
					}break;
				case DID_SHORTTREND_HR:{
					gbKeyType = KEY_UD;
					SetFocus(cobHRScale);
					}break;
				case DID_SHORTTREND_SPO2:{
					gbKeyType = KEY_UD;
					SetFocus(cobSpo2Scale);
					}break;
				case DID_SHORTTREND_RESP:{
					gbKeyType = KEY_UD;
					SetFocus(cobRespScale);
					}break;
				case DID_SHORTTREND_ST:{
					gbKeyType = KEY_UD;
					SetFocus(cobSTScale);
					}break;
				case DID_SHORTTREND_IBP1:{
					gbKeyType = KEY_UD;
					SetFocus(cobIBP1Scale);
					}break;
				case DID_SHORTTREND_IBP2:{
					gbKeyType = KEY_UD;
					SetFocus(cobIBP2Scale);
					}break;
				case DID_SHORTTREND_ETCO2:{
					gbKeyType = KEY_UD;
					SetFocus(cobEtCO2Scale);
					}break;
				case DID_SHORTTREND_CI:{
					gbKeyType = KEY_UD;
					SetFocus(cobCIScale);
					}break;
				case DID_SHORTTREND_OK:{
					//UpdateWindow(ghWndPanel_ShortTrend,TRUE);
					NewChangeShortTrendScale();
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
			ChangeWavePanelsLeft((PRECT)NULL);	
			EndDialog(hDlg, wParam);	
		}break;
	}
	
	temp = DefaultDialogProc(hDlg, message, wParam, lParam);
	return(temp);
}


/*
	建立对话框
*/
void CreateShortTrendSet(HWND hWnd)
{
	
 	DlgSet.controls = CtrlSet;
 	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}


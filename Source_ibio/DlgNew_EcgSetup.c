/*************************************************************************** 
 *	Module Name:	DlgNew_EcgSetup
 *
 *	Abstract:ECG 参数设置
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2012-12-7 14:14:18
 *	
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"

//DID_NEWECGSET
#define DID_NEWECGSET_HELP			DID_NEWECGSET
#define DID_NEWECGSET_OK			DID_NEWECGSET+1
#define DID_NEWECGSET_CAPTION		DID_NEWECGSET+2
#define DID_NEWECGSET_HRSRC			DID_NEWECGSET+3
#define DID_NEWECGSET_LEADTYPE		DID_NEWECGSET+4
#define DID_NEWECGSET_ECGMODE		DID_NEWECGSET+5
#define DID_NEWECGSET_ACFILTER		DID_NEWECGSET+6
#define DID_NEWECGSET_DRIFT			DID_NEWECGSET+7
#define DID_NEWECGSET_EMG			DID_NEWECGSET+8
#define DID_NEWECGSET_DISPLAYPR		DID_NEWECGSET+9
#define DID_NEWECGSET_PACEMAKER		DID_NEWECGSET+10
#define DID_NEWECGSET_HRSRC_COB			DID_NEWECGSET+11
#define DID_NEWECGSET_LEADTYPE_COB		DID_NEWECGSET+12
#define DID_NEWECGSET_ECGMODE_COB		DID_NEWECGSET+13
#define DID_NEWECGSET_ACFILTER_COB		DID_NEWECGSET+14
#define DID_NEWECGSET_DRIFT_COB			DID_NEWECGSET+15
#define DID_NEWECGSET_EMG_COB			DID_NEWECGSET+16
#define DID_NEWECGSET_DISPLAYPR_COB		DID_NEWECGSET+17
#define DID_NEWECGSET_PACEMAKER_COB		DID_NEWECGSET+18
#define DID_NEWECGSET_CASCADE			DID_NEWECGSET+19
#define DID_NEWECGSET_CASCADE_COB		DID_NEWECGSET+20

//帮助对话框
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;

//其他控件 
static HWND btnOk;
static HWND btnHRSrc;
static HWND btnLeadType;
static HWND btnCascade;
static HWND btnEcgMode;
static HWND btnACFilter;
static HWND btnDrift;
static HWND btnEMG;
static HWND btnDisplayPR;
static HWND btnPacemaker;
static HWND cobHRSrc;
static HWND cobLeadType;
static HWND cobCascade;
static HWND cobEcgMode;
static HWND cobACFilter;
static HWND cobDrift;
static HWND cobEMG;
static HWND cobDisplayPR;
static HWND cobPacemaker;
//控件回调函数
static WNDPROC OldBtnProc;
static WNDPROC OldCobProc;


//用户对波形面板的配置, 适用于:  Normal, Short Trend, Nibp List
extern WAVE_PANEL_CFG gWaveCfg_User[WAVEPANEL_MAX_COUNT];

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
		DID_NEWECGSET_HELP,
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
				case DID_NEWECGSET_HRSRC:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGSET, "help_hrsrc", strHelp, sizeof strHelp,"Set HR source.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWECGSET_LEADTYPE:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGSET, "help_leadtype", strHelp, sizeof strHelp,"Set ECG leads type.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWECGSET_CASCADE:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGSET, "help_cascade", strHelp, sizeof strHelp,"Set ECG cascade on/off.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWECGSET_ECGMODE:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGSET, "help_ecgmode", strHelp, sizeof strHelp,"Set work module.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWECGSET_ACFILTER:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGSET, "help_acfilter", strHelp, sizeof strHelp,"Set up AC filter on/off.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWECGSET_DRIFT:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGSET, "help_drift", strHelp, sizeof strHelp,"Set up Drift filter on/off.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWECGSET_EMG:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGSET, "help_emg", strHelp, sizeof strHelp,"Set up electromyography filtering.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWECGSET_DISPLAYPR:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGSET, "help_displaypr", strHelp, sizeof strHelp,"Set display PR.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWECGSET_PACEMAKER:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGSET, "help_pacemaker", strHelp, sizeof strHelp,"Set pacemaker.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWECGSET_OK:{
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

int Set_ECGLeadType()
{
	
	Set_Ecg_Lead();
	//如果选择三导联，则关闭第二道波形显示
	if(gCfgEcg.bLeadType == ECGTYPE_3LEAD)
		gWaveCfg_User[1].bSwitch = SWITCH_OFF;
	else
		gWaveCfg_User[1].bSwitch = SWITCH_ON;

	//写入配置文件
	SetIntValueToResFile(gFileSetup, "WaveSetup", "wave1_switch",  gWaveCfg_User[1].bSwitch, 1);

	NewInitWaveDraw();
	//退出所有菜单
	ReturnMainView();	
	
	return 0;
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
						case DID_NEWECGSET_HRSRC_COB:{
							gCfgEcg.bAlmSource = index;
							gCfgPulse.bAlmSource = index;
							SetIntValueToResFile(gFileSetup, "ECGSetup", "alarm_source",  gCfgEcg.bAlmSource, 1);
							SetFocus(btnHRSrc);
							IsAlm_HR(&gValueEcg, &gCfgEcg, TRUE);
						}break;
						case DID_NEWECGSET_LEADTYPE_COB:{
							gCfgEcg.bLeadType = index;
							SetIntValueToResFile(gFileSetup, "ECGSetup", "lead_type",  gCfgEcg.bLeadType, 1);
							SetFocus(btnLeadType);
							Set_ECGLeadType();
							
						}break;
						case DID_NEWECGSET_CASCADE_COB:{
							gCfgEcg.bCascade = index;
							SetIntValueToResFile(gFileSetup, "ECGSetup", "cascade",  gCfgEcg.bCascade, 1);
							SetFocus(btnCascade);
// 							ChangeWaveY();
							
							NewInitWaveDraw();
							//退出所有菜单
							ReturnMainView();
						}break;
						case DID_NEWECGSET_ECGMODE_COB:{
							gCfgEcg.bMode =  index;
							SetIntValueToResFile(gFileSetup, "ECGSetup", "mode",  gCfgEcg.bMode, 1);
							SetFocus(btnEcgMode);
						
							Set_Ecg_Mode();
						}break;
						//工频滤波
						case DID_NEWECGSET_ACFILTER_COB:{
							gCfgEcg.bFilterAC = index;
							SetIntValueToResFile(gFileSetup, "ECGSetup", "filter_ac",  gCfgEcg.bFilterAC, 1);
							SetFocus(btnACFilter);
						//	SetEcgACFilter();
							SetiBioEcgACFilter();
						}break;
						//漂移滤波
						case DID_NEWECGSET_DRIFT_COB:{
							gCfgEcg.bFilterDrift = index;
							SetIntValueToResFile(gFileSetup, "ECGSetup", "filter_drift",  gCfgEcg.bFilterDrift, 1);
							SetFocus(btnDrift);
						//	SetEcgDriftFilter();
						}break;
						//肌电滤波
						case DID_NEWECGSET_EMG_COB:{
							gCfgEcg.bFilterHum= index;
							SetIntValueToResFile(gFileSetup, "ECGSetup", "filter_emg",  gCfgEcg.bFilterHum, 1);
							SetFocus(btnEMG);
						//	SetEcgHumFilter();
						}break;
						case DID_NEWECGSET_DISPLAYPR_COB:{
							
						}break;
						case DID_NEWECGSET_PACEMAKER_COB:{
							gCfgEcg.bPacemaker = index;
							SetIntValueToResFile(gFileSetup, "ECGSetup", "pacemaker",  gCfgEcg.bPacemaker, 1);
							SetFocus(btnPacemaker);

							Set_Ecg_Pace_mode();
						
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
					     DID_NEWECGSET_CAPTION, 0, 5, 240, 25, hDlg, 0);
	editHelp = GetDlgItem(hDlg, DID_NEWECGSET_HELP);

	btnHRSrc = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECGSET_HRSRC, 10, 30+29*0, 125, 26, hDlg, 0);
	btnLeadType = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECGSET_LEADTYPE, 10, 30+29*1, 125, 26, hDlg, 0);
	btnCascade= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECGSET_CASCADE, 10, 30+29*2, 125, 26, hDlg, 0);
	btnEcgMode = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECGSET_ECGMODE, 10, 30+29*3, 125, 26, hDlg, 0);
/*
	btnACFilter = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECGSET_ACFILTER, 10, 30+29*4, 125, 26, hDlg, 0);
	btnDrift = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECGSET_DRIFT, 10, 30+29*5, 125, 26, hDlg, 0);
	btnEMG = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECGSET_EMG, 10, 30+29*6, 125, 26, hDlg, 0);
	btnDisplayPR= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECGSET_DISPLAYPR, 10, 30+29*7, 125, 26, hDlg, 0);
*/
	btnPacemaker= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECGSET_PACEMAKER, 10, 30+29*4, 125, 26, hDlg, 0);
	btnOk = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECGSET_OK, 10, 30+29*5, 125, 26, hDlg, 0);

	cobHRSrc = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_NEWECGSET_HRSRC_COB, 140, 30+29*0, 95, 26, hDlg, 0);
	cobLeadType = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_NEWECGSET_LEADTYPE_COB, 140, 30+29*1, 95, 26, hDlg, 0);
	cobCascade= CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_NEWECGSET_CASCADE_COB, 140, 30+29*2, 95, 26, hDlg, 0);
	cobEcgMode = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_NEWECGSET_ECGMODE_COB, 140, 30+29*3, 95, 26, hDlg, 0);
/*
	cobACFilter = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_NEWECGSET_ACFILTER_COB, 140, 30+29*4, 95, 26, hDlg, 0);
	cobDrift = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_NEWECGSET_DRIFT_COB, 140, 30+29*5 ,95, 26, hDlg, 0);
	cobEMG = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_NEWECGSET_EMG_COB, 140, 30+29*6, 95, 26, hDlg, 0);
	cobDisplayPR = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_NEWECGSET_DISPLAYPR_COB, 140, 30+29*7, 95, 26, hDlg, 0);
*/
	cobPacemaker= CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_NEWECGSET_PACEMAKER_COB, 140, 30+29*4, 95, 26, hDlg, 0);
	
	return 0;
}
static int InitCtlName(HWND hDlg)
{
	char strMenu[100];

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGSET, "caption", strMenu, sizeof strMenu,"ECG Setup");
	SetWindowText(editCaption, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "previous", strMenu, sizeof strMenu,"Previous");	
	SetWindowText(btnOk, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGSET, "hr_source", strMenu, sizeof strMenu,"HR Source");	
	SetWindowText(btnHRSrc, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGSET, "leadtype", strMenu, sizeof strMenu,"Lead Type");	
	SetWindowText(btnLeadType, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGSET, "cascade", strMenu, sizeof strMenu,"Cascade");	
	SetWindowText(btnCascade, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGSET, "ecgmode", strMenu, sizeof strMenu,"Mode");	
	SetWindowText(btnEcgMode, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGSET, "acfilter", strMenu, sizeof strMenu,"AC Filter");	
	SetWindowText(btnACFilter, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGSET, "drift", strMenu, sizeof strMenu,"Drift");	
	SetWindowText(btnDrift, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGSET, "emg", strMenu, sizeof strMenu,"EMG");	
	SetWindowText(btnEMG, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGSET, "displayPR", strMenu, sizeof strMenu,"Display PR");	
	SetWindowText(btnDisplayPR, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGSET, "pacemaker", strMenu, sizeof strMenu,"Pacemaker");	
	SetWindowText(btnPacemaker, strMenu);
	
	
	return 0;
}

static int InitCobList(HWND hDlg)
{
	int i;
	char strOFF[50];
	char strON[50];
	unsigned char *strHRSrc[3]={"AUTO","ECG","PLETH"};
	unsigned char *strLeadType[2]={"5 Lead","3 Lead"};
	char strMenu[50];

	for(i=0;i<3;i++)
		{
	SendMessage(cobHRSrc, CB_ADDSTRING, 0, (LPARAM)strHRSrc[i]);
	}
	SendMessage(cobHRSrc, CB_SETCURSEL, gCfgEcg.bAlmSource, 0);
	//Lead type
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGSET, "cob_5Lead", strMenu, sizeof strMenu,"5 Lead");
	SendMessage(cobLeadType, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGSET, "cob_3Lead", strMenu, sizeof strMenu,"3 Lead");
	SendMessage(cobLeadType, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobLeadType, CB_SETCURSEL, gCfgEcg.bLeadType, 0);

	//ecg mode
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGSET, "cob_diagn", strMenu, sizeof strMenu,"Diagn");
	SendMessage(cobEcgMode, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGSET, "cob_monit", strMenu, sizeof strMenu,"Monit");
	SendMessage(cobEcgMode, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGSET, "cob_oper", strMenu, sizeof strMenu,"Oper");
	SendMessage(cobEcgMode, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobEcgMode, CB_SETCURSEL, gCfgEcg.bMode, 0);

	memset(strOFF, 0, sizeof strOFF);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "off", strOFF, sizeof strOFF,"OFF");
	memset(strON, 0, sizeof strON);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "on", strON, sizeof strON,"ON");
	//cascade
	SendMessage(cobCascade, CB_ADDSTRING, 0, (LPARAM)strOFF);
	SendMessage(cobCascade, CB_ADDSTRING, 0, (LPARAM)strON);
	SendMessage(cobCascade, CB_SETCURSEL, gCfgEcg.bCascade, 0);
	
	SendMessage(cobDrift, CB_ADDSTRING, 0, (LPARAM)strOFF);
	SendMessage(cobDrift, CB_ADDSTRING, 0, (LPARAM)strON);
	SendMessage(cobDrift, CB_SETCURSEL, gCfgEcg.bFilterDrift, 0);

	SendMessage(cobACFilter, CB_ADDSTRING, 0, (LPARAM)strOFF);
	SendMessage(cobACFilter, CB_ADDSTRING, 0, (LPARAM)strON);
	SendMessage(cobACFilter, CB_SETCURSEL, gCfgEcg.bFilterAC, 0);

	SendMessage(cobEMG, CB_ADDSTRING, 0, (LPARAM)strOFF);
	SendMessage(cobEMG, CB_ADDSTRING, 0, (LPARAM)strON);
	SendMessage(cobEMG, CB_SETCURSEL, gCfgEcg.bFilterHum, 0);

	SendMessage(cobDisplayPR, CB_ADDSTRING, 0, (LPARAM)strOFF);
	SendMessage(cobDisplayPR, CB_ADDSTRING, 0, (LPARAM)strON);
	SendMessage(cobDisplayPR, CB_SETCURSEL, 0, 0);

	SendMessage(cobPacemaker, CB_ADDSTRING, 0, (LPARAM)strOFF);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGSET, "cob_pacemode1", strMenu, sizeof strMenu,"Mode1");
	SendMessage(cobPacemaker, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGSET, "cob_pacemode2", strMenu, sizeof strMenu,"Mode2");
	SendMessage(cobPacemaker, CB_ADDSTRING, 0, (LPARAM)strMenu);
	
	SendMessage(cobPacemaker, CB_SETCURSEL, gCfgEcg.bPacemaker, 0);
//	SendMessage(cobScreen, CB_ADDSTRING, 0, (LPARAM)strMenu);
//	SendMessage(cobScreen, CB_SETCURSEL, bInterface, 0);

	return 0;
}
static int InitCtlCallBackProc(HWND hDlg)
{

	OldBtnProc = SetWindowCallbackProc(btnHRSrc, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnLeadType, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnCascade, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnEcgMode, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnACFilter, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnDrift, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnEMG, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnDisplayPR, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnPacemaker, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);

	OldCobProc = SetWindowCallbackProc(cobHRSrc, CobProc);
	OldCobProc = SetWindowCallbackProc(cobLeadType, CobProc);
	OldCobProc = SetWindowCallbackProc(cobCascade, CobProc);
	OldCobProc = SetWindowCallbackProc(cobEcgMode, CobProc);
	OldCobProc = SetWindowCallbackProc(cobACFilter, CobProc);
	OldCobProc = SetWindowCallbackProc(cobDrift, CobProc);
	OldCobProc = SetWindowCallbackProc(cobEMG, CobProc);
	OldCobProc = SetWindowCallbackProc(cobDisplayPR, CobProc);
	OldCobProc = SetWindowCallbackProc(cobPacemaker, CobProc);
	
	OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp);
	
	return 0;
}

static int InitCtlStatus(HWND hDlg)
{
	EnableWindow(btnACFilter,FALSE);
	EnableWindow(cobACFilter,FALSE);
	
	EnableWindow(btnDrift,FALSE);
	EnableWindow(cobDrift,FALSE);

	EnableWindow(btnEMG,FALSE);
	EnableWindow(cobEMG,FALSE);
	
	EnableWindow(btnDisplayPR,FALSE);
	EnableWindow(cobDisplayPR,FALSE);
	
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
			//初始化列表框
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
				case DID_NEWECGSET_HRSRC:{
					gbKeyType = KEY_UD;
					SetFocus(cobHRSrc);
					}break;
				case DID_NEWECGSET_LEADTYPE:{
					gbKeyType = KEY_UD;
					SetFocus(cobLeadType);
					}break;
				case DID_NEWECGSET_CASCADE:{
					gbKeyType = KEY_UD;
					SetFocus(cobCascade);
					}break;
				case DID_NEWECGSET_ECGMODE:{
					gbKeyType = KEY_UD;
					SetFocus(cobEcgMode);
					}break;
				case DID_NEWECGSET_ACFILTER:{
					gbKeyType = KEY_UD;
					SetFocus(cobACFilter);
					}break;
				case DID_NEWECGSET_DRIFT:{
					gbKeyType = KEY_UD;
					SetFocus(cobDrift);
					}break;
				case DID_NEWECGSET_EMG:{
					gbKeyType = KEY_UD;
					SetFocus(cobEMG);
					}break;
				case DID_NEWECGSET_DISPLAYPR:{
					gbKeyType = KEY_UD;
					SetFocus(cobDisplayPR);
					}break;
				case DID_NEWECGSET_PACEMAKER:{
					gbKeyType = KEY_UD;
					SetFocus(cobPacemaker);
					}break;
				case DID_NEWECGSET_OK:{
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
void CreateDlgNewECGSET(HWND hWnd)
{
 	DlgSet.controls = CtrlSet;
 	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}


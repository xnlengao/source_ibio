/*************************************************************************** 
 *	Module Name:	DlgNew_AlarmLevel
 *
 *	Abstract:系统报警等级设置菜单
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *		2012-12-11 14:14:18
 *	
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"

//DID_MENU
#define DID_ALARMLEVEL_HELP			DID_ALARMLEVEL
#define DID_ALARMLEVEL_OK			DID_ALARMLEVEL+1
#define DID_ALARMLEVEL_CAPTION		DID_ALARMLEVEL+2
#define DID_ALARMLEVEL_DEFAULT		DID_ALARMLEVEL+3
#define DID_ALARMLEVEL_HR		DID_ALARMLEVEL+4
#define DID_ALARMLEVEL_SPO2		DID_ALARMLEVEL+5
#define DID_ALARMLEVEL_NIBP		DID_ALARMLEVEL+6
#define DID_ALARMLEVEL_RESP		DID_ALARMLEVEL+7
#define DID_ALARMLEVEL_TEMP		DID_ALARMLEVEL+8
#define DID_ALARMLEVEL_IBP		DID_ALARMLEVEL+9
#define DID_ALARMLEVEL_ETCO2		DID_ALARMLEVEL+10
#define DID_ALARMLEVEL_FICO2		DID_ALARMLEVEL+11
#define DID_ALARMLEVEL_ETAA		DID_ALARMLEVEL+12
#define DID_ALARMLEVEL_FIAA		DID_ALARMLEVEL+13
#define DID_ALARMLEVEL_ETN2O		DID_ALARMLEVEL+14
#define DID_ALARMLEVEL_FIN2O		DID_ALARMLEVEL+15
#define DID_ALARMLEVEL_ETO2		DID_ALARMLEVEL+16
#define DID_ALARMLEVEL_FIO2		DID_ALARMLEVEL+17
#define DID_ALARMLEVEL_ICG		DID_ALARMLEVEL+18
#define DID_ALARMLEVEL_BIS		DID_ALARMLEVEL+19
#define DID_ALARMLEVEL_HR_COB		DID_ALARMLEVEL+20
#define DID_ALARMLEVEL_SPO2_COB		DID_ALARMLEVEL+21
#define DID_ALARMLEVEL_NIBP_COB		DID_ALARMLEVEL+22
#define DID_ALARMLEVEL_RESP_COB		DID_ALARMLEVEL+23
#define DID_ALARMLEVEL_TEMP_COB		DID_ALARMLEVEL+24
#define DID_ALARMLEVEL_IBP_COB		DID_ALARMLEVEL+25
#define DID_ALARMLEVEL_ETCO2_COB		DID_ALARMLEVEL+26
#define DID_ALARMLEVEL_FICO2_COB		DID_ALARMLEVEL+27
#define DID_ALARMLEVEL_ETAA_COB		DID_ALARMLEVEL+28
#define DID_ALARMLEVEL_FIAA_COB		DID_ALARMLEVEL+29
#define DID_ALARMLEVEL_ETN2O_COB		DID_ALARMLEVEL+30
#define DID_ALARMLEVEL_FIN2O_COB		DID_ALARMLEVEL+31
#define DID_ALARMLEVEL_ETO2_COB		DID_ALARMLEVEL+32
#define DID_ALARMLEVEL_FIO2_COB		DID_ALARMLEVEL+33
#define DID_ALARMLEVEL_ICG_COB		DID_ALARMLEVEL+34
#define DID_ALARMLEVEL_BIS_COB		DID_ALARMLEVEL+35

//帮助对话框
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;

//其他控件 
static HWND btnOk;
static HWND btnDefault;
static HWND btnAlmHR;
static HWND btnAlmSpo2;
static HWND btnAlmNIBP;
static HWND btnAlmResp;
static HWND btnAlmTEMP;
static HWND btnAlmIBP;
static HWND btnAlmEtCO2;
static HWND btnAlmFiCO2;
static HWND btnAlmEtAA;
static HWND btnAlmFiAA;
static HWND btnAlmEtN2O;
static HWND btnAlmFiN2O;
static HWND btnAlmEtO2;
static HWND btnAlmFiO2;
static HWND btnAlmICG;
static HWND btnAlmBIS;
static HWND cobAlmHR;
static HWND cobAlmSpo2;
static HWND cobAlmNIBP;
static HWND cobAlmResp;
static HWND cobAlmTEMP;
static HWND cobAlmIBP;
static HWND cobAlmEtCO2;
static HWND cobAlmFiCO2;
static HWND cobAlmEtAA;
static HWND cobAlmFiAA;
static HWND cobAlmEtN2O;
static HWND cobAlmFiN2O;
static HWND cobAlmEtO2;
static HWND cobAlmFiO2;
static HWND cobAlmICG;
static HWND cobAlmBIS;

//控件回调函数
static WNDPROC OldBtnProc;
static WNDPROC OldCobProc;
static unsigned char* strAlmLevel[16]={"HR","Spo2","NIBP","Resp","TEMP","IBP","EtCO2","FiCO2","EtAA","FiAA","EtN2O","FiN2O","EtO2","FiO2","ICG","BIS"};
static char strOff[100];
static char strLow[100];
static char strMid[100];
static char strHigh[100];

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
		DID_ALARMLEVEL_HELP,
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
				case DID_ALARMLEVEL_DEFAULT:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMCONTROL, "help_alloff", strHelp, sizeof strHelp,"Default alarm level.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALARMLEVEL_OK:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "help_previous", strHelp, sizeof strHelp,"Return to previous menu.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALARMLEVEL_HR:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMCONTROL, "help_hr", strHelp, sizeof strHelp,"Set HR alarm level.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALARMLEVEL_SPO2:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMCONTROL, "help_spo2", strHelp, sizeof strHelp,"Set Spo2 alarm level.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALARMLEVEL_NIBP:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMCONTROL, "help_nibp", strHelp, sizeof strHelp,"Set NIBP alarm level.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALARMLEVEL_RESP:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMCONTROL, "help_resp", strHelp, sizeof strHelp,"Set Resp alarm level.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALARMLEVEL_TEMP:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMCONTROL, "help_temp", strHelp, sizeof strHelp,"Set Temp alarm level.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALARMLEVEL_IBP:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMCONTROL, "help_IBP", strHelp, sizeof strHelp,"Set IBP alarm level.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALARMLEVEL_ETCO2:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMCONTROL, "help_EtCO2", strHelp, sizeof strHelp,"Set EtCO2 alarm level.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALARMLEVEL_FICO2:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMCONTROL, "help_FiCO2", strHelp, sizeof strHelp,"Set FiCO2 alarm level.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALARMLEVEL_ETAA:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMCONTROL, "help_EtAA", strHelp, sizeof strHelp,"Set EtAA alarm level.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALARMLEVEL_FIAA:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMCONTROL, "help_FiAA", strHelp, sizeof strHelp,"Set FiAA alarm level.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALARMLEVEL_ETN2O:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMCONTROL, "help_EtN2O", strHelp, sizeof strHelp,"Set EtN2O alarm level.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALARMLEVEL_FIN2O:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes,STR_SETTING_DLG_ALMCONTROL, "help_FiN2O", strHelp, sizeof strHelp,"Set FiN2O alarm level.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALARMLEVEL_ETO2:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMCONTROL, "help_EtO2", strHelp, sizeof strHelp,"Set EtO2 alarm level.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALARMLEVEL_FIO2:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMCONTROL, "help_FiO2", strHelp, sizeof strHelp,"Set FiO2 alarm level.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALARMLEVEL_ICG:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMCONTROL, "help_ICG", strHelp, sizeof strHelp,"Set ICG alarm level.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALARMLEVEL_BIS:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMCONTROL, "help_BIS", strHelp, sizeof strHelp,"Set BIS alarm level.");
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
						case DID_ALARMLEVEL_HR_COB:{
							gCfgEcg.bAlmControl = index;
							SetIntValueToResFile(gFileSetup, "ECGSetup", "alarm_control",  gCfgEcg.bAlmControl, 1);
							IsAlm_HR(&gValueEcg, &gCfgEcg, TRUE);
							IsAlm_ST1(&gValueEcg, &gCfgEcg, TRUE);
							IsAlm_ST2(&gValueEcg, &gCfgEcg, TRUE);
							IsAlm_Arr(&gValueEcg, &gCfgEcg, TRUE);
							SetFocus(btnAlmHR);
						}break;
						case DID_ALARMLEVEL_SPO2_COB:{
							gCfgSpO2.bAlmControl = index;
							SetIntValueToResFile(gFileSetup, "SPO2Setup", "alarm_control",  gCfgSpO2.bAlmControl, 1);
							IsAlm_SpO2(&gValueSpO2, &gCfgSpO2, TRUE);
							SetFocus(btnAlmSpo2);
						}break;
						case DID_ALARMLEVEL_NIBP_COB:{
							gCfgNibp.bAlmControl = index;
							SetIntValueToResFile(gFileSetup, "NIBPSetup", "alarm_control",  gCfgNibp.bAlmControl, 1);
							//改变报警级别
							IsAlm_NIBP(TRUE, TRUE);
							SetFocus(btnAlmNIBP);
						}break;
						case DID_ALARMLEVEL_RESP_COB:{
							gCfgResp.bAlmControl = index;
							SetIntValueToResFile(gFileSetup, "RESPSetup", "alarm_control",  gCfgResp.bAlmControl, 1);
							IsAlm_RR(&gValueResp, &gCfgResp, TRUE);
							SetFocus(btnAlmResp);
						}break;
						case DID_ALARMLEVEL_TEMP_COB:{
							gCfgTemp.bAlmControl = index;
							SetIntValueToResFile(gFileSetup, "TEMPSetup", "alarm_control",  gCfgTemp.bAlmControl, 1);
							//改变参数的报警级别
							IsAlm_T1(&gValueTemp, &gCfgTemp, TRUE);
							IsAlm_T2(&gValueTemp, &gCfgTemp, TRUE);
						 	IsAlm_TD(&gValueTemp, &gCfgTemp, TRUE);
							
							SetFocus(btnAlmTEMP);
						}break;
						case DID_ALARMLEVEL_IBP_COB:{
							gCfgIbp1.bAlmControl = index;
							SetIntValueToResFile(gFileSetup, "IBP1Setup", "alarm_control",  gCfgIbp1.bAlmControl, 1);
							//改变报警级别
							IsAlm_IBP1(TRUE, TRUE);
							
							gCfgIbp2.bAlmControl = index;
							SetIntValueToResFile(gFileSetup, "IBP2Setup", "alarm_control",  gCfgIbp2.bAlmControl, 1);
							//改变报警级别
							IsAlm_IBP2(TRUE, TRUE);
							SetFocus(btnAlmIBP);
						}break;
						case DID_ALARMLEVEL_ETCO2_COB:{
							gCfgCO2.bAlmControl = index;
							SetIntValueToResFile(gFileSetup, "CO2Setup", "alarm_control",  gCfgCO2.bAlmControl, 1);
							//改变参数的报警级别
							IsAlm_EtCO2(&gValueCO2, &gCfgCO2, TRUE);
							SetFocus(btnAlmEtCO2);
						}break;
						case DID_ALARMLEVEL_FICO2_COB:{
							gCfgCO2.bAlmControl = index;
							SetIntValueToResFile(gFileSetup, "CO2Setup", "alarm_control",  gCfgCO2.bAlmControl, 1);
							//改变参数的报警级别
							IsAlm_FiCO2(&gValueCO2, &gCfgCO2, TRUE);
							SetFocus(btnAlmFiCO2);
						}break;
						case DID_ALARMLEVEL_ETAA_COB:{
							SetFocus(btnAlmEtAA);
						}break;
						case DID_ALARMLEVEL_FIAA_COB:{
							SetFocus(btnAlmFiAA);
						}break;
						case DID_ALARMLEVEL_ETN2O_COB:{
							SetFocus(btnAlmEtN2O);
						}break;
						case DID_ALARMLEVEL_FIN2O_COB:{
							SetFocus(btnAlmFiN2O);
						}break;
						case DID_ALARMLEVEL_ETO2_COB:{
							SetFocus(btnAlmEtO2);
						}break;
						case DID_ALARMLEVEL_FIO2_COB:{
							SetFocus(btnAlmEtO2);
						}break;
						case DID_ALARMLEVEL_ICG_COB:{
							SetFocus(btnAlmICG);
						}break;
						case DID_ALARMLEVEL_BIS_COB:{
							SetFocus(btnAlmBIS);
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
					     DID_ALARMLEVEL_CAPTION, 0, 5, 250, 25, hDlg, 0);
	editHelp = GetDlgItem(hDlg, DID_ALARMLEVEL_HELP);
	
	
	btnAlmHR= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_ALARMLEVEL_HR, 10, 30+29*0, 125, 26, hDlg, 0);
	btnAlmSpo2= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_ALARMLEVEL_SPO2, 10, 30+29*1, 125, 26, hDlg, 0);
	btnAlmNIBP= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_ALARMLEVEL_NIBP, 10, 30+29*2, 125, 26, hDlg, 0);
	btnAlmResp= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_ALARMLEVEL_RESP, 10, 30+29*3, 125, 26, hDlg, 0);
	btnAlmTEMP= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_ALARMLEVEL_TEMP, 10, 30+29*4, 125, 26, hDlg, 0);
	btnAlmIBP= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_ALARMLEVEL_IBP, 10, 30+29*5, 125, 26, hDlg, 0);
	btnAlmEtCO2= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_ALARMLEVEL_ETCO2, 10, 30+29*6, 125, 26, hDlg, 0);
	btnAlmFiCO2= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_ALARMLEVEL_FICO2, 10, 30+29*7, 125, 26, hDlg, 0);
/*
//	btnAlmEtAA= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
//					     DID_ALARMLEVEL_ETAA, 130, 30+30*0, 50, 26, hDlg, 0);
	btnAlmFiAA= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_ALARMLEVEL_FIAA, 130, 30+30*1, 50, 26, hDlg, 0);
	btnAlmEtN2O= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_ALARMLEVEL_ETN2O, 130, 30+30*2, 50, 26, hDlg, 0);
	btnAlmFiN2O= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_ALARMLEVEL_FIN2O, 130, 30+30*3, 50, 26, hDlg, 0);
	btnAlmEtO2= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_ALARMLEVEL_ETO2, 130, 30+30*4, 50, 26, hDlg, 0);
	btnAlmFiO2= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_ALARMLEVEL_FIO2, 130, 30+30*5, 50, 26, hDlg, 0);
	btnAlmICG= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_ALARMLEVEL_ICG, 130, 30+30*6, 50, 26, hDlg, 0);
	btnAlmBIS= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_ALARMLEVEL_BIS, 130, 30+30*7, 50, 26, hDlg, 0);
*/
	btnDefault= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_ALARMLEVEL_DEFAULT, 10, 30+29*8, 125, 26, hDlg, 0);
	btnOk = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_ALARMLEVEL_OK, 10, 30+29*9, 125, 26, hDlg, 0);
//cob
	cobAlmHR = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,
					DID_ALARMLEVEL_HR_COB, 140, 30+29*0, 95, 26, hDlg, 0);
	cobAlmSpo2 = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,
					DID_ALARMLEVEL_SPO2_COB, 140, 30+29*1, 95, 26, hDlg, 0);
	cobAlmNIBP = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,
					DID_ALARMLEVEL_NIBP_COB, 140, 30+29*2, 95, 26, hDlg, 0);
	cobAlmResp = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,
					DID_ALARMLEVEL_RESP_COB, 140, 30+29*3, 95, 26, hDlg, 0);
	cobAlmTEMP = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_ALARMLEVEL_TEMP_COB, 140, 30+29*4, 95, 26, hDlg, 0);
	cobAlmIBP = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,
					DID_ALARMLEVEL_IBP_COB, 140, 30+29*5, 95, 26, hDlg, 0);
	cobAlmEtCO2 = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_ALARMLEVEL_ETCO2_COB, 140, 30+29*6, 95, 26, hDlg, 0);
	cobAlmFiCO2 = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_ALARMLEVEL_FICO2_COB, 140, 30+29*7, 95, 26, hDlg, 0);
	
	/*
	cobAlmEtAA= CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_ALARMLEVEL_ETAA_COB, 185, 30+30*0, 60, 26, hDlg, 0);
	cobAlmFiAA= CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,
					DID_ALARMLEVEL_FIAA_COB, 185, 30+30*1, 60, 26, hDlg, 0);
	cobAlmEtN2O= CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_ALARMLEVEL_ETN2O_COB, 185, 30+30*2, 60, 26, hDlg, 0);
	cobAlmFiN2O= CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_ALARMLEVEL_FIN2O_COB, 185, 30+30*3, 60, 26, hDlg, 0);
	cobAlmEtO2= CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_ALARMLEVEL_ETO2_COB, 185, 30+30*4, 60, 26, hDlg, 0);
	cobAlmFiO2= CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,
					DID_ALARMLEVEL_FIO2_COB, 185, 30+30*5, 60, 26, hDlg, 0);
	cobAlmICG= CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,
					DID_ALARMLEVEL_ICG_COB, 185, 30+30*6, 60, 26, hDlg, 0);
	cobAlmBIS= CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,
					DID_ALARMLEVEL_BIS_COB, 185, 30+30*7, 60, 26, hDlg, 0);
	*/
	
	return 0;
}
static int InitCtlName(HWND hDlg)
{
	char strMenu[100];
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMCONTROL, "caption", strMenu, sizeof strMenu,"Alarm Level");
	SetWindowText(editCaption, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "previous", strMenu, sizeof strMenu,"Previous");	
	SetWindowText(btnOk, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMCONTROL, "default", strMenu, sizeof strMenu,"Default");	
	SetWindowText(btnDefault, strMenu);
	
	SetWindowText(btnAlmHR, strAlmLevel[0]);
	SetWindowText(btnAlmSpo2, strAlmLevel[1]);
	SetWindowText(btnAlmNIBP, strAlmLevel[2]);
	SetWindowText(btnAlmResp, strAlmLevel[3]);
	SetWindowText(btnAlmTEMP, strAlmLevel[4]);
	SetWindowText(btnAlmIBP, strAlmLevel[5]);
	SetWindowText(btnAlmEtCO2, strAlmLevel[6]);
	SetWindowText(btnAlmFiCO2, strAlmLevel[7]);
	/*
//	SetWindowText(btnAlmEtAA, strAlmLevel[8]);
	SetWindowText(btnAlmFiAA, strAlmLevel[9]);
	SetWindowText(btnAlmEtN2O, strAlmLevel[10]);
	SetWindowText(btnAlmFiN2O, strAlmLevel[11]);
	SetWindowText(btnAlmEtO2, strAlmLevel[12]);
	SetWindowText(btnAlmFiO2, strAlmLevel[13]);
	SetWindowText(btnAlmICG, strAlmLevel[14]);
	SetWindowText(btnAlmBIS, strAlmLevel[15]);
	*/
	return 0;
}
static int SetCobList(HWND hwnd)
{
	SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)strOff);
	SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)strLow);
	SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)strMid);
	SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)strHigh);
	
	return 0;
}
static int InitCobList(HWND hDlg)
{
	char *strCobAlm[3]={};
	int i;
	
	memset(strOff, 0, sizeof strOff);
	GetStringFromResFiles(gsLanguageRes, "ALMLEVEL", "cob_off", strOff, sizeof strOff,"Off");
	memset(strLow, 0, sizeof strLow);
	GetStringFromResFiles(gsLanguageRes, "ALMLEVEL", "cob_low", strLow, sizeof strLow,"Low");
	memset(strMid, 0, sizeof strMid);
	GetStringFromResFiles(gsLanguageRes, "ALMLEVEL", "cob_mid", strMid, sizeof strMid,"Mid");
	memset(strHigh, 0, sizeof strHigh);
	GetStringFromResFiles(gsLanguageRes, "ALMLEVEL", "cob_High", strHigh, sizeof strHigh,"High");


	SetCobList(cobAlmHR);
	SendMessage(cobAlmHR, CB_SETCURSEL,gCfgEcg.bAlmControl, 0);
	SetCobList(cobAlmSpo2);
	SendMessage(cobAlmSpo2, CB_SETCURSEL,gCfgSpO2.bAlmControl, 0);
	SetCobList(cobAlmNIBP);
	SendMessage(cobAlmNIBP, CB_SETCURSEL,gCfgNibp.bAlmControl, 0);
	SetCobList(cobAlmResp);
	SendMessage(cobAlmResp, CB_SETCURSEL,gCfgResp.bAlmControl, 0);
	SetCobList(cobAlmTEMP);
	SendMessage(cobAlmTEMP, CB_SETCURSEL,gCfgTemp.bAlmControl, 0);
	SetCobList(cobAlmIBP);
	SendMessage(cobAlmIBP, CB_SETCURSEL,gCfgIbp1.bAlmControl, 0);
	SetCobList(cobAlmEtCO2);
	SendMessage(cobAlmEtCO2, CB_SETCURSEL,gCfgCO2.bAlmControl, 0);
	SetCobList(cobAlmFiCO2);
	SendMessage(cobAlmFiCO2, CB_SETCURSEL,0, 0);
	/*
//	SetCobList(cobAlmEtAA);
//	SendMessage(cobAlmEtAA, CB_SETCURSEL,0, 0);
	SetCobList(cobAlmFiAA);
	SendMessage(cobAlmFiAA, CB_SETCURSEL,0, 0);
	SetCobList(cobAlmEtN2O);
	SendMessage(cobAlmEtN2O, CB_SETCURSEL,0, 0);
	SetCobList(cobAlmFiN2O);
	SendMessage(cobAlmFiN2O, CB_SETCURSEL,0, 0);
	SetCobList(cobAlmEtO2);
	SendMessage(cobAlmEtO2, CB_SETCURSEL,0, 0);
	SetCobList(cobAlmFiO2);
	SendMessage(cobAlmFiO2, CB_SETCURSEL,0, 0);
	SetCobList(cobAlmICG);
	SendMessage(cobAlmICG, CB_SETCURSEL,0, 0);
	SetCobList(cobAlmBIS);
	SendMessage(cobAlmBIS, CB_SETCURSEL,0, 0);
	*/
	return 0;
}
static int InitCtlCallBackProc(HWND hDlg)
{
	OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnDefault, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnAlmHR, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnAlmSpo2, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnAlmNIBP, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnAlmResp, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnAlmTEMP, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnAlmIBP, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnAlmEtCO2, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnAlmFiCO2, BtnProc);
/*	
//	OldBtnProc = SetWindowCallbackProc(btnAlmEtAA, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnAlmFiAA, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnAlmEtN2O, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnAlmFiN2O, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnAlmEtO2, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnAlmFiO2, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnAlmICG, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnAlmBIS, BtnProc);
*/
	OldCobProc = SetWindowCallbackProc(cobAlmHR, CobProc);
	OldCobProc = SetWindowCallbackProc(cobAlmSpo2, CobProc);
	OldCobProc = SetWindowCallbackProc(cobAlmNIBP, CobProc);
	OldCobProc = SetWindowCallbackProc(cobAlmResp, CobProc);
	OldCobProc = SetWindowCallbackProc(cobAlmTEMP, CobProc);
	OldCobProc = SetWindowCallbackProc(cobAlmIBP, CobProc);
	OldCobProc = SetWindowCallbackProc(cobAlmEtCO2, CobProc);
	OldCobProc = SetWindowCallbackProc(cobAlmFiCO2, CobProc);
/*	
	OldCobProc = SetWindowCallbackProc(cobAlmEtAA, CobProc);
	OldCobProc = SetWindowCallbackProc(cobAlmFiAA, CobProc);
	OldCobProc = SetWindowCallbackProc(cobAlmEtN2O, CobProc);
	OldCobProc = SetWindowCallbackProc(cobAlmFiN2O, CobProc);
	OldCobProc = SetWindowCallbackProc(cobAlmEtO2, CobProc);
	OldCobProc = SetWindowCallbackProc(cobAlmFiO2, CobProc);
	OldCobProc = SetWindowCallbackProc(cobAlmICG, CobProc);
	OldCobProc = SetWindowCallbackProc(cobAlmBIS, CobProc);
*/	
	OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp);
	
	return 0;
}

static int InitCtlStatus(HWND hDlg)
{
	
	if(!gbHaveSpo2){
		EnableWindow(btnAlmSpo2, FALSE);
		EnableWindow(cobAlmSpo2, FALSE);
	}
	if(!gbHaveNIBP){
		EnableWindow(btnAlmNIBP, FALSE);
		EnableWindow(cobAlmNIBP, FALSE);
	}
	if(!gbHaveIbp){
		EnableWindow(btnAlmIBP, FALSE);
		EnableWindow(cobAlmIBP, FALSE);
	}
	if(!gbHaveCo2){
		EnableWindow(btnAlmEtCO2, FALSE);
		EnableWindow(cobAlmEtCO2, FALSE);
		
		EnableWindow(btnAlmFiCO2, FALSE);
		EnableWindow(cobAlmFiCO2, FALSE);
	}
	if(!gbHaveResp){
		EnableWindow(btnAlmResp, FALSE);
		EnableWindow(cobAlmResp,FALSE);
	}
	if(!gbHaveTemp){
		EnableWindow(btnAlmTEMP, FALSE);
		EnableWindow(cobAlmTEMP, FALSE);
	}
	/*
	//暂定不能选择
	EnableWindow(btnAlmEtAA, FALSE);
	EnableWindow(cobAlmEtAA, FALSE);

	EnableWindow(btnAlmFiAA, FALSE);
	EnableWindow(cobAlmFiAA, FALSE);

	EnableWindow(btnAlmEtN2O, FALSE);
	EnableWindow(cobAlmEtN2O, FALSE);

	EnableWindow(btnAlmFiN2O, FALSE);
	EnableWindow(cobAlmFiN2O, FALSE);

	EnableWindow(btnAlmEtO2, FALSE);
	EnableWindow(cobAlmEtO2, FALSE);

	EnableWindow(btnAlmFiO2, FALSE);
	EnableWindow(cobAlmFiO2, FALSE);
	
	EnableWindow(btnAlmICG, FALSE);
	EnableWindow(cobAlmICG, FALSE);

	EnableWindow(btnAlmBIS, FALSE);
	EnableWindow(cobAlmBIS, FALSE);
	*/
	return 0;
}
/*
	重置报警级别为中级报警
*/
static int ResetAlmLevel(HWND hWnd)
{
	//HR
	gCfgEcg.bAlmControl = 0;
	SetIntValueToResFile(gFileSetup, "ECGSetup", "alarm_control",  gCfgEcg.bAlmControl, 1);
	IsAlm_HR(&gValueEcg, &gCfgEcg, TRUE);
	IsAlm_ST1(&gValueEcg, &gCfgEcg, TRUE);
	IsAlm_ST2(&gValueEcg, &gCfgEcg, TRUE);
	IsAlm_Arr(&gValueEcg, &gCfgEcg, TRUE);

	//Spo2
	gCfgSpO2.bAlmControl = 0;
	SetIntValueToResFile(gFileSetup, "SPO2Setup", "alarm_control",  gCfgSpO2.bAlmControl, 1);
	IsAlm_SpO2(&gValueSpO2, &gCfgSpO2, TRUE);

	//Nibp
	gCfgNibp.bAlmControl = 0;
	SetIntValueToResFile(gFileSetup, "NIBPSetup", "alarm_control",  gCfgNibp.bAlmControl, 1);
	//改变报警级别
	IsAlm_NIBP(TRUE, TRUE);
	
	//Resp
	gCfgResp.bAlmControl = 0;
	SetIntValueToResFile(gFileSetup, "RESPSetup", "alarm_control",  gCfgResp.bAlmControl, 1);
	IsAlm_RR(&gValueResp, &gCfgResp, TRUE);

	//temp
	gCfgTemp.bAlmControl = 0;
	SetIntValueToResFile(gFileSetup, "TEMPSetup", "alarm_control",  gCfgTemp.bAlmControl, 1);
	//改变参数的报警级别
	IsAlm_T1(&gValueTemp, &gCfgTemp, TRUE);
	IsAlm_T2(&gValueTemp, &gCfgTemp, TRUE);
 	IsAlm_TD(&gValueTemp, &gCfgTemp, TRUE);

	//ibp1
	gCfgIbp1.bAlmControl = 0;
	SetIntValueToResFile(gFileSetup, "IBP1Setup", "alarm_control",  gCfgIbp1.bAlmControl, 1);
	//改变报警级别
	IsAlm_IBP1(TRUE, TRUE);

	//ibp2
	gCfgIbp2.bAlmControl = 0;
	SetIntValueToResFile(gFileSetup, "IBP2Setup", "alarm_control",  gCfgIbp2.bAlmControl, 1);
	//改变报警级别
	IsAlm_IBP2(TRUE, TRUE);
	
	//co2
	gCfgCO2.bAlmControl = 0;
	SetIntValueToResFile(gFileSetup, "CO2Setup", "alarm_control",  gCfgCO2.bAlmControl, 1);
	//改变参数的报警级别
	ChangeCO2AlarmClass();

	InitCobList(hWnd);
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
				
				case DID_ALARMLEVEL_HR:{ 
					gbKeyType = KEY_UD;
					SetFocus(cobAlmHR);
					}break;
				case DID_ALARMLEVEL_SPO2:{ 
					gbKeyType = KEY_UD;
					SetFocus(cobAlmSpo2);
					}break;
				case DID_ALARMLEVEL_NIBP:{ 
					gbKeyType = KEY_UD;
					SetFocus(cobAlmNIBP);
					}break;
				case DID_ALARMLEVEL_RESP:{ 
					gbKeyType = KEY_UD;
					SetFocus(cobAlmResp);
					}break;
				case DID_ALARMLEVEL_TEMP:{ 
					gbKeyType = KEY_UD;
					SetFocus(cobAlmTEMP);
					}break;
				case DID_ALARMLEVEL_IBP:{ 
					gbKeyType = KEY_UD;
					SetFocus(cobAlmIBP);
					}break;
				case DID_ALARMLEVEL_ETCO2:{ 
					gbKeyType = KEY_UD;
					SetFocus(cobAlmEtCO2);
					}break;
				case DID_ALARMLEVEL_FICO2:{ 
					gbKeyType = KEY_UD;
					SetFocus(cobAlmFiCO2);
					}break;
				case DID_ALARMLEVEL_ETAA:{ 
					gbKeyType = KEY_UD;
					SetFocus(cobAlmEtAA);
					}break;
				case DID_ALARMLEVEL_FIAA:{ 
					gbKeyType = KEY_UD;
					SetFocus(cobAlmFiAA);
					}break;
				case DID_ALARMLEVEL_ETN2O:{ 
					gbKeyType = KEY_UD;
					SetFocus(cobAlmEtN2O);
					}break;
				case DID_ALARMLEVEL_FIN2O:{ 
					gbKeyType = KEY_UD;
					SetFocus(cobAlmFiN2O);
					}break;
				case DID_ALARMLEVEL_ETO2:{ 
					gbKeyType = KEY_UD;
					SetFocus(cobAlmEtO2);
					}break;
				case DID_ALARMLEVEL_FIO2:{ 
					gbKeyType = KEY_UD;
					SetFocus(cobAlmFiO2);
					}break;
				case DID_ALARMLEVEL_ICG:{ 
					gbKeyType = KEY_UD;
					SetFocus(cobAlmICG);
					}break;
				case DID_ALARMLEVEL_BIS:{ 
					gbKeyType = KEY_UD;
					SetFocus(cobAlmBIS);
					}break;
				case DID_ALARMLEVEL_DEFAULT:{ 
					BOOL res = FALSE;
					char strACKInfo[200];
					memset(strACKInfo, 0, sizeof strACKInfo);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMCONTROL, "ackinfo_update", strACKInfo, sizeof strACKInfo,"Default alarm level!!!!");	
					res = ACKDialog(hDlg, strACKInfo, ACK_ATTENTION);
					
					if(res){
						ResetAlmLevel(hDlg);
						}
					
					}break;
					
				case DID_ALARMLEVEL_OK:{ 
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
void CreateAlarmLevel(HWND hWnd)
{
 	DlgSet.controls = CtrlSet;
 	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}


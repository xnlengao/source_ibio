/*************************************************************************** 
 *	Module Name:	DlgNew_ModuleSet
 *
 *	Abstract:设置模块选项
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2012-12-13 10:14:18
 *	
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"

//DID_MODULESET
#define DID_MODULESET_HELP			DID_MODULESET
#define DID_MODULESET_OK			DID_MODULESET+1
#define DID_MODULESET_CAPTION		DID_MODULESET+2
#define DID_MODULESET_SPO2		DID_MODULESET+3
#define DID_MODULESET_NIBP		DID_MODULESET+4
#define DID_MODULESET_RESP		DID_MODULESET+5
#define DID_MODULESET_TEMP		DID_MODULESET+6
#define DID_MODULESET_IBP1		DID_MODULESET+7
#define DID_MODULESET_IBP2		DID_MODULESET+8
#define DID_MODULESET_CO2		DID_MODULESET+9
#define DID_MODULESET_SPO2_COB		DID_MODULESET+10
#define DID_MODULESET_NIBP_COB		DID_MODULESET+11
#define DID_MODULESET_RESP_COB		DID_MODULESET+12
#define DID_MODULESET_TEMP_COB		DID_MODULESET+13
#define DID_MODULESET_IBP1_COB		DID_MODULESET+14
#define DID_MODULESET_IBP2_COB		DID_MODULESET+15
#define DID_MODULESET_CO2_COB		DID_MODULESET+16



//帮助对话框
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;

//其他控件 
static HWND btnOk;
static HWND btnSpo2;
static HWND btnNIBP;
static HWND btnResp;
static HWND btnTemp;
static HWND btnIBP1;
static HWND btnIBP2;
static HWND btnCO2;
static HWND cobSpo2;
static HWND cobNIBP;
static HWND cobResp;
static HWND cobTemp;
static HWND cobIBP1;
static HWND cobIBP2;
static HWND cobCO2;

//控件回调函数
static WNDPROC OldBtnProc;
static WNDPROC OldCobProc;

static char strON[50];
static char strOFF[50];
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
		DID_MODULESET_HELP,
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
				case DID_MODULESET_SPO2:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SELECTMODULE, "help_Spo2", strHelp, sizeof strHelp,"Enable/disable SPO2 module.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MODULESET_NIBP:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SELECTMODULE, "help_NIBP", strHelp, sizeof strHelp,"Enable/disable NIBP module.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MODULESET_RESP:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SELECTMODULE, "help_Resp", strHelp, sizeof strHelp,"Enable/disable RESP module.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MODULESET_TEMP:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SELECTMODULE, "help_Temp", strHelp, sizeof strHelp,"Enable/disable TEMP module.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MODULESET_IBP1:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SELECTMODULE, "help_Ibp1", strHelp, sizeof strHelp,"Enable/disable IBP1 module.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MODULESET_IBP2:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SELECTMODULE, "help_Ibp2", strHelp, sizeof strHelp,"Enable/disable IBP2 module.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MODULESET_CO2:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SELECTMODULE, "help_CO2", strHelp, sizeof strHelp,"Enable/disable CO2 module.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MODULESET_OK:{
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
						case DID_MODULESET_SPO2_COB:{
							//写入机器配置文件，重新开机方生效
							SetIntValueToResFile(gFileMachineConfig, "HARDWARE", "spo2", index, 1);
							SetFocus(btnSpo2);
							
						}break;
						case DID_MODULESET_NIBP_COB:{
							
							SetIntValueToResFile(gFileMachineConfig, "HARDWARE", "nibp", index, 1);
							SetFocus(btnNIBP);
						}break;
						case DID_MODULESET_RESP_COB:{
							
							SetIntValueToResFile(gFileMachineConfig, "HARDWARE", "resp", index, 1);
							SetFocus(btnResp);
						}break;
						case DID_MODULESET_TEMP_COB:{
							
							SetIntValueToResFile(gFileMachineConfig, "HARDWARE", "temp", index, 1);
							SetFocus(btnTemp);
						}break;
						case DID_MODULESET_IBP1_COB:{
							
							SetIntValueToResFile(gFileMachineConfig, "HARDWARE", "ibp1", index, 1);	
							SetFocus(btnIBP1);
						}break;
						case DID_MODULESET_IBP2_COB:{
							
							SetIntValueToResFile(gFileMachineConfig, "HARDWARE", "ibp2", index, 1);
							SetFocus(btnIBP2);
						}break;
						case DID_MODULESET_CO2_COB:{
							
							SetIntValueToResFile(gFileMachineConfig, "HARDWARE", "co2", index, 1);	
							SetIntValueToResFile(gFileSetup, "WaveSetup", "wave6_switch", index, 1);	
							SetFocus(btnCO2);
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
	绘制下一页图标函数
*/
static int DrawIconProc(HWND hDlg)
{
	
	int i;
	for(i=1;i<8;i++){
		CreateWindow("static",gstrNextPage,WS_CHILD|WS_VISIBLE,IDC_STATIC,
	 		150, 33+30*i, 30, 16,hDlg,0);
	}
	return(0);
}


/*
	创建控件函数
*/
static int CreateCtlProc(HWND hDlg)
{
	editCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE, 
					     DID_MODULESET_CAPTION, 0, 5, 240, 25, hDlg, 0);
	editHelp = GetDlgItem(hDlg, DID_MODULESET_HELP);

	btnSpo2 = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MODULESET_SPO2, 10, 30+30*0, 125, 26, hDlg, 0);
	btnNIBP = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MODULESET_NIBP, 10, 30+30*1, 125, 26, hDlg, 0);
	btnResp = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MODULESET_RESP, 10, 30+30*2, 125, 26, hDlg, 0);
	btnTemp = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MODULESET_TEMP, 10, 30+30*3, 125, 26, hDlg, 0);
	btnIBP1 = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MODULESET_IBP1, 10, 30+30*4, 125, 26, hDlg, 0);
	btnIBP2 = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MODULESET_IBP2, 10, 30+30*5, 125, 26, hDlg, 0);
	btnCO2 = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MODULESET_CO2, 10, 30+30*6, 125, 26, hDlg, 0);
	
	btnOk = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MODULESET_OK, 10, 30+30*7, 125, 26, hDlg, 0);

	cobSpo2 = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
						DID_MODULESET_SPO2_COB, 140, 30+30*0, 90, 26, hDlg, 0);
	cobNIBP = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
						DID_MODULESET_NIBP_COB, 140, 30+30*1, 90, 26, hDlg, 0);
	cobResp = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
						DID_MODULESET_RESP_COB, 140, 30+30*2, 90, 26, hDlg, 0);
	cobTemp = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
						DID_MODULESET_TEMP_COB, 140, 30+30*3, 90, 26, hDlg, 0);
	cobIBP1 = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
						DID_MODULESET_IBP1_COB, 140, 30+30*4, 90, 26, hDlg, 0);
	cobIBP2 = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
						DID_MODULESET_IBP2_COB, 140, 30+30*5, 90, 26, hDlg, 0);
	cobCO2 = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
						DID_MODULESET_CO2_COB, 140, 30+30*6, 90, 26, hDlg, 0);
	return 0;
}
static int InitCtlName(HWND hDlg)
{
	char strMenu[100];

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SELECTMODULE, "caption", strMenu, sizeof strMenu,"Select Module");
	SetWindowText(editCaption, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "previous", strMenu, sizeof strMenu,"Previous");	
	SetWindowText(btnOk, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SELECTMODULE, "Spo2", strMenu, sizeof strMenu,"Spo2 Module");	
	SetWindowText(btnSpo2, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SELECTMODULE, "Nibp", strMenu, sizeof strMenu,"NIBP Module");	
	SetWindowText(btnNIBP, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SELECTMODULE, "Resp", strMenu, sizeof strMenu,"RESP Module");	
	SetWindowText(btnResp, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SELECTMODULE, "Temp", strMenu, sizeof strMenu,"TEMP Module");	
	SetWindowText(btnTemp, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SELECTMODULE, "IBP1", strMenu, sizeof strMenu,"IBP1 Module");	
	SetWindowText(btnIBP1, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SELECTMODULE, "IBP2", strMenu, sizeof strMenu,"IBP2 Module");	
	SetWindowText(btnIBP2, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SELECTMODULE, "CO2", strMenu, sizeof strMenu,"CO2 Module");	
	SetWindowText(btnCO2, strMenu);

	memset(strON, 0, sizeof strON);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "on", strON, sizeof strON,"ON");	
	
	memset(strOFF, 0, sizeof strOFF);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "off", strOFF, sizeof strOFF,"OFF");	
	

	return 0;
}
static int InitCobList(HWND hDlg)
{

	SendMessage(cobSpo2, CB_ADDSTRING, 0, (LPARAM)strOFF);
	SendMessage(cobSpo2, CB_ADDSTRING, 0, (LPARAM)strON);
	SendMessage(cobSpo2, CB_SETCURSEL,gbHaveSpo2, 0);

	SendMessage(cobNIBP, CB_ADDSTRING, 0, (LPARAM)strOFF);
	SendMessage(cobNIBP, CB_ADDSTRING, 0, (LPARAM)strON);
	SendMessage(cobNIBP, CB_SETCURSEL,gbHaveNIBP, 0);

	SendMessage(cobResp, CB_ADDSTRING, 0, (LPARAM)strOFF);
	SendMessage(cobResp, CB_ADDSTRING, 0, (LPARAM)strON);
	SendMessage(cobResp, CB_SETCURSEL,gbHaveResp, 0);

	SendMessage(cobTemp, CB_ADDSTRING, 0, (LPARAM)strOFF);
	SendMessage(cobTemp, CB_ADDSTRING, 0, (LPARAM)strON);
	SendMessage(cobTemp, CB_SETCURSEL,gbHaveTemp, 0);

	SendMessage(cobIBP1, CB_ADDSTRING, 0, (LPARAM)strOFF);
	SendMessage(cobIBP1, CB_ADDSTRING, 0, (LPARAM)strON);
	SendMessage(cobIBP1, CB_SETCURSEL,gbHaveIbp, 0);

	SendMessage(cobIBP2, CB_ADDSTRING, 0, (LPARAM)strOFF);
	SendMessage(cobIBP2, CB_ADDSTRING, 0, (LPARAM)strON);
	SendMessage(cobIBP2, CB_SETCURSEL,gbHaveIbp, 0);

	SendMessage(cobCO2, CB_ADDSTRING, 0, (LPARAM)strOFF);
	SendMessage(cobCO2, CB_ADDSTRING, 0, (LPARAM)strON);
	SendMessage(cobCO2, CB_SETCURSEL,gbHaveCo2, 0);
	
	return 0;
}
static int InitCtlCallBackProc(HWND hDlg)
{
	OldBtnProc = SetWindowCallbackProc(btnSpo2, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnNIBP, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnResp, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnTemp, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnIBP1, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnIBP2, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnCO2, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);

	OldCobProc = SetWindowCallbackProc(cobSpo2, CobProc);
	OldCobProc = SetWindowCallbackProc(cobNIBP, CobProc);
	OldCobProc = SetWindowCallbackProc(cobResp, CobProc);
	OldCobProc = SetWindowCallbackProc(cobTemp, CobProc);
	OldCobProc = SetWindowCallbackProc(cobIBP1, CobProc);
	OldCobProc = SetWindowCallbackProc(cobIBP2, CobProc);
	OldCobProc = SetWindowCallbackProc(cobCO2, CobProc);
	
	OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp);
	
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
			//创建控件
			CreateCtlProc(hDlg);
			//设置控件字符
			InitCtlName(hDlg);
			//初始化列表框
			InitCobList(hDlg);
			//设置控件回调函数
			InitCtlCallBackProc(hDlg);
			gbKeyType = KEY_TAB;
			SetFocus(btnOk);
		}break;
		case MSG_PAINT:{
			

		}break;
		case MSG_COMMAND:{
			int	id  = LOWORD(wParam);
			switch(id){
				case DID_MODULESET_SPO2:{
					gbKeyType = KEY_UD;
					SetFocus(cobSpo2);
					}break;
				case DID_MODULESET_NIBP:{
					gbKeyType = KEY_UD;
					SetFocus(cobNIBP);
					}break;
				case DID_MODULESET_RESP:{
					gbKeyType = KEY_UD;
					SetFocus(cobResp);
					}break;
				case DID_MODULESET_TEMP:{
					gbKeyType = KEY_UD;
					SetFocus(cobTemp);
					}break;
				case DID_MODULESET_IBP1:{
					gbKeyType = KEY_UD;
					SetFocus(cobIBP1);
					}break;
				case DID_MODULESET_IBP2:{
					gbKeyType = KEY_UD;
					SetFocus(cobIBP2);
					}break;
				case DID_MODULESET_CO2:{
					gbKeyType = KEY_UD;
					SetFocus(cobCO2);
					}break;
				case DID_MODULESET_OK:{
					BOOL res = FALSE;
					char strACKInfo[200];
					memset(strACKInfo, 0, sizeof strACKInfo);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SELECTMODULE, "ackinfo_moduleset", strACKInfo, sizeof strACKInfo,"you must restart your system for the configuration changes!");	
					res = ACKDialog(hDlg, strACKInfo, ACK_ATTENTION);
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
void CreateModuleSet(HWND hWnd)
{
	
 	DlgSet.controls = CtrlSet;
 	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}


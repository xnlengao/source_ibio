/*************************************************************************** 
 *	Module Name:	DlgNew_SystemSet
 *
 *	Abstract:系统设置菜单
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2012-12-6 14:14:18
 *	
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"

//DID_MENU
#define DID_SYSSET_HELP			DID_SYSTEMSET
#define DID_SYSSET_OK			DID_SYSTEMSET+1
#define DID_SYSSET_CAPTION		DID_SYSTEMSET+2
#define DID_SYSSET_LANGUAGE		DID_SYSTEMSET+3
#define DID_SYSSET_RECORDER		DID_SYSTEMSET+4
#define DID_SYSSET_TIMESETUP		DID_SYSTEMSET+5
#define DID_SYSSET_COLORSETUP		DID_SYSTEMSET+6
#define DID_SYSSET_NETSETUP		DID_SYSTEMSET+7
#define DID_SYSSET_MODECONF		DID_SYSTEMSET+8
#define DID_SYSSET_MACHINE			DID_SYSTEMSET+9
#define DID_SYSSET_LANGUAGE_COB		DID_SYSTEMSET+10


//帮助对话框
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;

//其他控件 
static HWND btnOk;
static HWND btnLanguage;
static HWND btnRecorder;
static HWND btnTimeSetup;
static HWND btnColorSetup;
static HWND btnNetSetup;
static HWND btnModeConf;
static HWND btnMachine;
static HWND cobLanguage;

//控件回调函数
static WNDPROC OldBtnProc;
static WNDPROC OldCobProc;


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
		DID_SYSSET_HELP,
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
				case DID_SYSSET_LANGUAGE:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SYSTEMSET, "help_language", strHelp, sizeof strHelp,"Select system language,restarting the monitor to effect the setting.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_SYSSET_RECORDER:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SYSTEMSET, "help_recorder", strHelp, sizeof strHelp,"Set recorder.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_SYSSET_TIMESETUP:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SYSTEMSET, "help_time", strHelp, sizeof strHelp,"After changing the system time ,the monitor must be restarted.");
					SetWindowBkColor(hWnd,COLOR_red);
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_SYSSET_COLORSETUP:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SYSTEMSET, "help_color", strHelp, sizeof strHelp,"Set up system color.");
					SetWindowBkColor(hWnd,COLOR_red);
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_SYSSET_MODECONF:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SYSTEMSET, "help_modeconf", strHelp, sizeof strHelp,"Restore,save or delete default configurations.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_SYSSET_MACHINE:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SYSTEMSET, "help_machine", strHelp, sizeof strHelp,"System setup,such as color setings of parameters,trend setup,calibration.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_SYSSET_OK:{
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
						case DID_SYSSET_LANGUAGE_COB:{
							gCfgSystem.bLanguage= index;
							SetIntValueToResFile(gFileSetup, "SystemSetup", "language",  gCfgSystem.bLanguage, 1);
							SetFocus(btnLanguage);
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
	for(i=1;i<6;i++){
		CreateWindow("static",gstrNextPage,WS_CHILD|WS_VISIBLE,IDC_STATIC,
	 		150, 30+30*i, 30, 16,hDlg,0);
	}
	
	return(0);
}


/*
	创建控件函数
*/
static int CreateCtlProc(HWND hDlg)
{
	editCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE, 
					     DID_SYSSET_CAPTION, 0, 5, 240, 25, hDlg, 0);
	editHelp = GetDlgItem(hDlg, DID_SYSSET_HELP);
	
	
	btnLanguage = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_SYSSET_LANGUAGE, 10, 30+30*0, 125, 26, hDlg, 0);
	btnRecorder = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
						DID_SYSSET_RECORDER, 10, 30+30*1, 125, 26, hDlg, 0);
	btnTimeSetup = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
						 DID_SYSSET_TIMESETUP, 10, 30+30*2, 125, 26, hDlg, 0);
	btnColorSetup = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
						 DID_SYSSET_COLORSETUP, 10, 30+30*3, 125, 26, hDlg, 0);
//	btnNetSetup = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
//						 DID_SYSSET_NETSETUP, 10, 30+30*4, 125, 26, hDlg, 0);
	btnModeConf = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
						 DID_SYSSET_MODECONF, 10, 30+30*4, 125, 26, hDlg, 0);
	btnMachine = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
						 DID_SYSSET_MACHINE, 10, 30+30*5, 125, 26, hDlg, 0);
	btnOk = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_SYSSET_OK, 10, 30+30*6, 125, 26, hDlg, 0);

	cobLanguage= CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_SYSSET_LANGUAGE_COB, 140, 30, 90, 26, hDlg, 0);
	
	return 0;
}
static int InitCtlName(HWND hDlg)
{
	char strMenu[100];

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SYSTEMSET, "caption", strMenu, sizeof strMenu,"System Setup");
	SetWindowText(editCaption, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "previous", strMenu, sizeof strMenu,"Previous");	
	SetWindowText(btnOk, strMenu);

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SYSTEMSET, "language", strMenu, sizeof strMenu,"Language");	
	SetWindowText(btnLanguage, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SYSTEMSET, "recorder", strMenu, sizeof strMenu,"Recorder");	
	SetWindowText(btnRecorder, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SYSTEMSET, "timeSetup", strMenu, sizeof strMenu,"Time Setup");	
	SetWindowText(btnTimeSetup, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SYSTEMSET, "colorSetup", strMenu, sizeof strMenu,"Color Setup");	
	SetWindowText(btnColorSetup, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SYSTEMSET, "netSetup", strMenu, sizeof strMenu,"Net Setup");	
	SetWindowText(btnNetSetup, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SYSTEMSET, "modeConf", strMenu, sizeof strMenu,"Mode Config");	
	SetWindowText(btnModeConf, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SYSTEMSET, "machine", strMenu, sizeof strMenu,"Machine");	
	SetWindowText(btnMachine, strMenu);

	
	return 0;
}

static int InitCobList(HWND hDlg)
{
	int i;
	unsigned char *strLanguage[]={"English", "Chinese"};
	
	for(i=0; i<2; i++){
			SendMessage(cobLanguage, CB_ADDSTRING, 0, (LPARAM)strLanguage[i]);
		}
	SendMessage(cobLanguage, CB_SETCURSEL,gCfgSystem.bLanguage, 0);

}
static int InitCtlCallBackProc(HWND hDlg)
{

	OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnLanguage, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnRecorder, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnTimeSetup, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnColorSetup, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnNetSetup, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnModeConf, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnMachine, BtnProc);

	OldCobProc = SetWindowCallbackProc(cobLanguage, CobProc);
	
	OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp);
	
	return 0;
}

static int InitCtlStatus(HWND hDlg)
{
	
	//如果记录仪故障，则该选项不可用
	if(gPrinterStatus == PRINT_STATUS_NOPRN){
		EnableWindow(btnRecorder, FALSE);
	}
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
			//绘制下一页图标
			DrawIconProc(hDlg);
			
			gbKeyType = KEY_TAB;
			SetFocus(btnOk);
		}break;
		case MSG_PAINT:{
			

		}break;
		case MSG_COMMAND:{
			int	id  = LOWORD(wParam);
			switch(id){
				case DID_SYSSET_LANGUAGE:{
					gbKeyType = KEY_UD;
					SetFocus(cobLanguage);
					}break;
				case DID_SYSSET_RECORDER:{
					CreateRecorderSet(hDlg);
					}break;
				case DID_SYSSET_TIMESETUP:{
					CreateTimeSet(hDlg);
					}break;
				case DID_SYSSET_COLORSETUP:{
					CreateDlgColor(hDlg);
					}break;
				case DID_SYSSET_NETSETUP:{
					CreateDlgNet(hDlg);
					}break;
				case DID_SYSSET_MODECONF:{
					CreateModeConfig(hDlg);
					}break;
				case DID_SYSSET_MACHINE:{
					GetWindowRect(btnMachine, &gRC_Input);
						if(PasswordInput(hDlg, gRC_Input, PASSTYPE_MACHINE)){
							CreateMachineSet(hDlg);
						}
						else{
							if(B_PRINTF)printf("%s:%d Error Password of USER.\n", __FILE__, __LINE__);
						}	
					}break;
				case DID_SYSSET_OK:{
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
void CreateSystemSet(HWND hWnd)
{
 	DlgSet.controls = CtrlSet;
 	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}


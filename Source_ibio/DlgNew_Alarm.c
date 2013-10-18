/*************************************************************************** 
 *	Module Name:	DlgSet_Alarm
 *
 *	Abstract:	设置报警功能的对话框
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2007-06-16 15:46:11
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"

//DID_ALMSETUP
#define DID_ALMSETUP_HELP					DID_ALMSETUP
#define DID_ALMSETUP_OK					DID_ALMSETUP+1
#define DID_ALMSETUP_CAPTION				DID_MAINMENU+2

#define DID_ALMSETUP_VOLUME				DID_ALMSETUP+3
#define DID_ALMSETUP_PAUSE				DID_ALMSETUP+4
#define DID_ALMSETUP_ALLPRINT				DID_ALMSETUP+5
#define DID_ALMSETUP_PRNTIME				DID_ALMSETUP+6
#define DID_ALMSETUP_VIEWLIMIT			DID_ALMSETUP+7
#define DID_ALMSETUP_CONTROL				DID_ALMSETUP+8
#define DID_ALMSETUP_PRINT				DID_ALMSETUP+9
#define DID_ALMSETUP_LIMIT					DID_ALMSETUP+10
#define DID_ALMSETUP_RECALL				DID_ALMSETUP+11
#define DID_ALMSETUP_PAUSE_COB			DID_ALMSETUP+12
#define DID_ALMSETUP_VOLUME_COB			DID_ALMSETUP+13
#define DID_ALMSETUP_ALLPRINT_COB		DID_ALMSETUP+14
#define DID_ALMSETUP_VIEWLIMIT_COB		DID_ALMSETUP+15
#define DID_ALMSETUP_PRNTIME_COB			DID_ALMSETUP+16


//帮助对话框
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;
//其他控件 
static HWND btnOk;
static HWND btnVolume;
static HWND btnPause;
static HWND btnAllPrint;
static HWND btnAlarmControl;
static HWND btnAlarmPrint;
static HWND btnAlarmLimit;
static HWND btnAlarmRecall;
static HWND btnViewLimit;
static HWND btnPrnTime;

static HWND cobVolume;
static HWND cobPause;
static HWND cobAllPrint;
static HWND cobViewLimit;
static HWND cobPrnTime;
//控件回调函数
static WNDPROC OldBtnProc;
static WNDPROC OldCobProc;

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
		DID_ALMSETUP_HELP,
		"",
		0
	}
};

//是否进入报警暂停状态
extern BOOL gbSuspending;
//报警暂停状态倒计时
extern int giSuspendTime;

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
				case DID_ALMSETUP_VOLUME_COB:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSET, "help_volume", strHelp, sizeof strHelp,"Set up the volume of alarm.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALMSETUP_PAUSE:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSET, "help_pause", strHelp, sizeof strHelp,"Set up alarm pause time.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALMSETUP_ALLPRINT:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSET, "help_print", strHelp, sizeof strHelp,"Set up alarm print.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALMSETUP_PRNTIME:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSET, "help_print_time", strHelp, sizeof strHelp,"Set up alarm print time.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALMSETUP_VIEWLIMIT:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSET, "help_view_limit", strHelp, sizeof strHelp,"Set up alarm limit.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALMSETUP_PRINT:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSET, "help_set_print", strHelp, sizeof strHelp,"Set up alarm print.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALMSETUP_CONTROL:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSET, "help_set_control", strHelp, sizeof strHelp,"Set up alarm contral.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALMSETUP_LIMIT:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSET, "help_set_comlimit", strHelp, sizeof strHelp,"Set up common alarm limit.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALMSETUP_OK:{
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
						case DID_ALMSETUP_VOLUME:{
							gCfgAlarm.bVolume = index;
							SetIntValueToResFile(gFileSetup, "AlarmSetup", "volume",  gCfgAlarm.bVolume, 2);
							SetFocus(btnVolume);	
						}break;
						case DID_ALMSETUP_PAUSE_COB:{
							if(gCfgAlarm.bPauseTime != index+1){
								gCfgAlarm.bPauseTime = index+1;
								SetIntValueToResFile(gFileSetup, "AlarmSetup", "pause_time",  gCfgAlarm.bPauseTime, 2);
								if(gbSuspending) gbSuspending = FALSE;
							}
							
							SetFocus(btnPause);
						}break;
						case DID_ALMSETUP_ALLPRINT_COB:{
							gCfgAlarm.bPrint = index;
							SetIntValueToResFile(gFileSetup, "AlarmSetup", "print",  gCfgAlarm.bPrint, 1);
							SetFocus(btnAllPrint);
						}break;
						case DID_ALMSETUP_PRNTIME_COB:{
							switch(index){
								case 0 : gCfgAlarm.bPrintWaveLength = 4; break;
								case 1 : gCfgAlarm.bPrintWaveLength = 8; break;
								case 2 : gCfgAlarm.bPrintWaveLength = 16; break;
								default: gCfgAlarm.bPrintWaveLength = 4; break;
							}
							SetIntValueToResFile(gFileSetup, "AlarmSetup", "wave_length",  gCfgAlarm.bPrintWaveLength, 3);
							SetFocus(btnPrnTime);
						}break;
						case DID_ALMSETUP_VIEWLIMIT_COB:{
							gCfgAlarm.bViewLimit = index;
							SetIntValueToResFile(gFileSetup, "AlarmSetup", "view_limit",  gCfgAlarm.bViewLimit, 1);
							SetFocus(btnViewLimit);
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
	绘制下一页图标函数
*/
static int DrawIconProc(HWND hDlg)
{
	
	int i;
	for(i=5;i<8;i++){
		CreateWindow("static",gstrNextPage,WS_CHILD|WS_VISIBLE,IDC_STATIC,
	 		150, 35+30*i, 30, 16,hDlg,0);
	}

	return(0);
}

//创建控件
static int CreateCtlProc(HWND hDlg)
{
	editHelp = GetDlgItem(hDlg, DID_ALMSETUP_HELP);
	editCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE  ,
					     DID_ALMSETUP_CAPTION, 0, 5, 250, 25, hDlg, 0);
	btnVolume = CreateWindow("button",  "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_ALMSETUP_VOLUME_COB, 10, 30+30*0, 125, 26, hDlg, 0);
	btnPause = CreateWindow("button",  "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_ALMSETUP_PAUSE, 10, 30+30*1, 125, 26, hDlg, 0);
	btnAllPrint = CreateWindow("button",  "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_ALMSETUP_ALLPRINT, 10, 30+30*2, 125, 26, hDlg, 0);
	btnPrnTime = CreateWindow("button",  "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_ALMSETUP_PRNTIME, 10, 30+30*3, 125, 26, hDlg, 0);
	btnViewLimit = CreateWindow("button",  "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_ALMSETUP_VIEWLIMIT, 10, 30+30*4, 125, 26, hDlg, 0);
	btnAlarmControl = CreateWindow("button",  "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_ALMSETUP_CONTROL, 10, 30+30*5, 125, 26, hDlg, 0);
	btnAlarmPrint = CreateWindow("button",  "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_ALMSETUP_PRINT, 10, 30+30*6, 125, 26, hDlg, 0);
	btnAlarmLimit = CreateWindow("button",  "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_ALMSETUP_LIMIT, 10, 30+30*7, 125, 26, hDlg, 0);
	btnOk = CreateWindow("button",  "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_ALMSETUP_OK, 10, 30+30*8, 125, 26, hDlg, 0);

	cobVolume = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,
					DID_ALMSETUP_VOLUME, 140, 30+30*0, 95, 26, hDlg, 0);
	cobPause = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,
					DID_ALMSETUP_PAUSE_COB, 140, 30+30*1, 95, 26, hDlg, 0);
	cobAllPrint = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,
					DID_ALMSETUP_ALLPRINT_COB, 140, 30+30*2, 95, 26, hDlg, 0);
	cobPrnTime = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,
					DID_ALMSETUP_PRNTIME_COB, 140, 30+30*3, 95, 26, hDlg, 0);
	cobViewLimit = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,
					DID_ALMSETUP_VIEWLIMIT_COB, 140, 30+30*4, 95, 26, hDlg, 0);
			
	return 0;
}
//设置控件字符
static int InitCtlName(HWND hDlg)
{
	char strMenu[100];
	//caption
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSET, "caption", strMenu, sizeof strMenu,"Alarm Setup");	
	SetWindowText(editCaption,strMenu);
	//Volume
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSET, "volume", strMenu, sizeof strMenu,"Volume");	
	SetWindowText(btnVolume,strMenu);
	//Pause
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSET, "pause", strMenu, sizeof strMenu,"Pause");	
	SetWindowText(btnPause,strMenu);
	//Print
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSET, "allprint", strMenu, sizeof strMenu,"Print");	
	SetWindowText(btnAllPrint,strMenu);
	//Print Time
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSET, "print_time", strMenu, sizeof strMenu,"Print Time");	
	SetWindowText(btnPrnTime,strMenu);
	//View Limit 
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSET, "view_limit", strMenu, sizeof strMenu,"View Limit");	
	SetWindowText(btnViewLimit,strMenu);
	//Alarm Control
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSET, "control", strMenu, sizeof strMenu,"Alarm Control");	
	SetWindowText(btnAlarmControl,strMenu);
	//Alarm Print
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSET, "set_print", strMenu, sizeof strMenu,"Set Alarm Print");	
	SetWindowText(btnAlarmPrint,strMenu);
	//Alarm Limit
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSET, "set_limit", strMenu, sizeof strMenu,"Set Alarm Limit");	
	SetWindowText(btnAlarmLimit,strMenu);	
			
	//Exit
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "previous", strMenu, sizeof strMenu,"previous");	
	SetWindowText(btnOk,strMenu);			
	return 0;
}
//初始化combobox
static int InitCobList(HWND hDlg)
{
	int i;
	char strMenu[100];
	//声音标记
	unsigned char *strVolume[]={"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};
	//暂停时间
	unsigned char *strPause[]={"1 Min", "2 Min", "3 Min"};
	//打印时间
	unsigned char *strPrnTime[]={"4 Sec", "8 Sec", "16 Sec"};
	//volume
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes,STR_SETTING_GENERAL, "off", strMenu, sizeof strMenu);	
	SendMessage(cobVolume, CB_ADDSTRING, 0, (LPARAM)strMenu);
	for(i=0; i<10; i++){
		SendMessage(cobVolume, CB_ADDSTRING, 0, (LPARAM)strVolume[i]);
	}
	SendMessage(cobVolume, CB_SETCURSEL, gCfgAlarm.bVolume, 0);
	//Pause time
	for(i=0; i<3; i++){
		SendMessage(cobPause, CB_ADDSTRING, 0, (LPARAM)strPause[i]);
	}
	SendMessage(cobPause, CB_SETCURSEL, gCfgAlarm.bPauseTime-1, 0);
	//All print
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "off", strMenu, sizeof strMenu);	
	SendMessage(cobAllPrint, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "on", strMenu, sizeof strMenu);	
	SendMessage(cobAllPrint, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobAllPrint, CB_SETCURSEL, gCfgAlarm.bPrint, 0);
	//print time
	for(i=0; i<3; i++){
		SendMessage(cobPrnTime, CB_ADDSTRING, 0, (LPARAM)strPrnTime[i]);
	}
	if(gCfgAlarm.bPrintWaveLength <=4)
		SendMessage(cobPrnTime, CB_SETCURSEL, 0, 0);
	else if(gCfgAlarm.bPrintWaveLength <=8)
		SendMessage(cobPrnTime, CB_SETCURSEL, 1, 0);
	else 
		SendMessage(cobPrnTime, CB_SETCURSEL, 2, 0);
	//view limit	
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "off", strMenu, sizeof strMenu);	
	SendMessage(cobViewLimit, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "on", strMenu, sizeof strMenu);	
	SendMessage(cobViewLimit, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobViewLimit, CB_SETCURSEL, gCfgAlarm.bViewLimit, 0);
				
	return 0;
}
//设置控件回调函数
static int InitCtlCallBackProc(HWND hDlg)
{
	OldBtnProc = SetWindowCallbackProc(btnVolume, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnPause, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnAllPrint, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnPrnTime, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnViewLimit, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnAlarmControl, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnAlarmPrint, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnAlarmLimit, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnAlarmRecall, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);
	
	OldCobProc = SetWindowCallbackProc(cobVolume, CobProc);
	OldCobProc = SetWindowCallbackProc(cobPause, CobProc);
	OldCobProc = SetWindowCallbackProc(cobAllPrint, CobProc);
	OldCobProc = SetWindowCallbackProc(cobPrnTime, CobProc);
	OldCobProc = SetWindowCallbackProc(cobViewLimit, CobProc);
	
	OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp); 
			
	return 0;
}
static int InitCtlStatus(HWND hDlg)
{
	//如果记录仪故障，则该选项不可用
	if(gPrinterStatus == PRNSTATUS_ERR){
		EnableWindow(btnAllPrint, FALSE);
		EnableWindow(cobAllPrint, FALSE);

		EnableWindow(btnPrnTime, FALSE);
		EnableWindow(cobPrnTime, FALSE);

		EnableWindow(btnAlarmPrint, FALSE);
	}
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
			//创建控件
			CreateCtlProc(hDlg);
			//设置控件字符
			InitCtlName(hDlg);
			//初始化combobox
			InitCobList(hDlg);
			//设置控件回调函数
			InitCtlCallBackProc(hDlg);
			//设置控件状态
			InitCtlStatus(hDlg);
			DrawIconProc(hDlg);
			gbKeyType = KEY_TAB;
			SetFocus(btnOk);
		}break;
		case MSG_PAINT:{

		}break;
		case MSG_COMMAND:{
			int	id  = LOWORD(wParam);
			
			switch(id){
				case DID_ALMSETUP_VOLUME_COB:{
					gbKeyType = KEY_UD;	
					SetFocus(cobVolume);
				}break;
				case DID_ALMSETUP_PAUSE:{
					gbKeyType = KEY_UD;	
					SetFocus(cobPause);
				}break;
				case DID_ALMSETUP_ALLPRINT:{
					gbKeyType = KEY_UD;	
					SetFocus(cobAllPrint);
				}break;
				case DID_ALMSETUP_PRNTIME:{
					gbKeyType = KEY_UD;	
					SetFocus(cobPrnTime);
				}break;
				case DID_ALMSETUP_VIEWLIMIT:{
					gbKeyType = KEY_UD;	
					SetFocus(cobViewLimit);
				}break;
				case DID_ALMSETUP_CONTROL:{
					//CreateDlgAlarmCtrl(hDlg);
					CreateAlarmLevel(hDlg);
				}break;
				case DID_ALMSETUP_PRINT:{
					CreateDlgAlarmPrint(hDlg);
				}break;
				case DID_ALMSETUP_LIMIT:{
					//CreateDlgAlarmLimit(hDlg);
					CreateCommonAlarm(hDlg);
				}break;
				case DID_ALMSETUP_RECALL:{
					CreateAlarmReview(hDlg);
					RestorWave();
				}break;
				case DID_ALMSETUP_OK:{
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
			gbKeyType = KEY_LR;	
			
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
void CreateAlarmSetup(HWND hWnd)
{
	DlgSet.controls = CtrlSet;
	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}

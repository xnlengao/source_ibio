/*************************************************************************** 
 *	Module Name:	DlgNew_Monitor
 *
 *	Abstract:	监护仪设置
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2012-12-3 10:14:18
 *	
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"

//DID_MONITOR
#define DID_MONITOR_HELP			DID_MONITORMENU
#define DID_MONITOR_OK				DID_MONITORMENU+1
#define DID_MONITOR_CAPTION			DID_MONITORMENU+2
#define DID_MONITOR_BEEPVOLUME		DID_MONITORMENU+3
#define DID_MONITOR_BEEPVOLUME_COB		DID_MONITORMENU+4
#define DID_MONITOR_ALARMVOLUME		DID_MONITORMENU+5
#define DID_MONITOR_ALARMVOLUME_COB		DID_MONITORMENU+6
#define DID_MONITOR_WAVESET			DID_MONITORMENU+7
#define DID_MONITOR_SELECTMOD		DID_MONITORMENU+8
#define DID_MONITOR_TRENDSET		DID_MONITORMENU+9
#define DID_MONITOR_SHORTTRENDSET		DID_MONITORMENU+10
#define DID_MONITOR_SYSTEMSET		DID_MONITORMENU+11
#define DID_MONITOR_SYSTEMINFO		DID_MONITORMENU+12
#define DID_MONITOR_DEMO			DID_MONITORMENU+13


//帮助对话框
static HWND StaticHelp;
static HWND StaticCaption;
static WNDPROC OldStaticProcHelp;

//其他控件 
static HWND btnOk;
static HWND btnBeepVolume;
static HWND cobBeepVolume;
static HWND btnAlarmVolume;
static HWND cobAlarmVolume;
static HWND btnWaveSet;
static HWND btnSelectMod;
static HWND btnTrendSet;
static HWND btnShortTrendSet;
static HWND btnSystemSet;
static HWND btnSystemInfo;
static HWND btnDemo;

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
		5,DLG_HIGH-50, DLG_LENGHT-15, 40,
		DID_MONITOR_HELP,
		"",
		0
	}
};
/*
	绘制下一页图标函数
*/
static int DrawIconProc(HWND hDlg)
{
	
	int i;
	for(i=2;i<7;i++){
		CreateWindow("static",gstrNextPage,WS_CHILD|WS_VISIBLE,IDC_STATIC,
	 		130, 33+30*i, 30, 16,hDlg,0);
	}
	return(0);
}

/*
	帮助控件回调函数
*/
static int StaticProcHelp(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
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
				case DID_MONITOR_BEEPVOLUME:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MONITOR, "beep_volume", strHelp, sizeof strHelp,"Set volume level.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MONITOR_ALARMVOLUME:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MONITOR, "help_alarm_volume", strHelp, sizeof strHelp,"Set alarm volume level.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MONITOR_WAVESET:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MONITOR, "help_wave_setup", strHelp, sizeof strHelp,"Set screen wave.");	
					SetWindowText(hWnd, strHelp);
				}break;
				/*
				case DID_MONITOR_SELECTMOD:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MONITOR, "select_module", strHelp, sizeof strHelp,"Set module.");	
					SetWindowText(hWnd, strHelp);
				}break;
				*/
				case DID_MONITOR_TRENDSET:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MONITOR, "help_trend_setup", strHelp, sizeof strHelp,"Set trend storge.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MONITOR_SHORTTRENDSET:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MONITOR, "help_short_trend_setup", strHelp, sizeof strHelp,"Set short trend view.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MONITOR_SYSTEMSET:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MONITOR, "help_system_setup", strHelp, sizeof strHelp,"System configuration.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MONITOR_SYSTEMINFO:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MONITOR, "help_system_info", strHelp, sizeof strHelp,"system info.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MONITOR_DEMO:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MONITOR, "help_demo", strHelp, sizeof strHelp,"Start/stop DEMO.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MONITOR_OK:{
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

	temp = (*OldStaticProcHelp)(hWnd, message, wParam, lParam);
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
					case DID_MONITOR_BEEPVOLUME_COB:{
							gCfgEcg.bBeepVolume= index;
							SetIntValueToResFile(gFileSetup, "ECGSetup", "beep_volume",  gCfgEcg.bBeepVolume, 2);
							SetFocus(btnBeepVolume);	
						}break;
					case DID_MONITOR_ALARMVOLUME_COB:{
							gCfgAlarm.bVolume = index;
							SetIntValueToResFile(gFileSetup, "AlarmSetup", "volume",  gCfgAlarm.bVolume, 2);
							SetFocus(btnAlarmVolume);	
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
			SendMessage(StaticHelp, MSG_PAINT, 0, (LPARAM)GetDlgCtrlID(hWnd));
		}break;
		case MSG_KILLFOCUS:{
			SetWindowBkColor(hWnd, KILLFOCUS_COLOR);
		}break;	
	}
	
	temp = (*OldBtnProc)(hWnd, message, wParam, lParam);
	return(temp);
}

//创建控件
static int CreateCtlProc(HWND hDlg)
{
	StaticHelp = GetDlgItem(hDlg, DID_MONITOR_HELP);
	StaticCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE, 
					     DID_MONITOR_CAPTION, 0, 5, 250, 25, hDlg, 0);
	btnBeepVolume = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON |BS_NOBORDER | BS_FLAT,
					DID_MONITOR_BEEPVOLUME, FIRSTBTN_X, FIRSTBTN_Y+BTN_HIGH*0, BTN_LEN, BTN_HIGH-4, hDlg, 0);
	btnAlarmVolume = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON |BS_NOBORDER | BS_FLAT,
					DID_MONITOR_ALARMVOLUME, FIRSTBTN_X, FIRSTBTN_Y+BTN_HIGH*1, BTN_LEN, BTN_HIGH-4, hDlg, 0);
	btnWaveSet = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON |BS_FLAT,
					     DID_MONITOR_WAVESET, FIRSTBTN_X, FIRSTBTN_Y+BTN_HIGH*2, BTN_LEN, BTN_HIGH-4, hDlg, 0);
//	btnTrendSet= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON |BS_FLAT,
//					     DID_MONITOR_TRENDSET, FIRSTBTN_X, FIRSTBTN_Y+BTN_HIGH*3, BTN_LEN, BTN_HIGH-4, hDlg, 0);
	btnShortTrendSet= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON |BS_FLAT,
					     DID_MONITOR_SHORTTRENDSET, FIRSTBTN_X, FIRSTBTN_Y+BTN_HIGH*3, BTN_LEN, BTN_HIGH-4, hDlg, 0);
	btnSystemSet= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON |BS_FLAT,
					     DID_MONITOR_SYSTEMSET, FIRSTBTN_X, FIRSTBTN_Y+BTN_HIGH*4, BTN_LEN, BTN_HIGH-4,hDlg, 0);
	btnSystemInfo= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON |BS_FLAT,
					     DID_MONITOR_SYSTEMINFO, FIRSTBTN_X, FIRSTBTN_Y+BTN_HIGH*5, BTN_LEN, BTN_HIGH-4, hDlg, 0);
	btnDemo= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON |BS_FLAT,
					     DID_MONITOR_DEMO, FIRSTBTN_X, FIRSTBTN_Y+BTN_HIGH*6, BTN_LEN, BTN_HIGH-4, hDlg, 0);
	btnOk = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MONITOR_OK, FIRSTBTN_X, FIRSTBTN_Y+BTN_HIGH*7, BTN_LEN, BTN_HIGH-4, hDlg, 0);
			
	cobBeepVolume = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_MONITOR_BEEPVOLUME_COB,  FIRSTBTN_X+BTN_LEN+10, FIRSTBTN_Y+BTN_HIGH*0, 90, 26, hDlg, 0);
	cobAlarmVolume = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_MONITOR_ALARMVOLUME_COB, FIRSTBTN_X+BTN_LEN+10, FIRSTBTN_Y+BTN_HIGH*1, 90, 26, hDlg, 0);
			
	return 0;
}
//设置控件字符
static int InitCtlName(HWND hDlg)
{
	char strMenu[100];

	//caption
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MONITOR, "caption", strMenu, sizeof strMenu,"Monitor Setup");	
	SetWindowText(StaticCaption,strMenu);		
	//Beep Volume 
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MONITOR, "beep_volume", strMenu, sizeof strMenu,"Beep Volume");	
	SetWindowText(btnBeepVolume,strMenu);		
	//Alarm Volume 
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MONITOR, "alarm_volume", strMenu, sizeof strMenu,"Alarm Volume");	
	SetWindowText(btnAlarmVolume,strMenu);		
	//Wave setup
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MONITOR, "wave_setup", strMenu, sizeof strMenu,"Wave Setup");	
	SetWindowText(btnWaveSet,strMenu);		
	//Trend setup
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MONITOR, "trend_setup", strMenu, sizeof strMenu,"Trend Setup");	
	SetWindowText(btnTrendSet,strMenu);		
	//Short Trend setup
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MONITOR, "short_trend_setup", strMenu, sizeof strMenu,"Short Trend");	
	SetWindowText(btnShortTrendSet,strMenu);		
	//System setup
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MONITOR, "system_setup", strMenu, sizeof strMenu,"System Setup");	
	SetWindowText(btnSystemSet,strMenu);		
	//System info
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MONITOR, "system_info", strMenu, sizeof strMenu,"System Info");	
	SetWindowText(btnSystemInfo,strMenu);		
	//Demo
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MONITOR, "set_demo", strMenu, sizeof strMenu,"Demo");	
	SetWindowText(btnDemo,strMenu);		
	//Exit
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "previous", strMenu, sizeof strMenu,"Previous");	
	SetWindowText(btnOk,strMenu);		
	return 0;
}
//初始化combobox
static int InitCobList(HWND hDlg)
{
	char strMenu[100];
	unsigned char *strVolume[]={"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};
	int i;
	//cobBeepVolume
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "off", strMenu, sizeof strMenu,"OFF");	
	SendMessage(cobBeepVolume, CB_ADDSTRING, 0, (LPARAM)strMenu);
	for(i=0; i<10; i++){
		SendMessage(cobBeepVolume, CB_ADDSTRING, 0, (LPARAM)strVolume[i]);
	}
	SendMessage(cobBeepVolume, CB_SETCURSEL,gCfgEcg.bBeepVolume, 0);
	//cobAlarmVolume
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "off", strMenu, sizeof strMenu,"OFF");	
	SendMessage(cobAlarmVolume, CB_ADDSTRING, 0, (LPARAM)strMenu);
	for(i=0; i<10; i++){
		SendMessage(cobAlarmVolume, CB_ADDSTRING, 0, (LPARAM)strVolume[i]);
	}
	SendMessage(cobAlarmVolume, CB_SETCURSEL, gCfgAlarm.bVolume, 0);
	return 0;
}
//设置控件回调函数
static int InitCtlCallBackProc(HWND hDlg)
{
	OldBtnProc = SetWindowCallbackProc(btnBeepVolume, BtnProc);
	OldCobProc = SetWindowCallbackProc(cobBeepVolume, CobProc);
	OldBtnProc = SetWindowCallbackProc(btnAlarmVolume, BtnProc);
	OldCobProc = SetWindowCallbackProc(cobAlarmVolume, CobProc);
	OldBtnProc = SetWindowCallbackProc(btnWaveSet, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnTrendSet, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnShortTrendSet, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSystemSet, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSystemInfo, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnDemo, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);
	OldStaticProcHelp  = SetWindowCallbackProc(StaticHelp, StaticProcHelp); 

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
			//初始化combobox
			InitCobList(hDlg);
			//设置控件回调函数
			InitCtlCallBackProc(hDlg);

			//绘制下一页图标
			DrawIconProc(hDlg);
			gbKeyType = KEY_TAB;
			//TODO:
			EnableWindow(btnWaveSet, FALSE);
			SetFocus(btnOk);
		}break;
		case MSG_PAINT:{
			

		}break;
		case MSG_COMMAND:{
			int	id  = LOWORD(wParam);
			switch(id){
				case DID_MONITOR_BEEPVOLUME:{
					gbKeyType = KEY_UD;
					SetFocus(cobBeepVolume);
					}break;
				case DID_MONITOR_ALARMVOLUME:{
					gbKeyType = KEY_UD;
					SetFocus(cobAlarmVolume);
					}break;
				case DID_MONITOR_WAVESET:{
					CreateWaveformSet(hDlg);
					}break;
				case DID_MONITOR_TRENDSET:{
					CreateTrendSet(hDlg);
					}break;
				case DID_MONITOR_SHORTTRENDSET:{
					CreateShortTrendSet(hDlg);
					}break;
				case DID_MONITOR_SYSTEMSET:{
					CreateSystemSet(hDlg);
					}break;
				case DID_MONITOR_SYSTEMINFO:{
					CreateSystemINFO(hDlg);
					}break;
				case DID_MONITOR_DEMO:{
					GetWindowRect(btnDemo, &gRC_Input);
						if(gbViewDemoData)
							gbViewDemoData=FALSE;
						else{
							if(PasswordInput(hDlg, gRC_Input, PASSTYPE_DEMO)){
								//CreateDlgFactory(hDlg);
							//	LoadDemoFiles();
								gbViewDemoData = TRUE;
							}
							else{
								//CreateMachineSet(hDlg);
								if(B_PRINTF)printf("%s:%d Error Password of Factory.\n", __FILE__, __LINE__);
							}	
						}
					}break;
				case DID_MONITOR_OK:{
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
			
			
			//退出所有菜单
		//	ReturnMainView();
		}break;
	}
	
	temp = DefaultDialogProc(hDlg, message, wParam, lParam);
	return(temp);
}


/*
	建立对话框
*/
void CreateMonitorSetupMenu(HWND hWnd)
{
	
 	DlgSet.controls = CtrlSet;
 	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}


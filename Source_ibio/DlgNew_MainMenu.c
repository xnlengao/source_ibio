/*************************************************************************** 
 *	Module Name:	DlgNew_MainMenu
 *
 *	Abstract:	新主菜单
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2012-11-30 10:14:18
 *	
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"

//DID_MENU
#define DID_MENU_HELP		DID_MAINMENU
#define DID_MENU_OK			DID_MAINMENU+1
#define DID_MENU_CAPTION	DID_MAINMENU+2
#define DID_MENU_SCREEN		DID_MAINMENU+3
#define DID_MENU_SCREEN_SET		DID_MAINMENU+4
#define DID_MENU_MONITOR		DID_MAINMENU+5
#define DID_MENU_TRENDREVIEW		DID_MAINMENU+6
#define DID_MENU_ALARMREVIEW		DID_MAINMENU+7
#define DID_MENU_ARRREVIEW		DID_MAINMENU+8
#define DID_MENU_ALARMSETUP		DID_MAINMENU+9
#define DID_MENU_PATIENTINFO		DID_MAINMENU+10
#define DID_MENU_CALCULATOR		DID_MAINMENU+11
#define DID_MENU_STANDBY		DID_MAINMENU+12


//帮助对话框
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;

//其他控件 
static HWND btnOk;
static HWND btnScreen;
static HWND cobScreen;
static HWND btnMonitor;
static HWND btnTrendReview;
static HWND btnAlarmReview;
static HWND btnARRReview;
static HWND btnAlarmSetup;
static HWND btnPatientInfo;
static HWND btnCalculator;
static HWND btnStandby;
//控件回调函数
static WNDPROC OldBtnProc;
static WNDPROC OldCobProc;

//用户选择的视图
BYTE bInterface= 0;
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
		//"mledit",
		//WS_CHILD | WS_VISIBLE | ES_AUTOWRAP | WS_BORDER ,
		"static",
		WS_CHILD | WS_VISIBLE | SS_LEFT ,
		5,DLG_HIGH-50, DLG_LENGHT-15, 40,
		DID_MENU_HELP,
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
				case DID_MENU_SCREEN:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MAINMENU, "help_screen", strHelp, sizeof strHelp,"Select different screen.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MENU_MONITOR:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes,STR_SETTING_DLG_MAINMENU, "help_monitor", strHelp, sizeof strHelp,"Set monitor param.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MENU_TRENDREVIEW:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MAINMENU, "help_trendreview", strHelp, sizeof strHelp,"View trend.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MENU_ALARMREVIEW:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MAINMENU, "help_alarmreview", strHelp, sizeof strHelp,"View alarm event.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MENU_ARRREVIEW:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MAINMENU, "help_arrrview", strHelp, sizeof strHelp,"View arrhythmia event.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MENU_ALARMSETUP:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MAINMENU, "help_alarmsetup", strHelp, sizeof strHelp,"Set alarm param.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MENU_PATIENTINFO:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MAINMENU, "help_patientinfo", strHelp, sizeof strHelp,"Patient info set.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MENU_CALCULATOR:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MAINMENU, "help_calculator", strHelp, sizeof strHelp,"Calculator select.");
					SetWindowText(hWnd, strHelp);
				}break;
		/*TODO 屏蔽standby功能 转换成趋势
				case DID_MENU_STANDBY:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes,STR_SETTING_DLG_MAINMENU, "help_standby", strHelp, sizeof strHelp,"Standby mode.");
					SetWindowText(hWnd, strHelp);
				}break;
				*/
				case DID_MENU_STANDBY:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes,STR_SETTING_DLG_MAINMENU, "help_trendreview", strHelp, sizeof strHelp,"View trend.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MENU_OK:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "help_backtomain", strHelp, sizeof strHelp,"Return to main screen.");
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
	static BYTE bOldInterface;
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
						case DID_MENU_SCREEN_SET:{
							switch(index){
								case 0 : bInterface = SCREEN_NORMAL; break;
								case 1 : bInterface = SCREEN_NIBPLIST; break;
								case 2 : bInterface = SCREEN_BIGCHAR; break;
								case 3 : bInterface = SCREEN_SHORTTREND; break;
								case 4 : bInterface = SCREEN_7LEADECG; break;
								case 5 : bInterface = SCREEN_MULTIECG; break;
								case 6 : bInterface = SCREEN_OXYCRG; break;
								case 7 : bInterface = SCREEN_OTHERBED; break;
								case 8 : bInterface = SCREEN_IBP; break;
								default: bInterface = SCREEN_NORMAL; break;
							}
							if(gCfgSystem.bInterface != bInterface){
								bOldInterface=gCfgSystem.bInterface;
								gCfgSystem.bInterface = bInterface;
								SetIntValueToResFile(gFileSetup, "SystemSetup", "interface", gCfgSystem.bInterface, 1);	
								printf("gCfgSystem.bInterface=%d\n",gCfgSystem.bInterface);
								NewDestroyShortTrend();
								NewDestroyCrg();
								NewDestroyOtherBed();
								SwitchParasView(bOldInterface);
								NewInitWaveDraw();
								
								ChangeECGLeadChannel();
							}
						//退出所有菜单
						ReturnMainView();
						//	SetFocus(btnScreen);
						}break;
					}	
				//	gbKeyType = KEY_TAB;
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
	for(i=0;i<7;i++){
		CreateWindow("static",gstrNextPage,WS_CHILD|WS_VISIBLE,IDC_STATIC,
	 		130, 63+30*i, 30, 16,hDlg,0);
	}

	return(0);
}

//创建控件
static int CreateCtlProc(HWND hDlg)
{
	editHelp = GetDlgItem(hDlg, DID_MENU_HELP);
	editCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE  ,
					     DID_MENU_CAPTION, 0, 5, 250, 25, hDlg, 0);
	btnScreen = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON |BS_NOBORDER | BS_FLAT,
					DID_MENU_SCREEN, FIRSTBTN_X, FIRSTBTN_Y+BTN_HIGH*0, BTN_LEN, BTN_HIGH-4, hDlg, 0);
	btnMonitor = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON |BS_FLAT,
					     DID_MENU_MONITOR, FIRSTBTN_X, FIRSTBTN_Y+BTN_HIGH*1, BTN_LEN, BTN_HIGH-4, hDlg, 0);
	btnAlarmReview = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MENU_ALARMREVIEW, FIRSTBTN_X, FIRSTBTN_Y+BTN_HIGH*2, BTN_LEN, BTN_HIGH-4, hDlg, 0);
//TODO:暂时把standby做成趋势按钮
	btnStandby = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MENU_STANDBY, FIRSTBTN_X, FIRSTBTN_Y+BTN_HIGH*3, BTN_LEN, BTN_HIGH-4, hDlg, 0);
	btnARRReview = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MENU_ARRREVIEW, FIRSTBTN_X, FIRSTBTN_Y+BTN_HIGH*4, BTN_LEN, BTN_HIGH-4, hDlg, 0);
	btnAlarmSetup= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MENU_ALARMSETUP, FIRSTBTN_X, FIRSTBTN_Y+BTN_HIGH*5, BTN_LEN, BTN_HIGH-4, hDlg, 0);	
	btnPatientInfo = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MENU_PATIENTINFO, FIRSTBTN_X, FIRSTBTN_Y+BTN_HIGH*6, BTN_LEN, BTN_HIGH-4, hDlg, 0);
	btnCalculator = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MENU_CALCULATOR, FIRSTBTN_X, FIRSTBTN_Y+BTN_HIGH*7, BTN_LEN, BTN_HIGH-4, hDlg, 0);
	btnOk = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MENU_OK, FIRSTBTN_X, FIRSTBTN_Y+BTN_HIGH*8, BTN_LEN, BTN_HIGH-4, hDlg, 0);

	cobScreen = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_MENU_SCREEN_SET, FIRSTBTN_X+BTN_LEN+5, FIRSTBTN_Y+BTN_HIGH*0, 110, 26, hDlg, 150);
			
	return 0;
}
//设置控件字符
static int InitCtlName(HWND hDlg)
{
	char strMenu[100];
	
	//caption
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MAINMENU, "caption", strMenu, sizeof strMenu,"Main Menu");	
	SetWindowText(editCaption,strMenu);
	//Screen select
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MAINMENU, "screen", strMenu, sizeof strMenu,"Screen");	
	SetWindowText(btnScreen,strMenu);
	//Monitor
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MAINMENU, "monitor", strMenu, sizeof strMenu,"Monitor");	
	SetWindowText(btnMonitor,strMenu);
	//Alarm Review
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MAINMENU, "alarm_review", strMenu, sizeof strMenu,"Alarm Review");	
	SetWindowText(btnAlarmReview,strMenu);
	//ARR Review
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MAINMENU, "arr_review", strMenu, sizeof strMenu,"ARR Review");	
	SetWindowText(btnARRReview,strMenu);
	//Alarm setup
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MAINMENU, "alarm_setup", strMenu, sizeof strMenu,"Alarm Setup");	
	SetWindowText(btnAlarmSetup,strMenu);
	//Patient info
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MAINMENU, "patient_info", strMenu, sizeof strMenu,"Patient Info");	
	SetWindowText(btnPatientInfo,strMenu);
	//Calculator
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MAINMENU, "calculator", strMenu, sizeof strMenu,"Calculator");	
	SetWindowText(btnCalculator,strMenu);
	//Standby
	
	memset(strMenu, 0, sizeof strMenu);
	//GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MAINMENU, "standby", strMenu, sizeof strMenu,"Standby");	
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MAINMENU, "trendview", strMenu, sizeof strMenu,"Trend View");	

	SetWindowText(btnStandby,strMenu);
	
	//Exit
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "backtomain", strMenu, sizeof strMenu,"Back to Main");	
	SetWindowText(btnOk,strMenu);				

	return 0;
}
//初始化combobox
static int InitCobList(HWND hDlg)
{
		char strMenu[100];
		memset(strMenu, 0, sizeof strMenu);
		GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MAINMENU, "standard", strMenu, sizeof strMenu,"Standard");	
		SendMessage(cobScreen, CB_ADDSTRING, 0, (LPARAM)strMenu);
		memset(strMenu, 0, sizeof strMenu);
		GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MAINMENU, "nibp_review", strMenu, sizeof strMenu,"NIBP Review");	
		SendMessage(cobScreen, CB_ADDSTRING, 0, (LPARAM)strMenu);
		memset(strMenu, 0, sizeof strMenu);
		GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MAINMENU, "big_number", strMenu, sizeof strMenu,"Big Number");	
		SendMessage(cobScreen, CB_ADDSTRING, 0, (LPARAM)strMenu);
		memset(strMenu, 0, sizeof strMenu);
		GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MAINMENU, "short_trend", strMenu, sizeof strMenu,"Short Trend");	
		SendMessage(cobScreen, CB_ADDSTRING, 0, (LPARAM)strMenu);
		memset(strMenu, 0, sizeof strMenu);
		GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MAINMENU, "7_leads_only", strMenu, sizeof strMenu,"7 Leads Only");	
		SendMessage(cobScreen, CB_ADDSTRING, 0, (LPARAM)strMenu);
		memset(strMenu, 0, sizeof strMenu);
		GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MAINMENU, "7_leads", strMenu, sizeof strMenu,"7 Leads");	
		SendMessage(cobScreen, CB_ADDSTRING, 0, (LPARAM)strMenu);
		memset(strMenu, 0, sizeof strMenu);
		GetStringFromResFiles(gsLanguageRes,STR_SETTING_DLG_MAINMENU, "oxyCRG", strMenu, sizeof strMenu,"OxyCRG");	
		SendMessage(cobScreen, CB_ADDSTRING, 0, (LPARAM)strMenu);
		memset(strMenu, 0, sizeof strMenu);
		GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MAINMENU, "other_bed", strMenu, sizeof strMenu,"Other Bed");	
		SendMessage(cobScreen, CB_ADDSTRING, 0, (LPARAM)strMenu);
		if(gbHaveIbp){
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MAINMENU, "ibp", strMenu, sizeof strMenu,"IBP");	
			SendMessage(cobScreen, CB_ADDSTRING, 0, (LPARAM)strMenu);
		}
		SendMessage(cobScreen, CB_SETCURSEL, gCfgSystem.bInterface, 0);
	return 0;
}
//设置控件回调函数
static int InitCtlCallBackProc(HWND hDlg)
{

	OldBtnProc = SetWindowCallbackProc(btnScreen, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnMonitor, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnAlarmReview, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnARRReview, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnAlarmSetup, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnPatientInfo, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnCalculator, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnStandby, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);
	OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp); 
	
	OldCobProc = SetWindowCallbackProc(cobScreen, CobProc);
	
	return 0;
}

static int InitCtlStatus(HWND hDlg)
{
	
	//EnableWindow(btnARRReview, FALSE);

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
				case DID_MENU_SCREEN:{
					gbKeyType = KEY_UD;
					SetFocus(cobScreen);
				}break;
				case DID_MENU_MONITOR:{
					CreateMonitorSetupMenu(hDlg);
				}break;
				
				case DID_MENU_ALARMREVIEW:{
					ReturnMainView();
					PostMessage (ghWndMain, MSG_KEYDOWN, SCANCODE_R, 0);
				}break;
				
				case DID_MENU_ARRREVIEW:{
					ReturnMainView();
					PostMessage (ghWndMain, MSG_KEYDOWN, SCANCODE_W, 0);
				}break;
				case DID_MENU_ALARMSETUP:{
					CreateAlarmSetup(hDlg);
				//	CreateCommonAlarm(hDlg);
				}break;
				case DID_MENU_PATIENTINFO:{
					CreateDlgPatient(hDlg);
				}break;
				case DID_MENU_CALCULATOR:{
					ReturnMainView();
					PostMessage (ghWndMain, MSG_KEYDOWN, SCANCODE_D, 0);
					//BroadcastMessage(MSG_CLOSE,0,0);
					//PostMessage(hDlg, MSG_CLOSE, 0, 0L);
				}break;
				case DID_MENU_STANDBY:{
					PostMessage (ghWndMain, MSG_KEYDOWN, SCANCODE_Y, 0);
					//Test();
				//	CreateAlarmSetup(ghWndMain);
				}break;
				case DID_MENU_OK:{
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
void CCreateMainMenu(HWND hWnd)
{
	
 	DlgSet.controls = CtrlSet;
 	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}


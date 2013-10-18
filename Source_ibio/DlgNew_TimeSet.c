/*************************************************************************** 
 *	Module Name:	DlgNew_TimeSet
 *
 *	Abstract:系统时间设置
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

//DID_TIMESET
#define DID_TIMESET_HELP			DID_TIMESET
#define DID_TIMESET_OK			DID_TIMESET+1
#define DID_TIMESET_CAPTION		DID_TIMESET+2
#define DID_TIMESET_YEAR			DID_TIMESET+3
#define DID_TIMESET_MONTH		DID_TIMESET+4
#define DID_TIMESET_DAY			DID_TIMESET+5
#define DID_TIMESET_HOUR			DID_TIMESET+6
#define DID_TIMESET_MINUTE		DID_TIMESET+7
#define DID_TIMESET_SECOND		DID_TIMESET+8
#define DID_TIMESET_YEAR_COB		DID_TIMESET+9
#define DID_TIMESET_MONTH_COB		DID_TIMESET+10
#define DID_TIMESET_DAY_COB		DID_TIMESET+11
#define DID_TIMESET_HOUR_COB		DID_TIMESET+12
#define DID_TIMESET_MINUTE_COB		DID_TIMESET+13
#define DID_TIMESET_SECOND_COB		DID_TIMESET+14



//帮助对话框
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;

//其他控件 
static HWND btnOk;
static HWND btnSetYear;
static HWND btnSetMonth;
static HWND btnSetDay;
static HWND btnSetHour;
static HWND btnSetMinute;
static HWND btnSetSecond;
static HWND cobSetYear;
static HWND cobSetMonth;
static HWND cobSetDay;
static HWND cobSetHour;
static HWND cobSetMinute;
static HWND cobSetSecond;

//控件回调函数
static WNDPROC OldBtnProc;
static WNDPROC OldCobProc;

//当前时间
static S_TIME sCurTime;
//修改后的时间
static S_TIME sTmpTime;
static int LimitMonth;

static int SetLimitOfMonth(S_TIME *time);
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
		DID_TIMESET_HELP,
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
				case DID_TIMESET_YEAR:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_TIME, "help_year", strHelp, sizeof strHelp,"Set system time.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_TIMESET_MONTH:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_TIME, "help_month", strHelp, sizeof strHelp,"Set system time.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_TIMESET_DAY:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_TIME, "help_day", strHelp, sizeof strHelp,"Set system time.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_TIMESET_HOUR:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_TIME, "help_hour", strHelp, sizeof strHelp,"Set system time.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_TIMESET_MINUTE:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_TIME, "help_minute", strHelp, sizeof strHelp,"Set system time.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_TIMESET_SECOND:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_TIME, "help_second", strHelp, sizeof strHelp,"Set system time.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_TIMESET_OK:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "help_previous", strHelp, sizeof strHelp,"Update Time or return to the previous menu.");
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
					index = SendMessage(hWnd, CB_GETSPINVALUE, 0, 0);
						
					switch(id){
						case DID_TIMESET_YEAR_COB:{
							sTmpTime.wYear=index;
							SetLimitOfMonth(&sTmpTime);
							if(SendMessage(cobSetDay, CB_GETSPINVALUE, 0, 0)>LimitMonth)
								SendMessage(cobSetDay, CB_SETSPINVALUE, (WPARAM)LimitMonth, 0);
							SetFocus(btnSetYear);
						}break;
						case DID_TIMESET_MONTH_COB:{
							sTmpTime.bMonth=index;
							SetLimitOfMonth(&sTmpTime);
							if(SendMessage(cobSetDay, CB_GETSPINVALUE, 0, 0)>LimitMonth)
								SendMessage(cobSetDay, CB_SETSPINVALUE, (WPARAM)LimitMonth, 0);
							SetFocus(btnSetMonth);
						}break;
						case DID_TIMESET_DAY_COB:{
							sTmpTime.bDay=index;
							SetFocus(btnSetDay);
						}break;
						case DID_TIMESET_HOUR_COB:{
							sTmpTime.bHour=index;
							SetFocus(btnSetHour);
						}break;
						case DID_TIMESET_MINUTE_COB:{
							sTmpTime.bMin=index;
							SetFocus(btnSetMinute);
						}break;
						case DID_TIMESET_SECOND_COB:{
							sTmpTime.bSec=index;
							SetFocus(btnSetSecond);
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
					     DID_TIMESET_CAPTION, 0, 5, 240, 25, hDlg, 0);
	editHelp = GetDlgItem(hDlg, DID_TIMESET_HELP);
	
	

	btnSetYear = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_TIMESET_YEAR, 10, 30+30*0, 125, 26, hDlg, 0);
	btnSetMonth = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_TIMESET_MONTH, 10, 30+30*1, 125, 26, hDlg, 0);
	btnSetDay = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_TIMESET_DAY, 10, 30+30*2, 125, 26, hDlg, 0);
	btnSetHour = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_TIMESET_HOUR, 10, 30+30*3, 125, 26, hDlg, 0);
	btnSetMinute = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_TIMESET_MINUTE, 10, 30+30*4, 125, 26, hDlg, 0);
	btnSetSecond = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_TIMESET_SECOND, 10, 30+30*5, 125, 26, hDlg, 0);
	btnOk = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_TIMESET_OK, 10, 30+30*6, 125, 26, hDlg, 0);
	
	cobSetYear = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_AUTOSPIN, 
					DID_TIMESET_YEAR_COB, 140, 30+30*0, 95, 26, hDlg, 0);
	cobSetMonth = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_AUTOSPIN, 
					DID_TIMESET_MONTH_COB, 140, 30+30*1, 95, 26, hDlg, 0);
	cobSetDay = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_AUTOSPIN, 
					DID_TIMESET_DAY_COB, 140, 30+30*2, 95, 26, hDlg, 0);
	cobSetHour = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_AUTOSPIN, 
					DID_TIMESET_HOUR_COB, 140, 30+30*3, 95, 26, hDlg, 0);
	cobSetMinute = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_AUTOSPIN, 
					DID_TIMESET_MINUTE_COB, 140, 30+30*4, 95, 26, hDlg, 0);
	cobSetSecond = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_AUTOSPIN, 
					DID_TIMESET_SECOND_COB, 140, 30+30*5, 95, 26, hDlg, 0);
	return 0;
}
static int InitCtlName(HWND hDlg)
{
	char strMenu[100];

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_TIME, "caption", strMenu, sizeof strMenu,"Time Setup");
	SetWindowText(editCaption, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "previous", strMenu, sizeof strMenu,"Previous/Update");	
	SetWindowText(btnOk, strMenu);

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_TIME, "year", strMenu, sizeof strMenu,"Year");	
	SetWindowText(btnSetYear, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_TIME, "month", strMenu, sizeof strMenu,"Month");	
	SetWindowText(btnSetMonth, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_TIME, "day", strMenu, sizeof strMenu,"Day");	
	SetWindowText(btnSetDay, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_TIME, "hour", strMenu, sizeof strMenu,"Hour");	
	SetWindowText(btnSetHour, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_TIME, "minute", strMenu, sizeof strMenu,"Minute");	
	SetWindowText(btnSetMinute, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_TIME, "second", strMenu, sizeof strMenu,"Second");	
	SetWindowText(btnSetSecond, strMenu);


	return 0;
}
static int SetLimitOfMonth(S_TIME *time)
{
	
	if(time->bMonth==2){
		if(time->wYear%4==0 && time->wYear%100!=0)
			LimitMonth=29;
		else
			LimitMonth=28;
	}
	else if(time->bMonth==1||time->bMonth==3||time->bMonth==5||
			time->bMonth==7||time->bMonth==8||time->bMonth==10||time->bMonth==12){
		LimitMonth=31;
		}
	else
		LimitMonth=30;

	SendMessage(cobSetDay, CB_SETSPINRANGE, (WPARAM)1, (LPARAM)LimitMonth);
	return 0;
}
static int InitCobList(HWND hDlg)
{
	//得到当前时间
	GetSysTime(&sCurTime);
	sTmpTime.wYear=sCurTime.wYear;
	sTmpTime.bMonth=sCurTime.bMonth;
	sTmpTime.bDay=sCurTime.bDay;
	sTmpTime.bHour=sCurTime.bHour;
	sTmpTime.bMin=sCurTime.bMin;
	sTmpTime.bSec=sCurTime.bSec;
	SetLimitOfMonth(&sCurTime);
	
	SendMessage(cobSetYear, CB_SETSPINRANGE, (WPARAM)1970, (LPARAM)2200);
	SendMessage(cobSetYear, CB_SETSPINVALUE, (WPARAM)sCurTime.wYear, 0);

	SendMessage(cobSetMonth, CB_SETSPINRANGE, (WPARAM)1, (LPARAM)12);
	SendMessage(cobSetMonth, CB_SETSPINVALUE, (WPARAM)sCurTime.bMonth, 0);

	SendMessage(cobSetDay, CB_SETSPINRANGE, (WPARAM)1, (LPARAM)LimitMonth);
	SendMessage(cobSetDay, CB_SETSPINVALUE, (WPARAM)sCurTime.bDay, 0);
	
	SendMessage(cobSetHour, CB_SETSPINRANGE, (WPARAM)0, (LPARAM)23);
	SendMessage(cobSetHour, CB_SETSPINVALUE, (WPARAM)sCurTime.bHour, 0);

	SendMessage(cobSetMinute, CB_SETSPINRANGE, (WPARAM)0, (LPARAM)59);
	SendMessage(cobSetMinute, CB_SETSPINVALUE, (WPARAM)sCurTime.bMin, 0);

	SendMessage(cobSetSecond, CB_SETSPINRANGE, (WPARAM)0, (LPARAM)59);
	SendMessage(cobSetSecond, CB_SETSPINVALUE, (WPARAM)sCurTime.bSec, 0);
	return 0;
}

static int InitCtlCallBackProc(HWND hDlg)
{
	OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);
	OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp);

	OldBtnProc = SetWindowCallbackProc(btnSetYear, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSetMonth, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSetDay, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSetHour, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSetMinute, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSetSecond, BtnProc);

	OldCobProc = SetWindowCallbackProc(cobSetYear, CobProc);
	OldCobProc = SetWindowCallbackProc(cobSetMonth, CobProc);
	OldCobProc = SetWindowCallbackProc(cobSetDay, CobProc);
	OldCobProc = SetWindowCallbackProc(cobSetHour, CobProc);
	OldCobProc = SetWindowCallbackProc(cobSetMinute, CobProc);
	OldCobProc = SetWindowCallbackProc(cobSetSecond, CobProc);
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
			
			gbKeyType = KEY_TAB;
			SetFocus(btnOk);
		}break;
		case MSG_PAINT:{
			

		}break;
		case MSG_COMMAND:{
			int	id  = LOWORD(wParam);
			switch(id){
				case DID_TIMESET_YEAR:{
					gbKeyType = KEY_UD;
					SetFocus(cobSetYear);
					}break;
				case DID_TIMESET_MONTH:{
					gbKeyType = KEY_UD;
					SetFocus(cobSetMonth);
					}break;
				case DID_TIMESET_DAY:{
					gbKeyType = KEY_UD;
					SetFocus(cobSetDay);
					}break;
				case DID_TIMESET_HOUR:{
					gbKeyType = KEY_UD;
					SetFocus(cobSetHour);
					}break;
				case DID_TIMESET_MINUTE:{
					gbKeyType = KEY_UD;
					SetFocus(cobSetMinute);
					}break;
				case DID_TIMESET_SECOND:{
					gbKeyType = KEY_UD;
					SetFocus(cobSetSecond);
					}break;
				case DID_TIMESET_OK:{
					BOOL res = FALSE;
					char strACKInfo[200];
					memset(strACKInfo, 0, sizeof strACKInfo);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_TIME, "ackinfo_update", strACKInfo, sizeof strACKInfo,"Updating the time can make the corresponding change of trend, and former alarm recorder no longer to be reliable.");	
					res = ACKDialog(hDlg, strACKInfo, ACK_ATTENTION);
					
					if(res){
						sTmpTime.wYear = SendMessage(cobSetYear, CB_GETSPINVALUE, 0, 0);
						sTmpTime.bMonth = SendMessage(cobSetMonth, CB_GETSPINVALUE, 0, 0);
						sTmpTime.bDay = SendMessage(cobSetDay, CB_GETSPINVALUE, 0, 0);
						sTmpTime.bHour = SendMessage(cobSetHour, CB_GETSPINVALUE, 0, 0);
						sTmpTime.bMin = SendMessage(cobSetMinute, CB_GETSPINVALUE, 0, 0);
						sTmpTime.bSec = SendMessage(cobSetSecond, CB_GETSPINVALUE, 0, 0);
						SetSysTime(sTmpTime);
						}
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
void CreateTimeSet(HWND hWnd)
{
 	DlgSet.controls = CtrlSet;
 	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}


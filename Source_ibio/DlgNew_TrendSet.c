/*************************************************************************** 
 *	Module Name:	DlgNew_TrendSet
 *
 *	Abstract:趋势存储设置
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2012-12-6 10:14:18
 *	
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"

//DID_MENU
#define DID_TRENDSET_HELP		DID_TRENDSET
#define DID_TRENDSET_OK			DID_TRENDSET+1
#define DID_TRENDSET_CAPTION		DID_TRENDSET+2
#define DID_TRENDSET_INTERVAL				DID_TRENDSET+3
#define DID_TRENDSET_INTERVAL_COB			DID_TRENDSET+4
#define DID_TRENDSET_NIBPSTORAGE			DID_TRENDSET+5
#define DID_TRENDSET_NIBPSTORAGE_COB		DID_TRENDSET+6
#define DID_TRENDSET_ALMSTORAGE				DID_TRENDSET+7
#define DID_TRENDSET_ALMSTORAGE_COB		DID_TRENDSET+8
#define DID_TRENDSET_WARNSTORAGE			DID_TRENDSET+9
#define DID_TRENDSET_WARNSTORAGE_COB		DID_TRENDSET+10



//帮助对话框
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;

//其他控件 
static HWND btnOk;
static HWND btnInterval;
static HWND btnNIBPStorage;
static HWND btnALMStorage;
static HWND btnWarnStorage;
static HWND cobInterval;
static HWND cobNIBPStorage;
static HWND cobALMStorage;
static HWND cobWarnStorage;

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
		DID_TRENDSET_HELP,
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
				case DID_TRENDSET_INTERVAL:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, "TRENDSET", "help_interval", strHelp, sizeof strHelp,"Set auto trend interval time.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_TRENDSET_NIBPSTORAGE:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, "TRENDSET", "help_NIBPstorage", strHelp, sizeof strHelp,"Set to save trend data at every measurement ponit of NIBP.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_TRENDSET_ALMSTORAGE:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, "TRENDSET", "help_ALMstorage", strHelp, sizeof strHelp,"Set to save trend data when a high alarm event happens.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_TRENDSET_WARNSTORAGE:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, "TRENDSET", "help_Warnstorage", strHelp, sizeof strHelp,"Set to save trend data when a middle alarm event happens.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_TRENDSET_OK:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, "TRENDSET", "help_exit", strHelp, sizeof strHelp,"Return to previous menu.");
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
						case DID_TRENDSET_INTERVAL_COB:{
							giTrendInterval= index;
							SetIntValueToResFile(gFileSetup, "GlobalSetup", "TrendInterval",  giTrendInterval, 2);
							SetFocus(btnInterval);	
						}break;
						case DID_TRENDSET_NIBPSTORAGE_COB:{
							gbNIBPStorage= index;
							SetIntValueToResFile(gFileSetup, "GlobalSetup", "NIBPStorage",  gbNIBPStorage, 2);
							SetFocus(btnNIBPStorage);	
						}break;
						case DID_TRENDSET_ALMSTORAGE_COB:{
							gbALMStorage= index;
							SetIntValueToResFile(gFileSetup, "GlobalSetup", "ALMStorage",  gbALMStorage, 2);
							SetFocus(btnALMStorage);	
						}break;
						case DID_TRENDSET_WARNSTORAGE_COB:{
							gbWarnStorage= index;
							SetIntValueToResFile(gFileSetup, "GlobalSetup", "WarnStorage",  gbWarnStorage, 2);
							SetFocus(btnWarnStorage);	
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
	//声音标记
	unsigned char *strInterval[]={"OFF","1 min", "2 min", "3 min", "4 min", "5 min", "10 min", "15 min", "20 min", "25min", "30 min"};
	switch(message){
		case MSG_INITDIALOG:{
			
			//caption
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "TRENDSET", "menu_caption", strMenu, sizeof strMenu,"Trend Store Setup");	
			editCaption = CreateWindow("static", strMenu, WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE, 
					     DID_TRENDSET_CAPTION, 0, 5, 240, 25, hDlg, 0);
			//Interval
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "TRENDSET", "interval", strMenu, sizeof strMenu,"Interval");	
			btnInterval = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_TRENDSET_INTERVAL, 10, 30, 125, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnInterval, BtnProc);

			cobInterval = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_TRENDSET_INTERVAL_COB, 140, 30, 90, 26, hDlg, 0);
			OldCobProc = SetWindowCallbackProc(cobInterval, CobProc);
			
			for(i=0; i<11; i++){
				SendMessage(cobInterval, CB_ADDSTRING, 0, (LPARAM)strInterval[i]);
			}	
			SendMessage(cobInterval, CB_SETCURSEL,giTrendInterval, 0);
			
			//NIBP storage
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "TRENDSET", "NIBPStorage", strMenu, sizeof strMenu,"NIBP Storage");	
			btnNIBPStorage = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_TRENDSET_NIBPSTORAGE, 10, 60, 125, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnNIBPStorage, BtnProc);

			cobNIBPStorage = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_TRENDSET_NIBPSTORAGE_COB, 140, 60, 90, 26, hDlg, 0);
			OldCobProc = SetWindowCallbackProc(cobNIBPStorage, CobProc);
		
			SendMessage(cobNIBPStorage, CB_ADDSTRING, 0, (LPARAM)"OFF");
			SendMessage(cobNIBPStorage, CB_ADDSTRING, 0, (LPARAM)"ON");
			SendMessage(cobNIBPStorage, CB_SETCURSEL,gbNIBPStorage, 0);
			
			//alarm storage
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "TRENDSET", "ALMStorage", strMenu, sizeof strMenu,"ALM Storage");	
			btnALMStorage = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_TRENDSET_ALMSTORAGE, 10, 90, 125, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnALMStorage, BtnProc);

			cobALMStorage = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_TRENDSET_ALMSTORAGE_COB, 140, 90, 90, 26, hDlg, 0);
			OldCobProc = SetWindowCallbackProc(cobALMStorage, CobProc);
		
			SendMessage(cobALMStorage, CB_ADDSTRING, 0, (LPARAM)"OFF");
			SendMessage(cobALMStorage, CB_ADDSTRING, 0, (LPARAM)"ON");
			SendMessage(cobALMStorage, CB_SETCURSEL,gbALMStorage, 0);
			
			//warn storage
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "TRENDSET", "WarnStorage", strMenu, sizeof strMenu,"Warn Storage");	
			btnWarnStorage = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_TRENDSET_WARNSTORAGE, 10, 120, 125, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnWarnStorage, BtnProc);

			cobWarnStorage = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_TRENDSET_WARNSTORAGE_COB, 140, 120, 90, 26, hDlg, 0);
			OldCobProc = SetWindowCallbackProc(cobWarnStorage, CobProc);
		
			SendMessage(cobWarnStorage, CB_ADDSTRING, 0, (LPARAM)"OFF");
			SendMessage(cobWarnStorage, CB_ADDSTRING, 0, (LPARAM)"ON");
			SendMessage(cobWarnStorage, CB_SETCURSEL,gbWarnStorage, 0);
			
			//Exit
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "MENU", "backtomain", strMenu, sizeof strMenu,"Previous");	
			btnOk = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_TRENDSET_OK, 10, 150, 125, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);
			
			editHelp = GetDlgItem(hDlg, DID_TRENDSET_HELP);
			OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp); 
			gbKeyType = KEY_TAB;
			SetFocus(btnOk);
		}break;
		case MSG_PAINT:{
			

		}break;
		case MSG_COMMAND:{
			int	id  = LOWORD(wParam);
			switch(id){
				case DID_TRENDSET_INTERVAL:{
					gbKeyType = KEY_UD;
					SetFocus(cobInterval);
					}break;
				case DID_TRENDSET_NIBPSTORAGE:{
					gbKeyType = KEY_UD;
					SetFocus(cobNIBPStorage);
					}break;
				case DID_TRENDSET_ALMSTORAGE:{
					gbKeyType = KEY_UD;
					SetFocus(cobALMStorage);
					}break;
				case DID_TRENDSET_WARNSTORAGE:{
					gbKeyType = KEY_UD;
					SetFocus(cobWarnStorage);
					}break;
				case DID_TRENDSET_OK:{
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
void CreateTrendSet(HWND hWnd)
{
	
 	DlgSet.controls = CtrlSet;
 	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}


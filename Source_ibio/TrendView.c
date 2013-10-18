/*************************************************************************** 
 *	Module Name:	TrendView
 *
 *	Abstract:	趋势显示
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-07-13 15:24:54
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"
#include "General_Funcs.h"

#define DID_TV					8000
#define DID_TV_HELP				DID_TV+1
#define DID_TV_EXIT				DID_TV+2
#define DID_TV_GRAPH			DID_TV+3
#define DID_TV_TABLE			DID_TV+4
#define DID_TV_NIBP				DID_TV+5
#define DID_TV_CAPTION			DID_TV+6

//帮助对话框
static HWND editHelp;
static HWND stcCaption;
static WNDPROC OldEditProcHelp;
static HWND btnExit;
static HWND btnGraph;
static HWND btnTable;
static HWND btnNibp;

static WNDPROC  OldBtnProc;
static WNDPROC  OldCobProc;

static DLGTEMPLATE DlgTrendView= {
 	WS_VISIBLE ,
	WS_EX_NOCLOSEBOX,   
	DLG_X, DLG_Y, DLG_LENGHT, DLG_HIGH,	
	"",
	0, 0,
	1,      
	NULL,
	0
};

static CTRLDATA CtrlTrendView[] = {
	{
		"static",
		WS_CHILD | WS_VISIBLE | ES_LEFT ,
		10,340, 225, 55,
		DID_TV_HELP,
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
				case DID_TV_GRAPH:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_trend_graph", strHelp, sizeof strHelp);	
 					SetWindowText(hWnd, strHelp);
				}break;
				case DID_TV_TABLE:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_trend_list", strHelp, sizeof strHelp);	
 					SetWindowText(hWnd, strHelp);
				}break;
				case DID_TV_NIBP:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_trend_nibp", strHelp, sizeof strHelp);	
 					SetWindowText(hWnd, strHelp);
				}break;
				case DID_TV_EXIT:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_exit", strHelp, sizeof strHelp);	
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

static  int BtnProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	
	id = GetDlgCtrlID(hWnd);
		
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

static int CobProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	int index;
	//选择的趋势文件名
	char strFileName[20] = {0};
	int res;
	
	id = GetDlgCtrlID(hWnd);
	
	switch(message){
		case MSG_COMMAND:{
		case MSG_KEYUP:{
			if(wParam==SCANCODE_ENTER||CBN_EDITCHANGE==HIWORD(wParam)){
					index = SendMessage(hWnd, CB_GETCURSEL, 0, 0);
// 					switch(id){
// 
// 					}
					gbKeyType = KEY_TAB;
				}break;
			}
			return 0;
		}break;	
	}	
	
	temp = (*OldCobProc)(hWnd, message, wParam, lParam);
	return(temp);
}

static int DlgProcTrendView(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	char strMenu[100];
	int res; 
	RECT rcDlg;
	
	switch(message){
		case MSG_INITDIALOG:{
			//改变波形位置
			GetClientRect(hDlg, &rcDlg);
			ChangeWavePanelsLeft(&rcDlg);

			stcCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE, 
					     DID_TV_CAPTION, 0, 0, 250, 30, hDlg, 0);
			
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "TRENDVIEW", "caption", strMenu, sizeof strMenu);	
			SetWindowText(stcCaption, strMenu);
			
			gbKeyType = KEY_TAB;
						
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "TRENDVIEW", "graph", strMenu, sizeof strMenu);	
			btnGraph = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					      DID_TV_GRAPH, 10, 40, 125, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnGraph, BtnProc);
			
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "TRENDVIEW", "table", strMenu, sizeof strMenu);	
			btnTable = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_TV_TABLE, 10, 70, 125, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnTable, BtnProc);
			
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "TRENDVIEW", "nibp_recall", strMenu, sizeof strMenu);	
			btnNibp = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_TV_NIBP, 10, 100, 125, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnNibp, BtnProc);
			
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "GENERAL", "exit", strMenu, sizeof strMenu);	
			btnExit= CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					      DID_TV_EXIT, 10, 130, 125, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnExit, BtnProc);
			
			editHelp = GetDlgItem(hDlg, DID_TV_HELP);
 			OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp); 
		
			SetFocus(btnExit);
		}break;
		case MSG_PAINT:{
		
		}break;
		case MSG_COMMAND:{
			int	id   = LOWORD(wParam);
			
			switch(id){
				case DID_TV_GRAPH:{
					PostMessage (ghWndMain, MSG_KEYDOWN, SCANCODE_G, 0);
					//CreateTrendGraph(hDlg);
					//RestorWave();
				}break;
				case DID_TV_TABLE:{
					PostMessage (ghWndMain, MSG_KEYDOWN, SCANCODE_T, 0);
					//CreateTrendTable(hDlg);
				//	RestorWave();
				}break;
				case DID_TV_NIBP:{
					PostMessage (ghWndMain, MSG_KEYDOWN, SCANCODE_N, 0);
					//CreateNibpTable(hDlg);
					//RestorWave();
				}break;
				case DID_TV_EXIT:{
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
			//恢复波形的位置
 			ChangeWavePanelsLeft((PRECT)NULL);
		}break;
	}
	
	temp = temp = DefaultDialogProc(hDlg, message, wParam, lParam);
	return temp;
}


//-------------------------------------------------- interface ----------------------------------------------------
/**
	创建趋势显示对话框
*/
void CreateTrendView(HWND hWnd)
{
	DlgTrendView.controls = CtrlTrendView;
	DialogBoxIndirectParam(&DlgTrendView, hWnd, DlgProcTrendView, 0L);    
}


/*************************************************************************** 
 *	Module Name:	Dlg_Freeze
 *
 *	Abstract:	确认波形冻结的对话框
 *			
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-12-14 20:19:51
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"


#define DID_DF_EXIT	        DID_DF+1
#define DID_DF_VIEW	        DID_DF+2

static HWND btnExit;
static HWND btnView;

//控件回调函数
static WNDPROC OldBtnProc;	
	
//对话框属性
static DLGTEMPLATE DlgSet= {
	WS_VISIBLE | WS_BORDER,
	WS_EX_NOCLOSEBOX,	
 	360, 500, 190, 65,	
	"",
	0, 0,
	1,		
	NULL,
	0
};

static CTRLDATA CtrlSet[] = {
	{
		"button",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON, 
		50, 30, 75, 26,
		DID_DF_EXIT,
		"",
		0
	}
};

//信息显示的区域
static RECT rcInfo;

//冻结和解冻波形
extern BOOL gbFreezeKey;

/*
	显示信息
*/
static int ViewInfo(HDC hdc, RECT rc)
{
	char strInfo[100];
	if(hdc == (HDC)NULL) return -11;

	SetBkColor(hdc, COLOR_lightgray);
 	SetTextColor(hdc, COLOR_red);
	SetBrushColor(hdc, COLOR_lightgray);
		
  	FillBox(hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);

	GetStringFromResFile(gsLanguageRes, "GENERAL", "freeze", strInfo, sizeof strInfo);	
	SelectFont(hdc, gFontSystem);
	DrawText(hdc, strInfo, -1, &rc, DT_NOCLIP | DT_CENTER| DT_WORDBREAK);

	return 0;
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
	int temp;
	char strMenu[100];
	int i;
	RECT rcDlg;
		
	switch(message){
		case MSG_INITDIALOG:{
			gbKeyType = KEY_TAB;
			
			//设置信息显示的区域
			GetClientRect(hDlg, &rcDlg);
			SetRect(&rcInfo, rcDlg.left+5, rcDlg.top+5, rcDlg.right -5, rcDlg.top+25);
			/*
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "WAVE_REVIEW", "browse", strMenu, sizeof strMenu);	
			btnView = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_DF_VIEW, 100, 30, 75, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnView, BtnProc);
			*/
			
			//OK
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "GENERAL", "exit", strMenu, sizeof strMenu);	
			btnExit = GetDlgItem(hDlg, DID_DF_EXIT);
			OldBtnProc = SetWindowCallbackProc(btnExit, BtnProc); 
			SetWindowText(btnExit, strMenu);
			
			SetFocus(btnExit);
		}break;
		case MSG_PAINT:{
 			HDC hdc;
			hdc = GetClientDC(hDlg);
			if(hdc !=(HDC)NULL){
				ViewInfo(hdc, rcInfo);
				ReleaseDC(hdc);
			}
		}break;
		case MSG_COMMAND:{
			int	id  = LOWORD(wParam);
			
			switch(id){
				case DID_DF_VIEW:{
					CreateWaveReview(hDlg);
			
					EndDialog(hDlg, wParam);	
					return 0;
				}break;
				case DID_DF_EXIT:{
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
			
			//解冻波形
			UnfreezeWaves();
			gbFreezeKey = FALSE;
			
			//恢复波形位置
// 			ChangeWavePanelsLeft((PRECT)NULL);
			RestorWave();
			
			//回到主界面
			ReturnMainView();
			
			EndDialog(hDlg, wParam);	
		}break;
	}
	
	temp = DefaultDialogProc(hDlg, message, wParam, lParam);
	return(temp);
}

//------------- interface ------------
/*
	建立对话框
*/
void CreateDlgFreeze(HWND hWnd)
{
#if SCREEN_1024	
	DlgSet.x = SCREEN_RIGHT-500,
	DlgSet.y = SCREEN_BOTTOM-100,
#elif  SCREEN_640
	DlgSet.x = SCREEN_RIGHT-460,
	DlgSet.y = SCREEN_BOTTOM-100,
#else
	DlgSet.x = SCREEN_RIGHT-460,
	DlgSet.y = SCREEN_BOTTOM-100,
#endif	
	
	DlgSet.controls = CtrlSet;
	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}

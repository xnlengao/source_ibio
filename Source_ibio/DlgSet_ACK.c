/*************************************************************************** 
 *	Module Name:	DlgSet_ACK
 *
 *	Abstract:	确认信息的对话框
 *			根据信息的关注程度，用不同的背景显示信息
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2007-06-15 13:17:13	
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"

//DID_ACk
#define DID_ACk_INFO			DID_ACk
#define DID_ACk_OK			DID_ACk+1
#define DID_ACk_CANCEL		DID_ACk+2
#define DID_ACk_CAPTION		DID_ACk+2

static HWND editInfo;
static HWND btnOK;
static HWND btnCancel;
static HWND btnInfo;
static HWND editCaption;

//控件回调函数
static WNDPROC OldBtnProc;	
static WNDPROC OldEditProc;

//对话框属性
static DLGTEMPLATE DlgSet= {
	WS_VISIBLE | WS_BORDER,
	WS_EX_NOCLOSEBOX,	
	//5, 64, 240, 150,
	DLG_X, 70, 250, 200,	
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
		10, 310, 215, 50,
		DID_ACk_INFO,
		"",
		0
		
	}

};

//提示信息
static char strInfo[200]={0};
//严重程度
static BYTE bInfoClass = ACK_NORMAL;
//用户选择的结果
static BOOL bResult = FALSE;
//信息显示的区域
static RECT rcInfo;

#define BK_NORMAL		COLOR_lightgray
#define TEXT_NORMAL		COLOR_black
#define BK_ATTENTION		COLOR_darkred
#define TEXT_ATTENTION	COLOR_yellow
#define BK_TERRIBLE		COLOR_darkred
#define TEXT_TERRIBLE		COLOR_yellow


/*
	显示信息
*/
static int ViewInfo(HDC hdc, RECT rc)
{
	if(hdc == (HDC)NULL) return -1;
	
	switch(bInfoClass){
		case ACK_ATTENTION:{
			SetBkColor(hdc, BK_ATTENTION);
			SetTextColor(hdc, TEXT_ATTENTION);
			SetBrushColor(hdc, BK_ATTENTION);
		}break;
		case ACK_TERRIBLE:{
			SetBkColor(hdc, BK_TERRIBLE);
			SetTextColor(hdc, TEXT_TERRIBLE);
			SetBrushColor(hdc, BK_TERRIBLE);
		}break;
		default:{
			SetBkColor(hdc, BK_NORMAL);
			SetTextColor(hdc, TEXT_NORMAL);
			SetBrushColor(hdc, BK_NORMAL);
		}break;
	}
	
	FillBox(hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);

	SelectFont(hdc, gFontSystem);
 	DrawText(hdc, strInfo, -1, &rc, DT_LEFT | DT_WORDBREAK );
	//DrawText(hdc, strInfo, -1, &rc, DT_NOCLIP | DT_LEFT | DT_VCENTER | DT_SINGLELINE);
/*
	memset(strInfo, 0, sizeof strInfo);
	GetStringFromResFile(gsLanguageRes, "HELP", "help_patient_update", strInfo, sizeof strInfo);	
	SetWindowText(editInfo, strInfo);	
*/	
	return 0;
}

/*
	帮助控件回调函数
*/
static int EditProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;

	temp = (*OldEditProc)(hWnd, message, wParam, lParam);
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

			editCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE, 
					     DID_ACk_CAPTION, 0, 0, 240, 30, hDlg, 0);
			
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, STR_SETTING_ACK, "caption", strMenu, sizeof strMenu);	
			SetWindowText(editCaption, strMenu);
			gbKeyType = KEY_TAB;
	
			//设置信息显示的区域
			GetClientRect(hDlg, &rcDlg);
			SetRect(&rcInfo, rcDlg.left+5, rcDlg.top+40, rcDlg.right -5, rcDlg.bottom-45);

	
			//ok
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, STR_SETTING_ACK, "ok", strMenu, sizeof strMenu);	
			btnOK = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_ACk_OK, 10, 160, 95, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnOK, BtnProc);
			
			//cancel
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, STR_SETTING_ACK, "cancel", strMenu, sizeof strMenu);	
			btnCancel = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_ACk_CANCEL, 130, 160, 95, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnCancel, BtnProc);

	
			SetFocus(btnCancel);
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
				case DID_ACk_OK:{
					bResult = TRUE;
					PostMessage(hDlg, MSG_CLOSE, 0, 0L);
				}break;
				case DID_ACk_CANCEL:{
					bResult = FALSE;
					PostMessage(hDlg, MSG_CLOSE, 0, 0L);
				}break;
			}
		}break;	
		case MSG_KEYUP:{
			switch(wParam){
				case SCANCODE_ESCAPE:{
					bResult = FALSE;
					PostMessage(hDlg, MSG_CLOSE, 0, 0L);
				}break;
			}//end switch
		}break;
		case MSG_CLOSE:{
			gbKeyType = KEY_TAB;	
			EndDialog(hDlg, wParam);	
		}break;
	}
	
	temp = DefaultDialogProc(hDlg, message, wParam, lParam);
	return(temp);
}

/*
	建立对话框
*/
static void CreateDlgACK(HWND hWnd)
{
	DlgSet.controls = CtrlSet;
	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}

//------------- interface ------------
/*
	bClass: 严重程度
*/
int ACKDialog(HWND hWnd, const char *info, BYTE bClass)
{
	bResult = FALSE;
	//if(B_PRINTF) printf("ackdialog1111111\n");
	memset(strInfo, 0, sizeof strInfo);
	if(info != NULL){
		snprintf(strInfo, sizeof strInfo, "%s", info);
		bInfoClass = bClass;
	}
	else{
		GetStringFromResFile(gsLanguageRes, STR_SETTING_ACK, "default_info", strInfo, sizeof strInfo);	
		bInfoClass = ACK_NORMAL;
	}

	CreateDlgACK(hWnd);
				
	return bResult;
}

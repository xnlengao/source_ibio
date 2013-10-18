/*************************************************************************** 
 *	Module Name:	SetColor
 *
 *	Abstract:	设置颜色
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-07-02 09:20:22
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"

#define IDD_SETCOLOR_OK				DID_SETCOLOR
#define IDD_SETCOLOR_RED				DID_SETCOLOR+1
#define IDD_SETCOLOR_YELLOW			DID_SETCOLOR+2
#define IDD_SETCOLOR_BLUE				DID_SETCOLOR+3
#define IDD_SETCOLOR_GREEN			DID_SETCOLOR+4
#define IDD_SETCOLOR_CYAN				DID_SETCOLOR+5
#define IDD_SETCOLOR_MAGENTA			DID_SETCOLOR+6
#define IDD_SETCOLOR_LIGHTGRAY		DID_SETCOLOR+7
#define IDD_SETCOLOR_LIGHTWHITE		DID_SETCOLOR+8
#define IDD_SETCOLOR_DARKRED			DID_SETCOLOR+9
#define IDD_SETCOLOR_DARKYELLOW		DID_SETCOLOR+10
#define IDD_SETCOLOR_DARKBLUE			DID_SETCOLOR+11
#define IDD_SETCOLOR_DARKGREEN		DID_SETCOLOR+12
#define IDD_SETCOLOR_DARKCYAN		DID_SETCOLOR+13
#define IDD_SETCOLOR_DARKMAGENTA	DID_SETCOLOR+14
#define IDD_SETCOLOR_DARKGRAY		DID_SETCOLOR+15

HWND btnOK;

//宽度
#define BUTTON_WIDTH	38


//-- 对话框回调函数 --
static int DlgProcKeyboard(HWND hDlg, int message, WPARAM wParam, LPARAM lParam);
//-- 小键盘按钮函数 --
static int BtnProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);

static WNDPROC OldBtnProc;

static DLGTEMPLATE DlgSet= {
	WS_VISIBLE | WS_BORDER,
	WS_EX_NONE,
	5, 64, 190, 150,    //top根据空间的位置变化
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
		10, 112, 80, 25,
		IDD_SETCOLOR_OK,
		"OK",
		0
	}
};

//输入控件的坐标
static RECT rcInput;
//对话框的坐标
static RECT rcDlg;

static int DlgProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	static int	iInputCount =0;
		
	switch(message){
		case MSG_INITDIALOG:{
			HWND btnColor;
			int i, j;
			int iTopOfDlg;
			
			GetWindowRect(hDlg, &rcDlg);
			//移动窗口适应输入控件
			iTopOfDlg = rcInput.bottom;	
			MoveWindow(hDlg, rcDlg.left, gRCDlg_Input.top + iTopOfDlg+22, rcDlg.right-rcDlg.left, rcDlg.bottom - rcDlg.top, FALSE);
					
			gbKeyType = KEY_TAB;
			
			btnOK = GetDlgItem(hDlg, IDD_SETCOLOR_OK);
			OldBtnProc = SetWindowCallbackProc(btnOK, BtnProc); 
			
			for(i=0; i<5; i++){
				btnColor = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
						IDD_SETCOLOR_RED+i, BUTTON_WIDTH*i, 0, BUTTON_WIDTH-4, 25, hDlg, 0);
				OldBtnProc = SetWindowCallbackProc(btnColor, BtnProc); 				
			}
			for(i=5, j=0; i<10; i++,j++){
				btnColor = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
						IDD_SETCOLOR_RED+i, BUTTON_WIDTH*j, 27, BUTTON_WIDTH-4, 25, hDlg, 0);
				OldBtnProc = SetWindowCallbackProc(btnColor, BtnProc); 				
			}
			for(i=11, j=0; i<16; i++,j++){
				btnColor = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
						IDD_SETCOLOR_RED+i, BUTTON_WIDTH*j, 54, BUTTON_WIDTH-4, 25, hDlg, 0);
				OldBtnProc = SetWindowCallbackProc(btnColor, BtnProc); 				
			}
						
			//-- 默认焦点 --
			SendNotifyMessage(hDlg, MSG_KEYDOWN, (WPARAM)SCANCODE_TAB, (LPARAM)0);
		
		}break;
		case MSG_COMMAND:{
			int	id   = LOWORD(wParam);
		
/*			switch(id){
				//-- 删除 --
				case IDD_DLG_KEY_DEL:{
					if(ghWnd_Input != (HWND)NULL)
						SendMessage(ghWnd_Input, MSG_KEYDOWN, (WPARAM)SCANCODE_BACKSPACE, (LPARAM)0);				
				}  break;
				//-- 清空 --
				case IDD_DLG_KEY_CLR:{
					if(ghWnd_Input != (HWND)NULL){
						memset(strInput, 0, sizeof strInput);
						iInputCount = 0;
						SetWindowText(ghWnd_Input, strInput);
					}
				} break;
				//-- 退出 --
				case IDD_DLG_KEY_OK:{
					SendMessage(hDlg, MSG_CLOSE, 0, 0L); 
				} break;
			}
			//--- 字母--
			if(id>=IDD_DLG_KEY_A && id<=IDD_DLG_KEY_9){
				if(iInputCount<MAXINPUT-1){
					strncat(strInput, strKey[id-IDD_DLG_KEY_A], 1);				
// 					printf("%s:%d You have input:%s.\n", __FILE__, __LINE__, strInput);
					if(ghWnd_Input != (HWND)NULL)
						SetWindowText(ghWnd_Input, strInput);
				}
				iInputCount ++;
			}
		*/
		}break;
		case MSG_KEYUP:{
			switch(wParam){
				case SCANCODE_ESCAPE:{
					SendMessage(hDlg, MSG_CLOSE, 0, 0L);
				}break;
			}//end switch
		}break;
		case MSG_CLOSE:{
			
			ghWnd_Input = (HWND)NULL;
			EndDialog(hDlg, wParam);
		}break;
	}

	temp = DefaultDialogProc(hDlg, message, wParam, lParam);
	return(temp);
}

//-- 小键盘对话框按钮函数
static int BtnProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	HDC hdc;
	RECT rc;
	
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
	
//建立小键盘对话框 --
static void CreateDlgColorSet(HWND hWnd)
{
	DlgSet.controls = CtrlSet;
	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}	

//-------- interface -----------
int SetColor(HWND hWnd)
// int SetColor(HWND hWnd, RECT rc)
{
// 	rcInput = rc;
	CreateDlgColorSet(hWnd);
			
	return 0;
}



















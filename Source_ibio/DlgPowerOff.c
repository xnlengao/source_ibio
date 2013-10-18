/*************************************************************************** 
 *	Module Name:	DlgPowerOff
 *
 *	Abstract:	确认电源关闭的对话框
 *			
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2013-06-03 13:17:13	
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"

#define _ID_TIMER  	100


#define DID_POWER_INFO		DID_POWER
#define DID_POWER_OK		DID_POWER+1
#define DID_POWER_CANCEL	DID_POWER+2
#define DID_POWER_CAPTION		DID_POWER+3

static HWND editInfo;
static HWND btnOK;
static HWND btnCancel;
static HWND editCaption;


//控件回调函数
static WNDPROC OldBtnProc;	
static WNDPROC OldEditProc;


//对话框属性
static DLGTEMPLATE DlgSet= {
	WS_VISIBLE | WS_BORDER,
	WS_EX_NOCLOSEBOX,	
	200, 200, 250, 120,	
	"",
	0, 0,
	1,		
	NULL,
	0
};

static CTRLDATA CtrlSet[] = {
	{
		"static",
         	WS_CHILD | WS_VISIBLE | ES_AUTOWRAP |SS_CENTER,
		0, 5, 250, 30,
      	 	DID_POWER_CAPTION,
		"",
		0
	}
};
BOOL bShow_Poweroff;
//提示信息
static char strInfo[200]={0};
//严重程度
static BYTE bInfoClass = ACK_NORMAL;
//用户选择的结果
static BOOL bResult = FALSE;
//信息显示的区域
static RECT rcInfo;

//是否重启系统
static BOOL bRebootSystem = FALSE;
//取消倒计时
static int iTimeCount ;
char strCancel[100];

#define BK_NORMAL			COLOR_lightgray
#define TEXT_NORMAL		COLOR_black
#define BK_ATTENTION		COLOR_darkred
#define TEXT_ATTENTION		COLOR_yellow
#define BK_TERRIBLE			COLOR_darkred
#define TEXT_TERRIBLE		COLOR_yellow


/*
	显示信息
*/
static int ViewPowerInfo(HDC hdc, RECT rc)
{
	
	
	if(hdc == (HDC)NULL) return -1;
	
	SetBkColor(hdc, BK_NORMAL);
	SetTextColor(hdc, TEXT_NORMAL);
	SetBrushColor(hdc, COLOR_darkred);
	
	FillBox(hdc, rc.left, rc.top, RECTW(rc), RECTH(rc));

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
	//char strCancel[100];
	int i;
	RECT rcDlg;
		
	switch(message){
		case MSG_INITDIALOG:{
			bShow_Poweroff=TRUE;
			iTimeCount=10;
			//caption
			 editCaption = GetDlgItem(hDlg, DID_POWER_CAPTION);
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_POWEROFF, "caption", strMenu, sizeof strMenu,"Power Off");	
			SetWindowText(editCaption,strMenu);

			memset(strMenu, 0, sizeof strMenu);
			
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_POWEROFF, "poweroff_info", strMenu, sizeof strMenu,"Are you sure shutdown monitor system?");	


			editInfo = CreateWindow("static", strMenu, WS_CHILD | SS_CENTER | WS_VISIBLE ,
					 DID_POWER_INFO  , 5, 40, 250, 25, hDlg, 0);
			
			//OldEditProc = SetWindowCallbackProc(editInfo, EditProc); 
			//SetWindowText(editInfo, strInfo);
			
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_ACK, "ok", strMenu, sizeof strMenu,"OK");	
			btnOK = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
				     DID_POWER_OK, 15,75, 100, 26, hDlg, 0);
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_ACK, "cancel", strCancel, sizeof strCancel,"Cancel");	
			sprintf( strMenu, "%s(%d)", strCancel,iTimeCount);
			btnCancel= CreateWindow("button",strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
				     DID_POWER_CANCEL, 135, 75, 100, 26, hDlg, 0);
			
			OldBtnProc = SetWindowCallbackProc(btnOK, BtnProc);		
			OldBtnProc = SetWindowCallbackProc(btnCancel, BtnProc);	
			
			gbKeyType = KEY_TAB;
			SetFocus(btnCancel);
			
			//如果是重启计算机，建立1S计时器
			SetTimer(hDlg, _ID_TIMER,100);	
				
		}break;
		case MSG_PAINT:{

  			break;
			HDC hdc;

			//设置信息显示的区域
			GetClientRect(hDlg, &rcDlg);
			SetRect(&rcInfo, rcDlg.left+5, rcDlg.top+5, rcDlg.right -5, rcDlg.bottom-5);

			hdc = BeginPaint(hDlg);
			if(hdc !=(HDC)NULL){
				SetBkColor(hdc, BK_NORMAL);
				SetTextColor(hdc, TEXT_NORMAL);
// 				SetBrushColor(hdc, COLOR_darkred);
	 			SetBrushColor(hdc, BK_NORMAL);
				FillBox(hdc, rcInfo.left, rcInfo.top, RECTW(rcInfo), RECTH(rcInfo));
//  			 	SelectFont(hdc, gFontSystem);
 				DrawText(hdc, strInfo, -1, &rcInfo, DT_NOCLIP | DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				
				EndPaint(hDlg, hdc);
			}
		}break;
		case MSG_TIMER:{
			
			iTimeCount --;
			if(B_PRINTF) printf("%s:%d Reboot time count :%d\n", __FILE__, __LINE__, iTimeCount);
			
			if(iTimeCount<=0){
				//重启系统
				if(B_PRINTF) printf("%s:%d Reboot system.\n", __FILE__, __LINE__, iTimeCount);
				PostMessage(hDlg, MSG_CLOSE, 0, 0L);
			}
			
			sprintf( strMenu, "%s(%d)", strCancel,iTimeCount);
			SetWindowText(btnCancel,strMenu);
			
		}break;
		case MSG_COMMAND:{
			int	id  = LOWORD(wParam);
			
			switch(id){
				case DID_POWER_OK:{
					 CMD_Power(0);
					PostMessage(hDlg, MSG_CLOSE, 0, 0L);
				}break;
				case DID_POWER_CANCEL:{
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
			bShow_Poweroff=FALSE;
			KillTimer (hDlg, _ID_TIMER); 
			EndDialog(hDlg, wParam);	
		}break;
	}

	temp = DefaultDialogProc(hDlg, message, wParam, lParam);
	return(temp);
}

/*
	建立对话框
*/
int  CreateDlgPower(HWND hWnd)
{
	DlgSet.controls = CtrlSet;
	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
	return 0;
}


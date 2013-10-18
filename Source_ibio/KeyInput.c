/*************************************************************************** 
 *	Module Name:	KeyInput
 *
 *	Abstract:	小键盘输入
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2007-06-15 11:25:47
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"


#define IDD_DLG_KEY_A		DID_KEYINPUT
#define IDD_DLG_KEY_B		DID_KEYINPUT+1
#define IDD_DLG_KEY_C		DID_KEYINPUT+2
#define IDD_DLG_KEY_D		DID_KEYINPUT+3
#define IDD_DLG_KEY_E		DID_KEYINPUT+4
#define IDD_DLG_KEY_F		DID_KEYINPUT+5
#define IDD_DLG_KEY_G		DID_KEYINPUT+6
#define IDD_DLG_KEY_H		DID_KEYINPUT+7
#define IDD_DLG_KEY_I		DID_KEYINPUT+8
#define IDD_DLG_KEY_J		DID_KEYINPUT+9
#define IDD_DLG_KEY_K		DID_KEYINPUT+10
#define IDD_DLG_KEY_L		DID_KEYINPUT+11
#define IDD_DLG_KEY_M		DID_KEYINPUT+12
#define IDD_DLG_KEY_N		DID_KEYINPUT+13
#define IDD_DLG_KEY_O		DID_KEYINPUT+14
#define IDD_DLG_KEY_P		DID_KEYINPUT+15
#define IDD_DLG_KEY_Q		DID_KEYINPUT+16
#define IDD_DLG_KEY_R		DID_KEYINPUT+17
#define IDD_DLG_KEY_S		DID_KEYINPUT+18
#define IDD_DLG_KEY_T		DID_KEYINPUT+19
#define IDD_DLG_KEY_U		DID_KEYINPUT+20
#define IDD_DLG_KEY_V		DID_KEYINPUT+21
#define IDD_DLG_KEY_W		DID_KEYINPUT+22
#define IDD_DLG_KEY_X		DID_KEYINPUT+23
#define IDD_DLG_KEY_Y		DID_KEYINPUT+24
#define IDD_DLG_KEY_Z		DID_KEYINPUT+25
#define IDD_DLG_KEY_0		DID_KEYINPUT+26
#define IDD_DLG_KEY_1		DID_KEYINPUT+27
#define IDD_DLG_KEY_2		DID_KEYINPUT+28
#define IDD_DLG_KEY_3		DID_KEYINPUT+29
#define IDD_DLG_KEY_4		DID_KEYINPUT+30
#define IDD_DLG_KEY_5		DID_KEYINPUT+31
#define IDD_DLG_KEY_6		DID_KEYINPUT+32
#define IDD_DLG_KEY_7		DID_KEYINPUT+33
#define IDD_DLG_KEY_8		DID_KEYINPUT+34
#define IDD_DLG_KEY_9		DID_KEYINPUT+35
#define IDD_DLG_KEY_DEL	DID_KEYINPUT+36
#define IDD_DLG_KEY_CLR	DID_KEYINPUT+37
#define IDD_DLG_KEY_OK		DID_KEYINPUT+38
//键盘输入的最大字符数
#define MAXINPUT	8

//键码宽度
#define KEY_WIDTH	27

static unsigned char strSaveConf[50];
static char	strInput[MAXINPUT+1]={0}; 	//输入的字符串
static char	strOldInput[MAXINPUT+1]={0}; 	//输入的字符串	
static int	iInputCount =0;
static int flag=0;	
//-- 对话框回调函数 --
static int DlgProcKeyboard(HWND hDlg, int message, WPARAM wParam, LPARAM lParam);
//-- 小键盘按钮函数 --
static int BtnProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam);

static WNDPROC OldBtnProc;

DLGTEMPLATE DlgKeyboard = {
	WS_VISIBLE | WS_BORDER,
	WS_EX_NONE,
	5, 64, 248, 150,    //top根据空间的位置变化
	"",
	0, 0,
	1,		
	NULL,
	0
};
CTRLDATA CtrlKeyboard[] = {
	{
		"button",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
		0, 0, 25, 25,
		IDD_DLG_KEY_A,
		"A",
		0
	}
};

//输入控件的坐标
static RECT rcInput;
//对话框的坐标
static RECT rcDlg;

//键盘字符定义
static char *strKey[40]={"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", 
	"O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "0", "1", "2", "3", "4", "5", 
	"6", "7", "8", "9", "DEL", "CLR", "OK","Cancel"};

//-- 小键盘回调函数 --
static int DlgProcKeyboard(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	char strMenu[100];
		
	switch(message){
		case MSG_INITDIALOG:{
			HWND btnKey;
			int i, j;
			int iTopOfDlg;
			
			GetWindowRect(hDlg, &rcDlg);
			//移动窗口适应输入控件
//  			iTopOfDlg = rcInput.bottom;	
//  			MoveWindow(hDlg, rcDlg.left, gRCDlg_Input.top + iTopOfDlg+22, rcDlg.right-rcDlg.left, rcDlg.bottom - rcDlg.top, FALSE);
			
			//memset(strInput, 0, sizeof strInput);
			
		//	iInputCount =0;
			
			btnKey = GetDlgItem(hDlg, IDD_DLG_KEY_A);
			OldBtnProc = SetWindowCallbackProc(btnKey, BtnProc); 
			
			for(i=1; i<9; i++){
				btnKey = CreateWindow("button", strKey[i], WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
						IDD_DLG_KEY_A+i, KEY_WIDTH*i, 0, 25, 25, hDlg, 0);
				OldBtnProc = SetWindowCallbackProc(btnKey, BtnProc); 				
			}
			for(i=9, j=0; i<18; i++, j++){
				btnKey = CreateWindow("button", strKey[i], WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
						IDD_DLG_KEY_A+i, KEY_WIDTH*j, 27, 25, 25, hDlg, 0);
				OldBtnProc = SetWindowCallbackProc(btnKey, BtnProc);
			}
			for(i=18, j=0; i<27; i++, j++){
				btnKey = CreateWindow("button", strKey[i], WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
						IDD_DLG_KEY_A+i, KEY_WIDTH*j, 54, 25, 25, hDlg, 0);
				OldBtnProc = SetWindowCallbackProc(btnKey, BtnProc);
			}
			for(i=27, j=0; i<36; i++, j++){
				btnKey = CreateWindow("button", strKey[i], WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
						IDD_DLG_KEY_A+i, KEY_WIDTH*j, 81, 25, 25, hDlg, 0);
				OldBtnProc = SetWindowCallbackProc(btnKey, BtnProc);
			}

			//del
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_KEYBOARD, "delete", strMenu, sizeof strMenu,"DEL");	
			btnKey = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					      IDD_DLG_KEY_DEL, 5, 112, 75, 25, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnKey, BtnProc);
			
			//Clear
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_KEYBOARD, "clear", strMenu, sizeof strMenu,"CLR");	
			btnKey = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					      IDD_DLG_KEY_CLR, 85, 112, 75, 25, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnKey, BtnProc);
			
			//OK
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_KEYBOARD, "ok", strMenu, sizeof strMenu,"OK");	
			btnKey = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					      IDD_DLG_KEY_OK, 165, 112, 75, 25, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnKey, BtnProc);
			
			//-- 默认焦点 --
			SendNotifyMessage(hDlg, MSG_KEYDOWN, (WPARAM)SCANCODE_TAB, (LPARAM)0);
		
		}break;
		case MSG_COMMAND:{
			int	id   = LOWORD(wParam);
		
			switch(id){
				//-- 删除 --
				case IDD_DLG_KEY_DEL:{
					if(ghWnd_Input != (HWND)NULL)
						*(strInput+strlen(strInput)-1)='\0';
						iInputCount --;
						SetWindowText(ghWnd_Input, strInput);
						
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
					//SendMessage(ghWnd_Input, MSG_GETTEXT, sizeof strSaveConf, (LPARAM)strSaveConf);
				//	printf("strSaveConf=%s\n",strSaveConf);
					SendMessage(hDlg, MSG_CLOSE, 0, 0L); 
				} break;
				
			}
			//--- 字母--
			if(id>=IDD_DLG_KEY_A && id<=IDD_DLG_KEY_9){
				printf("%d\n",iInputCount);
				if(iInputCount<MAXINPUT){
					strncat(strInput, strKey[id-IDD_DLG_KEY_A], 1);				
 				//	printf("%s:%d You have input:%s,iInputCount =%d.\n", __FILE__, __LINE__, strInput,iInputCount);
				
					if(ghWnd_Input != (HWND)NULL)
						SetWindowText(ghWnd_Input, strInput);
					iInputCount ++;
				}
				
			}
		
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
static void CreateDlgKeyboard(HWND hWnd,int xlength)
{
	DlgKeyboard.x = rcInput.left-xlength;
 	DlgKeyboard.y = rcInput.bottom+40;
	DlgKeyboard.controls = CtrlKeyboard;
	DialogBoxIndirectParam(&DlgKeyboard, hWnd, DlgProcKeyboard, 0L);	
}	


//-------- interface -----------
int KeyInputs(HWND hWnd, RECT rc,const unsigned char*strinput,int xlength)
{
	
		rcInput = rc;
		memset(strInput, 0, sizeof strInput);
		snprintf(strInput, sizeof strInput, "%s", strinput);
		iInputCount =strlen(strInput);
		printf("strInput=%s iInputCount=%d \n",strInput,iInputCount);
		CreateDlgKeyboard(hWnd,xlength);
		return 0;
	
}

/*************************************************************************** 
 *	Module Name:	ColorSelect
 *
 *	Abstract:	颜色选择
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-11-08 15:51:45
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"


#define IDD_COLOR_OK			IDD_COLOR
#define IDD_COLOR_RED			IDD_COLOR+1
#define IDD_COLOR_DARKRED		IDD_COLOR+2
#define IDD_COLOR_YELLOW		IDD_COLOR+3
#define IDD_COLOR_DARKYELLOW	IDD_COLOR+4
#define IDD_COLOR_BLUE			IDD_COLOR+5
#define IDD_COLOR_DARKBLUE		IDD_COLOR+6
#define IDD_COLOR_GREEN		IDD_COLOR+7
#define IDD_COLOR_DARKGREEN	IDD_COLOR+8
#define IDD_COLOR_CYAN			IDD_COLOR+9
#define IDD_COLOR_DARKCYAN		IDD_COLOR+10
#define IDD_COLOR_MAGNETA		IDD_COLOR+11
#define IDD_COLOR_DARKMAGENTA	IDD_COLOR+12
#define IDD_COLOR_LIGHTGRAY		IDD_COLOR+13
#define IDD_COLOR_DARKGRAY		IDD_COLOR+14
#define IDD_COLOR_LIGHTWHITE	IDD_COLOR+15
#define IDD_COLOR_EXT1			IDD_COLOR+16
#define IDD_COLOR_EXT2			IDD_COLOR+17
#define IDD_COLOR_EXT3			IDD_COLOR+18
#define IDD_COLOR_EXT4			IDD_COLOR+19
#define IDD_COLOR_EXT5			IDD_COLOR+20
#define IDD_COLOR_EXT6			IDD_COLOR+21

#define COLOR_EXT1 			179	//浅紫色
#define COLOR_EXT2			243	//浅粉色
#define COLOR_EXT3 			240	//橙色
#define COLOR_EXT4 			90	//浅青色
#define COLOR_EXT5			161	//浅褐色
#define COLOR_EXT6			47	//天空蓝

static HWND btnRed, btnDarkRed, btnYellow, btnDarkYellow;
static HWND btnBlue, btnDarkBlue, btnGreen, btnDarkGreen;
static HWND btnCyan, btnDarkCyan, btnMagenta, btnDarkMagenta;
static HWND btnLightGray, btnDarkGray, btnLightWhite;
static HWND btnOk;


static HWND btncolorext1,btncolorext2,btncolorext3,btncolorext4,btncolorext5,btncolorext6;
//浅紫色 ，浅粉色，橙色，浅青色，浅褐色，天空蓝。

static WNDPROC OldBtnProc;
static WNDPROC OldBtnProcColor;

//输入控件的坐标
static RECT rcInput;

//显示选择颜色的区域
static RECT rcColorView = {10, 125, 85, 150};

//选择的颜色
static int iSelColor = 0;

static HWND ghWndSelf = (HWND)NULL;


static DLGTEMPLATE DlgSet= {
	WS_VISIBLE | WS_BORDER,
	WS_EX_NONE,
	1, 64, 210, 160,    //top根据空间的位置变化
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
		110, 125, 80, 25,
		IDD_COLOR_OK,
		"",
		0
	}
};


//显示选择的颜色
static int ViewSelColor(HWND hWnd, RECT rc, int iColor)
{
	HDC hdc;
	
	if(hWnd == (HWND)NULL ) return -1;
	
	hdc = GetClientDC(hWnd);
	if(hdc == (HDC)NULL) return -1;
	
	SetBrushColor(hdc, COLOR_lightgray);
	FillBox(hdc, rc.left, rc.top, RECTW(rc), RECTH(rc));
	
	SetBrushColor(hdc, iColor);
	FillBox(hdc, rc.left, rc.top, RECTW(rc), RECTH(rc));

	ReleaseDC(hdc);
	return 0;
}


static int BtnProcColor(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	RECT rc;
	HDC hdc= (HDC)NULL;
	
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
 	}
	
	temp = (*OldBtnProcColor)(hWnd, message, wParam, lParam);
	return(temp);
}

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
static int CreateCtlProc(HWND hDlg)
{
		btnDarkRed = CreateWindow("button", "", WS_CHILD | WS_TABSTOP | WS_VISIBLE,
					IDD_COLOR_DARKRED, 5, 5, 35, 25, hDlg, 0);
		btnYellow = CreateWindow("button", "", WS_CHILD | WS_TABSTOP | WS_VISIBLE,
					IDD_COLOR_YELLOW, 45, 5, 35, 25, hDlg, 0);
		btnDarkYellow = CreateWindow("button", "", WS_CHILD | WS_TABSTOP | WS_VISIBLE,
					IDD_COLOR_DARKYELLOW, 85, 5, 35, 25, hDlg, 0);
		btnBlue = CreateWindow("button", "", WS_CHILD | WS_TABSTOP | WS_VISIBLE,
					IDD_COLOR_BLUE, 125, 5, 35, 25, hDlg, 0);
		btnDarkBlue = CreateWindow("button", "", WS_CHILD | WS_TABSTOP | WS_VISIBLE,
				   	IDD_COLOR_DARKBLUE,165, 5, 35, 25, hDlg, 0);
		
		btnGreen = CreateWindow("button", "", WS_CHILD | WS_TABSTOP | WS_VISIBLE,
					IDD_COLOR_GREEN, 5, 35, 35, 25, hDlg, 0);
		btnDarkGreen = CreateWindow("button", "", WS_CHILD | WS_TABSTOP | WS_VISIBLE,
					IDD_COLOR_DARKGREEN, 45, 35, 35, 25, hDlg, 0);
		btnCyan = CreateWindow("button", "", WS_CHILD | WS_TABSTOP | WS_VISIBLE,
					IDD_COLOR_CYAN, 85, 35, 35, 25, hDlg, 0);
		btnDarkCyan = CreateWindow("button", "", WS_CHILD | WS_TABSTOP | WS_VISIBLE,
					IDD_COLOR_DARKCYAN, 125, 35, 35, 25, hDlg, 0);
		btnMagenta = CreateWindow("button", "", WS_CHILD | WS_TABSTOP | WS_VISIBLE,
					IDD_COLOR_MAGNETA, 165, 35, 35, 25, hDlg, 0);
		
		btnDarkMagenta = CreateWindow("button", "", WS_CHILD | WS_TABSTOP | WS_VISIBLE,
				IDD_COLOR_DARKMAGENTA, 5, 65, 35, 25, hDlg, 0);
		btnLightGray = CreateWindow("button", "", WS_CHILD | WS_TABSTOP | WS_VISIBLE,
				IDD_COLOR_LIGHTGRAY, 45, 65, 35, 25, hDlg, 0);
		btnDarkGray = CreateWindow("button", "", WS_CHILD | WS_TABSTOP | WS_VISIBLE,
				IDD_COLOR_DARKGRAY, 85, 65, 35, 25, hDlg, 0);
		btnLightWhite = CreateWindow("button", "", WS_CHILD | WS_TABSTOP | WS_VISIBLE,
				IDD_COLOR_LIGHTWHITE, 125, 65, 35, 25, hDlg, 0);
		btncolorext1 = CreateWindow("button", "", WS_CHILD | WS_TABSTOP | WS_VISIBLE,
					IDD_COLOR_EXT1, 165, 65, 35, 25, hDlg, 0);

		btncolorext4 = CreateWindow("button", "", WS_CHILD | WS_TABSTOP | WS_VISIBLE,
					IDD_COLOR_EXT4, 5,  95, 35, 25, hDlg, 0);
		btncolorext2 = CreateWindow("button", "", WS_CHILD | WS_TABSTOP | WS_VISIBLE,
				IDD_COLOR_EXT2, 45,  95, 35, 25, hDlg, 0);
		btncolorext5 = CreateWindow("button", "", WS_CHILD | WS_TABSTOP | WS_VISIBLE,
				IDD_COLOR_EXT5, 85,  95, 35, 25, hDlg, 0);
		btncolorext3 = CreateWindow("button", "", WS_CHILD | WS_TABSTOP | WS_VISIBLE,
				IDD_COLOR_EXT3, 125,  95, 35, 25, hDlg, 0);
		btncolorext6	 = CreateWindow("button", "", WS_CHILD | WS_TABSTOP | WS_VISIBLE,
				IDD_COLOR_EXT6, 165,  95, 35, 25, hDlg, 0);

		btnOk  = GetDlgItem(hDlg, IDD_COLOR_OK);

	return 0;
}
//设置控件字符
static int	InitCtlName(HWND hDlg)
{
	char strMenu[100];
	SetWindowBkColor(btnDarkRed, COLOR_darkred);
	SetWindowBkColor(btnYellow, COLOR_yellow);
	SetWindowBkColor(btnDarkYellow, COLOR_darkyellow);
	SetWindowBkColor(btnBlue, COLOR_blue);
	SetWindowBkColor(btncolorext1, COLOR_EXT1);//浅紫色 lilac
	SetWindowBkColor(btncolorext4, COLOR_EXT4);//浅青色light cyan		
	SetWindowBkColor(btnDarkBlue, COLOR_darkblue);
	SetWindowBkColor(btnGreen, COLOR_green);
	SetWindowBkColor(btnDarkGreen, COLOR_darkgreen);
	SetWindowBkColor(btnCyan, COLOR_cyan);
	SetWindowBkColor(btnDarkCyan, COLOR_darkcyan);
	SetWindowBkColor(btncolorext2, COLOR_EXT2);//浅粉色light pink
	SetWindowBkColor(btncolorext5, COLOR_EXT5);//浅褐色
	SetWindowBkColor(btnMagenta, COLOR_magenta);
	SetWindowBkColor(btnDarkMagenta, COLOR_darkmagenta);
	SetWindowBkColor(btnLightGray, COLOR_lightgray);//灰绿grey green
	SetWindowBkColor(btnDarkGray, COLOR_darkgray);//
	SetWindowBkColor(btnLightWhite, COLOR_lightwhite);
	SetWindowBkColor(btncolorext3, COLOR_EXT3);//橙色orange
	SetWindowBkColor(btncolorext6, COLOR_EXT6);//天空蓝sky blue
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "GENERAL", "exit", strMenu, sizeof strMenu);	
	SetWindowText(btnOk, strMenu);
	
	return 0;
}
	//设置控件回调函数
static int InitCtlCallBackProc(HWND hDlg)
{
	
	OldBtnProcColor = SetWindowCallbackProc(btnDarkRed, BtnProcColor); 	
	OldBtnProcColor = SetWindowCallbackProc(btnYellow, BtnProcColor); 
	OldBtnProcColor = SetWindowCallbackProc(btnDarkYellow, BtnProcColor); 				
	OldBtnProcColor = SetWindowCallbackProc(btnBlue, BtnProcColor); 				
	OldBtnProcColor = SetWindowCallbackProc(btncolorext1, BtnProcColor); 				
	OldBtnProcColor = SetWindowCallbackProc(btncolorext4, BtnProcColor); 				
	OldBtnProcColor = SetWindowCallbackProc(btnDarkBlue, BtnProcColor); 				
	OldBtnProcColor = SetWindowCallbackProc(btnGreen, BtnProcColor); 				
	OldBtnProcColor = SetWindowCallbackProc(btnDarkGreen, BtnProcColor); 				
	OldBtnProcColor = SetWindowCallbackProc(btnCyan, BtnProcColor); 				
	OldBtnProcColor = SetWindowCallbackProc(btnDarkCyan, BtnProcColor); 				
	OldBtnProcColor = SetWindowCallbackProc(btncolorext2, BtnProcColor); 				
	OldBtnProcColor = SetWindowCallbackProc(btncolorext5, BtnProcColor); 				
	OldBtnProcColor = SetWindowCallbackProc(btnMagenta, BtnProcColor); 		
	OldBtnProcColor = SetWindowCallbackProc(btnDarkMagenta, BtnProcColor); 				
	OldBtnProcColor = SetWindowCallbackProc(btnLightGray, BtnProcColor); 				
	OldBtnProcColor = SetWindowCallbackProc(btnDarkGray, BtnProcColor); 				
	OldBtnProcColor = SetWindowCallbackProc(btnLightWhite, BtnProcColor); 				
	OldBtnProcColor = SetWindowCallbackProc(btncolorext3, BtnProcColor); 				
	OldBtnProcColor = SetWindowCallbackProc(btncolorext6, BtnProcColor); 				
	OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc); 
	
	return 0;
	
}
static int DlgProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;

	switch(message){
		case MSG_INITDIALOG:{
			
			int iTopOfDlg;
			int iWidth = 38;
			
			ghWndSelf = hDlg;
			//创建控件
			CreateCtlProc(hDlg);
			//设置控件字符
			InitCtlName(hDlg);
			//设置控件回调函数
			InitCtlCallBackProc(hDlg);
			
		//	btnRed = CreateWindow("button", "", WS_CHILD | WS_TABSTOP | WS_VISIBLE,
		//			      IDD_COLOR_RED, 0, 5, 25, 25, hDlg, 0);
		//	OldBtnProcColor = SetWindowCallbackProc(btnRed, BtnProcColor); 				
 		//	SetWindowBkColor(btnRed, COLOR_red);
	
			//-- 默认焦点 --
 			SetFocus(btnOk);
		}break;
		case MSG_PAINT:{
			ViewSelColor(hDlg, rcColorView, iSelColor);
		}break;
		case MSG_COMMAND:{
			int	id   = LOWORD(wParam);
		
			switch(id){
				case IDD_COLOR_RED:{
					iSelColor = COLOR_red;
					ViewSelColor(hDlg, rcColorView, iSelColor);
				}break;
				case IDD_COLOR_DARKRED:{
					iSelColor = COLOR_darkred;
					ViewSelColor(hDlg, rcColorView, iSelColor);
				}break;
				case IDD_COLOR_YELLOW:{
					iSelColor = COLOR_yellow;
					ViewSelColor(hDlg, rcColorView, iSelColor);
				}break;
				case IDD_COLOR_DARKYELLOW:{
					iSelColor = COLOR_darkyellow;
					ViewSelColor(hDlg, rcColorView, iSelColor);
				}break;
				case IDD_COLOR_BLUE:{
					iSelColor = COLOR_blue;
					ViewSelColor(hDlg, rcColorView, iSelColor);
				}break;
				case IDD_COLOR_DARKBLUE:{
					iSelColor = COLOR_darkblue;
					ViewSelColor(hDlg, rcColorView, iSelColor);
				}break;
				case IDD_COLOR_GREEN:{
					iSelColor = COLOR_green;
					ViewSelColor(hDlg, rcColorView, iSelColor);
				}break;
				case IDD_COLOR_DARKGREEN:{
					iSelColor = COLOR_darkgreen;
					ViewSelColor(hDlg, rcColorView, iSelColor);
				}break;
				case IDD_COLOR_CYAN:{
					iSelColor = COLOR_cyan;
					ViewSelColor(hDlg, rcColorView, iSelColor);
				}break;
				case IDD_COLOR_DARKCYAN:{
					iSelColor = COLOR_darkcyan;
					ViewSelColor(hDlg, rcColorView, iSelColor);
				}break;
				case IDD_COLOR_MAGNETA:{
					iSelColor = COLOR_magenta;
					ViewSelColor(hDlg, rcColorView, iSelColor);
				}break;
				case IDD_COLOR_DARKMAGENTA:{
					iSelColor = COLOR_darkmagenta;
					ViewSelColor(hDlg, rcColorView, iSelColor);
				}break;
				case IDD_COLOR_LIGHTGRAY:{
					iSelColor = COLOR_lightgray;
					ViewSelColor(hDlg, rcColorView, iSelColor);
				}break;
				case IDD_COLOR_DARKGRAY:{
					iSelColor = COLOR_darkgray;
					ViewSelColor(hDlg, rcColorView, iSelColor);
				}break;
				case IDD_COLOR_LIGHTWHITE:{
					iSelColor = COLOR_lightwhite;
					ViewSelColor(hDlg, rcColorView, iSelColor);
					}break;
				case IDD_COLOR_EXT1:{
					iSelColor = COLOR_EXT1;
					ViewSelColor(hDlg, rcColorView, iSelColor);
					}break;
				case IDD_COLOR_EXT2:{
					iSelColor = COLOR_EXT2;
					ViewSelColor(hDlg, rcColorView, iSelColor);
					}break;
				case IDD_COLOR_EXT3:{
					iSelColor = COLOR_EXT3;
					ViewSelColor(hDlg, rcColorView, iSelColor);
					}break;
				case IDD_COLOR_EXT4:{
					iSelColor = COLOR_EXT4;
					ViewSelColor(hDlg, rcColorView, iSelColor);
					}break;
				case IDD_COLOR_EXT5:{
					iSelColor = COLOR_EXT5;
					ViewSelColor(hDlg, rcColorView, iSelColor);
					}break;
				case IDD_COLOR_EXT6:{
					iSelColor = COLOR_EXT6;
					ViewSelColor(hDlg, rcColorView, iSelColor);
					}break;
				
				//-- 退出 --
				case IDD_COLOR_OK:{
					SendMessage(hDlg, MSG_CLOSE, 0, 0L); 
				} break;
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
			ghWndSelf = (HWND)NULL;
			EndDialog(hDlg, wParam);
		}break;
	}

	temp = DefaultDialogProc(hDlg, message, wParam, lParam);
	return(temp);
}

static void CreateDlg(HWND hWnd)
{
	DlgSet.y = rcInput.bottom + 52;
	DlgSet.controls = CtrlSet;
	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}	

//-------- interface -----------
int ColorSelect(HWND hWnd, RECT rc, int iColor)
{
	rcInput = rc;
	iSelColor = iColor;
 	CreateDlg(hWnd);

	return iSelColor;
}

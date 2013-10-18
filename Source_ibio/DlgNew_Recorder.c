/*************************************************************************** 
 *	Module Name:	DlgSet_Printer
 *
 *	Abstract:	设置打印功能的对话框
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2008-01-02 14:50:44
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"

//DID_RECORDER				
#define DID_RECORDER_HELP				DID_RECORDER
#define DID_RECORDER_OK				DID_RECORDER+1
#define DID_RECORDER_CAPTION			DID_RECORDER+1
#define DID_RECORDER_WAVE1			DID_RECORDER+3
#define DID_RECORDER_WAVE2			DID_RECORDER+5
#define DID_RECORDER_WAVE3			DID_RECORDER+7
#define DID_RECORDER_DURATION		DID_RECORDER+9
#define DID_RECORDER_SPEED			DID_RECORDER+11
#define DID_RECORDER_AUTO				DID_RECORDER+13
#define DID_RECORDER_WAVE1_COB			DID_RECORDER+2
#define DID_RECORDER_WAVE2_COB			DID_RECORDER+4
#define DID_RECORDER_WAVE3_COB			DID_RECORDER+6
#define DID_RECORDER_DURATION_EDIT		DID_RECORDER+8
#define DID_RECORDER_SPEED_COB			DID_RECORDER+10
#define DID_RECORDER_AUTO_EDIT			DID_RECORDER+12


//帮助对话框
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;
//其他控件 
static HWND btnOk;
static HWND btnWave1, cobWave1;
static HWND btnWave2, cobWave2;
static HWND btnWave3, cobWave3;
static HWND btnDuration, editDuration;
static HWND btnSpeed, cobSpeed;
static HWND btnAuto, editAuto;

//控件回调函数
static WNDPROC OldBtnProc;
static WNDPROC OldCobProc;
static WNDPROC OldEditProc;

//对话框属性
static DLGTEMPLATE DlgSet= {
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
		5,DLG_HIGH-50, DLG_LENGHT-15, 40,
		DID_RECORDER_HELP,
		"",
		0
	}
};


//临时存储上下限，调节的时候使用
static int iTmpTime =0, iTmpInter = 0;

//上下限刻度调整的增量
#define PRINT_LIMIT_TIME_INTER		10 	//秒
#define PRINT_LIMIT_AUTO_INTER		10 	//分钟

//限值
#define PRINT_LIMIT_TIME_MAX		REALPRINTTIME_MAX
#define PRINT_LIMIT_TIME_MIN		0
#define PRINT_LIMIT_AUTO_MAX		REALPRINTAUTO_MAX
#define PRINT_LIMIT_AUTO_MIN		0


/*
	设置上下限文字
	bType:
		0-Time
		1-Auto
*/
static int SetLimitText(HWND hWnd, int iLimit, BYTE bType)
{
	unsigned char strLimit[50] = {0};
	unsigned char strUnit[20] = {0};
	
	memset(strUnit, 0, sizeof strUnit);
	memset(strLimit, 0, sizeof strLimit);
	if(bType == 0){
		GetStringFromResFile(gsLanguageRes, "GENERAL", "unit_second", strUnit, sizeof strUnit);		
		if(iLimit <=0){
			GetStringFromResFile(gsLanguageRes, "PRINTERSET", "continue", strLimit, sizeof strLimit);
		}
		else{
			snprintf(strLimit, sizeof strLimit, "%d %s", iLimit, strUnit);
		}	
	}	
	else{
		GetStringFromResFile(gsLanguageRes, "GENERAL", "unit_min", strUnit, sizeof strUnit);		
		if(iLimit <=0){
			GetStringFromResFile(gsLanguageRes, "GENERAL", "off", strLimit, sizeof strLimit);
		}
		else{
			snprintf(strLimit, sizeof strLimit, "%d %s", iLimit, strUnit);
		}	
	}
	
	SetWindowText(hWnd, strLimit);
	
	return 0;
}

/*
	编辑框回调函数
*/
static int EditProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int iVaue;
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
		case MSG_KEYUP:{
			id = GetDlgCtrlID(hWnd);
			
			switch(wParam){
				case SCANCODE_CURSORBLOCKDOWN:{
					switch(id){
						case DID_RECORDER_DURATION_EDIT:{
							if(iTmpTime < PRINT_LIMIT_TIME_MAX){
								iTmpTime += PRINT_LIMIT_TIME_INTER;
								SetLimitText(hWnd, iTmpTime, 0);			
							}
						}break;
						case DID_RECORDER_AUTO_EDIT:{
							if(iTmpInter < PRINT_LIMIT_AUTO_MAX){
								iTmpInter += PRINT_LIMIT_AUTO_INTER;
								SetLimitText(hWnd, iTmpInter, 1);			
							}
						}break;
					}
				}break;
				case SCANCODE_CURSORBLOCKUP:{
					switch(id){
						case DID_RECORDER_DURATION_EDIT:{
							if(iTmpTime > PRINT_LIMIT_TIME_MIN){
								iTmpTime -= PRINT_LIMIT_TIME_INTER;
								SetLimitText(hWnd, iTmpTime, 0);			
							}
						}break;
						case DID_RECORDER_AUTO_EDIT:{
							if(iTmpInter > PRINT_LIMIT_AUTO_MIN){
								iTmpInter -= PRINT_LIMIT_AUTO_INTER;
								SetLimitText(hWnd, iTmpInter, 1);			
							}
						}break;
					}
				}break;
				case SCANCODE_ENTER:{
					switch(id){
						case DID_RECORDER_DURATION_EDIT:{
							gCfgPrinter.iDuration = iTmpTime;
							SetIntValueToResFile(gFileSetup, "PrintSetup", "duration",  gCfgPrinter.iDuration,3);
							SetFocus(btnDuration);
						}break;
						case DID_RECORDER_AUTO_EDIT:{
							gCfgPrinter.iInter = iTmpInter;
							SetIntValueToResFile(gFileSetup, "PrintSetup", "inter",  gCfgPrinter.iInter,3);
							SetFocus(btnAuto);
						}break;
						
					}	
					gbKeyType = KEY_TAB;
				}break;
			}
			return 0;
		}break;	
	}
	
	temp = (*OldEditProc)(hWnd, message, wParam, lParam);
	return(temp);
}		

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
				case DID_RECORDER_WAVE1:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, STR_SETTING_DLG_PRINTER, "help_printer_wave1", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_RECORDER_WAVE2:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, STR_SETTING_DLG_PRINTER, "help_printer_wave2", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_RECORDER_WAVE3:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, STR_SETTING_DLG_PRINTER, "help_printer_wave3", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_RECORDER_DURATION:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, STR_SETTING_DLG_PRINTER, "help_printer_length", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_RECORDER_SPEED:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes,STR_SETTING_DLG_PRINTER, "help_printer_speed", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_RECORDER_OK:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "help_previous", strHelp, sizeof strHelp);	
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
	列表框消息回调函数
*/
static int CobProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	int index;
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
		case MSG_COMMAND:{
		case MSG_KEYUP:{
			if(wParam==SCANCODE_ENTER||CBN_EDITCHANGE==HIWORD(wParam)){
					id = GetDlgCtrlID(hWnd);
					index = SendMessage(hWnd, CB_GETCURSEL, 0, 0);
					int i;
					switch(id){
						case DID_RECORDER_WAVE1_COB:{
							gCfgPrinter.bWave1 = index;	
							/*
							int Count = SendMessage(hWnd, CB_GETCOUNT, 0, 0);
					
							
							if(gCfgPrinter.bWave1 == gCfgPrinter.bWave2)
								{
									gCfgPrinter.bWave1++;
									gCfgPrinter.bWave1=gCfgPrinter.bWave1%Count;
									SetIntValueToResFile(gFileSetup, "PrintSetup", "wave1",  gCfgPrinter.bWave1, 2);
									SendMessage(cobWave1, CB_SETCURSEL, gCfgPrinter.bWave1, 0);
									//SetFocus(btnWave1);
									if(B_PRINTF) printf("%d %d %d %d",gCfgPrinter.bWave1,gCfgPrinter.bWave2,gCfgPrinter.bWave3,__LINE__);
								}
							
							if(gCfgPrinter.bWave1 == gCfgPrinter.bWave3)
								{
									gCfgPrinter.bWave1++;
									gCfgPrinter.bWave1=gCfgPrinter.bWave1%Count;
									SetIntValueToResFile(gFileSetup, "PrintSetup", "wave1",  gCfgPrinter.bWave1, 2);
									SendMessage(cobWave1, CB_SETCURSEL, gCfgPrinter.bWave1, 0);
									//SetFocus(btnWave1);
									if(B_PRINTF) printf("%d %d %d %d",gCfgPrinter.bWave1,gCfgPrinter.bWave2,gCfgPrinter.bWave3,__LINE__);
								}
							if(gCfgPrinter.bWave1 == gCfgPrinter.bWave2)
								{
									gCfgPrinter.bWave1++;
									gCfgPrinter.bWave1=gCfgPrinter.bWave1%Count;
									SetIntValueToResFile(gFileSetup, "PrintSetup", "wave1",  gCfgPrinter.bWave1, 2);
									SendMessage(cobWave1, CB_SETCURSEL, gCfgPrinter.bWave1, 0);
									//SetFocus(btnWave1);
									if(B_PRINTF) printf("%d %d %d %d",gCfgPrinter.bWave1,gCfgPrinter.bWave2,gCfgPrinter.bWave3,__LINE__);
								}
							
							if(gCfgPrinter.bWave1 == gCfgPrinter.bWave3)
								{
									gCfgPrinter.bWave1++;
									gCfgPrinter.bWave1=gCfgPrinter.bWave1%Count;
									SetIntValueToResFile(gFileSetup, "PrintSetup", "wave1",  gCfgPrinter.bWave1, 2);
									SendMessage(cobWave1, CB_SETCURSEL, gCfgPrinter.bWave1, 0);
									//SetFocus(btnWave1);
									if(B_PRINTF) printf("%d %d %d %d",gCfgPrinter.bWave1,gCfgPrinter.bWave2,gCfgPrinter.bWave3,__LINE__);
								}
							*/
							SetIntValueToResFile(gFileSetup, "PrintSetup", "wave1",  gCfgPrinter.bWave1, 2);
							SendMessage(cobWave1, CB_SETCURSEL, gCfgPrinter.bWave1, 0);
							SetFocus(btnWave1);
				
						}break;
						case DID_RECORDER_WAVE2_COB:{
							gCfgPrinter.bWave2 = index;
							/*
							int Count = SendMessage(hWnd, CB_GETCOUNT, 0, 0);
							if(gCfgPrinter.bWave2 == gCfgPrinter.bWave1)
								{
									gCfgPrinter.bWave2++;
									gCfgPrinter.bWave2=gCfgPrinter.bWave2%Count;
									SetIntValueToResFile(gFileSetup, "PrintSetup", "wave2",  gCfgPrinter.bWave2, 2);
									SendMessage(cobWave2, CB_SETCURSEL, gCfgPrinter.bWave2, 0);
									//SetFocus(btnWave2);
									if(B_PRINTF) printf("%d %d %d %d",gCfgPrinter.bWave1,gCfgPrinter.bWave2,gCfgPrinter.bWave3,__LINE__);
								}
							if(gCfgPrinter.bWave2 == gCfgPrinter.bWave3)
								{
									gCfgPrinter.bWave2++;
									gCfgPrinter.bWave2=gCfgPrinter.bWave2%Count;
									SetIntValueToResFile(gFileSetup, "PrintSetup", "wave2",  gCfgPrinter.bWave2, 2);
									SendMessage(cobWave2, CB_SETCURSEL, gCfgPrinter.bWave2, 0);
									//SetFocus(btnWave2);
									if(B_PRINTF) printf("%d %d %d %d",gCfgPrinter.bWave1,gCfgPrinter.bWave2,gCfgPrinter.bWave3,__LINE__);
								}
							if(gCfgPrinter.bWave2 == gCfgPrinter.bWave1)
								{
									gCfgPrinter.bWave2++;
									gCfgPrinter.bWave2=gCfgPrinter.bWave2%Count;
									SetIntValueToResFile(gFileSetup, "PrintSetup", "wave2",  gCfgPrinter.bWave2, 2);
									SendMessage(cobWave2, CB_SETCURSEL, gCfgPrinter.bWave2, 0);
									//SetFocus(btnWave2);
									if(B_PRINTF) printf("%d %d %d %d",gCfgPrinter.bWave1,gCfgPrinter.bWave2,gCfgPrinter.bWave3,__LINE__);
								}
							if(gCfgPrinter.bWave2 == gCfgPrinter.bWave3)
								{
									gCfgPrinter.bWave2++;
									gCfgPrinter.bWave2=gCfgPrinter.bWave2%Count;
									SetIntValueToResFile(gFileSetup, "PrintSetup", "wave2",  gCfgPrinter.bWave2, 2);
									SendMessage(cobWave2, CB_SETCURSEL, gCfgPrinter.bWave2, 0);
									//SetFocus(btnWave2);
									if(B_PRINTF) printf("%d %d %d %d",gCfgPrinter.bWave1,gCfgPrinter.bWave2,gCfgPrinter.bWave3,__LINE__);
								}
							*/
							SetIntValueToResFile(gFileSetup, "PrintSetup", "wave2",  gCfgPrinter.bWave2, 2);
							SendMessage(cobWave2, CB_SETCURSEL, gCfgPrinter.bWave2, 0);
							SetFocus(btnWave2);

						}break;
						case DID_RECORDER_WAVE3_COB:{
							gCfgPrinter.bWave3 = index;
							/*
							int Count = SendMessage(hWnd, CB_GETCOUNT, 0, 0);
							if(gCfgPrinter.bWave3 == gCfgPrinter.bWave1)
								{
									gCfgPrinter.bWave3++;
									gCfgPrinter.bWave3=gCfgPrinter.bWave3%Count;
									SetIntValueToResFile(gFileSetup, "PrintSetup", "wave3",  gCfgPrinter.bWave3, 2);
									SendMessage(cobWave3, CB_SETCURSEL, gCfgPrinter.bWave3, 0);
									//SetFocus(btnWave3);
									if(B_PRINTF) printf("%d %d %d %d",gCfgPrinter.bWave1,gCfgPrinter.bWave2,gCfgPrinter.bWave3,__LINE__);
								}
							if(gCfgPrinter.bWave3 == gCfgPrinter.bWave2)
								{
									gCfgPrinter.bWave3++;
									gCfgPrinter.bWave3=gCfgPrinter.bWave3%Count;
									SetIntValueToResFile(gFileSetup, "PrintSetup", "wave3",  gCfgPrinter.bWave3, 2);
									SendMessage(cobWave3, CB_SETCURSEL, gCfgPrinter.bWave3, 0);
									//SetFocus(btnWave3);
									if(B_PRINTF) printf("%d %d %d %d",gCfgPrinter.bWave1,gCfgPrinter.bWave2,gCfgPrinter.bWave3,__LINE__);
								}
							if(gCfgPrinter.bWave3 == gCfgPrinter.bWave1)
								{
									gCfgPrinter.bWave3++;
									gCfgPrinter.bWave3=gCfgPrinter.bWave3%Count;
									SetIntValueToResFile(gFileSetup, "PrintSetup", "wave3",  gCfgPrinter.bWave3, 2);
									SendMessage(cobWave3, CB_SETCURSEL, gCfgPrinter.bWave3, 0);
									//SetFocus(btnWave3);
									if(B_PRINTF) printf("%d %d %d %d",gCfgPrinter.bWave1,gCfgPrinter.bWave2,gCfgPrinter.bWave3,__LINE__);
								}
							if(gCfgPrinter.bWave3 == gCfgPrinter.bWave2)
								{
									gCfgPrinter.bWave3++;
									gCfgPrinter.bWave3=gCfgPrinter.bWave3%Count;
									SetIntValueToResFile(gFileSetup, "PrintSetup", "wave3",  gCfgPrinter.bWave3, 2);
									SendMessage(cobWave3, CB_SETCURSEL, gCfgPrinter.bWave3, 0);
									//SetFocus(btnWave3);
									if(B_PRINTF) printf("%d %d %d %d",gCfgPrinter.bWave1,gCfgPrinter.bWave2,gCfgPrinter.bWave3,__LINE__);
								}
								*/
							SetIntValueToResFile(gFileSetup, "PrintSetup", "wave3",  gCfgPrinter.bWave3, 2);
							SendMessage(cobWave3, CB_SETCURSEL, gCfgPrinter.bWave3, 0);
							SetFocus(btnWave3);
					
						}break;
						case DID_RECORDER_SPEED_COB:{
							gCfgPrinter.bSpeed = index;
							SetIntValueToResFile(gFileSetup, "PrintSetup", "speed",  gCfgPrinter.bSpeed, 2);
							
							Set_Printer_Speed();
							SetFocus(btnSpeed);	
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

//创建控件
static int CreateCtlProc(HWND hDlg)
{
	editHelp = GetDlgItem(hDlg, DID_RECORDER_HELP);
	editCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE  ,
					     DID_RECORDER_CAPTION, 0, 5, 250, 25, hDlg, 0);
	btnWave1 = CreateWindow("button",  "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
 					DID_RECORDER_WAVE1, 10, 30+30*0, 125, 26, hDlg, 0);
	btnWave2 = CreateWindow("button",  "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_RECORDER_WAVE2, 10, 30+30*1, 125, 26, hDlg, 0);
	btnWave3 = CreateWindow("button",  "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_RECORDER_WAVE3, 10, 30+30*2, 125, 26, hDlg, 0);
	btnSpeed = CreateWindow("button",  "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_RECORDER_SPEED, 10, 30+30*3, 125, 26, hDlg, 0);
	btnDuration = CreateWindow("button",  "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_RECORDER_DURATION, 10, 30+30*4, 125, 26, hDlg, 0);
// 	btnAuto = CreateWindow("button",  "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
// 					DID_RECORDER_AUTO, 10, 30+30*0, 125, 25, hDlg, 0);
	btnOk = CreateWindow("button",  "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_RECORDER_OK, 10, 30+30*5, 125, 26, hDlg, 0);
			
	cobWave1 = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
						DID_RECORDER_WAVE1_COB, 140, 30+30*0, 95, 26, hDlg, 0);
	cobWave2 = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_RECORDER_WAVE2_COB, 140, 30+30*1, 95, 26, hDlg, 0);
	cobWave3 = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_RECORDER_WAVE3_COB, 140, 30+30*2, 95, 26, hDlg, 0);
	cobSpeed = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_RECORDER_SPEED_COB, 140, 30+30*3, 95, 26, hDlg, 0);
	editDuration = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_RECORDER_DURATION_EDIT, 140, 30+30*4, 95, 24, hDlg, 0);
// 			editAuto = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
// 					DID_RECORDER_AUTO_EDIT, 140, 30+30*0, 95, 23, hDlg, 0);	

	return 0;
}
//设置控件字符
static int InitCtlName(HWND hDlg)
{
	
	char strMenu[100];
	//caption
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes,STR_SETTING_DLG_PRINTER, "caption", strMenu, sizeof strMenu,"Recorder Setup");	
	SetWindowText(editCaption,strMenu);

	//Wave1
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_DLG_PRINTER, "wave1", strMenu, sizeof strMenu);	
	SetWindowText(btnWave1,strMenu);
	//Wave2
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_DLG_PRINTER, "wave2", strMenu, sizeof strMenu);	
	SetWindowText(btnWave2,strMenu);
	//Wave3
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_DLG_PRINTER, "wave3", strMenu, sizeof strMenu);	
	SetWindowText(btnWave3,strMenu);
	//Speed
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_DLG_PRINTER, "speed", strMenu, sizeof strMenu);	
	SetWindowText(btnSpeed,strMenu);
	//Duration
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_DLG_PRINTER, "duration", strMenu, sizeof strMenu);	
	SetWindowText(btnDuration,strMenu);

	//Exit
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "previous", strMenu, sizeof strMenu);	
	SetWindowText(btnOk,strMenu);
	
	return 0;
}
//初始化combobox
static int InitCobList(HWND hDlg)
{
	int i;
	char strMenu[100];
	
	
	//填充Wave1 Wave2 Wave3
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "GENERAL", "off", strMenu, sizeof strMenu);	
	//SendMessage(cobWave1, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobWave2, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobWave3, CB_ADDSTRING, 0, (LPARAM)strMenu);
	
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "WAVE", "ecg1", strMenu, sizeof strMenu);	
	SendMessage(cobWave1, CB_ADDSTRING, 0, (LPARAM)strMenu);
//	SendMessage(cobWave2, CB_ADDSTRING, 0, (LPARAM)strMenu);
//	SendMessage(cobWave3, CB_ADDSTRING, 0, (LPARAM)strMenu);
	if(gCfgEcg.bLeadType==0){
		memset(strMenu, 0, sizeof strMenu);
		GetStringFromResFile(gsLanguageRes, "WAVE", "ecg2", strMenu, sizeof strMenu);	
		SendMessage(cobWave1, CB_ADDSTRING, 0, (LPARAM)strMenu);
//		SendMessage(cobWave2, CB_ADDSTRING, 0, (LPARAM)strMenu);
//		SendMessage(cobWave3, CB_ADDSTRING, 0, (LPARAM)strMenu);
	}
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "WAVE", "spo2", strMenu, sizeof strMenu);	
//	SendMessage(cobWave1, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobWave2, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobWave3, CB_ADDSTRING, 0, (LPARAM)strMenu);
	
	
	if(gbHaveIbp){
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "WAVE", "ibp1", strMenu, sizeof strMenu);	
	//	SendMessage(cobWave1, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobWave2, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobWave3, CB_ADDSTRING, 0, (LPARAM)strMenu);
	
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "WAVE", "ibp2", strMenu, sizeof strMenu);	
	//	SendMessage(cobWave1, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobWave2, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobWave3, CB_ADDSTRING, 0, (LPARAM)strMenu);
		}
	
	if(gbHaveCo2){
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "WAVE", "co2", strMenu, sizeof strMenu);	
	//	SendMessage(cobWave1, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobWave2, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobWave3, CB_ADDSTRING, 0, (LPARAM)strMenu);
		}
	else{
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "WAVE", "resp", strMenu, sizeof strMenu);	
	//	SendMessage(cobWave1, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobWave2, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobWave3, CB_ADDSTRING, 0, (LPARAM)strMenu);
	}
	SendMessage(cobWave1, CB_SETCURSEL, gCfgPrinter.bWave1, 0);
	SendMessage(cobWave2, CB_SETCURSEL, gCfgPrinter.bWave2, 0);
	SendMessage(cobWave3, CB_SETCURSEL, gCfgPrinter.bWave3, 0);

//speed
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "GENERAL", "speed_125", strMenu, sizeof strMenu);	
	SendMessage(cobSpeed, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "GENERAL", "speed_25", strMenu, sizeof strMenu);	
	SendMessage(cobSpeed, CB_ADDSTRING, 0, (LPARAM)strMenu);
 	memset(strMenu, 0, sizeof strMenu);
 	GetStringFromResFile(gsLanguageRes, "GENERAL", "speed_50", strMenu, sizeof strMenu);	
 	SendMessage(cobSpeed, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobSpeed, CB_SETCURSEL, gCfgPrinter.bSpeed, 0);	
	return 0;
}

//初始化Edit
static int InitEditText(HWND hDlg)
{	
	//获得报警限值
	iTmpTime = gCfgPrinter.iDuration;
	iTmpInter = gCfgPrinter.iInter;
	SetLimitText(editDuration, gCfgPrinter.iDuration, 0);
//  	SetLimitText(editAuto, gCfgPrinter.iInter, 1);
 		
	return 0;
}
//设置控件回调函数
static int InitCtlCallBackProc(HWND hDlg)
{
	OldBtnProc = SetWindowCallbackProc(btnWave1, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnWave2, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnWave3, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnDuration, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSpeed, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);
// 			OldBtnProc = SetWindowCallbackProc(btnAuto, BtnProc);

	OldCobProc = SetWindowCallbackProc(cobWave1, CobProc);
	OldCobProc = SetWindowCallbackProc(cobWave2, CobProc);
	OldCobProc = SetWindowCallbackProc(cobWave3, CobProc);
	OldEditProc = SetWindowCallbackProc(editDuration, EditProc);
	OldCobProc = SetWindowCallbackProc(cobSpeed, CobProc);
// 			OldEditProc = SetWindowCallbackProc(editAuto, EditProc);
	OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp); 
			
	return 0;
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
			//创建控件
			CreateCtlProc(hDlg);
			//设置控件字符
			InitCtlName(hDlg);
			//初始化combobox
			InitCobList(hDlg);
			//初始化Edit
			InitEditText(hDlg);
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
				case DID_RECORDER_WAVE1:{
					gbKeyType = KEY_UD;	
					SetFocus(cobWave1);
				}break;
				case DID_RECORDER_WAVE2:{
					gbKeyType = KEY_UD;	
					SetFocus(cobWave2);
				}break;
				case DID_RECORDER_WAVE3:{
					gbKeyType = KEY_UD;	
					SetFocus(cobWave3);
				}break;
				case DID_RECORDER_DURATION:{
					gbKeyType = KEY_UD;	
					SetFocus(editDuration);
				}break;
				case DID_RECORDER_SPEED:{
					gbKeyType = KEY_UD;	
					SetFocus(cobSpeed);
				}break;
				case DID_RECORDER_AUTO:{
					gbKeyType = KEY_UD;	
					SetFocus(editAuto);
				}break;
				case DID_RECORDER_OK:{
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
			
			EndDialog(hDlg, wParam);	
		}break;
	}
	
	temp = DefaultDialogProc(hDlg, message, wParam, lParam);
	return(temp);
}


/*
	建立对话框
*/
void CreateRecorderSet(HWND hWnd)
{
	DlgSet.controls = CtrlSet;
	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}

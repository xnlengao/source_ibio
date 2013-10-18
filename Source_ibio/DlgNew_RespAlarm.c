/*************************************************************************** 
 *	Module Name:	DlgNew_RespAlarm
 *
 *	Abstract:Ecg设置中的报警设置选项
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2013-1-21 	16:14:18
 *	
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"

//DID_NEWRESPALMALM
#define DID_NEWRESPALM_HELP			DID_NEWRESPALM
#define DID_NEWRESPALM_OK			DID_NEWRESPALM+1
#define DID_NEWRESPALM_CAPTION		DID_NEWRESPALM+2
#define DID_NEWRESPALM_ALM			DID_NEWRESPALM+3
#define DID_NEWRESPALM_ALMPRINT		DID_NEWRESPALM+4
#define DID_NEWRESPALM_HIGH_RR		DID_NEWRESPALM+5
#define DID_NEWRESPALM_LOW_RR		DID_NEWRESPALM+6
#define DID_NEWRESPALM_ALM_COB			DID_NEWRESPALM+7
#define DID_NEWRESPALM_ALMPRINT_COB		DID_NEWRESPALM+8
#define DID_NEWRESPALM_HIGH_RR_EDIT		DID_NEWRESPALM+9
#define DID_NEWRESPALM_LOW_RR_EDIT		DID_NEWRESPALM+10


//上下限刻度调整的增量
#define RR_LIMIT_INTER		1	//rpm

//限值
#define RR_LIMIT_MAX		RR_MAX
#define RR_LIMIT_MIN		RR_MIN
//上下限值最小幅度
#define RR_LIMITRANGE_MIN 		1
//帮助对话框
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;

//其他控件 
static HWND btnOk;
static HWND btnALM;
static HWND btnALMPrint;
static HWND btnHigh_RR;
static HWND btnLow_RR;

static HWND cobALM;
static HWND cobALMPrint;
static HWND editHigh_RR;
static HWND editLow_RR;


//控件回调函数
static WNDPROC OldBtnProc;
static WNDPROC OldCobProc;
static WNDPROC OldEditProc;


//临时存储上下限，调节的时候使用
static int iTmpHigh_RR =0, iTmpLow_RR = 0;

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
		DID_NEWRESPALM_HELP,
		"",
		0
	}
};

/*
	设置上下限文字
	bType: 0-RR, 1-Apnea
*/
static int SetLimitText(HWND hWnd, int iLimit)
{
	unsigned char strLimit[50] = {0};
	unsigned char strUnit[20] = {0};
	
	memset(strUnit, 0, sizeof strUnit);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "unit_rpm", strUnit, sizeof strUnit,"rpm");		
	memset(strLimit, 0, sizeof strLimit);
	snprintf(strLimit, sizeof strLimit, "%d %s", iLimit, strUnit);

	SetWindowText(hWnd, strLimit);
	
	return 0;
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
			
				case DID_NEWRESPALM_ALM:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "help_alm_class", strHelp, sizeof strHelp,"Set up alarm ON/OFF and class.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWRESPALM_ALMPRINT:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "help_alm_print", strHelp, sizeof strHelp,"Setup print or not when alarm.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWRESPALM_HIGH_RR:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_RESP_ALM, "help_rrhigh", strHelp, sizeof strHelp,"Set up RR alarm high limit.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWRESPALM_LOW_RR:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_RESP_ALM, "help_rrlow", strHelp, sizeof strHelp,"Set up RR alarm low limit.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWRESPALM_OK:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "help_previous", strHelp, sizeof strHelp,"Return to the previous menu.");
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
						case DID_NEWRESPALM_ALM_COB:{
							gCfgResp.bAlmControl = index;
							SetIntValueToResFile(gFileSetup, STR_SETTING_DLG_RESP_ALM, "alarm_control",  gCfgResp.bAlmControl, 1);
							SetFocus(btnALM);
							IsAlm_RR(&gValueResp, &gCfgResp, TRUE);
						}break;
						case DID_NEWRESPALM_ALMPRINT_COB:{
							gCfgResp.bPrnControl = index;
							SetIntValueToResFile(gFileSetup, STR_SETTING_DLG_RESP_ALM, "alarm_print",  gCfgResp.bPrnControl, 1);
							SetFocus(btnALMPrint);
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
						case DID_NEWRESPALM_HIGH_RR_EDIT:{
							if(iTmpHigh_RR < RR_LIMIT_MAX){
								iTmpHigh_RR += RR_LIMIT_INTER;
								SetLimitText(hWnd, iTmpHigh_RR);			
							}
						}break;
						case DID_NEWRESPALM_LOW_RR_EDIT:{
							if(iTmpLow_RR < (iTmpHigh_RR - RR_LIMITRANGE_MIN)){
								iTmpLow_RR += RR_LIMIT_INTER;
								SetLimitText(hWnd, iTmpLow_RR);			
							}
						}break;
						
					}
				}break;
				case SCANCODE_CURSORBLOCKUP:{
					switch(id){
						case DID_NEWRESPALM_HIGH_RR_EDIT:{
							if(iTmpHigh_RR > (iTmpLow_RR + RR_LIMITRANGE_MIN)){
								iTmpHigh_RR -= RR_LIMIT_INTER;
								SetLimitText(hWnd, iTmpHigh_RR);			
							}
						}break;
						case DID_NEWRESPALM_LOW_RR_EDIT:{
							if(iTmpLow_RR > RR_LIMIT_MIN){
								iTmpLow_RR -= RR_LIMIT_INTER;
								SetLimitText(hWnd, iTmpLow_RR);			
							}
						}break;
					
					}
				}break;
				case SCANCODE_ENTER:{
					switch(id){
						case DID_NEWRESPALM_HIGH_RR_EDIT:{
							gCfgResp.wHigh_RR = iTmpHigh_RR;
							SetIntValueToResFile(gFileSetup, "RESPSetup", "rr_high",  gCfgResp.wHigh_RR, 3);
							SetFocus(btnHigh_RR);
						}break;
						case DID_NEWRESPALM_LOW_RR_EDIT:{
							gCfgResp.wLow_RR = iTmpLow_RR;
							SetIntValueToResFile(gFileSetup, "RESPSetup", "rr_low",  gCfgResp.wLow_RR, 3);
							SetFocus(btnLow_RR);
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
	创建控件函数
*/
static int CreateCtlProc(HWND hDlg)
{
	editCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE, 
					     DID_NEWRESPALM_CAPTION, 0, 5, 240, 25, hDlg, 0);
	editHelp = GetDlgItem(hDlg, DID_NEWRESPALM_HELP);

	btnALM = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWRESPALM_ALM, 10, 30+30*0, 125, 26, hDlg, 0);
	btnALMPrint= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWRESPALM_ALMPRINT, 10, 30+30*1, 125, 26, hDlg, 0);
	btnHigh_RR = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWRESPALM_HIGH_RR, 10, 30+30*2, 125, 26, hDlg, 0);
	btnLow_RR = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWRESPALM_LOW_RR, 10, 30+30*3, 125, 26, hDlg, 0);
	btnOk = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWRESPALM_OK, 10, 30+30*4, 125, 26, hDlg, 0);

	cobALM = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_NEWRESPALM_ALM_COB, 140, 30+30*0, 95, 26, hDlg, 0);
	cobALMPrint = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_NEWRESPALM_ALMPRINT_COB, 140, 30+30*1, 95, 26, hDlg, 0);
	editHigh_RR  = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_NEWRESPALM_HIGH_RR_EDIT, 140, 30+30*2, 95, 23, hDlg, 0);
	editLow_RR = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_NEWRESPALM_LOW_RR_EDIT, 140, 30+30*3, 95, 23, hDlg, 0);
	
	return 0;
}
static int InitCtlName(HWND hDlg)
{
	char strMenu[100];

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_RESP_ALM, "caption", strMenu, sizeof strMenu,"Resp  Alarm");
	SetWindowText(editCaption, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "previous", strMenu, sizeof strMenu,"Previous");	
	SetWindowText(btnOk, strMenu);

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "alarm_class", strMenu, sizeof strMenu,"ALM Class");	
	SetWindowText(btnALM, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "alarm_print", strMenu, sizeof strMenu,"ALM Print");	
	SetWindowText(btnALMPrint, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_RESP_ALM, "high_rr", strMenu, sizeof strMenu,"RR High");	
	SetWindowText(btnHigh_RR, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_RESP_ALM, "low_rr", strMenu, sizeof strMenu,"RR Low");	
	SetWindowText(btnLow_RR, strMenu);
	return 0;
}

//初始化combobox
static int InitCobList(HWND hDlg)
{
	int i;
	char strMenu[100];
	
	//almclass
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "off", strMenu, sizeof strMenu,"OFF");	
	SendMessage(cobALM, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "low", strMenu, sizeof strMenu,"Low");	
	SendMessage(cobALM, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "middle", strMenu, sizeof strMenu,"Middle");	
	SendMessage(cobALM, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "high", strMenu, sizeof strMenu,"High");	
	SendMessage(cobALM, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobALM, CB_SETCURSEL, gCfgResp.bAlmControl, 0);
	//almprint	
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes,STR_SETTING_GENERAL, "off", strMenu, sizeof strMenu,"OFF");	
	SendMessage(cobALMPrint, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "on", strMenu, sizeof strMenu,"ON");	
	SendMessage(cobALMPrint, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobALMPrint, CB_SETCURSEL, gCfgResp.bPrnControl, 0);
	
	return 0;
}

static int InitCtlCallBackProc(HWND hDlg)
{
	OldBtnProc = SetWindowCallbackProc(btnALM, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnALMPrint, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnHigh_RR, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnLow_RR, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);
	
	OldCobProc = SetWindowCallbackProc(cobALM, CobProc);
	OldCobProc = SetWindowCallbackProc(cobALMPrint, CobProc);
	
	OldEditProc = SetWindowCallbackProc(editHigh_RR, EditProc);
	OldEditProc = SetWindowCallbackProc(editLow_RR, EditProc);
	
	OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp);
	
	return 0;
}

static int InitCtlStatus(HWND hDlg)
{
	
	//如果记录仪故障，则该选项不可用
	if(gPrinterStatus == PRINT_STATUS_NOPRN||gCfgAlarm.bPrint==0){
			EnableWindow(btnALMPrint, FALSE);
			EnableWindow(cobALMPrint, FALSE);
	}
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
			//获得限值
			iTmpHigh_RR = gCfgResp.wHigh_RR;
			iTmpLow_RR = gCfgResp.wLow_RR;
		
			//创建控件
			CreateCtlProc(hDlg);
			//设置控件字符
			InitCtlName(hDlg);
			//初始化combobox
			InitCobList(hDlg);
			//设置控件回调函数
			InitCtlCallBackProc(hDlg);
			//设置控件状态
			InitCtlStatus(hDlg);

			SetLimitText(editHigh_RR, iTmpHigh_RR);
			SetLimitText(editLow_RR, iTmpLow_RR);

			gbKeyType = KEY_TAB;
			SetFocus(btnOk);
		}break;
		case MSG_PAINT:{
			

		}break;
		case MSG_COMMAND:{
			int	id  = LOWORD(wParam);
			switch(id){
				case DID_NEWRESPALM_ALM:{
					gbKeyType = KEY_UD;
					SetFocus(cobALM);
					}break;
				case DID_NEWRESPALM_ALMPRINT:{
					gbKeyType = KEY_UD;
					SetFocus(cobALMPrint);
					}break;
				case DID_NEWRESPALM_HIGH_RR:{
					gbKeyType = KEY_UD;
					SetFocus(editHigh_RR);
					}break;
				case DID_NEWRESPALM_LOW_RR:{
					gbKeyType = KEY_UD;
					SetFocus(editLow_RR);
					}break;
				case DID_NEWRESPALM_OK:{
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
void CreateDlgNewRESPALM(HWND hWnd)
{
 	DlgSet.controls = CtrlSet;
 	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}


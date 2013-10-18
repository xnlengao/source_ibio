/*************************************************************************** 
 *	Module Name:	DlgSet_PRAlarm
 *
 *	Abstract:	设置PR配置信息的对话框
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2013-1-22 13:30:45
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"

//DID_NEWPRALM
#define DID_NEWPRALM_HELP				DID_NEWPRALM
#define DID_NEWPRALM_OK 				DID_NEWPRALM+1
#define DID_NEWPRALM_CAPTION 				DID_NEWPRALM+2
#define DID_NEWPRALM_ALMCLASS			DID_NEWPRALM+3
#define DID_NEWPRALM_ALMPRINT			DID_NEWPRALM+4
#define DID_NEWPRALM_HIGH_PR			DID_NEWPRALM+5
#define DID_NEWPRALM_LOW_PR			DID_NEWPRALM+6
#define DID_NEWPRALM_ALMCLASS_COB		DID_NEWPRALM+7
#define DID_NEWPRALM_ALMPRINT_COB		DID_NEWPRALM+8
#define DID_NEWPRALM_HIGH_PR_EDIT		DID_NEWPRALM+9
#define DID_NEWPRALM_LOW_PR_EDIT		DID_NEWPRALM+10

#define DID_NEWPRALM_ALMSOURCE		DID_NEWPRALM+11
#define DID_NEWPRALM_SET_ALMSOURCE	DID_NEWPRALM+12
#define DID_NEWPRALM_VOLUME			DID_NEWPRALM+13
#define DID_NEWPRALM_SET_VOLUME		DID_NEWPRALM+14
//帮助对话框
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;
//其他控件
static HWND btnOk;

static HWND btnSetAlmSource;
static HWND cobAlmSource;
static HWND btnSetVolume;
static HWND cobVolume;

static HWND btnAlmClass;
static HWND btnAlmPrint;
static HWND btnHigh_PR;
static HWND btnLow_PR;
static HWND cobAlmClass;
static HWND cobAlmPrint;
static HWND editHigh_PR;
static HWND editLow_PR;
		
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
		//"mledit",
		//WS_CHILD | WS_VISIBLE | ES_AUTOWRAP | WS_BORDER ,
		"static",
		WS_CHILD | WS_VISIBLE | SS_LEFT ,
		5,DLG_HIGH-50, DLG_LENGHT-15, 40,
		DID_NEWPRALM_HELP,
		"",
		0
	}
};

//临时存储上下限，调节的时候使用
static int iTmpHigh =0, iTmpLow = 0;

//上下限刻度调整的增量
#define PR_LIMIT_INTER		1	//per

//限值
#define PR_LIMIT_MAX		PR_MAX
#define PR_LIMIT_MIN		PR_MIN

//上下限值最小幅度
#define PR_LIMITRANGE_MIN 		1

/*
	设置上下限文字
*/
static int SetLimitText(HWND hWnd, int iLimit)
{
	unsigned char strLimit[30] = {0};
	unsigned char strUnit[10] = {0};
	
	memset(strUnit, 0, sizeof strUnit);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "unit_bpm", strUnit, sizeof strUnit);		
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
				case DID_NEWPRALM_ALMCLASS_COB:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "help_alm_class", strHelp, sizeof strHelp,"Set up alarm ON/OFF and class.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWPRALM_ALMPRINT_COB:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "help_alm_print", strHelp, sizeof strHelp,"Set up alarm print.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWPRALM_HIGH_PR_EDIT:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SPO2PRALM, "help_pr_high", strHelp, sizeof strHelp,"Set up PR alarm high limit.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWPRALM_LOW_PR_EDIT:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SPO2PRALM, "help_pr_low", strHelp, sizeof strHelp,"Set up PR alarm low limit.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWPRALM_OK:{
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
						case DID_NEWPRALM_ALMSOURCE:{
							gCfgPulse.bAlmSource = index;
							gCfgEcg.bAlmSource = index;
							SetIntValueToResFile(gFileSetup, "ECGSetup", "alarm_source",  gCfgPulse.bAlmSource, 1);
							SetFocus(btnSetAlmSource);
							IsAlm_PR(&gValuePulse, &gCfgPulse, TRUE);
						}break;
						case DID_NEWPRALM_ALMCLASS:{
							gCfgPulse.bAlmControl = index;
							SetIntValueToResFile(gFileSetup, "PULSESetup", "alarm_control",  gCfgPulse.bAlmControl, 1);
							SetFocus(btnAlmClass);
							IsAlm_PR(&gValuePulse, &gCfgPulse, TRUE);
						}break;
						case DID_NEWPRALM_ALMPRINT:{
							gCfgPulse.bPrnControl = index;
							SetIntValueToResFile(gFileSetup, "PULSESetup", "alarm_print",  gCfgPulse.bPrnControl, 1);
							SetFocus(btnAlmPrint);
						}break;
						case DID_NEWPRALM_VOLUME:{
							gCfgPulse.bBeepVolume = index;
							SetIntValueToResFile(gFileSetup, "PULSESetup", "beep_volume",  gCfgPulse.bBeepVolume, 2);
							gCfgEcg.bBeepVolume = gCfgPulse.bBeepVolume;
							SetIntValueToResFile(gFileSetup, "ECGSetup", "beep_volume",  gCfgEcg.bBeepVolume, 2);
							SetFocus(btnSetVolume);
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
						case DID_NEWPRALM_HIGH_PR:{
							if(iTmpHigh < PR_LIMIT_MAX){
								iTmpHigh += PR_LIMIT_INTER;
								SetLimitText(hWnd, iTmpHigh);			
							}
						}break;
						case DID_NEWPRALM_LOW_PR:{
							if(iTmpLow < (iTmpHigh - PR_LIMITRANGE_MIN)){
								iTmpLow += PR_LIMIT_INTER;
								SetLimitText(hWnd, iTmpLow);			
							}
						}break;
					}
				}break;
				case SCANCODE_CURSORBLOCKUP:{
					switch(id){
						case DID_NEWPRALM_HIGH_PR:{
							if(iTmpHigh > (iTmpLow + PR_LIMITRANGE_MIN)){
								iTmpHigh -= PR_LIMIT_INTER;
								SetLimitText(hWnd, iTmpHigh);			
							}
						}break;
						case DID_NEWPRALM_LOW_PR:{
							if(iTmpLow > PR_LIMIT_MIN){
								iTmpLow -= PR_LIMIT_INTER;
								SetLimitText(hWnd, iTmpLow);			
							}
						}break;
					}
				}break;
				case SCANCODE_ENTER:{
					switch(id){
						case DID_NEWPRALM_HIGH_PR:{
							gCfgPulse.wHigh_PR = iTmpHigh;
							SetIntValueToResFile(gFileSetup, "PULSESetup", "pr_high",  gCfgPulse.wHigh_PR, 3);
							gCfgEcg.wHigh_HR = iTmpHigh;
							SetIntValueToResFile(gFileSetup, "ECGSetup", "hr_high",  gCfgEcg.wHigh_HR, 3);
							SetFocus(btnHigh_PR);
						}break;
						case DID_NEWPRALM_LOW_PR:{
							gCfgPulse.wLow_PR = iTmpLow;
							SetIntValueToResFile(gFileSetup, "PULSESetup", "pr_low",  gCfgPulse.wLow_PR, 3);
							gCfgEcg.wLow_HR = iTmpLow;
							SetIntValueToResFile(gFileSetup, "ECGSetup", "hr_low",  gCfgEcg.wLow_HR, 3);
							SetFocus(btnLow_PR);
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

//创建控件
static int CreateCtlProc(HWND hDlg)
{
	editHelp = GetDlgItem(hDlg, DID_NEWPRALM_HELP);
	editCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE  ,
					    DID_NEWPRALM_CAPTION, 0, 5, 250, 25, hDlg, 0);
	btnAlmClass = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_NEWPRALM_ALMCLASS_COB, 10, 30+30*0, 115, 26, hDlg, 0);
	btnAlmPrint = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_NEWPRALM_ALMPRINT_COB, 10, 30+30*1, 115, 26, hDlg, 0);
	btnHigh_PR = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_NEWPRALM_HIGH_PR_EDIT, 10, 30+30*2, 115, 26, hDlg, 0);
	btnLow_PR = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_NEWPRALM_LOW_PR_EDIT, 10, 30+30*3, 115, 26, hDlg, 0);
	btnOk= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					    DID_NEWPRALM_OK, 10, 30+30*4, 115, 26, hDlg, 0);
			
	cobAlmClass = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_NEWPRALM_ALMCLASS, 130, 30+30*0, 95, 26, hDlg, 0);
	cobAlmPrint = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_NEWPRALM_ALMPRINT, 130, 30+30*1, 95, 26, hDlg, 0);
	editHigh_PR = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_NEWPRALM_HIGH_PR, 130, 30+30*2, 95, 23, hDlg, 0);
	editLow_PR = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_NEWPRALM_LOW_PR, 130, 30+30*3, 95, 23, hDlg, 0);
			
	return 0;
}
//设置控件字符
static int InitCtlName(HWND hDlg)
{
	char strMenu[100];
	//caption
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SPO2PRALM, "caption", strMenu, sizeof strMenu,"PR Alarm");	
	SetWindowText(editCaption,strMenu);
	
	//Alarm Class
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "alarm_class", strMenu, sizeof strMenu,"Alm Class");	
	SetWindowText(btnAlmClass,strMenu);
	//Alarm Print
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "alarm_print", strMenu, sizeof strMenu,"Alm Print");	
	SetWindowText(btnAlmPrint,strMenu);
	//High Limit Of PR
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SPO2PRALM, "high_pr", strMenu, sizeof strMenu,"PR High");	
	SetWindowText(btnHigh_PR,strMenu);
	//Low Limit Of PR
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SPO2PRALM, "low_pr", strMenu, sizeof strMenu,"PR Low");	
	SetWindowText(btnLow_PR,strMenu);
	//Exit
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "previous", strMenu, sizeof strMenu,"Preious");	
	SetWindowText(btnOk,strMenu);
	return 0;
}
//初始化combobox
static int InitCobList(HWND hDlg)
{
	char strMenu[100];
	//alm class
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "off", strMenu, sizeof strMenu);	
	SendMessage(cobAlmClass, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "low", strMenu, sizeof strMenu);	
	SendMessage(cobAlmClass, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "middle", strMenu, sizeof strMenu);	
	SendMessage(cobAlmClass, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes,STR_SETTING_GENERAL, "high", strMenu, sizeof strMenu);	
	SendMessage(cobAlmClass, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobAlmClass, CB_SETCURSEL, gCfgPulse.bAlmControl, 0);
	//alm print
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "off", strMenu, sizeof strMenu);	
	SendMessage(cobAlmPrint, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "on", strMenu, sizeof strMenu);	
	SendMessage(cobAlmPrint, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobAlmPrint, CB_SETCURSEL, gCfgPulse.bPrnControl, 0);
		
	return 0;
}
//初始化Edit
static int	InitEditText(HWND hDlg)
{
	
	//获得限值
	iTmpHigh = gCfgPulse.wHigh_PR;
	iTmpLow = gCfgPulse.wLow_PR;
	SetLimitText(editHigh_PR, gCfgPulse.wHigh_PR);
	SetLimitText(editLow_PR, gCfgPulse.wLow_PR);
	

}
//设置控件回调函数
static int InitCtlCallBackProc(HWND hDlg)
{
			
	OldBtnProc = SetWindowCallbackProc(btnAlmClass, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnAlmPrint, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnHigh_PR, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnLow_PR, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);
		
	OldCobProc = SetWindowCallbackProc(cobAlmClass, CobProc);
	OldCobProc = SetWindowCallbackProc(cobAlmPrint, CobProc);
	OldEditProc = SetWindowCallbackProc(editHigh_PR, EditProc);
	OldEditProc = SetWindowCallbackProc(editLow_PR, EditProc);
	OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp); 
			
			
	return 0;
}
static int InitCtlStatus(HWND hDlg)
{
	//如果记录仪故障，则该选项不可用
	if(gPrinterStatus == PRNSTATUS_ERR){
			EnableWindow(btnAlmPrint, FALSE);
			EnableWindow(cobAlmPrint, FALSE);
	}
	return 0;
}
/*
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
			//初始化edit
			InitEditText(hDlg);
			//设置控件回调函数
			InitCtlCallBackProc(hDlg);
			InitCtlStatus(hDlg);
			gbKeyType = KEY_TAB;
			SetFocus(btnOk);
		}break;
		case MSG_PAINT:{

		}break;
		case MSG_COMMAND:{
			int	id   = LOWORD(wParam);
			
			switch(id){
				case DID_NEWPRALM_SET_ALMSOURCE:{
					gbKeyType = KEY_UD;
					SetFocus(cobAlmSource);
				}break;
				case DID_NEWPRALM_ALMCLASS_COB:{
					gbKeyType = KEY_UD;
					SetFocus(cobAlmClass);
				}break;
				case DID_NEWPRALM_ALMPRINT_COB:{
					gbKeyType = KEY_UD;
					SetFocus(cobAlmPrint);
				}break;
				case DID_NEWPRALM_HIGH_PR_EDIT:{
					gbKeyType = KEY_UD;
					SetFocus(editHigh_PR);
				}break;
				case DID_NEWPRALM_LOW_PR_EDIT:{
					gbKeyType = KEY_UD;
					SetFocus(editLow_PR);
				}break;
				case DID_NEWPRALM_SET_VOLUME:{
					gbKeyType = KEY_UD;
					SetFocus(cobVolume);
				}break;
				case DID_NEWPRALM_OK:{
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
			
			//向中央机发送信息
			NetSend_SpO2Info();

			EndDialog(hDlg, wParam);	
		}break;
	}
	
	temp = DefaultDialogProc(hDlg, message, wParam, lParam);
	return(temp);
}

/*
	建立对话框
*/
void CreateDlgNewPRALM(HWND hWnd)
{
	DlgSet.controls = CtrlSet;
	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}


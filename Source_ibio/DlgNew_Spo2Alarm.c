/*************************************************************************** 
 *	Module Name:	DlgNew_Spo2Alarm
 *
 *	Abstract:	配置信息对话框
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2013-1-22 11:45:53
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"

//DID_NEWSPO2ALM
#define DID_NEWSPO2ALM_HELP 				DID_NEWSPO2ALM
#define DID_NEWSPO2ALM_OK					DID_NEWSPO2ALM+1
#define DID_NEWSPO2ALM_CAPTION				DID_NEWSPO2ALM+2
#define DID_NEWSPO2ALM_ALMCLASS			DID_NEWSPO2ALM+3
#define DID_NEWSPO2ALM_ALMPRINT			DID_NEWSPO2ALM+4
#define DID_NEWSPO2ALM_HIGH				DID_NEWSPO2ALM+5
#define DID_NEWSPO2ALM_LOW				DID_NEWSPO2ALM+6
#define DID_NEWSPO2ALM_LOWEST			DID_NEWSPO2ALM+7
#define DID_NEWSPO2ALM_ALMCLASS_COB		DID_NEWSPO2ALM+8
#define DID_NEWSPO2ALM_ALMPRINT_COB		DID_NEWSPO2ALM+9
#define DID_NEWSPO2ALM_HIGH_COB			DID_NEWSPO2ALM+10
#define DID_NEWSPO2ALM_LOW_COB			DID_NEWSPO2ALM+11
#define DID_NEWSPO2ALM_LOWEST_COB		DID_NEWSPO2ALM+12

//帮助对话框
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;
//其他控件
static HWND btnOk;

static HWND btnAlmClass;
static HWND btnAlmPrint;
static HWND btnHigh;
static HWND btnLow;
static HWND btnLowest;

static HWND cobAlmClass;
static HWND cobAlmPrint;

static HWND editHigh;
static HWND editLow;
static HWND editLowest;

//控件回调函数
static WNDPROC OldBtnProc;	
static WNDPROC OldCobProc;
static WNDPROC OldEditProc;

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
		5,DLG_HIGH-50, DLG_LENGHT-15, 40,
		DID_NEWSPO2ALM_HELP,
		"",
		0
	}
};


//临时存储上下限，调节的时候使用
static int iTmpHigh =0, iTmpLow = 0, iTmpLowest = 0;

//上下限刻度调整的增量
#define SPO2_LIMIT_INTER		1	//per

//限值
#define SPO2_LIMIT_MAX		SPO2_MAX
#define SPO2_LIMIT_MIN		SPO2_MIN

//上下限值最小幅度
#define SPO2_LIMITRANGE_MIN 		1


/*
	设置上下限文字
*/
static int SetLimitText(HWND hWnd, int iLimit)
{
	unsigned char strLimit[50] = {0};
	unsigned char strUnit[20] = {0};
	
	memset(strUnit, 0, sizeof strUnit);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "unit_per", strUnit, sizeof strUnit);		
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
				case DID_NEWSPO2ALM_OK:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "help_previous", strHelp, sizeof strHelp,"Return to the previous menu.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWSPO2ALM_ALMCLASS_COB:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "help_alm_class", strHelp, sizeof strHelp,"Set up alarm ON/OFF and class.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWSPO2ALM_ALMPRINT_COB:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "help_alm_print", strHelp, sizeof strHelp,"Set up alarm print.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWSPO2ALM_HIGH_COB:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SPO2ALM, "help_high_spo2", strHelp, sizeof strHelp,"Set up SpO2 alarm high limit.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWSPO2ALM_LOW_COB:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SPO2ALM, "help_low_spo2", strHelp, sizeof strHelp,"Set up SpO2 alarm low limit.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWSPO2ALM_LOWEST_COB:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SPO2ALM, "help_lowest_spo2", strHelp, sizeof strHelp,"Set up lowest SpO2 limit. System will alarm when surpasses this limit.");	
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
						case DID_NEWSPO2ALM_ALMCLASS:{
							gCfgSpO2.bAlmControl = index;
							SetIntValueToResFile(gFileSetup, "SPO2Setup", "alarm_control",  gCfgSpO2.bAlmControl, 1);
							SetFocus(btnAlmClass);
							IsAlm_SpO2(&gValueSpO2, &gCfgSpO2, TRUE);
						}break;
						case DID_NEWSPO2ALM_ALMPRINT:{
							gCfgSpO2.bPrnControl = index;
							SetIntValueToResFile(gFileSetup, "SPO2Setup", "alarm_print",  gCfgSpO2.bPrnControl, 1);
							SetFocus(btnAlmPrint);
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
						case DID_NEWSPO2ALM_HIGH:{
							if(iTmpHigh < SPO2_LIMIT_MAX){
								iTmpHigh += SPO2_LIMIT_INTER;
								SetLimitText(hWnd, iTmpHigh);			
							}
						}break;
						case DID_NEWSPO2ALM_LOW:{
							if(iTmpLow < (iTmpHigh - SPO2_LIMITRANGE_MIN)){
								iTmpLow += SPO2_LIMIT_INTER;
								SetLimitText(hWnd, iTmpLow);			
							}
						}break;
						case DID_NEWSPO2ALM_LOWEST:{
							if(iTmpLowest  < iTmpLow){
								iTmpLowest += SPO2_LIMIT_INTER;
								SetLimitText(hWnd, iTmpLowest);			
							}
						}break;
					}
				}break;
				case SCANCODE_CURSORBLOCKUP:{
					switch(id){
						case DID_NEWSPO2ALM_HIGH:{
							if(iTmpHigh > (iTmpLow + SPO2_LIMITRANGE_MIN)){
								iTmpHigh -= SPO2_LIMIT_INTER;
								SetLimitText(hWnd, iTmpHigh);			
							}
						}break;
						case DID_NEWSPO2ALM_LOW:{
							if(iTmpLow > iTmpLowest){
								iTmpLow -= SPO2_LIMIT_INTER;
								SetLimitText(hWnd, iTmpLow);			
							}
						}break;
						case DID_NEWSPO2ALM_LOWEST:{
							if(iTmpLowest  > SPO2_LIMIT_MIN){
								iTmpLowest -= SPO2_LIMIT_INTER;
								SetLimitText(hWnd, iTmpLowest);			
							}
						}break;
					}
				}break;
				case SCANCODE_ENTER:{
					switch(id){
						case DID_NEWSPO2ALM_HIGH:{
							gCfgSpO2.bHigh_SpO2 = iTmpHigh;
							SetIntValueToResFile(gFileSetup, "SPO2Setup", "spo2_high",  gCfgSpO2.bHigh_SpO2, 3);
							SetFocus(btnHigh);
						}break;
						case DID_NEWSPO2ALM_LOW:{
							gCfgSpO2.bLow_SpO2 = iTmpLow;
							SetIntValueToResFile(gFileSetup, "SPO2Setup", "spo2_low",  gCfgSpO2.bLow_SpO2, 3);
							SetFocus(btnLow);
						}break;
						case DID_NEWSPO2ALM_LOWEST:{
							gCfgSpO2.bLowest_SpO2 = iTmpLowest;
							SetIntValueToResFile(gFileSetup, "SPO2Setup", "spo2_lowest",  gCfgSpO2.bLowest_SpO2,3);
							SetFocus(btnLowest);
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
	editHelp = GetDlgItem(hDlg, DID_NEWSPO2ALM_HELP);
	editCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE  ,
					     DID_NEWSPO2ALM_CAPTION, 0, 5, 250, 25, hDlg, 0);
	btnAlmClass = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
				DID_NEWSPO2ALM_ALMCLASS_COB, 10, 30+30*0, 115, 26, hDlg, 0);
	btnAlmPrint = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
				DID_NEWSPO2ALM_ALMPRINT_COB, 10, 30+30*1, 115, 26, hDlg, 0);
	btnHigh = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
				DID_NEWSPO2ALM_HIGH_COB, 10, 30+30*2, 115, 26, hDlg, 0);
	btnLow= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
				DID_NEWSPO2ALM_LOW_COB, 10, 30+30*3, 115, 26, hDlg, 0);
	btnLowest = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
				DID_NEWSPO2ALM_LOWEST_COB, 10, 30+30*4, 115, 26, hDlg, 0);
	btnOk= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					    DID_NEWSPO2ALM_OK, 10, 30+30*5, 115, 26, hDlg, 0);
			

	cobAlmClass = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
				DID_NEWSPO2ALM_ALMCLASS, 130, 30+30*0, 95, 26, hDlg, 0);
	cobAlmPrint = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
				DID_NEWSPO2ALM_ALMPRINT, 130, 30+30*1, 95, 26, hDlg, 0);
	editHigh = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
				DID_NEWSPO2ALM_HIGH, 130, 30+30*2, 95, 23, hDlg, 0);
	editLow = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
				DID_NEWSPO2ALM_LOW, 130, 30+30*3, 95, 23, hDlg, 0);
	editLowest = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
				DID_NEWSPO2ALM_LOWEST, 130, 30+30*4, 95, 23, hDlg, 0);
			
	return 0;
}
//设置控件字符
static int InitCtlName(HWND hDlg)
{
	char strMenu[100];
	//caption
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SPO2ALM, "caption", strMenu, sizeof strMenu,"SPO2 Alarm");	
	SetWindowText(editCaption,strMenu);

	//Alarm Class
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "alarm_class", strMenu, sizeof strMenu,"Alm");	
	SetWindowText(btnAlmClass,strMenu);
	//Alarm print
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "alarm_print", strMenu, sizeof strMenu,"Alm Print");	
	SetWindowText(btnAlmPrint,strMenu);

	//High Limit Of SpO2
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SPO2ALM, "high_spo2", strMenu, sizeof strMenu,"SPO2 High");	
	SetWindowText(btnHigh,strMenu);		
	//Low Limit Of SpO2
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SPO2ALM, "low_spo2", strMenu, sizeof strMenu,"SPO2 Low");	
	SetWindowText(btnLow,strMenu);
	//Lowest Limit Of SpO2
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SPO2ALM, "lowest_spo2", strMenu, sizeof strMenu,"SPO2 Lowest");	
	SetWindowText(btnLowest,strMenu);
	//Exit
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "previous", strMenu, sizeof strMenu,"Previous");	
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
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "high", strMenu, sizeof strMenu);	
	SendMessage(cobAlmClass, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobAlmClass, CB_SETCURSEL, gCfgSpO2.bAlmControl, 0);
	//alm print
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "off", strMenu, sizeof strMenu);	
	SendMessage(cobAlmPrint, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "on", strMenu, sizeof strMenu);	
	SendMessage(cobAlmPrint, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobAlmPrint, CB_SETCURSEL, gCfgSpO2.bPrnControl, 0);
		
			
	return 0;
}
//初始化Edit
static int	InitEditText(HWND hDlg)
{
	//获得报警限值
	iTmpHigh = gCfgSpO2.bHigh_SpO2;
	iTmpLow = gCfgSpO2.bLow_SpO2;
	iTmpLowest = gCfgSpO2.bLowest_SpO2;

	SetLimitText(editHigh, gCfgSpO2.bHigh_SpO2);
	SetLimitText(editLow, gCfgSpO2.bLow_SpO2);
	SetLimitText(editLowest, gCfgSpO2.bLowest_SpO2);
	

}
//设置控件回调函数
static int InitCtlCallBackProc(HWND hDlg)
{

	OldBtnProc = SetWindowCallbackProc(btnAlmClass, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnAlmPrint, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnHigh, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnLow, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnLowest, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);
		
	OldCobProc = SetWindowCallbackProc(cobAlmClass, CobProc);
	OldCobProc = SetWindowCallbackProc(cobAlmPrint, CobProc);
	OldEditProc = SetWindowCallbackProc(editHigh, EditProc);
	OldEditProc = SetWindowCallbackProc(editLow, EditProc);
	OldEditProc = SetWindowCallbackProc(editLowest, EditProc);
	OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp); 
			
			
	return 0;
}
static int InitCtlStatus(HWND hDlg)
{
	//如果记录仪故障，则该选项不可用
	if(gPrinterStatus == PRINT_STATUS_NOPRN||gCfgAlarm.bPrint==0){
		EnableWindow(btnAlmPrint, FALSE);
			EnableWindow(cobAlmPrint, FALSE);
	}
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
			InitCtlStatus(hDlg);
			gbKeyType = KEY_TAB;			
			SetFocus(btnOk);
		}break;
		case MSG_PAINT:{

		}break;
		case MSG_COMMAND:{
			int	id   = LOWORD(wParam);
			
			switch(id){
				case DID_NEWSPO2ALM_ALMCLASS_COB:{
					gbKeyType = KEY_UD;
					SetFocus(cobAlmClass);
				}break;
				case DID_NEWSPO2ALM_ALMPRINT_COB:{
					gbKeyType = KEY_UD;
					SetFocus(cobAlmPrint);
				}break;
				case DID_NEWSPO2ALM_HIGH_COB:{
					gbKeyType = KEY_UD;
					SetFocus(editHigh);
				}break;
				case DID_NEWSPO2ALM_LOW_COB:{
					gbKeyType = KEY_UD;
					SetFocus(editLow);
				}break;
				case DID_NEWSPO2ALM_LOWEST_COB:{
					gbKeyType = KEY_UD;
					SetFocus(editLowest);
				}break;
				case DID_NEWSPO2ALM_OK:{
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
void CreateDlgNewSpo2ALM(HWND hWnd)
{
	DlgSet.controls = CtrlSet;
	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}

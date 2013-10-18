/*************************************************************************** 
 *	Module Name:	DlgNew_NibpAlarm.c
 *
 *	Abstract:	设置NIBP配置信息的对话框
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2013-1-23 16:37:28
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"
#include "General_Funcs.h"

//DID_NEWNIBPALM
#define DID_NEWNIBPALM_HELP				DID_NEWNIBPALM
#define DID_NEWNIBPALM_OK 				DID_NEWNIBPALM+1
#define DID_NEWNIBPALM_CAPTION 			DID_NEWNIBPALM+2
#define DID_NEWNIBPALM_ALMSOURCE		DID_NEWNIBPALM+3
#define DID_NEWNIBPALM_ALMCLASS			DID_NEWNIBPALM+4
#define DID_NEWNIBPALM_ALMPRINT			DID_NEWNIBPALM+5
#define DID_NEWNIBPALM_HIGH_SYS			DID_NEWNIBPALM+6
#define DID_NEWNIBPALM_LOW_SYS			DID_NEWNIBPALM+7
#define DID_NEWNIBPALM_HIGH_MEAN			DID_NEWNIBPALM+8
#define DID_NEWNIBPALM_LOW_MEAN			DID_NEWNIBPALM+9
#define DID_NEWNIBPALM_HIGH_DIA			DID_NEWNIBPALM+10
#define DID_NEWNIBPALM_LOW_DIA			DID_NEWNIBPALM+11
#define DID_NEWNIBPALM_ALMSOURCE_COB	DID_NEWNIBPALM+12
#define DID_NEWNIBPALM_ALMCLASS_COB		DID_NEWNIBPALM+13
#define DID_NEWNIBPALM_ALMPRINT_COB		DID_NEWNIBPALM+14
#define DID_NEWNIBPALM_HIGH_SYS_EDIT		DID_NEWNIBPALM+15
#define DID_NEWNIBPALM_LOW_SYS_EDIT		DID_NEWNIBPALM+16
#define DID_NEWNIBPALM_HIGH_MEAN_EDIT	DID_NEWNIBPALM+17
#define DID_NEWNIBPALM_LOW_MEAN_EDIT	DID_NEWNIBPALM+18
#define DID_NEWNIBPALM_HIGH_DIA_EDIT		DID_NEWNIBPALM+19
#define DID_NEWNIBPALM_LOW_DIA_EDIT		DID_NEWNIBPALM+20

//帮助对话框
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;
//其他控件
static HWND btnOk;
static HWND btnAlmSource;
static HWND btnAlmClass;
static HWND btnAlmPrint;
static HWND btnHigh_Sys;
static HWND btnLow_Sys;
static HWND btnHigh_Dia;
static HWND btnLow_Dia;
static HWND btnHigh_Mean;
static HWND btnLow_Mean;
static HWND cobAlmSource;
static HWND cobAlmClass;
static HWND cobAlmPrint;
static HWND editHigh_Sys;
static HWND editLow_Sys;
static HWND editHigh_Dia;
static HWND editLow_Dia;
static HWND editHigh_Mean;
static HWND editLow_Mean;

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
		DID_NEWNIBPALM_HELP,
		"",
		0
	}
};


//临时存储上下限，调节的时候使用
static int iTmpHigh_Sys =0, iTmpLow_Sys =0;
static int iTmpHigh_Dia =0, iTmpLow_Dia =0;
static int iTmpHigh_Mean =0, iTmpLow_Mean =0;

//上下限刻度调整的增量mmHg
#define NIBP_LIMIT_INTER	1

//限值 mmHg
#define NIBP_LIMIT_MAX		NIBP_MAX
#define NIBP_LIMIT_MIN		NIBP_MIN

//上下限值最小幅度
#define NIBP_LIMITRANGE_MIN 	1


/*
	根据单位设置上下限
*/
static int SetLimitText(BOOL bUnit, HWND hWnd, int iLimit)
{
	unsigned char strLimit[50] = {0};
	unsigned char strUnit[20] = {0};
	
	switch(bUnit){
		case NIBP_UNIT_KPA:{
			memset(strUnit, 0, sizeof strUnit);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "unit_kpa", strUnit, sizeof strUnit,"kpa");		
			memset(strLimit, 0, sizeof strLimit);
			snprintf(strLimit, sizeof strLimit, "%3.1f %s", (float)(iLimit / 7.5), strUnit);
			SetWindowText(hWnd, strLimit);
		}break;
		default:{
			//NIBPUNIT_MMHG
			memset(strUnit, 0, sizeof strUnit);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "unit_mmhg", strUnit, sizeof strUnit,"mmHg");		
			memset(strLimit, 0, sizeof strLimit);
			snprintf(strLimit, sizeof strLimit, "%d %s", iLimit, strUnit);
			SetWindowText(hWnd, strLimit);
		}break;
	}
	
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
				case DID_NEWNIBPALM_ALMSOURCE_COB:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPALM, "help_alm_source", strHelp, sizeof strHelp,"Set up the alarm source of NIBP.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWNIBPALM_ALMCLASS_COB:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "help_alm_class", strHelp, sizeof strHelp,"Set up alarm class.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWNIBPALM_ALMPRINT_COB:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "help_alm_print", strHelp, sizeof strHelp,"Set up alarm print.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWNIBPALM_HIGH_SYS_EDIT:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPALM, "help_high_sys", strHelp, sizeof strHelp,"Set up SYS alarm high limit.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWNIBPALM_LOW_SYS_EDIT:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPALM, "help_low_sys", strHelp, sizeof strHelp,"Set up SYS alarm low limit.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWNIBPALM_HIGH_DIA_EDIT:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPALM, "help_high_dia", strHelp, sizeof strHelp,"Set up DIA alarm high limit.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWNIBPALM_LOW_DIA_EDIT:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPALM, "help_low_dia", strHelp, sizeof strHelp,"Set up DIA alarm low limit.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWNIBPALM_HIGH_MEAN_EDIT:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPALM, "help_high_mean", strHelp, sizeof strHelp,"Set up MEAN alarm high limit.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWNIBPALM_LOW_MEAN_EDIT:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPALM, "help_low_mean", strHelp, sizeof strHelp,"Set up MEAN alarm low limit.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWNIBPALM_OK:{
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
						case DID_NEWNIBPALM_ALMSOURCE:{
							gCfgNibp.bAlmSource = index;
							SetIntValueToResFile(gFileSetup, "NIBPSetup", "alarm_source",  gCfgNibp.bAlmSource, 1);
							SetFocus(btnAlmSource);
							//改变报警源
							IsAlm_NIBP(TRUE, TRUE);
						}break;
						case DID_NEWNIBPALM_ALMCLASS:{
							gCfgNibp.bAlmControl = index;
							SetIntValueToResFile(gFileSetup, "NIBPSetup", "alarm_control",  gCfgNibp.bAlmControl, 1);
							SetFocus(btnAlmClass);
							if(B_PRINTF)printf("gCfgNibp.bAlmControl=%d\n",gCfgNibp.bAlmControl);
							//改变报警级别
							IsAlm_NIBP(TRUE, TRUE);
						}break;
						case DID_NEWNIBPALM_ALMPRINT:{
							gCfgNibp.bPrnControl = index;
							SetIntValueToResFile(gFileSetup, "NIBPSetup", "alarm_print",  gCfgNibp.bPrnControl, 1);
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
						case DID_NEWNIBPALM_HIGH_SYS:{
							if(iTmpHigh_Sys <NIBP_LIMIT_MAX){
								iTmpHigh_Sys += NIBP_LIMIT_INTER;
								SetLimitText(gCfgNibp.bUnit, hWnd, iTmpHigh_Sys);			
							}
						}break;
						case DID_NEWNIBPALM_LOW_SYS:{
							if(iTmpLow_Sys < (iTmpHigh_Sys - NIBP_LIMITRANGE_MIN)){
								iTmpLow_Sys += NIBP_LIMIT_INTER;
								SetLimitText(gCfgNibp.bUnit, hWnd, iTmpLow_Sys);			
							}
						}break;
						case DID_NEWNIBPALM_HIGH_DIA:{
							if(iTmpHigh_Dia <NIBP_LIMIT_MAX){
								iTmpHigh_Dia += NIBP_LIMIT_INTER;
								SetLimitText(gCfgNibp.bUnit, hWnd, iTmpHigh_Dia);			
							}
						}break;
						case DID_NEWNIBPALM_LOW_DIA:{
							if(iTmpLow_Dia < (iTmpHigh_Dia - NIBP_LIMITRANGE_MIN)){
								iTmpLow_Dia += NIBP_LIMIT_INTER;
								SetLimitText(gCfgNibp.bUnit, hWnd, iTmpLow_Dia);			
							}
						}break;
						case DID_NEWNIBPALM_HIGH_MEAN:{
							if(iTmpHigh_Mean <NIBP_LIMIT_MAX){
								iTmpHigh_Mean += NIBP_LIMIT_INTER;
								SetLimitText(gCfgNibp.bUnit, hWnd, iTmpHigh_Mean);			
							}
						}break;
						case DID_NEWNIBPALM_LOW_MEAN:{
							if(iTmpLow_Mean < (iTmpHigh_Mean - NIBP_LIMITRANGE_MIN)){
								iTmpLow_Mean += NIBP_LIMIT_INTER;
								SetLimitText(gCfgNibp.bUnit, hWnd, iTmpLow_Mean);			
							}
						}break;
					}
				}break;
				case SCANCODE_CURSORBLOCKUP:{
					switch(id){
						case DID_NEWNIBPALM_HIGH_SYS:{
							if(iTmpHigh_Sys > (iTmpLow_Sys + NIBP_LIMITRANGE_MIN)){
								iTmpHigh_Sys -= NIBP_LIMIT_INTER;
								SetLimitText(gCfgNibp.bUnit, hWnd, iTmpHigh_Sys);			
							}
						}break;
						case DID_NEWNIBPALM_LOW_SYS:{
							if(iTmpLow_Sys >NIBP_LIMIT_MIN){
								iTmpLow_Sys -= NIBP_LIMIT_INTER;
								SetLimitText(gCfgNibp.bUnit, hWnd, iTmpLow_Sys);			
							}
						}break;
						case DID_NEWNIBPALM_HIGH_DIA:{
							if(iTmpHigh_Dia > (iTmpLow_Dia + NIBP_LIMITRANGE_MIN)){
								iTmpHigh_Dia -= NIBP_LIMIT_INTER;
								SetLimitText(gCfgNibp.bUnit, hWnd, iTmpHigh_Dia);			
							}
						}break;
						case DID_NEWNIBPALM_LOW_DIA:{
							if(iTmpLow_Dia>NIBP_LIMIT_MIN){
								iTmpLow_Dia -= NIBP_LIMIT_INTER;
								SetLimitText(gCfgNibp.bUnit, hWnd, iTmpLow_Dia);			
							}
						}break;
						case DID_NEWNIBPALM_HIGH_MEAN:{
							if(iTmpHigh_Mean > (iTmpLow_Mean + NIBP_LIMITRANGE_MIN)){
								iTmpHigh_Mean -= NIBP_LIMIT_INTER;
								SetLimitText(gCfgNibp.bUnit, hWnd, iTmpHigh_Mean);			
							}
						}break;
						case DID_NEWNIBPALM_LOW_MEAN:{
							if(iTmpLow_Mean>NIBP_LIMIT_MIN){
								iTmpLow_Mean -= NIBP_LIMIT_INTER;
								SetLimitText(gCfgNibp.bUnit, hWnd, iTmpLow_Mean);			
							}
						}break;
					}
				}break;
				case SCANCODE_ENTER:{
					switch(id){
						case DID_NEWNIBPALM_HIGH_SYS:{
							gCfgNibp.wHigh_Sys = iTmpHigh_Sys;
							SetIntValueToResFile(gFileSetup, "NIBPSetup", "sys_high",  gCfgNibp.wHigh_Sys, 4);
							SetFocus(btnHigh_Sys);
							IsAlm_NIBP(TRUE, TRUE);
						}break;
						case DID_NEWNIBPALM_LOW_SYS:{
							gCfgNibp.wLow_Sys = iTmpLow_Sys;
							SetIntValueToResFile(gFileSetup, "NIBPSetup", "sys_low",  gCfgNibp.wLow_Sys, 4);
							SetFocus(btnLow_Sys);
							IsAlm_NIBP(TRUE, TRUE);
						}break;
						case DID_NEWNIBPALM_HIGH_DIA:{
							gCfgNibp.wHigh_Dia = iTmpHigh_Dia;
							SetIntValueToResFile(gFileSetup, "NIBPSetup", "dia_high",  gCfgNibp.wHigh_Dia, 4);
							SetFocus(btnHigh_Dia);
							IsAlm_NIBP(TRUE, TRUE);
						}break;
						case DID_NEWNIBPALM_LOW_DIA:{
							gCfgNibp.wLow_Dia = iTmpLow_Dia;
							SetIntValueToResFile(gFileSetup, "NIBPSetup", "dia_low",  gCfgNibp.wLow_Dia, 4);
							SetFocus(btnLow_Dia);
							IsAlm_NIBP(TRUE, TRUE);
						}break;
						case DID_NEWNIBPALM_HIGH_MEAN:{
							gCfgNibp.wHigh_Mean = iTmpHigh_Mean;
							SetIntValueToResFile(gFileSetup, "NIBPSetup", "mean_high",  gCfgNibp.wHigh_Mean, 4);
							SetFocus(btnHigh_Mean);
							IsAlm_NIBP(TRUE, TRUE);
						}break;
						case DID_NEWNIBPALM_LOW_MEAN:{
							gCfgNibp.wLow_Mean = iTmpLow_Mean;
							SetIntValueToResFile(gFileSetup, "NIBPSetup", "mean_low",  gCfgNibp.wLow_Mean, 4);
							SetFocus(btnLow_Mean);
							IsAlm_NIBP(TRUE, TRUE);
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
	editHelp = GetDlgItem(hDlg, DID_NEWNIBPALM_HELP);
	editCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE  ,
					     DID_NEWNIBPALM_CAPTION, 0, 5, 250, 25, hDlg, 0);
	btnAlmSource = CreateWindow("button",  "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_NEWNIBPALM_ALMSOURCE_COB, 10, 30+29*0, 125, 26, hDlg, 0);
	btnAlmClass = CreateWindow("button",  "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_NEWNIBPALM_ALMCLASS_COB, 10, 30+29*1, 125, 26, hDlg, 0);
	btnAlmPrint = CreateWindow("button",  "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_NEWNIBPALM_ALMPRINT_COB, 10, 30+29*2, 125, 26, hDlg, 0);
	btnHigh_Sys = CreateWindow("button",  "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_NEWNIBPALM_HIGH_SYS_EDIT, 10, 30+29*3, 125, 26, hDlg, 0);
	btnLow_Sys = CreateWindow("button",  "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_NEWNIBPALM_LOW_SYS_EDIT, 10, 30+29*4, 125, 26, hDlg, 0);
	btnHigh_Mean = CreateWindow("button",  "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_NEWNIBPALM_HIGH_MEAN_EDIT, 10, 30+29*5, 125, 26, hDlg, 0);
	btnLow_Mean = CreateWindow("button",  "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_NEWNIBPALM_LOW_MEAN_EDIT, 10, 30+29*6, 125, 26, hDlg, 0);
	btnHigh_Dia = CreateWindow("button",  "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_NEWNIBPALM_HIGH_DIA_EDIT, 10, 30+29*7, 125, 26, hDlg, 0);
	btnLow_Dia = CreateWindow("button",  "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_NEWNIBPALM_LOW_DIA_EDIT, 10, 30+29*8, 125, 26, hDlg, 0);
	btnOk= CreateWindow("button",  "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
				    DID_NEWNIBPALM_OK, 10, 30+29*9, 125, 26, hDlg, 0);

	cobAlmSource = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					       DID_NEWNIBPALM_ALMSOURCE, 140, 30+29*0, 95, 26, hDlg, 0);
	cobAlmClass = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_NEWNIBPALM_ALMCLASS, 140, 30+29*1, 95, 26, hDlg, 0);
	cobAlmPrint = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_NEWNIBPALM_ALMPRINT, 140, 30+29*2, 95, 26, hDlg, 0);
	editHigh_Sys = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_NEWNIBPALM_HIGH_SYS, 140, 30+29*3, 95, 26, hDlg, 0);
	editLow_Sys = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_NEWNIBPALM_LOW_SYS, 140, 30+29*4, 95, 26, hDlg, 0);
	editHigh_Mean = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_NEWNIBPALM_HIGH_MEAN, 140, 30+29*5, 95, 26, hDlg, 0);
	editLow_Mean= CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_NEWNIBPALM_LOW_MEAN, 140, 30+29*6, 95, 26, hDlg, 0);
	editHigh_Dia = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_NEWNIBPALM_HIGH_DIA, 140, 30+29*7, 95, 26, hDlg, 0);
	editLow_Dia = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_NEWNIBPALM_LOW_DIA, 140, 30+29*8, 95, 26, hDlg, 0);
			
	return 0;
}
//设置控件字符
static int InitCtlName(HWND hDlg)
{
	char strMenu[100];
	//caption
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPALM, "caption", strMenu, sizeof strMenu,"NIBP Alarm");	
	SetWindowText(editCaption,strMenu);
	
	//Alarm Source 
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPALM, "alarm_source", strMenu, sizeof strMenu,"Alm Source");	
	SetWindowText(btnAlmSource,strMenu);
	//Alarm Class
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "alarm_class", strMenu, sizeof strMenu,"Alm Class");	
	SetWindowText(btnAlmClass,strMenu);
	//Alarm Print
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "alarm_print", strMenu, sizeof strMenu,"Alm Print");	
	SetWindowText(btnAlmPrint,strMenu);
	//High Limit Of Sys
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPALM, "high_sys", strMenu, sizeof strMenu,"Sys High");	
	SetWindowText(btnHigh_Sys,strMenu);
	//Low Limit Of Sys
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPALM, "low_sys", strMenu, sizeof strMenu,"Sys Low");	
	SetWindowText(btnLow_Sys,strMenu);
	//High Limit Of Mean
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPALM, "high_mean", strMenu, sizeof strMenu,"Mean High");	
	SetWindowText(btnHigh_Mean,strMenu);
	//Low Limit Of Mean
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPALM, "low_mean", strMenu, sizeof strMenu,"Mean Low");	
	SetWindowText(btnLow_Mean,strMenu);
	
	//High Limit Of Dia
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPALM, "high_dia", strMenu, sizeof strMenu,"Dia High");	
	SetWindowText(btnHigh_Dia,strMenu);
	//Low Limit Of Dia
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPALM, "low_dia", strMenu, sizeof strMenu,"Dia Low");	
	SetWindowText(btnLow_Dia,strMenu);
	//Exit
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "previous", strMenu, sizeof strMenu,"Previous");	
	SetWindowText(btnOk,strMenu);			
	return 0;
}
//初始化combobox
static int InitCobList(HWND hDlg)
{
	int i;
	char strMenu[100];
	//报警来源
	unsigned char *strAlmSource[] = {"all", "sys", "mean", "dia", "sys_mean", "mean_dia", "sys_dia"};
	unsigned char *strAlmSourceDef[] = {"S/D/M", "S", "M", "D", "S/M", "M/D", "S/D"};

	//almSource	
	for(i=0; i<7; i++){
		memset(strMenu, 0, sizeof strMenu);
		GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPALM, strAlmSource[i], strMenu, sizeof strMenu,strAlmSourceDef[i]);	
		SendMessage(cobAlmSource, CB_ADDSTRING, 0, (LPARAM)strMenu);
	}
	SendMessage(cobAlmSource, CB_SETCURSEL, gCfgNibp.bAlmSource, 0);
	//alm class
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "off", strMenu, sizeof strMenu,"OFF");	
	SendMessage(cobAlmClass, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "low", strMenu, sizeof strMenu,"Low");	
	SendMessage(cobAlmClass, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "middle", strMenu, sizeof strMenu,"Middle");	
	SendMessage(cobAlmClass, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "high", strMenu, sizeof strMenu,"High");	
	SendMessage(cobAlmClass, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobAlmClass, CB_SETCURSEL, gCfgNibp.bAlmControl, 0);
	//alm print
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "off", strMenu, sizeof strMenu,"OFF");	
	SendMessage(cobAlmPrint, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "on", strMenu, sizeof strMenu,"ON");	
	SendMessage(cobAlmPrint, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobAlmPrint, CB_SETCURSEL, gCfgNibp.bPrnControl, 0);
			
	return 0;
}
static int InitEditText(HWND hDlg)
{
	//获得限值
	iTmpHigh_Sys = gCfgNibp.wHigh_Sys;
	iTmpHigh_Dia = gCfgNibp.wHigh_Dia;
	iTmpHigh_Mean= gCfgNibp.wHigh_Mean;
	iTmpLow_Sys = gCfgNibp.wLow_Sys;
	iTmpLow_Dia = gCfgNibp.wLow_Dia;
	iTmpLow_Mean = gCfgNibp.wLow_Mean;
	
	SetLimitText(gCfgNibp.bUnit, editHigh_Sys, gCfgNibp.wHigh_Sys);
	SetLimitText(gCfgNibp.bUnit, editLow_Sys, gCfgNibp.wLow_Sys);
	SetLimitText(gCfgNibp.bUnit, editHigh_Mean, gCfgNibp.wHigh_Mean);
	SetLimitText(gCfgNibp.bUnit, editLow_Mean, gCfgNibp.wLow_Mean);
	SetLimitText(gCfgNibp.bUnit, editHigh_Dia, gCfgNibp.wHigh_Dia);
	SetLimitText(gCfgNibp.bUnit, editLow_Dia, gCfgNibp.wLow_Dia);
			
	return 0;
}
//设置控件回调函数
static int InitCtlCallBackProc(HWND hDlg)
{
	OldBtnProc = SetWindowCallbackProc(btnAlmSource, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnAlmClass, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnAlmPrint, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnHigh_Sys, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnLow_Sys, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnHigh_Mean, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnLow_Mean, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnHigh_Dia, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnLow_Dia, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);

	OldCobProc = SetWindowCallbackProc(cobAlmSource, CobProc);
	OldCobProc = SetWindowCallbackProc(cobAlmClass, CobProc);
	OldCobProc = SetWindowCallbackProc(cobAlmPrint, CobProc);
	OldEditProc = SetWindowCallbackProc(editHigh_Sys, EditProc);
	OldEditProc = SetWindowCallbackProc(editLow_Sys, EditProc);
	OldEditProc = SetWindowCallbackProc(editHigh_Mean, EditProc);
	OldEditProc = SetWindowCallbackProc(editLow_Mean, EditProc);
	OldEditProc = SetWindowCallbackProc(editHigh_Dia, EditProc);
	OldEditProc = SetWindowCallbackProc(editLow_Dia, EditProc);
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
			//设置控件状态
			InitCtlStatus(hDlg);
			
			
			
			
			gbKeyType = KEY_TAB;			
			SetFocus(btnOk);
		}break;
		case MSG_PAINT:{

		}break;
		case MSG_COMMAND:{
			int	id   = LOWORD(wParam);
			
			switch(id){
				case DID_NEWNIBPALM_ALMSOURCE_COB:{
					gbKeyType = KEY_UD;
					SetFocus(cobAlmSource);
				}break;
				case DID_NEWNIBPALM_ALMCLASS_COB:{
					gbKeyType = KEY_UD;
					SetFocus(cobAlmClass);
				}break;
				case DID_NEWNIBPALM_ALMPRINT_COB:{
					gbKeyType = KEY_UD;
					SetFocus(cobAlmPrint);
				}break;
				case DID_NEWNIBPALM_HIGH_SYS_EDIT:{
					gbKeyType = KEY_UD;
					SetFocus(editHigh_Sys);
				}break;
				case DID_NEWNIBPALM_LOW_SYS_EDIT:{
					gbKeyType = KEY_UD;
					SetFocus(editLow_Sys);
				}break;
				case DID_NEWNIBPALM_HIGH_MEAN_EDIT:{
					gbKeyType = KEY_UD;
					SetFocus(editHigh_Mean);
				}break;
				case DID_NEWNIBPALM_LOW_MEAN_EDIT:{
					gbKeyType = KEY_UD;
					SetFocus(editLow_Mean);
				}break;
				case DID_NEWNIBPALM_HIGH_DIA_EDIT:{
					gbKeyType = KEY_UD;
					SetFocus(editHigh_Dia);
				}break;
				case DID_NEWNIBPALM_LOW_DIA_EDIT:{
					gbKeyType = KEY_UD;
					SetFocus(editLow_Dia);
				}break;
				
				case DID_NEWNIBPALM_OK:{
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
void CreateDlgNewNIBPALM(HWND hWnd)
{
	DlgSet.controls = CtrlSet;
	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}


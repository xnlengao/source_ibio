/*************************************************************************** 
 *	Module Name:	DlgSet_Ibp1Set
 *
 *	Abstract:	设置IBP1配置信息的对话框
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-10-06 11:33:04
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"
#include "General_Funcs.h"

#define  DID_IBP1SET						7100
#define DID_IBP1SET_HELP				DID_IBP1SET
#define DID_IBP1SET_OK 					DID_IBP1SET+1
#define DID_IBP1SET_ALMSOURCE			DID_IBP1SET+2
#define DID_IBP1SET_SET_ALMSOURCE		DID_IBP1SET+3
#define DID_IBP1SET_ALMCLASS			DID_IBP1SET+4
#define DID_IBP1SET_SET_ALMCLASS		DID_IBP1SET+5
#define DID_IBP1SET_ALMPRINT			DID_IBP1SET+6
#define DID_IBP1SET_SET_ALMPRINT		DID_IBP1SET+7
#define DID_IBP1SET_HIGH_SYS			DID_IBP1SET+8
#define DID_IBP1SET_SET_HIGH_SYS		DID_IBP1SET+9
#define DID_IBP1SET_LOW_SYS			DID_IBP1SET+10
#define DID_IBP1SET_SET_LOW_SYS		DID_IBP1SET+11
#define DID_IBP1SET_HIGH_MEAN			DID_IBP1SET+12
#define DID_IBP1SET_SET_HIGH_MEAN		DID_IBP1SET+13
#define DID_IBP1SET_LOW_MEAN			DID_IBP1SET+14
#define DID_IBP1SET_SET_LOW_MEAN		DID_IBP1SET+15
#define DID_IBP1SET_HIGH_DIA			DID_IBP1SET+16
#define DID_IBP1SET_SET_HIGH_DIA		DID_IBP1SET+17
#define DID_IBP1SET_LOW_DIA			DID_IBP1SET+18
#define DID_IBP1SET_SET_LOW_DIA		DID_IBP1SET+19
#define DID_IBP1_SET_CAPTON			DID_IBP1SET+20

//帮助对话框
static HWND editHelp;
static WNDPROC OldEditProcHelp;
//其他控件
static HWND btnOk;
static HWND btnSetAlmSource;
static HWND cobAlmSource;
static HWND btnSetAlmClass;
static HWND cobAlmClass;
static HWND btnSetAlmPrint;
static HWND cobAlmPrint;
static HWND btnSetHigh_Sys;
static HWND editHigh_Sys;
static HWND btnSetLow_Sys;
static HWND editLow_Sys;
static HWND btnSetHigh_Dia;
static HWND editHigh_Dia;
static HWND btnSetLow_Dia;
static HWND editLow_Dia;
static HWND btnSetHigh_Mean;
static HWND editHigh_Mean;
static HWND btnSetLow_Mean;
static HWND editLow_Mean;
static HWND editCaption;

//控件回调函数
static WNDPROC OldBtnProc;	
static WNDPROC OldCobProc;
static WNDPROC OldEditProc;

			//限值
unsigned char strLimit[100]={0};
			//报警来源
unsigned char *strAlmSource[] = {"all", "sys", "mean", "dia", "sys_mean", "mean_dia", "sys_dia"};

//对话框属性
//对话框属性
static DLGTEMPLATE DlgSet= {
	WS_VISIBLE || WS_BORDER,
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
		DID_IBP1SET_HELP,
		"",
		0
	}
};


//临时存储上下限，调节的时候使用
static int iTmpHigh_Sys =0, iTmpLow_Sys =0;
static int iTmpHigh_Dia =0, iTmpLow_Dia =0;
static int iTmpHigh_Mean =0, iTmpLow_Mean =0;

//上下限刻度调整的增量mmHg
#define IBP1_LIMIT_INTER	1

//限值 mmHg
#define IBP1_LIMIT_MAX		IBP_MAX
#define IBP1_LIMIT_MIN		IBP_MIN

//上下限值最小幅度
#define IBP1_LIMITRANGE_MIN 	1


/*
	根据单位设置上下限
*/
static int SetLimitText(BOOL bUnit, HWND hWnd, int iLimit)
{
	unsigned char strLimit[50] = {0};
	unsigned char strUnit[20] = {0};
	
	switch(bUnit){
		case IBPUNIT_KPA:{
			memset(strUnit, 0, sizeof strUnit);
			GetStringFromResFiles(gsLanguageRes, "GENERAL", "unit_kpa", strUnit, sizeof strUnit,"Kpa");		
			memset(strLimit, 0, sizeof strLimit);
			snprintf(strLimit, sizeof strLimit, "%3.1f %s", (float)(iLimit / 7.5), strUnit);
			SetWindowText(hWnd, strLimit);
		}break;
		/*case IBPUNIT_CMH2O:{
			memset(strUnit, 0, sizeof strUnit);
			GetStringFromResFile(gsLanguageRes, "GENERAL", "unit_cmh2o", strUnit, sizeof strUnit);		
			memset(strLimit, 0, sizeof strLimit);
			snprintf(strLimit, sizeof strLimit, "%d %s", (int)(iLimit*1.36), strUnit);
			SetWindowText(hWnd, strLimit);
		}break;*/
		default:{
			//IBPUNIT_MMHG
			memset(strUnit, 0, sizeof strUnit);
			GetStringFromResFiles(gsLanguageRes, "GENERAL", "unit_mmhg", strUnit, sizeof strUnit,"mmHg");		
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
	SetWindowBkColor(hWnd,COLOR_black);
#ifdef FONTCOLOR	
	SetWindowElementColorEx( hWnd, FGC_CONTROL_NORMAL,COLOR_yellow);
#endif		
	switch(message){
		case MSG_PAINT:{
			id = lParam;
			switch(id){
				case DID_IBP1SET_SET_ALMSOURCE:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_ibpset_almsource", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_IBP1SET_SET_ALMCLASS:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_ibpset_almclass", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_IBP1SET_SET_ALMPRINT:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_ibpset_almprint", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_IBP1SET_SET_HIGH_SYS:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_ibpset_high_sys", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_IBP1SET_SET_LOW_SYS:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_ibpset_low_sys", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_IBP1SET_SET_HIGH_MEAN:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_ibpset_high_mean", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_IBP1SET_SET_LOW_MEAN:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_ibpset_low_mean", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_IBP1SET_SET_HIGH_DIA:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_ibpset_high_dia", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_IBP1SET_SET_LOW_DIA:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_ibpset_low_dia", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_IBP1SET_OK:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_exit", strHelp, sizeof strHelp);	
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
						case DID_IBP1SET_ALMSOURCE:{
							gCfgIbp1.bAlmSource = index;
							SetIntValueToResFile(gFileSetup, "IBP1Setup", "alarm_source",  gCfgIbp1.bAlmSource, 1);
							SetFocus(btnSetAlmSource);
							//改变报警源
							IsAlm_IBP1(TRUE, TRUE);
						}break;
						case DID_IBP1SET_ALMCLASS:{
							gCfgIbp1.bAlmControl = index;
							SetIntValueToResFile(gFileSetup, "IBP1Setup", "alarm_control",  gCfgIbp1.bAlmControl, 1);
							SetFocus(btnSetAlmClass);
							//改变报警级别
							IsAlm_IBP1(TRUE, TRUE);	
						}break;
						case DID_IBP1SET_ALMPRINT:{
							gCfgIbp1.bPrnControl = index;
							SetIntValueToResFile(gFileSetup, "IBP1Setup", "alarm_print",  gCfgIbp1.bPrnControl, 1);
							SetFocus(btnSetAlmPrint);
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
						case DID_IBP1SET_HIGH_SYS:{
							if(iTmpHigh_Sys <IBP1_LIMIT_MAX){
								iTmpHigh_Sys += IBP1_LIMIT_INTER;
								SetLimitText(gCfgIbp1.bUnit, hWnd, iTmpHigh_Sys);			
							}
						}break;
						case DID_IBP1SET_LOW_SYS:{
							if(iTmpLow_Sys < (iTmpHigh_Sys - IBP1_LIMITRANGE_MIN)){
								iTmpLow_Sys += IBP1_LIMIT_INTER;
								SetLimitText(gCfgIbp1.bUnit, hWnd, iTmpLow_Sys);			
							}
						}break;
						case DID_IBP1SET_HIGH_DIA:{
							if(iTmpHigh_Dia <IBP1_LIMIT_MAX){
								iTmpHigh_Dia += IBP1_LIMIT_INTER;
								SetLimitText(gCfgIbp1.bUnit, hWnd, iTmpHigh_Dia);			
							}
						}break;
						case DID_IBP1SET_LOW_DIA:{
							if(iTmpLow_Dia < (iTmpHigh_Dia - IBP1_LIMITRANGE_MIN)){
								iTmpLow_Dia += IBP1_LIMIT_INTER;
								SetLimitText(gCfgIbp1.bUnit, hWnd, iTmpLow_Dia);			
							}
						}break;
						case DID_IBP1SET_HIGH_MEAN:{
							if(iTmpHigh_Mean <IBP1_LIMIT_MAX){
								iTmpHigh_Mean += IBP1_LIMIT_INTER;
								SetLimitText(gCfgIbp1.bUnit, hWnd, iTmpHigh_Mean);			
							}
						}break;
						case DID_IBP1SET_LOW_MEAN:{
							if(iTmpLow_Mean < (iTmpHigh_Mean - IBP1_LIMITRANGE_MIN)){
								iTmpLow_Mean += IBP1_LIMIT_INTER;
								SetLimitText(gCfgIbp1.bUnit, hWnd, iTmpLow_Mean);			
							}
						}break;
					}
				}break;
				case SCANCODE_CURSORBLOCKUP:{
					switch(id){
						case DID_IBP1SET_HIGH_SYS:{
							if(iTmpHigh_Sys > (iTmpLow_Sys + IBP1_LIMITRANGE_MIN)){
								iTmpHigh_Sys -= IBP1_LIMIT_INTER;
								SetLimitText(gCfgIbp1.bUnit, hWnd, iTmpHigh_Sys);			
							}
						}break;
						case DID_IBP1SET_LOW_SYS:{
							if(iTmpLow_Sys >IBP1_LIMIT_MIN){
								iTmpLow_Sys -= IBP1_LIMIT_INTER;
								SetLimitText(gCfgIbp1.bUnit, hWnd, iTmpLow_Sys);			
							}
						}break;
						case DID_IBP1SET_HIGH_DIA:{
							if(iTmpHigh_Dia > (iTmpLow_Dia + IBP1_LIMITRANGE_MIN)){
								iTmpHigh_Dia -= IBP1_LIMIT_INTER;
								SetLimitText(gCfgIbp1.bUnit, hWnd, iTmpHigh_Dia);			
							}
						}break;
						case DID_IBP1SET_LOW_DIA:{
							if(iTmpLow_Dia>IBP1_LIMIT_MIN){
								iTmpLow_Dia -= IBP1_LIMIT_INTER;
								SetLimitText(gCfgIbp1.bUnit, hWnd, iTmpLow_Dia);			
							}
						}break;
						case DID_IBP1SET_HIGH_MEAN:{
							if(iTmpHigh_Mean > (iTmpLow_Mean + IBP1_LIMITRANGE_MIN)){
								iTmpHigh_Mean -= IBP1_LIMIT_INTER;
								SetLimitText(gCfgIbp1.bUnit, hWnd, iTmpHigh_Mean);			
							}
						}break;
						case DID_IBP1SET_LOW_MEAN:{
							if(iTmpLow_Mean>IBP1_LIMIT_MIN){
								iTmpLow_Mean -= IBP1_LIMIT_INTER;
								SetLimitText(gCfgIbp1.bUnit, hWnd, iTmpLow_Mean);			
							}
						}break;
					}
				}break;
				case SCANCODE_ENTER:{
					switch(id){
						case DID_IBP1SET_HIGH_SYS:{
							gCfgIbp1.iHigh_Sys = iTmpHigh_Sys;
							SetIntValueToResFile(gFileSetup, "IBP1Setup", "sys_high",  gCfgIbp1.iHigh_Sys, 4);
							SetFocus(btnSetHigh_Sys);
						}break;
						case DID_IBP1SET_LOW_SYS:{
							gCfgIbp1.iLow_Sys = iTmpLow_Sys;
							SetIntValueToResFile(gFileSetup, "IBP1Setup", "sys_low",  gCfgIbp1.iLow_Sys, 4);
							SetFocus(btnSetLow_Sys);
						}break;
						case DID_IBP1SET_HIGH_DIA:{
							gCfgIbp1.iHigh_Dia = iTmpHigh_Dia;
							SetIntValueToResFile(gFileSetup, "IBP1Setup", "dia_high",  gCfgIbp1.iHigh_Dia, 4);
							SetFocus(btnSetHigh_Dia);
						}break;
						case DID_IBP1SET_LOW_DIA:{
							gCfgIbp1.iLow_Dia = iTmpLow_Dia;
							SetIntValueToResFile(gFileSetup, "IBP1Setup", "dia_low",  gCfgIbp1.iLow_Dia, 4);
							SetFocus(btnSetLow_Dia);
						}break;
						case DID_IBP1SET_HIGH_MEAN:{
							gCfgIbp1.iHigh_Mean = iTmpHigh_Mean;
							SetIntValueToResFile(gFileSetup, "IBP1Setup", "mean_high",  gCfgIbp1.iHigh_Mean, 4);
							SetFocus(btnSetHigh_Mean);
						}break;
						case DID_IBP1SET_LOW_MEAN:{
							gCfgIbp1.iLow_Mean = iTmpLow_Mean;
							SetIntValueToResFile(gFileSetup, "IBP1Setup", "mean_low",  gCfgIbp1.iLow_Mean, 4);
							SetFocus(btnSetLow_Mean);
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
	对话框回调函数
*/
static int CreateCtlProc(HWND hDlg)
{
			char strMenu[100];
			editCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE  ,
					DID_IBP1_SET_CAPTON, 0, 5, 250, 26, hDlg, 0);

			btnSetAlmSource = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_IBP1SET_SET_ALMSOURCE, 10, 30+29*0, 115, 26, hDlg, 0);

			btnSetAlmClass = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_IBP1SET_SET_ALMCLASS, 10, 30+29*1, 115, 26, hDlg, 0);
			btnSetAlmPrint = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_IBP1SET_SET_ALMPRINT, 10, 30+29*2, 115, 26, hDlg, 0);
			btnSetHigh_Sys = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_IBP1SET_SET_HIGH_SYS, 10, 30+29*3, 115, 26, hDlg, 0);
			btnSetLow_Sys = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_IBP1SET_SET_LOW_SYS, 10, 30+29*4, 115, 26, hDlg, 0);
			btnSetHigh_Mean = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_IBP1SET_SET_HIGH_MEAN, 10, 30+29*5, 115, 26, hDlg, 0);
			btnSetLow_Mean = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_IBP1SET_SET_LOW_MEAN, 10, 30+29*6, 115, 26, hDlg, 0);
			btnSetHigh_Dia = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_IBP1SET_SET_HIGH_DIA, 10, 30+29*7, 115, 26, hDlg, 0);
			btnSetLow_Dia = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_IBP1SET_SET_LOW_DIA, 10, 30+29*8, 115, 26, hDlg, 0);
			btnOk= CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					    DID_IBP1SET_OK, 10, 30+29*9, 115, 26, hDlg, 0);

			


			cobAlmSource = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_IBP1SET_ALMSOURCE, 130, 30+29*0, 95, 26, hDlg, 0);

			cobAlmClass = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_IBP1SET_ALMCLASS, 130, 30+29*1, 95, 26, hDlg, 0);
			cobAlmPrint = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_IBP1SET_ALMPRINT, 130, 30+29*2, 95, 26, hDlg, 0);


			editHigh_Sys = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_IBP1SET_HIGH_SYS, 130, 30+29*3, 95, 23, hDlg, 0);
			editLow_Sys = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_IBP1SET_LOW_SYS, 130, 30+29*4, 95, 23, hDlg, 0);
			editHigh_Mean = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_IBP1SET_HIGH_MEAN, 130, 30+29*5, 95, 23, hDlg, 0);
			editLow_Mean= CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_IBP1SET_LOW_MEAN, 130, 30+29*6, 95, 23, hDlg, 0);
			editHigh_Dia = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_IBP1SET_HIGH_DIA, 130, 30+29*7, 95, 23, hDlg, 0);
			editLow_Dia = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_IBP1SET_LOW_DIA, 130, 30+29*8, 95, 23, hDlg, 0);			
			editHelp = GetDlgItem(hDlg, DID_IBP1SET_HELP);


}

static int InitCtlName(HWND hDlg)
{

			char strMenu[100];
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "IBP", "caption_ibp_alarm", strMenu, sizeof strMenu,"IBP Alarm");	
			SetWindowText(editCaption,strMenu);
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "IBP", "alarm_source", strMenu, sizeof strMenu,"Source");	
			SetWindowText(btnSetAlmSource,strMenu);

			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "GENERAL", "alarm_class", strMenu, sizeof strMenu,"Class");	
			SetWindowText(btnSetAlmClass,strMenu);
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "GENERAL", "alarm_print", strMenu, sizeof strMenu,"Print");
			SetWindowText(btnSetAlmPrint,strMenu);
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "IBP", "high_sys", strMenu, sizeof strMenu,"High Sys");
			SetWindowText(btnSetHigh_Sys,strMenu);
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "IBP", "low_sys", strMenu, sizeof strMenu,"Low_Sys");	
			SetWindowText(btnSetLow_Sys,strMenu);
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "IBP", "high_mean", strMenu, sizeof strMenu,"High Mean");	
			SetWindowText(btnSetHigh_Mean,strMenu);
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "IBP", "low_mean", strMenu, sizeof strMenu,"Low Mean");	
			SetWindowText(btnSetLow_Mean,strMenu);
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "IBP", "high_dia", strMenu, sizeof strMenu,"High Dia");	
			SetWindowText(btnSetHigh_Dia,strMenu);		
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "IBP", "low_dia", strMenu, sizeof strMenu,"Low Dia");	
			SetWindowText(btnSetLow_Dia,strMenu);
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "GENERAL", "exit", strMenu, sizeof strMenu,"Exit");	
			SetWindowText(btnOk,strMenu);


}
static int InitCobList(HWND hDlg)
{
	int i;
	char strMenu[100];

			for(i=0; i<7; i++){
				memset(strMenu, 0, sizeof strMenu);
				GetStringFromResFile(gsLanguageRes, "IBP", strAlmSource[i], strMenu, sizeof strMenu);	
				SendMessage(cobAlmSource, CB_ADDSTRING, 0, (LPARAM)strMenu);
			}
			SendMessage(cobAlmSource, CB_SETCURSEL, gCfgIbp1.bAlmSource, 0);



			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "GENERAL", "off", strMenu, sizeof strMenu,"off");	
			SendMessage(cobAlmClass, CB_ADDSTRING, 0, (LPARAM)strMenu);
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "GENERAL", "low", strMenu, sizeof strMenu,"low");	
			SendMessage(cobAlmClass, CB_ADDSTRING, 0, (LPARAM)strMenu);
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "GENERAL", "middle", strMenu, sizeof strMenu,"middle");	
			SendMessage(cobAlmClass, CB_ADDSTRING, 0, (LPARAM)strMenu);
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "GENERAL", "high", strMenu, sizeof strMenu,"high");	
			SendMessage(cobAlmClass, CB_ADDSTRING, 0, (LPARAM)strMenu);
			SendMessage(cobAlmClass, CB_SETCURSEL, gCfgIbp1.bAlmControl, 0);


			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "GENERAL", "off", strMenu, sizeof strMenu,"off");	
			SendMessage(cobAlmPrint, CB_ADDSTRING, 0, (LPARAM)strMenu);
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "GENERAL", "on", strMenu, sizeof strMenu,"on");	
			SendMessage(cobAlmPrint, CB_ADDSTRING, 0, (LPARAM)strMenu);
			SendMessage(cobAlmPrint, CB_SETCURSEL, gCfgIbp1.bPrnControl, 0);


}
static int InitCtlCallBackProc(HWND hDlg)
{
			OldBtnProc = SetWindowCallbackProc(btnSetAlmSource, BtnProc);						

			OldCobProc = SetWindowCallbackProc(cobAlmSource, CobProc);

			OldBtnProc = SetWindowCallbackProc(btnSetAlmClass, BtnProc);
			
			OldCobProc = SetWindowCallbackProc(cobAlmClass, CobProc);

			OldBtnProc = SetWindowCallbackProc(btnSetAlmPrint, BtnProc);
			OldCobProc = SetWindowCallbackProc(cobAlmPrint, CobProc);
			OldBtnProc = SetWindowCallbackProc(btnSetHigh_Sys, BtnProc);
	
			OldEditProc = SetWindowCallbackProc(editHigh_Sys, EditProc);

			OldBtnProc = SetWindowCallbackProc(btnSetLow_Sys, BtnProc);
			
			OldEditProc = SetWindowCallbackProc(editLow_Sys, EditProc);

			OldBtnProc = SetWindowCallbackProc(btnSetHigh_Mean, BtnProc);
			
			OldEditProc = SetWindowCallbackProc(editHigh_Mean, EditProc);

			OldBtnProc = SetWindowCallbackProc(btnSetLow_Mean, BtnProc);

			OldEditProc = SetWindowCallbackProc(editLow_Mean, EditProc);

			OldBtnProc = SetWindowCallbackProc(btnSetHigh_Dia, BtnProc);
			
			OldEditProc = SetWindowCallbackProc(editHigh_Dia, EditProc);

			OldBtnProc = SetWindowCallbackProc(btnSetLow_Dia, BtnProc);
		
			OldEditProc = SetWindowCallbackProc(editLow_Dia, EditProc);

			OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);
				
			OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp); 
}
static void SetLimt()
{
			//获得限值
			iTmpHigh_Sys = gCfgIbp1.iHigh_Sys;
			iTmpHigh_Dia = gCfgIbp1.iHigh_Dia;
			iTmpHigh_Mean= gCfgIbp1.iHigh_Mean;
			iTmpLow_Sys = gCfgIbp1.iLow_Sys;
			iTmpLow_Dia = gCfgIbp1.iLow_Dia;
			iTmpLow_Mean = gCfgIbp1.iLow_Mean;

			SetLimitText(gCfgIbp1.bUnit, editHigh_Sys, gCfgIbp1.iHigh_Sys);
			SetLimitText(gCfgIbp1.bUnit, editLow_Sys, gCfgIbp1.iLow_Sys);
			SetLimitText(gCfgIbp1.bUnit, editHigh_Mean, gCfgIbp1.iHigh_Mean);			
			SetLimitText(gCfgIbp1.bUnit, editLow_Mean, gCfgIbp1.iLow_Mean);
			SetLimitText(gCfgIbp1.bUnit, editHigh_Dia, gCfgIbp1.iHigh_Dia);
			SetLimitText(gCfgIbp1.bUnit, editLow_Dia, gCfgIbp1.iLow_Dia);
					
}
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

			//设置控件回调函数
			InitCtlCallBackProc(hDlg);

			SetLimt();
			
			gbKeyType = KEY_TAB;
			
			//获得窗口坐标
			GetWindowRect(hDlg, &gRCDlg_Adjust);
								
			SetFocus(btnOk);
		}break;
		case MSG_PAINT:{

		}break;
		case MSG_COMMAND:{
			int	id   = LOWORD(wParam);
			
			switch(id){
				case DID_IBP1SET_SET_ALMSOURCE:{
					gbKeyType = KEY_UD;
					SetFocus(cobAlmSource);
				}break;
				case DID_IBP1SET_SET_ALMCLASS:{
					gbKeyType = KEY_UD;
					SetFocus(cobAlmClass);
				}break;
				case DID_IBP1SET_SET_ALMPRINT:{
					gbKeyType = KEY_UD;
					SetFocus(cobAlmPrint);
				}break;
				case DID_IBP1SET_SET_HIGH_SYS:{
					gbKeyType = KEY_UD;
					SetFocus(editHigh_Sys);
				}break;
				case DID_IBP1SET_SET_LOW_SYS:{
					gbKeyType = KEY_UD;
					SetFocus(editLow_Sys);
				}break;
				case DID_IBP1SET_SET_HIGH_DIA:{
					gbKeyType = KEY_UD;
					SetFocus(editHigh_Dia);
				}break;
				case DID_IBP1SET_SET_LOW_DIA:{
					gbKeyType = KEY_UD;
					SetFocus(editLow_Dia);
				}break;
				case DID_IBP1SET_SET_HIGH_MEAN:{
					gbKeyType = KEY_UD;
					SetFocus(editHigh_Mean);
				}break;
				case DID_IBP1SET_SET_LOW_MEAN:{
					gbKeyType = KEY_UD;
					SetFocus(editLow_Mean);
				}break;
				case DID_IBP1SET_OK:{
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
void CreateDlgIbp1Set(HWND hWnd)
{
	DlgSet.controls = CtrlSet;
	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}


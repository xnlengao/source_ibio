/*************************************************************************** 
 *	Module Name:	DlgNew_Temp
 *
 *	Abstract:	设置Temp配置信息的对话框
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2013-1-22 14:44:06
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"
#include "AlarmManage.h"

//DID_NEWTEMP
#define DID_NEWTEMP_HELP				DID_NEWTEMP
#define DID_NEWTEMP_OK				DID_NEWTEMP+1
#define DID_NEWTEMP_CAPTION				DID_NEWTEMP+2
#define DID_NEWTEMP_UNIT				DID_NEWTEMP+3
#define DID_NEWTEMP_ALMCLASS			DID_NEWTEMP+4
#define DID_NEWTEMP_ALMPRINT			DID_NEWTEMP+5
#define DID_NEWTEMP_HIGH_T1			DID_NEWTEMP+6
#define DID_NEWTEMP_LOW_T1			DID_NEWTEMP+7
#define DID_NEWTEMP_HIGH_T2			DID_NEWTEMP+8
#define DID_NEWTEMP_LOW_T2			DID_NEWTEMP+9
#define DID_NEWTEMP_HIGH_TD			DID_NEWTEMP+10
#define DID_NEWTEMP_UNIT_COB			DID_NEWTEMP+11
#define DID_NEWTEMP_ALMCLASS_COB		DID_NEWTEMP+12
#define DID_NEWTEMP_ALMPRINT_COB		DID_NEWTEMP+13
#define DID_NEWTEMP_HIGH_T1_EDIT		DID_NEWTEMP+14
#define DID_NEWTEMP_LOW_T1_EDIT		DID_NEWTEMP+15
#define DID_NEWTEMP_HIGH_T2_EDIT		DID_NEWTEMP+16
#define DID_NEWTEMP_LOW_T2_COB		DID_NEWTEMP+17
#define DID_NEWTEMP_HIGH_TD_COB		DID_NEWTEMP+18

//帮助对话框
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;

//其他控件
static HWND btnOk;
static HWND btnUnit;
static HWND btnAlmClass;
static HWND btnAlmPrint;
static HWND btnHigh_T1;
static HWND btnLow_T1;
static HWND btnHigh_T2;
static HWND btnLow_T2;
static HWND btnHigh_TD;
static HWND cobUnit;
static HWND cobAlmClass;
static HWND cobAlmPrint;
static HWND editHigh_T1;
static HWND editLow_T1;
static HWND editHigh_T2;
static HWND editLow_T2;
static HWND editHigh_TD;

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
		DID_NEWTEMP_HELP,
		"",
		0
	}
};

//临时存储上下限，调节的时候使用
static int iTmpHigh_T1 = 0, iTmpLow_T1 = 0;
static int iTmpHigh_T2 = 0, iTmpLow_T2 = 0;
static int iTmpHigh_TD = 0; 

//上下限刻度调整的增量 10xC
#define TEMP_LIMIT_INTER		1	

//限值 10xC
#define TEMP_LIMIT_MAX	TEMP_MAX
#define TEMP_LIMIT_MIN	TEMP_MIN
#define TD_LIMIT_MAX		200
#define TD_LIMIT_MIN		0

//上下限值最小幅度 10xC
#define TEMP_LIMITRANGE_MIN 	1


/*
	根据单位设置上下限
*/
static int SetLimitText(BOOL bUnit, HWND hWnd, int iLimit)
{
	unsigned char strLimit[50] = {0};
	unsigned char strUnit[20] = {0};
	
	switch(bUnit){
		case TEMP_UNIT_F:{
			memset(strUnit, 0, sizeof strUnit);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "unit_f", strUnit, sizeof strUnit,"癋");		
			memset(strLimit, 0, sizeof strLimit);
			snprintf(strLimit, sizeof strLimit, "%.1f %s", (double)iLimit / 10.0*1.8+32.0, strUnit);
			SetWindowText(hWnd, strLimit);
		}break;
		default:{
			//TEMP_UNIT_C
			memset(strUnit, 0, sizeof strUnit);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "unit_c", strUnit, sizeof strUnit,"癈");		
			memset(strLimit, 0, sizeof strLimit);
			snprintf(strLimit, sizeof strLimit, "%.1f %s", (double)iLimit / 10.0, strUnit);
			SetWindowText(hWnd, strLimit);
		}break;
	}
	
	return 0;
}


//改变单位,同时改变限值显示的方式
static int ChangeLimitView(BYTE bUnit)
{
	
	SetLimitText(bUnit, editHigh_T1, iTmpHigh_T1);
	SetLimitText(bUnit, editHigh_T2, iTmpHigh_T2);
	SetLimitText(bUnit, editHigh_TD, iTmpHigh_TD);
	SetLimitText(bUnit, editLow_T1, iTmpLow_T1);
	SetLimitText(bUnit, editLow_T2, iTmpLow_T2);
			
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
				case DID_NEWTEMP_ALMCLASS_COB:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "help_alm_class", strHelp, sizeof strHelp,"Set up alarm ON/OFF and class.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWTEMP_ALMPRINT_COB:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "help_alm_print", strHelp, sizeof strHelp,"Set up alarm print.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWTEMP_UNIT_COB:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_TEMP, "help_unit", strHelp, sizeof strHelp,"Set up Temp uint.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWTEMP_HIGH_T1_EDIT:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_TEMP, "help_high_t1", strHelp, sizeof strHelp,"Set up T1 alarm high limit.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWTEMP_LOW_T1_EDIT:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_TEMP, "help_low_t1", strHelp, sizeof strHelp,"Set up T1 alarm low limit.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWTEMP_HIGH_T2_EDIT:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_TEMP, "help_high_t2", strHelp, sizeof strHelp,"Set up T2 alarm high limit.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWTEMP_LOW_T2_COB:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_TEMP, "help_low_t2", strHelp, sizeof strHelp,"Set up T2 alarm low limit.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWTEMP_HIGH_TD_COB:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_TEMP, "help_high_td", strHelp, sizeof strHelp,"Set up TD alarm  limit.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWTEMP_OK:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "help_backtomain", strHelp, sizeof strHelp,"Back to Main");	
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

//改变参数的报警级别
static int ChangeAlarmClass()
{
	B_ALM_INFO sAlmInfo;
	
	IsAlm_T1(&gValueTemp, &gCfgTemp, TRUE);
	IsAlm_T2(&gValueTemp, &gCfgTemp, TRUE);
 	IsAlm_TD(&gValueTemp, &gCfgTemp, TRUE);
	
	return 0;
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
						case DID_NEWTEMP_ALMCLASS:{
							gCfgTemp.bAlmControl = index;
							SetIntValueToResFile(gFileSetup, "TEMPSetup", "alarm_control",  gCfgTemp.bAlmControl, 1);
							SetFocus(btnAlmClass);
							//改变参数的报警级别
							ChangeAlarmClass();
						}break;
						case DID_NEWTEMP_ALMPRINT:{
							gCfgTemp.bPrnControl = index;
							SetIntValueToResFile(gFileSetup, "TEMPSetup", "alarm_print",  gCfgTemp.bPrnControl, 1);
							SetFocus(btnAlmPrint);
						}break;
						case DID_NEWTEMP_UNIT:{
							//更新限值
							if(gCfgTemp.bUnit != index){
								ChangeLimitView(index);
							}
							gCfgTemp.bUnit = index;
							SetIntValueToResFile(gFileSetup, "TEMPSetup", "unit",  gCfgTemp.bUnit, 1);
							SetFocus(btnUnit);
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
						case DID_NEWTEMP_HIGH_T1:{
							if(iTmpHigh_T1 <TEMP_LIMIT_MAX){
								iTmpHigh_T1 += TEMP_LIMIT_INTER;
								SetLimitText(gCfgTemp.bUnit, hWnd, iTmpHigh_T1);			
							}
						}break;
						case DID_NEWTEMP_LOW_T1:{
							if(iTmpLow_T1 < (iTmpHigh_T1 - TEMP_LIMITRANGE_MIN)){
								iTmpLow_T1 += TEMP_LIMIT_INTER;
								SetLimitText(gCfgTemp.bUnit, hWnd, iTmpLow_T1);			
							}
						}break;
						case DID_NEWTEMP_HIGH_T2:{
							if(iTmpHigh_T2 <TEMP_LIMIT_MAX){
								iTmpHigh_T2 += TEMP_LIMIT_INTER;
								SetLimitText(gCfgTemp.bUnit, hWnd, iTmpHigh_T2);			
							}
						}break;
						case DID_NEWTEMP_LOW_T2:{
							if(iTmpLow_T2 < (iTmpHigh_T2 - TEMP_LIMITRANGE_MIN)){
								iTmpLow_T2 += TEMP_LIMIT_INTER;
								SetLimitText(gCfgTemp.bUnit, hWnd, iTmpLow_T2);			
							}
						}break;
						case DID_NEWTEMP_HIGH_TD:{
							if(iTmpHigh_TD < TD_LIMIT_MAX){
								iTmpHigh_TD += TEMP_LIMIT_INTER;
								SetLimitText(gCfgTemp.bUnit, hWnd, iTmpHigh_TD);			
							}
						}break;
					}
				}break;
				case SCANCODE_CURSORBLOCKUP:{
					switch(id){
						case DID_NEWTEMP_HIGH_T1:{
							if(iTmpHigh_T1 > (iTmpLow_T1 + TEMP_LIMITRANGE_MIN)){
								iTmpHigh_T1 -= TEMP_LIMIT_INTER;
								SetLimitText(gCfgTemp.bUnit, hWnd, iTmpHigh_T1);			
							}
						}break;
						case DID_NEWTEMP_LOW_T1:{
							if(iTmpLow_T1 > TEMP_LIMIT_MIN){
								iTmpLow_T1 -= TEMP_LIMIT_INTER;
								SetLimitText(gCfgTemp.bUnit, hWnd, iTmpLow_T1);			
							}
						}break;
						case DID_NEWTEMP_HIGH_T2:{
							if(iTmpHigh_T2 > (iTmpLow_T2 + TEMP_LIMITRANGE_MIN)){
								iTmpHigh_T2 -= TEMP_LIMIT_INTER;
								SetLimitText(gCfgTemp.bUnit, hWnd, iTmpHigh_T2);			
							}
						}break;
						case DID_NEWTEMP_LOW_T2:{
							if(iTmpLow_T2 > TEMP_LIMIT_MIN){
								iTmpLow_T2 -= TEMP_LIMIT_INTER;
								SetLimitText(gCfgTemp.bUnit, hWnd, iTmpLow_T2);			
							}
						}break;
						case DID_NEWTEMP_HIGH_TD:{
							if(iTmpHigh_TD > TD_LIMIT_MIN){
								iTmpHigh_TD -= TEMP_LIMIT_INTER;
								SetLimitText(gCfgTemp.bUnit, hWnd, iTmpHigh_TD);			
							}
						}break;
						
					}
				}break;
				case SCANCODE_ENTER:{
					switch(id){
						case DID_NEWTEMP_HIGH_T1:{
							gCfgTemp.wHigh_T1 = iTmpHigh_T1;
							SetIntValueToResFile(gFileSetup, "TEMPSetup", "t1_high",  gCfgTemp.wHigh_T1, 3);
							SetFocus(btnHigh_T1);
						}break;
						case DID_NEWTEMP_LOW_T1:{
							gCfgTemp.wLow_T1 = iTmpLow_T1;
							SetIntValueToResFile(gFileSetup, "TEMPSetup", "t1_low",  gCfgTemp.wLow_T1, 3);
							SetFocus(btnLow_T1);
						}break;
						case DID_NEWTEMP_HIGH_T2:{
							gCfgTemp.wHigh_T2 = iTmpHigh_T2;
							SetIntValueToResFile(gFileSetup, "TEMPSetup", "t2_high",  gCfgTemp.wHigh_T2, 3);
							SetFocus(btnHigh_T2);
						}break;
						case DID_NEWTEMP_LOW_T2:{
							gCfgTemp.wLow_T2 = iTmpLow_T2;
							SetIntValueToResFile(gFileSetup, "TEMPSetup", "t2_low",  gCfgTemp.wLow_T2, 3);
							SetFocus(btnLow_T2);
						}break;
						case DID_NEWTEMP_HIGH_TD:{
							gCfgTemp.wHigh_TD = iTmpHigh_TD;
							SetIntValueToResFile(gFileSetup, "TEMPSetup", "td_high",  gCfgTemp.wHigh_TD, 3);
							SetFocus(btnHigh_TD);
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
	editHelp = GetDlgItem(hDlg, DID_NEWTEMP_HELP);
	editCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE  ,
					    DID_NEWTEMP_CAPTION, 0, 5, 250, 25, hDlg, 0);
	btnUnit = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_NEWTEMP_UNIT_COB, 10, 30+30*0, 115, 26, hDlg, 0);
	btnAlmClass = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_NEWTEMP_ALMCLASS_COB, 10, 30+30*1, 115, 26, hDlg, 0);
	btnAlmPrint = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_NEWTEMP_ALMPRINT_COB, 10, 30+30*2, 115, 26, hDlg, 0);
	btnHigh_T1 = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_NEWTEMP_HIGH_T1_EDIT, 10, 30+30*3, 115, 26, hDlg, 0);
	btnLow_T1 = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_NEWTEMP_LOW_T1_EDIT, 10, 30+30*4, 115, 26, hDlg, 0);
	btnHigh_T2 = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_NEWTEMP_HIGH_T2_EDIT, 10, 30+30*5, 115, 26, hDlg, 0);
	btnLow_T2 = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_NEWTEMP_LOW_T2_COB, 10, 30+30*6, 115, 26, hDlg, 0);
	btnHigh_TD = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_NEWTEMP_HIGH_TD_COB, 10, 30+30*7, 115, 26, hDlg, 0);
	btnOk= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					    DID_NEWTEMP_OK, 10, 30+30*8, 115, 26, hDlg, 0);
			

	cobUnit = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_NEWTEMP_UNIT, 130, 30+30*0, 95, 26, hDlg, 0);
	cobAlmClass = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_NEWTEMP_ALMCLASS, 130, 30+30*1, 95, 26, hDlg, 0);
	cobAlmPrint = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_NEWTEMP_ALMPRINT, 130, 30+30*2, 95, 26, hDlg, 0);
	editHigh_T1 = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_NEWTEMP_HIGH_T1, 130, 30+30*3, 95, 23, hDlg, 0);
	editLow_T1 = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_NEWTEMP_LOW_T1, 130, 30+30*4, 95, 23, hDlg, 0);
	editHigh_T2 = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_NEWTEMP_HIGH_T2, 130, 30+30*5, 95, 23, hDlg, 0);
	editLow_T2 = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_NEWTEMP_LOW_T2, 130, 30+30*6, 95, 23, hDlg, 0);
	editHigh_TD = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_NEWTEMP_HIGH_TD, 130, 30+30*7, 95, 23, hDlg, 0);
			
	return 0;
}
//设置控件字符
static int InitCtlName(HWND hDlg)
{
	char strMenu[100];
	//caption
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_TEMP, "caption", strMenu, sizeof strMenu,"TEMP");	
	SetWindowText(editCaption,strMenu);
	//Unit
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_TEMP, "unit", strMenu, sizeof strMenu,"Unit");	
	SetWindowText(btnUnit,strMenu);
	//Alarm Class
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "alarm_class", strMenu, sizeof strMenu,"Alm Class");	
	SetWindowText(btnAlmClass,strMenu);
	//Alarm Print
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "alarm_print", strMenu, sizeof strMenu,"Alm Print");	
	SetWindowText(btnAlmPrint,strMenu);
	//High Limit Of T1
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_TEMP, "high_t1", strMenu, sizeof strMenu,"T1 High");	
	SetWindowText(btnHigh_T1,strMenu);	
	//Low Limit Of T1
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_TEMP, "low_t1", strMenu, sizeof strMenu,"T1 Low");	
	SetWindowText(btnLow_T1,strMenu);
	//High Limit Of T2
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_TEMP, "high_t2", strMenu, sizeof strMenu,"T2 High");	
	SetWindowText(btnHigh_T2,strMenu);
	//Low Limit Of T2
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes,STR_SETTING_DLG_TEMP, "low_t2", strMenu, sizeof strMenu,"T2 Low");	
	SetWindowText(btnLow_T2,strMenu);
	//High Limit Of TD
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_TEMP, "high_td", strMenu, sizeof strMenu,"TD High");	
	SetWindowText(btnHigh_TD,strMenu);	
	//Exit
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "backtomain", strMenu, sizeof strMenu,"Back to Main");	
	SetWindowText(btnOk,strMenu);		
	return 0;
}
//初始化combobox
static int InitCobList(HWND hDlg)
{	
	char strMenu[100];
	unsigned char *strUnitIndex[] = {"unit_c", "unit_f"};	
	//Unit
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, strUnitIndex[TEMP_UNIT_C], strMenu, sizeof strMenu,"癈");	
	SendMessage(cobUnit, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, strUnitIndex[TEMP_UNIT_F], strMenu, sizeof strMenu,"癋");	
	SendMessage(cobUnit, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobUnit, CB_SETCURSEL, gCfgTemp.bUnit, 0);
	//alarm class
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "off", strMenu, sizeof strMenu,"OFF");	
	SendMessage(cobAlmClass, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "low", strMenu, sizeof strMenu,"Low");	
	SendMessage(cobAlmClass, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "middle", strMenu, sizeof strMenu,"Middle");	
	SendMessage(cobAlmClass, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "high", strMenu, sizeof strMenu,"High");	
	SendMessage(cobAlmClass, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobAlmClass, CB_SETCURSEL, gCfgTemp.bAlmControl, 0);
	//alarm print
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "off", strMenu, sizeof strMenu,"OFF");	
	SendMessage(cobAlmPrint, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "on", strMenu, sizeof strMenu,"ON");	
	SendMessage(cobAlmPrint, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobAlmPrint, CB_SETCURSEL, gCfgTemp.bPrnControl, 0);

	return 0;
}

static int InitEditText(HWND hDlg)
{
	//获得限值
	iTmpHigh_T1 = gCfgTemp.wHigh_T1;
	iTmpHigh_T2 = gCfgTemp.wHigh_T2;
	iTmpHigh_TD = gCfgTemp.wHigh_TD;
	iTmpLow_T1 = gCfgTemp.wLow_T1;
	iTmpLow_T2 = gCfgTemp.wLow_T2;
	
	SetLimitText(gCfgTemp.bUnit, editHigh_T1, gCfgTemp.wHigh_T1);
	SetLimitText(gCfgTemp.bUnit, editLow_T1, gCfgTemp.wLow_T1);
	SetLimitText(gCfgTemp.bUnit, editHigh_T2, gCfgTemp.wHigh_T2);
	SetLimitText(gCfgTemp.bUnit, editLow_T2, gCfgTemp.wLow_T2);
	SetLimitText(gCfgTemp.bUnit, editHigh_TD, gCfgTemp.wHigh_TD);

	return 0;
}
//设置控件回调函数
static int InitCtlCallBackProc(HWND hDlg)
{
	OldBtnProc = SetWindowCallbackProc(btnUnit, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnAlmClass, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnAlmPrint, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnHigh_T1, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnLow_T1, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnHigh_T2, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnLow_T2, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnHigh_TD, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);
		
	OldCobProc = SetWindowCallbackProc(cobUnit, CobProc);
	OldCobProc = SetWindowCallbackProc(cobAlmClass, CobProc);
	OldCobProc = SetWindowCallbackProc(cobAlmPrint, CobProc);
	OldEditProc = SetWindowCallbackProc(editHigh_T1, EditProc);
	OldEditProc = SetWindowCallbackProc(editLow_T1, EditProc);
	OldEditProc = SetWindowCallbackProc(editHigh_T2, EditProc);
	OldEditProc = SetWindowCallbackProc(editLow_T2, EditProc);
	OldEditProc = SetWindowCallbackProc(editHigh_TD, EditProc);
		
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
	char strMenu[100]={0};
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
				case DID_NEWTEMP_ALMCLASS_COB:{
					gbKeyType = KEY_UD;
					SetFocus(cobAlmClass);
				}break;
				case DID_NEWTEMP_ALMPRINT_COB:{
					gbKeyType = KEY_UD;
					SetFocus(cobAlmPrint);
				}break;
				case DID_NEWTEMP_UNIT_COB:{
					gbKeyType = KEY_UD;
					SetFocus(cobUnit);
				}break;
				case DID_NEWTEMP_HIGH_T1_EDIT:{
					gbKeyType = KEY_UD;
					SetFocus(editHigh_T1);
				}break;
				case DID_NEWTEMP_LOW_T1_EDIT:{
					gbKeyType = KEY_UD;
					SetFocus(editLow_T1);
				}break;
				case DID_NEWTEMP_HIGH_T2_EDIT:{
					gbKeyType = KEY_UD;
					SetFocus(editHigh_T2);
				}break;
				case DID_NEWTEMP_LOW_T2_COB:{
					gbKeyType = KEY_UD;
					SetFocus(editLow_T2);
				}break;
				case DID_NEWTEMP_HIGH_TD_COB:{
					gbKeyType = KEY_UD;
					SetFocus(editHigh_TD);
				}break;
				case DID_NEWTEMP_OK:{
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
			gbKeyType = KEY_LR;	
			//恢复波形位置
		//	ChangeWavePanelsLeft((PRECT)NULL);
			
			//向中央机发送信息
		//	NetSend_Temp1Info();
		//	NetSend_Temp2Info();

			EndDialog(hDlg, wParam);	
		}break;
	}
	
	temp = DefaultDialogProc(hDlg, message, wParam, lParam);
	return(temp);
}

/*
	建立对话框
*/
void CreateDlgNewTemp(HWND hWnd)
{
	DlgSet.controls = CtrlSet;
	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}




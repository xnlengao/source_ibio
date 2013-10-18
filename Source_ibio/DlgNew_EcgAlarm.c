/*************************************************************************** 
 *	Module Name:	DlgNew_EcgAlarm
 *
 *	Abstract:Ecg设置中的报警设置选项
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2013-1-17 	16:14:18
 *	
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"

//DID_NEWECGALM
#define DID_NEWECGALM_HELP			DID_NEWECGALM
#define DID_NEWECGALM_OK				DID_NEWECGALM+1
#define DID_NEWECGALM_CAPTION		DID_NEWECGALM+2
#define DID_NEWECGALM_ALM				DID_NEWECGALM+3
#define DID_NEWECGALM_ALMPRINT		DID_NEWECGALM+4
#define DID_NEWECGALM_HIGH_HR			DID_NEWECGALM+5
#define DID_NEWECGALM_LOW_HR			DID_NEWECGALM+6
#define DID_NEWECGALM_ALM_COB				DID_NEWECGALM+7
#define DID_NEWECGALM_ALMPRINT_COB		DID_NEWECGALM+8
#define DID_NEWECGALM_HIGH_HR_EDIT			DID_NEWECGALM+9
#define DID_NEWECGALM_LOW_HR_EDIT			DID_NEWECGALM+10
#define DID_NEWECGALM_HIGH_ST1			DID_NEWECGALM+11
#define DID_NEWECGALM_LOW_ST1			DID_NEWECGALM+12
#define DID_NEWECGALM_HIGH_ST2			DID_NEWECGALM+13
#define DID_NEWECGALM_LOW_ST2			DID_NEWECGALM+14
#define DID_NEWECGALM_HIGH_ST1_EDIT			DID_NEWECGALM+15
#define DID_NEWECGALM_LOW_ST1_EDIT			DID_NEWECGALM+16
#define DID_NEWECGALM_HIGH_ST2_EDIT			DID_NEWECGALM+17
#define DID_NEWECGALM_LOW_ST2_EDIT			DID_NEWECGALM+18

//上下限刻度调整的增量
#define HR_LIMIT_INTER		1	

//限值
#define HR_LIMIT_MAX		HR_MAX
#define HR_LIMIT_MIN		HR_MIN

//ST上下限值最小幅度
#define HR_LIMITRANGE_MIN 		1
//x100
#define ST_LIMIT_HIGH			ST_MAX
#define ST_LIMIT_LOW			ST_MIN

#define ST_ADJUST_INTER		10			//0.1

//帮助对话框
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;

//其他控件 
static HWND btnOk;
static HWND btnAlm;
static HWND btnAlmPrint;
static HWND btnHigh_HR;
static HWND btnLow_HR;
static HWND btnHigh_ST1;
static HWND btnLow_ST1;
static HWND btnHigh_ST2;
static HWND btnLow_ST2;
static HWND cobAlm;
static HWND cobAlmPrint;

static HWND editHigh_HR;
static HWND editLow_HR;
static HWND editHigh_ST1;
static HWND editLow_ST1;
static HWND editHigh_ST2;
static HWND editLow_ST2;

//控件回调函数
static WNDPROC OldBtnProc;
static WNDPROC OldCobProc;
static WNDPROC OldEditProc;

//临时存储上下限，调节的时候使用
static int iTmpHigh_HR =0, iTmpLow_HR = 0;
static int iTmpHigh_ST1 =0, iTmpLow_ST1 = 0;
static int iTmpHigh_ST2 =0, iTmpLow_ST2 = 0;


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
		DID_NEWECGALM_HELP,
		"",
		0
	}
};

/*
	设置上下限文字
*/
static int SetLimitText(HWND hWnd, int iLimit)
{
	unsigned char strLimit[30] = {0};
	unsigned char strUnit[10] = {0};
	
	memset(strUnit, 0, sizeof strUnit);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "unit_bpm", strUnit, sizeof strUnit,"bpm");		
	memset(strLimit, 0, sizeof strLimit);
	snprintf(strLimit, sizeof strLimit, "%d %s", iLimit, strUnit);
	SetWindowText(hWnd, strLimit);
	
	return 0;
}
//设置上下限文字
static int SetSTLimitText(HWND hWnd, int iLimit)
{
	unsigned char strLimit[15] = {0};
	

	snprintf(strLimit, sizeof strLimit, "%.2f mv", (float)(iLimit/100.0));
				
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
				case DID_NEWECGALM_OK:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "help_previous", strHelp, sizeof strHelp,"Return to the previous menu.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWECGALM_ALM:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "help_alm_class", strHelp, sizeof strHelp,"Set up alarm ON/OFF and class.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWECGALM_ALMPRINT:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "help_alm_print", strHelp, sizeof strHelp,"Set up print or not when alarm.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWECGALM_HIGH_HR:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGALM, "help_hrhigh", strHelp, sizeof strHelp,"Set up HR alarm high limit.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWECGALM_LOW_HR:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGALM, "help_hrlow", strHelp, sizeof strHelp,"Set up HR alarm low limit.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWECGALM_HIGH_ST1:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGALM, "help_st1high", strHelp, sizeof strHelp,"Set up ST1 alarm high limit.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWECGALM_LOW_ST1:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGALM, "help_st1low", strHelp, sizeof strHelp,"Set up ST1 alarm low limit.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWECGALM_HIGH_ST2:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGALM, "help_st2high", strHelp, sizeof strHelp,"Set up ST2  alarm high limit.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWECGALM_LOW_ST2:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGALM, "help_st2low", strHelp, sizeof strHelp,"Set up ST2 alarm low limit.");
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
						case DID_NEWECGALM_ALM_COB:{
							gCfgEcg.bAlmControl = index;
							SetIntValueToResFile(gFileSetup, "ECGSetup", "alarm_control",  gCfgEcg.bAlmControl, 1);
							SetFocus(btnAlm);
							IsAlm_HR(&gValueEcg, &gCfgEcg, TRUE);
							IsAlm_ST1(&gValueEcg, &gCfgEcg, TRUE);
							IsAlm_ST2(&gValueEcg, &gCfgEcg, TRUE);
							IsAlm_Arr(&gValueEcg, &gCfgEcg, TRUE);
						}break;
						case DID_NEWECGALM_ALMPRINT_COB:{
							gCfgEcg.bPrnControl = index;
							SetIntValueToResFile(gFileSetup, "ECGSetup", "alarm_print",  gCfgEcg.bPrnControl, 1);
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
						case DID_NEWECGALM_HIGH_HR_EDIT:{
							if(iTmpHigh_HR < HR_LIMIT_MAX){
								iTmpHigh_HR += HR_LIMIT_INTER;
								SetLimitText(hWnd, iTmpHigh_HR);			
							}
						}break;
						case DID_NEWECGALM_LOW_HR_EDIT:{
							if(iTmpLow_HR < (iTmpHigh_HR - HR_LIMITRANGE_MIN)){
								iTmpLow_HR += HR_LIMIT_INTER;
								SetLimitText(hWnd, iTmpLow_HR);			
							}
						}break;
						case DID_NEWECGALM_HIGH_ST1_EDIT:{
							if(iTmpHigh_ST1< ST_LIMIT_HIGH){
								iTmpHigh_ST1 += ST_ADJUST_INTER;
								SetSTLimitText(hWnd, iTmpHigh_ST1);			
							}
						}break;
						case DID_NEWECGALM_LOW_ST1_EDIT:{
							if(iTmpLow_ST1< iTmpHigh_ST1){
								iTmpLow_ST1 += ST_ADJUST_INTER;
								SetSTLimitText(hWnd, iTmpLow_ST1);			
							}
						}break;
							case DID_NEWECGALM_HIGH_ST2_EDIT:{
							if(iTmpHigh_ST2< ST_LIMIT_HIGH){
								iTmpHigh_ST2 += ST_ADJUST_INTER;
								SetSTLimitText(hWnd, iTmpHigh_ST2);			
							}
						}break;
						case DID_NEWECGALM_LOW_ST2_EDIT:{
							if(iTmpLow_ST2< iTmpHigh_ST2){
								iTmpLow_ST2 += ST_ADJUST_INTER;
								SetSTLimitText(hWnd, iTmpLow_ST2);			
							}
						}break;
					}
				}break;
				case SCANCODE_CURSORBLOCKUP:{
					switch(id){
						case DID_NEWECGALM_HIGH_HR_EDIT:{
							if(iTmpHigh_HR > (iTmpLow_HR + HR_LIMITRANGE_MIN)){
								iTmpHigh_HR -= HR_LIMIT_INTER;
								SetLimitText(hWnd, iTmpHigh_HR);			
							}
						}break;
						case DID_NEWECGALM_LOW_HR_EDIT:{
							if(iTmpLow_HR > HR_LIMIT_MIN){
								iTmpLow_HR -= HR_LIMIT_INTER;
								SetLimitText(hWnd, iTmpLow_HR);			
							}
						}break;
						case DID_NEWECGALM_HIGH_ST1_EDIT:{
							if(iTmpHigh_ST1 > iTmpLow_ST1){
								iTmpHigh_ST1 -= ST_ADJUST_INTER;
								SetSTLimitText(hWnd, iTmpHigh_ST1);			
							}
						}break;
						case DID_NEWECGALM_LOW_ST1_EDIT:{
							if(iTmpLow_ST1 > ST_LIMIT_LOW){
								iTmpLow_ST1 -= ST_ADJUST_INTER;
								SetSTLimitText(hWnd, iTmpLow_ST1);			
							}
						}break;
						
						case DID_NEWECGALM_HIGH_ST2_EDIT:{
							if(iTmpHigh_ST2 > iTmpLow_ST2){
								iTmpHigh_ST2 -= ST_ADJUST_INTER;
								SetSTLimitText(hWnd, iTmpHigh_ST2);			
							}
						}break;
						case DID_NEWECGALM_LOW_ST2_EDIT:{
							if(iTmpLow_ST2 > ST_LIMIT_LOW){
								iTmpLow_ST2 -= ST_ADJUST_INTER;
								SetSTLimitText(hWnd, iTmpLow_ST2);			
							}
						}break;
						
					}
				}break;
				case SCANCODE_ENTER:{
					switch(id){
						case DID_NEWECGALM_HIGH_HR_EDIT:{
							gCfgEcg.wHigh_HR = iTmpHigh_HR;
							SetIntValueToResFile(gFileSetup, "ECGSetup", "hr_high",  gCfgEcg.wHigh_HR, 3);
							gCfgPulse.wHigh_PR = iTmpHigh_HR;
							SetIntValueToResFile(gFileSetup, "PULSESetup", "pr_high",  gCfgPulse.wHigh_PR, 3);
							SetFocus(btnHigh_HR);
						}break;
						case DID_NEWECGALM_LOW_HR_EDIT:{
							gCfgEcg.wLow_HR = iTmpLow_HR;
							SetIntValueToResFile(gFileSetup, "ECGSetup", "hr_low",  gCfgEcg.wLow_HR, 3);
							gCfgPulse.wLow_PR = iTmpLow_HR;
							SetIntValueToResFile(gFileSetup, "PULSESetup", "pr_low",  gCfgPulse.wLow_PR, 3);
							SetFocus(btnLow_HR);
						}break;
						case DID_NEWECGALM_HIGH_ST1_EDIT:{
 							gCfgEcg.iSt1_High = iTmpHigh_ST1;
 							SetIntValueToResFile(gFileSetup, "ECGSetup", "st1_high",  gCfgEcg.iSt1_High,5);
 							SetFocus(btnHigh_ST1);
						}break;
						case DID_NEWECGALM_LOW_ST1_EDIT:{
 							gCfgEcg.iSt1_Low = iTmpLow_ST1 ;
 							SetIntValueToResFile(gFileSetup, "ECGSetup", "st1_low",  gCfgEcg.iSt1_Low,5);
 							SetFocus(btnLow_ST1);
						}break;
						case DID_NEWECGALM_HIGH_ST2_EDIT:{
 							gCfgEcg.iSt2_High = iTmpHigh_ST2;
 							SetIntValueToResFile(gFileSetup, "ECGSetup", "st2_high",  gCfgEcg.iSt2_High,5);
 							SetFocus(btnHigh_ST2);
						}break;
						case DID_NEWECGALM_LOW_ST2_EDIT:{
 							gCfgEcg.iSt2_Low = iTmpLow_ST2 ;
 							SetIntValueToResFile(gFileSetup, "ECGSetup", "st2_low",  gCfgEcg.iSt2_Low,5);
 							SetFocus(btnLow_ST2);
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
					     DID_NEWECGALM_CAPTION, 0, 5, 240, 25, hDlg, 0);
	editHelp = GetDlgItem(hDlg, DID_NEWECGALM_HELP);

	btnAlm= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECGALM_ALM, 10, 30+30*0, 125, 26, hDlg, 0);
	btnAlmPrint= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECGALM_ALMPRINT, 10, 30+30*1, 125, 26, hDlg, 0);
	btnHigh_HR= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECGALM_HIGH_HR, 10, 30+30*2, 125, 26, hDlg, 0);
	btnLow_HR= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECGALM_LOW_HR, 10, 30+30*3, 125, 26, hDlg, 0);
	btnHigh_ST1= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECGALM_HIGH_ST1, 10, 30+30*4, 125, 26, hDlg, 0);
	btnLow_ST1= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECGALM_LOW_ST1, 10, 30+30*5, 125, 26, hDlg, 0);
	btnHigh_ST2= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECGALM_HIGH_ST2, 10, 30+30*6, 125, 26, hDlg, 0);
	btnLow_ST2= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECGALM_LOW_ST2, 10, 30+30*7, 125, 26, hDlg, 0);

	btnOk = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECGALM_OK, 10, 30+30*8, 125, 26, hDlg, 0);

	cobAlm = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_NEWECGALM_ALM_COB, 140, 30+30*0, 95, 26, hDlg, 0);
	cobAlmPrint= CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_NEWECGALM_ALMPRINT_COB, 140, 30+30*1, 95, 26, hDlg, 0);
	editHigh_HR = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_NEWECGALM_HIGH_HR_EDIT, 140, 30+30*2, 95, 23, hDlg, 0);
	editLow_HR = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_NEWECGALM_LOW_HR_EDIT, 140, 30+30*3, 95, 23, hDlg, 0);
	editHigh_ST1 = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_NEWECGALM_HIGH_ST1_EDIT, 140, 30+30*4, 95, 23, hDlg, 0);
	editLow_ST1  = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_NEWECGALM_LOW_ST1_EDIT, 140, 30+30*5, 95, 23, hDlg, 0);
	editHigh_ST2  = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_NEWECGALM_HIGH_ST2_EDIT, 140, 30+30*6, 95, 23, hDlg, 0);
	editLow_ST2 = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_NEWECGALM_LOW_ST2_EDIT, 140, 30+30*7, 95, 23, hDlg, 0);
	
	return 0;
}
static int InitCtlName(HWND hDlg)
{
	char strMenu[100];

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGALM, "caption", strMenu, sizeof strMenu,"Ecg Alarm");
	SetWindowText(editCaption, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "previous", strMenu, sizeof strMenu,"Previous");	
	SetWindowText(btnOk, strMenu);

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "alarm_class", strMenu, sizeof strMenu,"Alm Class");	
	SetWindowText(btnAlm, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "alarm_print", strMenu, sizeof strMenu,"Alm Print");	
	SetWindowText(btnAlmPrint, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGALM, "hr_high", strMenu, sizeof strMenu,"HR High");	
	SetWindowText(btnHigh_HR, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGALM, "hr_low", strMenu, sizeof strMenu,"HR Low");	
	SetWindowText(btnLow_HR, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGALM, "st1_high", strMenu, sizeof strMenu,"ST1 High");	
	SetWindowText(btnHigh_ST1, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGALM, "st1_low", strMenu, sizeof strMenu,"ST1 Low");	
	SetWindowText(btnLow_ST1, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGALM, "st2_high", strMenu, sizeof strMenu,"ST2 High");	
	SetWindowText(btnHigh_ST2, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGALM, "st2_low", strMenu, sizeof strMenu,"ST2 Low");	
	SetWindowText(btnLow_ST2, strMenu);
	return 0;
}

static int InitCobList(HWND hDlg)
{
	int i;
	char strOFF[50];
	char strON[50];
	char strMenu[50];
	
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "off", strMenu, sizeof strMenu,"OFF");	
	SendMessage(cobAlm, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "low", strMenu, sizeof strMenu,"Low");	
	SendMessage(cobAlm, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "middle", strMenu, sizeof strMenu,"Middle");	
	SendMessage(cobAlm, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "high", strMenu, sizeof strMenu,"High");	
	SendMessage(cobAlm, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobAlm, CB_SETCURSEL, gCfgEcg.bAlmControl, 0);

	memset(strOFF, 0, sizeof strOFF);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "off", strOFF, sizeof strOFF,"OFF");
	memset(strON, 0, sizeof strON);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "on", strON, sizeof strON,"ON");

	SendMessage(cobAlmPrint, CB_ADDSTRING, 0, (LPARAM)strOFF);
	SendMessage(cobAlmPrint, CB_ADDSTRING, 0, (LPARAM)strON);
	SendMessage(cobAlmPrint, CB_SETCURSEL, gCfgEcg.bPrnControl, 0);
	
	//获得限值
	iTmpHigh_HR = gCfgEcg.wHigh_HR;
	iTmpLow_HR = gCfgEcg.wLow_HR;
	SetLimitText(editHigh_HR, gCfgEcg.wHigh_HR);
	SetLimitText(editLow_HR, gCfgEcg.wLow_HR);
		//获得限值
	iTmpHigh_ST1 = gCfgEcg.iSt1_High;
	iTmpLow_ST1 = gCfgEcg.iSt1_Low;
	SetSTLimitText(editHigh_ST1, gCfgEcg.iSt1_High);
	SetSTLimitText(editLow_ST1, gCfgEcg.iSt1_Low);
		//获得限值
	iTmpHigh_ST2 = gCfgEcg.iSt2_High;
	iTmpLow_ST2 = gCfgEcg.iSt2_Low;
	SetSTLimitText(editHigh_ST2, gCfgEcg.iSt2_High);
	SetSTLimitText(editLow_ST2, gCfgEcg.iSt2_Low);
	return 0;
}
static int InitCtlCallBackProc(HWND hDlg)
{

	OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnAlm, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnAlmPrint, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnHigh_HR, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnLow_HR, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnHigh_ST1, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnLow_ST1, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnHigh_ST2, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnLow_ST2, BtnProc);
	OldCobProc = SetWindowCallbackProc(cobAlm, CobProc);
	OldCobProc = SetWindowCallbackProc(cobAlmPrint, CobProc);
	
	OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp);
	
	OldEditProc = SetWindowCallbackProc(editHigh_HR, EditProc);
	OldEditProc = SetWindowCallbackProc(editLow_HR, EditProc);
	OldEditProc = SetWindowCallbackProc(editHigh_ST1, EditProc);
	OldEditProc = SetWindowCallbackProc(editLow_ST1, EditProc);
	OldEditProc = SetWindowCallbackProc(editHigh_ST2, EditProc);
	OldEditProc = SetWindowCallbackProc(editLow_ST2, EditProc);
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
	HDC hdc;
	int temp;
	char strMenu[100];
	int i;
	RECT rcDlg;
	RECT rcCaption;
	switch(message){
		case MSG_INITDIALOG:{
			//创建控件
			CreateCtlProc(hDlg);
			//设置控件字符
			InitCtlName(hDlg);
			//初始化列表框
			InitCobList(hDlg);
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
			int	id  = LOWORD(wParam);
			switch(id){
				case DID_NEWECGALM_ALM:{
					gbKeyType = KEY_UD;
					SetFocus(cobAlm);
					}break;
				case DID_NEWECGALM_ALMPRINT:{
					gbKeyType = KEY_UD;
					SetFocus(cobAlmPrint);
					}break;
				case DID_NEWECGALM_HIGH_HR:{
					gbKeyType = KEY_UD;
					SetFocus(editHigh_HR);
					}break;
				case DID_NEWECGALM_LOW_HR:{
					gbKeyType = KEY_UD;
					SetFocus(editLow_HR);
					}break;
				case DID_NEWECGALM_HIGH_ST1:{
					gbKeyType = KEY_UD;
					SetFocus(editHigh_ST1);
					}break;
				case DID_NEWECGALM_LOW_ST1:{
					gbKeyType = KEY_UD;
					SetFocus(editLow_ST1);
					}break;
				case DID_NEWECGALM_HIGH_ST2:{
					gbKeyType = KEY_UD;
					SetFocus(editHigh_ST2);
					}break;
				case DID_NEWECGALM_LOW_ST2:{
					gbKeyType = KEY_UD;
					SetFocus(editLow_ST2);
					}break;
				case DID_NEWECGALM_OK:{
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
void CreateDlgNewECGALM(HWND hWnd)
{
 	DlgSet.controls = CtrlSet;
 	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}


/*************************************************************************** 
 *	Module Name:	DlgSet_CO2Set
 *
 *	Abstract:	设置CO2配置信息的对话框
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-10-06 15:39:20
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"
#include "General_Funcs.h"

#define DID_CO2SET	6400
#define DID_CO2SET_HELP				DID_CO2SET
#define DID_CO2SET_OK				DID_CO2SET+1
#define DID_CO2SET_ALMCLASS			DID_CO2SET+2
#define DID_CO2SET_SET_ALMCLASS		DID_CO2SET+3
#define DID_CO2SET_ALMPRINT			DID_CO2SET+4
#define DID_CO2SET_SET_ALMPRINT		DID_CO2SET+5
#define DID_CO2SET_HIGH_ETCO2		DID_CO2SET+6
#define DID_CO2SET_SET_HIGH_ETCO2	DID_CO2SET+7
#define DID_CO2SET_LOW_ETCO2		DID_CO2SET+8
#define DID_CO2SET_SET_LOW_ETCO2	DID_CO2SET+9
#define DID_CO2SET_HIGH_FICO2		DID_CO2SET+10
#define DID_CO2SET_SET_HIGH_FICO2	DID_CO2SET+11
#define DID_CO2SET_HIGH_RR			DID_CO2SET+12
#define DID_CO2SET_SET_HIGH_RR		DID_CO2SET+13
#define DID_CO2SET_LOW_RR			DID_CO2SET+14
#define DID_CO2SET_SET_LOW_RR		DID_CO2SET+15
#define DID_CO2SET_APNEA			DID_CO2SET+16
#define DID_CO2SET_SET_APNEA		DID_CO2SET+17
#define DID_CO2SET_CAPTION			DID_CO2SET+18
//帮助对话框
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;
//其他控件
static HWND btnOk;
static HWND btnSetAlmClass;
static HWND cobAlmClass;
static HWND btnSetAlmPrint;
static HWND cobAlmPrint;
static HWND btnSetHigh_EtCO2;
static HWND editHigh_EtCO2;
static HWND btnSetLow_EtCO2;
static HWND editLow_EtCO2;
static HWND btnSetHigh_FiCO2;
static HWND editHigh_FiCO2;
static HWND btnSetHigh_RR;
static HWND editHigh_RR;
static HWND btnSetLow_RR;
static HWND editLow_RR;
static HWND btnSetApnea;
static HWND editApnea;

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
		//WS_CHILD | WS_VISIBLE | SS_LEFT|SS_WHITERECT ,
		5,DLG_HIGH-50, DLG_LENGHT-15, 40,
		DID_CO2SET_HELP,
		"",
		0
	}
};

//临时存储上下限，调节的时候使用
static int iTmpHigh_EtCO2 =0, iTmpLow_EtCO2=0;
static int iTmpHigh_RR =0, iTmpLow_RR=0;
static int iTmpHigh_FiCO2 =0;
static int iTmpApnea  =0;

//上下限刻度调整的增量
#define CO2_LIMIT_INTER		1	//mmHg
#define RR_LIMIT_INTER		1	//rpm	
#define APNEA_LIMIT_INTER	1	//Sec	

//限值
#define CO2_LIMIT_MAX		CO2_MAX	//mmHg
#define CO2_LIMIT_MIN		CO2_MIN		//mmHg
#define RR_LIMIT_MAX			RR_MAX		//rpm
#define RR_LIMIT_MIN			RR_MIN		//rpm
#define APNEA_LIMIT_MAX		60			//Sec
#define APNEA_LIMIT_MIN		0			//Sec	

//上下限值最小幅度
#define CO2_LIMITRANGE_MIN 	1
#define RR_LIMITRANGE_MIN 	1


//改变参数的报警级别
static int ChangeAlarmClass()
{
	IsAlm_EtCO2(&gValueCO2, &gCfgCO2, TRUE);
	IsAlm_FiCO2(&gValueCO2, &gCfgCO2, TRUE);
	
	return 0;
}

/*
	根据单位设置上下限
	bType: 0-CO2, 1-RR, 2-Apnea
*/
static int SetLimitText(BOOL bUnit, HWND hWnd, int iLimit, BYTE bType)
{
	unsigned char strLimit[50] = {0};
	unsigned char strUnit[20] = {0};
	
	if(bType ==0){
		switch(bUnit){
			case CO2UNIT_PER:{
				memset(strUnit, 0, sizeof strUnit);
				GetStringFromResFile(gsLanguageRes, "GENERAL", "unit_per", strUnit, sizeof strUnit);		
				memset(strLimit, 0, sizeof strLimit);
				snprintf(strLimit, sizeof strLimit, "%3.1f %s", (float)(iLimit / 7.6), strUnit);
				SetWindowText(hWnd, strLimit);
			}break;
			default:{
			//CO2UNIT_MMHG
				memset(strUnit, 0, sizeof strUnit);
				GetStringFromResFile(gsLanguageRes, "GENERAL", "unit_mmhg", strUnit, sizeof strUnit);		
				memset(strLimit, 0, sizeof strLimit);
				snprintf(strLimit, sizeof strLimit, "%d %s", iLimit, strUnit);
				SetWindowText(hWnd, strLimit);
			}break;
		}
	}
	else if(bType ==1){
		memset(strUnit, 0, sizeof strUnit);
		GetStringFromResFile(gsLanguageRes, "GENERAL", "unit_rpm", strUnit, sizeof strUnit);		
		memset(strLimit, 0, sizeof strLimit);
		snprintf(strLimit, sizeof strLimit, "%d %s", iLimit, strUnit);
		SetWindowText(hWnd, strLimit);
	}
	else{
		memset(strUnit, 0, sizeof strUnit);
		GetStringFromResFile(gsLanguageRes, "GENERAL", "unit_second", strUnit, sizeof strUnit);		
		memset(strLimit, 0, sizeof strLimit);
		snprintf(strLimit, sizeof strLimit, "%d %s", iLimit, strUnit);
		SetWindowText(hWnd, strLimit);
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
				case DID_CO2SET_SET_ALMCLASS:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_alarm_class", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_CO2SET_SET_ALMPRINT:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_alarm_print", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_CO2SET_SET_HIGH_ETCO2:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_co2_high_etco2", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_CO2SET_SET_LOW_ETCO2:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_co2_low_etco2", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_CO2SET_SET_HIGH_FICO2:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_co2_high_fico2", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_CO2SET_OK:{
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
						case DID_CO2SET_ALMCLASS:{
							gCfgCO2.bAlmControl = index;
							SetIntValueToResFile(gFileSetup, "CO2Setup", "alarm_control",  gCfgCO2.bAlmControl, 1);
							SetFocus(btnSetAlmClass);
							//改变参数的报警级别
							ChangeAlarmClass();
						}break;
						case DID_CO2SET_ALMPRINT:{
							gCfgCO2.bPrnControl = index;
							SetIntValueToResFile(gFileSetup, "CO2Setup", "alarm_print",  gCfgCO2.bPrnControl, 1);
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
 						case DID_CO2SET_HIGH_ETCO2:{
 							if(iTmpHigh_EtCO2 <CO2_LIMIT_MAX){
								iTmpHigh_EtCO2 += CO2_LIMIT_INTER;
								SetLimitText(gCfgCO2.bUnit, hWnd, iTmpHigh_EtCO2, 0);			
							}
						}break;
						case DID_CO2SET_LOW_ETCO2:{
							if(iTmpLow_EtCO2 < (iTmpHigh_EtCO2 - CO2_LIMITRANGE_MIN)){
								iTmpLow_EtCO2 += CO2_LIMIT_INTER;
								SetLimitText(gCfgCO2.bUnit, hWnd, iTmpLow_EtCO2, 0);			
							}
						}break;
						case DID_CO2SET_HIGH_FICO2:{
							if(iTmpHigh_FiCO2 < CO2_LIMIT_MAX){
								iTmpHigh_FiCO2 += CO2_LIMIT_INTER;
								SetLimitText(gCfgCO2.bUnit, hWnd, iTmpHigh_FiCO2, 0);			
							}
						}break;
						case DID_CO2SET_HIGH_RR:{
							if(iTmpHigh_RR <RR_LIMIT_MAX){
								iTmpHigh_RR += RR_LIMIT_INTER;
								SetLimitText(gCfgCO2.bUnit, hWnd, iTmpHigh_RR, 1);			
							}
						}break;
						case DID_CO2SET_LOW_RR:{
							if(iTmpLow_RR < (iTmpHigh_RR - RR_LIMITRANGE_MIN)){
								iTmpLow_RR += RR_LIMIT_INTER;
								SetLimitText(gCfgCO2.bUnit, hWnd, iTmpLow_RR, 1);			
							}
						}break;
						case DID_CO2SET_APNEA:{
							if(iTmpApnea <APNEA_LIMIT_MAX){
								iTmpApnea += APNEA_LIMIT_INTER;
								SetLimitText(gCfgCO2.bUnit, hWnd, iTmpApnea, 2);			
							}
						}break;
					}
				}break;
				case SCANCODE_CURSORBLOCKUP:{
					switch(id){
						case DID_CO2SET_HIGH_ETCO2:{
							if(iTmpHigh_EtCO2 > (iTmpLow_EtCO2 + CO2_LIMITRANGE_MIN)){
								iTmpHigh_EtCO2 -= CO2_LIMIT_INTER;
								SetLimitText(gCfgCO2.bUnit, hWnd, iTmpHigh_EtCO2, 0);			
							}
						}break;
						case DID_CO2SET_LOW_ETCO2:{
							if(iTmpLow_EtCO2 > CO2_LIMIT_MIN){
								iTmpLow_EtCO2 -= CO2_LIMIT_INTER;
								SetLimitText(gCfgCO2.bUnit, hWnd, iTmpLow_EtCO2, 0);			
							}
						}break;
						case DID_CO2SET_HIGH_FICO2:{
							if(iTmpHigh_FiCO2 > CO2_LIMIT_MIN){
								iTmpHigh_FiCO2 -= CO2_LIMIT_INTER;
								SetLimitText(gCfgCO2.bUnit, hWnd, iTmpHigh_FiCO2, 0);			
							}
						}break;
						case DID_CO2SET_HIGH_RR:{
							if(iTmpHigh_RR > (iTmpLow_RR + RR_LIMITRANGE_MIN)){
								iTmpHigh_RR -= RR_LIMIT_INTER;
								SetLimitText(gCfgCO2.bUnit, hWnd, iTmpHigh_RR, 1);			
							}
						}break;
						case DID_CO2SET_LOW_RR:{
							if(iTmpLow_RR >RR_LIMIT_MIN){
								iTmpLow_RR -= RR_LIMIT_INTER;
								SetLimitText(gCfgCO2.bUnit, hWnd, iTmpLow_RR, 1);			
							}
						}break;
						case DID_CO2SET_APNEA:{
							if(iTmpApnea > APNEA_LIMIT_MIN){
								iTmpApnea -= APNEA_LIMIT_INTER;
								SetLimitText(gCfgCO2.bUnit, hWnd, iTmpApnea, 2);			
							}
						}break;
					}
				}break;
				case SCANCODE_ENTER:{
					switch(id){
						case DID_CO2SET_HIGH_ETCO2:{
							gCfgCO2.wHigh_EtCO2= iTmpHigh_EtCO2;
							SetIntValueToResFile(gFileSetup, "CO2Setup", "etco2_high",  gCfgCO2.wHigh_EtCO2, 4);
							SetFocus(btnSetHigh_EtCO2);
						}break;
						case DID_CO2SET_LOW_ETCO2:{
							gCfgCO2.wLow_EtCO2 = iTmpLow_EtCO2;
							SetIntValueToResFile(gFileSetup, "CO2Setup", "etco2_low",  gCfgCO2.wLow_EtCO2, 4);
							SetFocus(btnSetLow_EtCO2);
						}break;
						case DID_CO2SET_HIGH_FICO2:{
							gCfgCO2.wHigh_FiCO2 = iTmpHigh_FiCO2;
							SetIntValueToResFile(gFileSetup, "CO2Setup", "fico2_high",  gCfgCO2.wHigh_FiCO2, 4);
							SetFocus(btnSetHigh_FiCO2);
						}break;
						case DID_CO2SET_HIGH_RR:{
							gCfgCO2.wHigh_awRR = iTmpHigh_RR;
							SetIntValueToResFile(gFileSetup, "CO2Setup", "awrr_high",  gCfgCO2.wHigh_awRR, 4);
							SetFocus(btnSetHigh_RR);
						}break;
						case DID_CO2SET_LOW_RR:{
							gCfgCO2.wLow_awRR = iTmpLow_RR;
							SetIntValueToResFile(gFileSetup, "CO2Setup", "awrr_low",  gCfgCO2.wLow_awRR, 4);
							SetFocus(btnSetLow_RR);
						}break;
						case DID_CO2SET_APNEA:{
							gCfgCO2.wApneaTime = iTmpApnea;
							SetIntValueToResFile(gFileSetup, "CO2Setup", "apnea_time",  gCfgCO2.wApneaTime, 4);
							SetFocus(btnSetApnea);
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
static int  CreateCtlProc(HWND hDlg)
{
		editHelp = GetDlgItem(hDlg, DID_CO2SET_HELP);
		editCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER |  WS_VISIBLE, 
					     DID_CO2SET_CAPTION, 0, 5, 240, 25, hDlg, 0);
		btnSetAlmClass = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
				DID_CO2SET_SET_ALMCLASS, 10, 30+30*0, 125, 26, hDlg, 0);
		btnSetAlmPrint = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
				DID_CO2SET_SET_ALMPRINT, 10, 30+30*1, 125, 26, hDlg, 0);
		btnSetHigh_EtCO2 = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
				DID_CO2SET_SET_HIGH_ETCO2, 10, 30+30*2, 125, 26, hDlg, 0);
		btnSetLow_EtCO2 = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
				DID_CO2SET_SET_LOW_ETCO2, 10, 30+30*3, 125, 26, hDlg, 0);
		btnSetHigh_FiCO2= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
				DID_CO2SET_SET_HIGH_FICO2, 10, 30+30*4, 125, 26, hDlg, 0);
		btnOk= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
				    DID_CO2SET_OK, 10, 30+30*5, 125, 26, hDlg, 0);
		
		cobAlmClass = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST, 
				DID_CO2SET_ALMCLASS, 140, 30+30*0, 95, 26, hDlg, 0);
		cobAlmPrint = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST, 
				DID_CO2SET_ALMPRINT, 140, 30+30*1, 95, 26, hDlg, 0);
		editHigh_EtCO2 = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
				DID_CO2SET_HIGH_ETCO2, 140, 30+30*2, 95, 23, hDlg, 0);
		editLow_EtCO2 = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
				DID_CO2SET_LOW_ETCO2, 140, 30+30*3, 95, 23, hDlg, 0);
		editHigh_FiCO2 = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
				DID_CO2SET_HIGH_FICO2, 140, 30+30*4, 95, 23, hDlg, 0);
	

	return 0;
}
//设置控件字符
static int InitCtlName(HWND hDlg)
{
	char strMenu[100];
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, "CO2", "caption1", strMenu, sizeof strMenu,"CO2 Alarm");
	SetWindowText(editCaption, strMenu);
	//Alarm Class
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "GENERAL", "alarm_class", strMenu, sizeof strMenu);	
	SetWindowText(btnSetAlmClass, strMenu);
	//Alarm Print
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "GENERAL", "alarm_print", strMenu, sizeof strMenu);	
	SetWindowText(btnSetAlmPrint, strMenu);
	//High Limit Of EtCO2
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "CO2", "high_etco2", strMenu, sizeof strMenu);	
	SetWindowText(btnSetHigh_EtCO2, strMenu);
	//Low Limit Of EtCO2
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "CO2", "low_etco2", strMenu, sizeof strMenu);	
	SetWindowText(btnSetLow_EtCO2, strMenu);
	//High Limit Of FiCO2
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "CO2", "high_fico2", strMenu, sizeof strMenu);
	SetWindowText(btnSetHigh_FiCO2, strMenu);
	//Exit
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "GENERAL", "exit", strMenu, sizeof strMenu);	
	SetWindowText(btnOk, strMenu);
	return 0;
}
//初始化列表框
static int InitCobList(HWND hDlg)
{	
	char strMenu[100];
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "GENERAL", "off", strMenu, sizeof strMenu);	
	SendMessage(cobAlmClass, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "GENERAL", "low", strMenu, sizeof strMenu);	
	SendMessage(cobAlmClass, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "GENERAL", "middle", strMenu, sizeof strMenu);	
	SendMessage(cobAlmClass, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "GENERAL", "high", strMenu, sizeof strMenu);	
	SendMessage(cobAlmClass, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobAlmClass, CB_SETCURSEL, gCfgCO2.bAlmControl, 0);

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "GENERAL", "off", strMenu, sizeof strMenu);	
	SendMessage(cobAlmPrint, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "GENERAL", "on", strMenu, sizeof strMenu);	
	SendMessage(cobAlmPrint, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobAlmPrint, CB_SETCURSEL, gCfgCO2.bPrnControl, 0);
	
	SetLimitText(gCfgCO2.bUnit, editHigh_EtCO2, gCfgCO2.wHigh_EtCO2, 0);
	SetLimitText(gCfgCO2.bUnit, editLow_EtCO2, gCfgCO2.wLow_EtCO2, 0);		
	SetLimitText(gCfgCO2.bUnit, editHigh_FiCO2, gCfgCO2.wHigh_FiCO2, 0);
	
	return 0;
}
//设置控件回调函数
static int InitCtlCallBackProc(HWND hDlg)
{
	OldBtnProc = SetWindowCallbackProc(btnSetAlmClass, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSetAlmPrint, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSetHigh_EtCO2, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSetLow_EtCO2, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSetHigh_FiCO2, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);
	
	OldCobProc = SetWindowCallbackProc(cobAlmClass, CobProc);
	OldCobProc = SetWindowCallbackProc(cobAlmPrint, CobProc);
	
	OldEditProc = SetWindowCallbackProc(editHigh_EtCO2, EditProc);
	OldEditProc = SetWindowCallbackProc(editLow_EtCO2, EditProc);
	OldEditProc = SetWindowCallbackProc(editHigh_FiCO2, EditProc);
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
			//获得限值
			iTmpHigh_EtCO2 = gCfgCO2.wHigh_EtCO2;
			iTmpHigh_FiCO2 = gCfgCO2.wHigh_FiCO2;
		//	iTmpHigh_RR  = gCfgCO2.wHigh_awRR;
			iTmpLow_EtCO2 = gCfgCO2.wLow_EtCO2;
		//	iTmpLow_RR  = gCfgCO2.wLow_awRR;
		//	iTmpApnea = gCfgCO2.wApneaTime;
			
			//创建控件
			CreateCtlProc(hDlg);
			//设置控件字符
			InitCtlName(hDlg);
			//初始化列表框
			InitCobList(hDlg);
			//设置控件回调函数
			InitCtlCallBackProc(hDlg);
			
			gbKeyType = KEY_TAB;
			
			SetFocus(btnOk);
					
		}break;
		case MSG_PAINT:{

		}break;
		case MSG_COMMAND:{
			int	id   = LOWORD(wParam);
			
			switch(id){
				case DID_CO2SET_SET_ALMCLASS:{
					gbKeyType = KEY_UD;
					SetFocus(cobAlmClass);
				}break;
				case DID_CO2SET_SET_ALMPRINT:{
					gbKeyType = KEY_UD;
					SetFocus(cobAlmPrint);
				}break;
				case DID_CO2SET_SET_HIGH_ETCO2:{
					gbKeyType = KEY_UD;
					SetFocus(editHigh_EtCO2);
				}break;
				case DID_CO2SET_SET_LOW_ETCO2:{
					gbKeyType = KEY_UD;
					SetFocus(editLow_EtCO2);
				}break;
				case DID_CO2SET_SET_HIGH_FICO2:{
					gbKeyType = KEY_UD;
					SetFocus(editHigh_FiCO2);
				}break;
				case DID_CO2SET_SET_HIGH_RR:{
					gbKeyType = KEY_UD;
					SetFocus(editHigh_RR);
				}break;
				case DID_CO2SET_SET_LOW_RR:{
					gbKeyType = KEY_UD;
					SetFocus(editLow_RR);
				}break;
				case DID_CO2SET_SET_APNEA:{
					gbKeyType = KEY_UD;
					SetFocus(editApnea);
				}break;
				case DID_CO2SET_OK:{
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
void CreateDlgCO2Set(HWND hWnd)
{
	DlgSet.controls = CtrlSet;
	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}


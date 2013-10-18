/*************************************************************************** 
 *	Module Name:	DlgSet_CO2
 *
 *	Abstract:	设置CO2配置信息的主对话框
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-09-24 10:01:01
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"


#define DID_CO2_HELP		DID_CO2
#define DID_CO2_OK			DID_CO2+1
#define DID_CO2_SPEED		DID_CO2+2
#define DID_CO2_SET_SPEED	DID_CO2+3
#define DID_CO2_RANGE		DID_CO2+4
#define DID_CO2_SET_RANGE	DID_CO2+5
#define DID_CO2_UNIT			DID_CO2+6
#define DID_CO2_SET_UNIT	DID_CO2+7
#define DID_CO2_RATE		DID_CO2+8
#define DID_CO2_SET_RATE	DID_CO2+9
#define DID_CO2_PUMP		DID_CO2+10
#define DID_CO2_SET_ALARM	DID_CO2+11
#define DID_CO2_SET_RESP	DID_CO2+12
#define DID_CO2_CAPTION		DID_CO2+13

//帮助对话框
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;
//其他控件
static HWND btnOk;
static HWND btnSetSpeed, cobSpeed;
static HWND btnSetRange, cobRange;
static HWND btnSetUnit, cobUnit;
static HWND btnSetRate, cobRate;
static HWND btnPump;
static HWND btnSetAlarm;
static HWND btnSetResp;


//控件回调函数
static WNDPROC OldBtnProc;	
static WNDPROC OldCobProc;


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
		DID_CO2_HELP,
		"",
		0
	}
};


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
	SetWindowElementColorEx( hWnd,  FGC_CONTROL_NORMAL,COLOR_yellow);
#endif
	switch(message){
		case MSG_PAINT:{
			id = lParam;
			switch(id){
				case DID_CO2_OK:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_exit", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_CO2_SET_SPEED:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_co2_speed", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_CO2_SET_RANGE:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_co2_range", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_CO2_SET_RATE:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_co2_rate", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_CO2_SET_UNIT:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_co2_unit", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_CO2_PUMP:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_co2_pump", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_CO2_SET_ALARM:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_co2_set_alarm", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_CO2_SET_RESP:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_co2_set_rr", strHelp, sizeof strHelp);	
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
 						case DID_CO2_SPEED:{
 							gCfgCO2.bScanSpeed = index;
							SetIntValueToResFile(gFileSetup, "CO2Setup", "scan_speed",  gCfgCO2.bScanSpeed, 1);
							gCfgResp.bScanSpeed = index;
							SetIntValueToResFile(gFileSetup, "RESPSetup", "scan_speed",  gCfgResp.bScanSpeed, 1);
 							SetFocus(btnSetSpeed);
							ChangeWaveSpeed_CO2();
							ChangeWaveSpeed_Resp();
 						}break;
						case DID_CO2_UNIT:{
							gCfgCO2.bUnit = index;
							SetIntValueToResFile(gFileSetup, "CO2Setup", "unit",  gCfgCO2.bUnit, 1);
							SetFocus(btnSetUnit);
						}break;
						case DID_CO2_RATE:{
							gCfgCO2.bRate = index;
							SetIntValueToResFile(gFileSetup, "CO2Setup", "rate",  gCfgCO2.bRate, 1);
							SetFocus(btnSetRate);
							
							//设置气体采样速度
						//	CPTCmd_SetFlowrate();
						}break;
						case DID_CO2_RANGE:{
							gCfgCO2.bRange = index;
							SetIntValueToResFile(gFileSetup, "CO2Setup", "range",  gCfgCO2.bRange, 1);
							SetFocus(btnSetRange);
							
							//改变CO2波形幅度
// 							ChangeWaveRange_CO2();
							ChangeWaveGain_CO2();
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

static int CreateCtlProc(HWND hDlg)
{
	editCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER |  WS_VISIBLE, 
					     DID_CO2_CAPTION, 0, 5, 240, 25, hDlg, 0);
	editHelp = GetDlgItem(hDlg, DID_CO2_HELP);
	btnSetSpeed= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
				DID_CO2_SET_SPEED, 10, 30+30*0, 125, 26, hDlg, 0);
	btnSetRange = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
			DID_CO2_SET_RANGE, 10, 30+30*1, 125, 26, hDlg, 0);
	btnSetUnit = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
			DID_CO2_SET_UNIT, 10, 30+30*2, 125, 26, hDlg, 0);
	btnSetRate = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
			DID_CO2_SET_RATE, 10, 30+30*3, 125, 26, hDlg, 0);
	btnPump = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
			DID_CO2_PUMP, 10, 30+30*4, 125, 26, hDlg, 0);
	btnSetAlarm = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
			DID_CO2_SET_ALARM, 10, 30+30*5, 125, 26, hDlg, 0);

	btnSetResp = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_CO2_SET_RESP, 10, 30+30*6, 125, 26, hDlg, 0);
	btnOk= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
			    DID_CO2_OK, 10, 30+30*7, 125, 26, hDlg, 0);
	
	cobSpeed = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST, 
			DID_CO2_SPEED, 140, 30+30*0, 95, 26, hDlg, 0);
	cobRange = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST, 
			DID_CO2_RANGE, 140, 30+30*1, 95, 26, hDlg, 0);
	cobUnit = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST, 
			DID_CO2_UNIT, 140, 30+30*2, 95, 26, hDlg, 0);
	cobRate = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST, 
			DID_CO2_RATE, 140, 30+30*3, 95, 26, hDlg, 0);
	return 0;
}
static int InitCtlName(HWND hDlg)
{
	char strMenu[100];
	
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "CO2", "caption", strMenu, sizeof strMenu,"CO2");	
	SetWindowText(editCaption, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "CO2", "speed", strMenu, sizeof strMenu);	
	SetWindowText(btnSetSpeed, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "CO2", "range", strMenu, sizeof strMenu);	
	SetWindowText(btnSetRange, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "CO2", "unit", strMenu, sizeof strMenu);	
	SetWindowText(btnSetUnit, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "CO2", "rate", strMenu, sizeof strMenu);	
	SetWindowText(btnSetRate, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "CO2", "pump", strMenu, sizeof strMenu);	
	SetWindowText(btnPump, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "CO2", "alarm", strMenu, sizeof strMenu);	
	SetWindowText(btnSetAlarm, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "CO2", "set_resp", strMenu, sizeof strMenu);	
	SetWindowText(btnSetResp, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "GENERAL", "exit", strMenu, sizeof strMenu);	
	SetWindowText(btnOk, strMenu);
			
	
	return 0;
}
static int InitCobList(HWND hDlg)
{
	int i;
	char strMenu[100];
	unsigned char *strFlowrate[]={"50cc/min", "75cc/min", "100cc/min", "150cc/min", "200cc/min"};


	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "GENERAL", "speed_625", strMenu, sizeof strMenu);	
	SendMessage(cobSpeed, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "GENERAL", "speed_125", strMenu, sizeof strMenu);	
	SendMessage(cobSpeed, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "GENERAL", "speed_25", strMenu, sizeof strMenu);	
	SendMessage(cobSpeed, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobSpeed, CB_SETCURSEL, gCfgCO2.bScanSpeed, 0);

	
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "CO2", "range_big", strMenu, sizeof strMenu);	
	SendMessage(cobRange, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "CO2", "range_middle", strMenu, sizeof strMenu);	
	SendMessage(cobRange, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "CO2", "range_small", strMenu, sizeof strMenu);	
	SendMessage(cobRange, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobRange, CB_SETCURSEL, gCfgCO2.bRange, 0);

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "GENERAL", "unit_mmhg", strMenu, sizeof strMenu);	
	SendMessage(cobUnit, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "GENERAL", "unit_per", strMenu, sizeof strMenu);	
	SendMessage(cobUnit, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobUnit, CB_SETCURSEL, gCfgCO2.bUnit, 0);

	for(i=0; i<5; i++){
		SendMessage(cobRate, CB_ADDSTRING, 0, (LPARAM)strFlowrate[i]);
	}
	SendMessage(cobRate, CB_SETCURSEL, gCfgCO2.bRate, 0);

	return 0;
}
static int InitCtlCallBackProc(HWND hDlg)
{
	OldBtnProc = SetWindowCallbackProc(btnSetSpeed, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSetRange, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSetUnit, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSetRate, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnPump, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSetAlarm, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSetResp, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);
	
	OldCobProc = SetWindowCallbackProc(cobSpeed, CobProc);
	OldCobProc = SetWindowCallbackProc(cobRange, CobProc);
	OldCobProc = SetWindowCallbackProc(cobUnit, CobProc);
	OldCobProc = SetWindowCallbackProc(cobRate, CobProc);
	

	OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp); 
	return 0;
}
static int InitCtlStatus(HWND hDlg)
{

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
				case DID_CO2_SET_SPEED:{
					gbKeyType = KEY_UD;
					SetFocus(cobSpeed);
				}break;
				case DID_CO2_SET_RANGE:{
					gbKeyType = KEY_UD;
					SetFocus(cobRange);
				}break;
				case DID_CO2_SET_UNIT:{
					gbKeyType = KEY_UD;
					SetFocus(cobUnit);
				}break;
				case DID_CO2_SET_RATE:{
					gbKeyType = KEY_UD;
					SetFocus(cobRate);
				}break;
				case DID_CO2_PUMP:{
					if(gValueCO2.bStatus == CO2_STATUS_STOP){
	  			//		CPTCmd_ActivatedPump();	//启动气泵
//  						CAPCmd_Incoming();
					}
					else{
 				//		CPTCmd_ClosedPump();	//关闭气泵	
//    						CAPCmd_Stop();
					}
				}break;
				case DID_CO2_SET_ALARM:{
					CreateDlgCO2Set(hDlg);
				}break;
				case DID_CO2_SET_RESP:{
 					CreateDlgNewRESPALM(hDlg);
				}break;
				case DID_CO2_OK:{
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
			ChangeWavePanelsLeft((PRECT)NULL);
			
			//向中央机发送信息
			NetSend_CO2Info();
			
			EndDialog(hDlg, wParam);	
		}break;
	}
	
	temp = DefaultDialogProc(hDlg, message, wParam, lParam);
	return(temp);
}

/*
	建立设置对话框
*/
void CreateDlgCO2(HWND hWnd)
{
	DlgSet.controls = CtrlSet;
	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}



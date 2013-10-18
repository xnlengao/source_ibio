/*************************************************************************** 
 *	Module Name:	DlgSet_AlarmPrint
 *
 *	Abstract:	统一设置参数报警控制的对话框
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-07-10 13:56:20
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"

//DID_ALARMPRINT
#define DID_ALARMPRINT_OK				DID_ALARMCTRL
#define DID_ALARMPRINT_ALL_OFF			DID_ALARMCTRL+1
#define DID_ALARMPRINT_ALL_ON			DID_ALARMCTRL+2
#define DID_ALARMPRINT_HR				DID_ALARMCTRL+3
#define DID_ALARMPRINT_SET_HR			DID_ALARMCTRL+4
#define DID_ALARMPRINT_NIBP				DID_ALARMCTRL+5
#define DID_ALARMPRINT_SET_NIBP		DID_ALARMCTRL+6
#define DID_ALARMPRINT_SPO2			DID_ALARMCTRL+7
#define DID_ALARMPRINT_SET_SPO2		DID_ALARMCTRL+9
#define DID_ALARMPRINT_PR				DID_ALARMCTRL+10
#define DID_ALARMPRINT_SET_PR			DID_ALARMCTRL+11
#define DID_ALARMPRINT_RESP			DID_ALARMCTRL+12
#define DID_ALARMPRINT_SET_RESP		DID_ALARMCTRL+13
#define DID_ALARMPRINT_TEMP			DID_ALARMCTRL+14
#define DID_ALARMPRINT_SET_TEMP		DID_ALARMCTRL+15
#define DID_ALARMPRINT_CAPTION			DID_ALARMCTRL+16
#define DID_ALARMPRINT_HELP			DID_ALARMCTRL+17

//其他控件 
static HWND btnOk;
static HWND editCaption;
static HWND editHelp;
static HWND btnAllOff;
static HWND btnAllOn;
static HWND btnSetHR;
static HWND cobHR;
static HWND btnSetNibp;
static HWND cobNibp;
static HWND btnSetSpO2;
static HWND cobSpO2;
static HWND btnSetPR;
static HWND cobPR;
static HWND btnSetResp;
static HWND cobResp;
static HWND btnSetTemp;
static HWND cobTemp;

//控件回调函数
static WNDPROC OldBtnProc;
static WNDPROC OldCobProc;
static WNDPROC OldEditProcHelp;
//是否选择了全X
static BOOL bSelAllStatus = FALSE;

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
		DID_ALARMPRINT_HELP,
		"",
		0
	}
};

/*
	设置选择框的选择项
*/
static int SetCobIndex()
{
	SendMessage(cobHR, CB_SETCURSEL, gCfgEcg.bPrnControl, 0);
	SendMessage(cobNibp, CB_SETCURSEL, gCfgNibp.bPrnControl, 0);
	SendMessage(cobSpO2, CB_SETCURSEL, gCfgSpO2.bPrnControl, 0);
	SendMessage(cobPR, CB_SETCURSEL, gCfgPulse.bPrnControl, 0);
	SendMessage(cobResp, CB_SETCURSEL, gCfgResp.bPrnControl, 0);
	SendMessage(cobTemp, CB_SETCURSEL, gCfgTemp.bPrnControl, 0);
	
	return 0;
}

/*
	统一设置报警打印
*/
static int AlarmAllPrint(BYTE bAlarmPrint)
{
	gCfgEcg.bPrnControl = bAlarmPrint;
	gCfgNibp.bPrnControl = bAlarmPrint;
	gCfgSpO2.bPrnControl = bAlarmPrint;
	gCfgPulse.bPrnControl = bAlarmPrint;
	gCfgResp.bPrnControl = bAlarmPrint;
	gCfgTemp.bPrnControl = bAlarmPrint;
	
	SetCobIndex();
	
	return 0;
}

/*
	写配置文件
*/
static int WriteCfgFile()
{
	SetIntValueToResFile(gFileSetup, "ECGSetup", "alarm_print",  gCfgEcg.bPrnControl, 1);
	SetIntValueToResFile(gFileSetup, "NIBPSetup", "alarm_print",  gCfgNibp.bPrnControl, 1);
	SetIntValueToResFile(gFileSetup, "SPO2Setup", "alarm_print",  gCfgSpO2.bPrnControl, 1);
	SetIntValueToResFile(gFileSetup, "PULSESetup", "alarm_print",  gCfgPulse.bPrnControl, 1);
	SetIntValueToResFile(gFileSetup, "RESPSetup", "alarm_print",  gCfgResp.bPrnControl, 1);
	SetIntValueToResFile(gFileSetup, "TEMPSetup", "alarm_print",  gCfgTemp.bPrnControl, 1);
	
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
				case DID_ALARMPRINT_SET_HR:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMPRINT, "help_hr", strHelp, sizeof strHelp,"Set up HR pause time.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALARMPRINT_SET_NIBP:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMPRINT, "help_nibp", strHelp, sizeof strHelp,"Set up NIBP alarm print.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALARMPRINT_SET_SPO2:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMPRINT, "help_spo2", strHelp, sizeof strHelp,"Set up Spo2 alarm print .");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALARMPRINT_SET_PR:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMPRINT, "help_pr", strHelp, sizeof strHelp,"Set up PR alarm print .");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALARMPRINT_SET_RESP:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMPRINT, "help_resp", strHelp, sizeof strHelp,"Set up resp alarm print .");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALARMPRINT_SET_TEMP:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMPRINT, "help_temp", strHelp, sizeof strHelp,"Set up temp alarm print .");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALARMPRINT_ALL_OFF:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMPRINT, "help_alloff", strHelp, sizeof strHelp,"Set up alarm print all off.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALARMPRINT_ALL_ON:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMPRINT, "help_allon", strHelp, sizeof strHelp,"Set up alarm print all on.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_ALARMPRINT_OK:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "help_previous", strHelp, sizeof strHelp,"Return to previous menu.");	
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
	
	switch(message){
		case MSG_COMMAND:{
		case MSG_KEYUP:{
			if(wParam==SCANCODE_ENTER||CBN_EDITCHANGE==HIWORD(wParam)){
					id = GetDlgCtrlID(hWnd);
					index = SendMessage(hWnd, CB_GETCURSEL, 0, 0);
						
					switch(id){
						case DID_ALARMPRINT_HR:{
							gCfgEcg.bPrnControl = index;
							SetIntValueToResFile(gFileSetup, "ECGSetup", "alarm_print",  gCfgEcg.bPrnControl, 1);
							SetFocus(btnSetHR);	
						}break;
						case DID_ALARMPRINT_SPO2:{
							gCfgSpO2.bPrnControl = index;
							SetIntValueToResFile(gFileSetup, "SPO2Setup", "alarm_print",  gCfgSpO2.bPrnControl, 1);
							SetFocus(btnSetSpO2);	
						}break;
						case DID_ALARMPRINT_PR:{
							gCfgPulse.bPrnControl = index;
							SetIntValueToResFile(gFileSetup, "PULSESetup", "alarm_print",  gCfgPulse.bPrnControl, 1);
							SetFocus(btnSetHR);	
						}break;
						case DID_ALARMPRINT_RESP:{
							gCfgResp.bPrnControl = index;
							SetIntValueToResFile(gFileSetup, "RESPSetup", "alarm_print",  gCfgResp.bPrnControl, 1);
							SetFocus(btnSetResp);	
						}break;
						case DID_ALARMPRINT_TEMP:{
							gCfgTemp.bPrnControl = index;
							SetIntValueToResFile(gFileSetup, "TEMPSetup", "alarm_print",  gCfgTemp.bPrnControl, 1);
							SetFocus(btnSetTemp);	
						}break;
						case DID_ALARMPRINT_NIBP:{
							gCfgNibp.bPrnControl = index;
							SetIntValueToResFile(gFileSetup, "NIBPSetup", "alarm_print",  gCfgNibp.bPrnControl, 1);
							SetFocus(btnSetNibp);	
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
static int CreateCtlProc(HWND hDlg)
{
	editHelp = GetDlgItem(hDlg, DID_ALARMPRINT_HELP);
	editCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE  ,
					     DID_ALARMPRINT_CAPTION, 0, 5, 250, 25, hDlg, 0);
	btnSetHR = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
			DID_ALARMPRINT_SET_HR, 10, 30+30*0, 125, 25, hDlg, 0);
	btnSetNibp = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
			DID_ALARMPRINT_SET_NIBP, 10, 30+30*1, 125, 25, hDlg, 0);
	btnSetSpO2 = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
			DID_ALARMPRINT_SET_SPO2, 10, 30+30*2, 125, 25, hDlg, 0);
	btnSetPR = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
			DID_ALARMPRINT_SET_PR, 10, 30+30*3, 125, 25, hDlg, 0);
	btnSetResp = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
			DID_ALARMPRINT_SET_RESP, 10, 30+30*4, 125, 25, hDlg, 0);
	btnSetTemp = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
			DID_ALARMPRINT_SET_TEMP, 10, 30+30*5, 125, 25, hDlg, 0);
	btnAllOff = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
			DID_ALARMPRINT_ALL_OFF, 10, 30+30*6, 125, 25, hDlg, 0);
	btnAllOn = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
			DID_ALARMPRINT_ALL_ON, 10, 30+30*7, 125, 25, hDlg, 0);
	btnOk =  CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
			DID_ALARMPRINT_OK, 10, 30+30*8, 125, 25, hDlg, 0);

	cobHR = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
			DID_ALARMPRINT_HR, 140, 30+30*0, 95, 25, hDlg, 0);
	cobNibp = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
			     DID_ALARMPRINT_NIBP, 140, 30+30*1, 95, 25, hDlg, 0);
	cobSpO2 = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
			       DID_ALARMPRINT_SET_SPO2, 140, 30+30*2, 95, 25, hDlg, 0);
	cobPR = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
			       DID_ALARMPRINT_SET_PR, 140, 30+30*3, 95, 25, hDlg, 0);
	cobResp = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
			     DID_ALARMPRINT_SET_RESP, 140, 30+30*4, 95, 25, hDlg, 0);
	cobTemp = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
			       DID_ALARMPRINT_TEMP, 140, 30+30*5, 95, 25, hDlg, 0);
	
	return 0;
}
static int InitCtlName(HWND hDlg)
{
	char strMenu[100];	
	//caption
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMPRINT, "caption", strMenu, sizeof strMenu,"Alarm Print Set");	
	SetWindowText(editCaption,strMenu);
	//HR
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "hr", strMenu, sizeof strMenu,"HR");	
	SetWindowText(btnSetHR, strMenu);
	//NIBP
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "nibp", strMenu, sizeof strMenu,"Nibp");	
	SetWindowText(btnSetNibp, strMenu);
	//SpO2
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "spo2", strMenu, sizeof strMenu,"Spo2");	
	SetWindowText(btnSetSpO2, strMenu);
	//PR
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "pr", strMenu, sizeof strMenu,"PR");	
	SetWindowText(btnSetPR, strMenu);
	//Resp
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes,STR_SETTING_GENERAL, "resp", strMenu, sizeof strMenu,"Resp");	
	SetWindowText(btnSetResp, strMenu);
	//TEMP
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "temp", strMenu, sizeof strMenu,"Temp");	
	SetWindowText(btnSetTemp, strMenu);
	//all off
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMPRINT, "all_off", strMenu, sizeof strMenu,"All Off");
	SetWindowText(btnAllOff, strMenu);
	//all on
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMPRINT, "all_on", strMenu, sizeof strMenu,"All On");	
	SetWindowText(btnAllOn, strMenu);
	//OK
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "previous", strMenu, sizeof strMenu,"Previous");	
	SetWindowText(btnOk, strMenu);
	
	return 0;
}
static int InitCobList(HWND hDlg)
{
	unsigned char strClass[2][20]={0};
	char strMenu[100];
	int i;
	
	//加载级别字符串
	memset(strClass[0], 0, sizeof strClass[0]);
	GetStringFromResFile(gsLanguageRes, "GENERAL", "off", strClass[0], sizeof strClass[0]);	
	memset(strClass[1], 0, sizeof strClass[1]);
	GetStringFromResFile(gsLanguageRes, "GENERAL", "on", strClass[1], sizeof strClass[1]);	
			
	for(i=0; i<2; i++){
		SendMessage(cobHR, CB_ADDSTRING, 0, (LPARAM)strClass[i]);
	}
	SendMessage(cobHR, CB_SETCURSEL, gCfgEcg.bPrnControl, 0);
	
	for(i=0; i<2; i++){
		SendMessage(cobNibp, CB_ADDSTRING, 0, (LPARAM)strClass[i]);
	}
	SendMessage(cobNibp, CB_SETCURSEL, gCfgNibp.bPrnControl, 0);
	
	for(i=0; i<2; i++){
		SendMessage(cobSpO2, CB_ADDSTRING, 0, (LPARAM)strClass[i]);
	}
	SendMessage(cobSpO2, CB_SETCURSEL, gCfgSpO2.bPrnControl, 0);
	
	for(i=0; i<2; i++){
		SendMessage(cobPR, CB_ADDSTRING, 0, (LPARAM)strClass[i]);
	}
	SendMessage(cobPR, CB_SETCURSEL, gCfgPulse.bPrnControl, 0);
	
	for(i=0; i<2; i++){
		SendMessage(cobResp, CB_ADDSTRING, 0, (LPARAM)strClass[i]);
	}
	SendMessage(cobResp, CB_SETCURSEL, gCfgResp.bPrnControl, 0);
	
	for(i=0; i<2; i++){
		SendMessage(cobTemp, CB_ADDSTRING, 0, (LPARAM)strClass[i]);
	}
	SendMessage(cobTemp, CB_SETCURSEL, gCfgTemp.bPrnControl, 0);
	
	return 0;
}
static int InitCtlCallBackProc(HWND hDlg)
{
	OldBtnProc = SetWindowCallbackProc(btnSetHR, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSetNibp, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSetSpO2, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSetPR, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSetResp, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSetTemp, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnAllOff, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnAllOn, BtnProc);
	OldBtnProc  = SetWindowCallbackProc(btnOk, BtnProc); 
	
	OldCobProc = SetWindowCallbackProc(cobHR, CobProc);
	OldCobProc = SetWindowCallbackProc(cobNibp, CobProc);
	OldCobProc = SetWindowCallbackProc(cobSpO2, CobProc);
	OldCobProc = SetWindowCallbackProc(cobPR, CobProc);
	OldCobProc = SetWindowCallbackProc(cobResp, CobProc);
	OldCobProc = SetWindowCallbackProc(cobTemp, CobProc);

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
			//设置控件回调函数
			InitCtlCallBackProc(hDlg);
			
			//初始时没有全选
			bSelAllStatus = FALSE;
			gbKeyType = KEY_TAB;
			SetFocus(btnOk);
		}break;
		case MSG_PAINT:{

		}break;
		case MSG_COMMAND:{
			int	id  = LOWORD(wParam);
			
			switch(id){
 				case DID_ALARMPRINT_SET_HR:{
 					gbKeyType = KEY_UD;	
 					SetFocus(cobHR);
 				}break;
				case DID_ALARMPRINT_SET_NIBP:{
					gbKeyType = KEY_UD;	
					SetFocus(cobNibp);
				}break;
				case DID_ALARMPRINT_SET_SPO2:{
					gbKeyType = KEY_UD;	
					SetFocus(cobSpO2);
				}break;
				case DID_ALARMPRINT_SET_PR:{
					gbKeyType = KEY_UD;	
					SetFocus(cobPR);
				}break;
				case DID_ALARMPRINT_SET_RESP:{
					gbKeyType = KEY_UD;	
					SetFocus(cobResp);
				}break;
				case DID_ALARMPRINT_SET_TEMP:{
					gbKeyType = KEY_UD;	
					SetFocus(cobTemp);
				}break;
				case DID_ALARMPRINT_ALL_OFF:{
					bSelAllStatus = TRUE;
					AlarmAllPrint(ALARM_PRINT_OFF);
				}break;
				case DID_ALARMPRINT_ALL_ON:{
					bSelAllStatus = TRUE;
					AlarmAllPrint(ALARM_PRINT_ON);
				}break;
 				case DID_ALARMPRINT_OK:{
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

			//根据是否全选,写配置文件
			if(bSelAllStatus){
				WriteCfgFile();
			}				
			
			EndDialog(hDlg, wParam);	
		}break;
	}
	
	temp = DefaultDialogProc(hDlg, message, wParam, lParam);
	return(temp);
}


/*
	建立对话框
*/
void CreateDlgAlarmPrint(HWND hWnd)
{
	DlgSet.controls = CtrlSet;
	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}

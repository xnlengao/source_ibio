/*************************************************************************** 
 *	Module Name:	DlgNew_SYSMaint.c
 *
 *	Abstract:系统维护菜单
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2012-12-13 8:14:18
 *	
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"

//DID_MAINTSYS
#define DID_MAINTSYS_HELP					DID_MAINTSYS
#define DID_MAINTSYS_OK					DID_MAINTSYS+1
#define DID_MAINTSYS_CAPTION				DID_MAINTSYS+2
#define DID_MAINTSYS_OXYCRG				DID_MAINTSYS+3
#define DID_MAINTSYS_LINETYPE				DID_MAINTSYS+4
#define DID_MAINTSYS_HUM					DID_MAINTSYS+5
#define DID_MAINTSYS_TEMPSENSOR			DID_MAINTSYS+6
#define DID_MAINTSYS_OXYCRG_COB			DID_MAINTSYS+7
#define DID_MAINTSYS_LINETYPE_COB			DID_MAINTSYS+8
#define DID_MAINTSYS_HUM_COB				DID_MAINTSYS+9
#define DID_MAINTSYS_TEMPSENSOR_COB		DID_MAINTSYS+10

//帮助对话框
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;

//其他控件 
static HWND btnOk;
static HWND btnOxyCRG;
static HWND btnLineType;
static HWND btnHUM;
static HWND btnTempSensor;

static HWND cobOxyCRG;
static HWND cobLineType;
static HWND cobHUM;
static HWND cobTempSensor;
//控件回调函数
static WNDPROC OldBtnProc;
static WNDPROC OldCobProc;


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
		DID_MAINTSYS_HELP,
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
	SetWindowElementColorEx( hWnd, FGC_CONTROL_NORMAL,COLOR_yellow);
#endif
	switch(message){
		case MSG_PAINT:{
			id = lParam;
			switch(id){
				case DID_MAINTSYS_OXYCRG:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SYSMAINT, "help_oxycrg", strHelp, sizeof strHelp,"Set up OxyCRG time Frame.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MAINTSYS_LINETYPE:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SYSMAINT, "help_linetype", strHelp, sizeof strHelp,"Set up line type.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MAINTSYS_HUM:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SYSMAINT, "help_hum", strHelp, sizeof strHelp,"Set up AC Frequency.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MAINTSYS_TEMPSENSOR:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SYSMAINT, "help_tempsensor", strHelp, sizeof strHelp,"Set up temperature sensor.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MAINTSYS_OK:{
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
						case DID_MAINTSYS_OXYCRG_COB:{
							if(gCfgSystem.bOxyCrgScale != index){
								gCfgSystem.bOxyCrgScale = index;
								SetIntValueToResFile(gFileSetup, "SystemSetup", "oxycrg_scale",  gCfgSystem.bOxyCrgScale, 1);
								//改变视图刻度
								NewChangeCrgScale();
							}
							SetFocus(btnOxyCRG);
						}break;
						case DID_MAINTSYS_LINETYPE_COB:{
							if(gCfgSystem.bLineType != index){
								gCfgSystem.bLineType = index;
								SetIntValueToResFile(gFileSetup, "SystemSetup", "line_type",  gCfgSystem.bLineType, 1);
							}
							SetFocus(btnLineType);
						}break;
						case DID_MAINTSYS_HUM_COB:{
							if(gCfgSystem.bACFreq != index){
								gCfgSystem.bACFreq = index;
								SetIntValueToResFile(gFileSetup, "SystemSetup", "ac_freq",  gCfgSystem.bACFreq, 1);
								//设置交流频率
							//	SetACFreq();
							
							}
							SetFocus(btnHUM);
						}break;
						case DID_MAINTSYS_TEMPSENSOR_COB:{
							   gCfgTemp.bTempType= index;
							if(gCfgTemp.bTempType >1) gCfgTemp.bTempType = 0;
 							SetIntValueToResFile(gFileMachineConfig, "SENSOR", "sensor_temp",  gbSensor_Temp, 1);
                           			SetIBioTempType();
							SetFocus(btnTempSensor);
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
	绘制下一页图标函数
*/
static int DrawIconProc(HWND hDlg)
{
	
	int i;
	for(i=0;i<4;i++){
		CreateWindow("static",gstrNextPage,WS_CHILD|WS_VISIBLE,IDC_STATIC,
	 		150, 33+30*i, 30, 16,hDlg,0);
	}
	return(0);
}


/*
	创建控件函数
*/
static int CreateCtlProc(HWND hDlg)
{
	editCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE, 
					     DID_MAINTSYS_CAPTION, 0, 5, 240, 25, hDlg, 0);
	editHelp = GetDlgItem(hDlg, DID_MAINTSYS_HELP);

	btnOxyCRG = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MAINTSYS_OXYCRG, 10, 30+30*0, 125, 26, hDlg, 0);
	btnLineType = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MAINTSYS_LINETYPE, 10, 30+30*1, 125, 26, hDlg, 0);
	btnHUM = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MAINTSYS_HUM, 10, 30+30*2, 125, 26, hDlg, 0);
	btnTempSensor = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MAINTSYS_TEMPSENSOR, 10, 30+30*3, 125, 26, hDlg, 0);
	
	btnOk = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MAINTSYS_OK, 10, 30+30*4, 125, 26, hDlg, 0);

	cobOxyCRG = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_MAINTSYS_OXYCRG_COB, 140, 30+30*0, 95, 26, hDlg, 0);
	cobLineType = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_MAINTSYS_LINETYPE_COB, 140, 30+30*1, 95, 26, hDlg, 0);
	cobHUM = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_MAINTSYS_HUM_COB, 140, 30+30*2, 95, 26, hDlg, 0);
	cobTempSensor = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_MAINTSYS_TEMPSENSOR_COB, 140, 30+30*3, 95, 26, hDlg, 0);
	
	return 0;
}
static int InitCtlName(HWND hDlg)
{
	char strMenu[100];

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SYSMAINT, "caption", strMenu, sizeof strMenu,"Sys Maintenance");
	SetWindowText(editCaption, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "previous", strMenu, sizeof strMenu,"Previous");	
	SetWindowText(btnOk, strMenu);
	
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SYSMAINT, "oxycrg", strMenu, sizeof strMenu,"OxyCRG");	
	SetWindowText(btnOxyCRG, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SYSMAINT, "linetype", strMenu, sizeof strMenu,"Line Type");	
	SetWindowText(btnLineType, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SYSMAINT, "hum", strMenu, sizeof strMenu,"HUM");	
	SetWindowText(btnHUM, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SYSMAINT, "tempsensor", strMenu, sizeof strMenu,"Temp Sensor");	
	SetWindowText(btnTempSensor, strMenu);
	
	return 0;
}

//初始化combobox
static int InitCobList(HWND hDlg)
{
	char strMenu[100];
	char strUnit[10];
	static unsigned char *strOxyCrgScale[] = {"60", "120", "300", "600", "900", "1800"};
	static unsigned char *strAC[] = {"50Hz", "60Hz"};
	//oxycrg
	memset(strUnit, 0, sizeof strUnit);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "oxycrg_unit", strUnit, sizeof strUnit);	
	memset(strMenu, 0, sizeof strMenu);
	snprintf(strMenu, sizeof strMenu, "%s%s", strOxyCrgScale[0], strUnit);
	SendMessage(cobOxyCRG, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	snprintf(strMenu, sizeof strMenu, "%s%s", strOxyCrgScale[1], strUnit);
	SendMessage(cobOxyCRG, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	snprintf(strMenu, sizeof strMenu, "%s%s", strOxyCrgScale[2], strUnit);
	SendMessage(cobOxyCRG, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	snprintf(strMenu, sizeof strMenu, "%s%s", strOxyCrgScale[3], strUnit);
	SendMessage(cobOxyCRG, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	snprintf(strMenu, sizeof strMenu, "%s%s", strOxyCrgScale[4], strUnit);
	SendMessage(cobOxyCRG, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	snprintf(strMenu, sizeof strMenu, "%s%s", strOxyCrgScale[5], strUnit);
	SendMessage(cobOxyCRG, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobOxyCRG, CB_SETCURSEL, gCfgSystem.bOxyCrgScale, 0);
	//HUM acfreq
	SendMessage(cobHUM, CB_ADDSTRING, 0, (LPARAM)strAC[0]);
	SendMessage(cobHUM, CB_ADDSTRING, 0, (LPARAM)strAC[1]);
	SendMessage(cobHUM, CB_SETCURSEL, gCfgSystem.bACFreq, 0);
	//line type
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "USER", "line_wide", strMenu, sizeof strMenu);	
	SendMessage(cobLineType, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "USER", "line_normal", strMenu, sizeof strMenu);	
	SendMessage(cobLineType, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobLineType, CB_SETCURSEL, gCfgSystem.bLineType, 0);
	//temp sensor
	SendMessage(cobTempSensor, CB_ADDSTRING, 0, (LPARAM)"YSI");
	SendMessage(cobTempSensor, CB_ADDSTRING, 0, (LPARAM)"CY-F");
	SendMessage(cobTempSensor, CB_SETCURSEL, gCfgTemp.bTempType, 0);;
	return 0;
}

static int InitCtlCallBackProc(HWND hDlg)
{
	OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnOxyCRG, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnLineType, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnHUM, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnTempSensor, BtnProc);

	OldCobProc = SetWindowCallbackProc(cobOxyCRG, CobProc);
	OldCobProc = SetWindowCallbackProc(cobLineType, CobProc);
	OldCobProc = SetWindowCallbackProc(cobHUM, CobProc);
	OldCobProc = SetWindowCallbackProc(cobTempSensor, CobProc);
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
			//初始化combobox
			InitCobList(hDlg);
			//设置控件回调函数
			InitCtlCallBackProc(hDlg);
			//绘制下一页菜单
		//	DrawIconProc(hDlg);
			gbKeyType = KEY_TAB;
			SetFocus(btnOk);
		}break;
		case MSG_PAINT:{
			

		}break;
		case MSG_COMMAND:{
			int	id  = LOWORD(wParam);
			switch(id){
				case DID_MAINTSYS_OXYCRG:{
					gbKeyType = KEY_UD;
					SetFocus(cobOxyCRG);
					}break;
				case DID_MAINTSYS_LINETYPE:{
					gbKeyType = KEY_UD;
					SetFocus(cobLineType);
					}break;
				case DID_MAINTSYS_HUM:{
					gbKeyType = KEY_UD;
					SetFocus(cobHUM);
					}break;
				case DID_MAINTSYS_TEMPSENSOR:{
					gbKeyType = KEY_UD;
					SetFocus(cobTempSensor);
					}break;
				case DID_MAINTSYS_OK:{
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
void CreateMaintSYS(HWND hWnd)
{
 	DlgSet.controls = CtrlSet;
 	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}


/*************************************************************************** 
 *	Module Name:	DlgNew_SystemINFO
 *
 *	Abstract:显示系统选项
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2012-12-6 10:14:18
 *	
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"

//DID_SYSINFO
#define DID_SYSINFO_HELP				DID_SYSINFO
#define DID_SYSINFO_OK					DID_SYSINFO+1
#define DID_SYSINFO_CAPTION				DID_SYSINFO+2
#define DID_SYSINFO_STC					DID_SYSINFO+3



//帮助对话框
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;

//其他控件 
static HWND btnOk;

static HWND stcModuleSV[2];//参数板软件版本号soft version
static HWND stcModuleHV[2];//参数板软件版本号hard version
static HWND stcModuleUNIT[2];//参数板uid
static HWND stcModuleSN[2];//参数板SN
static HWND stcPowerSV[2];//电源板软件版本号
static HWND stcPowerHV[2];//电源板软件版本号
static HWND stcSoftVersion[2];//主软件版本号



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
		DID_SYSINFO_HELP,
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
				case DID_SYSINFO_OK:{
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

//创建控件
static int CreateCtlProc(HWND hDlg)
{
	editHelp = GetDlgItem(hDlg, DID_SYSINFO_HELP);
	editCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE, 
					     DID_SYSINFO_CAPTION, 0,5, 240, 25, hDlg, 0);
	btnOk = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_SYSINFO_OK, 10, 280, 100, 26, hDlg, 0);


	stcModuleSV[0]=CreateWindow("static", "", WS_CHILD | SS_NOTIFY  | SS_RIGHT |  WS_VISIBLE, 
					     DID_SYSINFO_STC, 2, 30, 140, 26, hDlg, 0);
	stcModuleHV[0]=CreateWindow("static", "", WS_CHILD | SS_NOTIFY  | SS_RIGHT |WS_VISIBLE, 
					     DID_SYSINFO_STC+1, 2, 60, 140, 26, hDlg, 0);
	stcModuleUNIT[0]=CreateWindow("static", "", WS_CHILD | SS_NOTIFY  |SS_RIGHT | WS_VISIBLE, 
					     DID_SYSINFO_STC+2, 2, 90, 140, 26, hDlg, 0);
	stcModuleSN[0]=CreateWindow("static", "", WS_CHILD | SS_NOTIFY  | SS_RIGHT |WS_VISIBLE, 
					     DID_SYSINFO_STC+3, 2, 120, 140, 26, hDlg, 0);
	stcPowerSV[0]=CreateWindow("static", "", WS_CHILD | SS_NOTIFY  | SS_RIGHT |WS_VISIBLE, 
					     DID_SYSINFO_STC+4, 2, 150, 140, 26, hDlg, 0);
	stcPowerHV[0]=CreateWindow("static", "", WS_CHILD | SS_NOTIFY  |SS_RIGHT | WS_VISIBLE, 
					     DID_SYSINFO_STC+5, 2, 180, 140, 26, hDlg, 0);
	stcSoftVersion[0]=CreateWindow("static", "", WS_CHILD | SS_NOTIFY  | SS_RIGHT |WS_VISIBLE, 
					     DID_SYSINFO_STC+6, 2, 210, 140, 26, hDlg, 0);
	
	stcModuleSV[1]=CreateWindow("static", "", WS_CHILD | SS_NOTIFY  | WS_VISIBLE, 
					     DID_SYSINFO_STC+7, 150, 30, 100, 26, hDlg, 0);
	stcModuleHV[1]=CreateWindow("static", "", WS_CHILD | SS_NOTIFY  | WS_VISIBLE, 
					     DID_SYSINFO_STC+8, 150, 60, 100, 26, hDlg, 0);
	stcModuleUNIT[1]=CreateWindow("static", "", WS_CHILD | SS_NOTIFY  | WS_VISIBLE, 
					     DID_SYSINFO_STC+9, 150, 90, 100, 26, hDlg, 0);
	stcModuleSN[1]=CreateWindow("static", "", WS_CHILD | SS_NOTIFY  | WS_VISIBLE, 
					     DID_SYSINFO_STC+10, 150, 120, 100, 26, hDlg, 0);
	stcPowerSV[1]=CreateWindow("static", "", WS_CHILD | SS_NOTIFY  | WS_VISIBLE, 
					     DID_SYSINFO_STC+11, 150, 150, 100, 26, hDlg, 0);
	stcPowerHV[1]=CreateWindow("static", "", WS_CHILD | SS_NOTIFY  | WS_VISIBLE, 
					     DID_SYSINFO_STC+12, 150, 180, 100, 26, hDlg, 0);
	stcSoftVersion[1]=CreateWindow("static", "", WS_CHILD | SS_NOTIFY  | WS_VISIBLE, 
					     DID_SYSINFO_STC+13, 150, 210, 100, 50, hDlg, 0);


	return 0;
}

//设置控件字符
static int InitCtlName(HWND hDlg)
{
	char strMenu[100];
	char strModuleHV[10],strModuleSV[10],strModuleSN[10],strModuleUnitID[10],strPowerHV[10],strPowerSV[10],strSoftVersion[50];

	if(SystemStatus.wModuleHV!=0)
		snprintf(strModuleHV,sizeof(strModuleHV),"%d.%.02d",SystemStatus.wModuleHV>>8,SystemStatus.wModuleHV&0x0f);
	else
		snprintf(strModuleHV,sizeof(strModuleHV),"-.--");
	
	if(SystemStatus.wModuleSV!=0)
		snprintf(strModuleSV,sizeof(strModuleSV),"%d.%.02d",SystemStatus.wModuleSV>>8,SystemStatus.wModuleSV&0x0f);
	else
		snprintf(strModuleSV,sizeof(strModuleSV),"-.--");
	
	if(SystemStatus.wModuleSN!=0)
		snprintf(strModuleSN,sizeof(strModuleSN),"%.08x",SystemStatus.wModuleSN);
	else
		snprintf(strModuleSN,sizeof(strModuleSN),"----");
	
	if(SystemStatus.lModuleUnitID!=0)
		snprintf(strModuleUnitID,sizeof(strModuleUnitID),"%.08x",SystemStatus.lModuleUnitID);
	else
		snprintf(strModuleUnitID,sizeof(strModuleUnitID),"--------");
	
	
	if(SystemStatus.wPowerHV!=0)
		snprintf(strPowerHV,sizeof(strPowerHV),"%.04x",SystemStatus.wPowerHV);
	else
		snprintf(strPowerHV,sizeof(strPowerHV),"----");
	
	if(SystemStatus.wPowerSV!=0)
		snprintf(strPowerSV,sizeof(strPowerSV),"%.04x",SystemStatus.wPowerSV);
	else
		snprintf(strPowerSV,sizeof(strPowerSV),"----");
	
	
	sprintf(strSoftVersion,"%s \n%s\n%s ",VERSION_SOFT,__TIME__,__DATE__);
		

	//caption
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SYSINFO, "caption", strMenu, sizeof strMenu,"System Info");	
	SetWindowText(editCaption, strMenu);
	
	//Module SV
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SYSINFO, "module_sv", strMenu, sizeof strMenu,"Module Soft Version :");	
	SetWindowText(stcModuleSV[0], strMenu);
	
	//Module HV
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SYSINFO, "module_hv", strMenu, sizeof strMenu,"Module Hard Version :");	
	SetWindowText(stcModuleHV[0], strMenu);
	
	//Module UNIT
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SYSINFO, "module_unit_id", strMenu, sizeof strMenu,"Module Unit ID :");	
	SetWindowText(stcModuleUNIT[0], strMenu);

	//Module SN
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SYSINFO, "module_sn", strMenu, sizeof strMenu,"Module Serial Number :");	
	SetWindowText(stcModuleSN[0], strMenu);
	
	//Power SV
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SYSINFO, "power_sv", strMenu, sizeof strMenu,"Power Soft Version :");	
	SetWindowText(stcPowerSV[0], strMenu);

	//Power HV
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SYSINFO, "power_hv", strMenu, sizeof strMenu,"Power Hard Version :");	
	SetWindowText(stcPowerHV[0], strMenu);
	
	//Soft Version
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SYSINFO, "soft_version", strMenu, sizeof strMenu,"Soft Version :");	
	SetWindowText(stcSoftVersion[0], strMenu);	
	
	//Exit
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "previous", strMenu, sizeof strMenu,"Previous");	

	SetWindowText(btnOk, strMenu);
	
	SetWindowText(stcModuleSV[1], strModuleSV);
	SetWindowText(stcModuleHV[1], strModuleHV);
	SetWindowText(stcModuleUNIT[1], strModuleUnitID);
	SetWindowText(stcModuleSN[1], strModuleSN);
	SetWindowText(stcPowerSV[1], strPowerSV);
	SetWindowText(stcPowerHV[1], strPowerHV);
	SetWindowText(stcSoftVersion[1], strSoftVersion);
	
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
		
			OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);
			
			OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp); 
			gbKeyType = KEY_TAB;
			SetFocus(btnOk);
		}break;
		case MSG_PAINT:{
			

		}break;
		case MSG_COMMAND:{
			int	id  = LOWORD(wParam);
			switch(id){
				case DID_SYSINFO_OK:{
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
void CreateSystemINFO(HWND hWnd)
{
	
 	DlgSet.controls = CtrlSet;
 	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}


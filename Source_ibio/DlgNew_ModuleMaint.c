/*************************************************************************** 
 *	Module Name:	DlgNew_ModuleMaint
 *
 *	Abstract:模块校准菜单
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2012-12-12 14:14:18
 *	
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"

//DID_MAINTMODULE
#define DID_MAINTMODULE_HELP			DID_MAINTMODULE
#define DID_MAINTMODULE_OK			DID_MAINTMODULE+1
#define DID_MAINTMODULE_CAPTION		DID_MAINTMODULE+2
#define DID_MAINTMODULE_NIBPLEAK				DID_MAINTMODULE+3
#define DID_MAINTMODULE_NIBPCAL				DID_MAINTMODULE+4
#define DID_MAINTMODULE_ECGSCALE		DID_MAINTMODULE+5
#define DID_MAINTMODULE_CO2GAIN			DID_MAINTMODULE+6
#define DID_MAINTMODULE_CO2CAL			DID_MAINTMODULE+7
#define DID_MAINTMODULE_CO2FLOW		DID_MAINTMODULE+8
#define DID_MAINTMODULE_NIBPMAINT		DID_MAINTMODULE+9
#define DID_MAINTMODULE_IBPMAINT		DID_MAINTMODULE+10
#define DID_MAINTMODULE_NIBPLEAK_COB				DID_MAINTMODULE+11
#define DID_MAINTMODULE_NIBPCAL_COB				DID_MAINTMODULE+12
#define DID_MAINTMODULE_ECGSCALE_COB			DID_MAINTMODULE+13
#define DID_MAINTMODULE_CO2GAIN_COB				DID_MAINTMODULE+14
#define DID_MAINTMODULE_CO2CAL_COB				DID_MAINTMODULE+15
#define DID_MAINTMODULE_CO2FLOW_COB			DID_MAINTMODULE+16

//帮助对话框
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;

//其他控件 
static HWND btnOk;
static HWND btnNibpLeak;
static HWND btnNibpCal;
static HWND btnEcgScale;
static HWND btnCo2Gain;
static HWND btnCo2Cal;
static HWND btnCo2Flow;
static HWND btnNIBPMaint;
static HWND btnIBPMaint;
static HWND cobNibpLeak;
static HWND cobNibpCal;
static HWND cobEcgScale;
static HWND cobCo2Gain;
static HWND cobCo2Cal;
static HWND cobCo2Flow;

//控件回调函数
static WNDPROC OldBtnProc;
static WNDPROC OldCobProc;

//增益校准状态
extern BOOL gbCO2GainCal;

//NIBP漏气检测标志位
extern BOOL gbNibpLeak;

//ECG是否处在定标状态
extern BOOL gbiBioECGCaling;
//NIBP校准标志位
extern BOOL gbNibpCal;
//默认气体为5.00
static int iTmpGas = 50;

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
		DID_MAINTMODULE_HELP,
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
				case DID_MAINTMODULE_NIBPLEAK:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MODULEMAINT, "help_nibpleak", strHelp, sizeof strHelp,"Nibp leak test.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MAINTMODULE_ECGSCALE:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MODULEMAINT, "help_ecgscale", strHelp, sizeof strHelp,"Set ECG scale.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MAINTMODULE_NIBPCAL:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MODULEMAINT, "help_nibpcal", strHelp, sizeof strHelp,"Set NIBP calibrate.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MAINTMODULE_CO2CAL:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MODULEMAINT, "help_co2cal", strHelp, sizeof strHelp,"Set CO2 calibrate.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MAINTMODULE_CO2FLOW:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MODULEMAINT, "help_co2flow", strHelp, sizeof strHelp,"Set CO2 flow.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MAINTMODULE_IBPMAINT:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MODULEMAINT, "help_ibpmaint", strHelp, sizeof strHelp,"Set IBP maint.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MAINTMODULE_OK:{
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
	char strlen[100];
	
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
						case DID_MAINTMODULE_NIBPLEAK_COB:{
								if(index==1){
								
								iBioNibpLeakTest();
								
								gValueNibp.bCanceled = FALSE;
							
								EnableWindow(btnOk, FALSE);
							}else{
								StopiBioNibpMeasure();
								gbNibpLeak=FALSE;
								gValueNibp.bCanceled = TRUE;
								EnableWindow(btnOk, TRUE);
							}
							SetFocus(btnNibpLeak);
						}break;
						case DID_MAINTMODULE_NIBPCAL_COB:{
							 if(index==1){
								CalibrateiBioNibp();
								gValueNibp.bCanceled = FALSE;
								EnableWindow(btnOk, FALSE);
							}else{
								StopiBioNibpMeasure();
								 gbNibpCal=FALSE;
								gValueNibp.bCanceled = TRUE;
								 EnableWindow(btnOk, TRUE);

							}
							SetFocus(btnNibpCal);
						}break;
						
						case DID_MAINTMODULE_ECGSCALE_COB:{
							 if(index!=0){
							 		gbiBioECGCaling=TRUE;
				 				//	StartiBioCalibrateECG();
					 				Start_Ecg_Cal();
								  }
								  else{
								  	gbiBioECGCaling=FALSE;
								//	  StopiBioCalibrateECG();
									  Stop_Ecg_Cal();
								  }
							SetFocus(btnEcgScale);
						}break;
						case DID_MAINTMODULE_CO2GAIN_COB:{
							 if(index!=0){
							 		gbCO2GainCal=TRUE;
				 			//		CPTCmd_GainCal(iTmpGas);
								  }
								  else{
								  	gbCO2GainCal=FALSE;
								  }
					
							SetFocus(btnCo2Gain);
						}break;
						case DID_MAINTMODULE_CO2CAL_COB:{
							iTmpGas=index*100+500;
							if(B_PRINTF)printf("iTmpGas=%d\n",iTmpGas);
							SetFocus(btnCo2Cal);
						}break;
						case DID_MAINTMODULE_CO2FLOW_COB:{
							
							SetFocus(btnCo2Flow);
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
	
	CreateWindow("static",gstrNextPage,WS_CHILD|WS_VISIBLE,IDC_STATIC,
 		150, 33+30*3, 30, 16,hDlg,0);
	CreateWindow("static",gstrNextPage,WS_CHILD|WS_VISIBLE,IDC_STATIC,
 		150, 33+30*4, 30, 16,hDlg,0);
	return(0);
}


/*
	创建控件函数
*/
static int CreateCtlProc(HWND hDlg)
{
	editCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE, 
					     DID_MAINTMODULE_CAPTION, 0, 5, 240, 25, hDlg, 0);
	editHelp = GetDlgItem(hDlg, DID_MAINTMODULE_HELP);
	
	
	btnEcgScale = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MAINTMODULE_ECGSCALE, 10, 30+30*0, 125, 26, hDlg, 0);
/*
	btnNibpCal = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MAINTMODULE_NIBPCAL, 10, 30+30*1, 125, 26, hDlg, 0);
	btnNibpLeak = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MAINTMODULE_NIBPLEAK, 10, 30+30*2, 125, 26, hDlg, 0);
	btnCo2Cal = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					    DID_MAINTMODULE_CO2CAL, 10, 30+30*2, 125, 26, hDlg, 0);
*/
	btnCo2Gain = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MAINTMODULE_CO2GAIN, 10, 30+30*1, 125, 26, hDlg, 0);
	btnCo2Flow = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					    DID_MAINTMODULE_CO2FLOW, 10, 30+30*2, 125, 26, hDlg, 0);

	btnNIBPMaint= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					    DID_MAINTMODULE_NIBPMAINT, 10, 30+30*3, 125, 26, hDlg, 0);
	btnIBPMaint= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					    DID_MAINTMODULE_NIBPMAINT, 10, 30+30*4, 125, 26, hDlg, 0);
	btnOk = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MAINTMODULE_OK, 10, 30+30*5, 125, 26, hDlg, 0);

	
	cobEcgScale = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY|CBS_READONLY,  
					DID_MAINTMODULE_ECGSCALE_COB, 140, 30+30*0, 95, 26, hDlg, 0);
/*
	cobNibpCal = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_MAINTMODULE_NIBPCAL_COB, 140, 30+30*1, 95, 26, hDlg, 0);
	cobNibpLeak = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_MAINTMODULE_NIBPLEAK_COB, 140, 30+30*2, 95, 26, hDlg, 0);

	cobCo2Cal = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_MAINTMODULE_CO2CAL_COB, 140, 30+30*2, 95, 26, hDlg, 0);
*/	
	cobCo2Gain = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_MAINTMODULE_CO2GAIN_COB, 140, 30+30*1, 95, 26, hDlg, 0);
	cobCo2Flow = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_MAINTMODULE_CO2FLOW_COB, 140, 30+30*2, 95, 26, hDlg, 0);

			
	return 0;
}
static int InitCtlName(HWND hDlg)
{
	char strMenu[100];

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MODULEMAINT, "caption", strMenu, sizeof strMenu,"Module Maintenance");
	SetWindowText(editCaption, strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "previous", strMenu, sizeof strMenu,"Previous");	
	SetWindowText(btnOk, strMenu);

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MODULEMAINT, "nibpleak", strMenu, sizeof strMenu,"Nibp Leak");	
	SetWindowText(btnNibpLeak, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MODULEMAINT, "nibpcal", strMenu, sizeof strMenu,"Nibp CAL");	
	SetWindowText(btnNibpCal, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MODULEMAINT, "ecgscale", strMenu, sizeof strMenu,"Ecg CAL");	
	SetWindowText(btnEcgScale, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MODULEMAINT, "co2gain", strMenu, sizeof strMenu,"CO2 GAIN CAL");	
	SetWindowText(btnCo2Gain, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MODULEMAINT, "co2cal", strMenu, sizeof strMenu,"CO2 CalMode");	
	SetWindowText(btnCo2Cal, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MODULEMAINT, "co2flow", strMenu, sizeof strMenu,"CO2 Flow");	
	SetWindowText(btnCo2Flow, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MODULEMAINT, "nibpmaint", strMenu, sizeof strMenu,"NIBP Maint");	
	SetWindowText(btnNIBPMaint, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MODULEMAINT, "ibpmaint", strMenu, sizeof strMenu,"IBP Maint");	
	SetWindowText(btnIBPMaint, strMenu);
	
	
	return 0;
}
static int InitCobList(HWND hDlg)
{
	char strMenu[100];
	int i;
	unsigned char *strHUM[2]={"50 Hz","60 Hz"};
	unsigned char *strCO2Flow[5]={"50 cc/min","75 cc/min","100 cc/min","150 cc/min","200 cc/min"};
	unsigned char *strCO2CalMode[6]={"5.0 %","6.0 %","7.0 %","8.0 %","9.0 %","10.0 %"};

	//nibp leak
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "off", strMenu, sizeof strMenu,"OFF");	
	SendMessage(cobNibpLeak, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "on", strMenu, sizeof strMenu,"ON");	
	SendMessage(cobNibpLeak, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobNibpLeak, CB_SETCURSEL,gbNibpLeak, 0);

	//nibp cal
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "off", strMenu, sizeof strMenu,"OFF");	
	SendMessage(cobNibpCal, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "on", strMenu, sizeof strMenu,"ON");	
	SendMessage(cobNibpCal, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobNibpCal, CB_SETCURSEL,gbNibpCal, 0);

	//ecg scale
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "off", strMenu, sizeof strMenu,"OFF");	
	SendMessage(cobEcgScale, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "on", strMenu, sizeof strMenu,"ON");	
	SendMessage(cobEcgScale, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobEcgScale, CB_SETCURSEL,gbiBioECGCaling, 0);

	//co2 gain cal
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "off", strMenu, sizeof strMenu,"OFF");	
	SendMessage(cobCo2Gain, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "on", strMenu, sizeof strMenu,"ON");	
	SendMessage(cobCo2Gain, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobCo2Gain, CB_SETCURSEL,gbCO2GainCal, 0);

	SendMessage(cobCo2Cal, CB_SETSPINRANGE, (WPARAM)5.0, (LPARAM)10.0);
	SendMessage(cobCo2Cal, CB_SETSPINVALUE, (WPARAM)5.0, 0);

	//co2 calmode
	for(i=0;i<6;i++)
	{
		SendMessage(cobCo2Cal, CB_ADDSTRING, 0, (LPARAM)strCO2CalMode[i]);
	}
	SendMessage(cobCo2Cal, CB_SETCURSEL,0, 0);
	
	//co2 flow
	for(i=0;i<5;i++)
	{
		SendMessage(cobCo2Flow, CB_ADDSTRING, 0, (LPARAM)strCO2Flow[i]);
	}
	SendMessage(cobCo2Flow, CB_SETCURSEL,gCfgCO2.bRate, 0);

	
	return 0;
}
static int InitCtlCallBackProc(HWND hDlg)
{

	OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnNibpLeak, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnNibpCal, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnEcgScale, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnCo2Gain, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnCo2Cal, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnCo2Flow, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnNIBPMaint, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnIBPMaint, BtnProc);
	
	OldCobProc = SetWindowCallbackProc(cobNibpLeak, CobProc);
	OldCobProc = SetWindowCallbackProc(cobNibpCal, CobProc);
	OldCobProc = SetWindowCallbackProc(cobEcgScale, CobProc);
	OldCobProc = SetWindowCallbackProc(cobCo2Gain, CobProc);
	OldCobProc = SetWindowCallbackProc(cobCo2Cal, CobProc);
	OldCobProc = SetWindowCallbackProc(cobCo2Flow, CobProc);
	
	OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp);
	
	return 0;
}

static int InitCtlStatus(HWND hDlg)
{
	if(!gbHaveCo2){
		EnableWindow(btnCo2Gain, FALSE);
		EnableWindow(cobCo2Gain, FALSE);
		
		EnableWindow(btnCo2Cal, FALSE);
		EnableWindow(cobCo2Cal, FALSE);

		EnableWindow(btnCo2Flow, FALSE);
		EnableWindow(cobCo2Flow, FALSE);
		
	}
	
	if(!gbHaveIbp){
		EnableWindow(btnIBPMaint, FALSE);
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
			DrawIconProc(hDlg);
			gbKeyType = KEY_TAB;
			SetFocus(btnOk);
		}break;
		case MSG_PAINT:{
			

		}break;
		case MSG_COMMAND:{
			int	id  = LOWORD(wParam);
			switch(id){
				case DID_MAINTMODULE_NIBPCAL:{
				//	gbKeyType = KEY_UD;
				//	SetFocus(cobNibpCal);
					CreateMaintNIBP(hDlg);
					}break;
				case DID_MAINTMODULE_NIBPLEAK:{
					gbKeyType = KEY_UD;
					SetFocus(cobNibpLeak);
					}break;
				case DID_MAINTMODULE_ECGSCALE:{
					gbKeyType = KEY_UD;
					SetFocus(cobEcgScale);
					}break;
				case DID_MAINTMODULE_CO2GAIN:{
					gbKeyType = KEY_UD;
					SetFocus(cobCo2Gain);
					}break;
				case DID_MAINTMODULE_CO2CAL:{
					gbKeyType = KEY_UD;
					SetFocus(cobCo2Cal);
					}break;
				case DID_MAINTMODULE_CO2FLOW:{
					gbKeyType = KEY_UD;
					SetFocus(cobCo2Flow);
					}break;
				case DID_MAINTMODULE_NIBPMAINT:{
					CreateMaintNIBP(hDlg);
					}break;
				case DID_MAINTMODULE_IBPMAINT:{
					}break;
				case DID_MAINTMODULE_OK:{
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
void CreateMaintModule(HWND hWnd)
{
 	DlgSet.controls = CtrlSet;
 	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}


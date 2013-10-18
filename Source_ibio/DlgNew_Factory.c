/*************************************************************************** 
 *	Module Name:	DlgNew_NIBPMaint
 *
 *	Abstract:NIBP校准与维护
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2012-12-12 16:14:18
 *	
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"

//DID_NEWFACTORY
#define DID_NEWFACTORY_HELP				DID_NEWFACTORY
#define DID_NEWFACTORY_OK					DID_NEWFACTORY+1
#define DID_NEWFACTORY_CAPTION			DID_NEWFACTORY+2
#define DID_NEWFACTORY_MODULESET			DID_NEWFACTORY+3
#define DID_NEWFACTORY_ECGCAL			DID_NEWFACTORY+4
#define DID_NEWFACTORY_NIBPCAL			DID_NEWFACTORY+5
#define DID_NEWFACTORY_ECGCAL_COB			DID_NEWFACTORY+6
#define DID_NEWFACTORY_NIBPCAL_COB			DID_NEWFACTORY+7

//帮助对话框
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;

//其他控件 
static HWND btnOk;
static HWND btnModuleSet;
static HWND btnNibpCAL;
static HWND btnEcgCAL;
static HWND cobNibpCAL;
static HWND cobEcgCAL;

//控件回调函数
static WNDPROC OldBtnProc;
static WNDPROC OldCobProc;

//ECG是否处在定标状态
extern BOOL gbiBioECGCaling;
//NIBP校准标志位
extern BOOL gbNibpCal;

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
		DID_NEWFACTORY_HELP,
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
				case DID_NEWFACTORY_MODULESET:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_FACTORY, "help_module", strHelp, sizeof strHelp,"Module set.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWFACTORY_ECGCAL:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_FACTORY, "help_ecg", strHelp, sizeof strHelp,"Ecg calibrate.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWFACTORY_NIBPCAL:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_FACTORY, "help_nibp", strHelp, sizeof strHelp,"Nibp calibrate.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWFACTORY_OK:{
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
						case DID_NEWFACTORY_ECGCAL_COB:{
							 if(index!=0){
							 		gbiBioECGCaling=TRUE;
				 					StartiBioCalibrateECG();
								  }
								  else{
								  	gbiBioECGCaling=FALSE;
									  StopiBioCalibrateECG();	
								  }
							SetFocus(btnEcgCAL);
						}break;
						case DID_NEWFACTORY_NIBPCAL_COB:{
							 if(index==1){
								CalibrateiBioNibp();
								
								EnableWindow(btnEcgCAL, FALSE);
								EnableWindow(cobEcgCAL, FALSE);
								EnableWindow(btnModuleSet, FALSE);
								EnableWindow(btnOk, FALSE);
							}else{
								StopiBioNibpMeasure();
								 gbNibpCal=FALSE;

								EnableWindow(btnEcgCAL, TRUE);
								EnableWindow(cobEcgCAL, TRUE);
								EnableWindow(btnModuleSet, TRUE);
								EnableWindow(btnOk, TRUE);
							}
							SetFocus(btnNibpCAL);
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
	创建控件函数
*/
static int CreateCtlProc(HWND hDlg)
{
	editCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE,
					     DID_NEWFACTORY_CAPTION, 0, 5, 240, 25, hDlg, 0);
	editHelp = GetDlgItem(hDlg, DID_NEWFACTORY_HELP);
	
	btnModuleSet= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWFACTORY_MODULESET, 10, 30+30*0, 125, 26, hDlg, 0);
	btnEcgCAL= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWFACTORY_ECGCAL, 10, 30+30*1, 125, 26, hDlg, 0);
	btnNibpCAL= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWFACTORY_NIBPCAL, 10, 30+30*2, 125, 26, hDlg, 0);
	btnOk = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWFACTORY_OK, 10, 30+30*3, 125, 26, hDlg, 0);

	cobEcgCAL= CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					      DID_NEWFACTORY_ECGCAL_COB, 140, 30+30*1, 95, 26, hDlg, 0);
	cobNibpCAL= CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					      DID_NEWFACTORY_NIBPCAL_COB, 140, 30+30*2, 95, 26, hDlg, 0);
			
	return 0;
}
static int InitCtlName(HWND hDlg)
{
	char strMenu[100];

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_FACTORY, "caption", strMenu, sizeof strMenu,"Factory");
	SetWindowText(editCaption, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "previous", strMenu, sizeof strMenu,"Previous");	
	SetWindowText(btnOk, strMenu);
	
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_FACTORY, "nibpcal", strMenu, sizeof strMenu,"Nibp CAL");	
	SetWindowText(btnNibpCAL, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_FACTORY, "ecgcal", strMenu, sizeof strMenu,"Ecg CAL");	
	SetWindowText(btnEcgCAL, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_FACTORY, "moduleset", strMenu, sizeof strMenu,"Module Set");	
	SetWindowText(btnModuleSet, strMenu);
	
	return 0;
}

//初始化combobox
static int InitCobList(HWND hDlg)
{
	char strMenu[100];
	//ecg cal
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "off", strMenu, sizeof strMenu,"OFF");	
	SendMessage(cobEcgCAL, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "on", strMenu, sizeof strMenu,"ON");	
	SendMessage(cobEcgCAL, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobEcgCAL, CB_SETCURSEL, gbiBioECGCaling, 0);
	//nibp cal
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "off", strMenu, sizeof strMenu,"OFF");	
	SendMessage(cobNibpCAL, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "on", strMenu, sizeof strMenu,"ON");	
	SendMessage(cobNibpCAL, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobNibpCAL, CB_SETCURSEL, gbNibpCal, 0);
	
	return 0;
}

static int InitCtlCallBackProc(HWND hDlg)
{
	OldBtnProc = SetWindowCallbackProc(btnNibpCAL, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnEcgCAL, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnModuleSet, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);
	
	OldCobProc = SetWindowCallbackProc(cobNibpCAL, CobProc);
	OldCobProc = SetWindowCallbackProc(cobEcgCAL, CobProc);
	
	OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp);
	
	return 0;
}

static int InitCtlStatus(HWND hDlg)
{
	//EnableWindow(btnModuleSet, FALSE);
	
	return 0;
}
/*
	绘制下一页图标函数
*/
static int DrawIconProc(HWND hDlg)
{
	
	CreateWindow("static",gstrNextPage,WS_CHILD|WS_VISIBLE,IDC_STATIC,
 		150, 33+30*0, 30, 16,hDlg,0);
	
	return(0);
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
			//设置控件状态
			InitCtlStatus(hDlg);
			//绘制下一页图标
			DrawIconProc(hDlg);
			gbKeyType = KEY_TAB;
			SetFocus(btnOk);
		}break;
		case MSG_PAINT:{
			

		}break;
		case MSG_COMMAND:{
			int	id  = LOWORD(wParam);
			switch(id){
				case DID_NEWFACTORY_MODULESET:{
					CreateModuleSet(hDlg);
					}break;
				case DID_NEWFACTORY_ECGCAL:{
					gbKeyType = KEY_UD;
					SetFocus(cobEcgCAL);
					}break;
				case DID_NEWFACTORY_NIBPCAL:{
					gbKeyType = KEY_UD;
					SetFocus(cobNibpCAL);
					}break;
				case DID_NEWFACTORY_OK:{
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
void CreateDlgNewFactory(HWND hWnd)
{
 	DlgSet.controls = CtrlSet;
 	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}


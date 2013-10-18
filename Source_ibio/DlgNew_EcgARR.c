/*************************************************************************** 
 *	Module Name:	DlgNew_EcgARR
 *
 *	Abstract:ECG 心率分析设置
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2012-12-7 14:14:18
 *	
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"

//DID_NEWECGSET
#define DID_NEWECGARR_HELP		DID_NEWECGARR
#define DID_NEWECGARR_OK			DID_NEWECGARR+1
#define DID_NEWECGARR_CAPTION		DID_NEWECGARR+2
#define DID_NEWECGARR_ARR			DID_NEWECGARR+3
#define DID_NEWECGARR_PVCS		DID_NEWECGARR+4
#define DID_NEWECGARR_RELEARN	DID_NEWECGARR+5
#define DID_NEWECGARR_ARR_COB	DID_NEWECGARR+6
#define DID_NEWECGARR_PVCS_COB	DID_NEWECGARR+7

//帮助对话框
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;

//其他控件 
static HWND btnOk;
static HWND btnArr;
static HWND btnPVCs;
static HWND btnRelearn;
static HWND cobArr;
static HWND cobPVCs;

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
		DID_NEWECGARR_HELP,
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
				case DID_NEWECGARR_ARR:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGARR, "help_arr", strHelp, sizeof strHelp,"Enable/Disable arrhythmia analyse.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWECGARR_PVCS:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGARR, "help_pvcs", strHelp, sizeof strHelp,"Enable/Disable PVC analyse.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWECGARR_RELEARN:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGARR, "help_relearn", strHelp, sizeof strHelp,"Arrhythmia relearning.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWECGARR_OK:{
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
						case DID_NEWECGARR_ARR_COB:{
							gCfgEcg.bArrAnalyse=index;
							SetIntValueToResFile(gFileSetup, "ECGSetup", "arr_analyse",  gCfgEcg.bArrAnalyse, 1);

							SetFocus(btnArr);
						}break;
						case DID_NEWECGARR_PVCS_COB:{
							gCfgEcg.bPVCs=index;
							
							SetIntValueToResFile(gFileSetup, "ECGSetup", "pvcs",  gCfgEcg.bPVCs, 1);
							SetFocus(btnPVCs);
						}break;
						default:break;
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
					     DID_NEWECGARR_CAPTION, 0, 5, 240, 25, hDlg, 0);
	editHelp = GetDlgItem(hDlg, DID_NEWECGARR_HELP);

	btnArr= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECGARR_ARR, 10, 30+29*0, 125, 26, hDlg, 0);
	btnPVCs= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECGARR_PVCS, 10, 30+29*1, 125, 26, hDlg, 0);
	btnRelearn= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECGARR_RELEARN, 10, 30+29*2, 125, 26, hDlg, 0);
	
	btnOk = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECGARR_OK, 10, 30+29*3, 125, 26, hDlg, 0);

	cobArr= CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_NEWECGARR_ARR_COB, 140, 30+30*0, 95, 26, hDlg, 0);
	cobPVCs= CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_NEWECGARR_PVCS_COB, 140, 30+30*1, 95, 26, hDlg, 0);
	
	return 0;
}
static int InitCtlName(HWND hDlg)
{
	char strMenu[100];

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGARR, "caption", strMenu, sizeof strMenu,"ECG ARR Analyse");
	SetWindowText(editCaption, strMenu);
	
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "previous", strMenu, sizeof strMenu,"Previous");	
	SetWindowText(btnOk, strMenu);

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGARR, "arr", strMenu, sizeof strMenu,"Arr Analyse");	
	SetWindowText(btnArr, strMenu);
	
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGARR, "pvcs", strMenu, sizeof strMenu,"PVCs");	
	SetWindowText(btnPVCs, strMenu);
	
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECGARR, "relearn", strMenu, sizeof strMenu,"ECG Relearn");	
	SetWindowText(btnRelearn, strMenu);

	return 0;
}

static int InitCobList(HWND hDlg)
{
	char strOFF[50];
	char strON[50];
	char strMenu[50];
	
	memset(strOFF, 0, sizeof strOFF);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "off", strOFF, sizeof strOFF,"OFF");
	memset(strON, 0, sizeof strON);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "on", strON, sizeof strON,"ON");

	SendMessage(cobArr, CB_ADDSTRING, 0, (LPARAM)strOFF);
	SendMessage(cobArr, CB_ADDSTRING, 0, (LPARAM)strON);
	SendMessage(cobArr, CB_SETCURSEL,gCfgEcg.bArrAnalyse, 0);

	SendMessage(cobPVCs, CB_ADDSTRING, 0, (LPARAM)strOFF);
	SendMessage(cobPVCs, CB_ADDSTRING, 0, (LPARAM)strON);
	SendMessage(cobPVCs, CB_SETCURSEL, gCfgEcg.bPVCs, 0);
	

	return 0;
}
static int InitCtlCallBackProc(HWND hDlg)
{

	OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnArr, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnPVCs, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnRelearn, BtnProc);

	OldCobProc = SetWindowCallbackProc(cobArr, CobProc);
	OldCobProc = SetWindowCallbackProc(cobPVCs, CobProc);

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
				case DID_NEWECGARR_ARR:{
					gbKeyType = KEY_UD;
					SetFocus(cobArr);
					}break;
				case DID_NEWECGARR_PVCS:{
					gbKeyType = KEY_UD;
					SetFocus(cobPVCs);
					}break;
				case DID_NEWECGARR_RELEARN:{
					Set_ECG_Relearn();

					}break;
				case DID_NEWECGARR_OK:{
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
void CreateDlgNewECGARR(HWND hWnd)
{
 	DlgSet.controls = CtrlSet;
 	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}


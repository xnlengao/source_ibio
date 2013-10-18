/*************************************************************************** 
 *	Module Name:	DlgNew_Spo2
 *
 *	Abstract:Spo2设置菜单
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2013-1-18 	9:14:18
 *	
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"

//DID_NEWSPO2
#define DID_NEWSPO2_HELP			DID_NEWSPO2
#define DID_NEWSPO2_OK			DID_NEWSPO2+1
#define DID_NEWSPO2_CAPTION		DID_NEWSPO2+2
#define DID_NEWSPO2_SPEED			DID_NEWSPO2+3
#define DID_NEWSPO2_Gain			DID_NEWSPO2+4
#define DID_NEWSPO2_SENSE			DID_NEWSPO2+5
#define DID_NEWSPO2_DRAWMODE		DID_NEWSPO2+6
#define DID_NEWSPO2_ALMSPO2		DID_NEWSPO2+7
#define DID_NEWSPO2_ALMPR			DID_NEWSPO2+8
#define DID_NEWSPO2_SPEED_COB			DID_NEWSPO2+9
#define DID_NEWSPO2_GAIN_COB		DID_NEWSPO2+10
#define DID_NEWSPO2_SENSE_COB		DID_NEWSPO2+11
#define DID_NEWSPO2_DRAWMODE_COB		DID_NEWSPO2+12

//帮助对话框
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;

//其他控件 
static HWND btnOk;
static HWND btnSpeed;
static HWND btnGain;
static HWND btnSense;
static HWND btnDrawMode;
static HWND btnALMSpo2;
static HWND btnALMPR;

static HWND cobSpeed;
static HWND cobGain;
static HWND cobSense;
static HWND cobDrawMode;

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
		DID_NEWSPO2_HELP,
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
				case DID_NEWSPO2_SPEED:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SPO2, "help_speed", strHelp, sizeof strHelp,"Set up waveform speed.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWSPO2_Gain:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SPO2, "help_gain", strHelp, sizeof strHelp,"Set up waveform gain.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWSPO2_SENSE:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SPO2, "help_sense", strHelp, sizeof strHelp,"Set up SPO2 sensitivity.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWSPO2_DRAWMODE:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SPO2, "help_drawmode", strHelp, sizeof strHelp,"Set up draw mode of Spo2 waveform.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWSPO2_ALMSPO2:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SPO2, "help_spo2alarm", strHelp, sizeof strHelp,"Set up SPO2 alarm.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWSPO2_ALMPR:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SPO2, "help_pralarm", strHelp, sizeof strHelp,"Set up PR alarm.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWSPO2_OK:{
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
						case DID_NEWSPO2_SPEED_COB:{
							gCfgSpO2.bScanSpeed = index+1;
							SetIntValueToResFile(gFileSetup, STR_SETTING_DLG_SPO2, "scan_speed",  gCfgSpO2.bScanSpeed, 1);
// 							ChangeWaveX();
							ChangeWaveSpeed_SpO2();
							SetFocus(btnSpeed);
						}break;
						case DID_NEWSPO2_GAIN_COB:{
							gCfgSpO2.bGain = index;
							SetIntValueToResFile(gFileSetup, STR_SETTING_DLG_SPO2, "gain",  gCfgSpO2.bGain, 1);
							//SetSpO2Gain();
							SetFocus(btnGain);
						}break;
						case DID_NEWSPO2_SENSE_COB:{
							gCfgSpO2.bResponse = index;
							SetIntValueToResFile(gFileSetup, STR_SETTING_DLG_SPO2, "response",  gCfgSpO2.bResponse, 1);
							Set_Spo2_Sen();
							SetFocus(btnSense);
						}break;
						case DID_NEWSPO2_DRAWMODE_COB:{
							gCfgSpO2.bDrawMode= index;
							SetIntValueToResFile(gFileSetup, STR_SETTING_DLG_SPO2, "draw_mode",  gCfgSpO2.bDrawMode, 1);
							SetFocus(btnDrawMode);
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
	for(i=3;i<5;i++){
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
					     DID_NEWSPO2_CAPTION, 0, 5, 240, 25, hDlg, 0);
	editHelp = GetDlgItem(hDlg, DID_NEWSPO2_HELP);

	
	btnSpeed = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWSPO2_SPEED, 10, 30+30*0, 125, 26, hDlg, 0);
//	btnGain = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
//					     DID_NEWSPO2_Gain, 10, 30+30*1, 125, 26, hDlg, 0);
	btnSense = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWSPO2_SENSE, 10, 30+30*1, 125, 26, hDlg, 0);
	btnDrawMode = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWSPO2_DRAWMODE, 10, 30+30*2, 125, 26, hDlg, 0);
	btnALMSpo2 = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWSPO2_ALMSPO2, 10, 30+30*3, 125, 26, hDlg, 0);
	btnALMPR = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWSPO2_ALMPR, 10, 30+30*4, 125, 26, hDlg, 0);
	btnOk = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWSPO2_OK, 10, 30+30*5, 125, 26, hDlg, 0);

	cobSpeed = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_NEWSPO2_SPEED_COB, 140, 30+30*0, 95, 26, hDlg, 0);
//	cobGain = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
//					DID_NEWSPO2_GAIN_COB, 140, 30+30*1, 95, 26, hDlg, 0);
	cobSense = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_NEWSPO2_SENSE_COB, 140, 30+30*1, 95, 26, hDlg, 0);
	cobDrawMode = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_NEWSPO2_DRAWMODE_COB, 140, 30+30*2, 95, 26, hDlg, 0);

	return 0;
}
static int InitCtlName(HWND hDlg)
{
	char strMenu[100];

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SPO2, "caption", strMenu, sizeof strMenu,"SPO2");
	SetWindowText(editCaption, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "previous", strMenu, sizeof strMenu,"Previous");	
	SetWindowText(btnOk, strMenu);
	
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SPO2, "speed", strMenu, sizeof strMenu,"Speed");	
	SetWindowText(btnSpeed, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SPO2, "gain", strMenu, sizeof strMenu,"Gain");	
	SetWindowText(btnGain, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SPO2, "sense", strMenu, sizeof strMenu,"Sense");	
	SetWindowText(btnSense, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SPO2, "drawmode", strMenu, sizeof strMenu,"Draw Mode");	
	SetWindowText(btnDrawMode, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SPO2, "almspo2", strMenu, sizeof strMenu,"SPO2 ALM");	
	SetWindowText(btnALMSpo2, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SPO2, "almpr", strMenu, sizeof strMenu,"PR ALM");	
	SetWindowText(btnALMPR, strMenu);
	
	return 0;
}

//初始化combobox
static int InitCobList(HWND hDlg)
{
	char strMenu[100];
	//speed
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "speed_125", strMenu, sizeof strMenu,"12.5mm/s");	
	SendMessage(cobSpeed, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "speed_25", strMenu, sizeof strMenu,"25mm/s");	
	SendMessage(cobSpeed, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "speed_50", strMenu, sizeof strMenu,"50mm/s");	
	SendMessage(cobSpeed, CB_ADDSTRING, 0, (LPARAM)strMenu);
	//因为最小的扫描速度是6.25mm/s,索引值为0, 而ECG的最小扫描速度为12.5mm/s，索引为1
	SendMessage(cobSpeed, CB_SETCURSEL, gCfgSpO2.bScanSpeed-1, 0);
	//Gain
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "auto", strMenu, sizeof strMenu,"Auto");	
	SendMessage(cobGain, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "gain_05", strMenu, sizeof strMenu,"X0.5");	
	SendMessage(cobGain, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "gain_1", strMenu, sizeof strMenu,"X1");	
	SendMessage(cobGain, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "gain_2", strMenu, sizeof strMenu,"X2");	
	SendMessage(cobGain, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobGain, CB_SETCURSEL, gCfgSpO2.bGain, 0);
	//sense
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SPO2, "fast", strMenu, sizeof strMenu,"Fast");	
	SendMessage(cobSense, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SPO2, "normal", strMenu, sizeof strMenu,"Normal");	
	SendMessage(cobSense, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_SPO2, "slow", strMenu, sizeof strMenu,"Slow");	
	SendMessage(cobSense, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobSense, CB_SETCURSEL, gCfgSpO2.bResponse, 0);
	//drawmode
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "line", strMenu, sizeof strMenu,"Line");	
	SendMessage(cobDrawMode, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "fill", strMenu, sizeof strMenu,"Fill");	
	SendMessage(cobDrawMode, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobDrawMode, CB_SETCURSEL, gCfgSpO2.bDrawMode, 0);
		
	
	return 0;
}
static int InitCtlCallBackProc(HWND hDlg)
{

	OldBtnProc = SetWindowCallbackProc(btnSpeed, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnGain, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSense, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnDrawMode, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnALMSpo2, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnALMPR, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);

	OldCobProc = SetWindowCallbackProc(cobSpeed, CobProc);
	OldCobProc = SetWindowCallbackProc(cobGain, CobProc);
	OldCobProc = SetWindowCallbackProc(cobSense, CobProc);
	OldCobProc = SetWindowCallbackProc(cobDrawMode, CobProc);
	
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
			
				case DID_NEWSPO2_SPEED:{
					gbKeyType = KEY_UD;
					SetFocus(cobSpeed);
					}break;
				case DID_NEWSPO2_Gain:{
					gbKeyType = KEY_UD;
					SetFocus(cobGain);
					}break;
				case DID_NEWSPO2_SENSE:{
					gbKeyType = KEY_UD;
					SetFocus(cobSense);
					}break;
				case DID_NEWSPO2_DRAWMODE:{
					gbKeyType = KEY_UD;
					SetFocus(cobDrawMode);
					}break;
				case DID_NEWSPO2_ALMSPO2:{
					CreateDlgNewSpo2ALM(hDlg);
					}break;
				case DID_NEWSPO2_ALMPR:{
					CreateDlgNewPRALM(hDlg);
					}break;
				case DID_NEWSPO2_OK:{
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
			//ChangeWavePanelsLeft((PRECT)NULL);	
			EndDialog(hDlg, wParam);	
		}break;
	}
	
	temp = DefaultDialogProc(hDlg, message, wParam, lParam);
	return(temp);
}


/*
	建立对话框
*/
void CreateDlgNewSpo2(HWND hWnd)
{
 	DlgSet.controls = CtrlSet;
 	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}


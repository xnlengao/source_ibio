/*************************************************************************** 
 *	Module Name:	DlgNew_Ecg
 *
 *	Abstract:设置ECG配置信息的
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *	2012-12-20 14:14:18
 *	
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"

//DID_NEWECG
#define DID_NEWECG_HELP			DID_NEWECG
#define DID_NEWECG_OK			DID_NEWECG+1
#define DID_NEWECG_CAPTION		DID_NEWECG+2
#define DID_NEWECG_ECG1			DID_NEWECG+3
#define DID_NEWECG_GAIN			DID_NEWECG+4
#define DID_NEWECG_SPEED		DID_NEWECG+5
#define DID_NEWECG_BEEPVOL		DID_NEWECG+6
#define DID_NEWECG_ECGSETUP		DID_NEWECG+7
#define DID_NEWECG_ECGALM		DID_NEWECG+8
#define DID_NEWECG_ECGARR		DID_NEWECG+9
#define DID_NEWECG_STADJUST	DID_NEWECG+10
#define DID_NEWECG_ECG1_COB			DID_NEWECG+11
#define DID_NEWECG_GAIN_COB			DID_NEWECG+12
#define DID_NEWECG_SPEED_COB			DID_NEWECG+13
#define DID_NEWECG_BEEPVOL_COB		DID_NEWECG+14
#define DID_NEWECG_ECG2			DID_NEWECG+15
#define DID_NEWECG_ECG2_COB			DID_NEWECG+16
//帮助对话框
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;

//其他控件 
static HWND btnOk;
static HWND btnECG1;
static HWND btnECG2;
static HWND btnGain;
static HWND btnSpeed;
static HWND btnBeepVol;
static HWND btnEcgSetup;
static HWND btnEcgAlmSet;
static HWND btnEcgARRSet;
static HWND btnSTAdjust;
static HWND cobECG1;
static HWND cobECG2;
static HWND cobGain;
static HWND cobSpeed;
static HWND cobBeepVol;
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
		DID_NEWECG_HELP,
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
				case DID_NEWECG_ECG1:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECG, "help_ecg1", strHelp, sizeof strHelp,"Set main ECG channel,this is main analysis lead.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWECG_ECG2:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECG, "help_ecg2", strHelp, sizeof strHelp,"Set second ECG channel,this is second analysis lead.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWECG_GAIN:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECG, "help_gain", strHelp, sizeof strHelp,"Set ECG waveform gain.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWECG_SPEED:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECG, "help_speed", strHelp, sizeof strHelp,"Set ECG wavform scanning speed.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWECG_BEEPVOL:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECG, "help_beepvol", strHelp, sizeof strHelp,"Set volume level.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWECG_ECGSETUP:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECG, "help_ecgsetup", strHelp, sizeof strHelp,"Set ECG parameter.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWECG_ECGALM:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECG, "help_ecgalm", strHelp, sizeof strHelp,"Set ECG alarm .");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWECG_ECGARR:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECG, "help_ecgarr", strHelp, sizeof strHelp,"Set ARR analysis.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWECG_STADJUST:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECG, "help_stadjust", strHelp, sizeof strHelp,"Set ST adjust.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWECG_OK:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "help_backtomain", strHelp, sizeof strHelp,"Return to main screen.");
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
						case DID_NEWECG_ECG1_COB:{
							gCfgEcg.bChannel1 = index;
							
							if(gCfgEcg.bLeadType==ECGTYPE_5LEAD)
								if(gCfgEcg.bChannel1==gCfgEcg.bChannel2)gCfgEcg.bChannel1=(gCfgEcg.bChannel1+1)%7;
							SendMessage(cobECG1, CB_SETCURSEL, gCfgEcg.bChannel1, 0);
							SetIntValueToResFile(gFileSetup, "ECGSetup", "channel1",  gCfgEcg.bChannel1, 1);

							Set_Ecg_Lead();
							SetFocus(btnECG1);
						}break;
						case DID_NEWECG_ECG2_COB:{
							gCfgEcg.bChannel2 = index;
							if(gCfgEcg.bChannel1==gCfgEcg.bChannel2)gCfgEcg.bChannel2=(gCfgEcg.bChannel2+1)%7;
							SendMessage(cobECG2, CB_SETCURSEL, gCfgEcg.bChannel2, 0);
							SetIntValueToResFile(gFileSetup, "ECGSetup", "channel2",  gCfgEcg.bChannel2, 1);

							Set_Ecg_Lead();
							SetFocus(btnECG2);
						}break;
						case DID_NEWECG_GAIN_COB:{
							gCfgEcg.bGain = index;
							SetIntValueToResFile(gFileSetup, "ECGSetup", "gain",  gCfgEcg.bGain, 1);
							SetFocus(btnGain);
							
							CalcEcgRuler();
							
							
						}break;
						case DID_NEWECG_SPEED_COB:{
							 gCfgEcg.bScanSpeed = index; 
							SetIntValueToResFile(gFileSetup, "ECGSetup", "scan_speed",  gCfgEcg.bScanSpeed, 1);
							SetFocus(btnSpeed);
							ChangeWaveSpeed_Ecg();
							
						}break;
						case DID_NEWECG_BEEPVOL_COB:{
							gCfgEcg.bBeepVolume= index;
							SetIntValueToResFile(gFileSetup, "ECGSetup", "beep_volume",  gCfgEcg.bBeepVolume, 2);
							SetFocus(btnBeepVol);	
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
	for(i=5;i<9;i++){
		CreateWindow("static",gstrNextPage,WS_CHILD|WS_VISIBLE,IDC_STATIC,
	 		150, 30+29*i, 30, 16,hDlg,0);
	}
	return(0);
}


/*
	创建控件函数
*/
static int CreateCtlProc(HWND hDlg)
{
	editCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER |  WS_VISIBLE, 
					     DID_NEWECG_CAPTION, 0, 5, 240, 25, hDlg, 0);
	editHelp = GetDlgItem(hDlg, DID_NEWECG_HELP);

	btnECG1 = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECG_ECG1, 10, 30+29*0, 125, 26, hDlg, 0);
	btnECG2 = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECG_ECG2, 10, 30+29*1, 125, 26, hDlg, 0);
	btnGain = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECG_GAIN, 10, 30+29*2, 125, 26, hDlg, 0);
	btnSpeed = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECG_SPEED, 10, 30+29*3, 125, 26, hDlg, 0);
	btnBeepVol = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECG_BEEPVOL, 10, 30+29*4, 125, 26, hDlg, 0);
	btnEcgSetup = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECG_ECGSETUP, 10, 30+29*5, 125, 26, hDlg, 0);
	btnEcgAlmSet = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECG_ECGALM, 10, 30+29*6, 125, 26, hDlg, 0);
	btnEcgARRSet = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECG_ECGARR, 10, 30+29*7, 125, 26, hDlg, 0);
	btnSTAdjust= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECG_STADJUST, 10, 30+29*8, 125, 26, hDlg, 0);
	btnOk = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWECG_OK, 10, 30+29*9, 125, 26, hDlg, 0);

	cobECG1 = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,
					DID_NEWECG_ECG1_COB, 140, 30+29*0, 90, 26, hDlg, 0);
	cobECG2 = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,
					DID_NEWECG_ECG2_COB, 140, 30+29*1, 90, 26, hDlg, 0);
	cobGain = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,
					DID_NEWECG_GAIN_COB, 140, 30+29*2, 90, 26, hDlg, 0);
	cobSpeed = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,
					DID_NEWECG_SPEED_COB, 140, 30+29*3, 90, 26, hDlg, 0);
	cobBeepVol = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_NEWECG_BEEPVOL_COB, 140, 30+29*4, 90, 26, hDlg, 0);
	
	return 0;
}
static int InitCtlName(HWND hDlg)
{
	char strMenu[100];

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECG, "caption", strMenu, sizeof strMenu,"ECG");
	SetWindowText(editCaption, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "backtomain", strMenu, sizeof strMenu,"Back to main");	
	SetWindowText(btnOk, strMenu);

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECG, "ecg1", strMenu, sizeof strMenu,"ECG1");	
	SetWindowText(btnECG1, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECG, "ecg2", strMenu, sizeof strMenu,"ECG2");	
	SetWindowText(btnECG2, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECG, "gain", strMenu, sizeof strMenu,"ECG Gain");	
	SetWindowText(btnGain, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECG, "speed", strMenu, sizeof strMenu,"Scan Speed");	
	SetWindowText(btnSpeed, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECG, "beepvol", strMenu, sizeof strMenu,"Beep Volume");	
	SetWindowText(btnBeepVol, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECG, "ecgset", strMenu, sizeof strMenu,"ECG Setup");	
	SetWindowText(btnEcgSetup, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECG, "ecgalm", strMenu, sizeof strMenu,"Alarm Setup");	
	SetWindowText(btnEcgAlmSet, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECG, "ecgarr", strMenu, sizeof strMenu,"ARR Setup");	
	SetWindowText(btnEcgARRSet, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ECG, "stadjust", strMenu, sizeof strMenu,"ST Adjust");	
	SetWindowText(btnSTAdjust, strMenu);
	
	
	return 0;
}
static int InitECG1List(HWND hDlg)
{
	int i;
	int ecg1count=0;
	unsigned char *strEcg1[7]={"I","II","III","AVR","AVL","AVF","V-"};
	if(gCfgEcg.bLeadType==ECGTYPE_3LEAD)
		ecg1count=3;
	else
		ecg1count=7;
	for(i=0;i<ecg1count;i++){
		SendMessage(cobECG1, CB_ADDSTRING, 0, (LPARAM)strEcg1[i]);
		SendMessage(cobECG2, CB_ADDSTRING, 0, (LPARAM)strEcg1[i]);
	}
	if(gCfgEcg.bLeadType==ECGTYPE_3LEAD&&gCfgEcg.bChannel1>2)gCfgEcg.bChannel1=0;
	SendMessage(cobECG1, CB_SETCURSEL,  gCfgEcg.bChannel1, 0);
	SendMessage(cobECG2, CB_SETCURSEL,  gCfgEcg.bChannel2, 0);
	return 0;
}
static int InitCobList(HWND hDlg)
{
	int i;
	char strMenu[10]={0};
	unsigned char *strGain[5]={"0.25 X","0.5 X","1.0 X","2.0 X"};
	unsigned char *strSpeed[5]={"6.25 mm/s","12.5 mm/s","25 mm/s","50 mm/s"};
	unsigned char *strVolume[10]={"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};
	InitECG1List(hDlg);
	
	for(i=0;i<4;i++){
		SendMessage(cobGain, CB_ADDSTRING, 0, (LPARAM)strGain[i]);
	}
	SendMessage(cobGain, CB_SETCURSEL,   gCfgEcg.bGain, 0);
	
	for(i=0;i<4;i++){
		SendMessage(cobSpeed, CB_ADDSTRING, 0, (LPARAM)strSpeed[i]);
	}
	SendMessage(cobSpeed, CB_SETCURSEL,   gCfgEcg.bScanSpeed, 0);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "off", strMenu, sizeof strMenu,"OFF");	
	SendMessage(cobBeepVol, CB_ADDSTRING, 0, (LPARAM)strMenu);
	for(i=0;i<10;i++){
		SendMessage(cobBeepVol, CB_ADDSTRING, 0, (LPARAM)strVolume[i]);
	}
	SendMessage(cobBeepVol, CB_SETCURSEL,   gCfgEcg.bBeepVolume, 0);

	
	return 0;
}
static int InitCtlCallBackProc(HWND hDlg)
{

	OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnECG1, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnECG2, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnGain, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSpeed, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnBeepVol, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnEcgSetup, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnEcgAlmSet, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnEcgARRSet, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSTAdjust,BtnProc);
	
	OldCobProc = SetWindowCallbackProc(cobECG1, CobProc);
	OldCobProc = SetWindowCallbackProc(cobECG2, CobProc);
	OldCobProc = SetWindowCallbackProc(cobGain, CobProc);
	OldCobProc = SetWindowCallbackProc(cobSpeed, CobProc);
	OldCobProc = SetWindowCallbackProc(cobBeepVol, CobProc);
	OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp);
	
	return 0;
}

static int InitCtlStatus(HWND hDlg)
{
	if(gCfgEcg.bLeadType==ECGTYPE_3LEAD){
		EnableWindow(btnECG2, FALSE);
		EnableWindow(cobECG2, FALSE);
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
			Get_ST_templet();
			Get_Ecg_ST_ISO();
			gbKeyType = KEY_TAB;	
			SetFocus(btnOk);
		}break;
		case MSG_PAINT:{
			

		}break;
		case MSG_COMMAND:{
			int	id  = LOWORD(wParam);
			switch(id){
				case DID_NEWECG_ECG1:{
					gbKeyType = KEY_UD;
					SetFocus(cobECG1);
					}break;
				case DID_NEWECG_ECG2:{
					gbKeyType = KEY_UD;
					SetFocus(cobECG2);
					}break;
				case DID_NEWECG_GAIN:{
					gbKeyType = KEY_UD;
					SetFocus(cobGain);
					}break;
				case DID_NEWECG_SPEED:{
					gbKeyType = KEY_UD;
					SetFocus(cobSpeed);
					}break;
				case DID_NEWECG_BEEPVOL:{
					gbKeyType = KEY_UD;
					SetFocus(cobBeepVol);
					}break;
				case DID_NEWECG_ECGSETUP:{
					CreateDlgNewECGSET(hDlg);
					}break;
				case DID_NEWECG_ECGALM:{
					CreateDlgNewECGALM(hDlg);
					}break;
				case DID_NEWECG_ECGARR:{
					CreateDlgNewECGARR(hDlg);
					}break;
				case DID_NEWECG_STADJUST:{
					CreateDlgNewECGSTAdjust(hDlg);
					}break;
				case DID_NEWECG_OK:{
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
void CreateDlgNewECG(HWND hWnd)
{
 	DlgSet.controls = CtrlSet;
 	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}


/*************************************************************************** 
 *	Module Name:	DlgNew_Resp
 *
 *	Abstract:Ecg设置中的报警设置选项
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2013-1-21 	16:14:18
 *	
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"

//DID_NEWRESP
#define DID_NEWRESP_HELP			DID_NEWRESP
#define DID_NEWRESP_OK			DID_NEWRESP+1
#define DID_NEWRESP_CAPTION		DID_NEWRESP+2
#define DID_NEWRESP_SPEED			DID_NEWRESP+3
#define DID_NEWRESP_GAIN			DID_NEWRESP+4
#define DID_NEWRESP_APNEA			DID_NEWRESP+5
#define DID_NEWRESP_SPEED_COB			DID_NEWRESP+6
#define DID_NEWRESP_GAIN_COB			DID_NEWRESP+7
#define DID_NEWRESP_APNEA_COB			DID_NEWRESP+8
#define DID_NEWRESP_RRTYPE			DID_NEWRESP+9
#define DID_NEWRESP_RRTYPE_COB		DID_NEWRESP+10
#define DID_NEWRESP_RRALM				DID_NEWRESP+11

//帮助对话框
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;

//其他控件 
static HWND btnOk;
static HWND btnSpeed;
static HWND btnGain;
static HWND btnApnea;
static HWND btnRRType;
static HWND btnRRALM;

static HWND cobSpeed;
static HWND cobGain;
static HWND cobApnea;
static HWND cobRRType;



//控件回调函数
static WNDPROC OldBtnProc;
static WNDPROC OldCobProc;
static WNDPROC OldEditProc;

//临时存储上下限，调节的时候使用
static int iTmpHigh_RR =0, iTmpLow_RR = 0;
//更改窒息报警时间
BOOL gbApneaTimeChanged = FALSE;
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
		DID_NEWRESP_HELP,
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
			
				case DID_NEWRESP_SPEED:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_RESP, "help_speed", strHelp, sizeof strHelp,"Set up waveform speed.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWRESP_GAIN:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_RESP, "help_gain", strHelp, sizeof strHelp,"Set up waveform gain.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWRESP_APNEA:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_RESP, "help_apnea", strHelp, sizeof strHelp,"Set up apnea alarm time.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWRESP_RRTYPE:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_RESP, "help_leadtype", strHelp, sizeof strHelp,"Set up Resp leadtype.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWRESP_RRALM:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_RESP, "help_rralarm", strHelp, sizeof strHelp,"Set up Resp alarm.");
					SetWindowText(hWnd, strHelp);
				}break;
				
				case DID_NEWRESP_OK:{
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
							case DID_NEWRESP_SPEED_COB:{
							gCfgResp.bScanSpeed = index;
							SetIntValueToResFile(gFileSetup, "RESPSetup", "scan_speed",  gCfgResp.bScanSpeed, 1);
							SetFocus(btnSpeed);
// 							ChangeWaveX();
							ChangeWaveSpeed_Resp();
						}break;
						case DID_NEWRESP_GAIN_COB:{
							gCfgResp.bGain = index;
							SetIntValueToResFile(gFileSetup, "RESPSetup", "gain",  gCfgResp.bGain, 1);
//							SetiBioRespGain();
							Set_RespGain();
							SetFocus(btnGain);	
						}break;
						case DID_NEWRESP_APNEA_COB:{
							gCfgResp.bApnea= index;
							SetIntValueToResFile(gFileSetup, "RESPSetup", "apnea",  gCfgResp.bApnea, 1);
							if(gCfgResp.bApnea!=0){
								gCfgResp.wApneaTime=gCfgResp.bApnea*5+5;
							}else
								gCfgResp.wApneaTime=0;
							SetIntValueToResFile(gFileSetup, "RESPSetup", "apnea_time",  gCfgResp.wApneaTime,3);
						
							SetiBioRespApnea();
							SetFocus(btnApnea);	
							gbApneaTimeChanged = TRUE;
						}break;
						case DID_NEWRESP_RRTYPE_COB:{
							gCfgResp.bLeadType= index;
							SetIntValueToResFile(gFileSetup, "RESPSetup", "leadtype",  gCfgResp.bLeadType, 1);
							
//							SetiBioRespType(gCfgResp.bLeadType);
							Set_RespLead();
							SetFocus(btnRRType);	
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
 		150, 33+30*4, 30, 16,hDlg,0);
	
	return(0);
}

	
/*
	创建控件函数
*/
static int CreateCtlProc(HWND hDlg)
{
	editCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE, 
					     DID_NEWRESP_CAPTION, 0, 5, 240, 25, hDlg, 0);
	editHelp = GetDlgItem(hDlg, DID_NEWRESP_HELP);


	btnSpeed = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWRESP_SPEED, 10, 30+30*0, 125, 26, hDlg, 0);
	btnGain = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWRESP_GAIN, 10, 30+30*1, 125, 26, hDlg, 0);
	btnApnea = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWRESP_APNEA, 10, 30+30*2, 125, 26, hDlg, 0);
	btnRRType= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWRESP_RRTYPE, 10, 30+30*3, 125, 26, hDlg, 0);
	btnRRALM= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWRESP_RRALM, 10, 30+30*4, 125, 26, hDlg, 0);
	
	btnOk = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_NEWRESP_OK, 10, 30+30*5, 125, 26, hDlg, 0);

	cobSpeed = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_NEWRESP_SPEED_COB, 140, 30+30*0, 95, 26, hDlg, 0);
	cobGain = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_NEWRESP_GAIN_COB, 140, 30+30*1, 95, 26, hDlg, 0);
	cobApnea = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_NEWRESP_APNEA_COB, 140, 30+30*2, 95, 26, hDlg, 0);
	cobRRType= CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_NEWRESP_RRTYPE_COB, 140, 30+30*3, 95, 26, hDlg, 0);
	return 0;
}
static int InitCtlName(HWND hDlg)
{
	char strMenu[100];

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_RESP, "caption", strMenu, sizeof strMenu,"RESP");
	SetWindowText(editCaption, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "previous", strMenu, sizeof strMenu,"Previous");	
	SetWindowText(btnOk, strMenu);

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_RESP, "speed", strMenu, sizeof strMenu,"Speed");	
	SetWindowText(btnSpeed, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_RESP, "gain", strMenu, sizeof strMenu,"Gain");	
	SetWindowText(btnGain, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_RESP, "apnea", strMenu, sizeof strMenu,"Apnea");	
	SetWindowText(btnApnea, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_RESP, "rrtype", strMenu, sizeof strMenu,"RR Type");	
	SetWindowText(btnRRType, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_RESP, "rralm", strMenu, sizeof strMenu,"RR Alarm");	
	SetWindowText(btnRRALM, strMenu);
	
	
	return 0;
}

//初始化combobox
static int InitCobList(HWND hDlg)
{
	int i;
	char strMenu[100];
	unsigned char *strApnea[8]={"OFF","10 s","15 s","20 s","25 s","30 s","35 s","40 s"};
	//speed
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "speed_625", strMenu, sizeof strMenu,"6.25mm/s");	
	SendMessage(cobSpeed, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "speed_125", strMenu, sizeof strMenu,"12.5mm/s");	
	SendMessage(cobSpeed, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "speed_25", strMenu, sizeof strMenu,"25mm/s");	
	SendMessage(cobSpeed, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobSpeed, CB_SETCURSEL, gCfgResp.bScanSpeed, 0);
	//gain

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "gain_025", strMenu, sizeof strMenu,"X0.25");	
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
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "gain_4", strMenu, sizeof strMenu,"X4");	
	SendMessage(cobGain, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "gain_8", strMenu, sizeof strMenu,"X8");	
	SendMessage(cobGain, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "gain_16", strMenu, sizeof strMenu,"X16");	
	SendMessage(cobGain, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "gain_32", strMenu, sizeof strMenu,"X32");	
	SendMessage(cobGain, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobGain, CB_SETCURSEL, gCfgResp.bGain, 0);
	//apnea
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "off", strMenu, sizeof strMenu,"OFF");	
	SendMessage(cobApnea, CB_ADDSTRING, 0, (LPARAM)strMenu);
	for(i=1;i<8;i++){
		SendMessage(cobApnea, CB_ADDSTRING, 0, (LPARAM)strApnea[i]);
	}
	SendMessage(cobApnea, CB_SETCURSEL, gCfgResp.bApnea, 0);
	//RRtype
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "off", strMenu, sizeof strMenu,"OFF");	
	SendMessage(cobRRType, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "I_Iead", strMenu, sizeof strMenu,"I Lead");	
	SendMessage(cobRRType, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "II_Iead", strMenu, sizeof strMenu,"II Lead");	
	SendMessage(cobRRType, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobRRType, CB_SETCURSEL, gCfgResp.bLeadType, 0);
	
	return 0;
}

static int InitCtlCallBackProc(HWND hDlg)
{
	OldBtnProc = SetWindowCallbackProc(btnSpeed, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnGain, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnApnea, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnRRType, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnRRALM, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);
	
	OldCobProc = SetWindowCallbackProc(cobSpeed, CobProc);
	OldCobProc = SetWindowCallbackProc(cobGain, CobProc);
	OldCobProc = SetWindowCallbackProc(cobApnea, CobProc);
	OldCobProc = SetWindowCallbackProc(cobRRType, CobProc);

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
			InitCtlStatus(hDlg);//获得限值
		
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
			
				case DID_NEWRESP_SPEED:{
					gbKeyType = KEY_UD;
					SetFocus(cobSpeed);
					}break;
				case DID_NEWRESP_GAIN:{
					gbKeyType = KEY_UD;
					SetFocus(cobGain);
					}break;
				case DID_NEWRESP_APNEA:{
					gbKeyType = KEY_UD;
					SetFocus(cobApnea);
					}break;
				case DID_NEWRESP_RRTYPE:{
					gbKeyType = KEY_UD;
					SetFocus(cobRRType);
					}break;
				case DID_NEWRESP_RRALM:{
					CreateDlgNewRESPALM(hDlg);
					}break;
				case DID_NEWRESP_OK:{
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
void CreateDlgNewRESP(HWND hWnd)
{
 	DlgSet.controls = CtrlSet;
 	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}


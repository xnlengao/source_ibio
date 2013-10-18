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

//DID_MAINTNIBP
#define DID_MAINTNIBP_HELP			DID_MAINTNIBP
#define DID_MAINTNIBP_OK			DID_MAINTNIBP+1
#define DID_MAINTNIBP_CAPTION		DID_MAINTNIBP+2
#define DID_MAINTNIBP_LEAK			DID_MAINTNIBP+3
#define DID_MAINTNIBP_RESET			DID_MAINTNIBP+4
#define DID_MAINTNIBP_PUMPTEST		DID_MAINTNIBP+5
#define DID_MAINTNIBP_OPTEST		DID_MAINTNIBP+6
#define DID_MAINTNIBP_CAL			DID_MAINTNIBP+7
#define DID_MAINTNIBP_STOPCAL		DID_MAINTNIBP+8
#define DID_MAINTNIBP_INFO			DID_MAINTNIBP+9


//帮助对话框
static HWND editHelp;
static HWND editCaption;


//其他控件 
static HWND btnOk;
static HWND btnLeakTest;
static HWND btnReset;
static HWND btnPumpTest;
static HWND btnOPTest;
static HWND btnCal;
static HWND btnStopCal;
static HWND stcInfo;
//控件回调函数
static WNDPROC OldBtnProc;
static WNDPROC OldCobProc;
static WNDPROC OldEditProcHelp;
static WNDPROC OldStcProcHelp;

//NIBP漏气检测标志位
extern BOOL gbNibpLeak;



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
		DID_MAINTNIBP_HELP,
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
				case DID_MAINTNIBP_OK:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "help_previous", strHelp, sizeof strHelp,"Return to the previous menu.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MAINTNIBP_RESET:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "help_reset", strHelp, sizeof strHelp,"NIBP reset.");	
					SetWindowText(hWnd, strHelp);
				}break;
				
				case DID_MAINTNIBP_LEAK:{
					memset(strHelp, 0, sizeof strHelp);
					
					if(wParam==0)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "help_leak", strHelp, sizeof strHelp,"NIBP leak test.");	

					if(wParam==NBP_FDB_LEAK_START_OK)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "leaktest_start", strHelp, sizeof strHelp,"Leak testing.");
					if(wParam==NBP_FDB_LEAK_START_ABT)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "leaktest_startabort", strHelp, sizeof strHelp,"Leak abort.");
					if(wParam == NBP_FDB_LEAK_YES)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "leaktest_yes", strHelp, sizeof strHelp,"NIBP Pneumatic Leak.");
					if(wParam == NBP_FDB_LEAK_NO)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "leaktest_no", strHelp, sizeof strHelp,"NIBP Pneumatic No Leak.");
					if(wParam == NBP_FDB_LEAK_PUMPERR)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "leaktest_pumperr", strHelp, sizeof strHelp,"NIBP Pneumatic pump error.");

					SetWindowText(hWnd, strHelp);
					
				}break;
				case DID_MAINTNIBP_PUMPTEST:{
					memset(strHelp, 0, sizeof strHelp);
					if(wParam==0)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "help_pump", strHelp, sizeof strHelp,"NIBP pump test.");	

					if(wParam==NBP_FDB_PVT_START_OK)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "pumptest_start", strHelp, sizeof strHelp,"pump testing.");
					if(wParam==NBP_FDB_PVT_START_ABT)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "pumptest_startabort", strHelp, sizeof strHelp,"pump abort.");
					if(wParam == NBP_FDB_LEAK_YES)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "pumptest_ok", strHelp, sizeof strHelp,"pump test ok.");
					if(wParam == NBP_FDB_LEAK_NO)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "pumptest_no", strHelp, sizeof strHelp,"pump test fail.");
					if(wParam == NBP_FDB_LEAK_PUMPERR)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "pumptest_pumperr", strHelp, sizeof strHelp," test pump error.");

					SetWindowText(hWnd, strHelp);
					
				}break;
				case DID_MAINTNIBP_OPTEST:{
					 
					memset(strHelp, 0, sizeof strHelp);
					if(wParam==0)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "help_opt", strHelp, sizeof strHelp,"NIBP over pressure test.");	
	
					if(wParam==NBP_FDB_OVP_START_OK)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "optest_start", strHelp, sizeof strHelp,"Over pressure testing.");
					if(wParam==NBP_FDB_OVP_START_ABT)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "optest_startabort", strHelp, sizeof strHelp,"Over pressure test abort.");
					if(wParam == NBP_FDB_OVP_NEO_OK)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "optest_neo_ok", strHelp, sizeof strHelp,"NIBP Neonate /Pediactric Overpressue protection ok.");
					if(wParam == NBP_FDB_OVP_NEO_FAIL)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "optest_neo_no", strHelp, sizeof strHelp,"NIBP Neonate /Pediactric Overpressue protection fail.");
					if(wParam == NBP_FDB_OVP_ADU_OK)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "optest_adu_ok", strHelp, sizeof strHelp,"NIBP Adult Overpressue protection ok.");
					if(wParam == NBP_FDB_OVP_ADU_FAIL)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "optest_adu_no", strHelp, sizeof strHelp,"NIBP Adult Overpressue protection fail.");
					if(wParam == NBP_FDB_OVP_PNEU_ERR)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "optest_pneu_err", strHelp, sizeof strHelp,"NIBP Adult Overpressue protection fail.");

					SetWindowText(hWnd, strHelp);
					
				}break;
				case DID_MAINTNIBP_CAL:{
					
					memset(strHelp, 0, sizeof strHelp);
					if(wParam==0)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "help_cal", strHelp, sizeof strHelp,"Start NIBP Calibration and set input pressure to 200mmHg.");	

					if(wParam==1)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "cal_status_1", strHelp, sizeof strHelp,"Set input press to 200mmHg.");
			
					if(wParam==NBP_FDB_CAL_START_OK)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "cal_start", strHelp, sizeof strHelp,"NIBP calibrating.");
					if(wParam==NBP_FDB_CAL_START_ABT)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "cal_start_abort", strHelp, sizeof strHelp,"NIBP calibrating abort.");
					if(wParam == NBP_FDB_CAL_EXPRS_ERR)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "cal_exprs_err", strHelp, sizeof strHelp,"NIBP exprs error.");
					if(wParam == NBP_FDB_CAL_SMC_OT)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "cal_smc_ot", strHelp, sizeof strHelp,"NIBP smc out time.");
					if(wParam == NBP_FDB_CAL_ZERO_OT)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "cal_zero_ot", strHelp, sizeof strHelp,"NIBP zero out time.");
					if(wParam == NBP_FDB_CAL_E200_OT)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "cal_e200_ot", strHelp, sizeof strHelp,"NIBP E200 out time.");
					if(wParam == NBP_FDB_CAL_SPAN_OT)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "cal_span_ot", strHelp, sizeof strHelp,"NIBP span out time.");
					if(wParam == NBP_FDB_CAL_SAVE_OT)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "cal_save_ot", strHelp, sizeof strHelp,"NIBP save out time.");
					if(wParam == NBP_FDB_CAL_DONE_OK)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "cal_done_ok", strHelp, sizeof strHelp,"NIBP calibration ok.");
					if(wParam == NBP_FDB_CAL_DATA_ERR)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "cal_data_err", strHelp, sizeof strHelp,"NIBP data error.");

					SetWindowText(hWnd, strHelp);
					
				}break;
				case DID_MAINTNIBP_STOPCAL:{
					memset(strHelp, 0, sizeof strHelp);
					
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "help_stopcal", strHelp, sizeof strHelp,"Stop NIBP calibration.");	
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
	帮助控件回调函数
*/
static int StcProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
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
			switch(lParam){
				case DID_MAINTNIBP_LEAK:{
					memset(strHelp, 0, sizeof strHelp);
					
					if(wParam==0)
						strcpy(strHelp,"leak test");
					if(wParam==NBP_FDB_LEAK_START_OK)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "leaktest_start", strHelp, sizeof strHelp,"Leak testing.");
					if(wParam==NBP_FDB_LEAK_START_ABT)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "leaktest_startabort", strHelp, sizeof strHelp,"Leak abort.");
					if(wParam == NBP_FDB_LEAK_YES)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "leaktest_ok", strHelp, sizeof strHelp,"NIBP Pneumatic Leak.");
					if(wParam == NBP_FDB_LEAK_NO)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "leaktest_no", strHelp, sizeof strHelp,"NIBP Pneumatic No Leak.");
					if(wParam == NBP_FDB_LEAK_PUMPERR)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "leaktest_pumperr", strHelp, sizeof strHelp,"NIBP Pneumatic pump error.");

					SetWindowText(hWnd, strHelp);
					
				}break;
				case DID_MAINTNIBP_PUMPTEST:{
					memset(strHelp, 0, sizeof strHelp);
					if(wParam==0)
						strcpy(strHelp,"pump test");
					if(wParam==NBP_FDB_PVT_START_OK)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "pumptest_start", strHelp, sizeof strHelp,"pump testing.");
					if(wParam==NBP_FDB_PVT_START_ABT)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "pumptest_startabort", strHelp, sizeof strHelp,"pump abort.");
					if(wParam == NBP_FDB_LEAK_YES)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "pumptest_ok", strHelp, sizeof strHelp,"pump test ok.");
					if(wParam == NBP_FDB_LEAK_NO)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "pumptest_no", strHelp, sizeof strHelp,"pump test fail.");
					if(wParam == NBP_FDB_LEAK_PUMPERR)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "pumptest_pumperr", strHelp, sizeof strHelp," test pump error.");

					SetWindowText(hWnd, strHelp);
					
				}break;
				case DID_MAINTNIBP_OPTEST:{
					 
					memset(strHelp, 0, sizeof strHelp);
					if(wParam==0)
						strcpy(strHelp,"optest");
					if(wParam==NBP_FDB_OVP_START_OK)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "optest_start", strHelp, sizeof strHelp,"Over pressure testing.");
					if(wParam==NBP_FDB_OVP_START_ABT)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "optest_startabort", strHelp, sizeof strHelp,"Over pressure test abort.");
					if(wParam == NBP_FDB_OVP_NEO_OK)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "optest_neo_ok", strHelp, sizeof strHelp,"NIBP Neonate /Pediactric Overpressue protection ok.");
					if(wParam == NBP_FDB_OVP_NEO_FAIL)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "optest_neo_no", strHelp, sizeof strHelp,"NIBP Neonate /Pediactric Overpressue protection fail.");
					if(wParam == NBP_FDB_OVP_ADU_OK)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "optest_adu_ok", strHelp, sizeof strHelp,"NIBP Adult Overpressue protection ok.");
					if(wParam == NBP_FDB_OVP_ADU_FAIL)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "optest_adu_no", strHelp, sizeof strHelp,"NIBP Adult Overpressue protection fail.");
					if(wParam == NBP_FDB_OVP_PNEU_ERR)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "optest_pneu_err", strHelp, sizeof strHelp,"NIBP Adult Overpressue protection fail.");

					SetWindowText(hWnd, strHelp);
					
				}break;
				case DID_MAINTNIBP_CAL:{
					
					memset(strHelp, 0, sizeof strHelp);
					if(wParam==0)
						strcpy(strHelp,"cal start");
					if(wParam==1)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "cal_status_1", strHelp, sizeof strHelp,"Set input press to 200mmHg.");
					if(wParam==2)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "cal_status_2", strHelp, sizeof strHelp,"cal_status_2.");
					if(wParam==3)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "cal_status_3", strHelp, sizeof strHelp,"cal_status_3.");

					if(wParam==NBP_FDB_CAL_START_OK)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "cal_start", strHelp, sizeof strHelp,"NIBP calibrating.");
					if(wParam==NBP_FDB_CAL_START_ABT)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "cal_start_abort", strHelp, sizeof strHelp,"NIBP calibrating abort.");
					if(wParam == NBP_FDB_CAL_EXPRS_ERR)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "cal_exprs_err", strHelp, sizeof strHelp,"NIBP exprs error.");
					if(wParam == NBP_FDB_CAL_SMC_OT)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "cal_smc_ot", strHelp, sizeof strHelp,"NIBP smc out time.");
					if(wParam == NBP_FDB_CAL_ZERO_OT)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "cal_zero_ot", strHelp, sizeof strHelp,"NIBP zero out time.");
					if(wParam == NBP_FDB_CAL_E200_OT)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "cal_e200_ot", strHelp, sizeof strHelp,"NIBP E200 out time.");
					if(wParam == NBP_FDB_CAL_SPAN_OT)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "cal_span_ot", strHelp, sizeof strHelp,"NIBP span out time.");
					if(wParam == NBP_FDB_CAL_SAVE_OT)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "cal_save_ot", strHelp, sizeof strHelp,"NIBP save out time.");
					if(wParam == NBP_FDB_CAL_DONE_OK)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "cal_done_ok", strHelp, sizeof strHelp,"NIBP calibration ok.");
					if(wParam == NBP_FDB_CAL_DATA_ERR)
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "cal_data_err", strHelp, sizeof strHelp,"NIBP data error.");

					SetWindowText(hWnd, strHelp);
					
				}break;
				
				default:{
					SetWindowText(hWnd, "");
				}break;
			}break;
		}break;	
	}

	temp = (*OldStcProcHelp)(hWnd, message, wParam, lParam);
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
	创建控件函数
*/
static int CreateCtlProc(HWND hDlg)
{

	editCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE, 
					     DID_MAINTNIBP_CAPTION, 0, 5, 240, 25, hDlg, 0);
	editHelp = GetDlgItem(hDlg, DID_MAINTNIBP_HELP);

	btnLeakTest= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MAINTNIBP_LEAK, 10, 30+30*0, 160, 26, hDlg, 0);
//	btnPumpTest= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
//					     DID_MAINTNIBP_PUMPTEST,10, 30+30*1, 160, 26, hDlg, 0);
	btnOPTest= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MAINTNIBP_OPTEST,10, 30+30*1, 160, 26, hDlg, 0);
	
	btnCal= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MAINTNIBP_CAL,10, 30+30*2, 160, 26, hDlg, 0);
	btnStopCal= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MAINTNIBP_STOPCAL,10, 30+30*3, 160, 26, hDlg, 0);

	btnReset= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MAINTNIBP_RESET, 10, 30+30*4, 160, 26, hDlg, 0);
	btnOk = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MAINTNIBP_OK, 10, 30+30*5, 160, 26, hDlg, 0);
	
//	stcInfo=CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_LEFT | WS_VISIBLE  ,
//					     		DID_MAINTNIBP_INFO, 15, 30+29*8, 240, 52, hDlg, 0);
	
			
	return 0;
}
static int InitCtlName(HWND hDlg)
{
	char strMenu[100];

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "caption", strMenu, sizeof strMenu,"NIBP Maintenance");
	SetWindowText(editCaption, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "previous", strMenu, sizeof strMenu,"Previous");	
	SetWindowText(btnOk, strMenu);
	
	
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "leaktest", strMenu, sizeof strMenu,"Leak Test");	
	SetWindowText(btnLeakTest, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "pumptest", strMenu, sizeof strMenu,"Pump Test");	
	SetWindowText(btnPumpTest, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "optest", strMenu, sizeof strMenu,"Over Pressure Test");	
	SetWindowText(btnOPTest, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "cal", strMenu, sizeof strMenu,"start cal");	
	SetWindowText(btnCal, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "stopcal", strMenu, sizeof strMenu,"Stop Cal");	
	SetWindowText(btnStopCal, strMenu);

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "reset", strMenu, sizeof strMenu,"Reset");	
	SetWindowText(btnReset, strMenu);
	
	return 0;
}



static int InitCtlCallBackProc(HWND hDlg)
{

	OldBtnProc = SetWindowCallbackProc(btnLeakTest, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnReset, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnPumpTest, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnOPTest, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnCal, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnStopCal, BtnProc);

	OldStcProcHelp=SetWindowCallbackProc(stcInfo, StcProc);

	
	OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp);
	
	return 0;
}

static int InitCtlStatus(HWND hDlg)
{
	
	
	return 0;
}
int update_Leaktest_info()
{
	
	SendMessage(editHelp, MSG_PAINT, gValueNibp.wFDB,DID_MAINTNIBP_LEAK);
	return 0;
}
int update_pumptest_info()
{
	SendMessage(editHelp, MSG_PAINT, gValueNibp.wFDB,DID_MAINTNIBP_PUMPTEST);
	return 0;
}

int update_overpressuretest_info()
{
	SendMessage(editHelp, MSG_PAINT, gValueNibp.wFDB,DID_MAINTNIBP_OPTEST);
	return 0;
}
int update_cal_info()
{
	SendMessage(editHelp, MSG_PAINT, gValueNibp.wFDB,DID_MAINTNIBP_CAL);
	return 0;
}
int update_cal_info_1()
{
	SendMessage(editHelp, MSG_PAINT, gValueNibp.wFDB,DID_MAINTNIBP_STOPCAL);	
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
				case DID_MAINTNIBP_LEAK:{
					if(gValueNibp.bStatus==NIBP_SYSTEM_IDLE){
						StartNIBPLeakTest();
					}else{
						
						StopNibpMeasure();
						SendMessage(editHelp, MSG_PAINT, 0, DID_MAINTNIBP_LEAK);
						
					}
					
				}break;
				case DID_MAINTNIBP_PUMPTEST:{
					if(gValueNibp.bStatus==NIBP_SYSTEM_IDLE){
						 
						StartNIBPPumpTest();
						
					}else{
						
						StopNibpMeasure();
						SendMessage(editHelp, MSG_PAINT, 0, DID_MAINTNIBP_PUMPTEST);
						
					}
						
				}break;
				case DID_MAINTNIBP_OPTEST:{
					if(gValueNibp.bStatus==NIBP_SYSTEM_IDLE){
						 
						StartNIBPOverPressureTest(gCfgNibp.bObject);
						
					}else{
						
						StopNibpMeasure();
						SendMessage(editHelp, MSG_PAINT, 0, DID_MAINTNIBP_OPTEST);
						
					}
				
				}break;
				case DID_MAINTNIBP_CAL:{
					
					if(gValueNibp.bStatus==NIBP_SYSTEM_IDLE){
							StartNibpCalTest();
							
						BOOL res = FALSE;
					
						memset(strMenu, 0, sizeof strMenu);
						GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "ackinfo_nibpcal", strMenu, sizeof strMenu,"make sure input pressure is 200mmHg.");	
						res = ACKDialog(hDlg, strMenu, ACK_ATTENTION);
						
						if(res){
							 	NibpCalPressure();
								SetFocus(btnStopCal);
								EnableWindow(btnCal, FALSE);
							
						}else{
								StopNibpMeasure();
						}
					
					}else{
						StopNibpMeasure();
					}
					
				}break;
				
				case DID_MAINTNIBP_STOPCAL:{
					
					StopNibpMeasure();
					EnableWindow(btnCal, TRUE);
					memset(strMenu, 0, sizeof strMenu);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBPMAINT, "cal", strMenu, sizeof strMenu,"start cal");	
					SetWindowText(btnCal, strMenu);
								
				}break;
				case DID_MAINTNIBP_RESET:{
					
					Nibp_Reset();
					gCfgNibp.bObject=NIBP_OBJECT_ADULT;
					gCfgNibp.bMode=NIBP_MODE_MANUAL;
					SetIntValueToResFile(gFileSetup, "NIBPSetup", "object",  gCfgNibp.bObject, 1);
					SetIntValueToResFile(gFileSetup, "NIBPSetup", "mode",  gCfgNibp.bMode, 1);
					
				}break;
				case DID_MAINTNIBP_OK:{
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
void CreateMaintNIBP(HWND hWnd)
{
 	DlgSet.controls = CtrlSet;
 	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}


/*************************************************************************** 
 *	Module Name:	DlgNew_AlarmSetup
 *
 *	Abstract:报警设置
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2012-12-13 14:14:18
 *	
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"

//DID_COMMONALM
#define DID_COMMONALM_HELP			DID_COMMONALM
#define DID_COMMONALM_OK				DID_COMMONALM+1
#define DID_COMMONALM_CAPTION		DID_COMMONALM+2
#define DID_COMMONALM_HR				DID_COMMONALM+3
#define DID_COMMONALM_SPO2			DID_COMMONALM+4
#define DID_COMMONALM_NIBPSYS		DID_COMMONALM+5
#define DID_COMMONALM_NIBPDIA			DID_COMMONALM+6
#define DID_COMMONALM_NIBPMEAN		DID_COMMONALM+7
#define DID_COMMONALM_RESP			DID_COMMONALM+8
#define DID_COMMONALM_T1				DID_COMMONALM+9
#define DID_COMMONALM_T2				DID_COMMONALM+10
#define DID_COMMONALM_DEFAULT		DID_COMMONALM+11

#define DID_COMMONALM_HR_COB			DID_COMMONALM+12
#define DID_COMMONALM_SPO2_COB			DID_COMMONALM+13
#define DID_COMMONALM_NIBPSYS_COB			DID_COMMONALM+14
#define DID_COMMONALM_NIBPDIA_COB			DID_COMMONALM+15
#define DID_COMMONALM_NIBPMEAN_COB			DID_COMMONALM+16
#define DID_COMMONALM_RESP_COB			DID_COMMONALM+17
#define DID_COMMONALM_T1_COB				DID_COMMONALM+18
#define DID_COMMONALM_T2_COB				DID_COMMONALM+19

#define DID_COMMONALM_HR_HIGH			DID_COMMONALM+20
#define DID_COMMONALM_SPO2_HIGH			DID_COMMONALM+21
#define DID_COMMONALM_NIBPSYS_HIGH			DID_COMMONALM+22
#define DID_COMMONALM_NIBPDIA_HIGH			DID_COMMONALM+23
#define DID_COMMONALM_NIBPMEAN_HIGH			DID_COMMONALM+24
#define DID_COMMONALM_RESP_HIGH			DID_COMMONALM+25
#define DID_COMMONALM_T1_HIGH			DID_COMMONALM+26
#define DID_COMMONALM_T2_HIGH			DID_COMMONALM+27

#define DID_COMMONALM_HR_LOW				DID_COMMONALM+28
#define DID_COMMONALM_SPO2_LOW			DID_COMMONALM+29
#define DID_COMMONALM_NIBPSYS_LOW			DID_COMMONALM+30
#define DID_COMMONALM_NIBPDIA_LOW			DID_COMMONALM+31
#define DID_COMMONALM_NIBPMEAN_LOW			DID_COMMONALM+32
#define DID_COMMONALM_RESP_LOW			DID_COMMONALM+33
#define DID_COMMONALM_T1_LOW				DID_COMMONALM+34
#define DID_COMMONALM_T2_LOW				DID_COMMONALM+35


//上下限刻度调整的增量
#define LIMIT_INTER		1	
//限值
#define HR_LIMIT_MAX		HR_MAX
#define HR_LIMIT_MIN		HR_MIN
#define SPO2_LIMIT_MAX		SPO2_MAX
#define SPO2_LIMIT_MIN		SPO2_MIN
#define NIBPSYS_LIMIT_MAX		NIBP_MAX
#define NIBPSYS_LIMIT_MIN		NIBP_MIN
#define NIBPDIA_LIMIT_MAX		NIBP_MAX
#define NIBPDIA_LIMIT_MIN		NIBP_MIN
#define NIBPMEAN_LIMIT_MAX		NIBP_MAX
#define NIBPMEAN_LIMIT_MIN		NIBP_MIN
#define RESP_LIMIT_MAX		RR_MAX
#define RESP_LIMIT_MIN		RR_MIN
#define T1_LIMIT_MAX		TEMP_MAX
#define T1_LIMIT_MIN		TEMP_MIN
#define T2_LIMIT_MAX		TEMP_MAX
#define T2_LIMIT_MIN		TEMP_MIN

//帮助对话框
static HWND editHelp;
static HWND editCaption;

//控件回调函数
static WNDPROC OldEditProcHelp;
static WNDPROC OldEditProc;
static WNDPROC OldBtnProc;
static WNDPROC OldCobProc;

//其他控件 
static HWND btnOk;
static HWND btnHR;
static HWND btnSpo2;
static HWND btnNIBPSys;
static HWND btnNIBPDia;
static HWND btnNIBPMean;
static HWND btnResp;
static HWND btnT1;
static HWND btnT2;
static HWND btnDefault;

static HWND cobHR;
static HWND highHR;
static HWND lowHR;
static HWND cobSpo2;
static HWND highSpo2;
static HWND lowSpo2;
static HWND cobNIBPSys;
static HWND highNIBPSys;
static HWND lowNIBPSys;
static HWND cobNIBPDia;
static HWND highNIBPDia;
static HWND lowNIBPDia;
static HWND cobNIBPMean;
static HWND highNIBPMean;
static HWND lowNIBPMean;
static HWND cobResp;
static HWND highResp;
static HWND lowResp;
static HWND cobT1;
static HWND highT1;
static HWND lowT1;
static HWND cobT2;
static HWND highT2;
static HWND lowT2;


static char strOff[100];
static char strLow[100];
static char strMid[100];
static char strHigh[100];
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
		DID_COMMONALM_HELP,
		"",
		0
	}
};

//临时存储上下限，调节的时候使用
static int iTmpHighHR =0, iTmpLowHR = 0;
static int iTmpHighSpo2 =0, iTmpLowSpo2 = 0;
static int iTmpHighNIBPSys =0, iTmpLowNIBPSys = 0;
static int iTmpHighNIBPDia =0, iTmpLowNIBPDia = 0;
static int iTmpHighNIBPMean =0, iTmpLowNIBPMean = 0;
static int iTmpHighResp =0, iTmpLowResp = 0;
static int iTmpHighT1=0, iTmpLowT1= 0;
static int iTmpHighT2=0, iTmpLowT2 = 0;
static int flag=0;
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
				case DID_COMMONALM_HR:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSETUP, "help_hr", strHelp, sizeof strHelp,"Set alarm for HR.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_COMMONALM_SPO2:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSETUP, "help_spo2", strHelp, sizeof strHelp,"Set alarm for SPO2.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_COMMONALM_NIBPSYS:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes,STR_SETTING_DLG_ALMSETUP, "help_nibpsys", strHelp, sizeof strHelp,"Set alarm for NIBP Sys.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_COMMONALM_NIBPDIA:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSETUP, "help_nibpdia", strHelp, sizeof strHelp,"Set alarm for NIBP Dia.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_COMMONALM_NIBPMEAN:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSETUP, "help_nibpmean", strHelp, sizeof strHelp,"Set alarm for NIBP Mean.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_COMMONALM_RESP:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSETUP, "help_resp", strHelp, sizeof strHelp,"Set alarm for Resp.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_COMMONALM_T1:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSETUP, "help_temp1", strHelp, sizeof strHelp,"Set alarm for TEMP1.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_COMMONALM_T2:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSETUP, "help_temp2", strHelp, sizeof strHelp,"Set alarm for TEMP2.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_COMMONALM_DEFAULT:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSETUP, "help_default", strHelp, sizeof strHelp,"Default alarm limit.");
					SetWindowText(hWnd, strHelp);
				}break;
				
				case DID_COMMONALM_OK:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "help_previous", strHelp, sizeof strHelp,"Return to the previous menu.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_COMMONALM_HR_HIGH	:		
				case DID_COMMONALM_SPO2_HIGH:			
				case DID_COMMONALM_NIBPSYS_HIGH:			
				case DID_COMMONALM_NIBPDIA_HIGH	:	
				case DID_COMMONALM_NIBPMEAN_HIGH:		
				case DID_COMMONALM_RESP_HIGH:	
				case DID_COMMONALM_T1_HIGH	:		
				case DID_COMMONALM_T2_HIGH	:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSETUP, "help_high", strHelp, sizeof strHelp,"Set alarm upper limit.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_COMMONALM_HR_LOW	:		
				case DID_COMMONALM_SPO2_LOW:			
				case DID_COMMONALM_NIBPSYS_LOW:			
				case DID_COMMONALM_NIBPDIA_LOW	:	
				case DID_COMMONALM_NIBPMEAN_LOW:		
				case DID_COMMONALM_RESP_LOW:	
				case DID_COMMONALM_T1_LOW	:		
				case DID_COMMONALM_T2_LOW	:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSETUP, "help_low", strHelp, sizeof strHelp,"Set alarm lower limit.");
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

//设置上下限文字
static int SetLimitText(HWND hWnd, int iLimit)
{
	unsigned char strLimit[15] = {0};
	if(hWnd==highT1||hWnd==highT2||hWnd==lowT1||hWnd==lowT2)
		snprintf(strLimit, sizeof strLimit, "%.1f", (float)(iLimit/10.0));
	else
		snprintf(strLimit, sizeof strLimit, "%d", iLimit);
	SetWindowText(hWnd, strLimit);
	
	return 0;
}
/*
	编辑框回调函数
*/

static int EditProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int iVaue;
	int id;
	RECT rc;
	HDC hdc= (HDC)NULL;
	
	GetWindowRect(hWnd, &rc);
	switch(message){
		case MSG_SETFOCUS:{
			hdc = GetClientDC(GetParent(hWnd));
			if(hdc != (HDC)NULL){
				SetPenColor(hdc, SETFOCUS_COLOR);
				Rectangle(hdc, rc.left-1, rc.top-1, rc.right+1, rc.bottom+1);
				Rectangle(hdc, rc.left-2, rc.top-2, rc.right+2, rc.bottom+2);
				ReleaseDC(hdc);
				SendMessage(editHelp, MSG_PAINT, 0, (LPARAM)GetDlgCtrlID(hWnd));
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
		case MSG_KEYUP:{
			id = GetDlgCtrlID(hWnd);
			
			switch(wParam){
				case SCANCODE_CURSORBLOCKDOWN:{
					switch(id){
						case DID_COMMONALM_HR_HIGH:{
							if(iTmpHighHR<HR_LIMIT_MAX){
								iTmpHighHR += LIMIT_INTER;
								SetLimitText(hWnd, iTmpHighHR);
							}
						}break;
						case DID_COMMONALM_SPO2_HIGH:{
							if(iTmpHighSpo2<SPO2_LIMIT_MAX){
								iTmpHighSpo2 += LIMIT_INTER;
								SetLimitText(hWnd, iTmpHighSpo2);
							}
						}break;
						case DID_COMMONALM_NIBPSYS_HIGH:{
							if(iTmpHighNIBPSys<NIBPSYS_LIMIT_MAX){
								iTmpHighNIBPSys += LIMIT_INTER;
								SetLimitText(hWnd, iTmpHighNIBPSys);
							}
						}break;
						case DID_COMMONALM_NIBPDIA_HIGH:{
							if(iTmpHighNIBPDia<NIBPDIA_LIMIT_MAX){
								iTmpHighNIBPDia += LIMIT_INTER;
								SetLimitText(hWnd, iTmpHighNIBPDia);
							}
						}break;
						case DID_COMMONALM_NIBPMEAN_HIGH:{
							if(iTmpHighNIBPMean<NIBPMEAN_LIMIT_MAX){
								iTmpHighNIBPMean += LIMIT_INTER;
								SetLimitText(hWnd, iTmpHighNIBPMean);
							}
						}break;
						case DID_COMMONALM_RESP_HIGH:{
							if(iTmpHighResp<RESP_LIMIT_MAX){
								iTmpHighResp += LIMIT_INTER;
								SetLimitText(hWnd, iTmpHighResp);
							}
						}break;
						case DID_COMMONALM_T1_HIGH:{
							if(iTmpHighT1<T1_LIMIT_MAX){
								iTmpHighT1 += LIMIT_INTER;
								SetLimitText(hWnd, iTmpHighT1);
							}
						}break;
						case DID_COMMONALM_T2_HIGH:{
							if(iTmpHighT2<T2_LIMIT_MAX){
								iTmpHighT2 += LIMIT_INTER;
								SetLimitText(hWnd, iTmpHighT2);
							}
						}break;
						case DID_COMMONALM_HR_LOW:{
							if(iTmpLowHR<HR_LIMIT_MAX&&iTmpLowHR<iTmpHighHR){
								iTmpLowHR += LIMIT_INTER;
								SetLimitText(hWnd, iTmpLowHR);
							}
						}break;
						case DID_COMMONALM_SPO2_LOW:{
							if(iTmpLowSpo2<SPO2_LIMIT_MAX&&iTmpLowSpo2<iTmpHighSpo2){
								iTmpLowSpo2 += LIMIT_INTER;
								SetLimitText(hWnd, iTmpLowSpo2);
							}
						}break;
						case DID_COMMONALM_NIBPSYS_LOW:{
							if(iTmpLowNIBPSys<NIBPSYS_LIMIT_MAX&&iTmpLowNIBPSys<iTmpHighNIBPSys){
								iTmpLowNIBPSys += LIMIT_INTER;
								SetLimitText(hWnd, iTmpLowNIBPSys);
							}
						}break;
						case DID_COMMONALM_NIBPDIA_LOW:{
							if(iTmpLowNIBPDia<NIBPDIA_LIMIT_MAX&&iTmpLowNIBPDia<iTmpHighNIBPDia){
								iTmpLowNIBPDia += LIMIT_INTER;
								SetLimitText(hWnd, iTmpLowNIBPDia);
							}
						}break;
						case DID_COMMONALM_NIBPMEAN_LOW:{
							if(iTmpLowNIBPMean<NIBPMEAN_LIMIT_MAX&&iTmpLowNIBPMean<iTmpHighNIBPMean){
								iTmpLowNIBPMean += LIMIT_INTER;
								SetLimitText(hWnd, iTmpLowNIBPMean);
							}
						}break;
						case DID_COMMONALM_RESP_LOW:{
							if(iTmpLowResp<RESP_LIMIT_MAX&&iTmpLowResp<iTmpHighResp){
								iTmpLowResp += LIMIT_INTER;
								SetLimitText(hWnd, iTmpLowResp);
							}
						}break;
						case DID_COMMONALM_T1_LOW:{
							if(iTmpLowT1<T1_LIMIT_MAX&&iTmpLowT1<iTmpHighT1){
								iTmpLowT1 += LIMIT_INTER;
								SetLimitText(hWnd, iTmpLowT1);
							}
						}break;
						case DID_COMMONALM_T2_LOW:{
							if(iTmpLowT2<T2_LIMIT_MAX&&iTmpLowT2<iTmpHighT2){
								iTmpLowT2 += LIMIT_INTER;
								SetLimitText(hWnd, iTmpLowT2);
							}
						}break;
					}
				}break;
				case SCANCODE_CURSORBLOCKUP:{
					switch(id){
					case DID_COMMONALM_HR_HIGH:{
							if(iTmpHighHR>HR_LIMIT_MIN&&iTmpHighHR>iTmpLowHR){
								iTmpHighHR -= LIMIT_INTER;
								SetLimitText(hWnd, iTmpHighHR);
							}
						}break;
						case DID_COMMONALM_SPO2_HIGH:{
							if(iTmpHighSpo2>SPO2_LIMIT_MIN&&iTmpHighSpo2>iTmpLowSpo2){
								iTmpHighSpo2 -= LIMIT_INTER;
								SetLimitText(hWnd, iTmpHighSpo2);
							}
						}break;
						case DID_COMMONALM_NIBPSYS_HIGH:{
							if(iTmpHighNIBPSys>NIBPSYS_LIMIT_MIN&&iTmpHighNIBPSys>iTmpLowNIBPSys){
								iTmpHighNIBPSys -= LIMIT_INTER;
								SetLimitText(hWnd, iTmpHighNIBPSys);
							}
						}break;
						case DID_COMMONALM_NIBPDIA_HIGH:{
							if(iTmpHighNIBPDia>NIBPDIA_LIMIT_MIN&&iTmpHighNIBPDia>iTmpLowNIBPDia){
								iTmpHighNIBPDia -= LIMIT_INTER;
								SetLimitText(hWnd, iTmpHighNIBPDia);
							}
						}break;
						case DID_COMMONALM_NIBPMEAN_HIGH:{
							if(iTmpHighNIBPMean>NIBPMEAN_LIMIT_MIN&&iTmpHighNIBPMean>iTmpLowNIBPMean){
								iTmpHighNIBPMean -= LIMIT_INTER;
								SetLimitText(hWnd, iTmpHighNIBPMean);
							}
						}break;
						case DID_COMMONALM_RESP_HIGH:{
							if(iTmpHighResp>RESP_LIMIT_MIN&&iTmpHighResp>iTmpLowResp){
								iTmpHighResp -= LIMIT_INTER;
								SetLimitText(hWnd, iTmpHighResp);
							}
						}break;
						case DID_COMMONALM_T1_HIGH:{
							if(iTmpHighT1>T1_LIMIT_MIN&&iTmpHighT1>iTmpLowT1){
								iTmpHighT1 -= LIMIT_INTER;
								SetLimitText(hWnd, iTmpHighT1);
							}
						}break;
						case DID_COMMONALM_T2_HIGH:{
							if(iTmpHighT2>T2_LIMIT_MIN&&iTmpHighT2>iTmpLowT2){
								iTmpHighT2 -= LIMIT_INTER;
								SetLimitText(hWnd, iTmpHighT2);
							}
						}break;
							case DID_COMMONALM_HR_LOW:{
							if(iTmpLowHR>HR_LIMIT_MIN){
								iTmpLowHR -= LIMIT_INTER;
								SetLimitText(hWnd, iTmpLowHR);
							}
						}break;
						case DID_COMMONALM_SPO2_LOW:{
							if(iTmpLowSpo2>SPO2_LIMIT_MIN){
								iTmpLowSpo2 -= LIMIT_INTER;
								SetLimitText(hWnd, iTmpLowSpo2);
							}
						}break;
						case DID_COMMONALM_NIBPSYS_LOW:{
							if(iTmpLowNIBPSys>NIBPSYS_LIMIT_MIN){
								iTmpLowNIBPSys -= LIMIT_INTER;
								SetLimitText(hWnd, iTmpLowNIBPSys);
							}
						}break;
						case DID_COMMONALM_NIBPDIA_LOW:{
							if(iTmpLowNIBPDia>NIBPDIA_LIMIT_MIN){
								iTmpLowNIBPDia -= LIMIT_INTER;
								SetLimitText(hWnd, iTmpLowNIBPDia);
							}
						}break;
						case DID_COMMONALM_NIBPMEAN_LOW:{
							if(iTmpLowNIBPMean>NIBPMEAN_LIMIT_MIN){
								iTmpLowNIBPMean -= LIMIT_INTER;
								SetLimitText(hWnd, iTmpLowNIBPMean);
							}
						}break;
						case DID_COMMONALM_RESP_LOW:{
							if(iTmpLowResp>RESP_LIMIT_MIN){
								iTmpLowResp -= LIMIT_INTER;
								SetLimitText(hWnd, iTmpLowResp);
							}
						}break;
						case DID_COMMONALM_T1_LOW:{
							if(iTmpLowT1>T1_LIMIT_MIN){
								iTmpLowT1 -= LIMIT_INTER;
								SetLimitText(hWnd, iTmpLowT1);
							}
						}break;
						case DID_COMMONALM_T2_LOW:{
							if(iTmpLowT2>T2_LIMIT_MIN){
								iTmpLowT2 -= LIMIT_INTER;
								SetLimitText(hWnd, iTmpLowT2);
							}
						}break;
						}
				}break;
					
				case SCANCODE_ENTER:{
					switch(id){
						case DID_COMMONALM_HR_HIGH:{
							gCfgEcg.wHigh_HR= iTmpHighHR;
							SetIntValueToResFile(gFileSetup, "ECGSetup", "hr_high",  gCfgEcg.wHigh_HR, 3);
							SetFocus(lowHR);
						}break;
						case DID_COMMONALM_SPO2_HIGH:{
							gCfgSpO2.bHigh_SpO2= iTmpHighSpo2;
							SetIntValueToResFile(gFileSetup, "SPO2Setup", "spo2_high",  gCfgSpO2.bHigh_SpO2, 3);
							SetFocus(lowSpo2);
						}break;
						case DID_COMMONALM_NIBPSYS_HIGH:{
							gCfgNibp.wHigh_Sys= iTmpHighNIBPSys;
							SetIntValueToResFile(gFileSetup, "NIBPSetup", "sys_high",  gCfgNibp.wHigh_Sys, 3);
							SetFocus(lowNIBPSys);
						}break;
						case DID_COMMONALM_NIBPDIA_HIGH:{
							gCfgNibp.wHigh_Dia= iTmpHighNIBPDia;
							SetIntValueToResFile(gFileSetup, "NIBPSetup", "dia_high",  gCfgNibp.wHigh_Dia, 3);
							SetFocus(lowNIBPDia);
						}break;
						case DID_COMMONALM_NIBPMEAN_HIGH:{
							gCfgNibp.wHigh_Mean= iTmpHighNIBPMean;
							SetIntValueToResFile(gFileSetup,  "NIBPSetup", "mean_high",  gCfgNibp.wHigh_Mean, 3);
							SetFocus(lowNIBPMean);
						}break;
						case DID_COMMONALM_RESP_HIGH:{
							gCfgResp.wHigh_RR= iTmpHighResp;
							SetIntValueToResFile(gFileSetup, "RESPSetup", "rr_high",  gCfgResp.wHigh_RR, 3);
							SetFocus(lowResp);
						}break;
						case DID_COMMONALM_T1_HIGH:{
							gCfgTemp.wHigh_T1= iTmpHighT1;
							SetIntValueToResFile(gFileSetup, "TEMPSetup", "t1_high",  gCfgTemp.wHigh_T1, 3);
							SetFocus(lowT1);
						}break;
						case DID_COMMONALM_T2_HIGH:{
							gCfgTemp.wHigh_T2= iTmpHighT2;
							SetIntValueToResFile(gFileSetup, "TEMPSetup", "t2_high",  gCfgTemp.wHigh_T2, 3);
							SetFocus(lowT2);
						}break;
				
						case DID_COMMONALM_HR_LOW:{
							gCfgEcg.wLow_HR= iTmpLowHR;
							SetIntValueToResFile(gFileSetup, "ECGSetup", "hr_low",  gCfgEcg.wLow_HR, 3);
							gbKeyType = KEY_TAB;
							SetFocus(btnHR);
						}break;
						case DID_COMMONALM_SPO2_LOW:{
							gCfgSpO2.bLow_SpO2= iTmpLowSpo2;
							SetIntValueToResFile(gFileSetup, "SPO2Setup", "spo2_low",  gCfgSpO2.bLow_SpO2, 3);
							gbKeyType = KEY_TAB;
							SetFocus(btnSpo2);
						}break;
						case DID_COMMONALM_NIBPSYS_LOW:{
							gCfgNibp.wLow_Sys= iTmpLowNIBPSys;
							SetIntValueToResFile(gFileSetup, "NIBPSetup", "sys_low",  gCfgNibp.wLow_Sys, 3);
							gbKeyType = KEY_TAB;
							SetFocus(btnNIBPSys);
						}break;
						case DID_COMMONALM_NIBPDIA_LOW:{
							gCfgNibp.wLow_Dia= iTmpLowNIBPDia;
							SetIntValueToResFile(gFileSetup, "NIBPSetup", "dia_low",  gCfgNibp.wLow_Dia, 3);
							gbKeyType = KEY_TAB;
							SetFocus(btnNIBPDia);
						}break;
						case DID_COMMONALM_NIBPMEAN_LOW:{
							gCfgNibp.wLow_Mean= iTmpLowNIBPMean;
							SetIntValueToResFile(gFileSetup,  "NIBPSetup", "mean_low",  gCfgNibp.wLow_Mean, 3);
							gbKeyType = KEY_TAB;
							SetFocus(btnNIBPMean);
						}break;
						case DID_COMMONALM_RESP_LOW:{
							gCfgResp.wLow_RR= iTmpLowResp;
							SetIntValueToResFile(gFileSetup, "RESPSetup", "rr_low",  gCfgResp.wLow_RR, 3);
							gbKeyType = KEY_TAB;
							SetFocus(btnResp);
						}break;
						case DID_COMMONALM_T1_LOW:{
							gCfgTemp.wLow_T1= iTmpLowT1;
							SetIntValueToResFile(gFileSetup, "TEMPSetup", "t1_low",  gCfgTemp.wLow_T1, 3);
							gbKeyType = KEY_TAB;
							SetFocus(btnT1);
						}break;
						case DID_COMMONALM_T2_LOW:{
							gCfgTemp.wLow_T2= iTmpLowT2;
							SetIntValueToResFile(gFileSetup, "TEMPSetup", "t2_low",  gCfgTemp.wLow_T2, 3);
							gbKeyType = KEY_TAB;
							SetFocus(btnT2);
						}break;
					}	
				}break;
			}
			return 0;
		}break;	
	}
		
	temp = (*OldEditProc)(hWnd, message, wParam, lParam);
	return(temp);
}
static int SetAlarmHR(int index)
{
	if(flag==0){
		flag++;
		gCfgEcg.bAlmControl = index;
		SetIntValueToResFile(gFileSetup, "ECGSetup", "alarm_control",  gCfgEcg.bAlmControl, 1);
		IsAlm_HR(&gValueEcg, &gCfgEcg, TRUE);
		IsAlm_ST1(&gValueEcg, &gCfgEcg, TRUE);
		IsAlm_ST2(&gValueEcg, &gCfgEcg, TRUE);
		IsAlm_Arr(&gValueEcg, &gCfgEcg, TRUE);
		
		return 0;
	}
	else
		flag=0;
	return 0;
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
						case DID_COMMONALM_HR_COB:{
							gCfgEcg.bAlmControl = index;
							SetIntValueToResFile(gFileSetup, "ECGSetup", "alarm_control",  gCfgEcg.bAlmControl, 1);
							IsAlm_HR(&gValueEcg, &gCfgEcg, TRUE);
							IsAlm_ST1(&gValueEcg, &gCfgEcg, TRUE);
							IsAlm_ST2(&gValueEcg, &gCfgEcg, TRUE);
							IsAlm_Arr(&gValueEcg, &gCfgEcg, TRUE);
							
							SetFocus(highHR);
								
						}break;
						case DID_COMMONALM_SPO2_COB:{
							gCfgSpO2.bAlmControl = index;
							SetIntValueToResFile(gFileSetup, "SPO2Setup", "alarm_control",  gCfgSpO2.bAlmControl, 1);
							IsAlm_SpO2(&gValueSpO2, &gCfgSpO2, TRUE);
							gbKeyType = KEY_UD;
							SetFocus(highSpo2);
						}break;
						case DID_COMMONALM_NIBPSYS_COB:{
							gCfgNibp.bAlmControl = index;
							SetIntValueToResFile(gFileSetup, "NIBPSetup", "alarm_control",  gCfgNibp.bAlmControl, 1);
							//改变报警级别
							IsAlm_NIBP(TRUE, TRUE);
							gbKeyType = KEY_UD;
							SetFocus(highNIBPSys);
						}break;
						case DID_COMMONALM_NIBPDIA_COB:{
							gCfgNibp.bAlmControl = index;
							SetIntValueToResFile(gFileSetup, "NIBPSetup", "alarm_control",  gCfgNibp.bAlmControl, 1);
							//改变报警级别
							IsAlm_NIBP(TRUE, TRUE);
							gbKeyType = KEY_UD;
							SetFocus(highNIBPDia);
							
						}break;
						case DID_COMMONALM_NIBPMEAN_COB:{
							gCfgNibp.bAlmControl = index;
							SetIntValueToResFile(gFileSetup, "NIBPSetup", "alarm_control",  gCfgNibp.bAlmControl, 1);
							//改变报警级别
							IsAlm_NIBP(TRUE, TRUE);
							gbKeyType = KEY_UD;
							SetFocus(highNIBPMean);
						}break;
						case DID_COMMONALM_RESP_COB:{
							gCfgResp.bAlmControl = index;
							SetIntValueToResFile(gFileSetup, "RESPSetup", "alarm_control",  gCfgResp.bAlmControl, 1);
							IsAlm_RR(&gValueResp, &gCfgResp, TRUE);
							gbKeyType = KEY_UD;
							SetFocus(highResp);
						}break;
						case DID_COMMONALM_T1_COB:{
							gCfgTemp.bAlmControl = index;
							SetIntValueToResFile(gFileSetup, "TEMPSetup", "alarm_control",  gCfgTemp.bAlmControl, 1);
							//改变参数的报警级别
							IsAlm_T1(&gValueTemp, &gCfgTemp, TRUE);
							//IsAlm_T2(&gValueTemp, &gCfgTemp, TRUE);
						 	IsAlm_TD(&gValueTemp, &gCfgTemp, TRUE);
							gbKeyType = KEY_UD;
							SetFocus(highT1);
						}break;
						case DID_COMMONALM_T2_COB:{
							gCfgTemp.bAlmControl = index;
							SetIntValueToResFile(gFileSetup, "TEMPSetup", "alarm_control",  gCfgTemp.bAlmControl, 1);
							//改变参数的报警级别
							//IsAlm_T1(&gValueTemp, &gCfgTemp, TRUE);
							IsAlm_T2(&gValueTemp, &gCfgTemp, TRUE);
						 	IsAlm_TD(&gValueTemp, &gCfgTemp, TRUE);
							gbKeyType = KEY_UD;
							SetFocus(highT2);
						}break;
						
					}	
				//	gbKeyType = KEY_TAB;
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
					     DID_COMMONALM_CAPTION, 0, 5, 250, 25, hDlg, 0);
	editHelp = GetDlgItem(hDlg, DID_COMMONALM_HELP);

	btnHR = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_COMMONALM_HR, 5, 30+29*0, 110, 26, hDlg, 0);
	btnSpo2 = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_COMMONALM_SPO2, 5, 30+29*1, 110, 26, hDlg, 0);
	btnNIBPSys = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_COMMONALM_NIBPSYS, 5, 30+29*2, 110, 26, hDlg, 0);
	btnNIBPDia = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_COMMONALM_NIBPDIA, 5, 30+29*3, 110, 26, hDlg, 0);
	btnNIBPMean = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_COMMONALM_NIBPMEAN, 5, 30+29*4, 110, 26, hDlg, 0);
	btnResp = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_COMMONALM_RESP, 5, 30+29*5, 110, 26, hDlg, 0);
	btnT1 = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_COMMONALM_T1, 5, 30+29*6, 110, 26, hDlg, 0);
	btnT2 = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_COMMONALM_T2, 5, 30+29*7, 110, 26, hDlg, 0);
	btnDefault= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_COMMONALM_DEFAULT, 5, 30+29*8, 110, 26, hDlg, 0);
	btnOk = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_COMMONALM_OK, 5, 30+29*9, 110, 26, hDlg, 0);

/*
	cobHR= CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST,  
					DID_COMMONALM_HR_COB, 105, 40, 60, 26, hDlg, 0);
	cobSpo2= CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST,  
					DID_COMMONALM_HR_COB, 105, 70, 60, 26, hDlg, 0);
	cobNIBPSys= CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST,  
					DID_COMMONALM_HR_COB, 105, 100, 60, 26, hDlg, 0);
	cobNIBPDia= CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST,  
					DID_COMMONALM_HR_COB, 105, 130, 60, 26, hDlg, 0);
	cobNIBPMean= CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST,  
					DID_COMMONALM_HR_COB, 105, 160, 60, 26, hDlg, 0);
	cobResp= CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST,  
					DID_COMMONALM_HR_COB, 105, 190, 60, 26, hDlg, 0);
	cobT1= CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST,  
					DID_COMMONALM_HR_COB, 105, 220, 60, 26, hDlg, 0);
	cobT2= CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST,  
					DID_COMMONALM_HR_COB, 105, 250, 60, 26, hDlg, 0);
	*/
	
	highHR = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER |ES_CENTER,
								   DID_COMMONALM_HR_HIGH, 120, 30+29*0, 60, 23, hDlg, 0);
	highSpo2 = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER |ES_CENTER,
								   DID_COMMONALM_SPO2_HIGH, 120, 30+29*1, 60, 23, hDlg, 0);
	highNIBPSys = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER |ES_CENTER,
								   DID_COMMONALM_NIBPSYS_HIGH, 120, 30+29*2, 60, 23, hDlg, 0);
	highNIBPDia = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER |ES_CENTER,
								   DID_COMMONALM_NIBPDIA_HIGH, 120, 30+29*3, 60, 23, hDlg, 0);
	highNIBPMean = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER |ES_CENTER,
								   DID_COMMONALM_NIBPMEAN_HIGH, 120, 30+29*4, 60, 23, hDlg, 0);
	highResp = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER |ES_CENTER,
								   DID_COMMONALM_RESP_HIGH, 120, 30+29*5, 60, 23, hDlg, 0);
	highT1 = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER |ES_CENTER,
								   DID_COMMONALM_T1_HIGH, 120, 30+29*6, 60, 23, hDlg, 0);
	highT2 = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER |ES_CENTER,
								   DID_COMMONALM_T2_HIGH, 120, 30+29*7, 60, 23, hDlg, 0);
	
	lowHR = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER |ES_CENTER,
								   DID_COMMONALM_HR_LOW, 182, 30+29*0, 60, 23, hDlg, 0);
	lowSpo2 = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER |ES_CENTER,
								   DID_COMMONALM_SPO2_LOW, 182, 30+29*1, 60, 23, hDlg, 0);
	lowNIBPSys = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER |ES_CENTER,
								   DID_COMMONALM_NIBPSYS_LOW, 182, 30+29*2, 60, 23, hDlg, 0);
	lowNIBPDia = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER |ES_CENTER,
								   DID_COMMONALM_NIBPDIA_LOW, 182, 30+29*3, 60, 23, hDlg, 0);
	lowNIBPMean = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER |ES_CENTER,
								   DID_COMMONALM_NIBPMEAN_LOW, 182, 30+29*4, 60, 23, hDlg, 0);
	lowResp = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER |ES_CENTER,
								   DID_COMMONALM_RESP_LOW, 182, 30+29*5, 60, 23, hDlg, 0);
	lowT1 = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER |ES_CENTER,
								   DID_COMMONALM_T1_LOW, 182, 30+29*6, 60, 23, hDlg, 0);
	lowT2 = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER |ES_CENTER,
								   DID_COMMONALM_T2_LOW, 182, 30+29*7, 60, 23, hDlg, 0);
	
	return 0;
}
static int InitCtlName(HWND hDlg)
{
	char strMenu[100];

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSETUP, "caption", strMenu, sizeof strMenu,"Common Alarm");
	SetWindowText(editCaption, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "previous", strMenu, sizeof strMenu,"Previous");	
	SetWindowText(btnOk, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSETUP, "hr", strMenu, sizeof strMenu,"HR");	
	SetWindowText(btnHR, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSETUP, "spo2", strMenu, sizeof strMenu,"SPO2");	
	SetWindowText(btnSpo2, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSETUP, "nibp_sys", strMenu, sizeof strMenu,"NIBP SYS");	
	SetWindowText(btnNIBPSys, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSETUP, "nibp_dia", strMenu, sizeof strMenu,"NIBP DIA");	
	SetWindowText(btnNIBPDia, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSETUP, "nibp_mean", strMenu, sizeof strMenu,"NIBP MEAN");	
	SetWindowText(btnNIBPMean,strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSETUP, "resp", strMenu, sizeof strMenu,"RESP");	
	SetWindowText(btnResp, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSETUP, "t1", strMenu, sizeof strMenu,"T1");	
	SetWindowText(btnT1, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSETUP, "t2", strMenu, sizeof strMenu,"T2");	
	SetWindowText(btnT2, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSETUP, "default", strMenu, sizeof strMenu,"Default");	
	SetWindowText(btnDefault, strMenu);

	
	memset(strOff, 0, sizeof strOff);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "off", strOff, sizeof strOff,"Off");
	memset(strLow, 0, sizeof strLow);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "low", strLow, sizeof strLow,"Low");
	memset(strMid, 0, sizeof strMid);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "mid", strMid, sizeof strMid,"Mid");
	memset(strHigh, 0, sizeof strHigh);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "High", strHigh, sizeof strHigh,"High");

	
	return 0;
}
static int SetCobList(HWND hwnd)
{
	SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)strOff);
	SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)strLow);
	SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)strMid);
	SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM)strHigh);

	return 0;
}
static int InitCobList(HWND hDlg)
{

	SetCobList(cobHR);
	SendMessage(cobHR, CB_SETCURSEL,gCfgEcg.bAlmControl, 0);
	SetCobList(cobSpo2);
	SendMessage(cobSpo2, CB_SETCURSEL,gCfgSpO2.bAlmControl, 0);
	SetCobList(cobNIBPSys);
	SendMessage(cobNIBPSys, CB_SETCURSEL,gCfgNibp.bAlmControl, 0);
	SetCobList(cobNIBPDia);
	SendMessage(cobNIBPDia, CB_SETCURSEL,gCfgNibp.bAlmControl, 0);
	SetCobList(cobNIBPMean);
	SendMessage(cobNIBPMean, CB_SETCURSEL,gCfgNibp.bAlmControl, 0);
	SetCobList(cobResp);
	SendMessage(cobResp, CB_SETCURSEL,gCfgResp.bAlmControl, 0);
	SetCobList(cobT1);
	SendMessage(cobT1, CB_SETCURSEL,gCfgTemp.bAlmControl, 0);
	SetCobList(cobT2);
	SendMessage(cobT2, CB_SETCURSEL,gCfgTemp.bAlmControl, 0);
	
	
	return 0;
}
static int InitCtlCallBackProc(HWND hDlg)
{

	OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnHR, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSpo2, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnNIBPSys, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnNIBPDia, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnNIBPMean, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnResp, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnT1, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnT2, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnDefault, BtnProc);
	
	OldCobProc = SetWindowCallbackProc(cobHR, CobProc);
	OldCobProc = SetWindowCallbackProc(cobSpo2, CobProc);
	OldCobProc = SetWindowCallbackProc(cobNIBPSys, CobProc);
	OldCobProc = SetWindowCallbackProc(cobNIBPDia, CobProc);
	OldCobProc = SetWindowCallbackProc(cobNIBPMean, CobProc);
	OldCobProc = SetWindowCallbackProc(cobResp, CobProc);
	OldCobProc = SetWindowCallbackProc(cobT1, CobProc);
	OldCobProc = SetWindowCallbackProc(cobT2, CobProc);
	
	OldEditProc = SetWindowCallbackProc(highHR, EditProc);
	OldEditProc = SetWindowCallbackProc(highSpo2, EditProc);
	OldEditProc = SetWindowCallbackProc(highNIBPSys, EditProc);
	OldEditProc = SetWindowCallbackProc(highNIBPDia, EditProc);
	OldEditProc = SetWindowCallbackProc(highNIBPMean, EditProc);
	OldEditProc = SetWindowCallbackProc(highResp, EditProc);
	OldEditProc = SetWindowCallbackProc(highT1, EditProc);
	OldEditProc = SetWindowCallbackProc(highT2, EditProc);

	OldEditProc = SetWindowCallbackProc(lowHR, EditProc);
	OldEditProc = SetWindowCallbackProc(lowSpo2, EditProc);
	OldEditProc = SetWindowCallbackProc(lowNIBPSys, EditProc);
	OldEditProc = SetWindowCallbackProc(lowNIBPDia, EditProc);
	OldEditProc = SetWindowCallbackProc(lowNIBPMean, EditProc);
	OldEditProc = SetWindowCallbackProc(lowResp, EditProc);
	OldEditProc = SetWindowCallbackProc(lowT1, EditProc);
	OldEditProc = SetWindowCallbackProc(lowT2, EditProc);
	
	OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp);
	
	return 0;
}
static int InitEditCtl()
{	

	//获取各模块报警上下限
	 iTmpHighHR =gCfgEcg.wHigh_HR, iTmpLowHR =gCfgEcg.wLow_HR;
	 iTmpHighSpo2 =gCfgSpO2.bHigh_SpO2, iTmpLowSpo2 = gCfgSpO2.bLow_SpO2;
	 iTmpHighNIBPSys =gCfgNibp.wHigh_Sys, iTmpLowNIBPSys =gCfgNibp.wLow_Sys;
	 iTmpHighNIBPDia =gCfgNibp.wHigh_Dia, iTmpLowNIBPDia = gCfgNibp.wLow_Dia;
	 iTmpHighNIBPMean =gCfgNibp.wHigh_Mean, iTmpLowNIBPMean = gCfgNibp.wLow_Mean;
	 iTmpHighResp =gCfgResp.wHigh_RR, iTmpLowResp =gCfgResp.wLow_RR;
	 iTmpHighT1=gCfgTemp.wHigh_T1, iTmpLowT1=gCfgTemp.wLow_T1;
	 iTmpHighT2=gCfgTemp.wHigh_T2, iTmpLowT2 = gCfgTemp.wLow_T2;
	 
	//初始化
	SetLimitText(highHR,iTmpHighHR);
	SetLimitText(highSpo2,iTmpHighSpo2);
	SetLimitText(highNIBPSys,iTmpHighNIBPSys);
	SetLimitText(highNIBPDia,iTmpHighNIBPDia);
	SetLimitText(highNIBPMean,iTmpHighNIBPMean);
	SetLimitText(highResp,iTmpHighResp);
	SetLimitText(highT1,iTmpHighT1);
	SetLimitText(highT2,iTmpHighT2);

	SetLimitText(lowHR,iTmpLowHR);
	SetLimitText(lowSpo2,iTmpLowSpo2);
	SetLimitText(lowNIBPSys,iTmpLowNIBPSys);
	SetLimitText(lowNIBPDia,iTmpLowNIBPDia);
	SetLimitText(lowNIBPMean,iTmpLowNIBPMean);
	SetLimitText(lowResp,iTmpLowResp);
	SetLimitText(lowT1,iTmpLowT1);
	SetLimitText(lowT2,iTmpLowT2);
	return 0;
}
static int InitCtlStatus(HWND hDlg)
{
	
	
	return 0;
}
static int ResetAlmLimit()
{

	gCfgEcg.wHigh_HR =120, gCfgEcg.wLow_HR = 50;
	gCfgSpO2.bHigh_SpO2 =100, gCfgSpO2.bLow_SpO2 = 90;
	gCfgNibp.wHigh_Sys =160, gCfgNibp.wLow_Sys = 90;
	gCfgNibp.wHigh_Dia =90,  gCfgNibp.wLow_Dia = 50;
	gCfgNibp.wHigh_Mean =110, gCfgNibp.wLow_Mean =60;
	gCfgResp.wHigh_RR=30, gCfgResp.wLow_RR = 8;
	gCfgTemp.wHigh_T1=390, gCfgTemp.wLow_T1= 360;
	gCfgTemp.wHigh_T2=390, gCfgTemp.wLow_T2 = 360;

	InitEditCtl();
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
			InitEditCtl();
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
				case DID_COMMONALM_HR:{
					gbKeyType = KEY_UD;
					SetFocus(highHR);
					}break;
				case DID_COMMONALM_SPO2:{
					gbKeyType = KEY_UD;
					SetFocus(highSpo2);
					}break;
				case DID_COMMONALM_NIBPSYS:{
					gbKeyType = KEY_UD;
					SetFocus(highNIBPSys);
					}break;
				case DID_COMMONALM_NIBPDIA:{
					gbKeyType = KEY_UD;
					SetFocus(highNIBPDia);
					}break;
				case DID_COMMONALM_NIBPMEAN:{
					gbKeyType = KEY_UD;
					SetFocus(highNIBPMean);
					}break;
				case DID_COMMONALM_RESP:{
					gbKeyType = KEY_UD;
					SetFocus(highResp);
					}break;
				case DID_COMMONALM_T1:{
					gbKeyType = KEY_UD;
					SetFocus(highT1);
					}break;
				case DID_COMMONALM_T2:{
					gbKeyType = KEY_UD;
					SetFocus(highT2);
					}break;
				case DID_COMMONALM_DEFAULT:{
					BOOL res = FALSE;
					char strACKInfo[200];
					memset(strACKInfo, 0, sizeof strACKInfo);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_ALMSETUP, "ackinfo_update", strACKInfo, sizeof strACKInfo,"Default alarm limit?");	
					res = ACKDialog(hDlg, strACKInfo, ACK_ATTENTION);
					if(res){
						ResetAlmLimit();
					}
					}break;
				case DID_COMMONALM_OK:{
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
void CreateCommonAlarm(HWND hWnd)
{
 	DlgSet.controls = CtrlSet;
 	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}


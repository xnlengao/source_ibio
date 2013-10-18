#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"



#define DID_IBP1_ZERO						8100
#define DID_IBP1_ZERO_CAPTION				DID_IBP1_ZERO
#define DID_IBP1_ZERO_ZERO					DID_IBP1_ZERO+1
#define DID_IBP1_ZERO_ADJUSTVALUE			DID_IBP1_ZERO+2
#define DID_IBP1_ZERO_ADJUSTBTN			DID_IBP1_ZERO+3
#define DID_IBP1_ZERO_OK					DID_IBP1_ZERO+4
#define DID_IBP1_ZERO_EDITVIEW			DID_IBP1_ZERO+5
#define DID_IBP1_ZERO_HELP					DID_IBP1_ZERO+6
#define DID_IBP1_ZERO_EDITKP				DID_IBP1_ZERO+7
#define DID_IBP1_ZERO_TIME1				DID_IBP1_ZERO+8
#define DID_IBP1_ZERO_TIME2				DID_IBP1_ZERO+9
#define DID_IBP1_ZERO_INFO1				DID_IBP1_ZERO+10
#define DID_IBP1_ZERO_INFO2				DID_IBP1_ZERO+11
#define DID_IBP1_ZERO_INFO 				DID_IBP1_ZERO+11               

//帮助对话框
static HWND editHelp;
static WNDPROC OldEditProcHelp;

//其他控件
static HWND btnOk;
static HWND btnadjustzero;
static HWND btnadjustKp;
static HWND editCaption;
//static HWND editzeroview;
static HWND editsetkp;
static HWND btnadjust;

static HWND edittime;
static HWND editzeroview;
//static HWND editinfo1;
//static HWND editinfo2;
//static HWND info;
//控件回调函数
static WNDPROC OldBtnProc;	
static WNDPROC OldEditProc;
static HWND ibp1whnd;
static S_TIME itime;

static int iTmpTop,iTmpBottom,initvalue,flag_cal=0,flag_zero=0,flagtime=0;
//对话框属性
static DLGTEMPLATE DlgSet= {
	WS_VISIBLE || WS_BORDER,
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
		DID_IBP1_ZERO_HELP,
		"",
		0
	}
};

int IBP1time()
{

	if(flagtime==1){
		
	HDC hdc;
	char strMenu[100];
	
	if(ibp1whnd==(HWND)NULL) return -1;
		//获得绘图DC
 	hdc = GetClientDC(ibp1whnd);
 	if(hdc == (HDC)NULL) return -1;

	memset(strMenu, 0, sizeof strMenu);
	snprintf(strMenu, sizeof strMenu, "%d %s %d %s %d",  gValueIbp1.sCalTime.wYear,"-"
			,gValueIbp1.sCalTime.bMonth,"-",gValueIbp1.sCalTime.bDay);
	SetWindowText(edittime,strMenu);	
	
	ReleaseDC(hdc);
	flagtime=0;
	
	}
	return 0;
}
int IBP1Calinfo()
{
	if(flag_cal==1){
	HDC hdc;
	char strMenu[100];
	if(ibp1whnd==(HWND)NULL) return -1;
		//获得绘图DC
 	hdc = GetClientDC(ibp1whnd);
 	if(hdc == (HDC)NULL) return -1;
	
	printf("~~~~~~gValueIbp1.bCalResult=%d\n",gValueIbp1.bCalResult);
		if(gValueIbp1.bCalResult==0){
		memset(strMenu, 0, sizeof strMenu);
		GetStringFromResFiles(gsLanguageRes, "IBP", "cal_info_0", strMenu, sizeof strMenu,"Cal success.");	
		SetWindowText(editzeroview,strMenu);
		}
		if(gValueIbp1.bCalResult==1){
		memset(strMenu, 0, sizeof strMenu);
		GetStringFromResFiles(gsLanguageRes, "IBP", "cal_info_1", strMenu, sizeof strMenu,"Cal fault(1):pulsating.");	
		SetWindowText(editzeroview,strMenu);
		}
		if(gValueIbp1.bCalResult==2){
		memset(strMenu, 0, sizeof strMenu);
		GetStringFromResFiles(gsLanguageRes, "IBP", "cal_info_2", strMenu, sizeof strMenu,"Cal fault(2):overrange.");	
		SetWindowText(editzeroview,strMenu);
		}
		if(gValueIbp1.bCalResult==3){
		memset(strMenu, 0, sizeof strMenu);
		GetStringFromResFiles(gsLanguageRes, "IBP", "cal_info_3", strMenu, sizeof strMenu,"Cal fault(3):not zero.");	
		SetWindowText(editzeroview,strMenu);
		}
		if(gValueIbp1.bCalResult==4){
		memset(strMenu, 0, sizeof strMenu);
		GetStringFromResFiles(gsLanguageRes, "IBP", "cal_info_4", strMenu, sizeof strMenu,"Cal fault(4):lead off.");	
		SetWindowText(editzeroview,strMenu);
		}
		if(gValueIbp1.bCalResult==10){
		memset(strMenu, 0, sizeof strMenu);
		GetStringFromResFiles(gsLanguageRes, "IBP", "no_set_cal", strMenu, sizeof strMenu,"Didn't Cal.");	
		SetWindowText(editzeroview,strMenu);
		}


			
		flag_cal=0;

		ReleaseDC(hdc);
			}
		return 0;

}



int IBP1Zeroinfo()
{
	if(flag_zero==1){
		HDC hdc;
		char strMenu[100];
		if(ibp1whnd==(HWND)NULL) return -1;
			//获得绘图DC
	 	hdc = GetClientDC(ibp1whnd);
	 	if(hdc == (HDC)NULL) return -1;
		printf("ibp1 zeor info %d\n",gValueIbp1.bZeroResult);
		if(gValueIbp1.bZeroResult==0){
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "IBP", "zero_info_0", strMenu, sizeof strMenu,"Zero success.");	
			SetWindowText(editzeroview,strMenu);

			 EnableWindow(btnadjust,TRUE);
		}
		if(gValueIbp1.bZeroResult==1){
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "IBP", "zero_info_1", strMenu, sizeof strMenu,"Zero fault(1):pulsating.");	
			SetWindowText(editzeroview,strMenu);
		}
		if(gValueIbp1.bZeroResult==2){
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "IBP", "zero_info_2", strMenu, sizeof strMenu,"Zero fault(2):overrange.");	
			SetWindowText(editzeroview,strMenu);
		}
		if(gValueIbp1.bZeroResult==3){
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "IBP", "zero_info_3", strMenu, sizeof strMenu,"Zero fault(3):lead off.");	
			SetWindowText(editzeroview,strMenu);
		}
		if(gValueIbp1.bZeroResult==4){
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "IBP", "zero_info_4", strMenu, sizeof strMenu,"Zero fault(4):lead off.");	
			SetWindowText(editzeroview,strMenu);
		}
		if(gValueIbp1.bZeroResult==10){
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "IBP", "no_set_zero", strMenu, sizeof strMenu,"Didn't zero");	
			SetWindowText(editzeroview,strMenu);
		}
		
		flag_zero=0;

		ReleaseDC(hdc);
		
	}
	return 0;
}
static int SetScaleText(HWND hWnd, int iLimit)
{
	unsigned char strLimit[50] = {0};
	unsigned char strUnit[20] = {0};
	switch(gCfgIbp1.bUnit){
		case IBPUNIT_KPA:{
			SetWindowText(hWnd, strLimit);
			memset(strUnit, 0, sizeof strUnit);
			GetStringFromResFiles(gsLanguageRes, "GENERAL", "unit_kpa", strUnit, sizeof strUnit,"Kpa");		
			memset(strLimit, 0, sizeof strLimit);
			snprintf(strLimit, sizeof strLimit, "%3.1f %s", (float)(iLimit / 7.5), strUnit);
			SetWindowText(hWnd, strLimit);
		}break;
		/*case IBPUNIT_CMH2O:{
			memset(strUnit, 0, sizeof strUnit);
			GetStringFromResFile(gsLanguageRes, "GENERAL", "unit_cmh2o", strUnit, sizeof strUnit);		
			memset(strLimit, 0, sizeof strLimit);
			snprintf(strLimit, sizeof strLimit, "%d", iLimit);
			SetWindowText(hWnd, strLimit);
		}break;*/
		default:{
			//IBPUNIT_MMHG
			memset(strUnit, 0, sizeof strUnit);
			GetStringFromResFiles(gsLanguageRes, "GENERAL", "unit_mmhg", strUnit, sizeof strUnit,"mmHg");		
			memset(strLimit, 0, sizeof strLimit);
			snprintf(strLimit, sizeof strLimit, "%d %s", iLimit, strUnit);
			SetWindowText(hWnd, strLimit);
		}break;
	}
	
	return 0;
}
static int EditProcHelp(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	char strHelp[200] = {0};
	SetWindowBkColor(hWnd,COLOR_black);
#ifdef FONTCOLOR	
	SetWindowElementColorEx( hWnd, FGC_CONTROL_NORMAL,COLOR_yellow);
#endif	
	switch(message){
		case MSG_PAINT:{
			id = lParam;
			switch(id){
				case DID_IBP1_ZERO_ZERO:{
					
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "IBP", "set_help_zero", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
						

				}break;
				case DID_IBP1_ZERO_ADJUSTVALUE:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "IBP", "set_help_cal", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_IBP1_ZERO_ADJUSTBTN:{
					
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "IBP", "set_help_cal_start", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
					

				}break;
				case DID_IBP1_ZERO_OK:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "GENERAL", "exit", strHelp, sizeof strHelp);	
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
						case DID_IBP1_ZERO_EDITKP:{
							if(initvalue<iTmpTop){
								initvalue += 5;
								SetScaleText(editsetkp,initvalue);
		
							}
						}break;
					}
				}break;
				case SCANCODE_CURSORBLOCKUP:{
					switch(id){

						case DID_IBP1_ZERO_EDITKP:{
							if(initvalue >iTmpBottom){
								initvalue -=5;
								SetScaleText(editsetkp,initvalue);
		
							}
						}break;
					}
				}break;
				case SCANCODE_ENTER:{
					switch(id){
						case DID_IBP1_ZERO_EDITKP:{

							SetFocus(btnadjustKp);

						}break;

					}	
					gbKeyType = KEY_TAB;
				}break;
			}
			return 0;
		}break;	
	}
	
	temp = (*OldEditProc)(hWnd, message, wParam, lParam);
	return(temp);
}		


static int CreateCtlProc(HWND hDlg)
{
			editCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE  ,
					     		DID_IBP1_ZERO_CAPTION, 0, 5, 250, 26, hDlg, 0);
			btnadjustzero = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
							DID_IBP1_ZERO_ZERO, 10, 30+29*0, 115, 26, hDlg, 0);
			btnadjustKp = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
							DID_IBP1_ZERO_ADJUSTVALUE, 10, 30+29*1, 115, 26, hDlg, 0);
			btnadjust=CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
							DID_IBP1_ZERO_ADJUSTBTN, 10, 30+29*2, 115, 26, hDlg, 0);
			btnOk= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
							DID_IBP1_ZERO_OK, 10, 30+29*3, 115, 26, hDlg, 0);			
			editzeroview =CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_LEFT | WS_VISIBLE  ,
					     		DID_IBP1_ZERO_INFO, 15, 30+29*6, 240, 52, hDlg, 0);
			editsetkp = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER, 
							DID_IBP1_ZERO_EDITKP, 130, 30+29*1,95, 26, hDlg, 0);
			editHelp = GetDlgItem(hDlg, DID_IBP1_ZERO_HELP);
			
			edittime = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE  ,
					     		DID_IBP1_ZERO_TIME2, 125, 33+29*2, 110, 32, hDlg, 0);

}



static int InitCtlName(HWND hDlg)
{
			char strMenu[100];
			
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "IBP", "caption_cal", strMenu, sizeof strMenu,"Calibrate And Zero");	
			SetWindowText(editCaption,strMenu);


			memset(strMenu, 0, sizeof strMenu);
			
		//	GetSysTime(&itime);
			snprintf(strMenu, sizeof strMenu, "%d - %d - %d",  gValueIbp1.sCalTime.wYear
				,gValueIbp1.sCalTime.bMonth,gValueIbp1.sCalTime.bDay);
			//GetStringFromResFiles(gsLanguageRes, "IBP", "```", strMenu, sizeof strMenu,"0000-00-00");	
			SetWindowText(edittime,strMenu);

			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "IBP", "set_zero", strMenu, sizeof strMenu,"Zero");	
			SetWindowText(btnadjustzero,strMenu);

			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "IBP", "set_cal_value", strMenu, sizeof strMenu,"Value");	
			SetWindowText(btnadjustKp,strMenu);
			
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "IBP", "set_cal", strMenu, sizeof strMenu,"Calibrate");	
			SetWindowText(btnadjust,strMenu);
			
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "GENERAL", "exit", strMenu, sizeof strMenu,"exit");	
			SetWindowText(btnOk,strMenu);
			

			
}

static int InitCtlCallBackProc(HWND hDlg)
{
			OldBtnProc = SetWindowCallbackProc(btnadjustzero, BtnProc);
			OldBtnProc = SetWindowCallbackProc(btnadjustKp, BtnProc);
			OldBtnProc = SetWindowCallbackProc(btnadjust, BtnProc);
			OldEditProc = SetWindowCallbackProc(editsetkp, EditProc);
			OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);			
			OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp);
}
static int InitCtlStatus(HWND hDlg)
{
//	if(gValueIbp1.bZeroResult != 0)
		EnableWindow(btnadjust,FALSE);
	
	return 0;
}
static void SetLimt()
{
		static char str[10];
		//得到波形上下限
		iTmpTop =300 ;
		iTmpBottom =80 ;
		initvalue=200;
	
		SetScaleText(editsetkp,initvalue);
			

}


static int DlgProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	char strMenu[100];
	int i;
	RECT rcDlg;
		
	switch(message){
		case MSG_INITDIALOG:{
			ibp1whnd=hDlg;
			iBioCMD_IBP_POLL_STATE(0,1);
			
			//创建控件
			CreateCtlProc(hDlg);
			//设置控件字符
			InitCtlName(hDlg);
			//设置控件回调函数
			InitCtlCallBackProc(hDlg);
			InitCtlStatus(hDlg);
			SetLimt();

			gbKeyType = KEY_TAB;
			iBioCMD_IBP_POLL_STATE(0,0);	
						
			SetFocus(btnOk);
		}break;
		case MSG_PAINT:{

				//IBPZeroinfo();

		}break;
		case MSG_COMMAND:{
			int	id   = LOWORD(wParam);
			
			switch(id){
				case DID_IBP1_ZERO_ZERO:{

						flag_zero=1;
						gValueIbp1.Calviewmode=2;
						
						iBioCMD_IBP_ZERO(0);
					//	iBioCMD_IBP_POLL_STATE(0,0);	
					//	SetFocus(btnadjustzero);
					
				}break;
				case DID_IBP1_ZERO_ADJUSTVALUE:{
					gbKeyType = KEY_UD;
					SetFocus(editsetkp);
				}break;

				case DID_IBP1_ZERO_OK:{
					PostMessage(hDlg, MSG_CLOSE, 0, 0L);
				}break;
				case DID_IBP1_ZERO_ADJUSTBTN:{
					flag_cal=1;
					flagtime=1;
					gValueIbp1.Calviewmode=1;
					GetSysTime(&itime);
				//	iBioCMD_IBP_POLL_STATE(0,0);
					iBioCMD_IBP_CAL1(initvalue-80,&itime);
					iBioCMD_IBP_POLL_STATE(0,1);
					SetFocus(btnadjust);
					
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
			
			//向中央机发送信息
			NetSend_Ibp1Info();


			EndDialog(hDlg, wParam);	
		}break;
	}
	
	temp = DefaultDialogProc(hDlg, message, wParam, lParam);
	return(temp);
}
void CreateDlgIbp1Setzero(HWND hWnd)
{
	DlgSet.controls = CtrlSet;
	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}

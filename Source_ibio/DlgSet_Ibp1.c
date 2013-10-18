/*************************************************************************** 
 *	Module Name:	DlgSet_Ibp1
 *
 *	Abstract:	设置IBP1配置信息的主对话框
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2007-06-14 16:32:08	
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"

#define DID_IBP1					7000
#define DID_IBP1_HELP			DID_IBP1
#define DID_IBP1_OK 				DID_IBP1+1
#define DID_IBP1_LABEL			DID_IBP1+2
#define DID_IBP1_SET_LABEL		DID_IBP1+3
#define DID_IBP1_UNIT			DID_IBP1+4
#define DID_IBP1_SET_UNIT		DID_IBP1+5
#define DID_IBP1_VIEW			DID_IBP1+6
#define DID_IBP1_SET_VIEW		DID_IBP1+7
#define DID_IBP1_SPEED			DID_IBP1+8
#define DID_IBP1_SET_SPEED		DID_IBP1+9
#define DID_IBP1_TOP				DID_IBP1+10
#define DID_IBP1_SET_TOP		DID_IBP1+11
#define DID_IBP1_MIDDLE			DID_IBP1+12
#define DID_IBP1_SET_MIDDLE	DID_IBP1+13
#define DID_IBP1_BOTTOM		DID_IBP1+14
#define DID_IBP1_SET_BOTTOM	DID_IBP1+15
#define DID_IBP1_ZERO			DID_IBP1+16
#define DID_IBP1_SET_ALARM		DID_IBP1+17
#define DID_NEWIBP1_CAPTION	DID_IBP1+18
#define DID_IBP1_FILTER			DID_IBP1+19
#define DID_IBP1_SET_FILTER		DID_IBP1+20
//帮助对话框
static HWND editHelp;
static WNDPROC OldEditProcHelp;
//其他控件
static HWND btnOk;
static HWND btnSetLabel;
static HWND cobLabel;
static HWND btnSetUnit;
static HWND cobUnit;
static HWND btnSetView;
static HWND cobView;
static HWND btnSetSpeed;
static HWND cobSpeed;
static HWND btnSetTop;
static HWND editTop;
static HWND btnSetMiddle;
static HWND editMiddle;
static HWND btnSetBottom;
static HWND editBottom;
static HWND btnZero;
static HWND btnSetAlarm;
static HWND editCaption;
static HWND btnSetFilter;
static HWND cobFilter;

//控件回调函数
static WNDPROC OldBtnProc;	
static WNDPROC OldCobProc;
static WNDPROC OldEditProc;

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
		DID_IBP1_HELP,
		"",
		0
	}
};


//波形刻度调整的增量mmHg
#define IBP1_SCALE_INTER 		5

//刻度限值
#define IBP1_SCALE_MAX		IBP_MAX
#define IBP1_SCALE_MIN		IBP_MIN

//波形最小幅度
#define IBP1_RANGE_MIN 		20

//IBP 标名索引
extern unsigned char *gStrIbpLabelIndex[];

//临时存储上下限，调节的时候使用
static int iTmpScaleTop =0, iTmpScaleBottom =0;
//显示
unsigned char *strViewIndex[]={"view_all", "view_mean"};	
	

/*
	根据单位设置标尺上下限
*/
static int SetScaleText(HWND hWnd, int iLimit)
{
	unsigned char strLimit[50] = {0};
	unsigned char strUnit[20] = {0};
	
	switch(gCfgIbp1.bUnit){
		case IBPUNIT_KPA:{
			memset(strUnit, 0, sizeof strUnit);
			GetStringFromResFiles(gsLanguageRes, "GENERAL", "unit_kpa", strUnit, sizeof strUnit,"Kpa");		
			memset(strLimit, 0, sizeof strLimit);
			snprintf(strLimit, sizeof strLimit, "%3.1f %s", (float)(iLimit / 7.5), strUnit);
			SetWindowText(hWnd, strLimit);
		}break;
		/*case IBPUNIT_CMH2O:{
			memset(strUnit, 0, sizeof strUnit);
			GetStringFromResFile(gsLanguageRes, "GENERAL", "unit_cmh20", strUnit, sizeof strUnit);		
			memset(strLimit, 0, sizeof strLimit);
			snprintf(strLimit, sizeof strLimit, "%d %s", iLimit, strUnit);
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

/*
	血管不同的自动设置项目：
	1.显示方式
	2.单位
	3.波形范围
	4.报警源
	5.报警限
*/
static int ChangeIbp1Lable(BYTE bLabel)
{
	switch(bLabel){
		case IBPLAB_ART:
		case IBPLAB_PA:
		case IBPLAB_AO:
		case IBPLAB_UAP:
		case IBPLAB_BAP:
		case IBPLAB_FAP:{
			//Unit: mmHg 
			gCfgIbp1.bUnit = IBPUNIT_MMHG;
			SendMessage(cobUnit, CB_SETCURSEL, gCfgIbp1.bUnit, 0);
			
			//View Mode: SD(m)
			gCfgIbp1.bViewMode = IBPVIEW_ALL;
			SendMessage(cobView, CB_SETCURSEL, gCfgIbp1.bViewMode, 0);
			//改变显示模式
			ChangeIbp1ViewMode();
			
			//Scale 0~150
			gCfgIbp1.iScaleTop = 150;		
			iTmpScaleTop = gCfgIbp1.iScaleTop;
			SetScaleText(editTop, gCfgIbp1.iScaleTop);
			gCfgIbp1.iScaleBottom = 0;		
			iTmpScaleBottom = gCfgIbp1.iScaleBottom;
			SetScaleText(editBottom, gCfgIbp1.iScaleBottom);
			//改变波形显示
			ChangeWaveRange_Ibp1();
			
			//Alarm Source: 可选
			//Sys Limit 160~90
			gCfgIbp1.iHigh_Sys = 160;
			gCfgIbp1.iLow_Sys = 90;
			
			//Dia Limit 90~50
			gCfgIbp1.iHigh_Dia = 90;
			gCfgIbp1.iLow_Dia = 50;
			
			//Mean Limit 110~70
			gCfgIbp1.iHigh_Mean = 110;
			gCfgIbp1.iLow_Mean = 70;
		}break;
		case IBPLAB_CVP:
		case IBPLAB_LAP:
		case IBPLAB_RAP:
		case IBPLAB_ICP:
		case IBPLAB_UVP:{
			//Unit: cmH2O
			//gCfgIbp1.bUnit = IBPUNIT_CMH2O;
			gCfgIbp1.bUnit = IBPUNIT_MMHG;
			SendMessage(cobUnit, CB_SETCURSEL, gCfgIbp1.bUnit, 0);
			
			//View Mode: M(sd)
			gCfgIbp1.bViewMode = IBPVIEW_M;
			SendMessage(cobView, CB_SETCURSEL, gCfgIbp1.bViewMode, 0);
			//改变显示模式
			ChangeIbp1ViewMode();
			
			//Scale 0~40
			gCfgIbp1.iScaleTop = 40;		
			iTmpScaleTop = gCfgIbp1.iScaleTop;
			SetScaleText(editTop, gCfgIbp1.iScaleTop);
			gCfgIbp1.iScaleBottom = 0;		
			iTmpScaleBottom = gCfgIbp1.iScaleBottom;
			SetScaleText(editBottom, gCfgIbp1.iScaleBottom);
			//改变波形显示
			ChangeWaveRange_Ibp1();
			
			//Alarm Source: Mean
			gCfgIbp1.bAlmSource = IBP_ALM_SRC_M;
					
			//Mean Limit 10~0
			gCfgIbp1.iHigh_Mean = 10;
			gCfgIbp1.iLow_Mean = 0;
			
		}break;
	}
	
	//写入配置文件
	SetIntValueToResFile(gFileSetup, "IBP1Setup", "unit",  gCfgIbp1.bUnit, 1);
	SetIntValueToResFile(gFileSetup, "IBP1Setup", "view_mode",  gCfgIbp1.bViewMode, 1);
	SetIntValueToResFile(gFileSetup, "IBP1Setup", "scale_top",  gCfgIbp1.iScaleTop, 4);
	SetIntValueToResFile(gFileSetup, "IBP1Setup", "scale_bottom",  gCfgIbp1.iScaleBottom, 4);
	SetIntValueToResFile(gFileSetup, "IBP1Setup", "alarm_source",  gCfgIbp1.bAlmSource, 1);
	SetIntValueToResFile(gFileSetup, "IBP1Setup", "sys_high",  gCfgIbp1.iHigh_Sys, 4);
	SetIntValueToResFile(gFileSetup, "IBP1Setup", "sys_low",  gCfgIbp1.iLow_Sys, 4);
	SetIntValueToResFile(gFileSetup, "IBP1Setup", "dia_high",  gCfgIbp1.iHigh_Dia, 4);
	SetIntValueToResFile(gFileSetup, "IBP1Setup", "dia_low",  gCfgIbp1.iLow_Dia, 4);
	SetIntValueToResFile(gFileSetup, "IBP1Setup", "mean_high",  gCfgIbp1.iHigh_Mean, 4);
	SetIntValueToResFile(gFileSetup, "IBP1Setup", "mean_low",  gCfgIbp1.iLow_Mean, 4);
	
	return 0;
}


/*
	帮助控件回调函数
*/
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
				case DID_IBP1_SET_LABEL:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_ibp_label", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_IBP1_SET_UNIT:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_ibp_unit", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_IBP1_SET_VIEW:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_ibp_view", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_IBP1_SET_SPEED:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_ibp_speed", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_IBP1_SET_TOP:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_ibp_scale_top", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_IBP1_SET_MIDDLE:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_ibp_scale_middle", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_IBP1_SET_BOTTOM:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_ibp_scale_bottom", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_IBP1_SET_ALARM:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_ibp_set_alarm", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_IBP1_ZERO:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_ibp_zero", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_IBP1_OK:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_exit", strHelp, sizeof strHelp);	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_IBP1_FILTER:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFile(gsLanguageRes, "HELP", "help_ibp_filter", strHelp, sizeof strHelp);	
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
						case DID_IBP1_LABEL:{
							if(gCfgIbp1.bLabel != index){
								ChangeIbp1Lable(index);
							}
							gCfgIbp1.bLabel = index;
							SetIntValueToResFile(gFileSetup, "IBP1Setup", "label",  gCfgIbp1.bLabel, 1);
							ChangeWaveGain_Ibp1();
							SetFocus(btnSetLabel);
						}break;
						case DID_IBP1_UNIT:{
							gCfgIbp1.bUnit = index;
							SetIntValueToResFile(gFileSetup, "IBP1Setup", "unit",  gCfgIbp1.bUnit, 1);
							SetFocus(btnSetUnit);
							
							//根据单位设置标尺上下限
							SetScaleText(editTop, gCfgIbp1.iScaleTop);		
							SetScaleText(editBottom, gCfgIbp1.iScaleBottom);		
						}break;
						case DID_IBP1_VIEW:{
							gCfgIbp1.bViewMode = index;
							SetIntValueToResFile(gFileSetup, "IBP1Setup", "view_mode",  gCfgIbp1.bViewMode, 1);
							SetFocus(btnSetView);
							ChangeIbp1ViewMode();
						}break;
						case DID_IBP1_SPEED:{
							gCfgIbp1.bScanSpeed = index+1;
							SetIntValueToResFile(gFileSetup, "IBP1Setup", "scan_speed",  gCfgIbp1.bScanSpeed, 1);
							SetFocus(btnSetSpeed);
							ChangeWaveSpeed_Ibp1();
						}break;
						case DID_IBP1_SET_FILTER:{
							gCfgIbp1.bFilterMode= index;
							SetIntValueToResFile(gFileSetup, "IBP1Setup", "filter_mode",  gCfgIbp1.bFilterMode, 1);
							SetFocus(btnSetFilter);
							ChangeFilter_Ibp1();
							
							}break;
					}	
					gbKeyType = KEY_TAB;
				}break;
			}
			return 0;
		}break;	
	}	
	temp = (*OldCobProc)(hWnd, message, wParam, lParam);
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
						case DID_IBP1_TOP:{
							if(iTmpScaleTop<IBP1_SCALE_MAX){
								iTmpScaleTop += IBP1_SCALE_INTER;
								SetScaleText(hWnd, iTmpScaleTop);			
							}
						}break;
						case DID_IBP1_BOTTOM:{
							if(iTmpScaleBottom < (iTmpScaleTop - IBP1_RANGE_MIN)){
								iTmpScaleBottom += IBP1_SCALE_INTER;
								SetScaleText(hWnd, iTmpScaleBottom);			
							}
						}break;
					}
				}break;
				case SCANCODE_CURSORBLOCKUP:{
					switch(id){
						case DID_IBP1_TOP:{
							if(iTmpScaleTop > (iTmpScaleBottom + IBP1_RANGE_MIN)){
								iTmpScaleTop -= IBP1_SCALE_INTER;
								SetScaleText(hWnd, iTmpScaleTop);			
							}
						}break;
						case DID_IBP1_BOTTOM:{
							if(iTmpScaleBottom >IBP1_SCALE_MIN){
								iTmpScaleBottom -= IBP1_SCALE_INTER;
								SetScaleText(hWnd, iTmpScaleBottom);			
							}
						}break;
					}
				}break;
				case SCANCODE_ENTER:{
					switch(id){
						case DID_IBP1_TOP:{
							gCfgIbp1.iScaleTop = iTmpScaleTop;
							SetIntValueToResFile(gFileSetup, "IBP1Setup", "scale_top",  gCfgIbp1.iScaleTop, 4);
							SetFocus(btnSetTop);
// 							ChangeWaveRange_Ibp1();
							ChangeWaveGain_Ibp1();
						}break;
						case DID_IBP1_BOTTOM:{
							gCfgIbp1.iScaleBottom = iTmpScaleBottom;
							SetIntValueToResFile(gFileSetup, "IBP1Setup", "scale_bottom",  gCfgIbp1.iScaleBottom, 4);
							SetFocus(btnSetBottom);
// 							ChangeWaveRange_Ibp1();
							ChangeWaveGain_Ibp1();
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

/*
	对话框回调函数
*/
static int CreateCtlProc(HWND hDlg)
{
			editCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE  ,
					     		DID_NEWIBP1_CAPTION, 0, 5, 250, 26, hDlg, 0);
													
			btnSetLabel = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
							DID_IBP1_SET_LABEL, 10, 30+29*0, 115, 26, hDlg, 0);
			btnSetView = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
							DID_IBP1_SET_VIEW, 10, 30+29*1 ,115, 26, hDlg, 0);
			btnSetUnit = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
							DID_IBP1_SET_UNIT, 10, 30+29*2, 115, 26, hDlg, 0);
			btnSetSpeed = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
							DID_IBP1_SET_SPEED, 10, 30+29*3, 115, 26, hDlg, 0);
			btnSetFilter = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
							DID_IBP1_FILTER, 10, 30+29*4, 115, 26, hDlg, 0);

			btnSetTop = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
							DID_IBP1_SET_TOP, 10, 30+29*5, 115, 26, hDlg, 0);
			btnSetBottom = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
							DID_IBP1_SET_BOTTOM, 10, 30+29*6, 115, 26, hDlg, 0);

			btnZero = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
							DID_IBP1_ZERO, 10, 30+29*7, 115, 26, hDlg, 0);
			btnSetAlarm = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					       DID_IBP1_SET_ALARM, 10, 30+29*8, 115, 26, hDlg, 0);
			btnOk= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
							DID_IBP1_OK, 10, 30+29*9, 115, 26, hDlg, 0);

			
			cobLabel = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
							DID_IBP1_LABEL, 130, 30+29*0, 95, 26, hDlg, 0);

			cobView = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
							DID_IBP1_VIEW, 130, 30+29*1, 95, 26, hDlg, 0);

			cobUnit = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
							DID_IBP1_UNIT, 130, 30+29*2, 95, 26, hDlg, 0);

			cobSpeed = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
							DID_IBP1_SPEED, 130, 30+29*3, 95, 23, hDlg, 0);
			cobFilter= CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
							DID_IBP1_SET_FILTER, 130, 30+29*4, 95, 23, hDlg, 0);	


			editTop = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
							DID_IBP1_TOP, 130, 30+29*5, 95, 23, hDlg, 0);

			editBottom = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
							DID_IBP1_BOTTOM, 130, 30+29*6, 95, 23, hDlg, 0);

			editHelp = GetDlgItem(hDlg, DID_IBP1_HELP);	


			
}

static int InitCtlName(HWND hDlg)
{
			char strMenu[100];
			
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "IBP1", "caption", strMenu, sizeof strMenu,"IBP1");	
			SetWindowText(editCaption,strMenu);

			
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "IBP", "label", strMenu, sizeof strMenu,"Label");	
			SetWindowText(btnSetLabel,strMenu);			

			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "IBP", "view", strMenu, sizeof strMenu,"View Mode");
			SetWindowText(btnSetView,strMenu);			


			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "IBP", "unit", strMenu, sizeof strMenu,"Unit");
			SetWindowText(btnSetUnit,strMenu);			


			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "IBP", "speed", strMenu, sizeof strMenu,"Speed");	
			SetWindowText(btnSetSpeed,strMenu);			

			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "IBP", "filter", strMenu, sizeof strMenu,"Fliter Mode");	
			SetWindowText(btnSetFilter,strMenu);

			//Scale Top
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "IBP", "scale_top", strMenu, sizeof strMenu,"scale Top");	
			SetWindowText(btnSetTop,strMenu);			


			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "IBP", "scale_bottom", strMenu, sizeof strMenu,"scale Btm");				
			SetWindowText(btnSetBottom,strMenu);			


			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "IBP", "cal", strMenu, sizeof strMenu,"Cal And Zero");	
			SetWindowText(btnZero,strMenu);			


			//Set Alarm
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "IBP", "set_alarm", strMenu, sizeof strMenu,"Set Alarm");	
			
			SetWindowText(btnSetAlarm,strMenu);			


			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "GENERAL", "exit", strMenu, sizeof strMenu,"exit");	
			SetWindowText(btnOk,strMenu);			

			

}

static int InitCobList(HWND hDlg)
{
	int i;
	char strMenu[100];

			for(i=0; i<8; i++){
				memset(strMenu, 0, sizeof strMenu);
				GetStringFromResFile(gsLanguageRes, "IBP", gStrIbpLabelIndex[i], strMenu, sizeof strMenu);	
				SendMessage(cobLabel, CB_ADDSTRING, 0, (LPARAM)strMenu);
			}
			SendMessage(cobLabel, CB_SETCURSEL, gCfgIbp1.bLabel, 0);

			for(i=0; i<2; i++){
				memset(strMenu, 0, sizeof strMenu);
				GetStringFromResFile(gsLanguageRes, "IBP", strViewIndex[i], strMenu, sizeof strMenu);	
				SendMessage(cobView, CB_ADDSTRING, 0, (LPARAM)strMenu);
			}
			SendMessage(cobView, CB_SETCURSEL, gCfgIbp1.bViewMode, 0);


			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "GENERAL", "unit_mmhg", strMenu, sizeof strMenu,"mmHg");	
			SendMessage(cobUnit, CB_ADDSTRING, 0, (LPARAM)strMenu);
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "GENERAL", "unit_kpa", strMenu, sizeof strMenu,"Kpa");	
			SendMessage(cobUnit, CB_ADDSTRING, 0, (LPARAM)strMenu);
		//	memset(strMenu, 0, sizeof strMenu);
		//	GetStringFromResFile(gsLanguageRes, "GENERAL", "unit_cmh2o", strMenu, sizeof strMenu);	
			//SendMessage(cobUnit, CB_ADDSTRING, 0, (LPARAM)strMenu);
			SendMessage(cobUnit, CB_SETCURSEL, gCfgIbp1.bUnit, 0);

			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "GENERAL", "speed_125", strMenu, sizeof strMenu,"12.5");	
			SendMessage(cobSpeed, CB_ADDSTRING, 0, (LPARAM)strMenu);
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "GENERAL", "speed_25", strMenu, sizeof strMenu,"25");	
			SendMessage(cobSpeed, CB_ADDSTRING, 0, (LPARAM)strMenu);
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "GENERAL", "speed_50", strMenu, sizeof strMenu,"50");	
			SendMessage(cobSpeed, CB_ADDSTRING, 0, (LPARAM)strMenu);
			//因为最小的扫描速度是6.25mm/s,索引值为0, 而IBP的最小扫描速度为12.5mm/s，索引为1
			SendMessage(cobSpeed, CB_SETCURSEL, gCfgIbp1.bScanSpeed-1, 0);


			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "GENERAL", "smoothing_filtering", strMenu, sizeof strMenu,"smoothing_filtering");	
			SendMessage(cobFilter, CB_ADDSTRING, 0, (LPARAM)strMenu);
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "GENERAL", "normal_filtering", strMenu, sizeof strMenu,"smoothing_filtering");	
			SendMessage(cobFilter, CB_ADDSTRING, 0, (LPARAM)strMenu);
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "GENERAL", "filter_off", strMenu, sizeof strMenu,"smoothing_filtering");	
			SendMessage(cobFilter, CB_ADDSTRING, 0, (LPARAM)strMenu);



			SendMessage(cobFilter, CB_SETCURSEL, gCfgIbp1.bFilterMode, 0);



	
}
static int InitCtlCallBackProc(HWND hDlg)
{
			OldBtnProc = SetWindowCallbackProc(btnSetLabel, BtnProc);
			OldBtnProc = SetWindowCallbackProc(btnSetView, BtnProc);
			OldBtnProc = SetWindowCallbackProc(btnSetUnit, BtnProc);
			OldBtnProc = SetWindowCallbackProc(btnSetTop, BtnProc);
			OldBtnProc = SetWindowCallbackProc(btnSetSpeed, BtnProc);
			OldBtnProc = SetWindowCallbackProc(btnSetBottom, BtnProc);
			OldBtnProc = SetWindowCallbackProc(btnSetFilter, BtnProc);			
			OldBtnProc = SetWindowCallbackProc(btnZero, BtnProc);
			OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);
			OldBtnProc = SetWindowCallbackProc(btnSetAlarm, BtnProc);
			
			OldCobProc = SetWindowCallbackProc(cobLabel, CobProc);			
			OldCobProc = SetWindowCallbackProc(cobView, CobProc);			
			OldCobProc = SetWindowCallbackProc(cobUnit, CobProc);
			OldCobProc = SetWindowCallbackProc(cobSpeed, CobProc);
			OldCobProc = SetWindowCallbackProc(cobFilter, CobProc);			

			OldEditProc = SetWindowCallbackProc(editTop, EditProc);
			OldEditProc = SetWindowCallbackProc(editBottom, EditProc);
			OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp);

 

}

static int DrawIconProc(HWND hDlg)
{
	
	int i;
	for(i=7;i<9;i++){
		CreateWindow("static",">>",WS_CHILD|WS_VISIBLE,IDC_STATIC,
	 		140, 30+29*i, 30, 16,hDlg,0);
		
	}
	return 0;

	

}
static void SetLimt()
{
			//得到波形上下限
		iTmpScaleTop = gCfgIbp1.iScaleTop;
		iTmpScaleBottom = gCfgIbp1.iScaleBottom;
			
			
		SetScaleText(editTop, gCfgIbp1.iScaleTop);	
		SetScaleText(editTop, gCfgIbp1.iScaleTop);		
		SetScaleText(editBottom, gCfgIbp1.iScaleBottom);
}
static int DlgProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	char strMenu[100];
	int i;
	RECT rcDlg;
		
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
			//绘制下一页图标
			DrawIconProc(hDlg);
		

			SetLimt();

			gbKeyType = KEY_TAB;

						
			SetFocus(btnOk);
		}break;
		case MSG_PAINT:{

		}break;
		case MSG_COMMAND:{
			int	id   = LOWORD(wParam);
			
			switch(id){
				case DID_IBP1_SET_LABEL:{
					gbKeyType = KEY_UD;
					SetFocus(cobLabel);
				}break;
				case DID_IBP1_SET_UNIT:{
					gbKeyType = KEY_UD;
					SetFocus(cobUnit);
				}break;
				case DID_IBP1_SET_VIEW:{
					gbKeyType = KEY_UD;
					SetFocus(cobView);
				}break;
				case DID_IBP1_SET_SPEED:{
					gbKeyType = KEY_UD;
					SetFocus(cobSpeed);
				}break;
				case DID_IBP1_FILTER:{
					gbKeyType = KEY_UD;
					SetFocus(cobFilter);
				}break;				
				case DID_IBP1_SET_TOP:{
					gbKeyType = KEY_UD;
					SetFocus(editTop);
				}break;
				case DID_IBP1_SET_BOTTOM:{
					gbKeyType = KEY_UD;
					SetFocus(editBottom);
				}break;
				case DID_IBP1_ZERO:{
					//置状态为校0状态
					//gValueIbp1.bStatus = IBP_STATUS_ZERO;
					CreateDlgIbp1Setzero(hDlg);
				}break;
				case DID_IBP1_SET_ALARM:{
 					CreateDlgIbp1Set(hDlg);
					RestorWave();
				}break;
				case DID_IBP1_OK:{
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
			
			//向中央机发送信息
			NetSend_Ibp1Info();


			EndDialog(hDlg, wParam);	
		}break;
	}
	
	temp = DefaultDialogProc(hDlg, message, wParam, lParam);
	return(temp);
}

/*
	建立对话框
*/
void CreateDlgIbp1(HWND hWnd)
{
	DlgSet.controls = CtrlSet;
	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}








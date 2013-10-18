/*************************************************************************** 
 *	Module Name:	DlgSet_Nibp
 *
 *	Abstract:	设置Nibp配置信息的对话框
 *	VP:Venipuncture 静脉穿刺
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2007-06-12 18:42:37
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"
#include "General_Funcs.h"
#include "Nibp_Measure.h"

//DID_NEWNIBP
#define DID_NEWNIBP_HELP				DID_NEWNIBP
#define DID_NEWNIBP_OK 				DID_NEWNIBP+1
#define DID_NEWNIBP_CAPTION 			DID_NEWNIBP+2
#define DID_NEWNIBP_UNIT				DID_NEWNIBP+3
#define DID_NEWNIBP_OBJECT			DID_NEWNIBP+4
#define DID_NEWNIBP_MODE				DID_NEWNIBP+5
#define DID_NEWNIBP_INTERVAL			DID_NEWNIBP+6
#define DID_NEWNIBP_INITPRES			DID_NEWNIBP+7
#define DID_NEWNIBP_RESTE				DID_NEWNIBP+8
#define DID_NEWNIBP_ALARM				DID_NEWNIBP+9
#define DID_NEWNIBP_UNIT_COB			DID_NEWNIBP+10
#define DID_NEWNIBP_OBJECT_COB		DID_NEWNIBP+11
#define DID_NEWNIBP_MODE_COB			DID_NEWNIBP+12
#define DID_NEWNIBP_INTERVAL_COB		DID_NEWNIBP+13
#define DID_NEWNIBP_INITPRES_EDIT		DID_NEWNIBP+14
#define DID_NEWNIBP_VP  				DID_NEWNIBP+15 
#define DID_NEWNIBP_VP_EDIT  			DID_NEWNIBP+16
#define DID_NEWNIBP_STARTVEN  		DID_NEWNIBP+17

//帮助对话框
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;

//其他控件
static HWND btnOk;
static HWND btnUnit;			//压力显示单位kpa=mmHg/7.5
static HWND btnObject;		//病人类型
static HWND btnMode;			//测量模式: 手动/自动/连续
static HWND btnInterval;		//测量周期
static HWND btnInitPres; 		//初始化初次充气压力
static HWND btnVP;			//Venipuncture静脉穿刺压力
static HWND btnStartVen;		//启动静脉穿刺
static HWND btnAlarm;			
static HWND btnReset;

static HWND cobUnit;
static HWND cobObject;
static HWND cobInterval;
static HWND editInitPres;
static HWND editVP;
static HWND cobMode;

//控件回调函数
static WNDPROC OldBtnProc;	
static WNDPROC OldCobProc;
static WNDPROC OldEditProc;

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
		DID_NEWNIBP_HELP,
		"",
		0
	}
};

//临时存储上下限，调节的时候使用
static int iTmpInitPres = 0,iTmpVP = 0;

//上下限刻度调整的增量 mmHg
#define PRES_LIMIT_INTER	20	
#define VP_LIMIT_INTER	1

//限值 mmHg
#define VP_LIMIT_MAX		120
#define VP_LIMIT_MIN		20

//上下限
static int Limit_initPres_High=0,Limit_initPres_Low=0,Limit_initPres_Init=0;;
static int Limit_VP_High=0,Limit_VP_Low=0,Limit_VP_Init=0;
BOOL NIBP_Object_Change;
//周期测量时间
static unsigned char *strCycleTime[]={"1min", "2min", "3min", "4min", "5min", "10min",
		"15min", "30min", "60min", "90min", "120min", "180min", "240min", "480min"};
static int InitLimit();	
static int InitEditText();
static int InitCtlStatus();
/*
	转换索引为实际间隔时间
	 返回: 实际的分钟
*/
int TransformIndexToTime(int iIndex)
{
	int iMin=0;
		
	switch(iIndex){
		case 0:{ iMin = 1;}break;
		case 1:{ iMin = 2;}break;
		case 2:{ iMin = 3;}break;
		case 3:{ iMin = 4;}break;
		case 4:{ iMin = 5;}break;
		case 5:{ iMin = 10;}break;
		case 6:{ iMin = 15;}break;
		case 7:{ iMin = 30;}break;
		case 8:{ iMin = 60;}break;
		case 9:{ iMin = 90;}break;
		case 10:{ iMin = 120;}break;
		case 11:{ iMin = 180;}break;
		case 12:{ iMin = 240;}break;
		case 13:{ iMin = 480;}break;
		default:{ 
			iMin = 5;	
		}break;
	}
	
// 	if(B_PRINTF) printf("%s:%d IndexToTime  Index is %d, cycle is %d.\n", __FILE__, __LINE__, iIndex, iMin);
	
	return iMin;
}

/*
	转换实际时间为索引
	返回:索引
*/

int TransformTimeToIndex(int iMin)
{
	int index=0;
	
	switch(iMin){
		case 1:{ index = 0;}break;
		case 2:{ index = 1;}break;
		case 3:{ index = 2;}break;
		case 4:{ index = 3;}break;
		case 5:{ index = 4;}break;
		case 10:{ index = 5;}break;
		case 15:{ index = 6;}break;
		case 30:{ index = 7;}break;
		case 60:{ index = 8;}break;
		case 90:{ index = 9;}break;
		case 120:{ index = 10;}break;
		case 180:{ index = 11;}break;
		case 240:{ index = 12;}break;
		case 480:{ index = 13;}break;
		default:{
			index = 4;
		}
	}
	
// 	if(B_PRINTF) printf("%s:%d TimeToIndex cycle is %d, Index is %d.\n", __FILE__, __LINE__, iMin, index);
	
	return index;
}


/*
	根据单位设置上下限
*/
static int SetLimitText(BOOL bUnit, HWND hWnd, int iLimit)
{
	unsigned char strLimit[50] = {0};
	unsigned char strUnit[20] = {0};
	
	switch(bUnit){
		case NIBP_UNIT_KPA:{
			memset(strUnit, 0, sizeof strUnit);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "unit_kpa", strUnit, sizeof strUnit,"kpa");		
			memset(strLimit, 0, sizeof strLimit);
			snprintf(strLimit, sizeof strLimit, "%3.1f %s", (float)(iLimit / 7.5), strUnit);
			SetWindowText(hWnd, strLimit);
		}break;
		default:{
			//NIBP_UNIT_MMHG
			memset(strUnit, 0, sizeof strUnit);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "unit_mmhg", strUnit, sizeof strUnit,"mmHg");		
			memset(strLimit, 0, sizeof strLimit);
			snprintf(strLimit, sizeof strLimit, "%d %s", iLimit, strUnit);
			SetWindowText(hWnd, strLimit);
		}break;
	}
	
	return 0;
}

//改变单位时,同时改变显示
static int ChangeLimitView(BYTE bUnit)
{
	
	SetLimitText(bUnit, editInitPres, iTmpInitPres);
	SetLimitText(bUnit, editVP, iTmpVP);
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
	//设置帮助栏背景框颜色和字体颜色
	SetWindowBkColor(hWnd,COLOR_black);
#ifdef FONTCOLOR	
	SetWindowElementColorEx( hWnd, FGC_CONTROL_NORMAL,COLOR_yellow);
#endif
	switch(message){
		case MSG_PAINT:{
			id = lParam;
			switch(id){
				case DID_NEWNIBP_UNIT:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBP, "help_nibp_unit", strHelp, sizeof strHelp,"Set up NIBP unit.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWNIBP_OBJECT:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBP, "help_nibp_object", strHelp, sizeof strHelp,"Set up measurement types.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWNIBP_MODE:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBP, "help_nibp_mode", strHelp, sizeof strHelp,"Set up measurement mode.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWNIBP_INTERVAL:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBP, "help_nibp_interval", strHelp, sizeof strHelp,"Set up interval with AUTO measurement.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWNIBP_INITPRES:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBP, "help_initpressure", strHelp, sizeof strHelp,"Initialize inflate(mmHg).");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWNIBP_VP:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBP, "help_VP", strHelp, sizeof strHelp,"NIBP Venipuncture setup.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWNIBP_STARTVEN:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBP, "help_startven", strHelp, sizeof strHelp,"NIBP Venipuncture start.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWNIBP_RESTE:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBP, "help_nibp_reset", strHelp, sizeof strHelp,"Reset NIBP pun.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWNIBP_ALARM:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBP, "help_nibp_set_alarm", strHelp, sizeof strHelp,"Set NIBP Alarm.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NEWNIBP_OK:{
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
extern int giNibpTimeCount_Auto;	//计时器
static int CobProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	int index;
	int NibpPeriod;
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
 						case DID_NEWNIBP_UNIT_COB:{
							//更新限值
							if(gCfgNibp.bUnit != index){
								ChangeLimitView(index);
							}
							gCfgNibp.bUnit = index;
							SetIntValueToResFile(gFileSetup, "NIBPSetup", "unit",  gCfgNibp.bUnit, 1);
 							SetFocus(btnUnit);
 						}break;
						case DID_NEWNIBP_OBJECT_COB:{
							gCfgNibp.bObject = index;
							SetIntValueToResFile(gFileSetup, "NIBPSetup", "object",  gCfgNibp.bObject, 1);
							SetFocus(btnObject);
							NIBP_Object_Change=TRUE;
//							SetNibpObject();
							if(B_PRINTF) printf("***bStatus:%d******gCfgNibp.bObject=%d\n",gValueNibp.bMeasureType,gCfgNibp.bObject);

							//如果正在测量中改变了对象,停止测量
							if(gValueNibp.bStatus== NIBP_SYSTEM_RUNNING){
								if(B_PRINTF) printf("************\n");
								NIBP_Cancel();
							}
							//设置NIBP病人类型
							//SetiBioNIBPPatType();
							
							//初始化上下限
							InitLimit();
							//初始化combobox
							InitEditText(NIBP_Object_Change);
							
							//改变静脉穿刺时间
							if(gCfgNibp.bMode == NIBP_MODE_PUNCTURE){
								if(gCfgNibp.bObject == NIBP_OBJECT_BABY)
									DrawNibpCycleGraph(0, NIBP_PUNCTURE_TIME_BABY, 2);
								else
									DrawNibpCycleGraph(0, NIBP_PUNCTURE_TIME_ADULT, 2);
							}
							NIBP_Object_Change=FALSE;

						
						}break;
						case DID_NEWNIBP_MODE_COB:{
							if(gCfgNibp.bMode != index){
 									NIBP_Cancel();
							}
							gCfgNibp.bMode = index;
							SetIntValueToResFile(gFileSetup, "NIBPSetup", "mode",  gCfgNibp.bMode, 1);
							//更改按键状态
							InitCtlStatus();
							SetFocus(btnMode);
							//周期设置为手动模式，自动测量时间由软件控制
								
							switch(gCfgNibp.bMode){
								case NIBP_MODE_MANUAL:{
								//	SetIBioNibpPeriod(0);
								}break;
								case NIBP_MODE_AUTO:{
									DrawNibpCycleGraph(0, gCfgNibp.wInterval*60, 0);
									NibpPeriod=TransformTimeToIndex(gCfgNibp.wInterval)+1;
								//	SetIBioNibpPeriod(0);
								}break;
								case NIBP_MODE_SERIES:{
									DrawNibpCycleGraph(0, NIBP_STAT_TIME, 1);
								//	DrawNibpCycleGraph(0, gCfgNibp.wInterval*60, 1);

								}break;
								case NIBP_MODE_PUNCTURE:{
									if(gCfgNibp.bObject == NIBP_OBJECT_BABY)
										DrawNibpCycleGraph(0, NIBP_PUNCTURE_TIME_BABY, 2);
									else
										DrawNibpCycleGraph(0, NIBP_PUNCTURE_TIME_ADULT, 2);
								}break;
								default:{
									//清空计时区域
									DrawNibpCycleGraph(0, -1, 0);
								}break;
							}
						}break;
						case DID_NEWNIBP_INTERVAL_COB:{
							if(gCfgNibp.wInterval != TransformIndexToTime(index)){
 								gCfgNibp.wInterval = TransformIndexToTime(index);
							
 								NIBP_Cancel();
								//周期设置为手动模式，自动测量时间由软件控制
							//	SetIBioNibpPeriod(0);
								DrawNibpCycleGraph(0, gCfgNibp.wInterval*60, 0);
							}
  							SetIntValueToResFile(gFileSetup, "NIBPSetup", "interval",  gCfgNibp.wInterval, 3);
							SetFocus(btnInterval);
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
						case DID_NEWNIBP_INITPRES_EDIT:{
							if(iTmpInitPres <Limit_initPres_High){
									if(iTmpInitPres<80)
										iTmpInitPres+=10;
									else
										iTmpInitPres += PRES_LIMIT_INTER;
								SetLimitText(gCfgNibp.bUnit, hWnd, iTmpInitPres);			
							}
						}break;
						case DID_NEWNIBP_VP_EDIT:{
							if(iTmpVP <Limit_VP_High){
								iTmpVP += VP_LIMIT_INTER;
								SetLimitText(gCfgNibp.bUnit, hWnd, iTmpVP);			
							}
						}break;
					}
				}break;
				case SCANCODE_CURSORBLOCKUP:{
					switch(id){
						case DID_NEWNIBP_INITPRES_EDIT:{
							if(iTmpInitPres > Limit_initPres_Low){
								if(iTmpInitPres<80)
										iTmpInitPres-=10;
								else
									iTmpInitPres -= PRES_LIMIT_INTER;
								SetLimitText(gCfgNibp.bUnit, hWnd, iTmpInitPres);			
							}
						}break;
						case DID_NEWNIBP_VP_EDIT:{
							if(iTmpVP > Limit_VP_Low){
								iTmpVP -= VP_LIMIT_INTER;
								SetLimitText(gCfgNibp.bUnit, hWnd, iTmpVP);			
							}
						}break;
					}
				}break;
				case SCANCODE_ENTER:{
					switch(id){
						case DID_NEWNIBP_INITPRES_EDIT:{
							gCfgNibp.wInitPres = iTmpInitPres;
							SetIntValueToResFile(gFileSetup, "NIBPSetup", "init_pres",  gCfgNibp.wInitPres, 4);
							//初始化充气压力
						//	SetiBioNibpInitPres();
						
							Set_NIBP_Init_pressure(2-gCfgNibp.bObject);
							
							SetFocus(btnInitPres);
						}break;
						case DID_NEWNIBP_VP_EDIT:{
							gCfgNibp.wVP= iTmpVP;
							SetIntValueToResFile(gFileSetup, "NIBPSetup", "venipuncture",  gCfgNibp.wVP, 4);
							
							SetNibpVenipuncture();
							
							EnableWindow(btnStartVen, TRUE);
							SetFocus(btnVP);
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
//创建控件
static int CreateCtlProc(HWND hDlg)
{
	editHelp = GetDlgItem(hDlg, DID_NEWNIBP_HELP);
	editCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE  ,
					     DID_NEWNIBP_CAPTION, 0, 5, 250, 25, hDlg, 0);
	
	btnUnit = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_NEWNIBP_UNIT, 10, 30+29*0, 125, 26, hDlg, 0);
	btnObject = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_NEWNIBP_OBJECT, 10, 30+29*1, 125, 26, hDlg, 0);
	btnMode = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_NEWNIBP_MODE, 10, 30+29*2, 125, 26, hDlg, 0);
	btnInterval = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_NEWNIBP_INTERVAL, 10, 30+29*3, 125, 26, hDlg, 0);
	btnInitPres = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
 					DID_NEWNIBP_INITPRES, 10, 30+29*4, 125, 25, hDlg, 0);
	btnVP = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_NEWNIBP_VP, 10, 30+29*5, 125, 26, hDlg, 0);
	btnStartVen= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_NEWNIBP_STARTVEN, 10, 30+29*6, 125, 26, hDlg, 0);
	btnReset= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_NEWNIBP_RESTE, 10, 30+29*7, 125, 26, hDlg, 0);

	btnAlarm = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_NEWNIBP_ALARM, 10, 30+29*8, 125, 26, hDlg, 0);
	btnOk= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					    DID_NEWNIBP_OK, 10, 30+29*9, 125, 26, hDlg, 0);
			
	cobUnit = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_NEWNIBP_UNIT_COB, 140, 30+29*0, 90, 26, hDlg, 0);
	cobObject = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					       DID_NEWNIBP_OBJECT_COB, 140, 30+29*1, 90, 26, hDlg, 0);
	cobMode = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_NEWNIBP_MODE_COB, 140, 30+29*2, 90, 26, hDlg, 0);
	cobInterval = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_NEWNIBP_INTERVAL_COB, 140, 30+29*3, 90, 26, hDlg, 0);
	editInitPres = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
 					DID_NEWNIBP_INITPRES_EDIT, 140, 30+29*4, 90, 23, hDlg, 0);
 	editVP= CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
 					DID_NEWNIBP_VP_EDIT, 140, 30+29*5, 90, 23, hDlg, 0);
 		
	return 0;
}
/*
	绘制下一页图标函数
*/
static int DrawIconProc(HWND hDlg)
{
	
	CreateWindow("static",gstrNextPage,WS_CHILD|WS_VISIBLE,IDC_STATIC,
 		150, 33+29*8, 30, 16,hDlg,0);
	CreateWindow("static",gstrNextPage,WS_CHILD|WS_VISIBLE,IDC_STATIC,
 		150, 33+29*7, 30, 16,hDlg,0);
	
	return(0);
}
//设置控件字符
static int InitCtlName(HWND hDlg)
{
	char strMenu[100];

	//caption
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBP, "caption", strMenu, sizeof strMenu,"NIBP");	
	SetWindowText(editCaption,strMenu);
	
	//Unit
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBP, "unit", strMenu, sizeof strMenu,"Unit");	
	SetWindowText(btnUnit,strMenu);

	//Object
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBP, "object", strMenu, sizeof strMenu,"Object");	
	SetWindowText(btnObject,strMenu);

	//Mode
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBP, "mode", strMenu, sizeof strMenu,"Mode");	
	SetWindowText(btnMode,strMenu);
	
	//Interval
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBP, "interval", strMenu, sizeof strMenu,"Auto Time");	
	SetWindowText(btnInterval,strMenu);
	
	//Init pressure 初始化充气压
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBP, "init_pres", strMenu, sizeof strMenu,"Init Pressure");	
	SetWindowText(btnInitPres,strMenu);

	//VP 设置静脉穿刺压
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBP, "venipuncture", strMenu, sizeof strMenu,"Venipuncture");	
	SetWindowText(btnVP,strMenu);
	//StartVen开启静脉穿刺压
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBP, "startven", strMenu, sizeof strMenu,"Start Pun");	
	SetWindowText(btnStartVen,strMenu);
	
	//Reset
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBP, "reset", strMenu, sizeof strMenu,"Reset");	
	SetWindowText(btnReset,strMenu);
				
	//Set Alarm 
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBP, "set_alarm", strMenu, sizeof strMenu,"NIBP Alarm");	
	SetWindowText(btnAlarm,strMenu);
	
	//Exit
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "backtomain", strMenu, sizeof strMenu,"Back to main");	
	SetWindowText(btnOk,strMenu);
				
	return 0;
}
//初始化combobox
static int InitCobList(HWND hDlg)
{
	int i;
	char strMenu[100];
	//Unit
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "unit_mmhg", strMenu, sizeof strMenu,"mmHg");	
	SendMessage(cobUnit, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "unit_kpa", strMenu, sizeof strMenu,"kpa");	
	SendMessage(cobUnit, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobUnit, CB_SETCURSEL, gCfgNibp.bUnit, 0);
	//Object 测量对象
	 if(gbMonitorObject ==0){
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "adult", strMenu, sizeof strMenu,"Adult");	
			SendMessage(cobObject, CB_ADDSTRING, 0, (LPARAM)strMenu);
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "child", strMenu, sizeof strMenu,"Child");	
			SendMessage(cobObject, CB_ADDSTRING, 0, (LPARAM)strMenu);
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "baby", strMenu, sizeof strMenu,"Baby");	
			SendMessage(cobObject, CB_ADDSTRING, 0, (LPARAM)strMenu);
		 }
		 else{
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "big_animal", strMenu, sizeof strMenu,"Big amimal");	
			SendMessage(cobObject, CB_ADDSTRING, 0, (LPARAM)strMenu);
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "middle_animal", strMenu, sizeof strMenu,"Middle amimal");	
			SendMessage(cobObject, CB_ADDSTRING, 0, (LPARAM)strMenu);
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "small_animal", strMenu, sizeof strMenu,"Small amimal");	
			SendMessage(cobObject, CB_ADDSTRING, 0, (LPARAM)strMenu);
		 }
		// gCfgNibp.bObject = gCfgPatient.bObject;
		if(B_PRINTF) printf("%s %d nibp:%d\n",__FILE__,__LINE__,gCfgNibp.bObject);
		SendMessage(cobObject, CB_SETCURSEL, gCfgNibp.bObject, 0);
		//Mode测量模式
		memset(strMenu, 0, sizeof strMenu);
		GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBP, "mode_manual", strMenu, sizeof strMenu,"Manual");	
		SendMessage(cobMode, CB_ADDSTRING, 0, (LPARAM)strMenu);
		memset(strMenu, 0, sizeof strMenu);
		GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBP, "mode_auto", strMenu, sizeof strMenu,"Auto");	
		SendMessage(cobMode, CB_ADDSTRING, 0, (LPARAM)strMenu);
		memset(strMenu, 0, sizeof strMenu);
		GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NIBP, "mode_series", strMenu, sizeof strMenu,"Series");	
		SendMessage(cobMode, CB_ADDSTRING, 0, (LPARAM)strMenu);
// 		memset(strMenu, 0, sizeof strMenu);
// 		GetStringFromResFile(gsLanguageRes, "NIBP", "mode_puncture", strMenu, sizeof strMenu);	
// 		SendMessage(cobMode, CB_ADDSTRING, 0, (LPARAM)strMenu);
		SendMessage(cobMode, CB_SETCURSEL, gCfgNibp.bMode, 0);
		//周期间隔时间
		for(i=0; i<14; i++){
				SendMessage(cobInterval, CB_ADDSTRING, 0, (LPARAM)strCycleTime[i]);
			}
			SendMessage(cobInterval, CB_SETCURSEL, TransformTimeToIndex(gCfgNibp.wInterval), 0);
			
	return 0;
}
/*
*	设置报警上下限
*/
static int InitLimit()
{		
	//成人 :80~240mmHg 20~120mmHg 160mmHg 80mmHg
	//儿童:80~200mmHg 20~80mmHg  120mmHg 60mmHg
	//新生儿:60~120mmHg 20~50mmHg 70mmHg 40mmHg
	
	switch(gCfgNibp.bObject){
		case ADULT:{
			Limit_initPres_High=250;
			Limit_initPres_Low=100;
			Limit_initPres_Init=160;
			
			Limit_VP_High=120;
			Limit_VP_Low=20;
			Limit_VP_Init=80;
										
		}break;
		case CHILD:{
			Limit_initPres_High=140;
			Limit_initPres_Low=70;
			Limit_initPres_Init=120;
			
			Limit_VP_High=80;
			Limit_VP_Low=20;
			Limit_VP_Init=60;
										
		}break;
		case BABY:{
			Limit_initPres_High=140;
			Limit_initPres_Low=70;
			Limit_initPres_Init=70;

			Limit_VP_High=50;
			Limit_VP_Low=20;
			Limit_VP_Init=40;
										
		}break;

	}

	
	gCfgNibp.wInitPres=Limit_initPres_Init;
	gCfgNibp.wVP=Limit_VP_Init;
	SetIntValueToResFile(gFileSetup, "NIBPSetup", "init_pres",  gCfgNibp.wInitPres, 4);
	SetIntValueToResFile(gFileSetup, "NIBPSetup", "venipuncture",  gCfgNibp.wVP, 4);
									
	SetLimitText(gCfgNibp.bUnit, editInitPres, iTmpInitPres);
	SetLimitText(gCfgNibp.bUnit, editVP, iTmpVP);	
	
	SetNibpVenipuncture();	
	return 0;
}
//初始化编辑框
static int InitEditText(BOOL Init)
{
	//窗口初始化判断是否超出上下限
	if(gCfgNibp.wInitPres>Limit_initPres_High||gCfgNibp.wInitPres<Limit_initPres_Low)
		gCfgNibp.wInitPres=Limit_initPres_Init;
	
	if(gCfgNibp.wVP>Limit_VP_High||gCfgNibp.wVP<Limit_VP_Low)
		gCfgNibp.wVP=Limit_VP_Init;
	//如是改变病人类型,重置
	if(NIBP_Object_Change){
		gCfgNibp.wInitPres=Limit_initPres_Init;
		gCfgNibp.wVP=Limit_VP_Init;
	}
		
	iTmpInitPres = gCfgNibp.wInitPres;
	iTmpVP= gCfgNibp.wVP;
	
	SetIntValueToResFile(gFileSetup, "NIBPSetup", "init_pres",  gCfgNibp.wInitPres, 4);
	SetIntValueToResFile(gFileSetup, "NIBPSetup", "venipuncture",  gCfgNibp.wVP, 4);
										
	SetLimitText(gCfgNibp.bUnit, editInitPres, iTmpInitPres);
	SetLimitText(gCfgNibp.bUnit, editVP, iTmpVP);	
	return 0;
}

//设置控件回调函数
static int InitCtlCallBackProc(HWND hDlg)
{
	OldBtnProc = SetWindowCallbackProc(btnUnit, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnObject, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnMode, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnInterval, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnInitPres, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnVP, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnStartVen, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnReset, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnAlarm, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);
	
	OldCobProc = SetWindowCallbackProc(cobUnit, CobProc);
	OldCobProc = SetWindowCallbackProc(cobObject, CobProc);
	OldCobProc = SetWindowCallbackProc(cobMode, CobProc);
	OldCobProc = SetWindowCallbackProc(cobInterval, CobProc);
	OldEditProc = SetWindowCallbackProc(editInitPres, EditProc);
	OldEditProc = SetWindowCallbackProc(editVP, EditProc);
	OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp); 
			
	return 0;
}
//设置控件状态
static int InitCtlStatus()
{
if(gCfgNibp.bMode==NIBP_MODE_AUTO){
		EnableWindow(btnInterval, TRUE);
		EnableWindow(cobInterval, TRUE);
	}else{
		EnableWindow(btnInterval, FALSE);
		EnableWindow(cobInterval, FALSE);
	}
	/*
//测量中不可操作的参数 
	if(gValueNibp.bStatus!= NIBP_SYSTEM_IDLE){
		
		EnableWindow(btnObject, FALSE);
		EnableWindow(cobObject, FALSE);

		EnableWindow(btnMode, FALSE);
		EnableWindow(cobMode, FALSE);

		EnableWindow(btnInitPres, FALSE);
		EnableWindow(editInitPres, FALSE);
		
	
	}else{
		EnableWindow(btnObject, TRUE);
		EnableWindow(cobObject, TRUE);

		EnableWindow(btnMode, TRUE);
		EnableWindow(cobMode, TRUE);

		EnableWindow(btnInitPres, TRUE);
		EnableWindow(editInitPres, TRUE);
	
	}	
*/	
	
	return 0;
}
/*
	对话框回调函数
*/
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
			//初始化上下限
			InitLimit();
			//初始化combobox
			InitEditText(NIBP_Object_Change);
			//设置控件状态
			InitCtlStatus();
			//设置控件回调函数
			InitCtlCallBackProc(hDlg);
			
			//绘制下一页图标
			DrawIconProc(hDlg);
			gbKeyType = KEY_TAB;	
			SetFocus(btnOk);
		}break;
		case MSG_PAINT:{

		}break;
		case MSG_COMMAND:{
			int	id   = LOWORD(wParam);
			
			switch(id){
				case DID_NEWNIBP_UNIT:{
					gbKeyType = KEY_UD;
					SetFocus(cobUnit);
				}break;
				case DID_NEWNIBP_OBJECT:{
					gbKeyType = KEY_UD;
					SetFocus(cobObject);
				}break;
				case DID_NEWNIBP_MODE:{
					gbKeyType = KEY_UD;
					SetFocus(cobMode);
				}break;
				case DID_NEWNIBP_INTERVAL:{
					gbKeyType = KEY_UD;
					SetFocus(cobInterval);
				}break;
				case DID_NEWNIBP_INITPRES:{
					gbKeyType = KEY_UD;
					SetFocus(editInitPres);
				}break;
				case DID_NEWNIBP_VP:{
					gbKeyType = KEY_UD;
					SetFocus(editVP);
				}break;
				case DID_NEWNIBP_STARTVEN:{
					if(gValueNibp.bStatus==NIBP_SYSTEM_IDLE)
						StartVenipuncture();
					else
						StopNibpMeasure();
						
					
				}break;
				
				case DID_NEWNIBP_RESTE:{

				//	CreateMaintNIBP(hDlg);
				
					Nibp_Reset();
					gCfgNibp.bObject=NIBP_OBJECT_ADULT;
					gCfgNibp.bMode=NIBP_MODE_MANUAL;
					SendMessage(cobObject, CB_SETCURSEL, gCfgNibp.bObject, 0);	
					SendMessage(cobMode, CB_SETCURSEL, gCfgNibp.bMode, 0);
					
					SetIntValueToResFile(gFileSetup, "NIBPSetup", "object",  gCfgNibp.bObject, 1);
					SetIntValueToResFile(gFileSetup, "NIBPSetup", "mode",  gCfgNibp.bMode, 1);
					
					//初始化上下限
					InitLimit();
					//初始化combobox
					InitEditText(NIBP_Object_Change);
					EnableWindow(btnInterval, FALSE);
					EnableWindow(cobInterval, FALSE);
					
				}break;
				case DID_NEWNIBP_ALARM:{
					//CreateDlgNibpSet(hDlg);
					CreateDlgNewNIBPALM(hDlg);
					RestorWave();
				}break;
				case DID_NEWNIBP_OK:{
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
		//	ChangeWavePanelsLeft((PRECT)NULL);
			
			//向中央机发送信息
			NetSend_NibpInfo();

			EndDialog(hDlg, wParam);	
		}break;
	}
	
	temp = DefaultDialogProc(hDlg, message, wParam, lParam);
	return(temp);
}

/*
	建立对话框
*/
void CreateDlgNewNibp(HWND hWnd)
{
 	DlgSet.controls = CtrlSet;
 	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}

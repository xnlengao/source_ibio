/*************************************************************************** 
 *	Module Name:	DlgSet_Patient
 *
 *	Abstract:	设置Patient信息的主对话框
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2007-06-15 10:06:51	
 ***************************************************************************/
 
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"

//DID_PATIENT
#define DID_PATIENT_HELP					DID_PATIENT
#define DID_PATIENT_OK 						DID_PATIENT+1
#define DID_PATIENT_CAPTION					DID_PATIENT+2

#define DID_PATIENT_ROOM					DID_PATIENT+3
#define DID_PATIENT_ID						DID_PATIENT+4
#define DID_PATIENT_DOCTOR					DID_PATIENT+5
#define DID_PATIENT_NAME					DID_PATIENT+6
#define DID_PATIENT_ADMIT					DID_PATIENT+7
#define DID_PATIENT_BIRTH					DID_PATIENT+8
#define DID_PATIENT_SEX						DID_PATIENT+9
#define DID_PATIENT_OBJECT					DID_PATIENT+10
#define DID_PATIENT_HEIGHT					DID_PATIENT+11
#define DID_PATIENT_WEIGHT					DID_PATIENT+12
#define DID_PATIENT_BLOOD					DID_PATIENT+13
#define DID_PATIENT_PACEMAKER				DID_PATIENT+14
#define DID_PATIENT_UPDATE					DID_PATIENT+15
#define DID_PATIENT_AGE						DID_PATIENT+16

#define DID_PATIENT_ROOM_EDIT				DID_PATIENT+17
#define DID_PATIENT_ID_EDIT					DID_PATIENT+18
#define DID_PATIENT_DOCTOR_EDIT			DID_PATIENT+19
#define DID_PATIENT_NAME_EDIT				DID_PATIENT+20
#define DID_PATIENT_ADMIT_EDIT				DID_PATIENT+21
#define DID_PATIENT_BIRTH_EDIT				DID_PATIENT+22
#define DID_PATIENT_SEX_COB				DID_PATIENT+23
#define DID_PATIENT_OBJECT_COB				DID_PATIENT+24
#define DID_PATIENT_HEIGHT_EDIT				DID_PATIENT+25
#define DID_PATIENT_WEIGHT_EDIT				DID_PATIENT+26
#define DID_PATIENT_BLOOD_COB				DID_PATIENT+27
#define DID_PATIENT_PACEMAKER_COB		DID_PATIENT+28
#define DID_PATIENT_AGE_EDIT				DID_PATIENT+29



//帮助对话框
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;
//其他控件

static HWND btnOk;
static HWND btnSetRoom;
static HWND btnSetID;
static HWND btnSetDoctor;
static HWND btnSetName;
static HWND btnSetAdmit;
static HWND btnSetBirth;
static HWND btnSetSex;
static HWND btnSetObject;
static HWND btnSetWeight;
static HWND btnSetHeight;
static HWND btnSetBlood;
static HWND btnSetPacemaker;
static HWND btnUpdate;
static HWND btnSetAge;

static HWND editRoom;
static HWND editID;
static HWND editDoctor;
static HWND editName;
static HWND editAdmit;
static HWND editBirth;
static HWND cobSex;
static HWND cobObject;
static HWND editWeight;
static HWND editHeight;
static HWND cobBlood;
static HWND cobPacemaker;
static HWND editAge;


//控件回调函数
static WNDPROC OldBtnProc;	
static WNDPROC OldCobProc;
static WNDPROC OldEditProc;

//对话框属性
static DLGTEMPLATE DlgSet= {
	WS_VISIBLE | WS_BORDER,
	WS_EX_NOCLOSEBOX,	
	DLG_X, DLG_Y, DLG_LENGHT+40, DLG_HIGH,	
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
		5,DLG_HIGH-50, DLG_LENGHT+40-15, 40,
		DID_PATIENT_HELP,
		"",
		0
	}
};

//临时存储上下限，调节的时候使用
static int iTmp_Age =0;
static int iTmp_Height =0;
static int iTmp_Weight =0;

//上下限刻度调整的增量
#define AGE_LIMIT_INTER		1
#define HEIGHT_LIMIT_INTER	1
#define WEIGHT_LIMIT_INTER	1
	
//软键盘离输入框多长距离
#define  KEY_XLENGTH_LEFT		70
#define KEY_XLENGTH_RIGHT		170
//限值
#define AGE_LIMIT_MAX		AGE_MAX
#define AGE_LIMIT_MIN		AGE_MIN
#define HEIGHT_LIMIT_MAX		HEIGHT_MAX
#define HEIGHT_LIMIT_MIN		HEIGHT_MIN
#define WEIGHT_LIMIT_MAX	WEIGHT_MAX
#define WEIGHT_LIMIT_MIN		WEIGHT_MIN


//调节限值用到的变量
static int iCurLimitValue = 0;
static unsigned char sLimitValue[15]={0};

extern BOOL gbUpdatePatientInfo;
extern BOOL NIBP_Object_Change;
/*
	设置上下限文字
	bType: 0-Age, 1-Height, 2-Weight
*/
static int SetLimitText(HWND hWnd, int iLimit, BYTE bType)
{
	unsigned char strLimit[30] = {0};
	unsigned char strUnit[10] = {0};
		
	memset(strUnit, 0, sizeof strUnit);
	switch(bType){
		case 1:{
			GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "unit_cm", strUnit, sizeof strUnit);				
		}break;
		case 2:{
			GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "unit_kg", strUnit, sizeof strUnit);				
		}break;
	}
	memset(strLimit, 0, sizeof strLimit);
	snprintf(strLimit, sizeof strLimit, "%d %s", iLimit, strUnit);
	SetWindowText(hWnd, strLimit);
	
	return 0;
}
int Set_Patient_Object()
{
	int index;
	

	index = SendMessage(cobObject, CB_GETCURSEL, 0, 0);
	gCfgPatient.bObject = index;
	SetIntValueToResFile(gFileSetup, "PatientSetup", "object",  gCfgPatient.bObject, 1);

	
	//更新一些测量参数的信息，如connected为FALSE等
	gValueEcg.bConnected = FALSE;
	gValueSpO2.bConnected = FALSE;
	gValueTemp.bConnected_T1 = FALSE;
	gValueTemp.bConnected_T2 = FALSE;
		
	if(B_PRINTF) printf("Patient  Object: %d.\n ", gCfgPatient.bObject);
	//SetiBioPatientType();
	Set_Patient_info();
	
	//加载默认设置
	LoadSetup(gCfgPatient.bObject);
	
	//改变NIBP测量对象
	gCfgNibp.bObject = gCfgPatient.bObject;
	SetIntValueToResFile(gFileSetup, "NIBPSetup", "object",  gCfgNibp.bObject, 1);
	
	if(B_PRINTF) printf("-------------------- Object: %d.\n ", gCfgNibp.bObject);
	
	//如果正在测量中改变了对象,停止测量
	if(gValueNibp.bStatus == NIBP_SYSTEM_RUNNING){
		NIBP_Cancel();
	}
	
	return 0;
}
int Set_Patient_Room()
{
	//Room
	SendMessage(editRoom, MSG_GETTEXT, sizeof sLimitValue, (LPARAM)sLimitValue);
	memset(gCfgPatient.sRoom, 0, sizeof(gCfgPatient.sRoom));
	snprintf(gCfgPatient.sRoom, sizeof(gCfgPatient.sRoom),"%s" ,sLimitValue);
	SetValueToEtcFile(gFileSetup, "PatientSetup", "room",  gCfgPatient.sRoom);
	return 0;
}

int Set_Patient_ID()
{
	SendMessage(editID, MSG_GETTEXT, sizeof sLimitValue, (LPARAM)sLimitValue);
	memset(gCfgPatient.sID, 0, sizeof(gCfgPatient.sID));
	snprintf(gCfgPatient.sID, sizeof(gCfgPatient.sID),"%s" ,sLimitValue);
	SetValueToEtcFile(gFileSetup, "PatientSetup", "id",  gCfgPatient.sID);
	return 0;
}

int Set_Patient_Doctor()
{
	SendMessage(editDoctor, MSG_GETTEXT, sizeof sLimitValue, (LPARAM)sLimitValue);
	memset(gCfgPatient.sDoctor, 0, sizeof(gCfgPatient.sDoctor));
	snprintf(gCfgPatient.sDoctor, sizeof(gCfgPatient.sDoctor), "%s",sLimitValue);
	SetValueToEtcFile(gFileSetup, "PatientSetup", "doctor",  gCfgPatient.sDoctor);
	return 0;
}

int Set_Patient_Name()
{
	SendMessage(editName, MSG_GETTEXT, sizeof sLimitValue, (LPARAM)sLimitValue);
	memset(gCfgPatient.sName, 0, sizeof(gCfgPatient.sName));
	snprintf(gCfgPatient.sName, sizeof(gCfgPatient.sName), "%s",sLimitValue);
	SetValueToEtcFile(gFileSetup, "PatientSetup", "name",  gCfgPatient.sName);
	return 0;
}

int Set_Patient_Admit()
{
	SendMessage(editAdmit, MSG_GETTEXT, sizeof sLimitValue, (LPARAM)sLimitValue);
	memset(gCfgPatient.sAdmit, 0, sizeof(gCfgPatient.sAdmit));
	snprintf(gCfgPatient.sAdmit, sizeof(gCfgPatient.sAdmit), "%s",sLimitValue);
	SetValueToEtcFile(gFileSetup, "PatientSetup", "admit",  gCfgPatient.sAdmit);
	return 0;
}

int Set_Patient_Birth()
{
	SendMessage(editBirth, MSG_GETTEXT, sizeof sLimitValue, (LPARAM)sLimitValue);
	memset(gCfgPatient.sBirth, 0, sizeof(gCfgPatient.sBirth));
	snprintf(gCfgPatient.sBirth, sizeof(gCfgPatient.sBirth), "%s",sLimitValue);
	SetValueToEtcFile(gFileSetup, "PatientSetup", "birth",  gCfgPatient.sBirth);
	
	return 0;
}

int Set_Patient_Height()
{
	SendMessage(editHeight, MSG_GETTEXT, sizeof sLimitValue, (LPARAM)sLimitValue);
	iCurLimitValue = atoi(sLimitValue);
	gCfgPatient.wHeight = iCurLimitValue;
	SetIntValueToResFile(gFileSetup, "PatientSetup", "height",  gCfgPatient.wHeight, 3);
	return 0;
}

int Set_Patient_Weight()
{
	SendMessage(editWeight, MSG_GETTEXT, sizeof sLimitValue, (LPARAM)sLimitValue);
	iCurLimitValue = atoi(sLimitValue);
	gCfgPatient.wWeight = iCurLimitValue;
	SetIntValueToResFile(gFileSetup, "PatientSetup", "weight",  gCfgPatient.wWeight, 3);
	return 0;
}

int Set_Patient_Sex()
{
	int index;
	index = SendMessage(cobSex, CB_GETCURSEL, 0, 0);
	gCfgPatient.bSex = index;
	SetIntValueToResFile(gFileSetup, "PatientSetup", "sex",  gCfgPatient.bSex, 1);
	return 0;
}

int Set_Patient_Blood()
{
	int index;
	index = SendMessage(cobBlood, CB_GETCURSEL, 0, 0);
	gCfgPatient.bBlood= index;
	SetIntValueToResFile(gFileSetup, "PatientSetup", "blood",  gCfgPatient.bBlood, 1);
	
	return 0;
}
int Set_Patient_pacemaker()
{
	int index;
	index = SendMessage(cobPacemaker, CB_GETCURSEL, 0, 0);
	gCfgPatient.bPacemaker = index;
	SetIntValueToResFile(gFileSetup, "PatientSetup", "pacemaker",  gCfgPatient.bPacemaker, 1);
	return 0;
}

/*
	更新病人信息
*/
//报警数据存储
extern CA_ALMDATAARRAY	gAlmDataArray;
static int UpdatePatientInfo()
{
	int index;
	
	//Object
	Set_Patient_Object();
	
	//Room
	Set_Patient_Room();
	
	//ID
	Set_Patient_ID();
 
	//doctor
	Set_Patient_Doctor();
	 
	//Name
	Set_Patient_Name();
	 
	//Admit
	Set_Patient_Admit();
	 
	//birth
	Set_Patient_Birth();
	
	//Age
	SendMessage(editAge, MSG_GETTEXT, sizeof sLimitValue, (LPARAM)sLimitValue);
	iCurLimitValue = atoi(sLimitValue);
	gCfgPatient.bAge = iCurLimitValue;
	SetIntValueToResFile(gFileSetup, "PatientSetup", "age",  gCfgPatient.bAge, 3);
	
	//Height
	Set_Patient_Height();
	
	//Weight
	Set_Patient_Weight();
	
	//Sex
	Set_Patient_Sex();
	
	//blood
	Set_Patient_Blood();
	
	//Pacemaker
	Set_Patient_pacemaker();
	
	
	
	gbUpdatePatientInfo = TRUE;
	
	//清空报警存储数据 16/08/2011 14:57:18
	memset(gAlmDataArray.Datas, 0, sizeof gAlmDataArray.Datas);
	gAlmDataArray.wHeadIndex = 0;
	gAlmDataArray.wTailIndex = 0;
	gAlmDataArray.wCount = 0;
	gAlmDataArray.bLoop = FALSE;
	
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
				case DID_PATIENT_ID:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_PATIENT, "help_patient_id", strHelp, sizeof strHelp,"Set up patient's ID.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_PATIENT_NAME:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_PATIENT, "help_patient_name", strHelp, sizeof strHelp,"Set up patient's name.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_PATIENT_SEX:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_PATIENT, "help_patient_sex", strHelp, sizeof strHelp,"Set up patient's sex.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_PATIENT_OBJECT:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_PATIENT, "help_patient_object", strHelp, sizeof strHelp,"This operation will change some relative set, eg. Default alarm limit.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_PATIENT_PACEMAKER:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_PATIENT, "help_patient_pacemaker", strHelp, sizeof strHelp,"Set up patient on the type of pacemaker or not.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_PATIENT_AGE:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_PATIENT, "help_patient_age", strHelp, sizeof strHelp,"Set up patient's age.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_PATIENT_HEIGHT:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_PATIENT, "help_patient_height", strHelp, sizeof strHelp,"Set up patient's height.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_PATIENT_WEIGHT:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_PATIENT, "help_patient_weight", strHelp, sizeof strHelp,"Set up patient's weight.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_PATIENT_UPDATE:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_PATIENT, "help_patient_update", strHelp, sizeof strHelp,"This operation will delete measurement data before.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_PATIENT_OK:{
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
 						case DID_PATIENT_SEX_COB:{
							gbKeyType = KEY_TAB;
 							SetFocus(btnSetSex);
 						}break;
						case DID_PATIENT_OBJECT_COB:{
							gbKeyType = KEY_TAB;
							SetFocus(btnSetObject);
						}break;
						case DID_PATIENT_BLOOD_COB:{
							gbKeyType = KEY_TAB;
							SetFocus(btnSetBlood);
						}break;
						case DID_PATIENT_PACEMAKER:{
							gbKeyType = KEY_TAB;
							SetFocus(btnSetPacemaker);
						}break;
					}	
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
						case DID_PATIENT_AGE_EDIT:{
							if(iTmp_Age  < AGE_LIMIT_MAX){
								iTmp_Age += AGE_LIMIT_INTER;
								SetLimitText(hWnd, iTmp_Age, 0);			
							}
						}break;
						case DID_PATIENT_HEIGHT_EDIT:{
							if(iTmp_Height < HEIGHT_LIMIT_MAX){
								iTmp_Height += HEIGHT_LIMIT_INTER;
								SetLimitText(hWnd, iTmp_Height, 1);			
							}
						}break;
						case DID_PATIENT_WEIGHT_EDIT:{
							if(iTmp_Weight < WEIGHT_LIMIT_MAX){
								iTmp_Weight += WEIGHT_LIMIT_INTER;
								SetLimitText(hWnd, iTmp_Weight, 2);			
							}
						}break;
					}
				}break;
				case SCANCODE_CURSORBLOCKUP:{
					switch(id){
						case DID_PATIENT_AGE_EDIT:{
							if(iTmp_Age > AGE_LIMIT_MIN){
								iTmp_Age -= AGE_LIMIT_INTER;
								SetLimitText(hWnd, iTmp_Age, 0);			
							}
						}break;
						case DID_PATIENT_HEIGHT_EDIT:{
							if(iTmp_Height > HEIGHT_LIMIT_MIN){
								iTmp_Height -= HEIGHT_LIMIT_INTER;
								SetLimitText(hWnd, iTmp_Height, 1);			
							}
						}break;
						case DID_PATIENT_WEIGHT_EDIT:{
							if(iTmp_Weight > WEIGHT_LIMIT_MIN){
								iTmp_Weight -= WEIGHT_LIMIT_INTER;
								SetLimitText(hWnd, iTmp_Weight, 2);			
							}
						}break;
					}
				}break;
				case SCANCODE_ENTER:{
					switch(id){
						case DID_PATIENT_AGE_EDIT:{
							SetFocus(btnSetAge);
						}break;
						case DID_PATIENT_HEIGHT_EDIT:{
							SetFocus(btnSetHeight);
						}break;
						case DID_PATIENT_WEIGHT_EDIT:{
							SetFocus(btnSetWeight);
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
	editHelp = GetDlgItem(hDlg, DID_PATIENT_HELP);
	editCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE, 
					     DID_PATIENT_CAPTION, 0, 5, DLG_LENGHT+40, 25, hDlg, 0);
	
	btnSetRoom = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_PATIENT_ROOM, 5, 30+30*0, 60, 26, hDlg, 0);
	btnSetID = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_PATIENT_ID, 145, 30+30*0, 60, 26, hDlg, 0);
	btnSetDoctor = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_PATIENT_DOCTOR, 5, 30+30*1, 60, 26, hDlg, 0);
	btnSetName = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_PATIENT_NAME, 145, 30+30*1, 60, 26, hDlg, 0);
	btnSetAdmit = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_PATIENT_ADMIT, 5, 30+30*2, 60, 26, hDlg, 0);
	btnSetBirth = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_PATIENT_BIRTH, 145, 30+30*2, 60, 26, hDlg, 0);
//	btnSetAge = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
//					DID_PATIENT_AGE, 5, 30+30*2, 125, 26, hDlg, 0);
	btnSetHeight = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_PATIENT_HEIGHT, 5, 30+30*3, 60, 26, hDlg, 0);
	btnSetWeight = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_PATIENT_WEIGHT, 145, 30+30*3, 60, 26, hDlg, 0);
	btnSetSex = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_PATIENT_SEX, 5, 30+30*4, 60, 26, hDlg, 0);
	btnSetObject = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_PATIENT_OBJECT, 145, 30+30*4, 60, 26, hDlg, 0);
	btnSetBlood= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_PATIENT_BLOOD, 5, 30+30*5, 60, 26, hDlg, 0);
	btnSetPacemaker = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_PATIENT_PACEMAKER, 145, 30+30*5, 60, 26, hDlg, 0);
	btnUpdate = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					DID_PATIENT_UPDATE, 5, 30+30*6, 135, 26, hDlg, 0);
	btnOk= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					    DID_PATIENT_OK, 5, 30+30*7, 135, 26, hDlg, 0);

	
	editRoom= CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_PATIENT_ROOM_EDIT, 70, 30+30*0, 70, 23, hDlg, 0);
	editID = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_PATIENT_ID_EDIT, 210, 30+30*0, 70, 23, hDlg, 0);
	editDoctor= CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_PATIENT_DOCTOR_EDIT, 70, 30+30*1, 70, 23, hDlg, 0);
	editName = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_PATIENT_NAME_EDIT, 210, 30+30*1, 70, 23, hDlg, 0);
//	editAge = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
//					DID_PATIENT_AGE_EDIT, 140, 30+30*2, 95, 23, hDlg, 0);
	editAdmit= CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_PATIENT_ADMIT_EDIT, 70, 30+30*2, 70, 24, hDlg, 0);
	editBirth= CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_PATIENT_BIRTH_EDIT, 210, 30+30*2, 70, 24, hDlg, 0);
	editHeight = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_PATIENT_HEIGHT_EDIT, 70, 30+30*3, 70, 24, hDlg, 0);
	editWeight = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER,
					DID_PATIENT_WEIGHT_EDIT, 210, 30+30*3, 70, 24, hDlg, 0);
	cobSex = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_PATIENT_SEX_COB, 70, 30+30*4, 70, 26, hDlg, 0);
	cobObject = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_PATIENT_OBJECT_COB, 210, 30+30*4, 70, 26, hDlg, 0);
	cobBlood = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_PATIENT_BLOOD_COB, 70, 30+30*5, 70, 26, hDlg, 0);
	cobPacemaker = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_PATIENT_PACEMAKER, 210, 30+30*5, 70, 26, hDlg, 0);
	
	return 0;
}

static int InitCtlName(HWND hDlg)
{
	char strMenu[100];

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_PATIENT, "caption", strMenu, sizeof strMenu,"Patient Setup");
	SetWindowText(editCaption, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_PATIENT, "room", strMenu, sizeof strMenu,"Room");	
	SetWindowText(btnSetRoom, strMenu);	
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_PATIENT, "id", strMenu, sizeof strMenu,"Case NO.");	
	SetWindowText(btnSetID, strMenu);	
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_PATIENT, "doctor", strMenu, sizeof strMenu,"Doctor");	
	SetWindowText(btnSetDoctor, strMenu);	
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_PATIENT, "name", strMenu, sizeof strMenu,"Name");	
	SetWindowText(btnSetName, strMenu);	
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_PATIENT, "admit", strMenu, sizeof strMenu,"Admit");	
	SetWindowText(btnSetAdmit, strMenu);	
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_PATIENT, "birth", strMenu, sizeof strMenu,"Birth");	
	SetWindowText(btnSetBirth, strMenu);	
//	memset(strMenu, 0, sizeof strMenu);
//	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_PATIENT, "age", strMenu, sizeof strMenu,"Age");	
//	SetWindowText(btnSetAge, strMenu);	
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_PATIENT, "height", strMenu, sizeof strMenu,"Height");	
	SetWindowText(btnSetHeight, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_PATIENT, "weight", strMenu, sizeof strMenu,"Weight");	
	SetWindowText(btnSetWeight, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_PATIENT, "sex", strMenu, sizeof strMenu,"Sex");	
	SetWindowText(btnSetSex, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_PATIENT, "object", strMenu, sizeof strMenu,"Object");	
	SetWindowText(btnSetObject, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_PATIENT, "blood", strMenu, sizeof strMenu,"Blood");	
	SetWindowText(btnSetBlood, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_PATIENT, "pacemaker", strMenu, sizeof strMenu,"Pacemaker");	
	SetWindowText(btnSetPacemaker, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_PATIENT, "update", strMenu, sizeof strMenu,"Update");	
	SetWindowText(btnUpdate, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, STR_SETTING_GENERAL, "previous", strMenu, sizeof strMenu,"Previous");	
	SetWindowText(btnOk, strMenu);

	SetWindowText(editRoom, gCfgPatient.sRoom);
	SetWindowText(editID, gCfgPatient.sID);
	SetWindowText(editDoctor, gCfgPatient.sDoctor);
	SetWindowText(editName, gCfgPatient.sName);
	SetWindowText(editAdmit, gCfgPatient.sAdmit);
	SetWindowText(editBirth, gCfgPatient.sBirth);
	SetLimitText(editAge, gCfgPatient.bAge, 0);
	SetLimitText(editHeight, gCfgPatient.wHeight, 1);
	SetLimitText(editWeight, gCfgPatient.wWeight, 2);
	return 0;
}
static int InitCobList(HWND hDlg)
{
	char strMenu[100];
		int i;
	BYTE *strBlood[5]={"A","B","O","AB","N/A"};
	
	iTmp_Age = gCfgPatient.bAge;
	iTmp_Height = gCfgPatient.wHeight;
	iTmp_Weight = gCfgPatient.wWeight;
	
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "male", strMenu, sizeof strMenu,"male");	
	SendMessage(cobSex, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "female", strMenu, sizeof strMenu,"female");	
	SendMessage(cobSex, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobSex, CB_SETCURSEL, gCfgPatient.bSex, 0);
	
	if(gbMonitorObject ==0){
		memset(strMenu, 0, sizeof strMenu);
		GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "adult", strMenu, sizeof strMenu,"adult");	
		SendMessage(cobObject, CB_ADDSTRING, 0, (LPARAM)strMenu);
		memset(strMenu, 0, sizeof strMenu);
		GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "child", strMenu, sizeof strMenu,"child");	
		SendMessage(cobObject, CB_ADDSTRING, 0, (LPARAM)strMenu);
		memset(strMenu, 0, sizeof strMenu);
		GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "baby", strMenu, sizeof strMenu,"baby");	
		SendMessage(cobObject, CB_ADDSTRING, 0, (LPARAM)strMenu);
	}
	else{
		memset(strMenu, 0, sizeof strMenu);
		GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "big_animal", strMenu, sizeof strMenu,"big_animal");	
		SendMessage(cobObject, CB_ADDSTRING, 0, (LPARAM)strMenu);
		memset(strMenu, 0, sizeof strMenu);
		GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "middle_animal", strMenu, sizeof strMenu,"middle_animal");	
		SendMessage(cobObject, CB_ADDSTRING, 0, (LPARAM)strMenu);
		memset(strMenu, 0, sizeof strMenu);
		GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "small_animal", strMenu, sizeof strMenu,"small_animal");	
		SendMessage(cobObject, CB_ADDSTRING, 0, (LPARAM)strMenu);
	}
	SendMessage(cobObject, CB_SETCURSEL, gCfgPatient.bObject, 0);

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "no", strMenu, sizeof strMenu,"NO");	
	SendMessage(cobPacemaker, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "yes", strMenu, sizeof strMenu,"YES");	
	SendMessage(cobPacemaker, CB_ADDSTRING, 0, (LPARAM)strMenu);
	SendMessage(cobPacemaker, CB_SETCURSEL, gCfgPatient.bPacemaker, 0);

	for(i=0;i<5;i++){
		SendMessage(cobBlood, CB_ADDSTRING, 0, (LPARAM)strBlood[i]);
	}
	SendMessage(cobBlood, CB_SETCURSEL, gCfgPatient.bBlood, 0);
	return 0;
}

//设置控件回调函数
static int InitCtlCallBackProc(HWND hDlg)
{
	OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp); 
	
	OldBtnProc = SetWindowCallbackProc(btnSetRoom, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSetID, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSetDoctor, BtnProc);	
	OldBtnProc = SetWindowCallbackProc(btnSetName, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSetAdmit, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSetBirth, BtnProc);
//	OldBtnProc = SetWindowCallbackProc(btnSetAge, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSetHeight, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSetWeight, BtnProc);	
	OldBtnProc = SetWindowCallbackProc(btnSetSex, BtnProc);	
	OldBtnProc = SetWindowCallbackProc(btnSetObject, BtnProc);	
	OldBtnProc = SetWindowCallbackProc(btnSetBlood, BtnProc);	
	OldBtnProc = SetWindowCallbackProc(btnSetPacemaker, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnUpdate, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);

	OldEditProc = SetWindowCallbackProc(editRoom, EditProc);	
	OldEditProc = SetWindowCallbackProc(editID, EditProc);
	OldEditProc = SetWindowCallbackProc(editDoctor, EditProc);
	OldEditProc = SetWindowCallbackProc(editName, EditProc);
	OldEditProc = SetWindowCallbackProc(editAdmit, EditProc);
	OldEditProc = SetWindowCallbackProc(editBirth, EditProc);
//	OldEditProc = SetWindowCallbackProc(editAge, EditProc);
	OldEditProc = SetWindowCallbackProc(editHeight, EditProc);
	OldEditProc = SetWindowCallbackProc(editWeight, EditProc);
	OldCobProc = SetWindowCallbackProc(cobSex, CobProc);
	OldCobProc = SetWindowCallbackProc(cobObject, CobProc);
	OldCobProc = SetWindowCallbackProc(cobBlood, CobProc);
	OldCobProc = SetWindowCallbackProc(cobPacemaker, CobProc);
	
	
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
			//初始化列表框
			InitCobList(hDlg);
			//设置控件回调函数
			InitCtlCallBackProc(hDlg);
			
			SetFocus(btnOk);
						
		}break;
		case MSG_PAINT:{

		}break;
		case MSG_COMMAND:{
			int	id  = LOWORD(wParam);
			unsigned char strinput[50]={};
			switch(id){
				case DID_PATIENT_ROOM:{
					ghWnd_Input = editRoom;
					GetWindowRect(editRoom, &gRC_Input);
					GetWindowText(editRoom,strinput,sizeof(strinput));
					
					KeyInputs(hDlg, gRC_Input,strinput,KEY_XLENGTH_LEFT);
				}break;
				case DID_PATIENT_ID:{
					ghWnd_Input = editID;
					GetWindowRect(editID, &gRC_Input);
					GetWindowText(editID,strinput,sizeof(strinput));
					printf("%s\n",strinput);
					KeyInputs(hDlg, gRC_Input,strinput,KEY_XLENGTH_RIGHT);
				}break;
				case DID_PATIENT_DOCTOR:{
					ghWnd_Input = editDoctor;
					GetWindowRect(editDoctor, &gRC_Input);
					GetWindowText(editDoctor,strinput,sizeof(strinput));
					KeyInputs(hDlg, gRC_Input,strinput,KEY_XLENGTH_LEFT);
				}break;
				case DID_PATIENT_NAME:{
					ghWnd_Input = editName;
					GetWindowRect(editName, &gRC_Input);
					GetWindowText(editName,strinput,sizeof(strinput));
					KeyInputs(hDlg, gRC_Input,strinput,KEY_XLENGTH_RIGHT);
				}break;
				case DID_PATIENT_ADMIT:{
					ghWnd_Input = editAdmit;
					GetWindowRect(editAdmit, &gRC_Input);
					GetWindowText(editAdmit,strinput,sizeof(strinput));
					KeyInputs(hDlg, gRC_Input,strinput,KEY_XLENGTH_LEFT);
				}break;
				case DID_PATIENT_BIRTH:{
					ghWnd_Input = editBirth;
					GetWindowRect(editBirth, &gRC_Input);
					GetWindowText(editBirth,strinput,sizeof(strinput));
					KeyInputs(hDlg, gRC_Input,strinput,KEY_XLENGTH_RIGHT);
				}break;
				case DID_PATIENT_SEX:{
					gbKeyType = KEY_UD;
					SetFocus(cobSex);
				}break;
				case DID_PATIENT_OBJECT:{
					gbKeyType = KEY_UD;
					SetFocus(cobObject);
				}break;
				case DID_PATIENT_BLOOD:{
					gbKeyType = KEY_UD;
					SetFocus(cobBlood);	
				}break;
				case DID_PATIENT_PACEMAKER:{
					gbKeyType = KEY_UD;
					SetFocus(cobPacemaker);
				}break;
				case DID_PATIENT_AGE:{
					gbKeyType = KEY_UD;
					SetFocus(editAge);
				}break;
				case DID_PATIENT_HEIGHT:{
					gbKeyType = KEY_UD;
					SetFocus(editHeight);
				}break;
				case DID_PATIENT_WEIGHT:{
					gbKeyType = KEY_UD;
					SetFocus(editWeight);	
				}break;
				case DID_PATIENT_UPDATE:{
					//更新病人信息，包括病人资料，趋势数据，报警数据等等
					BOOL res = FALSE;
					
					memset(strMenu, 0, sizeof strMenu);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_PATIENT, "ackinfo_update", strMenu, sizeof strMenu,"This operation will delete measurement data before, eg. Trace, alarm information.");	
					res = ACKDialog(hDlg, strMenu, ACK_ATTENTION);
					
					if(res){
						if(B_PRINTF) printf("%s:%d Return TRUE.\n", __FILE__, __LINE__);
						UpdatePatientInfo();
						
						//初始化趋势数据
						InitTrendArray();
					
						//向中央机发送信息
						NetSend_PatientInfo();
						
						//设置病人类型,NIBP与之同步 2009-12-29 09:09:01
						gCfgNibp.bObject = gCfgPatient.bObject;
						SetIntValueToResFile(gFileSetup, "NIBPSetup", "object",  gCfgNibp.bObject, 1);
						//SetNibpObject();
						NIBP_Object_Change=TRUE;
						//如果正在测量中改变了对象,停止测量
						if(gValueNibp.bStatus == NIBP_STATUS_MANUAL){
							NIBP_Cancel();
						}
							
					}
							
				}break;
				case DID_PATIENT_OK:{
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
// 			ChangeWavePanelsLeft((PRECT)NULL);
			
			EndDialog(hDlg, wParam);	
		}break;
	}
	
	temp = DefaultDialogProc(hDlg, message, wParam, lParam);
	return(temp);
}

/*
	建立对话框
*/
void CreateDlgPatient(HWND hWnd)
{
	DlgSet.controls = CtrlSet;
	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}


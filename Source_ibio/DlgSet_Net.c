/*************************************************************************** 
 *	Module Name:	DlgSet_Net
 *
 *	Abstract:	设置Net的主对话框
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *					2008-10-24 14:43:12
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"



#define DID_NET_HELP		DID_NET
#define DID_NET_OK			DID_NET+1
#define DID_NET_CNACEL		DID_NET+2
#define DID_NET_BED_SET		DID_NET+3
#define DID_NET_BED			DID_NET+4
#define DID_NET_IP_SET		DID_NET+5
#define DID_NET_IP0			DID_NET+6
#define DID_NET_IP1			DID_NET+7
#define DID_NET_IP2			DID_NET+8
#define DID_NET_IP3			DID_NET+9
#define DID_NET_MASK_SET		DID_NET+10
#define DID_NET_MASK0		DID_NET+11
#define DID_NET_MASK1		DID_NET+12
#define DID_NET_MASK2		DID_NET+13
#define DID_NET_MASK3		DID_NET+14
#define DID_NET_GW_SET		DID_NET+15
#define DID_NET_GW0			DID_NET+16
#define DID_NET_GW1			DID_NET+17
#define DID_NET_GW2			DID_NET+18
#define DID_NET_GW3			DID_NET+19
#define DID_NET_CAPTION			DID_NET+20
#define DID_NET_POINT			DID_NET+21




//帮助对话框
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;

//其他控件
static HWND btnOk;
static HWND btnCancel;
static HWND btnSetBed;
static HWND btnSetIP;
static HWND btnSetMask;
static HWND btnSetGW;
static HWND editBed;
static HWND editIP0, editIP1, editIP2, editIP3;
static HWND editMask0, editMask1,editMask2,editMask3;
static HWND editGW0, editGW1, editGW2, editGW3;

//控件回调函数
static WNDPROC OldBtnProc;	
static WNDPROC OldEditProc;

//对话框属性
static DLGTEMPLATE DlgSet= {
	WS_VISIBLE | WS_BORDER,
	WS_EX_NOCLOSEBOX,	
	DLG_X, DLG_Y, DLG_LENGHT+25, DLG_HIGH,	
// 5, 32, 310, 240,
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
		5,DLG_HIGH-50, DLG_LENGHT+25-15, 40,
		DID_NET_HELP,
		"",
		0
	}
};


//网络设置是否改变
static BOOL gbNetChanged = FALSE;


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
				case DID_NET_BED_SET:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NET, "help_bedno", strHelp, sizeof strHelp,"Set bed number.");	
					SetWindowText(hWnd, strHelp);
				
				}break;
				case DID_NET_IP_SET:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NET, "help_ip", strHelp, sizeof strHelp,"Set IP address.");	
					SetWindowText(hWnd, strHelp);
				
				}break;
				case DID_NET_MASK_SET:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NET, "help_mask", strHelp, sizeof strHelp,"Set IP mask.");	
					SetWindowText(hWnd, strHelp);
				
				}break;
				case DID_NET_GW_SET:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NET, "help_gateway", strHelp, sizeof strHelp,"Set Gateway.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NET_OK:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NET, "help_ok", strHelp, sizeof strHelp,"Change the  configuration.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_NET_CNACEL:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NET, "help_canel", strHelp, sizeof strHelp,"Return to previous menu.");	
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


//临时存储上下限，调节的时候使用
static int iTmpBed=0; 
static int iTmpIP0 = 0,iTmpIP1 = 0, iTmpIP2 = 0, iTmpIP3 = 0;
static int iTmpMask0 = 0,iTmpMask1 = 0, iTmpMask2 = 0, iTmpMask3 = 0; 
static int iTmpGW0 = 0,iTmpGW1 = 0, iTmpGW2 = 0, iTmpGW3 = 0;  

//上下限刻度调整的增量
#define NET_LIMIT_INTER		1	//per

//限值
#define BED_LIMIT_MAX	240
#define BED_LIMIT_MIN		1

#define NET_LIMIT_MAX	255
#define NET_LIMIT_MIN		0


/*
	设置上下限文字
*/
static int SetLimitText(HWND hWnd, int iLimit)
{
	unsigned char strLimit[30] = {0};
	unsigned char strUnit[10] = {0};
	
	memset(strLimit, 0, sizeof strLimit);
	snprintf(strLimit, sizeof strLimit, "%d", iLimit, strUnit);
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
						case DID_NET_BED:{
							if(iTmpBed<BED_LIMIT_MAX){
								iTmpBed += NET_LIMIT_INTER;
								//iTmpIP3 += NET_LIMIT_INTER;
								SetLimitText(hWnd, iTmpBed);			
								//SetLimitText(editIP3, iTmpIP3);
							}
						}break;
						case DID_NET_IP0:{
							if(iTmpIP0<NET_LIMIT_MAX){
								iTmpIP0 += NET_LIMIT_INTER;
								SetLimitText(hWnd, iTmpIP0);			
							}
						}break;
						case DID_NET_IP1:{
							if(iTmpIP1<NET_LIMIT_MAX){
								iTmpIP1 += NET_LIMIT_INTER;
								SetLimitText(hWnd, iTmpIP1);			
							}
						}break;
						case DID_NET_IP2:{
							if(iTmpIP2<NET_LIMIT_MAX){
								iTmpIP2 += NET_LIMIT_INTER;
								SetLimitText(hWnd, iTmpIP2);			
							}
						}break;
						case DID_NET_IP3:{//与床号的限制相同
							if(iTmpIP3<BED_LIMIT_MAX){
								iTmpIP3 += NET_LIMIT_INTER;
								SetLimitText(hWnd, iTmpIP3);			
							}
						}break;
						case DID_NET_MASK0:{
							if(iTmpMask0<NET_LIMIT_MAX){
								iTmpMask0 += NET_LIMIT_INTER;
								SetLimitText(hWnd, iTmpMask0);			
							}
						}break;
						case DID_NET_MASK1:{
							if(iTmpMask1<NET_LIMIT_MAX){
								iTmpMask1 += NET_LIMIT_INTER;
								SetLimitText(hWnd, iTmpMask1);			
							}
						}break;
						case DID_NET_MASK2:{
							if(iTmpMask2<NET_LIMIT_MAX){
								iTmpMask2 += NET_LIMIT_INTER;
								SetLimitText(hWnd, iTmpMask2);			
							}
						}break;
						case DID_NET_MASK3:{
							if(iTmpMask3<NET_LIMIT_MAX){
								iTmpMask3 += NET_LIMIT_INTER;
								SetLimitText(hWnd, iTmpMask3);			
							}
						}break;
						case DID_NET_GW0:{
							if(iTmpGW0<NET_LIMIT_MAX){
								iTmpGW0 += NET_LIMIT_INTER;
								SetLimitText(hWnd, iTmpGW0);			
							}
						}break;
						case DID_NET_GW1:{
							if(iTmpGW1<NET_LIMIT_MAX){
								iTmpGW1 += NET_LIMIT_INTER;
								SetLimitText(hWnd, iTmpGW1);			
							}
						}break;
						case DID_NET_GW2:{
							if(iTmpGW2<NET_LIMIT_MAX){
								iTmpGW2 += NET_LIMIT_INTER;
								SetLimitText(hWnd, iTmpGW2);			
							}
						}break;
						case DID_NET_GW3:{
							if(iTmpGW3<NET_LIMIT_MAX){
								iTmpGW3 += NET_LIMIT_INTER;
								SetLimitText(hWnd, iTmpGW3);			
							}
						}break;
					}
				}break;
				case SCANCODE_CURSORBLOCKUP:{
					switch(id){
						case DID_NET_BED:{
							if(iTmpBed>BED_LIMIT_MIN){
								iTmpBed -= NET_LIMIT_INTER;
								//iTmpIP3 -= NET_LIMIT_INTER;
								SetLimitText(hWnd, iTmpBed);		
								//SetLimitText(editIP3, iTmpIP3);	
							}
						}break;
						case DID_NET_IP0:{
							if(iTmpIP0>NET_LIMIT_MIN){
								iTmpIP0 -= NET_LIMIT_INTER;
								SetLimitText(hWnd, iTmpIP0);			
							}
						}break;
						case DID_NET_IP1:{
							if(iTmpIP1>NET_LIMIT_MIN){
								iTmpIP1 -= NET_LIMIT_INTER;
								SetLimitText(hWnd, iTmpIP1);
							}
						}break;
						case DID_NET_IP2:{
							if(iTmpIP2>NET_LIMIT_MIN){
								iTmpIP2 -= NET_LIMIT_INTER;
								SetLimitText(hWnd, iTmpIP2);
							}
						}break;
						case DID_NET_IP3:{//与床号的限制相同
							if(iTmpIP3>BED_LIMIT_MIN){
								iTmpIP3 -= NET_LIMIT_INTER;
								SetLimitText(hWnd, iTmpIP3);
							}
						}break;
						case DID_NET_MASK0:{
							if(iTmpMask0>NET_LIMIT_MIN){
								iTmpMask0 -= NET_LIMIT_INTER;
								SetLimitText(hWnd, iTmpMask0);			
							}
						}break;
						case DID_NET_MASK1:{
							if(iTmpMask1>NET_LIMIT_MIN){
								iTmpMask1 -= NET_LIMIT_INTER;
								SetLimitText(hWnd, iTmpMask1);			
							}
						}break;
						case DID_NET_MASK2:{
							if(iTmpMask2>NET_LIMIT_MIN){
								iTmpMask2 -= NET_LIMIT_INTER;
								SetLimitText(hWnd, iTmpMask2);			
							}
						}break;
						case DID_NET_MASK3:{
							if(iTmpMask3>NET_LIMIT_MIN){
								iTmpMask3 -= NET_LIMIT_INTER;
								SetLimitText(hWnd, iTmpMask3);			
							}
						}break;
						case DID_NET_GW0:{
							if(iTmpGW0>NET_LIMIT_MIN){
								iTmpGW0 -= NET_LIMIT_INTER;
								SetLimitText(hWnd, iTmpGW0);			
							}
						}break;
						case DID_NET_GW1:{
							if(iTmpGW1>NET_LIMIT_MIN){
								iTmpGW1 -= NET_LIMIT_INTER;
								SetLimitText(hWnd, iTmpGW1);			
							}
						}break;
						case DID_NET_GW2:{
							if(iTmpGW2>NET_LIMIT_MIN){
								iTmpGW2 -= NET_LIMIT_INTER;
								SetLimitText(hWnd, iTmpGW2);			
							}
						}break;
						case DID_NET_GW3:{
							if(iTmpGW3>NET_LIMIT_MIN){
								iTmpGW3 -= NET_LIMIT_INTER;
								SetLimitText(hWnd, iTmpGW3);			
							}
						}break;
					}
				}break;
				case SCANCODE_ENTER:{
					switch(id){
						case DID_NET_BED:{
							SetFocus(btnSetBed);
							gbKeyType = KEY_TAB;
						}break;
						case DID_NET_IP0:{
							SetFocus(editIP1);
							gbKeyType = KEY_UD;
						}break;
						case DID_NET_IP1:{
							SetFocus(editIP2);
							gbKeyType = KEY_UD;
						}break;
						case DID_NET_IP2:{
							SetFocus(editIP3);
							gbKeyType = KEY_UD;
						}break;
						case DID_NET_IP3:{
							SetFocus(btnSetIP);
							gbKeyType = KEY_TAB;
						}break;
						case DID_NET_MASK0:{
							SetFocus(editMask1);
							gbKeyType = KEY_UD;
						}break;
						case DID_NET_MASK1:{
							SetFocus(editMask2);
							gbKeyType = KEY_UD;
						}break;
						case DID_NET_MASK2:{
							SetFocus(editMask3);
							gbKeyType = KEY_UD;
						}break;
						case DID_NET_MASK3:{
							SetFocus(btnSetMask);
							gbKeyType = KEY_TAB;
						}break;
						case DID_NET_GW0:{
							SetFocus(editGW1);
							gbKeyType = KEY_UD;
						}break;
						case DID_NET_GW1:{
							SetFocus(editGW2);
							gbKeyType = KEY_UD;
						}break;
						case DID_NET_GW2:{
							SetFocus(editGW3);
							gbKeyType = KEY_UD;
						}break;
						case DID_NET_GW3:{
							SetFocus(btnSetGW);
							gbKeyType = KEY_TAB;
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
		
		
/*
	对话框回调函数
*/
static int DlgProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	char strMenu[100];
	int i;
	RECT rcDlg;
	HDC hdc;
	switch(message){
		case MSG_INITDIALOG:{
			//caption
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NET, "caption", strMenu, sizeof strMenu,"Net Setup");	
			editCaption = CreateWindow("static", strMenu, WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE, 
					     DID_NET_CAPTION, 0,5, 280, 25, hDlg, 0);
			
/*
			if(gCfgSystem.bLanguage == LANGUAGE_SPANISH)
				SetWindowCaption(hDlg, "Configuracion red");
			else
				SetWindowCaption(hDlg, "Net Setup");
*/
			gbKeyType = KEY_TAB;
			
			gbNetChanged = FALSE;
			
// 			memset(strMenu, 0, sizeof strMenu);
// 			GetStringFromResFile(gsLanguageRes, "TIME", "year", strMenu, sizeof strMenu);	
/*
			if(gCfgSystem.bLanguage == LANGUAGE_SPANISH)
				btnSetBed = CreateWindow("button", "Cama NO.", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
										DID_NET_BED_SET, 10, 5, 100, 25, hDlg, 0);
			else
				btnSetBed = CreateWindow("button", "Bed NO.", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
										DID_NET_BED_SET, 10, 5, 100, 25, hDlg, 0);
*/			
			
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NET, "bedNo", strMenu, sizeof strMenu,"Bed NO.");	
			btnSetBed = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
							DID_NET_BED_SET, 5, 40, 80, 25, hDlg, 0);					
			OldBtnProc = SetWindowCallbackProc(btnSetBed, BtnProc);

			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NET, "IP", strMenu, sizeof strMenu,"IP");	
			btnSetIP = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
									 DID_NET_IP_SET, 5, 70, 80, 25, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnSetIP, BtnProc);
			/*
			if(gCfgSystem.bLanguage == LANGUAGE_SPANISH)
				btnSetMask = CreateWindow("button", "Mascara", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
										DID_NET_MASK_SET, 10, 65, 100, 25, hDlg, 0);
			else
				btnSetMask = CreateWindow("button", "MASK", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
										DID_NET_MASK_SET, 10, 65, 100, 25, hDlg, 0);
			*/
			
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes,STR_SETTING_DLG_NET, "mask", strMenu, sizeof strMenu,"MASK");	
			btnSetMask = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
										DID_NET_MASK_SET, 5, 100, 80, 25, hDlg, 0)	;					
			OldBtnProc = SetWindowCallbackProc(btnSetMask, BtnProc);
			/*
			if(gCfgSystem.bLanguage == LANGUAGE_SPANISH)
				btnSetGW= CreateWindow("button", "Pasarela", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
									  DID_NET_GW_SET, 10, 95, 100, 25, hDlg, 0);
			else
				btnSetGW= CreateWindow("button", "Gateway", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
									  DID_NET_GW_SET, 10, 95, 100, 25, hDlg, 0);
			*/
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NET, "gateway", strMenu, sizeof strMenu,"Gateway");	
			  btnSetGW= CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
			  						DID_NET_GW_SET, 5, 130, 80, 25, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnSetGW, BtnProc);
		
			//获得限值
			iTmpBed = gCfgNet.wBedNO;
			editBed = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER |ES_CENTER,
								   DID_NET_BED, 90, 40, 40, 23, hDlg, 0);
			OldEditProc = SetWindowCallbackProc(editBed, EditProc);
			SetLimitText(editBed, iTmpBed);					
			
			//Get Net Info
			unsigned char tmpstr[16];
			unsigned char *ptmp;
			unsigned int ivalue;
			unsigned int mask_value;
			unsigned int gw_value;
			
			//IP
			strncpy(tmpstr, gCfgNet.cIP, 16);
  			ptmp = (unsigned char *)strtok(tmpstr, ".");
  			ivalue = strtol(ptmp, NULL, 10);
  			iTmpIP0 = ivalue;
 			ptmp = (unsigned char *)strtok(NULL, ".");
 			ivalue = strtol(ptmp, NULL, 10);
			iTmpIP1 = ivalue;
			ptmp = (unsigned char *)strtok(NULL, ".");
			ivalue = strtol(ptmp, NULL, 10);
			iTmpIP2 = ivalue;
			ptmp = (unsigned char *)strtok(NULL, ".");
			ivalue = strtol(ptmp, NULL, 10);
			iTmpIP3 = ivalue;
			
			//Mask
			strncpy(tmpstr, gCfgNet.cMask, 16);
			ptmp = (unsigned char *)strtok(tmpstr, ".");
			ivalue = strtol(ptmp, NULL, 10);
			iTmpMask0= ivalue;
			ptmp = (unsigned char *)strtok(NULL, ".");
			ivalue = strtol(ptmp, NULL, 10);
			iTmpMask1 = ivalue;
			ptmp = (unsigned char *)strtok(NULL, ".");
			ivalue = strtol(ptmp, NULL, 10);
			iTmpMask2= ivalue;
			ptmp = (unsigned char *)strtok(NULL, ".");
			ivalue = strtol(ptmp, NULL, 10);
			iTmpMask3= ivalue;

			//GW
			strncpy(tmpstr, gCfgNet.cGW, 16);
			ptmp = (unsigned char *)strtok(tmpstr, ".");
			ivalue = strtol(ptmp, NULL, 10);
			iTmpGW0 = ivalue;
			ptmp = (unsigned char *)strtok(NULL, ".");
			ivalue = strtol(ptmp, NULL, 10);
			iTmpGW1 = ivalue;
			ptmp = (unsigned char *)strtok(NULL, ".");
			ivalue = strtol(ptmp, NULL, 10);
			iTmpGW2 = ivalue;
			ptmp = (unsigned char *)strtok(NULL, ".");
			ivalue = strtol(ptmp, NULL, 10);
			iTmpGW3 = ivalue;
		
			editIP0 = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER|ES_CENTER,
								   DID_NET_IP0, 90, 70, 40, 23, hDlg, 0);
			OldEditProc = SetWindowCallbackProc(editIP0, EditProc);
  			SetLimitText(editIP0, iTmpIP0);					
			
			editIP1 = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER|ES_CENTER,
								   DID_NET_IP1, 135, 70, 40, 23, hDlg, 0);
			OldEditProc = SetWindowCallbackProc(editIP1, EditProc);
 			SetLimitText(editIP1, iTmpIP1);					
			
			editIP2 = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER|ES_CENTER,
								   DID_NET_IP2, 180, 70, 40, 23, hDlg, 0);
			OldEditProc = SetWindowCallbackProc(editIP2, EditProc);
 			SetLimitText(editIP2, iTmpIP2);

			editIP3 = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER|ES_CENTER,
								   DID_NET_IP3, 225, 70, 40, 23, hDlg, 0);
			OldEditProc = SetWindowCallbackProc(editIP3, EditProc);
 			SetLimitText(editIP3, iTmpIP3);
			
			editMask0 = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER|ES_CENTER,
								   DID_NET_MASK0, 90, 100, 40, 23, hDlg, 0);
			OldEditProc = SetWindowCallbackProc(editMask0, EditProc);
			SetLimitText(editMask0, iTmpMask0);
			
			editMask1 = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER|ES_CENTER,
								   DID_NET_MASK1, 135, 100, 40, 23, hDlg, 0);
			OldEditProc = SetWindowCallbackProc(editMask1, EditProc);
			SetLimitText(editMask1, iTmpMask1);
			
			editMask2 = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER|ES_CENTER,
								   DID_NET_MASK2, 180, 100, 40, 23, hDlg, 0);
			OldEditProc = SetWindowCallbackProc(editMask2, EditProc);
			SetLimitText(editMask2, iTmpMask2);
			
			editMask3= CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER|ES_CENTER,
								   DID_NET_MASK3, 225, 100, 40, 23, hDlg, 0);
			OldEditProc = SetWindowCallbackProc(editMask3, EditProc);
			SetLimitText(editMask3, iTmpMask3);
			
			editGW0 = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER|ES_CENTER,
									 DID_NET_GW0, 90, 130, 40, 23, hDlg, 0);
			OldEditProc = SetWindowCallbackProc(editGW0, EditProc);
			SetLimitText(editGW0, iTmpGW0);
			
			editGW1 = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER|ES_CENTER,
									 DID_NET_GW1, 135, 130, 40, 23, hDlg, 0);
			OldEditProc = SetWindowCallbackProc(editGW1, EditProc);
			SetLimitText(editGW1, iTmpGW1);
			
			editGW2 = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER|ES_CENTER,
									 DID_NET_GW2, 180, 130, 40, 23, hDlg, 0);
			OldEditProc = SetWindowCallbackProc(editGW2, EditProc);
			SetLimitText(editGW2, iTmpGW2);
			
			editGW3 = CreateWindow("sledit", "", WS_CHILD | WS_VISIBLE | WS_BORDER|ES_CENTER,
									DID_NET_GW3, 225, 130, 40, 23, hDlg, 0);
			OldEditProc = SetWindowCallbackProc(editGW3, EditProc);
			SetLimitText(editGW3, iTmpGW3);
			
			//Exit
// 			memset(strMenu, 0, sizeof strMenu);
// 			GetStringFromResFile(gsLanguageRes, "GENERAL", "exit", strMenu, sizeof strMenu);	
/*
			if(gCfgSystem.bLanguage == LANGUAGE_SPANISH)
				btnOk= CreateWindow("button", "OK", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
								DID_NET_OK, 10, 130, 135, 25, hDlg, 0);
			else
				btnOk= CreateWindow("button", "OK", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
								DID_NET_OK, 10, 130, 135, 25, hDlg, 0);
			*/
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NET, "OK", strMenu, sizeof strMenu,"OK");	
			btnOk= CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
								DID_NET_OK, 5, 160, 80, 25, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);
			/*
			if(gCfgSystem.bLanguage == LANGUAGE_SPANISH)
				btnCancel = CreateWindow("button", "Cancelar", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
								DID_NET_CNACEL, 160, 130, 135, 25, hDlg, 0);
			else
				btnCancel = CreateWindow("button", "Cancel", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
								DID_NET_CNACEL, 160, 130, 135, 25, hDlg, 0);
			*/
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_NET, "Cancel", strMenu, sizeof strMenu,"Cancel");
			btnCancel = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
								DID_NET_CNACEL, 5, 190, 80, 25, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnCancel, BtnProc);
			
			editHelp = GetDlgItem(hDlg, DID_NET_HELP);
			OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp); 
			
			CreateWindow("static", ".", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE, 
					     		DID_NET_POINT, 130, 78, 5, 25, hDlg, 0);
			CreateWindow("static", ".", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE, 
					     		DID_NET_POINT, 175, 78, 5, 25, hDlg, 0);
			CreateWindow("static", ".", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE, 
					     		DID_NET_POINT, 220, 78, 5, 25, hDlg, 0);

			CreateWindow("static", ".", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE, 
					     		DID_NET_POINT, 130, 108, 5, 25, hDlg, 0);
			CreateWindow("static", ".", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE, 
					     		DID_NET_POINT, 175, 108, 5, 25, hDlg, 0);
			CreateWindow("static", ".", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE, 
					     		DID_NET_POINT, 220, 108, 5, 25, hDlg, 0);

			CreateWindow("static", ".", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE, 
					     		DID_NET_POINT, 130, 138, 5, 25, hDlg, 0);
			CreateWindow("static", ".", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE, 
					     		DID_NET_POINT, 175, 138, 5, 25, hDlg, 0);
			CreateWindow("static", ".", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE, 
					     		DID_NET_POINT, 220, 138, 5, 25, hDlg, 0);
			
			SetFocus(btnCancel);
		}break;
		case MSG_PAINT:{
		
		}break;
		case MSG_COMMAND:{
			int	id  = LOWORD(wParam);
			
			switch(id){
				case DID_NET_BED_SET:{
					gbKeyType = KEY_UD;
					SetFocus(editBed);
				}break;
				case DID_NET_IP_SET:{
					gbKeyType = KEY_UD;
					SetFocus(editIP0);
				}break;
				case DID_NET_MASK_SET:{
					gbKeyType = KEY_UD;
					SetFocus(editMask0);
				}break;
				case DID_NET_GW_SET:{
					gbKeyType = KEY_UD;
					SetFocus(editGW0);
				}break;
				case DID_NET_CNACEL:{
					gbNetChanged = FALSE;
					PostMessage(hDlg, MSG_CLOSE, 0, 0L);
				}break;
				case DID_NET_OK:{
					gbNetChanged = TRUE;
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
				if(B_PRINTF) printf("*****************************\n");
			if(gbNetChanged){
				//Get Setup Value
				if(B_PRINTF) printf("Set Net Config: \n");
				gCfgNet.wBedNO = iTmpBed;
				if(B_PRINTF) printf("Bed: %d.\n", gCfgNet.wBedNO);
				memset(gCfgNet.cIP, 0, 16);
				snprintf(gCfgNet.cIP, 16, "%d.%d.%d.%d", iTmpIP0, iTmpIP1, iTmpIP2,iTmpIP3);
				
				if(B_PRINTF) printf("IP: %s.\n", gCfgNet.cIP);
				memset(gCfgNet.cMask, 0, 16);
				snprintf(gCfgNet.cMask, 16, "%d.%d.%d.%d", iTmpMask0, iTmpMask1, iTmpMask2, iTmpMask3);
				if(B_PRINTF) printf("MASK: %s.\n", gCfgNet.cMask);
				memset(gCfgNet.cGW, 0, 16);
				snprintf(gCfgNet.cGW, 16, "%d.%d.%d.%d", iTmpGW0, iTmpGW1, iTmpGW2, iTmpGW3);
				if(B_PRINTF) printf("GW: %s.\n", gCfgNet.cGW);

				//把网络配置写到配置文件
				SetIntValueToResFile(gFileMachineConfig, "NET", "bed",  gCfgNet.wBedNO, 3);
				SetValueToEtcFile(gFileMachineConfig, "NET", "ip", gCfgNet.cIP);
				SetValueToEtcFile(gFileMachineConfig, "NET", "mask", gCfgNet.cMask);
				SetValueToEtcFile(gFileMachineConfig, "NET", "gw", gCfgNet.cGW);
				
				NetSet(&gCfgNet);

				//填充网络发送的报文头
				FillNetMsgHead();
			}
		
			EndDialog(hDlg, wParam);	
		}break;
	}
	
	temp = DefaultDialogProc(hDlg, message, wParam, lParam);
	return(temp);
}

/*
	建立对话框
*/
void CreateDlgNet(HWND hWnd)
{
	DlgSet.controls = CtrlSet;
	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}

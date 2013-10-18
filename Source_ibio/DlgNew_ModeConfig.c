/*************************************************************************** 
 *	Module Name:	DlgNew_ModeConfig
 *
 *	Abstract:模式选择
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2012-12-7 16:14:18
 *	
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"
#include <dirent.h>
//DID_MODECONF
#define DID_MODECONF_HELP			DID_MODECONF
#define DID_MODECONF_OK				DID_MODECONF+1
#define DID_MODECONF_CAPTION		DID_MODECONF+2
#define DID_MODECONF_FACTORY		DID_MODECONF+3
#define DID_MODECONF_USERCONF		DID_MODECONF+4
#define DID_MODECONF_SAVECONF		DID_MODECONF+5
#define DID_MODECONF_DELCONF		DID_MODECONF+6
#define DID_MODECONF_FACTORY_COB		DID_MODECONF+7
#define DID_MODECONF_USERCONF_COB		DID_MODECONF+8
#define DID_MODECONF_SAVECONF_COB		DID_MODECONF+9
#define DID_MODECONF_DELCONF_COB		DID_MODECONF+10



//帮助对话框
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;

//其他控件 
static HWND btnOk;
static HWND btnFactory;
static HWND btnUserConf;
static HWND btnSaveConf;
static HWND btnDelConf;
static HWND cobFactory;
static HWND cobUserConf;
static HWND cobSaveConf;
static HWND cobDelConf;

//控件回调函数
static WNDPROC OldBtnProc;
static WNDPROC OldCobProc;

static int count=0;
static int flag=0;
static unsigned char *strCobUser[10];
static unsigned char strSaveConf[50];
static unsigned char strSetConf[50];
static int GetUserConfig();
static int SetUserConfig(const char *strSetConf);
static int DelUserConfig(const char *strSetConf);
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
		DID_MODECONF_HELP,
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
				case DID_MODECONF_FACTORY:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MODECONF, "help_factory", strHelp, sizeof strHelp,"Restore the factory default configuration.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MODECONF_USERCONF:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MODECONF, "help_userconf", strHelp, sizeof strHelp,"Select user default configuration as current configuration.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MODECONF_SAVECONF:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MODECONF, "help_saveconf", strHelp, sizeof strHelp,"Save current configuration as user default configuration.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MODECONF_DELCONF:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MODECONF, "help_delconf", strHelp, sizeof strHelp,"Delete saved user default configuration.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_MODECONF_OK:{
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
static int LoadFactoryConf(HWND hWnd,int index)
{
	BOOL res = FALSE;
	char strACKInfo[200];
	if(flag == 0)
	{
		flag++;
		if(B_PRINTF)printf("res=%d\n",res);
		memset(strACKInfo, 0, sizeof strACKInfo);
		GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MODECONF, "ackinfo_factory", strACKInfo, sizeof strACKInfo,"Are you sure restore the factory default configuration?");	
		res = ACKDialog(hWnd, strACKInfo, ACK_ATTENTION);
		
		if(res){
			if(gCfgPatient.bObject!=index){
				gCfgPatient.bObject= index;
				//加载配置
				LoadSetup(gCfgPatient.bObject);
				if(B_PRINTF)printf("gCfgPatient.bObject=%d\n",gCfgPatient.bObject);
			}
		}
		return 0;
	}
	else
		flag=0;
	return 0;
	
}

static int LoadUserConfig(HWND hWnd,int index)
{
	BOOL res = FALSE;
	char strACKInfo[200];
	if(flag == 0)
	{
		flag++;
		memset(strSetConf, 0, sizeof strSetConf);
		SendMessage(cobUserConf, MSG_GETTEXT, sizeof strSetConf, (LPARAM)strSetConf);
		memset(strACKInfo, 0, sizeof strACKInfo);
		GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MODECONF, "ackinfo_user", strACKInfo, sizeof strACKInfo,"Are you sure select user default configuration as current configuration?");	
		res = ACKDialog(hWnd, strACKInfo, ACK_ATTENTION);
		if(res){
			SetUserConfig(strSetConf);
			LoadResentSetup();
			GetUserConfig();//刷新列表
			}
		
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
	BOOL res = FALSE;
	char strACKInfo[200];
	
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
			if(wParam==SCANCODE_ENTER||CBN_EDITCHANGE ==HIWORD(wParam)){
					id = GetDlgCtrlID(hWnd);
					index = SendMessage(hWnd, CB_GETCURSEL, 0, 0);
						
					switch(id){
						case DID_MODECONF_FACTORY_COB:{
							if(index<3){
								LoadFactoryConf(hWnd,index);
								SetFocus(btnFactory);	
							}
							SetFocus(btnFactory);
						}break;
						case DID_MODECONF_USERCONF_COB:{
							if(index!=0){
								LoadUserConfig(hWnd,index);
								
								SetFocus(btnUserConf);	
								}
							SetFocus(btnUserConf);	
						}break;
						case DID_MODECONF_DELCONF_COB:{
							memset(strSetConf, 0, sizeof strSetConf);
							SendMessage(cobDelConf, MSG_GETTEXT, sizeof strSetConf, (LPARAM)strSetConf);
							if(index!=0){
								DelUserConfig(strSetConf);
								GetUserConfig();//刷新列表
							}
							SetFocus(btnDelConf);	
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
	创建控件函数
*/
static int CreateCtlProc(HWND hDlg)
{
	editCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE, 
					     DID_MODECONF_CAPTION, 0, 5, 240, 25, hDlg, 0);
	editHelp = GetDlgItem(hDlg, DID_MODECONF_HELP);

	btnFactory = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MODECONF_FACTORY, 10, 30+30*0, 125, 26, hDlg, 0);
	btnUserConf = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MODECONF_USERCONF, 10, 30+30*1, 125, 26, hDlg, 0);
	btnDelConf = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MODECONF_DELCONF, 10, 30+30*2, 125, 26, hDlg, 0);
	btnSaveConf = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MODECONF_SAVECONF, 10,30+30*3 , 125, 26, hDlg, 0);
	btnOk = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_MODECONF_OK, 10, 30+30*4, 125, 26, hDlg, 0);
	
	cobFactory = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_MODECONF_FACTORY_COB, 140, 30+30*0, 95, 26, hDlg, 0);
	cobUserConf = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_MODECONF_USERCONF_COB, 140, 30+30*1, 95, 26, hDlg, 0);
	cobDelConf = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
					DID_MODECONF_DELCONF_COB, 140, 30+30*2, 95, 26, hDlg, 0);
	cobSaveConf = CreateWindow("edit", "", WS_CHILD | WS_VISIBLE |WS_BORDER,
					DID_MODECONF_SAVECONF_COB, 140, 30+30*3, 95, 26, hDlg, 0);
	return 0;
}
static int InitCtlName(HWND hDlg)
{
	char strMenu[100];

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MODECONF, "caption", strMenu, sizeof strMenu,"Mode Setup");
	SetWindowText(editCaption, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "previous", strMenu, sizeof strMenu,"Previous");	
	SetWindowText(btnOk, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MODECONF, "factory", strMenu, sizeof strMenu,"Factory");	
	SetWindowText(btnFactory, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MODECONF, "user_conf", strMenu, sizeof strMenu,"User Config");	
	SetWindowText(btnUserConf, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MODECONF, "save_conf", strMenu, sizeof strMenu,"Save Config");	
	SetWindowText(btnSaveConf, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MODECONF, "del_conf", strMenu, sizeof strMenu,"Delete");	
	SetWindowText(btnDelConf, strMenu);

	
	return 0;
}
/*
static int GetUserConfig()
{
	char strMenu[100];
	char *strCancel="Cencel";
	int i;
	char *strUserconf[10]={"userconf0","userconf1","userconf2","userconf3","userconf4","userconf5","userconf6","userconf7","userconf8","userconf8"};
	count=0;
	strCobUser[count]=strCancel;
	SendMessage(cobUserConf, CB_ADDSTRING, 0, (LPARAM)strCobUser[count]);
	SendMessage(cobDelConf, CB_ADDSTRING, 0, (LPARAM)strCobUser[count]);
	count++;
	for(i=0;i<10;i++)
	{
		memset(strMenu, 0, sizeof strMenu);
		GetStringFromResFiles(gFileMachineConfig, "USERCONF",strUserconf[i], strMenu, sizeof strMenu,"*");
		if(strcmp(strMenu,"*")){
			strCobUser[count]=strMenu;
			SendMessage(cobUserConf, CB_ADDSTRING, 0, (LPARAM)strCobUser[count]);
			SendMessage(cobDelConf, CB_ADDSTRING, 0, (LPARAM)strCobUser[count]);
		//	printf("strCobUser[%d]=%s\n",count,strCobUser[count]);
			count++;
		}
	}
	SendMessage(cobUserConf, CB_SETCURSEL,0, 0);
	SendMessage(cobDelConf, CB_SETCURSEL,0, 0);
	return 0;
}
*/
static int GetUserConfig()
{
	int i;
	unsigned char strCommand[200];
	char *strCancel="Cancel";
	DIR * dir;
        struct dirent * ptr;
	count=0;
	strCobUser[count]=strCancel;
	count++;
		//扫描用户配置文件夹下的配置文件
	dir=opendir("cfg/UserCfg");
	while((ptr = readdir(dir)) != NULL)
               {
                        if((strcmp(ptr->d_name,".") == 0) || (strcmp(ptr->d_name,"..") == 0))
                        {
                                continue;
                        }
                      printf("cfgname=%s\n",ptr->d_name);
					  
			strCobUser[count]= ptr->d_name;
			count ++;
                }
	SendMessage(cobUserConf, CB_RESETCONTENT,0, 0);
	SendMessage(cobDelConf, CB_RESETCONTENT,0, 0);
	for(i=0;i<count;i++)
		{
		   	SendMessage(cobUserConf, CB_ADDSTRING, 0, (LPARAM)strCobUser[i]);
			SendMessage(cobDelConf, CB_ADDSTRING, 0, (LPARAM)strCobUser[i]);
		}
	SendMessage(cobUserConf, CB_SETCURSEL,0, 0);
	SendMessage(cobDelConf, CB_SETCURSEL,0, 0);
	//保存当前配置为新用户配置
 	//snprintf(strCommand, sizeof strCommand, "cp %s cfg/UserCfg/%s", gFileSetup, strCFGName);
 	//printf("Command is %s\n", strCommand);
 	//system(strCommand);
	
	return 0;
}
static int InitCobList(HWND hDlg)
{
	char strMenu[100];
	int i;
	
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MODECONF, "cob_adult", strMenu, sizeof strMenu,"Adult");
	SendMessage(cobFactory, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MODECONF, "cob_child", strMenu, sizeof strMenu,"Child");
	SendMessage(cobFactory, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MODECONF, "cob_neonatal", strMenu, sizeof strMenu,"Neonatal");
	SendMessage(cobFactory, CB_ADDSTRING, 0, (LPARAM)strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_MODECONF, "cob_cancel", strMenu, sizeof strMenu,"Cancel");
	SendMessage(cobFactory, CB_ADDSTRING, 0, (LPARAM)strMenu);
	
	SendMessage(cobFactory, CB_SETCURSEL,gCfgPatient.bObject, 0);

	return 0;
}

static int InitCtlCallBackProc(HWND hDlg)
{

	OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);
	OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp);

	OldBtnProc = SetWindowCallbackProc(btnFactory, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnUserConf, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnSaveConf, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnDelConf, BtnProc);

	OldCobProc = SetWindowCallbackProc(cobFactory, CobProc);
	OldCobProc = SetWindowCallbackProc(cobUserConf, CobProc);
	//OldCobProc = SetWindowCallbackProc(cobSaveConf, EditProcHelp);
	OldCobProc = SetWindowCallbackProc(cobDelConf, CobProc);
	return 0;
}
/*
	保存用户设置:把当前配置拷贝到usercfg目录下
*/
static int SaveCFG(const char *strSaveConf)
{
	unsigned char strCommand[200];
	unsigned char strCFGName[200];
	snprintf(strCFGName, sizeof strCFGName, "%s.cfg", strSaveConf);
	
	printf("strSaveConf=%s\n",strCFGName);

	//保存当前配置为新用户配置
 	snprintf(strCommand, sizeof strCommand, "cp %s cfg/UserCfg/%s", gFileSetup, strCFGName);
 	printf("Command is %s\n", strCommand);
 	system(strCommand);

	return 0;
}
/*
	使用用户配置:把用户配置拷贝为Resent.cfg
*/
static int SetUserConfig(const char *strSetConf)
{
	unsigned char strCommand[200];
	unsigned char strCFGName[200];
	
	
	//snprintf(strCFGName, sizeof strCFGName, "%s.cfg", strSaveConf);
	
	//printf("strSaveConf=%s\n",strCFGName);

	//把用户配置文件拷贝到resent.cfg
 	snprintf(strCommand, sizeof strCommand, "cp  cfg/UserCfg/%s %s", strSetConf,gFileSetup);
 	printf("Command is %s\n", strCommand);
 	system(strCommand);

	
	return 0;
}
/*
	删除配置文件
*/
static int DelUserConfig(const char *strSetConf)
{
	unsigned char strCommand[200];
	unsigned char strCFGName[200];
	//把用户配置文件拷贝到resent.cfg
 	snprintf(strCommand, sizeof strCommand, "rm cfg/UserCfg/%s", strSetConf);
 	printf("Command is %s\n", strCommand);
 	system(strCommand);
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
	unsigned char strinput[50]={};
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
	
			GetUserConfig();
			gbKeyType = KEY_TAB;
			SetFocus(btnOk);
		}break;
		case MSG_PAINT:{
			

		}break;
		case MSG_COMMAND:{
			int	id  = LOWORD(wParam);
			switch(id){
				case DID_MODECONF_FACTORY:{
					gbKeyType = KEY_UD;
					SetFocus(cobFactory);
					}break;
				case DID_MODECONF_USERCONF:{
					gbKeyType = KEY_UD;
					SetFocus(cobUserConf);
					}break;
				case DID_MODECONF_SAVECONF:{
					ghWnd_Input = cobSaveConf;
					GetWindowRect(cobSaveConf, &gRC_Input);
					GetWindowText(cobSaveConf,strinput,sizeof(strinput));
					KeyInputs(hDlg, gRC_Input,strinput,140);
					
					SendMessage(cobSaveConf, MSG_GETTEXT, sizeof strSaveConf, (LPARAM)strSaveConf);
					if(B_PRINTF)printf("strSaveConfilen=%d\n",strlen(strSaveConf));
					if(strlen(strSaveConf)){
						SaveCFG(strSaveConf);
						GetUserConfig();//刷新列表
						}
					}break;
				case DID_MODECONF_DELCONF:{
					gbKeyType = KEY_UD;
					SetFocus(cobDelConf);
					}break;
				case DID_MODECONF_OK:{
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
void CreateModeConfig(HWND hWnd)
{
 	DlgSet.controls = CtrlSet;
 	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}


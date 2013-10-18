/*************************************************************************** 
 *	Module Name:	DlgNew_waveformset.c
 *
 *	Abstract:	波形设置
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2012-12-3 16:55:00
 *	
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"
#include "NewDrawWave.h"
//DID_WAVESET
#define DID_WAVESET_HELP			DID_WAVESET
#define DID_WAVESET_OK				DID_WAVESET+1
#define DID_WAVESET_CAPTION			DID_WAVESET+2
#define DID_WAVESET_WAVEFORM0			DID_WAVESET+3
#define DID_WAVESET_WAVEFORM0_COB			DID_WAVESET+4
#define DID_WAVESET_WAVEFORM1			DID_WAVESET+5
#define DID_WAVESET_WAVEFORM1_COB			DID_WAVESET+6
#define DID_WAVESET_WAVEFORM2			DID_WAVESET+7
#define DID_WAVESET_WAVEFORM2_COB			DID_WAVESET+8
#define DID_WAVESET_WAVEFORM3			DID_WAVESET+9
#define DID_WAVESET_WAVEFORM3_COB			DID_WAVESET+10
#define DID_WAVESET_WAVEFORM4			DID_WAVESET+11
#define DID_WAVESET_WAVEFORM4_COB			DID_WAVESET+12
#define DID_WAVESET_WAVEFORM5			DID_WAVESET+13
#define DID_WAVESET_WAVEFORM5_COB			DID_WAVESET+14
#define DID_WAVESET_WAVEFORM6			DID_WAVESET+15
#define DID_WAVESET_WAVEFORM6_COB			DID_WAVESET+16
#define DID_WAVESET_WAVEFORM7			DID_WAVESET+17
#define DID_WAVESET_WAVEFORM7_COB			DID_WAVESET+18
#define DID_WAVESET_WAVEFORM8			DID_WAVESET+19
#define DID_WAVESET_WAVEFORM8_COB			DID_WAVESET+20
#define DID_WAVESET_WAVEFORM9			DID_WAVESET+21
#define DID_WAVESET_WAVEFORM9_COB			DID_WAVESET+22






//帮助对话框
static HWND StaticHelp;
static HWND StaticCaption;
static WNDPROC OldStaticProcHelp;

//其他控件 
static HWND btnOk;
static HWND btnwaveform0;
static HWND cobwaveform0;
static HWND btnwaveform1;
static HWND cobwaveform1;
static HWND btnwaveform2;
static HWND cobwaveform2;
static HWND btnwaveform3;
static HWND cobwaveform3;
static HWND btnwaveform4;
static HWND cobwaveform4;
static HWND btnwaveform5;
static HWND cobwaveform5;
static HWND btnwaveform6;
static HWND cobwaveform6;
static HWND btnwaveform7;
static HWND cobwaveform7;
static HWND btnwaveform8;
static HWND cobwaveform8;
static HWND btnwaveform9;
static HWND cobwaveform9;
//控件回调函数
static WNDPROC OldBtnProc;
static WNDPROC OldCobProc;

static int InitCobList();
static int GetStrIndex();
static int AddStrToCobList(HWND hWnd,unsigned char *strIndex[],int iCobCount);
unsigned char *strWaveIndex[]={"OFF", "ECG1","Cascade","I","II","III","AVR","AVL","AVF","V-","Spo2","Resp","IBP1","IBP2","CO2"};
	
static int flag=0;


//对话框属性
static DLGTEMPLATE DlgSet= {
	//WS_VISIBLE | WS_CAPTION | WS_BORDER,
	WS_VISIBLE | WS_BORDER,
	WS_EX_NOCLOSEBOX,	
	DLG_X, DLG_Y, DLG_LENGHT+20, DLG_HIGH,	
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
		5,DLG_HIGH-50, DLG_LENGHT-15+20, 40,
		DID_WAVESET_HELP,
		"",
		0
	}
};

/*
	帮助控件回调函数
*/
static int StaticProcHelp(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
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
				case DID_WAVESET_WAVEFORM0:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_WAVEFORM, "help_waveform0", strHelp, sizeof strHelp,"Select NO.1 waveform.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_WAVESET_WAVEFORM1:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_WAVEFORM, "help_waveform1", strHelp, sizeof strHelp,"Select NO.2 waveform.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_WAVESET_WAVEFORM2:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_WAVEFORM, "help_waveform2", strHelp, sizeof strHelp,"Select NO.3 waveform.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_WAVESET_WAVEFORM3:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_WAVEFORM, "help_waveform3", strHelp, sizeof strHelp,"Select NO.4 waveform.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_WAVESET_WAVEFORM4:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_WAVEFORM, "help_waveform4", strHelp, sizeof strHelp,"Select NO.5 waveform.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_WAVESET_WAVEFORM5:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_WAVEFORM, "help_waveform5", strHelp, sizeof strHelp,"Select NO.6 waveform.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_WAVESET_WAVEFORM6:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_WAVEFORM, "help_waveform6", strHelp, sizeof strHelp,"Select NO.7 waveform.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_WAVESET_WAVEFORM7:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_WAVEFORM, "help_waveform7", strHelp, sizeof strHelp,"Select NO.8 waveform.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_WAVESET_WAVEFORM8:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_WAVEFORM, "help_waveform8", strHelp, sizeof strHelp,"Select NO.9 waveform.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_WAVESET_WAVEFORM9:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_WAVEFORM, "help_waveform9", strHelp, sizeof strHelp,"Select NO.10 waveform.");	
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_WAVESET_OK:{
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

	temp = (*OldStaticProcHelp)(hWnd, message, wParam, lParam);
	return(temp);
}

//写入文件
static int SaveUsrWaveSet()
{
	int i;
	for(i=0;i<10;i++)
	{
		if(i==WAVEFORM_ECG1&&WaveformCob[0].bID!=WAVEFORM_ECG1)
			WaveformCob[0].bID=WAVEFORM_ECG1;
		if(i>1&&(WaveformCob[i].bID==WAVEFORM_ECG1 ||WaveformCob[i].bID==WAVEFORM_CASCADE))
			WaveformCob[i].bID=WAVEFORM_OFF;
		
	}
	
	SetIntValueToResFile(gFileSetup, "WaveformSetup", "waveform0_id",  WaveformCob[0].bID, 1);
	SetIntValueToResFile(gFileSetup, "WaveformSetup", "waveform1_id",  WaveformCob[1].bID, 1);
	SetIntValueToResFile(gFileSetup, "WaveformSetup", "waveform2_id",  WaveformCob[2].bID, 1);
	SetIntValueToResFile(gFileSetup, "WaveformSetup", "waveform3_id",  WaveformCob[3].bID, 1);
	SetIntValueToResFile(gFileSetup, "WaveformSetup", "waveform4_id",  WaveformCob[4].bID, 1);
	SetIntValueToResFile(gFileSetup, "WaveformSetup", "waveform5_id",  WaveformCob[5].bID, 1);
	SetIntValueToResFile(gFileSetup, "WaveformSetup", "waveform6_id",  WaveformCob[6].bID, 1);
	SetIntValueToResFile(gFileSetup, "WaveformSetup", "waveform7_id",  WaveformCob[7].bID, 1);
	SetIntValueToResFile(gFileSetup, "WaveformSetup", "waveform8_id",  WaveformCob[8].bID, 1);
	SetIntValueToResFile(gFileSetup, "WaveformSetup", "waveform9_id",  WaveformCob[9].bID, 1);
	return 0;
}

static int GetNewCobList(HWND hWnd, int CobIndex,int index)
{
	int i;
	if(flag == 0)
	{
		for(i=0;i<15;i++)
			{
				if(tmpWaveform[i].strWaveform==WaveformCob[CobIndex].strIndex[index])
					{
						tmpWaveform[WaveformCob[CobIndex].bID].bSwitch=0;
						WaveformCob[CobIndex].bID=tmpWaveform[i].bID;
						if(WaveformCob[CobIndex].bID!=0)tmpWaveform[WaveformCob[CobIndex].bID].bSwitch=1;
						
					}
			}
		//波形排序得到当前未选择的选项保存在strTemp中
		GetStrIndex();
		SendMessage(hWnd, CB_RESETCONTENT,0, 0);
		AddStrToCobList(hWnd,WaveformCob[CobIndex].strIndex,WaveformCob[CobIndex].Count);
				
		flag++;
		return 0;
	}
	else
		flag =0;
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
	int i;
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
					case DID_WAVESET_WAVEFORM0_COB:{
							
							SetFocus(btnwaveform0);	
						}break;
					case DID_WAVESET_WAVEFORM1_COB:{
							GetNewCobList(cobwaveform1,1,index);
							
							SetFocus(btnwaveform1);	
						}break;
					case DID_WAVESET_WAVEFORM2_COB:{
							GetNewCobList(cobwaveform2,2,index);
							
							SetFocus(btnwaveform2);	
						}break;
					case DID_WAVESET_WAVEFORM3_COB:{
							GetNewCobList(cobwaveform3,3,index);
							
							SetFocus(btnwaveform3);	
						}break;
					case DID_WAVESET_WAVEFORM4_COB:{
							GetNewCobList(cobwaveform4,4,index);
							
							SetFocus(btnwaveform4);	
						}break;
					case DID_WAVESET_WAVEFORM5_COB:{
							GetNewCobList(cobwaveform5,5,index);
						
							SetFocus(btnwaveform5);	
						}break;
					
					case DID_WAVESET_WAVEFORM6_COB:{
							GetNewCobList(cobwaveform6,6,index);
						
							SetFocus(btnwaveform6);	
						}break;
					
					case DID_WAVESET_WAVEFORM7_COB:{
							GetNewCobList(cobwaveform7,7,index);
							
							SetFocus(btnwaveform7);	
						}break;
					case DID_WAVESET_WAVEFORM8_COB:{
							GetNewCobList(cobwaveform8,8,index);
							
							SetFocus(btnwaveform8);	
						}break;
					case DID_WAVESET_WAVEFORM9_COB:{
							GetNewCobList(cobwaveform9,9,index);
							
							SetFocus(btnwaveform9);	
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
			SendMessage(StaticHelp, MSG_PAINT, 0, (LPARAM)GetDlgCtrlID(hWnd));
		}break;
		case MSG_KILLFOCUS:{
			SetWindowBkColor(hWnd, KILLFOCUS_COLOR);
		}break;	
	}
	
	temp = (*OldBtnProc)(hWnd, message, wParam, lParam);
	return(temp);
}
/*
	刷新列表
*/

static int AddStrToCobList(HWND hWnd,unsigned char *strIndex[],int iCobCount)
{
	int i;
	if(B_PRINTF) printf("strIndex[d]=%s\n",strIndex[0]);
	for(i=0;i<iCobCount;i++)
		{
			SendMessage(hWnd, CB_ADDSTRING, 0, (LPARAM)strIndex[i]);
		}
	SendMessage(hWnd, CB_SETCURSEL,0, 0);
	return 0;
}



static int GetStrIndex()
{
	
	int i,j,k,m;

	for(i=0;i<10;i++)
		{
			WaveformCob[i].Count=0;
		}
	for(i=0;i<10;i++)//每个列表循环得出每个列表的数组
		{
			if(i==0)  //列表1ECG1
				{
					tmpWaveform[WAVEFORM_ECG1].bSwitch = 1;
					WaveformCob[i].strIndex[0]=strWaveIndex[WAVEFORM_ECG1];
					WaveformCob[i].Count++;
					//if(B_PRINTF) printf("WaveformCob[0].strIndex[0]=%s %d\n",WaveformCob[i].strIndex[0],WaveformCob[i].Count);
					continue ;
				}
			if(i==1)//cascade
				tmpWaveform[WAVEFORM_CASCADE].bSwitch = 0;
			else
				tmpWaveform[WAVEFORM_CASCADE].bSwitch = 1;
			for(j=0;j<15;j++)//遍历公共数组得到可用列表
				{
					for(k=0;k<10;k++)//每个列表中当前选项在公共数组中打上flag
						{
							if(WaveformCob[k].bID !=SWITCH_OFF)
								{
								tmpWaveform[WaveformCob[k].bID].bSwitch=1;	
								WaveformCob[k].strIndex[0]=strWaveIndex[WaveformCob[k].bID];
								}
							else //OFF
								{
									WaveformCob[k].strIndex[0]=strWaveIndex[0];
								}
						}
					if(!tmpWaveform[j].bSwitch)
						{
							if(WaveformCob[i].strIndex[0]!=NULL&&WaveformCob[i].Count==0)
								if(WaveformCob[i].strIndex[0]!=strWaveIndex[0])
									WaveformCob[i].Count++;
							WaveformCob[i].strIndex[WaveformCob[i].Count]=strWaveIndex[j];
							WaveformCob[i].Count++;
						}
					//if(B_PRINTF) printf("WaveformCob[i].strIndex[WaveformCob[i].Count]=%s\n",WaveformCob[i].strIndex[WaveformCob[i].Count]);
				}
		//	for(m=0;m<WaveformCob[i].Count;m++)
		//		if(B_PRINTF) printf("WaveformCob[%d].strIndex[%d]=%s\n",i,m,WaveformCob[i].strIndex[m]);
//
			
		}
	return 0;
}

static int Initwaveform()
{
	int i;
	/*
	if(gbHaveIbp)
		{
			gCfgIbp1.bSwitch |=0x3;
			gCfgIbp2.bSwitch |=0x3;
		}
	else
		{
			gCfgIbp1.bSwitch |=0x0;//bit7=1 无模块 bit6=1 波形无
			gCfgIbp2.bSwitch |=0x0;
		}
	if(gbHaveCo2)
		gCfgCO2.bSwitch |=0x3;
	else
		gCfgCO2.bSwitch |=0x0;
		*/
	//初始化公共数组中的标志位
	for(i=0;i<15;i++)
	{
		tmpWaveform[i].bID=i;
		tmpWaveform[i].strWaveform=strWaveIndex[i];
	}
	
	tmpWaveform[WAVEFORM_OFF].bSwitch=0;//off
	tmpWaveform[WAVEFORM_ECG1].bSwitch=0;//ECG1
	if(!gCfgEcg.bCascade) tmpWaveform[WAVEFORM_CASCADE].bSwitch=0;
	///3导联时只显示ECG1 5导联时显示全部,通道1波形去掉
	if(gCfgEcg.bLeadType == ECGTYPE_3LEAD){
		for(i=0;i<7;i++){
			tmpWaveform[i+3].bSwitch=1;
		}
	
	}
	else{
		for(i=0;i<7;i++){
			tmpWaveform[i+3].bSwitch=0;
		}
		tmpWaveform[gCfgEcg.bChannel1+3].bSwitch=1;
	}
	//tmpWaveform[gCfgEcg.bChannel1+3].bSwitch=0;//ECG1显示的波形
	/*
	for(i=0;i<8;i++)
		{
		if(B_PRINTF) printf("gCfgEcg.bSwitch[i]=%x\n",gCfgEcg.bSwitch & 0x1<<(7-i));
			if(gCfgEcg.bSwitch & 1<<(7-i))//switch on bit7~bit1 I II III AVL AVR AVF V
				tmpWaveform[i+3].bSwitch=1;
			else
				tmpWaveform[i+3].bSwitch=0;
		}
		*/
		
	if(!gbHaveSpo2)
		tmpWaveform[WAVEFORM_SPO2].bSwitch=1;//SPO2
	else
		tmpWaveform[WAVEFORM_SPO2].bSwitch=0;
	if(!gbHaveResp)
		tmpWaveform[WAVEFORM_RESP].bSwitch=1;//RESP
	else
		tmpWaveform[WAVEFORM_RESP].bSwitch=0;
	if(!gbHaveIbp)
		tmpWaveform[WAVEFORM_IBP1].bSwitch=1;//IBP1
	else
		tmpWaveform[WAVEFORM_IBP1].bSwitch=0;
	if(!gbHaveIbp)
		tmpWaveform[WAVEFORM_IBP2].bSwitch=1;//IBP2
	else
		tmpWaveform[WAVEFORM_IBP2].bSwitch=0;
	if(!gbHaveCo2)
		tmpWaveform[WAVEFORM_CO2].bSwitch=1;//CO2
	else
		tmpWaveform[WAVEFORM_CO2].bSwitch=0;
	
	return 0;
}
/*
	初始化下拉列表框
*/
static int InitCobList()
{
	int i;
	unsigned char *strTemp[15]={0};
	
	//gCfgEcg.bSwitch |=0x0; 
	
	Initwaveform();
	
	
	//波形排序得到当前未选择的选项保存在strTemp中
	GetStrIndex();

	
	AddStrToCobList(cobwaveform0,WaveformCob[0].strIndex,WaveformCob[0].Count);
	AddStrToCobList(cobwaveform1,WaveformCob[1].strIndex,WaveformCob[1].Count);
	AddStrToCobList(cobwaveform2,WaveformCob[2].strIndex,WaveformCob[2].Count);
	AddStrToCobList(cobwaveform3,WaveformCob[3].strIndex,WaveformCob[3].Count);
	AddStrToCobList(cobwaveform4,WaveformCob[4].strIndex,WaveformCob[4].Count);
	AddStrToCobList(cobwaveform5,WaveformCob[5].strIndex,WaveformCob[5].Count);
	AddStrToCobList(cobwaveform6,WaveformCob[6].strIndex,WaveformCob[6].Count);
	AddStrToCobList(cobwaveform7,WaveformCob[7].strIndex,WaveformCob[7].Count);
	AddStrToCobList(cobwaveform8,WaveformCob[8].strIndex,WaveformCob[8].Count);
	AddStrToCobList(cobwaveform9,WaveformCob[9].strIndex,WaveformCob[9].Count);
				
	/*
	for(i=0;i<15;i++)
	{
		if(B_PRINTF) printf("1111tmpWaveform[%d].bSwitch=%d bID=%d\n",i,tmpWaveform[i].bSwitch,tmpWaveform[i].bID);
	}
	*/
	//填充到下拉列表中	
//	AddStrToCobList(hWnd,WaveformCob[i].strIndex,WaveformCob[i].Count);
	
	return 0;
}


//创建控件
static int CreateCtlProc(HWND hDlg)
{
	StaticHelp = GetDlgItem(hDlg, DID_WAVESET_HELP);
	StaticCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE, 
			     DID_WAVESET_CAPTION, 0,5, 250, 25, hDlg, 0);

	btnwaveform0= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON |BS_NOBORDER | BS_FLAT,
			DID_WAVESET_WAVEFORM0,  FIRSTBTN_X, FIRSTBTN_Y+BTN_HIGH*0, 60, BTN_HIGH-4, hDlg, 0);
	btnwaveform1= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON |BS_NOBORDER | BS_FLAT,
			DID_WAVESET_WAVEFORM1,  FIRSTBTN_X, FIRSTBTN_Y+BTN_HIGH*1, 60, BTN_HIGH-4,hDlg, 0);
	btnwaveform2= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON |BS_NOBORDER | BS_FLAT,
			DID_WAVESET_WAVEFORM2,  FIRSTBTN_X, FIRSTBTN_Y+BTN_HIGH*2, 60, BTN_HIGH-4, hDlg, 0);
	btnwaveform3= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON |BS_NOBORDER | BS_FLAT,
			DID_WAVESET_WAVEFORM3,  FIRSTBTN_X, FIRSTBTN_Y+BTN_HIGH*3, 60, BTN_HIGH-4,hDlg, 0);
	btnwaveform4= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON |BS_NOBORDER | BS_FLAT,
			DID_WAVESET_WAVEFORM4,  FIRSTBTN_X, FIRSTBTN_Y+BTN_HIGH*4, 60, BTN_HIGH-4, hDlg, 0);

	btnwaveform5= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON |BS_NOBORDER | BS_FLAT,
			DID_WAVESET_WAVEFORM5,  FIRSTBTN_X+130, FIRSTBTN_Y+BTN_HIGH*0, 60, BTN_HIGH-4,hDlg, 0);
	btnwaveform6= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON |BS_NOBORDER | BS_FLAT,
			DID_WAVESET_WAVEFORM6,  FIRSTBTN_X+130, FIRSTBTN_Y+BTN_HIGH*1, 60, BTN_HIGH-4,hDlg, 0);
	btnwaveform7= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON |BS_NOBORDER | BS_FLAT,
			DID_WAVESET_WAVEFORM7,  FIRSTBTN_X+130, FIRSTBTN_Y+BTN_HIGH*2, 60, BTN_HIGH-4, hDlg, 0);
	btnwaveform8= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON |BS_NOBORDER | BS_FLAT,
			DID_WAVESET_WAVEFORM8,  FIRSTBTN_X+130, FIRSTBTN_Y+BTN_HIGH*3, 60, BTN_HIGH-4, hDlg, 0);
	btnwaveform9= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON |BS_NOBORDER | BS_FLAT,
			DID_WAVESET_WAVEFORM9, FIRSTBTN_X+130, FIRSTBTN_Y+BTN_HIGH*4, 60, BTN_HIGH-4, hDlg, 0);
	btnOk = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
			     DID_WAVESET_OK, FIRSTBTN_X, FIRSTBTN_Y+BTN_HIGH*5, 60, BTN_HIGH-4, hDlg, 0);
	
	cobwaveform0 = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
			DID_WAVESET_WAVEFORM0_COB,  FIRSTBTN_X+65, FIRSTBTN_Y+BTN_HIGH*0, 60, 26, hDlg, 0);
	cobwaveform1 = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
			DID_WAVESET_WAVEFORM1_COB, FIRSTBTN_X+65, FIRSTBTN_Y+BTN_HIGH*1, 60, 26, hDlg, 0);
	cobwaveform2 = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
			DID_WAVESET_WAVEFORM2_COB, FIRSTBTN_X+65, FIRSTBTN_Y+BTN_HIGH*2, 60, 26, hDlg, 0);
	cobwaveform3 = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
			DID_WAVESET_WAVEFORM3_COB,  FIRSTBTN_X+65, FIRSTBTN_Y+BTN_HIGH*3, 60, 26, hDlg, 0);
	cobwaveform4 = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
			DID_WAVESET_WAVEFORM4_COB,  FIRSTBTN_X+65, FIRSTBTN_Y+BTN_HIGH*4, 60, 26, hDlg, 0);

	cobwaveform5 = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
			DID_WAVESET_WAVEFORM5_COB,  FIRSTBTN_X+65+130, FIRSTBTN_Y+BTN_HIGH*0, 60, 26, hDlg, 0);
	cobwaveform6 = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
			DID_WAVESET_WAVEFORM6_COB,  FIRSTBTN_X+65+130, FIRSTBTN_Y+BTN_HIGH*1, 60, 26, hDlg, 0);
	cobwaveform7 = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
			DID_WAVESET_WAVEFORM7_COB,  FIRSTBTN_X+65+130, FIRSTBTN_Y+BTN_HIGH*2, 60, 26,hDlg, 0);
	cobwaveform8 = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
			DID_WAVESET_WAVEFORM8_COB, FIRSTBTN_X+65+130, FIRSTBTN_Y+BTN_HIGH*3, 60, 26, hDlg, 0);
	cobwaveform9 = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,  
			DID_WAVESET_WAVEFORM9_COB, FIRSTBTN_X+65+130, FIRSTBTN_Y+BTN_HIGH*4, 60, 26, hDlg, 0);
	
		
	return 0;
}
//设置控件字符
static int InitCtlName(HWND hDlg)
{
	char strMenu[100];
	//caption
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_WAVEFORM, "caption", strMenu, sizeof strMenu,"Waveform Setup");	
	SetWindowText(StaticCaption,strMenu);
	//waveform 0
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_WAVEFORM, "waveform_0", strMenu, sizeof strMenu,"Wave 1");	
	SetWindowText(btnwaveform0,strMenu);
	//waveform 1
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_WAVEFORM, "waveform_1", strMenu, sizeof strMenu,"Wave 2");	
	SetWindowText(btnwaveform1,strMenu);
	//waveform 2
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_WAVEFORM, "waveform_2", strMenu, sizeof strMenu,"Wave 3");	
	SetWindowText(btnwaveform2,strMenu);
	//waveform 3
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_WAVEFORM, "waveform_3", strMenu, sizeof strMenu,"Wave 4");	
	SetWindowText(btnwaveform3,strMenu);
	//waveform 4
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_WAVEFORM, "waveform_4", strMenu, sizeof strMenu,"Wave 5");	
	SetWindowText(btnwaveform4,strMenu);
	//waveform 5
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_WAVEFORM, "waveform_5", strMenu, sizeof strMenu,"Wave 6");	
	SetWindowText(btnwaveform5,strMenu);
	//waveform 6
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_WAVEFORM, "waveform_6", strMenu, sizeof strMenu,"Wave 7");	
	SetWindowText(btnwaveform6,strMenu);
	//waveform 7
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_WAVEFORM, "waveform_7", strMenu, sizeof strMenu,"Wave 8");	
	SetWindowText(btnwaveform7,strMenu);
	//waveform 8
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_WAVEFORM, "waveform_8", strMenu, sizeof strMenu,"Wave 9");	
	SetWindowText(btnwaveform8,strMenu);
	//waveform 9
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_WAVEFORM, "waveform_9", strMenu, sizeof strMenu,"Wave 10");	
	SetWindowText(btnwaveform9,strMenu);
	//Exit
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "previous", strMenu, sizeof strMenu,"Previous");	
	SetWindowText(btnOk,strMenu);	
	return 0;
}

//设置控件回调函数
static int InitCtlCallBackProc(HWND hDlg)
{
		OldBtnProc = SetWindowCallbackProc(btnwaveform0, BtnProc);
		OldBtnProc = SetWindowCallbackProc(btnwaveform1, BtnProc);
		OldBtnProc = SetWindowCallbackProc(btnwaveform2, BtnProc);
		OldBtnProc = SetWindowCallbackProc(btnwaveform3, BtnProc);
		OldBtnProc = SetWindowCallbackProc(btnwaveform4, BtnProc);
		OldBtnProc = SetWindowCallbackProc(btnwaveform5, BtnProc);
		OldBtnProc = SetWindowCallbackProc(btnwaveform6, BtnProc);
		OldBtnProc = SetWindowCallbackProc(btnwaveform7, BtnProc);
		OldBtnProc = SetWindowCallbackProc(btnwaveform8, BtnProc);
		OldBtnProc = SetWindowCallbackProc(btnwaveform9, BtnProc);
		OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);
		
		OldCobProc = SetWindowCallbackProc(cobwaveform0, CobProc);
		OldCobProc = SetWindowCallbackProc(cobwaveform1, CobProc);
		OldCobProc = SetWindowCallbackProc(cobwaveform2, CobProc);
		OldCobProc = SetWindowCallbackProc(cobwaveform3, CobProc);
		OldCobProc = SetWindowCallbackProc(cobwaveform4, CobProc);
		OldCobProc = SetWindowCallbackProc(cobwaveform5, CobProc);
		OldCobProc = SetWindowCallbackProc(cobwaveform6, CobProc);
		OldCobProc = SetWindowCallbackProc(cobwaveform7, CobProc);
		OldCobProc = SetWindowCallbackProc(cobwaveform8, CobProc);
		OldCobProc = SetWindowCallbackProc(cobwaveform9, CobProc);
		OldStaticProcHelp  = SetWindowCallbackProc(StaticHelp, StaticProcHelp); 
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
			
			gbKeyType = KEY_TAB;
			SetFocus(btnOk);
		}break;
		case MSG_PAINT:{
			

		}break;
		case MSG_COMMAND:{
			int	id  = LOWORD(wParam);
			switch(id){
				case DID_WAVESET_WAVEFORM0:{
					gbKeyType = KEY_UD;
					SetFocus(cobwaveform0);
					}break;
				case DID_WAVESET_WAVEFORM1:{
					gbKeyType = KEY_UD;
					//GetStrIndex();
					SendMessage(cobwaveform1, CB_RESETCONTENT,0, 0);
					AddStrToCobList(cobwaveform1,WaveformCob[1].strIndex,WaveformCob[1].Count);
					
					SetFocus(cobwaveform1);
					}break;
				case DID_WAVESET_WAVEFORM2:{
					gbKeyType = KEY_UD;
					//GetStrIndex();
					SendMessage(cobwaveform2, CB_RESETCONTENT,0, 0);
					AddStrToCobList(cobwaveform2,WaveformCob[2].strIndex,WaveformCob[2].Count);
					
					SetFocus(cobwaveform2);
					}break;
				case DID_WAVESET_WAVEFORM3:{
					gbKeyType = KEY_UD;
					
					SendMessage(cobwaveform3, CB_RESETCONTENT,0, 0);
					AddStrToCobList(cobwaveform3,WaveformCob[3].strIndex,WaveformCob[3].Count);
					
					SetFocus(cobwaveform3);
					}break;
				case DID_WAVESET_WAVEFORM4:{
					gbKeyType = KEY_UD;
					SendMessage(cobwaveform4, CB_RESETCONTENT,0, 0);
					AddStrToCobList(cobwaveform4,WaveformCob[4].strIndex,WaveformCob[4].Count);
					
					SetFocus(cobwaveform4);
					}break;
				case DID_WAVESET_WAVEFORM5:{
					gbKeyType = KEY_UD;
					SendMessage(cobwaveform5, CB_RESETCONTENT,0, 0);
					AddStrToCobList(cobwaveform5,WaveformCob[5].strIndex,WaveformCob[5].Count);
					
					SetFocus(cobwaveform5);
					}break;
				case DID_WAVESET_WAVEFORM6:{
					gbKeyType = KEY_UD;
					SendMessage(cobwaveform6, CB_RESETCONTENT,0, 0);
					AddStrToCobList(cobwaveform6,WaveformCob[6].strIndex,WaveformCob[6].Count);
					
					SetFocus(cobwaveform6);
					}break;
				case DID_WAVESET_WAVEFORM7:{
					gbKeyType = KEY_UD;
					SendMessage(cobwaveform7, CB_RESETCONTENT,0, 0);
					AddStrToCobList(cobwaveform7,WaveformCob[7].strIndex,WaveformCob[7].Count);
					
					SetFocus(cobwaveform7);
					}break;
				case DID_WAVESET_WAVEFORM8:{
					gbKeyType = KEY_UD;
					SendMessage(cobwaveform8, CB_RESETCONTENT,0, 0);
					AddStrToCobList(cobwaveform8,WaveformCob[8].strIndex,WaveformCob[8].Count);
					
					SetFocus(cobwaveform8);
					}break;
				case DID_WAVESET_WAVEFORM9:{
					gbKeyType = KEY_UD;
					SendMessage(cobwaveform9, CB_RESETCONTENT,0, 0);
					AddStrToCobList(cobwaveform9,WaveformCob[9].strIndex,WaveformCob[9].Count);
					
					SetFocus(cobwaveform9);
					}break;
				case DID_WAVESET_OK:{
					//把当前各列表值写入文件保存
					SaveUsrWaveSet();
					//根据选项更新波形
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
			
			
			//退出所有菜单
		//	ReturnMainView();
		}break;
	}
	
	temp = DefaultDialogProc(hDlg, message, wParam, lParam);
	return(temp);
}


/*
	建立对话框
*/
void CreateWaveformSet(HWND hWnd)
{
	
 	DlgSet.controls = CtrlSet;
 	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}


/*************************************************************************** 
 *	Module Name:	GUI_Main
 *
 *	Abstract:	GUI主程序
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2007-06-09 13:50:18
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"
#include "NewDrawWave.h"

//主窗口句柄
HWND ghWndMain;

//按钮
#define BTN_MAIN			1000
#define BTN_SET1		BTN_MAIN+1
#define BTN_SET2		BTN_MAIN+2
#define BTN_SET3		BTN_MAIN+3
#define BTN_SET4		BTN_MAIN+4
#define BTN_SET5		BTN_MAIN+5
#define BTN_SET6		BTN_MAIN+6
#define BTN_SET7		BTN_MAIN+7
#define BTN_SET8		BTN_MAIN+8
#define BTN_BEDNO		BTN_MAIN+9
#define BTN_WAVE		BTN_MAIN+10
#define COB_BEDNO		BTN_MAIN+11
#define COB_WAVE		BTN_MAIN+12


//菜单起始按钮
#define BTN_HEAD		BTN_MAIN
//菜单结束按钮
#define BTN_TAIL		BTN_WAVE

static HWND btnPatient = (HWND)NULL;
static HWND btnSystem = (HWND)NULL;
static HWND btnAlarm = (HWND)NULL;
static HWND btnTrend = (HWND)NULL;
static HWND btnView = (HWND)NULL;

#define PARA_SET			0
#define PARA_SET_ECG		PARA_SET+1
#define PARA_SET_NIBP		PARA_SET+2
#define PARA_SET_SPO2		PARA_SET+3
#define PARA_SET_RESP		PARA_SET+4
#define PARA_SET_TEMP	PARA_SET+5
#define PARA_SET_IBP1		PARA_SET+6
#define PARA_SET_IBP2		PARA_SET+7
#define PARA_SET_CO2		PARA_SET+8
//参数设置按钮数量
#define PARA_SET_COUNT		11

//参数设置按钮
typedef struct {
	BYTE bID;			//PARA_SET_XXX		
	HWND hWnd;
}PARASET, *PPARASET;

//参数设置按钮
static PARASET btnParaSet[PARA_SET_COUNT];

static WNDPROC OldBtnProc;
static WNDPROC  OldCobProc;

//主窗口定时器
#define	_ID_TIMER_MAIN	106
static BOOL bCreateMainTimerOK = FALSE;

//存储报警数据的互斥量 TODO:临时在此初始化
extern pthread_mutex_t mtSaveAlarmData;          

//控制是否可以进行绘制的变量
extern BOOL gbNCanDrawWave;

extern  RECT gRCEcg;
extern  RECT gRCNibp;
extern  RECT gRCSpO2AndPR;
extern  RECT gRCResp;
extern  RECT gRCTemp;
extern  RECT gRCCo2;
extern  RECT gRCIbp1;
extern  RECT gRCIbp2;
extern  RECT gRCIbp2;

//调试看门狗
static BOOL gbFeedDog = TRUE;

//////////// Extend Panel ////////////////

#define PANEL_SHORTTREND		PANEL_EXTEND
HWND ghWndPanel_ShortTrend = (HWND)NULL;
static WNDPROC OldPanelProc_ShortTrend;

#define PANEL_OXYCRG			PANEL_EXTEND+1
HWND ghWndPanel_Crg= (HWND)NULL;
static WNDPROC OldPanelProc_Crg;

#define PANEL_OTHERBED			PANEL_EXTEND+2
HWND ghWndPanel_OtherBed= (HWND)NULL;
static WNDPROC OldPanelProc_OtherBed;

#define PANEL_HEART			PANEL_EXTEND+3
HWND ghWndPanel_Heart = (HWND)NULL;

static int iCount=0;
//调试电源关闭
BOOL gbTestPowerDown = FALSE;



////////////////////////////
//是否存储ECG数据
extern BOOL gbSaveRealEcgData;
//是否已经存储足够的ECG数据
extern BOOL gbSaveRealEcgDataOK;

//是否进入报警暂停状态
extern BOOL gbSuspending;
//报警暂停状态倒计时
extern int giSuspendTime;

//是否静音
extern BOOL gbSilence;

 HWND btnBedNO;
 HWND btnWave;
 HWND cobBedNO;
 HWND cobWave;
 BYTE gbOtherBedWaveIndex=0;
 //波形复位控制
extern BOOL gbOtherBedResetWaves;
//保存波形数据到文件开关
 BOOL gbSaveWaveValuetofile=FALSE;
//冻结波形
extern BOOL gbFreezeKey;
//趋势图标
int gbTrendStatus=0;

extern HWND hWnd_NibpTable;
extern HWND hWnd_TrendGraph;
extern HWND hWnd_TrendTable;

extern NWAVE_PANEL gNWavePanelsOtherBed;

S_OB_INFO ob_bedlist[MAX_OB_IP];
extern BYTE gbOtherBedIndex;
extern char gsOB_IP[16];
extern BOOL gbChangeBedIndex;
extern BOOL gbOB_Status;
extern int giOBMacNO;
////////////////////////////
extern CA_TRENDARRAY gTrendDatas;
extern BOOL bShow_Poweroff;
extern BOOL AUTO_Shutdown;
extern int gfdWdt;
extern BYTE gOB_RecvIPNum;
extern BOOL Recevie_Status;
int UpdateIbp()
{
	
	if(btnParaSet[5].hWnd!= (HWND)NULL){	
		UpdateWindow(btnParaSet[5].hWnd,TRUE);
	}
	if(btnParaSet[6].hWnd!= (HWND)NULL){	
		UpdateWindow(btnParaSet[6].hWnd,TRUE);
	}
	return 0;
}
/*
	扩展面板回调函数
*/
static int PanelProc_ShortTrend(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	
	temp = (*OldPanelProc_ShortTrend)(hWnd, message, wParam, lParam);
	switch(message){
		case MSG_PAINT:{
			//更新短趋势
			if(gCfgSystem.bInterface == SCREEN_SHORTTREND)
				NewUpdateShortTrend();
			else
				RestorWave();
			return 0;
		}break;
		default: break;
	}                                                                                                                  
	return(temp);
}

static int PanelProc_Crg(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	
 	temp = (*OldPanelProc_Crg)(hWnd, message, wParam, lParam);
	switch(message){
		case MSG_PAINT:{
			break;				//2010-01-11 08:46:28  防止oxyCRG刷新错误
			//更新oxyCRG
			if(gCfgSystem.bInterface == SCREEN_OXYCRG)
				NewUpdateCrg();
			else
				RestorWave();
			return 0;
		}break;
		default: break;
	}                                                                                                                  
	return(temp);
}

static int PanelProc_OtherBed(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	
 	temp = (*OldPanelProc_OtherBed)(hWnd, message, wParam, lParam);
	switch(message){
		case MSG_PAINT:{
			//更新oxyCRG
			if(gCfgSystem.bInterface == SCREEN_OTHERBED)
				NewUpdateOtherBed();
			else
				RestorWave();
			return 0;
		}break;
		default: break;
	}                                                                                                                  
	return(temp);
}
	
/*
	按钮消息回调函数
*/
static int BtnProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	
	temp = (*OldBtnProc)(hWnd, message, wParam, lParam);
		
	switch(message){
 		case MSG_SETFOCUS:{
 			SetWindowBkColor(hWnd, SETFOCUS_COLOR);
 		}break;
 		case MSG_KILLFOCUS:{
 			SetWindowBkColor(hWnd, KILLFOCUS_COLOR);
 		}break;
		default: break;
	}                                                                                                                  

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
	static int iOldbedindex=0;
	static unsigned char oldip[16];
	RECT rc;
	HDC hdc= (HDC)NULL;
	GetWindowRect(hWnd, &rc);	
	switch(message){
		case MSG_SETFOCUS:{
			
			SendMessage(hWnd, CBN_DROPDOWN, 1, 0);

			/*
			hdc = GetClientDC(GetParent(hWnd));
			
			if(hdc != (HDC)NULL){
				SetPenColor(hdc, SETFOCUS_COLOR);
				Rectangle(hdc, rc.left-1, rc.top-1, rc.right+1, rc.bottom+1);
				Rectangle(hdc, rc.left-2, rc.top-2, rc.right+2, rc.bottom+2);
				ReleaseDC(hdc);
			}
			*/
		}break;
		case MSG_KILLFOCUS:{
			/*
			hdc = GetClientDC(GetParent(hWnd));
			if(hdc !=(HDC)NULL){
				SetPenColor(hdc, KILLFOCUS_COLOR);
				Rectangle(hdc, rc.left-1, rc.top-1, rc.right+1, rc.bottom+1);
				Rectangle(hdc, rc.left-2, rc.top-2, rc.right+2, rc.bottom+2);
				ReleaseDC(hdc);
			}
			*/
		}break;
	case MSG_COMMAND:{
		case MSG_KEYUP:{
			if(wParam==SCANCODE_ENTER||CBN_EDITCHANGE==HIWORD(wParam)){
					id = GetDlgCtrlID(hWnd);
					index = SendMessage(hWnd, CB_GETCURSEL, 0, 0);
						
					switch(id){
						case COB_BEDNO:{
							if(B_PRINTF)printf("********************index=%d,gbOtherBedIndex=%s\n",index,oldip);

							if(index!=gbOtherBedIndex){
								gbOtherBedIndex=index;
								if(B_PRINTF)printf("11111111111111111gbOtherBedIndex=%d,iOldbedindex=%d\n",gbOtherBedIndex,iOldbedindex);
								//如果为no bed 则复位
								if(gbOtherBedIndex==0){
									gOB_RecvIPNum=0;
								//	OB_NetSend_STOPConnect(ob_bedlist[iOldbedindex].IPaddr);
									OB_NetSend_STOPConnect(oldip);
									OB_ResetDraw();
								}else{
									if(B_PRINTF)printf("222gbOtherBedIndex=%d,gsOB_IP=%s\n",gbOtherBedIndex,ob_bedlist[gbOtherBedIndex].IPaddr);
									OB_ResetDraw();
									
									if(iOldbedindex!=0)
										OB_NetSend_STOPConnect(oldip);
									
							 		//发送数据连接请求
							 			OB_NetSend_Ask_Pack(ob_bedlist[gbOtherBedIndex].IPaddr);
									
								}
								iOldbedindex=gbOtherBedIndex;
								strcpy(oldip,ob_bedlist[gbOtherBedIndex].IPaddr);
							}
							SetFocus(btnBedNO);
						}break;
						case COB_WAVE:{
							if(index!=gbOtherBedWaveIndex){
								gbOtherBedWaveIndex=index;
								gbOtherBedResetWaves=TRUE;
									if(gbOtherBedWaveIndex<7) {
										gNWavePanelsOtherBed.bWaveID=NWID_ECG1;
									}else if(gbOtherBedWaveIndex==7){
										gNWavePanelsOtherBed.bWaveID=NWID_SPO2;
									}else{
										gNWavePanelsOtherBed.bWaveID=NWID_RESP;
									}
									gNWavePanelsOtherBed.bWaveCount=1;
									
									SetWavePanelXY_OB(&gNWavePanelsOtherBed);
									
							}
							SetFocus(btnWave);
						}break;
					}	
					gbKeyType = KEY_LR;
				}break;
			}
			return 0;
		}break;	
	}	
	temp = (*OldCobProc)(hWnd, message, wParam, lParam);
	return(temp);
}

/*
	设置按钮文字(单独分出来，是为了切换语言时使用)
*/
int SetMainBtnText()
{
	char strMenu[100]={0};
	
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "MAIN", "patient", strMenu, sizeof strMenu);	
	if(btnPatient != (HWND)NULL) SetWindowText(btnPatient, strMenu);
	
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "MAIN", "system", strMenu, sizeof strMenu);	
	if(btnSystem!= (HWND)NULL) SetWindowText(btnSystem, strMenu);
	
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "MAIN", "alarm", strMenu, sizeof strMenu);	
	if(btnAlarm!= (HWND)NULL) SetWindowText(btnAlarm, strMenu);
	
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "MAIN", "trend", strMenu, sizeof strMenu);	
	if(btnTrend!= (HWND)NULL) SetWindowText(btnTrend, strMenu);
	
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFile(gsLanguageRes, "MAIN", "view", strMenu, sizeof strMenu);	
	if(btnView!= (HWND)NULL) SetWindowText(btnView, strMenu);
	
	return 0;
}


/*
	设置竖向显示的文字
*/
static int SetVText(HWND hWnd, char *strText)
{
	char strLabel[50] = {0};
	int iLen;
	int i, j;
	
	if(hWnd == (HWND)NULL) return -1;
	
	iLen = strlen(strText);
	
 	//printf("%s %d Text is %s. length is %d.\n", __FILE__, __LINE__, strText, iLen);
	
	if(iLen > 25) iLen = 25;

#if SCREEN_1024
	for(i=0, j=0; j<iLen; i+=2, j++){
		strLabel[i] = '\n';
		strLabel[i+1] = '\n';
		strLabel[i+2] = strText[j];
		strLabel[i+3] = '\n';
	}
#elif	SCREEN_640
	for(i=0, j=0; j<iLen; i+=2, j++){
		strLabel[i] = strText[j];
		strLabel[i+1] = '\n';
	}
#else
	for(i=0, j=0; j<iLen; i+=2, j++){
		strLabel[i] = '\n';
		strLabel[i+1] = strText[j];
		strLabel[i+2] = '\n';
	}
#endif

//	printf("%s %d Change Text is %s. length is %d.\n", __FILE__, __LINE__, strLabel, strlen(strLabel));
	
	SetWindowText(hWnd, strLabel);
//	SetWindowText(hWnd, "E\n");
			
	return 0;
}

int SetParaBtnText()
{
	int i;
	char strMenu[100]={0};
	
	for(i=0; i<PARA_SET_COUNT; i++){
		if(btnParaSet[i].hWnd != (HWND)NULL){
			memset(strMenu, 0, sizeof strMenu);
			switch(btnParaSet[i].bID ) {
				case PARA_SET_ECG: {
					GetStringFromResFiles(gsLanguageRes, "MAIN", "ecg", strMenu, sizeof strMenu,"ECG");	
				}break;	
				case PARA_SET_SPO2: {
					GetStringFromResFiles(gsLanguageRes, "MAIN", "spo2", strMenu, sizeof strMenu,"SPO2");	
				}break;	
				case PARA_SET_NIBP: {
					GetStringFromResFiles(gsLanguageRes, "MAIN", "nibp", strMenu, sizeof strMenu,"NIBP");	
				}break;	
				case PARA_SET_RESP: {
					GetStringFromResFiles(gsLanguageRes, "MAIN", "resp", strMenu, sizeof strMenu,"RESP");	
				}break;	
				case PARA_SET_TEMP: {
					GetStringFromResFiles(gsLanguageRes, "MAIN", "temp", strMenu, sizeof strMenu,"TEMP");	
				}break;	
				case PARA_SET_IBP1: {
					GetStringFromResFiles(gsLanguageRes, "MAIN", "ibp1", strMenu, sizeof strMenu,"IBP1");	
				}break;	
				case PARA_SET_IBP2: {
					GetStringFromResFiles(gsLanguageRes, "MAIN", "ibp2", strMenu, sizeof strMenu,"IBP2");	
				}break;	
				case PARA_SET_CO2: {
					GetStringFromResFiles(gsLanguageRes, "MAIN", "co2", strMenu, sizeof strMenu,"CO2");	
				}break;	
			}
			SetVText(btnParaSet[i].hWnd, strMenu);
		}
	}
	

	return 0;
}
unsigned int GetOwnIPAddr(unsigned char *IP)
{
	//Get Net Info
	unsigned char tmpstr[16];
	unsigned char *ptmp;
	unsigned int ivalue;
	
	//IP
	strncpy(tmpstr, IP, 16);
	ptmp = (unsigned char *)strtok(tmpstr, ".");
	ptmp = (unsigned char *)strtok(NULL, ".");
	ptmp = (unsigned char *)strtok(NULL, ".");
	ptmp = (unsigned char *)strtok(NULL, ".");
	ivalue = strtol(ptmp, NULL, 10);
	
	return ivalue;
}


int SetOB_CobList(BOOL Init)
{
	int i;
	char bedno[5]={0};
	static int oldBedCount=0;
	static int oldbedno[MAX_OB_IP]={0};
	char getchar[10];
//	static int oldbedmac[MAX_OB_IP]={0};
	static BOOL oldbedflag[MAX_OB_IP];
	int BedCount=1;
	BOOL change=FALSE;
	unsigned int localIP,iOldIndexIP,iNewCurrIndex;
 
	localIP=GetOwnIPAddr(gCfgNet.cIP);
	iOldIndexIP=ob_bedlist[gbOtherBedIndex].MacNo;
	
	for(i=0;i<MAX_OB_IP;i++){
		
		if(OB_Info[i].FLAG){
			//TODO
			//是否为本机ip，是则跳过，
			if(localIP==OB_Info[i].MacNo) continue;
			
			//连接状态是否为忙且已经和本机连接，是则跳过，
			if(OB_Info[i].Status!=0&&OB_Info[i].Status!=localIP) continue;
			//保存该网段空闲IP地址到数组
			ob_bedlist[BedCount].BedNo=OB_Info[i].BedNo;
			ob_bedlist[BedCount].MacNo=OB_Info[i].MacNo;
			ob_bedlist[BedCount].Status=OB_Info[i].Status;
			 strcpy(ob_bedlist[BedCount].IPaddr,OB_Info[i].IPaddr);
			 
		//	if(B_PRINTF)printf("localIP=%d,bed count:%d,BedNo:%d,Status:%d,ipaddr:%s\n",localIP,BedCount,ob_bedlist[BedCount].BedNo,ob_bedlist[BedCount].Status,ob_bedlist[BedCount].IPaddr);

			// if(i==iOldIndexIP) iNewCurrIndex=BedCount;
			//ip没改，修改了床号，需要更新列表
			if(OB_Info[i].BedNo!=oldbedno[i])  change=TRUE;
			oldbedno[i]=OB_Info[i].BedNo;

			BedCount++;
			
		}

		

	}
	//如果ip数量有变化、修改了床号、初始化需要重置列表
	if(oldBedCount!=BedCount ||change||Init){

		//如果有新的ip在当前选择之前，那么重新设置列表焦点项
		
		
		
		 SendMessage(cobBedNO, CB_RESETCONTENT, 0, 0);
		 SendMessage(cobBedNO, CB_ADDSTRING, 0, (LPARAM)"NO BED");
		for(i=1;i<BedCount;i++){

			 if(ob_bedlist[i].MacNo==gOB_RecvIPNum) {
			 	gbOtherBedIndex=i;
			 }
			
			snprintf(bedno,sizeof bedno,"%d",ob_bedlist[i].BedNo);
			SendMessage(cobBedNO, CB_ADDSTRING, 0, (LPARAM)bedno);
		}
		
	//	SendMessage(cobBedNO, CB_SETCURSEL,iNewCurrIndex, 0);
	SendMessage(cobBedNO, CB_SETCURSEL,gbOtherBedIndex, 0);
		oldBedCount=BedCount;
	}
	
	return 0;
}
/*
	标准主界面的按钮顺序:
	Ecg->nibp->spo2->resp->temp
*/
static int CreateNoIBPBtn(HWND hWnd)
{

//ecg
	btnParaSet[0].hWnd = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON  | BS_MULTLINE,
			BTN_SET1,PARAVIEW_LEFT-20,PARAVIEW_TOP+gRCEcg.top, 20,RECTH(gRCEcg) , hWnd, 0);
//nibp
	 btnParaSet[1].hWnd = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_MULTLINE,
			BTN_SET2, PARAVIEW_LEFT-20,PARAVIEW_TOP+gRCNibp.top, 20,RECTH(gRCNibp) , hWnd, 0);
//spo2
	btnParaSet[2].hWnd = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_MULTLINE,
			BTN_SET3, PARAVIEW_LEFT-20,PARAVIEW_TOP+gRCSpO2AndPR.top, 20,RECTH(gRCSpO2AndPR) , hWnd, 0);
//resp
	btnParaSet[3].hWnd = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_MULTLINE,
			BTN_SET4, PARAVIEW_LEFT-20,PARAVIEW_TOP+gRCResp.top, 20,RECTH(gRCResp) ,hWnd, 0);
//temp
	btnParaSet[4].hWnd = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_MULTLINE,
			BTN_SET5, PARAVIEW_LEFT-20,PARAVIEW_TOP+gRCTemp.top, 20,RECTH(gRCTemp) , hWnd, 0);		

	btnParaSet[0].bID = PARA_SET_ECG;
	btnParaSet[1].bID = PARA_SET_NIBP;
	btnParaSet[2].bID = PARA_SET_SPO2;
	if(gbHaveCo2){
		btnParaSet[3].bID = PARA_SET_CO2;
	}
	else{
		btnParaSet[3].bID = PARA_SET_RESP;	
	}
	btnParaSet[4].bID = PARA_SET_TEMP;

	OldBtnProc = SetWindowCallbackProc(btnParaSet[0].hWnd, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnParaSet[1].hWnd, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnParaSet[2].hWnd, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnParaSet[3].hWnd, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnParaSet[4].hWnd, BtnProc);
	
	return 0;
}
/*
	大字体界面时主界面的按钮顺序:
	Ecg->spo2->resp->temp->nibp
*/
static int CreateBigCharMenuBtn(HWND hWnd)
{
		//ecg
		btnParaSet[0].hWnd = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON  | BS_MULTLINE,
				BTN_SET1,PARAVIEW_LEFT-20,PARAVIEW_TOP+gRCEcg.top, 20,RECTH(gRCEcg)+1 , hWnd, 0);
		//spo2
		btnParaSet[1].hWnd = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_MULTLINE,
				BTN_SET2, PARAVIEW_LEFT-20,PARAVIEW_TOP+gRCSpO2AndPR.top, 20,RECTH(gRCSpO2AndPR) +1, hWnd, 0);
		//resp
		btnParaSet[2].hWnd = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_MULTLINE,
 				BTN_SET3, PARAVIEW_LEFT-20,PARAVIEW_TOP+gRCResp.top, 20,RECTH(gRCResp) +1,hWnd, 0);
		//temp
		btnParaSet[3].hWnd = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_MULTLINE,
 				BTN_SET4, PARAVIEW_LEFT-20,PARAVIEW_TOP+gRCTemp.top, 20,RECTH(gRCTemp)+1 , hWnd, 0);		
		//nibp
		btnParaSet[4].hWnd = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_MULTLINE,
				BTN_SET5, SCREEN_LEFT+1,RC_BIGCHAR_BOTTOM+5, 20,RECTH(gRCNibp), hWnd, 0);

		
		btnParaSet[0].bID = PARA_SET_ECG;
		btnParaSet[1].bID = PARA_SET_SPO2;
		if(gbHaveCo2){
			btnParaSet[2].bID = PARA_SET_CO2;
		}
		else{
			btnParaSet[2].bID = PARA_SET_RESP;	
		}
		btnParaSet[3].bID = PARA_SET_TEMP;
		btnParaSet[4].bID = PARA_SET_NIBP;
		
		OldBtnProc = SetWindowCallbackProc(btnParaSet[0].hWnd, BtnProc);
		OldBtnProc = SetWindowCallbackProc(btnParaSet[1].hWnd, BtnProc);
		OldBtnProc = SetWindowCallbackProc(btnParaSet[2].hWnd, BtnProc);
		OldBtnProc = SetWindowCallbackProc(btnParaSet[3].hWnd, BtnProc);
		OldBtnProc = SetWindowCallbackProc(btnParaSet[4].hWnd, BtnProc);
	return 0;
}
/*
	他床观察界面的按钮顺序:
	Ecg->nibp->spo2->resp->temp->bedno->wave
*/
static int CreateOtherBedBtn(HWND hWnd)
{
	int i;
	static unsigned char* strWave5Lead[] = {"ECG  I", "ECG  II", "ECG  III", "ECG  AVR", "ECG  AVL", "ECG  AVF", "ECG  V1", "SPO2", "RESP"};
	static unsigned char* strWave3Lead[] = {"ECG  I", "ECG  II", "ECG  III", "SPO2", "RESP"};
	
//ecg
	btnParaSet[0].hWnd = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON  | BS_MULTLINE,
			BTN_SET1,PARAVIEW_LEFT-20,PARAVIEW_TOP+gRCEcg.top, 20,RECTH(gRCEcg)+1 , hWnd, 0);
//nibp
	 btnParaSet[1].hWnd = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_MULTLINE,
			BTN_SET2, PARAVIEW_LEFT-20,PARAVIEW_TOP+gRCNibp.top, 20,RECTH(gRCNibp)+1 , hWnd, 0);
//spo2
	btnParaSet[2].hWnd = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_MULTLINE,
			BTN_SET3, PARAVIEW_LEFT-20,PARAVIEW_TOP+gRCSpO2AndPR.top, 20,RECTH(gRCSpO2AndPR)+1 , hWnd, 0);
//resp
	btnParaSet[3].hWnd = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_MULTLINE,
			BTN_SET4, PARAVIEW_LEFT-20,PARAVIEW_TOP+gRCResp.top, 20,RECTH(gRCResp)+1 ,hWnd, 0);
//temp
	btnParaSet[4].hWnd = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_MULTLINE,
			BTN_SET5, PARAVIEW_LEFT-20,PARAVIEW_TOP+gRCTemp.top, 20,RECTH(gRCTemp)+1 , hWnd, 0);		

	btnBedNO = CreateWindow("button",  "Bed No", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					BTN_BEDNO, RC_OTHERBED_LEFT+1,  RC_OTHERBED_BOTTOM+20, 100, 26, hWnd, 0);
	btnWave = CreateWindow("button",  "Wave", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					BTN_WAVE, RC_OTHERBED_LEFT+205, RC_OTHERBED_BOTTOM+20, 100, 26, hWnd, 0);
	cobBedNO = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,
					     COB_BEDNO, RC_OTHERBED_LEFT+105, RC_OTHERBED_BOTTOM+20, 95, 26, hWnd, 0);
	cobWave = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE |  CBS_DROPDOWNLIST|CBS_READONLY,
					COB_WAVE, RC_OTHERBED_LEFT+310, RC_OTHERBED_BOTTOM+20, 95, 26, hWnd, 0);
//	printf("localIP=%d\n",GetOwnIPAddr());
	SetOB_CobList(TRUE);
	
	for(i=0;i<9;i++){
		SendMessage(cobWave, CB_ADDSTRING, 0, (LPARAM)strWave5Lead[i]);
	}
	
	SendMessage(cobWave, CB_SETCURSEL, gbOtherBedWaveIndex, 0);
	
	OldBtnProc = SetWindowCallbackProc(btnBedNO, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnWave, BtnProc);
	OldCobProc = SetWindowCallbackProc(cobBedNO, CobProc);
	OldCobProc = SetWindowCallbackProc(cobWave, CobProc);
	
	btnParaSet[0].bID = PARA_SET_ECG;
	btnParaSet[1].bID = PARA_SET_NIBP;
	btnParaSet[2].bID = PARA_SET_SPO2;
	if(gbHaveCo2){
		btnParaSet[3].bID = PARA_SET_CO2;
	}
	else{
		btnParaSet[3].bID = PARA_SET_RESP;	
	}
	btnParaSet[4].bID = PARA_SET_TEMP;

	OldBtnProc = SetWindowCallbackProc(btnParaSet[0].hWnd, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnParaSet[1].hWnd, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnParaSet[2].hWnd, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnParaSet[3].hWnd, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnParaSet[4].hWnd, BtnProc);
	
	return 0;
}

static int CreateHaveIBPBtn(HWND hWnd)
{
	
//ecg
	btnParaSet[0].hWnd = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON  | BS_MULTLINE,
			BTN_SET1,PARAVIEW_LEFT-20,PARAVIEW_TOP+gRCEcg.top, 20,RECTH(gRCEcg)+1 , hWnd, 0);
//nibp
	 btnParaSet[1].hWnd = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_MULTLINE,
			BTN_SET2, PARAVIEW_LEFT-20,PARAVIEW_TOP+gRCNibp.top, 20,RECTH(gRCNibp)+1 , hWnd, 0);
//spo2
	btnParaSet[2].hWnd = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_MULTLINE,
			BTN_SET3, PARAVIEW_LEFT-20,PARAVIEW_TOP+gRCSpO2AndPR.top, 20,RECTH(gRCSpO2AndPR)+1 , hWnd, 0);
//resp
	btnParaSet[3].hWnd = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_MULTLINE,
			BTN_SET4, PARAVIEW_LEFT-20,PARAVIEW_TOP+gRCResp.top, 20,RECTH(gRCResp)+1 ,hWnd, 0);
//temp
	btnParaSet[4].hWnd = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_MULTLINE,
			BTN_SET5, PARAVIEW_LEFT-20,PARAVIEW_TOP+gRCTemp.top, 20,RECTH(gRCTemp)+1 , hWnd, 0);		
//ibp1
	btnParaSet[6].hWnd = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_MULTLINE,
			BTN_SET7, SCREEN_LEFT+1,PARAVIEW_TOP+gRCTemp.top, 20,RECTH(gRCTemp)+1 ,hWnd, 0);

//ibp2
	btnParaSet[5].hWnd = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_MULTLINE,
			BTN_SET6, PARAVIEW_LEFT/2,PARAVIEW_TOP+gRCTemp.top, 20,RECTH(gRCTemp)+1 , hWnd, 0);		



	
	btnParaSet[0].bID = PARA_SET_ECG;
	btnParaSet[1].bID = PARA_SET_NIBP;
	btnParaSet[2].bID = PARA_SET_SPO2;
	if(gbHaveCo2){
		btnParaSet[3].bID = PARA_SET_CO2;
	}
	else{
		btnParaSet[3].bID = PARA_SET_RESP;	
	}
	btnParaSet[4].bID = PARA_SET_TEMP;
	btnParaSet[5].bID = PARA_SET_IBP2;
	btnParaSet[6].bID = PARA_SET_IBP1;

	OldBtnProc = SetWindowCallbackProc(btnParaSet[0].hWnd, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnParaSet[1].hWnd, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnParaSet[2].hWnd, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnParaSet[3].hWnd, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnParaSet[4].hWnd, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnParaSet[5].hWnd, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnParaSet[6].hWnd, BtnProc);

	return 0;
}
//创建按键并显示不同的名字
int NewCreateParaMenu(HWND hWnd, BYTE bInterface)
{
	
	char strMenu[100]={0};
	int i;
	
	
if(hWnd == (HWND)NULL) return -1;
		
	for(i =0; i<PARA_SET_COUNT; i ++)	{
		btnParaSet[i].hWnd  = (HWND)NULL;
		btnParaSet[i].bID = PARA_SET;
	}
	
//大字符界面焦点顺序
if(gCfgSystem.bInterface==SCREEN_BIGCHAR){
		CreateBigCharMenuBtn(hWnd);
}else if(gCfgSystem.bInterface==SCREEN_OTHERBED){
		CreateOtherBedBtn(hWnd);
}else{//非大字符界面
	
	if(gbHaveIbp&&gCfgSystem.bInterface==SCREEN_IBP){
		CreateHaveIBPBtn(hWnd);
	}
	else{
		
		CreateNoIBPBtn(hWnd);

	}
}
	SetParaBtnText();
	gbKeyType = KEY_LR;
	
	//设置初始焦点
	SetFocusChild(btnParaSet[0].hWnd);
	return 0;
}



/*
	初始化菜单按钮,程序启动时调用
*/
static int InitParaMenu()
{
	int i;

	for(i=0; i<PARA_SET_COUNT; i++){
		btnParaSet[i].hWnd = (HWND)NULL;
		btnParaSet[i].bID = PARA_SET;
	}

	return 0;
}
     
/*
	销毁参数菜单
	切换屏幕时,需要调用该函数
*/
int DestroyParaMenu()
{
	int i;
	
	for(i=0; i<PARA_SET_COUNT; i++){
		if(btnParaSet[i].hWnd != (HWND)NULL){
			DestroyWindow(btnParaSet[i].hWnd);	
			btnParaSet[i].hWnd = (HWND)NULL;
			btnParaSet[i].bID = PARA_SET;
		}
	}
	if(btnBedNO!= (HWND)NULL){	
		DestroyWindow(btnBedNO);
		btnBedNO=(HWND)NULL;
	}
	if(cobBedNO!= (HWND)NULL){	
		DestroyWindow(cobBedNO);
		cobBedNO=(HWND)NULL;
	}
	if(btnWave!= (HWND)NULL){
		DestroyWindow(btnWave);
		btnWave=(HWND)NULL;
	}
	if(btnWave!= (HWND)NULL){
		DestroyWindow(cobWave);
		cobWave=(HWND)NULL;
	}
	return 0;
}

//////////// create extend panel ////////////////
static int CreateExtendPanel()
{
	//Short Trned
	ghWndPanel_ShortTrend = CreateWindow("static", "", 
 					     WS_CHILD | SS_BLACKRECT,
					     PANEL_SHORTTREND, 
					     RC_SHORTTREND_RIGHT+5, 
					     RC_SHORTTREND_TOP, 
					     RC_NORMAL_RIGHT - (RC_SHORTTREND_RIGHT+2), 
					     RC_SHORTTREND_BOTTOM - RC_SHORTTREND_TOP, 
					     ghWndMain, 0);
	OldPanelProc_ShortTrend = SetWindowCallbackProc(ghWndPanel_ShortTrend, PanelProc_ShortTrend);
	//oxyCrg
	ghWndPanel_Crg = CreateWindow("static", "", 
				             WS_CHILD | SS_BLACKRECT,
					     PANEL_OXYCRG, 
					     RC_OXYCRG_LEFT, 
					     RC_OXYCRG_BOTTOM+5, 
					     RC_NORMAL_RIGHT - RC_OXYCRG_LEFT, 
					     RC_NORMAL_BOTTOM - (RC_OXYCRG_BOTTOM+5), 
					     ghWndMain, 0);
	OldPanelProc_Crg = SetWindowCallbackProc(ghWndPanel_Crg, PanelProc_Crg);

	//OtherBed
	ghWndPanel_OtherBed = CreateWindow("static", "", 
				             WS_CHILD | SS_BLACKRECT,
					     PANEL_OTHERBED, 
					     RC_OTHERBED_LEFT, 
					     RC_OTHERBED_BOTTOM+20+28, 
					     RC_NORMAL_RIGHT - RC_OTHERBED_LEFT+5, 
					     RC_NORMAL_BOTTOM - (RC_OTHERBED_BOTTOM)-20-28, 
					     ghWndMain, 0);
	OldPanelProc_OtherBed = SetWindowCallbackProc(ghWndPanel_OtherBed, PanelProc_OtherBed);
	
	//Heart Icon
	ghWndPanel_Heart = CreateWindow("static", "", 
				      WS_CHILD | SS_BLACKRECT |WS_VISIBLE,
				      PANEL_HEART, 
				      SCREEN_RIGHT-30, 
				      SCREEN_TOP+55, 
				      40, 
				      22, 
				      ghWndMain, 0);
	
	
	return 0;
}
///////////////////////////////////////////


//是否正在绘制参数
extern BOOL gbParasViewing;
/*
	主窗口消息处理函数
*/
static int MainWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	char strMenu[100]={0};
	int i;
	RECT rc;
		
	GetClientRect(hWnd, &rc);	
	
	switch(message){
		case MSG_CREATE:{
			ghWndMain = hWnd;
			
			//加载语言资源文件
			LoadLanguageRes();
			
			//加载相应的字库和字体
			LoadFonts();
					
			//加载系统图标
			LoadSystemIcons();
			
			//下一页图标改成字符>>
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "nextpage", gstrNextPage,sizeof gstrNextPage,">>");

			//创建报警线程
   			pthread_mutex_init(&mtSaveAlarmData, NULL);
    			InitAlarmManage();
	
			//初始化iBio测量模块
		//	InitiBio();
			NewInitiBio();
			//初始化参数菜单
			InitParaMenu();

			//启动显示参数线程
			
  			InitParasView(gCfgSystem.bInterface);
			//CreateViewParasProc();
			//创建参数菜单
			NewCreateParaMenu(hWnd, gCfgSystem.bInterface);

			//创建拓展面板short trend, oxyCRG,otherbed
			CreateExtendPanel();		
			
			//初始化波形绘图
 			NewInitWaveDraw();

			//启动绘图线程
 			NewCreateDrawWaveProc();
			
			//初始化网络
 			NetInit();
			
			//初始化打印机TODO: TTL串口暂时与参数板冲突
//			Init_Printer_UC50();
		
			//初始化电源板通信串口	TTL串口
			Init_Prower();

			//初始化键盘
			 Deal_Rotor();
	
			//获取系统自检结果
	//		iBioCMD_GET_POST_RESULT();
			
			//建立半秒钟的定时器
 			bCreateMainTimerOK = SetTimer(hWnd, _ID_TIMER_MAIN, 20);

			//打开看门狗
		
			if(gCfgSystem.bWDTEnable){
				printf("%s:%d Start Watchdog.\n", __FILE__, __LINE__);
				//WDT_Start();     
		//		InitDevice_Wdt();
			}
	
		//	gbViewDemoData=TRUE;
			 CreateARRAlarmReview(hWnd);
		}break;
		case MSG_TIMER:{
			static BYTE bAlmViewTimerCount = 0;
			//电源正在关闭时，不显示
			if(gPowerStatus.bShutDown == POWER_SHUTDOWN){
				break;
			}
			
			//显示测量参数
			if(!gbParasViewing)
       				NewViewParas_bak();
			else
				printf("%s:%d Viewing paras.\n", __FILE__, __LINE__);

			//显示波形参数
			DrawWaveParas();	
		
			Set_broad_para();
	
		//	if((bAlmViewTimerCount%2)!=0)
		//		Proc_Recv_Power();

			//显示报警信息1s一次
		
			if(bAlmViewTimerCount<5){
				bAlmViewTimerCount ++;
			}
			else{
				bAlmViewTimerCount = 0;
				ViewPhyInfo();
				ViewTecInfo();
					
				//更新短趋势
				if(gCfgSystem.bInterface == SCREEN_SHORTTREND && gbNCanDrawWave)
					NewUpdateShortTrend();
				
				//更新oxyCRG
				if(gCfgSystem.bInterface == SCREEN_OXYCRG && gbNCanDrawWave)
					NewUpdateCrg();
				
				//更新OtherBed
				if(gCfgSystem.bInterface == SCREEN_OTHERBED&& gbNCanDrawWave){
					//显示它床参数
					DrawOtherBed();
					//实时更新它床床位列表
					SetOB_CobList(FALSE);
				}
			//发送广播报文 1秒发一次
				NetSend_Braodcast();		
			//电源板通讯接收
				Proc_Recv_Power();
				//喂狗
			//	WDT_Feed();
			}
			 
			break;
		}break;
		case B_MSG_TIMER:{

		}break;
		case B_MSG_WAVE:{

		}break;
		case MSG_PAINT:{

		}break;
		case MSG_COMMAND:{
			int	id   = LOWORD(wParam);
			int para_id = 0;
			static BOOL bTest = TRUE;
			
			switch(id){
				
				case BTN_SET1:{
					para_id = btnParaSet[0].bID;
				}break;
				case BTN_SET2:{
					para_id = btnParaSet[1].bID;
				}break;
				case BTN_SET3:{
					para_id = btnParaSet[2].bID;
				}break;
				case BTN_SET4:{
					para_id = btnParaSet[3].bID;
				}break;
				case BTN_SET5:{
					para_id = btnParaSet[4].bID;
				}break;
				case BTN_SET6:{
					para_id = btnParaSet[5].bID;
				}break;
				case BTN_SET7:{
					para_id = btnParaSet[6].bID;
				}break;
				case BTN_SET8:{
					para_id = btnParaSet[7].bID;
				}break;
				case BTN_BEDNO:{
					gbKeyType = KEY_UD;
					SetFocus(cobBedNO);
				}break;
				case BTN_WAVE:{
					gbKeyType = KEY_UD;
					SetFocus(cobWave);
				}break;
			}

			//参数设置
			switch(para_id){
				case PARA_SET_ECG:{
					CreateDlgNewECG(ghWndMain);
					RestorWave();
				}break;
				case PARA_SET_NIBP:{
 					CreateDlgNewNibp(ghWndMain);
					RestorWave();
				}break;
				case PARA_SET_SPO2:{
					CreateDlgNewSpo2(ghWndMain);
					RestorWave();
				}break;
				case PARA_SET_RESP:{
					CreateDlgNewRESP(ghWndMain);
					RestorWave();
				}break;
				case PARA_SET_TEMP:{
					CreateDlgNewTemp(ghWndMain);
					RestorWave();
				}break;
				case PARA_SET_CO2:{
					CreateDlgCO2(ghWndMain);
					RestorWave();
				}break;
				case PARA_SET_IBP1:{
					
					CreateDlgIbp1(ghWndMain);
					RestorWave();
					
				}break;
				case PARA_SET_IBP2:{
					
					CreateDlgIbp2(ghWndMain);
					RestorWave();
				}break;
			}
		}break;
		case MSG_CHAR:{
			switch(wParam){
				case 'q':{
					PostMessage(hWnd, MSG_CLOSE, 0, 0);
				}break;
				case 'h':{
					//建立屏幕回顾对话框
					CreateDlgFreeze(hWnd);
					gbSaveWaveValuetofile=TRUE;
				}break;
				case 'd':{//drug
					//	NewInitWaveDraw();
						//退出所有菜单
						ReturnMainView();
						CreateDRUG(ghWndMain);
						RestorWave();
				}break;
		/*other view*/		
				case 'g':{//trend graph
						//退出所有菜单
						ReturnMainView();
						CreateTrendGraph(ghWndMain);
						RestorWave();
				}break;
				case 't':{//graph table(list)
						//退出所有菜单
						ReturnMainView();
						CreateTrendTable(ghWndMain);
						RestorWave();
				}break;
				case 'n':{//nibp list
						//退出所有菜单
						ReturnMainView();
						CreateNibpTable(ghWndMain);
						RestorWave();
				}break;
				case 'r':{//alarm review
					//	NewInitWaveDraw();
						//退出所有菜单
						ReturnMainView();
						CreateAlarmReview(ghWndMain);
						RestorWave();
				}break;
				case 'y':{//trend view
					
					/*
					if(GetActiveWindow()==hWnd_NibpTable||
						GetActiveWindow()==hWnd_TrendGraph||
						GetActiveWindow()==hWnd_TrendTable){
						printf("1111gbTrendStatus=%d\n",gbTrendStatus);
							ReturnMainView();
					}else{
					*/
						
						ReturnMainView();
						if(gbTrendStatus==0)//graph
							PostMessage(ghWndMain, MSG_CHAR, (WPARAM)'g', (LPARAM)0);
						else if(gbTrendStatus==1)//table
							PostMessage(ghWndMain, MSG_CHAR, (WPARAM)'t', (LPARAM)0);
						else if(gbTrendStatus==2)//nibp
							PostMessage(ghWndMain, MSG_CHAR, (WPARAM)'n', (LPARAM)0);
				//	}
	
				}break;
				case 'w':{
					CreateARRAlarmReview(ghWndMain);
					RestorWave();
				}break;
			//fun btn	
				case 'm':{//main menu
				
						//退出所有菜单
						if(GetActiveWindow()==ghWndMain){
							ReturnMainView();
							CCreateMainMenu(ghWndMain);	
							RestorWave();
							
						}else{
							ReturnMainView();
						}

				}break;
				case 'f':{
					//建立屏幕回顾对话框
					if(gbFreezeKey){
						UnfreezeWaves();
					}
					else{
						FreezeWaves();
					}
					gbFreezeKey = !gbFreezeKey;

				}break;
				case 'a':{//pause
					//如果是静音状态，则取消静音状态, 并且不响应
					if(gbSilence){
						gbSilence = FALSE;
						gbSuspending = FALSE;
	 					break;
					} 
					gbSuspending = !gbSuspending;
					//初始化报警暂停时间
					giSuspendTime = gCfgAlarm.bPauseTime * 60;
					if(gbSuspending) Clean_Alarm();
	//				printf("%d.%d,%d,%d\n",GetActiveWindow(),hWnd_NibpTable,hWnd_TrendGraph,hWnd_TrendTable);



//TODO:暂时根据按键板分布设置为pause，下面为打开趋势图命令
	#if 0
					if(GetActiveWindow()==hWnd_NibpTable||
						GetActiveWindow()==hWnd_TrendGraph||
						GetActiveWindow()==hWnd_TrendTable){
							ReturnMainView();
					}else{
						ReturnMainView();
						if(gbTrendStatus==0)//graph
							PostMessage(ghWndMain, MSG_CHAR, (WPARAM)'g', (LPARAM)0);
						else if(gbTrendStatus==1)//table
							PostMessage(ghWndMain, MSG_CHAR, (WPARAM)'t', (LPARAM)0);
						else if(gbTrendStatus==2)//nibp
							PostMessage(ghWndMain, MSG_CHAR, (WPARAM)'n', (LPARAM)0);
					}
	#endif		
				}break;
				case 'p':{
					
					
				//切换视图
				//	SwitchView();
  			
				//	拷贝屏幕
		    		//	CopyScreenByRect();
		    		
		    		/*
		    		static int loop;
				if(loop==0){
			    		StopNibpMeasure();
					printf("+++++++stop nibp \n");
				}else if(loop==1){
					StartNibpMeasure(7);
					printf("+++++++start nibp cal\n");
				}else{
					NibpCalPressure();
					printf("++++++++send cal pressure\n");
				}
				loop=(loop+1)%3;	
				*/
				
				
				
		#if  0
			//如果打印机空闲，则启动实时打印，否则停止目前所有的打印
				if(gPrinterStatus != PRINT_STATUS_BUSY
					&&gPrinterStatus!=PRINT_STATUS_ERR
					&&gPrinterStatus!=PRINT_STATUS_NOPRN
					&&gPrinterWorkType == PRNWORK_NONE){
					printf("%s:%d Start real print. \n", __FILE__, __LINE__);
					Printer_StartPrintRealWave();
				}
				else{
					if(gPrinterWorkType == PRNWORK_REAL){
						//停止实时打印任务
						printf("%s:%d Stop real print. \n", __FILE__, __LINE__);
						Printer_StopPrintRealWave();
					}
					else {
						//停止当前的打印任务
						printf("%s:%d Stop current print. \n", __FILE__, __LINE__);
						Printer_StopCurWork();	
						
					}
				}	
			#endif
				}break;
				case 's':{//silence
					//如果是静音状态，则取消静音状态, 并且不响应
					//如果处在报警暂停状态,则解除报警暂停状态,此时不响应暂停键
					if(gbSuspending){
						gbSuspending = FALSE;
						giSuspendTime = gCfgAlarm.bPauseTime * 60;
					} 
					//静音功能
					gbSilence = !gbSilence;
				}break;
				case 'o':{//power off
					if(gPowerStatus.bPowerType!=POWER_AC&&AUTO_Shutdown){
						CMD_Power(0);
					}
					if(bShow_Poweroff == FALSE){
						CreateDlgPower(ghWndMain);
						RestorWave();
					}
				}break;
				case 'z':{//nibp
					if(gValueNibp.bStatus!=NIBP_SYSTEM_RUNNING){
						//启动NIBP 测量
						if(B_PRINTF) printf("%s:%d Start NIBP... ...\n", __FILE__, __LINE__);
						NIBP_Start();
						
					}
					else{
						//停止NIBP测量
						if(B_PRINTF) printf("%s:%d Cancel NIBP\n", __FILE__, __LINE__);
						NIBP_Cancel();
					
					}
				
				}break;
			}
		}break;
		case MSG_KEYUP:{
			HWND hWndCur;
			HWND hWndNext;
			static int iIDCur;
						
			hWndCur = GetFocusChild(hWnd);
			if(hWndCur != -1) iIDCur = GetDlgCtrlID(hWndCur);

			if(wParam == SCANCODE_CURSORBLOCKLEFT){
				//Left
				if(iIDCur>BTN_HEAD) iIDCur --;
				else iIDCur = BTN_TAIL;
				hWndNext = GetDlgItem(hWnd, iIDCur);
				while(hWndNext==0){
					if(iIDCur>BTN_HEAD) iIDCur --;
					else iIDCur = BTN_TAIL;
					hWndNext = GetDlgItem(hWnd, iIDCur);
				}
				SetFocusChild(hWndNext);
			}
			else if(wParam == SCANCODE_CURSORBLOCKRIGHT){
				//Right
				if(iIDCur<BTN_TAIL) iIDCur ++;
				else iIDCur = BTN_HEAD;
				hWndNext = GetDlgItem(hWnd, iIDCur);
				while(hWndNext==0){
					if(iIDCur<BTN_TAIL) iIDCur ++;
					else iIDCur = BTN_HEAD;
					hWndNext = GetDlgItem(hWnd, iIDCur);
				}
				SetFocusChild(hWndNext);

			}
		}break;
		case MSG_CLOSE:{
			//因为使用广播MSG_CLOSE来实现一键退出菜单
			
		//	DestroyMainWindow(hWnd);
		//	PostQuitMessage(hWnd);
			return 0;
		}break;
	}
	
	return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

/*
	初始化主窗口属性
*/

static void InitCreateInfo(PMAINWINCREATE pCreateInfo)
{
	pCreateInfo->dwStyle		= WS_VISIBLE ;		//窗口风格
	pCreateInfo->dwExStyle	= WS_EX_NONE;		//附加风格
	pCreateInfo->spCaption  	= "TT";				//标题
	pCreateInfo->hMenu  	  	= 0;					//菜单句柄
 	pCreateInfo->hCursor		= GetSystemCursor (IDC_ARROW);		//光标句柄
// 	pCreateInfo->hCursor		= NULL;				//光标句柄
	pCreateInfo->hIcon		= 0;					//图标
	pCreateInfo->MainWindowProc = MainWinProc;		//消息处理函数
	pCreateInfo->lx			= 0;					//左上角绝对横坐标
	pCreateInfo->ty			= 0;					//左上角绝对纵坐标
	pCreateInfo->rx			= SCREEN_RIGHT;				//右下角绝对横坐标
	pCreateInfo->by			= SCREEN_BOTTOM;				//右下角绝对纵坐标
       	pCreateInfo->iBkColor		= COLOR_black;		//背景色
//        	pCreateInfo->iBkColor		= COLOR_lightgray;	//背景色
  	pCreateInfo->dwAddData = 0;					//附加值
	pCreateInfo->hHosting	= HWND_DESKTOP;		//消息队列的属主窗口
}

//--------------------- interface -----------------
/*
	切换不同屏幕下的参数菜单
*/
int SwitchParasMenu()
{
	//销毁菜单
	DestroyParaMenu();
	
	//重新创建菜单
	NewCreateParaMenu(ghWndMain, gCfgSystem.bInterface);
	
	return 0;
}

/*
	GUI起始函数
*/
int GUI_Main(int argc, const char *argv[])
{
	MAINWINCREATE	CreateInfo;	//主窗体属性
	MSG		Msg;
/*	
#if defined(_LITE_VERSION) && !(_STAND_ALONE)
	int i;
	const char* layer = NULL;
	RECT max_rect = {0, 0, 0, 0};

	for (i = 1; i < argc; i++) {
		if (strcmp (argv[i], "-layer") == 0) {
			layer = argv[i + 1];
			break;
		}
	}
	
	GetLayerInfo (layer, &max_rect, NULL, NULL, NULL);
	if (JoinLayer (layer, argv[0], 
	    max_rect.left, max_rect.top, 
	    max_rect.left + SCREEN_RIGHT, 
	    max_rect.top + SCREEN_BOTTOM) == INV_LAYER_HANDLE) {
		    exit (1);
	    }
	    if (!InitVectorialFonts ()) {
		    exit (2);
	    }
#endif
*/
	InitMiniGUIExt();				//初始化扩展控件库
	InitCreateInfo (&CreateInfo);
	ghWndMain = CreateMainWindow (&CreateInfo);
	if (ghWndMain == HWND_INVALID)
		return -1;

	ShowWindow(ghWndMain, SW_SHOWNORMAL);

	while( GetMessage (&Msg, ghWndMain) ) {
		TranslateMessage (&Msg);
		DispatchMessage (&Msg);
	}

	MainWindowThreadCleanup (ghWndMain);
	
// 	if(hdcScreen !=(HDC)NULL){
// 		DeleteCompatibleDC(hdcScreen);
// 	}
#if _LITE_VERSION
   	TermVectorialFonts ();
#endif
	return 0;
}



#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif

/*************************************************************************** 
 *	Module Name:	ScreenReview
 *
 *	Abstract:	回放拷贝的屏幕
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-07-31 17:00:26
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"
#include "DrawWave.h"

//DID_SCREENREVIEW
#define DID_SR_HELP		DID_SCREENREVIEW
#define DID_SR_OK			DID_SCREENREVIEW+1
#define DID_SR_NEXT		DID_SCREENREVIEW+2
#define DID_SR_PREVIOUS		DID_SCREENREVIEW+3
#define DID_SR_SET_SELECT	DID_SCREENREVIEW+4
#define DID_SR_SELECT		DID_SCREENREVIEW+5

static HWND editHelp;
static HWND btnOK;
static HWND btnNext;
static HWND btnPrevious;
static HWND btnSelect;
static HWND cobSelect;

static WNDPROC  OldBtnProc;
static WNDPROC  OldCobProc;

static DLGTEMPLATE DlgSR= {
	WS_VISIBLE,
	WS_EX_NOCLOSEBOX,   
  	5, 3, 545, 568,    
	"",
	0, 0,
	1,      
	NULL,
	0
};

static CTRLDATA CtrlSR[] = {
	{
		"mledit",
		WS_CHILD,
		450, 303, 80, 1,
		DID_SR_HELP,
		"",
		0
	}
};

//屏幕回顾线程
pthread_t 	ptScreenReview;

//传口自身的句柄
static HWND hWndSelf = (HWND)NULL;

//回放的区域
static RECT rcReview;

//时间
static RECT rcTime;

//主窗口句柄
extern HWND ghWndMain;
//是否绘制波形
//extern BOOL gbCanDrawWave;	


//初始化回放区域
static int InitReviewRect(HWND hWnd)
{
	RECT rc;
	HDC hdc;
	BITMAP bmp;
	
	if(hWnd == (HWND)NULL) return -1;
	
	GetClientRect(hWnd, &rc);

 	SetRect(&rcReview, rc.left+1, rc.top+30, rc.right-1, rc.bottom-1);
	SetRect(&rcTime, rc.left+1, rc.top+1, rc.left+200, rc.top+25);
			
	hdc = GetClientDC(hWndSelf);
	if(hdc == (HDC)NULL) return -1;
	SetBrushColor(hdc, COLOR_black);
	FillBox(hdc, rcReview.left, rcReview.top, RECTW(rcReview), RECTH(rcReview)) ;
			
	LoadBitmap(hdc, &bmp, "screen/20070815164425.bmp");
	FillBoxWithBitmap(hdc, rcReview.left, rcReview.top, RECTW(rcReview), RECTH(rcReview), &bmp) ;
	
	UnloadBitmap(&bmp);
	
	//Time
	SelectFont(hdc, gFontSystem);
	SetTextColor(hdc, COLOR_black);
	SetBkColor(hdc, COLOR_lightgray);
	DrawText(hdc, "2007/07/31 18:05:22", -1, &rcTime, DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE);
	
	ReleaseDC(hdc);
	
	return 0;
}


static  int BtnProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	
	id = GetDlgCtrlID(hWnd);
			
	switch(message){
		case MSG_SETFOCUS:{
			SetWindowBkColor(hWnd, SETFOCUS_COLOR);	
		}break;
		case MSG_KILLFOCUS:{
			SetWindowBkColor(hWnd, KILLFOCUS_COLOR);
		}break;
	}
	
	temp = (*OldBtnProc)(hWnd, message, wParam, lParam);
	return(temp);
}       

static int CobProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	int index;

	id = GetDlgCtrlID(hWnd);
	
	switch(message){
		case MSG_COMMAND:{
		case MSG_KEYUP:{
			if(wParam==SCANCODE_ENTER||CBN_EDITCHANGE==HIWORD(wParam)){
					index = SendMessage(hWnd, CB_GETCURSEL, 0, 0);
					
					gbKeyType = KEY_TAB;
				}break;
			}
			return 0;
		}break;	
	}	
	
	temp = (*OldCobProc)(hWnd, message, wParam, lParam);
	return(temp);
}

static int DlgProcSR(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	char strMenu[100];
	int res; 
	int i;
	
	switch(message){
		case MSG_CREATE:{
			gbKeyType = KEY_TAB;
						
			hWndSelf = hDlg;
                                                                                  
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "SCREEN_REVIEW", "caption", strMenu, sizeof strMenu);	
			SetWindowCaption(hDlg, strMenu);

			//Select
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "SCREEN_REVIEW", "select", strMenu, sizeof strMenu);	
			btnSelect = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_SR_SELECT, 305, 2, 70, 25, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnSelect, BtnProc);
			
			//退出
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "GENERAL", "exit", strMenu, sizeof strMenu);	
			btnOK = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					     DID_SR_OK, 380, 2, 70, 25, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnOK, BtnProc);
			
			//Previous
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "SCREEN_REVIEW", "previous", strMenu, sizeof strMenu);	
			btnPrevious = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					       DID_SR_PREVIOUS, 455, 2, 40, 25, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnPrevious, BtnProc);

			//Next
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "SCREEN_REVIEW", "next", strMenu, sizeof strMenu);	
			btnNext = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					     DID_SR_NEXT, 500, 2, 40, 25, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnNext, BtnProc);
		}break;
		case MSG_PAINT:{
   			//初始化回放区域
			InitReviewRect(hWndSelf);
		}break;
		case MSG_COMMAND:{
			int	id   = LOWORD(wParam);
			
			switch(id){
				case DID_SR_OK:{
					PostMessage(hDlg, MSG_CLOSE, 0, 0L);
				}break;
			}
		}break;
		case MSG_KEYUP:{
			switch(wParam){
				case SCANCODE_ESCAPE:{
					PostMessage(hDlg, MSG_CLOSE, 0, 0L);
				}break;
			}
		}break;
		case MSG_CLOSE:{
			gbKeyType = KEY_LR;
			
			//恢复波形绘制
	//		gbCanDrawWave = TRUE;
			
			//恢复波形位置
			ChangeWavePanelsLeft((PRECT)NULL);


			hWndSelf  = (HWND)NULL;
			EndDialog(hDlg, wParam);
		}break;
	}

	temp = DefaultDialogProc(hDlg, message, wParam, lParam);
	return temp;
}

//-------------------------------------------------- interface ----------------------------------------------------
/**
	拷贝屏幕
*/
int CopyScreenByRect()
{
	RECT rcWaves;
	BITMAP bmpWaves;
	int save_ret;
	S_TIME sTime;
	unsigned char strName[30]={0};
	
	/*
	//根据屏幕类型确定拷贝的尺寸
	switch(gCfgSystem.bInterface){
	case SCREEN_SHORTTREND:{
	SetRect(&rcWaves, 
	VW_SHORTTREND_LEFT, 
	VW_SHORTTREND_TOP, 
	VW_SHORTTREND_RIGHT, 
	VW_SHORTTREND_BOTTOM);
}break;
	case SCREEN_MULTIECG:{
	SetRect(&rcWaves, 
	VW_NORMAL_LEFT, 
	VW_NORMAL_TOP, 
	VW_NORMAL_RIGHT, 
	VW_NORMAL_BOTTOM);
}break;
	case SCREEN_BIGCHAR:{
	SetRect(&rcWaves, 
	VW_BIGCHAR_LEFT, 
	VW_BIGCHAR_TOP, 
	VW_BIGCHAR_RIGHT, 
	VW_BIGCHAR_BOTTOM);
}break;
	case SCREEN_NIBPLIST:{
	SetRect(&rcWaves, 
	VW_NIBPLIST_LEFT, 
	VW_NIBPLIST_TOP, 
	VW_NIBPLIST_RIGHT, 
	VW_NIBPLIST_BOTTOM);
}break;
	case SCREEN_OXYCRG:{
	SetRect(&rcWaves, 
	VW_OXYCRG_LEFT, 
	VW_OXYCRG_TOP, 
	VW_OXYCRG_RIGHT, 
	VW_OXYCRG_BOTTOM);
}break;
	default:{
	SetRect(&rcWaves, 
	VW_NORMAL_LEFT, 
	VW_NORMAL_TOP, 
	VW_NORMAL_RIGHT, 
	VW_NORMAL_BOTTOM);
}break;
}
	*/
	
	//尺寸定为薄薪显示区域,包括趋势等信息
// 	SetRect(&rcWaves, 
// 		 VW_NORMAL_LEFT, 
// 		 VW_NORMAL_TOP, 
// 		 VW_NORMAL_RIGHT, 
// 		 VW_NORMAL_BOTTOM);

	//全部区域
#if SCREEN_1024
	SetRect(&rcWaves, 0, 0, 1024, 768);
#elif SCREEN_640
	SetRect(&rcWaves, 0, 0, 800, 600);
#else
	SetRect(&rcWaves, 0, 0, 800, 600);
#endif
	
	
// 	bmpWaves.bmWidth = VW_NORMAL_RIGHT - VW_NORMAL_LEFT;
// 	bmpWaves.bmHeight = VW_NORMAL_BOTTOM - VW_NORMAL_TOP;
#if SCREEN_1024
	bmpWaves.bmWidth = 1024;
 	bmpWaves.bmHeight = 768;
#elif SCREEN_640
	bmpWaves.bmWidth = 800;
 	bmpWaves.bmHeight = 600;
#else
	bmpWaves.bmWidth = 800;
 	bmpWaves.bmHeight = 600;
#endif
	
	bmpWaves.bmBits = NULL;
	GetBitmapFromDC (HDC_SCREEN, rcWaves.left, rcWaves.top,
			 RECTW(rcWaves), RECTH(rcWaves), &bmpWaves);

	if (!bmpWaves.bmBits) {
		fprintf (stderr, "SaveMainWindowContent: SaveBox error.\n");
		return -1;
	}

	//获取存储时间
	GetSysTime(&sTime);
	snprintf(strName, sizeof strName, "screen/%.4d%.2d%.2d%.2d%.2d%.2d.bmp", 
		 sTime.wYear, sTime.bMonth, sTime.bDay, sTime.bHour, sTime.bMin, sTime.bSec);
	printf("%s:%d Save Screen at %s.\n", __FILE__, __LINE__, strName);
	
	save_ret = SaveBitmap(HDC_SCREEN, &bmpWaves, strName);

	free(bmpWaves.bmBits);
		
	return 0;
}
 
 
/**
	创建屏幕回顾对话框
 */
int CreateScreenReview(HWND hWnd)
{
	DlgSR.controls = CtrlSR;
	DialogBoxIndirectParam(&DlgSR, hWnd, DlgProcSR, 0L);    
	
	return 0;
}

int StartScreenReview()
{
	PostMessage(ghWndMain, MSG_CHAR, (WPARAM)'h', (LPARAM)0);
	return 0;
}

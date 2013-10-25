/*************************************************************************** 
 *	Module Name:	NibpTable
 *
 *	Abstract:	NIBP趋势表显示
 *			思路：从趋势数组中提取出NIBP数据显示
 *			
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-07-20 09:24:50
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"
#include "General_Funcs.h"

//DID_NIBPLIST

#define DID_NL_HELP				DID_NIBPLIST+1
#define DID_NL_OK				DID_NIBPLIST+2
#define DID_NL_CURSOR			DID_NIBPLIST+3
#define DID_NL_PRINT			DID_NIBPLIST+4
#define DID_NL_SETUP			DID_NIBPLIST+5
#define DID_NL_TRENDGRAPH			DID_NIBPLIST+6
#define DID_NL_TRENDLIST			DID_NIBPLIST+7

static HWND editHelp;
static HWND btnOK;
static HWND btnCursor;
static HWND btnPrint;
static HWND btnSetup;
static HWND btnNibp;
static HWND btnToTrendGraph;
static HWND btnToTrendList;

static WNDPROC  OldBtnProc;
static WNDPROC  OldCobProc;

static DLGTEMPLATE DlgSet = {
	WS_BORDER | WS_VISIBLE ,
	WS_EX_NOCLOSEBOX,   
#if SCREEN_640
	5, 200, 535-115,285,    
#else
	5, 200, 535,285,    
#endif	 
	"",
	0, 0,
	1,      
	NULL,
	0
};

static CTRLDATA CtrlSet[] = {
	{
		"mledit",
		WS_CHILD,
		450, 303, 1, 1,
		DID_NL_HELP,
		"",
		0
	}
};

//趋势表参数标志
#define NL_PARA_NONE		0
#define NL_PARA_MARK		1
#define NL_PARA_TIME		2
#define NL_PARA_HR			3
#define NL_PARA_SPO2		4
#define NL_PARA_PR			5
#define NL_PARA_NSYS		6
#define NL_PARA_NDIA		7
#define NL_PARA_NMEAN		8
#define NL_PARA_RR			9
#define NL_PARA_T1			10
#define NL_PARA_T2			11
#define NL_PARA_TD			12
#define NL_PARA_CO2			13
#define NL_PARA_IBP1		14
#define NL_PARA_IBP2		15

//游标移动方向
#define PAGE_INIT			0	//初始化页面
#define PAGE_UP				1	//向上(New)
#define PAGE_DOWN			2	//向下(Old)

//趋势表框架颜色
#define COLOR_TABLEFRAME		COLOR_darkgray	
//趋势表的背景颜色                        	
#define COLOR_TABLEBK			COLOR_black
//趋势表的文字颜色
#define COLOR_TABLETEXT		COLOR_lightgray
//趋势表固定列(Mark, Time)的宽度
#define WIDTH_FIXEDCOL		45

//趋势表的条目的文字的长度
#define TABLE_ITEM_TXTLENGTH		20		
//趋势表具体条目的结构
typedef struct {
	RECT rect;
	unsigned char *strValue[TABLE_ITEM_TXTLENGTH];
}TABLE_ITEM, *PTABLE_ITEM;

//趋势表最大可绘制的行数(Label, Datas),不包括Status
#define ROW_MAXCOUNT	11
//趋势表最大可绘制的列数(Mark, Time, Paras)
#define COL_MAXCOUNT		13	
//趋势表列内容的结构
typedef struct _TABLE_COL_CONTENT{
	BYTE bColID;		//列标志
	int iRowCount;	//实际的行数
	TABLE_ITEM ColDatas[ROW_MAXCOUNT];	//列内容
}TABLE_COL_CONTENT, *PTABLE_COL_CONTENT;

//趋势表内容的结构
typedef struct _TABLE_CONTENT{
	int iColCount;		//实际的列数
	TABLE_COL_CONTENT Datas[COL_MAXCOUNT];	
}TABLE_CONTENT, *PTABLE_CONTENT;

//趋势表显示的内容
static TABLE_CONTENT	gTableContent;

//趋势表框架
static RECT gRcTableFrame;
//趋势表状态区域
static RECT gRcTableStatus;

//趋势表行的高度
static int giRowHeight;

//--- 分辨率(分钟) ----
static int gTableInter = 1;  

//趋势数组
extern CA_TRENDARRAY gTrendDatas;

//存放提取NIBP后的趋势数组
static CA_TRENDARRAY gNibpTrendDatas;

//趋势数据窗口, 数据从最新(Head)数据往前(Tail)读, 按条读数据
static WORD  gCurrentIndex = 0;

//传口自身的句柄
static HWND hWndSelf = (HWND)NULL;

HWND hWnd_NibpTable = (HWND)NULL;
//是否处在浏览波形的状态
static BOOL bBrowsePage = FALSE;

//趋势数量
static int giTrendDataCount = 0;
extern int gbTrendStatus;
//打印参数信号量
extern sem_t semReal_Print;
//---------- 函数声明 ------------
//创建趋势表
static int InitNibpTable(HWND hWnd);
//更新页面数据
static int UpdateNibpPage(BYTE bType);


/*
	从趋势数组中提取出NIBP数据
*/
static int PickupNibpData()
{
	WORD wHead, wTail, wSize;
	BOOL bLoop;
	int i, j;
	int iCount;
	
	//病人信息
	gNibpTrendDatas.sPatientInfo = gTrendDatas.sPatientInfo;
	//起始时间
	gNibpTrendDatas.sStartTime = gTrendDatas.sStartTime;
	
	//初始化NIBP数组
	gNibpTrendDatas.wHeadIndex = 0;
	gNibpTrendDatas.wTailIndex = 0;
	gNibpTrendDatas.bLoop = FALSE;
	gNibpTrendDatas.wCount = 0;
	
	//从趋势数组中取出NIBP数据, 最多MAXTRENDCOUNT条，不存在翻转的情况
	wHead = gTrendDatas.wHeadIndex;
	wTail = gTrendDatas.wTailIndex;
	bLoop = gTrendDatas.bLoop;
	wSize = MAXTRENDCOUNT;
	
	iCount = 0;
	if(wHead < wTail){
// 		for(i=wHead; i>=0; i--){
		for(i=wTail; i<=(wSize-1); i++){
			if(iCount < wSize){
				if((gTrendDatas.Datas[i].bLogFlag && TRENDNIBP) && (gTrendDatas.Datas[i].dwHaveFlag & HAVENIBP)){
					gNibpTrendDatas.wHeadIndex ++;
					gNibpTrendDatas.Datas[gNibpTrendDatas.wHeadIndex] = gTrendDatas.Datas[i]; 
					iCount++;
				}
			}
			else{
				goto SEARCH_OVER;
			}
		}
// 		for(i=(wSize-1); i>=wTail; i--){
		for(i=0; i<=wHead; i++){
			if(iCount < wSize){
				if((gTrendDatas.Datas[i].bLogFlag && TRENDNIBP) && (gTrendDatas.Datas[i].dwHaveFlag & HAVENIBP)){
					gNibpTrendDatas.wHeadIndex ++;
					gNibpTrendDatas.Datas[gNibpTrendDatas.wHeadIndex] = gTrendDatas.Datas[i]; 
					iCount++;
				}
			}
			else{
				goto SEARCH_OVER;
			}
		}
	}
	else if(bLoop){
// 		for(i=wHead; i>=wTail; i--){
		for(i=wTail; i<=wTail; i++){
			if(iCount < wSize){
				if((gTrendDatas.Datas[i].bLogFlag && TRENDNIBP) && (gTrendDatas.Datas[i].dwHaveFlag & HAVENIBP)){
					gNibpTrendDatas.wHeadIndex ++;
					gNibpTrendDatas.Datas[gNibpTrendDatas.wHeadIndex] = gTrendDatas.Datas[i]; 
					iCount++;
				}
			}
			else{
				goto SEARCH_OVER;
			}
		}
	}
	else{
// 		for(i=wHead; i>wTail; i--){
		for(i=(wTail+1); i<=wHead; i++){
			if(iCount < wSize){
				if((gTrendDatas.Datas[i].bLogFlag && TRENDNIBP) && (gTrendDatas.Datas[i].dwHaveFlag & HAVENIBP)){
					gNibpTrendDatas.wHeadIndex ++;	
					gNibpTrendDatas.Datas[gNibpTrendDatas.wHeadIndex] = gTrendDatas.Datas[i]; 
					iCount++;
				}
			}
			else{
				goto SEARCH_OVER;
			}
		}
	}
	
SEARCH_OVER:
		gNibpTrendDatas.wCount = iCount;	
		if(B_PRINTF) printf("%s:%d Pick up %d NIBP datas.\n", __FILE__, __LINE__, iCount);
		
	return 0;
}


//打印当前列表
 int PrintNIBPTable()
{
	REC_STRING  sInfo;
	int i,j;
	int res;
	
	sInfo.iRowCount = ROW_MAXCOUNT;
	
	for(j=1; j<gTableContent.iColCount; j++){
		//清空
		memset(sInfo.strText, 0, MAX_ROW_COUNT*MAX_TEXT_LENGTH);
		//填充
		for(i=0; i<ROW_MAXCOUNT; i++){
			snprintf(sInfo.strText[i], MAX_TEXT_LENGTH, "%s ", 
				 gTableContent.Datas[j].ColDatas[i].strValue);	
 	//		if(B_PRINTF) printf("Col:%d Row:%d --- %s\n", j, i, sInfo.strText[i]);
		}
		//打印
		res = Printer_PrintString(&sInfo);
	}
	
	//空白走纸
//	Printer_MovePaper(15);
	Printer_Move_Paper(15);
	return 0;
}


static  int BtnProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	
	id = GetDlgCtrlID(hWnd);
		
	switch(message){
		case MSG_SETFOCUS:{
			if(bBrowsePage){
				SetWindowBkColor(hWnd, BROWSE_COLOR);	
			}
			else{
				SetWindowBkColor(hWnd, SETFOCUS_COLOR);	
			}
		}break;
		case MSG_KILLFOCUS:{
			SetWindowBkColor(hWnd, KILLFOCUS_COLOR);
		}break;
		case MSG_KEYUP:{
			switch(wParam){
				case SCANCODE_ENTER:{
					switch(id){
						case DID_NL_CURSOR:{
							bBrowsePage = !bBrowsePage;
							if(bBrowsePage){
								//gbKeyType = KEY_LR;
								gbKeyType = KEY_AD;
								SetWindowBkColor(hWnd, BROWSE_COLOR);
							} 
							else{
								gbKeyType = KEY_TAB;
								SetWindowBkColor(hWnd, SETFOCUS_COLOR);	
							}
						}break;		
					}//end switch(id)
				}break;
			}
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

static int DlgProcNIBPTable(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	char strMenu[100];
	int res; 
	
	switch(message){
		case MSG_CREATE:{
			gbKeyType = KEY_TAB;
			gbTrendStatus=2;			
			hWndSelf = hDlg;
                     hWnd_NibpTable=hDlg;
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "TRENDTABLE", "caption", strMenu, sizeof strMenu);	
			SetWindowCaption(hDlg, strMenu);
				
// 			//设置
// 			memset(strMenu, 0, sizeof strMenu);
// 			GetStringFromResFile(gsLanguageRes, "TRENDTABLE", "setup", strMenu, sizeof strMenu);	
// 			btnSetup = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
// 					DID_NL_SETUP, 5, 10, 70, 25, hDlg, 0);
// 			OldBtnProc = SetWindowCallbackProc(btnSetup, BtnProc);
// 			EnableWindow(btnSetup, FALSE);
			
			//浏览
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "TRENDTABLE", "cursor", strMenu, sizeof strMenu);	
			btnCursor = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_NL_CURSOR, 5, 10, 70, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnCursor, BtnProc);
			
			//打印
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "TRENDTABLE", "print", strMenu, sizeof strMenu);	
			btnPrint = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					DID_NL_PRINT, 5, 40, 70, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnPrint, BtnProc);
			if(gPrinterStatus == PRNSTATUS_ERR)
				EnableWindow(btnPrint, FALSE);
			
			//切换到趋势图
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "GENERAL", "trendgraph", strMenu, sizeof strMenu,"Graph");	
			btnToTrendGraph= CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					     DID_NL_TRENDGRAPH, 5, 70, 70, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnToTrendGraph, BtnProc);		
			//切换到趋势列表
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFiles(gsLanguageRes, "GENERAL", "trendlist", strMenu, sizeof strMenu,"List");	
			btnToTrendList= CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					     DID_NL_TRENDLIST, 5, 100, 70, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnToTrendList, BtnProc);							
			//退出
			memset(strMenu, 0, sizeof strMenu);
			GetStringFromResFile(gsLanguageRes, "GENERAL", "exit", strMenu, sizeof strMenu);	
			btnOK = CreateWindow("button", strMenu, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
					     DID_NL_OK, 5, 130, 70, 26, hDlg, 0);
			OldBtnProc = SetWindowCallbackProc(btnOK, BtnProc);
			
			//提取NIBP数据
			PickupNibpData();
			
			SetFocus(btnOK);
		}break;
		case MSG_PAINT:{
			InitNibpTable(hDlg);
		}break;
		case MSG_COMMAND:{
			int	id   = LOWORD(wParam);
			
			switch(id){
				case DID_NL_PRINT:{
					gPrintingType=PRN_WORK_NIBP;
					sem_post(&semReal_Print);
					
				//	PrintNIBPTable();
				}break;
				case DID_NL_TRENDGRAPH:{
					PostMessage (ghWndMain, MSG_KEYDOWN, SCANCODE_G, 0);
				}break;
				case DID_NL_TRENDLIST:{
					PostMessage (ghWndMain, MSG_KEYDOWN, SCANCODE_T, 0);
				}break;
				case DID_NL_OK:{
					PostMessage(hDlg, MSG_CLOSE, 0, 0L);
				}break;
			}
		}break;
		case MSG_KEYUP:{
			switch(wParam){
				case SCANCODE_ESCAPE:{
					PostMessage(hDlg, MSG_CLOSE, 0, 0L);
				}break;
				//case SCANCODE_CURSORBLOCKLEFT:{
				case SCANCODE_A:{
					if(bBrowsePage){
						SetFocus(btnCursor);
						UpdateNibpPage(PAGE_UP);
						return TRUE;	
					}
				}break;
				//case SCANCODE_CURSORBLOCKRIGHT:{
				case SCANCODE_D:{
					if(bBrowsePage){
						SetFocus(btnCursor);
						UpdateNibpPage(PAGE_DOWN);
						return TRUE;	
					}
				}break;
			}
		}break;
		case MSG_CLOSE:{
			gbKeyType = KEY_TAB;
			
			hWndSelf  = (HWND)NULL;
			EndDialog(hDlg, wParam);
		}break;
	}
	
	temp = temp = DefaultDialogProc(hDlg, message, wParam, lParam);
	return temp;
}


/**
	根据配置初始化趋势表内容
	@param:pContent: 趋势表内容
 */
static int InitTableContent(PTABLE_CONTENT pContent)
{
	int i, j;
	
	if(pContent == NULL) return -1;
	
		
	//根据配置设置列标志
	pContent->Datas[0].bColID = NL_PARA_MARK;
	pContent->Datas[1].bColID = NL_PARA_TIME;
	//TODO:根据配置文件配置
	pContent->Datas[2].bColID = NL_PARA_NSYS;
	pContent->Datas[3].bColID = NL_PARA_NDIA;
	pContent->Datas[4].bColID = NL_PARA_NMEAN;
	pContent->Datas[5].bColID = NL_PARA_HR;
//	pContent->Datas[6].bColID = NL_PARA_SPO2;
//	pContent->Datas[7].bColID = NL_PARA_RR;
// 	pContent->Datas[8].bColID = NL_PARA_T1;
// 	pContent->Datas[9].bColID = NL_PARA_T2;
		
	//获得实际列数
	pContent->iColCount = 6;		//Mark & Time is Fixed, 即最少有固定两列
	
	//根据趋势数量设置行数，不超过ROW_MAXCOUNT行
	pContent->Datas[0].iRowCount =1;	//最少有一行标签	
	pContent->Datas[1].iRowCount =1;	//最少有一行标签	
	
	//初始化列表的数据
	//列
	for(j=0; j<gTableContent.iColCount; j++){
		//行
		for(i=0; i<ROW_MAXCOUNT; i++){
			memset(pContent->Datas[j].ColDatas[i].strValue, 0, TABLE_ITEM_TXTLENGTH);
		}
	}
	
	return 0;
}

/**
	初始化趋势表的相关区域
	@param:hWnd: 窗口句柄
 */
static int InitTableRects(HWND hWnd)
{
	RECT rcWnd;
	int i, j;
	int iCount;			//可变列的数量
	int iTotalWidth;		//可变列所占的总宽度	
	int iItemWidth;		//可变列项目所占的宽度
	
	if(hWnd == (HWND)NULL) return -1;
	
	//获得窗口的区域
	GetClientRect(hWnd, &rcWnd);
		
	//设置框架的区域, 去掉左边的操作区域
	SetRect(&gRcTableFrame, rcWnd.left+85, rcWnd.top+1, rcWnd.right-1, rcWnd.bottom-1);

	//根据最大行数计算行的高度
	giRowHeight = (gRcTableFrame.bottom - gRcTableFrame.top)/(ROW_MAXCOUNT+1);	 //包括status
	
	//设置状态的区域
	SetRect(&gRcTableStatus, gRcTableFrame.left, gRcTableFrame.top, gRcTableFrame.right, gRcTableFrame.top+giRowHeight + giRowHeight/4-10);
	

	//设置内容的区域	
	//固定列(Mark & Time)
	for(i=0; i<ROW_MAXCOUNT; i++){
		//MARK
		gTableContent.Datas[0].ColDatas[i].rect.left = gRcTableStatus.left;
		gTableContent.Datas[0].ColDatas[i].rect.top = gRcTableStatus.bottom+giRowHeight*i;
		gTableContent.Datas[0].ColDatas[i].rect.right= gTableContent.Datas[0].ColDatas[i].rect.left + 1;
		gTableContent.Datas[0].ColDatas[i].rect.bottom = gTableContent.Datas[0].ColDatas[i].rect.top + giRowHeight;
			
		//TIME
		gTableContent.Datas[1].ColDatas[i].rect.left = gTableContent.Datas[0].ColDatas[i].rect.right;
		gTableContent.Datas[1].ColDatas[i].rect.top = gTableContent.Datas[0].ColDatas[i].rect.top;
		gTableContent.Datas[1].ColDatas[i].rect.right = gTableContent.Datas[1].ColDatas[i].rect.left + WIDTH_FIXEDCOL*2;
		gTableContent.Datas[1].ColDatas[i].rect.bottom = gTableContent.Datas[1].ColDatas[i].rect.top + giRowHeight;
	}

	//可变列，根据数量动态设定区域
	iCount = gTableContent.iColCount -2;
	if(iCount !=0){
		iTotalWidth = (gRcTableFrame.right- gTableContent.Datas[1].ColDatas[0].rect.right);
		iItemWidth = iTotalWidth / iCount;
//  		if(B_PRINTF) printf("%s:%d Total %d Cols, Width is %d, Item Width is %d.\n", __FILE__, __LINE__, gTableContent.iColCount, 
//  			iTotalWidth, iItemWidth);
		
		//列
		for(j=2; j<gTableContent.iColCount; j++){
			//行
			for(i=0; i<ROW_MAXCOUNT; i++){
				gTableContent.Datas[j].ColDatas[i].rect.left = gTableContent.Datas[j-1].ColDatas[i].rect.right;
				gTableContent.Datas[j].ColDatas[i].rect.top = gTableContent.Datas[j-1].ColDatas[i].rect.top;
				gTableContent.Datas[j].ColDatas[i].rect.right = gTableContent.Datas[j].ColDatas[i].rect.left + iItemWidth;
				gTableContent.Datas[j].ColDatas[i].rect.bottom = gTableContent.Datas[j].ColDatas[i].rect.top + giRowHeight;
			}
		}
	}
	return 0;
}

/**
	绘制趋势表的框架
 */
static int DrawTableFrame(HDC hdc)
{
	int i, j;
	int iColCount;		//列数
	
	if(hdc == (HDC)NULL) return -1;
	
	SetBrushColor(hdc, COLOR_TABLEBK);
	SetPenColor(hdc, COLOR_TABLEFRAME);
	
	//清屏
	FillBox(hdc, gRcTableFrame.left, gRcTableFrame.top, gRcTableFrame.right - gRcTableFrame.left, gRcTableFrame.bottom - gRcTableFrame.top);
	
	//外框
	Rectangle(hdc, gRcTableFrame.left, gRcTableFrame.top, gRcTableFrame.right, gRcTableFrame.bottom);
	
	
	//状态区域
	MoveTo(hdc, gRcTableStatus.left, gRcTableStatus.bottom);
	LineTo(hdc, gRcTableStatus.right, gRcTableStatus.bottom);

	//内容区域
	
	//得到列的数量
	iColCount = gTableContent.iColCount;
	//行
//   for(i=0; i<ROW_MAXCOUNT; i++){
	//仅绘制标签行
	for(i=0; i<1; i++){	
		MoveTo(hdc, gTableContent.Datas[0].ColDatas[i].rect.left, gTableContent.Datas[0].ColDatas[i].rect.bottom);
		LineTo(hdc, gRcTableFrame.right, gTableContent.Datas[0].ColDatas[i].rect.bottom);		
	}
	//列
	for(i=1; i<iColCount-1; i++){
		MoveTo(hdc, gTableContent.Datas[i].ColDatas[0].rect.right, gTableContent.Datas[i].ColDatas[0].rect.top);
		LineTo(hdc, gTableContent.Datas[i].ColDatas[0].rect.right, gRcTableFrame.bottom);
	}

// 	//绘制全部区域
// 	for(j=0; j<iColCount; j++){
	// 			
//  		for(i=0; i<ROW_MAXCOUNT; i++){
// 			Rectangle(hdc, 
// 				    gTableContent.Datas[j].ColDatas[i].rect.left,
// 				    gTableContent.Datas[j].ColDatas[i].rect.top, 
// 				    gTableContent.Datas[j].ColDatas[i].rect.right, 
// 				    gTableContent.Datas[j].ColDatas[i].rect.bottom
// 				   );
// 		}
// 	}
	// 
	return 0;
}

/**
	取得趋势表的标签信息
 */
static int GetTableLabels(PTABLE_CONTENT pContent)
{
	//参数标签，与参数标志对应,考虑到打印，采用英文
// 	unsigned char *strLabels[]={"","", "time", "hr", "spo2", "pr", "nsys", "ndia", "nmean", "rr", "temp1", "temp2", "td", "etco2", "ibp1","ibp2"};
	unsigned char *strLabels[]={"","", "TIME", "HR", "SpO2", "PR", "SYS", "DIA", "MAP", "RR", "TEMP1", "TEMP2", "Td", "EtCO2", "IBP1","IBP2"};
	unsigned char *strLabels_Span[]={"","", "Hora", "FC", "SpO2", "FP", "SIS", "DIA", "MED", "FR", "T1", "T2", "DT", "EtCO2", "IBP1","IBP2"};
	
	unsigned char strMenu[20]={0};
	BYTE bID;
	int i;		
		
	if(pContent == NULL) return -1;
	
	for(i=0; i<(pContent->iColCount); i++){
// 		memset(strMenu, 0, sizeof strMenu);
// 		GetStringFromResFile(gsLanguageRes, "GENERAL", strLabels[pContent->Datas[i].bColID], strMenu, sizeof strMenu);	
// 		snprintf((unsigned char *)(pContent->Datas[i].ColDatas[0].strValue), TABLE_ITEM_TXTLENGTH, "%s", strMenu);
		if(gCfgSystem.bLanguage == LANGUAGE_SPANISH)
			snprintf((unsigned char *)(pContent->Datas[i].ColDatas[0].strValue), TABLE_ITEM_TXTLENGTH, "%s", strLabels_Span[pContent->Datas[i].bColID]);
		else
			snprintf((unsigned char *)(pContent->Datas[i].ColDatas[0].strValue), TABLE_ITEM_TXTLENGTH, "%s", strLabels[pContent->Datas[i].bColID]);
	}
		
	return 0;
}

/**
	更新趋势表的标签
 */
static int UpdateTableLabels(HDC hdc)
{
	RECT rc;
	int i;
	int res;
	
	if(hdc == (HDC)NULL) return -1;
	
	//获得标签
	res = GetTableLabels(&gTableContent);
			
	if(res != -1){
		SetBkColor(hdc, COLOR_TABLEBK);
		SetTextColor(hdc, COLOR_TABLETEXT);

		for(i=0; i<(gTableContent.iColCount); i++){
//  			if(B_PRINTF) printf("Label(%d) is %s.\n", i, gTableContent.Datas[i].ColDatas[0].strValue);
			DrawText(hdc, (unsigned char *)(gTableContent.Datas[i].ColDatas[0].strValue), -1, 
				 &(gTableContent.Datas[i].ColDatas[0].rect), 
				 DT_NOCLIP | DT_SINGLELINE | DT_VCENTER | DT_CENTER);
		}
	}
	else{
		return -1;
	}
	
	return 0;
}

/**
	填充一行数据
	@param:iRowIndex 行号
 */
static int FillTableItem(PTABLE_CONTENT pContent, const PTRENDDATA pTrendData, int iRowIndex)
{
	BYTE bID;
	int i;
	unsigned char strValue[TABLE_ITEM_TXTLENGTH] = {0};
	S_TIME sTmpTime;
	
	if(pContent == NULL) return -1;
	
	//0是Label
	if(iRowIndex<1) iRowIndex = 1;
	
	if(pTrendData == NULL){
		//清空表格
		for(i=0; i<(pContent->iColCount); i++){
			bID = pContent->Datas[i].bColID;
			pContent->Datas[i].iRowCount = iRowIndex +1;
			memset(strValue, sizeof strValue, 0);
			memset((unsigned char *)(pContent->Datas[i].ColDatas[iRowIndex].strValue), TABLE_ITEM_TXTLENGTH, 0);
			snprintf((unsigned char *)(pContent->Datas[i].ColDatas[iRowIndex].strValue), TABLE_ITEM_TXTLENGTH, "%s", strValue);
		}	
			
		return -1;
	} 
	
	for(i=0; i<(pContent->iColCount); i++){
		bID = pContent->Datas[i].bColID;
		pContent->Datas[i].iRowCount = iRowIndex +1;
		memset(strValue, sizeof strValue, 0);
		//获得记录的时间
		sTmpTime = pTrendData->sTime;
			
		switch(bID){
			case NL_PARA_TIME:{
				snprintf(strValue, sizeof strValue, "%.2d/%.2d %.2d:%.2d", sTmpTime.bMonth, sTmpTime.bDay, sTmpTime.bHour, sTmpTime.bMin);
			}break;
			case NL_PARA_HR:{
				if(pTrendData->dwHaveFlag & HAVEHR)
					snprintf(strValue, sizeof strValue, "%d", pTrendData->wHR);
				else
					snprintf(strValue, sizeof strValue, "---");
			}break;
			case NL_PARA_SPO2:{
				if(pTrendData->dwHaveFlag & HAVESPO2)
					snprintf(strValue, sizeof strValue, "%d", pTrendData->bSpO2);
				else
					snprintf(strValue, sizeof strValue, "---");
			}break;
			case NL_PARA_NSYS:{
                		//判断是否有NIBP数据
				if((pTrendData->bLogFlag & TRENDNIBP)  && (pTrendData->dwHaveFlag & HAVENIBP)){
					snprintf(strValue, sizeof strValue, "%d", pTrendData->sNIBP.wSys);
				}
				else{
					snprintf(strValue, sizeof strValue, " ");
				}
			}break;
			case NL_PARA_NDIA:{
				if((pTrendData->bLogFlag & TRENDNIBP)  && (pTrendData->dwHaveFlag & HAVENIBP)){
					snprintf(strValue, sizeof strValue, "%d", pTrendData->sNIBP.wDia);                         
				}
				else{
					snprintf(strValue, sizeof strValue, " ");
				}
			}break;
			case NL_PARA_NMEAN:{
				if((pTrendData->bLogFlag & TRENDNIBP)  && (pTrendData->dwHaveFlag & HAVENIBP)){
					snprintf(strValue, sizeof strValue, "%d", pTrendData->sNIBP.wMean);                         
				}
				else{
					snprintf(strValue, sizeof strValue, " ");
				}
			}break;
			case NL_PARA_RR:{
				if(pTrendData->dwHaveFlag & HAVERESP)
					snprintf(strValue, sizeof strValue, "%d", pTrendData->wRR);                         
				else
					snprintf(strValue, sizeof strValue, "---");
			}break;             
			case NL_PARA_PR:{
				if(pTrendData->dwHaveFlag & HAVEPR)
					snprintf(strValue, sizeof strValue, "%d", pTrendData->wPR);                           
				else
					snprintf(strValue, sizeof strValue, "---");
			}break;
			case NL_PARA_T1:{
				if(pTrendData->dwHaveFlag & HAVET1)
					snprintf(strValue, sizeof strValue, "%3.1f", (float)(((float)(pTrendData->wTemps[0]))/10.0));
				else
					snprintf(strValue, sizeof strValue, "--.-");
// 				snprintf(strValue, sizeof strValue, "37.5");
			}break;
			case NL_PARA_T2:{
				if(pTrendData->dwHaveFlag & HAVET2)
					snprintf(strValue, sizeof strValue, "%3.1f", (float)(((float)(pTrendData->wTemps[1]))/10.0));
				else
					snprintf(strValue, sizeof strValue, "--.-");
				
// 				snprintf(strValue, sizeof strValue, "37.5");
			}break;
			case NL_PARA_TD:{
				snprintf(strValue, sizeof strValue, "--.-");
			}break;
			case NL_PARA_CO2:{
				if(pTrendData->dwHaveFlag & HAVECO2)
					snprintf(strValue, sizeof strValue, "%d", pTrendData->sCO2.wEtCO2);                           
				else
					snprintf(strValue, sizeof strValue, "--");	
			}break;
			case NL_PARA_IBP1:{
				if(pTrendData->dwHaveFlag & HAVEIBP1)
					snprintf(strValue, sizeof strValue, "%d/%d (%d)", 
						pTrendData->sIBPs[0].iSys, pTrendData->sIBPs[0].iDia, pTrendData->sIBPs[0].iMean);                           
				else
					snprintf(strValue, sizeof strValue, "--/-- (--)");	
			}break;
			case NL_PARA_IBP2:{
				if(pTrendData->dwHaveFlag & HAVEIBP1)
					snprintf(strValue, sizeof strValue, "%d/%d (%d)", 
						pTrendData->sIBPs[0].iSys, pTrendData->sIBPs[0].iDia, pTrendData->sIBPs[0].iMean);                           
				else
					snprintf(strValue, sizeof strValue, "--/-- (--)");	
			}break;
			default:{
				snprintf(strValue, sizeof strValue, "");
			}break;
		}
//  		if(B_PRINTF) printf("%s:%d COL %d  : %s \n", __FILE__, __LINE__, i, strValue);
		memset((unsigned char *)(pContent->Datas[i].ColDatas[iRowIndex].strValue), TABLE_ITEM_TXTLENGTH, 0);
		snprintf((unsigned char *)(pContent->Datas[i].ColDatas[iRowIndex].strValue), TABLE_ITEM_TXTLENGTH, "%s", strValue);
// 		if(B_PRINTF) printf("%s:%d COL %d  : %s \n", __FILE__, __LINE__, i, 
// 			 pContent->Datas[i].ColDatas[iRowIndex].strValue);
	}
	return 0;
}

/**
	查找数据填充表格
	@param:pContent 表格
	@param:wStartIndex 开始索引
	@param:iCount 读取的数量
 */
static int FillTable(PTABLE_CONTENT pContent, WORD wStartIndex, int iCount)
{
	WORD wHead, wTail, wSize;
	BOOL bLoop;
	long int  i,j;              
	int iRow = 1;
	
	//读取的数量不能超过可显示的数据行数
	if(iCount > ROW_MAXCOUNT-1) iCount = ROW_MAXCOUNT - 1;
	
	//清空Table
	for(i=1; i<ROW_MAXCOUNT; i++){
		FillTableItem(pContent, NULL, i);
	}
	
	//从 wStartIndex 处读取 iCount条数据，并填充到表格结构中
	wHead = wStartIndex;
	wTail = gTrendDatas.wTailIndex;
	bLoop = gNibpTrendDatas.bLoop;
	wSize = MAXTRENDCOUNT;
	
	if(wHead < wTail){
		for(i=wHead, j=0; i>=0 && j<iCount; i--, j++){
			FillTableItem(pContent, &(gNibpTrendDatas.Datas[i]), iRow);
			iRow ++;
		}
		for(i=(wSize-1); i>=wTail && j<iCount; i--, j++){
			FillTableItem(pContent, &(gNibpTrendDatas.Datas[i]), iRow);
			iRow ++;
		}
	}
	else if(bLoop){
		for(i=wHead, j=0; i>=wTail && j<iCount; i--, j++){
			FillTableItem(pContent, &(gNibpTrendDatas.Datas[i]), iRow);
			iRow ++;
		}
	}
	else{
		for(i=wHead, j=0; i>wTail && j<iCount; i--, j++){
			FillTableItem(pContent, &(gNibpTrendDatas.Datas[i]), iRow);
			iRow ++;
		}
	}

	return 0;
}

/*
	更新趋势数据
*/
static int UpdateTableDatas(HWND hWnd)
{
	int res;
	int i, j;
	HDC hdc;
	
	hdc = GetClientDC(hWnd);
	
	if(hdc == (HDC)NULL) return -1;
	
	SelectFont(hdc, gFontSystem);
	
	res = FillTable(&gTableContent, gCurrentIndex, ROW_MAXCOUNT);
	
	if(res != -1){
		//填充表格
		SetBkColor(hdc, COLOR_TABLEBK);
		SetTextColor(hdc, COLOR_TABLETEXT);
		SetBrushColor(hdc, COLOR_TABLEBK);
		
		for(i=0; i<(gTableContent.iColCount); i++){
			for(j=0; j<ROW_MAXCOUNT; j++){
				//清除文字
				FillBox(hdc,
					gTableContent.Datas[i].ColDatas[j].rect.left+1, 
					gTableContent.Datas[i].ColDatas[j].rect.top+1,
					gTableContent.Datas[i].ColDatas[j].rect.right-1 - gTableContent.Datas[i].ColDatas[j].rect.left,
					gTableContent.Datas[i].ColDatas[j].rect.bottom-1 - gTableContent.Datas[i].ColDatas[j].rect.top
				       );
				
				//输出文字		
				DrawText(hdc, (unsigned char *)(gTableContent.Datas[i].ColDatas[j].strValue), -1, 
					 &(gTableContent.Datas[i].ColDatas[j].rect), 
					 DT_NOCLIP | DT_SINGLELINE | DT_VCENTER | DT_CENTER);
			}
		}
	}
	else{
		ReleaseDC(hdc);
		return -1;
	}
	
	ReleaseDC(hdc);
	return 0;

}

/*
	更新状态区域 
*/
static int UpdateTableStatus(HWND hWnd)
{
	HDC hdc;
	unsigned char strStatus[50] = {0};	
	int iPageCount=0;
	int iCurPage=0;

	if(hWnd == (HWND)NULL) return -1;
	
	hdc = GetClientDC(hWnd);
	if(hdc == (HDC)NULL) return -1;
		
	//根据趋势数量和一页显示的数量计算页数
	if((giTrendDataCount % (ROW_MAXCOUNT-1))>0){
		iPageCount = giTrendDataCount / (ROW_MAXCOUNT-1) +1;
		//计算当前页数
		iCurPage = iPageCount -  gCurrentIndex / (ROW_MAXCOUNT -1);
		printf("11111111111111111111\n");
	}
	else{
		iPageCount = giTrendDataCount / (ROW_MAXCOUNT-1);
				//计算当前页数
		iCurPage = iPageCount -  gCurrentIndex / (ROW_MAXCOUNT -1)+1;

		printf("222222222222222222222222\n");
	}
				
	

	memset(strStatus, 0, sizeof strStatus);
	snprintf(strStatus, sizeof strStatus, "%d / %d ", iCurPage, iPageCount);
	
	SetBrushColor(hdc, COLOR_black);
	FillBox(hdc, gRcTableStatus.left, gRcTableStatus.top, RECTW(gRcTableStatus), RECTH(gRcTableStatus) );
	
	SetBkColor(hdc, COLOR_TABLEBK);
	SetTextColor(hdc, COLOR_TABLETEXT);
	DrawText(hdc, "NIBP List", -1,  &gRcTableStatus, DT_NOCLIP | DT_SINGLELINE | DT_VCENTER | DT_LEFT);

	DrawText(hdc, strStatus, -1,  &gRcTableStatus, DT_NOCLIP | DT_SINGLELINE | DT_VCENTER | DT_RIGHT);

	ReleaseDC(hdc);
	
	return 0;
}

/**
	翻页
	改变 startIndex 的值
	@param:bType PAGE_XXX
 */
static int MoveTablePage(BYTE bType)
{
	
	giTrendDataCount = gNibpTrendDatas.wCount;
	
	switch(bType){
		case PAGE_UP:{
			gCurrentIndex += ROW_MAXCOUNT-1;	
		}break;
		case PAGE_DOWN:{
			gCurrentIndex -= ROW_MAXCOUNT-1;	
		}break;
		default:{
			gCurrentIndex = gNibpTrendDatas.wHeadIndex;
		}break;
	}
	
	if(gCurrentIndex > gNibpTrendDatas.wHeadIndex) gCurrentIndex = gNibpTrendDatas.wHeadIndex;
	if(gCurrentIndex < gNibpTrendDatas.wTailIndex) gCurrentIndex = gNibpTrendDatas.wTailIndex;

	return 0;
}

/**
	更性页面数据
	@param:bType PAGE_XXX
 */
static int UpdateNibpPage(BYTE bType)
{
	//当前的数据索引
	MoveTablePage(bType);
 	//更新内容
	UpdateTableDatas(hWndSelf);
	UpdateTableStatus(hWndSelf);
	
	return 0;
}

/*
	根据配置创建趋势列表
*/
static int InitNibpTable(HWND hWnd)
{
	HDC hdc;
	int res;
	
	if(hWnd == (HWND)NULL) return -1;
	
	//根据配置初始化趋势表内容
	InitTableContent(&gTableContent);
	
	//初始化趋势表的相关区域
	res = InitTableRects(hWnd);
	
 	//获得绘图DC
	hdc = GetClientDC(hWnd);
	if(hdc == (HDC)NULL) return -1;
	SelectFont(hdc, gFontSystem);
 	
 	//绘制框架
	res = DrawTableFrame(hdc);
	if(res == -1){
		if(B_PRINTF) printf("%s:%d Draw Table Frame Failure.\n", __FILE__, __LINE__);
		ReleaseDC(hdc);
		return -1;
	}
 			
 	//更新标签
	UpdateTableLabels(hdc);
	
	ReleaseDC(hdc);
	
	//当前的数据索引
	MoveTablePage(PAGE_INIT);
			
	//得到数据量
	giTrendDataCount = gNibpTrendDatas.wCount;
	
 	//更新内容
	UpdateTableDatas(hWnd);
	UpdateTableStatus(hWnd);

	
	return 0;
}

//-------------------------------------------------- interface ----------------------------------------------------
/**
	创建趋势显示对话框
 */
void CreateNibpTable(HWND hWnd)
{

	//初始化趋势图参数
	int High_Dlg;
	if(gCfgSystem.bInterface==SCREEN_NORMAL||gCfgSystem.bInterface==SCREEN_7LEADECG
		||gCfgSystem.bInterface==SCREEN_MULTIECG||gCfgSystem.bInterface==SCREEN_SHORTTREND){
		High_Dlg=0;
	}else
		High_Dlg=HEIGHT_CTL;

#if SCREEN_640
	DlgSet.x = SCREEN_LEFT,
	DlgSet.y = SCREEN_BOTTOM-285, 	
#else
	DlgSet.x = SCREEN_LEFT,
	DlgSet.y = SCREEN_BOTTOM-285-High_Dlg, 		
#endif

	DlgSet.controls = CtrlSet;
	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProcNIBPTable, 0L);    
}

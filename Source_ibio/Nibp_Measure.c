/*************************************************************************** 
 *	Module Name:	Nibp_Measure
 *
 *	Abstract:	NIBP测量程序
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-07-27 11:09:40
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Nibp_Measure.h"
	
#define START_MAUNAL			0
#define START_AUTO				1
#define START_ATAT				2
#define START_STATIC			3
#define START_LEAK				4
#define START_PUMP				5
#define START_OVERPRESSURE	6
#define START_CAL				7


//周期测量线程
pthread_t 	ptNibpAuto;
//连续测量线程
pthread_t 	ptNibpStat;
//筋脉穿刺线程
pthread_t 	ptNibpPuncture;
	
//周期测量定时器信号量
static sem_t semNibpAuto;
//连续测量定时器信号量
static sem_t semNibpStat;
//静脉穿刺信号量
static sem_t semNibpPuncture;

//控制是否取消AUTO测量线程
static BOOL bNibpAutoCancel = TRUE;
//控制是否取消STAT测量线程
static BOOL bNibpStatCancel = TRUE;
//控制是否取消穿刺测量线程
static BOOL bNibpPunctureCancel = TRUE;
	
//复位自动测量计时
static BOOL bNibpAutoReset = TRUE;
//是否已经启动周期测量
static BOOL bNibpAutoHasStart = FALSE;
	
//周期测量时间 显示图形时使用
static unsigned char *strAutoTime[]={"1min", "2min", "3min", "4min", "5min", "10min",
	"15min", "30min", "60min", "90min", "120min", "180min", "240min", "480min"};	
	
//主窗口
extern HWND ghWndMain;
	
//显示区域
extern RECT gRCNibp_InterTime;

/**
	绘制倒计时棒图刻度和进度
	单位:秒
	bType: 0-Auto, 1-Stat, 2-Puncture
*/
int DrawNibpCycleGraph(int iTime, int iMax, BYTE bType)
{
	int iStep;
	
	return 0;
	
	if(iMax >0){
		iStep = (iTime * (gRCNibp_InterTime.right - gRCNibp_InterTime.left) )/ iMax;
		//左边为已过时间
		SetBrushColor(HDC_SCREEN, COLOR_darkblue);
		FillBox(HDC_SCREEN, gRCNibp_InterTime.left, gRCNibp_InterTime.top, iStep, gRCNibp_InterTime.bottom - gRCNibp_InterTime.top);
		//右边为剩余时间
		SetBrushColor(HDC_SCREEN, COLOR_black);
		FillBox(HDC_SCREEN, gRCNibp_InterTime.left+iStep, gRCNibp_InterTime.top, gRCNibp_InterTime.right - (gRCNibp_InterTime.left+iStep), gRCNibp_InterTime.bottom - gRCNibp_InterTime.top);
		
		//剩余时间
		SetTextColor(HDC_SCREEN, gCfgNibp.iColor);
		SetBkColor(HDC_SCREEN, COLOR_black);
		SelectFont(HDC_SCREEN, gFontSystem);
		
		if(bType ==1){
			//Stat
			DrawText(HDC_SCREEN, "5min", -1, &gRCNibp_InterTime, DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE);
		}
		else if(bType ==2) {
	//	if(bType ==2) {
			//Puncture
			if(gCfgNibp.bObject == NIBP_OBJECT_BABY)
				DrawText(HDC_SCREEN, "80s", -1, &gRCNibp_InterTime, DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE);	
			else
				DrawText(HDC_SCREEN, "170s", -1, &gRCNibp_InterTime, DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE);	
		}
		else{
			//Auto or STAT
			DrawText(HDC_SCREEN, strAutoTime[TransformTimeToIndex(gCfgNibp.wInterval)], -1, &gRCNibp_InterTime, DT_NOCLIP | DT_VCENTER | DT_CENTER | DT_SINGLELINE);
		}
		
		//画刻度 : |__|__|__|__|
		SetPenColor(HDC_SCREEN, gCfgNibp.iColor);
		MoveTo(HDC_SCREEN, gRCNibp_InterTime.left, gRCNibp_InterTime.bottom);
		LineTo(HDC_SCREEN, gRCNibp_InterTime.right, gRCNibp_InterTime.bottom);
		
		MoveTo(HDC_SCREEN, gRCNibp_InterTime.left, gRCNibp_InterTime.bottom);
		LineTo(HDC_SCREEN, gRCNibp_InterTime.left, gRCNibp_InterTime.bottom - (gRCNibp_InterTime.bottom-gRCNibp_InterTime.top)/2);
		
		MoveTo(HDC_SCREEN, gRCNibp_InterTime.right, gRCNibp_InterTime.bottom);
		LineTo(HDC_SCREEN, gRCNibp_InterTime.right, gRCNibp_InterTime.bottom - (gRCNibp_InterTime.bottom-gRCNibp_InterTime.top)/2);
				
		//Center
		MoveTo(HDC_SCREEN, gRCNibp_InterTime.left + (gRCNibp_InterTime.right - gRCNibp_InterTime.left)/2, gRCNibp_InterTime.bottom);
		LineTo(HDC_SCREEN, gRCNibp_InterTime.left + (gRCNibp_InterTime.right - gRCNibp_InterTime.left)/2, gRCNibp_InterTime.top);
		
		MoveTo(HDC_SCREEN, gRCNibp_InterTime.left + (gRCNibp_InterTime.right - gRCNibp_InterTime.left)/4, gRCNibp_InterTime.bottom);
		LineTo(HDC_SCREEN, gRCNibp_InterTime.left + (gRCNibp_InterTime.right - gRCNibp_InterTime.left)/4, gRCNibp_InterTime.bottom - (gRCNibp_InterTime.bottom-gRCNibp_InterTime.top)/2);
		
		MoveTo(HDC_SCREEN, gRCNibp_InterTime.left + (gRCNibp_InterTime.right - gRCNibp_InterTime.left)/4*3, gRCNibp_InterTime.bottom);
		LineTo(HDC_SCREEN, gRCNibp_InterTime.left + (gRCNibp_InterTime.right - gRCNibp_InterTime.left)/4*3, gRCNibp_InterTime.bottom - (gRCNibp_InterTime.bottom-gRCNibp_InterTime.top)/2);
		
		Rectangle(HDC_SCREEN, gRCNibp_InterTime.left, gRCNibp_InterTime.top, gRCNibp_InterTime.right, gRCNibp_InterTime.bottom);
	}
	else{
		//清屏
		SetBrushColor(HDC_SCREEN, COLOR_black);
		FillBox(HDC_SCREEN, gRCNibp_InterTime.left, gRCNibp_InterTime.top, gRCNibp_InterTime.right - gRCNibp_InterTime.left+1, gRCNibp_InterTime.bottom - gRCNibp_InterTime.top+1);
	}
	return 0;
}

/**
	开始一次NIBP测量
 */
static int StartSingleNibpMeasure()
{
	//取消状态为假
	gValueNibp.bCanceled = FALSE;
	//获得当前测量的时间
	GetSysTime(&(gValueNibp.sTime));
	//开始测量
	
//	StartiBioNibpMeasure();
	StartNibpMeasure(START_MAUNAL);
	return 0;
}
/**
	开始一次自动测量
 */
static int StartSingleAutoNibpMeasure()
{
	//取消状态为假
	gValueNibp.bCanceled = FALSE;
	//获得当前测量的时间
	GetSysTime(&(gValueNibp.sTime));
	//开始测量
	
//	StartiBioNibpMeasure();
	StartNibpMeasure(START_AUTO);
	return 0;
}

/**
	取消测量
 */
static int CancelSingleNibpMeasure()
{
	printf("stop ++++++++++\n");
	//停止测量
	//CancelNibpMeasure();
//	StopiBioNibpMeasure();
	StopNibpMeasure();
	//取消测量为真
	gValueNibp.bCanceled = TRUE;
	//清除错误代码
	gValueNibp.wErrCode = 0;
//	gValueNibp.bMeasureType=NIBP_TYPE_MANUAL;
	return 0;
}

/**
	开始静脉穿刺
*/
static int StartPuncture()
{
//	NibpLeakTest(gCfgNibp.wInitPres);

	return 0;
}

/**
	取消静脉穿刺
*/
static int CancelPuncture()
{
//	CancelNibpMeasure();
	return 0;
}


//周期测量计时线程
int giNibpTimeCount_Auto = 0;	//计时器
static void *ThreadProcNibpAuto(void *arg)
{
// 	int iTimeCount = 0;	//计时器
	int iTimeMax = 0;		//计时上限

	for(;;){
		sem_wait(&semNibpAuto);
				
		//取消线程
		if(bNibpAutoCancel){
			giNibpTimeCount_Auto = 0;
			iTimeMax = gCfgNibp.wInterval*60;
			DrawNibpCycleGraph(giNibpTimeCount_Auto, iTimeMax, 0);
			if(B_PRINTF) printf("(%s:%d)Cnacel AUTO Test.\n", __FILE__, __LINE__);
			break;
		} 
		
		//复位计时器
		if(bNibpAutoReset){
			giNibpTimeCount_Auto = 0;
			iTimeMax = gCfgNibp.wInterval*60;
			bNibpAutoReset = FALSE;
//  			continue;
		}
		else{
			if(giNibpTimeCount_Auto < iTimeMax){
				giNibpTimeCount_Auto ++;
//  				if(B_PRINTF) printf("%s:%d NIBP Auto Count is %d.\n", __FILE__, __LINE__, iTimeCount);
			}
			else{
				//开始一次NIBP测量
				StartSingleNibpMeasure();
				giNibpTimeCount_Auto = 0;
			}
		}
		DrawNibpCycleGraph(giNibpTimeCount_Auto, iTimeMax, 0);
// 		if(B_PRINTF) printf("(%s:%d)Running AUTO Test, MAX is %d, TimeCount is %d.\n", __FILE__, __LINE__, iTimeMax, giNibpTimeCount_Auto);
	}
	
	giNibpTimeCount_Auto = 0;
	sem_destroy(&semNibpAuto);
	if(B_PRINTF) printf("%s:%d Cancel NIBP Auto Measure thread.\n", __FILE__, __LINE__);
	pthread_exit("Cancel NIBP Auto Measure thread.\n");
}


/**
	开始NIBP自动测量
*/
static int StartAutoNibpMeasure()
{
	int res;
	
	//开始一次NIBP测量
 	StartSingleAutoNibpMeasure();
	
	bNibpAutoCancel = FALSE; 
	res = sem_init(&semNibpAuto, 0, 0);
	res = pthread_create(&ptNibpAuto, NULL, ThreadProcNibpAuto, NULL);
	UpdateNibpAutoTimer(TRUE);
	
	bNibpAutoHasStart = TRUE;
	
	return 0;
}

 int StopNibpAutoMeasure_Error()
{
	if(bNibpAutoHasStart){
		bNibpAutoCancel = TRUE;
		sem_post(&semNibpAuto);
		bNibpAutoHasStart = FALSE;
	}
	return 0;
}

/**
	取消NIBP自动测量
*/
static int CancelNibpAutoMeasure()
{

	if(B_PRINTF) printf("----------------Cancel NIBP .\n");
	bNibpAutoCancel = TRUE;
	sem_post(&semNibpAuto);
	//取消当前的测量
	CancelSingleNibpMeasure();
			
	bNibpAutoHasStart = FALSE;
	return 0;
}

/**
	连续测量线程
*/
int giNibpTimeCount_Stat = 0;	//计时器
static void *ThreadProcNibpStat(void *arg)
{
// 	int iTimeCount = 0;	//计时器
	int iTimeMax = NIBP_STAT_TIME;	//计时上限
	int iTimeInter = 0;	//两次测量之间的间隔
	
	//启动一次NIBP测量
	StartSingleAutoNibpMeasure();	
	
	//计数器复位
	giNibpTimeCount_Stat = 0;
	
	for(;;){
		sem_wait(&semNibpStat);
	
		//取消线程
		if(bNibpStatCancel){
			giNibpTimeCount_Stat = 0;
			iTimeMax = NIBP_STAT_TIME;
		//	iTimeMax = gCfgNibp.wInterval*60;
			DrawNibpCycleGraph(giNibpTimeCount_Stat, iTimeMax, 1);
			break;
		}
		if(giNibpTimeCount_Stat < iTimeMax){
			giNibpTimeCount_Stat ++;
 		//	if(B_PRINTF) printf("%s:%d NIBP Stat Count is %d.\n", __FILE__, __LINE__, iTimeCount);
			//判断上次测量是否结束,如果结束,计数X秒 启动下次测量,否则继续计数
		
			if(gValueNibp.bStatus== NIBP_SYSTEM_IDLE){
				//如果上次测量出错，则立即停止测量
				if(gValueNibp.wErrCode !=0) {
					if(B_PRINTF) printf("%s:%d NIBP Stat Error is %d.\n", __FILE__, __LINE__, gValueNibp.wErrCode);
					break;
				}
				iTimeInter ++;
				//间隔两秒钟
				if(iTimeInter > NIBP_STAT_INTER){
					iTimeInter =0;
					//启动下次测量
					StartSingleAutoNibpMeasure();	
					if(B_PRINTF) printf("Next NIBP measuring.\n");
				}	
				else{
					if(B_PRINTF) printf("Time Inter is %d\n", iTimeInter);
				}
			}
			else{
				iTimeInter = 0;
			}
		}
		else{
			//判断上次测量是否结束,如果结束立即取消线程,否则,等待该次测量结束后,取消线程
			while(gValueNibp.bStatus!= NIBP_SYSTEM_IDLE){
				//if(B_PRINTF) printf("NIBP still running.\n");	
			}
			DrawNibpCycleGraph(giNibpTimeCount_Stat, iTimeMax,1);
			break;
		}
		
		DrawNibpCycleGraph(giNibpTimeCount_Stat, iTimeMax,1);
	}

	//清空区域
	DrawNibpCycleGraph(0, -1, 0);
	
	giNibpTimeCount_Stat = 0;
			
	sem_destroy(&semNibpStat);
	if(B_PRINTF) printf("%s:%d Cancel NIBP STAT Measure thread.\n", __FILE__, __LINE__);
	pthread_exit("Cancel NIBP STAT Measure thread.\n");
}


/**
	开始NIBP连续测量
 */
static int StartStatNibpMeasure()
{
	int res;
	bNibpStatCancel = FALSE; 
	if(B_PRINTF) printf("start star nibp measure~~bNibpStatCancel=%d~~~~~~~~~~~\n",bNibpStatCancel);
	
	res = sem_init(&semNibpStat, 0, 0);
	res = pthread_create(&ptNibpStat, NULL, ThreadProcNibpStat, NULL);
	
	return 0;
}

/**
	取消NIBP自动测量
 */
static int CancelNibpStatMeasure()
{

	bNibpStatCancel= TRUE;
	sem_post(&semNibpStat);
	//取消当前的测量
	CancelSingleNibpMeasure();
	
	return 0;
}

/**
	静脉穿刺线程
 */
int  giNibpTimeCount_Punc = 0;
static void *ThreadProcNibpPuncture(void *arg)
{
// 	int iTimeCount = 0;	//计时器
	int iTimeMax = NIBP_PUNCTURE_TIME_ADULT;	//计时上限
	int iTimeInter = 0;	//两次测量之间的间隔
	
	if(gCfgNibp.bObject == NIBP_OBJECT_BABY){
		iTimeMax = NIBP_PUNCTURE_TIME_BABY;
	}
	else{
		iTimeMax = NIBP_PUNCTURE_TIME_ADULT;
	}
	
	
	//启动穿刺
//	StartPuncture();	
	
	//计数器复位
	giNibpTimeCount_Punc = 0;
	
	for(;;){
		sem_wait(&semNibpPuncture);
		
		//取消线程
		if(bNibpPunctureCancel){
			giNibpTimeCount_Punc = 0;
			DrawNibpCycleGraph(giNibpTimeCount_Punc, iTimeMax, 2);
			break;
		}
				
		if(giNibpTimeCount_Punc < iTimeMax){
			giNibpTimeCount_Punc ++;
// 			if(B_PRINTF) printf("Puncture Timer %d.\n", giNibpTimeCount_Punc);
		}
		else{
			//取消穿刺
			CancelPuncture();
// 			if(B_PRINTF) printf("Puncture Timeout %d.\n", iTimeCount);
			DrawNibpCycleGraph(giNibpTimeCount_Punc, iTimeMax,2);
			break;
		}
		
		DrawNibpCycleGraph(giNibpTimeCount_Punc, iTimeMax,2);
	}

	//清空区域
	DrawNibpCycleGraph(0, -1, 0);
	
	giNibpTimeCount_Punc = 0;
			 
	sem_destroy(&semNibpPuncture);
	if(B_PRINTF) printf("%s:%d Cancel NIBP Puncture Measure thread.\n", __FILE__, __LINE__);
	pthread_exit("Cancel NIBP Puncture Measure thread.\n");
}

/**
	开始NIBP穿刺
 */
static int StartPunctureNibpMeasure()
{
	int res;
	
	bNibpPunctureCancel = FALSE; 
	res = sem_init(&semNibpPuncture, 0, 0);
	res = pthread_create(&ptNibpPuncture, NULL, ThreadProcNibpPuncture, NULL);
	
	return 0;
}

/**
	取消NIBP自动测量
 */
static int CancelNibpPunctureMeasure()
{

	bNibpPunctureCancel= TRUE;
	sem_post(&semNibpPuncture);
	//取消当前的测量
	CancelPuncture();
	
	return 0;
}


//--------------------------------------- interface ----------------------------------------
/**
	定时器通知线程更新
	bReset : 是否需要复位
*/
int UpdateNibpAutoTimer(BOOL bReset)
{
	if(bNibpAutoCancel) return -1;
	
	bNibpAutoReset = bReset;
	sem_post(&semNibpAuto);
	
	return 0;
}

/**
	定时器通知线程更新
 */
int UpdateNibpStatTimer()
{
	if(bNibpStatCancel) return -1;
	
	sem_post(&semNibpStat);
	
	return 0;
}

/**
	定时器通知线程更新
 */
int UpdateNibpPunctureTimer()
{
	if(bNibpPunctureCancel) return -1;
	
	sem_post(&semNibpPuncture);
	
	return 0;
}

//根据情况启动NIBP测量
int NIBP_Start()
{
	//判断当前的状态
	switch(gValueNibp.bStatus){
		case NIBP_SYSTEM_IDLE:{
			switch(gCfgNibp.bMode){
				case NIBP_MODE_AUTO:{
					//判断是否已经启动周期测量,如果是,则启动单次测量,复位计数器
					if(bNibpAutoHasStart){
						//单次测量
						StartSingleAutoNibpMeasure();
						//复位计数器
						UpdateNibpAutoTimer(TRUE);
					}
					else{
						//否则,启动周期测量
						StartAutoNibpMeasure();
					}
				}break;
				case NIBP_MODE_SERIES:{
					//如果已经开始自动测量,则取消自动测量
					if(bNibpAutoHasStart){
						//取消自动测量
						CancelNibpAutoMeasure();
					}
					//启动连续测量
					StartStatNibpMeasure();
				}break;
				case NIBP_MODE_PUNCTURE:{
					if(bNibpAutoHasStart){
						//取消自动测量
						CancelNibpAutoMeasure();
					}
					//启动穿刺
					StartPunctureNibpMeasure();	
				}break;
				default:{
					//单次测量
					StartSingleNibpMeasure();
				}break;
			}
		}break;
		default:{
			//正在从测量中
		}break;
	}
	
	return 0;
}

//根据情况取消测量
int NIBP_Cancel()
{
	
	//判断当前的状态
	switch(gValueNibp.bStatus){
		printf("111111\n");
		case NIBP_SYSTEM_IDLE:{
			//判断是否已经启动周期测量
			if(bNibpAutoHasStart){
				//取消自动测量
				CancelNibpAutoMeasure();
			}	
			CancelNibpAutoMeasure();
		}break;
		default:{
				printf("12222222\n");
			//判断类型
			switch(gCfgNibp.bMode){
				case NIBP_MODE_AUTO:{
					//取消自动测量
					CancelNibpAutoMeasure();
				}break;
				case NIBP_MODE_SERIES:{
					//取消连续测量
					CancelNibpStatMeasure();
				}break;
				case NIBP_MODE_PUNCTURE:{
					//取消穿刺
					CancelNibpPunctureMeasure();
				}break;
				default:{
					//取消单次测量
					CancelSingleNibpMeasure();	
				}break;
			}
		}break;
	}
	
	return 0;
}


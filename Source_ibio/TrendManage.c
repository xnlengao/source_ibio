/*************************************************************************** 
 *	Module Name:	TrendManage
 *
 *	Abstract:	趋势数据管理程序
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-07-12 09:16:55
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "General_Funcs.h"
#include "Global.h"


//主窗口句柄
extern HWND ghWndMain;

#define TRENDFILE_NAME		TRENDPATH

//趋势自动存储的周期，单位：秒
#define AUTOSAVETREND_CYCLE	60	
// #define AUTOSAVETREND_CYCLE	1

//趋势数组
CA_TRENDARRAY gTrendDatas;
CA_SHORTTREND gShortTrendDatas;

//趋势文件是否已经建立,如果没有建立,需要初始化趋势数据
BOOL gbTrendFileExist = FALSE;

//是否可以开始数据存储,用来等待数据加载
BOOL gbCanRecordTrend = FALSE;

//趋势存储线程
static pthread_t ptSaveTrend;	
//趋势存储互斥量
static pthread_mutex_t mtSaveTrend;
extern sem_t semCalctime;


//记录一条趋势数据
int  RecOneTrendRecord(BYTE bLogFlag);
static int TrendArray_Add(const PTRENDDATA pData);
//生成调试使用的数据
static BOOL bDebugData = FALSE;

static TRENDDATA tmpTrend1;
int  AddNullRecord(BYTE bLogFlag)
{
	//单条趋势数据
	//TRENDDATA tmpTrend;
	//记录时间
	S_TIME sTime;
	//时间偏移量
	long int liTimeOffset=0;
	

	
	//获取存储时间
	GetSysTime(&sTime);
	
	//初始化报警数据
	memset(&tmpTrend1, 0, sizeof tmpTrend1);
	
	//计算此次记录的时间与起始时间的偏移量, 精确到秒
	//liTimeOffset = CompTime(&sTime, &gTrendDatas.sStartTime);

	tmpTrend1.liOffset +=1 ;
	tmpTrend1.sTime = sTime;
 		
 	//记录原因
 	tmpTrend1.bLogFlag = bLogFlag;
 	//数据有效标志
 	tmpTrend1.dwHaveFlag = 0;
 	

	
 	//HR
	if(bDebugData){
 		tmpTrend1.wHR = 0;
		tmpTrend1.dwHaveFlag |= HAVEHR;
	}
	else{
		if(gValueEcg.wHR !=0){
			tmpTrend1.wHR = 0; 
			tmpTrend1.dwHaveFlag |= HAVEHR;
		}
		else if(bLogFlag == TRENDNIBP){
			tmpTrend1.wHR =0; 
			tmpTrend1.dwHaveFlag |= HAVEHR;
			
			}
			
	}
 	
	//RR
	if(bDebugData){
		tmpTrend1.wRR = 0;
		tmpTrend1.dwHaveFlag |= HAVERESP;
	}
	else{
		if(!gValueResp.bOff && gValueResp.wRR!=0 ){
			tmpTrend1.wRR = 0;
			tmpTrend1.dwHaveFlag |= HAVERESP;
		}
	}
 	
 	//SpO2
	if(bDebugData){
		tmpTrend1.bSpO2 = 0;
		tmpTrend1.dwHaveFlag |= HAVESPO2;
	}
	else{
		if(gValueSpO2.bSpO2 !=0 && !gValueSpO2.bOff){
			tmpTrend1.bSpO2 = 0;
			tmpTrend1.dwHaveFlag |= HAVESPO2;
		}
	}
	
	//PR
	if(bDebugData){
		tmpTrend1.wPR = 0;
		tmpTrend1.dwHaveFlag |= HAVEPR;	
	}
	else{
		if(gValuePulse.wPR !=0 && !gValuePulse.bOff){
			tmpTrend1.wPR = 0;	
			tmpTrend1.dwHaveFlag |= HAVEPR;
		}
	}
	
 	//Temp1
	if(!gValueTemp.bOff_T1){
		tmpTrend1.wTemps[0] = 0;
 		tmpTrend1.dwHaveFlag |= HAVET1;
 	}	
	//Temp2
	if(!gValueTemp.bOff_T2){
		tmpTrend1.wTemps[1] =0;
		tmpTrend1.dwHaveFlag |= HAVET2;
	}	
	
	//CO2
	if(gbHaveCo2 
		  && gValueCO2.wEtCO2 !=0 
		  && gValueCO2.bStatus ==CO2_STATUS_RUN ){
		tmpTrend1.sCO2.wEtCO2 =	0;
		tmpTrend1.sCO2.wFiCO2 = 	0;
		tmpTrend1.sCO2.wAwRR = 	0;
		tmpTrend1.dwHaveFlag |= HAVECO2;
	}	
	//IBP1
	if(gbHaveIbp && !gValueIbp1.bOff){
		tmpTrend1.sIBPs[0].iSys = 0;
		tmpTrend1.sIBPs[0].iDia = 0;
		tmpTrend1.sIBPs[0].iMean = 0;
		tmpTrend1.dwHaveFlag |= HAVEIBP1;

	}
	//IBP2
	if(gbHaveIbp && !gValueIbp2.bOff){
		tmpTrend1.sIBPs[1].iSys = 0;
		tmpTrend1.sIBPs[1].iDia = 0;
		tmpTrend1.sIBPs[1].iMean = 0;
		tmpTrend1.dwHaveFlag |= HAVEIBP2;

	}
	
 	//NIBP
 	if(bLogFlag == TRENDNIBP){
		if(bDebugData){
			tmpTrend1.sNIBP.wSys = 0;
			tmpTrend1.sNIBP.wDia = 0;
			tmpTrend1.sNIBP.wMean= 0;
			tmpTrend1.dwHaveFlag |= HAVENIBP;	
		}
		else{
			if(IS_RANGE(gValueNibp.wSys, NIBP_MIN+1, NIBP_MAX) 
				&& IS_RANGE(gValueNibp.wDia, NIBP_MIN+1, NIBP_MAX)
				&& IS_RANGE(gValueNibp.wMean, NIBP_MIN+1, NIBP_MAX)){
			
				tmpTrend1.sNIBP.wSys = 0;
				tmpTrend1.sNIBP.wDia =0;
				tmpTrend1.sNIBP.wMean=0;
				tmpTrend1.dwHaveFlag |= HAVENIBP;
				
			}
		}
 	}

	//添加到趋势数组
	TrendArray_Add(&tmpTrend1);

	return 0;
}








/**
	添加一条数据到数组,在HeadIndex处添加最新的数据
	注意:按此法存储,未翻转前,数组索引[0]处无数据,读取数据时要注意
*/
static int TrendArray_Add(const PTRENDDATA pData)
{
	WORD wHead, wTail, wSize, wCount;
	BOOL bLoop;
	
	if(pData == NULL) return -1;
	
	wHead = gTrendDatas.wHeadIndex;
	wTail = gTrendDatas.wTailIndex;
	bLoop = gTrendDatas.bLoop;
	wSize = MAXTRENDCOUNT;
	

	if(wHead < (wSize-1)){
		wHead ++;

		if(bLoop){
			//如果是翻转的情况
			if(wTail < (wSize-1)){
				wTail ++;

			}
			else{
				wTail = 0;
			}
		}
	}
	else{
		//数据已经存满,翻转
		wHead = 0;
		wTail = wHead +1;
		bLoop = TRUE;
	}
	
	gTrendDatas.wHeadIndex = wHead;
	gTrendDatas.wTailIndex = wTail;
	gTrendDatas.bLoop = bLoop;
	gTrendDatas.Datas[gTrendDatas.wHeadIndex] = *pData;
	//计算实际存储的数量
	if(gTrendDatas.wHeadIndex < gTrendDatas.wTailIndex){
		//数据已经存满,并翻转
		gTrendDatas.wCount = MAXTRENDCOUNT;
	}
	else{
		gTrendDatas.wCount +=1;
	}

	return wHead;
}

/**
	添加一条数据到短趋势数组,在Head处添加最新的数据
	注意:按此法存储,未翻转前,数组索引[0]处无数据,读取数据时要注意
*/
static int ShortTrendArray_Add(const PTRENDDATA pData)
{
	WORD wHead;
	BOOL bFull;
	
	if(pData == NULL) return -1;

	wHead = gShortTrendDatas.wHead;
	bFull = gShortTrendDatas.bFull;

	if(wHead < (MAXSHORTTREND-1)){
		wHead ++;
		
	}else{
		//数据已经存满,翻转
		wHead = 0;
		bFull= TRUE;
	}
	
	gShortTrendDatas.wHead = wHead;
	gShortTrendDatas.bFull = bFull;
	gShortTrendDatas.Datas[wHead] = *pData;
	
}
/**
	保存趋势到FLash
	设计思路：
		1、以趋势存储的病人姓名为文件名保存
		2、后缀固定为 .ttt
*/


int SaveTrendToFlash(PCA_TRENDARRAY pTrendDatas)
{
	unsigned char strFileName[30] = {0};
	FILE *fpTrendData;
	int res;
	
	if(B_PRINTF)printf("%s:%d Start Save Trend Data.\n", __FILE__, __LINE__);
	if(pTrendDatas == NULL) return -1;
	//设置文件名
	snprintf(strFileName, sizeof strFileName, TRENDFILE_NAME, pTrendDatas->sPatientInfo.sName);
	
	gbTrendFileWrite_OK = 0;
	SetIntValueToResFile(gFileSetup, "FLASH_TREND", "saveok",  gbTrendFileWrite_OK, 1);
	fpTrendData = fopen(strFileName, "w+");
	if(fpTrendData == NULL) return -1;
	
	//写文件
	res= fwrite(pTrendDatas, sizeof(CA_TRENDARRAY), 1, fpTrendData); 
	
	fclose(fpTrendData);
	if(B_PRINTF)printf("%s:%d Save Trend to File %s  OK, size: %d.\n", __FILE__, __LINE__, strFileName, sizeof(CA_TRENDARRAY));
	//增加一个写成功的标志，预防突然断电时，没有完全写入文件，导致文件损坏，无法开机  2008-04-16 17:56:44
	gbTrendFileWrite_OK = 1;
	SetIntValueToResFile(gFileSetup, "FLASH_TREND", "saveok",  gbTrendFileWrite_OK, 1);
	
	return 0;
}

/**
	加载数据至数组
*/
static int LoadTrendToArray()
{
	int fd,ii;
	int InterValue=0,InterValues=0;
	struct stat fd_stat;	//文件信息
	PCA_TRENDARRAY pTrendDatas;
	CA_TRENDARRAY   TmpValue;
	S_TIME			TmpTime,TmpCurTime,TmpTimes;
				
	if(B_PRINTF)printf("%s:%d Loading Trend File (%s) ... ... \n", __FILE__, __LINE__, TRENDFILE_NAME);
	
	fd = open(TRENDFILE_NAME, O_RDONLY);
	if(fd<0){
		if(B_PRINTF)printf("(%s:%d)Read file %s failure.\n", __FILE__, __LINE__, TRENDFILE_NAME);
		return -1;
	}
		

	//映射内存
 	pTrendDatas = (PCA_TRENDARRAY)mmap(0, sizeof(CA_TRENDARRAY), PROT_READ, MAP_PRIVATE, fd, 0); 
	

			
	if(pTrendDatas == MAP_FAILED){
		if(B_PRINTF)printf("(%s:%d)MAP_FAILED.\n", __FILE__, __LINE__);
		close(fd);
		return -1;	
	} 
	memset(&TmpValue, 0, sizeof(CA_TRENDARRAY));
 	memcpy(&TmpValue, pTrendDatas, sizeof(CA_TRENDARRAY));
	TmpTime=TmpValue.Datas[TmpValue.wHeadIndex].sTime;
	printf("~~~%d  %d   %d  %d  %d  %d  \n " , TmpTime.wYear,TmpTime.bMonth ,TmpTime.bDay,TmpTime.bHour,TmpTime.bMin,TmpTime.bSec);
	GetSysTime(&TmpCurTime);
	TmpTimes=TmpCurTime;
	printf("~~~%d  %d   %d  %d  %d  %d  \n " , TmpCurTime.wYear,TmpCurTime.bMonth ,TmpCurTime.bDay,TmpCurTime.bHour,TmpCurTime.bMin,TmpCurTime.bSec);
	InterValue = ((int)CompTime(&TmpTimes , &TmpTime))/60;

	printf("~~~!!!%d~~!!!\n",InterValue);

	if(InterValue>60*24*5){
			memset(&gTrendDatas, 0, sizeof(CA_TRENDARRAY));
			InitTrendArray();
			printf(" da  yu  san shi miao!!!!da  yu  san shi miao!!!!da  yu  san shi miao!!!!da  yu  san shi miao!!!!\n");
		}
	else{
	//拷贝内存
			memset(&gTrendDatas, 0, sizeof(CA_TRENDARRAY));	
 			memcpy(&gTrendDatas, pTrendDatas, sizeof(CA_TRENDARRAY));
			for(ii=1;ii<InterValue;ii++){
					AddNullRecord(0x01);
				}
			
			printf(" kao bei zhong  !!!!!!kao bei zhong  !!!!!! kao bei zhong  !!!!!!\n");


		}
	


	//释放内存
 	munmap((PCA_TRENDARRAY)pTrendDatas, sizeof(CA_TRENDARRAY));

	
	close(fd);
	
 	if(B_PRINTF)printf("OK!\n");
	
	if(B_PRINTF)printf("%s:%d Trend Data -- Name %s, count :%d, %.2d-%.2d %.2d:%.2d:%.2d head:%d, tail:%d.\n",
	       __FILE__, __LINE__, 
	       gTrendDatas.sPatientInfo.sName,
	       gTrendDatas.wCount, 
	       gTrendDatas.sStartTime.bMonth, 
	       gTrendDatas.sStartTime.bDay, 
	       gTrendDatas.sStartTime.bHour, 
	       gTrendDatas.sStartTime.bMin, 
	       gTrendDatas.sStartTime.bSec,
	       gTrendDatas.wHeadIndex, 
	       gTrendDatas.wTailIndex);
	
	return 0;
}

/**
	从Flash加载趋势数据
	程序启动时, 如果flash上有趋势数据，则加载趋势数据，并在此基础上添加新的趋势数据
	否则, 置初始化标志, 初始化趋势数据
	@return 0:成功, -1文件不存在,或者没有趋势数据

*/
static int LoadTrendFromFlash(PCA_TRENDARRAY pTrendDatas)
{
	FILE *fp = NULL;
	struct stat statConfig;
	
	if(pTrendDatas == NULL) return -1;
	
	
	gbCanRecordTrend = FALSE;
	
	//检查文件是否存在
	fp = fopen(TRENDFILE_NAME, "r");
	
	if(fp == NULL){
	if(B_PRINTF)	printf("%s:%d File %s not exist.\n", __FILE__, __LINE__, TRENDFILE_NAME);
		gbTrendFileExist = FALSE;
		
		gbCanRecordTrend = TRUE;
		return -1;
	}else{
		
		//获得存储是否成功的标志，否则清除文件
		GetIntValueFromEtcFile(gFileSetup, "FLASH_TREND", "saveok", &gbTrendFileWrite_OK);
		if(!gbTrendFileWrite_OK){
			//文件存储不成功，建立新的空白文件
		if(B_PRINTF)	printf("%s:%d The trend file may be Error.\n", __FILE__, __LINE__);
			gbTrendFileExist = FALSE;
			
			gbCanRecordTrend = TRUE;
			
			fclose(fp);
			return -1;
		}
		else{
			//根据文件的大小判断文件是否损坏	
			fstat(fileno(fp), &statConfig);
	                //-- 如果文件小于20Byte,则判断文件损坏
			if(statConfig.st_size <1){
				if(B_PRINTF)printf("%s:%d File %s, size %ld. Error!.\n", __FILE__, __LINE__, TRENDFILE_NAME, statConfig.st_size);
				gbTrendFileExist = FALSE;
			
				gbCanRecordTrend = TRUE;
			
				fclose(fp);
				return -1;
			}
			else{
				if(B_PRINTF)printf("%s:%d File %s OK!.\n", __FILE__, __LINE__, TRENDFILE_NAME);
				gbTrendFileExist = TRUE;
			}

		}
		
		fclose(fp);
	}

	//加载数据
	if(gbTrendFileExist)
	 	LoadTrendToArray();
	
	gbCanRecordTrend = TRUE;
	return 0;
}

/**
	自动趋势存储线程
	设计思路：
		1、间隔为1秒钟的循环，因为要查询是否正在关机
		2、利用计数器保存趋势
*/
//调试电源关闭
extern BOOL gbTestPowerDown;
static void *ThreadProcSaveTrend(void *arg)
{
	struct timeval timeout;
	int delay;
	int iCount = AUTOSAVETREND_CYCLE,iCountTime=0;
	int temp;
		
	for(;;){
 		if(iCountTime>2*60){
			//把当前趋势存储在Flash
  			SaveTrendToFlash(&gTrendDatas);
			iCountTime=1;
		}
		else{
			//把趋势存储在内存
			if(iCount >=AUTOSAVETREND_CYCLE){
			//if(iCount >=2){
				//加锁
				pthread_mutex_lock(&mtSaveTrend);
				RecOneTrendRecord(TRENDAUTO);
				//解锁
				pthread_mutex_unlock(&mtSaveTrend);
				iCount = 1;
			}	
		}

		timeout.tv_sec = 1;
		timeout.tv_usec= 0;
		delay = select(FD_SETSIZE, NULL, NULL, NULL, &timeout);
		iCount +=1;
		iCountTime+=1;
		
	}
}


//-------------------------- interface ------------------------------
/**
	初始化趋势数组
 */
int InitTrendArray()
{
	S_TIME sTime;
	
	//加锁
	pthread_mutex_lock(&mtSaveTrend);
	
	//获取起始时间
	GetSysTime(&sTime);
	gTrendDatas.sStartTime = sTime;
	//获取病人信息
	gTrendDatas.sPatientInfo = gCfgPatient;
	//初始化趋势数据
	gTrendDatas.wHeadIndex = 0;
	gTrendDatas.wTailIndex = 0;
	gTrendDatas.wCount = 0;
	gTrendDatas.bLoop  = FALSE;

	//初始化短趋势数据
	gShortTrendDatas.wHead= 0;
	gShortTrendDatas.bFull= FALSE;
	
	//解锁
	pthread_mutex_unlock(&mtSaveTrend);
	
	return 0;
}

/**
	记录一条趋势数据
	bLogFlag:记录的原因
*/
//单条趋势数据
static TRENDDATA tmpTrend;
int  RecOneTrendRecord(BYTE bLogFlag)
{
	//单条趋势数据
	//TRENDDATA tmpTrend;
	//记录时间
	S_TIME sTime;
	//时间偏移量
	long int liTimeOffset=0;
	
	//判断是否可以加载数据
	if(!gbCanRecordTrend) {
		if(B_PRINTF)printf("Cannt Record trend data.\n");
		return -1;
	}
	
	//获取存储时间
	GetSysTime(&sTime);
	
	//初始化报警数据
	memset(&tmpTrend, 0, sizeof tmpTrend);
	
	//计算此次记录的时间与起始时间的偏移量, 精确到秒
	liTimeOffset = CompTime(&sTime, &gTrendDatas.sStartTime);
//	printf("###%d  %d   %d  %d  %d  %d###  \n " , sTime.wYear,sTime.bMonth ,sTime.bDay,sTime.bHour,sTime.bMin,sTime.bSec);
//	printf("###%d  %d   %d  %d  %d  %d###  \n " , gTrendDatas.sStartTime.wYear,gTrendDatas.sStartTime.bMonth ,
//		gTrendDatas.sStartTime.bDay,gTrendDatas.sStartTime.bHour,gTrendDatas.sStartTime.bMin,gTrendDatas.sStartTime.bSec);

	//(gTrendDatas.Datas[gTrendDatas.wHeadIndex].sTime)

	tmpTrend.liOffset = liTimeOffset;
	tmpTrend.sTime = sTime;
 		
 	//记录原因
 	tmpTrend.bLogFlag = bLogFlag;
 	//数据有效标志
 	tmpTrend.dwHaveFlag = 0;
 	
 	//获取数据
 	//HR
	if(bDebugData){
 		tmpTrend.wHR = 50+(long int)(50.0*rand()/(RAND_MAX+1.0));
		tmpTrend.dwHaveFlag |= HAVEHR;
	}
	else{
		if(gValueEcg.wHR !=0){
			tmpTrend.wHR = gValueEcg.wHR; 
			tmpTrend.dwHaveFlag |= HAVEHR;
		}
		else if(bLogFlag == TRENDNIBP){
			tmpTrend.wHR = gValueNibp.wPR; 
			tmpTrend.dwHaveFlag |= HAVEHR;
			
			}
			
	}
 	
	//RR
	if(bDebugData){
		tmpTrend.wRR = 15+(long int)(10.0*rand()/(RAND_MAX+1.0));
		tmpTrend.dwHaveFlag |= HAVERESP;
	}
	else{
		if(!gValueResp.bOff && gValueResp.wRR!=0 ){
			tmpTrend.wRR = gValueResp.wRR;
			tmpTrend.dwHaveFlag |= HAVERESP;
		}
	}
 	
 	//SpO2
	if(bDebugData){
		tmpTrend.bSpO2 = 95+(long int)(5.0*rand()/(RAND_MAX+1.0));
		tmpTrend.dwHaveFlag |= HAVESPO2;
	}
	else{
		if(gValueSpO2.bSpO2 !=0 && !gValueSpO2.bOff){
			tmpTrend.bSpO2 = gValueSpO2.bSpO2;
			tmpTrend.dwHaveFlag |= HAVESPO2;
		}
	}
	
	//PR
	if(bDebugData){
		tmpTrend.wPR = 50+(long int)(50.0*rand()/(RAND_MAX+1.0));
		tmpTrend.dwHaveFlag |= HAVEPR;	
	}
	else{
		if(gValuePulse.wPR !=0 && !gValuePulse.bOff){
			tmpTrend.wPR = gValuePulse.wPR;	
			tmpTrend.dwHaveFlag |= HAVEPR;
		}
	}
	
 	//Temp1
	if(!gValueTemp.bOff_T1){
		tmpTrend.wTemps[0] = gValueTemp.wT1;
 		tmpTrend.dwHaveFlag |= HAVET1;
 	}	
	//Temp2
	if(!gValueTemp.bOff_T2){
		tmpTrend.wTemps[1] = gValueTemp.wT2;
		tmpTrend.dwHaveFlag |= HAVET2;
	}	
	
	//CO2
	if(gbHaveCo2 
		  && gValueCO2.wEtCO2 !=0 
		  && gValueCO2.bStatus ==CO2_STATUS_RUN ){
		tmpTrend.sCO2.wEtCO2 = gValueCO2.wEtCO2;
		tmpTrend.sCO2.wFiCO2 = gValueCO2.wFiCO2;
		tmpTrend.sCO2.wAwRR = gValueCO2.wAwRR;
		tmpTrend.dwHaveFlag |= HAVECO2;
	}	
	//IBP1
	if(gbHaveIbp && !gValueIbp1.bOff){
		tmpTrend.sIBPs[0].iSys = gValueIbp1.iSys;
		tmpTrend.sIBPs[0].iDia = gValueIbp1.iDia;
		tmpTrend.sIBPs[0].iMean = gValueIbp1.iMean;
		tmpTrend.dwHaveFlag |= HAVEIBP1;
// 		printf("Save Trend IBP1 : %d / %d   %d.\n ", gValueIbp1.iSys, gValueIbp1.iDia, gValueIbp1.iMean);
	}
	//IBP2
	if(gbHaveIbp && !gValueIbp2.bOff){
		tmpTrend.sIBPs[1].iSys = gValueIbp2.iSys;
		tmpTrend.sIBPs[1].iDia = gValueIbp2.iDia;
		tmpTrend.sIBPs[1].iMean = gValueIbp2.iMean;
		tmpTrend.dwHaveFlag |= HAVEIBP2;
// 		printf("Save Trend IBP2 : %d / %d   %d.\n ", gValueIbp2.iSys, gValueIbp2.iDia, gValueIbp2.iMean);
	}
	
 	//NIBP
 	if(bLogFlag == TRENDNIBP){
		if(bDebugData){
			tmpTrend.sNIBP.wSys = 70+(long int)(80.0*rand()/(RAND_MAX+1.0));
			tmpTrend.sNIBP.wDia = 30+(long int)(20.0*rand()/(RAND_MAX+1.0));
			tmpTrend.sNIBP.wMean= (tmpTrend.sNIBP.wSys + tmpTrend.sNIBP.wDia)/2;
			tmpTrend.dwHaveFlag |= HAVENIBP;	
		}
		else{
			if(IS_RANGE(gValueNibp.wSys, NIBP_MIN+1, NIBP_MAX) 
				&& IS_RANGE(gValueNibp.wDia, NIBP_MIN+1, NIBP_MAX)
				&& IS_RANGE(gValueNibp.wMean, NIBP_MIN+1, NIBP_MAX)){
			
				tmpTrend.sNIBP.wSys = gValueNibp.wSys;
				tmpTrend.sNIBP.wDia = gValueNibp.wDia;
				tmpTrend.sNIBP.wMean= gValueNibp.wMean;
				//if(gValueEcg.wHR == 0){
				//tmpTrend.wHR = gValueNibp.wPR; 
					//}
				tmpTrend.dwHaveFlag |= HAVENIBP;
				
			}
		}
 	}

	//添加到趋势数组
	TrendArray_Add(&tmpTrend);
	 ShortTrendArray_Add(&tmpTrend);	
//  	printf("%s : %d Trend data start time is %.4d/%.2d/%.2d %.2d:%.2d:%.2d\n", __FILE__, __LINE__, 
//  		 gTrendDatas.sStartTime.wYear, gTrendDatas.sStartTime.bMonth, gTrendDatas.sStartTime.bDay, 
//  		 gTrendDatas.sStartTime.bHour, gTrendDatas.sStartTime.bMin, gTrendDatas.sStartTime.bSec);
		
//   	printf("%s : %d Current Trend data time is %.4d/%.2d/%.2d %.2d:%.2d:%.2d, Offset is %d. T1 is %d.\n", __FILE__, __LINE__, 
//   		 sTime.wYear, sTime.bMonth, sTime.bDay, 
// 		 sTime.bHour, sTime.bMin, sTime.bSec, liTimeOffset, tmpTrend.wTemps[0]);

// 	printf("%s:%d Save Trend Flag %d, Total %d records. Head:%d, Tail:%d.\n", __FILE__, __LINE__, 
// 	       bLogFlag, gTrendDatas.wCount, gTrendDatas.wHeadIndex, gTrendDatas.wTailIndex);
	
	return 0;
}

/**
	读取趋势数据的例子
	按照由早到晚的顺序取出数据
*/
int TrendArray_Get()
{
	WORD wHead, wTail, wSize;
	BOOL bLoop;
	int i;
	
	wHead = gTrendDatas.wHeadIndex;
	wTail = gTrendDatas.wTailIndex;
	bLoop = gTrendDatas.bLoop;
	wSize = MAXTRENDCOUNT;
	
	if(B_PRINTF)printf("--------%s:%d Get Time Offset(Up)head %d tail %d : ", __FILE__, __LINE__, wHead, wTail);
	if(wHead < wTail){
		printf(" Rollback \n");
		for(i=wTail; i<=(wSize-1); i++){
			printf(" %ld ", gTrendDatas.Datas[i].liOffset);
		}
		printf("\n");
		for(i=0; i<=wHead; i++){
			printf(" %ld ", gTrendDatas.Datas[i].liOffset);
		}
	}
	else if(bLoop){
		printf(" Spacial \n");
// 		for(i=wTail; i<=(wHead-1); i++){
// 			printf(" %d ", gTrendDatas.Datas[i].liOffset);
// 		}
// 		printf("\n");
// 		printf(" %d ", gTrendDatas.Datas[wHead].liOffset);
		for(i=wTail; i<=wHead; i++){
			printf(" %ld ", gTrendDatas.Datas[i].liOffset);
		}
	}
	else{
		printf(" Normal ");
		for(i=(wTail+1); i<=wHead; i++){
			printf(" %ld ", gTrendDatas.Datas[i].liOffset);
		}
	}
	
	printf("\n");
	
	return 0;
}

/**
	读取趋势数据的例子
	按照由晚到早的顺序取出数据
 */
int TrendArray_Read()
{
	WORD wHead, wTail, wSize;
	BOOL bLoop;
	int i;
	
	wHead = gTrendDatas.wHeadIndex;
	wTail = gTrendDatas.wTailIndex;
	bLoop = gTrendDatas.bLoop;
	wSize = MAXTRENDCOUNT;
	
	printf("--------%s:%d Get Time Offset(Down)head %d tail %d : ", __FILE__, __LINE__, wHead, wTail);
	if(wHead < wTail){
		printf(" Rollback \n");
 		for(i=wHead; i>=0; i--){
  			printf(" %ld ", gTrendDatas.Datas[i].liOffset);
 		}
		printf("\n");
 		for(i=(wSize-1); i>=wTail; i--){
 			printf(" %ld ", gTrendDatas.Datas[i].liOffset);
 		}
	}
	else if(bLoop){
		printf(" Spacial \n");
		for(i=wHead; i>=wTail; i--){
			printf(" %ld ", gTrendDatas.Datas[i].liOffset);
		}
	}
	else{
		printf(" Normal ");
		for(i=wHead; i>wTail; i--){
			printf(" %ld ", gTrendDatas.Datas[i].liOffset);
		}
	}
	
	printf("\n");
	
	return 0;
}

/**
	自动趋势存储函数
*/
int StartAutoTrendSave()
{
	int res;
	
	//初始化互斥量
	pthread_mutex_init(&mtSaveTrend, NULL);
	
	//加载趋势数据
	LoadTrendFromFlash(&gTrendDatas);
	
	//初始化趋势数组,如果没有建立趋势文件,则初始化数据结构,否则
	if(!gbTrendFileExist){
		InitTrendArray();
	}

	//Demo Data
	srand((long int)time(0));        
	
 	res = pthread_create(&ptSaveTrend, NULL, ThreadProcSaveTrend, NULL);
	
	if(res !=0){
		return -1;
	}
	
	return 0;
}

/**
	取出指定时刻的数据
	在取值的时候,要注意那些离散的趋势数据,如NIBP
	@param:sTime 指定时刻
	@param:pData 取出的数据
	@return: 0-OK, -1-Failure
*/
int GetOneTrendRecord(S_TIME sTime, PTRENDDATA pData)
{
	WORD wHead, wTail, wSize;
	BOOL bLoop;
	S_TIME tmpTime;
	int i,j;
	int res;
	
	if(pData == NULL) return -1;
	
	//从 Head--->Tail 遍历数据库

	wHead = gTrendDatas.wHeadIndex;
	wTail = gTrendDatas.wTailIndex;
	bLoop = gTrendDatas.bLoop;
	wSize = MAXTRENDCOUNT;
	
	if(wHead < wTail){
// 		printf(" Rollback \n");
		for(i=wHead; i>=0; i--){
// 			printf(" %d ", gTrendDatas.Datas[i].liOffset);
			tmpTime = gTrendDatas.Datas[i].sTime;
			res = CompTimeWithMin(&sTime, &tmpTime);
			if(res == 0){
				//取出数据
				*pData = gTrendDatas.Datas[i];
				
				//检查是否有离散数据(NIBP)
				if(pData->bLogFlag & TRENDNIBP){
					//如果是NIBP,不再继续寻找
					return 0;
				}else{
					//搜索同一时刻保存的数据
					j=i-1;
					tmpTime = gTrendDatas.Datas[j].sTime;
					res = CompTimeWithMin(&sTime, &tmpTime);
	
					while(res == 0){
						if(j < 0) break;
// 						printf("Log Flag is %d . i: %d,  j: %d.\n", gTrendDatas.Datas[j].bLogFlag, i, j);
						tmpTime = gTrendDatas.Datas[j].sTime;
						res = CompTimeWithMin(&sTime, &tmpTime);
						
						if(gTrendDatas.Datas[j].bLogFlag & TRENDNIBP){
							*pData = gTrendDatas.Datas[j];
// 							printf("Log Flag is %d  ===>NIBP.\n", pData->bLogFlag);
						}
						j--;
					}
				}

				return 0;
			}
		}
// 		printf("\n");
		for(i=(wSize-1); i>=wTail; i--){
// 			printf(" %d ", gTrendDatas.Datas[i].liOffset);
			tmpTime = gTrendDatas.Datas[i].sTime;
			res = CompTimeWithMin(&sTime, &tmpTime);
			if(res == 0){
				//取出数据
				*pData = gTrendDatas.Datas[i];
				
				//检查是否有离散数据(NIBP)
				if(pData->bLogFlag & TRENDNIBP){
					//如果是NIBP,不再继续寻找
					return 0;
				}else{
					//搜索同一时刻保存的数据
					j=i-1;
					tmpTime = gTrendDatas.Datas[j].sTime;
					res = CompTimeWithMin(&sTime, &tmpTime);
	
					while(res == 0){
						if(j <wTail) break;
// 						printf("Log Flag is %d . i: %d,  j: %d.\n", gTrendDatas.Datas[j].bLogFlag, i, j);
						tmpTime = gTrendDatas.Datas[j].sTime;
						res = CompTimeWithMin(&sTime, &tmpTime);
						
						if(gTrendDatas.Datas[j].bLogFlag & TRENDNIBP){
							*pData = gTrendDatas.Datas[j];
// 							printf("Log Flag is %d  ===>NIBP.\n", pData->bLogFlag);
						}
						j--;
					}
				}
				
				return 0;
			}
 		}
	}
	else if(bLoop){
// 		printf(" Spacial \n");
		for(i=wHead; i>=wTail; i--){
// 			printf(" %d ", gTrendDatas.Datas[i].liOffset);
			tmpTime = gTrendDatas.Datas[i].sTime;
			res = CompTimeWithMin(&sTime, &tmpTime);
			if(res == 0){
				//取出数据
				*pData = gTrendDatas.Datas[i];
				
				//检查是否有离散数据(NIBP)
				if(pData->bLogFlag & TRENDNIBP){
					//如果是NIBP,不再继续寻找
					return 0;
				}else{
					//搜索同一时刻保存的数据
					j=i-1;
					tmpTime = gTrendDatas.Datas[j].sTime;
					res = CompTimeWithMin(&sTime, &tmpTime);
	
					while(res == 0){
						if(j <wTail) break;
// 						printf("Log Flag is %d . i: %d,  j: %d.\n", gTrendDatas.Datas[j].bLogFlag, i, j);
						tmpTime = gTrendDatas.Datas[j].sTime;
						res = CompTimeWithMin(&sTime, &tmpTime);
						
						if(gTrendDatas.Datas[j].bLogFlag & TRENDNIBP){
							*pData = gTrendDatas.Datas[j];
// 							printf("Log Flag is %d  ===>NIBP.\n", pData->bLogFlag);
						}
						j--;
					}
				}
				
				return 0;
			}
		}
	}
	else{
// 		printf(" Normal ");
		for(i=wHead; i>wTail; i--){
// 			printf(" %d ", gTrendDatas.Datas[i].liOffset);
			tmpTime = gTrendDatas.Datas[i].sTime;
			res = CompTimeWithMin(&sTime, &tmpTime);
			if(res == 0){
				//取出数据
				*pData = gTrendDatas.Datas[i];
				
				//检查是否有离散数据(NIBP)
				if(pData->bLogFlag & TRENDNIBP){
					//如果是NIBP,不再继续寻找
					return 0;
				}else{
					//搜索同一时刻保存的数据
					j=i-1;
					tmpTime = gTrendDatas.Datas[j].sTime;
					res = CompTimeWithMin(&sTime, &tmpTime);
	
					while(res == 0){
						if(j <=wTail) break;
// 						printf("Log Flag is %d . i: %d,  j: %d.\n", gTrendDatas.Datas[j].bLogFlag, i, j);
						tmpTime = gTrendDatas.Datas[j].sTime;
						res = CompTimeWithMin(&sTime, &tmpTime);
						
						if(gTrendDatas.Datas[j].bLogFlag & TRENDNIBP){
							*pData = gTrendDatas.Datas[j];
// 							printf("Log Flag is %d  ===>NIBP.\n", pData->bLogFlag);
						}
						j--;
					}
				}
				return 0;
			}
		}
	}
	
// 	printf("\n");

	return -1;
}


/*
	清空趋势记录
*/
int ClearTrendData()
{
	//加锁
	pthread_mutex_lock(&mtSaveTrend);
	
	memset(&gTrendDatas, 0, sizeof(CA_TRENDARRAY));
	//解锁
	pthread_mutex_unlock(&mtSaveTrend);
	
	return 0;
}

/*
	对记录时间进行修正
	liOffset:时间偏移量,单位:秒
*/
int ReviseTrendTime(long int liOffset)
{
	//新的起始时间
	S_TIME sNewStartTime;
	int i, iCount;
	
	printf("%s:%d time offset is %ld.\n", __FILE__, __LINE__, liOffset);
	
	printf("%s : %d Old start time is %.4d/%.2d/%.2d %.2d:%.2d:%.2d\n", __FILE__, __LINE__, 
 		 gTrendDatas.sStartTime.wYear, gTrendDatas.sStartTime.bMonth, gTrendDatas.sStartTime.bDay, 
 		 gTrendDatas.sStartTime.bHour, gTrendDatas.sStartTime.bMin, gTrendDatas.sStartTime.bSec);

	//加锁
	pthread_mutex_lock(&mtSaveTrend);
	
	//计算新的时间
	NewCalcTime(&(gTrendDatas.sStartTime), &sNewStartTime, liOffset);
	
	printf("%s : %d New start time is %.4d/%.2d/%.2d %.2d:%.2d:%.2d\n", __FILE__, __LINE__, 
	       sNewStartTime.wYear, sNewStartTime.bMonth, sNewStartTime.bDay, 
	       sNewStartTime.bHour, sNewStartTime.bMin, sNewStartTime.bSec);
	
	//更新起始时间
	gTrendDatas.sStartTime = sNewStartTime;
	
	//根据记录的offset, 修正数据记录的时间
	for(i=0; i<gTrendDatas.wCount; i++){
		NewCalcTime(&(gTrendDatas.sStartTime), 
			      &(gTrendDatas.Datas[i].sTime), 
			      gTrendDatas.Datas[i].liOffset);
	}

	
	//解锁
	pthread_mutex_unlock(&mtSaveTrend);
	
	return 0;
}		

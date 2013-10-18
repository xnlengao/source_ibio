/*************************************************************************** 
 *	Module Name:	RealSave
 *
 *	Abstract:	实时存储
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-08-03 10:09:15
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "General_Funcs.h"
#include "AlarmManage.h"
#include "Global.h"

/*
	实时存储波形和参数,供冻结回放,和报警存储等功能使用
	思路: 在串口接收程序里,实时打包波形数据,满200包(1秒钟,根据采样率定)时, 写入当时的参数信息,然后添加一条实时存储的信息,
*/

/*
	报警存储
	思路: 报警产生时, 建立存储线程, 然后从实时数据中取出前x秒的数据填入报警数据中, 开始计时, 存储满后x秒的数据后, 退出线程
	
	思路2：2008-04-24 16:29:58
		程序开始时建立一个32秒的缓冲区，并实时存储数据，
		当报警发生时，标记时刻，然后继续存储16秒后，加入报警时间数组
*/

//实时数据数组
CA_REALSAVEARRAY 	gRealDataArray;
//报警实时数据
CA_REALALMSAVEARRAY	gRealAlmDataArray;
//单条实时数据
REALSAVEDATA		gRealData;

//实时ECG数组
CA_REALECGARRAY 	gRealEcgArray;
//单条实时ECG数据
REALECGDATA		gRealEcg;

//报警数据存储
CA_ALMDATAARRAY	gAlmDataArray;

//存储报警数据的线程
static pthread_t ptSaveAlarmData;
//存储报警数据的互斥量
pthread_mutex_t mtSaveAlarmData;                                             

static BOOL bDebug = TRUE;
//static BOOL bDebug = TRUE;

//打印参数信号量
extern sem_t semReal_Print;
extern sem_t semCalctime;
/**
	添加一条数据到数组,在HeadIndex处添加最新的数据
	注意:按此法存储,未翻转前,数组索引[0]处无数据,读取数据时要注意
 */
static int RealArray_Add(const PREALSAVEDATA pData, BOOL bFirst)
{
	WORD wHead, wTail, wSize, wCount;
	BOOL bLoop;
	S_TIME sTime;
	
	if(pData == NULL) return -1;
	
	//如果首次\添加数据, 则添加起始时间
	if(bFirst){
		GetSysTime(&sTime);
		gRealDataArray.sStartTime = sTime;
	}
	
	wHead = gRealDataArray.wHeadIndex;
	wTail = gRealDataArray.wTailIndex;
	bLoop = gRealDataArray.bLoop;
	wSize = MAXREALSAVECOUNT;
	
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
	
	gRealDataArray.wHeadIndex = wHead;
	gRealDataArray.wTailIndex = wTail;
	gRealDataArray.bLoop = bLoop;
	gRealDataArray.Datas[gRealDataArray.wHeadIndex] = *pData;
	//计算实际存储的数量
	if(gRealDataArray.wHeadIndex < gRealDataArray.wTailIndex){
		//数据已经存满,并翻转
		gRealDataArray.wCount = MAXREALSAVECOUNT;
	}
	else{
		gRealDataArray.wCount +=1;
	}

	return wHead;
}



static int RealAlmArray_Add(const PREALSAVEDATA pData, BOOL bFirst)
{
	WORD wHead, wTail, wSize, wCount;
	BOOL bLoop;
	S_TIME sTime;
	
	if(pData == NULL) return -1;
	
	//如果首次\添加数据, 则添加起始时间
	if(bFirst){
		GetSysTime(&sTime);
		gRealAlmDataArray.sStartTime = sTime;
	}
	
	wHead = gRealAlmDataArray.wHeadIndex;
	wTail = gRealAlmDataArray.wTailIndex;
	bLoop = gRealAlmDataArray.bLoop;
	wSize = MAXREALALARMSAVECOUNT;
	
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
	
	gRealAlmDataArray.wHeadIndex = wHead;
	gRealAlmDataArray.wTailIndex = wTail;
	gRealAlmDataArray.bLoop = bLoop;
	gRealAlmDataArray.Datas[gRealAlmDataArray.wHeadIndex] = *pData;
	//计算实际存储的数量
	if(gRealAlmDataArray.wHeadIndex < gRealAlmDataArray.wTailIndex){
		//数据已经存满,并翻转
		gRealAlmDataArray.wCount = MAXREALALARMSAVECOUNT;
	}
	else{
		gRealAlmDataArray.wCount +=1;
	}

	return wHead;
}


//----------------------------- interface -------------------------
/**
	填充单条实时数据, 考虑到Resp Spo2以及其他波形不同步
	以ECG的采样率为基准，进行存储
*/
int FillOneRealSaveData()
{
	static int iCount = 199;
	//记录时间
	S_TIME sTime;
	//时间偏移量
	long int liTimeOffset=0;
	static BOOL bFirst = TRUE;			//是否首次添加记录
	int i;
	
	if(iCount >=0 ){
		//继续添加波形
		//Ecg
		if(gCfgEcg.bLeadType == ECGTYPE_5LEAD){
			gRealData.waves[iCount].bEcg1 = gValueEcg.iWaves[ECGWAVE_I];
			gRealData.waves[iCount].bEcg2 = gValueEcg.iWaves[ECGWAVE_II];
			gRealData.waves[iCount].bEcg3 = gValueEcg.iWaves[ECGWAVE_V];
		}
		else{
			gRealData.waves[iCount].bEcg1 = gValueEcg.iWaves[ECGWAVE_I];
			gRealData.waves[iCount].bEcg2 = gValueEcg.iWaves[ECGWAVE_II];
			gRealData.waves[iCount].bEcg3 = gValueEcg.iWaves[ECGWAVE_III];
		}	
		
		//SpO2 
		gRealData.waves[iCount].bSpO2= gValueSpO2.bWave;
		
		//Resp
		gRealData.waves[iCount].bResp = gValueResp.iWave;	
		
		//CO2
		if(gbHaveCo2){
			gRealData.waves[iCount].bCO2 = gValueCO2.wWave;	
		}
		else{
			gRealData.waves[iCount].bCO2 = 0;	
		}
		if(gbHaveIbp){
			gRealData.waves[iCount].iIbp1= gValueIbp1.iWave;	
			gRealData.waves[iCount].iIbp2= gValueIbp2.iWave;	
		}
		else{
			gRealData.waves[iCount].iIbp1= 0;	
			gRealData.waves[iCount].iIbp2= 0;	
		}
		
		iCount --;
	}
	
	if(iCount <0){
		iCount = 199;
		//获取存储时间
		GetSysTime(&sTime);
		//计算此次记录的时间与起始时间的偏移量, 精确到秒
		liTimeOffset = CompTime(&sTime, &(gRealDataArray.sStartTime));
		gRealData.paras.liOffset = liTimeOffset;
		gRealData.paras.sTime = sTime;
		
		//记录原因
		gRealData.paras.bLogFlag = TRENDMARK;
	 	//数据有效标志
		gRealData.paras.dwHaveFlag = 0;
		
		//添加参数信息
		if(gValueEcg.wHR !=0){
			gRealData.paras.wHR = gValueEcg.wHR; 
			gRealData.paras.dwHaveFlag |= HAVEHR;
		}
		if(!gValueResp.bOff){
			gRealData.paras.wRR = gValueResp.wRR;
			gRealData.paras.dwHaveFlag |= HAVERESP;
		}
		//if(gValueSpO2.bSpO2 !=0 && !gValueSpO2.bOff){
		if(gValueSpO2.bSpO2 !=0 ){
			gRealData.paras.bSpO2 = gValueSpO2.bSpO2;
			gRealData.paras.dwHaveFlag |= HAVESPO2;
		}
		//if(gValuePulse.wPR !=0 && !gValuePulse.bOff){
		if(gValuePulse.wPR !=0){
			gRealData.paras.wPR = gValuePulse.wPR;	
			gRealData.paras.dwHaveFlag |= HAVEPR;
		}
		//Temp1
		if(!gValueTemp.bOff_T1){
			gRealData.paras.wTemps[0] = gValueTemp.wT1;
			gRealData.paras.dwHaveFlag |= HAVET1;
		}	
		//Temp2
		if(!gValueTemp.bOff_T2){
			gRealData.paras.wTemps[1] = gValueTemp.wT2;
			gRealData.paras.dwHaveFlag |= HAVET2;
		}	
		//NIBP
		if(gValueNibp.wSys !=0 && gValueNibp.wDia !=0 && gValueNibp.wMean !=0){
			gRealData.paras.sNIBP.wSys = gValueNibp.wSys;
			gRealData.paras.sNIBP.wDia = gValueNibp.wDia;
			gRealData.paras.sNIBP.wMean = gValueNibp.wMean;
			gRealData.paras.dwHaveFlag |= HAVENIBP;
		}
		//CO2
		if(gbHaveCo2 
			&& gValueCO2.wEtCO2 !=0 
			 && gValueCO2.bStatus ==CO2_STATUS_RUN){
			
			gRealData.paras.sCO2.wEtCO2 = gValueCO2.wEtCO2;
			gRealData.paras.sCO2.wFiCO2 = gValueCO2.wFiCO2;
			gRealData.paras.dwHaveFlag |= HAVECO2;
		}
		//IBP1
		if(gbHaveIbp && !gValueIbp1.bOff){
			gRealData.paras.sIBPs[0].iSys = gValueIbp1.iSys;
			gRealData.paras.sIBPs[0].iDia = gValueIbp1.iDia;
			gRealData.paras.sIBPs[0].iMean = gValueIbp1.iMean;
			gRealData.paras.dwHaveFlag |= HAVEIBP1;
		}
		//IBP2
		if(gbHaveIbp && !gValueIbp2.bOff){
			gRealData.paras.sIBPs[1].iSys = gValueIbp2.iSys;
			gRealData.paras.sIBPs[1].iDia = gValueIbp2.iDia;
			gRealData.paras.sIBPs[1].iMean = gValueIbp2.iMean;
			gRealData.paras.dwHaveFlag |= HAVEIBP2;
		}
	
		//将记录添加到数组中
		RealArray_Add(&gRealData, bFirst);
		RealAlmArray_Add(&gRealData, bFirst);
		bFirst = FALSE;
		
		
//  		printf("%s:%d RealSave. Total %d records. Head %d, Tail %d. Time:%.2d:%.2d:%.2d\n", __FILE__, __LINE__, 
//  		       gRealDataArray.wCount, gRealDataArray.wHeadIndex, gRealDataArray.wTailIndex, 
//  		       gRealData.paras.sTime.bHour, gRealData.paras.sTime.bMin, gRealData.paras.sTime.bSec);
		       
// 		       gRealDataArray.Datas[gRealDataArray.wHeadIndex].paras.sTime.bMin,
// 		       gRealDataArray.Datas[gRealDataArray.wHeadIndex].paras.sTime.bSec);
	}
	
	return 0;
}

//-------------------------------  Alarm data Save Thread --------------------------------
/**
	添加一条数据到数组,在HeadIndex处添加最新的数据
	注意:按此法存储,未翻转前,数组索引[0]处无数据,读取数据时要注意
 */
static int AlarmDataArray_Add(const PALARMDATA pData)
{
	WORD wHead, wTail, wSize, wCount;
	BOOL bLoop;
	
	if(pData == NULL) return -1;
	
	wHead = gAlmDataArray.wHeadIndex;
	wTail = gAlmDataArray.wTailIndex;
	bLoop = gAlmDataArray.bLoop;
	wSize = MAXALARMDATACOUNT;
	
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
	
	gAlmDataArray.wHeadIndex = wHead;
	gAlmDataArray.wTailIndex = wTail;
	gAlmDataArray.bLoop = bLoop;
	gAlmDataArray.Datas[gAlmDataArray.wHeadIndex] = *pData;
	
	if(B_PRINTF)printf("%s:%d Alarm ID is %d, class is %d. time is %.2d:%.2d:%.2d, SpO2 is %d.\n", __FILE__, __LINE__, pData->bAlmID, pData->bAlmClass, 
						pData->paras.sTime.bHour, pData->paras.sTime.bMin, pData->paras.sTime.bSec, 
						pData->paras.bSpO2); 
						
	//计算实际存储的数量
	if(gAlmDataArray.wHeadIndex < gAlmDataArray.wTailIndex){
		//数据已经存满,并翻转
		gAlmDataArray.wCount = MAXALARMDATACOUNT;
	}
	else{
		gAlmDataArray.wCount +=1;
	}
	
	if(B_PRINTF)printf("Add one Alarm Data ID is %d, Total Count is %d.\n ", gAlmDataArray.Datas[wHead].bAlmID, gAlmDataArray.wCount);
	
	return wHead;
}
//报警打印
int Print_Alm_Data()
{
	Printer_PrintAlarmData(&gPrintAlarmData);
	return 0;
}	

//如果报警发生，暂停16秒，然后取出此时的  gRealAlmDataArray 数据，存入报警事件数组
//单条报警数据,如果放在线程外面，则不能同时开存储线程，需要等线程结束之后在开启下次存储线程
static ALARMDATA sAlmData;
static BOOL FinishedSaveAlarmData = TRUE;
static void *NewThreadProcSaveAlarmData(void *arg)
{

	//ALARMDATA sAlmData;
	B_ALM_INFO sAlarm = *(PB_ALM_INFO)arg;

	WORD wHead, wTail, wSize;
	BOOL bLoop;
	int i, j;
	int iCount;
	int iCount_Print;
	
	int iTimeCount = 0;
	int iTimeCount_Print = 0;
	BOOL bPrint = FALSE;	//是否已经打印
	BOOL bSaveParas = FALSE;	//是否已经存储参数信息
	
	FinishedSaveAlarmData = FALSE;
	if(sAlarm.bClass == ALARM_OFF || sAlarm.bClass >=ALARM_INFO){
		if(!B_PRINTF)printf("%s:%d Alarm is turned off. Stopped saving alarm data.\n ", __FILE__, __LINE__);
		FinishedSaveAlarmData = TRUE;
		pthread_exit("");
	}
		
	//初始化报警数据
	memset(&sAlmData, 0, sizeof sAlmData);
	memset(&gPrintAlarmData, 0, sizeof gPrintAlarmData);
printf("adfafadfadsf====gRealAlmDataArray.wCount=%d\n",gRealAlmDataArray.wCount);		
	while(gRealAlmDataArray.wCount == 0);

	//记录此时的报警类型以及参数信息
	sAlmData.bAlmType = ALARM_PHYSIOLOGICAL;		//生理报警
	sAlmData.bAlmID = sAlarm.bAlmID;
	sAlmData.bAlmClass = sAlarm.bClass;
// 	//获得参数数据
// 	sAlmData.paras = gRealAlmDataArray.Datas[gRealAlmDataArray.wHeadIndex].paras;
// 	//获得当前时间
// 	GetSysTime(&(sAlmData.paras.sTime));
	//获取报警数据的参数信息
	gPrintAlarmData.bAlmType = ALARM_PHYSIOLOGICAL;		//生理报警
	gPrintAlarmData.bAlmID = sAlarm.bAlmID;
	gPrintAlarmData.bAlmClass = sAlarm.bClass;
// 	gPrintAlarmData.paras = gRealAlmDataArray.Datas[gRealAlmDataArray.wHeadIndex].paras;
// 	GetSysTime(&(gPrintAlarmData.paras.sTime));
	for(;;){
		//等待16秒(MAXALARMWAVELENGTH/2)，然后存储
		if(iTimeCount < MAXALARMWAVELENGTH/2){
			sleep(1);
			iTimeCount ++;
			iTimeCount_Print ++;
			
			//存储一次参数数据,之所以延迟是因为存储于实际的值有时间上的延迟
			if((!bSaveParas)  && (iTimeCount==2)){
				bSaveParas = TRUE;
				//时间
				GetSysTime(&(sAlmData.paras.sTime));
				
				sAlmData.paras = gRealDataArray.Datas[gRealAlmDataArray.wHeadIndex].paras;
			
				sAlmData.paras.wHR = gValueEcg.wHR;
				sAlmData.paras.wPR = gValuePulse.wPR;
				sAlmData.paras.wRR = gValueResp.wRR;
				sAlmData.paras.iSTs[0] = gValueEcg.iST1;
				sAlmData.paras.iSTs[1] = gValueEcg.iST2;
				sAlmData.paras.bSpO2 = gValueSpO2.bSpO2;
				sAlmData.paras.sNIBP.wSys = gValueNibp.wSys;
				sAlmData.paras.sNIBP.wMean= gValueNibp.wMean;
				sAlmData.paras.sNIBP.wDia= gValueNibp.wDia;
				sAlmData.paras.sIBPs[0].iSys = gValueIbp1.iSys;
				sAlmData.paras.sIBPs[0].iMean = gValueIbp1.iMean;
				sAlmData.paras.sIBPs[0].iDia = gValueIbp1.iDia;
				sAlmData.paras.sIBPs[1].iSys = gValueIbp2.iSys;
				sAlmData.paras.sIBPs[1].iMean = gValueIbp2.iMean;
				sAlmData.paras.sIBPs[1].iDia = gValueIbp2.iDia;
				sAlmData.paras.wTemps[0] = gValueTemp.wT1;
				sAlmData.paras.wTemps[1] = gValueTemp.wT2;
				sAlmData.paras.sCO2.wEtCO2   = gValueCO2.wEtCO2;
				sAlmData.paras.sCO2.wFiCO2   = gValueCO2.wFiCO2;
				sAlmData.paras.sCO2.wAwRR= gValueCO2.wAwRR;

				if(!B_PRINTF)printf("%s:%d Alarm ID is %d, class is %d. time is %.2d:%.2d:%.2d, SpO2 is %d. HR is %d    REALARRY: %d   %d.\n", __FILE__, __LINE__, sAlmData.bAlmID, sAlmData.bAlmClass, 
						sAlmData.paras.sTime.bHour, sAlmData.paras.sTime.bMin, sAlmData.paras.sTime.bSec, 
						sAlmData.paras.bSpO2, sAlmData.paras.wHR, 
						 gRealDataArray.Datas[gRealAlmDataArray.wHeadIndex].paras.bSpO2,
						 gRealDataArray.Datas[gRealAlmDataArray.wHeadIndex].paras.wHR); 
				//gPrintAlarmData.paras = gRealAlmDataArray.Datas[gRealAlmDataArray.wHeadIndex].paras;
				gPrintAlarmData.paras = sAlmData.paras;
				//GetSysTime(&(gPrintAlarmData.paras.sTime));
			}

			if(bDebug) printf("Alarm Time count :%d.\n", iTimeCount);
			//等待x秒(x<16)后，取出波形，开始打印
			if(iTimeCount_Print < (gCfgAlarm.bPrintWaveLength/2 +1) ){
				if(bDebug) printf("alarm print count %d\n", iTimeCount_Print);
			}
			else if(!bPrint){
				//即从当前 wHeadIndex 取之前 x条数据, 包括 wHeadIndex(即当前的数据)
				wHead = gRealAlmDataArray.wHeadIndex;
				wTail = gRealAlmDataArray.wTailIndex;
				bLoop = gRealAlmDataArray.bLoop;
				wSize = MAXREALALARMSAVECOUNT;
				
				iCount_Print = gCfgAlarm.bPrintWaveLength;
				
				if(bDebug){
					printf("print Head is %d, Tail is %d, Count is %d\n", wHead, wTail, iCount_Print);
				}
				if(wHead < wTail){
					if(bDebug)
						printf("print Real Data Rollback \n");
					for(i=wHead; i>=0; i--, iCount_Print--){
						if(iCount_Print>=0){
					//copy Wave Data
							memcpy((PTRENDWAVE)(gPrintAlarmData.waves[iCount_Print]), (PTRENDWAVE)(gRealAlmDataArray.Datas[i].waves), SAMPLING_RATE * sizeof(TRENDWAVE));
							if(bDebug)
								printf(" %d", iCount_Print);
						}
						else{
							break;
						}
					}
					if(bDebug)
						printf("\n");
					for(i=(wSize-1); i>=wTail; i--, iCount_Print--){
						if(iCount_Print>=0){
						//copy Wave Data
							memcpy((PTRENDWAVE)(gPrintAlarmData.waves[iCount_Print]), (PTRENDWAVE)(gRealAlmDataArray.Datas[i].waves), SAMPLING_RATE * sizeof(TRENDWAVE));
							if(bDebug)
								printf(" %d", iCount_Print);
						}
						else{
							break;
						}	
					}
				}
				else if(bLoop){
					if(bDebug)
						printf("print Real Data Spacial \n");
					for(i=wHead; i>=wTail; i--, iCount_Print--){
						if(iCount_Print>=0){
						//copy Wave Data
							memcpy((PTRENDWAVE)(gPrintAlarmData.waves[iCount_Print]), (PTRENDWAVE)(gRealAlmDataArray.Datas[i].waves), SAMPLING_RATE * sizeof(TRENDWAVE));
							if(bDebug)
								printf(" %d", iCount_Print);
						}
						else{
							break;
						}
					}
				}
				else{
					if(bDebug)
						printf("print Real Data  Normal: ");
					for(i=wHead; i>wTail; i--, iCount_Print--){
						if(iCount_Print>=0){
							//copy Wave Data
							memcpy((PTRENDWAVE)(gPrintAlarmData.waves[iCount_Print]), (PTRENDWAVE)(gRealAlmDataArray.Datas[i].waves), SAMPLING_RATE * sizeof(TRENDWAVE));
							if(bDebug)
								printf(" %d", iCount_Print);
						}
						else{
							break;
						}
					}
				}
				if(bDebug)
					printf("\n");
					
				//打印报警数据
				
		
				if(gCfgAlarm.bPrint == SWITCH_ON && sAlarm.bPrint == ALARM_PRINT_ON){
					//Printer_PrintAlarmData(&gPrintAlarmData);
					gPrintingType=PRN_WORK_ALMPRINT;
					sem_post(&semReal_Print);
				}
				
				bPrint = TRUE;
			}
		}
		else{
			
			//拷贝波形数据
			//即从当前 wHeadIndex 取之前 x条数据, 包括 wHeadIndex(即当前的数据)
			wHead = gRealAlmDataArray.wHeadIndex;
			wTail = gRealAlmDataArray.wTailIndex;
			bLoop = gRealAlmDataArray.bLoop;
			wSize = MAXREALALARMSAVECOUNT;
			
			//获取Head-Tail的波形数据
			iCount = MAXALARMWAVELENGTH-1;
			if(bDebug){
				printf("Head is %d, Tail is %d, Count is %d\n", wHead, wTail, iCount);
			}
			
			if(wHead < wTail){
				if(bDebug)
					printf("Real Data Rollback \n");
				for(i=wHead; i>=0; i--, iCount--){
					if(iCount>=0){
					//copy Wave Data
						memcpy((PTRENDWAVE)(sAlmData.waves[iCount]), (PTRENDWAVE)(gRealAlmDataArray.Datas[i].waves), SAMPLING_RATE * sizeof(TRENDWAVE));
						if(bDebug)
							printf(" %d", iCount);
					}
					else{
						break;
					}
				}
				if(bDebug)
					printf("\n");
				for(i=(wSize-1); i>=wTail; i--, iCount--){
					if(iCount>=0){
						//copy Wave Data
						memcpy((PTRENDWAVE)(sAlmData.waves[iCount]), (PTRENDWAVE)(gRealAlmDataArray.Datas[i].waves), SAMPLING_RATE * sizeof(TRENDWAVE));
						if(bDebug)
							printf(" %d", iCount);
					}
					else{
						break;
					}	
				}
			}
			else if(bLoop){
				if(bDebug)
					printf("Real Data Spacial \n");
				for(i=wHead; i>=wTail; i--, iCount--){
					if(iCount>=0){
				//copy Wave Data
						memcpy((PTRENDWAVE)(sAlmData.waves[iCount]), (PTRENDWAVE)(gRealAlmDataArray.Datas[i].waves), SAMPLING_RATE * sizeof(TRENDWAVE));
						if(bDebug)
							printf(" %d", iCount);
					}
					else{
						break;
					}
				}
			}
			else{
				if(bDebug)
					printf("Real Data  Normal: ");
				for(i=wHead; i>wTail; i--, iCount--){
					if(iCount>=0){
				//copy Wave Data
						memcpy((PTRENDWAVE)(sAlmData.waves[iCount]), (PTRENDWAVE)(gRealAlmDataArray.Datas[i].waves), SAMPLING_RATE * sizeof(TRENDWAVE));
						if(bDebug)
							printf(" %d", iCount);
					}
					else{
						break;
					}
				}
			}
			if(bDebug)
				printf("\n");
			
			
			
// 			for(i=0; i<MAXALARMWAVELENGTH; i++){
// 				memcpy((PTRENDWAVE)(sAlmData.waves[i]), (PTRENDWAVE)(gRealAlmDataArray.Datas[i].waves), SAMPLING_RATE * sizeof(TRENDWAVE));
// 			}
// 		
			//加锁
			pthread_mutex_lock(&mtSaveAlarmData);
			AlarmDataArray_Add(&sAlmData);
			//解锁
			pthread_mutex_unlock(&mtSaveAlarmData);
			
			break;
		}
	}
printf("%s:%d Alarm ID is %d, class is %d. time is %.2d:%.2d:%.2d, SpO2 is %d.\n", __FILE__, __LINE__, sAlmData.bAlmID, sAlmData.bAlmClass, 
						sAlmData.paras.sTime.bHour, sAlmData.paras.sTime.bMin, sAlmData.paras.sTime.bSec, 
						sAlmData.paras.bSpO2); 
	printf("%s:%d  Cancel  Save Alarm Data thread.\n", __FILE__, __LINE__);
	FinishedSaveAlarmData = TRUE;
	pthread_exit("Cancel Save Alarm Data thread.\n");
}
		
static void *ThreadProcSaveAlarmData(void *arg)
{
	WORD wHead, wTail, wSize;
	BOOL bLoop;
	int i,j,k; 
	int iCount;				//拷贝数据计数器
	int iTimeCount;			//时间计数器
	int iCount_Print;			//拷贝数据计数器
	int iTimeCount_Print;	//时间计数器
	
	
	//单条报警数据
	ALARMDATA sAlmData;
  	B_ALM_INFO sAlarm = *(PB_ALM_INFO)arg;
	BOOL bSaveParas = FALSE;	//是否已经存储参数信息
	BOOL bPrint = FALSE;	//是否已经打印
		
	if(bDebug){
		printf("%s:%d Start Save Alarm Data thread.\n", __FILE__, __LINE__);
		printf("--------------%s:%d Alarm ID is %d, class is %d.\n", __FILE__, __LINE__, sAlarm.bAlmID, sAlarm.bClass);
	}
	
	
	if(sAlarm.bClass == ALARM_OFF || sAlarm.bClass >=ALARM_INFO){
		return ;
	}
	
	//初始化报警数据
	memset(&sAlmData, 0, sizeof sAlmData);
	memset(&gPrintAlarmData, 0, sizeof gPrintAlarmData);
	
	//查找之前的X秒的数据, 因为RealData是按照秒来存储的,所以查找索引即可
	//即从当前 wHeadIndex 取之前 x条数据, 包括 wHeadIndex(即当前的数据)
	wHead = gRealDataArray.wHeadIndex;
	wTail = gRealDataArray.wTailIndex;
	bLoop = gRealDataArray.bLoop;
	wSize = MAXREALSAVECOUNT;
	
	while(gRealDataArray.wCount == 0);
	
	//记录此时的报警类型以及参数信息
	sAlmData.bAlmType = ALARM_PHYSIOLOGICAL;		//生理报警
	sAlmData.bAlmID = sAlarm.bAlmID;
	sAlmData.bAlmClass = sAlarm.bClass;
	//打印的数据
	gPrintAlarmData.bAlmType = ALARM_PHYSIOLOGICAL;		//生理报警
	gPrintAlarmData.bAlmID = sAlmData.bAlmID;
	gPrintAlarmData.bAlmClass = sAlmData.bAlmClass;
	
	
// 	//TODO:此处记载的数据可能滞后
//  	sAlmData.paras = gRealDataArray.Datas[wHead].paras;
// 	//时间
// 	GetSysTime(&(sAlmData.paras.sTime));
	
// 	if(bDebug)
// 		printf("%s:%d Alarm ID is %d, class is %d. time is %.2d:%.2d:%.2d, Head is %d.\n", __FILE__, __LINE__, sAlmData.bAlmID, sAlmData.bAlmClass, 
// 		       sAlmData.paras.sTime.bHour, sAlmData.paras.sTime.bMin, sAlmData.paras.sTime.bSec, 
// 		       gRealDataArray.Datas[wHead].paras.liOffset);
/*	
	printf("%s:%d Alarm ID is %d, class is %d. time is %.2d:%.2d:%.2d, SpO2 is %d.\n", __FILE__, __LINE__, sAlmData.bAlmID, sAlmData.bAlmClass, 
	       sAlmData.paras.sTime.bHour, sAlmData.paras.sTime.bMin, sAlmData.paras.sTime.bSec, 
	       sAlmData.paras.bSpO2);              */
	
	//获取前16秒的波形数据(Head之前的16条数据)
	iCount = MAXALARMWAVELENGTH/2;
	if(bDebug){
		printf("Head is %d, Tail is %d, Count is %d\n", wHead, wTail, iCount);
	}
	if(wHead < wTail){
		if(bDebug)
			printf("Real Data Rollback \n");
		for(i=wHead; i>=0; i--, iCount--){
			if(iCount>=0){
				//copy Wave Data
				memcpy((PTRENDWAVE)(sAlmData.waves[iCount]), (PTRENDWAVE)(gRealDataArray.Datas[i].waves), SAMPLING_RATE * sizeof(TRENDWAVE));
				if(bDebug)
					printf(" %d", iCount);
			}
			else{
				break;
			}
		}
		if(bDebug)
			printf("\n");
		for(i=(wSize-1); i>=wTail; i--, iCount--){
			if(iCount>=0){
				//copy Wave Data
				memcpy((PTRENDWAVE)(sAlmData.waves[iCount]), (PTRENDWAVE)(gRealDataArray.Datas[i].waves), SAMPLING_RATE * sizeof(TRENDWAVE));
				if(bDebug)
					printf(" %d", iCount);
			}
			else{
				break;
			}	
		}
	}
	else if(bLoop){
		if(bDebug)
			printf("Real Data Spacial \n");
		for(i=wHead; i>=wTail; i--, iCount--){
			if(iCount>=0){
				//copy Wave Data
				memcpy((PTRENDWAVE)(sAlmData.waves[iCount]), (PTRENDWAVE)(gRealDataArray.Datas[i].waves), SAMPLING_RATE * sizeof(TRENDWAVE));
				if(bDebug)
					printf(" %d", iCount);
			}
			else{
				break;
			}
		}
	}
	else{
		if(bDebug)
			printf("Real Data  Normal: ");
		for(i=wHead; i>wTail; i--, iCount--){
			if(iCount>=0){
				//copy Wave Data
				memcpy((PTRENDWAVE)(sAlmData.waves[iCount]), (PTRENDWAVE)(gRealDataArray.Datas[i].waves), SAMPLING_RATE * sizeof(TRENDWAVE));
 				if(bDebug)
 					printf(" %d", iCount);
			}
			else{
				break;
			}
		}
	}
	if(bDebug)
		printf("\n");
	
	
	
	//获取后16秒的波形数据(意义秒钟的间隔读取Head处的数据)
	iCount = MAXALARMWAVELENGTH/2+1;
	
// 	iCount_Print = (gCfgAlarm.bPrintWaveLength+1) /2 +1;	
	
	//暂时实现报警前x秒波形的打印，因为如果打印之后的波形，会有延迟
	//获取报警波形数据
	for(j=iCount-gCfgAlarm.bPrintWaveLength, k=0; j<=iCount; j++,k++){
		printf("Copy Alarm wave  %d\n", j);
		memcpy((PTRENDWAVE)(gPrintAlarmData.waves[k]), (PTRENDWAVE)(sAlmData.waves[j]), SAMPLING_RATE * sizeof(TRENDWAVE));	
	}
	
	//获取报警数据的参数信息
	gPrintAlarmData.paras = gRealDataArray.Datas[gRealDataArray.wHeadIndex].paras;
	GetSysTime(&(gPrintAlarmData.paras.sTime));
	//打印报警数据
	Printer_PrintAlarmData(&gPrintAlarmData);
	
	for(i=MAXALARMWAVELENGTH/2; i<MAXALARMWAVELENGTH; i++, iCount ++, iCount_Print ++){
		wHead = gRealDataArray.wHeadIndex;
		if(iCount < MAXALARMWAVELENGTH){
 			sleep(1);
			//存储一次参数数据,之所以延迟是因为存储于实际的值有时间上的延迟
			if(!bSaveParas && iCount == MAXALARMWAVELENGTH/2+4){
				bSaveParas = TRUE;
				sAlmData.paras = gRealDataArray.Datas[wHead].paras;
				//时间
				GetSysTime(&(sAlmData.paras.sTime));
			}

			//copy Wave Data
			memcpy((PTRENDWAVE)(sAlmData.waves[iCount]), (PTRENDWAVE)(gRealDataArray.Datas[wHead].waves), SAMPLING_RATE * sizeof(TRENDWAVE));
			/*
			if(iCount_Print < (gCfgAlarm.bPrintWaveLength+1) ){
// 				memcpy((PTRENDWAVE)(gPrintAlarmData.waves[iCount_Print]), (PTRENDWAVE)(gRealDataArray.Datas[wHead].waves), SAMPLING_RATE * sizeof(TRENDWAVE));	
				printf("alarm  count %d\n", iCount_Print);
			}
			else if(!bPrint){
				//获取报警波形数据
				for(j=iCount-gCfgAlarm.bPrintWaveLength, k=0; j<=iCount; j++,k++){
					printf("Copy Alarm wave  %d\n", j);
					memcpy((PTRENDWAVE)(gPrintAlarmData.waves[k]), (PTRENDWAVE)(sAlmData.waves[j]), SAMPLING_RATE * sizeof(TRENDWAVE));	
				}
// 				gPrintAlarmData =  sAlmData;
				//获取报警数据的参数信息
				gPrintAlarmData.paras = gRealDataArray.Datas[gRealDataArray.wHeadIndex].paras;
				GetSysTime(&(gPrintAlarmData.paras.sTime));
				//打印报警数据
				Printer_PrintAlarmData(&gPrintAlarmData);
				bPrint = TRUE; 
			}
			*/
			if(bDebug)
				printf("count %d\n", iCount);
		}
	}
	
	//加锁
	pthread_mutex_lock(&mtSaveAlarmData);
	AlarmDataArray_Add(&sAlmData);
	//解锁
	pthread_mutex_unlock(&mtSaveAlarmData);
		
	printf("%s:%d Cancel Save Alarm Data thread.\n", __FILE__, __LINE__);
	pthread_exit("Cancel Save Alarm Data thread.\n");
}

//记录一条报警数据
int SaveOneAlarmData(B_ALM_INFO sInfo)
{
	B_ALM_INFO sAlm;
	int res = 0;
	
	sAlm = sInfo;
       
	if(sAlm.bClass == ALARM_OFF) return -1;
	printf("------------%d\n",FinishedSaveAlarmData);
//   	res = pthread_create(&ptSaveAlarmData, NULL, ThreadProcSaveAlarmData, &sAlm);
	if(FinishedSaveAlarmData) {
		usleep(20000);		//延时等待线程退出
		res = pthread_create(&ptSaveAlarmData, NULL, NewThreadProcSaveAlarmData, &sAlm);
		usleep(20000);		//延时等待线程创建完
	}
	else{
		printf("----------------------- %s:%d  Cann't create thread, saving Alarm Data .\n", __FILE__, __LINE__);
	}
	
	return res;
}


///////////////////////// 心电存储和分析  2008-08-04 10:41:26//////////////////
//实时ECG波形  I II III   V1~V6
typedef struct {
	BYTE bWave[4];		
}REAL_ECG_DATA, *PREAL_ECG_DATA;
REAL_ECG_DATA gRealEcg_Data;


//实时ECG波形存储, 单位:  秒 * SAMPLING_RATE
#define MAXREALECG_COUNT	12000  		 //60*200
//#define MAXREALECG_COUNT		120000   	//60*200*X 
typedef struct {
	long int 		wHeadIndex;				//循环数组首指针
	long int 		wTailIndex;				//循环数组尾指针
	BOOL 		bLoop;					//数组翻转标记
	long int 		wCount;					//实际存储的数据
	REAL_ECG_DATA		Datas[MAXREALECG_COUNT];
}REAL_ECGARRAY, *PREAL_ECGARRAY;
REAL_ECGARRAY gRealEcg_Array;

//是否存储ECG数据
BOOL gbSaveRealEcgData = FALSE;
BOOL gbSaveRealEcgDataOK = FALSE;

//用于文件存储的ECG数据结构
typedef struct {
 	S_TIME 				sStartTime;			//开始存储数据的时间	
	REAL_ECG_DATA		Datas[MAXREALECG_COUNT];
}REALSAVEDATA_ECG, *PREALSAVEDATA_ECG;


/**
	添加一条数据到数组,在HeadIndex处添加最新的数据
	注意:按此法存储,未翻转前,数组索引[0]处无数据,读取数据时要注意
 */
static int RealEcgArray_Add(const PREAL_ECG_DATA pData)
{
	long int wHead, wTail, wSize, wCount;
	BOOL bLoop;
	
	if(pData == NULL) return -1;
	
	wHead = gRealEcg_Array.wHeadIndex;
	wTail = gRealEcg_Array.wTailIndex;
	bLoop = gRealEcg_Array.bLoop;
	wSize = MAXREALECG_COUNT;
	
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
		gbSaveRealEcgDataOK = TRUE;
	}
	
	gRealEcg_Array.wHeadIndex = wHead;
	gRealEcg_Array.wTailIndex = wTail;
	gRealEcg_Array.bLoop = bLoop;
	gRealEcg_Array.Datas[gRealEcg_Array.wHeadIndex] = *pData;
	//计算实际存储的数量
	if(gRealEcg_Array.wHeadIndex < gRealEcg_Array.wTailIndex){
		//数据已经存满,并翻转
		gRealEcg_Array.wCount = MAXREALECG_COUNT;
	}
	else{
		gRealEcg_Array.wCount +=1;
	}

	return wHead;
}

/**
	填充单条实时ECG数据
	以ECG的采样率为基准，进行存储
 */
int FillOneRealEcg_Data()
{
	/*
	static int iCount = 199;
	int i;
	
	if(iCount >=0 ){
		gRealEcg_Data.bWave[0][iCount]  = gValueEcg.iWaves[0];
		gRealEcg_Data.bWave[1][iCount]  = gValueEcg.iWaves[1];
		gRealEcg_Data.bWave[2][iCount]  = gValueEcg.iWaves[2];
		iCount --;
	}
	
	if(iCount <0){                             
		iCount = 199;
		//将记录添加到数组中
		RealEcgArray_Add(&gRealEcg_Data);

		printf("%s:%d RealECGSave. Total %d records. Head %d, Tail %d.\n", __FILE__, __LINE__, 
		       gRealEcg_Array.wCount, gRealEcg_Array.wHeadIndex, gRealEcg_Array.wTailIndex);
	}
	*/
	
	static int iCount = 199;
	
	gRealEcg_Data.bWave[0]  = gValueEcg.iWaves[ECGWAVE_I];
	gRealEcg_Data.bWave[1]  = gValueEcg.iWaves[ECGWAVE_II];
	gRealEcg_Data.bWave[2]  = gValueEcg.iWaves[ECGWAVE_III];
	gRealEcg_Data.bWave[3]  = gValueEcg.iWaves[ECGWAVE_V];
	
	
	//将记录添加到数组中
	RealEcgArray_Add(&gRealEcg_Data);
	
	iCount --;
	
	if(iCount <0){                             
		iCount = 199;
		printf("%s:%d RealECGSave. Total %ld records. Head %ld, Tail %ld.\n", __FILE__, __LINE__, 
		       gRealEcg_Array.wCount, gRealEcg_Array.wHeadIndex, gRealEcg_Array.wTailIndex);
	}
	
	return 0;
}

int SaveEcgDataToFile()
{
	long int wHead, wTail, wSize;
	BOOL bLoop;
	long int  i, iCount;
	REALSAVEDATA_ECG  tmpDatas;
// 	REAL_ECG_DATA		tmpDatas[MAXREALECG_COUNT];

	wHead = gRealEcg_Array.wHeadIndex;
	wTail = gRealEcg_Array.wTailIndex;
	bLoop = gRealEcg_Array.bLoop;
	wSize = MAXREALECG_COUNT;
	
	//获得存储时间
 	GetSysTime(&(tmpDatas.sStartTime));
	
	iCount =0;
	//拷贝当前数据(排序后)
	/*
	if(wHead < wTail){
		for(i=wHead; i>=0; i--, iCount++){
			tmpDatas.Datas[iCount].bWave[0] = gRealEcg_Array.Datas[i].bWave[0];
			tmpDatas.Datas[iCount].bWave[1] = gRealEcg_Array.Datas[i].bWave[1];
			tmpDatas.Datas[iCount].bWave[2] = gRealEcg_Array.Datas[i].bWave[2];
			tmpDatas.Datas[iCount].bWave[3] = gRealEcg_Array.Datas[i].bWave[3];
		}
		for(i=(wSize-1); i>=wTail; i--,iCount++){
			tmpDatas.Datas[iCount].bWave[0] = gRealEcg_Array.Datas[i].bWave[0];
			tmpDatas.Datas[iCount].bWave[1] = gRealEcg_Array.Datas[i].bWave[1];
			tmpDatas.Datas[iCount].bWave[2] = gRealEcg_Array.Datas[i].bWave[2];
			tmpDatas.Datas[iCount].bWave[3] = gRealEcg_Array.Datas[i].bWave[3];
		}
	}
	else if(bLoop){
		for(i=wHead; i>=wTail; i--,iCount++){
			tmpDatas.Datas[iCount].bWave[0] = gRealEcg_Array.Datas[i].bWave[0];
			tmpDatas.Datas[iCount].bWave[1] = gRealEcg_Array.Datas[i].bWave[1];
			tmpDatas.Datas[iCount].bWave[2] = gRealEcg_Array.Datas[i].bWave[2];
			tmpDatas.Datas[iCount].bWave[3] = gRealEcg_Array.Datas[i].bWave[3];
		}
	}
	else{
		for(i=wHead; i>wTail; i--,iCount++){
			tmpDatas.Datas[iCount].bWave[0] = gRealEcg_Array.Datas[i].bWave[0];
			tmpDatas.Datas[iCount].bWave[1] = gRealEcg_Array.Datas[i].bWave[1];
			tmpDatas.Datas[iCount].bWave[2] = gRealEcg_Array.Datas[i].bWave[2];
			tmpDatas.Datas[iCount].bWave[3] = gRealEcg_Array.Datas[i].bWave[3];
		}
	}
	*/
	
	if(wHead < wTail){
		for(i=wTail; i<wSize; i++,iCount++){
			tmpDatas.Datas[iCount].bWave[0] = gRealEcg_Array.Datas[i].bWave[0];
			tmpDatas.Datas[iCount].bWave[1] = gRealEcg_Array.Datas[i].bWave[1];
			tmpDatas.Datas[iCount].bWave[2] = gRealEcg_Array.Datas[i].bWave[2];
			tmpDatas.Datas[iCount].bWave[3] = gRealEcg_Array.Datas[i].bWave[3];
		}
		for(i=0; i<=wHead; i++, iCount++){
			tmpDatas.Datas[iCount].bWave[0] = gRealEcg_Array.Datas[i].bWave[0];
			tmpDatas.Datas[iCount].bWave[1] = gRealEcg_Array.Datas[i].bWave[1];
			tmpDatas.Datas[iCount].bWave[2] = gRealEcg_Array.Datas[i].bWave[2];
			tmpDatas.Datas[iCount].bWave[3] = gRealEcg_Array.Datas[i].bWave[3];
		}
	}
	else if(bLoop){
		for(i=wTail; i<=wHead; i++,iCount++){	
			tmpDatas.Datas[iCount].bWave[0] = gRealEcg_Array.Datas[i].bWave[0];
			tmpDatas.Datas[iCount].bWave[1] = gRealEcg_Array.Datas[i].bWave[1];
			tmpDatas.Datas[iCount].bWave[2] = gRealEcg_Array.Datas[i].bWave[2];
			tmpDatas.Datas[iCount].bWave[3] = gRealEcg_Array.Datas[i].bWave[3];
		}
	}
	else{
		for(i=wTail+1; i<=wHead; i++,iCount++){
			tmpDatas.Datas[iCount].bWave[0] = gRealEcg_Array.Datas[i].bWave[0];
			tmpDatas.Datas[iCount].bWave[1] = gRealEcg_Array.Datas[i].bWave[1];
			tmpDatas.Datas[iCount].bWave[2] = gRealEcg_Array.Datas[i].bWave[2];
			tmpDatas.Datas[iCount].bWave[3] = gRealEcg_Array.Datas[i].bWave[3];
		}
	}

	
	//把数据写入文件
	FILE *fp;
	fp  = fopen("ecg_records.dat", "w+");
	fwrite(&tmpDatas, sizeof (REALSAVEDATA_ECG), 1, fp); 
	fclose(fp);
	
	printf("%s:%d --------------------Save %ld ECG datas to File. (wHead %ld   wTail %ld)\n", __FILE__, __LINE__, iCount, wHead, wTail);
	
	return 0;
	
}



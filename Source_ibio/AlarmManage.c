/*************************************************************************** 
 *	Module Name:	AlarmManage
 *
 *	Abstract:		报警信息管理
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2007-06-27 14:28:42
 ***************************************************************************/
/*
思路：
	
	报警：
	a. 保存报警信息的原则：
		1、两种报警，两个独立的显示区域
		2、使用用链表保存，方便插入和删除
		3、新发生(改变级别也算是新发生)的报警，中断当前的显示顺序，优先显示
		4、以1－2Hz的频率循环显示所有的报警信息

	b. 三个基本的接口函数
		1、AddAlarm()		添加新的报警		Lock-->查询链表是否有此记录(N)-->添加新的记录-->Unlock-->通知显示Reset
		2、AlarmLifted()   	解除报警 		Lock-->查询链表是否有此记录(Y)-->删除此记录-->Unlock-->通知显示Reset	
		3、ChangeAlarm()     	改变报警		Lock-->查询链表是否有此记录(Y)-->删除此记录-->记录更改后的级别不是"关闭"(Y)-->添加新纪录-->Unlock-->通知显示Reset
*/
#include "assert.h"
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "dllist.h"
#include "AlarmManage.h"

//链表查找数据成功
#define DL_FIND_OK		1	
//链表查找数据失败
#define DL_FIND_ERR	0	

//生理参数报警线程互斥量
pthread_mutex_t mtPhyAlm;	
//技术参数报警线程互斥量
pthread_mutex_t mtTecAlm;	

//生理报警链表
DLLIST  *gDListAlmPhy;		
//技术报警链表
DLLIST  *gDListAlmTec;		

//是否需要重新显示报警信息
BOOL bResetView_Phy = FALSE;
BOOL bResetView_Tec = FALSE;

//是否进入报警暂停状态
extern BOOL gbSuspending;

//是否静音状态
extern BOOL gbSilence;


//------------------------------------ 关于链表的一些操作函数 -----------------------------------------
/*
	打印链表所有项目
*/
static int ProcPrintAlmItems(int Tag, void *Memory, void *Args)
{

	B_ALM_INFO *source = Memory;
			
	assert(Tag == 0);
	
	if(source !=NULL){
//  		printf("(%s:%d)Read AlmID:%d,  Status:%d,  Class:%d  \n", __FILE__, __LINE__, 
//  			 source->bAlmID, source->bStatus, source->bClass);
	}
	else{
// 		printf("(%s:%d)source is NULL ........\n", __FILE__, __LINE__);
	}
	
	return 0;
}

/*
	在链表中查找指定记录
*/
static int ProcFindAlmItem(int Tag, void *Memory, void *Args)
{
	B_ALM_INFO *source = Memory;
	B_ALM_INFO *target = Args;
			
	assert(Tag == 0);
	
	if(source !=NULL){
		if(target !=NULL){
			if(source->bAlmID == target->bAlmID){
//    				printf("\n** Get AlmID:%d,  Status:%d,  Class:%d  \n\n", 
//   					 source->bAlmID, source->bStatus, source->bClass);
				return DL_FIND_OK;
			}	
		}
		else{
			if(B_PRINTF) printf("(%s:%d)target is NULL ........\n", __FILE__, __LINE__);
		}
	}
	else{
		if(B_PRINTF) printf("(%s:%d)source is NULL ........\n", __FILE__, __LINE__);
	}
	
	return DL_FIND_ERR;
}
//--------------------------------------------------------------------------------------------------------------------------
/*
	增加一条生理参数报警记录
*/
static int AddPhyAlarm(B_ALM_INFO info)
{
	int i;
	int res;
	int iListNum;
	DLLIST *FindAddr=NULL;			//查找到的记录的地址
	

	if(info.bAlmID == AP_NONE){
// 		if(B_PRINTF) printf("%s:%d Append Unkonwn Type Alarm.\n", __FILE__, __LINE__);
		return 0;	
	} 
		
	//加锁
	pthread_mutex_lock(&mtPhyAlm);

	//检查该记录是否已经存在
	res = DLWalk((DLLIST *)gDListAlmPhy, &FindAddr, ProcFindAlmItem, &info);
	
	//如果该记录不存在,则添加新的记录,否则返回
	if(res == DL_FIND_ERR){
		if(DL_SUCCESS != DLPrepend((DLLIST **)&gDListAlmPhy, 0, &info, sizeof(B_ALM_INFO))){
// 		if(DL_SUCCESS != DLAppend((DLLIST **)&gDListAlmPhy, 0, &info, sizeof(B_ALM_INFO))){
			if(B_PRINTF) printf("(%s:%d)Couldn't allocate enough memory.\n", __FILE__, __LINE__);
		}	
		else{
 			if(B_PRINTF) printf("(%s:%d)Add record at end of list.\n", __FILE__, __LINE__);	
		}
	}
	else{
 		if(B_PRINTF) printf("%s:%d The record has been in existence.\n", __FILE__, __LINE__);
		//解锁
		pthread_mutex_unlock(&mtPhyAlm);
		return -1;
	}

      //打印链表
//     	iListNum = DLCount((DLLIST *)gDListAlmPhy);
//     	printf("(%s:%d)List  has  %d Infos.\n\n", __FILE__, __LINE__, iListNum);		
//     	if(iListNum>0) res = DLWalk((DLLIST *)gDListAlmPhy, NULL, ProcPrintAlmItems, NULL);
		
	//解锁
	pthread_mutex_unlock(&mtPhyAlm);
	
	//重新显示
	bResetView_Phy = TRUE;
	
	if(B_PRINTF) printf("%s:%d Add One PhyAlm Info.\n", __FILE__, __LINE__);
	
	return 0;
}

/*
	解除一条生理报警
*/
static int PhyAlarmLifted(B_ALM_INFO info)
{
	int i;
	int res;
	int iListNum;
	DLLIST *DelAddr=NULL;			//查找到的记录的地址
	DLLIST *SaveList;				//保存链表起始位置
	B_ALM_INFO *DelData=NULL;		//查找到的记录
	
	//加锁
	pthread_mutex_lock(&mtPhyAlm);
	
	//检查该记录是否存在
	res = DLWalk((DLLIST *)gDListAlmPhy, &DelAddr, ProcFindAlmItem, &info);
	
	//如果存在,删除该记录,否则返回
	if(res == DL_FIND_OK){
		if(DelAddr != NULL){
			DelData = (B_ALM_INFO *)DLGetData(DelAddr, NULL, NULL);
			if(DelData != NULL){
				//保存链表地址
				if(DelAddr->Prev !=NULL){
					gDListAlmPhy = DelAddr->Prev;
				}
				else if(DelAddr->Next !=NULL)
				{
					gDListAlmPhy = DelAddr->Next;
				} 
				else{
					gDListAlmPhy = NULL;
				}
				//删除原有的信息
				DLDelete((DLLIST *)DelAddr);
			}
		}
	}
	else{
// 		if(B_PRINTF) printf("%s:%d The Phy records (%d) do not exist.\n", __FILE__, __LINE__, info.bAlmID);
		//解锁
		pthread_mutex_unlock(&mtPhyAlm);
		return -1;
	}
	
	//打印链表
// 	iListNum = DLCount((DLLIST *)gDListAlmPhy);
// 	if(B_PRINTF) printf("(%s:%d)Del::List  has  %d Infos.\n", __FILE__, __LINE__, iListNum);		
// 	res = DLWalk((DLLIST *)gDListAlmPhy, NULL, ProcPrintAlmItems, NULL);
	
	//解锁
	pthread_mutex_unlock(&mtPhyAlm);
	
	//--- 更新显示标志 ---
	bResetView_Phy = TRUE;

// 	if(B_PRINTF) printf("%s:%d Del One PhyAlm Info.\n", __FILE__, __LINE__);
	return 0;
}

/*
	改变一条生理报警的级别
*/
static int ChangePhyAlarm(B_ALM_INFO info)
{
	int res;
	
	//删除指定的记录
	res = PhyAlarmLifted(info);
	
	//如果记录不存在(删除不成功)直接返回
	if(res != 0){
		return -1;
	}
	
	//如果删除成功,则根据目前的报警级别增加新的报警记录
	//如果报警解别是关闭,直接返回
	if(info.bClass == ALARM_OFF){
		return -1;
	}
	
	//添加新的记录
	AddPhyAlarm(info);
	
	return 0;
}

/*
	查找当前生理报警的最高级别
	return -1: error
	pInfo: 存放报警记录详细资料
*/
static int GetPhyAlarmClass(PB_ALM_INFO pInfo)
{
	int res;
	int iListNum = 0;
	int iClass = -1;
	int i;
	DLLIST *FindAddr=NULL;			//查找到的记录的地址
	B_ALM_INFO *pFindInfo=NULL;		//查找到的记录
	B_ALM_INFO FindInfo;				//查找到的记录
		
	iListNum = DLCount((DLLIST *)gDListAlmPhy);
	
	if(iListNum <=0) iClass = -1;
	//遍历链表,查找级别最高的记录
	FindAddr = DLGetFirst(gDListAlmPhy);
	for(i=0; i<iListNum;i++){
		if(FindAddr != NULL){
// 			pthread_mutex_lock(&mtPhyAlm);
			pFindInfo = (B_ALM_INFO *)DLGetData(FindAddr, NULL, NULL);
			FindInfo = *pFindInfo;
			FindAddr = FindAddr->Next;
// 			pthread_mutex_unlock(&mtPhyAlm);
			if(pFindInfo !=NULL){
				if(FindInfo.bClass == ALARM_INFO) FindInfo.bClass = ALARM_OFF;
				if(iClass < FindInfo.bClass){
					iClass = FindInfo.bClass;
					*pInfo = *pFindInfo;
				}
				//如果查到第一个高级报警的记录,就不用再查了
				if(iClass >= ALARM_HIGH && iClass != ALARM_INFO){
					break;	
				}
			}
		}
		else{
			if(B_PRINTF) printf("%s:%d No Data Found.\n", __FILE__, __LINE__);
		}
				
	}
	
	return iClass;
}


/*
	增加一条技术参数报警记录
*/
static int AddTecAlarm(B_ALM_INFO info)
{
	int i;
	int res;
	int iListNum;
	DLLIST *FindAddr=NULL;			//查找到的记录的地址

	if(info.bAlmID == AT_NONE){
// 		if(B_PRINTF) printf("%s:%d Append Unkonwn Type Alarm.\n", __FILE__, __LINE__);
		return 0;	
	} 
		
	//加锁
	pthread_mutex_lock(&mtTecAlm);

	//检查该记录是否已经存在
	res = DLWalk((DLLIST *)gDListAlmTec, &FindAddr, ProcFindAlmItem, &info);
	
	//如果该记录不存在,则添加新的记录,否则返回
	if(res == DL_FIND_ERR){
		if(DL_SUCCESS != DLPrepend((DLLIST **)&gDListAlmTec, 0, &info, sizeof(B_ALM_INFO))){
//  			if(B_PRINTF) printf("(%s:%d)Couldn't allocate enough memory.\n", __FILE__, __LINE__);
		}	
		else{
// 			if(B_PRINTF) printf("(%s:%d)Add record at end of list.\n", __FILE__, __LINE__);	
		}
	}
	else{
// 		if(B_PRINTF) printf("%s:%d The record has been in existence.\n", __FILE__, __LINE__);
		//解锁
		pthread_mutex_unlock(&mtTecAlm);
		return -1;
	}
	

//		打印链表
// 	iListNum = DLCount((DLLIST *)gDListAlmTec);
// 	if(B_PRINTF) printf("(%s:%d)List  has  %d Infos.\n\n", __FILE__, __LINE__, iListNum);		
// 	if(iListNum>0) res = DLWalk((DLLIST *)gDListAlmTec, NULL, ProcPrintAlmItems, NULL);
		
	//解锁
	pthread_mutex_unlock(&mtTecAlm);
	
	//重新显示
	bResetView_Tec = TRUE;
	
	if(B_PRINTF) printf("%s:%d Add One TecAlm Info.\n", __FILE__, __LINE__);
	
	return 0;
}

/*
	解除一条技术报警
*/
static int TecAlarmLifted(B_ALM_INFO info)
{
	int i;
	int res;
	int iListNum;
	DLLIST *DelAddr=NULL;			//查找到的记录的地址
	DLLIST *SaveList;				//保存链表起始位置
	B_ALM_INFO *DelData=NULL;		//查找到的记录
	
	//加锁
	pthread_mutex_lock(&mtTecAlm);
	
	//检查该记录是否存在
	res = DLWalk((DLLIST *)gDListAlmTec, &DelAddr, ProcFindAlmItem, &info);
	
	//如果存在,删除该记录,否则返回
	if(res == DL_FIND_OK){
		if(DelAddr != NULL){
			DelData = (B_ALM_INFO *)DLGetData(DelAddr, NULL, NULL);
			if(DelData != NULL){
				//保存链表地址
				if(DelAddr->Prev !=NULL){
					gDListAlmTec = DelAddr->Prev;
				}
				else if(DelAddr->Next !=NULL) {
					gDListAlmTec = DelAddr->Next;
				} 
				else{
					gDListAlmTec = NULL;
				}
				//删除原有的信息
				DLDelete((DLLIST *)DelAddr);
			}
		}
	}
	else{
// 		if(B_PRINTF) printf("%s:%d The Tec records (%d) do not exist.\n", __FILE__, __LINE__, info.bAlmID);
		//解锁
		pthread_mutex_unlock(&mtTecAlm);
		return -1;
	}
	
	//打印链表
// 	iListNum = DLCount((DLLIST *)gDListAlmTec);
// 	if(B_PRINTF) printf("(%s:%d)Del::List  has  %d Infos.\n", __FILE__, __LINE__, iListNum);		
// 	res = DLWalk((DLLIST *)gDListAlmTec, NULL, ProcPrintAlmItems, NULL);
	
	//解锁
	pthread_mutex_unlock(&mtTecAlm);
	
	//--- 更新显示标志 ---
	bResetView_Tec = TRUE;

// 	if(B_PRINTF) printf("%s:%d Del One PhyAlm Info.\n", __FILE__, __LINE__);
	return 0;
}

/*
	改变一条技术报警的级别
*/

static int ChangeTecAlarm(B_ALM_INFO info)
{
	int res;
	
	//删除指定的记录
	res = TecAlarmLifted(info);
	
	//如果记录不存在(删除不成功)直接返回
	if(res != 0){
		return -1;
	}
	
	//如果删除成功,则根据目前的报警级别增加新的报警记录
	//如果报警解别是关闭,直接返回
	if(info.bClass == ALARM_OFF){
		return -1;
	}
	
	//添加新的记录
	AddTecAlarm(info);
	
	return 0;
}

/*
	查找当前技术报警的最高级别
	return -1: error
	pInfo: 存放报警记录详细资料
*/
static int GetTecAlarmClass(PB_ALM_INFO pInfo)
{
	int res;
	int iListNum = 0;
	int iClass = -1;
	int i;
	DLLIST *FindAddr=NULL;			//查找到的记录的地址
	B_ALM_INFO *pFindInfo=NULL;		//查找到的记录
	B_ALM_INFO FindInfo;				//查找到的记录
		
	iListNum = DLCount((DLLIST *)gDListAlmTec);
	
	if(iListNum <=0) iClass = -1;
	
	//遍历链表,查找级别最高的记录
	FindAddr = DLGetFirst(gDListAlmTec);
	for(i=0; i<iListNum;i++){
		if(FindAddr != NULL){
// 			pthread_mutex_lock(&mtPhyAlm);
			pFindInfo = (B_ALM_INFO *)DLGetData(FindAddr, NULL, NULL);
			FindInfo = *pFindInfo;
			FindAddr = FindAddr->Next;
// 			pthread_mutex_unlock(&mtPhyAlm);
			if(pFindInfo !=NULL){
				if(FindInfo.bClass == ALARM_INFO) FindInfo.bClass = ALARM_OFF;
				if(iClass < FindInfo.bClass){
					iClass = FindInfo.bClass;
					*pInfo = *pFindInfo;
				}
				//如果查到第一个高级报警的记录,就不用再查了
				if(iClass >= ALARM_HIGH && iClass !=ALARM_INFO){
					break;	
				}
			}
		}
		else{
			if(B_PRINTF) printf("%s:%d No Data Found.\n", __FILE__, __LINE__);
		}
				
	}
	
	return iClass;
}


static int Test()
{
	int res;
	B_ALM_INFO testInfo;
	
	if(B_PRINTF) printf("\n---------------------------------Add Debug 00\n");
	testInfo.bAlmID = AP_SPO2_HIGH;
	testInfo.bStatus = ALMSTATUS_ALARM;
	testInfo.bClass = ALARM_LOW;
	res = AddPhyAlarm(testInfo);
// 	sleep(1);
	if(B_PRINTF) printf("\n---------------------------------Add Debug 01\n");
	testInfo.bAlmID = AP_HR_HIGH;
	testInfo.bStatus = ALMSTATUS_ALARM;
	testInfo.bClass = ALARM_MID;
	res = AddPhyAlarm(testInfo);
// 	sleep(1);
	if(B_PRINTF) printf("\n---------------------------------Add Debug 02\n");
	testInfo.bAlmID = AP_SPO2_HIGH;
	testInfo.bStatus = ALMSTATUS_ALARM;
	testInfo.bClass = ALARM_LOW;
	res = AddPhyAlarm(testInfo);
// 	sleep(1);
	if(B_PRINTF) printf("\n---------------------------------Add Debug 03\n");
	testInfo.bAlmID = AP_TD_HIGH;
	testInfo.bStatus = ALMSTATUS_ALARM;
	testInfo.bClass = ALARM_HIGH;
	res = AddPhyAlarm(testInfo);
// 	sleep(1);
	if(B_PRINTF) printf("\n---------------------------------Add Debug 04\n");
	testInfo.bAlmID = AP_NSYS_HIGH;
	testInfo.bStatus = ALMSTATUS_ALARM;
	testInfo.bClass = ALARM_HIGH;
	res = AddPhyAlarm(testInfo);
// 	sleep(1);
	if(B_PRINTF) printf("\n---------------------------------Add Debug 05\n");
	testInfo.bAlmID = AP_NDIA_HIGH;
	testInfo.bStatus = ALMSTATUS_ALARM;
	testInfo.bClass = ALARM_HIGH;
	res = AddPhyAlarm(testInfo);
	sleep(1);
	if(B_PRINTF) printf("\n---------------------------------Del Debug 00\n");
	testInfo.bAlmID = AP_SPO2_HIGH;
	testInfo.bStatus = ALMSTATUS_NORMAL;
	testInfo.bClass = ALARM_HIGH;
	res = PhyAlarmLifted(testInfo);
// 	sleep(1);
	if(B_PRINTF) printf("\n---------------------------------Del Debug 01\n");
	testInfo.bAlmID = AP_T1_HIGH;
	testInfo.bStatus = ALMSTATUS_NORMAL;
	testInfo.bClass = ALARM_HIGH;
	res = PhyAlarmLifted(testInfo);
//  	sleep(1);
	if(B_PRINTF) printf("\n---------------------------------Del Debug 02\n");
	testInfo.bAlmID = AP_TD_HIGH;
	testInfo.bStatus = ALMSTATUS_NORMAL;
	testInfo.bClass = ALARM_LOW;
	res = PhyAlarmLifted(testInfo);
//  	sleep(1);
		
	res = GetPhyAlarmClass(&testInfo);
	if(B_PRINTF) printf("%s:%d  At present, the most alarm record is : Class(%d)  -----  ID-%d, Class-%d, Status-%d.\n", __FILE__, __LINE__, 
	       res, testInfo.bAlmID, testInfo.bClass, testInfo.bStatus);
	
	if(B_PRINTF) printf("\n---------------------------------Add Debug 00\n");
	testInfo.bAlmID = AT_ECGOFF_V;
	testInfo.bStatus = ALMSTATUS_ALARM;
	testInfo.bClass = ALARM_LOW;
	res = AddTecAlarm(testInfo);
// 	sleep(1);
	if(B_PRINTF) printf("\n---------------------------------Add Debug 01\n");
	testInfo.bAlmID = AT_RESP_OVERRANGE;
	testInfo.bStatus = ALMSTATUS_ALARM;
	testInfo.bClass = ALARM_MID;
	res = AddTecAlarm(testInfo);
// 	sleep(1);
	if(B_PRINTF) printf("\n---------------------------------Add Debug 02\n");
	testInfo.bAlmID = AT_SPO2_MODULEINITERR;
	testInfo.bStatus = ALMSTATUS_ALARM;
	testInfo.bClass = ALARM_LOW;
	res = AddTecAlarm(testInfo);
// 	sleep(1);
	if(B_PRINTF) printf("\n---------------------------------Add Debug 03\n");
	testInfo.bAlmID = AT_SPO2_OVERRANGE;
	testInfo.bStatus = ALMSTATUS_ALARM;
	testInfo.bClass = ALARM_HIGH;
	res = AddTecAlarm(testInfo);
// 	sleep(1);

	
	return 0;
}


//---------------------------- interface ----------------------------------
/*
	初始化报警管理
*/
int InitAlarmManage()
{
	int res;
	
	//初始化互斥量
	res = pthread_mutex_init(&mtPhyAlm, NULL);
	res = pthread_mutex_init(&mtTecAlm, NULL);
 	
	//初始化报警显示
 	InitAlarmView();
	
	//初始化报警发声
	InitAlarmSound();
	
	//初始化报警灯光
	InitAlarmLight();
	
// 	Test();
			
	return 0;
}

/*
	添加一条生理报警信息
*/
int AddAlarm_Phy(B_ALM_INFO InfoAlm)
{
	int res;
	
	//return 0;
	
	res = AddPhyAlarm(InfoAlm);

	if(res ==0){
		//TODO:如果成功增加的话,此处需增加报警存储
		if(B_PRINTF) printf("%s:%d Add one alarm info. AlmID is %d, Class is %d. Print is %d.\n", __FILE__, __LINE__, 
		       InfoAlm.bAlmID, InfoAlm.bClass, InfoAlm.bPrint);
		SaveOneAlarmData(InfoAlm);
		
		//如果处在报警暂停状态下,则自动解除报警暂停
		gbSuspending = FALSE;
		
		//如果处在静音状态，则取消静音
		gbSilence = FALSE;
	}

	return res;
}



/*
	删除一条生理报警信息
*/
int AlarmLifted_Phy(B_ALM_INFO InfoAlm)
{
	int res;
	
//  	return -1;
	
	res = PhyAlarmLifted(InfoAlm);
	
	return res;
}

/*
	改变一条生理报警的级别
*/
int ChangeAlarm_Phy(B_ALM_INFO InfoAlm)
{
	int res;
	
	ChangePhyAlarm(InfoAlm);
			
	return -1;	
}

		
/*
	得到当前生理报警最高级别的报警
	返回级别
*/
int GetHighestAlarm_Phy(PB_ALM_INFO pInfoAlm)
{
	int res;
	
	if(pInfoAlm == NULL) return -1;
	
	res = GetPhyAlarmClass(pInfoAlm);
	
	return res;
}

/*
	添加一条技术报警信息
*/
int AddAlarm_Tec(B_ALM_INFO InfoAlm)
{
	int res;
	
  	//return -1;
	
	res = AddTecAlarm(InfoAlm);

	//如果处在报警暂停状态下,则自动解除报警暂停
	gbSuspending = FALSE;
	
	//如果处在静音状态，则取消静音
	gbSilence = FALSE;

	return res;
}

/*
	删除一条技术报警信息
*/
int AlarmLifted_Tec(B_ALM_INFO InfoAlm)
{
	int res;
	
// 	return -1;
	
	res = TecAlarmLifted(InfoAlm);
	
	return res;
}

/*
	改变一条生理报警的级别
*/
int ChangeAlarm_Tec(B_ALM_INFO InfoAlm)
{
	int res;
		
	ChangeTecAlarm(InfoAlm);
			
	return -1;	
}


/*
	得到当前技术报警最高级别的报警
	返回级别
*/
int GetHighestAlarm_Tec(PB_ALM_INFO pInfoAlm)
{
	int res;
	
	if(pInfoAlm == NULL) return -1;
	
	res = GetTecAlarmClass(pInfoAlm);
	
	return res;
}

/*
	得到当前生理&技术报警最高级别的报警
	返回级别
*/
int GetHighestAlarm()
// int GetHighestAlarm(PB_ALM_INFO pInfoAlm)
{
	int res, res_phy, res_tec;
	B_ALM_INFO info_phy;
	B_ALM_INFO info_tec;
	
	//得到最高级别的生理报警
	res_phy = GetPhyAlarmClass(&info_phy);
// 	if(res_phy == ALARM_INFO) res_phy = ALARM_OFF;
	//得到最高级别的技术报警
	res_tec = GetTecAlarmClass(&info_tec);
// 	if(res_tec == ALARM_INFO) res_tec = ALARM_OFF;
	//比较两个级别
	res = MAX(res_phy, res_tec);
	
// 	if(B_PRINTF) printf("%s:%d Current Highest Alarm Class is %d.\n", __FILE__, __LINE__, res);
	
	return res;
}
//按下pause键时清除一次性报警标志位
int Clean_Alarm()
{
	unsigned char buf;
	buf |= 0x07<<2;
	IsAlm_Power(&buf,FALSE);
//	printf("---------------clean alarm\n");
	return 0;
}


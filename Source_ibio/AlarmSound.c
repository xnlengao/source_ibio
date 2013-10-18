/*************************************************************************** 
 *	Module Name:	AlarmSound
 *
 *	Abstract:		报警声音显示
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-07-04 09:52:35
 ***************************************************************************/
/*
思路:
	1、定时更新(1s),查询当前是否报警和报警的最高级别
	2、以报警的最高级别作为发声和灯光的依据
*/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "AlarmManage.h"


//参数报警发声线程
static pthread_t ptAlarmSound;
//发声线程信号量
static sem_t semAlarmSound;

//高级报警间隔 10+4S 
#define TIMER_ALARMSOUND_HIGH		14
//中级报警间隔 25S
#define TIMER_ALARMSOUND_MIDDLE	25
//低级报警间隔 25S
#define TIMER_ALARMSOUND_LOW		25

//是否进入报警暂停状态
extern BOOL gbSuspending;

//报警发声线程
static void *ThreadProcAlarmSound(void *arg)
{
	//之前的报警级别
	BYTE bPrevAlarmClass = ALARM_OFF;
	//当前的报警级别
	BYTE bCurrentAlarmClass = ALARM_OFF;
	//发声间隔计数器
	int iSoundCount = 0;
	int iSoundCountMax = 0;
	
	for(;;){
		sem_wait(&semAlarmSound);	
				
		//得到当前报警的最高级别
		bCurrentAlarmClass = GetHighestAlarm();
		
		//与之前的级别比较,如果相同,则增加计数器,然后根据计数器决定是否发声
		//否则,立即按照当前级别发声,然后重置计数器
		if(bPrevAlarmClass != bCurrentAlarmClass){
			//重置计数器
			iSoundCount = 0;
			//按照当前级别发声
			AlarmSound(bCurrentAlarmClass);
//  			if(B_PRINTF) printf("%s:%d Pre Class is %d. Current Class is %d. Sound\n", __FILE__, __LINE__, bPrevAlarmClass, bCurrentAlarmClass);
		}
		else{
			switch(bCurrentAlarmClass){
				case ALARM_HIGH:{
					iSoundCountMax = TIMER_ALARMSOUND_HIGH;
				}break;
				case ALARM_MID:{
					iSoundCountMax = TIMER_ALARMSOUND_MIDDLE;
				}break;
				case ALARM_LOW:{
					iSoundCountMax = TIMER_ALARMSOUND_LOW;
				}break;
				default:{
					iSoundCountMax = TIMER_ALARMSOUND_LOW;
				}break;
			}
			
			//如果计数器到时,则按照级别发声,重置计数器,否则计数器++
			if(iSoundCount >=iSoundCountMax){
				//按照当前级别发声
				if(!gbSuspending)
					AlarmSound(bCurrentAlarmClass);
// 				if(B_PRINTF) printf("%s:%d Class is %d. Timer Count is %d. Sound\n", __FILE__, __LINE__, bCurrentAlarmClass, iSoundCount);		
				iSoundCount = 0;
			}
			else{
				iSoundCount ++;
			}
		}
		
		bPrevAlarmClass = bCurrentAlarmClass;
	}
}

//----------------- interface --------------------
/*
	创建报警发声线程
*/
int InitAlarmSound()
{
	int res;
	
	res = sem_init(&semAlarmSound, 0, 0);
	
	res = pthread_create(&ptAlarmSound, NULL, ThreadProcAlarmSound, NULL);
	
	return res;
}


/*
	更新发生线程(定时器调用)
*/
int UpdateAlarmSound()
{
	sem_post(&semAlarmSound);
	return 0;
}

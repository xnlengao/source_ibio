/*************************************************************************** 
 *	Module Name:	AlarmLight
 *
 *	Abstract:		±¨¾¯µÆ¹âÏÔÊ¾
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-07-04 14:16:11
 ***************************************************************************/

#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "AlarmManage.h"


#define FLASH_INTER		200000

static pthread_t ptAlarmLight;
static sem_t semAlarmLight;

#define TIMER_ALARMLIGHT_HIGH		0		
#define TIMER_ALARMLIGHT_MIDDLE	1
#define TIMER_ALARMSOUND_LOW		0


static int AlarmLight(BYTE bClass)
{
	int res;
	
	
	switch(bClass){
		case ALARM_HIGH:{
			YellowLed_Off();
			GreenLed_Off();	
			RedLed_On(HIGH_ALARM_LED);
		}break;
		case ALARM_MID:{
			RedLed_Off();
			GreenLed_Off();	
			YellowLed_On(MIDDLE_ALARM_LED);
		}break;
		case ALARM_LOW:{
			RedLed_Off();
			YellowLed_Off();
			GreenLed_On(LOW_ALARM_LED);
		}break;
		default:{
			RedLed_Off();
			YellowLed_Off();
			GreenLed_Off();	
		}break;
	}
	
	return res;
}


static void *ThreadProcAlarmLight(void *arg)
{
	BYTE bPrevAlarmClass = ALARM_OFF;
	BYTE bCurrentAlarmClass = ALARM_OFF;
	int iLightCount = 0;
	int iLightCountMax = 0;
	
	for(;;){
		sem_wait(&semAlarmLight);	
				
		bCurrentAlarmClass = GetHighestAlarm();

		AlarmLight(bCurrentAlarmClass);
	
	}
}

//----------------- interface --------------------
int InitAlarmLight()
{
	int res;
	
	res = sem_init(&semAlarmLight, 0, 0);
	
	res = pthread_create(&ptAlarmLight, NULL, ThreadProcAlarmLight, NULL);
	
	return res;
}


int UpdateAlarmLight()
{
 	sem_post(&semAlarmLight);
	return 0;
}



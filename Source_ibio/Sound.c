/*************************************************************************** 
 *	Module Name:	Sound
 *
 *	Abstract:	发声线程函数
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2007-06-22 10:01:50
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"

//发声信号量
sem_t	semSound;				
sem_t	semBeep;
//发声
static pthread_t ptSound;
static pthread_t ptBeep;
	

//-- 声音设备 --
static int gfdSound = 0;
//-- 混音设备 --
static int gfdMixer = 0;

//-- 气阀和报警灯等设备接口 ---
extern int gfdInterface;

//声音文件的大小 Byte
//beep-1368 	//Middle-32756	//Low-13676	//High-111436



//--------- 声音文件Buffer ----------
//Beep
#define SOUNDBUFF_BEEP_MAX	1368		//1368
unsigned char SoundBuff_Beep[SOUNDBUFF_BEEP_MAX]={0};
//High Alarm
#define SOUNDBUFF_ALM_HIGH_MAX	(117360/2)
unsigned char SoundBuff_Alm_High[SOUNDBUFF_ALM_HIGH_MAX]={0};
//Middle Alarm
#define SOUNDBUFF_ALM_MIDDLE_MAX	60000	// 32756
unsigned char SoundBuff_Alm_Middle[SOUNDBUFF_ALM_MIDDLE_MAX]={0};
//Low Alarm
#define SOUNDBUFF_ALM_LOW_MAX	60000	//13676
unsigned char SoundBuff_Alm_Low[SOUNDBUFF_ALM_LOW_MAX]={0};

//调节音量
int SetVolume(BYTE bVolume);

static BOOL bAlarming = FALSE;

//是否静音
BOOL gbSilence = FALSE;

/*
	初始化音频文件缓冲
*/
static int InitSoundBuffer()
{
	int fd = 0;
	int iLen = 0;
	
	//Beep
 	if((fd = open ("sound/beep.wav",O_RDWR))<0){
		printf(" Can't open beep.wav!\n");
		return -1;
	}
	memset(SoundBuff_Beep, 0, sizeof SoundBuff_Beep);
	iLen = read(fd, SoundBuff_Beep, sizeof SoundBuff_Beep);
	close(fd);
	
	//Alarm High
	if((fd = open ("sound/Alarm_High.wav",O_RDWR))<0){
		printf("--------------Can't open Alarm_High.wav!\n");
		return -1;
	}
	memset(SoundBuff_Alm_High, 0, sizeof SoundBuff_Alm_High);
	iLen = read(fd, SoundBuff_Alm_High, sizeof SoundBuff_Alm_High);
//	memcpy(SoundBuff_Alm_High+(sizeof SoundBuff_Alm_High)/2,SoundBuff_Alm_High, (sizeof SoundBuff_Alm_High)/2);
	close(fd);
	
	//Alarm Middle
	if((fd = open ("sound/Alarm_Middle.wav",O_RDWR))<0){
		printf(" --------------Can't open Alarm_Middle.wav!\n");
		return -1;
	}
	memset(SoundBuff_Alm_Middle, 0, sizeof SoundBuff_Alm_Middle);
	iLen = read(fd, SoundBuff_Alm_Middle, sizeof SoundBuff_Alm_Middle);
	close(fd);
	
	//Alarm Low
	if((fd = open ("sound/Alarm_Low.wav",O_RDWR))<0){
		printf("----------------Can't open Alarm_Low.wav!\n");
		return -1;
	}
	
	memset(SoundBuff_Alm_Low, 0, sizeof SoundBuff_Alm_Low);
	iLen = read(fd, SoundBuff_Alm_Low, sizeof SoundBuff_Alm_Low);
	close(fd);
	
	return 0;
}


/*
	初始化音频设备
*/
static int InitSoundDevice()
{
	int i;
	
	//DSP
//  	gfdSound = open("/dev/sound/dsp", O_WRONLY | O_ACCMODE);
 	gfdSound = open("/dev/dsp", O_WRONLY);
	
	if(gfdSound <0){
		printf("%s:%d ----------------------------------Open Sound Device Failure.\n", __FILE__, __LINE__);
		return -1;
	} 
		
	i=1;
	ioctl(gfdSound,SNDCTL_DSP_RESET,(char *)&i);
	i=1;
	ioctl(gfdSound,SNDCTL_DSP_SYNC,(char *)&i);
	i=1;
	ioctl(gfdSound,SNDCTL_DSP_NONBLOCK,(char *)&i);
  	i=16000;
 	ioctl(gfdSound,SNDCTL_DSP_SPEED,(char *)&i);
//  	i=1;
//  	ioctl(gfdSound,SNDCTL_DSP_CHANNELS,(char *)&i);
//	i=1;
 // 	ioctl(gfdSound,SNDCTL_DSP_STEREO,(char *)&i);
 	i=16;
 	ioctl(gfdSound,SNDCTL_DSP_SETFMT,(char *)&i);
// 	i=3;
// 	ioctl(gfdSound,SNDCTL_DSP_SETTRIGGER,(char *)&i);
// 	i=3;
// 	ioctl(gfdSound,SNDCTL_DSP_SETFRAGMENT,(char *)&i);
// 	i=1;
// 	ioctl(gfdSound,SNDCTL_DSP_PROFILE,(char *)&i);
	
	return 0;
}



/*
*/
static int SyncSoundDevice()
{
	int iValue;
	
	if(gfdSound <=0) return -1;
	
	iValue = 0;
	ioctl(gfdSound,SNDCTL_DSP_SYNC,(char *)&iValue);
	
	return 0;
}

/*
	设置音量(操作mixer)
	bVolume: 0~100
*/
static int SetVolume_Mixer(BYTE bVolume)
{
	int fd;
	long int iVolume = 0;	//百分比	
	long int iGain = 0;	//百分比
	int iLeft, iRight;
	
	fd = open("/dev/sound/mixer", O_RDWR);

	if(fd<0){
		printf("%s:%d Open Mixer Device Failure.\n", __FILE__, __LINE__);
		return -1;
	} 
	
	iVolume = bVolume;
	
	//调节增益
	iGain = (20<<8) + 20;
	ioctl(fd,SOUND_MIXER_WRITE_OGAIN,&iGain);
	
	ioctl(fd,SOUND_MIXER_READ_OGAIN,&iGain);

	iLeft = iGain & 0xFF;
	iRight = (iGain & 0xFF00)>>8;
	printf("-------------%s:%d Gain %ld-- Left:%d, right:%d\n", __FILE__, __LINE__, iGain, iLeft, iRight);
	
	//调节音量
	ioctl(fd,SOUND_MIXER_WRITE_VOLUME,&iVolume);
	
	ioctl(fd,SOUND_MIXER_READ_VOLUME,&iVolume);
	
	iLeft = iVolume & 0xFF;
	iRight = (iVolume & 0xFF00)>>8;
	printf("-------------%s:%d Volume-- Left:%d, right:%d\n", __FILE__, __LINE__, iLeft, iRight);
	
	close(fd);
	return 0;
}

/*
	设置音量(操作mixer)
	bVolume: 0~100
	直接操作dsp
*/
static int SetVolume_Dsp(BYTE bVolume)
{
	int fd;
	long int iVolume = 0;	//百分比	
	long int iGain = 0;	//百分比
	int iLeft, iRight;
	
	fd = gfdSound;

	if(fd<0){
		printf("%s:%d Open Sound Device Failure.\n", __FILE__, __LINE__);
		return -1;
	} 
	
	iVolume = bVolume;
	
	//调节增益(不用，UDA1341的驱动没有实现)
// 	iGain = (20<<8) + 20;
// 	ioctl(fd,SOUND_MIXER_WRITE_OGAIN,&iGain);
// 	
// 	ioctl(fd,SOUND_MIXER_READ_OGAIN,&iGain);
// 	iLeft = iGain & 0xFF;
// 	iRight = (iGain & 0xFF00)>>8;
// // 	printf("-------------%s:%d Gain %d-- Left:%d, right:%d\n", __FILE__, __LINE__, iGain, iLeft, iRight);
	
	//调节音量
// 	printf("-------------%s:%d Set Volume-- %.ld\n", __FILE__, __LINE__, iVolume);
  	ioctl(fd,SOUND_MIXER_WRITE_VOLUME,&iVolume);
	
	ioctl(fd,SOUND_MIXER_READ_VOLUME,&iVolume);
	
	iLeft = iVolume & 0xFF;
	iRight = (iVolume & 0xFF00)>>8;
//  	printf("-------------%s:%d Read Volume-- Left:%d, right:%d\n", __FILE__, __LINE__, iLeft, iRight);
	
	return 0;
}

/*
	Beep发声
*/
static int Beep_Sound()
{
	if(gfdSound <= 0) return -1;
	
	if(gCfgEcg.bBeepVolume == VOLUME_OFF) return -1;
	
	if(!bAlarming){
		//复位
  		SyncSoundDevice();
		//设置音量
		SetVolume(gCfgEcg.bBeepVolume*10);
		//发声
		write(gfdSound, SoundBuff_Beep, SOUNDBUFF_BEEP_MAX);
	}
	
	return 0;
}
		
/*
	高级报警
*/
static int Sound_Alarm_High()
{

	if(gfdSound <= 0) return -1;
		
	if(gCfgAlarm.bVolume == VOLUME_OFF) return -1;
	
	bAlarming = TRUE;
	//复位
	SyncSoundDevice();
	//设置音量
	SetVolume(gCfgAlarm.bVolume*10);
	
	//发声
	write(gfdSound, SoundBuff_Alm_High, SOUNDBUFF_ALM_HIGH_MAX);
	//复位
	SyncSoundDevice();
	//发声
	write(gfdSound, SoundBuff_Alm_High, SOUNDBUFF_ALM_HIGH_MAX);
	//延时等待发声完毕
	usleep(3000000);
	
	bAlarming = FALSE;
	
	return 0;
}

 /*
	中级报警
 */
static int Sound_Alarm_Middle()
{
	if(gfdSound <= 0) return -1;
	if(gCfgAlarm.bVolume == VOLUME_OFF) return -1;
	
	bAlarming = TRUE;
	//复位
	SyncSoundDevice();
	//设置音量
	SetVolume(gCfgAlarm.bVolume*10);
	//发声
	write(gfdSound, SoundBuff_Alm_Middle, SOUNDBUFF_ALM_MIDDLE_MAX);
	
	//延时等待发声完毕
	usleep(1500000);
	
	bAlarming = FALSE;
	return 0;
}

/*
	低级报警
*/
static int Sound_Alarm_Low()
{

	if(gfdSound <= 0) return -1;
	if(gCfgAlarm.bVolume == VOLUME_OFF) return -1;
	
	bAlarming = TRUE;
	//复位
	SyncSoundDevice();
	//设置音量
	SetVolume(gCfgAlarm.bVolume*10);
	//发声
	write(gfdSound, SoundBuff_Alm_Low, SOUNDBUFF_ALM_LOW_MAX);
	
	//延时等待发声完毕
	usleep(1200000);
	
	bAlarming = FALSE;
	return 0;
}

static void *ThreadProcBeep(void *arg)
{
	int iSemValue;	//绘图信号量的数量
	
	for(;;){
		sem_wait(&semBeep);
		
		sem_getvalue(&semBeep, &iSemValue);
		
		if(iSemValue >1){
// 			printf("********iWaveSemValue=%d\n",iSemValue);
			sem_init(&semBeep, 0, 0);
			sem_getvalue(&semBeep, &iSemValue);
// 			printf("********iWaveSemValue=%d\n",iSemValue);
 			continue;
		}

 		Beep_Sound();
	}
}

//------------------- interface -------------------
/*
	创建发声线程	
	return: 0-OK, -1:Failure
*/
int CreateSoundProc()
{
	int res;
	
	//初始化音频设备
	InitSoundDevice();
	
	//初始化音频文件
	InitSoundBuffer();
	
	//设置初始音量,测试
	SetVolume(55);
	
	//初始化信号量
	res = sem_init(&semBeep, 0, 0);

	//创建beep线程
	res = pthread_create(&ptBeep, NULL, ThreadProcBeep, NULL);
			
	return res;
}

/*
	设置音量
	bVolume : 音量(0-100)
*/
int SetVolume(BYTE bVolume)
{

	SetVolume_Dsp(bVolume);
	
	return 0;
}

/*
	Beep发声
*/
int Sound_Beep()
{
	if(gfdSound <= 0) return -1;
	
	//电源正在关闭时，不处理
	if(gPowerStatus.bShutDown == POWER_SHUTDOWN){
		return 0;		
	}
		
	if(!gbSilence){
		sem_post(&semBeep);
	}
	
	return 0;
}

/*
	报警发声
*/
int AlarmSound(BYTE bClass)
{
	int res=0;
	
	//电源正在关闭时，不处理
	if(gPowerStatus.bShutDown == POWER_SHUTDOWN){
		return 0;		
	}
	
	if(!gbSilence){
		switch(bClass){
			case ALARM_HIGH:{
				res = Sound_Alarm_High();
			}break;
			case ALARM_MID:{
				res = Sound_Alarm_Middle();
			}break;
			case ALARM_LOW:{
				res = Sound_Alarm_Low();
			}break;
			default:break;
		}
	}
	
	return res;
}

 /*
	声光测试
 */
int Test_LightAndSound()
{
	if(gfdSound <= 0) return -1;

	//复位
	SyncSoundDevice();
	//设置音量
	SetVolume(50);
	//发声
	write(gfdSound, SoundBuff_Alm_Middle, SOUNDBUFF_ALM_MIDDLE_MAX);
	//延时等待发声完毕
// 	usleep(1500000);
	
	RedLed_On(LOW_ALARM_LED);
	usleep(300000);
	RedLed_Off();
	YellowLed_On(LOW_ALARM_LED);
	usleep(300000);
	YellowLed_Off();
	GreenLed_On(LOW_ALARM_LED);
	usleep(300000);
	GreenLed_Off();
	
	
	return 0;
}

/*************************************************************************** 
 *	Module Name:	ExtDevice_Funcs
 *
 *	Abstract:	扩展的硬件的操作函数
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-05-28 09:48:38
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "ExtDevice_Funcs.h"


#define DEVICE_FUNCBTN 			"/dev/input/event0"
#define DEVICE_ROTOR 			"/dev/rotor0"
#define DEVICE_KEY				"/dev/keyboard0"
#define DEVICE_LED				"/dev/cpld0"
#define DEVICE_INTERFACE 		"/dev/interface" 
#define DEVICE_PWM 				"/dev/pwm_device" 
#define DEVICE_WDT				"/dev/watchdog" 


//-- ioctl para --
#define INTERFACE_IOC_MAGIC 	'j'

#define IOCTL_CMD_MAGIC 'c'
//led灯闪烁频率
#define 	IO_LED_FREQ 	
/*点亮红色报警灯,以及设置闪烁频率*/
#define WCE2400_CPLD_ENABLE_RED_LED	_IOW(IOCTL_CMD_MAGIC, 0xc6, unsigned char)	
#define WCE2400_CPLD_DISABLE_RED_LED	_IO(IOCTL_CMD_MAGIC, 0xc7)	
/*点亮黄色报警灯,以及设置闪烁频率*/
#define WCE2400_CPLD_ENABLE_YELLOW_LED	_IOW(IOCTL_CMD_MAGIC, 0xc8, unsigned char)
#define WCE2400_CPLD_DISABLE_YELLOW_LED	_IO(IOCTL_CMD_MAGIC, 0xc9)	
/*点亮绿色报警灯,以及设置闪烁频率*/
#define WCE2400_CPLD_ENABLE_GREEN_LED	_IOW(IOCTL_CMD_MAGIC, 0xca, unsigned char)
#define WCE2400_CPLD_DISABLE_GREEN_LED	_IO(IOCTL_CMD_MAGIC, 0xcb)	

/*成人阀(快阀)开*/
#define PUMP_IOC_QUICK_ON		_IO(INTERFACE_IOC_MAGIC, 5)
/*成人阀(快阀)关*/
#define PUMP_IOC_QUICK_OFF		_IO(INTERFACE_IOC_MAGIC, 6)
/*儿童阀(中阀)开*/
#define PUMP_IOC_MIDDLE_ON		_IO(INTERFACE_IOC_MAGIC, 7)
/*儿童阀(中阀)关*/
#define PUMP_IOC_MIDDLE_OFF		_IO(INTERFACE_IOC_MAGIC, 8)
/*获得机器工作状态：1－正常工作，0－16秒后将关闭电源*/
#define IOC_GETPOWERSTATUS		_IO(INTERFACE_IOC_MAGIC, 9)
/*获得电源类型：1－使用网电源，0－使用内置电池*/
#define IOC_GETPOWERTYPE		_IO(INTERFACE_IOC_MAGIC, 10)
/*获得电池电量：*/
#define IOC_GETBATTERYSTATUS	_IO(INTERFACE_IOC_MAGIC, 11)

//-- ioctl para --
#define PWMDEV_IOC_MAGIC 	'k'

/*改变占空比*/
#define PWMDEV_IOC_CHANGEDUTY	_IO(PWMDEV_IOC_MAGIC, 1)
/*改变定时值（周期）*/
#define PWMDEV_IOC_CHANGECYCLE	_IO(PWMDEV_IOC_MAGIC, 2)
/*启动定时器*/
#define PWMDEV_IOC_STARTTIMER	_IO(PWMDEV_IOC_MAGIC, 3)
/*关闭定时器*/
#define PWMDEV_IOC_CLOSETIMER	_IO(PWMDEV_IOC_MAGIC, 4)

//Watchdog
//-- ioctl para --
#define WD_IOC_MAGIC 	'j'
//初始化Watchdog
#define WD_IOC_INIT		_IO(WD_IOC_MAGIC, 1)
//启动
#define WD_IOC_START	_IO(WD_IOC_MAGIC, 2)
//停止
#define WD_IOC_STOP		_IO(WD_IOC_MAGIC, 3)
//喂狗
#define WD_IOC_FEED		_IO(WD_IOC_MAGIC, 4)


/*
	扩展的设备
*/
//旋钮
int gfdRotor= -1;
//event0 事件
int gfdKeyboard = -1;
//内核键盘
int gfdKey = -1;
//Led灯驱动
int gfdLed = -1;


//气阀和报警灯等设备接口
int gfdInterface = 0;
//气泵
int gfdPump = 0;
//看门狗
int gfdWdt = 0;


/*
	初始化键盘设备
*/
static int InitDevice_Key()
{
//keyboard
	gfdKey = open(DEVICE_KEY, O_RDWR);
	if(B_PRINTF)printf("***Device -- Key is %d***.\n", gfdKey);
	if(gfdKey<0) return -1;
	
//fun_btn
//	gfdKeyboard = open(DEVICE_FUNCBTN, O_RDWR);
//	printf("***Device -- Keyboard is %d***.\n", gfdKeyboard);
//	if(gfdKeyboard<0) return -1;
	
//rotor
	gfdRotor= open(DEVICE_ROTOR, O_RDWR);
	if(B_PRINTF)printf("***Device -- Rotor is %d***.\n", gfdRotor);
	gbKeyType = KEY_LR;
	if(gfdRotor<0) return -2;

	
	return 0;
}
static int InitDevice_Led()
{
	
	gfdLed= open(DEVICE_LED, O_RDWR);
	
	if(B_PRINTF)printf("Device -- Led is %d.\n", gfdLed);
	
	if(gfdLed<0) return -1;
	
	return 0;
}


static int InitDevice_Pump()
{
	
	gfdPump = open(DEVICE_PWM, O_RDWR);
	if(B_PRINTF)printf("Device -- PWM is %d.\n", gfdPump);
			
	if(gfdPump<=0) return -1;
		
	return 0;

}

/*
	初始化看门狗
*/
 int InitDevice_Wdt()
{
	gfdWdt = open(DEVICE_WDT, O_RDWR);
	if(B_PRINTF)printf("Device -- Watchdog  is %d.\n", gfdWdt);
	if(gfdWdt <=0) return -1;

	return 0;
}

//---------------- interface functions -------------------

/*
	初始化扩展设备
*/
int InitExtDevices()
{
	//打开设备
	InitDevice_Key();
	InitDevice_Led();

//	InitDevice_Pump();
//	InitDevice_Wdt();
		
	return 0;
}

/*
	led报警灯操作函数
*/

//红灯闪烁
int RedLed_On(unsigned char freq)
{
	if(gfdLed<0) return -1;
	
	ioctl(gfdLed,WCE2400_CPLD_ENABLE_RED_LED,freq);
	
	return 0;
}
//红灯灭
int RedLed_Off()
{
	if(gfdLed <0) return -1;

	ioctl(gfdLed,WCE2400_CPLD_DISABLE_RED_LED,NULL);
	
	return 0;
}
//黄灯亮
int YellowLed_On(unsigned char freq)
{
	if(gfdLed <0) return -1;
	
	ioctl(gfdLed,WCE2400_CPLD_ENABLE_YELLOW_LED,freq);
	
	return 0;
}
//黄灯灭
int YellowLed_Off()
{
	if(gfdLed <=0) return -1;
	
	ioctl(gfdLed,WCE2400_CPLD_DISABLE_YELLOW_LED,NULL);
	
	return 0;
}
//绿灯亮
int GreenLed_On(unsigned char freq)
{
	if(gfdLed <0) return -1;
	
	ioctl(gfdLed,WCE2400_CPLD_ENABLE_GREEN_LED,freq);
	
	return 0;
}
//绿灯灭
int GreenLed_Off()
{
	if(gfdLed ==0) return -1;
	
	ioctl(gfdLed,WCE2400_CPLD_DISABLE_GREEN_LED,NULL);
	
	return 0;
}




//快阀开
int AdultValveOn()
{
	if(gfdInterface ==0) return -1;
	
	ioctl(gfdInterface, PUMP_IOC_QUICK_ON);
	
	return 0;
}
//快阀关
int AdultValveOff()
{
	if(gfdInterface ==0) return -1;
	
	ioctl(gfdInterface, PUMP_IOC_QUICK_OFF);
	
	return 0;
}
//中阀开
int ChildValveOn()
{
	if(gfdInterface ==0) return -1;
	
	ioctl(gfdInterface, PUMP_IOC_MIDDLE_ON);
	
	return 0;
}
//中阀关
int ChildValveOff()
{
	if(gfdInterface ==0) return -1;
	
	ioctl(gfdInterface, PUMP_IOC_MIDDLE_OFF);
	
	return 0;
}

/*获得机器工作状态：1－正常工作，0－16秒后将关闭电源*/
int GetWorkStatus()
{
	int res;
	return	POWER_WORKING;

	if(gfdInterface ==0) return -1;
	
	res = ioctl(gfdInterface, IOC_GETPOWERSTATUS);
	
	if(res ==0){
		if(B_PRINTF)printf("%s:%d Power Off: %d.\n", __FILE__, __LINE__, res);
		//关闭看门狗
		WDT_Stop();
		if(B_PRINTF)printf("%s:%d Stop Watchdog.\n", __FILE__, __LINE__);
	}
 	
	if(res ==1) return  POWER_WORKING;
	else return POWER_SHUTDOWN;
	
	return res;
}

/*获得机器电源类型：1－网电源，0－电池*/
int GetPowerType()
{
	int res=0;
	return POWER_AC;
	if(gfdInterface ==0) return -1;
	
	res = ioctl(gfdInterface, IOC_GETPOWERTYPE);
	
// 	printf("%s:%d Power Type: %d.\n", __FILE__, __LINE__, res);
	
	if(res ==1) return POWER_AC;
	else return POWER_DC;
	
	return res;
}

/*
	获得电池电量	
*/
int GetBatteryStatus()
{
	int res =0;
	return 2;
	res = ioctl(gfdInterface, IOC_GETBATTERYSTATUS);
	
// 	printf("%s:%d Battery: %d.\n", __FILE__, __LINE__, res);
	
	return res;
}


/*
	调节占空比
*/
int PWM_ChangeRate(int rate)
{
	int res;
	
	if(gfdPump == 0) return -1;
	
	res = ioctl(gfdPump, PWMDEV_IOC_CHANGEDUTY, rate);
	
// 	printf("Change PWM rate to %d.\n", rate);
	return res;
}

/*
	启动PWM定时器
*/
int PWM_StartTimer()
{
	int res;
	
	if(gfdPump == 0) return -1;
	
	res = ioctl(gfdPump, PWMDEV_IOC_STARTTIMER);
	
	return res;
}

/*
	关闭定时器
*/
int PWM_CloseTimer()
{
	int res;
	
	if(gfdPump == 0) return -1;
	
	res = ioctl(gfdPump, PWMDEV_IOC_CLOSETIMER);
	
	return res;
}

//启动看门狗
int WDT_Start()
{
	int res;
	
	if(gfdWdt == 0) return -1;
	
	//暂时屏蔽看门狗
	
     //   	write(gfdWdt, "a", 1);
	
//	res = ioctl(gfdWdt, WD_IOC_START);
	
	return res;
}

//停止看门狗
int WDT_Stop()
{
	int res;
	
	if(gfdWdt == 0) return -1;
	
//	res = ioctl(gfdWdt, WD_IOC_STOP);
	 write(gfdWdt, "V", 1);
        close(gfdWdt);
        gfdWdt = 0;
	return res;
}


//喂狗
int WDT_Feed()
{
	int res;
	
	if(gfdWdt == 0) return -1;

	write(gfdWdt, "a", 1);
//	res = ioctl(gfdWdt, WD_IOC_FEED);

	return res;
}

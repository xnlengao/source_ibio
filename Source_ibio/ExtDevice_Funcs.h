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

#ifndef __INCLUDEFILES_H__
#define __INCLUDEFILES_H__
      	
/*
	初始化扩展设备
*/
int InitExtDevices();
/*
	注意,此处灯光的On and Off 的ioctl正好搞反了,呵呵
*/
//红灯亮
int RedLightOn();
//红灯灭
int RedLightOff();
//黄灯亮
int YellowLightOn();
//黄灯灭
int YellowLightOff();
//快阀开
int AdultValveOn();
//快阀关
int AdultValveOff();
//中阀开
int ChildValveOn();
//中阀关
int ChildValveOff();
/*获得机器工作状态：1－正常工作，0－16秒后将关闭电源*/
int GetWorkStatus();
/*获得机器电源类型：1－网电源，0－电池*/
int GetPowerType();
/*
	获得电池电量	
*/
int GetBatteryStatus();
/*
	调节占空比
*/
int PWM_ChangeRate(int rate);
/*
	启动PWM定时器
*/
int PWM_StartTimer();
/*
	关闭定时器
*/
int PWM_CloseTimer();
/*
	键盘操作	
*/
int Key_Up();
int Key_Down();
int Key_Left();
int Key_Right();
int Key_Tab();
int Key_ShiftTab();
int Key_Space();
int Key_Enter();
int Key_Esc();
      	
/*
	看门狗
*/
int WDT_Start();
int WDT_Feed();
      	
#endif	//__INCLUDEFILES_H__

/*************************************************************************** 
 *	Module Name:	main.c	
 *
 *	Abstract:	主程序
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *	            2007-05-22 19:41:59
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"

/*
	入口函数
*/
int MiniGUIMain(int argc, const char *argv[])
{
	
	//初始化写文件的互斥量
	InitWriteFileMutex();
	
	//初始化系统
	InitSystem();

	//初始化扩展设备
	InitExtDevices();
	
	//创建发声线程
	CreateSoundProc();
	
	//--- 获得电源状态 ---
	GetPowerStatus();
	
	//初始化数据管理
	InitDataManage();
	
 	//初始化系统串口
 	InitSystemSerials();

	//启动自动趋势存储
  	StartAutoTrendSave();
	
	//启动打印机
//	Printer_Open();

	//声光测试
 	Test_LightAndSound();
	
	//-- 启动图形界面 --
 	GUI_Main(argc, argv);
	
	printf("%s:%d EXIT main\n", __FILE__, __LINE__);
	
	
	return 0;	
}


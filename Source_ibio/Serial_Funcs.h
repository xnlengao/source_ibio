/*************************************************************************** 
 *	Module Name:	Serial_Funcs
 *
 *	Abstract:	串口操作函数
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *				2007-05-28 09:14:06
 ***************************************************************************/

#ifndef __SERIAL_FUNCS_H__
#define __SERIAL_FUNCS_H__

//根据波特率计算接收超时
#define TIMEOUT_SEC(buflen,baud) 	(buflen*20/baud+2)
#define TIMEOUT_USEC		0

/*******************************************
*	函数名:		port_open()
*	输入:		
*			devname:串口设备		
*			baud: 波特率			
*			size:数据位
*			check:	校验			0--No, 1--Odd, 2--even
*			stop: 停止位			1-1, 2-2
*			control:数据流控		0-- no control, 1--hardware, 2--software
*	输出:
*			-1: Error
*
*	功能描述：	打开并初始化指定串口 
*	作者:		
*	日期:		
********************************************/
int port_open(const unsigned char *devname, const char *baud, int size, int check, int stop, int control);
/*******************************************
*	函数名:		port_close()
*	输入:		
*			devname:串口设备		
*
*	输出:
*			-1: Error
*
*	功能描述：	关闭指定串口 
*	作者:		
*	日期:		
********************************************/
int port_close(int fdcom);

/*******************************************
*	函数名:		port_send()
*	输入:		
*			fdcom: 串口句柄						
*			datas: 准备发送的数据				
*			datalen: 发送的数据长度				
*
*	输出:
*			-1: Error
*
*	功能描述：	串口0发送数据 
*	作者:		
*	日期:		
********************************************/
int port_send(int fdcom, const unsigned char *datas, int datalen);
/*******************************************
*	函数名:		port_recv()
*	输入:		
*		fdcom: 串口句柄						
*		datas: 数据缓存							
*		datalen: 准备接收数据的长度
*		baudrate: 波特率				
*
*	输出:
*			-1: Error
*
*	功能描述：	串口0接收数据 
*	作者:		
*	日期:		
********************************************/
int port_recv(int fdcom, char *data, int datalen, int baudrate);

/*
	初始化系统串口
*/
int InitSystemSerials();

#endif	//__SERIAL_FUNCS_H__

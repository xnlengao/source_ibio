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
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Serial_Funcs.h"

//定义波特率
int baud_tbl[] = {B4800, B9600, B19200, B38400, B57600, B115200};
char *baud_str[] = {"4800", "9600", "19200", "38400", "57600", "115200"};


//串口写操作的同步信号量
sem_t 	semSend;
//ttyS1:rs232-j15  ttyS2:ttl-j20 tts0:ttl-j19 tts1:ttl-j16
unsigned char *gSerialsName[] = {"/dev/ttyS1", "/dev/ttyS2", 
	"/dev/tts0", "/dev/tts1", "/dev/tts2"};
/*
	初始化串口写操作的同步信号量
*/
int InitSemForPortSend()
{
	int res;
		
	res = sem_init(&semSend, 0, 1);

	return res;
}

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
int port_open(const unsigned char *devname, const char *baud, int size, int check, int stop, int control)
{
	struct termios newterm;
	int speed, i;
	int fd ;

	speed = B115200;
	for(i=0; i<6; i++){
		if(strcmp(baud, baud_str[i])==0){
			speed = baud_tbl[i];
		}
	}
	
	fd = open(devname, O_RDWR | O_NONBLOCK | O_NOCTTY);
	printf("%s:%d open %s !\n", __FILE__, __LINE__, devname);
	if(fd<0){
		DEBUGINFO();
		printf("%s:%d open %s failure!\n", __FILE__, __LINE__, devname);
		return -1;
	}
	
	tcgetattr(fd, &newterm);
	cfmakeraw(&newterm);

	cfsetispeed(&newterm, speed);
	cfsetospeed(&newterm, speed);
	
	switch(control){
		case 1:{//hardware control
			newterm.c_cflag |= CRTSCTS;
		}break;
		case 2:{//software control
			newterm.c_cflag |= IXON | IXOFF |IXANY;
		}break;
		default:{//no flow control	
			newterm.c_cflag &= ~CRTSCTS;
		}break;
	}

	newterm.c_cflag &= ~CSIZE;
	switch(size){
		case 5:{
			newterm.c_cflag |= CS5;
		}break;
		case 6:{
			newterm.c_cflag |= CS6;
		}break;
		case 7:{
			newterm.c_cflag |= CS7;
		}break;
		default:{
			newterm.c_cflag |= CS8;
		}break;
	}

	switch(check){                                
		case 1:{//odd parity
			newterm.c_cflag |= PARENB;	
			newterm.c_cflag &= ~PARODD;
		}break;
		case 2:{//even parity
			newterm.c_cflag |= PARENB;
			newterm.c_cflag |= PARODD;
		}break;
		default:{//no parity
			newterm.c_cflag &= ~PARENB; 
		}break; 	
	}
	
	switch(stop){
		case 2:{//2 stop bits
			newterm.c_cflag |= CSTOPB;	
		}break;
		default:{// 1 stop bit
			newterm.c_cflag &= ~CSTOPB;
		}break;
	}
	tcsetattr(fd, TCSANOW, &newterm);

	return fd;
}

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
int port_close(int fdcom)
{
	if(fdcom>0) {
		close(fdcom);
		return 0;
	}
	else{
		return -1;
	}
}

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
*	功能描述：	串口发送数据 
*	作者:		
*	日期:		
********************************************/
int port_send(int fdcom, const unsigned char *datas, int datalen)
{
	
	int len = 0;
	int i;
	
	sem_wait(&semSend);
// 	DEBUGINFO();
// 	printf("%d port_send: ", fdcom);
	for(i=0; i< datalen; i++){
		len = write(fdcom, datas+i, 1);	//实际写入的长度
// 		printf("%.2X ", *(data+i));
		if(len != 1){
			tcflush(fdcom, TCOFLUSH);
			sem_post(&semSend);
			return -1;
		}
		usleep(5000);
	}
// 	printf(" End.\n");
	sem_post(&semSend);
			
	return datalen;
}

/*******************************************
*	函数名:		port_recv0()
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
int port_recv(int fdcom, char *data, int datalen, int baudrate)
{
	int readlen, fs_sel;
	
	fd_set	fs_read;
	struct timeval tv_timeout;

	FD_ZERO(&fs_read);
	FD_SET(fdcom, &fs_read);
	tv_timeout.tv_sec = TIMEOUT_SEC(datalen, baudrate);
	tv_timeout.tv_usec = TIMEOUT_USEC;
		
	fs_sel = select(fdcom+1, &fs_read, NULL, NULL, &tv_timeout);
	if(fs_sel>0){
		readlen = read(fdcom, data, datalen);
		return(readlen);
	}
	else{
		return(-1);
	}
	
	return (readlen);
}

/*
	初始化系统串口
*/
int InitSystemSerials()
{
	//初始化串口写操作的同步信号量
	InitSemForPortSend();
	
	return 0;
}



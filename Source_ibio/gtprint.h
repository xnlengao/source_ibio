#ifndef __GUOTENG_PRINT__

#define __GUOTENG_PRINT__


/**
macro define section
**/

//define function return error code
#define	GT_RECORDER_INVALID			0x01
#define	GT_RECORDER_OPENED			0x02
#define	GT_RECORDER_NO_OPEN			0x03
#define	GT_RECORDER_NO_THREAD			0x04



//print recorder return status macro
//for callback usage
#define GT_RECORDER_STATUS_UNKNOWN		0x00		// init status
#define GT_RECORDER_STATUS_NO_FIND		0x01		// not find printer status
#define GT_RECORDER_STATUS_IDLE			0x02		// idle status
#define GT_RECORDER_STATUS_PRINTING		0x03		// printing status
#define GT_RECORDER_STATUS_BUSY			0x04		// busy status
#define GT_RECORDER_STATUS_NO_PAPER		0x05		// no paper status
//only for internal debug usage
#define GT_RECORDER_STATUS_REQUEST_REPEAT	0x10		// for internal use status




//define print recorder text print macro
#define MAX_ROW_COUNT				14  		//最多打印字串的行数
#define MAX_TEXT_LENGTH				200		//字串的长度

//define wave print macro
#define MAX_REALWAVE_COUNT			3		//最多同时打印3条实时波形
#define MAX_REALWAVE_LENGTH			400		//一次传输的实时波形的最大长度

//define screen print macro
#define MAX_COPYWAVE_COUNT			3		//最多拷屏打印的波形数量
#define MAX_COPYWAVE_LENGTH			7000		//拷屏打印的波形长度, 250Hz*8s

//define print speed macro
#define GT_PRINT_SPEED_UNKNOWN			0x00		//??
#define GT_PRINT_SPEED_12			0x01		//12.5 mm/s
#define GT_PRINT_SPEED_25			0x02		//25.0 mm/s
#define GT_PRINT_SPEED_50			0x03		//50.0 mm/s

//define print frequence
#define	GT_FREQUENCE_100			0x01		//100 Hz
#define	GT_FREQUENCE_200			0x02		//200 Hz
#define	GT_FREQUENCE_300			0x03		//300 Hz
#define	GT_FREQUENCE_400			0x04		//400 Hz
#define	GT_FREQUENCE_500			0x05		//500 Hz


//define print grid
#define	GT_GRID_FALSE				0x00		// no grid
#define	GT_GRID_TRUE				0x01		// has grid


//define print max channel
#define	GT_PRINT_MAX_CHANNEL			0x03		// max channel


//define print max packet count if printer is slower
#define	GT_PRINT_MAX_PACKET_COUNT		1000		// max print packet count











#ifdef __cplusplus
extern	"C" {
#endif /* __cplusplus */



/**
user data type define section
**/

//guoteng print recorder type define
enum GTPRINTER {
    GT_NULL_PRINTER = -1,
    GT_S0_PRINTER,
    GT_S1_PRINTER,
    GT_S2_PRINTER,
    GT_S3_PRINTER,
    GT_S4_PRINTER,
    GT_S5_PRINTER,
    GT_S6_PRINTER,
    GT_S7_PRINTER,
    GT_LPT_PRINTER
};

//guoteng print recorder text print struct
typedef struct {
    char 		strText[MAX_ROW_COUNT][MAX_TEXT_LENGTH];	//欲打印的字符串
    unsigned int 	iRowCount;					//指定打印的行数
}REC_STRING, *PREC_STRING;


//guoteng wave struct
typedef struct {
	unsigned char 	bSpeed; 					//打印速度	12.5, 25. 50mm/s
	unsigned char 	bFreq;   					//采样率	100, 200, 300,... ... 1000Hz 	 
	unsigned char 	bGrid;   					//是否打印网格
		 int 	baseline[GT_PRINT_MAX_CHANNEL];			//波形的基线位置
}REC_WAVEPARAM, *PREC_WAVEPARAM;					//打印波形的参数
		
typedef struct {
		 int 	iWave[MAX_REALWAVE_COUNT][MAX_REALWAVE_LENGTH];
	unsigned int 	iCount;
	unsigned int 	iLength;	
}REC_REALWAVE, *PREC_REALWAVE;


//guotent screen print struct
typedef struct {
		 int 	iWave[MAX_COPYWAVE_COUNT][MAX_COPYWAVE_LENGTH];
	unsigned int 	iCount;
	unsigned int 	iLength;	
}REC_COPYWAVE, *PREC_COPYWAVE;


typedef struct {
    	char 			strText[MAX_ROW_COUNT][MAX_TEXT_LENGTH];
    	unsigned int 		iRowCount;
    	unsigned int 		iMaxLen;
}REC_STRINGT, *PREC_STRINGT;


/**
print recorder interface define section
**/


/**
description:
    open print recorder function

param:
    input: enum GTPRINTER

return value:
    0     - OK
    < 0   - Error
**/
int  RecorderOpen( enum GTPRINTER printer );


/**
description:
    close print recorder function

param:
    void

return value:
    0   - OK
    -1  - Error
**/
int  RecorderClose( void );


/**
description:
    dectection print recorder status function

param:
    input/output: int *status

return value:
    0     - OK
    < 0   - Error
**/
int  RecorderGetStatus( int *status );



/**
description:
    get print recorder paper width function

param:
    input/output: void

return value:
    PAPER WIDTH
**/
int  RecorderGetPaperWidth( void );



/**
description:
    set print speed function

param:
    input: int speed
    the value must be in GT_PRINT_SPEED_12, GT_PRINT_SPEED_25, GT_PRINT_SPEED_50

return value:
    0     - OK
    < 0   - Error
**/
int  SetRecorderPrintSpeed( int speed );



/**
description:
    print text function

param:
    input: PREC_STRING pdata
    user manage memory

return value:
    0     - OK
    < 0   - Error
**/
int  RecorderPrintString( const PREC_STRING pdata );


/**
description:
    move paper function

param:
    input: int column

return value:
    0     - OK
    < 0   - Error
**/
int  RecorderMovePaper( const unsigned int column );


/**
description:
    start print wave function

param:
    input: PREC_REALWAVE  pdata				//打印的波形
    input: PREC_WAVEPARAM pparam			//打印的的参数
    user manage memory

return value:
    0     - OK
    < 0   - Error
**/
int  RecorderStartPrintRealWave( const PREC_REALWAVE pdata, const PREC_WAVEPARAM pparam );		


/**
description:
    print wave function

param:
    input: PREC_REALWAVE  pdata				//打印的波形
    user manage memory

return value:
    0     - OK
    < 0   - Error
**/

int  RecorderSetPrintRealWave( const PREC_REALWAVE pdata );


/**
description:
    stop wave print function

param:
     void

return value:
    0     - OK
    < 0   - Error
**/
int  RecorderStopPrintRealWave( void );


/**
description:
    screen print function

param:
    input: PREC_COPYWAVE  pdata				//打印的波形
    input: PREC_WAVEPARAM pparam			//打印的的参数
return value:
    0     - OK
    < 0   - Error
**/
int  RecorderStartPrintCopyWave( const PREC_COPYWAVE pdata, const PREC_WAVEPARAM pparam );


/**
description:
    screen print function

param:
    input: PREC_COPYWAVE  pdata				//打印的波形
return value:
    0     - OK
    < 0   - Error
**/
int  RecorderSetPrintCopyWave( const PREC_COPYWAVE pdata );


/**
description:
    stop screen print function

param:
     void

return value:
    0     - OK
    < 0   - Error
**/
int  RecorderStopPrintCopyWave( void );



/**
description:
    stop  print function
    allow to print buffer data

param:
     void

return value:
    0     - OK
    < 0   - Error
**/
int  RecorderStopPrint( void );



/**
description:
    get printer status function

param:
     void

return value:
    status value    - OK
    < 0  	 - Error
**/
int  GetPrinterStatus( void );





#ifdef __cplusplus
}
#endif /* __cplusplus */
	


#endif /* __GUOTENG_PRINT__ */



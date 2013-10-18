/*************************************************************************** 
 *	Module Name:	General_Funcs
 *
 *	Abstract:	一些通用函数
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *	
 ***************************************************************************/
#ifndef __GENERAL_FUNCS_H__
#define __GENERAL_FUNCS_H__

//-- 位掩码定义 --
#define BIT0		0X0001
#define BIT1		0X0002
#define BIT2		0x0004
#define BIT3		0x0008
#define BIT4		0x0010
#define BIT5		0x0020
#define BIT6		0x0040
#define BIT7		0x0080
#define BIT8		0x0100
#define BIT9		0x0200
#define BIT10		0x0400
#define BIT11		0x0800
#define BIT12		0x1000
#define BIT13		0x2000
#define BIT14		0x4000
#define BIT15		0x8000

//取低8位
#define LOBYTE(w)	((BYTE)(w))
//取高8位
#define HIBYTE(w)	((BYTE)(((WORD)(w) >> 8) & 0xFF))
//Make A Word
#define MAKEWORD(low, high)		((WORD)(((BYTE)(low)) | (((WORD)((BYTE)(high))) << 8)))

//确定 X 的范围
#define	IS_RANGE(x, min, max)	((x)>=(min) && (x)<=(max))
//比较 X,Y
#define BMAX(x,y) 	(((x) > (y)) ? (x) : (y))
#define BMIN(x,y) 	(((x) < (y)) ? (x) :(y))

//压力单位转换系数
#define mmHgTokPa		7.5	// mmHg/系数
#define mmHgTocmH2O	1.33	// mmHg*系数
#define kPaTocmH2O		10.0	// kPa*系数

//mmHg -> 百分比（气体浓度）
#define mmHgToPer	7.55     	//mmHg/系数

/*
	体温转换
	C->F   :   C*1.8+32
	F->C  :   (F-32)/1.8
*/

//计算时间函数CalcTime()的变量
#define TIME_AFTER		0	//计算之后的时间
#define TIME_BEFORE	1	//计算之前的时间

//宽线段的类型(线尾)
#define WIDELINE_DOT	0	//点线段
#define WIDELINE_RECT	1	//矩形线段
#define WIDELINE_CYCLE	2	//圆形线段
//宽线段的线型
#define REALWIDELINE	0	//实线	

#endif	//__GENERAL_FUNCS_H__

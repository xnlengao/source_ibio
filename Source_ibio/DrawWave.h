/*************************************************************************** 
 *	Module Name:	DrawWave
 *
 *	Abstract:	绘图主程序
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2007-06-18 09:23:48
 ***************************************************************************/
#ifndef __DRAWWAVE_H__
#define __DRAWWAVE_H__


//定义波形限值
#define WAVEMAX_ECG	4095
#define WAVEMAX_SPO2	255	
#define WAVEMAX_RESP	255	
#define WAVEMAX_CO2	99
#define WAVEMAX_IBP		400

//基线
//#define BASELINE_ECG	127
//#define BASELINE_SPO2	50
//#define BASELINE_RESP	127
//#define BASELINE_CO2	(WAVEMAX_CO2/2)

//波形面板背景颜色
#define	PANEL_BKMODE		SS_BLACKRECT
#define	PANEL_BKCOLOR	COLOR_black		

//波形面板字符颜色
#define	PANEL_TEXTCOLOR	COLOR_lightgray 

//面板定义


//面板之间的间隔
#define PANEL_INTER		3	

//常规视图, BigChar, MultiECG, 绘图显示区域坐标
#define VW_NORMAL_LEFT	5
#define VW_NORMAL_TOP		35
#define VW_NORMAL_RIGHT	545
#define VW_NORMAL_BOTTOM	560	//565
//有扩展参数(IBP)的情况下
#define VW_NORMAL_BOTTOM_EXT	440	

//短趋势视图, 绘图显示区域坐标
#define VW_SHORTTREND_LEFT	145
#define VW_SHORTTREND_TOP		35
#define VW_SHORTTREND_RIGHT	540
#define VW_SHORTTREND_BOTTOM	565
//有扩展参数(IBP)的情况下
#define VW_SHORTTREND_BOTTOM_EXT	440	

//Big Chaacter 视图
#define VW_BIGCHAR_LEFT		5
#define VW_BIGCHAR_TOP		35
#define VW_BIGCHAR_RIGHT		545
#define VW_BIGCHAR_BOTTOM	450

//NIBP List 绘图显示区域坐标
#define VW_NIBPLIST_LEFT			5
#define VW_NIBPLIST_TOP			35
#define VW_NIBPLIST_RIGHT		540
#define VW_NIBPLIST_BOTTOM		370
#define VW_NIBPLIST_BOTTOM_EXT	250


//OxyCRG 绘图显示区域坐标
#define VW_OXYCRG_LEFT	5
#define VW_OXYCRG_TOP		35
#define VW_OXYCRG_RIGHT	540
#define VW_OXYCRG_BOTTOM	275		//450
#define VW_OXYCRG_BOTTOM_EXT	200


//扫描棒宽度（扫描速度不同，宽度也不同）
#define FILLWIDTH_6		10	
#define FILLWIDTH_12		10	
#define FILLWIDTH_25		5
#define FILLWIDTH_50		5
#define FILLWIDTH		8


//波形面板区域
typedef struct {
	int left;
	int top;
	int right;
	int bottom;
	int height;
	int width;
}WP_RECT, *PWP_RECT;

//波形面板,系统使用
typedef struct {
	BYTE bID;			//波形类型标识
	HWND hWnd;			//面板句柄
	HDC hdc;				//绘图环境
	WP_RECT wpRc;		//面板绘图区域
	BOOL bReset; 		//复位绘图(重新开始绘图)
	BOOL bCascade;		//是否级联
	BYTE bWaveCount;	//绘制的波形数量
	float fXAxis;			//X轴缩放系数，根据扫描速度来定	
	float fYAxis;			//Y轴缩放系数, 根据波形增益来定	
	int iXStep;			//X轴的步进值，X上的坐标系固定缩放为最慢的速度，通过改变步进值来改变扫描速度
	int iYMax;			//Y轴最大值
	int iYMin;			//Y轴最小值
	//波形采样点对应屏幕像素的比例, 根据波形增益来定(ECG定标时使用)
	float fSampleToPixel;	
	//波形上下移动的幅度，根据波形数量来定(ECG定标时使用)
	int iMove;
}WAVE_PANEL, *PWAVE_PANEL;

#endif //__DRAWWAVE_H__

/*************************************************************************** 
 *	Module Name:	AlarmManger
 *
 *	Abstract:		报警信息管理
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2007-06-27 10:12:29
 ***************************************************************************/
#ifndef __ALARMMANAGE_H__
#define __ALARMMANAGE_H__


//生理报警信息ID(未加说明的,报警级别用户可调)
#define AP_NONE				0
//ECG
#define AP_HR_HIGH				AP_NONE+1		
#define AP_HR_LOW				AP_NONE+2		
#define AP_ECG_WEAKSIGNAL		AP_NONE+3		//信号太弱,高级报警
//RESP
#define AP_RR_HIGH				AP_NONE+4
#define AP_RR_LOW				AP_NONE+5
#define AP_APNEA				AP_NONE+6		//窒息,高级报警,用户可以关闭
//SPO2
#define AP_SPO2_HIGH			AP_NONE+7
#define AP_SPO2_LOW			AP_NONE+8
#define AP_SPO2_LOWEST		AP_NONE+9
//Pulse
#define AP_PR_HIGH				AP_NONE+10
#define AP_PR_LOW				AP_NONE+11
//TEMP
#define AP_T1_HIGH				AP_NONE+12
#define AP_T1_LOW				AP_NONE+13
#define AP_T2_HIGH				AP_NONE+14
#define AP_T2_LOW				AP_NONE+15
#define AP_TD_HIGH				AP_NONE+16
//NIBP
#define AP_NSYS_HIGH			AP_NONE+17
#define AP_NSYS_LOW			AP_NONE+18
#define AP_NDIA_HIGH			AP_NONE+19
#define AP_NDIA_LOW			AP_NONE+20
#define AP_NMEAN_HIGH			AP_NONE+21
#define AP_NMEAN_LOW			AP_NONE+22
//CO2
#define AP_ETCO2_HIGH			AP_NONE+23
#define AP_ETCO2_LOW			AP_NONE+24
#define AP_FICO2_HIGH			AP_NONE+25
//IBP1
#define AP_ISYS1_HIGH			AP_NONE+26
#define AP_ISYS1_LOW			AP_NONE+27
#define AP_IDIA1_HIGH			AP_NONE+28
#define AP_IDIA1_LOW			AP_NONE+29
#define AP_IMEAN1_HIGH			AP_NONE+30
#define AP_IMEAN1_LOW			AP_NONE+31
//IBP2
#define AP_ISYS2_HIGH			AP_NONE+32
#define AP_ISYS2_LOW			AP_NONE+33
#define AP_IDIA2_HIGH			AP_NONE+34
#define AP_IDIA2_LOW			AP_NONE+35
#define AP_IMEAN2_HIGH			AP_NONE+36
#define AP_IMEAN2_LOW			AP_NONE+37

#define AP_ST1_HIGH				AP_NONE+38
#define AP_ST1_LOW				AP_NONE+39
#define AP_ST2_HIGH				AP_NONE+40
#define AP_ST2_LOW				AP_NONE+41

#define AP_ARR					AP_NONE+42




//生理参数报警数量
//#define ALM_PHY_INFOMAX		38
#define ALM_PHY_INFOMAX		43
//单条生理参数报警信息长度
#define ALM_PHY_INFOLEN		50

//技术报警信息ID
#define AT_NONE			0
//ECG Info
#define AT_ECG_COMERR				AT_NONE+1		//ECG 通讯故障－－高
#define AT_ECG_COMSTOP			AT_NONE+2		//ECG 通讯停止－－高
#define AT_ECG_MODULEERR			AT_NONE+3		//ECG 模块故障－－高
#define AT_ECGOFF_V					AT_NONE+4		//V导脱落－－低
#define AT_ECGOFF_RA				AT_NONE+5		//RA脱落－－低
#define AT_ECGOFF_LL				AT_NONE+6		//LL脱落－－低
#define AT_ECGOFF_LA				AT_NONE+7		//LA脱落－－低
#define AT_ECG_LIMITERR			AT_NONE+8 		//报警限错－－高
#define AT_ECG_INTERFERE			AT_NONE+9 		//干扰太大－－低
#define AT_ECG_OVERRANGE			AT_NONE+10 		//测量值超出测量范围－－高

#define AT_ECG_OVERLOAD1			AT_NONE+11 		//测量值超出测量范围－－高new
#define AT_ECG_OVERLOAD2			AT_NONE+12 		//测量值超出测量范围－－高new
#define AT_ECG_OVERLOAD3			AT_NONE+13 		//测量值超出测量范围－－高new

//RESP Info
#define AT_RESP_LIMITERR			AT_NONE+14 		//报警限错－－高
#define AT_RESP_OVERRANGE			AT_NONE+15 		//测量值超出测量范围－－高
#define AT_CVA						AP_NONE+16		//心动干扰--------中	new

//SpO2 Info
#define AT_SPO2_COMERR			AT_NONE+17		//通讯故障－－高
#define AT_SPO2_COMSTOP			AT_NONE+18		//通讯停止－－高
#define AT_SPO2_MODULEINITERR		AT_NONE+19		//模块初始化错误－－高
#define AT_SPO2_MODULEERR			AT_NONE+20		//测量模块故障－－高
#define AT_SPO2_LIMITERR			AT_NONE+21 		//报警限错－－高
#define AT_SPO2_OVERRANGE			AT_NONE+22 		//测量值超出测量范围－－高
#define AT_SPO2_NOSEMSOR			AT_NONE+23 		//未接探头
#define AT_SPO2_SENSORERR			AT_NONE+24 		//探头故障或损坏
#define AT_SPO2_UNKNOWN			AT_NONE+25 		//探头无法识别
#define AT_SPO2_INCOMPATIBLE		AT_NONE+26 		//探头不兼容
#define AT_SPO2_SENSOROFF			AT_NONE+27 		//探头脱落
//合并SpO2测量错误
#define AT_SPO2_ERROR				AT_NONE+28		//测量错误－－级别由错误类型来定
//Pulse Info
#define AT_PULSE_LIMITERR			AT_NONE+29 		//报警限错－－高
#define AT_PULSE_OVERRANGE		AT_NONE+30 		//测量值超出测量范围－－高
//NIBP Info
#define AT_NIBP_COMERR				AT_NONE+31		//通讯故障－－高
#define AT_NIBP_COMSTOP			AT_NONE+32		//通讯停止－－高
#define AT_NIBP_MODULEERR			AT_NONE+33		//模块故障－－高
#define AT_NIBP_SYS_LIMITERR		AT_NONE+34	 	//报警限错－－高
#define AT_NIBP_DIA_LIMITERR		AT_NONE+35	 	//报警限错－－高
#define AT_NIBP_MEAN_LIMITERR		AT_NONE+36 		//报警限错－－高
#define AT_NIBP_OVERRANGE			AT_NONE+37 		//测量值超出测量范围－－高
//合并NIBP测量错误
#define AT_NIBP_ERROR				AT_NONE+38		//测量错误－－级别由错误类型来定
//TEMP Info
#define AT_TEMP_COMERR			AT_NONE+39		//通讯故障－－高
#define AT_TEMP_COMSTOP			AT_NONE+40		//通讯停止－－高
#define AT_TEMP_MODULEERR			AT_NONE+41		//模块故障－－高
#define AT_TEMP_T1OVERRANGE		AT_NONE+42		//TEMP1测量超限－－高
#define AT_TEMP_T2OVERRANGE		AT_NONE+43		//TEMP2测量超限－－高
#define AT_TEMP_T1SENSOROFF		AT_NONE+44		//TEMP1探头脱落－－低
#define AT_TEMP_T2SENSOROFF		AT_NONE+45		//TEMP2探头脱落－－低
#define AT_TEMP_T1LIMITERRR		AT_NONE+46		//报警限错－－高
#define AT_TEMP_T2LIMITERRR		AT_NONE+47		//报警限错－－高
#define AT_TEMP_TDLIMITERRR		AT_NONE+48		//报警限错－－高
//CO2
#define AT_CO2_COMERR				AT_NONE+49		//通讯故障－－高
#define AT_CO2_COMSTOP			AT_NONE+50		//通讯停止－－高
#define AT_CO2_MODULEERR			AT_NONE+51		//模块故障－－高
#define AT_CO2_OVERRANGE			AT_NONE+52		//模块故障－－高
#define AT_CO2_ETCO2LIMITERR		AT_NONE+53		//报警限错－－高
#define AT_CO2_FICO2LIMITERR		AT_NONE+54		//报警限错－－高
#define AT_CO2_AWRRLIMITERR		AT_NONE+55		//报警限错－－高
//IBP1
#define AT_IBP1_COMERR				AT_NONE+56		//通讯故障－－高
#define AT_IBP1_COMSTOP			AT_NONE+57		//通讯停止－－高
#define AT_IBP1_MODULEERR			AT_NONE+58		//模块故障－－高
#define AT_IBP1_SYS_LIMITERR		AT_NONE+59 		//报警限错－－高
#define AT_IBP1_DIA_LIMITERR			AT_NONE+60 		//报警限错－－高
#define AT_IBP1_MEAN_LIMITERR		AT_NONE+61 		//报警限错－－高
#define AT_IBP1_OVERRANGE			AT_NONE+62 		//测量值超出测量范围－－高
#define AT_IBP1_SENSOROFF			AT_NONE+63		//探头脱落－－低
//IBP2
#define AT_IBP2_COMERR				AT_NONE+64		//通讯故障－－高
#define AT_IBP2_COMSTOP			AT_NONE+65		//通讯停止－－高
#define AT_IBP2_MODULEERR			AT_NONE+66		//模块故障－－高
#define AT_IBP2_SYS_LIMITERR		AT_NONE+67 		//报警限错－－高
#define AT_IBP2_DIA_LIMITERR		AT_NONE+68 		//报警限错－－高
#define AT_IBP2_MEAN_LIMITERR		AT_NONE+69 		//报警限错－－高
#define AT_IBP2_OVERRANGE			AT_NONE+70 		//测量值超出测量范围－－高
#define AT_IBP2_SENSOROFF			AT_NONE+71		//探头脱落－－低
//SYSTEM Info
#define AT_BATTERY_LOW			AT_NONE+72		//电池电量低－－高
													
#define AT_BOARD33V_LOW			AT_NONE+73		//系统状态3.3V电压太低－－高new
#define AT_BOARD33V_HIGH			AT_NONE+74		//系统状态3.3V电压太高－－高new
#define AT_BOARD5V_LOW				AT_NONE+75		//系统状态5V电压太低－－高new
#define AT_BOARD5V_HIGH			AT_NONE+76		//系统状态5V电压太高－－高new
#define AT_SELFTEST_WATCHDOG		AT_NONE+77		//系统自检watchdog错误－－高new
#define AT_SELFTEST_AD				AT_NONE+78		//系统自检A/D错误－－高new
#define AT_SELFTEST_RAM			AT_NONE+79		//系统自检RAM错误－－高new
#define AT_SELFTEST_ROM			AT_NONE+80		//系统自检ROM错误－－高new
#define AT_SELFTEST_CPU			AT_NONE+81		//系统自检CPU错误－－高new
#define AT_SELFTEST_IBP				AT_NONE+82		//系统自检IBP错误－－高new
#define AT_SELFTEST_NBP			AT_NONE+83		//系统自检NBP错误－－高new
#define AT_SELFTEST_SPO2			AT_NONE+84		//系统自检SPO2错误－－高new
#define AT_SELFTEST_TEMP			AT_NONE+85		//系统自检TEMP错误－－高new
#define AT_SELFTEST_RESP			AT_NONE+86		//系统自检RESP错误－－高new
#define AT_SELFTEST_ECG			AT_NONE+87		//系统自检ECG错误－－高new

#define AT_POWER_ABNORMAL 		AT_NONE+88		//上次异常关机－－低new
#define AT_POWER_FORCE 	 		AT_NONE+89		//上次强制关机－－低new
#define AT_POWER_COMM		  		AT_NONE+90		//电源通讯错误－－低new
													
//技术参数报警数量
#define	ALM_TEC_INFOMAX			91		
//单条技术参数报警信息长度
#define	ALM_TEC_INFOLEN			50		

//报警状态
#define ALMSTATUS_ALARM		1
#define ALMSTATUS_NORMAL		0

/*
	报警信息结构
*/
typedef struct _B_ALM_INFO{
	BYTE bAlmID;	//报警信息ID，0－没有报警信息
	BYTE bStatus;	//ALMSTATUS_NORMAL－正常，ALMSTATUS_ALARM－报警
	BYTE bClass;	//ALARM_OFF, ALARM_LOW, ALARM_MID, ALARM_HIGH, ALARM_INFO
	BYTE bPrint;	//是否进行报警打印: ALARM_PRINT_OFF, ALARM_PRINT_ON
}B_ALM_INFO, *PB_ALM_INFO;


//定义信息显示的颜色
#define HIGHALM_BKCOLOR	COLOR_red
#define MIDALM_BKCOLOR		COLOR_yellow
#define LOWALM_BKCOLOR		COLOR_cyan
#define NORMAL_BKCOLOR		COLOR_darkgray
#define NORMAL_TXTCOLOR		COLOR_lightwhite
#define NONE_BKCOLOR		COLOR_black
//#define NONE_BKCOLOR		 RGB2Pixel (hdc, 40, 60, 110)
#define INFO_BKCOLOR		COLOR_cyan
#define INFO_TXTCOLOR		COLOR_black




#endif  //__ALARMMANAGE_H__	

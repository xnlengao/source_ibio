/*************************************************************************** 
 *	Module Name:	ComiBio
 *
 *	Abstract:	ComiBio头文件定义
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2012-12-25 11:29:25
 ***************************************************************************/
#ifndef __COMIBIO_H__
#define __COMIBIO_H__
/*--------------------------------------------------------
Data field module -> host
---------------------------------------------------------*/
#define ID_RESET 		0x01
#define ID_COM_STS 		0x02
#define ID_SELFTEST 		0x03
#define ID_CMD_ACK 		0x04
#define ID_ECGWAVE 		0x05
#define ID_LEAD_STS 		0x06
#define ID_HR 			0x07
#define ID_ST 				0x0B
#define ID_ST_PATTERN 	0x0C
#define ID_RESPWAVE 		0x10
#define ID_RR 			0x11
#define ID_APNEA 			0x12
#define ID_CVA 			0x13
#define ID_TEMP 			0x15
#define ID_SPO2_WAVE 	0x16
#define ID_SPO2_PR 		0x17
#define ID_IBP_WAVE 		0x18
#define ID_IBP_STS 		0x19
#define ID_IBP_DATA 		0x1A
#define ID_IBP_CAL 		0x1B
#define ID_IBP_CAL_TIME 	0x1C
#define ID_NBP_CUFPRE 	0x20
#define ID_NBP_END 		0x21
#define ID_NBP_RESULT1 	0x22
#define ID_NBP_RESULT2 	0x23
#define ID_NBP_STS 		0x24
/*--------------------------------------------------------
Data field host -> module
---------------------------------------------------------*/
#define CMD_GET_POST_RESULT 	0x40
#define CMD_PAT_TYPE 		0x42
#define CMD_LEADSYSTEM 		0x45
#define CMD_LEADTYPE 		0x46
#define CMD_FILTER_MODE 		0x47
#define CMD_ECG_GAIN 		0x48
#define CMD_ECG_CAL 			0x49
#define CMD_ECG_TRAP 		0x4A
#define CMD_ID_PACE 			0x4B
#define CMD_ST_ISO 			0x4C
#define CMD_ECG_CHANNEL 		0x4D
#define CMD_ECG_LEARN 		0x4E
#define CMD_RESP_GAIN 		0x50
#define CMD_RESP_TYPE 		0x67
#define CMD_APNEA_ALM 		0x52
#define CMD_TEMP_TYPE 		0x53
#define CMD_SPO2	 			0x54
#define CMD_NBP_START 		0x55
#define CMD_NBP_END 			0x56
#define CMD_NBP_PERIOD 		0x57
#define CMD_NBP_CAL 			0x58
#define CMD_NBP_RESET 		0x59
#define CMD_NBP_PNEUMATIC 	0x5A
#define CMD_NBP_POLL_STATE 	0x5B
#define CMD_NBP_PRE_PRESSURE 	0x5C
#define CMD_NBP_CONTINUE 		0x5D
#define CMD_NBP_GET_RESULT 		0x5E
#define CMD_IBP_CHANNEL 			0x60
#define CMD_IBP_ZERO 		0x61
#define CMD_IBP_CAL1 			0x62
#define CMD_IBP_CAL2 			0x63
#define CMD_IBP_FILTER 		0x64
#define CMD_IBP_POLL_STATE 	0x65


/*
	初始化PM9K测量模块
*/
int InitiBio();
int iBioCMD_GET_POST_RESULT();
int iBioCMD_ECG_CAL(BYTE CalMode);
int SetiBioPatientType();
int SetiBioNIBPPatType();
int SetiBioECGGain();
int SetiBioEcgLeadType();
int SetiBioECGMode();
int SetiBioEcgPace();
int SetiBioEcgACFilter();
int StartiBioCalibrateECG();
int StopiBioCalibrateECG();
int SetiBioRespGain();
int SetiBioRespApnea();
int SetiBioRespType(BYTE leadtype);
int SetiBioSpO2Sense();
int StartiBioNibpMeasure();
int StopiBioNibpMeasure();
int CalibrateiBioNibp();
int iBioNibpLeakTest();
int SetiBioNibpInitPres();
int GetNibpResult();
int GetNibpPollState();
int SetIBioNibpPeriod(BYTE Period);
int SetIBioTempType();
#endif	//__PM9K_H__

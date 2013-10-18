/*************************************************************************** 
 *	Module Name:	CENTRAL
 *
 *	Abstract:	中央机处理程序
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2007-06-09 13:29:25
 ***************************************************************************/
#ifndef __CENTRAL_H__
#define __CENTRAL_H__

//中央机IP地址范围241~254
//广播地址
#define BROADCASTIP				255

//接收缓冲最大长度
#define NETRECVBUFF_MAX_SIZE			2000


//当参数数据无效时的值
#define ERRCODE_W			-30000
#define ERRCODE_B			255

//报文结构
#define	ECG_WAVE_PER_SEC			200	//250 //
#define	SPO2_WAVE_PER_SEC		50	//125	//
#define	RESP_WAVE_PER_SEC		50	//125	//
#define	CO2_WAVE_PER_SEC			100
#define	IBP_WAVE_PER_SEC			100

typedef char S8;
typedef unsigned char U8;
typedef unsigned short int U16;
typedef short int S16;


typedef  struct __attribute__((packed)) __MESSAGEHEAD
{
	U8	sync[2];
	U8	dev_type;
	U8	src_ID;
	U8	dst_ID;
	U8	machine_cfg;
	
}MESSAGEHEAD,*LPMESSAGEHEAD;

typedef  struct __attribute__((packed)) __CENTERDATA
{
	MESSAGEHEAD	message_head;
	U8	cmd;
	U8	message_ID;
	U16 message_len;
	
	U8	ECG_ID;
	U16	ECG_len;
	U16	ECG_module_sta;
	U16	ECG_lead_sta;
	U16	ECG_para_HR;
	U8	ECG_wave[ECG_WAVE_PER_SEC*3];
	
	U8	SPO2_ID;
	U16	SPO2_len;
	U16 SPO2_module_sta;
	U8 	SPO2_para_spo2;
	U16	SPO2_para_PR;
	U8	SPO2_wave[SPO2_WAVE_PER_SEC];
	
	U8	RESP_ID;
	U16	RESP_len;
	U8 	RESP_module_sta;
	U16	RESP_para_RR;
	U8	RESP_wave[RESP_WAVE_PER_SEC];

	U8	NIBP_ID;
	U16	NIBP_len;
	U8	NIBP_module_sta;
	U8	NIBP_err_info;
	U16	NIBP_para_SYS;
	U16	NIBP_para_MEAN;
	U16	NIBP_para_DIA;
	U16 NIBP_para_HR;
	
	U8	TEMP1_ID;
	U16	TEMP1_len;
	U8	TEMP1_module_sta;
	U16	TEMP1_para_temp;
	
	U8	TEMP2_ID;
	U16	TEMP2_len;
	U8	TEMP2_module_sta;
	U16 TEMP2_para_temp;

	U8	CO2_ID;
	U16	CO2_len;
	U8	CO2_module_sta;
	U8	CO2_para_etCO2;
	U8	CO2_para_fiCO2;
	U16	CO2_para_baro;
	U16	CO2_para_awRR;
	U8	CO2_wave[CO2_WAVE_PER_SEC];

	U8	IBP1_ID;
	U16	IBP1_len;
	U8	IBP1_module_sta;
	U8	IBP1_vas_name;
// 	U16	IBP1_para_SYS;
// 	U16 IBP1_para_MEAN;
// 	U16	IBP1_para_DIA;
// 	U16 IBP1_para_HR;
// 	U16	IBP1_wave[IBP_WAVE_PER_SEC];
	S16	IBP1_para_SYS;
	S16 IBP1_para_MEAN;
	S16	IBP1_para_DIA;
	S16 IBP1_para_HR;
	S16	IBP1_wave[IBP_WAVE_PER_SEC];
	

	U8	IBP2_ID;
	U16 IBP2_len;
	U8	IBP2_module_sta;
	U8	IBP2_vas_name;
// 	U16	IBP2_para_SYS;
// 	U16 IBP2_para_MEAN;
// 	U16	IBP2_para_DIA;
// 	U16 IBP2_para_HR;
// 	U16	IBP2_wave[IBP_WAVE_PER_SEC];
	S16	IBP2_para_SYS;
	S16	IBP2_para_MEAN;
	S16	IBP2_para_DIA;
	S16	IBP2_para_HR;
	S16	IBP2_wave[IBP_WAVE_PER_SEC];
/*
	U8	FETAL_ID;
	U16 FETAL_len;
	U8	FETAL_module_sta;
	U16	FETAL_FHR;
	U16	FETAL_OP;
	U8	FETAL_FM;
*/
	U8	msg_tail[2];
}CENTERDATA, *LPCENTERDATA;

typedef  struct __attribute__((packed)) __BROADCASTMSG
{
	MESSAGEHEAD	message_head;
	U8	cmd;
	U8	machine_cfg;
	U8	status1;
	U8	status2;
	U8	room_ID;
	U8	bed_ID;
	U8	msg_tail[2];
}BROADCASTMSG,*LPBROADCASTMSG;

typedef  struct __attribute__((packed)) __ASKPACK
{
	MESSAGEHEAD	message_head;
	U8	cmd;
	U8	high_port;
	U8	low_port;
	U8	msg_tail[2];
}ASKPACK,*LPASKPACK;

typedef  struct __attribute__((packed)) __ANSWERMSG
{
	MESSAGEHEAD	message_head;
	U8	cmd;
	U8	answer;
	U8	msg_tail[2];
}ANSWERMSG,*LPANSWERMSG;

typedef  struct __attribute__((packed)) __STOPMSG
{
	MESSAGEHEAD	message_head;
	U8	cmd;
	U8	msg_tail[2];
}STOPMSG,*LPSTOPMSG;

#define	MAX_CENTER_DATA_PKG		10
typedef struct __attribute__((packed)) __CENTERDATAQUE
{
	CENTERDATA	CenterData[MAX_CENTER_DATA_PKG];
	U8	pack_sta[10];
	U8	write_pkg;
	U8	read_pkg;
	
}CENTERDATAQUE,*LPCENTERDATAQUE;

typedef union __IP_ADDR_
{
  int intip;
  U8  chrip[4];
}IP_ADDR;

typedef struct __attribute__((packed)) __PATIENT_INFO
{
	S8	 	DEPT[20];
	S8 		PAT_NO[20];
	U8 		bed_no;
	S8 		DOCTOR[20];
	S8 		NAME[20];
	U8 		sex;
	U8 		pat_type;
	S8 		ADMIT[20];
	S8 		BIRTH[20];
	U16		heigh;
	U16		weight;
	U8		blood;
}PATIENT_INFO,*LPPATIENT_INFO;

typedef  struct __attribute__((packed)) __PAT_INFO_MSG
{
	MESSAGEHEAD	message_head;
	U8	cmd;
	U16	info_no;
	U8	info_buff[20];
	U8	msg_tail[2];
}PAT_INFO_MSG,*LPPAT_INFO_MSG;

typedef  struct __attribute__((packed)) __PARA_CFG_MSG
{
	MESSAGEHEAD	message_head;
	U8	cmd;
	U16	info_no;
	U16	info_val;
	U8	msg_tail[2];
}PARA_CFG_MSG,*LPPARA_CFG_MSG;

typedef  struct __attribute__((packed)) __CENTER_ALM_MSG
{
	MESSAGEHEAD	message_head;
	U8	cmd;
	U8	alm_class;
	U8	alm_idx;
	U8	alm_sta;
	U8	msg_tail[2];
}CENTER_ALM_MSG,*LPCENTER_ALM_MSG;

typedef  struct __attribute__((packed)) __CENTER_RESEND_MSG
{
	MESSAGEHEAD	message_head;
	U8	cmd;
	U8	resend_num;
	U8	msg_tail[2];
}CENTER_RESEND_MSG,*LPCENTER_RESEND_MSG;

#endif		//__CENTRAL_H__

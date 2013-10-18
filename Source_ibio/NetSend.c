/*************************************************************************** 
 *	Module Name:	NetSend
 *
 *	Abstract:		网络传输程序
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *					2008-10-21 14:30:33
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "General_Funcs.h"
#include "Central.h"



//中央机的端口
int giCentralPort_Data = 2000;
int giCentralPort_Alarm = 2001;
int giCentralPort_Info = 2002;

//他床数据包端口
extern int giOtherBedPort_Data;

//床边机套接字
extern int gfdMonitorSocket;

//中央机地址
extern char gsCentralIP[16];
extern char gsOB_IP[16];
//与中央机连接的状态 FALSE:未连接中央机，TRUE：已经连上中央机
extern BOOL gbCentralStatus;
//中央机的机器号 241~254
extern int giCentralMacNO;

//中央机地址
extern char gsOtherCentralIP[16];
//与中央机连接的状态 FALSE:未连接中央机，TRUE：已经连上中央机
extern BOOL gbOtherCentralStatus;
//中央机的机器号 241~254
extern int giOtherCentralMacNO;


extern int giOBMacNO;
extern BOOL gbOB_Status;

//网络包发送线程
pthread_t ptID_NetSend_Func;

//报文头、尾
#define PACKHEAD_LEN	6
//设备类型，源机器号，目标机器号，机器配置
BYTE gbPackHead[PACKHEAD_LEN] = {0x55, 0xAA, 0x00, 0x00, 0x00, 0x00};
//BYTE gbPackHead[PACKHEAD_LEN] = {0x5A, 0xA5, 0x00, 0x00, 0x00, 0x00};

#define PACKTAIL_LEN		2
BYTE gbPackTail[PACKTAIL_LEN] = {0x77, 0xEE};
//BYTE gbPackTail[PACKTAIL_LEN] = {0x7E, 0xE7};

//报文内容
MESSAGEHEAD			gsMsgHead;				//报文头
BROADCASTMSG			gsBroadcastMsg;			//广播信息
ASKPACK				gsAskPackMsg;			//请求数据连接
ANSWERMSG				gsAnswerMsg;			//应答报文
STOPMSG					gsStopMsg;				//停止通讯报文
CENTERDATA   			gsCentralData;			//数据报文
PAT_INFO_MSG			gsPatInfoMsg;			//病人信息
PARA_CFG_MSG			gsParaCfgMsg;			//参数配置信息
CENTER_ALM_MSG			gsCentralAlmMsg;			//报警信息	
CENTER_RESEND_MSG			gsCentralResendMsg;			//重发报文
/*
	填充报文头，网络程序一开始的时候调用，或者机器号改变时调用
*/
int FillNetMsgHead()
{
	BYTE bStatus = 0x01;
	unsigned int localIP;
	localIP=GetOwnIPAddr(gCfgNet.cIP);
	gsMsgHead.sync[0] = gbPackHead[0];
	gsMsgHead.sync[1] = gbPackHead[1];
	gsMsgHead.dev_type = NMACHINE_MULTI_PARA; //设备类型
	gsMsgHead.src_ID = localIP;//源机器号
	gsMsgHead.dst_ID = 255;		//目标机器号:广播
	//机器配置bit0:常规参数,bit1~2:ibp,bit3:co2 bit4:gas,bit5:c.o.,bit6:ecg12,bit7:保留
 	if(gbHaveIbp)  bStatus |= 0x06;		//double IBP
 	if(gbHaveCo2) bStatus |= 0x08;		//co2
	gsMsgHead.machine_cfg = bStatus;

	return 0;
}


/*
	发送广播报文
*/
int NetSend_Braodcast()
{
	int iLen;
	
	gsBroadcastMsg.message_head = gsMsgHead;
	gsBroadcastMsg.cmd = NCMD_BROADCASE;
	gsBroadcastMsg.machine_cfg = gsMsgHead.machine_cfg;
	gsBroadcastMsg.status1 = giOBMacNO;
	gsBroadcastMsg.status2 = giCentralMacNO;
	gsBroadcastMsg.room_ID = 0x00;
	gsBroadcastMsg.bed_ID = gCfgNet.wBedNO;
	gsBroadcastMsg.msg_tail[0] = gbPackTail[0];
	gsBroadcastMsg.msg_tail[1] =  gbPackTail[1];
//	printf("braodcase\n");
	iLen = NetSend(gfdMonitorSocket, GetNetIPAddr(BROADCASTIP), giCentralPort_Info, &gsBroadcastMsg, sizeof (BROADCASTMSG));
	
	return iLen;
}

int OB_NetSend_Ask_Pack(char *Send_IP)
{
	int iLen;
	
	gsAskPackMsg.message_head = gsMsgHead;
	gsAskPackMsg.cmd = NCMD_ASK_PACK;
	gsAskPackMsg.high_port = (giOtherBedPort_Data>>8)&0xff;
	gsAskPackMsg.low_port = giOtherBedPort_Data&0xff;
	gsAskPackMsg.msg_tail[0] = gbPackTail[0];
	gsAskPackMsg.msg_tail[1] =  gbPackTail[1];

	iLen = NetSend(gfdMonitorSocket, Send_IP, giOtherBedPort_Data, &gsAskPackMsg, sizeof(ASKPACK));
	
	printf("OB_NetSend_Ask_Pack:gsOB_IP=%s,low_port=%x,giOtherBedPort_Data=%d\n",Send_IP,gsAskPackMsg.low_port ,giOtherBedPort_Data);

	return iLen;
}

/*
	应答请求连接
*/
int OB_NetSend_Connect(BOOL bOK,char *Send_IP)
{
	int iLen;
	
	gsAnswerMsg.message_head = gsMsgHead;
	gsAnswerMsg.cmd = NCMD_OB_ANSWER_PACK;
	if(bOK) gsAnswerMsg.answer = 1;
	else gsAnswerMsg.answer = 0;
	gsAnswerMsg.msg_tail[0] = gbPackTail[0];
	gsAnswerMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, Send_IP, giOtherBedPort_Data, &gsAnswerMsg, sizeof(ANSWERMSG));

	if(B_PRINTF)printf("OB_NetSend_Connect:gsOB_IP=%s,answer=%d\n",Send_IP,gsAnswerMsg.answer );

	return iLen;
}

/*
	它床主->从发送停止通讯通知
*/
int OB_NetSend_STOPConnect(char *Send_IP)
{
	int iLen;
	
	gsStopMsg.message_head = gsMsgHead;
	gsStopMsg.cmd = NCMD_OB_STOP_COMM;
	gsStopMsg.msg_tail[0] = gbPackTail[0];
	gsStopMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, Send_IP, giOtherBedPort_Data, &gsStopMsg, sizeof(STOPMSG));
	printf("OB_NetSend_STOPConnect:%s\n",Send_IP);
	return iLen;
}


/*
	应答请求连接
*/
int NetSend_Connect(BOOL bOK)
{
	int iLen;
	
	gsAnswerMsg.message_head = gsMsgHead;
	gsAnswerMsg.cmd =  NCMD_ANSWER_PACK;
	if(bOK) gsAnswerMsg.answer = 1;
	else gsAnswerMsg.answer = 0;
	gsAnswerMsg.msg_tail[0] = gbPackTail[0];
	gsAnswerMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsAnswerMsg, sizeof(ANSWERMSG));

	return iLen;
}
/*
	other center应答请求连接
*/
int NetSend_Connect_Other(BOOL bOK)
{
	int iLen;
	
	gsAnswerMsg.message_head = gsMsgHead;
	gsAnswerMsg.cmd =  NCMD_ANSWER_PACK;
	if(bOK) gsAnswerMsg.answer = 1;
	else gsAnswerMsg.answer = 0;
	gsAnswerMsg.msg_tail[0] = gbPackTail[0];
	gsAnswerMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsAnswerMsg, sizeof(ANSWERMSG));

	return iLen;
}
/*
	请求重发报文
*/
int NetSend_RESEND(BYTE resend_num)
{
	int iLen;
	
	gsCentralResendMsg.message_head = gsMsgHead;
	gsCentralResendMsg.cmd = NCMD_NO_RESENDDATA;
	gsCentralResendMsg.resend_num = resend_num;
	gsCentralResendMsg.msg_tail[0] = gbPackTail[0];
	gsCentralResendMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Data, &gsCentralResendMsg, sizeof(CENTER_RESEND_MSG));
	printf("resend  no resend data\n");
	return iLen;
}

/*
	发送数据报文
*/
//供网络发送的数据缓冲区
BYTE gbNetDataBuff[2048] = {0};

//供网络发送的波形缓冲区(1秒)和实际打包的长度
U8 gbNetWaveBuff_Ecg[(ECG_WAVE_PER_SEC*3)+10] = {0};			//200 x 3Leads
int giNetWaveLength_Ecg = 0;
U8 gbNetWaveBuff_SpO2[SPO2_WAVE_PER_SEC+10] = {0};
int giNetWaveLength_SpO2 = 0;
U8 gbNetWaveBuff_Resp[RESP_WAVE_PER_SEC+10] = {0};
int giNetWaveLength_Resp = 0;
U8 gbNetWaveBuff_CO2[CO2_WAVE_PER_SEC+10] = {0};
int giNetWaveLength_CO2 = 0;
U16 gbNetWaveBuff_IBP1[IBP_WAVE_PER_SEC+10] = {0};	
int giNetWaveLength_Ibp1 = 0;
U16 gbNetWaveBuff_IBP2[IBP_WAVE_PER_SEC+10] = {0};	
int giNetWaveLength_Ibp2 = 0;

#define LENGTH_SN 			3///1	 //报文号长度+length
#define LENGTH_END			2	//报文尾长度
#define LENGTH_ECG			9	
#define LENGTH_SPO2			8	
#define LENGTH_RESP		6	
#define LENGTH_NIBP			13	
#define LENGTH_TEMP1		6	
#define LENGTH_TEMP2		6	
#define LENGTH_CO2			10
#define LENGTH_IBP1			13
#define LENGTH_IBP2			13

//ECG 报文组包
static WORD CreateNetData_ECG(LPCENTERDATA gsCentralData)
{
	int i;
	WORD wLength_Ecg = 0;
	
	//**********ECG ID**********
	gsCentralData->ECG_ID = NMODULE_ECG;
	
	//**********ECG Data Length**********
//	wLength_Ecg = LENGTH_ECG+giNetWaveLength_Ecg;
	wLength_Ecg = LENGTH_ECG+ECG_WAVE_PER_SEC*3;
	gsCentralData->ECG_len = wLength_Ecg;
	
	//********** ECG Status **********
	gsCentralData->ECG_module_sta = 0;
	
	//Bit7:6   10-Diag, 01-Mon, 00-Ope
	switch(gCfgEcg.bMode){
		case ECGMODE_DIAG: gsCentralData->ECG_module_sta |= 0x0080; break;
		case ECGMODE_MON:  gsCentralData->ECG_module_sta |= 0x0040; break;
	}
	//Bit5:4  00-0.25, 01-0.5, 10-1, 11-2
	switch(gCfgEcg.bGain){
		case ECGGAIN_025X : gsCentralData->ECG_module_sta &= 0x00CF; break;
		case ECGGAIN_05X : gsCentralData->ECG_module_sta |= 0x0010; break;
		case ECGGAIN_2X : gsCentralData->ECG_module_sta |= 0x0030; break;
		default : gsCentralData->ECG_module_sta |= 0x0020; break;		//x1
	}
// 	gsCentralData.ECG_module_sta = gsCentralData.ECG_module_sta <<8;
		
	
	//**********导联状态**********
	gsCentralData->ECG_lead_sta = 0;
	//Bit1:0   00-5Leads, 01-3Leads, 10-12Leads
	switch(gCfgEcg.bLeadType){
		case ECGTYPE_5LEAD: gsCentralData->ECG_lead_sta |=0x0000; break;
		case ECGTYPE_3LEAD: gsCentralData->ECG_lead_sta |=0x0001; break;
		default: break;
	}
	//Bit3:2 监护导连，在三导连的模式下测量的导连：00-I, 01-II, 10-III
	switch(gCfgEcg.bChannel1){
		case ECGWAVE_II : gsCentralData->ECG_lead_sta |=0x0004; break;
		case ECGWAVE_III : gsCentralData->ECG_lead_sta |=0x0008; break;
		default: break;
	}
	//Bit4  RA  0-Normal 1-Off
	if(gValueEcg.bRAOff) gsCentralData->ECG_lead_sta |=0x0010;
	//Bit5  RL  0-Normal 1-Off
// 	if(gValueEcg.bRAOff) gbNetDataBuff[15] |=0x20;
	//Bit6  LA  0-Normal 1-Off
	if(gValueEcg.bLAOff) gsCentralData->ECG_lead_sta |=0x0040;
	//Bit7  LL  0-Normal 1-Off
	if(gValueEcg.bLLOff) gsCentralData->ECG_lead_sta |=0x0080;
	//Bit8  V1  0-Normal 1-Off
	if(!gValueEcg.bVOff) gsCentralData->ECG_lead_sta &=0xFEFF;

	//**********HR**********
	if(gValueEcg.wHR >=30 && gValueEcg.wHR <HR_MAX){
		gsCentralData->ECG_para_HR = gValueEcg.wHR;
	}
	else
		gsCentralData->ECG_para_HR = ERRCODE_W;
	
	//**********Wave**********

	for(i=0; i<ECG_WAVE_PER_SEC*3; i++){
	 		gsCentralData->ECG_wave[i] = gbNetWaveBuff_Ecg[i];
	}

	return wLength_Ecg;
	
	
}

//SPO2 报文组包
static WORD CreateNetData_SPO2(LPCENTERDATA gsCentralData)
{
	int i;
	WORD wModuleDataLength_SpO2;
	
	//**********ID**********
	gsCentralData->SPO2_ID = NMODULE_SPO2;
	
	//**********Data Length**********
//	wModuleDataLength_SpO2 = LENGTH_SPO2+giNetWaveLength_SpO2;
	wModuleDataLength_SpO2 = LENGTH_SPO2+SPO2_WAVE_PER_SEC;
	gsCentralData->SPO2_len = wModuleDataLength_SpO2;
	
	//**********Status **********
	gsCentralData->SPO2_module_sta = 0;
	
	//SpO2脱落bit1 :spo2 off 
	if(gValueSpO2.bOff){
		gsCentralData->SPO2_module_sta |= 0x0002;
	}
	
	//SpO2测量状态 bit5~2 TODO:
	/*
	if(gValueSpO2.bErrCode & SPO2_INTERFERE){
		gsCentralData.SPO2_module_sta |= 
	}else if(gValueSpO2.bErrCode & SPO2_SEARCHPULSE){
		GetStringFromResFile(gsLanguageRes, "TEC_INFO", "spo2_error_search", strInfo, sizeof strInfo);		
	}else if(gValueSpO2.bErrCode & SPO2_HIGHLIGHT){
		GetStringFromResFile(gsLanguageRes, "TEC_INFO", "spo2_error_highlight", strInfo, sizeof strInfo);		
	}else if(gValueSpO2.bErrCode & SPO2_WEAKSIGNAL){
		GetStringFromResFile(gsLanguageRes, "TEC_INFO", "spo2_error_weaksignal", strInfo, sizeof strInfo);
	}else if(gValueSpO2.bErrCode & SPO2_SIGNALBAD){
		GetStringFromResFile(gsLanguageRes, "TEC_INFO", "spo2_error_signal_bad", strInfo, sizeof strInfo);
	}
	*/
	//SpO2增益 bit7:6 00：Auto；01：x0.5；10：x1；11：x2 
	switch(gCfgSpO2.bGain){
		case SPO2GAIN_05X: gsCentralData->SPO2_module_sta |= 0x0040;break;
		case SPO2GAIN_1X: gsCentralData->SPO2_module_sta |= 0x0080;break;
		case SPO2GAIN_2X: gsCentralData->SPO2_module_sta |= 0x00C0;break;
		default : break;
	}
	
	//**********SPO2 Para**********
	if(gValueSpO2.bSpO2>0 && gValueSpO2.bSpO2<=100){
		gsCentralData->SPO2_para_spo2 = gValueSpO2.bSpO2;
		gsCentralData->SPO2_para_PR = gValuePulse.wPR;
	}
	else{
		gsCentralData->SPO2_para_spo2 = ERRCODE_B;
		gsCentralData->SPO2_para_PR = ERRCODE_W;
		
	}
	
	//**********Wave**********
	for(i=0; i<SPO2_WAVE_PER_SEC; i++){
		gsCentralData->SPO2_wave[i] = gbNetWaveBuff_SpO2[i];
	}
	return wModuleDataLength_SpO2;
}

//RESP 报文组包
static WORD CreateNetData_RESP(LPCENTERDATA gsCentralData)
{
	int i;
	WORD wModuleDataLength_Resp;
	
	//**********ID**********
	gsCentralData->RESP_ID = NMODULE_RESP;
	
	//**********Data Length**********
//	wModuleDataLength_Resp = LENGTH_RESP+giNetWaveLength_Resp;
	wModuleDataLength_Resp = LENGTH_RESP+RESP_WAVE_PER_SEC;
//	if(B_PRINTF) printf("%s:%d  NET DATA : Resp  Length :%d.\n", __FILE__, __LINE__, wModuleDataLength_Resp);
	gsCentralData->RESP_len = wModuleDataLength_Resp;
 
	//**********Status **********
	gsCentralData->RESP_module_sta = 0;
	//RESP脱落bit1 :0：Normal；1：Off 
	if(gValueResp.bOff){
		gsCentralData->RESP_module_sta |= 0x0002;
	}
	//RESP 增益 bit3:2 00：x0.5；01：x1； 10：x2；11：x4 
	switch(gCfgResp.bGain){
		case RESPGAIN_1X: gsCentralData->SPO2_module_sta |= 0x0040;break;
		case RESPGAIN_2X: gsCentralData->SPO2_module_sta |= 0x0080;break;
		case RESPGAIN_4X: gsCentralData->SPO2_module_sta |= 0x00C0;break;
		default : 
 			gsCentralData->SPO2_module_sta |= 0x0040;break;
		break;
	}
	
	//**********Para**********
	if(!gValueResp.bOff)
		gsCentralData->RESP_para_RR = gValueResp.wRR;
	else
		gsCentralData->RESP_para_RR = ERRCODE_W;
	
	//**********Wave**********
	for(i=0; i<RESP_WAVE_PER_SEC; i++){
		gsCentralData->RESP_wave[i] = gbNetWaveBuff_Resp[i];
	}
	
	return wModuleDataLength_Resp;
}

//NIBP 报文组包
static WORD CreateNetData_NIBP(LPCENTERDATA gsCentralData)
{
	WORD wModuleDataLength_NIBP;
	
	//**********ID**********
	gsCentralData->NIBP_ID = NMODULE_NIBP;
	
	//**********Data Length**********
	wModuleDataLength_NIBP = LENGTH_NIBP;
	gsCentralData->NIBP_len = wModuleDataLength_NIBP;
 
	//**********Status TODO**********
	gsCentralData->NIBP_module_sta = 0;
	//NIBP测量对象 [2:1]
	gsCentralData->NIBP_module_sta|=(gValueNibp.bObject )<<1;
	//	NIBP 测量方式[4:3]
	if(gCfgNibp.bMode!=0)
		gsCentralData->NIBP_module_sta|=(0x1)<<3;

	
	gsCentralData->NIBP_err_info = 0;
	if(gValueNibp.wErrCode & NIBP_ERR_CUFF_TOOLAX){//袖带过松 
			gsCentralData->NIBP_err_info=0x02;
	}else if(gValueNibp.wErrCode & NIBP_ERR_CUFF_LEAK){//漏气
		gsCentralData->NIBP_err_info=0x08;
	}else if(gValueNibp.wErrCode & NIBP_ERR_PRESSURE){//气压错误 
		gsCentralData->NIBP_err_info=0x08;
	}else if(gValueNibp.wErrCode & NIBP_ERR_WEAK_SIGNAL){//微弱信号
		gsCentralData->NIBP_err_info=0x04; 
	}else if(gValueNibp.wErrCode & NIBP_ERR_OVER_RANGE){//超范围
		gsCentralData->NIBP_err_info=0x09; 
	}else if(gValueNibp.wErrCode & NIBP_ERR_EXCESSIVE_MOTION){//过分运动
		gsCentralData->NIBP_err_info=0x03 ;
	}else if(gValueNibp.wErrCode & NIBP_ERR_OVERPRESSURE){//过压 
		gsCentralData->NIBP_err_info=0x05 ;
	}else if(gValueNibp.wErrCode & NIBP_ERR_SIGNAL_SATURATION){//信号饱和 
		gsCentralData->NIBP_err_info=0x07 ;
	}else if(gValueNibp.wErrCode & NIBP_ERR_MODULE_FAULT){//系统错误 
		gsCentralData->NIBP_err_info=0x08;
	}else if(gValueNibp.wErrCode & NIBP_ERR_TIMEOUT){//超时 
		gsCentralData->NIBP_err_info=0x06 ;
	}else if(gValueNibp.wErrCode & NIBP_ERR_CUFF_MISTAKE){//袖带类型错
		gsCentralData->NIBP_err_info=0x01;
	}

	
	//**********Para**********
	if(gValueNibp.wSys>0 && gValueNibp.wSys<NIBP_MAX){
		gsCentralData->NIBP_para_SYS = gValueNibp.wSys;
		gsCentralData->NIBP_para_MEAN = gValueNibp.wMean;
		gsCentralData->NIBP_para_DIA = gValueNibp.wDia;
		gsCentralData->NIBP_para_HR = 0;
	}
	else{
		gsCentralData->NIBP_para_SYS = ERRCODE_W;
		gsCentralData->NIBP_para_MEAN = ERRCODE_W;
		gsCentralData->NIBP_para_DIA = ERRCODE_W;
		gsCentralData->NIBP_para_HR = 0;
		
	}
	if(gbViewDemoData){
		gsCentralData->NIBP_para_SYS = 115;
		gsCentralData->NIBP_para_MEAN = 95;
		gsCentralData->NIBP_para_DIA = 75;
		gsCentralData->NIBP_para_HR = 0;
	
	}
	return wModuleDataLength_NIBP;
}

//Temp1 报文组包
static WORD CreateNetData_TEMP1(LPCENTERDATA gsCentralData)
{
	WORD wModuleDataLength_TEMP1;
	
	//**********ID**********
	gsCentralData->TEMP1_ID = NMODULE_TEMP1;
	
	//**********Data Length**********
	wModuleDataLength_TEMP1 = LENGTH_TEMP1;
	gsCentralData->TEMP1_len = wModuleDataLength_TEMP1;

	//**********Status TODO**********
	gsCentralData->TEMP1_module_sta = 0;
	if(gValueTemp.bOff_T1)
		gsCentralData->TEMP1_module_sta|=0x1<<1;

	//**********Para**********
	if(!gValueTemp.bOff_T1)
		gsCentralData->TEMP1_para_temp = gValueTemp.wT1;
	else
		gsCentralData->TEMP1_para_temp = ERRCODE_W;
		
	return wModuleDataLength_TEMP1;
}

//Temp2 报文组包
static WORD CreateNetData_TEMP2(LPCENTERDATA gsCentralData)
{
	WORD wModuleDataLength_TEMP2;
	
	//**********ID**********
	gsCentralData->TEMP2_ID = NMODULE_TEMP2;
	
	//**********Data Length**********
	wModuleDataLength_TEMP2 = LENGTH_TEMP1;
	gsCentralData->TEMP2_len = wModuleDataLength_TEMP2;

	//**********Status TODO**********
	gsCentralData->TEMP2_module_sta = 0;
	if(gValueTemp.bOff_T2)
		gsCentralData->TEMP2_module_sta|=0x1<<1;
	
	//**********Para**********
	if(!gValueTemp.bOff_T2)
		gsCentralData->TEMP2_para_temp = gValueTemp.wT2;
	else
		gsCentralData->TEMP2_para_temp = ERRCODE_W;

	
	return wModuleDataLength_TEMP2;
}

//CO2 报文组包
static WORD CreateNetData_CO2(LPCENTERDATA gsCentralData)
{
	int i;
	WORD wModuleDataLength_CO2;
	
	//**********ID**********
	gsCentralData->CO2_ID = NMODULE_CO2;
	
	//**********Data Length**********
	wModuleDataLength_CO2 = LENGTH_CO2+ CO2_WAVE_PER_SEC;
	gsCentralData->CO2_len = wModuleDataLength_CO2;

	//**********Status TODO**********
	gsCentralData->CO2_module_sta = 0;
	if(gValueCO2.bOff)
		gsCentralData->CO2_module_sta |= 0x01<<1;
	if(gValueCO2.bStatus==CO2_STATUS_RUN)
		gsCentralData->CO2_module_sta |= 0x01<<2;
	
	//**********Para**********
	if(!gValueCO2.bOff){
		gsCentralData->CO2_para_etCO2 = gValueCO2.wEtCO2;
		gsCentralData->CO2_para_fiCO2 = gValueCO2.wFiCO2;
		gsCentralData->CO2_para_baro = gValueCO2.wBaro;
		gsCentralData->CO2_para_awRR = gValueCO2.wAwRR;
	}
	else{
		/*
		gsCentralData->CO2_para_etCO2 = ERRCODE_B;
		gsCentralData->CO2_para_fiCO2 = ERRCODE_B;
		gsCentralData->CO2_para_baro = ERRCODE_W;
		gsCentralData->CO2_para_awRR = ERRCODE_W;
		*/
		gsCentralData->CO2_para_etCO2 = 12;
		gsCentralData->CO2_para_fiCO2 = 13;
		gsCentralData->CO2_para_baro = 14;
		gsCentralData->CO2_para_awRR = 15;
	}
	
	//**********Wave**********
	for(i=0; i<CO2_WAVE_PER_SEC; i++)
	//	gsCentralData->CO2_wave[i] = gbNetWaveBuff_CO2[i];		
	gsCentralData->CO2_wave[i] = i*2;
	return wModuleDataLength_CO2;
}

//IBP1 报文组包
static WORD CreateNetData_IBP1(LPCENTERDATA gsCentralData)
{
	int i;
	WORD wModuleDataLength_IBP1;
	
	//**********ID**********
	gsCentralData->IBP1_ID = NMODULE_IBP1;
	
	//**********Data Length**********
//	wModuleDataLength_IBP1 = LENGTH_IBP1+ giNetWaveLength_Ibp1*2;
	wModuleDataLength_IBP1 = LENGTH_IBP1+ IBP_WAVE_PER_SEC*2;
	gsCentralData->IBP1_len = wModuleDataLength_IBP1;

	//**********Status TODO**********
	gsCentralData->IBP1_module_sta = 0;
	if(gValueIbp1.bOff)
		gsCentralData->IBP1_module_sta |= 0x01<<1;	
	
	//**********血管 TODO**********
	gsCentralData->IBP1_vas_name = 0;
	if(gCfgIbp1.bLabel==IBPLAB_ART)
		gsCentralData->IBP1_vas_name = 0x01;
	if(gCfgIbp1.bLabel==IBPLAB_PA)
		gsCentralData->IBP1_vas_name = 0x02;
	if(gCfgIbp1.bLabel==IBPLAB_CVP)
		gsCentralData->IBP1_vas_name = 0x07;
	if(gCfgIbp1.bLabel==IBPLAB_LAP)
		gsCentralData->IBP1_vas_name = 0x08;
	if(gCfgIbp1.bLabel==IBPLAB_RAP)
		gsCentralData->IBP1_vas_name = 0x09;
	if(gCfgIbp1.bLabel==IBPLAB_ICP)
		gsCentralData->IBP1_vas_name = 0x0A;



	//**********Para**********
	if(!gValueIbp1.bOff){
		gsCentralData->IBP1_para_SYS = gValueIbp1.iSys;
		gsCentralData->IBP1_para_MEAN = gValueIbp1.iMean;
		gsCentralData->IBP1_para_DIA = gValueIbp1.iDia;
	}
	else{
		gsCentralData->IBP1_para_SYS = ERRCODE_W;
		gsCentralData->IBP1_para_MEAN = ERRCODE_W;
		gsCentralData->IBP1_para_DIA = ERRCODE_W;
	}
  	
	//**********Wave**********
	for(i=0; i<IBP_WAVE_PER_SEC; i++){
  		gsCentralData->IBP1_wave[i] = gbNetWaveBuff_IBP1[i];
//  		gsCentralData->IBP1_wave[i] = i*2;				
	}
	
	return wModuleDataLength_IBP1;
	
}


//IBP2报文组包
static WORD CreateNetData_IBP2(LPCENTERDATA gsCentralData)
{
	int i;
	WORD wModuleDataLength_IBP2;
	
	//**********ID**********
	gsCentralData->IBP2_ID = NMODULE_IBP2;
	
	//**********Data Length**********
//	wModuleDataLength_IBP2 = LENGTH_IBP2 + giNetWaveLength_Ibp2*2;
	wModuleDataLength_IBP2 = LENGTH_IBP2 + IBP_WAVE_PER_SEC*2;
	gsCentralData->IBP2_len = wModuleDataLength_IBP2;

	//**********Status TODO**********
	gsCentralData->IBP2_module_sta = 0;
	
	//**********血管 TODO**********
	gsCentralData->IBP2_vas_name = 0;
	if(gCfgIbp2.bLabel==IBPLAB_ART)
		gsCentralData->IBP2_vas_name = 0x01;
	if(gCfgIbp2.bLabel==IBPLAB_PA)
		gsCentralData->IBP2_vas_name = 0x02;
	if(gCfgIbp2.bLabel==IBPLAB_CVP)
		gsCentralData->IBP2_vas_name = 0x07;
	if(gCfgIbp2.bLabel==IBPLAB_LAP)
		gsCentralData->IBP2_vas_name = 0x08;
	if(gCfgIbp2.bLabel==IBPLAB_RAP)
		gsCentralData->IBP2_vas_name = 0x09;
	if(gCfgIbp2.bLabel==IBPLAB_ICP)
		gsCentralData->IBP2_vas_name = 0x0A;

	//**********Para**********
	if(!gValueIbp2.bOff){
  		gsCentralData->IBP2_para_SYS = gValueIbp2.iSys;
  		gsCentralData->IBP2_para_MEAN = gValueIbp2.iMean;
  		gsCentralData->IBP2_para_DIA = gValueIbp2.iDia;
	}
	else{
		gsCentralData->IBP2_para_SYS = ERRCODE_W;
		gsCentralData->IBP2_para_MEAN = ERRCODE_W;
		gsCentralData->IBP2_para_DIA = ERRCODE_W;
	}
	
	//**********Wave**********
	for(i=0; i<IBP_WAVE_PER_SEC; i++){
 		gsCentralData->IBP2_wave[i] = gbNetWaveBuff_IBP2[i];
// 		gsCentralData->IBP2_wave[i] = i*3;				
	}
	return wModuleDataLength_IBP2;
}
/*
//IFETAL报文组包
static WORD CreateNetData_Fetal(LPCENTERDATA gsCentralData)
{
	int i;
	WORD wModuleDataLength_Fetal;
	
	//**********ID**********
	gsCentralData->FETAL_ID = 0x0B;
	
	//**********Data Length**********
	wModuleDataLength_Fetal = 9;
	gsCentralData->FETAL_len = wModuleDataLength_Fetal;

	//**********Status TODO**********
	gsCentralData->FETAL_module_sta = 0;
	
	//**********FETAL_FHR TODO**********
	gsCentralData->FETAL_FHR = 0;
	
	gsCentralData->FETAL_OP = 0;

	gsCentralData->FETAL_FM = 0;
	
	return wModuleDataLength_Fetal;
}
*/
/*
	组织数据报文
	注意：协议的报文对于字中高低位的解释： 7～0，15～8 （shit!）
*/
int PackNetData()
{
	int i, j, iLen;
	
	int res;
	S_ECG_VALUE ecgValue;
	
	static BYTE bPackCount = 0;
	WORD wPackLength = 0;
	WORD wModuleDataLength_Ecg = 0;
	WORD wModuleDataLength_SpO2 = 0;
	WORD wModuleDataLength_Resp = 0;
	WORD wModuleDataLength_NIBP = 0;
	WORD wModuleDataLength_TEMP1 = 0;
	WORD wModuleDataLength_TEMP2 = 0;
	WORD wModuleDataLength_CO2 = 0;
	WORD wModuleDataLength_IBP1 = 0;
	WORD wModuleDataLength_IBP2 = 0;
	WORD wModuleDataLength_Fetal = 0;
	
	gsCentralData.message_head = gsMsgHead;
	gsCentralData.cmd = NCMD_DATA_PACK;	
	gsCentralData.message_ID = bPackCount;

	//------ECG Module data-----------
	wModuleDataLength_Ecg=CreateNetData_ECG(&gsCentralData);
	
	//------SpO2 Module data-----------
	wModuleDataLength_SpO2=CreateNetData_SPO2(&gsCentralData);
	
	//------RESP Module data-----------
	wModuleDataLength_Resp=CreateNetData_RESP(&gsCentralData);
	
	//------NIBP Module data-----------
	wModuleDataLength_NIBP=CreateNetData_NIBP(&gsCentralData);
	
	//------TEMP1 Module data-----------	
	wModuleDataLength_TEMP1=CreateNetData_TEMP1(&gsCentralData);
	
	//------TEMP2 Module data-----------
	wModuleDataLength_TEMP2=CreateNetData_TEMP2(&gsCentralData);

//包尾---------TODO:结构体直接发送，包尾不好处理，先这样直接插入处理
	gsCentralData.CO2_ID =gbPackTail[0];
	gsCentralData.CO2_len = gbPackTail[1];

//TODO: 中央机会根据配置来读取相应数据，如果发送数据与配置不同 会导致中央机死机	/*
	if(gsCentralData.message_head.machine_cfg&0x08||gsCentralData.message_head.machine_cfg&0x06){
	//------CO2 Module data-----------
		wModuleDataLength_CO2=CreateNetData_CO2(&gsCentralData);
	//包尾---------TODO:结构体直接发送，包尾不好处理，先这样直接插入处理
//	if(B_PRINTF)printf("---------wModuleDataLength_CO2=%d------\n",wModuleDataLength_CO2);
		gsCentralData.IBP1_ID = gbPackTail[0];
		gsCentralData.IBP1_len = gbPackTail[1];
	}

	if(gsCentralData.message_head.machine_cfg&0x06){
		//------IBP1 Module data-----------
		wModuleDataLength_IBP1=CreateNetData_IBP1(&gsCentralData);
		
		//------IBP2 Module data-----------
		wModuleDataLength_IBP2=CreateNetData_IBP2(&gsCentralData);
	
	}

//	wModuleDataLength_Fetal=CreateNetData_Fetal(&gsCentralData);

	//Pack Length  报文号+报文尾+Datas Length
	wPackLength = LENGTH_SN+LENGTH_END+wModuleDataLength_Ecg + wModuleDataLength_SpO2 
					+ wModuleDataLength_Resp + wModuleDataLength_NIBP + wModuleDataLength_TEMP1 
					+ wModuleDataLength_TEMP2 +wModuleDataLength_CO2+wModuleDataLength_IBP1
					+wModuleDataLength_IBP2+wModuleDataLength_Fetal;
	

//	if(B_PRINTF) printf("%s:%d Send Pack(%d) Length is %d   Total size:%d,Ecg=%d,SpO2=%d,Resp=%d.\n", __FILE__, __LINE__,
//					bPackCount, wPackLength, sizeof gsCentralData,giNetWaveLength_Ecg,giNetWaveLength_SpO2,giNetWaveLength_Resp);
	if(B_PRINTF) printf("***ecg:%d,spo2:%d,resp:%d,nibp:%d,temp1:%d,temp2:%d,co2:%d,ibp1:%d,ibp2:%d,fetal:%d*****************\n",
		wModuleDataLength_Ecg,wModuleDataLength_SpO2,wModuleDataLength_Resp,wModuleDataLength_NIBP,
		wModuleDataLength_TEMP1,wModuleDataLength_TEMP2,wModuleDataLength_CO2,wModuleDataLength_IBP1,wModuleDataLength_IBP2,wModuleDataLength_Fetal);

	gsCentralData.message_len = wPackLength;	

	
	//Tail
	gsCentralData.msg_tail[0] = gbPackTail[0];
	gsCentralData.msg_tail[1] =  gbPackTail[1];
		
	//报文号
	if(bPackCount <255) bPackCount ++;
	else bPackCount = 0;
	
	
	return iLen;
}

/*
	发送数据报文
*/
int NetSend_Data()
{
	int i, iLen,idatasize;	
	//组织数据报文
	if(gbCentralStatus||gbOB_Status||gbOtherCentralStatus){
		PackNetData();
		idatasize= gsCentralData.message_len + 7;
	//	if(B_PRINTF)printf("**************idatasize=%d\n",idatasize);
	}
	
	//发送数据报文	
	if(gbCentralStatus){
		if(B_PRINTF) printf("111111central data send to %s  length=%d,ID=%d\n",gsCentralIP,gsCentralData.message_len,gsCentralData.message_ID);
		
		iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Data, &gsCentralData, idatasize);

	}
	if(gbOtherCentralStatus){
		if(B_PRINTF) printf("2222central data send to %s  length=%d,ID=%d\n",gsOtherCentralIP,gsCentralData.message_len,gsCentralData.message_ID);
		
		iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Data, &gsCentralData, idatasize);

	}
	
	if(gbOB_Status&&giOBMacNO!=0){
		
	//	if(B_PRINTF)printf("22222other bed data send to:%s\n",gsOB_IP);
		iLen = NetSend(gfdMonitorSocket, gsOB_IP, giOtherBedPort_Data, &gsCentralData, sizeof(CENTERDATA));
	}
	
	//复位波形缓冲
	memset(gbNetWaveBuff_Ecg, 0, sizeof gbNetWaveBuff_Ecg);
	giNetWaveLength_Ecg = 0;
	memset(gbNetWaveBuff_SpO2, 0, sizeof gbNetWaveBuff_SpO2);
	giNetWaveLength_SpO2 = 0;
	memset(gbNetWaveBuff_Resp, 0, sizeof gbNetWaveBuff_Resp);
	giNetWaveLength_Resp = 0;
	memset(gbNetWaveBuff_CO2, 0, sizeof gbNetWaveBuff_CO2);
	giNetWaveLength_CO2 = 0;
	memset(gbNetWaveBuff_IBP1, 0, sizeof gbNetWaveBuff_IBP1);
	giNetWaveLength_Ibp1= 0;
	memset(gbNetWaveBuff_IBP2, 0, sizeof gbNetWaveBuff_IBP2);
	giNetWaveLength_Ibp2= 0;

	return iLen;
}

int NetSend_Data_Broadcast()
{
	int i, iLen;

		
	//组织数据报文
	PackNetData();
	//发送数据报文	
// 	iLen = NetSend(gfdMonitorSocket, "127.0.0.1", giOtherBedPort_Data, &gsCentralData, sizeof(CENTERDATA));

	if(gbOB_Status){
		if(B_PRINTF)printf("send to:gsOB_IP=%s\n",gsOB_IP);
		iLen = NetSend(gfdMonitorSocket, gsOB_IP, giOtherBedPort_Data, &gsCentralData, sizeof(CENTERDATA));
	}
	//复位波形缓冲
	
 	memset(gbNetWaveBuff_Ecg, 0, sizeof gbNetWaveBuff_Ecg);
	giNetWaveLength_Ecg = 0;
	memset(gbNetWaveBuff_SpO2, 0, sizeof gbNetWaveBuff_SpO2);
	giNetWaveLength_SpO2 = 0;
	memset(gbNetWaveBuff_Resp, 0, sizeof gbNetWaveBuff_Resp);
	giNetWaveLength_Resp = 0;
	memset(gbNetWaveBuff_CO2, 0, sizeof gbNetWaveBuff_CO2);
	giNetWaveLength_CO2 = 0;
	memset(gbNetWaveBuff_IBP1, 0, sizeof gbNetWaveBuff_IBP1);
	giNetWaveLength_Ibp1= 0;
	memset(gbNetWaveBuff_IBP2, 0, sizeof gbNetWaveBuff_IBP2);
	giNetWaveLength_Ibp2= 0;

	return iLen;
}

/*
	发送病人信息报文
*/
int NetSend_PatientInfo_Room()
{

	int iLen;
			
	gsPatInfoMsg.message_head = gsMsgHead;
	gsPatInfoMsg.cmd = NCMD_UPLOAD_PATINFO;
	gsPatInfoMsg.info_no = 1;		//Patient NO.
	memset(gsPatInfoMsg.info_buff, 0, 20);
	snprintf(gsPatInfoMsg.info_buff, 20, "%s", gCfgPatient.sRoom);
	gsPatInfoMsg.msg_tail[0] = gbPackTail[0];
	gsPatInfoMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsPatInfoMsg, sizeof(PAT_INFO_MSG));
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsPatInfoMsg, sizeof(PAT_INFO_MSG));
	return iLen;
}

int NetSend_PatientInfo_PatID()
{
	int iLen;
			
	gsPatInfoMsg.message_head = gsMsgHead;
	gsPatInfoMsg.cmd = NCMD_UPLOAD_PATINFO;
	gsPatInfoMsg.info_no = 2;		//Patient NO.
	memset(gsPatInfoMsg.info_buff, 0, 20);
	snprintf(gsPatInfoMsg.info_buff, 20, "%s", gCfgPatient.sID);
	gsPatInfoMsg.msg_tail[0] = gbPackTail[0];
	gsPatInfoMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsPatInfoMsg, sizeof(PAT_INFO_MSG));
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsPatInfoMsg, sizeof(PAT_INFO_MSG));
	
	return iLen;
}

int NetSend_PatientInfo_Bedno()
{
	int iLen;
			
	gsPatInfoMsg.message_head = gsMsgHead;
	gsPatInfoMsg.cmd = NCMD_UPLOAD_PATINFO;
	gsPatInfoMsg.info_no = 3;		//Patient NO.
	memset(gsPatInfoMsg.info_buff, 0, 20);
	snprintf(gsPatInfoMsg.info_buff, 20, "%s", gCfgPatient.sBed);
	gsPatInfoMsg.msg_tail[0] = gbPackTail[0];
	gsPatInfoMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsPatInfoMsg, sizeof(PAT_INFO_MSG));
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsPatInfoMsg, sizeof(PAT_INFO_MSG));
	return iLen;
}

int NetSend_PatientInfo_Doctor()
{
	int iLen;
			
	gsPatInfoMsg.message_head = gsMsgHead;
	gsPatInfoMsg.cmd = NCMD_UPLOAD_PATINFO;
	gsPatInfoMsg.info_no = 4;		//Name
	memset(gsPatInfoMsg.info_buff, 0, 20);
	snprintf(gsPatInfoMsg.info_buff, 20, "%s", gCfgPatient.sDoctor);
	gsPatInfoMsg.msg_tail[0] = gbPackTail[0];
	gsPatInfoMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsPatInfoMsg, sizeof(PAT_INFO_MSG));
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsPatInfoMsg, sizeof(PAT_INFO_MSG));
	return iLen;
}

int NetSend_PatientInfo_Name()
{
	int iLen;
			
	gsPatInfoMsg.message_head = gsMsgHead;
	gsPatInfoMsg.cmd = NCMD_UPLOAD_PATINFO;
	gsPatInfoMsg.info_no = 5;		//Name
	memset(gsPatInfoMsg.info_buff, 0, 20);
	snprintf(gsPatInfoMsg.info_buff, 20, "%s", gCfgPatient.sName);
	gsPatInfoMsg.msg_tail[0] = gbPackTail[0];
	gsPatInfoMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsPatInfoMsg, sizeof(PAT_INFO_MSG));
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsPatInfoMsg, sizeof(PAT_INFO_MSG));
	return iLen;
}

int NetSend_PatientInfo_Sex()
{
	int iLen;
			
	gsPatInfoMsg.message_head = gsMsgHead;
	gsPatInfoMsg.cmd = NCMD_UPLOAD_PATINFO;
	gsPatInfoMsg.info_no = 6;		//Sex
	memset(gsPatInfoMsg.info_buff, 0, 20);
	if(gCfgPatient.bSex == MALE)
		snprintf(gsPatInfoMsg.info_buff, 20, "%s", "MALE");
	else
		snprintf(gsPatInfoMsg.info_buff, 20, "%s", "FEMALE");
	gsPatInfoMsg.msg_tail[0] = gbPackTail[0];
	gsPatInfoMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsPatInfoMsg, sizeof(PAT_INFO_MSG));
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsPatInfoMsg, sizeof(PAT_INFO_MSG));
	return iLen;
}

int NetSend_PatientInfo_Type()
{
	int iLen;
			
	gsPatInfoMsg.message_head = gsMsgHead;
	gsPatInfoMsg.cmd = NCMD_UPLOAD_PATINFO;
	gsPatInfoMsg.info_no = 7;		//Type
	memset(gsPatInfoMsg.info_buff, 0, 20);
	switch(gCfgPatient.bObject){
		case CHILD: snprintf(gsPatInfoMsg.info_buff, 20, "%s", "BABY");break;
		case BABY: snprintf(gsPatInfoMsg.info_buff, 20, "%s", "NEO");break;
		default: snprintf(gsPatInfoMsg.info_buff, 20, "%s", "ADU");break;
	}
	gsPatInfoMsg.msg_tail[0] = gbPackTail[0];
	gsPatInfoMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsPatInfoMsg, sizeof(PAT_INFO_MSG));
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsPatInfoMsg, sizeof(PAT_INFO_MSG));
	return iLen;
}

int NetSend_PatientInfo_Admit()
{
	int iLen;
			
	gsPatInfoMsg.message_head = gsMsgHead;
	gsPatInfoMsg.cmd = NCMD_UPLOAD_PATINFO;
	gsPatInfoMsg.info_no = 8;		//Name
	memset(gsPatInfoMsg.info_buff, 0, 20);
	snprintf(gsPatInfoMsg.info_buff, 20, "%s", gCfgPatient.sAdmit);
	gsPatInfoMsg.msg_tail[0] = gbPackTail[0];
	gsPatInfoMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsPatInfoMsg, sizeof(PAT_INFO_MSG));
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsPatInfoMsg, sizeof(PAT_INFO_MSG));
	
	return iLen;
}

int NetSend_PatientInfo_Birth()
{
	int iLen;
			
	gsPatInfoMsg.message_head = gsMsgHead;
	gsPatInfoMsg.cmd = NCMD_UPLOAD_PATINFO;
	gsPatInfoMsg.info_no = 9;		//Name
	memset(gsPatInfoMsg.info_buff, 0, 20);
	snprintf(gsPatInfoMsg.info_buff, 20, "%s", gCfgPatient.sBirth);
	gsPatInfoMsg.msg_tail[0] = gbPackTail[0];
	gsPatInfoMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsPatInfoMsg, sizeof(PAT_INFO_MSG));
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsPatInfoMsg, sizeof(PAT_INFO_MSG));
	return iLen;
}
int NetSend_PatientInfo_Height()
{
	int iLen;
			
	gsPatInfoMsg.message_head = gsMsgHead;
	gsPatInfoMsg.cmd = NCMD_UPLOAD_PATINFO;
	gsPatInfoMsg.info_no = 10;		//Height
	memset(gsPatInfoMsg.info_buff, 0, 20);
	snprintf(gsPatInfoMsg.info_buff, 20, "%d", gCfgPatient.wHeight);
	gsPatInfoMsg.msg_tail[0] = gbPackTail[0];
	gsPatInfoMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsPatInfoMsg, sizeof(PAT_INFO_MSG));
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsPatInfoMsg, sizeof(PAT_INFO_MSG));
	return iLen;
}

int NetSend_PatientInfo_Weight()
{
	int iLen;
			
	gsPatInfoMsg.message_head = gsMsgHead;
	gsPatInfoMsg.cmd = NCMD_UPLOAD_PATINFO;
	gsPatInfoMsg.info_no = 11;		//Weight
	memset(gsPatInfoMsg.info_buff, 0, 20);
 	snprintf(gsPatInfoMsg.info_buff, 20, "%d", gCfgPatient.wWeight);
	gsPatInfoMsg.msg_tail[0] = gbPackTail[0];
	gsPatInfoMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsPatInfoMsg, sizeof(PAT_INFO_MSG));
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsPatInfoMsg, sizeof(PAT_INFO_MSG));
	return iLen;
}

int NetSend_PatientInfo_Blood()
{
	int iLen;
			
	gsPatInfoMsg.message_head = gsMsgHead;
	gsPatInfoMsg.cmd = NCMD_UPLOAD_PATINFO;
	gsPatInfoMsg.info_no = 12;		//Weight
	memset(gsPatInfoMsg.info_buff, 0, 20);
	switch(gCfgPatient.bBlood){
		case BLOOD_A: snprintf(gsPatInfoMsg.info_buff, 20, "%s", "A");break;
		case BLOOD_B: snprintf(gsPatInfoMsg.info_buff, 20, "%s", "B");break;
		case BLOOD_O: snprintf(gsPatInfoMsg.info_buff, 20, "%s", "O");break;
		case BLOOD_AB: snprintf(gsPatInfoMsg.info_buff, 20, "%s", "AB");break;
		default: snprintf(gsPatInfoMsg.info_buff, 20, "%s", "N/A");break;
	}
	gsPatInfoMsg.msg_tail[0] = gbPackTail[0];
	gsPatInfoMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsPatInfoMsg, sizeof(PAT_INFO_MSG));
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsPatInfoMsg, sizeof(PAT_INFO_MSG));
	return iLen;
}


int NetSend_PatientInfo()
{
	//Room
	NetSend_PatientInfo_Room();
	//Pat NO.
	NetSend_PatientInfo_PatID();
	//Doctor
 	NetSend_PatientInfo_Doctor();
 	//Name
 	NetSend_PatientInfo_Name();
 	//Sex
 	NetSend_PatientInfo_Sex();
 	//type
 	NetSend_PatientInfo_Type();
	//Admit
 	NetSend_PatientInfo_Admit();
 	//Birth
 	NetSend_PatientInfo_Birth();
 	//Height
 	NetSend_PatientInfo_Height();
 	//Weight
 	NetSend_PatientInfo_Weight();
	//Blood
 	NetSend_PatientInfo_Blood();
	printf("++++++++++++++Net send patient info!\n");
	return 0;
}

/*
	发送ECG Info
*/
int NetSend_EcgInfo()
{
	int iOffset = 100;
	
	gsParaCfgMsg.message_head = gsMsgHead;
	gsParaCfgMsg.cmd = NCMD_UPLOAD_CFG;
	
	//Alarm Lever
	gsParaCfgMsg.info_no = iOffset+1;
	switch(gCfgEcg.bAlmControl){
		case ALARM_LOW: gsParaCfgMsg.info_val = 2; break;
		case ALARM_MID: gsParaCfgMsg.info_val = 1; break;
		case ALARM_HIGH: gsParaCfgMsg.info_val = 0; break;
		default : gsParaCfgMsg.info_val = 3; break;
	}
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));

	//HR High
	gsParaCfgMsg.info_no = iOffset+2;
	gsParaCfgMsg.info_val = gCfgEcg.wHigh_HR;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));

	//HR Low
	gsParaCfgMsg.info_no = iOffset+3;
	gsParaCfgMsg.info_val = gCfgEcg.wLow_HR;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	
	//Lead Type
	gsParaCfgMsg.info_no = iOffset+4;
	if(gCfgEcg.bLeadType == ECGTYPE_5LEAD)
		gsParaCfgMsg.info_val = 1;
	else
		gsParaCfgMsg.info_val = 0;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));

	//Mon Type
	gsParaCfgMsg.info_no = iOffset+5;
	switch(gCfgEcg.bMode){
		case ECGMODE_DIAG: gsParaCfgMsg.info_val = 2; break;
		case ECGMODE_MON: gsParaCfgMsg.info_val = 1; break;
		case ECGMODE_OPE: gsParaCfgMsg.info_val = 0; break;
		default : gsParaCfgMsg.info_val = 0; break;
	}
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	return 0;
}

/*
	发送SpO2Info
*/
int NetSend_SpO2Info()
{
	int iOffset = 200;
	
	gsParaCfgMsg.message_head = gsMsgHead;
	gsParaCfgMsg.cmd = NCMD_UPLOAD_CFG;
	
	//Alarm Lever
	gsParaCfgMsg.info_no = iOffset+1;
	switch(gCfgSpO2.bAlmControl){
		case ALARM_LOW: gsParaCfgMsg.info_val = 2; break;
		case ALARM_MID: gsParaCfgMsg.info_val = 1; break;
		case ALARM_HIGH: gsParaCfgMsg.info_val = 0; break;
		default : gsParaCfgMsg.info_val = 3; break;
	}
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));

	//SpO2 High
	gsParaCfgMsg.info_no = iOffset+2;
	gsParaCfgMsg.info_val = gCfgSpO2.bHigh_SpO2;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));

	//SpO2 Low
	gsParaCfgMsg.info_no = iOffset+3;
	gsParaCfgMsg.info_val = gCfgSpO2.bLow_SpO2;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));

	//PR High
	gsParaCfgMsg.info_no = iOffset+4;
	gsParaCfgMsg.info_val = gCfgPulse.wHigh_PR;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	
	//PR Low
	gsParaCfgMsg.info_no = iOffset+5;
	gsParaCfgMsg.info_val = gCfgPulse.wLow_PR;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	
	return 0;
}

/*
	发送Resp Info
*/
int NetSend_RespInfo()
{
	int iOffset = 300;
	
	gsParaCfgMsg.message_head = gsMsgHead;
	gsParaCfgMsg.cmd = NCMD_UPLOAD_CFG;
	
	//Alarm Lever
	gsParaCfgMsg.info_no = iOffset+1;
	switch(gCfgResp.bAlmControl){
		case ALARM_LOW: gsParaCfgMsg.info_val = 2; break;
		case ALARM_MID: gsParaCfgMsg.info_val = 1; break;
		case ALARM_HIGH: gsParaCfgMsg.info_val = 0; break;
		default : gsParaCfgMsg.info_val = 3; break;
	}
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	
	//RR High
	gsParaCfgMsg.info_no = iOffset+2;
	gsParaCfgMsg.info_val = gCfgResp.wHigh_RR;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	
	//RR Low
	gsParaCfgMsg.info_no = iOffset+3;
	gsParaCfgMsg.info_val = gCfgResp.wLow_RR;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	
	//Apnea  TODO :与协议不符，暂不做
	
	
	return 0;
}

/*
	发送NIBP Info
*/
int NetSend_NibpInfo()
{
	int iOffset = 400;
	
	gsParaCfgMsg.message_head = gsMsgHead;
	gsParaCfgMsg.cmd = NCMD_UPLOAD_CFG;
	
	//Alarm Lever
	gsParaCfgMsg.info_no = iOffset+1;
	switch(gCfgNibp.bAlmControl){
		case ALARM_LOW: gsParaCfgMsg.info_val = 2; break;
		case ALARM_MID: gsParaCfgMsg.info_val = 1; break;
		case ALARM_HIGH: gsParaCfgMsg.info_val = 0; break;
		default : gsParaCfgMsg.info_val = 3; break;
	}
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	
	//SYS High
	gsParaCfgMsg.info_no = iOffset+2;
	gsParaCfgMsg.info_val = gCfgNibp.wHigh_Sys;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	
	//SYS Low
	gsParaCfgMsg.info_no = iOffset+3;
	gsParaCfgMsg.info_val = gCfgNibp.wLow_Sys;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	
	//MAP High
	gsParaCfgMsg.info_no = iOffset+4;
	gsParaCfgMsg.info_val = gCfgNibp.wHigh_Mean;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	
	//MAP Low
	gsParaCfgMsg.info_no = iOffset+5;
	gsParaCfgMsg.info_val = gCfgNibp.wLow_Mean;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	
	//DIA High
	gsParaCfgMsg.info_no = iOffset+6;
	gsParaCfgMsg.info_val = gCfgNibp.wHigh_Dia;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));

	//DIA Low
	gsParaCfgMsg.info_no = iOffset+7;
	gsParaCfgMsg.info_val = gCfgNibp.wLow_Dia;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	
	//Intervalue TODO:与协议不符，暂不做
	
	return 0;
}

/*
	发送Temp1 Info
*/
int NetSend_Temp1Info()
{
	int iOffset = 500;
	
	gsParaCfgMsg.message_head = gsMsgHead;
	gsParaCfgMsg.cmd = NCMD_UPLOAD_CFG;
	
	//Alarm Lever
	gsParaCfgMsg.info_no = iOffset+1;
	switch(gCfgTemp.bAlmControl){
		case ALARM_LOW: gsParaCfgMsg.info_val = 2; break;
		case ALARM_MID: gsParaCfgMsg.info_val = 1; break;
		case ALARM_HIGH: gsParaCfgMsg.info_val = 0; break;
		default : gsParaCfgMsg.info_val = 3; break;
	}
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	
	//Temp1 High
	gsParaCfgMsg.info_no = iOffset+2;
	gsParaCfgMsg.info_val = gCfgTemp.wHigh_T1;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));

	//Temp1 Low
	gsParaCfgMsg.info_no = iOffset+3;
	gsParaCfgMsg.info_val = gCfgTemp.wLow_T1;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));

	return 0;
}

/*
	发送Temp2 Info
*/
int NetSend_Temp2Info()
{
	int iOffset = 600;
	
	gsParaCfgMsg.message_head = gsMsgHead;
	gsParaCfgMsg.cmd = NCMD_UPLOAD_CFG;
	
	//Alarm Lever
	gsParaCfgMsg.info_no = iOffset+1;
	switch(gCfgTemp.bAlmControl){
		case ALARM_LOW: gsParaCfgMsg.info_val = 2; break;
		case ALARM_MID: gsParaCfgMsg.info_val = 1; break;
		case ALARM_HIGH: gsParaCfgMsg.info_val = 0; break;
		default : gsParaCfgMsg.info_val = 3; break;
	}
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));

	//Temp2 High
	gsParaCfgMsg.info_no = iOffset+2;
	gsParaCfgMsg.info_val = gCfgTemp.wHigh_T2;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	
	//Temp2 Low
	gsParaCfgMsg.info_no = iOffset+3;
	gsParaCfgMsg.info_val = gCfgTemp.wLow_T2;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	
	return 0;
}

/*
	发送CO2 Info
*/
int NetSend_CO2Info()
{
	int iOffset = 700;
	
	gsParaCfgMsg.message_head = gsMsgHead;
	gsParaCfgMsg.cmd = NCMD_UPLOAD_CFG;
	
	//Alarm Lever
	gsParaCfgMsg.info_no = iOffset+1;
	switch(gCfgCO2.bAlmControl){
		case ALARM_LOW: gsParaCfgMsg.info_val = 2; break;
		case ALARM_MID: gsParaCfgMsg.info_val = 1; break;
		case ALARM_HIGH: gsParaCfgMsg.info_val = 0; break;
		default : gsParaCfgMsg.info_val = 3; break;
	}
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	
	//EtCO2 High
	gsParaCfgMsg.info_no = iOffset+2;
	gsParaCfgMsg.info_val = gCfgCO2.wHigh_EtCO2;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	
	//EtCO2 Low
	gsParaCfgMsg.info_no = iOffset+3;
	gsParaCfgMsg.info_val = gCfgCO2.wLow_EtCO2;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));

	//FiCO2 High
	gsParaCfgMsg.info_no = iOffset+4;
	gsParaCfgMsg.info_val = gCfgCO2.wHigh_FiCO2;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	
	//FiCO2 Low
	gsParaCfgMsg.info_no = iOffset+5;
	gsParaCfgMsg.info_val = gCfgCO2.wLow_FiCO2;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	
	
	//awRR High
	gsParaCfgMsg.info_no = iOffset+6;
	gsParaCfgMsg.info_val = gCfgCO2.wHigh_awRR;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	
	//EtCO2 Low
	gsParaCfgMsg.info_no = iOffset+7;
	gsParaCfgMsg.info_val = gCfgCO2.wLow_awRR;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));

	return 0;
}

/*
	发送IBP1 Info
*/
int NetSend_Ibp1Info()
{
	int iOffset = 800;
	
	gsParaCfgMsg.message_head = gsMsgHead;
	gsParaCfgMsg.cmd = NCMD_UPLOAD_CFG;
	
	//Alarm Lever
	gsParaCfgMsg.info_no = iOffset+1;
	switch(gCfgIbp1.bAlmControl){
		case ALARM_LOW: gsParaCfgMsg.info_val = 2; break;
		case ALARM_MID: gsParaCfgMsg.info_val = 1; break;
		case ALARM_HIGH: gsParaCfgMsg.info_val = 0; break;
		default : gsParaCfgMsg.info_val = 3; break;
	}
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	
	//Sys High
	gsParaCfgMsg.info_no = iOffset+2;
	gsParaCfgMsg.info_val = gCfgIbp1.iHigh_Sys;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));	
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));	
	
	//Sys Low
	gsParaCfgMsg.info_no = iOffset+3;
	gsParaCfgMsg.info_val = gCfgIbp1.iLow_Sys;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));	
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));	
	
	//Mean High
	gsParaCfgMsg.info_no = iOffset+4;
	gsParaCfgMsg.info_val = gCfgIbp1.iHigh_Mean;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));	
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));	
	
	//Mean Low
	gsParaCfgMsg.info_no = iOffset+5;
	gsParaCfgMsg.info_val = gCfgIbp1.iLow_Mean;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));	
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));	
	
	//Dia High
	gsParaCfgMsg.info_no = iOffset+6;
	gsParaCfgMsg.info_val = gCfgIbp1.iHigh_Dia;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));	
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));	
	
	//Dia Low
	gsParaCfgMsg.info_no = iOffset+7;
	gsParaCfgMsg.info_val = gCfgIbp1.iLow_Dia;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));	
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));	
	return 0;
}

/*
	发送IBP2 Info
*/
int NetSend_Ibp2Info()
{
	int iOffset = 900;
	
	gsParaCfgMsg.message_head = gsMsgHead;
	gsParaCfgMsg.cmd = NCMD_UPLOAD_CFG;
	
	//Alarm Lever
	gsParaCfgMsg.info_no = iOffset+1;
	switch(gCfgIbp2.bAlmControl){
		case ALARM_LOW: gsParaCfgMsg.info_val = 2; break;
		case ALARM_MID: gsParaCfgMsg.info_val = 1; break;
		case ALARM_HIGH: gsParaCfgMsg.info_val = 0; break;
		default : gsParaCfgMsg.info_val = 3; break;
	}
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));
	
	//Sys High
	gsParaCfgMsg.info_no = iOffset+2;
	gsParaCfgMsg.info_val = gCfgIbp2.iHigh_Sys;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));	
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));	
	
	//Sys Low
	gsParaCfgMsg.info_no = iOffset+3;
	gsParaCfgMsg.info_val = gCfgIbp2.iLow_Sys;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));	
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));	
	//Mean High
	gsParaCfgMsg.info_no = iOffset+4;
	gsParaCfgMsg.info_val = gCfgIbp2.iHigh_Mean;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));	
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));	
	
	//Mean Low
	gsParaCfgMsg.info_no = iOffset+5;
	gsParaCfgMsg.info_val = gCfgIbp2.iLow_Mean;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));	
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));	
	
	//Dia High
	gsParaCfgMsg.info_no = iOffset+6;
	gsParaCfgMsg.info_val = gCfgIbp2.iHigh_Dia;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));	
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));	
	
	//Dia Low
	gsParaCfgMsg.info_no = iOffset+7;
	gsParaCfgMsg.info_val = gCfgIbp2.iLow_Dia;
	gsParaCfgMsg.msg_tail[0] = gbPackTail[0];
	gsParaCfgMsg.msg_tail[1] =  gbPackTail[1];
	NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));	
	NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Info, &gsParaCfgMsg, sizeof(PARA_CFG_MSG));	
	
	return 0;
}


#define  CENTRAL_ALM_TEC		0		//技术报警
#define  CENTRAL_ALM_PHY		1		//生理报警

/*
	报警信息
*/
int NetSend_Alm_HR_Low()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx = 1;
	//报警状态
	gsCentralAlmMsg.alm_sta = 1;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_HR_High()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx = 1;
	//报警状态
	gsCentralAlmMsg.alm_sta = 2;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	return iLen;
}

int NetSend_Alm_RR_Low()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx = 6;
	//报警状态
	gsCentralAlmMsg.alm_sta = 1;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_RR_High()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx = 6;
	//报警状态
	gsCentralAlmMsg.alm_sta = 2;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_RR_Apnea()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx = 6;
	//报警状态
	gsCentralAlmMsg.alm_sta = 3;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_SpO2_Low()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx = 7;
	//报警状态
	gsCentralAlmMsg.alm_sta = 1;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_SpO2_High()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx = 7;
	//报警状态
	gsCentralAlmMsg.alm_sta = 2;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_PR_Low()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx = 8;
	//报警状态
	gsCentralAlmMsg.alm_sta = 1;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_PR_High()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx = 8;
	//报警状态
	gsCentralAlmMsg.alm_sta = 2;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_NSys_Low()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx = 9;
	//报警状态
	gsCentralAlmMsg.alm_sta = 1;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_NSys_High()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx = 9;
	//报警状态
	gsCentralAlmMsg.alm_sta = 2;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_NMean_Low()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx = 10;
	//报警状态
	gsCentralAlmMsg.alm_sta = 1;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_NMean_High()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx = 10;
	//报警状态
	gsCentralAlmMsg.alm_sta = 2;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_NDia_Low()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx = 11;
	//报警状态
	gsCentralAlmMsg.alm_sta = 1;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_NDia_High()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx = 11;
	//报警状态
	gsCentralAlmMsg.alm_sta = 2;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_Temp1_Low()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx = 12;
	//报警状态
	gsCentralAlmMsg.alm_sta = 1;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_Temp1_High()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx = 12;
	//报警状态
	gsCentralAlmMsg.alm_sta = 2;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_Temp2_Low()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx = 13;
	//报警状态
	gsCentralAlmMsg.alm_sta = 1;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_Temp2_High()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx = 13;
	//报警状态
	gsCentralAlmMsg.alm_sta = 2;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_TD_High()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx = 14;
	//报警状态
	gsCentralAlmMsg.alm_sta = 1;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_ISys1_Low()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx = 15;
	//报警状态
	gsCentralAlmMsg.alm_sta = 1;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_ISys1_High()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx = 15;
	//报警状态
	gsCentralAlmMsg.alm_sta = 2;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_IMean1_Low()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx = 16;
	//报警状态
	gsCentralAlmMsg.alm_sta = 1;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_IMean1_High()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx = 16;
	//报警状态
	gsCentralAlmMsg.alm_sta = 2;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_IDia1_Low()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx = 17;
	//报警状态
	gsCentralAlmMsg.alm_sta = 1;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_IDia1_High()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx = 17;
	//报警状态
	gsCentralAlmMsg.alm_sta = 2;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_ISys2_Low()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx = 18;
	//报警状态
	gsCentralAlmMsg.alm_sta = 1;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_ISys2_High()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx = 18;
	//报警状态
	gsCentralAlmMsg.alm_sta = 2;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_IMean2_Low()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx = 19;
	//报警状态
	gsCentralAlmMsg.alm_sta = 1;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_IMean2_High()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx = 19;
	//报警状态
	gsCentralAlmMsg.alm_sta = 2;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_IDia2_Low()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx = 20;
	//报警状态
	gsCentralAlmMsg.alm_sta = 1;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	

	return iLen;
}

int NetSend_Alm_IDia2_High()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx = 20;
	//报警状态
	gsCentralAlmMsg.alm_sta = 2;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_EtCO2_Low()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx =22;
	//报警状态
	gsCentralAlmMsg.alm_sta = 1;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_EtCO2_High()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx =22;
	//报警状态
	gsCentralAlmMsg.alm_sta = 2;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_FiCO2_High()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;

	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx =23;
	//报警状态
	gsCentralAlmMsg.alm_sta = 1;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	return iLen;
}

int NetSend_Alm_awRR_Low()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx =24;
	//报警状态
	gsCentralAlmMsg.alm_sta = 1;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	return iLen;
}

int NetSend_Alm_awRR_High()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx =24;
	//报警状态
	gsCentralAlmMsg.alm_sta = 2;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_awRR_Apnea()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_PHY;
	//报警索引
	gsCentralAlmMsg.alm_idx =24;
	//报警状态
	gsCentralAlmMsg.alm_sta = 3;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

//技术报警
int NetSend_Alm_RR_Off()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_TEC;
	//报警索引
	gsCentralAlmMsg.alm_idx =81;
	//报警状态
	gsCentralAlmMsg.alm_sta = 5;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_RA_Off()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_TEC;
	//报警索引
	gsCentralAlmMsg.alm_idx =81;
	//报警状态
	gsCentralAlmMsg.alm_sta = 5;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_LL_Off()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_TEC;
	//报警索引
	gsCentralAlmMsg.alm_idx =81;
	//报警状态
	gsCentralAlmMsg.alm_sta = 3;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	return iLen;
}

int NetSend_Alm_LA_Off()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_TEC;
	//报警索引
	gsCentralAlmMsg.alm_idx =81;
	//报警状态
	gsCentralAlmMsg.alm_sta = 4;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	return iLen;
}

int NetSend_Alm_V_Off()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_TEC;
	//报警索引
	gsCentralAlmMsg.alm_idx =81;
	//报警状态
	gsCentralAlmMsg.alm_sta = 2;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_Resp_Off()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_TEC;
	//报警索引
	gsCentralAlmMsg.alm_idx =83;
	//报警状态
	gsCentralAlmMsg.alm_sta = 1;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_SpO2_Off()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_TEC;
	//报警索引
	gsCentralAlmMsg.alm_idx =84;
	//报警状态
	gsCentralAlmMsg.alm_sta = 2;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_Nibp_LooseCuff()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_TEC;
	//报警索引
	gsCentralAlmMsg.alm_idx =85;
	//报警状态
	gsCentralAlmMsg.alm_sta = 1;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_Nibp_Weak()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_TEC;
	//报警索引
	gsCentralAlmMsg.alm_idx =85;
	//报警状态
	gsCentralAlmMsg.alm_sta = 4;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_Nibp_Over()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_TEC;
	//报警索引
	gsCentralAlmMsg.alm_idx =85;
	//报警状态
	gsCentralAlmMsg.alm_sta = 6;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_Nibp_Error()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_TEC;
	//报警索引
	gsCentralAlmMsg.alm_idx =85;
	//报警状态
	gsCentralAlmMsg.alm_sta = 0x0B;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_Nibp_CuffErr()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_TEC;
	//报警索引
	gsCentralAlmMsg.alm_idx =85;
	//报警状态
	gsCentralAlmMsg.alm_sta = 0x09;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_Nibp_Timeout()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_TEC;
	//报警索引
	gsCentralAlmMsg.alm_idx =85;
	//报警状态
	gsCentralAlmMsg.alm_sta = 0x0A;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_Temp1_Off()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_TEC;
	//报警索引
	gsCentralAlmMsg.alm_idx =86;
	//报警状态
	gsCentralAlmMsg.alm_sta = 1;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}

int NetSend_Alm_Temp2_Off()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_TEC;
	//报警索引
	gsCentralAlmMsg.alm_idx =87;
	//报警状态
	gsCentralAlmMsg.alm_sta = 1;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}


int NetSend_Alm_Battery_Low()
{
	int iLen;
	
	gsCentralAlmMsg.message_head = gsMsgHead;
	gsCentralAlmMsg.cmd = NCMD_ALARM;
	
	//报警类型
	gsCentralAlmMsg.alm_class = CENTRAL_ALM_TEC;
	//报警索引
	gsCentralAlmMsg.alm_idx =99;
	//报警状态
	gsCentralAlmMsg.alm_sta = 1;
	
	gsCentralAlmMsg.msg_tail[0] = gbPackTail[0];
	gsCentralAlmMsg.msg_tail[1] =  gbPackTail[1];
	
	iLen = NetSend(gfdMonitorSocket, gsCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	iLen = NetSend(gfdMonitorSocket, gsOtherCentralIP, giCentralPort_Alarm, &gsCentralAlmMsg, sizeof(CENTER_ALM_MSG));	
	
	return iLen;
}






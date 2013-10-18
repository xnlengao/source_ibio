/*************************************************************************** 
 *	Module Name:	GeneralFuncs.c	
 *
 *	Abstract:	一些通用函数
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *	
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "General_Funcs.h"
 sem_t semCalctime;


/**
	获得系统电源状态
*/
int GetPowerStatus()
{
	gPowerStatus.bShutDown = GetWorkStatus();
	gPowerStatus.bPowerType = GetPowerType();
	gPowerStatus.bBatteryStatus = GetBatteryStatus();
	
	//判断是否有报警
	IsLow_Power(&gPowerStatus);
	
	return 0;
}
int GetStringFromResFiles(const char* pResFile, const char* pSection, const char* pKey, char* pString, int iLen,const char* pDefultStr)
{
	char* tmpStr;
	char* strDelim="#";
	char* p;
	int iChar;
	int iL;
	int i, j;
	int res;

	
	if(iLen <=1){
		 if(B_PRINTF) printf("---------------------------------------%s:%d    Get String from file failure!!!!!!!!!", __FILE__, __LINE__);
		  return -1;
	} 

	
	res = GetValueFromEtcFile(pResFile, pSection, pKey, pString, iLen-1);	
	
	if(res<0)  {
		//if(B_PRINTF) printf("---------------------------------------%s:%d    Get String from file failure!!!!!!!!!", __FILE__, __LINE__);
		if(pDefultStr!=NULL)
		strncpy(pString,pDefultStr,iLen);
	}
	return iLen;
	
	tmpStr = malloc(iLen);
	
	
	if(tmpStr !=NULL){
		
		res = GetValueFromEtcFile(pResFile, pSection, pKey, tmpStr, iLen);	
		if(res <0){
			if(B_PRINTF) printf("%s:%d Get String Failure.\n", __FILE__, __LINE__);
			free(tmpStr);
			return -1;
		}
 		if(B_PRINTF) printf("\n(%s:%d)Get source string is %s. lenth is %d.\n", __FILE__, __LINE__, tmpStr, strlen(tmpStr));
		p = (char *)strtok(tmpStr, strDelim);
		iL= strlen(p);
		
		snprintf(pString, iL+1, "%s", p);
		
  		if(B_PRINTF) printf("p: %s -- %d    pvalue: %s -- %d\n", p, iL, pString, strlen(pString));
		while((p=(char *)strtok(NULL, strDelim))){ 
			if(isdigit(*p)){
				iChar = atoi(p);
  				if(B_PRINTF) printf("D-p: %s ---%d\n", p, iChar);
				pString[iL]= iChar;
				iL +=1;
			}
			else{
  				if(B_PRINTF) printf("C-p: %s ---%d, iL is %d.\n", p, strlen(p), iL);
				for(i=iL, j=0; i<(iL+strlen(p)+1); i++,j++){
					pString[i] = p[j];
  					if(B_PRINTF) printf("i(%d) %c -> %c\n", i, p[j], pString[i]);
				}
  				if(B_PRINTF) printf("%s --%d\n", pString, strlen(pString));
//  				iL += strlen(p)+2;
				iL += strlen(p);
			}
		}
//   		if(B_PRINTF) printf("\n\n");
		
		free(tmpStr);
	}
	return iLen;
}


/**
	从资源文件中提取并翻译字符串（针对某些特殊的欧洲字符）
	输入:	
	@param pResFile:	语言资源文件	
	@param pSection:字段名
	@param pKey:键值
	@param pString:获取的字符串
	@param 	iLen: 要获取的长度
	输出:
		实际获取的长度，－1：Error

*/
int GetStringFromResFile(const char* pResFile, const char* pSection, const char* pKey, char* pString, int iLen)
{
	char* tmpStr;
	char* strDelim="#";
	char* p;
	int iChar;
	int iL;
	int i, j;
	int res;

	
	if(iLen <=1){
		 if(B_PRINTF) printf("---------------------------------------%s:%d    Get String from file failure!!!!!!!!!", __FILE__, __LINE__);
		  return -1;
	} 

	
	res = GetValueFromEtcFile(pResFile, pSection, pKey, pString, iLen-1);	
	
	if(res<0)  {
		if(B_PRINTF) printf("---------------------------------------%s:%d    Get String from file failure!!!!!!!!!\n", __FILE__, __LINE__);
		
	}
	return iLen;
	
	tmpStr = malloc(iLen);
	
	
	if(tmpStr !=NULL){
		
		res = GetValueFromEtcFile(pResFile, pSection, pKey, tmpStr, iLen);	
		if(res <0){
			if(B_PRINTF) printf("%s:%d Get String Failure.\n", __FILE__, __LINE__);
			free(tmpStr);
			return -1;
		}
 		if(B_PRINTF) printf("\n(%s:%d)Get source string is %s. lenth is %d.\n", __FILE__, __LINE__, tmpStr, strlen(tmpStr));
		p = (char *)strtok(tmpStr, strDelim);
		iL= strlen(p);
		
		snprintf(pString, iL+1, "%s", p);
		
  		if(B_PRINTF) printf("p: %s -- %d    pvalue: %s -- %d\n", p, iL, pString, strlen(pString));
		while((p=(char *)strtok(NULL, strDelim))){ 
			if(isdigit(*p)){
				iChar = atoi(p);
  				if(B_PRINTF) printf("D-p: %s ---%d\n", p, iChar);
				pString[iL]= iChar;
				iL +=1;
			}
			else{
  				if(B_PRINTF) printf("C-p: %s ---%d, iL is %d.\n", p, strlen(p), iL);
				for(i=iL, j=0; i<(iL+strlen(p)+1); i++,j++){
					pString[i] = p[j];
  					if(B_PRINTF) printf("i(%d) %c -> %c\n", i, p[j], pString[i]);
				}
  				if(B_PRINTF) printf("%s --%d\n", pString, strlen(pString));
//  				iL += strlen(p)+2;
				iL += strlen(p);
			}
		}
//   		if(B_PRINTF) printf("\n\n");
		
		free(tmpStr);
	}
	return iLen;
}


//写资源文件的互斥量
static pthread_mutex_t mtWriteResFile;

/**
	初始化写文件的互斥量
*/
int InitWriteFileMutex()
{
	
	//初始化写资源文件的互斥量
	pthread_mutex_init(&mtWriteResFile, NULL);
	
	return 0;
}

/**
	把整数写入配置文件
	参数同上
*/
int SetIntValueToResFile(const char* pEtcFile, const char* pSection, const char* pKey, int iValue, int iLen)
{
	char *strValue;
	
	if((iLen <=0) || (iLen>10)) return 0;
	
	//加锁
	pthread_mutex_lock(&mtWriteResFile);
	
	strValue = malloc(iLen+1);
	
	if(strValue !=NULL){
		snprintf(strValue, iLen+1, "%d", iValue);	
		if(B_PRINTF) printf("Write %d -- %s, length is %d.\n", iValue, strValue, iLen);
		SetValueToEtcFile(pEtcFile, pSection, pKey, strValue);
		free(strValue);
		//解锁
		pthread_mutex_unlock(&mtWriteResFile);
		return iLen+1;
	}
	
	//解锁                 
	pthread_mutex_unlock(&mtWriteResFile);
	return 0;
}


/**
	创建指定大小的TTF字体
	输入:
	@param size : 要建立字体的大小
	输出:
 		TTF逻辑字体
*/
PLOGFONT CreateFont_TTF( int size)
{
	PLOGFONT  TTF_View;
	
	//此处根据需要可以修改字体类型
 	TTF_View = CreateLogFont(NULL, "Times", "ISO8859-1",
 				 FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
 				 FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
 				 size, 0);
	
	return TTF_View;
}


/**
	获得系统时间
	输入:
	@param bsTime: 时间
	输出: 
		0-OK, -1-Failure
*/
int GetSysTime(S_TIME *bstime)
{
	time_t	  t;
	struct tm   *tm;

	time(&t);
	tm = localtime(&t);
	if(bstime !=NULL){
		bstime->wYear = tm->tm_year +1900;
		bstime->bMonth = tm->tm_mon +1;
		bstime->bDay = tm->tm_mday;
		bstime->bHour = tm->tm_hour;
		bstime->bMin = tm->tm_min;
		bstime->bSec = tm->tm_sec;
	}
	else{
		//if(B_PRINTF) printf("time is null.\n");
		return -1;
	}
	
	return 0;
}


/**
	设置系统时间
	输入:	 
	@param bsTime : 时间
	输出:
		0-OK, -1-Failure	
*/
int SetSysTime(S_TIME bsTime)
{
	unsigned char code[21] = {0}	;
	
	//-- 设置软件时钟 格式:月日时分年.秒 --
       // date 082509362010.30
	snprintf(code, sizeof code, "date %.2d%.2d%.2d%.2d%.4d.%.2d",
			 bsTime.bMonth, bsTime.bDay, bsTime.bHour, bsTime.bMin, bsTime.wYear, bsTime.bSec);
	system(code);
	
	//-- 设置硬件时钟 -- 
	system("/sbin/hwclock -w");
	//system("/work/clock -w");
	//settimeofday(&tv, NULL);	
	
	return 0;	
}

/**
	自定义整数除法(四舍五入)
	输入:
	@param number: 
	@param denom:	
	输出:
		quot(商)	
*/
int B_Div(int number, int denom)
{
	div_t tmp;
	int res;
	
	tmp = div(number, denom);
		
	res = tmp.quot;
	if(tmp.rem >=5) res+=1;
	
	return res;
}

/**
    ==================================================
    Description:    计算源时间经过iInter秒后的时间desTime
            
    Parameters:                             method of calling:(r/w/rw/other)
    @param srcTime:源时间                                     r 
    @param desTime:目标时间                                    w       
    @param iInter: 时间间隔(秒)                                 r
    @param bType: 
	#define TIME_AFTER		0	//计算之后的时间
	#define TIME_BEFORE	1	//计算之前的时间
    return:iInter
    ===================================================
*/
long CalcTime(const S_TIME *srcTime, S_TIME *desTime, long int iInter, BYTE bType)
{
	time_t ttmSrc;
	time_t ttmDes;
	struct tm tmSrc;
	struct tm tmDes;
    
    //转换 SYSTIME为 tm
	tmSrc.tm_year = srcTime->wYear -1900;
	tmSrc.tm_mon = srcTime->bMonth - 1;
	tmSrc.tm_mday = srcTime->bDay; 
	tmSrc.tm_hour = srcTime->bHour;
	tmSrc.tm_min = srcTime->bMin;
	tmSrc.tm_sec = srcTime->bSec;
//      if(B_PRINTF) printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@SrcTime: %d-%d-%d %d:%d:%d\n", 
//           tmSrc.tm_year, tmSrc.tm_mon, tmSrc.tm_mday,
//           tmSrc.tm_hour, tmSrc.tm_min, tmSrc.tm_sec);
    //转换 tm 为 time_t
	ttmSrc = mktime(&tmSrc);
    //计算 (time_t) ttmDes    
	if(bType == TIME_AFTER)
		ttmDes = (time_t)(ttmSrc + iInter);     
	else
		ttmDes = (time_t)(ttmSrc - iInter);     
    //转换 time_t 为 tm
	tmDes = *localtime(&ttmDes);
   //   if(B_PRINTF) printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@DesTime: %d-%d-%d %d:%d:%d\n", 
  //         tmDes.tm_year, tmDes.tm_mon, tmDes.tm_mday,
  //         tmDes.tm_hour, tmDes.tm_min, tmDes.tm_sec);
        
	desTime->wYear = tmDes.tm_year+1900;
	desTime->bMonth = tmDes.tm_mon+1;
	desTime->bDay = tmDes.tm_mday;
	desTime->bHour = tmDes.tm_hour;
	desTime->bMin = tmDes.tm_min;
	desTime->bSec = tmDes.tm_sec;

	return iInter;  
}

long NewCalcTime(const S_TIME *srcTime, S_TIME *desTime, long int iInter)
{
	time_t ttmSrc;
	time_t ttmDes;
	struct tm tmSrc;
	struct tm tmDes;
    
    //转换 SYSTIME为 tm
	tmSrc.tm_year = srcTime->wYear -1900;
	tmSrc.tm_mon = srcTime->bMonth - 1;
	tmSrc.tm_mday = srcTime->bDay; 
	tmSrc.tm_hour = srcTime->bHour;
	tmSrc.tm_min = srcTime->bMin;
	tmSrc.tm_sec = srcTime->bSec;
//      if(B_PRINTF) printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@SrcTime: %d-%d-%d %d:%d:%d\n", 
//           tmSrc.tm_year, tmSrc.tm_mon, tmSrc.tm_mday,
//           tmSrc.tm_hour, tmSrc.tm_min, tmSrc.tm_sec);
    //转换 tm 为 time_t
	ttmSrc = mktime(&tmSrc);
    //计算 (time_t) ttmDes    
	ttmDes = (time_t)(ttmSrc + iInter);     
    //转换 time_t 为 tm
	tmDes = *localtime(&ttmDes);
//      if(B_PRINTF) printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@DesTime: %d-%d-%d %d:%d:%d\n", 
//           tmDes.tm_year, tmDes.tm_mon, tmDes.tm_mday,
//           tmDes.tm_hour, tmDes.tm_min, tmDes.tm_sec);
        
	desTime->wYear = tmDes.tm_year+1900;
	desTime->bMonth = tmDes.tm_mon+1;
	desTime->bDay = tmDes.tm_mday;
	desTime->bHour = tmDes.tm_hour;
	desTime->bMin = tmDes.tm_min;
	desTime->bSec = tmDes.tm_sec;

	return iInter;  
}


/*
    ==================================================
    Description:    比较时间
            
    Parameters:                             method of calling:(r/w/rw/other)
        srcTime:源时间                                     r 
        desTime:目标时间                                    r       
        
    return:
        0:  srcTime等于desTime
        <0:  srcTime早于desTime
        >0: srcTime晚于desTime
    ===================================================
*/
long CompTime(const S_TIME *srcTime, const S_TIME *desTime)
{
	sem_wait(&semCalctime);
	time_t ttmSrc;
	time_t ttmDes;
	struct tm tmSrc;
	struct tm tmDes;
	double fInter;      
    
    //转换 SYSTIME为 tm
	tmSrc.tm_year = srcTime->wYear -1900;
	tmSrc.tm_mon = srcTime->bMonth - 1;
	tmSrc.tm_mday = srcTime->bDay; 
	tmSrc.tm_hour = srcTime->bHour;
	tmSrc.tm_min = srcTime->bMin;
	tmSrc.tm_sec = srcTime->bSec;
//      if(B_PRINTF) printf("\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@SrcTime: %d-%d-%d %d:%d:%d\n", 
//           tmSrc.tm_year, tmSrc.tm_mon, tmSrc.tm_mday,
//           tmSrc.tm_hour, tmSrc.tm_min, tmSrc.tm_sec);
    
	tmDes.tm_year = desTime->wYear -1900;
	tmDes.tm_mon = desTime->bMonth - 1;
	tmDes.tm_mday = desTime->bDay; 
	tmDes.tm_hour = desTime->bHour;
	tmDes.tm_min = desTime->bMin;
	tmDes.tm_sec = desTime->bSec;
//      if(B_PRINTF) printf("\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@SrcTime: %d-%d-%d %d:%d:%d\n", 
//           tmDes.tm_year, tmDes.tm_mon, tmDes.tm_mday,
//           tmDes.tm_hour, tmDes.tm_min, tmDes.tm_sec);
    
    //转换 tm 为 time_t
	ttmSrc = mktime(&tmSrc);
	ttmDes = mktime(&tmDes);
    
    //比较 (time_t)
	fInter = difftime(ttmSrc, ttmDes);
     // if(B_PRINTF) printf("%s%d  SrcTime -> DesTime   %f.\n", __FILE__, __LINE__, fInter);   
    	sem_post(&semCalctime);
	return (long)fInter;
}       
static long DateToMins(const S_TIME *sTime)
{
	const long mon[]={0,			
					 31*24*60L,	//Jun
					 (28+31)*24*60L,	//Feb
					 (28+31+31)*24*60L,	//Mar
					 (28+31+31+30)*24*60L,	//Apr
					 (28+31+31+30+31)*24*60L,	//
					 (28+31+31+30+31+30)*24*60L,	//Jue
					 (28+31+31+30+31+30+31)*24*60L,	//Jul
					 (28+31+31+30+31+30+31+31)*24*60L,	//Aug
					 (28+31+31+30+31+30+31+31+30)*24*60L,	//Seb
					 (28+31+31+30+31+30+31+31+30+31)*24*60L,	//Oct
					 (28+31+31+30+31+30+31+31+30+31+30)*24*60,	//Nov
					 (28+31+31+30+31+30+31+31+30+31+30+31)*24*60	//Des
					};
	const  long day[]={ 0, 
					1*24*60L,
					2*24*60L,
					3*24*60L, 
					4*24*60L,
					5*24*60L,
					6*24*60L,
					7*24*60L,
					8*24*60L,
					9*24*60L,
					10*24*60L,
					11*24*60L,
					12*24*60L,
					13*24*60L,
					14*24*60L,
					15*24*60L,
					16*24*60L, 
					17*24*60L,
					18*24*60L,
					19*24*60L,
					20*24*60L,
					21*24*60L,
					22*24*60L,
					23*24*60L,
					24*24*60L,
					25*24*60L,
					26*24*60L,
					27*24*60L,
					28*24*60L,
					29*24*60L,
					30*24*60L,
					31*24*60L };
					
		
	long year, mins;
	year=sTime->wYear-1968;
	mins = year*365*24*60+24*60;
	mins+=(year/4)*( 24*60); //for leap year
	if( !(year&3L) )
	{
		if( sTime->bMonth<=2 ){
			mins -= (24*60);
		}
	}
	mins += mon[sTime->bMonth - 1];
	mins += day[sTime->bDay - 1];
	mins += sTime->bHour*60;
	mins += sTime->bMin;
	return mins;
}
long NewCompTime(const S_TIME *srcTime, const S_TIME *desTime)
{
	double fInter,fInter1;     
	long time1,time2;

	time1=DateToMins(srcTime);
	time2=DateToMins(desTime);
	
	fInter1 = time1-time2;
	return (long)abs(fInter1);
}

/*
	以分钟为分辨率比较时间
*/
long CompTimeWithMin(const S_TIME *srcTime, const S_TIME *desTime)
{
	time_t ttmSrc;
	time_t ttmDes;
	struct tm tmSrc;
	struct tm tmDes;
	double fInter,fInter1;     
	long time1,time2;
#if 1
	time1=DateToMins(srcTime);
	time2=DateToMins(desTime);
	
	fInter1 = time1-time2;
	return (long)fInter1;
#else	
 
    //转换 SYSTIME为 tm
	tmSrc.tm_year = srcTime->wYear -1900;
	tmSrc.tm_mon = srcTime->bMonth - 1;
	tmSrc.tm_mday = srcTime->bDay; 
	tmSrc.tm_hour = srcTime->bHour;
	tmSrc.tm_min = srcTime->bMin;
	tmSrc.tm_sec = 0;

	tmDes.tm_year = desTime->wYear -1900;
	tmDes.tm_mon = desTime->bMonth - 1;
	tmDes.tm_mday = desTime->bDay; 
	tmDes.tm_hour = desTime->bHour;
	tmDes.tm_min = desTime->bMin;
	tmDes.tm_sec = 0;

    //转换 tm 为 time_t
	ttmSrc = mktime(&tmSrc);
	ttmDes = mktime(&tmDes);
    
    //比较 (time_t)
	fInter = difftime(ttmSrc, ttmDes);
	//  if(B_PRINTF) printf("%s%d  SrcTime -> DesTime   %f ,,, %f. \n", __FILE__, __LINE__, fInter,fInter1);   

	return (long)fInter/60;
  #endif  
}

/*
	测试比较时间的函数
*/
void TestCompTimeFunc()
{
	S_TIME sTime1, sTime2;
	int iInter;
	static int iSec = 1;
	long time1,time2;
	sTime1.wYear = 1976;
	sTime1.bMonth = 5;
	sTime1.bDay = 6;
	sTime1.bHour = 12;
	sTime1.bMin = 4;
	sTime1.bSec = 0;
	
	sTime2.wYear = 2013;
	sTime2.bMonth = 12;
	sTime2.bDay = 4;
	sTime2.bHour = 2;
	sTime2.bMin = 0;
	sTime2.bSec = 0;
	
	//iInter = CompTimeWithMin(&sTime1, &sTime2);
	time1=DateToMins(&sTime1);
	time2=DateToMins(&sTime2);
	iInter = time2-time1;
	
	if(B_PRINTF) printf("Time1:%.2d:%.2d:%.2d  Time2:%.2d:%.2d:%.2d  L1:%ld, L2:%ld,i=%d.\n", 
		   sTime1.bHour, sTime1.bMin, sTime1.bSec, sTime2.bHour, sTime2.bMin, sTime2.bSec, time1,time2,iInter);

	iInter = CompTimeWithMin(&sTime2, &sTime1);
	printf("iInter:%d\n",iInter);
	
	if(iSec>0) iSec --;
	else iSec = 59;
		
	return ;
}

/*
	四舍五入取整
	使用atof转换字符串到浮点数时有误差,例如atof("39.1") 结果是39.099998
	所以采用如下办法转换:
	FloatToInt((atof("39.1") * 10.0));
*/
int FloatToInt(double fValue)
{
	int res;
	double intergral;
	double fractional;
	
	//分解整数和小数
	fractional = modf(fValue, &intergral);
	//判断小数大小
	if(fractional > 0.5)
		res = (int)fValue +1;
	else
		res = (int)fValue;
	
//  	if(B_PRINTF) printf("%s:%d intergral-->%f,  fractional-->%f.   convert %d.\n", __FILE__, __LINE__, intergral, fractional, res);
	return res;
}
/*
	退出所有对话框，回到主界面
*/
int ReturnMainView()
{
	gbKeyType = KEY_LR;
	BroadcastMessage(MSG_CLOSE,0,0);
	return 0;
}

/**
 * Bresenham Line Algorithm
 * @param iLineStyle 设置线型 REALWIDELINE or Value>0
 * @param iLineWidth 设置线宽。
 * @param x1 
 * @param y1
 * @param x2
 * @param y2
 * @param bType 绘制类型 WIDELINE_DOT, WIDELINE_RECT, WIDELINE_CYCLE
 */
static void  bresenhamLine(HDC hdc, int iLineStyle, int iLineWidth, int x1, int y1, int x2, int y2, BYTE bType)
{
	int  x, y;
	int  dx, dy;
	int  incx, incy;
	int  balance;
	int i=0;
	int j=0;
	
	if (x2 >= x1){
		dx = x2 - x1;
		incx = 1;
	}
	else{
		dx = x1 - x2;
		incx = -1;
	}

	if (y2 >= y1){
		dy = y2 - y1;
		incy = 1;
	}
	else{
		dy = y1 - y2;
		incy = -1;
	}

	x = x1;
	y = y1;

	if (dx >= dy){
		dy <<= 1;
		balance = dy - dx;
		dx <<= 1;

		while (x != x2){
			if ( (i & iLineStyle)== REALWIDELINE ){
				switch(bType){
					case WIDELINE_RECT:{
						Rectangle(hdc, x, y,x+iLineWidth, y+iLineWidth);
					}break;
					case WIDELINE_CYCLE:{
						Circle(hdc, x, y,iLineWidth);
					}break;
					default:{
 						for(j=0; j<=iLineWidth; j++){
 							SetPixel(hdc, x, y+j,GetPenColor(hdc));
 						}	
					}break;
				}
			}
				
			if (balance >= 0){
				y += incy;
				balance -= dx;
			}
			balance += dy;
			x += incx;
			i ++;
		}
		if ( (i & iLineStyle)== REALWIDELINE ){
			switch(bType){
				case WIDELINE_RECT:{
					Rectangle(hdc, x, y,x+iLineWidth, y+iLineWidth);
				}break;
				case WIDELINE_CYCLE:{
					Circle(hdc, x, y,iLineWidth);
				}break;
				default:{
					for(j=0; j<=iLineWidth; j++){
						SetPixel(hdc, x, y+j,GetPenColor(hdc));
					}
				}break;
			}
		}
	}
	else{
		dx <<= 1;
		balance = dx - dy;
		dy <<= 1;

		while (y != y2){
			if ( (i & iLineStyle)== REALWIDELINE ){
				switch(bType){
					case WIDELINE_RECT:{
						Rectangle(hdc, x, y,x+iLineWidth, y+iLineWidth);
					}break;
					case WIDELINE_CYCLE:{
						Circle(hdc, x, y,iLineWidth);
					}break;
					default:{
						for(j=0; j<=iLineWidth; j++){
							SetPixel(hdc, x, y+j,GetPenColor(hdc));
						}
					}break;
				}
			}
			if (balance >= 0){
				x += incx;
				balance -= dy;
			}
			balance += dx;
			y += incy;
			i ++;
		}
		if ( (i & iLineStyle)== REALWIDELINE ){
			switch(bType){
				case WIDELINE_RECT:{
					Rectangle(hdc, x, y,x+iLineWidth, y+iLineWidth);
				}break;
				case WIDELINE_CYCLE:{
					Circle(hdc, x, y,iLineWidth);
				}break;
				default:{
					for(j=0; j<=iLineWidth; j++){
						SetPixel(hdc, x, y+j,GetPenColor(hdc));
					}
				}break;
			}
		}
	}
}

//绘制宽线段
int DrawWideLine(HDC hdc, int iLineStyle, int iLineWidth, int x1, int y1, int x2, int y2, BYTE bType)
{
 	bresenhamLine(hdc, iLineStyle, iLineWidth, x1, y1, x2, y2,bType);
 	return 0;
}

//检查flash的文件是否正确
int CheckFlashFiles()
{
	//检查是否有语言资源文件

	return 0;
}


//趋势数组
extern CA_TRENDARRAY gTrendDatas;
//主窗口句柄
extern HWND ghWndMain;

/*
	重启计算机
*/
int SystemRestart()
{
	
	//把当前趋势存储在Flash
	SaveTrendToFlash(&gTrendDatas);
//	PowerDialog(ghWndMain, NULL, 0, TRUE);
	system("reboot");
	return 0;
}


/*
*       -- 更改坐标系 --
*       ViewOrg_X, ViewOrg_Y: 视图原点; ViewExt_X, ViewExt_Y:  视图范围;
*       WindOrg_X, WindOrg_Y: 视图原点; WindExt_X, WindExt_Y: 窗口范围
*/
void  ChangeZBX(HDC hdc, int ViewOrg_X, int ViewOrg_Y, int ViewExt_X, int ViewExt_Y, int WindOrg_X, int WindOrg_Y, int WindExt_X, int WindExt_Y)
{
        POINT Pt_ViewOrg;
        POINT Pt_ViewExt;
        POINT Pt_WindExt;
        POINT Pt_WindOrg;
        
        
        Pt_ViewOrg.x = ViewOrg_X;
        Pt_ViewOrg.y = ViewOrg_Y;
        Pt_ViewExt.x = ViewExt_X;
        Pt_ViewExt.y = ViewExt_Y;
        Pt_WindOrg.x = WindOrg_X;
        Pt_WindOrg.y = WindOrg_Y;
        Pt_WindExt.x = WindExt_X;
        Pt_WindExt.y = WindExt_Y;
        
        SetMapMode(hdc, MM_ANISOTROPIC);
        SetViewportOrg(hdc, &Pt_ViewOrg);
        SetViewportExt(hdc, &Pt_ViewExt);
        SetWindowExt(hdc, &Pt_WindExt);
        SetWindowOrg(hdc, &Pt_WindOrg );
}

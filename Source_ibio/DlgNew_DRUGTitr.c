/*************************************************************************** 
 *	Module Name:	DlgNew_DRUGTitration
 *
 *	Abstract:滴定表显示
 *
 *	Revision History:
 *	Who		When		What
 *	--------	----------	-----------------------------
 *	Name		Date		Modification logs
 *			2012-12-19 14:14:18
 *	
 ***************************************************************************/
#include "IncludeFiles.h"
#include "DataStruct.h"
#include "Global.h"
#include "Dialog.h"

//DID_DRUGTITR
#define DID_DRUGTITR_HELP			DID_DRUGTITR
#define DID_DRUGTITR_CAPTION			DID_DRUGTITR+1
#define DID_DRUGTITR_OK				DID_DRUGTITR+2
#define DID_DRUGTITR_UD				DID_DRUGTITR+3
#define DID_DRUGTITR_DOSE			DID_DRUGTITR+4
#define DID_DRUGTITR_DOSE_COB		DID_DRUGTITR+5
#define DID_DRUGTITR_DOSETYPE		DID_DRUGTITR+6
#define DID_DRUGTITR_DOSETYPE_COB		DID_DRUGTITR+7
#define DID_DRUGTITR_STEP			DID_DRUGTITR+8
#define DID_DRUGTITR_STEP_COB		DID_DRUGTITR+9
#define DID_DRUGTITR_PARALAY		DID_DRUGTITR+10
#define DID_DRUGTITR_TIRALAY			DID_DRUGTITR+11
#define DID_DRUGTITR_RECORD			DID_DRUGTITR+12
#define DID_DRUGTITR_GAMOUNT			DID_DRUGTITR+15
#define DID_DRUGTITR_GVOLUME		DID_DRUGTITR+20
#define DID_DRUGTITR_GWEIGHT		DID_DRUGTITR+25
#define DID_DRUGTITR_GDOSETYPE		DID_DRUGTITR+30
#define DID_DRUGTITR_GINFRATE		DID_DRUGTITR+35
#define DID_DRUGTITR_GDRIPRATE		DID_DRUGTITR+40
#define DID_DRUGTITR_TITRHEAD		DID_DRUGTITR+45

//-----------药物单位--------------------
#define	DRUG_UNIT_MCG		0x00
#define	DRUG_UNIT_MG		0x01
#define	DRUG_UNIT_G		0x02
#define	DRUG_UNIT_MEQ		0x03
#define	DRUG_UNIT_U		0x04
#define	DRUG_UNIT_KU		0x05
#define	DRUG_UNIT_MU		0x06
#define	DRUG_UNIT_MLHR	0x07
#define	DRUG_UNIT_GTTMIN	0x08
#define	DRUG_UNIT_GTTML	0x0a
#define	DRUG_UNIT_HR		0x0b
#define	DRUG_UNIT_ML		0x0c
#define	DRUG_UNIT_KG		0x0d
#define	DRUG_UNIT_MSK		0xc0

#define	DRUG_DATA_AMOUNT		0
#define	DRUG_DATA_VOLUME		1
#define	DRUG_DATA_CONCEN		2
#define	DRUG_DATA_DOSEMIN		3
#define	DRUG_DATA_DOSEHR		4
#define	DRUG_DATA_DOSEKGMIN	5
#define	DRUG_DATA_DOSEKGHR	6
#define	DRUG_DATA_INFRATE		7
#define	DRUG_DATA_DRIPRATE	8
#define	DRUG_DATA_DRIPSIZE		9
#define	DRUG_DATA_DURATION	10

//------------滴定表---------------------
#define	V_TITR_DOSE		0
#define	V_TITR_INFRATE		1
#define	V_TITR_DRIPRATE		2

#define	V_TITR_DOSE_MIN	0
#define	V_TITR_DOSE_HR		1
#define	V_TITR_DOSE_KGMIN	2
#define	V_TITR_DOSE_KGHR	3

#define COLWIDTH 75
#define ROWWIDTH 20
//帮助对话框
static HWND editHelp;
static HWND editCaption;
static WNDPROC OldEditProcHelp;

//其他控件 
static HWND btnOk;
static HWND btnUD;
static HWND btnRecord;
static HWND stcDose;
static HWND cobDose;
static HWND stcDoseType;
static HWND cobDoseType;
static HWND stcStep;
static HWND cobStep;
static HWND stcParaLayout;
static HWND stcTitrLayout;
static HWND stcTitrHead[6];

//控件回调函数
static WNDPROC OldBtnProc;
static WNDPROC OldCobProc;

typedef struct {
	HWND hwndName;
	HWND hwndValue;
	HWND hwndUnit;
	RECT rcName;		//参数名称区域
	RECT rcValue;		//参数值区域
	RECT rcUnit;		//参数单位区域
	char  Name[50];  	//参数名称
	float value;		//参数值
	char strValue[50];
	char Unit[50];		//参数单位
}TITRPARA,*TITR_PARA;

TITRPARA gAmount,gDose,gTWeight,gVolume,gInfRate,gDripRate;

extern float  gWeight;
extern float gDrugData[11];
extern char gDrugUnit[11];
extern void Show_Drug_Unit( HWND hWnd, int unittype );
extern void Drug_Unit_To_String( char *pStr, int unittype );

//参数显示区域
static RECT gRcPara;
HWND ghWnd;
//滴定表显示区域
static RECT gRcTitr;
//滴定表表头显示区域
static RECT rcTitrHead[6];
static RECT rcTitrData[6];

static BOOL bCobEnter= FALSE;
static BOOL bBtnBrowse= FALSE;

static unsigned short	s_Titr_Page = 0;	//滴定表

typedef	struct _TITRDATA
{
	float	 dose;
	float	 other;
}TITRDATA, *LPTITRDATA;
#define TITR_LIST_PRT_LEN	30
TITRDATA	buff[TITR_LIST_PRT_LEN];

//对话框属性
static DLGTEMPLATE DlgSet= {
	//WS_VISIBLE | WS_CAPTION | WS_BORDER,
	WS_VISIBLE | WS_BORDER,
	WS_EX_NOCLOSEBOX,	
	0, DLG_Y, DLG_LENGHT+150, DLG_HIGH,	
	"",
	0, 0,
	1,		
	NULL,
	0
};

static CTRLDATA CtrlSet[] = {
	{
		"static",
		WS_CHILD | WS_VISIBLE | SS_LEFT ,
		//WS_CHILD | WS_VISIBLE | SS_LEFT|SS_WHITERECT ,
		5,338, 385, 25,
		DID_DRUGTITR_HELP,
		"",
		0
	}
};


void	 Format_Titr_Val( char* str, float val, float max, float min )
{
	
	if( val < min || val > max )
		strcpy( str, "---");
	else
		sprintf( str, "%-7.2f", val);
	
}
void  Titr_Unit_Conversion( float *conv, int baseunit, int convunit )
{
	baseunit &= (~DRUG_UNIT_MSK);
	convunit &= (~DRUG_UNIT_MSK);
	if( baseunit != convunit ){
		switch( baseunit )
		{
		case	DRUG_UNIT_MCG:	//mcg
			switch( convunit)
			{
			case	DRUG_UNIT_MG:		//mg
				*conv *= 1000.0;	//mg->mcg
				break;
			default:break;
			}
			break;
		case	DRUG_UNIT_MG:	//mg
			switch( convunit)
			{
			case	DRUG_UNIT_MCG:		//mcg
				*conv /= 1000.0;	//mcg->mg
				break;
			case	DRUG_UNIT_G:		//g
				*conv *= 1000.0;	//g->mg
				break;
			default:break;
			}
			break;
		case	DRUG_UNIT_G:	//g
			switch( convunit)
			{
			case	DRUG_UNIT_MG:		//mg
				*conv /= 1000.0;	//mg->g
				break;
			default:break;
			}
			break;
		case	DRUG_UNIT_U:	//Unit
			switch( convunit)
			{
			case	DRUG_UNIT_KU:		//KU
				*conv *= 1000.0;	//KU->Unit
				break;
			default:break;
			}
			break;
		case	DRUG_UNIT_KU:	//KU
			switch( convunit)
			{
			case	DRUG_UNIT_U:		//Unit
				*conv /= 1000.0;	//Unit->KU
				break;
			case	DRUG_UNIT_MU:		//MU
				*conv *= 1000.0;	//MU->KU
				break;
			default:break;
			}
			break;
		case	DRUG_UNIT_MU:	//MU
			switch( convunit)
			{
			case	DRUG_UNIT_KU:		//KU
				*conv /= 1000.0;	//KU->MU
				break;
			default:break;
			}
			break;
		default:break;
		}
	}
}

/////////////////////////////////////////////////
//	剂量/min 基准 求 输液速度
//	算法：输液速度 = 剂量/min × 60 ÷ 药物浓度
/////////////////////////////////////////////////
float  Titr_DoseMin_To_INFrate( float base )
{
	float fv;
	fv = gDrugData[DRUG_DATA_CONCEN];
	Titr_Unit_Conversion( &fv, gDrugUnit[DRUG_DATA_DOSEMIN], gDrugUnit[DRUG_DATA_CONCEN] );
	fv = base * 60.0 / fv;
	return fv;	//INFrate
}
/////////////////////////////////////////////////
//	剂量/hr 基准 求 输液速度
//	算法：输液速度 = 剂量/hr ÷ 药物浓度
/////////////////////////////////////////////////
float  Titr_DoseHr_To_INFrate( float base )
{
	float fv;

	fv = gDrugData[DRUG_DATA_CONCEN];
	Titr_Unit_Conversion( &fv, gDrugUnit[DRUG_DATA_DOSEHR], gDrugUnit[DRUG_DATA_CONCEN] );
	fv = base / fv;
	return fv;	//INFrate
}



/////////////////////////////////////////////////
//	剂量/kg/min 基准 求 输液速度
//	算法：输液速度 = 剂量/kg/min × (病人体重) × 60 ÷ 药物浓度
/////////////////////////////////////////////////
float  Titr_DoseKgMin_To_INFrate( float base )
{
	float fv;
	float m_fWeight;
	m_fWeight=(float)gWeight/100;
	fv = gDrugData[DRUG_DATA_CONCEN];
	Titr_Unit_Conversion( &fv, gDrugUnit[DRUG_DATA_DOSEKGMIN], gDrugUnit[DRUG_DATA_CONCEN] );
	fv = base * m_fWeight * 60 / fv;
	return fv;	//INFrate
}

/////////////////////////////////////////////////
//	剂量/kg/hr 基准 求 输液速度
//	算法：输液速度 = 剂量/kg/hr × (病人体重) ÷ 药物浓度
/////////////////////////////////////////////////
float  Titr_DoseKgHr_To_INFrate( float base )
{
	float fv;
	float m_fWeight;
	m_fWeight=(float)gWeight/100;
	fv = gDrugData[DRUG_DATA_CONCEN];
	Titr_Unit_Conversion( &fv, gDrugUnit[DRUG_DATA_DOSEKGHR], gDrugUnit[DRUG_DATA_CONCEN] );
	fv = base * m_fWeight / fv ;
	return fv;	//INFrate
}

/////////////////////////////////////////////////
//	输液速度 基准 求 剂量/min
//	算法：剂量/min = 输液速度 × 药物浓度 ÷ 60
/////////////////////////////////////////////////
float  Titr_INFrate_To_DoseMin( float base )
{
	float fv;
	fv = gDrugData[DRUG_DATA_CONCEN];
	Titr_Unit_Conversion( &fv, gDrugUnit[DRUG_DATA_DOSEMIN], gDrugUnit[DRUG_DATA_CONCEN] );
	fv = base*fv/60;
	return fv;// DOSE/min
}
/////////////////////////////////////////////////
//	 输液速度 基准 求 剂量/hr
//	算法：剂量/hr = 输液速度 × 药物浓度
/////////////////////////////////////////////////
float  Titr_INFrate_To_DoseHr( float base )
{
	float fv;
	fv = gDrugData[DRUG_DATA_CONCEN];
	Titr_Unit_Conversion( &fv, gDrugUnit[DRUG_DATA_DOSEHR], gDrugUnit[DRUG_DATA_CONCEN] );
	fv = base*fv;
	return fv;// DOSE/hr
}
/////////////////////////////////////////////////
//	输液速度 基准 求 剂量/kg/min
//	算法：剂量/kg/min = 输液速度 × 药物浓度 ÷ 60 ÷ (病人体重)
/////////////////////////////////////////////////
float  Titr_INFrate_To_DoseKgMin( float base )
{
	float fv;
	float m_fWeight;
	m_fWeight=(float)gWeight/100;
	fv = gDrugData[DRUG_DATA_CONCEN];
	Titr_Unit_Conversion( &fv, gDrugUnit[DRUG_DATA_DOSEKGMIN], gDrugUnit[DRUG_DATA_CONCEN] );
	fv = base*fv/m_fWeight/60.0;
	return fv;// DOSE/kg/min
}
/////////////////////////////////////////////////
//	输液速度 基准 求 剂量/kg/hr
//	算法：剂量/kg/hr = 输液速度 × 药物浓度 ÷ (病人体重)
/////////////////////////////////////////////////
float  Titr_INFrate_To_DoseKgHr( float base )
{
	float fv;
	float m_fWeight;
	m_fWeight=(float)gWeight/100;
	fv = gDrugData[DRUG_DATA_CONCEN];
	Titr_Unit_Conversion( &fv, gDrugUnit[DRUG_DATA_DOSEKGHR], gDrugUnit[DRUG_DATA_CONCEN] );
	fv = base*fv/m_fWeight;
	return fv;// DOSE/kg/hr
}
/////////////////////////////////////////////////
//	剂量/min 基准 求 滴速
//	算法：滴速 = 剂量/min × 每滴体积 ÷ 药物浓度
/////////////////////////////////////////////////
float  Titr_DoseMin_To_DripRate( float base )
{
	float fv;
	fv = gDrugData[DRUG_DATA_CONCEN];
	Titr_Unit_Conversion( &fv, gDrugUnit[DRUG_DATA_DOSEMIN], gDrugUnit[DRUG_DATA_CONCEN] );
	fv = base*gDrugData[DRUG_DATA_DRIPSIZE]/fv;
	return fv;//drip rate
}
/////////////////////////////////////////////////
//	剂量/hr 基准 求 滴速
//	算法：滴速 = 剂量/hr × 每滴体积 ÷ 药物浓度 ÷ 60
/////////////////////////////////////////////////
float  Titr_DoseHr_To_DripRate( float base )
{
	float fv;
	fv = gDrugData[DRUG_DATA_CONCEN];
	Titr_Unit_Conversion( &fv, gDrugUnit[DRUG_DATA_DOSEHR], gDrugUnit[DRUG_DATA_CONCEN] );
	fv = base*gDrugData[DRUG_DATA_DRIPSIZE]/fv/60.0;
	return fv;//drip rate
}
/////////////////////////////////////////////////
//	剂量/kg/min 基准 求 滴速
//	算法：滴速 = 剂量/kg/min × 每滴体积 × (病人体重) ÷ 药物浓度
/////////////////////////////////////////////////
float  Titr_DoseKgMin_To_DripRate( float base )
{
	float fv;
	
	float m_fWeight;
	m_fWeight=(float)gWeight/100;
	fv = gDrugData[DRUG_DATA_CONCEN];
	Titr_Unit_Conversion( &fv, gDrugUnit[DRUG_DATA_DOSEKGMIN], gDrugUnit[DRUG_DATA_CONCEN] );
	fv = base*gDrugData[DRUG_DATA_DRIPSIZE]*m_fWeight/fv;
	return fv;//drip rate
}
/////////////////////////////////////////////////
//	剂量/kg/hr 基准 求 滴速
//	算法：滴速 = 剂量/kg/hr × 每滴体积 × (病人体重) ÷ 药物浓度 ÷ 60
/////////////////////////////////////////////////
float  Titr_DoseKgHr_To_DripRate( float base )
{
	float fv;
	float m_fWeight;
	m_fWeight=(float)gWeight/100;
	fv = gDrugData[DRUG_DATA_CONCEN];
	Titr_Unit_Conversion( &fv, gDrugUnit[DRUG_DATA_DOSEKGHR], gDrugUnit[DRUG_DATA_CONCEN] );
	fv = base*gDrugData[DRUG_DATA_DRIPSIZE]*m_fWeight/fv/60.0;
	return fv;//drip rate
}
/////////////////////////////////////////////////
//	滴速 基准 求 剂量/min
//	算法：剂量/min = 滴速 × 药物浓度 ÷ 每滴体积
/////////////////////////////////////////////////
float  Titr_DripRate_To_DoseMin( float base )
{
	float fv;
	fv = gDrugData[DRUG_DATA_CONCEN];
	Titr_Unit_Conversion( &fv, gDrugUnit[DRUG_DATA_DOSEMIN], gDrugUnit[DRUG_DATA_CONCEN] );
	fv = base*fv/gDrugData[DRUG_DATA_DRIPSIZE];
	return fv;//dose/min
}
/////////////////////////////////////////////////
//	 滴速 基准 求 剂量/hr
//	算法：剂量/hr = 滴速 × 药物浓度 × 60 ÷ 每滴体积
/////////////////////////////////////////////////
float  Titr_DripRate_To_DoseHr( float base )
{
	float fv;
	fv = gDrugData[DRUG_DATA_CONCEN];
	Titr_Unit_Conversion( &fv, gDrugUnit[DRUG_DATA_DOSEHR], gDrugUnit[DRUG_DATA_CONCEN] );
	fv = base*fv*60.0/gDrugData[DRUG_DATA_DRIPSIZE];
	return fv;//dose/hr
}
/////////////////////////////////////////////////
//	滴速 基准 求 剂量/kg/min
//	算法：剂量/kg/min = 滴速 × 药物浓度 ÷ (病人体重) ÷ 每滴体积
/////////////////////////////////////////////////
float  Titr_DripRate_To_DoseKgMin( float base )
{
	float fv;
	float m_fWeight;
	m_fWeight=(float)gWeight/100;
	fv = gDrugData[DRUG_DATA_CONCEN];
	Titr_Unit_Conversion( &fv, gDrugUnit[DRUG_DATA_DOSEKGMIN], gDrugUnit[DRUG_DATA_CONCEN] );
	fv = base*fv/m_fWeight/gDrugData[DRUG_DATA_DRIPSIZE];
	return fv;//dose/kg/min
}
/////////////////////////////////////////////////
//	滴速 基准 求 剂量/kg/hr
//	算法：剂量/kg/hr = 滴速 × 药物浓度 × 60 ÷ (病人体重) ÷ 每滴体积
/////////////////////////////////////////////////
float  Titr_DripRate_To_DoseKgHr( float base )
{
	float fv;
	float m_fWeight;
	m_fWeight=(float)gWeight/100;
	fv = gDrugData[DRUG_DATA_CONCEN];
	Titr_Unit_Conversion( &fv, gDrugUnit[DRUG_DATA_DOSEKGHR], gDrugUnit[DRUG_DATA_CONCEN] );
	fv = base*fv*60.0/m_fWeight/gDrugData[DRUG_DATA_DRIPSIZE];
	return fv;//dose/kg/hr
}
//计算滴定表
void	 Cacl_Titr_Table( int page, int step, int base, int dosetype )
{

	int		i;
	int		j;
	int		infdrip;
	
	page = page*TITR_LIST_PRT_LEN*step;

	if(base==V_TITR_DRIPRATE)
		infdrip = V_TITR_DRIPRATE;
	else
		infdrip = V_TITR_INFRATE;

	switch( base )
	{
	case	V_TITR_DOSE:
		if( page<10000 ){
			for( i=0; i<TITR_LIST_PRT_LEN; i++ ){
				j = page+i*step;
				if( j<10000 ){
					buff[i].dose = (float)j;
					
					switch( dosetype )
					{
					case	V_TITR_DOSE_MIN:
						if( infdrip==V_TITR_INFRATE )
							buff[i].other = Titr_DoseMin_To_INFrate( buff[i].dose );
						else
							buff[i].other = Titr_DoseMin_To_DripRate( buff[i].dose );
						break;
					case	V_TITR_DOSE_HR:
						if( infdrip==V_TITR_INFRATE )
							buff[i].other = Titr_DoseHr_To_INFrate( buff[i].dose );
						else
							buff[i].other = Titr_DoseHr_To_DripRate(buff[i].dose );
						break;
					case	V_TITR_DOSE_KGMIN:
						if( infdrip==V_TITR_INFRATE )
							buff[i].other = Titr_DoseKgMin_To_INFrate( buff[i].dose );
						else
							buff[i].other = Titr_DoseKgMin_To_DripRate( buff[i].dose );
						break;
					case	V_TITR_DOSE_KGHR:
						if( infdrip==V_TITR_INFRATE )
							buff[i].other = Titr_DoseKgHr_To_INFrate( buff[i].dose );
						else
							buff[i].other = Titr_DoseKgHr_To_DripRate( buff[i].dose );
						break;
					default:buff[i].other = 10000.0;
						break;
					}
					
				}else{
					buff[i].dose =buff[i].other = 10000.0;
				}
			}
		}
		break;
	case	V_TITR_INFRATE:
		if( page < 1000 ){
			for( i=0; i<TITR_LIST_PRT_LEN; i++ ){
				j = page + i*step;
				if( j<1000 ){
					buff[i].other = (float)(j);
					switch( dosetype )
					{
					case	V_TITR_DOSE_MIN:
						buff[i].dose = Titr_INFrate_To_DoseMin( buff[i].other );
						break;
					case	V_TITR_DOSE_HR:
						buff[i].dose = Titr_INFrate_To_DoseHr(buff[i].other );
						break;
					case	V_TITR_DOSE_KGMIN:
						buff[i].dose = Titr_INFrate_To_DoseKgMin( buff[i].other );
						break;
					case	V_TITR_DOSE_KGHR:
						buff[i].dose = Titr_INFrate_To_DoseKgHr(buff[i].other );
						break;
					default:buff[i].dose = 10000.0;
					}
				}else{
					buff[i].dose = buff[i].other = 10000.0;
				}
			}
		}
		break;
	case	V_TITR_DRIPRATE:
		if( page < 1000 ){
			for( i=0; i<TITR_LIST_PRT_LEN; i++ ){
				j = page + i*step;
				if( j<1000 ){
					buff[i].other = (float)(j);
					switch( dosetype )
					{
					case	V_TITR_DOSE_MIN:
						buff[i].dose = Titr_DripRate_To_DoseMin( buff[i].other );
						break;
					case	V_TITR_DOSE_HR:
						buff[i].dose = Titr_DripRate_To_DoseHr( buff[i].other );
						break;
					case	V_TITR_DOSE_KGMIN:
						buff[i].dose = Titr_DripRate_To_DoseKgMin( buff[i].other );
						break;
					case	V_TITR_DOSE_KGHR:
						buff[i].dose = Titr_DripRate_To_DoseKgHr(buff[i].other );
						break;
					default:buff[i].dose = 10000.0;
					}
				}else{
					buff[i].dose = buff[i].other = 10000.0;
				}
			}
		}
		break;
	default:break;
	}

}

//显示滴定表
void	  Show_Titr_Table(HWND hDlg)
{
	HDC hdc;
	RECT rcDose;
	RECT rcOther;
	char m_buff[12];
	
	int		i;
	int		j;
	int		point;
	point = 0;
	hdc = GetClientDC(hDlg);
		if(hdc == (HDC)NULL) return ;
		
	SetBkColor(hdc, COLOR_lightgray);
	SetBrushColor(hdc, COLOR_lightgray);
	
	for( i=0; i<TITR_LIST_PRT_LEN/10; i++ ){
		SetRect(&(rcDose),rcTitrData[i*2].left, rcTitrData[i*2].top, rcTitrData[i*2].right, rcTitrData[i*2].top+ (RECTH(rcTitrData[i*2])/10));
		SetRect(&(rcOther),rcTitrData[i*2+1].left, rcTitrData[i*2+1].top, rcTitrData[i*2+1].right, rcTitrData[i*2+1].top+ (RECTH(rcTitrData[i*2+1])/10));
	//	if(B_PRINTF) printf("dose[%d] left,=%d,top=%d,right=%d,bottom=%d\n",i,rcDose.left,rcDose.top,rcDose.right,rcDose.bottom);
	//	if(B_PRINTF) printf("other[%d] left,=%d,top=%d,right=%d,bottom=%d\n",i,rcOther.left,rcOther.top,rcOther.right,rcOther.bottom);
		for( j=0; j<TITR_LIST_PRT_LEN/3; j++ ){
			Format_Titr_Val( m_buff, buff[point].dose, 9999.9949, 0.0 );
			FillBox(hdc, rcDose.left+2, rcDose.top+2, RECTW(rcDose)-4, RECTH(rcDose)-2);
			DrawText(hdc, m_buff, -1, &rcDose, 
				DT_NOCLIP | DT_SINGLELINE | DT_VCENTER | DT_CENTER );
			
			Format_Titr_Val( m_buff, buff[point].other, 999.9949, 0.0 );
			FillBox(hdc, rcOther.left+2, rcOther.top+2, RECTW(rcOther)-4, RECTH(rcOther)-2);
			DrawText(hdc, m_buff, -1, &rcOther, 
				DT_NOCLIP | DT_SINGLELINE | DT_VCENTER | DT_CENTER );
			point++;
			
			SetRect(&(rcDose),rcDose.left, rcDose.top+(RECTH(rcTitrData[i+1])/10), rcDose.right, rcDose.bottom+(RECTH(rcTitrData[i+1])/10));
			SetRect(&(rcOther),rcOther.left, rcOther.top+(RECTH(rcTitrData[i+1])/10), rcOther.right, rcOther.bottom+(RECTH(rcTitrData[i+1])/10));
			
		}
		
	}
	
	ReleaseDC(hdc);
}

/*更改dosetype 的显示*/
void Show_Titr_Dose_Data(int index)
{
	
	switch(index)
	{
		case	V_TITR_DOSE_MIN:	
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUGTITR, "DoseType", gDose.Name, sizeof gDose.Name,"Dose/min");
			SetWindowText(gDose.hwndName,gDose.Name);
			gDose.value=gDrugData[DRUG_DATA_DOSEMIN];
			snprintf(gDose.strValue, sizeof(gDose.strValue),"%.2f", gDose.value);
			SetWindowText(gDose.hwndValue,gDose.strValue);
			Show_Drug_Unit( gDose.hwndUnit,gDrugUnit[DRUG_DATA_DOSEMIN] );
			break;
		case	V_TITR_DOSE_HR:	
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUGTITR, "DoseType", gDose.Name, sizeof gDose.Name,"Dose/hr");
			SetWindowText(gDose.hwndName,gDose.Name);
			gDose.value=gDrugData[DRUG_DATA_DOSEHR];
			snprintf(gDose.strValue, sizeof(gDose.strValue),"%.2f", gDose.value);
			SetWindowText(gDose.hwndValue,gDose.strValue);
			Show_Drug_Unit( gDose.hwndUnit,gDrugUnit[DRUG_DATA_DOSEHR] );
			break;
		case	V_TITR_DOSE_KGMIN:
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUGTITR, "DoseType", gDose.Name, sizeof gDose.Name,"Dose/kg/min");
			SetWindowText(gDose.hwndName,gDose.Name);
			gDose.value=gDrugData[DRUG_DATA_DOSEKGMIN];
			snprintf(gDose.strValue, sizeof(gDose.strValue),"%.2f", gDose.value);
			SetWindowText(gDose.hwndValue,gDose.strValue);
			Show_Drug_Unit( gDose.hwndUnit,gDrugUnit[DRUG_DATA_DOSEKGMIN] );
			break;
		case	V_TITR_DOSE_KGHR:
			GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUGTITR, "DoseType", gDose.Name, sizeof gDose.Name,"Dose/kg/hr");
			SetWindowText(gDose.hwndName,gDose.Name);
			gDose.value=gDrugData[DRUG_DATA_DOSEKGHR];
			snprintf(gDose.strValue, sizeof(gDose.strValue),"%.2f", gDose.value);
			SetWindowText(gDose.hwndValue,gDose.strValue);
			Show_Drug_Unit( gDose.hwndUnit,gDrugUnit[DRUG_DATA_DOSEKGHR] );
			break;
		default:
			return;
	}
	
}
void	Show_Titr_Basic_Item( int basic )
{

	if(basic==V_TITR_INFRATE)
		{
			SetWindowText(stcTitrHead[1],"Inf Rate");
			SetWindowText(stcTitrHead[3],"Inf Rate");
			SetWindowText(stcTitrHead[5],"Inf Rate");
		}
	if(basic==V_TITR_DRIPRATE)
		{
			SetWindowText(stcTitrHead[1],"Drip Rate");
			SetWindowText(stcTitrHead[3],"Drip Rate");
			SetWindowText(stcTitrHead[5],"Drip Rate");
		}
}
/*向上翻页*/
void	Func_Titration_Up(HWND hWnd)
{
	
	int	pagemax;
	int	show = FALSE;
	int m_istep,m_ibasic,m_idosetype;
	//获取step值
	
	m_istep=SendMessage(cobStep, CB_GETCURSEL, 0, 0)+1;
	m_ibasic=SendMessage(cobDose, CB_GETCURSEL, 0, 0);
	m_idosetype=SendMessage(cobDoseType, CB_GETCURSEL, 0, 0);

	if( m_ibasic == V_TITR_DOSE )
		pagemax = 10000;
	else
		pagemax = 1000;
	
	pagemax = pagemax/(m_istep*TITR_LIST_PRT_LEN);

	if( s_Titr_Page<pagemax ){
		s_Titr_Page++;
		show = TRUE;
	}
	
	if( show ){
	//if(B_PRINTF) printf("up page=%d,m_istep=%d,m_ibasic=%d,m_idosetype=%d\n",s_Titr_Page,m_istep,m_ibasic,m_idosetype);
	//计算滴定表
	Cacl_Titr_Table( s_Titr_Page, m_istep, m_ibasic, m_idosetype);
	//显示滴定表
	Show_Titr_Table(hWnd);
	
	}

}
/*向下翻页*/
void	Func_Titration_Down(HWND hWnd)
{
	
	int pagemax;
	int show = FALSE;
	int m_istep,m_ibasic,m_idosetype;
	//获取step值
	
	m_istep=SendMessage(cobStep, CB_GETCURSEL, 0, 0)+1;
	m_ibasic=SendMessage(cobDose, CB_GETCURSEL, 0, 0);
	m_idosetype=SendMessage(cobDoseType, CB_GETCURSEL, 0, 0);

	if( m_ibasic == V_TITR_DOSE )
		pagemax = 10000;
	else
		pagemax = 1000;
	
	if( s_Titr_Page>0 ){
		s_Titr_Page--;
		show = TRUE;
	}
	
	if( show ){
		if(B_PRINTF) printf("down page=%d,m_istep=%d,m_ibasic=%d,m_idosetype=%d\n",s_Titr_Page,m_istep,m_ibasic,m_idosetype);
	//计算滴定表
	Cacl_Titr_Table( s_Titr_Page, m_istep, m_ibasic, m_idosetype);
	//显示滴定表
	Show_Titr_Table(hWnd);
	}
}
static int DrawTABFocusColor(HWND hWnd)
{
	RECT rc;
	HDC hdc= (HDC)NULL;
	GetWindowRect(hWnd, &rc);	
	hdc = GetClientDC(GetParent(hWnd));
	if(hdc != (HDC)NULL){
		SetPenColor(hdc, SETFOCUS_COLOR);
		Rectangle(hdc, rc.left-1, rc.top-1, rc.right+1, rc.bottom+1);
		Rectangle(hdc, rc.left-2, rc.top-2, rc.right+2, rc.bottom+2);
		ReleaseDC(hdc);
	}
	
	return 0;
}
static int DrawUDFocusColor(HWND hWnd)
{	
	RECT rc;
	HDC hdc= (HDC)NULL;
	GetWindowRect(hWnd, &rc);	
	hdc = GetClientDC(GetParent(hWnd));
	if(hdc != (HDC)NULL){
		SetPenColor(hdc, COLOR_green);
		Rectangle(hdc, rc.left-1, rc.top-1, rc.right+1, rc.bottom+1);
		Rectangle(hdc, rc.left-2, rc.top-2, rc.right+2, rc.bottom+2);
		ReleaseDC(hdc);
	}
	
	return 0;
}

/*
	帮助控件回调函数
*/
static int EditProcHelp(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	char strHelp[200] = {0};
	//设置帮助栏背景框颜色和字体颜色
	SetWindowBkColor(hWnd,COLOR_black);
#ifdef FONTCOLOR	
	SetWindowElementColorEx( hWnd, FGC_CONTROL_NORMAL,COLOR_yellow);
#endif
	switch(message){
		case MSG_PAINT:{
			id = lParam;
			switch(id){
				case DID_DRUGTITR_DOSE_COB:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUGTITR, "help_dose", strHelp, sizeof strHelp,"Select one as the input and caloulate the other one.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_DRUGTITR_DOSETYPE_COB:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUGTITR, "help_dosetype", strHelp, sizeof strHelp,"Select desired dose type.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_DRUGTITR_STEP_COB:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUGTITR, "help_step", strHelp, sizeof strHelp,"Set the interval between the referenced values.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_DRUGTITR_UD:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUGTITR, "help_pageupdown", strHelp, sizeof strHelp,"Page up/down to view more titrated values.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_DRUGTITR_RECORD:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUGTITR, "help_record", strHelp, sizeof strHelp,"Print the titration data on currently screen.");
					SetWindowText(hWnd, strHelp);
				}break;
				case DID_DRUGTITR_OK:{
					memset(strHelp, 0, sizeof strHelp);
					GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "help_previous", strHelp, sizeof strHelp,"Return to the previous menu.");
					SetWindowText(hWnd, strHelp);
				}break;
				default:{
					SetWindowText(hWnd, "");
				}break;
			}break;
		}break;	
	}

	temp = (*OldEditProcHelp)(hWnd, message, wParam, lParam);
	return(temp);
}

/*
	列表框消息回调函数
*/
static int CobProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	int index;
	RECT rc;
	HDC hdc= (HDC)NULL;
	int m_istep,m_ibasic,m_idosetype;
	
	GetWindowRect(hWnd, &rc);	
	switch(message){
		case MSG_SETFOCUS:{
			hdc = GetClientDC(GetParent(hWnd));
			if(hdc != (HDC)NULL){
				SetPenColor(hdc, SETFOCUS_COLOR);
				Rectangle(hdc, rc.left-1, rc.top-1, rc.right+1, rc.bottom+1);
				Rectangle(hdc, rc.left-2, rc.top-2, rc.right+2, rc.bottom+2);
				ReleaseDC(hdc);
				SendMessage(editHelp, MSG_PAINT, 0, (LPARAM)GetDlgCtrlID(hWnd));
			}
		}break;
		case MSG_KILLFOCUS:{
			hdc = GetClientDC(GetParent(hWnd));
			if(hdc !=(HDC)NULL){
				SetPenColor(hdc, KILLFOCUS_COLOR);
				Rectangle(hdc, rc.left-1, rc.top-1, rc.right+1, rc.bottom+1);
				Rectangle(hdc, rc.left-2, rc.top-2, rc.right+2, rc.bottom+2);
				ReleaseDC(hdc);
			}
		}break;
	case MSG_COMMAND:{
		case MSG_KEYUP:{
			
			if(wParam==SCANCODE_ENTER||CBN_EDITCHANGE==HIWORD(wParam)){
					id = GetDlgCtrlID(hWnd);
					index = SendMessage(hWnd, CB_GETCURSEL, 0, 0);
						
					switch(id){
						case DID_DRUGTITR_DOSE_COB:{
							bCobEnter= !bCobEnter;
							if(bCobEnter){
								gbKeyType = KEY_UD;
								DrawUDFocusColor(hWnd);
							} 
							else{
								gbKeyType = KEY_TAB;
								DrawTABFocusColor(hWnd);
							}
							m_istep=SendMessage(cobStep, CB_GETCURSEL, 0, 0)+1;
							m_ibasic=SendMessage(cobDose, CB_GETCURSEL, 0, 0);
							m_idosetype=SendMessage(cobDoseType, CB_GETCURSEL, 0, 0);
							
							//显示/剂量类型数据
							Show_Titr_Dose_Data( m_idosetype);
							s_Titr_Page = 0;
							
							//显示INF RATE/DRIP RATE标题
							Show_Titr_Basic_Item(m_ibasic);
							//计算滴定表
							Cacl_Titr_Table( s_Titr_Page, m_istep, m_ibasic, m_idosetype);
							//显示滴定表
							Show_Titr_Table(hWnd);
							
						}break;
						case DID_DRUGTITR_DOSETYPE_COB:{
							bCobEnter= !bCobEnter;
							if(bCobEnter){
								gbKeyType = KEY_UD;
								DrawUDFocusColor(hWnd);
							} 
							else{
								gbKeyType = KEY_TAB;
								DrawTABFocusColor(hWnd);
							}
							
							//获取step值
							m_istep=SendMessage(cobStep, CB_GETCURSEL, 0, 0)+1;
							m_ibasic=SendMessage(cobDose, CB_GETCURSEL, 0, 0);
							m_idosetype=SendMessage(cobDoseType, CB_GETCURSEL, 0, 0);
							
							//显示/剂量类型数据
							Show_Titr_Dose_Data( m_idosetype);
							s_Titr_Page = 0;
							
						//	if(B_PRINTF) printf("m_istep=%d,m_ibasic=%d,m_idosetype=%d\n",m_istep,m_ibasic,m_idosetype);
							//计算滴定表
							Cacl_Titr_Table( s_Titr_Page, m_istep, m_ibasic, m_idosetype);
							//显示滴定表
							Show_Titr_Table(hWnd);
						}break;
						case DID_DRUGTITR_STEP_COB:{
							bCobEnter= !bCobEnter;
							if(bCobEnter){
								gbKeyType = KEY_UD;
								DrawUDFocusColor(hWnd);
							} 
							else{
								gbKeyType = KEY_TAB;
								DrawTABFocusColor(hWnd);
							}
							m_istep=SendMessage(cobStep, CB_GETCURSEL, 0, 0)+1;
							m_ibasic=SendMessage(cobDose, CB_GETCURSEL, 0, 0);
							m_idosetype=SendMessage(cobDoseType, CB_GETCURSEL, 0, 0);
							
							s_Titr_Page = 0;
							//计算滴定表
							Cacl_Titr_Table( s_Titr_Page, m_istep, m_ibasic, m_idosetype);
							//显示滴定表
							Show_Titr_Table(hWnd);
							
						}break;
					}	
				//	gbKeyType = KEY_TAB;
				}break;
			}
			return 0;
		}break;	
	}	
	temp = (*OldCobProc)(hWnd, message, wParam, lParam);
	return(temp);}

/*
	按钮消息回调函数
*/
static int BtnProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	int temp;
	int id;
	id = GetDlgCtrlID(hWnd);
	switch(message){
		case MSG_SETFOCUS:{
			if(bBtnBrowse ){
				SetWindowBkColor(hWnd, BROWSE_COLOR);
			}
			else{
				SetWindowBkColor(hWnd, SETFOCUS_COLOR);	
			}
			SendMessage(editHelp, MSG_PAINT, 0, (LPARAM)GetDlgCtrlID(hWnd));
		}break;
		case MSG_KILLFOCUS:{
			SetWindowBkColor(hWnd, KILLFOCUS_COLOR);
		}break;	
		case MSG_KEYUP:{
			switch(wParam){
				case SCANCODE_ENTER:{
					switch(id){
						case DID_DRUGTITR_UD:{
							bBtnBrowse= ! bBtnBrowse;
							if(bBtnBrowse){
								gbKeyType = KEY_AD;
								SetWindowBkColor(hWnd, BROWSE_COLOR);
							} 
							else{
								gbKeyType = KEY_TAB;
								SetWindowBkColor(hWnd, SETFOCUS_COLOR);
							}
						}break;
					}//end switch(id)
				}break;
				case SCANCODE_A:{
					if(bBtnBrowse){
						SetFocus(btnUD);
						Func_Titration_Up(hWnd);
						UpdateWindow(ghWnd,TRUE);
						return TRUE;
					} 
				}break;
				case SCANCODE_D:{
					if(bBtnBrowse){
						SetFocus(btnUD);
						Func_Titration_Down(hWnd);
						UpdateWindow(ghWnd,TRUE);
						return TRUE;
					} 
				}break;
			}//end switch(wParam)
		}break;
		
		
	}
	
	temp = (*OldBtnProc)(hWnd, message, wParam, lParam);
	return(temp);
}


/*
	创建控件函数
*/
static int CreateCtlProc(HWND hDlg)
{
	editCaption = CreateWindow("static", "", WS_CHILD | SS_NOTIFY |SS_CENTER | WS_VISIBLE,
					     DID_DRUGTITR_CAPTION, 0, 5, 400, 20, hDlg, 0);
	//SetWindowBkColor(editCaption,COLOR_darkgray);
	editHelp = GetDlgItem(hDlg, DID_DRUGTITR_HELP);

	gAmount.hwndName= CreateWindow("static", "", WS_CHILD |  WS_VISIBLE, 
					     	DID_DRUGTITR_GAMOUNT, 20, 25, 50, 26, hDlg, 0);
	gAmount.hwndValue= CreateWindow("static", "", WS_CHILD | WS_VISIBLE, 
					     DID_DRUGTITR_GAMOUNT+1, 80, 25, 60, 26, hDlg, 0);
	gAmount.hwndUnit= CreateWindow("static", "", WS_CHILD | WS_VISIBLE, 
					     DID_DRUGTITR_GAMOUNT+2, 130, 25, 60, 26, hDlg, 0);
	
	gVolume.hwndName= CreateWindow("static", "", WS_CHILD |  WS_VISIBLE, 
					     	DID_DRUGTITR_GVOLUME, 20, 45, 50, 26, hDlg, 0);
	gVolume.hwndValue= CreateWindow("static", "", WS_CHILD | WS_VISIBLE, 
					     DID_DRUGTITR_GVOLUME+1, 80, 45, 60, 26, hDlg, 0);
	gVolume.hwndUnit= CreateWindow("static", "", WS_CHILD | WS_VISIBLE, 
					     DID_DRUGTITR_GVOLUME+2, 130, 45, 60, 26, hDlg, 0);
	
	gTWeight.hwndName= CreateWindow("static", "", WS_CHILD |  WS_VISIBLE, 
					     	DID_DRUGTITR_GWEIGHT, 20, 65, 50, 26, hDlg, 0);
	gTWeight.hwndValue= CreateWindow("static", "", WS_CHILD | WS_VISIBLE, 
					     DID_DRUGTITR_GWEIGHT+1, 80, 65, 60, 26, hDlg, 0);
	gTWeight.hwndUnit= CreateWindow("static", "", WS_CHILD | WS_VISIBLE, 
					     DID_DRUGTITR_GWEIGHT+2, 130, 65, 60, 26, hDlg, 0);
	
	gDose.hwndName= CreateWindow("static", "", WS_CHILD |  WS_VISIBLE, 
					     	DID_DRUGTITR_GDOSETYPE, 200, 26, 70, 22, hDlg, 0);
	gDose.hwndValue= CreateWindow("static", "", WS_CHILD | WS_VISIBLE, 
					     DID_DRUGTITR_GDOSETYPE+1, 270, 26, 60, 22, hDlg, 0);
	gDose.hwndUnit= CreateWindow("static", "", WS_CHILD | WS_VISIBLE, 
					     DID_DRUGTITR_GDOSETYPE+2, 320, 26, 60, 22, hDlg, 0);
	
	gInfRate.hwndName= CreateWindow("static", "", WS_CHILD |  WS_VISIBLE, 
					     	DID_DRUGTITR_GINFRATE, 200, 45, 50, 26, hDlg, 0);
	gInfRate.hwndValue= CreateWindow("static", "", WS_CHILD | WS_VISIBLE, 
					     DID_DRUGTITR_GINFRATE+1, 270, 45, 60, 26, hDlg, 0);
	gInfRate.hwndUnit= CreateWindow("static", "", WS_CHILD | WS_VISIBLE, 
					     DID_DRUGTITR_GINFRATE+2, 320, 45, 60, 26, hDlg, 0);
	
	gDripRate.hwndName= CreateWindow("static", "", WS_CHILD |  WS_VISIBLE, 
					     	DID_DRUGTITR_GDRIPRATE, 200, 65, 60, 26, hDlg, 0);
	gDripRate.hwndValue= CreateWindow("static", "", WS_CHILD | WS_VISIBLE, 
					     DID_DRUGTITR_GDRIPRATE+1, 270, 65, 60, 26, hDlg, 0);
	gDripRate.hwndUnit= CreateWindow("static", "", WS_CHILD | WS_VISIBLE, 
					     DID_DRUGTITR_GDRIPRATE+2, 320, 65, 60, 26, hDlg, 0);

	stcDose = CreateWindow("static", "", WS_CHILD |  WS_VISIBLE, 
					     DID_DRUGTITR_DOSE, 5, 285, 50, 26, hDlg, 0);
	stcDoseType = CreateWindow("static", "", WS_CHILD | WS_VISIBLE, 
					     DID_DRUGTITR_DOSETYPE, 150, 285, 60, 26, hDlg, 0);
	stcStep = CreateWindow("static", "", WS_CHILD | WS_VISIBLE, 
					     DID_DRUGTITR_STEP, 315, 285, 60, 26, hDlg, 0);

	cobDose = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE|  WS_TABSTOP |  CBS_DROPDOWNLIST|CBS_READONLY, 
					DID_DRUGTITR_DOSE_COB, 50,280, 90, 25, hDlg, 0);
	cobDoseType = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP|  CBS_DROPDOWNLIST|CBS_READONLY,
					DID_DRUGTITR_DOSETYPE_COB, 220, 280, 90, 25, hDlg, 0);
	cobStep = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE| WS_TABSTOP |  CBS_DROPDOWNLIST|CBS_READONLY,
					DID_DRUGTITR_STEP_COB, 350, 280, 40, 23, hDlg, 0);

	btnUD = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_DRUGTITR_UD, 5, 280+28, 110, 26, hDlg, 0);
	btnRecord= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_DRUGTITR_RECORD, 140, 280+28, 110, 26, hDlg, 0);
	btnOk = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_DRUGTITR_OK, 280, 280+28, 110, 26, hDlg, 0);

/*
	stcDose = CreateWindow("static", "", WS_CHILD |  WS_VISIBLE, 
					     DID_DRUGTITR_DOSE, 140, 280, 50, 26, hDlg, 0);
	stcDoseType = CreateWindow("static", "", WS_CHILD | WS_VISIBLE, 
					     DID_DRUGTITR_DOSETYPE, 120, 280+30, 60, 26, hDlg, 0);
	stcStep = CreateWindow("static", "", WS_CHILD | WS_VISIBLE, 
					     DID_DRUGTITR_STEP, 140, 280+60, 60, 26, hDlg, 0);

	btnUD = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_DRUGTITR_UD, 280, 280, 110, 26, hDlg, 0);
	btnRecord= CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_DRUGTITR_RECORD, 280, 280+28, 110, 26, hDlg, 0);
	btnOk = CreateWindow("button", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON ,
					     DID_DRUGTITR_OK, 280, 280+56, 110, 26, hDlg, 0);

	cobDose = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE|  WS_TABSTOP |  CBS_DROPDOWNLIST,  
					DID_DRUGTITR_DOSE_COB, 180,280, 90, 25, hDlg, 0);
	cobDoseType = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP|  CBS_DROPDOWNLIST,  
					DID_DRUGTITR_DOSETYPE_COB, 180, 280+30, 90, 25, hDlg, 0);
	cobStep = CreateWindow("combobox", "", WS_CHILD | WS_VISIBLE| WS_TABSTOP |  CBS_DROPDOWNLIST,  
					DID_DRUGTITR_STEP_COB, 180, 280+60, 90, 23, hDlg, 0);
*/
	return 0;
}
static int InitCtlName(HWND hDlg)
{
	char strMenu[100];
	int i;
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUGTITR, "caption", strMenu, sizeof strMenu,"Titration Table");
	SetWindowText(editCaption, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_GENERAL, "previous", strMenu, sizeof strMenu,"Previous");	
	SetWindowText(btnOk, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUGTITR, "page_updown", strMenu, sizeof strMenu,"Page Up/Dowm");	
	SetWindowText(btnUD, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUGTITR, "record", strMenu, sizeof strMenu,"record");	
	SetWindowText(btnRecord, strMenu);

	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUGTITR, "dose", strMenu, sizeof strMenu,"Dose");	
	SetWindowText(stcDose, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUGTITR, "dosetype", strMenu, sizeof strMenu,"Dose Type");	
	SetWindowText(stcDoseType, strMenu);
	memset(strMenu, 0, sizeof strMenu);
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUGTITR, "step", strMenu, sizeof strMenu,"Step");	
	SetWindowText(stcStep, strMenu);
	for(i=0;i<6;i++){
		if(i==0||i==2||i==4)
			stcTitrHead[i]= CreateWindow("static", "Dose", WS_CHILD |SS_CENTER |  WS_VISIBLE, 
					     DID_DRUGTITR_TITRHEAD+i, rcTitrHead[i].left+1, rcTitrHead[i].top+1, 60, 22, hDlg, 0);
		else
			stcTitrHead[i]= CreateWindow("static", "Inf Rate", WS_CHILD |SS_CENTER |  WS_VISIBLE, 
					     DID_DRUGTITR_TITRHEAD+i, rcTitrHead[i].left+1, rcTitrHead[i].top+1, 60, 22, hDlg, 0);
	}
	return 0;
}
static int InitCobList(HWND hDlg)
{
	int i;
	unsigned char *strDose[3]={"Dose","Inf Rate","Drip rate"};
	unsigned char *strDoseType[4]={"Dose/min","Dose/hr","Dose/kg/min","Dose/kg/hr"};
	unsigned char *strStep[10]={"1","2","3","4","5","6","7","8","9","10"};
	for(i=0;i<3;i++){
		SendMessage(cobDose, CB_ADDSTRING, 0, (LPARAM)strDose[i]);
	}
	SendMessage(cobDose, CB_SETCURSEL,0, 0);
	for(i=0;i<4;i++){
		SendMessage(cobDoseType, CB_ADDSTRING, 0, (LPARAM)strDoseType[i]);
	}
	SendMessage(cobDoseType, CB_SETCURSEL,0, 0);
	for(i=0;i<10;i++){
		SendMessage(cobStep, CB_ADDSTRING, 0, (LPARAM)strStep[i]);
	}
	SendMessage(cobStep, CB_SETCURSEL,0, 0);
	
	return 0;
}

static int SetRECTTitration(HWND hWnd)
{
	HDC hdc;
	RECT rc;
	int i;
	hdc = GetClientDC(hWnd);
		if(hdc == (HDC)NULL) return -1;
	GetClientRect(hWnd, &rc);
	//参数框
	SetRect(&(gRcPara),rc.left+5, rc.top+25, rc.right-7, rc.top+85);
	//滴定表框
	SetRect(&(gRcTitr),rc.left+5, rc.top+87, rc.right-5, rc.top+275);
	//滴定表表头显示区域
	for( i=0;i<6;i++){
		SetRect(&(rcTitrHead[i]),gRcTitr.left+(RECTW(gRcTitr)/6)*i,gRcTitr.top,gRcTitr.left+(RECTW(gRcTitr)/6)*(i+1),gRcTitr.top+25);	
	}
	//滴定表数据显示区域
	for( i=0;i<6;i++){
		SetRect(&(rcTitrData[i]),gRcTitr.left+(RECTW(gRcTitr)/6)*i,gRcTitr.top+26,gRcTitr.left+(RECTW(gRcTitr)/6)*(i+1),gRcTitr.bottom);	
	}
	return 0;
}
static int TitrDrawFrame(HWND hWnd)
{
	HDC hdc;
	RECT rc;
	int i;
	hdc = GetClientDC(hWnd);
		if(hdc == (HDC)NULL) return -1;
	
	SetPenColor(hdc, COLOR_darkgray);
	Rectangle(hdc,  gRcPara.left,   gRcPara.top,   gRcPara.right, gRcPara.bottom);
	Rectangle(hdc,  gRcTitr.left,   gRcTitr.top,   gRcTitr.right-2, gRcTitr.top+24);
	for(i=0;i<6;i++)	
	{
		Rectangle(hdc,  gRcTitr.left+(RECTW(gRcTitr)/6)*i,   
					gRcTitr.top,   
					gRcTitr.left+(RECTW(gRcTitr)/6)*(i+1), 
					gRcTitr.bottom);
	}
	

	
	/*
	//滴定表表头
	for(i=0;i<6;i++){
		if(i==0||i==2||i==4)
			DrawText(hdc, "Dose", -1, &rcTitrHead[i], DT_NOCLIP | DT_SINGLELINE | DT_VCENTER | DT_CENTER );
		else
			DrawText(hdc, "Drip Rate", -1, &rcTitrHead[i], DT_NOCLIP | DT_SINGLELINE | DT_VCENTER | DT_CENTER );
	}
	*/
	
	ReleaseDC(hdc);
	return 0;
}
static int InitCtlCallBackProc(HWND hDlg)
{

	OldBtnProc = SetWindowCallbackProc(btnOk, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnUD, BtnProc);
	OldBtnProc = SetWindowCallbackProc(btnRecord, BtnProc);
	
	OldCobProc = SetWindowCallbackProc(cobDose, CobProc);
	OldCobProc = SetWindowCallbackProc(cobDoseType, CobProc);
	OldCobProc = SetWindowCallbackProc(cobStep, CobProc);
	OldEditProcHelp  = SetWindowCallbackProc(editHelp, EditProcHelp);
	
	return 0;
}
static int GetValueFormDRUG()
{
	
	//参数名称
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUGTITR, "amount", gAmount.Name, sizeof gAmount.Name,"Amount");
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUGTITR, "volume", gVolume.Name, sizeof gVolume.Name,"Volume");
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUGTITR, "weight", gTWeight.Name, sizeof gTWeight.Name,"Weight");
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUGTITR, "dose_type", gDose.Name, sizeof gDose.Name,"Dose/min");
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUGTITR, "inf_rate", gInfRate.Name, sizeof gInfRate.Name,"Inf Rate");
	GetStringFromResFiles(gsLanguageRes, STR_SETTING_DLG_DRUGTITR, "drip_rate", gDripRate.Name, sizeof gDripRate.Name,"Drip Rate");
	SetWindowText(gAmount.hwndName,gAmount.Name);
	SetWindowText(gVolume.hwndName,gVolume.Name);
	SetWindowText(gTWeight.hwndName,gTWeight.Name);
	SetWindowText(gDose.hwndName,gDose.Name);
	SetWindowText(gInfRate.hwndName,gInfRate.Name);
	SetWindowText(gDripRate.hwndName,gDripRate.Name);
	
	//参数值
	gAmount.value=gDrugData[DRUG_DATA_AMOUNT];
	gVolume.value=gDrugData[DRUG_DATA_VOLUME];
	gTWeight.value=gWeight/100.0;
	gDose.value=gDrugData[DRUG_DATA_DOSEMIN];
	gInfRate.value=gDrugData[DRUG_DATA_INFRATE];
	gDripRate.value=gDrugData[DRUG_DATA_DRIPRATE];
	
	snprintf(gAmount.strValue, sizeof(gAmount.strValue),"%.2f", gAmount.value);
	snprintf(gVolume.strValue, sizeof(gVolume.strValue),"%.2f", gVolume.value);
	snprintf(gTWeight.strValue, sizeof(gTWeight.strValue),"%.2f", gTWeight.value);
	snprintf(gDose.strValue, sizeof(gDose.strValue),"%.2f", gDose.value);
	snprintf(gInfRate.strValue, sizeof(gInfRate.strValue),"%.2f", gInfRate.value);
	snprintf(gDripRate.strValue, sizeof(gDripRate.strValue),"%.2f", gDripRate.value);
	
	SetWindowText(gAmount.hwndValue,gAmount.strValue);
	SetWindowText(gVolume.hwndValue,gVolume.strValue);
	SetWindowText(gTWeight.hwndValue,gTWeight.strValue);
	SetWindowText(gDose.hwndValue,gDose.strValue);
	SetWindowText(gInfRate.hwndValue,gInfRate.strValue);
	SetWindowText(gDripRate.hwndValue,gDripRate.strValue);

	Show_Drug_Unit( gAmount.hwndUnit,	gDrugUnit[DRUG_DATA_AMOUNT]);
	Show_Drug_Unit( gVolume.hwndUnit,	gDrugUnit[DRUG_DATA_VOLUME]);
	Show_Drug_Unit( gTWeight.hwndUnit,	DRUG_UNIT_KG);
	Show_Drug_Unit( gDose.hwndUnit,	gDrugUnit[DRUG_DATA_DOSEMIN]);
	Show_Drug_Unit( gInfRate.hwndUnit,	gDrugUnit[DRUG_DATA_INFRATE]);
	Show_Drug_Unit( gDripRate.hwndUnit,	gDrugUnit[DRUG_DATA_DRIPRATE]);
	//if(B_PRINTF) printf("%f,%f,%f,%f,%f,%f\n",gAmount.value,gVolume.value,gTWeight.value,gDose.value,gInfRate.value,gDripRate.value);
	
	return 0;
}
static int InitCtlStatus(HWND hDlg)
{
	EnableWindow(btnRecord, FALSE);
	return 0;
}
/*
	对话框回调函数
*/
static int DlgProc(HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	
	int temp;
	char strMenu[100];
	int i;
	RECT rcDlg;
	RECT rcCaption;
	int m_istep,m_ibasic,m_idosetype;
	ghWnd=hDlg;
	switch(message){
		case MSG_INITDIALOG:{
			//设置区域
			SetRECTTitration(hDlg);
			//创建控件
			CreateCtlProc(hDlg);
			//设置控件字符
			InitCtlName(hDlg);
			//初始化列表框
			InitCobList(hDlg);
			//设置控件回调函数
			InitCtlCallBackProc(hDlg);
			//设置控件状态
			InitCtlStatus(hDlg);
			
			//设置参数区
			GetValueFormDRUG();
			
			TitrDrawFrame(hDlg);
			m_ibasic= SendMessage(cobDose, CB_GETCURSEL, 0, 0);
			m_idosetype= SendMessage(cobDoseType, CB_GETCURSEL, 0, 0);
			m_istep= SendMessage(cobStep, CB_GETCURSEL, 0, 0)+1;
			//计算滴定表
			Cacl_Titr_Table( s_Titr_Page, m_istep, m_ibasic, m_idosetype);
			Show_Titr_Table(hDlg);
			gbKeyType = KEY_TAB;
			SetFocus(btnOk);
		}break;
		case MSG_PAINT:{
			TitrDrawFrame(hDlg);
			Show_Titr_Table(hDlg);
		}break;
		case MSG_COMMAND:{
			int	id  = LOWORD(wParam);
			switch(id){
				case DID_DRUGTITR_OK:{
					PostMessage(hDlg, MSG_CLOSE, 0, 0L);
					}break;
				}
		}break;	
		case MSG_KEYUP:{
			switch(wParam){
				case SCANCODE_ESCAPE:{
					PostMessage(hDlg, MSG_CLOSE, 0, 0L);
				}break;
			}//end switch
		}break;
		case MSG_CLOSE:{
			gbKeyType = KEY_TAB;	
			//恢复波形位置
			ChangeWavePanelsLeft((PRECT)NULL);	
			EndDialog(hDlg, wParam);	
		}break;
	}
	
	temp = DefaultDialogProc(hDlg, message, wParam, lParam);
	return(temp);
}


/*
	建立对话框
*/
void CreateDRUGTITR(HWND hWnd)
{
			int High_Dlg;
if(gCfgSystem.bInterface==SCREEN_NORMAL||gCfgSystem.bInterface==SCREEN_7LEADECG
		||gCfgSystem.bInterface==SCREEN_MULTIECG||gCfgSystem.bInterface==SCREEN_SHORTTREND){
		High_Dlg=0;
	}else
		High_Dlg=160;
#if SCREEN_1024
	DlgSet.x = SCREEN_LEFT+100,
	DlgSet.y = SCREEN_TOP+180, 
#elif  SCREEN_640
	DlgSet.x = SCREEN_LEFT,
	DlgSet.y = SCREEN_BOTTOM-DLG_HIGH, 	
#else
	DlgSet.x = SCREEN_LEFT,
	DlgSet.y = SCREEN_BOTTOM-DLG_HIGH-High_Dlg, 
#endif

 	DlgSet.controls = CtrlSet;
 	DialogBoxIndirectParam(&DlgSet, hWnd, DlgProc, 0L);	
}


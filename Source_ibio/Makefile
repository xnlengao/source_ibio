#Makefile for uclibc
CROSS=
#CROSS=arm-uclibc-
CROSS=arm-linux-
CC=$(CROSS)gcc
CFLAGS=	

HEADFILE=dllist.h\
	IncludeFiles.h Global.h DataStruct.h General_Funcs.h Init_Funcs.h DataManage.h Serial_Funcs.h \
	ExtDevice_Funcs.h  Dialog.h   AlarmManage.h Nibp_Measure.h \
	NewDrawWave.h  Central.h  gtprint.h Printer_Funcs.h 

OBJECT=main.o GUI_Main.o \
	AlarmManage.o AlarmView.o AlarmJudgment.o AlarmSound.o AlarmLight.o\
	 Sound.o \
	dllist.o  RealSave.o WULine.o\
	Init_Funcs.o General_Funcs.o Printer_Funcs.o ExtDevice_Funcs.o Serial_Funcs.o\
	NetFunc.o NetRecv.o NetSend.o  NetRecv_Bed.o\
	DataManage.o Nibp_Measure.o\
	KeyInput.o Keyboard.o DlgPowerOff.o \
	TrendManage.o  NibpTable.o  TrendGraph.o TrendTable.o \
	DlgSet_ACK.o DlgSet_Patient.o DlgSet_Color.o ColorSelect.o DlgSet_AlarmPrint.o\
	Dlg_Freeze.o DlgSet_Net.o SetColor.o\
	NewDrawWave.o NewDrawWaveThread.o NewShortTrend.o NewOxyCRG.o NewOtherBed.o \
	NewViewData.o   ScreenReview.o WaveReview.o NewAlarmReview.o\
	DlgNew_MainMenu.o DlgNew_Monitor.o DlgNew_waveformset.o DlgNew_SystemSet.o DlgNew_TrendSet.o\
 	DlgNew_ShortTrend.o DlgNew_Recorder.o DlgNew_TimeSet.o DlgNew_ModeConfig.o \
	DlgNew_AlarmLevel.o NewKeyPassword.o DlgNew_Machine.o DlgNew_ModuleMaint.o DlgNew_NIBPMaint.o\
	DlgNew_SYSMaint.o DlgNew_ModuleSet.o DlgNew_SystemINFO.o DlgNew_AlarmSetup.o \
	DlgNew_DRUG.o DlgNew_DRUGTitr.o DlgNew_Ecg.o DlgNew_EcgSetup.o\
 	DlgNew_EcgAlarm.o DlgNew_EcgSTAdjust.o\
	DlgNew_Resp.o DlgNew_RespAlarm.o DlgNew_Spo2.o DlgNew_Spo2Alarm.o DlgNew_Spo2PRAlarm.o DlgNew_Temp.o\
 	DlgNew_Nibp.o DlgNew_NibpAlarm.o DlgNew_Alarm.o DlgNew_Factory.o Printer_UC50_Funcs.o Power_Funcs.o DlgSet_CO2.o DlgSet_CO2Set.o\
	DlgSet_Ibp1.o DlgSet_Ibp1Set.o DlgSet_Ibp1SetZero.o DlgSet_Ibp2.o\
	DlgSet_Ibp2Set.o DlgSet_Ibp2SetZero.o NewiBio.o ComiBio.o DlgNew_EcgARR.o ArrReview.o
#DlgNew_Ibp1.o DlgNew_Ibp1-1.o DlgNew_Ibp2.o DlgNew_Ibp2-1.o
TT:$(OBJECT) $(HEADFILE)
#	$(CC) -Wall -s -o TT $(OBJECT)  -lpthread -lminigui -lmgext  -lecganalysis  -lfreetype -ljpeg -lpng
	$(CC) -Wall -s -o TT $(OBJECT)  -lpthread -lminigui -lmgext -lttf -ljpeg -lpng
	cp TT /root/NFS/mntroot
#	cp TT /work/nfs_root/Moni
clean:
	rm  *.o -f
	rm TT -f



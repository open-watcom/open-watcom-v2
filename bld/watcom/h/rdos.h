
#ifndef _RDOS_H
#define _RDOS_H

#pragma pack( push, 1 )

#define RDOSAPI

#ifdef __WATCOMC__
#include "machtype.h"
#define real_math   xreal
#endif

#ifdef __GNUC__
#define real_math   long double
#endif

#include "rdoshdr.h"
#include "rdu.h"

#define FILE_ATTRIBUTE_READONLY         0x1
#define FILE_ATTRIBUTE_HIDDEN           0x2
#define FILE_ATTRIBUTE_SYSTEM           0x4
#define FILE_ATTRIBUTE_DIRECTORY        0x10
#define FILE_ATTRIBUTE_ARCHIVE          0x20
#define FILE_ATTRIBUTE_NORMAL           0x80

#define LGOP_NULL  0
#define LGOP_NONE  1
#define LGOP_OR  2
#define LGOP_AND  3
#define LGOP_XOR  4
#define LGOP_INVERT  5
#define LGOP_INVERT_OR  6
#define LGOP_INVERT_AND  7
#define LGOP_INVERT_XOR  8
#define LGOP_ADD  9
#define LGOP_SUBTRACT  10
#define LGOP_MULTIPLY  11

#define getred(pgc)       (((pgc)>>16)&0xFF)
#define getgreen(pgc)     (((pgc)>>8)&0xFF)
#define getblue(pgc)      ((pgc)&0xFF)
#define mkcolor(r,g,b)    (((r)<<16)|((g)<<8)|(b))

#define AUDIO_WIDGET_TYPE_OUTPUT      1
#define AUDIO_WIDGET_TYPE_INPUT       2
#define AUDIO_WIDGET_TYPE_MIXER       3
#define AUDIO_WIDGET_TYPE_SELECTOR    4
#define AUDIO_WIDGET_TYPE_PIN         5
#define AUDIO_WIDGET_TYPE_POWER       6
#define AUDIO_WIDGET_TYPE_BEEP        7

typedef struct ThreadState
{
     short int ID;
     char Name[32];
     unsigned long MsbTime;
     unsigned long LsbTime;
     char List[32];
     long Offset;
     short int Sel;
} ThreadState;

typedef struct Tss
{
    long cr3;
    long eip;
    long eflags;
    long eax;
    long ecx;
    long edx;
    long ebx;
    long esp;
    long ebp;
    long esi;
    long edi;
    short int es;
    short int cs;
    short int ss;
    short int ds;
    short int fs;
    short int gs;
    short int ldt;
    long dr[4];
    long dr7;
    long MathControl;
    long MathStatus;
    long MathTag;
    long MathEip;
    short int MathCs;
    long MathDataOffs;
    short int MathDataSel;
    real_math st[8];
    char WcSpace[16];
} Tss;

#define uss_handle  0
#define uss_counter 4
#define uss_val 8
#define uss_owner 10
#define uss_size 12

// Exception handling prototypes 

#define STATUS_BREAKPOINT               0x80000003L
#define STATUS_SINGLE_STEP              0x80000004L
#define STATUS_ACCESS_VIOLATION         0xC0000005L
#define STATUS_IN_PAGE_ERROR            0xC0000006L
#define STATUS_INVALID_HANDLE           0xC0000008L
#define STATUS_NO_MEMORY                0xC0000017L
#define STATUS_ILLEGAL_INSTRUCTION      0xC000001DL
#define STATUS_ARRAY_BOUNDS_EXCEEDED    0xC000008CL
#define STATUS_FLOAT_DENORMAL_OPERAND   0xC000008DL
#define STATUS_FLOAT_DIVIDE_BY_ZERO     0xC000008EL
#define STATUS_FLOAT_INEXACT_RESULT     0xC000008FL
#define STATUS_FLOAT_INVALID_OPERATION  0xC0000090L
#define STATUS_FLOAT_OVERFLOW           0xC0000091L
#define STATUS_FLOAT_STACK_CHECK        0xC0000092L
#define STATUS_FLOAT_UNDERFLOW          0xC0000093L
#define STATUS_INTEGER_DIVIDE_BY_ZERO   0xC0000094L
#define STATUS_INTEGER_OVERFLOW         0xC0000095L
#define STATUS_PRIVILEGED_INSTRUCTION   0xC0000096L
#define STATUS_STACK_OVERFLOW           0xC00000FDL
#define STATUS_CONTROL_C_EXIT           0xC000013AL

typedef struct _FLOATING_SAVE_AREA {
    int ControlWord;
    int StatusWord;
    int TagWord;
    int ErrorOffset;
    int ErrorSelector;
    int DataOffset;
    int DataSelector;
    char RegisterArea[80];
    int   Cr0NpxState;
} FLOATING_SAVE_AREA;

typedef struct _CONTEXT 
{
    long ContextFlags;
    long Dr0;
    long Dr1;
    long Dr2;
    long Dr3;
    long Dr6;
    long Dr7;
    FLOATING_SAVE_AREA FloatSave;
    long SegGs;
    long SegFs;
    long SegEs;
    long SegDs;
    long Edi;
    long Esi;
    long Ebx;
    long Edx;
    long Ecx;
    long Eax;
    long Ebp;
    long Eip;
    long SegCs;
    long EFlags;
    long Esp;
    long SegSs;
} CONTEXT;

typedef struct _EXCEPTION_RECORD
{
    long ExceptionCode;
    long ExceptionFlags;
    struct _EXCEPTION_RECORD *ExceptionRecord;
    void *ExceptionAddress;
} EXCEPTION_RECORD;

typedef struct _EXCEPTION_POINTERS {
    EXCEPTION_RECORD *ExceptionRecord;
    CONTEXT *ContextRecord;
} EXCEPTION_POINTERS;

#pragma pack( pop )


// API functions

#ifndef __GNUC__

#ifdef __cplusplus
extern "C" {
#endif

void RDOSAPI RdosTestGate();

void RDOSAPI RdosLoad32();

short int RDOSAPI RdosSwapShort(short int val);
long RDOSAPI RdosSwapLong(long val);

int RDOSAPI RdosGetCharSize(const char *str);

long RDOSAPI RdosGetLongRandom();
long RDOSAPI RdosGetRandom(long range);

long RDOSAPI RdosGetAcpiStatus();
int RDOSAPI RdosGetAcpiObject(int Index, char *AcpiName);
int RDOSAPI RdosGetAcpiMethod(int Object, int Index, char *AcpiName);
int RDOSAPI RdosGetAcpiDevice(int Index, char *AcpiName);
int RDOSAPI RdosGetAcpiDeviceIrq(int Device, int Index, int *Share, int *Polarity, int *TriggerMode);
int RDOSAPI RdosGetAcpiDeviceIo(int Device, int Index, int *Start, int *End);
int RDOSAPI RdosGetAcpiDeviceMem(int Device, int Index, int *Start, int *End);
int RDOSAPI RdosGetPciDeviceName(int Index, char *AcpiName);
int RDOSAPI RdosGetPciDeviceInfo(int Index, int *Bus, int *Device, int *Function);
int RDOSAPI RdosGetPciDeviceVendor(int Index, int *Vendor, int *Device);
int RDOSAPI RdosGetPciDeviceClass(int Index, int *Class, int *Subclass);
int RDOSAPI RdosGetPciDeviceIrq(int Index);
int RDOSAPI RdosGetCpuTemperature();

int RDOSAPI RdosGetAudioDeviceCount();
int RDOSAPI RdosGetAudioCodecCount(int Device);
int RDOSAPI RdosGetAudioWidgetInfo(int Device, int Codec, int Node, char *Info);
int RDOSAPI RdosGetAudioWidgetConnectionList(int Device, int Codec, int Node, int *ConnectionList);
int RDOSAPI RdosGetSelectedAudioConnection(int Device, int Codec, int Node);
void RDOSAPI RdosGetAudioInputAmpCap(int Device, int Codec, int Node, int *min, int *max);
void RDOSAPI RdosGetAudioOutputAmpCap(int Device, int Codec, int Node, int *min, int *max);
int RDOSAPI RdosHasAudioInputMute(int Device, int Codec, int Node);
int RDOSAPI RdosHasAudioOutputMute(int Device, int Codec, int Node);
int RDOSAPI RdosReadAudioInputAmp(int Device, int Codec, int Node, int Channel, int Input);
int RDOSAPI RdosReadAudioOutputAmp(int Device, int Codec, int Node, int Channel);
int RDOSAPI RdosIsAudioInputAmpMuted(int Device, int Codec, int Node, int Channel, int Input);
int RDOSAPI RdosIsAudioOutputAmpMuted(int Device, int Codec, int Node, int Channel);
int RDOSAPI RdosGetFixedAudioOutput(int *Device, int *Codec, int *Node);
int RDOSAPI RdosGetJackAudioOutput(int Num, int *Device, int *Codec, int *Node);
int RDOSAPI RdosGetJackAudioInput(int Num, int *Device, int *Codec, int *Node);

void RDOSAPI RdosSetTextMode();
int RDOSAPI RdosSetVideoMode(int *BitsPerPixel, int *xres, int *yres, int *linesize, void **buffer);
void RDOSAPI RdosSetClipRect(int handle, int xmin, int ymin, int xmax, int ymax);
void RDOSAPI RdosClearClipRect(int handle);
void RDOSAPI RdosSetDrawColor(int handle, int color);
void RDOSAPI RdosSetLGOP(int handle, int lgop);
void RDOSAPI RdosSetHollowStyle(int handle);
void RDOSAPI RdosSetFilledStyle(int handle);

int RDOSAPI RdosAnsiToUtf8(const char *AnsiStr, char *Utf8Str, int BufferSize);
int RDOSAPI RdosUtf8ToAnsi(const char *Utf8Str, char *AnsiStr, int BufferSize);

int RDOSAPI RdosOpenFont(int id, int height);
void RDOSAPI RdosCloseFont(int font);
void RDOSAPI RdosGetStringMetrics(int font, const char *str, int *width, int *height);
void RDOSAPI RdosSetFont(int handle, int font);
int RDOSAPI RdosGetPixel(int handle, int x, int y);
void RDOSAPI RdosSetPixel(int handle, int x, int y);
void RDOSAPI RdosBlit(int SrcHandle, int DestHandle, int width, int height,
                                int SrcX, int SrcY, int DestX, int DestY);
void RDOSAPI RdosDrawMask(int handle, void *mask, int RowSize, int width, int height,
                                int SrcX, int SrcY, int DestX, int DestY); 
void RDOSAPI RdosDrawLine(int handle, int x1, int y1, int x2, int y2);
void RDOSAPI RdosDrawString(int handle, int x, int y, const char *str);
void RDOSAPI RdosDrawRect(int handle, int x, int y, int width, int height);
void RDOSAPI RdosDrawEllipse(int handle, int x, int y, int width, int height);
int RDOSAPI RdosCreateBitmap(int BitsPerPixel, int width, int height);
int RDOSAPI RdosCreateAlphaBitmap(int width, int height);
int RDOSAPI RdosExtractValidBitmapMask(int handle);
int RDOSAPI RdosExtractInvalidBitmapMask(int handle);
int RDOSAPI RdosExtractAlphaBitmap(int handle);
int RDOSAPI RdosDuplicateBitmapHandle(int handle);
void RDOSAPI RdosCloseBitmap(int handle);
int RDOSAPI RdosCreateStringBitmap(int font, const char *str);
void RDOSAPI RdosGetBitmapInfo(int handle, int *BitPerPixel, int *width, int *height,
                                           int *linesize, void **buffer);

int RDOSAPI RdosCreateSprite(int DestHandle, int BitmapHandle, int MaskHandle, int lgop); 
void RDOSAPI RdosCloseSprite(int handle);
void RDOSAPI RdosShowSprite(int handle);
void RDOSAPI RdosHideSprite(int handle);
void RDOSAPI RdosMoveSprite(int handle, int x, int y);

void RDOSAPI RdosSetForeColor(int color);
void RDOSAPI RdosSetBackColor(int color);

#ifdef __RDOS__     // these are only available in user-mode

void *RDOSAPI RdosAllocateMem(int Size);
void RDOSAPI RdosFreeMem(void *ptr);

void *RDOSAPI RdosAllocateDebugMem(int Size);
void RDOSAPI RdosFreeDebugMem(void *ptr);

long RDOSAPI RdosGetThreadLinear(int Thread, int Sel, long Offset);
int RDOSAPI RdosReadThreadMem(int Thread, int Sel, long Offset, char *Buf, int Size);
int RDOSAPI RdosWriteThreadMem(int Thread, int Sel, long Offset, char *Buf, int Size);
int RDOSAPI RdosGetDebugThread();
void RDOSAPI RdosGetThreadTss(int Thread, Tss *tss);
void RDOSAPI RdosSetThreadTss(int Thread, Tss *tss);

int RDOSAPI RdosSetCodeBreak(int Thread, int Reg, int Sel, long Offset);
int RDOSAPI RdosSetReadDataBreak(int Thread, int Reg, int Sel, long Offset, int Size);
int RDOSAPI RdosSetWriteDataBreak(int Thread, int Reg, int Sel, long Offset, int Size);
void RDOSAPI RdosClearBreak(int Thread, int Reg);

void RDOSAPI RdosDebugTrace();
void RDOSAPI RdosDebugPace();
void RDOSAPI RdosDebugGo();
void RDOSAPI RdosDebugRun();
void RDOSAPI RdosDebugNext();

#endif

long long RDOSAPI RdosGetFreePhysical();
int RDOSAPI RdosGetFreeGdt();
int RDOSAPI RdosGetFreeSmallKernelLinear();
int RDOSAPI RdosGetFreeBigKernelLinear();
int RDOSAPI RdosGetFreeSmallLocalLinear();
int RDOSAPI RdosGetFreeBigLocalLinear();

int RDOSAPI RdosGetFreeHandles();
int RDOSAPI RdosGetFreeHandleMem();

int RDOSAPI RdosGetMaxComPort();
int RDOSAPI RdosOpenCom(char ID, long BaudRate, char Parity, char DataBits, char StopBits, int SendBufSize, int RecBufSize); 
void RDOSAPI RdosCloseCom(int Handle);
void RDOSAPI RdosFlushCom(int Handle);
char RDOSAPI RdosReadCom(int Handle);
int RDOSAPI RdosWriteCom(int Handle, char Val);
void RDOSAPI RdosEnableCts(int Handle);
void RDOSAPI RdosDisableCts(int Handle);
void RDOSAPI RdosEnableAutoRts(int Handle);
void RDOSAPI RdosDisableAutoRts(int Handle);
void RDOSAPI RdosSetDtr(int Handle);
void RDOSAPI RdosResetDtr(int Handle);
void RDOSAPI RdosSetRts(int Handle);
void RDOSAPI RdosResetRts(int Handle);
int RDOSAPI RdosGetReceiveBufferSpace(int Handle);
int RDOSAPI RdosGetSendBufferSpace(int Handle);
void RDOSAPI RdosWaitForSendCompletedCom(int Handle);
void RDOSAPI RdosResetCom(int Handle);

int RDOSAPI RdosGetMaxPrinters();
int RDOSAPI RdosOpenPrinter(char ID);
void RDOSAPI RdosClosePrinter(int Handle);
int RDOSAPI RdosGetPrinterName(int Handle, char *NameBuf);
int RDOSAPI RdosIsPrinterJammed(int Handle);
int RDOSAPI RdosIsPrinterPaperLow(int Handle);
int RDOSAPI RdosIsPrinterPaperEnd(int Handle);
int RDOSAPI RdosIsPrinterCutterJammed(int Handle);
int RDOSAPI RdosIsPrinterOk(int Handle);
int RDOSAPI RdosIsPrinterHeadLifted(int Handle);
int RDOSAPI RdosHasPrinterPaperInPresenter(int Handle);
int RDOSAPI RdosHasPrinterTemperatureError(int Handle);
int RDOSAPI RdosHasPrinterFeedError(int Handle);
void RDOSAPI RdosPrintTest(int Handle);
int RDOSAPI RdosCreatePrinterBitmap(int Handle, int Height);
void RDOSAPI RdosPrintBitmap(int Handle, int Bitmap);
void RDOSAPI RdosPresentPrinterMedia(int Handle, int Length);
void RDOSAPI RdosEjectPrinterMedia(int Handle);
void RDOSAPI RdosWaitForPrint(int Handle);
void RDOSAPI RdosResetPrinter(int Handle);

int RDOSAPI RdosGetMaxCardDev();
int RDOSAPI RdosOpenCardDev(char ID);
void RDOSAPI RdosCloseCardDev(int Handle);
int RDOSAPI RdosGetCardDevName(int Handle, char *NameBuf);
int RDOSAPI RdosIsCardDevOk(int Handle);
int RDOSAPI RdosIsCardDevBusy(int Handle);
int RDOSAPI RdosIsCardDevInserted(int Handle);
int RDOSAPI RdosHadCardDevInserted(int Handle);
void RDOSAPI RdosClearCardDevInserted(int Handle);
int RDOSAPI RdosWaitForCard(int Handle, char *Strip);

int RDOSAPI RdosOpenFile(const char *FileName, char Access);
int RDOSAPI RdosCreateFile(const char *FileName, int Attrib);
void RDOSAPI RdosCloseFile(int Handle);
int RDOSAPI RdosIsDevice(int Handle);
int RDOSAPI RdosDuplFile(int Handle);
long RDOSAPI RdosGetFileSize(int Handle);
void RDOSAPI RdosSetFileSize(int Handle, long Size);
long RDOSAPI RdosGetFilePos(int Handle);
void RDOSAPI RdosSetFilePos(int Handle, long Pos);
int RDOSAPI RdosReadFile(int Handle, void *Buf, int Size);
int RDOSAPI RdosWriteFile(int Handle, const void *Buf, int Size);
void RDOSAPI RdosGetFileTime(int Handle, unsigned long *MsbTime, unsigned long *LsbTime);
void RDOSAPI RdosSetFileTime(int Handle, unsigned long MsbTime, unsigned long LsbTime);

int RDOSAPI RdosCreateMapping(int Size);
int RDOSAPI RdosCreateNamedMapping(const char *Name, int Size); 
int RDOSAPI RdosCreateNamedFileMapping(const char *Name, int Size, int FileHandle);
int RDOSAPI RdosOpenNamedMapping(const char *Name);
void RDOSAPI RdosSyncMapping(int Handle);
void RDOSAPI RdosCloseMapping(int Handle);
void RDOSAPI RdosMapView(int Handle, int Offset, void *Base, int Size);
void RDOSAPI RdosUnmapView(int Handle);

int RDOSAPI RdosSetCurDrive(int Drive);
int RDOSAPI RdosGetCurDrive();
int RDOSAPI RdosSetCurDir(const char *PathName);
int RDOSAPI RdosGetCurDir(int Drive, char *PathName);
int RDOSAPI RdosMakeDir(const char *PathName);
int RDOSAPI RdosRemoveDir(const char *PathName);
int RDOSAPI RdosRenameFile(const char *ToName, const char *FromName);
int RDOSAPI RdosDeleteFile(const char *PathName);
int RDOSAPI RdosGetFileAttribute(const char *PathName, int *Attribute);
int RDOSAPI RdosSetFileAttribute(const char *PathName, int Attribute);
int RDOSAPI RdosOpenDir(const char *PathName);
void RDOSAPI RdosCloseDir(int Handle);
int RDOSAPI RdosReadDir(int Handle, int EntryNr, int MaxNameSize, char *PathName, long *FileSize, int *Attribute, unsigned long *MsbTime, unsigned long *LsbTime);

#ifdef __RDOS__     // these are only available in user-mode

void RDOSAPI RdosDefineFaultSave(int DiscNr, long StartSector, long Sectors);
void RDOSAPI RdosClearFaultSave();
int RDOSAPI RdosGetFaultThreadState(int ThreadNr, ThreadState *State);
int RDOSAPI RdosGetFaultThreadTss(int ThreadNr, Tss *tss);

int RDOSAPI RdosGetThreadState(int ThreadNr, ThreadState *State);
int RDOSAPI RdosSuspendThread(int Thread);
int RDOSAPI RdosSuspendAndSignalThread(int Thread);

#endif

int RDOSAPI RdosGetImageHeader(int Adapter, int Entry, TRdosObjectHeader *Header);
int RDOSAPI RdosGetImageData(int Adapter, int Entry, void *Buf);
int RDOSAPI RdosGetDeviceInfo(int CodeSel, char *Name, unsigned int *CodeSize, unsigned short int *DataSel, unsigned int *DataSize);

int RDOSAPI RdosGetSelectorInfo(int CodeSel, int *Limit, int *Bitness);

int RDOSAPI RdosHasHardReset();
void RDOSAPI RdosSoftReset();
void RDOSAPI RdosHardReset();
int RDOSAPI RdosPowerFailure();
int RDOSAPI RdosIsEmergencyStopped();
int RDOSAPI RdosGetCpuVersion(char *VendorStr, int *FeatureFlags, int *freq);
void RDOSAPI RdosGetVersion(int *Major, int *Minor, int *Release);
void RDOSAPI RdosCreateThread(void (*Start)(void *Param), const char *Name, void *Param, int StackSize);
void RDOSAPI RdosCreatePrioThread(void (*Start)(void *Param), int Prio, const char *Name, void *Param, int StackSize);
void RDOSAPI RdosTerminateThread();
int RDOSAPI RdosGetThreadHandle();

int RDOSAPI RdosHasGlobalTimer();
int RDOSAPI RdosGetCoreLoad(int Core, long long *NullTics, long long *CoreTics);
int RDOSAPI RdosGetCoreDuty(int Core, long long *CoreTics, long long *TotalTics);

int RDOSAPI RdosIs64BitExe(const char *prog);

#ifdef __RDOS__     // these are only available in user-mode

int RDOSAPI RdosExec(const char *prog, const char *param, const char *options);
int RDOSAPI RdosSpawn(const char *prog, const char *param, const char *startdir, const char *env, const char *options, int *thread);
int RDOSAPI RdosSpawnDebug(const char *prog, const char *param, const char *startdir, const char *env, const char *options, int *thread);
int RDOSAPI RdosFork(const char *name, const char *options);
void RDOSAPI RdosUnloadExe(int ExitCode);
void RDOSAPI RdosFreeProcessHandle(int handle);
int RDOSAPI RdosGetProcessExitCode(int handle);
void RDOSAPI RdosAddWaitForProcessEnd(int Handle, int ProcessHandle, void *ID);
int RDOSAPI RdosShowExceptionText();

#endif

void RDOSAPI RdosWaitMilli(int ms);
void RDOSAPI RdosWaitMicro(int us);
void RDOSAPI RdosWaitUntil(unsigned long msb, unsigned long lsb);

void RDOSAPI RdosGetSysTime(unsigned long *msb, unsigned long *lsb);
long long RDOSAPI RdosGetLongSysTime();
void RDOSAPI RdosGetTime(unsigned long *msb, unsigned long *lsb);
void RDOSAPI RdosSetTime(unsigned long msb, unsigned long lsb);
int RDOSAPI RdosDayOfWeek(int year, int month, int day);

void RDOSAPI RdosDosTimeDateToTics(unsigned short date, unsigned short time, unsigned long *msb, unsigned long *lsb);
void RDOSAPI RdosTicsToDosTimeDate(unsigned long msb, unsigned long lsb, unsigned short *date, unsigned short *time);

void RDOSAPI RdosDecodeMsbTics(unsigned long msb, int *year, int *month, int *day, int *hour);
void RDOSAPI RdosDecodeLsbTics(unsigned long lsb, int *min, int *sec, int *milli, int *micro); 

unsigned long RDOSAPI RdosCodeMsbTics(int year, int month, int day, int hour);
unsigned long RDOSAPI RdosCodeLsbTics(int min, int sec, int milli, int micro); 

void RDOSAPI RdosAddTics(unsigned long *msb, unsigned long *lsb, long tics);
void RDOSAPI RdosAddMicro(unsigned long *msb, unsigned long *lsb, long us);
void RDOSAPI RdosAddMilli(unsigned long *msb, unsigned long *lsb, long ms);
void RDOSAPI RdosAddSec(unsigned long *msb, unsigned long *lsb, long sec);
void RDOSAPI RdosAddMin(unsigned long *msb, unsigned long *lsb, long min);
void RDOSAPI RdosAddHour(unsigned long *msb, unsigned long *lsb, long hour);
void RDOSAPI RdosAddDay(unsigned long *msb, unsigned long *lsb, long day);
int RDOSAPI RdosSyncTime(long IP);

int RDOSAPI RdosCreateSection();
void RDOSAPI RdosDeleteSection(int Handle);
void RDOSAPI RdosEnterSection(int Handle);
void RDOSAPI RdosLeaveSection(int Handle);

int RDOSAPI RdosCreateWait();
void RDOSAPI RdosCloseWait(int Handle);
void * RDOSAPI RdosCheckWait(int Handle);
void * RDOSAPI RdosWaitForever(int Handle);
void * RDOSAPI RdosWaitTimeout(int Handle, int MillSec);
void * RDOSAPI RdosWaitUntilTimeout(int Handle, unsigned long msb, unsigned long lsb);
void RDOSAPI RdosStopWait(int Handle);
void RDOSAPI RdosRemoveWait(int Handle, void *ID);
void RDOSAPI RdosAddWaitForKeyboard(int Handle, void *ID);
void RDOSAPI RdosAddWaitForMouse(int Handle, void *ID);
void RDOSAPI RdosAddWaitForCom(int Handle, int ComHandle, void *ID);
void RDOSAPI RdosAddWaitForAdc(int Handle, int AdcHandle, void *ID);
void RDOSAPI RdosAddWaitForSysLog(int Handle, int SyslogHandle, void *ID);

int RDOSAPI RdosCreateSignal();
void RDOSAPI RdosResetSignal(int Handle);
int RDOSAPI RdosIsSignalled(int Handle);
void RDOSAPI RdosSetSignal(int Handle);
void RDOSAPI RdosFreeSignal(int Handle);
void RDOSAPI RdosAddWaitForSignal(int Handle, int SignalHandle, void *ID);

long RDOSAPI RdosGetIp();
long RDOSAPI RdosGetGateway();
int RDOSAPI RdosNameToIp(const char *HostName);
int RDOSAPI RdosIpToName(int Ip, char *HostName, int MaxSize);

void RDOSAPI RdosSendUdp(long RemoteIp, int LocalPort, int RemotePort, const char *Buf, int Size);
int RDOSAPI RdosBroadcastQueryUdp(const char *ReqBuf, int ReqSize, char *ReplyBuf, int *ReplySize, int DestPort, long Timeout);

int RDOSAPI RdosOpenUdpConnection(int RemoteIp, int LocalPort, int RemotePort);
void RDOSAPI RdosCloseUdpConnection(int Handle);
void RDOSAPI RdosSendUdpConnection(int Handle, const char *Buf, int Size);
int RDOSAPI RdosPeekUdpConnection(int Handle);
int RDOSAPI RdosReadUdpConnection(int Handle, void *Buf, int Size);
void RDOSAPI RdosAddWaitForUdpConnection(int Handle, int ConHandle, void *ID);

int RDOSAPI RdosCreateTcpListen(int Port, int MaxConnections, int BufferSize);
int RDOSAPI RdosGetTcpListen(int Handle);
void RDOSAPI RdosCloseTcpListen(int Handle);
void RDOSAPI RdosAddWaitForTcpListen(int Handle, int ConHandle, void *ID);

int RDOSAPI RdosOpenTcpConnection(int RemoteIp, int LocalPort, int RemotePort, int Timeout, int BufferSize);
int RDOSAPI RdosWaitForTcpConnection(int Handle, long Timeout);
void RDOSAPI RdosAddWaitForTcpConnection(int Handle, int ConHandle, void *ID);
void RDOSAPI RdosCloseTcpConnection(int Handle);
void RDOSAPI RdosDeleteTcpConnection(int Handle);
void RDOSAPI RdosAbortTcpConnection(int Handle);
void RDOSAPI RdosPushTcpConnection(int Handle);
int RDOSAPI RdosIsTcpConnectionClosed(int Handle);
int RDOSAPI RdosIsTcpConnectionIdle(int Handle);
long RDOSAPI RdosGetRemoteTcpConnectionIP(int Handle);
int RDOSAPI RdosGetRemoteTcpConnectionPort(int Handle);
int RDOSAPI RdosGetLocalTcpConnectionPort(int Handle);
int RDOSAPI RdosReadTcpConnection(int Handle, void *Buf, int Size);
int RDOSAPI RdosWriteTcpConnection(int Handle, const void *Buf, int Size);
int RDOSAPI RdosPollTcpConnection(int Handle);

int RDOSAPI RdosGetLocalMailslot(const char *Name);
int RDOSAPI RdosGetRemoteMailslot(long Ip, const char *Name);
void RDOSAPI RdosFreeMailslot(int Handle);
int RDOSAPI RdosSendMailslot(int Handle, const void *Msg, int Size, void *ReplyBuf, int MaxReplySize);

void RDOSAPI RdosDefineMailslot(const char *Name, int MaxSize);
int RDOSAPI RdosReceiveMailslot(void *Msg);
void RDOSAPI RdosReplyMailslot(const void *Msg, int Size);

void RDOSAPI RdosSetFocus(char FocusKey);
char RDOSAPI RdosGetFocus();

int RDOSAPI RdosSetKeyMap(const char *MapName);
void RDOSAPI RdosGetKeyMap(char *MapName);
void RDOSAPI RdosClearKeyboard();
int RDOSAPI RdosPollKeyboard();
int RDOSAPI RdosReadKeyboard();
int RDOSAPI RdosGetKeyboardState();
void RDOSAPI RdosPutKeyboard(int KeyCode, int VirtualKey, int ScanCode);
int RDOSAPI RdosPeekKeyEvent(int *ExtKey, int *KeyState, int *VirtualKey, int *ScanCode);
int RDOSAPI RdosReadKeyEvent(int *ExtKey, int *KeyState, int *VirtualKey, int *ScanCode);

void RDOSAPI RdosHideMouse();
void RDOSAPI RdosShowMouse();
void RDOSAPI RdosGetMousePosition(int *x, int *y);
void RDOSAPI RdosSetMousePosition(int x, int y);
void RDOSAPI RdosSetMouseWindow(int StartX, int StartY, int EndX, int EndY);
void RDOSAPI RdosSetMouseMickey(int x, int y);
int RDOSAPI RdosGetLeftButton();
int RDOSAPI RdosGetRightButton();
void RDOSAPI RdosGetLeftButtonPressPosition(int *x, int *y);
void RDOSAPI RdosGetRightButtonPressPosition(int *x, int *y);
void RDOSAPI RdosGetLeftButtonReleasePosition(int *x, int *y);
void RDOSAPI RdosGetRightButtonReleasePosition(int *x, int *y);

void RDOSAPI RdosGetCursorPosition(int *Row, int *Col);
void RDOSAPI RdosSetCursorPosition(int Row, int Col);
void RDOSAPI RdosWriteChar(char ch);
void RDOSAPI RdosWriteSizeString(const char *Buf, int Size);
void RDOSAPI RdosWriteAttributeString(int Row, int Col, const short int *Buf, int Size);
void RDOSAPI RdosWriteString(const char *Buf);
int RDOSAPI RdosReadLine(char *Buf, int MaxSize);

int RDOSAPI RdosPing(long Node, long Timeout);

int RDOSAPI RdosGetIdeDisc(int UnitNr);
int RDOSAPI RdosGetFloppyDisc(int UnitNr);

int RDOSAPI RdosSetDiscInfo(int DiscNr, int SectorSize, long Sectors, int BiosSectorsPerCyl, int BiosHeads);
int RDOSAPI RdosGetDiscInfo(int DiscNr, int *SectorSize, long *Sectors, int *BiosSectorsPerCyl, int *BiosHeads);
int RDOSAPI RdosReadDisc(int DiscNr, long Sector, char *Buf, int Size);
int RDOSAPI RdosWriteDisc(int DiscNr, long Sector, const char *Buf, int Size);

void RDOSAPI RdosGetRdfsInfo(void *CryptTab, void *KeyTab, void *ExtentSizeTab);
void RDOSAPI RdosDemandLoadDrive(int DriveNr);
int RDOSAPI RdosFormatDrive(int DiscNr, long StartSector, int Size, const char *FsName);

int RDOSAPI RdosAllocateFixedDrive(int DriveNr);
int RDOSAPI RdosAllocateStaticDrive();
int RDOSAPI RdosAllocateDynamicDrive();

int RDOSAPI RdosGetDriveInfo(int DriveNr, long *FreeUnits, int *BytesPerUnit, long *TotalUnits);
int RDOSAPI RdosGetDriveDiscParam(int DriveNr, int *DiscNr, long *StartSector, long *TotalSectors);

int RDOSAPI RdosCreateFileDrive(int Drive, long Size, const char *FsName, const char *FileName);
int RDOSAPI RdosOpenFileDrive(int Drive, const char *FileName);

int RDOSAPI RdosCreateCrc(unsigned short int CrcPoly);
void RDOSAPI RdosCloseCrc(int Handle);
unsigned short int RDOSAPI RdosCalcCrc(int Handle, unsigned short int CrcVal, const char *Buf, int Size);

#ifdef __RDOS__     // these are only available in user-mode

int RDOSAPI RdosGetModuleHandle();
const char *RDOSAPI RdosGetExeName();
const char *RDOSAPI RdosGetCmdLine();
const char *RDOSAPI RdosGetOptions();
int RDOSAPI RdosLoadDll(const char *Name);
void RDOSAPI RdosFreeDll(int handle);
int RDOSAPI RdosGetModuleName(int handle, char *Buf, int Size);
int RDOSAPI RdosReadResource(int handle, int ID, char *Buf, int Size);
int RDOSAPI RdosReadBinaryResource(int handle, int ID, char *Buf, int Size);

void * RDOSAPI RdosGetModuleProc(int handle, const char *ProcName);
char RDOSAPI RdosGetModuleFocusKey(int handle);

void RDOSAPI RdosAddWaitForDebugEvent(int Handle, int ProcessHandle, void *ID);
char RDOSAPI RdosGetDebugEvent(int handle, int *thread);
void RDOSAPI RdosGetDebugEventData(int handle, void *buf);
void RDOSAPI RdosClearDebugEvent(int handle);
void RDOSAPI RdosContinueDebugEvent(int handle, int thread);

#endif

void RDOSAPI RdosRemoteDebug(long IP);

int RDOSAPI RdosOpenSyslog();
void RDOSAPI RdosCloseSyslog(int handle);
int RDOSAPI RdosGetSyslog(int handle, int *severity, unsigned long *msb, unsigned long *lsb, char *buf, int size);

int RDOSAPI RdosOpenAdc(int channel);
void RDOSAPI RdosCloseAdc(int handle);
void RDOSAPI RdosDefineAdcTime(int handle, unsigned long msg, unsigned long lsb);
long RDOSAPI RdosReadAdc(int handle);

int RDOSAPI RdosReadSerialLines(int device, int *val);
int RDOSAPI RdosToggleSerialLine(int device, int line);
int RDOSAPI RdosReadSerialVal(int device, int line, int *val);
int RDOSAPI RdosWriteSerialVal(int device, int line, int val);
int RDOSAPI RdosReadSerialRaw(int device, int line, int *val);
int RDOSAPI RdosWriteSerialRaw(int device, int line, int val);

int RDOSAPI RdosOpenSysEnv();
int RDOSAPI RdosOpenProcessEnv();
void RDOSAPI RdosCloseEnv(int handle);
void RDOSAPI RdosAddEnvVar(int handle, const char *var, const char *value);
void RDOSAPI RdosDeleteEnvVar(int handle, const char *var);
int RDOSAPI RdosFindEnvVar(int handle, const char *var, char *value);
int RDOSAPI RdosGetEnvSize(int handle);
void RDOSAPI RdosGetEnvData(int handle, char *buf);
void RDOSAPI RdosSetEnvData(int handle, const char *buf);

int RDOSAPI RdosOpenSysIni();
int RDOSAPI RdosOpenIni(const char *filename);
void RDOSAPI RdosCloseIni(int handle);
int RDOSAPI RdosGotoIniSection(int handle, const char *name);
int RDOSAPI RdosRemoveIniSection(int handle, const char *name);
int RDOSAPI RdosReadIni(int handle, const char *var, char *str, int maxsize);
int RDOSAPI RdosWriteIni(int handle, const char *var, const char *str);
int RDOSAPI RdosDeleteIni(int handle, const char *var);
int RDOSAPI RdosGotoFirstIniVar(int handle);
int RDOSAPI RdosGotoNextIniVar(int handle);
int RDOSAPI RdosGetCurrIniVar(int handle, char *var, int maxsize);

void RDOSAPI RdosEnableStatusLED();
void RDOSAPI RdosDisableStatusLED();

void RDOSAPI RdosStartWatchdog(int timeout);
void RDOSAPI RdosKickWatchdog();
void RDOSAPI RdosStopWatchdog();

void RDOSAPI RdosStartNetCapture(int FileHandle);
void RDOSAPI RdosStopNetCapture();

int RDOSAPI RdosGetUsbDevice(int Controller, int Device, void *ptr, int maxsize);
int RDOSAPI RdosGetUsbConfig(int Controller, int Device, int Config, void *ptr, int maxsize);
int RDOSAPI RdosOpenUsbPipe(int Controller, int Device, int Pipe);
void RDOSAPI RdosCloseUsbPipe(int handle);
void RDOSAPI RdosResetUsbPipe(int handle);
void RDOSAPI RdosLockUsbPipe(int handle);
void RDOSAPI RdosUnlockUsbPipe(int handle);
void RDOSAPI RdosAddWaitForUsbPipe(int Handle, int PipeHandle, void *ID);
void RDOSAPI RdosWriteUsbControl(int Handle, const char *buf, int size);
void RDOSAPI RdosReqUsbData(int Handle, char *buf, int maxsize);
int RDOSAPI RdosGetUsbDataSize(int Handle);
void RDOSAPI RdosWriteUsbData(int Handle, const char *buf, int size);
void RDOSAPI RdosReqUsbStatus(int Handle);
void RDOSAPI RdosWriteUsbStatus(int Handle);
int RDOSAPI RdosIsUsbTransactionDone(int Handle);
int RDOSAPI RdosWasUsbTransactionOk(int Handle);

int RDOSAPI RdosOpenHid(int Controller, int Device);
void RDOSAPI RdosCloseHid(int handle);
int RDOSAPI RdosGetHidPipe(int Handle);
int RDOSAPI RdosReadHid(int Handle, char *buf, int size, int ms);
int RDOSAPI RdosWriteHid(int Handle, const char *buf, int size);

int RDOSAPI RdosGetAllocatedUsbBlocks();
int RDOSAPI RdosGetUsbCloseCount();

int RDOSAPI RdosHasICSP();
int RDOSAPI RdosOpenICSP(int DeviceID);
void RDOSAPI RdosCloseICSP(int Handle);
void RDOSAPI RdosResetICSP(int Handle);
int RDOSAPI RdosWriteICSPCommand(int Handle, int Cmd);
int RDOSAPI RdosWriteICSPData(int Handle, int Data);
int RDOSAPI RdosReadICSPData(int Handle, int *Data);

void RDOSAPI RdosSetCodecGpio0(int Value);

void RDOSAPI RdosGetOutputVolume(int *Left, int *Right);
void RDOSAPI RdosSetOutputVolume(int Left, int Right);

void RDOSAPI RdosGetMasterVolume(int *Left, int *Right);
void RDOSAPI RdosSetMasterVolume(int Left, int Right);

void RDOSAPI RdosGetLineOutVolume(int *Left, int *Right);
void RDOSAPI RdosSetLineOutVolume(int Left, int Right);

int RDOSAPI RdosHasAudio();

int RDOSAPI RdosCreateAudioOutChannel(int SampleRate, int Bits, int Volume);
void RDOSAPI RdosCloseAudioOutChannel(int Handle);
void RDOSAPI RdosWriteAudio(int Handle, int Size, const int *LSamples, const int*RSamples); 

int RDOSAPI RdosOpenFm(int SampleRate);
void RDOSAPI RdosCloseFm(int FmHandle);
void RDOSAPI RdosFmWait(int FmHandle, int Samples);

int RDOSAPI RdosCreateFmInstrument(int FmHandle, int C, int M, long double Beta);
void RDOSAPI RdosFreeFmInstrument(int Handle);
void RDOSAPI RdosSetFmAttack(int Handle, int Samples);
void RDOSAPI RdosSetFmSustain(int Handle, int VolumeHalf, int BetaHalf);
void RDOSAPI RdosSetFmRelease(int Handle, int VolumeHalf, int BetaHalf);
void RDOSAPI RdosPlayFmNote(int Handle, long double Freq, int PeakLeftVolume, int PeakRightVolume, int SustainSamples);

int RDOSAPI RdosHasTouch();

#ifdef __cplusplus
}
#endif

#endif

#ifdef __GNUC__
#include "rdosgcc.h"
#endif

#ifdef __WATCOMC__

#ifdef __FLAT__
#include "owflat.h"
#endif

#ifdef __COMPACT__
#include "owcomp.h"
#endif

#endif

#endif

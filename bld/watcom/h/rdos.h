/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2009-2010 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
*
* Description:  RDOS API header file
*
****************************************************************************/

#ifndef _RDOS_H
#define _RDOS_H

#pragma pack( push, 1 )

#ifdef __cplusplus
extern "C" {
#endif

#include "rdoshdr.h"

#ifdef __WATCOMC__

#include "machtype.h"
#include "rdu.h"
#define RDOSAPI

#define real_math   xreal

#else

#define RDOSAPI __stdcall
#define real_math   long double

#endif

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


// API functions

void RDOSAPI RdosTestGate();

#pragma aux RdosTestGate = \
    CallGate_test_gate;

void RDOSAPI RdosDebug();
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

int RDOSAPI RdosGetFreePhysical();
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

int RDOSAPI RdosGetMaxPrinters();
int RDOSAPI RdosOpenPrinter(char ID);
void RDOSAPI RdosClosePrinter(int Handle);
int RDOSAPI RdosGetPrinterName(int Handle, char *NameBuf);
int RDOSAPI RdosIsPrinterJammed(int Handle);
int RDOSAPI RdosIsPrinterPaperLow(int Handle);
int RDOSAPI RdosIsPrinterPaperEnd(int Handle);
int RDOSAPI RdosIsPrinterOk(int Handle);
int RDOSAPI RdosIsPrinterHeadLifted(int Handle);
int RDOSAPI RdosHasPrinterPaperInPresenter(int Handle);
void RDOSAPI RdosPrintTest(int Handle);
int RDOSAPI RdosCreatePrinterBitmap(int Handle, int Height);
void RDOSAPI RdosPrintBitmap(int Handle, int Bitmap);
void RDOSAPI RdosPresentPrinterMedia(int Handle, int Length);
void RDOSAPI RdosEjectPrinterMedia(int Handle);
void RDOSAPI RdosWaitForPrint(int Handle);

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
int RDOSAPI RdosRemoveIniSection(int handle);
int RDOSAPI RdosReadIni(int handle, const char *var, char *str, int maxsize);
int RDOSAPI RdosWriteIni(int handle, const char *var, const char *str);
int RDOSAPI RdosDeleteIni(int handle, const char *var);

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

int RDOSAPI RdosOpenICSP(int DeviceID);
void RDOSAPI RdosCloseICSP(int Handle);
int RDOSAPI RdosWriteICSPCommand(int Handle, int Cmd);
int RDOSAPI RdosWriteICSPData(int Handle, int Data);
int RDOSAPI RdosReadICSPData(int Handle, int *Data);

void RDOSAPI RdosSetCodecGpio0(int Value);

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

#ifdef __cplusplus
}
#endif

#ifdef __WATCOMC__

#ifdef __FLAT__
 
/* 32-bit compiler */

// check carry flag, and set eax=0 if set and eax=1 if clear
#define CarryToBool 0x73 4 0x33 0xC0 0xEB 5 0xB8 1 0 0 0

// check carry flag, and set ebx=0 if set and ebx=bx if clear
#define ValidateHandle 0x73 2 0x33 0xDB 0xF 0xB7 0xDB

// check carry flag, and set eax=0 if set
#define ValidateEax 0x73 2 0x33 0xC0

// check carry flag, and set ecx=0 if set
#define ValidateEcx 0x73 2 0x33 0xC9

// check carry flag, and set edx=0 if set
#define ValidateEdx 0x73 2 0x33 0xD2

// check carry flag, and set esi=0 if set
#define ValidateEsi 0x73 2 0x33 0xF6

// check carry flag, and set edi=0 if set
#define ValidateEdi 0x73 2 0x33 0xFF

// check disc id, set to -1 on carry, extend to eax
#define ValidateDisc 0x73 2 0xB0 0xFF 0xF 0xBE 0xC0

#pragma aux RdosDebug = \
    CallGate_debug_break;

#pragma aux RdosLoad32 = \
    CallGate_load_device32  \

#pragma aux RdosSwapShort = \
    "xchg al,ah"    \
    parm [ax]   \
    value [ax];

#pragma aux RdosSwapLong = \
    "xchg al,ah"    \
    "rol eax,16"    \
    "xchg al,ah"    \
    parm [eax]   \
    value [eax];

#pragma aux RdosGetCharSize = \
    "mov al,1" \
    "mov ah,[edi]" \
    "test ah,80h" \
    "jz size_ok" \
    "inc al" \
    "test ah,20h" \
    "jz size_ok" \
    "inc al" \
    "test ah,10h" \
    "jz size_ok" \
    "inc al" \
    "size_ok: " \
    "movzx eax,al" \
    parm [edi]   \
    value [eax];

#pragma aux RdosGetLongRandom = \
    CallGate_get_random  \
    value [eax];

#pragma aux RdosGetRandom = \
    CallGate_get_random  \
    "mul edx" \
    parm [edx] \
    value [edx] \
    modify [eax];

#pragma aux RdosGetAcpiStatus = \
    CallGate_get_acpi_status  \
    "jnc AcpiDone" \
    "mov eax,-1" \
    "AcpiDone: " \
    value [eax];

#pragma aux RdosGetAcpiObject = \
    CallGate_get_acpi_object  \
    CarryToBool \
    parm [eax] [edi] \
    value [eax];

#pragma aux RdosGetAcpiMethod = \
    CallGate_get_acpi_method  \
    CarryToBool \
    parm [eax] [edx] [edi] \
    value [eax];

#pragma aux RdosGetAcpiDevice = \
    CallGate_get_acpi_device  \
    CarryToBool \
    parm [eax] [edi] \
    value [eax];

#pragma aux RdosGetAcpiDeviceIrq = \
    "push ecx" \
    "push edx" \
    CallGate_get_acpi_device_irq  \
    "jc IrqFail" \
    "movzx ecx,ah" \
    "mov [ebx],ecx" \
    "movzx ecx,dl" \
    "mov [esi],ecx" \
    "movzx ecx,dh" \
    "mov [edi],ecx" \
    "movzx eax,al" \
    "jmp IrqDone" \
    "IrqFail:" \
    "mov eax,-1" \
    "IrqDone:" \
    "pop edx" \
    "pop ecx" \
    parm [eax] [edx] [ebx] [esi] [edi] \
    value [eax];

#pragma aux RdosGetAcpiDeviceIo = \
    "push ecx" \
    "push edi" \
    "push esi" \
    CallGate_get_acpi_device_io  \
    "jc IoFail" \
    "movzx eax,si" \
    "pop esi" \
    "mov [esi],eax" \
    "movzx eax,di" \
    "pop edi" \
    "mov [edi],eax" \
    "movzx eax,cx" \
    "jmp IoDone" \
    "IoFail:" \
    "pop esi" \
    "pop edi" \
    "xor eax,eax" \
    "IoDone:" \
    "pop ecx" \
    parm [eax] [edx] [esi] [edi] \
    value [eax];

#pragma aux RdosGetAcpiDeviceMem = \
    "push ecx" \
    "push edi" \
    "push esi" \
    CallGate_get_acpi_device_mem  \
    "jc MemFail" \
    "mov eax,esi" \
    "pop esi" \
    "mov [esi],eax" \
    "mov eax,edi" \
    "pop edi" \
    "mov [edi],eax" \
    "mov eax,ecx" \
    "jmp MemDone" \
    "MemFail:" \
    "pop esi" \
    "pop edi" \
    "xor eax,eax" \
    "MemDone:" \
    "pop ecx" \
    parm [eax] [edx] [esi] [edi] \
    value [eax];
    
#pragma aux RdosGetPciDeviceName = \
    CallGate_get_pci_device_name  \
    CarryToBool \
    parm [eax] [edi] \
    value [eax];

#pragma aux RdosGetPciDeviceInfo = \
    "push ecx" \
    "push ebx" \
    "push esi" \
    CallGate_get_pci_device_info  \
    "jc Fail" \
    "movzx eax,bh" \
    "pop esi" \
    "mov [esi],eax" \
    "movzx eax,bl" \
    "pop ebx" \
    "mov [ebx],eax" \
    "movzx eax,ch" \
    "pop ecx" \
    "mov [ecx],eax" \
    "mov eax,1" \
    "jmp Done" \
    "Fail:" \
    "pop esi" \
    "pop ebx" \
    "pop ecx" \
    "xor eax,eax" \
    "Done:" \
    parm [eax] [esi] [ebx] [ecx] \
    value [eax];

#pragma aux RdosGetPciDeviceVendor = \
    "push edx" \
    CallGate_get_pci_device_vendor  \
    "jc Fail" \
    "movzx eax,ax" \
    "mov [esi],eax" \
    "movzx edx,dx" \
    "mov [edi],edx" \
    "mov eax,1" \
    "jmp Done" \
    "Fail:" \
    "xor eax,eax" \
    "Done:" \
    "pop edx" \    
    parm [eax] [esi] [edi] \
    value [eax];

#pragma aux RdosGetPciDeviceClass = \
    CallGate_get_pci_device_class  \
    "jc Fail" \
    "push edx" \
    "movzx edx,ah" \
    "mov [esi],edx" \
    "movzx edx,al" \
    "mov [edi],edx" \
    "mov eax,1" \
    "pop edx" \
    "jmp Done" \
    "Fail:" \
    "xor eax,eax" \
    "Done:" \
    parm [eax] [esi] [edi] \
    value [eax];

#pragma aux RdosGetPciDeviceIrq = \
    CallGate_get_pci_device_irq  \
    "jc Fail" \
    "movzx eax,al" \
    "jmp Done" \
    "Fail:" \
    "mov eax,-1" \
    "Done:" \
    parm [eax] \
    value [eax];
    
#pragma aux RdosGetCpuTemperature = \
    CallGate_get_cpu_temperature  \
    "jnc TempDone" \
    "xor eax,eax" \
    "TempDone: " \
    value [eax];

#pragma aux RdosSetTextMode = \
    "mov ax,3"  \
    CallGate_set_video_mode  \
    modify [ax];

#pragma aux RdosSetVideoMode = \
    "push ebp" \
    "push esi" \
    "push edi" \
    "push eax" \
    "mov ax,[eax]" \
    "push ecx" \
    "mov cx,[ecx]" \
    "push edx" \
    "mov dx,[edx]" \
    CallGate_get_video_mode \
    ValidateEax \
    CallGate_set_video_mode  \
    "pop ebp" \
    "movzx edx,dx" \
    "mov [ebp],edx" \
    "pop ebp" \
    "movzx ecx,cx" \
    "mov [ebp],ecx" \
    "pop ebp" \
    "movzx eax,ax" \
    "mov [ebp],eax" \
    "pop ebp" \
    "mov [ebp],edi" \
    "pop ebp" \
    "movzx esi,si" \
    "mov [ebp],esi" \
    "pop ebp" \
    ValidateHandle \
    parm [eax] [ecx] [edx] [esi] [edi] \
    value [ebx] \
    modify [eax ecx edx esi edi];

#pragma aux RdosSetClipRect = \
    CallGate_set_clip_rect  \
    parm [ebx] [ecx] [edx] [esi] [edi];

#pragma aux RdosClearClipRect = \
    CallGate_clear_clip_rect  \
    parm [ebx];

#pragma aux RdosSetDrawColor = \
    CallGate_set_drawcolor  \
    parm [ebx] [eax];

#pragma aux RdosSetLGOP = \
    CallGate_set_lgop  \
    parm [ebx] [eax];

#pragma aux RdosSetHollowStyle = \
    CallGate_set_hollow_style  \
    parm [ebx];

#pragma aux RdosSetFilledStyle = \
    CallGate_set_filled_style  \
    parm [ebx];

#pragma aux RdosAnsiToUtf8 = \
    CallGate_ansi_to_utf8  \
    parm [esi] [edi] [ecx]  \
    value [eax];

#pragma aux RdosUtf8ToAnsi = \
    CallGate_utf8_to_ansi  \
    parm [esi] [edi] [ecx]  \
    value [eax];

#pragma aux RdosOpenFont = \
    CallGate_open_font  \
    ValidateHandle \
    parm [edx] [eax]  \
    value [ebx];

#pragma aux RdosCloseFont = \
    CallGate_close_font  \
    parm [ebx];

#pragma aux RdosGetStringMetrics = \
    CallGate_get_string_metrics  \
    "movzx ecx,cx" \
    "movzx edx,dx" \
    "mov [eax],ecx" \
    "mov [esi],edx" \
    parm [ebx] [edi] [eax] [esi] \
    modify [ecx edx];

#pragma aux RdosSetFont = \
    CallGate_set_font  \
    parm [ebx] [eax];

#pragma aux RdosGetPixel = \
    CallGate_get_pixel  \
    parm [ebx] [ecx] [edx]  \
    value [eax];

#pragma aux RdosSetPixel = \
    CallGate_set_pixel  \
    parm [ebx] [ecx] [edx];

// Blit here
// DrawMask here

#pragma aux RdosDrawLine = \
    CallGate_draw_line  \
    parm [ebx] [ecx] [edx] [esi] [edi];

#pragma aux RdosDrawString = \
    CallGate_draw_string  \
    parm [ebx] [ecx] [edx] [edi];

#pragma aux RdosDrawRect = \
    CallGate_draw_rect  \
    parm [ebx] [ecx] [edx] [esi] [edi];

#pragma aux RdosDrawEllipse = \
    CallGate_draw_ellipse  \
    parm [ebx] [ecx] [edx] [esi] [edi];

#pragma aux RdosCreateBitmap = \
    CallGate_create_bitmap  \
    ValidateHandle \
    parm [eax] [ecx] [edx] \
    value [ebx];

#pragma aux RdosDuplicateBitmapHandle = \
    CallGate_dup_bitmap_handle  \
    ValidateHandle  \
    parm [ebx]  \
    value [ebx];

#pragma aux RdosCloseBitmap = \
    CallGate_close_bitmap  \
    parm [ebx];

#pragma aux RdosCreateStringBitmap = \
    CallGate_create_string_bitmap  \
    ValidateHandle  \
    parm [ebx] [edi]    \
    value [ebx];


// GetBitmapInfo here

#pragma aux RdosCreateSprite = \
    CallGate_create_sprite  \
    ValidateHandle  \
    parm [ebx] [ecx] [edx] [eax]    \
    value [ebx];

#pragma aux RdosCloseSprite = \
    CallGate_close_sprite  \
    parm [ebx];

#pragma aux RdosShowSprite = \
    CallGate_show_sprite  \
    parm [ebx];

#pragma aux RdosHideSprite = \
    CallGate_hide_sprite  \
    parm [ebx];

#pragma aux RdosMoveSprite = \
    CallGate_move_sprite  \
    parm [ebx] [ecx] [edx];

#pragma aux RdosSetForeColor = \
    CallGate_set_forecolor  \
    parm [eax];

#pragma aux RdosSetBackColor = \
    CallGate_set_backcolor  \
    parm [eax];

#pragma aux RdosAllocateMem = \
    CallGate_allocate_app_mem  \
    ValidateEdx \
    parm [eax]  \
    value [edx];

#pragma aux RdosFreeMem = \
    CallGate_free_app_mem  \
    parm [edx];

#pragma aux RdosAllocateDebugMem = \
    CallGate_allocate_debug_app_mem  \
    ValidateEdx \
    parm [eax]  \
    value [edx];

#pragma aux RdosFreeDebugMem = \
    CallGate_free_debug_app_mem  \
    parm [edx];

#pragma aux RdosGetThreadLinear = \
    CallGate_get_thread_linear  \
    ValidateEdx \
    parm [ebx] [edx] [esi] \
    value [edx];

#pragma aux RdosReadThreadMem = \
    CallGate_read_thread_mem  \
    parm [ebx] [edx] [esi] [edi] [ecx] \
    value [eax];

#pragma aux RdosWriteThreadMem = \
    CallGate_write_thread_mem  \
    parm [ebx] [edx] [esi] [edi] [ecx] \
    value [eax];

#pragma aux RdosGetDebugThread = \
    CallGate_get_debug_thread  \
    ValidateEax \
    "movzx eax,ax"  \
    value [eax];

#pragma aux RdosGetThreadTss = \
    CallGate_get_thread_tss  \
    parm [ebx] [edi];

#pragma aux RdosSetThreadTss = \
    CallGate_set_thread_tss  \
    parm [ebx] [edi];

#pragma aux RdosRemoteDebug = \
    CallGate_remote_debug  \
    parm [edx];

#pragma aux RdosDebugTrace = \
    CallGate_debug_trace;

#pragma aux RdosDebugPace = \
    CallGate_debug_pace;

#pragma aux RdosDebugGo = \
    CallGate_debug_go;

#pragma aux RdosDebugRun = \
    CallGate_debug_run;

#pragma aux RdosDebugNext = \
    CallGate_debug_next;

#pragma aux RdosSetCodeBreak = \
    CallGate_set_code_break  \
    CarryToBool \
    parm [ebx] [eax] [si] [edi] \
    value [eax];

#pragma aux RdosSetReadDataBreak = \
    CallGate_set_read_data_break  \
    CarryToBool \
    parm [ebx] [eax] [si] [edi] [ecx] \
    value [eax];

#pragma aux RdosSetWriteDataBreak = \
    CallGate_set_write_data_break  \
    CarryToBool \
    parm [ebx] [eax] [si] [edi] [ecx] \
    value [eax];

#pragma aux RdosClearBreak = \
    CallGate_clear_break  \
    parm [ebx] [eax];

#pragma aux RdosGetFreePhysical = \
    CallGate_get_free_physical  \
    value [eax];

#pragma aux RdosGetFreeGdt = \
    CallGate_get_free_gdt  \
    "movzx eax,ax"  \
    value [eax];

#pragma aux RdosGetFreeSmallKernelLinear = \
    CallGate_available_small_linear  \
    value [eax];

#pragma aux RdosGetFreeBigKernelLinear = \
    CallGate_available_big_linear  \
    value [eax];

#pragma aux RdosGetFreeSmallLocalLinear = \
    CallGate_available_small_local_linear  \
    value [eax];

#pragma aux RdosGetFreeBigLocalLinear = \
    CallGate_available_big_local_linear  \
    value [eax];

#pragma aux RdosGetMaxComPort = \
    CallGate_get_max_com_port  \
    "movzx eax,al"  \
    value [eax];

#pragma aux RdosOpenCom = \
    CallGate_open_com  \
    ValidateHandle  \
    parm [al] [ecx] [bh] [ah] [bl] [esi] [edi]  \
    value [ebx];

#pragma aux RdosCloseCom = \
    CallGate_close_com  \
    parm [ebx];

#pragma aux RdosFlushCom = \
    CallGate_flush_com  \
    parm [ebx];

#pragma aux RdosReadCom = \
    CallGate_read_com  \
    parm [ebx]  \
    value [al];

#pragma aux RdosWriteCom = \
    CallGate_write_com  \
    "movsx eax,al"  \
    parm [ebx] [al] \
    value [eax];

#pragma aux RdosEnableCts = \
    CallGate_enable_cts  \
    parm [ebx];

#pragma aux RdosDisableCts = \
    CallGate_disable_cts  \
    parm [ebx];

#pragma aux RdosEnableAutoRts = \
    CallGate_enable_auto_rts  \
    parm [ebx];

#pragma aux RdosDisableAutoRts = \
    CallGate_disable_auto_rts  \
    parm [ebx];

#pragma aux RdosSetDtr = \
    CallGate_set_dtr  \
    parm [ebx];

#pragma aux RdosResetDtr = \
    CallGate_reset_dtr  \
    parm [ebx];

#pragma aux RdosSetRts = \
    CallGate_set_rts  \
    parm [ebx];

#pragma aux RdosResetRts = \
    CallGate_reset_rts  \
    parm [ebx];

#pragma aux RdosGetReceiveBufferSpace = \
    CallGate_get_com_receive_space  \
    ValidateEax \
    parm [ebx]  \
    value [eax];

#pragma aux RdosGetSendBufferSpace = \
    CallGate_get_com_send_space  \
    ValidateEax \
    parm [ebx]  \
    value [eax];

#pragma aux RdosWaitForSendCompletedCom = \
    CallGate_wait_for_send_completed_com  \
    parm [ebx];

#pragma aux RdosGetMaxPrinters = \
    CallGate_get_max_printer  \
    "movzx eax,al"  \
    value [eax];

#pragma aux RdosOpenPrinter = \
    CallGate_open_printer  \
    ValidateHandle  \
    parm [al]  \
    value [ebx];

#pragma aux RdosClosePrinter = \
    CallGate_close_printer  \
    parm [ebx];

#pragma aux RdosGetPrinterName = \
    CallGate_get_printer_name  \
    CarryToBool \
    parm [ebx] [edi] \
    value [eax];

#pragma aux RdosIsPrinterJammed = \
    CallGate_is_printer_jammed  \
    "cmc" \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosIsPrinterPaperLow = \
    CallGate_is_printer_paper_low  \
    "cmc" \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosIsPrinterPaperEnd = \
    CallGate_is_printer_paper_end  \
    "cmc" \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosIsPrinterOk = \
    CallGate_is_printer_ok  \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosIsPrinterHeadLifted = \
    CallGate_is_printer_head_lifted  \
    "cmc" \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosHasPrinterPaperInPresenter = \
    CallGate_has_printer_paper_in_presenter  \
    "cmc" \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosPrintTest = \
    CallGate_print_test  \
    parm [ebx];

#pragma aux RdosCreatePrinterBitmap = \
    CallGate_create_printer_bitmap  \
    "mov ebx,eax" \
    ValidateHandle \
    parm [ebx] [edx] \
    value [ebx];

#pragma aux RdosPrintBitmap = \
    CallGate_print_bitmap  \
    parm [ebx] [eax];

#pragma aux RdosPresentPrinterMedia = \
    CallGate_present_printer_media  \
    parm [ebx] [eax];

#pragma aux RdosEjectPrinterMedia = \
    CallGate_eject_printer_media  \
    parm [ebx];

#pragma aux RdosWaitForPrint = \
    CallGate_wait_for_print  \
    parm [ebx];

#pragma aux RdosOpenFile = \
    CallGate_open_file  \
    ValidateHandle  \
    parm [edi] [cl] \
    value [ebx];

#pragma aux RdosCreateFile = \
    CallGate_create_file  \
    ValidateHandle  \
    parm [edi] [ecx] \
    value [ebx];

#pragma aux RdosCloseFile = \
    CallGate_close_file  \
    parm [ebx];

#pragma aux RdosIsDevice = \
    CallGate_get_ioctl_data  \
    0x33 0xC0 0xF6 0xC6 0x80 0x74 1 0x40 \
    parm [ebx]  \
    value [eax] \
    modify [dx];

#pragma aux RdosDuplFile = \
    CallGate_dupl_file  \
    ValidateHandle  \
    parm [eax]  \
    value [ebx];

#pragma aux RdosGetFileSize = \
    CallGate_get_file_size  \
    ValidateEax \
    parm [ebx]  \
    value [eax];

#pragma aux RdosSetFileSize = \
    CallGate_set_file_size  \
    parm [ebx] [eax];
    
#pragma aux RdosGetFilePos = \
    CallGate_get_file_pos  \
    ValidateEax \
    parm [ebx]  \
    value [eax];

#pragma aux RdosSetFilePos = \
    CallGate_set_file_pos  \
    parm [ebx] [eax];
    
#pragma aux RdosReadFile = \
    CallGate_read_file  \
    ValidateEax \
    parm [ebx] [edi] [ecx]  \
    value [eax];

#pragma aux RdosWriteFile = \
    CallGate_write_file  \
    ValidateEax \
    parm [ebx] [edi] [ecx]  \
    value [eax];

#pragma aux RdosGetFileTime = \
    CallGate_get_file_time  \
    "mov [esi],edx" \
    "mov [edi],eax" \
    parm [ebx] [esi] [edi]  \
    modify [eax edx];

#pragma aux RdosSetFileTime = \
    CallGate_set_file_time  \
    parm [ebx] [edx] [eax];

#pragma aux RdosCreateMapping = \
    CallGate_create_mapping  \
    ValidateHandle  \
    parm [eax]  \
    value [ebx];

#pragma aux RdosCreateNamedMapping = \
    CallGate_create_named_mapping  \
    ValidateHandle  \
    parm [edi] [eax]  \
    value [ebx];

#pragma aux RdosCreateNamedFileMapping = \
    CallGate_create_named_file_mapping  \
    ValidateHandle  \
    parm [edi] [eax] [ebx]  \
    value [ebx];

#pragma aux RdosOpenNamedMapping = \
    CallGate_open_named_mapping  \
    ValidateHandle  \
    parm [edi] \
    value [ebx];

#pragma aux RdosSyncMapping = \
    CallGate_sync_mapping  \
    parm [ebx];

#pragma aux RdosCloseMapping = \
    CallGate_close_mapping  \
    parm [ebx];

#pragma aux RdosMapView = \
    CallGate_map_view  \
    parm [ebx] [eax] [edi] [ecx];

#pragma aux RdosUnmapView = \
    CallGate_unmap_view  \
    parm [ebx];

#pragma aux RdosSetCurDrive = \
    CallGate_set_cur_drive  \
    CarryToBool \
    parm [eax]  \
    value [eax];

#pragma aux RdosGetCurDrive = \
    CallGate_get_cur_drive  \
    "movzx eax,al"  \
    value [eax];

#pragma aux RdosGetCurDir = \
    CallGate_get_cur_dir  \
    CarryToBool \
    parm [eax] [edi]  \
    value [eax];

#pragma aux RdosSetCurDir = \
    CallGate_set_cur_dir  \
    CarryToBool \
    parm [edi]  \
    value [eax];

#pragma aux RdosMakeDir = \
    CallGate_make_dir  \
    CarryToBool \
    parm [edi]  \
    value [eax];

#pragma aux RdosRemoveDir = \
    CallGate_remove_dir  \
    CarryToBool \
    parm [edi]  \
    value [eax];

#pragma aux RdosRenameFile = \
    CallGate_rename_file  \
    CarryToBool \
    parm [edi] [esi]  \
    value [eax];

#pragma aux RdosDeleteFile = \
    CallGate_delete_file  \
    CarryToBool \
    parm [edi]  \
    value [eax];

#pragma aux RdosGetFileAttribute = \
    CallGate_get_file_attribute  \
    "movzx ecx,cx"  \
    "mov [eax],ecx" \
    CarryToBool \
    parm [edi] [eax]  \
    value [eax] \
    modify [ecx];

#pragma aux RdosSetFileAttribute = \
    CallGate_set_file_attribute  \
    CarryToBool \
    parm [edi] [ecx] \
    value [eax];

#pragma aux RdosOpenDir = \
    CallGate_open_dir  \
    ValidateHandle \
    parm [edi]  \
    value [ebx];

#pragma aux RdosCloseDir = \
    CallGate_close_dir  \
    parm [ebx];

// ReadDir here

#pragma aux RdosDefineFaultSave = \
    CallGate_define_fault_save  \
    parm [eax] [edx] [ecx];

#pragma aux RdosClearFaultSave = \
    CallGate_clear_fault_save;

#pragma aux RdosGetImageHeader = \
    CallGate_get_image_header  \
    CarryToBool \
    parm [eax] [edx] [edi] \
    value [eax];

#pragma aux RdosGetImageData = \
    CallGate_get_image_data  \
    CarryToBool \
    parm [eax] [edx] [edi] \
    value [eax];

#pragma aux RdosGetDeviceInfo = \
    "push edx" \
    CallGate_get_device_info  \
    "mov [ecx],eax" \
    CarryToBool \
    "mov [esi],edx" \
    "pop edx" \
    "mov [edx],bx" \
    parm [ebx] [edi] [ecx] [edx] [esi] \
    modify [ebx edx] \
    value [eax];

#pragma aux RdosGetSelectorInfo = \
    CallGate_get_selector_info  \
    "mov [esi],ecx" \
    "movzx eax,al" \
    "mov [edi],eax" \
    CarryToBool \
    parm [bx] [esi] [edi] \
    modify [ecx] \
    value [eax];

#pragma aux RdosGetFaultThreadState = \
    CallGate_get_fault_thread_state  \
    CarryToBool \
    parm [eax] [edi] \
    value [eax];

#pragma aux RdosGetFaultThreadTss = \
    CallGate_get_fault_thread_tss  \
    CarryToBool \
    parm [eax] [edi] \
    value [eax];

#pragma aux RdosGetThreadState = \
    CallGate_get_thread_state  \
    CarryToBool \
    parm [eax] [edi] \
    value [eax];

#pragma aux RdosSuspendThread = \
    CallGate_suspend_thread  \
    CarryToBool \
    parm [eax] \
    value [eax];

#pragma aux RdosSuspendAndSignalThread = \
    CallGate_suspend_and_signal_thread  \
    CarryToBool \
    parm [eax] \
    value [eax];

#pragma aux RdosHasHardReset = \
    CallGate_has_hard_reset \
    CarryToBool \
    value [eax];

#pragma aux RdosSoftReset = \
    CallGate_soft_reset;

#pragma aux RdosHardReset = \
    CallGate_hard_reset;

#pragma aux RdosPowerFailure = \
    CallGate_power_failure \
    "movzx eax,ax"  \
    value [eax];

#pragma aux RdosIsEmergencyStopped = \
    CallGate_is_emergency_stopped \
    CarryToBool \    
    value [eax];

#pragma aux RdosHasGlobalTimer = \
    CallGate_has_global_timer \
    CarryToBool \    
    value [eax];

#pragma aux RdosGetCoreLoad = \
    CallGate_get_core_load \
    "mov [esi],ebx" \
    "mov [esi+4],ecx" \
    "mov [edi],eax" \
    "mov [edi+4],edx" \
    CarryToBool \    
    parm [eax] [esi] [edi] \
    value [eax] \
    modify [ebx ecx edx];

#pragma aux RdosGetCoreDuty = \
    CallGate_get_core_duty \
    "mov [esi],eax" \
    "mov [esi+4],edx" \
    "mov [edi],ebx" \
    "mov [edi+4],ecx" \
    CarryToBool \    
    parm [eax] [esi] [edi] \
    value [eax] \
    modify [ebx ecx edx];

#pragma aux RdosGetVersion = \
    CallGate_get_version  \
    "movzx edx,dx"  \
    "mov [ebx],edx" \
    "movzx  eax,ax" \
    "mov [esi],eax" \
    "movzx ecx,cx"  \
    "mov [edi],ecx" \
    parm [ebx] [esi] [edi]  \
    modify [eax ecx edx];

#pragma aux RdosGetCpuVersion = \
    CallGate_get_cpu_version  \
    "movzx  eax,al" \
    "mov [esi],edx" \
    "mov [ecx],ebx" \
    parm [edi] [esi] [ecx] \
    value [eax];

#pragma aux RdosTerminateThread = \
    CallGate_terminate_thread;

#pragma aux RdosGetThreadHandle = \
    CallGate_get_thread \
    "movzx eax,ax"  \
    value [eax];

#pragma aux RdosExec = \
    "push gs" \
    "mov ax,ds" \
    "mov gs,ax" \
    CallGate_load_exe  \
    "pop gs" \
    CallGate_get_exit_code  \
    "movzx eax,ax"  \
    parm [esi] [edi] [ebx] \
    value [eax];

#pragma aux RdosFork = \
    CallGate_fork  \
    "movsx eax,ax"  \
    parm [esi] [edi] \
    value [eax];

#pragma aux RdosUnloadExe = \
    CallGate_unload_exe  \
    parm [eax];

#pragma aux RdosFreeProcessHandle = \
    CallGate_free_proc_handle  \
    parm [ebx];

#pragma aux RdosAddWaitForProcessEnd = \
    CallGate_add_wait_for_proc_end  \
    parm [ebx] [eax] [ecx];

#pragma aux RdosGetProcessExitCode = \
    CallGate_get_proc_exit_code  \
    "movsx eax,ax" \
    parm [ebx]  \
    value [eax];

#pragma aux RdosShowExceptionText = \
    CallGate_show_exception_text  \
    value [eax];

#pragma aux RdosWaitMilli = \
    CallGate_wait_milli  \
    parm [eax];

#pragma aux RdosWaitMicro = \
    CallGate_wait_micro  \
    parm [eax];

#pragma aux RdosWaitUntil = \
    CallGate_wait_until  \
    parm [edx] [eax];

#pragma aux RdosGetSysTime = \
    CallGate_get_system_time  \
    "mov [esi],edx" \
    "mov [edi],eax" \
    parm [esi] [edi] \
    modify [eax edx];

#pragma aux RdosGetTime = \
    CallGate_get_time  \
    "mov [esi],edx" \
    "mov [edi],eax" \
    parm [esi] [edi] \
    modify [eax edx];

#pragma aux RdosSetTime = \
    CallGate_get_system_time  \
    "sub esi,eax" \
    "sbb edi,edx" \
    "mov eax,esi" \
    "mov edx,edi" \
    CallGate_update_time  \
    CallGate_update_rtc  \
    parm [edi] [esi] \
    modify [eax edx esi edi];

#pragma aux RdosDayOfWeek = \
    "mov ch,al" \
    "xor ebx,ebx"   \
    "xor ah,ah" \
    CallGate_adjust_time  \
    "push edx"   \
    "mov eax,365"   \
    "imul dx"   \
    "push dx"   \
    "push ax"   \
    "pop ebx"   \
    "pop edx"   \
    CallGate_passed_days  \
    "dec edx"   \
    "shr edx,2" \
    "inc edx"   \
    "add ax,dx" \
    "add eax,ebx"   \
    "xor edx,edx"   \
    "add eax,5" \
    "mov ebx,7" \
    "div ebx"   \
    "movzx eax,dl" \
    parm [edx] [eax] [ecx] \
    value [eax] \
    modify [ebx ecx edx];

#pragma aux RdosDosTimeDateToTics = \
    "push ebx" \
    "push ecx" \
    "mov dx,si" \
    "mov ax,dx" \
    "shr dx,9" \
    "add dx,1980" \
    "mov cx,ax" \
    "shr cx,5" \
    "mov ch,cl" \
    "and ch,0Fh" \
    "mov cl,al" \
    "and cl,1Fh" \
    "mov bx,di" \
    "mov ax,bx" \
    "shr bx,11" \
    "mov bh,bl" \
    "shr ax,5" \
    "and al,3Fh" \
    "mov bl,al" \
    "mov ax,di" \
    "mov ah,al" \
    "add ah,ah" \
    "and ah,3Fh" \
    CallGate_time_to_binary  \
    "pop ecx" \
    "pop ebx" \
    "mov [ebx],edx" \
    "mov [ecx],eax" \
    parm [si] [di] [ebx] [ecx] \
    modify [eax edx];

#pragma aux RdosTicsToDosTimeDate = \
    CallGate_binary_to_time  \
    "shl cl,3" \
    "shr cx,3" \
    "sub dx,1980" \
    "mov dh,dl" \
    "shl dh,1" \
    "xor dl,dl" \
    "or dx,cx" \
    "mov al,ah" \
    "shr al,1" \
    "shl bl,2" \
    "shl bx,3" \
    "or bl,al" \
    "mov [esi],dx" \
    "mov [edi],bx" \
    parm [edx] [eax] [esi] [edi] \
    modify [eax ebx ecx edx];

#pragma aux RdosDecodeMsbTics = \
    "push ebx" \
    "push ecx" \
    "xor eax,eax" \
    CallGate_binary_to_time  \
    "movzx eax,bh" \
    "mov [edi],eax" \
    "movzx eax,cl" \
    "mov [esi],eax" \
    "movzx eax,ch" \
    "pop ecx" \
    "pop ebx" \
    "mov [ecx],eax" \
    "movzx eax,dx" \
    "mov [ebx],eax" \
    parm [edx] [ebx] [ecx] [esi] [edi] \
    modify [eax edx];

#pragma aux RdosDecodeLsbTics = \
    "mov edx,60" \
    "mul edx" \
    "mov [ebx],edx" \
    "mov edx,60" \
    "mul edx" \
    "mov [ecx],edx" \
    "mov edx,1000" \
    "mul edx" \
    "mov [esi],edx" \
    "mov edx,1000" \
    "mul edx" \
    "mov [edi],edx" \
    parm [eax] [ebx] [ecx] [esi] [edi] \
    modify [eax edx];

#pragma aux RdosCodeMsbTics = \
    "mov ch,al" \
    "mov bh,bl" \
    "xor bl,bl" \
    "xor ah,ah" \
    CallGate_time_to_binary  \
    parm [edx] [eax] [ecx] [ebx] \
    value [edx] \
    modify [eax bx cx];

#pragma aux RdosCodeLsbTics = \
    "xor dx,dx" \
    "xor cx,cx" \
    "xor bh,bh" \
    "mov ah,al" \
    CallGate_time_to_binary  \
    "mov ebx,eax" \
    "mov eax,1193046" \
    "mul esi" \
    "mov ecx,eax" \
    "mov eax,1193" \
    "mul edi" \
    "add eax,ecx" \
    "xor edx,edx" \
    "mov ecx,1000" \
    "div ecx" \
    "add eax,ebx" \
    parm [ebx] [eax] [esi] [edi] \
    value [eax] \
    modify [ebx ecx edx];

#pragma aux RdosAddTics = \
    "add [esi],eax" \
    "adc [edi],0"  \
    parm [edi] [esi] [eax];

#pragma aux RdosAddMicro = \
    "mov edx,1193"  \
    "imul edx"  \
    "xor edx,edx" \
    "mov ebx,1000" \
    "idiv ebx" \
    "add [esi],eax" \
    "adc dword ptr [edi],0"  \
    parm [edi] [esi] [eax] \
    modify [eax ebx edx];

#pragma aux RdosAddMilli = \
    "mov edx,1193"  \
    "imul edx"  \
    "add [esi],eax" \
    "adc [edi],edx"  \
    parm [edi] [esi] [eax] \
    modify [eax edx];

#pragma aux RdosAddSec = \
    "mov edx,1193046"  \
    "imul edx"  \
    "add [esi],eax" \
    "adc [edi],edx"  \
    parm [edi] [esi] [eax] \
    modify [eax edx];

#pragma aux RdosAddMin = \
    "mov edx,1193046*60"  \
    "imul edx"  \
    "add [esi],eax" \
    "adc [edi],edx"  \
    parm [edi] [esi] [eax] \
    modify [eax edx];

#pragma aux RdosAddHour = \
    "add [edi],eax"  \
        parm [edi] [esi] [eax];

#pragma aux RdosAddDay = \
    "mov edx,24"  \
    "imul edx"  \
    "add [edi],eax"  \
    parm [edi] [esi] [eax] \
    modify [eax edx];

#pragma aux RdosSyncTime = \
    CallGate_sync_time  \
    CarryToBool \
    parm [edx] \
    value [eax];

#pragma aux RdosCreateSection = \
    CallGate_create_user_section  \
    ValidateHandle  \
    value [ebx];

#pragma aux RdosDeleteSection = \
    CallGate_delete_user_section  \
    parm [ebx];

#pragma aux RdosEnterSection = \
    CallGate_enter_user_section  \
    parm [ebx];

#pragma aux RdosLeaveSection = \
    CallGate_leave_user_section  \
    parm [ebx];

#pragma aux RdosGetFreeHandles = \
    CallGate_get_free_handles  \
    "movzx eax,ax"  \
    value [eax];

#pragma aux RdosGetFreeHandleMem = \
    CallGate_get_free_handle_mem  \
    value [eax];

#pragma aux RdosCreateWait = \
    CallGate_create_wait  \
    ValidateHandle  \
    value [ebx];

#pragma aux RdosCloseWait = \
    CallGate_close_wait  \
    parm [ebx];

#pragma aux RdosCheckWait = \
    CallGate_is_wait_idle  \
    parm [ebx] \
    value [ecx];

#pragma aux RdosWaitForever = \
    CallGate_wait_no_timeout  \
    ValidateEcx \
    parm [ebx] \
    value [ecx]

#pragma aux RdosWaitTimeout = \
    "mov edx,1193" \
    "mul edx"   \
    "push edx"  \
    "push eax"  \
    CallGate_get_system_time \
    "pop ecx"   \
    "add eax,ecx"   \
    "pop ecx"   \
    "adc edx,ecx"   \
    CallGate_wait_timeout  \
    ValidateEcx \
    parm [ebx] [eax] \
    value [ecx] \
    modify [eax edx];

#pragma aux RdosWaitUntilTimeout = \
    CallGate_wait_timeout  \
    ValidateEcx \
    parm [ebx] [edx] [eax] \
    value [ecx];

#pragma aux RdosStopWait = \
    CallGate_stop_wait  \
    parm [ebx];

#pragma aux RdosRemoveWait = \
    CallGate_remove_wait  \
    parm [ebx] [ecx];

#pragma aux RdosAddWaitForKeyboard = \
    CallGate_add_wait_for_keyboard  \
    parm [ebx] [ecx];

#pragma aux RdosAddWaitForMouse = \
    CallGate_add_wait_for_mouse  \
    parm [ebx] [ecx];

#pragma aux RdosAddWaitForCom = \
    CallGate_add_wait_for_com  \
    parm [ebx] [eax] [ecx];

#pragma aux RdosAddWaitForSyslog = \
    CallGate_add_wait_for_syslog  \
    parm [ebx] [eax] [ecx];

#pragma aux RdosAddWaitForAdc = \
    CallGate_add_wait_for_adc  \
    parm [ebx] [eax] [ecx];

#pragma aux RdosCreateSignal = \
    CallGate_create_signal  \
    ValidateHandle  \
    value [ebx];

#pragma aux RdosResetSignal = \
    CallGate_reset_signal  \
    parm [ebx];

#pragma aux RdosSetSignal = \
    CallGate_set_signal  \
    parm [ebx];

#pragma aux RdosIsSignalled = \
    CallGate_is_signalled  \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosFreeSignal = \
    CallGate_free_signal  \
    parm [ebx];

#pragma aux RdosAddWaitForSignal = \
    CallGate_add_wait_for_signal  \
    parm [ebx] [eax] [ecx];

#pragma aux RdosGetIp = \
    CallGate_get_ip_address  \
    value [edx];

#pragma aux RdosGetGateway = \
    CallGate_get_gateway  \
    value [edx];

#pragma aux RdosNameToIp = \
    CallGate_name_to_ip  \
    parm [edi] \
    value [edx];

#pragma aux RdosIpToName = \
    CallGate_ip_to_name  \
    parm [edx] [edi] [ecx] \
    value [eax];

#pragma aux RdosCreateTcpListen = \
    CallGate_create_tcp_listen  \
    ValidateHandle \
    parm [esi] [eax] [ecx] \
    value [ebx];

#pragma aux RdosGetTcpListen = \
    CallGate_get_tcp_listen  \
    "movzx ebx,ax" \
    ValidateHandle \
    parm [ebx] \
    value [ebx] \
    modify [ax];

#pragma aux RdosCloseTcpListen = \
    CallGate_close_tcp_listen  \
    parm [ebx];

#pragma aux RdosAddWaitForTcpListen = \
    CallGate_add_wait_for_tcp_listen  \
    parm [ebx] [eax] [ecx];

#pragma aux RdosOpenTcpConnection = \
    CallGate_open_tcp_connection  \
    ValidateHandle \
    parm [edx] [esi] [edi] [eax] [ecx] \
    value [ebx];

#pragma aux RdosWaitForTcpConnection = \
    CallGate_wait_for_tcp_connection  \
    CarryToBool \
    parm [ebx] [eax] \
    value [eax];

#pragma aux RdosAddWaitForTcpConnection = \
    CallGate_add_wait_for_tcp_connection  \
    parm [ebx] [eax] [ecx];

#pragma aux RdosCloseTcpConnection = \
    CallGate_close_tcp_connection  \
    parm [ebx];

#pragma aux RdosDeleteTcpConnection = \
    CallGate_delete_tcp_connection  \
    parm [ebx];

#pragma aux RdosAbortTcpConnection = \
    CallGate_abort_tcp_connection  \
    parm [ebx];

#pragma aux RdosPushTcpConnection = \
    CallGate_push_tcp_connection  \
    parm [ebx];

#pragma aux RdosIsTcpConnectionClosed = \
    CallGate_is_tcp_connection_closed  \
    "cmc"   \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosIsTcpConnectionIdle = \
    CallGate_is_tcp_connection_idle  \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosGetRemoteTcpConnectionIP = \
    CallGate_get_remote_tcp_connection_ip  \
    ValidateEax \
    parm [ebx] \
    value [eax];

#pragma aux RdosGetRemoteTcpConnectionPort = \
    CallGate_get_remote_tcp_connection_port  \
    "movzx eax,ax"\
    ValidateEax \
    parm [ebx] \
    value [eax];

#pragma aux RdosGetLocalTcpConnectionPort = \
    CallGate_get_local_tcp_connection_port  \
    "movzx eax,ax"\
    ValidateEax \
    parm [ebx] \
    value [eax];

#pragma aux RdosReadTcpConnection = \
    CallGate_read_tcp_connection  \
    parm [ebx] [edi] [ecx] \
    value [eax];

#pragma aux RdosWriteTcpConnection = \
    CallGate_write_tcp_connection  \
    parm [ebx] [edi] [ecx] \
    value [eax];

#pragma aux RdosPollTcpConnection = \
    CallGate_poll_tcp_connection  \
    parm [ebx] \
    value [eax];

#pragma aux RdosGetLocalMailslot = \
    CallGate_get_local_mailslot  \
    ValidateHandle \
    parm [edi] \
    value [ebx];

#pragma aux RdosGetRemoteMailslot = \
    CallGate_get_remote_mailslot  \
    ValidateHandle \
    parm [edx] [edi] \
    value [ebx];

#pragma aux RdosFreeMailslot = \
    CallGate_free_mailslot  \
    parm [ebx];

#pragma aux RdosSendMailslot = \
    CallGate_send_mailslot  \
    ValidateEcx \
    parm [ebx] [esi] [ecx] [edi] [eax] \
    value [ecx];

#pragma aux RdosDefineMailslot = \
    CallGate_define_mailslot  \
    parm [edi] [ecx];

#pragma aux RdosReceiveMailslot = \
    CallGate_receive_mailslot  \
    parm [edi] \
    value [ecx];

#pragma aux RdosReplyMailslot = \
    CallGate_reply_mailslot  \
    parm [edi] [ecx];

#pragma aux RdosSetFocus = \
    CallGate_set_focus  \
    parm [al];

#pragma aux RdosGetFocus = \
    CallGate_get_focus  \

#pragma aux RdosSetKeyMap = \
    CallGate_set_key_layout \
    CarryToBool \
    parm [edi] \
    value [eax];

#pragma aux RdosGetKeyMap = \
    CallGate_get_key_layout \
    parm [edi];

#pragma aux RdosClearKeyboard = \
    CallGate_flush_keyboard;

#pragma aux RdosPollKeyboard = \
    CallGate_poll_keyboard  \
    CarryToBool \
    value [eax];

#pragma aux RdosReadKeyboard = \
    CallGate_read_keyboard  \
    "movzx eax,ax" \
    value [eax];

#pragma aux RdosGetKeyboardState = \
    CallGate_get_keyboard_state  \
    "movzx eax,ax" \
    value [eax];

#pragma aux RdosPutKeyboard = \
    "mov dh,cl" \
    CallGate_put_keyboard_code  \
    parm [eax] [edx] [ecx] \
    modify [dh];

#pragma aux RdosPeekKeyEvent = \
    "push ecx" \
    CallGate_peek_key_event  \
    "movzx eax,ax" \
    "mov [ebx],eax" \
    "movzx eax,cx" \
    "pop ecx" \
    "mov [ecx],eax" \
    "movzx eax,dl" \
    "mov [esi],eax" \
    "movzx eax,dh" \
    "mov [edi],eax" \
    CarryToBool \
    parm [ebx] [ecx] [esi] [edi] \
    value [eax] \
    modify [dx];

#pragma aux RdosReadKeyEvent = \
    "push ecx" \
    CallGate_read_key_event  \
    "movzx eax,ax" \
    "mov [ebx],eax" \
    "movzx eax,cx" \
    "pop ecx" \
    "mov [ecx],eax" \
    "movzx eax,dl" \
    "mov [esi],eax" \
    "movzx eax,dh" \
    "mov [edi],eax" \
    CarryToBool \
    parm [ebx] [ecx] [esi] [edi] \
    value [eax] \
    modify [dx];

#pragma aux RdosHideMouse = \
    CallGate_hide_mouse;

#pragma aux RdosShowMouse = \
    CallGate_show_mouse;

#pragma aux RdosGetMousePosition = \
    CallGate_get_mouse_position \
    "movzx ecx,cx" \
    "mov [esi],ecx" \
    "movzx edx,dx" \
    "mov [edi],edx" \
    parm [esi] [edi] \
    modify [ecx edx];

#pragma aux RdosSetMousePosition = \
    CallGate_set_mouse_position \
    parm [ecx] [edx];

#pragma aux RdosSetMouseWindow = \
    CallGate_set_mouse_window \
    parm [eax] [ebx] [ecx] [edx];

#pragma aux RdosSetMouseMickey = \
    CallGate_set_mouse_mickey \
    parm [ecx] [edx];

#pragma aux RdosGetLeftButton = \
    CallGate_get_left_button \
    CarryToBool \
    value [eax];

#pragma aux RdosGetRightButton = \
    CallGate_get_right_button \
    CarryToBool \
    value [eax];

#pragma aux RdosGetLeftButtonPressPosition = \
    CallGate_get_left_button_press_position \
    "movzx ecx,cx" \
    "mov [esi],ecx" \
    "movzx edx,dx" \
    "mov [edi],edx" \
    parm [esi] [edi] \
    modify [ecx edx];

#pragma aux RdosGetRightButtonPressPosition = \
    CallGate_get_right_button_press_position \
    "movzx ecx,cx" \
    "mov [esi],ecx" \
    "movzx edx,dx" \
    "mov [edi],edx" \
    parm [esi] [edi] \
    modify [ecx edx];

#pragma aux RdosGetLeftButtonReleasePosition = \
    CallGate_get_left_button_release_position \
    "movzx ecx,cx" \
    "mov [esi],ecx" \
    "movzx edx,dx" \
    "mov [edi],edx" \
    parm [esi] [edi] \
    modify [ecx edx];

#pragma aux RdosGetRightButtonReleasePosition = \
    CallGate_get_right_button_release_position \
    "movzx ecx,cx" \
    "mov [esi],ecx" \
    "movzx edx,dx" \
    "mov [edi],edx" \
    parm [esi] [edi] \
    modify [ecx edx];

#pragma aux RdosGetCursorPosition = \
    CallGate_get_cursor_position \
    "movzx ecx,cx" \
    "mov [esi],ecx" \
    "movzx edx,dx" \
    "mov [edi],edx" \
    parm [edi] [esi] \
    modify [ecx edx];

#pragma aux RdosSetCursorPosition = \
    CallGate_set_cursor_position \
    parm [edx] [ecx];

#pragma aux RdosWriteChar = \
    CallGate_write_char \
    parm [al];

#pragma aux RdosWriteSizeString = \
    CallGate_write_size_string \
    parm [edi] [ecx];

#pragma aux RdosWriteAttributeString = \
    CallGate_write_attrib_string \
    parm [edx] [eax] [edi] [ecx];

#pragma aux RdosWriteString = \
    CallGate_write_asciiz  \
    parm [edi];

#pragma aux RdosReadLine = \
    CallGate_read_con  \
    parm [edi] [ecx] \
    value [eax];

#pragma aux RdosPing = \
    CallGate_ping  \
    CarryToBool \
    parm [edx] [eax] \
    value [eax];

#pragma aux RdosGetIdeDisc = \
    CallGate_get_ide_disc  \
    ValidateDisc \
    parm [ebx] \
    value [eax];

#pragma aux RdosGetFloppyDisc = \
    CallGate_get_floppy_disc  \
    ValidateDisc \
    parm [ebx] \
    value [eax];

#pragma aux RdosSetDiscInfo = \
    CallGate_set_disc_info  \
    CarryToBool \
    parm [eax] [ecx] [edx] [esi] [edi] \
    value [eax];

#pragma aux RdosGetDiscInfo = \
    "push edi" \
    "push esi" \
    "push edx" \
    "push ecx" \
    CallGate_get_disc_info  \
    "pop ebx" \
    "movzx ecx,cx" \
    "mov [ebx],ecx" \
    "pop ebx" \
    "mov [ebx],edx" \
    "pop ebx" \
    "movzx esi,si" \
    "mov [ebx],esi" \
    "pop ebx" \
    "movzx edi,di" \
    "mov [ebx],edi" \
    CarryToBool \
    parm [eax] [ecx] [edx] [esi] [edi] \
    value [eax] \
    modify [ebx ecx edx esi edi];

#pragma aux RdosReadDisc = \
    CallGate_read_disc  \
    CarryToBool \
    parm [eax] [edx] [edi] [ecx] \
    value [eax];

#pragma aux RdosWriteDisc = \
    CallGate_write_disc  \
    CarryToBool \
    parm [eax] [edx] [edi] [ecx] \
    value [eax];

#pragma aux RdosGetRdfsInfo = \
    "push gs" \
    "mov ax,ds" \
    "mov gs,ax" \
    CallGate_get_rdfs_info  \
    "pop gs" \
    parm [esi] [edi] [ebx] \
    modify [ax];

#pragma aux RdosDemandLoadDrive = \
    CallGate_demand_load_drive  \
    parm [eax];

#pragma aux RdosFormatDrive = \
    CallGate_format_drive  \
    ValidateDisc \
    parm [eax] [edx] [ecx] [edi] \
    value [eax];

#pragma aux RdosAllocateFixedDrive = \
    CallGate_allocate_fixed_drive  \
    CarryToBool \
    parm [eax] \
    value [eax];

#pragma aux RdosAllocateStaticDrive = \
    CallGate_allocate_static_drive  \
    ValidateDisc \
    value [eax];

#pragma aux RdosAllocateDynamicDrive = \
    CallGate_allocate_dynamic_drive  \
    ValidateDisc \
    value [eax];

#pragma aux RdosGetDriveInfo = \
    CallGate_get_drive_info  \
    "mov [ebx],eax" \
    "movzx ecx,cx" \
    "mov [esi],ecx" \
    "mov [edi],edx" \
    CarryToBool \
    parm [eax] [ebx] [esi] [edi] \
    value [eax] \
    modify [ecx edx];

#pragma aux RdosGetDriveDiscParam = \
    CallGate_get_drive_disc_param  \
    "movzx eax,al" \
    "mov [ebx],eax" \
    "mov [esi],edx" \
    "mov [edi],ecx" \
    CarryToBool \
    parm [eax] [ebx] [esi] [edi] \
    value [eax] \
    modify [ecx edx];

#pragma aux RdosCreateFileDrive = \
    CallGate_create_file_drive  \
    CarryToBool \
    parm [eax] [ecx] [esi] [edi] \
    value [eax];

#pragma aux RdosOpenFileDrive = \
    CallGate_open_file_drive  \
    CarryToBool \
    parm [eax] [edi] \
    value [eax];

#pragma aux RdosCreateCrc = \
    CallGate_create_crc  \
    ValidateHandle \
    parm [ax] \
    value [ebx];

#pragma aux RdosCloseCrc = \
    CallGate_close_crc  \
    parm [ebx];

#pragma aux RdosCalcCrc = \
    CallGate_calc_crc  \
    parm [ebx] [ax] [edi] [ecx] \
    value [ax];

#pragma aux RdosGetModuleHandle = \
    "mov eax,fs:[0x24]" \
    value [eax];

#pragma aux RdosGetExeName = \
    CallGate_get_exe_name  \
    ValidateEdi \
    value [edi];

#pragma aux RdosGetCmdLine = \
    CallGate_get_cmd_line  \
    ValidateEdi \
    value [edi];

#pragma aux RdosGetOptions = \
    CallGate_get_options  \
    ValidateEdi \
    value [edi];

#pragma aux RdosLoadDll = \
    CallGate_load_dll  \
    ValidateHandle \
    parm [edi] \
    value [ebx];

#pragma aux RdosFreeDll = \
    CallGate_free_dll  \
    parm [ebx];

#pragma aux RdosGetModuleName = \
    CallGate_get_module_name  \
    ValidateEax \
    parm [ebx] [edi] [ecx] \
    value [eax];

// ReadResource here
// ReadBinaryResource here

#pragma aux RdosGetModuleProc = \
    CallGate_get_module_proc  \
    ValidateEsi \
    parm [ebx] [edi] \
    value [esi];

#pragma aux RdosGetModuleFocusKey = \
    CallGate_get_module_focus_key  \
    parm [ebx] \
    value [al];

#pragma aux RdosAddWaitForDebugEvent = \
    CallGate_add_wait_for_debug_event  \
    parm [ebx] [eax] [ecx];

#pragma aux RdosGetDebugEvent = \
    CallGate_get_debug_event  \
    "movzx eax,ax" \
    "mov [esi],eax" \
    parm [ebx] [esi] \
    value [bl] \
    modify [eax];

#pragma aux RdosGetDebugEventData = \
    CallGate_get_debug_event_data  \
    parm [ebx] [edi];

#pragma aux RdosClearDebugEvent = \
    CallGate_clear_debug_event  \
    parm [ebx];

#pragma aux RdosContinueDebugEvent = \
    CallGate_continue_debug_event  \
    parm [ebx] [eax];

#pragma aux RdosOpenSyslog = \
    CallGate_open_syslog  \
    ValidateHandle \
    value [ebx];

#pragma aux RdosCloseSyslog = \
    CallGate_close_syslog  \
    parm [ebx];

#pragma aux RdosGetSyslog = \
    "push edi" \
    "push esi" \
    "push edx" \
    "push eax" \
    CallGate_get_syslog  \
    "pop edi" \
    "mov [edi],eax" \
    "pop edi" \
    "mov [edi],edx" \
    "pop edi" \
    "movzx eax,si" \
    "xor ah,ah" \
    "mov [edi],eax" \
    "movzx eax,si" \
    "mov al,ah" \
    "xor ah,ah" \        
    parm [ebx] [esi] [edx] [eax] [edi] [ecx] \
    value [eax];

#pragma aux RdosOpenAdc = \
    CallGate_open_adc  \
    ValidateHandle \
    parm [eax] \
    value [ebx];

#pragma aux RdosCloseAdc = \
    CallGate_close_adc  \
    parm [ebx];

#pragma aux RdosDefineAdcTime = \
    CallGate_define_adc_time  \
    parm [ebx] [edx] [eax];

#pragma aux RdosReadAdc = \
    CallGate_read_adc  \
    parm [ebx] \
    value [eax];

#pragma aux RdosReadSerialLines = \
    "mov dh,cl" \
    CallGate_read_serial_lines  \
    "movzx eax,al" \
    "mov [esi],eax" \
    CarryToBool \
    parm [ecx] [esi] \
    value [eax] \
    modify [dh];

#pragma aux RdosToggleSerialLine = \
    "mov dh,cl" \
    CallGate_toggle_serial_line  \
    CarryToBool \
    parm [ecx] [edx] \
    value [eax] \
    modify [dh];

#pragma aux RdosReadSerialVal = \
    "mov dh,cl" \
    CallGate_read_serial_val  \
    "pushf" \
    "shl eax,8" \
    "mov [esi],eax" \
    "popf" \
    CarryToBool \
    parm [ecx] [edx] [esi] \
    value [eax] \
    modify [dh];

#pragma aux RdosWriteSerialVal = \
    "mov dh,cl" \
    "sar eax,8" \
    CallGate_write_serial_val  \
    CarryToBool \
    parm [ecx] [edx] [eax] \
    value [eax] \
    modify [dh];

#pragma aux RdosReadSerialRaw = \
    "mov dh,cl" \
    CallGate_read_serial_val  \
    "mov [esi],eax" \
    CarryToBool \
    parm [ecx] [edx] [esi] \
    value [eax] \
    modify [dh];

#pragma aux RdosWriteSerialRaw = \
    "mov dh,cl" \
    CallGate_write_serial_val  \
    CarryToBool \
    parm [ecx] [edx] [eax] \
    value [eax] \
    modify [dh];

#pragma aux RdosOpenSysEnv = \
    CallGate_open_sys_env \
    ValidateHandle \
    value [ebx];

#pragma aux RdosOpenProcessEnv = \
    CallGate_open_proc_env \
    ValidateHandle \
    value [ebx];

#pragma aux RdosCloseEnv = \
    CallGate_close_env \
    parm [ebx];

#pragma aux RdosAddEnvVar = \
    CallGate_add_env_var \
    parm [ebx] [esi] [edi];

#pragma aux RdosDeleteEnvVar = \
    CallGate_delete_env_var \
    parm [ebx] [esi];

#pragma aux RdosFindEnvVar = \
    CallGate_find_env_var \
    CarryToBool \
    parm [ebx] [esi] [edi] \
    value [eax];

#pragma aux RdosGetEnvSize = \
    CallGate_get_env_size \
    parm [ebx] \
    value [eax];

#pragma aux RdosGetEnvData = \
    "xor ax,ax" \
    "mov es:[edi],ax" \
    CallGate_get_env_data \
    parm [ebx] [edi] \
    modify [ax];

#pragma aux RdosSetEnvData = \
    CallGate_set_env_data \
    parm [ebx] [edi];

#pragma aux RdosOpenSysIni = \
    CallGate_open_sys_ini \
    ValidateHandle \
    value [ebx];

#pragma aux RdosOpenIni = \
    CallGate_open_ini \
    ValidateHandle \
    parm [edi] \
    value [ebx];

#pragma aux RdosCloseIni = \
    CallGate_close_ini \
    parm [ebx];

#pragma aux RdosGotoIniSection = \
    CallGate_goto_ini_section \
    CarryToBool \
    parm [ebx] [edi] \
    value [eax];

#pragma aux RdosRemoveIniSection = \
    CallGate_remove_ini_section \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosReadIni = \
    CallGate_read_ini \
    CarryToBool \
    parm [ebx] [esi] [edi] [ecx] \
    value [eax];

#pragma aux RdosWriteIni = \
    CallGate_write_ini \
    CarryToBool \
    parm [ebx] [esi] [edi] \
    value [eax];

#pragma aux RdosDeleteIni = \
    CallGate_delete_ini \
    CarryToBool \
    parm [ebx] [esi] \
    value [eax];

#pragma aux RdosEnableStatusLED = \
    CallGate_enable_status_led;

#pragma aux RdosDisableStatusLED = \
    CallGate_disable_status_led;

#pragma aux RdosStartWatchdog = \
    CallGate_start_watchdog \
    parm [eax];

#pragma aux RdosKickWatchdog = \
    CallGate_kick_watchdog;

#pragma aux RdosStopWatchdog = \
    CallGate_stop_watchdog;

#pragma aux RdosStartNetCapture = \
    CallGate_start_net_capture \
    parm [ebx];

#pragma aux RdosStopNetCapture = \
    CallGate_stop_net_capture;

#pragma aux RdosGetUsbDevice = \
    CallGate_get_usb_device \
    ValidateEax \
    parm [ebx] [eax] [edi] [ecx] \
    value [eax];

#pragma aux RdosGetUsbConfig = \
    CallGate_get_usb_config \
    ValidateEax \
    parm [ebx] [eax] [edx] [edi] [ecx] \
    value [eax];

#pragma aux RdosOpenUsbPipe = \
    CallGate_open_usb_pipe \
    ValidateHandle \
    parm [ebx] [eax] [edx] \
    value [ebx];

#pragma aux RdosCloseUsbPipe = \
    CallGate_close_usb_pipe \
    parm [ebx];

#pragma aux RdosResetUsbPipe = \
    CallGate_reset_usb_pipe \
    parm [ebx];

#pragma aux RdosLockUsbPipe = \
    CallGate_lock_usb_pipe \
    parm [ebx];

#pragma aux RdosUnlockUsbPipe = \
    CallGate_unlock_usb_pipe \
    parm [ebx];

#pragma aux RdosAddWaitForUsbPipe = \
    CallGate_add_wait_for_usb_pipe \
    parm [ebx] [eax] [ecx];

#pragma aux RdosWriteUsbControl = \
    CallGate_write_usb_control \
    parm [ebx] [edi] [ecx];

#pragma aux RdosReqUsbData = \
    CallGate_req_usb_data \
    parm [ebx] [edi] [ecx];

#pragma aux RdosGetUsbDataSize = \
    CallGate_get_usb_data_size \
    "movzx eax,ax" \
    parm [ebx] \
    value [eax];

#pragma aux RdosWriteUsbData = \
    CallGate_write_usb_data \
    parm [ebx] [edi] [ecx];

#pragma aux RdosReqUsbStatus = \
    CallGate_req_usb_status \
    parm [ebx];

#pragma aux RdosWriteUsbStatus = \
    CallGate_write_usb_status \
    parm [ebx];

#pragma aux RdosIsUsbTransactionDone = \
    CallGate_is_usb_trans_done \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosWasUsbTransactionOk = \
    CallGate_was_usb_trans_ok \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosGetAllocatedUsbBlocks = \
    CallGate_get_allocated_usb_blocks \
    value [eax];

#pragma aux RdosGetUsbCloseCount = \
    CallGate_get_usb_close_count \
    value [eax];

#pragma aux RdosOpenHid = \
    CallGate_open_hid \
    ValidateHandle \
    parm [ebx] [eax] \
    value [ebx];

#pragma aux RdosCloseHid = \
    CallGate_close_hid \
    parm [ebx];

#pragma aux RdosGetHidPipe = \
    CallGate_get_hid_pipe \
    "jc Fail" \
    "movzx eax,al" \
    "jmp Done" \
    "Fail:" \
    "xor eax,eax" \
    "Done:" \
    parm [ebx] \
    value [eax];

#pragma aux RdosReadHid = \
    CallGate_read_hid \
    CarryToBool \
    parm [ebx] [edi] [ecx] [eax] \
    value [eax];

#pragma aux RdosWriteHid = \
    CallGate_write_hid \
    CarryToBool \
    parm [ebx] [edi] [ecx] \
    value [eax];
    
#pragma aux RdosOpenICSP = \
    CallGate_open_icsp \
    ValidateHandle \
    parm [eax] \
    value [ebx];

#pragma aux RdosCloseICSP = \
    CallGate_close_icsp \
    parm [ebx];

#pragma aux RdosWriteICSPCommand = \
    CallGate_write_icsp_cmd \
    CarryToBool \
    parm [ebx] [eax] \
    value [eax];

#pragma aux RdosWriteICSPData = \
    CallGate_write_icsp_data \
    CarryToBool \
    parm [ebx] [eax] \
    value [eax];

#pragma aux RdosReadICSPData = \
    CallGate_read_icsp_data \
    "mov [edi],eax" \
    CarryToBool \
    parm [ebx] [edi] \
    value [eax];

#pragma aux RdosHasAudio = \
    CallGate_has_audio \
    CarryToBool \
    value [eax];

#pragma aux RdosSetCodecGpio0 = \
    CallGate_set_codec_gpio0 \
    parm [eax] \
    modify [eax];

#pragma aux RdosGetMasterVolume = \
    CallGate_get_master_volume \
    "mov cx,ax" \
    "mov dl,0x7F" \
    "sub dl,al" \
    "movsx edx,dl" \ 
    "mov eax,200" \
    "imul edx" \
    "sar eax,8" \
    "mov [esi],eax" \    
    "mov dl,0x7F" \
    "sub dl,ch" \
    "movsx edx,dl" \ 
    "mov eax,200" \
    "imul edx" \
    "sar eax,8" \
    "mov [edi],eax" \    
    parm [esi] [edi] \
    modify [eax cx edx];

#pragma aux RdosSetMasterVolume = \
    "mov ecx,edx" \
    "mov esi,eax" \
    "xor edx,edx" \
    "shl eax,8" \
    "sbb edx,0" \
    "mov esi,200" \
    "idiv esi" \
    "mov bl,0x7F" \
    "sub bl,al" \
    "adc bl,0" \
    "mov eax,ecx" \
    "mov esi,eax" \
    "xor edx,edx" \
    "shl eax,8" \
    "sbb edx,0" \
    "mov esi,200" \
    "idiv esi" \
    "mov bh,0x7F" \
    "sub bh,al" \
    "adc bh,0" \
    "mov ax,bx" \
    CallGate_set_master_volume \
    parm [eax] [edx] \
    modify [eax ebx ecx edx esi];

#pragma aux RdosGetLineOutVolume = \
    CallGate_get_line_out_volume \
    "mov cx,ax" \
    "mov dl,0x7F" \
    "sub dl,al" \
    "movsx edx,dl" \ 
    "mov eax,200" \
    "imul edx" \
    "sar eax,8" \
    "mov [esi],eax" \    
    "mov dl,0x7F" \
    "sub dl,ch" \
    "movsx edx,dl" \ 
    "mov eax,200" \
    "imul edx" \
    "sar eax,8" \
    "mov [edi],eax" \    
    parm [esi] [edi] \
    modify [eax cx edx];

#pragma aux RdosSetLineOutVolume = \
    "mov ecx,edx" \
    "mov esi,eax" \
    "xor edx,edx" \
    "shl eax,8" \
    "sbb edx,0" \
    "mov esi,200" \
    "idiv esi" \
    "mov bl,0x7F" \
    "sub bl,al" \
    "adc bl,0" \
    "mov eax,ecx" \
    "mov esi,eax" \
    "xor edx,edx" \
    "shl eax,8" \
    "sbb edx,0" \
    "mov esi,200" \
    "idiv esi" \
    "mov bh,0x7F" \
    "sub bh,al" \
    "adc bh,0" \
    "mov ax,bx" \
    CallGate_set_line_out_volume \
    parm [eax] [edx] \
    modify [eax ebx ecx edx esi];

#pragma aux RdosCreateAudioOutChannel = \
    "push eax" \
    "mov eax,edx" \
    "shl eax,16" \
    "xor edx,edx" \
    "mov ebx,100" \
    "div ebx" \
    "sub eax,1" \
    "adc eax,0" \
    "mov dx,ax" \
    "pop eax" \
    CallGate_create_audio_out_channel \
    ValidateHandle \
    parm [eax] [ecx] [edx] \
    modify [ebx] \
    value [ebx];

#pragma aux RdosCloseAudioOutChannel = \
    CallGate_close_audio_out_channel \
    parm [ebx];

#pragma aux RdosWriteAudio = \
    CallGate_write_audio \
    parm [ebx] [ecx] [esi] [edi];

#pragma aux RdosOpenFm = \
    CallGate_open_fm \
    ValidateHandle \
    parm [eax] \
    value [ebx];

#pragma aux RdosCloseFm = \
    CallGate_close_fm \
    parm [ebx];

#pragma aux RdosFmWait = \
    CallGate_fm_wait \
    parm [ebx] [eax];

#pragma aux RdosCreateFmInstrument = \
    CallGate_create_fm_instrument \
    ValidateHandle \
    parm [ebx] [eax] [edx] [8087] \
    value [ebx];

#pragma aux RdosFreeFmInstrument = \
    CallGate_free_fm_instrument \
    parm [ebx];

#pragma aux RdosSetFmAttack = \
    CallGate_set_fm_attack \
    parm [ebx] [eax];

#pragma aux RdosSetFmSustain = \
    CallGate_set_fm_sustain \
    parm [ebx] [eax] [edx];

#pragma aux RdosSetFmRelease = \
    CallGate_set_fm_release \
    parm [ebx] [eax] [edx];

#pragma aux RdosPlayFmNote = \
    CallGate_play_fm_note \
    parm [ebx] [8087] [eax] [edx] [ecx];

#elif __COMPACT__
 
/* 32-bit compact memory model (device-drivers) */

// check carry flag, and set eax=0 if set and eax=1 if clear
#define CarryToBool 0x73 4 0x33 0xC0 0xEB 5 0xB8 1 0 0 0

// check carry flag, and set ebx=0 if set and ebx=bx if clear
#define ValidateHandle 0x73 2 0x33 0xDB 0xF 0xB7 0xDB

// check carry flag, and set eax=0 if set
#define ValidateEax 0x73 2 0x33 0xC0

// check carry flag, and set ecx=0 if set
#define ValidateEcx 0x73 2 0x33 0xC9

// check carry flag, and set edx=0 if set
#define ValidateEdx 0x73 2 0x33 0xD2

// check carry flag, and set esi=0 if set
#define ValidateEsi 0x73 2 0x33 0xF6

// check carry flag, and set edi=0 if set
#define ValidateEdi 0x73 2 0x33 0xFF

// check disc id, set to -1 on carry, extend to eax
#define ValidateDisc 0x73 2 0xB0 0xFF 0xF 0xBE 0xC0

#pragma aux RdosDebug = \
    "int 3";

#pragma aux RdosLoad32 = \
    CallGate_load_device32  \

#pragma aux RdosSwapShort = \
    "xchg al,ah"    \
    parm [ax]   \
    value [ax];

#pragma aux RdosSwapLong = \
    "xchg al,ah"    \
    "rol eax,16"    \
    "xchg al,ah"    \
    parm [eax]   \
    value [eax];

#pragma aux RdosGetCharSize = \
    "mov al,1" \
    "mov ah,es:[edi]" \
    "test ah,80h" \
    "jz size_ok" \
    "inc al" \
    "test ah,20h" \
    "jz size_ok" \
    "inc al" \
    "test ah,10h" \
    "jz size_ok" \
    "inc al" \
    "size_ok: " \
    "movzx eax,al" \
    parm [es edi]   \
    value [eax];

#pragma aux RdosGetLongRandom = \
    CallGate_get_random  \
    value [eax];

#pragma aux RdosGetRandom = \
    CallGate_get_random  \
    "mul edx" \
    parm [edx] \
    value [edx] \
    modify [eax];

#pragma aux RdosSetClipRect = \
    CallGate_set_clip_rect  \
    parm [ebx] [ecx] [edx] [esi] [edi];

#pragma aux RdosClearClipRect = \
    CallGate_clear_clip_rect  \
    parm [ebx];

#pragma aux RdosSetDrawColor = \
    CallGate_set_drawcolor  \
    parm [ebx] [eax];

#pragma aux RdosSetLGOP = \
    CallGate_set_lgop  \
    parm [ebx] [eax];

#pragma aux RdosSetHollowStyle = \
    CallGate_set_hollow_style  \
    parm [ebx];

#pragma aux RdosSetFilledStyle = \
    CallGate_set_filled_style  \
    parm [ebx];

#pragma aux RdosAnsiToUtf8 = \
    "push ds" \
    "mov eax,fs" \
    "mov ds,eax" \ 
    CallGate_ansi_to_utf8  \
    "pop ds" \
    parm [fs esi] [es edi] [ecx]  \
    value [eax];

#pragma aux RdosUtf8ToAnsi = \
    "push ds" \
    "mov eax,fs" \
    "mov ds,eax" \ 
    CallGate_utf8_to_ansi  \
    "pop ds" \
    parm [fs esi] [es edi] [ecx]  \
    value [eax];

#pragma aux RdosOpenFont = \
    CallGate_open_font  \
    ValidateHandle \
    parm [edx] [eax]  \
    value [ebx];

#pragma aux RdosCloseFont = \
    CallGate_close_font  \
    parm [ebx];

#pragma aux RdosGetStringMetrics = \
    CallGate_get_string_metrics  \
    "movzx ecx,cx" \
    "movzx edx,dx" \
    "mov es:[eax],ecx" \
    "mov fs:[esi],edx" \
    parm [ebx] [edi] [es eax] [fs esi] \
    modify [ecx edx];

#pragma aux RdosSetFont = \
    CallGate_set_font  \
    parm [ebx] [eax];

#pragma aux RdosGetPixel = \
    CallGate_get_pixel  \
    parm [ebx] [ecx] [edx]  \
    value [eax];

#pragma aux RdosSetPixel = \
    CallGate_set_pixel  \
    parm [ebx] [ecx] [edx];

// Blit here
// DrawMask here

#pragma aux RdosDrawLine = \
    CallGate_draw_line  \
    parm [ebx] [ecx] [edx] [esi] [edi];

#pragma aux RdosDrawString = \
    CallGate_draw_string  \
    parm [ebx] [ecx] [edx] [es edi];

#pragma aux RdosDrawRect = \
    CallGate_draw_rect  \
    parm [ebx] [ecx] [edx] [esi] [edi];

#pragma aux RdosDrawEllipse = \
    CallGate_draw_ellipse  \
    parm [ebx] [ecx] [edx] [esi] [edi];

#pragma aux RdosCreateBitmap = \
    CallGate_create_bitmap  \
    ValidateHandle \
    parm [eax] [ecx] [edx] \
    value [ebx];

#pragma aux RdosDuplicateBitmapHandle = \
    CallGate_dup_bitmap_handle  \
    ValidateHandle  \
    parm [ebx]  \
    value [ebx];

#pragma aux RdosCloseBitmap = \
    CallGate_close_bitmap  \
    parm [ebx];

#pragma aux RdosCreateStringBitmap = \
    CallGate_create_string_bitmap  \
    ValidateHandle  \
    parm [ebx] [es edi]    \
    value [ebx];


// GetBitmapInfo here

#pragma aux RdosCreateSprite = \
    CallGate_create_sprite  \
    ValidateHandle  \
    parm [ebx] [ecx] [edx] [eax]    \
    value [ebx];

#pragma aux RdosCloseSprite = \
    CallGate_close_sprite  \
    parm [ebx];

#pragma aux RdosShowSprite = \
    CallGate_show_sprite  \
    parm [ebx];

#pragma aux RdosHideSprite = \
    CallGate_hide_sprite  \
    parm [ebx];

#pragma aux RdosMoveSprite = \
    CallGate_move_sprite  \
    parm [ebx] [ecx] [edx];

#pragma aux RdosSetForeColor = \
    CallGate_set_forecolor  \
    parm [eax];

#pragma aux RdosSetBackColor = \
    CallGate_set_backcolor  \
    parm [eax];

#pragma aux RdosGetFreePhysical = \
    CallGate_get_free_physical  \
    value [eax];

#pragma aux RdosGetFreeGdt = \
    CallGate_get_free_gdt  \
    "movzx eax,ax"  \
    value [eax];

#pragma aux RdosGetFreeSmallKernelLinear = \
    CallGate_available_small_linear  \
    value [eax];

#pragma aux RdosGetFreeBigKernelLinear = \
    CallGate_available_big_linear  \
    value [eax];

#pragma aux RdosGetFreeSmallLocalLinear = \
    CallGate_available_small_local_linear  \
    value [eax];

#pragma aux RdosGetFreeBigLocalLinear = \
    CallGate_available_big_local_linear  \
    value [eax];

#pragma aux RdosGetMaxComPort = \
    CallGate_get_max_com_port  \
    "movzx eax,al"  \
    value [eax];

#pragma aux RdosOpenCom = \
    CallGate_open_com  \
    ValidateHandle  \
    parm [al] [ecx] [bh] [ah] [bl] [esi] [edi]  \
    value [ebx];

#pragma aux RdosCloseCom = \
    CallGate_close_com  \
    parm [ebx];

#pragma aux RdosFlushCom = \
    CallGate_flush_com  \
    parm [ebx];

#pragma aux RdosReadCom = \
    CallGate_read_com  \
    parm [ebx]  \
    value [al];

#pragma aux RdosWriteCom = \
    CallGate_write_com  \
    "movsx eax,al"  \
    parm [ebx] [al] \
    value [eax];

#pragma aux RdosEnableCts = \
    CallGate_enable_cts  \
    parm [ebx];

#pragma aux RdosDisableCts = \
    CallGate_disable_cts  \
    parm [ebx];

#pragma aux RdosEnableAutoRts = \
    CallGate_enable_auto_rts  \
    parm [ebx];

#pragma aux RdosDisableAutoRts = \
    CallGate_disable_auto_rts  \
    parm [ebx];

#pragma aux RdosSetDtr = \
    CallGate_set_dtr  \
    parm [ebx];

#pragma aux RdosResetDtr = \
    CallGate_reset_dtr  \
    parm [ebx];

#pragma aux RdosSetRts = \
    CallGate_set_rts  \
    parm [ebx];

#pragma aux RdosResetRts = \
    CallGate_reset_rts  \
    parm [ebx];

#pragma aux RdosGetReceiveBufferSpace = \
    CallGate_get_com_receive_space  \
    ValidateEax \
    parm [ebx]  \
    value [eax];

#pragma aux RdosGetSendBufferSpace = \
    CallGate_get_com_send_space  \
    ValidateEax \
    parm [ebx]  \
    value [eax];

#pragma aux RdosWaitForSendCompletedCom = \
    CallGate_wait_for_send_completed_com  \
    parm [ebx];

#pragma aux RdosGetMaxPrinters = \
    CallGate_get_max_printer  \
    "movzx eax,al"  \
    value [eax];

#pragma aux RdosOpenPrinter = \
    CallGate_open_printer  \
    ValidateHandle  \
    parm [al]  \
    value [ebx];

#pragma aux RdosClosePrinter = \
    CallGate_close_printer  \
    parm [ebx];

#pragma aux RdosGetPrinterName = \
    CallGate_get_printer_name  \
    CarryToBool \
    parm [ebx] [es edi] \
    value [eax];

#pragma aux RdosIsPrinterJammed = \
    CallGate_is_printer_jammed  \
    "cmc" \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosIsPrinterPaperLow = \
    CallGate_is_printer_paper_low  \
    "cmc" \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosIsPrinterPaperEnd = \
    CallGate_is_printer_paper_end  \
    "cmc" \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosIsPrinterOk = \
    CallGate_is_printer_ok  \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosIsPrinterHeadLifted = \
    CallGate_is_printer_head_lifted  \
    "cmc" \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosHasPrinterPaperInPresenter = \
    CallGate_has_printer_paper_in_presenter  \
    "cmc" \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosPrintTest = \
    CallGate_print_test  \
    parm [ebx];

#pragma aux RdosCreatePrinterBitmap = \
    CallGate_create_printer_bitmap  \
    "mov ebx,eax" \
    ValidateHandle \
    parm [ebx] [edx] \
    value [ebx];

#pragma aux RdosPrintBitmap = \
    CallGate_print_bitmap  \
    parm [ebx] [eax];

#pragma aux RdosPresentPrinterMedia = \
    CallGate_present_printer_media  \
    parm [ebx] [eax];

#pragma aux RdosEjectPrinterMedia = \
    CallGate_eject_printer_media  \
    parm [ebx];

#pragma aux RdosWaitForPrint = \
    CallGate_wait_for_print  \
    parm [ebx];

#pragma aux RdosOpenFile = \
    CallGate_open_file  \
    ValidateHandle  \
    parm [es edi] [cl] \
    value [ebx];

#pragma aux RdosCreateFile = \
    CallGate_create_file  \
    ValidateHandle  \
    parm [es edi] [ecx] \
    value [ebx];

#pragma aux RdosCloseFile = \
    CallGate_close_file  \
    parm [ebx];

#pragma aux RdosIsDevice = \
    CallGate_get_ioctl_data  \
    0x33 0xC0 0xF6 0xC6 0x80 0x74 1 0x40 \
    parm [ebx]  \
    value [eax] \
    modify [dx];

#pragma aux RdosDuplFile = \
    CallGate_dupl_file  \
    ValidateHandle  \
    parm [eax]  \
    value [ebx];

#pragma aux RdosGetFileSize = \
    CallGate_get_file_size  \
    ValidateEax \
    parm [ebx]  \
    value [eax];

#pragma aux RdosSetFileSize = \
    CallGate_set_file_size  \
    parm [ebx] [eax];
    
#pragma aux RdosGetFilePos = \
    CallGate_get_file_pos  \
    ValidateEax \
    parm [ebx]  \
    value [eax];

#pragma aux RdosSetFilePos = \
    CallGate_set_file_pos  \
    parm [ebx] [eax];
    
#pragma aux RdosReadFile = \
    CallGate_read_file  \
    ValidateEax \
    parm [ebx] [es edi] [ecx]  \
    value [eax];

#pragma aux RdosWriteFile = \
    CallGate_write_file  \
    ValidateEax \
    parm [ebx] [es edi] [ecx]  \
    value [eax];

#pragma aux RdosGetFileTime = \
    CallGate_get_file_time  \
    "mov fs:[esi],edx" \
    "mov es:[edi],eax" \
    parm [ebx] [fs esi] [es edi]  \
    modify [eax edx];

#pragma aux RdosSetFileTime = \
    CallGate_set_file_time  \
    parm [ebx] [edx] [eax];

#pragma aux RdosCreateMapping = \
    CallGate_create_mapping  \
    ValidateHandle  \
    parm [eax]  \
    value [ebx];

#pragma aux RdosCreateNamedMapping = \
    CallGate_create_named_mapping  \
    ValidateHandle  \
    parm [es edi] [eax]  \
    value [ebx];

#pragma aux RdosCreateNamedFileMapping = \
    CallGate_create_named_file_mapping  \
    ValidateHandle  \
    parm [es edi] [eax] [ebx]  \
    value [ebx];

#pragma aux RdosOpenNamedMapping = \
    CallGate_open_named_mapping  \
    ValidateHandle  \
    parm [es edi] \
    value [ebx];

#pragma aux RdosSyncMapping = \
    CallGate_sync_mapping  \
    parm [ebx];

#pragma aux RdosCloseMapping = \
    CallGate_close_mapping  \
    parm [ebx];

#pragma aux RdosMapView = \
    CallGate_map_view  \
    parm [ebx] [eax] [es edi] [ecx];

#pragma aux RdosUnmapView = \
    CallGate_unmap_view  \
    parm [ebx];

#pragma aux RdosSetCurDrive = \
    CallGate_set_cur_drive  \
    CarryToBool \
    parm [eax]  \
    value [eax];

#pragma aux RdosGetCurDrive = \
    CallGate_get_cur_drive  \
    "movzx eax,al"  \
    value [eax];

#pragma aux RdosGetCurDir = \
    CallGate_get_cur_dir  \
    CarryToBool \
    parm [eax] [es edi]  \
    value [eax];

#pragma aux RdosSetCurDir = \
    CallGate_set_cur_dir  \
    CarryToBool \
    parm [es edi]  \
    value [eax];

#pragma aux RdosMakeDir = \
    CallGate_make_dir  \
    CarryToBool \
    parm [es edi]  \
    value [eax];

#pragma aux RdosRemoveDir = \
    CallGate_remove_dir  \
    CarryToBool \
    parm [es edi]  \
    value [eax];

#pragma aux RdosRenameFile = \
    "push ds" \
    "mov ds,edx" \
    CallGate_rename_file  \
    CarryToBool \
    "pop ds" \
    parm [es edi] [edx esi]  \
    value [eax];

#pragma aux RdosDeleteFile = \
    CallGate_delete_file  \
    CarryToBool \
    parm [es edi]  \
    value [eax];

#pragma aux RdosGetFileAttribute = \
    CallGate_get_file_attribute  \
    "movzx ecx,cx"  \
    "mov fs:[eax],ecx" \
    CarryToBool \
    parm [es edi] [fs eax]  \
    value [eax] \
    modify [ecx];

#pragma aux RdosSetFileAttribute = \
    CallGate_set_file_attribute  \
    CarryToBool \
    parm [es edi] [ecx] \
    value [eax];

#pragma aux RdosOpenDir = \
    CallGate_open_dir  \
    ValidateHandle \
    parm [es edi]  \
    value [ebx];

#pragma aux RdosCloseDir = \
    CallGate_close_dir  \
    parm [ebx];

// ReadDir here

#pragma aux RdosGetImageHeader = \
    CallGate_get_image_header  \
    CarryToBool \
    parm [eax] [edx] [es edi] \
    value [eax];

#pragma aux RdosGetImageData = \
    CallGate_get_image_data  \
    CarryToBool \
    parm [eax] [edx] [es edi] \
    value [eax];

#pragma aux RdosGetDeviceInfo = \
    "push ds" \
    "mov ds,eax" \
    "push edx" \
    CallGate_get_device_info  \
    "mov [ecx],eax" \
    CarryToBool \
    "mov gs:[esi],edx" \
    "pop edx" \
    "mov fs:[edx],bx" \
    "pop ds" \
    parm [ebx] [es edi] [eax ecx] [fs edx] [gs esi] \
    modify [ebx edx] \
    value [eax];

#pragma aux RdosGetSelectorInfo = \
    CallGate_get_selector_info  \
    "mov fs:[esi],ecx" \
    "movzx eax,al" \
    "mov es:[edi],eax" \
    CarryToBool \
    parm [bx] [fs esi] [es edi] \
    modify [ecx] \
    value [eax];

#pragma aux RdosSoftReset = \
    CallGate_soft_reset;

#pragma aux RdosHardReset = \
    CallGate_hard_reset;

#pragma aux RdosPowerFailure = \
    CallGate_power_failure \
    "movzx eax,ax"  \
    value [eax];

#pragma aux RdosIsEmergencyStopped = \
    CallGate_is_emergency_stopped \
    CarryToBool \    
    value [eax];

#pragma aux RdosHasGlobalTimer = \
    CallGate_has_global_timer \
    CarryToBool \    
    value [eax];

#pragma aux RdosGetCoreLoad = \
    CallGate_get_core_load \
    "mov fs:[esi],ebx" \
    "mov fs:[esi+4],ecx" \
    "mov es:[edi],eax" \
    "mov es:[edi+4],edx" \
    CarryToBool \    
    parm [eax] [fs esi] [es edi] \
    value [eax] \
    modify [ebx ecx edx];

#pragma aux RdosGetCoreDuty = \
    CallGate_get_core_duty \
    "mov fs:[esi],eax" \
    "mov fs:[esi+4],edx" \
    "mov es:[edi],ebx" \
    "mov es:[edi+4],ecx" \
    CarryToBool \    
    parm [eax] [fs esi] [es edi] \
    value [eax] \
    modify [ebx ecx edx];

#pragma aux RdosGetVersion = \
    CallGate_get_version  \
    "movzx edx,dx"  \
    "mov gs:[ebx],edx" \
    "movzx  eax,ax" \
    "mov fs:[esi],eax" \
    "movzx ecx,cx"  \
    "mov es:[edi],ecx" \
    parm [gs ebx] [fs esi] [es edi]  \
    modify [eax ecx edx];

#pragma aux RdosGetCpuVersion = \
    CallGate_get_cpu_version  \
    "movzx  eax,al" \
    "mov fs:[esi],edx" \
    "mov gs:[ecx],ebx" \
    parm [es edi] [fs esi] [gs ecx] \
    value [eax];

#pragma aux RdosTerminateThread = \
    CallGate_terminate_thread;

#pragma aux RdosGetThreadHandle = \
    CallGate_get_thread \
    "movzx eax,ax"  \
    value [eax];

#pragma aux RdosWaitMilli = \
    CallGate_wait_milli  \
    parm [eax];

#pragma aux RdosWaitMicro = \
    CallGate_wait_micro  \
    parm [eax];

#pragma aux RdosWaitUntil = \
    CallGate_wait_until  \
    parm [edx] [eax];

#pragma aux RdosGetSysTime = \
    CallGate_get_system_time  \
    "mov fs:[esi],edx" \
    "mov es:[edi],eax" \
    parm [fs esi] [es edi] \
    modify [eax edx];

#pragma aux RdosGetTime = \
    CallGate_get_time  \
    "mov fs:[esi],edx" \
    "mov es:[edi],eax" \
    parm [fs esi] [es edi] \
    modify [eax edx];

#pragma aux RdosSetTime = \
    CallGate_get_system_time  \
    "sub esi,eax" \
    "sbb edi,edx" \
    "mov eax,esi" \
    "mov edx,edi" \
    CallGate_update_time  \
    CallGate_update_rtc  \
    parm [edi] [esi] \
    modify [eax edx esi edi];

#pragma aux RdosDayOfWeek = \
    "mov ch,al" \
    "xor ebx,ebx"   \
    "xor ah,ah" \
    CallGate_adjust_time  \
    "push edx"   \
    "mov eax,365"   \
    "imul dx"   \
    "push dx"   \
    "push ax"   \
    "pop ebx"   \
    "pop edx"   \
    CallGate_passed_days  \
    "dec edx"   \
    "shr edx,2" \
    "inc edx"   \
    "add ax,dx" \
    "add eax,ebx"   \
    "xor edx,edx"   \
    "add eax,5" \
    "mov ebx,7" \
    "div ebx"   \
    "movzx eax,dl" \
    parm [edx] [eax] [ecx] \
    value [eax] \
    modify [ebx ecx edx];

#pragma aux RdosDosTimeDateToTics = \
    "push ebx" \
    "push ecx" \
    "mov dx,si" \
    "mov ax,dx" \
    "shr dx,9" \
    "add dx,1980" \
    "mov cx,ax" \
    "shr cx,5" \
    "mov ch,cl" \
    "and ch,0Fh" \
    "mov cl,al" \
    "and cl,1Fh" \
    "mov bx,di" \
    "mov ax,bx" \
    "shr bx,11" \
    "mov bh,bl" \
    "shr ax,5" \
    "and al,3Fh" \
    "mov bl,al" \
    "mov ax,di" \
    "mov ah,al" \
    "add ah,ah" \
    "and ah,3Fh" \
    CallGate_time_to_binary  \
    "pop ecx" \
    "pop ebx" \
    "mov fs:[ebx],edx" \
    "mov es:[ecx],eax" \
    parm [si] [di] [fs ebx] [es ecx] \
    modify [eax edx];

#pragma aux RdosTicsToDosTimeDate = \
    CallGate_binary_to_time  \
    "shl cl,3" \
    "shr cx,3" \
    "sub dx,1980" \
    "mov dh,dl" \
    "shl dh,1" \
    "xor dl,dl" \
    "or dx,cx" \
    "mov al,ah" \
    "shr al,1" \
    "shl bl,2" \
    "shl bx,3" \
    "or bl,al" \
    "mov fs:[esi],dx" \
    "mov es:[edi],bx" \
    parm [edx] [eax] [fs esi] [es edi] \
    modify [eax ebx ecx edx];

#pragma aux RdosDecodeMsbTics = \
    "push ds" \
    "mov ds,eax" \
    "push ebx" \
    "push ecx" \
    "xor eax,eax" \
    CallGate_binary_to_time  \
    "movzx eax,bh" \
    "mov es:[edi],eax" \
    "movzx eax,cl" \
    "mov [esi],eax" \
    "movzx eax,ch" \
    "pop ecx" \
    "pop ebx" \
    "mov gs:[ecx],eax" \
    "movzx eax,dx" \
    "mov fs:[ebx],eax" \
    "pop ds" \
    parm [edx] [fs ebx] [gs ecx] [eax esi] [es edi] \
    modify [eax edx];

#pragma aux RdosDecodeLsbTics = \
    "push ds" \
    "mov ds,edx" \
    "mov edx,60" \
    "mul edx" \
    "mov fs:[ebx],edx" \
    "mov edx,60" \
    "mul edx" \
    "mov gs:[ecx],edx" \
    "mov edx,1000" \
    "mul edx" \
    "mov [esi],edx" \
    "mov edx,1000" \
    "mul edx" \
    "mov es:[edi],edx" \
    "pop ds" \
    parm [eax] [fs ebx] [gs ecx] [edx esi] [es edi] \
    modify [eax edx];

#pragma aux RdosCodeMsbTics = \
    "mov ch,al" \
    "mov bh,bl" \
    "xor bl,bl" \
    "xor ah,ah" \
    CallGate_time_to_binary  \
    parm [edx] [eax] [ecx] [ebx] \
    value [edx] \
    modify [eax bx cx];

#pragma aux RdosCodeLsbTics = \
    "xor dx,dx" \
    "xor cx,cx" \
    "xor bh,bh" \
    "mov ah,al" \
    CallGate_time_to_binary  \
    "mov ebx,eax" \
    "mov eax,1193046" \
    "mul esi" \
    "mov ecx,eax" \
    "mov eax,1193" \
    "mul edi" \
    "add eax,ecx" \
    "xor edx,edx" \
    "mov ecx,1000" \
    "div ecx" \
    "add eax,ebx" \
    parm [ebx] [eax] [esi] [edi] \
    value [eax] \
    modify [ebx ecx edx];

#pragma aux RdosAddTics = \
    "add fs:[esi],eax" \
    "adc es:[edi],0"  \
    parm [es edi] [fs esi] [eax];

#pragma aux RdosAddMicro = \
    "mov edx,1193"  \
    "imul edx"  \
    "xor edx,edx" \
    "mov ebx,1000" \
    "idiv ebx" \
    "add fs:[esi],eax" \
    "adc dword ptr es:[edi],0"  \
    parm [es edi] [fs esi] [eax] \
    modify [eax ebx edx];

#pragma aux RdosAddMilli = \
    "mov edx,1193"  \
    "imul edx"  \
    "add fs:[esi],eax" \
    "adc es:[edi],edx"  \
    parm [es edi] [fs esi] [eax] \
    modify [eax edx];

#pragma aux RdosAddSec = \
    "mov edx,1193046"  \
    "imul edx"  \
    "add fs:[esi],eax" \
    "adc es:[edi],edx"  \
    parm [es edi] [fs esi] [eax] \
    modify [eax edx];

#pragma aux RdosAddMin = \
    "mov edx,1193046*60"  \
    "imul edx"  \
    "add fs:[esi],eax" \
    "adc es:[edi],edx"  \
    parm [es edi] [fs esi] [eax] \
    modify [eax edx];

#pragma aux RdosAddHour = \
    "add es:[edi],eax"  \
     parm [es edi] [fs esi] [eax];

#pragma aux RdosAddDay = \
    "mov edx,24"  \
    "imul edx"  \
    "add es:[edi],eax"  \
    parm [es edi] [fs esi] [eax] \
    modify [eax edx];

#pragma aux RdosSyncTime = \
    CallGate_sync_time  \
    CarryToBool \
    parm [edx] \
    value [eax];

#pragma aux RdosCreateSection = \
    CallGate_create_user_section  \
    ValidateHandle  \
    value [ebx];

#pragma aux RdosDeleteSection = \
    CallGate_delete_user_section  \
    parm [ebx];

#pragma aux RdosEnterSection = \
    CallGate_enter_user_section  \
    parm [ebx];

#pragma aux RdosLeaveSection = \
    CallGate_leave_user_section  \
    parm [ebx];

#pragma aux RdosGetFreeHandles = \
    CallGate_get_free_handles  \
    "movzx eax,ax"  \
    value [eax];

#pragma aux RdosGetFreeHandleMem = \
    CallGate_get_free_handle_mem  \
    value [eax];

#pragma aux RdosCreateWait = \
    CallGate_create_wait  \
    ValidateHandle  \
    value [ebx];

#pragma aux RdosCloseWait = \
    CallGate_close_wait  \
    parm [ebx];

#pragma aux RdosCheckWait = \
    CallGate_is_wait_idle  \
    parm [ebx] \
    value [ecx];

#pragma aux RdosWaitForever = \
    CallGate_wait_no_timeout  \
    ValidateEcx \
    parm [ebx] \
    value [ecx]

#pragma aux RdosWaitTimeout = \
    "mov edx,1193" \
    "mul edx"   \
    "push edx"  \
    "push eax"  \
    CallGate_get_system_time \
    "pop ecx"   \
    "add eax,ecx"   \
    "pop ecx"   \
    "adc edx,ecx"   \
    CallGate_wait_timeout  \
    ValidateEcx \
    parm [ebx] [eax] \
    value [ecx] \
    modify [eax edx];

#pragma aux RdosWaitUntilTimeout = \
    CallGate_wait_timeout  \
    ValidateEcx \
    parm [ebx] [edx] [eax] \
    value [ecx];

#pragma aux RdosStopWait = \
    CallGate_stop_wait  \
    parm [ebx];

#pragma aux RdosRemoveWait = \
    CallGate_remove_wait  \
    parm [ebx] [ecx];

#pragma aux RdosAddWaitForKeyboard = \
    CallGate_add_wait_for_keyboard  \
    parm [ebx] [ecx];

#pragma aux RdosAddWaitForMouse = \
    CallGate_add_wait_for_mouse  \
    parm [ebx] [ecx];

#pragma aux RdosAddWaitForCom = \
    CallGate_add_wait_for_com  \
    parm [ebx] [eax] [ecx];

#pragma aux RdosAddWaitForSyslog = \
    CallGate_add_wait_for_syslog  \
    parm [ebx] [eax] [ecx];

#pragma aux RdosAddWaitForAdc = \
    CallGate_add_wait_for_adc  \
    parm [ebx] [eax] [ecx];

#pragma aux RdosCreateSignal = \
    CallGate_create_signal  \
    ValidateHandle  \
    value [ebx];

#pragma aux RdosResetSignal = \
    CallGate_reset_signal  \
    parm [ebx];

#pragma aux RdosSetSignal = \
    CallGate_set_signal  \
    parm [ebx];

#pragma aux RdosIsSignalled = \
    CallGate_is_signalled  \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosFreeSignal = \
    CallGate_free_signal  \
    parm [ebx];

#pragma aux RdosAddWaitForSignal = \
    CallGate_add_wait_for_signal  \
    parm [ebx] [eax] [ecx];

#pragma aux RdosGetIp = \
    CallGate_get_ip_address  \
    value [edx];

#pragma aux RdosGetGateway = \
    CallGate_get_gateway  \
    value [edx];

#pragma aux RdosNameToIp = \
    CallGate_name_to_ip  \
    parm [es edi] \
    value [edx];

#pragma aux RdosIpToName = \
    CallGate_ip_to_name  \
    parm [edx] [es edi] [ecx] \
    value [eax];

#pragma aux RdosCreateTcpListen = \
    CallGate_create_tcp_listen  \
    ValidateHandle \
    parm [esi] [eax] [ecx] \
    value [ebx];

#pragma aux RdosGetTcpListen = \
    CallGate_get_tcp_listen  \
    "movzx ebx,ax" \
    ValidateHandle \
    parm [ebx] \
    value [ebx] \
    modify [ax];

#pragma aux RdosCloseTcpListen = \
    CallGate_close_tcp_listen  \
    parm [ebx];

#pragma aux RdosAddWaitForTcpListen = \
    CallGate_add_wait_for_tcp_listen  \
    parm [ebx] [eax] [ecx];

#pragma aux RdosOpenTcpConnection = \
    CallGate_open_tcp_connection  \
    ValidateHandle \
    parm [edx] [esi] [edi] [eax] [ecx] \
    value [ebx];

#pragma aux RdosWaitForTcpConnection = \
    CallGate_wait_for_tcp_connection  \
    CarryToBool \
    parm [ebx] [eax] \
    value [eax];

#pragma aux RdosAddWaitForTcpConnection = \
    CallGate_add_wait_for_tcp_connection  \
    parm [ebx] [eax] [ecx];

#pragma aux RdosCloseTcpConnection = \
    CallGate_close_tcp_connection  \
    parm [ebx];

#pragma aux RdosDeleteTcpConnection = \
    CallGate_delete_tcp_connection  \
    parm [ebx];

#pragma aux RdosAbortTcpConnection = \
    CallGate_abort_tcp_connection  \
    parm [ebx];

#pragma aux RdosPushTcpConnection = \
    CallGate_push_tcp_connection  \
    parm [ebx];

#pragma aux RdosIsTcpConnectionClosed = \
    CallGate_is_tcp_connection_closed  \
    "cmc"   \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosIsTcpConnectionIdle = \
    CallGate_is_tcp_connection_idle  \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosGetRemoteTcpConnectionIP = \
    CallGate_get_remote_tcp_connection_ip  \
    ValidateEax \
    parm [ebx] \
    value [eax];

#pragma aux RdosGetRemoteTcpConnectionPort = \
    CallGate_get_remote_tcp_connection_port  \
    "movzx eax,ax"\
    ValidateEax \
    parm [ebx] \
    value [eax];

#pragma aux RdosGetLocalTcpConnectionPort = \
    CallGate_get_local_tcp_connection_port  \
    "movzx eax,ax"\
    ValidateEax \
    parm [ebx] \
    value [eax];

#pragma aux RdosReadTcpConnection = \
    CallGate_read_tcp_connection  \
    parm [ebx] [es edi] [ecx] \
    value [eax];

#pragma aux RdosWriteTcpConnection = \
    CallGate_write_tcp_connection  \
    parm [ebx] [es edi] [ecx] \
    value [eax];

#pragma aux RdosPollTcpConnection = \
    CallGate_poll_tcp_connection  \
    parm [ebx] \
    value [eax];

#pragma aux RdosGetLocalMailslot = \
    CallGate_get_local_mailslot  \
    ValidateHandle \
    parm [es edi] \
    value [ebx];

#pragma aux RdosGetRemoteMailslot = \
    CallGate_get_remote_mailslot  \
    ValidateHandle \
    parm [edx] [es edi] \
    value [ebx];

#pragma aux RdosFreeMailslot = \
    CallGate_free_mailslot  \
    parm [ebx];

#pragma aux RdosSendMailslot = \
    "push ds" \
    "mov ds,edx" \
    CallGate_send_mailslot  \
    ValidateEcx \
    "pop ds" \
    parm [ebx] [edx esi] [ecx] [es edi] [eax] \
    value [ecx];

#pragma aux RdosDefineMailslot = \
    CallGate_define_mailslot  \
    parm [es edi] [ecx];

#pragma aux RdosReceiveMailslot = \
    CallGate_receive_mailslot  \
    parm [es edi] \
    value [ecx];

#pragma aux RdosReplyMailslot = \
    CallGate_reply_mailslot  \
    parm [es edi] [ecx];

#pragma aux RdosSetFocus = \
    CallGate_set_focus  \
    parm [al];

#pragma aux RdosGetFocus = \
    CallGate_get_focus  \

#pragma aux RdosSetKeyMap = \
    CallGate_set_key_layout \
    CarryToBool \
    parm [es edi] \
    value [eax];

#pragma aux RdosGetKeyMap = \
    CallGate_get_key_layout \
    parm [es edi];

#pragma aux RdosClearKeyboard = \
    CallGate_flush_keyboard;

#pragma aux RdosPollKeyboard = \
    CallGate_poll_keyboard  \
    CarryToBool \
    value [eax];

#pragma aux RdosReadKeyboard = \
    CallGate_read_keyboard  \
    "movzx eax,ax" \
    value [eax];

#pragma aux RdosGetKeyboardState = \
    CallGate_get_keyboard_state  \
    "movzx eax,ax" \
    value [eax];

#pragma aux RdosPutKeyboard = \
    "mov dh,cl" \
    CallGate_put_keyboard_code  \
    parm [eax] [edx] [ecx] \
    modify [dh];

#pragma aux RdosPeekKeyEvent = \
    "push ds" \
    "mov ds,edx" \
    "push ecx" \
    CallGate_peek_key_event  \
    "movzx eax,ax" \
    "mov fs:[ebx],eax" \
    "movzx eax,cx" \
    "pop ecx" \
    "mov gs:[ecx],eax" \
    "movzx eax,dl" \
    "mov [esi],eax" \
    "movzx eax,dh" \
    "mov es:[edi],eax" \
    CarryToBool \
    "pop ds" \
    parm [fs ebx] [gs ecx] [edx esi] [es edi] \
    value [eax] \
    modify [dx];

#pragma aux RdosReadKeyEvent = \
    "push ds" \
    "mov ds,edx" \
    "push ecx" \
    CallGate_read_key_event  \
    "movzx eax,ax" \
    "mov fs:[ebx],eax" \
    "movzx eax,cx" \
    "pop ecx" \
    "mov gs:[ecx],eax" \
    "movzx eax,dl" \
    "mov [esi],eax" \
    "movzx eax,dh" \
    "mov es:[edi],eax" \
    CarryToBool \
    "pop ds" \
    parm [fs ebx] [gs ecx] [edx esi] [es edi] \
    value [eax] \
    modify [dx];

#pragma aux RdosHideMouse = \
    CallGate_hide_mouse;

#pragma aux RdosShowMouse = \
    CallGate_show_mouse;

#pragma aux RdosGetMousePosition = \
    CallGate_get_mouse_position \
    "movzx ecx,cx" \
    "mov fs:[esi],ecx" \
    "movzx edx,dx" \
    "mov es:[edi],edx" \
    parm [fs esi] [es edi] \
    modify [ecx edx];

#pragma aux RdosSetMousePosition = \
    CallGate_set_mouse_position \
    parm [ecx] [edx];

#pragma aux RdosSetMouseWindow = \
    CallGate_set_mouse_window \
    parm [eax] [ebx] [ecx] [edx];

#pragma aux RdosSetMouseMickey = \
    CallGate_set_mouse_mickey \
    parm [ecx] [edx];

#pragma aux RdosGetLeftButton = \
    CallGate_get_left_button \
    CarryToBool \
    value [eax];

#pragma aux RdosGetRightButton = \
    CallGate_get_right_button \
    CarryToBool \
    value [eax];

#pragma aux RdosGetLeftButtonPressPosition = \
    CallGate_get_left_button_press_position \
    "movzx ecx,cx" \
    "mov fs:[esi],ecx" \
    "movzx edx,dx" \
    "mov es:[edi],edx" \
    parm [fs esi] [es edi] \
    modify [ecx edx];

#pragma aux RdosGetRightButtonPressPosition = \
    CallGate_get_right_button_press_position \
    "movzx ecx,cx" \
    "mov fs:[esi],ecx" \
    "movzx edx,dx" \
    "mov es:[edi],edx" \
    parm [fs esi] [es edi] \
    modify [ecx edx];

#pragma aux RdosGetLeftButtonReleasePosition = \
    CallGate_get_left_button_release_position \
    "movzx ecx,cx" \
    "mov fs:[esi],ecx" \
    "movzx edx,dx" \
    "mov es:[edi],edx" \
    parm [fs esi] [es edi] \
    modify [ecx edx];

#pragma aux RdosGetRightButtonReleasePosition = \
    CallGate_get_right_button_release_position \
    "movzx ecx,cx" \
    "mov fs:[esi],ecx" \
    "movzx edx,dx" \
    "mov es:[edi],edx" \
    parm [fs esi] [es edi] \
    modify [ecx edx];

#pragma aux RdosGetCursorPosition = \
    CallGate_get_cursor_position \
    "movzx ecx,cx" \
    "mov fs:[esi],ecx" \
    "movzx edx,dx" \
    "mov es:[edi],edx" \
    parm [es edi] [fs esi] \
    modify [ecx edx];

#pragma aux RdosSetCursorPosition = \
    CallGate_set_cursor_position \
    parm [edx] [ecx];

#pragma aux RdosWriteChar = \
    CallGate_write_char \
    parm [al];

#pragma aux RdosWriteSizeString = \
    CallGate_write_size_string \
    parm [es edi] [ecx];

#pragma aux RdosWriteAttributeString = \
    CallGate_write_attrib_string \
    parm [edx] [eax] [es edi] [ecx];

#pragma aux RdosWriteString = \
    CallGate_write_asciiz  \
    parm [es edi];

#pragma aux RdosReadLine = \
    CallGate_read_con  \
    parm [es edi] [ecx] \
    value [eax];

#pragma aux RdosPing = \
    CallGate_ping  \
    CarryToBool \
    parm [edx] [eax] \
    value [eax];

#pragma aux RdosGetIdeDisc = \
    CallGate_get_ide_disc  \
    ValidateDisc \
    parm [ebx] \
    value [eax];

#pragma aux RdosGetFloppyDisc = \
    CallGate_get_floppy_disc  \
    ValidateDisc \
    parm [ebx] \
    value [eax];

#pragma aux RdosSetDiscInfo = \
    CallGate_set_disc_info  \
    CarryToBool \
    parm [eax] [ecx] [edx] [esi] [edi] \
    value [eax];

#pragma aux RdosGetDiscInfo = \
    "push ds" \
    "mov ds,edx" \
    "push edi" \
    "push esi" \
    "push edx" \
    "push ecx" \
    CallGate_get_disc_info  \
    "pop ebx" \
    "movzx ecx,cx" \
    "mov fs:[ebx],ecx" \
    "pop ebx" \
    "mov gs:[ebx],edx" \
    "pop ebx" \
    "movzx esi,si" \
    "mov ds:[ebx],esi" \
    "pop ebx" \
    "movzx edi,di" \
    "mov es:[ebx],edi" \
    CarryToBool \
    "pop ds" \
    parm [eax] [fs ecx] [gs edx] [edx esi] [es edi] \
    value [eax] \
    modify [ebx ecx edx esi edi];

#pragma aux RdosReadDisc = \
    CallGate_read_disc  \
    CarryToBool \
    parm [eax] [edx] [es edi] [ecx] \
    value [eax];

#pragma aux RdosWriteDisc = \
    CallGate_write_disc  \
    CarryToBool \
    parm [eax] [edx] [es edi] [ecx] \
    value [eax];

#pragma aux RdosGetRdfsInfo = \
    "push gs" \
    "mov gs,edx" \
    CallGate_get_rdfs_info  \
    "pop gs" \
    parm [edx esi] [es edi] [gs ebx] \
    modify [ax];

#pragma aux RdosDemandLoadDrive = \
    CallGate_demand_load_drive  \
    parm [eax];

#pragma aux RdosFormatDrive = \
    CallGate_format_drive  \
    ValidateDisc \
    parm [eax] [edx] [ecx] [es edi] \
    value [eax];

#pragma aux RdosAllocateFixedDrive = \
    CallGate_allocate_fixed_drive  \
    CarryToBool \
    parm [eax] \
    value [eax];

#pragma aux RdosAllocateStaticDrive = \
    CallGate_allocate_static_drive  \
    ValidateDisc \
    value [eax];

#pragma aux RdosAllocateDynamicDrive = \
    CallGate_allocate_dynamic_drive  \
    ValidateDisc \
    value [eax];

#pragma aux RdosGetDriveInfo = \
    CallGate_get_drive_info  \
    "mov gs:[ebx],eax" \
    "movzx ecx,cx" \
    "mov fs:[esi],ecx" \
    "mov es:[edi],edx" \
    CarryToBool \
    parm [eax] [gs ebx] [fs esi] [es edi] \
    value [eax] \
    modify [ecx edx];

#pragma aux RdosGetDriveDiscParam = \
    CallGate_get_drive_disc_param  \
    "movzx eax,al" \
    "mov gs:[ebx],eax" \
    "mov fs:[esi],edx" \
    "mov es:[edi],ecx" \
    CarryToBool \
    parm [eax] [gs ebx] [fs esi] [es edi] \
    value [eax] \
    modify [ecx edx];

#pragma aux RdosCreateFileDrive = \
    "push ds" \
    "mov ds,edx" \
    CallGate_create_file_drive  \
    CarryToBool \
    "pop ds" \
    parm [eax] [ecx] [edx esi] [es edi] \
    value [eax];

#pragma aux RdosOpenFileDrive = \
    CallGate_open_file_drive  \
    CarryToBool \
    parm [eax] [es edi] \
    value [eax];

#pragma aux RdosCreateCrc = \
    CallGate_create_crc  \
    ValidateHandle \
    parm [ax] \
    value [ebx];

#pragma aux RdosCloseCrc = \
    CallGate_close_crc  \
    parm [ebx];

#pragma aux RdosCalcCrc = \
    CallGate_calc_crc  \
    parm [ebx] [ax] [es edi] [ecx] \
    value [ax];

#pragma aux RdosOpenSyslog = \
    CallGate_open_syslog  \
    ValidateHandle \
    value [ebx];

#pragma aux RdosCloseSyslog = \
    CallGate_close_syslog  \
    parm [ebx];

#pragma aux RdosGetSyslog = \
    "push edi" \
    "push esi" \
    "push edx" \
    "push eax" \
    CallGate_get_syslog  \
    "pop edi" \
    "mov gs:[edi],eax" \
    "pop edi" \
    "mov gs:[edi],edx" \
    "pop edi" \
    "movzx eax,si" \
    "xor ah,ah" \
    "mov fs:[edi],eax" \
    "movzx eax,si" \
    "mov al,ah" \
    "xor ah,ah" \        
    parm [ebx] [fs esi] [gs edx] [gs eax] [es edi] [ecx] \
    value [eax];

#pragma aux RdosOpenAdc = \
    CallGate_open_adc  \
    ValidateHandle \
    parm [eax] \
    value [ebx];

#pragma aux RdosCloseAdc = \
    CallGate_close_adc  \
    parm [ebx];

#pragma aux RdosDefineAdcTime = \
    CallGate_define_adc_time  \
    parm [ebx] [edx] [eax];

#pragma aux RdosReadAdc = \
    CallGate_read_adc  \
    parm [ebx] \
    value [eax];

#pragma aux RdosReadSerialLines = \
    "mov dh,cl" \
    CallGate_read_serial_lines  \
    "movzx eax,al" \
    "mov fs:[esi],eax" \
    CarryToBool \
    parm [ecx] [fs esi] \
    value [eax] \
    modify [dh];

#pragma aux RdosToggleSerialLine = \
    "mov dh,cl" \
    CallGate_toggle_serial_line  \
    CarryToBool \
    parm [ecx] [edx] \
    value [eax] \
    modify [dh];

#pragma aux RdosReadSerialVal = \
    "mov dh,cl" \
    CallGate_read_serial_val  \
    "pushf" \
    "shl eax,8" \
    "mov fs:[esi],eax" \
    "popf" \
    CarryToBool \
    parm [ecx] [edx] [fs esi] \
    value [eax] \
    modify [dh];

#pragma aux RdosWriteSerialVal = \
    "mov dh,cl" \
    "sar eax,8" \
    CallGate_write_serial_val  \
    CarryToBool \
    parm [ecx] [edx] [eax] \
    value [eax] \
    modify [dh];

#pragma aux RdosReadSerialRaw = \
    "mov dh,cl" \
    CallGate_read_serial_val  \
    "mov fs:[esi],eax" \
    CarryToBool \
    parm [ecx] [edx] [fs esi] \
    value [eax] \
    modify [dh];

#pragma aux RdosWriteSerialRaw = \
    "mov dh,cl" \
    CallGate_write_serial_val  \
    CarryToBool \
    parm [ecx] [edx] [eax] \
    value [eax] \
    modify [dh];

#pragma aux RdosOpenSysEnv = \
    CallGate_open_sys_env \
    ValidateHandle \
    value [ebx];

#pragma aux RdosOpenProcessEnv = \
    CallGate_open_proc_env \
    ValidateHandle \
    value [ebx];

#pragma aux RdosCloseEnv = \
    CallGate_close_env \
    parm [ebx];

#pragma aux RdosAddEnvVar = \
    "push ds" \
    "mov ds,edx" \
    CallGate_add_env_var \
    "pop ds" \
    parm [ebx] [edx esi] [es edi];

#pragma aux RdosDeleteEnvVar = \
    "push ds" \
    "mov ds,edx" \
    CallGate_delete_env_var \
    "pop ds" \
    parm [ebx] [edx esi];

#pragma aux RdosFindEnvVar = \
    "push ds" \
    "mov ds,edx" \
    CallGate_find_env_var \
    CarryToBool \
    "pop ds" \
    parm [ebx] [edx esi] [es edi] \
    value [eax];

#pragma aux RdosGetEnvSize = \
    CallGate_get_env_size \
    parm [ebx] \
    value [eax];

#pragma aux RdosGetEnvData = \
    "xor ax,ax" \
    "mov es:[edi],ax" \
    CallGate_get_env_data \
    parm [ebx] [es edi] \
    modify [ax];

#pragma aux RdosSetEnvData = \
    CallGate_set_env_data \
    parm [ebx] [es edi];

#pragma aux RdosOpenSysIni = \
    CallGate_open_sys_ini \
    ValidateHandle \
    value [ebx];

#pragma aux RdosOpenIni = \
    CallGate_open_ini \
    ValidateHandle \
    parm [es edi] \
    value [ebx];

#pragma aux RdosCloseIni = \
    CallGate_close_ini \
    parm [ebx];

#pragma aux RdosGotoIniSection = \
    CallGate_goto_ini_section \
    CarryToBool \
    parm [ebx] [es edi] \
    value [eax];

#pragma aux RdosRemoveIniSection = \
    CallGate_remove_ini_section \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosReadIni = \
    "push ds" \
    "mov ds,edx" \
    CallGate_read_ini \
    CarryToBool \
    "pop ds" \
    parm [ebx] [edx esi] [es edi] [ecx] \
    value [eax];

#pragma aux RdosWriteIni = \
    "push ds" \
    "mov ds,edx" \
    CallGate_write_ini \
    CarryToBool \
    "pop ds" \
    parm [ebx] [edx esi] [es edi] \
    value [eax];

#pragma aux RdosDeleteIni = \
    CallGate_delete_ini \
    CarryToBool \
    parm [ebx] [es esi] \
    value [eax];

#pragma aux RdosEnableStatusLED = \
    CallGate_enable_status_led;

#pragma aux RdosDisableStatusLED = \
    CallGate_disable_status_led;

#pragma aux RdosStartWatchdog = \
    CallGate_start_watchdog \
    parm [eax];

#pragma aux RdosKickWatchdog = \
    CallGate_kick_watchdog;

#pragma aux RdosStopWatchdog = \
    CallGate_stop_watchdog;

#pragma aux RdosStartNetCapture = \
    CallGate_start_net_capture \
    parm [ebx];

#pragma aux RdosStopNetCapture = \
    CallGate_stop_net_capture;

#pragma aux RdosGetUsbDevice = \
    CallGate_get_usb_device \
    parm [ebx] [eax] [es edi] [ecx] \
    value [eax];

#pragma aux RdosGetUsbConfig = \
    CallGate_get_usb_config \
    parm [ebx] [eax] [edx] [es edi] [ecx] \
    value [eax];

#pragma aux RdosOpenUsbPipe = \
    CallGate_open_usb_pipe \
    ValidateHandle \
    parm [ebx] [eax] [edx] \
    value [ebx];

#pragma aux RdosCloseUsbPipe = \
    CallGate_close_usb_pipe \
    parm [ebx];

#pragma aux RdosResetUsbPipe = \
    CallGate_reset_usb_pipe \
    parm [ebx];

#pragma aux RdosLockUsbPipe = \
    CallGate_lock_usb_pipe \
    parm [ebx];

#pragma aux RdosUnlockUsbPipe = \
    CallGate_unlock_usb_pipe \
    parm [ebx];

#pragma aux RdosAddWaitForUsbPipe = \
    CallGate_add_wait_for_usb_pipe \
    parm [ebx] [eax] [ecx];

#pragma aux RdosWriteUsbControl = \
    CallGate_write_usb_control \
    parm [ebx] [es edi] [ecx];

#pragma aux RdosReqUsbData = \
    CallGate_req_usb_data \
    parm [ebx] [es edi] [ecx];

#pragma aux RdosGetUsbDataSize = \
    CallGate_get_usb_data_size \
    "movzx eax,ax" \
    parm [ebx] \
    value [eax];

#pragma aux RdosWriteUsbData = \
    CallGate_write_usb_data \
    parm [ebx] [es edi] [ecx];

#pragma aux RdosReqUsbStatus = \
    CallGate_req_usb_status \
    parm [ebx];

#pragma aux RdosWriteUsbStatus = \
    CallGate_write_usb_status \
    parm [ebx];

#pragma aux RdosIsUsbTransactionDone = \
    CallGate_is_usb_trans_done \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosWasUsbTransactionOk = \
    CallGate_was_usb_trans_ok \
    CarryToBool \
    parm [ebx] \
    value [eax];

#pragma aux RdosGetUsbCloseCount = \
    CallGate_get_usb_close_count \
    value [eax];

#pragma aux RdosOpenICSP = \
    CallGate_open_icsp \
    ValidateHandle \
    parm [eax] \
    value [ebx];

#pragma aux RdosCloseICSP = \
    CallGate_close_icsp \
    parm [ebx];

#pragma aux RdosWriteICSPCommand = \
    CallGate_write_icsp_cmd \
    CarryToBool \
    parm [ebx] [eax] \
    value [eax];

#pragma aux RdosWriteICSPData = \
    CallGate_write_icsp_data \
    CarryToBool \
    parm [ebx] [eax] \
    value [eax];

#pragma aux RdosReadICSPData = \
    CallGate_read_icsp_data \
    "mov es:[edi],eax" \
    CarryToBool \
    parm [ebx] [es edi] \
    value [eax];

#pragma aux RdosSetCodecGpio0 = \
    CallGate_set_codec_gpio0 \
    parm [eax] \
    modify [eax];

#pragma aux RdosGetMasterVolume = \
    CallGate_get_master_volume \
    "mov cx,ax" \
    "mov dl,0x7F" \
    "sub dl,al" \
    "movsx edx,dl" \ 
    "mov eax,200" \
    "imul edx" \
    "sar eax,8" \
    "mov fs:[esi],eax" \    
    "mov dl,0x7F" \
    "sub dl,ch" \
    "movsx edx,dl" \ 
    "mov eax,200" \
    "imul edx" \
    "sar eax,8" \
    "mov es:[edi],eax" \    
    parm [fs esi] [es edi] \
    modify [eax cx edx];

#pragma aux RdosSetMasterVolume = \
    "mov ecx,edx" \
    "mov esi,eax" \
    "xor edx,edx" \
    "shl eax,8" \
    "sbb edx,0" \
    "mov esi,200" \
    "idiv esi" \
    "mov bl,0x7F" \
    "sub bl,al" \
    "adc bl,0" \
    "mov eax,ecx" \
    "mov esi,eax" \
    "xor edx,edx" \
    "shl eax,8" \
    "sbb edx,0" \
    "mov esi,200" \
    "idiv esi" \
    "mov bh,0x7F" \
    "sub bh,al" \
    "adc bh,0" \
    "mov ax,bx" \
    CallGate_set_master_volume \
    parm [eax] [edx] \
    modify [eax ebx ecx edx esi];

#pragma aux RdosGetLineOutVolume = \
    CallGate_get_line_out_volume \
    "mov cx,ax" \
    "mov dl,0x7F" \
    "sub dl,al" \
    "movsx edx,dl" \ 
    "mov eax,200" \
    "imul edx" \
    "sar eax,8" \
    "mov fs:[esi],eax" \    
    "mov dl,0x7F" \
    "sub dl,ch" \
    "movsx edx,dl" \ 
    "mov eax,200" \
    "imul edx" \
    "sar eax,8" \
    "mov es:[edi],eax" \    
    parm [fs esi] [es edi] \
    modify [eax cx edx];

#pragma aux RdosSetLineOutVolume = \
    "mov ecx,edx" \
    "mov esi,eax" \
    "xor edx,edx" \
    "shl eax,8" \
    "sbb edx,0" \
    "mov esi,200" \
    "idiv esi" \
    "mov bl,0x7F" \
    "sub bl,al" \
    "adc bl,0" \
    "mov eax,ecx" \
    "mov esi,eax" \
    "xor edx,edx" \
    "shl eax,8" \
    "sbb edx,0" \
    "mov esi,200" \
    "idiv esi" \
    "mov bh,0x7F" \
    "sub bh,al" \
    "adc bh,0" \
    "mov ax,bx" \
    CallGate_set_line_out_volume \
    parm [eax] [edx] \
    modify [eax ebx ecx edx esi];

#pragma aux RdosCreateAudioOutChannel = \
    "push eax" \
    "mov eax,edx" \
    "shl eax,16" \
    "xor edx,edx" \
    "mov ebx,100" \
    "div ebx" \
    "sub eax,1" \
    "adc eax,0" \
    "mov dx,ax" \
    "pop eax" \
    CallGate_create_audio_out_channel \
    ValidateHandle \
    parm [eax] [ecx] [edx] \
    modify [ebx] \
    value [ebx];

#pragma aux RdosCloseAudioOutChannel = \
    CallGate_close_audio_out_channel \
    parm [ebx];

#pragma aux RdosWriteAudio = \
    "push ds" \
    "mov ds,edx" \
    CallGate_write_audio \
    "pop ds" \
    parm [ebx] [ecx] [edx esi] [es edi];

#pragma aux RdosOpenFm = \
    CallGate_open_fm \
    ValidateHandle \
    parm [eax] \
    value [ebx];

#pragma aux RdosCloseFm = \
    CallGate_close_fm \
    parm [ebx];

#pragma aux RdosFmWait = \
    CallGate_fm_wait \
    parm [ebx] [eax];

#pragma aux RdosCreateFmInstrument = \
    CallGate_create_fm_instrument \
    ValidateHandle \
    parm [ebx] [eax] [edx] [8087] \
    value [ebx];

#pragma aux RdosFreeFmInstrument = \
    CallGate_free_fm_instrument \
    parm [ebx];

#pragma aux RdosSetFmAttack = \
    CallGate_set_fm_attack \
    parm [ebx] [eax];

#pragma aux RdosSetFmSustain = \
    CallGate_set_fm_sustain \
    parm [ebx] [eax] [edx];

#pragma aux RdosSetFmRelease = \
    CallGate_set_fm_release \
    parm [ebx] [eax] [edx];

#pragma aux RdosPlayFmNote = \
    CallGate_play_fm_note \
    parm [ebx] [8087] [eax] [edx] [ecx];

#else
 
/* 16-bit compiler */

// check carry flag, and set ax=0 if set and ax=1 if clear
#define CarryToBool 0x73 4 0x33 0xC0 0xEB 3 0xB8 1 0

// check carry flag, and set bx=0
#define ValidateHandle 0x73 2 0x33 0xDB

// check carry flag, and set ax=0 if set
#define ValidateAx 0x73 2 0x33 0xC0

// check carry flag, and set cx=0 if set
#define ValidateCx 0x73 2 0x33 0xC9

// check carry flag, and set dx=0 if set
#define ValidateDx 0x73 2 0x33 0xD2

// check carry flag, and set si=0 if set
#define ValidateSi 0x73 2 0x33 0xF6

// check carry flag, and set di=0 if set
#define ValidateDi 0x73 2 0x33 0xFF

// check disc id, set to -1 on carry
#define ValidateDisc 0x73 2 0xB0 0xFF


// removed as it is not up-to-date!!

#endif

#endif

#pragma pack( pop )

#endif

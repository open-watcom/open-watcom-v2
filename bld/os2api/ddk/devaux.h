#ifndef DEVAUX_H_INCLUDED
#define DEVAUX_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

typedef ULONG   LIN;        // 32-Bit linear address
typedef ULONG   FAR *PLIN;  // 16:16 pointer to a 32-Bit linear address

typedef struct _PAGELIST {
    ULONG   PhysAddr;
    ULONG   Size;
} PAGELIST, NEAR *NPPAGELIST, FAR *PPAGELIST;

extern ULONG  Device_Help;

/* Device helper codes */
typedef enum {
    DevHlp_SchedClock            = 0,
    DevHlp_DevDone               = 1,
    DevHlp_Yield                 = 2,
    DevHlp_TCYield               = 3,
    DevHlp_ProcBlock             = 4,
    DevHlp_ProcRun               = 5,
    DevHlp_SemRequest            = 6,
    DevHlp_SemClear              = 7,
    DevHlp_SemHandle             = 8,
    DevHlp_PushRequest           = 9,
    DevHlp_PullRequest           = 10,
    DevHlp_PullParticular        = 11,
    DevHlp_SortRequest           = 12,
    DevHlp_AllocReqPacket        = 13,
    DevHlp_FreeReqPacket         = 14,
    DevHlp_QueueInit             = 15,
    DevHlp_QueueFlush            = 16,
    DevHlp_QueueWrite            = 17,
    DevHlp_QueueRead             = 18,
    DevHlp_Lock                  = 19,
    DevHlp_Unlock                = 20,
    DevHlp_PhysToVirt            = 21,
    DevHlp_VirtToPhys            = 22,
    DevHlp_PhysToUVirt           = 23,
    DevHlp_AllocPhys             = 24,
    DevHlp_FreePhys              = 25,
    DevHlp_SetROMVector          = 26,
    DevHlp_SetIRQ                = 27,
    DevHlp_UnSetIRQ              = 28,
    DevHlp_SetTimer              = 29,
    DevHlp_ResetTimer            = 30,
    DevHlp_MonitorCreate         = 31,
    DevHlp_Register              = 32,
    DevHlp_DeRegister            = 33,
    DevHlp_MonWrite              = 34,
    DevHlp_MonFlush              = 35,
    DevHlp_GetDOSVar             = 36,
    DevHlp_SendEvent             = 37,
    DevHlp_ROMCritSection        = 38,
    DevHlp_VerifyAccess          = 39,
    DevHlp_RAS                   = 40,
    DevHlp_ABIOSGetParms         = 41,
    DevHlp_AttachDD              = 42,
    DevHlp_InternalError         = 43,
    DevHlp_ModifyPriority        = 44,
    DevHlp_AllocGDTSelector      = 45,
    DevHlp_PhysToGDTSelector     = 46,
    DevHlp_RealToProt            = 47,
    DevHlp_ProtToReal            = 48,
    DevHlp_EOI                   = 49,
    DevHlp_UnPhysToVirt          = 50,
    DevHlp_TickCount             = 51,
    DevHlp_GetLIDEntry           = 52,
    DevHlp_FreeLIDEntry          = 53,
    DevHlp_ABIOSCall             = 54,
    DevHlp_ABIOSCommonEntry      = 55,
    DevHlp_GetDeviceBlock        = 56,
    DevHlp_RegisterStackUsage    = 58,
    DevHlp_LogEntry              = 59,
    DevHlp_VideoPause            = 60,
    DevHlp_Save_Message          = 61,
    DevHlp_SegRealloc            = 62,
    DevHlp_PutWaitingQueue       = 63,
    DevHlp_GetWaitingQueue       = 64,
//  DevHlp_PhysToSys             = 65,  obsolete
//  DevHlp_PhysToSysHook         = 66,  obsolete
    DevHlp_RegisterDeviceClass   = 67,
    // 32-Bit DevHelps start at 80 (0x50)
    DevHlp_RegisterPDD           = 80,
    DevHlp_RegisterBeep          = 81,
    DevHlp_Beep                  = 82,
    DevHlp_FreeGDTSelector       = 83,
    DevHlp_PhysToGDTSel          = 84,
    DevHlp_VMLock                = 85,
    DevHlp_VMUnlock              = 86,
    DevHlp_VMAlloc               = 87,
    DevHlp_VMFree                = 88,
    DevHlp_VMProcessToGlobal     = 89,
    DevHlp_VMGlobalToProcess     = 90,
    DevHlp_VirtToLin             = 91,
    DevHlp_LinToGDTSelector      = 92,
    DevHlp_GetDescInfo           = 93,
    DevHlp_LinToPageList         = 94,
    DevHlp_PageListToLin         = 95,
    DevHlp_PageListToGDTSelector = 96,
    DevHlp_RegisterTmrDD         = 97,
    DevHlp_RegisterPerfCtrs      = 98,
    DevHlp_AllocateCtxHook       = 99,
    DevHlp_FreeCtxHook           = 100,
    DevHlp_ArmCtxHook            = 101,
    DevHlp_VMSetMem              = 102,
    DevHlp_OpenEventSem          = 103,
    DevHlp_CloseEventSem         = 104,
    DevHlp_PostEventSem          = 105,
    DevHlp_ResetEventSem         = 106,
    DevHlp_RegisterFreq          = 107,
    DevHlp_DynamicAPI            = 108,
    DevHlp_ProcRun2              = 109,
    DevHlp_CreateInt13VDM        = 110,
    DevHlp_RegisterKrnlExit      = 111,
    DevHlp_PMPostEventSem        = 112,
    // 0x70-0x7C unused
    DevHlp_KillProc              = 125,
    DevHlp_QSysState             = 126,
    DevHlp_OpenFile              = 127,
    DevHlp_CloseFile             = 128,
    DevHlp_ReadFile              = 129,
    DevHlp_ReadFileAt            = 130
} DevHlp_Code;


/* Macro to call device helper with carry flag check */
#define DEVHELP_CALL                    \
    "call dword ptr [Device_Help]"      \
    "jc   error"                        \
    "sub  ax,ax"                        \
    "error:"                            \
    value [ax]

/* Macro to call device helper with no error check */
#define DEVHELP_CALL_NOCHECK            \
    "call dword ptr [Device_Help]"      \
    "sub  ax,ax"                        \
    value [ax]



USHORT DevHelp_SchedClock( PFN NEAR *SchedRoutineAddr );
#pragma aux DevHelp_SchedClock =        \
    "mov  dl,00h"                       \
    DEVHELP_CALL_NOCHECK                \
    parm caller [ax]                    \
    modify nomemory exact [ax dl];


USHORT DevHelp_DevDone( REQP_ANY *ReqPkt );
#pragma aux DevHelp_DevDone =           \
    "mov  dl,01h"                       \
    DEVHELP_CALL                        \
    parm caller [es bx]                 \
    modify exact [ax dl];


USHORT DevHelp_Yield( void );
#pragma aux DevHelp_Yield =             \
    "mov  dl,02h"                       \
    DEVHELP_CALL_NOCHECK                \
    parm caller nomemory []             \
    modify nomemory exact [ax dl];


USHORT DevHelp_TCYield( void );
#pragma aux DevHelp_TCYield =           \
    "mov  dl,03h"                       \
    DEVHELP_CALL_NOCHECK                \
    parm caller nomemory []             \
    modify nomemory exact [ax dl];


/* DevHelp_ProcBlock wait flags */
typedef enum {
    WAIT_IS_INTERRUPTABLE = 0,
    WAIT_IS_NOT_INTERRUPTABLE
} DHProcBlock_flags;

/* DevHelp_ProcBlock return codes */
typedef enum {
    WAIT_TIMED_OUT   = 0x8001,
    WAIT_INTERRUPTED = 0x8003
} DHProcBlock_retcodes;

USHORT DevHelp_ProcBlock( ULONG EventId, ULONG WaitTime, USHORT IntWaitFlag );
#pragma aux DevHelp_ProcBlock =         \
    "xchg ax,bx"                        \
    "xchg cx,di"                        \
    "mov  dl,04h"                       \
    "call dword ptr [Device_Help]"      \
    "jc   error"                        \
    "mov  ax,0"                         \
    "error:"                            \
    value [ax]                          \
    parm caller nomemory [ax bx] [di cx] [dh] \
    modify nomemory exact [ax bx cx dl di];


USHORT DevHelp_ProcRun( ULONG EventId, PUSHORT AwakeCount );
#pragma aux DevHelp_ProcRun =           \
    "xchg ax,bx"                        \
    "mov  dl,05h"                       \
    "call dword ptr [Device_Help]"      \
    "mov  es:[si],ax"                   \
    "sub  ax,ax"                        \
    value [ax]                          \
    parm caller nomemory [ax bx] [es si] \
    modify exact [ax bx dl];


USHORT DevHelp_SemRequest( ULONG SemHandle, ULONG SemTimeout );
#pragma aux DevHelp_SemRequest =        \
    "xchg ax,bx"                        \
    "xchg di,cx"                        \
    "mov  dl,06h"                       \
    DEVHELP_CALL                        \
    parm nomemory [ax bx] [cx di]       \
    modify nomemory exact [ax bx cx di dl];


USHORT DevHelp_SemClear( ULONG SemHandle );
#pragma aux DevHelp_SemClear =          \
    "xchg ax,bx"                        \
    "mov  dl,07h"                       \
    DEVHELP_CALL                        \
    parm nomemory [ax bx]               \
    modify nomemory exact [ax bx dl];


/* DevHelp_SemHandle semaphore use flags */
typedef enum {
    SEMUSEFLAG_IN_USE     = 0,
    SEMUSEFLAG_NOT_IN_USE
} DHProcBlock_flags;

USHORT DevHelp_SemHandle( ULONG SemKey, USHORT SemUseFlag, PULONG SemHandle );
#pragma aux DevHelp_SemHandle =         \
    "xchg ax,bx"                        \
    "mov  dl,08h"                       \
    "call dword ptr [Device_Help]"      \
    "jc   error"                        \
    "mov  es:[si],bx"                   \
    "mov  es:[si+2],ax"                 \
    "sub  ax,ax"                        \
    "error:"                            \
    value [ax]                          \
    parm nomemory [ax bx] [dh] [es si]  \
    modify exact [ax bx dl];


USHORT DevHelp_PushRequest( NPBYTE Queue, PBYTE ReqPktddr );
#pragma aux DevHelp_PushRequest =       \
    "mov  dl,09h"                       \
    DEVHELP_CALL_NOCHECK                \
    parm [si] [es bx]                   \
    modify exact [ax dl];


USHORT DevHelp_PullRequest( NPBYTE Queue, PBYTE FAR *ReqPktAddr );
#pragma aux DevHelp_PullRequest =       \
    "push es"                           \
    "mov  dl,0Ah"                       \
    "call dword ptr [Device_Help]"      \
    "jc   error"                        \
    "push es"                           \
    "push bx"                           \
    "mov  bx,sp"                        \
    "les  bx,[esp]"                     \
    "pop  es:[bx]"                      \
    "pop  es:[bx+2]"                    \
    "sub  ax,ax"                        \
    "error:"                            \
    value [ax]                          \
    parm [si] []                        \
    modify exact [ax bx dl es];


USHORT DevHelp_PullParticular( NPBYTE Queue, PBYTE ReqPktAddr );
#pragma aux DevHelp_PullParticular =    \
    "mov  dl,0Bh"                       \
    DEVHELP_CALL                        \
    parm [si] [es bx]                   \
    modify exact [ax dl];


USHORT DevHelp_SortRequest( NPBYTE Queue, PBYTE ReqPktAddr );
#pragma aux DevHelp_SortRequest =       \
    "mov  dl,0Ch"                       \
    DEVHELP_CALL_NOCHECK                \
    parm [si] [es bx]                   \
    modify exact [ax dl];


/* DevHelp_AllocReqPacket wait flags */
typedef enum {
    WAIT_NOT_ALLOWED = 0,
    WAIT_IS_ALLOWED
} DHAllocReqPacket_flags;

USHORT DevHelp_AllocReqPacket( USHORT WaitFlag, PBYTE FAR *ReqPktAddr );
#pragma aux DevHelp_AllocReqPacket =    \
    "mov  dl,0Dh",                      \
    "call dword ptr [Device_Help]"      \
    "jc   error"                        \
    "push es"                           \
    "push bx"                           \
    "mov  bx,sp"                        \
    "les  bx,ss:[bx]"                   \
    "pop  es:[bx]"                      \
    "pop  es:[bx+2]"                    \
    "error:"                            \
    "mov  ax,0"                         \
    "sbb  ax,0"                         \
    value [ax]                          \
    parm caller [dh] []                 \
    modify exact [ax bx dl es];


USHORT DevHelp_FreeReqPacket( PBYTE ReqPktAddr );
#pragma aux DevHelp_FreeReqPacket =     \
    "mov  dl,0Eh",                      \
    DEVHELP_CALL_NOCHECK                \
    parm caller [es bx]                 \
    modify exact [ax dl];


/* Queue header structure */
typedef struct _QUEUEHDR {
    USHORT  QSize;
    USHORT  QChrOut;
    USHORT  QCount;
    BYTE    Queue[1];
} QUEUEHDR, FAR *PQUEUEHDR, NEAR *NPQUEUEHDR;

USHORT DevHelp_QueueInit( NPQUEUEHDR Queue );
#pragma aux DevHelp_QueueInit =         \
   "mov  dl,0Fh"                        \
    DEVHELP_CALL_NOCHECK                \
    parm [bx]                           \
    modify exact [ax dl];


USHORT DevHelp_QueueFlush( NPQUEUEHDR Queue );
#pragma aux DevHelp_QueueFlush =        \
    "mov  dl,10h"                       \
    DEVHELP_CALL_NOCHECK                \
    parm [bx]                           \
    modify exact [ax dl];


USHORT DevHelp_QueueWrite( NPQUEUEHDR Queue, UCHAR Char );
#pragma aux DevHelp_QueueWrite =        \
    "mov  dl,11h"                       \
    DEVHELP_CALL                        \
    parm [bx] [al]                      \
    modify exact [ax dl];


USHORT DevHelp_QueueRead( NPQUEUEHDR Queue, PBYTE Char );
#pragma aux DevHelp_QueueRead =         \
    "mov  dl,12h"                       \
    "call dword ptr [Device_Help]"      \
    "jc   error"                        \
    "mov  es:[di],al"                   \
    "sub  ax,ax"                        \
    "error:"                            \
    value [ax]                          \
    parm [bx] [es di]                   \
    modify exact [ax dl];


/* DevHelp_Lock flags */
typedef enum {
    LOCKTYPE_SHORT_ANYMEM = 0,
    LOCKTYPE_LONG_ANYMEM  = 1,
    LOCKTYPE_LONG_HIGHMEM = 3,
    LOCKTYPE_SHORT_VERIFY = 4
} DHLock_flags;

USHORT DevHelp_Lock( SEL Segment, USHORT LockType, USHORT WaitFlag, PULONG LockHandle );
#pragma aux DevHelp_Lock =              \
    "mov  dl,13h"                       \
    "call dword ptr [Device_Help]"      \
    "jc   error"                        \
    "mov  es:[di],bx"                   \
    "mov  es:[di+2],ax"                 \
    "sub  ax,ax"                        \
    "error:"                            \
    value [ax]                          \
    parm caller [ax] [bh] [bl] [es di]  \
    modify exact [ax dl];


USHORT DevHelp_UnLock( ULONG LockHandle );
#pragma aux DevHelp_UnLock =            \
    "xchg ax,bx"                        \
    "mov  dl,14h"                       \
    DEVHELP_CALL                        \
    parm nomemory [ax bx]               \
    modify nomemory exact [ax bx dl];


USHORT DevHelp_PhysToVirt( ULONG PhysAddr, USHORT usLength, PVOID SelOffset, PUSHORT ModeFlag );
#pragma aux DevHelp_PhysToVirt =        \
    "xchg ax,bx"                        \
    "mov  dx,15h"                       \
    "push ds"                           \
    "call dword ptr [Device_Help]"      \
    "jc   error"                        \
    "sub  ax,ax"                        \
    "mov  es:[di],si"                   \
    "mov  es:[di+2],ds"                 \
    "error:"                            \
    "pop  ds"                           \
    value [ax]                          \
    parm caller nomemory [bx ax] [cx] [es di] [] \
    modify exact [ax bx dx si];


USHORT DevHelp_VirtToPhys( PVOID SelOffset, PPHYSADDR PhysAddr );
#pragma aux DevHelp_VirtToPhys =        \
    "push ds"                           \
    "mov  dl,16h"                       \
    "push es"                           \
    "mov  si,ds"                        \
    "mov  es,si"                        \
    "mov  ds,bx"                        \
    "mov  si,ax"                        \
    "call dword ptr es:[Device_Help]"   \
    "pop  es"                           \
    "mov  es:[di+0],bx"                 \
    "mov  es:[di+2],ax"                 \
    "pop  ds"                           \
    "sub  ax,ax"                        \
    value [ax]                          \
    parm caller nomemory [ax bx] [es di] \
    modify exact [ax bx dl si];


/* DevHelp_PhysToUVirt request types */
typedef enum {
    SELTYPE_R3CODE = 0,
    SELTYPE_R3DATA,
    SELTYPE_FREE,
    SELTYPE_R2CODE,
    SELTYPE_R2DATA,
    SELTYPE_R3VIDEO
} DHPhysToUVirt_flags;

USHORT DevHelp_PhysToUVirt( ULONG PhysAddr, USHORT Length, USHORT flags, USHORT TagType, PVOID SelOffset );
#pragma aux DevHelp_PhysToUVirt =       \
    "xchg ax,bx"                        \
    "mov  dl,17h"                       \
    "call dword ptr [Device_Help]"      \
    "jc   error"                        \
    "push es"                           \
    "push bx"                           \
    "mov  bx,sp"                        \
    "les  bx,ss:[bx+4]"                 \
    "pop  es:[bx+0]"                    \
    "pop  es:[bx+2]"                    \
    "sub  ax,ax"                        \
    "error:"                            \
    value [ax]                          \
    parm caller nomemory [bx ax] [cx] [dh] [si] [] \
    modify exact [ax bx dl es];


/* DevHelp_AllocPhys allocation flags */
typedef enum {
    MEMTYPE_ABOVE_1M = 0,
    MEMTYPE_BELOW_1M
} DHAllocPhys_flags;

USHORT DevHelp_AllocPhys( ULONG lSize, USHORT MemType, PULONG PhysAddr );
#pragma aux DevHelp_AllocPhys =         \
    "xchg ax,bx"                        \
    "mov  dl,18h"                       \
    "call dword ptr [Device_Help]"      \
    "jc   error"                        \
    "mov  es:[di+0],bx"                 \
    "mov  es:[di+2],ax"                 \
    "sub  ax,ax"                        \
    "error:"                            \
    value [ax]                          \
    parm caller nomemory [ax bx] [dh] [es di] \
    modify exact [ax bx dx];


USHORT DevHelp_FreePhys( ULONG PhysAddr );
#pragma aux DevHelp_FreePhys =          \
    "xchg ax,bx"                        \
    "mov  dl,19h"                       \
    "call dword ptr [Device_Help]"      \
    "mov  ax,0"                         \
    "sbb  ax,0"                         \
    value [ax]                          \
    parm caller nomemory [ax bx]        \
    modify exact [ax bx dl];


USHORT DevHelp_SetIRQ( NPFN IRQHandler, USHORT IRQLevel, USHORT SharedFlag );
#pragma aux DevHelp_SetIRQ =            \
    "mov  dl,1Bh"                       \
    DEVHELP_CALL                        \
    parm caller nomemory [ax] [bx] [dh] \
    modify nomemory exact [ax dl];


USHORT DevHelp_UnSetIRQ( USHORT IRQLevel );
#pragma aux DevHelp_UnSetIRQ =          \
    "mov  dl,1Ch"                       \
    DEVHELP_CALL                        \
    parm caller nomemory [bx]           \
    modify nomemory exact [ax dl];


USHORT DevHelp_SetTimer( NPFN TimerHandler );
#pragma aux DevHelp_SetTimer =          \
    "mov  dl,1Dh"                       \
    DEVHELP_CALL                        \
    parm caller nomemory [ax]           \
    modify nomemory exact [ax dl];


USHORT DevHelp_ResetTimer( NPFN TimerHandler );
#pragma aux DevHelp_ResetTimer =        \
    "mov  dl,1Eh"                       \
    DEVHELP_CALL                        \
    parm caller nomemory [ax]           \
    modify nomemory exact [ax dl];




/* DevHelp_GetDOSVar identifiers */
typedef enum {
    DHGETDOSV_SYSINFOSEG        = 1,
    DHGETDOSV_LOCINFOSEG        = 2,
    DHGETDOSV_VECTORSDF         = 4,
    DHGETDOSV_VECTORREBOOT      = 5,
    DHGETDOSV_VECTORMSATS       = 6,
    DHGETDOSV_YIELDFLAG         = 7,
    DHGETDOSV_TCYIELDFLAG       = 8,
    DHGETDOSV_DOSCODEPAGE       = 11,
    DHGETDOSV_INTERRUPTLEV      = 13,
    DHGETDOSV_DEVICECLASSTABLE  = 14,
    DHGETDOSV_DMQSSEL           = 15,
    DHGETDOSV_APMINFO           = 16
} DHGetDOSVar_Index;

USHORT DevHelp_GetDOSVar( DHGetDOSVar_Index VarNumber, USHORT VarMember, PPVOID KernelVar );
#pragma aux DevHelp_GetDOSVar =         \
    "mov  dl,24h"                       \
    "call dword ptr [Device_Help]"      \
    "mov  es:[di+0],bx"                 \
    "mov  es:[di+2],ax"                 \
    "sub  ax,ax"                        \
    value [ax]                          \
    parm caller nomemory [al] [cx] [es di] \
    modify exact [ax bx dl];


/* DevHelp_SendEvent event codes */
typedef enum {
    EVENT_MOUSEHOTKEY = 0,
    EVENT_CTRLBREAK,
    EVENT_CTRLC,
    EVENT_CTRLNUMLOCK,
    EVENT_CTRLPRTSC,
    EVENT_SHIFTPRTSC,
    EVENT_KBDHOTKEY,
    EVENT_KBDREBOOT
} DHSendEvent_code;

USHORT DevHelp_SendEvent( USHORT EventType, USHORT Parm );
#pragma aux DevHelp_SendEvent =         \
    "mov  dl,25h"                       \
    "call dword ptr [Device_Help]"      \
    "mov  ax,0"                         \
    "sbb  ax,0"                         \
    value [ax]                          \
    parm nomemory [ah] [bx]             \
    modify nomemory exact [ax dl];


/* DevHelp_VerifyAccess access types */
typedef enum {
    VERIFY_READONLY = 0,
    VERIFY_READWRITE
} DHVerifyAccess_type;

USHORT DevHelp_VerifyAccess( SEL MemSelector, USHORT Length, USHORT MemOffset, UCHAR AccessFlag );
#pragma aux DevHelp_VerifyAccess =      \
    "mov  dl,27h"                       \
    DEVHELP_CALL                        \
    parm caller nomemory [ax] [cx] [di] [dh] \
    modify nomemory exact [ax dl];

/* Modified version of VerifyAccess that takes a far pointer instead of selector/offset */
USHORT DevHelp_VerifyAccessPtr( PVOID Pointer, USHORT Length, UCHAR AccessFlag );
#pragma aux DevHelp_VerifyAccessPtr =   \
    "xchg ax,di"                        \
    "mov  dl,27h"                       \
    DEVHELP_CALL                        \
    parm caller nomemory [ax di] [cx] [dh] \
    modify nomemory exact [ax dl di];



typedef struct _IDCTABLE {
    USHORT  RealOffset;     /* obsolete */
    USHORT  RealCS;         /* obsolete */
    USHORT  RealDS;         /* obsolete */
    VOID    (FAR *ProtIDCEntry)( VOID );
    USHORT  ProtDS;
} IDCTABLE, NEAR *NPIDCTABLE;

USHORT DevHelp_AttachDD( NPSZ DDName, NPIDCTABLE DDTable );
#pragma aux DevHelp_AttachDD =          \
    "mov  dl,2Ah"                       \
    "call dword ptr [Device_Help]"      \
    "mov  ax,0"                         \
    "sbb  ax,0"                         \
    value [ax]                          \
    parm caller [bx] [di]               \
    modify exact [ax dl];


USHORT DevHelp_InternalError( PSZ MsgText, USHORT MsgLength );
#pragma aux DevHelp_InternalError =     \
    "push ds"                           \
    "push es"                           \
    "pop  ds"                           \
    "pop  es"                           \
    "mov  dl,2Bh"                       \
    "jmp dword ptr es:[Device_Help]"    \
    parm [es si] [di]                   \
    modify nomemory exact []            \
    aborts;


USHORT DevHelp_AllocGDTSelector( PSEL Selectors, USHORT Count );
#pragma aux DevHelp_AllocGDTSelector =  \
    "mov  dl,2Dh"                       \
    DEVHELP_CALL                        \
    parm caller [es di] [cx]            \
    modify nomemory exact [ax dl];




USHORT DevHelp_FreeGDTSelector( SEL Selector );
#pragma aux DevHelp_FreeGDTSelector =   \
    "mov  dl,53h"                       \
    DEVHELP_CALL                        \
    parm caller nomemory [ax]           \
    modify nomemory exact [ax dl];


/* GDT selector mapping types for PhysToGDTSel and PageListToGDTSelector */
typedef enum {
    GDTSEL_R3CODE = 0,
    GDTSEL_R3DATA = 1,
    GDTSEL_R2CODE = 3,
    GDTSEL_R2DATA = 4,
    GDTSEL_R0CODE = 5,
    GDTSEL_R0DATA = 6,
    GDTSEL_ADDR32 = 128
} DHGDT_mapType;

// TODO: use pops instead of moves from stack
USHORT DevHelp_PhysToGDTSel( ULONG PhysAddr, ULONG Count, SEL Selector, UCHAR Access );
#pragma aux DevHelp_PhysToGDTSel =      \
    "push bp"                           \
    "mov  dl,54h"                       \
    "mov  bp,sp"                        \
    "mov  eax,[bp+2]"                   \
    "mov  ecx,[bp+6]"                   \
    "mov  si,[bp+10]"                   \
    "mov  dh,[bp+12]"                   \
    "call dword ptr [Device_Help]"      \
    "jc   error"                        \
    "sub  ax,ax"                        \
    "error:"                            \
    "pop  bp"                           \
    value [ax]                          \
    parm caller nomemory []             \
    modify nomemory exact [ax cx dx si];




USHORT DevHelp_VirtToLin( SEL Selector, ULONG Offset, PLIN LinearAddr );
#pragma aux DevHelp_VirtToLin =         \
    "mov  esi,[esp]"                    \
    "mov  dl,5Bh"                       \
    "call dword ptr [Device_Help]"      \
    "jc   error"                        \
    "les  bx,[esp+4]"                   \
    "mov  es:[bx],eax"                  \
    "sub  ax,ax"                        \
    "error:"                            \
    value [ax]                          \
    parm caller nomemory [ax] []        \
    modify exact [ax si es bx dl];

/* Modified version of VirtToLin that takes a far pointer instead of selector/offset */
USHORT DevHelp_VirtToLinPtr( PVOID Pointer, PLIN LinearAddr );
#pragma aux DevHelp_VirtToLinPtr =      \
    "xchg ax,si"                        \
    "movzx esi,si"                      \
    "mov  dl,5Bh"                       \
    "call dword ptr [Device_Help]"      \
    "jc   error"                        \
    "pop  bx"                           \
    "pop  es"                           \
    "mov  es:[bx],eax"                  \
    "sub  ax,ax"                        \
    "error:"                            \
    value [ax]                          \
    parm routine nomemory [ax si] []    \
    modify exact [ax si es bx dl];




USHORT DevHelp_OpenEventSem( ULONG hEvent );
#pragma aux DevHelp_OpenEventSem =      \
    "pop  eax"                          \
    "mov  dl,67h"                       \
    DEVHELP_CALL                        \
    parm routine nomemory []            \
    modify nomemory exact [ax dl];


USHORT DevHelp_CloseEventSem( ULONG hEvent );
#pragma aux DevHelp_CloseEventSem =     \
    "pop  eax"                          \
    "mov  dl,68h"                       \
    DEVHELP_CALL                        \
    parm routine nomemory []            \
    modify nomemory exact [ax dl];


USHORT DevHelp_PostEventSem( ULONG hEvent );
#pragma aux DevHelp_PostEventSem =      \
    "pop  eax"                          \
    "mov  dl,69h"                       \
    DEVHELP_CALL                        \
    parm routine nomemory []            \
    modify nomemory exact [ax dl];


USHORT DevHelp_ResetEventSem( ULONG hEvent, LIN pNumPosts );
#pragma aux DevHelp_ResetEventSem =     \
    "pop  eax"                          \
    "pop  edi"                          \
    "mov  dl,6Ah"                       \
    DEVHELP_CALL                        \
    parm routine nomemory []            \
    modify exact [ax dl di];

#ifdef __cplusplus
extern "C" {
#endif

#endif

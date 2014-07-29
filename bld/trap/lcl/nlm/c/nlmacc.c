/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  Low-level Netware implementation code for the debugger trap.
*
****************************************************************************/


#include "debugme.h"
#include <string.h>     /* may only call functions defined in nlmstrt.c */
#include <fcntl.h>
#include <stdio.h>
#include <nwtypes.h>

#include "miniproc.h"

#include "loader.h"
//#include "nlmheadr.h"
#if !defined ( _USE_NEW_KERNEL )
#include "nwsemaph.h"
#endif
#include "debugapi.h"

#undef TRUE
#undef FALSE

#include "cpuglob.h"
#include "dbg386.h"
#include "trpimp.h"
#include "trperr.h"
#include "packet.h"
#include "exenov.h"
#include "madregs.h"

#include "nlmport.h"
#include "nw3to4.h"

#include "x86cpu.h"
#include "miscx87.h"

#define NO_DREG     ((byte)-1)
#define NUM_DREG    4

#define FLG_T           0x0100UL

typedef LONG pPID;

#if defined ( _USE_NEW_KERNEL )
void *          kSemaphoreAlloc( const char *, long );
unsigned long   kSemaphoreExamineCount( void *sp );
int             kSemaphoreFree( void *sp );
int             kSemaphoreWait( void *sp );
int             kSemaphoreTimedWait( void *sp, unsigned long ms);
int             kSemaphoreSignal( void *sp );
int             kSemaphoreTry( void *sp );
int             kSemaphoreValidate( void *sp );
int             kGetSemaphoreInfo( void *sp, char *name, size_t nameMax,
                    unsigned long *count, int *waitingThreads, void *owner );

// R.E.
int             kSemaphoreSignalAll( void *sp );

#endif

/* Forward declarations */
static void ClearDebugRegs( void );
static void ActivateDebugRegs( void );

struct      {
    dword   address;
    byte    type;
    byte    size;
} DR[ NUM_DREG ];

typedef struct msb {
        struct msb                      *next;
        struct LoadDefinitionStructure  *load;
        pPID                             os_id;
        dword                           dbg_id;
        dword                           xnum;
        dword                           errnum;
        trap_cpu_regs                   cpu;
        trap_fpu_regs                   fpu;
#if defined ( _USE_NEW_KERNEL )
        void *                          ksem;
        void *                          tksem;
#else
        dword                           sem;
#endif
        char                            *description;
        unsigned                        asleep:1;/* sleeping on our semaphore */
        unsigned                        frozen:1;/* allowed to execute? */
        unsigned                        in_start_proc:1;/* our helper thread! */
        unsigned                        clib_created:1;/* created by CLIB*/
        unsigned                        to_be_killed:1;/* condemned */
        int                             exception_number;
} msb;

typedef enum {
        NLM_NONE,
        NLM_PRELOADING,
        NLM_LOADING,
        NLM_IN_START_PROC,
        NLM_FORCED_INIT_FAILURE,
        NLM_LOADED,
} nlmstate;

typedef struct nlm_entry {
        struct nlm_entry                *next;
        struct LoadDefinitionStructure  ld;
        bool                            is_load;
} nlm_entry;

static unsigned_8                       RealNPXType;

nlmstate                                NLMState;
#define BUFF_SIZE                       256
char                                    CmdLine[BUFF_SIZE];
char                                    NLMName[14];
char                                    UtilBuff[BUFF_SIZE];
msb                                     *MSB;
msb                                     *MSBHead;
trap_cpu_regs                           HelperThreadRegs;
dword                                   ThreadId;
#if defined ( _USE_NEW_KERNEL )
void *                                  kDebugSem = NULL;
void *                                  kHelperSem = NULL;
#else
dword                                   DebugSem = 0;
dword                                   HelperSem = 0;
#endif
struct LoadDefinitionStructure         *DebuggerLoadedNLM = NULL;
nlm_entry                               *NLMList = NULL;
nlm_entry                               *LastNLMListEntry = NULL;
bool                                    FakeBreak;
bool                                    DebuggerRunning;
bool                                    ExpectingEvent;
bool                                    TrapInt1;
opcode_type                             saved_opcode;
dword                                   DebugProcess;
byte                                    DebugPriority;
byte                                    RunningPriority;

/* from MYPRELUD.C */
extern struct ResourceTagStructure      *AllocTag;
extern struct ResourceTagStructure      *SemaphoreTag;
extern struct ResourceTagStructure      *ProcessTag;
extern void                             ServMessage(char*);


/* from NLMIO.C */
extern int IOCreat( char *name );
extern int IOOpen( char *openname, int openmode );
extern int IOClose( int closehandle );
extern int IOWrite( int writehandle, char *buff, int buff_len );
extern int IOWriteConsole( char *buff, int buff_len );
extern int IOSeek( int seekhandle, int seekmode, long seekpos );
extern int IORead( int readhandle, char *buff, int len );
extern void StringToNLMPath( char *name, char *res );

/* from NLMINTR.ASM */
extern void                     DoALongJumpTo( unsigned_32, unsigned_32, unsigned_32, unsigned_32, unsigned_32, unsigned_32, unsigned_32, unsigned_32, unsigned_32, unsigned_32 );
dword                           ReturnESP;
extern void                     Return( void );
extern int                      AdjustStack( dword old_esp, dword adjust );

/* from NLMCLIB.C */

extern struct LoadDefinitionStructure *GetNLMFromPID(pPID);
extern void                     BoobyTrapPID(pPID);
extern void                     UnBoobyTrapPID(pPID);
extern char                     *GetPIDName(pPID);
extern int                      ValidatePID(pPID);
extern void                     SetupPIDForACleanExit(pPID);
#if defined ( __NW40__ )
extern int                      ImportCLIBSymbols( void );
#endif

/* Must be CLIB! */
void  _exit( int __status );
#if defined ( __NW50__ )
void  NXVmExit( int status ) ;

int   get_app_type       ( void );

/* return flags for get_app_type()... */
#define LIBRARY_UNKNOWN      0x01  /* thread has default library context     */
#define LIBRARY_LIBC         0x02  /* thread has specific NKS/LibC context   */
#define LIBRARY_CLIB         0x04  /* thread has CLib context                */
#define LIBRARY_JAVA         0x08  /* thread belongs to Java Virtual Machine */

#endif


/* from SERVNAME.C */
extern char ServPref[];

typedef struct watch {
    addr48_ptr  addr;
    dword       value;
    dword       linear;
    short       dregs;
    short       len;
} watch;

#define MAX_WP  32
watch   WatchPoints[ MAX_WP ];
int     WatchCount;

/*
//  Code to release all waiters on a semaphore and delete it
*/
#if defined ( _USE_NEW_KERNEL )
int msb_KernelSemaphoreReleaseAll( msb * m )
{
    void *      ksem = m->ksem;
    m->ksem = NULL;
    m->tksem = ksem;

    return( kSemaphoreSignalAll( ksem ) );
}

int KernelSemaphoreReleaseAll( void * sp )
{
    kSemaphoreSignalAll( sp );
    return ( kSemaphoreFree( sp ) );
}

#endif

void NewNLMListEntry( struct LoadDefinitionStructure *ld )
{
    nlm_entry   *new;

    new = Alloc( sizeof( *new ), AllocTag );
    new->next = NLMList;
    new->ld = *ld;
    new->is_load = TRUE;
    NLMList = new;
//  _DBG_EVENT(( "NewNLMListEntry: %8x %s\r\n", ld, &(ld->LDFileName[1]) ));
}

void DeadNLMListEntry( struct LoadDefinitionStructure *ld )
{
    nlm_entry   *curr;

    for( curr = NLMList; curr != NULL; curr = curr->next ) {
        if( !curr->is_load )
            continue;
        if( curr->ld.LDCodeImageOffset != ld->LDCodeImageOffset )
            continue;
        curr->is_load = FALSE;
//      _DBG_EVENT(( "DeadNLMListEntry: %8x %s\r\n", ld, &(ld->LDFileName[1]) ));
        break;
    }
}

void FreeAnNLMListEntry( void )
{
    nlm_entry   *junk;

    junk = NLMList;
    NLMList = NLMList->next;
    Free( junk );
}

msb *LocateThread( pPID pid )
{
    msb         *m;

    m = MSBHead;
    _DBG_THREAD(( "Looking for  MSB for thread=%8x -- \r\n", pid ));
    while( m != NULL ) {
        if( m->os_id == pid ) {
            _DBG_THREAD(( "Found MSB=%8x TID=%d\r\n", m, m->dbg_id ));
            return( m );
        }
        m = m->next;
    }
    m = Alloc( sizeof( msb ), AllocTag );

    if( NULL == m ){
        _DBG_ERROR(( "LocateThread: Failed to allocate msb block\r\n" ));
        return NULL;
    }

    m->description = NULL;
    m->load = NULL;
    m->os_id = pid;
    m->dbg_id = ++ThreadId;
    m->asleep = FALSE;
    m->frozen = FALSE;
    m->in_start_proc = FALSE;
    m->clib_created = FALSE;
    m->to_be_killed = FALSE;
#if defined ( _USE_NEW_KERNEL )
    m->ksem = NULL;
    m->tksem = NULL;
#else
    m->sem = 0;
#endif
    m->next = MSBHead;
    //_DBG_THREAD(( "Allocating MSB=%8x TID=%d\r\n", m, m->dbg_id ));
    MSBHead = m;
    return( m );
}

void FreeThread( msb *m )
{
    msb **owner;
    msb *curr;

    _DBG_THREAD(( "Trying to free thread MSB=%8x -- \r\n", m ));
    owner = &MSBHead;
    for( ;; ) {
        curr = *owner;
        if( curr == NULL )
            return;
        if( curr == m ) {
            *owner = curr->next;
            _DBG_THREAD(( "Freed it\r\n" ));
#if defined ( _USE_NEW_KERNEL )
            if( m->ksem){
                kSemaphoreFree( m->ksem );
                m->ksem = NULL;
            }
#else
            if( m->sem )
                CDeAllocateSemaphore( m->sem );
#endif
            Free( m );
            _DBG_THREAD(( "\r\n" ));
            return;
        }
        owner = &(curr->next);
    }
}


void FreeInvalidThreads( void )
{
    msb **owner;
    msb *curr;

    owner = &MSBHead;
    for( ;; ) {
        curr = *owner;
        if( curr == NULL )
            return;
        if( !ValidatePID( curr->os_id ) ) {
            *owner = curr->next;    /* remove MSB from chain */
#if defined ( _USE_NEW_KERNEL )
            if( curr->ksem ){
                kSemaphoreFree( curr->ksem );
                curr->ksem = NULL;
            }
#else
            if( curr->sem )
                CDeAllocateSemaphore( curr->sem );
#endif
            //owner = &(curr->next);
            owner = &MSBHead;       /* start from the begiining again */
            Free( curr );           /* free MSB */
        } else {
            owner = &(curr->next);
        }
    }
}


void Suicide( void )
{
    CDestroyProcess( CGetMyProcessID() );
}


void TheBigSleep( void )
{
    while( TRUE ) {
        CSleepUntilInterrupt();
    }
}


void WakeDebugger( void )
{
    if( !DebuggerRunning ) {
        ClearDebugRegs();
        DebuggerRunning = TRUE;
#if defined ( _USE_NEW_KERNEL )
        kSemaphoreSignal( kDebugSem );
#else
        CVSemaphore( DebugSem );
#endif
    }
}


void SleepDebugger( void )
{
    DebuggerRunning = FALSE;
#if defined ( _USE_NEW_KERNEL )
    kSemaphoreWait( kDebugSem );
#else
    CPSemaphore( DebugSem );
#endif
    DebuggerRunning = TRUE;
}


#if 0
bool IsInOSCode( LONG eip )
{
    LoadedListHandle loadhandle;
    struct LoadDefinitionStructure *loaded;

    for( loadhandle = GetNextLoadedListEntry( 0 );
         loadhandle != 0; loadhandle = GetNextLoadedListEntry( loadhandle ) ) {
        loaded = ValidateModuleHandle( loadhandle );
        if( eip >= loaded->LDCodeImageOffset
         && eip <  loaded->LDCodeImageOffset + loaded->LDCodeImageLength ) {
            return( FALSE );
        }
    }
    if( DebuggerLoadedNLM ) { /* might not be on the LoadedList yet */
        if( eip >= DebuggerLoadedNLM->LDCodeImageOffset
         && eip <  DebuggerLoadedNLM->LDCodeImageOffset
                 + DebuggerLoadedNLM->LDCodeImageLength ) {
            return( FALSE );
        }
    }
    return( TRUE );
}


bool CheckIfBreakOKInOS( LONG eip )
{
    if( !IsInOSCode( eip ) )
        return( TRUE );
    ServMessage( TRP_NLM_cant_debug_os );
    return( FALSE );
}
#endif


unsigned_8 NPX( void )
{
    if( HAVE_EMU == 0 )
        return( RealNPXType );
#if defined ( __NW50__ ) || defined ( __NW40__ )
    return( X86_EMU );
#elif defined ( __NW30__ )
    {
        struct ExternalPublicDefinitionStructure    *epd;

        #define EMU_NAME        "__init_387_emulator"
        #define EMU_LEN         ( sizeof( EMU_NAME ) - 1 )

        for( epd = ExternalPublicList; epd != NULL; epd = epd->EPDLink ) {
            if( epd->EPDName[0] == EMU_LEN ) {
                if( memcmp( EMU_NAME, &epd->EPDName[1], EMU_LEN ) == 0 ) {
                    return( X86_EMU );
                }
            }
        }
        return( RealNPXType );
    }
#endif
}


#define SaveRegs( reg_struct ) \
    reg_struct.EAX = FieldEAX( frame ); \
    reg_struct.EBX = FieldEBX( frame ); \
    reg_struct.ECX = FieldECX( frame ); \
    reg_struct.EDX = FieldEDX( frame ); \
    reg_struct.ESI = FieldESI( frame ); \
    reg_struct.EDI = FieldEDI( frame ); \
    reg_struct.EBP = FieldEBP( frame ); \
    reg_struct.ESP = FieldESP( frame ); \
    reg_struct.EIP = FieldEIP( frame ); \
    reg_struct.EFL = FieldEFLAGS( frame ); \
    reg_struct.CS  = FieldCS( frame ); \
    reg_struct.DS  = FieldDS( frame ); \
    reg_struct.ES  = FieldES( frame ); \
    reg_struct.SS  = FieldDS( frame ); \
    reg_struct.FS  = FieldFS( frame ); \
    reg_struct.GS  = FieldGS( frame );


#ifdef DEBUG_ME
void DumpRegs( msb *m, char *str )
{
    if( DebugClasses & (D_THREAD|D_ERROR) ) {
        ConsolePrintf( "EAX:%8.8x EBX:%8.8x ECX:%8.8x EDX:%8.8x ESI:%8.8x ESI:%8.8x\r\n",
                   m->cpu.EAX, m->cpu.EBX, m->cpu.ECX, m->cpu.EDX,
                   m->cpu.ESI, m->cpu.EDI );
        ConsolePrintf( "ESP:%8.8x EBP:%8.8x EIP:%8.8x EFL:%8.8x %s=%8.8x\r\n",
                   m->cpu.ESP, m->cpu.EBP, m->cpu.EIP, m->cpu.EFL, str, m );
    }
}
#else
#define DumpRegs( m, s )
#endif

static void BigKludge( msb *m );
#pragma aux BigKludge parm [ esi ];

static LONG DebugEntry( StackFrame *frame )
{
    msb         *m;
    struct LoadDefinitionStructure *load;
    int         exception_number;
    BYTE        *description;
    long        error_code;

    exception_number = FieldExceptionNumber( frame );
    description = FieldExceptionDescription( frame );
    error_code = FieldErrorCode( frame );
    _DBG_EVENT(( "*DebugEntry: Event %d (%s): RunningProcess=%8.8x\r\n",
                exception_number, description, RunningProcess ));
    if( !ExpectingEvent ) {
        _DBG_EVENT(( "  Not expecting event\r\n" ));
        return( RETURN_TO_NEXT_DEBUGGER );
    }
    if( DebuggerRunning &&
        (  ( exception_number == KEYBOARD_BREAK_EVENT )
        || ( exception_number == ENTER_DEBUGGER_EVENT )
        || ( exception_number == KEYBOARD_BREAK_EVENT )
        || ( exception_number == INVALID_INTERRUPT_ABEND )
        || ( exception_number == ASSEMBLY_ABEND ) ) ) {
        return( RETURN_TO_NEXT_DEBUGGER );
    }
    if( exception_number == 1 && !TrapInt1 && !FakeBreak ) {
        return( RETURN_TO_NEXT_DEBUGGER ); /* Break set by system debugger */
    }
    if( exception_number == 2 ) { /* NMI */
        if( DebuggerRunning ) {
            return( RETURN_TO_NEXT_DEBUGGER );
        } else {
            FieldEFLAGS( frame ) |= FLG_T;
            FakeBreak = TRUE;
            return( RETURN_TO_PROGRAM );
        }
    }

    FreeInvalidThreads();
    load = (struct LoadDefinitionStructure *)error_code;
    switch( exception_number ) {
    case START_NLM_EVENT:
        /* The NLM is being loaded. We are on the loader thread here */
        _DBG_EVENT(( "  START_NLM_EVENT: LDS=%x (%s) D_LDS=%x NLMState=%x\r\n",
                        load, &(load->LDFileName[1]),
                        DebuggerLoadedNLM, NLMState ));
        if( NLMState == NLM_LOADED ) {
            m = LocateThread( RunningProcess );
            m->load = load;
            NewNLMListEntry( load );
            _DBG_EVENT(( "MSB=%x NLMState = NLM_LOADED\r\n", m ));
            break;
        } else if( NLMState == NLM_PRELOADING
                && !DebuggerLoadedNLM
                && memcmp( load->LDFileName, NLMName, NLMName[0] + 1 )
                      == 0  ) {
            m = LocateThread( RunningProcess );
            m->description = NULL;
            m->load = load;
            m->in_start_proc = TRUE;
            saved_opcode = *(opcode_type *)(m->load->LDInitializationProcedure);
            *(opcode_type *)(m->load->LDInitializationProcedure) = BRKPOINT;
            NLMState = NLM_LOADING;
            _DBG_EVENT(( "MSB=%x NLMState = NLM_LOADING\r\n", m ));
        } else {
            _DBG_EVENT(( "NLMState = unknown state\r\n" ));
        }
        return( RETURN_TO_PROGRAM );
    case TERMINATE_NLM_EVENT:
        _DBG_EVENT(( "  TERMINATE_NLM_EVENT: LDS=%x (%s) D_LDS=%x\r\n",
                        load, &(load->LDFileName[1]), DebuggerLoadedNLM ));
        if( load == DebuggerLoadedNLM ) {
            DebuggerLoadedNLM = NULL;
            ExpectingEvent = FALSE;
            MSB = NULL;
            WakeDebugger();
        } else {
            DeadNLMListEntry( load );
            if( DebuggerLoadedNLM != NULL )
                break;
        }
        return( RETURN_TO_PROGRAM );
    case START_THREAD_EVENT:
        load = GetNLMFromPID( RunningProcess );
        _DBG_EVENT(( "  START_THREAD_EVENT: LDS=%x (%s) D_LDS=%x\r\n",
                        load, &(load->LDFileName[1]), DebuggerLoadedNLM ));
        m = LocateThread( RunningProcess );
        m->load = load;
        if( load == DebuggerLoadedNLM ) {
            m->clib_created = TRUE;
        }
        _DBG_EVENT(( "MSB=%x\r\n", m ));
        return( RETURN_TO_PROGRAM );
    case TERMINATE_THREAD_EVENT:
        /* The thread is terminating. Free it's control block and let it die */
        load = GetNLMFromPID( RunningProcess ); // returns NULL
        _DBG_EVENT(( "  TERMINATE_THREAD_EVENT: LDS=%x (%s) D_LDS=%x,\r\n",
                        load, &(load->LDFileName[1]), DebuggerLoadedNLM ));
        m = LocateThread( RunningProcess );
        _DBG_EVENT(( "MSB=%x, NLM=%x\r\n", m, m->load ));
        FreeThread( m );
        return( RETURN_TO_PROGRAM );
    case THREAD_BOOBY_TRAP_EVENT:
        _DBG_EVENT(( "  THREAD_BOOBY_TRAPEVENT:\r\n" ));
#ifdef __NW40__
        if( GetFileServerMinorVersionNumber() < 10 ) {
//          FieldESP( frame ) += 4; // SignalDebuggerEvent points to return address
        }
#endif
    case ENTER_DEBUGGER_EVENT:
    case KEYBOARD_BREAK_EVENT:
    case INVALID_INTERRUPT_ABEND:
    case ASSEMBLY_ABEND:
    case BREAKPOINT_FUNCTION_EVENT:
        m = LocateThread( RunningProcess );
        break;
    case NLM_FAILED_INIT_EVENT:
        _DBG_EVENT(( "  NLM_FAILED_INIT_EVENT:\r\n" ));
        if( NLMState != NLM_FORCED_INIT_FAILURE ) {
            m = LocateThread( RunningProcess );
            DebuggerLoadedNLM = NULL;
            ExpectingEvent = FALSE;
            MSB = NULL;
            WakeDebugger();
        }
        return( RETURN_TO_PROGRAM );
        break;
    case 3:
        m = LocateThread( RunningProcess );
        if( NLMState == NLM_LOADING && m->in_start_proc ) {
            _DBG_EVENT(( "  3: Helper thread hits initial break\r\n" ));
            /* the helper thread hits a break point we set at start procedure */
            SaveRegs( HelperThreadRegs );
            DebuggerLoadedNLM = m->load;
            --(FieldEIP( frame ));
            *(opcode_type *)(FieldEIP( frame )) = saved_opcode;
            NLMState = NLM_IN_START_PROC;
            _DBG_EVENT(( "  3: NLMState = NLM_IN_START_PROC\r\n" ));
        } else {
            --(FieldEIP( frame ));
#if 0
            if( !CheckIfBreakOKInOS( FieldEIP( frame ) ) ) {
                return( RETURN_TO_NEXT_DEBUGGER );
            }
#endif
        }
        break;
    default:
        if( exception_number > 31 ) {
            return( RETURN_TO_NEXT_DEBUGGER );
        }
        m = LocateThread( RunningProcess );
#if 0
        if( !CheckIfBreakOKInOS( FieldEIP( frame ) ) ) {
            return( RETURN_TO_NEXT_DEBUGGER );
        }
#endif
        break;
    }
    m->xnum = exception_number;
    m->errnum = error_code;
    m->description = (char *)FieldExceptionDescription( frame );
    SaveRegs( m->cpu );
    DumpRegs( m, "DebugEntry" );
    if( NPX() != X86_NO ) {
        Read387( &m->fpu );
    }
/*
 *
 * This requires comment. Netware doesn't allow us to relinquish control
 * at interrupt time. This means we have to fake the application into
 * returning to the following routine and "becoming" the debugger.
 *
 */
    _DBG_EVENT(( "DebugEntry: Leaving MSB=%8x xnum=%d PID=%8x\r\n", m, m->xnum, m->os_id ));
    FieldESI( frame ) = (LONG)m;
    FieldEIP( frame ) = (LONG)&BigKludge;
    return( RETURN_TO_PROGRAM );
}

void JumpTo( msb *m )
{
    long        ax,bx,cx,dx,si,di,bp,sp,fl,ip;

    ax = m->cpu.EAX;
    bx = m->cpu.EBX;
    cx = m->cpu.ECX;
    dx = m->cpu.EDX;
    si = m->cpu.ESI;
    di = m->cpu.EDI;
    bp = m->cpu.EBP;
    sp = m->cpu.ESP;
    fl = m->cpu.EFL;
    ip = m->cpu.EIP;
    DumpRegs( m, "JumpTo" );
    if( m->to_be_killed )
        FreeThread( m );
    DoALongJumpTo( ax,bx,cx,dx,si,di,bp,sp,fl,ip );
}


void BigKludge( msb *m )
{
    _DBG_EVENT(( "*BigKludge: MSB=%8x xnum=%d PID=%8x\r\n", m, m->xnum, m->os_id ));
    m->asleep = TRUE;
#if defined ( _USE_NEW_KERNEL )
    if( NULL == m->ksem ){
        m->ksem = kSemaphoreAlloc( NULL, 0 );
        if( NULL == m->ksem ){
            _DBG_ERROR(( "*BigKludge: Failed to allocate semaphore\r\n" ));
        }
    }
#else
    if( m->sem == 0 ) {
        m->sem = CAllocSemaphore( 0, SemaphoreTag );
    }
#endif
    if( m->xnum != THREAD_BOOBY_TRAP_EVENT ) {
        _DBG_EVENT(( "  Waking up the debugger for debug event\r\n" ));
        BoobyTrapPID( m->os_id ); /* catch all other threads in NLM */
        if( !DebuggerRunning ) {
            MSB = m;
        }
        WakeDebugger();
    }
#if defined ( _USE_NEW_KERNEL )
    _DBG_THREAD(( "Putting to sleep MSB=%8x on sem=%8x\r\n", m, m->ksem ));
    kSemaphoreWait( m->ksem );
#else
    _DBG_THREAD(( "Putting to sleep MSB=%8x on sem=%8x\r\n", m, m->sem ));
    CPSemaphore( m->sem );
#endif
    m->asleep = FALSE;
    _DBG_EVENT(( "  Waking up as MSB=%8x\r\n", m ));
    if( NPX() != X86_NO ) {
        Write387( &m->fpu );
    }
    if( ( m->cpu.EFL & FLG_T ) && m->cpu.EIP == (dword)DebugEntry ) {
        /* If we are about to trace through this routine, don't! Ouch! */
        m->cpu.EFL &= ~FLG_T;
    }
    ActivateDebugRegs();
    UnBoobyTrapPID( m->os_id );
    //_DBG_THREAD(( "Rolling. EIP=%8x, ESP=%8x, Process=%8x\r\n", m->cpu.EIP, m->cpu.ESP, RunningProcess ));
    if( m->to_be_killed ) {
        if( m->in_start_proc ) {
            int         newESP;
            //_DBG_THREAD(( "     Attempting to kill myself!!!\r\n" ));
            newESP = m->cpu.ESP;
            FreeThread( m );
            if( newESP <= HelperThreadRegs.ESP ) {
                /* the stack frame has not already gone */
                m->cpu = HelperThreadRegs;
                ReturnESP = m->cpu.ESP;
                m->cpu.EIP = (dword)Return;
                m->cpu.EAX = -1;
                NLMState = NLM_FORCED_INIT_FAILURE;
                _DBG_EVENT(( "  NLMState = NLM_FORCED_INIT_FAILURE\r\n" ));
            }
        } else if( m->clib_created ) {
#if defined ( __NW50__ )
            int type = get_app_type();

            if( type & LIBRARY_CLIB ) {
                _DBG_THREAD(( "     Warping off to _exit!!!\r\n" ));
                m->cpu.EIP = (dword)_exit;
            } else if( type & LIBRARY_LIBC) {
                _DBG_THREAD(( "     Warping off to NXVMExit!!!\r\n" ));
                m->cpu.EIP = (dword)NXVmExit;
            } else {
                _DBG_THREAD(( "     Warping off to suicide!!!\r\n" ));
                m->cpu.EIP = (dword)Suicide;
            }
#else
            _DBG_THREAD(( "     Warping off to _exit!!!\r\n" ));
            m->cpu.EIP = (dword)_exit;
#endif
        } else {
            _DBG_THREAD(( "     Suicide!!!\r\n" ));
            m->cpu.EIP = (dword)Suicide;
        }
    }
    JumpTo( m );
}

#if defined ( __NW50__ )    /* This is duplicate to 4.0 but I want to change soon */
    static struct debuggerStructure DbgStruct = {
        NULL,
        NULL,
        (LONG(*)(StackFrame *))DebugEntry,
        AT_FIRST,
        TSS_FRAME_BIT
    };
#elif defined ( __NW40__ )
    static struct debuggerStructure DbgStruct = {
        NULL,
        NULL,
        (LONG(*)(StackFrame *))DebugEntry,
        AT_FIRST,
        TSS_FRAME_BIT
    };
#elif defined ( __NW30__ )
    static T_DebuggerStruct DbgStruct = {
        0,
        NULL,
        DebugEntry
    };
#endif

trap_retval ReqGet_sys_config( void )
{
    get_sys_config_ret  *ret;

    ret = GetOutPtr(0);
    ret->sys.cpu = X86CPUType();
    ret->sys.fpu = NPX();
    ret->sys.osmajor = FileServerMajorVersionNumber;
    ret->sys.osminor = FileServerMinorVersionNumber;
    ret->sys.os = OS_NW386;
    ret->sys.huge_shift = 12;
    ret->sys.mad = MAD_X86;
    return( sizeof( *ret ) );
}

trap_retval ReqMap_addr( void )
{
    nlm_entry   *curr;
    map_addr_req        *acc;
    map_addr_ret        *ret;
    struct LoadDefinitionStructure *ld;

    //NYI: needs work for DWARF flat mode
    acc = GetInPtr(0);
    ret = GetOutPtr(0);
    ret->out_addr.segment = acc->in_addr.segment;
    ret->out_addr.offset = acc->in_addr.offset;
    ret->lo_bound = 0;
    ret->hi_bound = 0;
    _DBG_MISC(( "AccMapAddr\r\n" ));
    curr = (nlm_entry *)acc->handle;
    if( curr == NULL ) { // main NLM
        _DBG_MISC(( "Main NLM\r\n" ));
        if( MSB == NULL )
            return( sizeof( *ret ) );
        if( MSB->load == NULL )
            return( sizeof( *ret ) );
        ld = MSB->load;
    } else {
        ld = &curr->ld;
    }
    ret->hi_bound = ~(addr48_off)0;
    _DBG_MISC(( "Mapping %4x:%8x\r\n", acc->in_addr.segment, acc->in_addr.offset ));
    switch( acc->in_addr.segment ) {
    case MAP_FLAT_CODE_SELECTOR:
    case MAP_FLAT_DATA_SELECTOR:
        if( ret->out_addr.offset & NOV_EXP_ISCODE ) {
            ret->lo_bound = NOV_EXP_ISCODE;
            ret->out_addr.offset &= ~NOV_EXP_ISCODE;
            ret->out_addr.offset += ld->LDCodeImageOffset;
        } else {
            ret->hi_bound = NOV_EXP_ISCODE - 1;
            ret->out_addr.offset += ld->LDDataImageOffset;
        }
        if( acc->in_addr.segment == MAP_FLAT_CODE_SELECTOR ) {
            ret->out_addr.segment = GetCS();
        } else {
            ret->out_addr.segment = GetDS();
        }
        break;
    case 1:
        ret->out_addr.offset += ld->LDCodeImageOffset;
        ret->out_addr.segment = GetCS();
        break;
    case 2:
        ret->out_addr.offset += ld->LDDataImageOffset;
        ret->out_addr.segment = GetDS();
        break;
    }
    _DBG_MISC(( "------  %4x:%8x\r\n", ret->out_addr.segment, ret->out_addr.offset ));
    return( sizeof( *ret ) );
}

//obsolete
trap_retval ReqAddr_info( void )
{
    addr_info_ret       *ret;

    ret = GetOutPtr( 0 );
    ret->is_big = TRUE;
    return( sizeof( *ret ) );
}

trap_retval ReqMachine_data( void )
{
    machine_data_ret    *ret;
    unsigned_8          *data;

    ret = GetOutPtr( 0 );
    data = GetOutPtr( sizeof( *ret ) );
    ret->cache_start = 0;
    ret->cache_end = ~(addr_off)0;
    *data = X86AC_BIG;
    return( sizeof( *ret ) + sizeof( *data ) );
}

static int ReadMemory( addr48_ptr *addr, unsigned long req, void *buf )
{
    if( MSB == NULL )
        return( -1 );
    if( CValidatePointer( (char *)addr->offset ) == 0 )
        return( -1 );
    if( CValidatePointer( (char *)addr->offset + req - 1 ) == 0 )
        return( -1 );
    memcpy( buf, (void *)addr->offset, req );
    return( 0 );
}

static int WriteMemory( addr48_ptr *addr, unsigned long req, void *buf )
{
    if( MSB == NULL )
        return( -1 );
    if( CValidatePointer( (char *)addr->offset ) == 0 )
        return( -1 );
    if( CValidatePointer( (char *)addr->offset + req - 1 ) == 0 )
        return( -1 );
    memcpy( (void *)addr->offset, buf, req );
    return( 0 );
}

static unsigned short ReadWrite( int (*rtn)(addr48_ptr *, unsigned long, void *), addr48_ptr *addr,
                                char *buff, unsigned short requested )
{
    int                 err;
    unsigned short      len;

    err = rtn( addr, (unsigned long)requested, buff );
    if( err == 0 ) {
        addr->offset += requested;
        return( requested );
    }
    len = requested;
    while( len != 0 ) {
        if( rtn( addr, 1UL, buff++ ) != 0 )
            break;
        --len;
        addr->offset++;
    }
    return( requested - len );
}


trap_retval ReqChecksum_mem( void )
{
    trap_elen           len;
    addr48_ptr          addr;
    int                 i;
    checksum_mem_req    *acc;
    checksum_mem_ret    *ret;
    trap_elen           want;
    trap_elen           got;

    acc = GetInPtr(0);
    ret = GetOutPtr(0);
    len = acc->len;
    addr.offset = acc->in_addr.offset;
    addr.segment = acc->in_addr.segment;
    ret->result = 0;
    while( len > 0 ) {
        want = len;
        if( want > BUFF_SIZE )
            want = BUFF_SIZE;
        got = ReadWrite( ReadMemory, &addr, UtilBuff, want );
        for( i = 0; i < got; ++i ) {
            ret->result += UtilBuff[ i ];
        }
        if( got != want )
            break;
        len -= want;
    }
    return( sizeof( *ret ) );
}


trap_retval ReqRead_mem( void )
{
    addr48_ptr          addr;
    read_mem_req        *acc;
    trap_elen           len;

    acc = GetInPtr(0);

    addr.offset = acc->mem_addr.offset;
    addr.segment = acc->mem_addr.segment;
    len = ReadWrite( ReadMemory, &addr, GetOutPtr(0), acc->len );
    return( len );
}

trap_retval ReqWrite_mem( void )
{
    addr48_ptr          addr;
    write_mem_req       *acc;
    write_mem_ret       *ret;
    trap_elen           len;

    acc = GetInPtr(0);
    ret = GetOutPtr(0);

    len = GetTotalSize() - sizeof(*acc);

    addr.offset = acc->mem_addr.offset;
    addr.segment = acc->mem_addr.segment;
    ret->len = ReadWrite( WriteMemory, &addr, GetInPtr(sizeof(*acc)), len );
    return( sizeof( *ret ) );
}


trap_retval ReqRead_io( void )
{
    read_io_req     *acc;
    void            *data;

    acc = GetInPtr(0);
    data = GetOutPtr( 0 );
    if( acc->len == 1 ) {
        *( (byte *)data ) = in_b( acc->IO_offset );
    } else if( acc->len == 2 ) {
        *( (word *)data ) = in_w( acc->IO_offset );
    } else {
        *( (dword *)data ) = in_d( acc->IO_offset );
    }
    return( acc->len );
}


trap_retval ReqWrite_io( void )
{
    write_io_req    *acc;
    write_io_ret    *ret;
    void            *data;
    trap_elen       len;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    data = GetInPtr( sizeof( *acc ) );
    len = GetTotalSize() - sizeof( *acc );
    if( len == 1 ) {
        out_b( acc->IO_offset, *( (byte *)data ) );
    } else if( len == 2 ) {
        out_w( acc->IO_offset, *( (word *)data ) );
    } else {
        out_d( acc->IO_offset, *( (dword *)data ) );
    }
    ret->len = len;
    return( sizeof( *ret ) );
}

//obsolete
trap_retval ReqRead_cpu( void )
{
    trap_cpu_regs       *regs;

    regs = GetOutPtr(0);
    if( MSB == NULL ) {
        memset( regs, 0, sizeof( *regs ) );
    } else {
        *regs = MSB->cpu;
    }
    return( sizeof( *regs ) );
}

//obsolete
trap_retval ReqRead_fpu( void )
{
    trap_fpu_regs       *regs;

    regs = GetOutPtr(0);
    if( MSB == NULL ) {
        memset( regs, 0, sizeof( *regs ) );
    } else {
        *regs = MSB->fpu;
    }
    return( sizeof( *regs ) );
}

//obsolete
trap_retval ReqWrite_cpu( void )
{
    if( MSB != NULL ) {
        MSB->cpu = *(trap_cpu_regs *)GetInPtr(sizeof(write_cpu_req));
    }
    return( 0 );
}

//obsolete
trap_retval ReqWrite_fpu( void )
{
    if( MSB != NULL ) {
        MSB->fpu = *(trap_fpu_regs *)GetInPtr(sizeof(write_fpu_req));
    }
    return( 0 );
}

trap_retval ReqRead_regs( void )
{
    mad_registers       *mr;

    mr = GetOutPtr(0);
    memset( mr, 0, sizeof( mr->x86 ) );
    if( MSB != NULL ) {
        mr->x86.cpu = *(struct x86_cpu *)&MSB->cpu;
        mr->x86.u.fpu = *(struct x86_fpu *)&MSB->fpu;
    }
    return( sizeof( mr->x86 ) );
}

trap_retval ReqWrite_regs( void )
{
    mad_registers       *mr;

    mr = GetInPtr(sizeof(write_regs_req));
    if( MSB != NULL ) {
        *(struct x86_cpu *)&MSB->cpu = mr->x86.cpu;
        *(struct x86_fpu *)&MSB->fpu = mr->x86.u.fpu;
    }
    return( 0 );
}

static char *LoadName;
static unsigned LoadLen;
static prog_load_ret *LoadRet;

static void LoadHelper( void )
{
    int         err;
    int         handle;
    char        *src, *dst;
    char        nlm_name[256];
    char        ch;
    unsigned    len;

    Enable();
    MSBHead = NULL;
    ThreadId = 0;
    MSB = NULL;
    src = LoadName;
    StringToNLMPath( src, nlm_name );
    dst = CmdLine;
    len = LoadLen;

    for( ;; ) {
        if( len == 0 )
            break;
        ch = *src;
        if( ch == '\0' )
            ch = ' ';
        *dst = ch;
        ++dst;
        ++src;
        --len;
    }

    if( dst > CmdLine && src[-1] == '\0' )
        --dst;
    *dst = '\0';
    LoadRet->err = 0;
    NLMState = NLM_PRELOADING;
    _DBG_EVENT(( "*LoadHelper: NLMState = NLM_PRELOADING\r\n" ));
    ExpectingEvent = TRUE;

    if( nlm_name[0] == '\0'
     || ( handle = IOOpen( nlm_name, O_RDONLY ) ) == -1
     || IOSeek( handle, SEEK_SET, offsetof( nlm_header, moduleName ) )
                != offsetof( nlm_header, moduleName )
     || IORead( handle, NLMName, 14 ) != 14
     || IOClose( handle ) != 0 ) {
        NLMState = NLM_NONE;
        _DBG_EVENT(( "  NLMState = NLM_NONE\r\n" ));
        DebuggerLoadedNLM = NULL;
        if( LoadRet != NULL )
            LoadRet->err = 2;
        _DBG_EVENT(( "  Waking up the debugger for noopen event\r\n" ));
        WakeDebugger();
    } else {
        _DBG_EVENT(( "  Name is '%S'\r\n", NLMName ));
        err = LoadModule( systemConsoleScreen, (BYTE *)CmdLine, LO_DEBUG );
        _DBG_EVENT(( "  Load ret code %d\r\n", err ));
        if( err != 0 ) {
            NLMState = NLM_NONE;
            _DBG_EVENT(( "    NLMState = NLM_NONE\r\n" ));
            DebuggerLoadedNLM = NULL;
            if( LoadRet != NULL )
                LoadRet->err = err;
            _DBG_EVENT(( "    Waking up the debugger for noload event\r\n" ));
            WakeDebugger();
        }
    }
    FreeThread( LocateThread( RunningProcess ) );

    if( NLMState != NLM_NONE) {
        NLMState = NLM_LOADED;
        _DBG_EVENT(( "LoadHelper: NLMState = NLM_LOADED\r\n" ));
#if defined ( _USE_NEW_KERNEL )
        kSemaphoreWait( kHelperSem );
#else
        CPSemaphore( HelperSem );
#endif
        _DBG_EVENT(( "LoadHelper: Helper awake -- calling KillMe NLM=%8x!\r\n", DebuggerLoadedNLM ));
        NLMState = NLM_NONE;
        _DBG_EVENT(( "LoadHelper: NLMState = NLM_NONE\r\n" ));
        if( DebuggerLoadedNLM )
            KillMe( DebuggerLoadedNLM );
    }
    DebuggerLoadedNLM = NULL;
    _DBG_EVENT(( "LoadHelper: Helper killing itself\r\n" ));
    Suicide();
}

#if 0
static void LoadHelper( void )
{
    int         err;
    int         handle;
    char        *src, *dst;
    char        nlm_name[256];
    char        ch;
    unsigned    len;

    Enable();
    MSBHead = NULL;
    ThreadId = 0;
    MSB = NULL;
    src = LoadName;
    StringToNLMPath( src, nlm_name );
    dst = CmdLine;
    len = LoadLen;
    for( ;; ) {
        if( len == 0 )
            break;
        ch = *src;
        if( ch == '\0' )
            ch = ' ';
        *dst = ch;
        ++dst;
        ++src;
        --len;
    }
    if( dst > CmdLine && src[-1] == '\0' )
        --dst;
    *dst = '\0';
    LoadRet->err = 0;
    NLMState = NLM_PRELOADING;
    _DBG_EVENT(( "*LoadHelper: NLMState = NLM_PRELOADING\r\n" ));
    ExpectingEvent = TRUE;
    if( nlm_name[0] == '\0'
     || ( handle = IOOpen( nlm_name, O_RDONLY ) ) == -1
     || IOSeek( handle, SEEK_SET, offsetof( nlm_header, moduleName ) )
                != offsetof( nlm_header, moduleName )
     || IORead( handle, NLMName, 14 ) != 14
     || IOClose( handle ) != 0 ) {
        NLMState = NLM_NONE;
        _DBG_EVENT(( "  NLMState = NLM_NONE\r\n" ));
        DebuggerLoadedNLM = NULL;
        if( LoadRet != NULL ) LoadRet->err = 2;
        _DBG_EVENT(( "  Waking up the debugger for noopen event\r\n" ));
        WakeDebugger();
    } else {
        _DBG_EVENT(( "  Name is '%S'\r\n", NLMName ));
        err = LoadModule( systemConsoleScreen, CmdLine, LO_DEBUG );
        _DBG_EVENT(( "  Load ret code %d\r\n", err ));
        if( err != 0 ) {
            NLMState = NLM_NONE;
            _DBG_EVENT(( "    NLMState = NLM_NONE\r\n" ));
            DebuggerLoadedNLM = NULL;
            if( LoadRet != NULL )
                LoadRet->err = err;
            _DBG_EVENT(( "    Waking up the debugger for noload event\r\n" ));
            WakeDebugger();
        }
    }
    FreeThread( LocateThread( RunningProcess ) );
    NLMState = NLM_LOADED;
    _DBG_EVENT(( "LoadHelper: NLMState = NLM_LOADED\r\n" ));
    CPSemaphore( HelperSem );
    _DBG_EVENT(( "LoadHelper: Helper awake -- calling KillMe NLM=%8x!\r\n", DebuggerLoadedNLM ));
    NLMState = NLM_NONE;
    _DBG_EVENT(( "LoadHelper: NLMState = NLM_NONE\r\n" ));
    if( DebuggerLoadedNLM )
        KillMe( DebuggerLoadedNLM );
    DebuggerLoadedNLM = NULL;
    _DBG_EVENT(( "LoadHelper: Helper killing itself\r\n" ));
    Suicide();
}
#endif

static char    helper_stack[8192]; /* We may return before thread dies! */

trap_retval ReqProg_load( void )
{
    prog_load_ret       *ret;
    struct LoadDefinitionStructure *ld;
    LoadedListHandle    nlm;

    LoadName = (char *)GetInPtr( sizeof( prog_load_req ) );
    LoadLen = GetTotalSize() - sizeof( prog_load_req );
    ret = GetOutPtr( 0 );
    LoadRet = ret;
    // scheduling priority, code address, stack top, stack len, process name, resource tag
    CMakeProcess( 50, &LoadHelper, &helper_stack[ sizeof( helper_stack ) ],
                    sizeof( helper_stack ), TRP_The_WATCOM_Debugger, ProcessTag );
    _DBG_EVENT(( "*ReqProg_load: Putting debugger to sleep for load of %s\r\n", LoadName ));
    SleepDebugger();
    _DBG_EVENT(( "ReqProg_load: Debugger awake after load\r\n" ));

    ret->task_id = 1;
    ret->flags = LD_FLAG_IS_BIG | LD_FLAG_IS_PROT;
    ret->mod_handle = 0;
    LoadRet = NULL;
    // The list that is traversed are all the loaded NLMs.
    // nlm is an index/handle of some sort.
    // ld is a pointer to a block of information about the NLM
    //    including its name.
    nlm = 0;
    for( ;; ) {
        nlm = GetNextLoadedListEntry( nlm );
        if( nlm == 0 )
            break;
        ld = ValidateModuleHandle( nlm );
        if( ld != DebuggerLoadedNLM ) {
            // skip the main NLM
            NewNLMListEntry( ld );
        }
    }
    _DBG_THREAD(("ReqProg_load: MSB after load=%8x\r\n", MSB));
    return( sizeof( *ret ) );
}

trap_retval ReqProg_kill( void )
{
    msb         *m;
    prog_kill_ret       *ret;

    FreeInvalidThreads();
    if( DebuggerLoadedNLM != NULL ) {
        m = MSBHead;
        while( m != NULL ) {
            m->to_be_killed = TRUE;
            if( m->asleep && m->clib_created ) {
                DebuggerLoadedNLM = NULL;
                SetupPIDForACleanExit( m->os_id );
            }
#if defined ( _USE_NEW_KERNEL )
            _DBG_THREAD(( "----- Releasing semaphore for MSB=%8x, sem=%8x\r\n", m, m->ksem ));
            msb_KernelSemaphoreReleaseAll( m );
            m->ksem = NULL;
#else
            _DBG_THREAD(( "----- Releasing semaphore for MSB=%8x, sem=%8x\r\n", m, m->sem ));
            CSemaphoreReleaseAll( m->sem ); /* will be freed when thread awakes */
            m->sem = 0;
#endif
            m = m->next;
        }
    }
    while( NLMList != NULL ) {
        FreeAnNLMListEntry();
    }
    LastNLMListEntry = NULL;
#if defined ( _USE_NEW_KERNEL )
    kSemaphoreSignal( kHelperSem );
#else
    CVSemaphore( HelperSem );
#endif
    ret = GetOutPtr( 0 );
    ret->err = 0;
    return( sizeof( *ret ) );
}


unsigned Execute( msb *which )
{
    msb         *m;

    for( m = MSBHead; m != NULL; m = m->next ) {
        _DBG_THREAD(( "check thread msb=%8x: frozen=%d, asleep=%d\r\n", m, m->frozen, m->asleep ));
        if( !m->frozen && m->asleep ) {
            if( which == NULL || which == m ) {
#if defined ( _USE_NEW_KERNEL )
                _DBG_THREAD(( "Letting a thread execute, MSB=%8x, sem=%8x\r\n", m, m->ksem ));
                kSemaphoreSignal( m->ksem );
#else
                _DBG_THREAD(( "Letting a thread execute, MSB=%8x, sem=%8x\r\n", m, m->sem ));
                CVSemaphore( m->sem );
#endif
            }
        }
    }
    /* freeze all threads!!! */
    _DBG_EVENT(( "*Execute: Putting debugger to sleep for execution\r\n" ));
    SleepDebugger();
    _DBG_EVENT(( "Execute: Debugger awake after execution\r\n" ));
    if( MSB == NULL )
        return( COND_TERMINATE );
    switch( MSB->xnum )
    {
    case 1:
        if( FakeBreak ) {
            FakeBreak = FALSE;
            return( COND_USER );
        }
        if( MSB->errnum & DR6_BS ) {
            return( COND_TRACE );
        } else if( MSB->errnum & ( DR6_B0+DR6_B1+DR6_B2+DR6_B3 ) ) {
            return( COND_WATCH );
        } else {
            return( COND_EXCEPTION );
        }
    case 3:
        return( COND_BREAK );
    case ENTER_DEBUGGER_EVENT:
    case KEYBOARD_BREAK_EVENT:
        return( COND_USER );
    case TERMINATE_NLM_EVENT:
        if( DebuggerLoadedNLM == NULL ) {
            return( COND_LIBRARIES | COND_TERMINATE );
        } else {
            return( COND_LIBRARIES );
        }
    case START_NLM_EVENT:
        return( COND_LIBRARIES );
    case START_THREAD_EVENT:
    case TERMINATE_THREAD_EVENT:
        return( 0 );
    case INVALID_INTERRUPT_ABEND:
    case ASSEMBLY_ABEND:
    case BREAKPOINT_FUNCTION_EVENT:
    default:
        return( COND_EXCEPTION );
    }
}

trap_retval ReqSet_watch( void )
{
    dword           l;
    set_watch_req   *acc;
    set_watch_ret   *ret;
    watch           *curr;
    int             i, needed;
    int             dreg_avail[4];

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = 1;
    ret->multiplier = 2000;
    if( WatchCount < MAX_WP && ReadMemory( &acc->watch_addr, 4UL, &l ) == 0 ) {
        ret->err = 0;
        curr = WatchPoints + WatchCount;
        curr->addr.segment = acc->watch_addr.segment;
        curr->addr.offset = acc->watch_addr.offset;
        curr->linear = acc->watch_addr.offset;
        curr->len = acc->size;
        curr->linear &= ~(curr->len-1);
        curr->dregs = ( curr->addr.offset & (curr->len-1) ) ? 2 : 1;
        curr->value = l;
        ++WatchCount;
        needed = 0;
        for( i = 0; i < WatchCount; ++i ) {
            needed += WatchPoints[ i ].dregs;
        }
        for( i = 0; i < NUM_DREG; ++i ) {
            dreg_avail[ i ] = DoReserveBreakpoint();
            if( dreg_avail[ i ] < 0 )
                break;
        }
        for( i = 0; i < NUM_DREG; ++i ) {
            if( dreg_avail[ i ] < 0 )
                break;
            UnReserveABreakpoint( dreg_avail[ i ] );
        }
        if( needed <= i )
            ret->multiplier |= USING_DEBUG_REG;
    }
    return( sizeof( *ret ) );
}

trap_retval ReqClear_watch( void )
{
    WatchCount = 0;
    return( 0 );
}

trap_retval ReqSet_break( void )
{
    set_break_req   *acc;
    set_break_ret   *ret;
    opcode_type     brk_opcode;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    if( ReadMemory( &acc->break_addr, sizeof( brk_opcode ), &brk_opcode ) != 0 ) {
        ret->old = 0;
    } else {
        ret->old = brk_opcode;
        brk_opcode = BRKPOINT;
        WriteMemory( &acc->break_addr, sizeof( brk_opcode ), &brk_opcode );
    }
    return( sizeof( *ret ) );
}

trap_retval ReqClear_break( void )
{
    clear_break_req     *acc;
    opcode_type         brk_opcode;

    acc = GetInPtr( 0 );
    brk_opcode = acc->old;
    WriteMemory( &acc->break_addr, sizeof( brk_opcode ), &brk_opcode );
    return( 0 );
}

static void ClearDebugRegs( void )
{
    int     i;

    for( i = 0; i < NUM_DREG; ++i ) {
        if( DR[i].type != NO_DREG ) {
            DR[i].type = NO_DREG;
            _DBG_DR(( "unreserve %d\r\n", i ));
            UnReserveABreakpoint( i );
        }
    }
}


static void ActivateDebugRegs( void )
{
    int         i;

    for( i = 0; i < NUM_DREG; ++i ) {
        if( DR[i].type != NO_DREG ) {
            _DBG_DR(( "set %d, addr=%8x, typ=%d, siz=%d\r\n", i, DR[i].address, DR[i].type, DR[i].size ));
           CSetABreakpoint( i, DR[i].address, DR[i].type, DR[i].size );
        }
    }
}

static bool SetDR( unsigned address, int len )
{
    int     i;
    i = DoReserveBreakpoint();
    _DBG_DR(( "reserved %d addr=%8x\r\n", i, address ));
    if( i < 0 ) {
        ClearDebugRegs();
        return( FALSE );
    }
    DR[i].address = address;
    DR[i].type = DR7_BWR;
    DR[i].size = len;
    return( TRUE );
}

static bool SetDebugRegs( void )
{
    int         i;
    unsigned    address;
    watch       *wp;

    for( i = WatchCount, wp = WatchPoints; i != 0; --i, ++wp ) {
        address = wp->addr.offset;
        _DBG_DR(( "offset = %8x, addr=%8x\r\n", wp->addr.offset, address ));
        if( !SetDR( wp->linear, wp->len ) )
            return( FALSE );
        if( wp->dregs == 2 ) {
            if( !SetDR( wp->linear+wp->len, wp->len ) )
                return( FALSE );
        }
    }
    return( TRUE );
}

static trap_elen ProgRun( bool step )
{
    watch       *wp;
    int         i;
    dword       value;
    prog_go_ret *ret;

    ret = GetOutPtr( 0 );
    ret->conditions = COND_THREAD;
    if( MSB == NULL ) {
        ret->conditions = COND_TERMINATE;
    } else if( step ) {
        MSB->cpu.EFL |= FLG_T;
        TrapInt1 = TRUE;
        ret->conditions |= Execute( MSB );
        TrapInt1 = FALSE;
    } else if( WatchCount != 0 ) {
        if( SetDebugRegs() ) {
            TrapInt1 = TRUE;
            ret->conditions |= Execute( NULL );
            TrapInt1 = FALSE;
        } else {
            for( ;; ) {
                MSB->cpu.EFL |= FLG_T;
                TrapInt1 = TRUE;
                ret->conditions |= Execute( MSB );
                TrapInt1 = FALSE;
                if( ret->conditions & COND_TERMINATE )
                    break;
                if( MSB->xnum != 1 )
                    break;
                if( !( MSB->errnum & DR6_BS ) )
                    break;
                for( wp = WatchPoints, i = WatchCount; i > 0; ++wp, --i ) {
                    ReadMemory( &wp->addr, 4UL, &value );
                    if( value != wp->value ) {
                        ret->conditions |= COND_WATCH;
                        goto leave;
                    }
                }
            }
        }
    } else {
        ret->conditions |= Execute( NULL );
    }
leave:
    if( MSB == NULL ) {
        ret->program_counter.offset = 0;
        ret->program_counter.segment = 0;
        ret->stack_pointer.offset = 0;
        ret->stack_pointer.segment = 0;
    } else {
        MSB->cpu.EFL &= ~FLG_T;
        ret->program_counter.offset = MSB->cpu.EIP;
        ret->program_counter.segment = MSB->cpu.CS;
        ret->stack_pointer.offset = MSB->cpu.ESP;
        ret->stack_pointer.segment = MSB->cpu.SS;
    }
    WatchCount = 0;
    return( sizeof( *ret ) );
}

trap_retval ReqProg_go( void )
{
    return( ProgRun( FALSE ) );
}

trap_retval ReqProg_step( void )
{
    return( ProgRun( TRUE ) );
}

trap_retval ReqGet_next_alias( void )
{
    get_next_alias_ret  *ret;

    ret = GetOutPtr( 0 );
    ret->seg = 0;
    ret->alias = 0;
    return( sizeof( *ret ) );
}

static msb *LocateTid( dword tid )
{
    msb *m;

    m = MSBHead;
    for( ;; ) {
        if( m == NULL )
            return( NULL );
        if( m->dbg_id == tid )
            return( m );
        m = m->next;
    }
}


trap_retval ReqThread_get_next( void )
{
    msb             *m;
    thread_get_next_req *acc;
    thread_get_next_ret *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    if( acc->thread == 0 ) {
        m = MSBHead;
    } else {
        m = LocateTid( acc->thread );
        if( m != NULL )
            m = m->next;
    }
    if( m != NULL ) {
        ret->thread = m->dbg_id;
        ret->state = m->frozen ? THREAD_FROZEN : THREAD_THAWED;
    } else {
        ret->thread = 0;
    }
    return( sizeof( *ret ) );
}


trap_retval ReqThread_set( void )
{
    msb             *m;
    thread_set_req  *acc;
    thread_set_ret  *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->err = 0;
    if( MSB != NULL ) {
        ret->old_thread = MSB->dbg_id;
    } else {
        ret->old_thread = 0;
    }
    if( acc->thread != 0 ) {
        m = LocateTid( acc->thread );
        if( m != NULL && m->asleep ) {
            _DBG_THREAD(( "AccSetThread to=%d (MSB=%8x) -- \r\n", m->dbg_id, m ));
            MSB = m;
        } else {
            ret->err = 1;
        }
    }
    return( sizeof( *ret ) );
}


trap_retval ReqThread_freeze( void )
{
    msb                 *m;
    thread_freeze_req   *acc;
    thread_freeze_ret   *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    m = LocateTid( acc->thread );
    if( m == NULL ) {
        ret->err = 1;  // Failed
    } else {
        _DBG_THREAD(( "freezing %8x\r\n", m ));
        m->frozen = TRUE;
        ret->err = 0;
    }
    return( sizeof( *ret ) );
}

trap_retval ReqThread_thaw( void )
{
    msb                 *m;
    thread_thaw_req     *acc;
    thread_thaw_ret     *ret;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    m = LocateTid( acc->thread );
    if( m == NULL ) {
        ret->err = 1;  // Failed
    } else {
        _DBG_THREAD(( "thawing %8x\r\n", m ));
        m->frozen = FALSE;
        ret->err = 0;
    }
    return( sizeof( *ret ) );
}


trap_retval ReqThread_get_extra( void )
{
    msb                     *m;
    char                    *name;
    thread_get_extra_req    *acc;

    acc = GetInPtr( 0 );
    name = GetOutPtr( 0 );
    if( acc->thread == 0 ) {
        strcpy( name, "name" );
    } else {
        m = LocateTid( acc->thread );
        if( m == NULL ) {
            *name = '\0';
        } else {
            strcpy( name, GetPIDName( m->os_id ) );
        }
    }
    return( strlen( name ) + 1 );
}

trap_retval ReqGet_err_text( void )
{
    char        *err;

    /* NYI */
    err = GetOutPtr( 0 );
    strcpy( err, TRP_ERR_unknown_system_error );
    return( strlen( err ) + 1 );
}

trap_retval ReqGet_message_text( void )
{
    get_message_text_ret        *ret;
    char                        *err_txt;

    ret = GetOutPtr( 0 );
    err_txt = GetOutPtr( sizeof(*ret) );
    if( MSB->description != NULL ) {
        strcpy( err_txt, MSB->description );
    } else {
        err_txt[0] = '\0';
    }
    ret->flags = MSG_NEWLINE | MSG_ERROR;
    return( sizeof( *ret ) + strlen( err_txt ) + 1 );
}

#if 0
static LoadedListHandle GetLoadedListHandle( struct LoadDefinitionStructure *ld )
{
    LoadedListHandle    nlm;

    if( ld == NULL )
        return( 0 );
    nlm = GetNextLoadedListEntry( 0 );
    while( nlm != 0 ) {
        if( ld == ValidateModuleHandle( nlm ) ) {
            return( nlm );
        }
        nlm = GetNextLoadedListEntry( nlm );
    }
    return( 0 );
}
#endif

trap_retval ReqGet_lib_name( void )
{
    int             len;
    get_lib_name_req    *acc;
    get_lib_name_ret    *ret;
    char            *name;
    nlm_entry           *curr;

    acc = GetInPtr(0);
    ret = GetOutPtr(0);

    if( acc->handle == 0 ) {
        curr = NLMList;
    } else {
        curr = ((nlm_entry *)acc->handle)->next;
    }
    if( curr == LastNLMListEntry ) {
        LastNLMListEntry = NLMList;
        ret->handle = 0;
        return( sizeof( *ret ) );
    }
    ret->handle = (unsigned long)curr;
    name = GetOutPtr(sizeof(*ret));
    len = curr->ld.LDFileName[0];
    memcpy( name, &curr->ld.LDFileName[1], len );
    name[len] = '\0';
    if( curr->is_load ) {
        _DBG_MISC(("Loaded an NLM %s\r\n", name));
        return( sizeof( *ret ) + len + 1 );
    } else {
        _DBG_MISC(("UnLoaded NLM %s\r\n", name));
        name[0] = '\0';
        return( sizeof( *ret ) + 1 );
    }
}

trap_retval ReqRedirect_stdin( void )
{
    redirect_stdin_ret  *ret;

    ret = GetOutPtr( 0 );
    ret->err = 1;
    return( sizeof( *ret ) );
}

trap_retval ReqRedirect_stdout( void )
{
    redirect_stdout_ret *ret;

    ret = GetOutPtr( 0 );
    ret->err = 1;
    return( sizeof( *ret ) );
}


trap_retval ReqSet_user_screen( void )
{
    /* NOT IMPLEMENTED */
    return( 0 );
}


trap_retval ReqSet_debug_screen( void )
{
    /* NOT IMPLEMENTED */
    return( 0 );
}


trap_retval ReqRead_user_keyboard( void )
{
    /* NOT IMPLEMENTED */
    read_user_keyboard_ret      *ret;

    ret = GetOutPtr( 0 );
    ret->key = '\0';
    return( sizeof( *ret ) );
}

trap_retval ReqSplit_cmd( void )
{
    char             *cmd;
    char             *start;
    split_cmd_ret       *ret;
    unsigned            len;

    cmd = GetInPtr( sizeof( split_cmd_req ) );
    start = cmd;
    ret = GetOutPtr( 0 );
    len = GetTotalSize() - sizeof( split_cmd_req );

    for( ;; ) {
        if( len == 0 )
            goto done;
        switch( *cmd ) {
        case '\0':
            goto done;
        case ' ':
        case '\t':
            ret->parm_start = cmd - start + 1;
            ret->cmd_end = cmd - start;
            return( sizeof( *ret ) );
        }
        ++cmd;
        --len;
    }
done:
    ret->parm_start = cmd - start;
    ret->cmd_end = cmd - start;
    return( sizeof( *ret ) );
}


trap_version TRAPENTRY TrapInit( char *parm, char *err, bool remote )
{
    trap_version        ver;
    extern              struct LoadDefinitionStructure *MyNLMHandle;

#if defined ( __NW40__ )
    ImportCLIBSymbols();
#endif
    remote = remote; parm = parm;
    err[0] = '\0'; /* all ok */
    ver.major = TRAP_MAJOR_VERSION;
    ver.minor = TRAP_MINOR_VERSION;
    ver.remote = FALSE;
    FakeBreak = FALSE;
    RealNPXType = NPXType();
    WatchCount = 0;
#if defined ( _USE_NEW_KERNEL )
    kDebugSem = kSemaphoreAlloc( NULL, 0 );
    kHelperSem = kSemaphoreAlloc( NULL, 0 );
#else
    DebugSem = CAllocSemaphore( 0, SemaphoreTag );
    HelperSem = CAllocSemaphore( 0, SemaphoreTag );
#endif
    DebuggerLoadedNLM = NULL;
    NLMState = NLM_NONE;
    DebuggerRunning = TRUE;
    ExpectingEvent = FALSE;
    TrapInt1 = FALSE;
    DbgStruct.ddRTag = DebugTag;
    RegisterDebuggerRTag( &DbgStruct, ALWAYS_AT_FIRST );
    return( ver );
}

void TRAPENTRY TrapFini( void )
{
    UnRegisterDebugger( &DbgStruct );
    ExpectingEvent = FALSE;
    while( MSBHead )
        FreeThread( MSBHead );
#if defined ( _USE_NEW_KERNEL )
    if( kDebugSem )
        KernelSemaphoreReleaseAll( kDebugSem );
    kDebugSem = NULL;
    if( kHelperSem )
        KernelSemaphoreReleaseAll( kHelperSem );
    kHelperSem = 0;
#else
    if( DebugSem )
        CSemaphoreReleaseAll( DebugSem );
    DebugSem = 0;
    if( HelperSem )
        CSemaphoreReleaseAll( HelperSem );
    HelperSem = 0;
#endif
    while( NLMList != NULL ) {
        FreeAnNLMListEntry();
    }
    LastNLMListEntry = NULL;
}


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
* Description:  RDOS debug-class ported to C from RDOS classlibrary
*
****************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "rdos.h"
#include "debug.h"
#include "cpuglob.h"

#define FALSE   0
#define TRUE    !FALSE

#define DEBUG_MEMORY_MODEL_FLAT 1
#define DEBUG_MEMORY_MODEL_16   2
#define DEBUG_MEMORY_MODEL_32   3

#define EVENT_EXCEPTION         1
#define EVENT_CREATE_THREAD     2
#define EVENT_CREATE_PROCESS    3
#define EVENT_TERMINATE_THREAD  4
#define EVENT_TERMINATE_PROCESS 5
#define EVENT_LOAD_DLL          6
#define EVENT_FREE_DLL          7
#define EVENT_KERNEL            8

// Exception events from RDOS

struct TCreateProcessEvent
{
    int FileHandle;
    int Handle;
    int Process;
    int Thread;
    unsigned int ImageBase;
    unsigned int ImageSize;
    unsigned int ObjectRva;
    unsigned int FsLinear;
    unsigned int StartEip;
    unsigned short StartCs;
};

struct TCreateThreadEvent
{
    int Thread;
    unsigned int FsLinear;
    unsigned int StartEip;
    unsigned short StartCs;
};

struct TLoadDllEvent
{
    int FileHandle;
    int Handle;
    unsigned int ImageBase;
    unsigned int ImageSize;
    unsigned int ObjectRva;
};

struct TExceptionEvent
{
    int Code;
    unsigned int Ptr;
    unsigned int Eip;
    unsigned short Cs;
};    

struct TKernelExceptionEvent
{
    unsigned short Vector;
};    

#define MAX_DEBUG_THREADS       16

static void DebugThread( void *Param );

static struct TDebug    *DebugArr[MAX_DEBUG_THREADS];
static int              ThreadArr[MAX_DEBUG_THREADS]; 
static int              HasBreak = FALSE;
static int              BreakSel;
static long             BreakOffset;
static char             SelfKey = 0;
static char             DebugKey = 0;

struct TDebug *GetCurrentDebug()
{
    int i;
    int handle = RdosGetThreadHandle();

    for( i = 0; i < MAX_DEBUG_THREADS; i++ )
        if( ThreadArr[i] == handle )
            return( DebugArr[i] );

    return( 0 );               
}

void SetCurrentDebug( struct TDebug *obj )
{
    int i;
    int handle = RdosGetThreadHandle();

    for( i = 0; i < MAX_DEBUG_THREADS; i++ ) {
        if( ThreadArr[i] == handle ) {
            ThreadArr[i] = 0;
            DebugArr[i] = 0;
            break;            
        }
    }

    if( obj ) { 
        for( i = 0; i < MAX_DEBUG_THREADS; i++ ) {
            if( ThreadArr[i] == 0) {
                ThreadArr[i] = handle;
                DebugArr[i] = obj;
            }
        }
    }
}

static void ReadThreadState( struct TDebugThread *obj )
{
    struct ThreadState state;
    int i;
    int ok;
    char str[21];

    ok = FALSE;
    
    for( i = 0; i < 256 && !ok; i++ ) {
        RdosGetThreadState( i, &state );
        if( state.ID == obj->ThreadID )
            ok = TRUE;
    }

    if (ok) {
        strncpy(str, state.Name, 20);
        str[20] = 0;
        
        for( i = 19; i >= 0; i-- )
            if( str[i] == ' ' )
                str[i] = 0;
            else
                break;

        if( obj->ThreadName )
            free( obj->ThreadName );

        obj->ThreadName = malloc( strlen( str ) + 1 );
        strcpy( obj->ThreadName, str );

        strncpy(str, state.List, 20);
        str[20] = 0;
        
        for( i = 19; i >= 0; i-- )
            if( str[i] == ' ' )
                str[i] = 0;
            else
                break;

        if( obj->ThreadList )
            free( obj->ThreadList );

        obj->ThreadList = malloc( strlen( str ) + 1 );
        strcpy( obj->ThreadList, str );
        
        obj->ListOffset = state.Offset;
        obj->ListSel = state.Sel;
    }
}

static void InitDebugThread( struct TDebugThread *obj )
{
    obj->FaultText = "No Fault";
    obj->ThreadName = 0;
    obj->ThreadList = 0;

    obj->FDebug = FALSE;
    obj->FWasTrace = FALSE;

    obj->FHasBreak = FALSE;
    obj->FHasTrace = FALSE;
    obj->FHasException = FALSE;
}

static void InitProcessDebugThread( struct TDebugThread *obj, struct TCreateProcessEvent *event )
{
    obj->ThreadID = event->Thread;
    obj->FsLinear = event->FsLinear;
    obj->Eip = event->StartEip;
    obj->Cs = event->StartCs;

    InitDebugThread( obj );
    ReadThreadState( obj );
}

static void InitThreadDebugThread( struct TDebugThread *obj, struct TCreateThreadEvent *event )
{
    obj->ThreadID = event->Thread;
    obj->FsLinear = event->FsLinear;
    obj->Eip = event->StartEip;
    obj->Cs = event->StartCs;

    InitDebugThread( obj );
    ReadThreadState( obj );
}

static void FreeDebugThread( struct TDebugThread *obj )
{
    if( obj->ThreadName )
        free( obj->ThreadName );
    obj->ThreadName = 0;

    if( obj->ThreadList )
        free( obj->ThreadList );
    obj->ThreadList = 0;
}

int IsDebug( struct TDebugThread *obj )
{
    return obj->FDebug;
}

int HasBreakOccurred( struct TDebugThread *obj )
{
    return obj->FHasBreak;
}

int HasTraceOccurred( struct TDebugThread *obj )
{
    return obj->FHasTrace;
}

int HasFaultOccurred( struct TDebugThread *obj )
{
    return obj->FHasException;
}

static void ClearThreadBreak( struct TDebugThread *obj )
{
    obj->FHasBreak = FALSE;
}

int GetThreadMemoryModel( struct TDebugThread *obj )
{
    int limit;
    int bitness;
    
    if (obj->Cs == 0x1B3)
        return DEBUG_MEMORY_MODEL_FLAT;

    if (RdosGetSelectorInfo(obj->Cs, &limit, &bitness))
    {
        if (limit == 0xFFFFFFFF)
            return DEBUG_MEMORY_MODEL_FLAT;
        
        if (bitness == 16)    
            return DEBUG_MEMORY_MODEL_16;

        if (bitness == 32)
            return DEBUG_MEMORY_MODEL_32;
    }

    return DEBUG_MEMORY_MODEL_16;
}

int ReadMem( struct TDebugThread *obj, int Sel, long Offset, char *Buf, int Size )
{
    return RdosReadThreadMem( obj->ThreadID, Sel, Offset, Buf, Size );
}

int WriteMem( struct TDebugThread *obj, int Sel, long Offset, char *Buf, int Size )
{
    return RdosWriteThreadMem( obj->ThreadID, Sel, Offset, Buf, Size );
}

static void SetupGo( struct TDebugThread *obj )
{
    int         update = FALSE;
    Tss         tss;
    opcode_type brk_opcode = 0;

    obj->FDebug = FALSE;
    obj->FWasTrace = FALSE;

    RdosGetThreadTss( obj->ThreadID, &tss );
    RdosReadThreadMem( obj->ThreadID, tss.cs, tss.eip, (char *)&brk_opcode, sizeof( brk_opcode ) );

    if( brk_opcode == BRKPOINT ) {
        tss.eip++;
        update = TRUE;
    }

    if( ( tss.eflags & TRACE_BIT ) != 0 ) {
        tss.eflags &= ~TRACE_BIT;
        update = TRUE;
    }

    if( update ) {
        RdosSetThreadTss( obj->ThreadID, &tss );
    }

}

static void SetupTrace( struct TDebugThread *obj )
{
    int         update = FALSE;
    Tss         tss;
    opcode_type brk_opcode = 0;

    obj->FDebug = FALSE;
    obj->FWasTrace = TRUE;

    RdosGetThreadTss( obj->ThreadID, &tss );

    RdosReadThreadMem( obj->ThreadID, tss.cs, tss.eip, (char *)&brk_opcode, sizeof( brk_opcode ) );

    if( brk_opcode == BRKPOINT ) {
        tss.eip++;
        update = TRUE;
    }

    if( ( tss.eflags & TRACE_BIT ) == 0 ) {
        tss.eflags |= TRACE_BIT;
        update = TRUE;
    }

    if( update ) {
        RdosSetThreadTss( obj->ThreadID, &tss );
    }
}

static void ActivateBreaks( struct TDebugThread *obj, struct TDebugBreak *BreakList, struct TDebugWatch *WatchList )
{
    struct TDebugBreak  *b = BreakList;
    struct TDebugWatch  *w = WatchList;
    opcode_type         brk_opcode = BRKPOINT;
    int                 bnum = 0;

    while( w ) {
        if (bnum < 4) {
            RdosSetWriteDataBreak( obj->ThreadID, bnum, w->Sel, w->Offset, w->Size );
            bnum++;
        }
        w = w->Next;
    }

    while( b ) {
        if ((b->Sel & 0x3) == 0x3) {
            RdosReadThreadMem( obj->ThreadID, b->Sel, b->Offset, (char *)&brk_opcode, sizeof( brk_opcode ) );
            b->Instr = brk_opcode;
            RdosWriteThreadMem( obj->ThreadID, b->Sel, b->Offset, (char *)&brk_opcode, sizeof( brk_opcode ) );
        } else {
            if (bnum < 4) {
                RdosSetCodeBreak( obj->ThreadID, bnum, b->Sel, b->Offset );
                bnum++;
            }
        }
        b = b->Next;
    }
}

static void DeactivateBreaks( struct TDebugThread *obj, struct TDebugBreak *BreakList, struct TDebugWatch *WatchList )
{
    struct TDebugBreak  *b = BreakList;
    struct TDebugWatch  *w = WatchList;
    int                 bnum = 0;
    opcode_type         brk_opcode;

    if( !obj->FWasTrace ) {
        while( w ) {
            if (bnum < 4) {
                RdosClearBreak( obj->ThreadID, bnum );
                bnum++;
            }
            w = w->Next;
        }

        while( b ) {
            if ((b->Sel & 0x3) == 0x3) {
                RdosWriteThreadMem( obj->ThreadID, b->Sel, b->Offset, (char *)&brk_opcode, sizeof( brk_opcode ) );
                b->Instr = brk_opcode;
            } else {
                if (bnum < 4) {
                    RdosClearBreak( obj->ThreadID, bnum );
                    bnum++;
                }
            }
            b = b->Next;
        }
    }
}

static void SetException( struct TDebugThread *obj, struct TExceptionEvent *event )
{
    Tss         tss;
    int         i;
    opcode_type brk_opcode = 0;

    obj->FHasBreak = FALSE;
    obj->FHasTrace = FALSE;
    obj->FHasException = FALSE;

    ReadThreadState( obj );
    RdosGetThreadTss( obj->ThreadID, &tss );

    obj->Cs = event->Cs;
    obj->Eip = event->Eip;

    RdosReadThreadMem( obj->ThreadID, obj->Cs, obj->Eip, (char *)&brk_opcode, sizeof( brk_opcode ) );
        
    if( brk_opcode == BRKPOINT )
        event->Code = 0x80000003;

    switch( event->Code ) {
        case 0x80000003:
            obj->FHasBreak = TRUE;
            break;

        case 0x80000004:
            obj->FHasTrace = TRUE;
            break;

        case 0xC0000005:
            obj->FaultText = "Access violation";
            obj->FHasException = TRUE;
            break;

        case 0xC0000017:
            obj->FaultText = "No memory";
            obj->FHasException = TRUE;
            break;

        case 0xC000001D:
            obj->FaultText = "Illegal instruction";
            obj->FHasException = TRUE;
            break;
            
        case 0xC0000025:
            obj->FaultText = "Noncontinuable exception";
            obj->FHasException = TRUE;
            break;

        case 0xC000008C:
            obj->FaultText = "Array bounds exceeded";
            obj->FHasException = TRUE;
            break;

        case 0xC0000094:
            obj->FaultText = "Integer divide by zero";
            obj->FHasException = TRUE;
            break;

        case 0xC0000095:
            obj->FaultText = "Integer overflow";
            obj->FHasException = TRUE;
            break;

        case 0xC0000096:
            obj->FaultText = "Priviliged instruction";
            obj->FHasException = TRUE;
            break;

        case 0xC00000FD:
            obj->FaultText = "Stack overflow";
            obj->FHasException = TRUE;
            break;

        case 0xC000013A:
            obj->FaultText = "Control-C exit";
            obj->FHasException = TRUE;
            break;

        case 0xC000008D:
            obj->FaultText = "Float denormal operand";
            obj->FHasException = TRUE;
            break;

        case 0xC000008E:
            obj->FaultText = "Float divide by zero";
            obj->FHasException = TRUE;
            break;

        case 0xC000008F:
            obj->FaultText = "Float inexact result";
            obj->FHasException = TRUE;
            break;

        case 0xC0000090:
            obj->FaultText = "Float invalid operation";
            obj->FHasException = TRUE;
            break;

        case 0xC0000091:
            obj->FaultText = "Float overflow";
            obj->FHasException = TRUE;
            break;

        case 0xC0000092:
            obj->FaultText = "Float stack check";
            obj->FHasException = TRUE;
            break;

        case 0xC0000093:
            obj->FaultText = "Float underflow";
            obj->FHasException = TRUE;
            break;

        default:
            obj->FaultText = "Protection fault";
            obj->FHasException = TRUE;
            break;
    }

    obj->Cr3 = tss.cr3;
    obj->Eflags = tss.eflags;
    obj->Eax = tss.eax;
    obj->Ecx = tss.ecx;
    obj->Edx = tss.edx;
    obj->Ebx = tss.ebx;
    obj->Esp = tss.esp;
    obj->Ebp = tss.ebp;
    obj->Esi = tss.esi;
    obj->Edi = tss.edi;
    obj->Es = tss.es;
    obj->Ss = tss.ss;
    obj->Ds = tss.ds;
    obj->Fs = tss.fs;
    obj->Gs = tss.gs;
    obj->Ldt = tss.ldt;

    for( i = 0; i < 4; i++ )
        obj->Dr[i] = tss.dr[i];

    obj->Dr7 = tss.dr7;
    obj->MathControl = tss.MathControl;
    obj->MathStatus = tss.MathStatus;
    obj->MathTag = tss.MathTag;
    obj->MathEip = tss.MathEip;
    obj->MathCs = tss.MathCs;
    obj->MathDataOffs = tss.MathDataOffs;
    obj->MathDataSel = tss.MathDataSel;

    for( i = 0; i < 8; i++ )
        obj->St[i] = tss.st[i];

    obj->FDebug = TRUE;
}

static void SetKernelException( struct TDebugThread *obj, struct TKernelExceptionEvent *event )
{
    Tss tss;
    int i;

    obj->FHasBreak = FALSE;
    obj->FHasTrace = FALSE;
    obj->FHasException = FALSE;

    ReadThreadState( obj );
    RdosGetThreadTss( obj->ThreadID, &tss );

    switch ( event->Vector ) {
        case 0:
            obj->FaultText = "Integer divide by zero";
            obj->FHasException = TRUE;
            break;

        case 1:
            obj->FaultText = "Hardware breakpoint";
            obj->FHasTrace = TRUE;
            break;

        case 3:
            obj->FaultText = "Software breakpoint";
            obj->FHasException = TRUE;
            break;

        case 4:
            obj->FaultText = "Integer overflow";
            obj->FHasException = TRUE;
            break;

        case 5:
            obj->FaultText = "Array bounds exceeded";
            obj->FHasException = TRUE;
            break;

        case 6:
            obj->FaultText = "Illegal instruction";
            obj->FHasException = TRUE;
            break;

        case 7:
            obj->FaultText = "Float invalid operation";
            obj->FHasException = TRUE;
            break;

        case 10:
            obj->FaultText = "Invalid TSS";
            obj->FHasException = TRUE;
            break;

        case 11:
            obj->FaultText = "Segment not present";
            obj->FHasException = TRUE;
            break;

        case 12:
            obj->FaultText = "Stack overflow";
            obj->FHasException = TRUE;
            break;

        default:
            obj->FaultText = "Protection fault";
            obj->FHasException = TRUE;
            break;
    }

    obj->Cs = tss.cs;
    obj->Eip = tss.eip;
    obj->Cr3 = tss.cr3;
    obj->Eflags = tss.eflags;
    obj->Eax = tss.eax;
    obj->Ecx = tss.ecx;
    obj->Edx = tss.edx;
    obj->Ebx = tss.ebx;
    obj->Esp = tss.esp;
    obj->Ebp = tss.ebp;
    obj->Esi = tss.esi;
    obj->Edi = tss.edi;
    obj->Es = tss.es;
    obj->Ss = tss.ss;
    obj->Ds = tss.ds;
    obj->Fs = tss.fs;
    obj->Gs = tss.gs;
    obj->Ldt = tss.ldt;

    for (i = 0; i < 4; i++)
        obj->Dr[i] = tss.dr[i];

    obj->Dr7 = tss.dr7;
    obj->MathControl = tss.MathControl;
    obj->MathStatus = tss.MathStatus;
    obj->MathTag = tss.MathTag;
    obj->MathEip = tss.MathEip;
    obj->MathCs = tss.MathCs;
    obj->MathDataOffs = tss.MathDataOffs;
    obj->MathDataSel = tss.MathDataSel;

    for (i = 0; i < 8; i++)
        obj->St[i] = tss.st[i];

    obj->FDebug = TRUE;
}

void WriteRegs( struct TDebugThread *obj )
{
    Tss tss;
    int i;

    RdosGetThreadTss( obj->ThreadID, &tss );

    tss.eflags = obj->Eflags;
    tss.eax = obj->Eax;
    tss.ecx = obj->Ecx;
    tss.edx = obj->Edx;
    tss.ebx = obj->Ebx;
    tss.esp = obj->Esp;
    tss.ebp = obj->Ebp;
    tss.esi = obj->Esi;
    tss.edi = obj->Edi;
    tss.es = obj->Es;
    tss.ss = obj->Ss;
    tss.ds = obj->Ds;
    tss.fs = obj->Fs;
    tss.gs = obj->Gs;

    tss.MathControl = obj->MathControl;
    tss.MathStatus = obj->MathStatus;
    tss.MathTag = obj->MathTag;

    for( i = 0; i < 8; i++ )
        tss.st[i] = obj->St[i];

    RdosSetThreadTss( obj->ThreadID, &tss );
}

static void ReadModuleName( struct TDebugModule *obj )
{
    char str[256];
    int size;

    size = RdosGetModuleName( obj->Handle, str, 255 );
    str[size] = 0;

    obj->ModuleName = malloc( strlen( str ) + 1 );
    strcpy( obj->ModuleName, str );
}

static void InitProcessDebugModule( struct TDebugModule *obj, struct TCreateProcessEvent *event )
{
    obj->FileHandle = event->FileHandle;
    obj->Handle = event->Handle;
    obj->ImageBase = event->ImageBase;
    obj->ImageSize = event->ImageSize;
    obj->ObjectRva = event->ObjectRva;
    obj->CodeSel = 0;
    obj->DataSel = 0;

    obj->FNew = FALSE;

    if ( SelfKey )
        DebugKey = RdosGetModuleFocusKey( event->Handle );

    ReadModuleName( obj );
}

static void InitDllDebugModule( struct TDebugModule *obj, struct TLoadDllEvent *event )
{
    obj->FileHandle = event->FileHandle;
    obj->Handle = event->Handle;
    obj->ImageBase = event->ImageBase;
    obj->ImageSize = event->ImageSize;
    obj->ObjectRva = event->ObjectRva;
    obj->CodeSel = 0;
    obj->DataSel = 0;

    obj->FNew = TRUE;

    ReadModuleName( obj );
}

static void InitKernelDebugModule( struct TDebugModule *obj, int Cs )
{
    obj->FileHandle = 0;

    if (RdosGetDeviceInfo(Cs, obj->ModuleName, &obj->ImageSize, &obj->DataSel, &obj->DataSize)) 
    {
        obj->Handle = 0x8000 | Cs;
        obj->ImageBase = 0;
        obj->ObjectRva = 0;
        obj->CodeSel = Cs;

        if (Cs == 0x30)
            strcpy(obj->ModuleName, "\\rdos\\kernel\\os\\kernel.exe");
    }
    else
        obj->Handle = 0;
        
    obj->FNew = TRUE;
}

static void FreeDebugModule( struct TDebugModule *obj )
{
    free( obj->ModuleName );
}

static void InitDebugBreak( struct TDebugBreak *obj, int sel, long offset, int Hw )
{
    obj->Sel = sel;
    obj->Offset = offset;
    obj->Instr = '\xCC';
    obj->Next = 0;
    obj->UseHw = Hw;
}

static void InitDebugWatch( struct TDebugWatch *obj, int sel, long offset, int size )
{
    obj->Sel = sel;
    obj->Offset = offset;
    obj->Size = size;
    obj->Next = 0;
}

void InitDebug( struct TDebug *obj, const char *Program, const char *Param, const char *StartDir )
{
    obj->FProgram = malloc( strlen( Program ) + 1 );
    strcpy( obj->FProgram, Program );

    obj->FParam = malloc( strlen( Param ) + 1 );
    strcpy( obj->FParam, Param );

    obj->FStartDir = malloc( strlen( StartDir ) + 1 );
    strcpy( obj->FStartDir, StartDir );

    obj->UserWait = RdosCreateWait();
    obj->UserSignal = RdosCreateSignal();
    RdosAddWaitForSignal( obj->UserWait, obj->UserSignal, obj );
        
    obj->FSection = RdosCreateSection();    

    obj->ThreadList = 0;
    obj->ModuleList = 0;
    obj->CurrentThread = 0;
    obj->NewThread = 0;
    obj->BreakList = 0;
    obj->WatchList = 0;

    obj->FThreadChanged = FALSE;
    obj->FModuleChanged = FALSE;
    obj->FHandle = 0;

    obj->FMemoryModel = DEBUG_MEMORY_MODEL_FLAT;
    obj->FConfigChange = FALSE;
    
    obj->FAsyncBreak = FALSE;
    obj->FAsyncSel = 0;
    obj->FAsyncOffset = 0;

    obj->FWaitLoad = TRUE;

    RdosCreateThread( DebugThread, "Debug device", obj, 0x4000 );
}

void FreeDebug( struct TDebug *obj )
{
    free( obj->FProgram );
    free( obj->FParam );
    free( obj->FStartDir );

    RdosCloseWait( obj->UserWait );
    RdosFreeSignal( obj->UserSignal );
    RdosDeleteSection( obj->FSection );
}

static void InsertThread( struct TDebug *obj, struct TDebugThread *thread )
{
    struct TDebugThread *t;

    RdosEnterSection( obj->FSection );

    thread->Next = 0;

    t = obj->ThreadList;
    if( t ) {
        while( t->Next )
            t = t->Next;

        t->Next = thread;            
    }
    else
        obj->ThreadList = thread;

    if( !obj->CurrentThread )
        obj->CurrentThread = thread;

    RdosLeaveSection( obj->FSection );
}

static struct TDebugModule *FindModule( struct TDebug *obj, int Cs )
{
    struct TDebugModule *m;

    RdosEnterSection( obj->FSection );

    m = obj->ModuleList;

    while (m) {
        if ( m->CodeSel == Cs )
            break;

        m = m->Next;
    }

    RdosLeaveSection( obj->FSection );

    return m;
}

static void InsertModule( struct TDebug *obj, struct TDebugModule *mod )
{
    struct TDebugModule *m;

    RdosEnterSection( obj->FSection );

    mod->Next = 0;

    m = obj->ModuleList;
    if( m ) {
        while( m->Next )
            m = m->Next;

        m->Next = mod;            
    }
    else
        obj->ModuleList = mod;

    RdosLeaveSection( obj->FSection );
}

static void RemoveThread( struct TDebug *obj, int thread )
{
    struct TDebugThread *p;
    struct TDebugThread *t;

    RdosEnterSection( obj->FSection );

    p = 0;
    t = obj->ThreadList;

    while( t ) {
        if( t->ThreadID == thread ) {
            if( p )
                p->Next = t->Next;
            else
                obj->ThreadList = t->Next;
            break;            
        }
        else {
            p = t;
            t = t->Next;
        }
    }

    if( t ) {
        if( t == obj->CurrentThread ) {
            obj->CurrentThread = 0;
            RdosLeaveSection( obj->FSection );

            RdosWaitMilli( 25 );

            RdosEnterSection( obj->FSection );
        }
        free( t );
    }

    RdosLeaveSection( obj->FSection );
}

static void RemoveModule( struct TDebug *obj, int module )
{
    struct TDebugModule *p;
    struct TDebugModule *m;

    RdosEnterSection( obj->FSection );

    p = 0;
    m = obj->ModuleList;

    while( m ) {
        if( m->Handle == module ) {
            if( p )
                p->Next = m->Next;
            else
                obj->ModuleList = m->Next;
            break;            
        }
        else {
            p = m;
            m = m->Next;
        }
    }

    if( m )
        free( m );

    RdosLeaveSection( obj->FSection );
}

void WaitForLoad( struct TDebug *obj )
{
    RdosWaitForever( obj->UserWait );
}

int HasThreadChange( struct TDebug *obj )
{
    return obj->FThreadChanged || obj->NewThread;
}

void ClearThreadChange( struct TDebug *obj )
{
    obj->FThreadChanged = FALSE;
    obj->NewThread = 0;
}

int HasModuleChange( struct TDebug *obj )
{
    return obj->FModuleChanged;
}

void ClearModuleChange( struct TDebug *obj )
{
    obj->FModuleChanged = FALSE;
}

int HasConfigChange( struct TDebug *obj )
{
    return obj->FConfigChange;
}

void ClearConfigChange( struct TDebug *obj )
{
    obj->FConfigChange = FALSE;
}

struct TDebugThread *GetMainThread( struct TDebug *obj )
{
    return obj->ThreadList;
}

struct TDebugModule *GetMainModule( struct TDebug *obj )
{
    return obj->ModuleList;
}

struct TDebugThread *GetCurrentThread( struct TDebug *obj )
{
    return obj->CurrentThread;
}

struct TDebugThread *GetNewThread( struct TDebug *obj )
{
    return obj->NewThread;
}

void SetCurrentThread( struct TDebug *obj, int ThreadID )
{
    struct TDebugThread *t;

    RdosEnterSection( obj->FSection );

    t = obj->ThreadList;
    while (t && t->ThreadID != ThreadID)
        t = t->Next;

    if (t)
        obj->CurrentThread = t;

    RdosLeaveSection( obj->FSection );
}

int GetNextThread( struct TDebug *obj, int ThreadID)
{
    int ID = 0xFFFF;
    struct TDebugThread *t;

    RdosEnterSection( obj->FSection );

    t = obj->ThreadList;
    while (t)
    {
        if (t->ThreadID > ThreadID && t->ThreadID < ID)
            ID = t->ThreadID;

        t = t->Next;            
    }

    RdosLeaveSection( obj->FSection );

    if (ID != 0xFFFF)
        return ID;
    else
        return 0;
}

int GetNextModule( struct TDebug *obj, int ModuleHandle )
{
    int Handle = 0xFFFF;
    struct TDebugModule *m;
    struct TDebugModule *rm = 0;

    RdosEnterSection( obj->FSection );

    m = obj->ModuleList;
    while (m)
    {
        if (m->FNew && m->Handle > ModuleHandle && m->Handle < Handle)
        {
            rm = m;
            Handle = m->Handle;
        }

        m = m->Next;            
    }

    RdosLeaveSection( obj->FSection );

    if (rm)
    {
        rm->FNew = FALSE;
        return Handle;
    }
    else
        return 0;
}

int GetMemoryModel( struct TDebug *obj )
{
    return obj->FMemoryModel;
}

struct TDebugThread *LockThread( struct TDebug *obj, int ThreadID )
{
    struct TDebugThread *t;

    RdosEnterSection( obj->FSection );

    t = obj->ThreadList;
    while( t && t->ThreadID != ThreadID )
        t = t->Next;            

    return t;
}

void UnlockThread( struct TDebug *obj )
{
    RdosLeaveSection( obj->FSection );
}

struct TDebugModule *LockModule( struct TDebug *obj, int Handle )
{
    struct TDebugModule *m;

    RdosEnterSection( obj->FSection );

    m = obj->ModuleList;
    while (m && m->Handle != Handle)
        m = m->Next;            

    return m;
}

void UnlockModule( struct TDebug *obj )
{
    RdosLeaveSection( obj->FSection );
}

int HasModule( struct TDebug *obj, const char *Name )
{
    struct TDebugModule *m;
    int found = FALSE;

    RdosEnterSection( obj->FSection );

    m = obj->ModuleList;
    while (m && !found)
    {
        if ( !strcmp( Name, m->ModuleName ) )
            found = TRUE;
        m = m->Next;    
    }        
    RdosLeaveSection( obj->FSection );

    return found;
}

void UpdateModules( struct TDebug *obj )
{
    struct TDebugModule *m;
    int model;

    model = GetThreadMemoryModel( obj->CurrentThread );                    
    if (model != obj->FMemoryModel) {
        obj->FMemoryModel = model;
        obj->FConfigChange = TRUE;
    }
    
    if ( obj->FMemoryModel != DEBUG_MEMORY_MODEL_FLAT ) {
        if ( !FindModule( obj, obj->CurrentThread->Cs ) ) {
            m = (struct TDebugModule *)malloc( sizeof( struct TDebugModule ) );
            InitKernelDebugModule( m, obj->CurrentThread->Cs );
            if (m->Handle) {
                InsertModule( obj, m );
                obj->FModuleChanged = TRUE;
            }
            else
                free( m );
        }
    }
}

int IsBreak( struct TDebug *obj, int Sel, long Offset )
{
    struct TDebugBreak *b;
    int ok = FALSE;
    
    RdosEnterSection( obj->FSection );

    b = obj->BreakList;

    while (b && !ok)
    {
        if (b->Sel == Sel && b->Offset == Offset)
            ok = TRUE;
        else
            b = b->Next;
    }

    RdosLeaveSection( obj->FSection );

    return ok;
}

int IsWatch( struct TDebug *obj, int Sel, long Offset )
{
    struct TDebugWatch *w;
    int ok = FALSE;
    
    RdosEnterSection( obj->FSection );

    w = obj->WatchList;

    while (w && !ok)
    {
        if (w->Sel == Sel && w->Offset == Offset)
            ok = TRUE;
        else
            w = w->Next;
    }

    RdosLeaveSection( obj->FSection );

    return ok;
}

void AddBreak( struct TDebug *obj, int Sel, long Offset, int Hw )
{
    struct TDebugBreak *newbr;
    struct TDebugBreak *b;
    int found = FALSE;

    newbr = (struct TDebugBreak *)malloc( sizeof( struct TDebugBreak ) );
    InitDebugBreak( newbr, Sel, Offset, Hw );    

    RdosEnterSection( obj->FSection );

    newbr->Next = 0;

    b = obj->BreakList;
    if( b ) {
        while( b->Next ) {
            if( b->Sel == Sel && b->Offset == Offset )
                found = TRUE;
            b = b->Next;
        }

        if( !found )
            b->Next = newbr;            
    }
    else
        obj->BreakList = newbr;

    RdosLeaveSection( obj->FSection );
}

void ClearBreak( struct TDebug *obj, int Sel, long Offset )
{
    struct TDebugBreak *b;
    struct TDebugBreak *delbr;
    
    RdosEnterSection( obj->FSection );

    b = obj->BreakList;

    if( b ) {
        if( b->Offset == Offset && b->Sel == Sel ) {
            obj->BreakList = b->Next;
            free( b );
        }
        else
        {
            while( b->Next ) {
                delbr = b->Next;
                
                if( delbr->Offset == Offset && delbr->Sel == Sel ) {
                    b->Next = delbr->Next;
                    free( delbr );
                }
                else
                    b = b->Next;                    
            }
        }
    }

    RdosLeaveSection( obj->FSection );
}

void AddWatch( struct TDebug *obj, int Sel, long Offset, int Size )
{
    struct TDebugWatch *neww;
    struct TDebugWatch *w;
    int found = FALSE;

    neww = (struct TDebugWatch *)malloc( sizeof( struct TDebugWatch ) );
    InitDebugWatch( neww, Sel, Offset, Size );    

    RdosEnterSection( obj->FSection );

    neww->Next = 0;

    w = obj->WatchList;
    if( w ) {
        while( w->Next ) {
            if( w->Sel == Sel && w->Offset == Offset )
                found = TRUE;
            w = w->Next;
        }

        if( !found )
            w->Next = neww;            
    }
    else
        obj->WatchList = neww;

    RdosLeaveSection( obj->FSection );
}

void ClearWatch( struct TDebug *obj, int Sel, long Offset, int Size )
{
    struct TDebugWatch *w;
    struct TDebugWatch *delw;
    
    RdosEnterSection( obj->FSection );

    w = obj->WatchList;

    if( w ) {
        if( w->Offset == Offset && w->Sel == Sel ) {
            obj->WatchList = w->Next;
            free( w );
        }
        else
        {
            while( w->Next ) {
                delw = w->Next;
                
                if( delw->Offset == Offset && delw->Sel == Sel ) {
                    w->Next = delw->Next;
                    free( delw );
                }
                else
                    w = w->Next;                    
            }
        }
    }

    RdosLeaveSection( obj->FSection );
}

void DoTrace( struct TDebug *obj )
{
    if ( ( obj->CurrentThread->Cs & 0x3 ) == 0x3 ) {
        SetupTrace( obj->CurrentThread );
        RdosContinueDebugEvent( obj->FHandle, obj->CurrentThread->ThreadID );
    } else {
        while ( RdosGetDebugThread() != obj->CurrentThread->ThreadID )
            RdosDebugNext();
        RdosDebugTrace();
    }
}

void DoGo( struct TDebug *obj )
{
    if ( ( obj->CurrentThread->Cs & 0x3 ) == 0x3 ) {
        SetupGo( obj->CurrentThread );
        ActivateBreaks( obj->CurrentThread, obj->BreakList, obj->WatchList );
        RdosContinueDebugEvent( obj->FHandle, obj->CurrentThread->ThreadID );
    } else {
        while ( RdosGetDebugThread() != obj->CurrentThread->ThreadID )
            RdosDebugNext();
        ActivateBreaks( obj->CurrentThread, obj->BreakList, obj->WatchList );
        RdosDebugRun();
    }
}

void SetUserScreen()
{
    if( DebugKey )
        RdosSetFocus( DebugKey );
}

void SetDebugScreen()
{
    if( SelfKey )
        RdosSetFocus( SelfKey );
}

void Go( struct TDebug *obj )
{
    if( obj->CurrentThread ) {
        RdosResetSignal( obj->UserSignal );
        DoGo( obj );
        RdosWaitForever( obj->UserWait );
    }
}

void Trace( struct TDebug *obj )
{
    char Instr[2] = {0, 0};
    int Sel;
    long Offset;

    if( obj->CurrentThread ) {
        Sel = obj->CurrentThread->Cs;
        Offset = obj->CurrentThread->Eip;

        ReadMem( obj->CurrentThread, Sel, Offset, Instr, 2 );

        if ( Instr[0] == 0xF && Instr[1] == 0xB ) {
            Offset += 7;
            AddBreak( obj, Sel, Offset, TRUE );
            Go( obj );
            ClearBreak( obj, Sel, Offset );
        } else {
            RdosResetSignal( obj->UserSignal );
            DoTrace( obj );
            RdosWaitForever( obj->UserWait );
        }
    }
}

int AsyncGo( struct TDebug *obj, int ms )
{
    void *wait;
    
    if( obj->CurrentThread ) {
        RdosResetSignal( obj->UserSignal );
        DoGo( obj );

        wait = RdosWaitTimeout( obj->UserWait, ms );
        if ( wait ) {
            return( TRUE );
        }
        else
            return( FALSE );
    }
    return( TRUE );
}

int AsyncTrace( struct TDebug *obj, int ms )
{
    char    Instr[2] = {0, 0};
    int     ok = TRUE;
    void    *wait;

    if( obj->CurrentThread ) {
        BreakSel = obj->CurrentThread->Cs;
        BreakOffset = obj->CurrentThread->Eip;

        ReadMem( obj->CurrentThread, BreakSel, BreakOffset, Instr, 2 );

        if ( Instr[0] == 0xF && Instr[1] == 0xB ) {
            BreakOffset += 7;
            AddBreak( obj, BreakSel, BreakOffset, TRUE );
            ok = AsyncGo( obj, ms );
            if( ok ) {
                ClearBreak( obj, BreakSel, BreakOffset );
            } else {
                HasBreak = TRUE;
            }
            return( ok );
        } else {
            RdosResetSignal( obj->UserSignal );
            DoTrace( obj );

            wait = RdosWaitTimeout( obj->UserWait, ms );
            if( wait ) {
                return( TRUE );
            }
            else
                return( FALSE );
        }
    }
    return( TRUE );
}

int AsyncPoll( struct TDebug *obj, int ms )
{
    void    *wait;

    wait = RdosWaitTimeout( obj->UserWait, ms );
    if (wait) {
        if( HasBreak ) {
            ClearBreak( obj, BreakSel, BreakOffset );
            HasBreak = FALSE;
        }
        return( TRUE );
    } else {
        return( FALSE );
    }
}

void ExitAsync( struct TDebug *obj )
{
    if ( obj->FAsyncBreak ) {
        ClearBreak( obj, obj->FAsyncSel, obj->FAsyncOffset );
        obj->FAsyncBreak = FALSE;
    }
}

static void HandleCreateProcess( struct TDebug *obj, struct TCreateProcessEvent *event )
{
    struct TDebugThread *dt;
    struct TDebugModule *dm;

    dt = (struct TDebugThread *)malloc( sizeof( struct TDebugThread ) );
    InitProcessDebugThread( dt, event );
    InsertThread( obj, dt );

    dm = (struct TDebugModule *)malloc( sizeof( struct TDebugModule ) );
    InitProcessDebugModule( dm, event );
    InsertModule( obj, dm);       
}

static void HandleTerminateProcess( struct TDebug *obj, int exitcode )
{
    struct TDebugThread *t;
    struct TDebugModule *m;

    RdosEnterSection( obj->FSection );

    while( obj->ThreadList ) {
        t = obj->ThreadList->Next;
        FreeDebugThread( obj->ThreadList );
        free( obj->ThreadList );
        obj->ThreadList = t;
    }

    while( obj->ModuleList ) {
        m = obj->ModuleList->Next;
        FreeDebugModule( obj->ModuleList );
        free( obj->ModuleList );
        obj->ModuleList = m;
    }

    obj->CurrentThread = 0;
    obj->NewThread = 0;
    obj->FThreadChanged = TRUE;
    obj->FModuleChanged = TRUE;

    RdosLeaveSection( obj->FSection );
}

static void HandleCreateThread( struct TDebug *obj, struct TCreateThreadEvent *event )
{
    struct TDebugThread *dt;

    dt = (struct TDebugThread *)malloc( sizeof( struct TDebugThread ) );
    InitThreadDebugThread( dt, event );
    InsertThread( obj, dt );
}

static void HandleTerminateThread( struct TDebug *obj, int thread )
{
    RemoveThread( obj, thread );
}

static void HandleException( struct TDebug *obj, struct TExceptionEvent *event, int thread )
{
    struct TDebugThread *Thread;

    RdosEnterSection( obj->FSection );

    Thread = obj->ThreadList;

    while( Thread && Thread->ThreadID != thread )
        Thread = Thread->Next;

    if( Thread ) {
        SetException( Thread, event );

        if ( obj->FWaitLoad )
            ClearThreadBreak( Thread );
        obj->FWaitLoad = FALSE;
    }

    RdosLeaveSection( obj->FSection );
}

static void HandleLoadDll( struct TDebug *obj, struct TLoadDllEvent *event )
{
    struct TDebugModule *dm;

    dm = (struct TDebugModule *)malloc( sizeof( struct TDebugModule ) );
    InitDllDebugModule( dm, event );
    InsertModule( obj, dm);       
}

static void HandleFreeDll( struct TDebug *obj, int module )
{
    RemoveModule( obj, module );
}

void HandleKernelException( struct TDebug *obj, struct TKernelExceptionEvent *event, int thread )
{
    struct TDebugThread *Thread;

    RdosEnterSection( obj->FSection );

    Thread = obj->ThreadList;

    while ( Thread && Thread->ThreadID != thread )
        Thread = Thread->Next;

    if (Thread)
        SetKernelException( Thread, event );

    RdosLeaveSection( obj->FSection );
}

static void SignalDebugData( struct TDebug *obj )
{
    int thread;
    char debtype;
    struct TCreateProcessEvent cpe;
    struct TCreateThreadEvent cte;
    struct TLoadDllEvent lde;
    struct TExceptionEvent ee;
    struct TKernelExceptionEvent kev;
    int ExitCode;
    int handle;
    struct TDebugThread *newt;

    RdosWaitMilli( 5 );

    debtype = RdosGetDebugEvent( obj->FHandle, &thread );

    switch (debtype) {
        case EVENT_EXCEPTION:
            RdosGetDebugEventData( obj->FHandle, &ee );
            HandleException( obj, &ee, thread );
            break;

        case EVENT_CREATE_THREAD:
            RdosGetDebugEventData( obj->FHandle, &cte );
            HandleCreateThread( obj, &cte );
            obj->FThreadChanged = TRUE;
            break;

        case EVENT_CREATE_PROCESS:
            RdosGetDebugEventData( obj->FHandle, &cpe );
            HandleCreateProcess( obj, &cpe );
            break;

        case EVENT_TERMINATE_THREAD:
            HandleTerminateThread( obj, thread );
            obj->FThreadChanged = TRUE;
            if( obj->CurrentThread->ThreadID == thread )
                obj->CurrentThread = 0;
            break;

        case EVENT_TERMINATE_PROCESS:
            RdosGetDebugEventData( obj->FHandle, &ExitCode );
            HandleTerminateProcess( obj, ExitCode);
            obj->FInstalled = FALSE;
            RdosSetSignal( obj->UserSignal );
            break;

        case EVENT_LOAD_DLL:
            RdosGetDebugEventData( obj->FHandle, &lde );
            HandleLoadDll( obj, &lde );
            obj->FModuleChanged = TRUE;
            break;

        case EVENT_FREE_DLL:
            RdosGetDebugEventData( obj->FHandle, &handle );
            HandleFreeDll( obj, handle );
            obj->FModuleChanged = TRUE;
            break;

        case EVENT_KERNEL:
            RdosGetDebugEventData( obj->FHandle, &kev );
            HandleKernelException( obj, &kev, thread );
            break;                    
    }

    RdosClearDebugEvent( obj->FHandle );

    if( debtype == EVENT_EXCEPTION || debtype == EVENT_KERNEL ) {
        if( obj->CurrentThread ) {
            DeactivateBreaks( obj->CurrentThread, obj->BreakList, obj->WatchList );

            if( thread != obj->CurrentThread->ThreadID ) {
                newt = LockThread( obj, thread );
                if( newt )
                    obj->NewThread = newt;
                UnlockThread( obj );
            }
            UpdateModules( obj );
        }

        RdosSetSignal( obj->UserSignal );
    }
    else
        RdosContinueDebugEvent( obj->FHandle, thread );
}

static void DebugThread( void *Param )
{
    int CurrModuleHandle;
    int WaitHandle;
    int thread;
    struct TDebug *obj = (struct TDebug *)Param;

    obj->FInstalled = TRUE;

    CurrModuleHandle = RdosGetModuleHandle();
    SelfKey = RdosGetModuleFocusKey( CurrModuleHandle );    
    if ( SelfKey != RdosGetFocus() )
        SelfKey = 0;
        
    RdosWaitMilli( 250 );

    obj->FHandle = RdosSpawnDebug( obj->FProgram, obj->FParam, obj->FStartDir, 0, 0, &thread);
        
    RdosWaitMilli( 250 );

    if( obj->FHandle ) {
        WaitHandle = RdosCreateWait();
        RdosAddWaitForDebugEvent( WaitHandle, obj->FHandle, obj );
        
        while( obj->FInstalled )
            if( RdosWaitForever( WaitHandle ) )
                SignalDebugData( obj );

        RdosCloseWait( WaitHandle );
    } else
        obj->FInstalled = FALSE;
}

int IsTerminated( struct TDebug *obj )
{
    return !obj->FInstalled;
}

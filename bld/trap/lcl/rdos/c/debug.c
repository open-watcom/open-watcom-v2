/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
#include <i86.h>
#include "bool.h"
#include "rdos.h"
#include "debug.h"
#include "brkptcpu.h"


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

#define MAX_DEBUG_THREADS       64

static void DebugThread( void *Param );

static struct TDebug    *DebugArr[MAX_DEBUG_THREADS];
static int              ThreadArr[MAX_DEBUG_THREADS];
static char             SelfKey = 0;
static char             DebugKey = 0;

struct TDebug *GetCurrentDebug( void )
{
    int i;
    int handle = RdosGetThreadHandle();

    for( i = 0; i < MAX_DEBUG_THREADS; i++ ) {
        if( ThreadArr[i] == handle ) {
            return( DebugArr[i] );
        }
    }
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

    ok = false;

    for( i = 0; i < 256 && !ok; i++ ) {
        RdosGetThreadState( i, &state );
        if( state.ID == obj->ThreadID ) {
            ok = true;
        }
    }

    if( ok ) {
        strncpy( str, state.Name, sizeof( str ) - 1 );
        str[sizeof( str ) - 1] = 0;

        for( i = sizeof( str ) - 2; i >= 0; i-- ) {
            if( str[i] == ' ' ) {
                str[i] = 0;
            } else {
                break;
            }
        }
        if( obj->ThreadName )
            free( obj->ThreadName );

        obj->ThreadName = malloc( strlen( str ) + 1 );
        strcpy( obj->ThreadName, str );

        strncpy( str, state.List, sizeof( str ) - 1 );
        str[sizeof( str ) - 1] = 0;

        for( i = sizeof( str ) - 2; i >= 0; i-- ) {
            if( str[i] == ' ' ) {
                str[i] = 0;
            } else {
                break;
            }
        }
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

    obj->FDebug = false;
    obj->FWasTrace = false;

    obj->FHasBreak = false;
    obj->FHasTrace = false;
    obj->FHasException = false;
    obj->FHasTempBp = 0;
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
    obj->FHasBreak = false;
}

static int GetThreadMemoryModel( struct TDebugThread *obj )
{
    int limit;
    int bitness;

    if( obj->Cs == 0x1B3 )
        return DEBUG_MEMORY_MODEL_FLAT;

    if( RdosGetSelectorInfo( obj->Cs, &limit, &bitness ) ) {
        if( limit == 0xFFFFFFFF )
            return DEBUG_MEMORY_MODEL_FLAT;

        if( bitness == 16 )
            return DEBUG_MEMORY_MODEL_16;

        if( bitness == 32 ) {
            return DEBUG_MEMORY_MODEL_32;
        }
    }

    return DEBUG_MEMORY_MODEL_16;
}

static void SetupGo( struct TDebugThread *obj, struct TDebugBreak *b )
{
    int         update = false;
    Tss         tss;

    obj->FDebug = false;
    obj->FWasTrace = false;

    RdosGetThreadTss( obj->ThreadID, &tss );

    if( b ) {
        obj->FHasTempBp = true;
        tss.eflags |= 0x10000;
        update = true;
    } else {
        obj->FHasTempBp = false;
    }
    if( tss.eflags & INTR_TF ) {
        tss.eflags &= ~INTR_TF;
        update = true;
    }

    if( update ) {
        RdosSetThreadTss( obj->ThreadID, &tss );
    }

    if( b ) {
        RdosSetCodeBreak(obj->ThreadID, 0, b->Sel, b->Offset);
    }
}

static void SetupTrace( struct TDebugThread *obj, struct TDebugBreak *b )
{
    int         update = false;
    Tss         tss;

    obj->FDebug = false;
    obj->FWasTrace = true;

    RdosGetThreadTss( obj->ThreadID, &tss );

    if( b ) {
        obj->FHasTempBp = true;
        tss.eflags |= 0x10000;
        update = true;
    } else {
        obj->FHasTempBp = false;
    }
    if( ( tss.eflags & INTR_TF ) == 0 ) {
        tss.eflags |= INTR_TF;
        update = true;
    }

    if( update ) {
        RdosSetThreadTss( obj->ThreadID, &tss );
    }

    if( b ) {
        RdosSetCodeBreak(obj->ThreadID, 0, b->Sel, b->Offset);
    }
}

static void ActivateBreaks( struct TDebugThread *obj, struct TDebugBreak *HwBreakList, struct TDebugWatch *WatchList )
{
    struct TDebugBreak  *b;
    struct TDebugWatch  *w;
    int                 bnum;

    if( obj->FHasTempBp ) {
        bnum = 1;
    } else {
        bnum = 0;
    }
    for( b = HwBreakList; b != NULL; b = b->Next ) {
        if( bnum < 4 ) {
            RdosSetCodeBreak( obj->ThreadID, bnum, b->Sel, b->Offset );
            bnum++;
        }
    }

    for( w = WatchList; w != NULL; w = w->Next ) {
        if( bnum < 4 ) {
            RdosSetWriteDataBreak( obj->ThreadID, bnum, w->Sel, w->Offset, w->Size );
            bnum++;
        }
    }
}

static void SetException( struct TDebugThread *obj, struct TExceptionEvent *event )
{
    Tss         tss;
    int         i;
    opcode_type brk_opcode = 0;
    int         bnum;

    for( bnum = 0; bnum < 4; bnum++ ) {
        RdosClearBreak( obj->ThreadID, bnum );
    }

    obj->FHasBreak = false;
    obj->FHasTrace = false;
    obj->FHasException = false;

    ReadThreadState( obj );
    RdosGetThreadTss( obj->ThreadID, &tss );

    obj->Cs = event->Cs;
    obj->Eip = event->Eip;

    RdosReadThreadMem( obj->ThreadID, obj->Cs, obj->Eip, (char *)&brk_opcode, sizeof( brk_opcode ) );

    if( brk_opcode == BRKPOINT )
        event->Code = 0x80000003;

    switch( event->Code ) {
        case 0x80000003:
            obj->FHasBreak = true;
            break;

        case 0x80000004:
            obj->FHasTrace = true;
            break;

        case 0xC0000005:
            obj->FaultText = "Access violation";
            obj->FHasException = true;
            break;

        case 0xC0000017:
            obj->FaultText = "No memory";
            obj->FHasException = true;
            break;

        case 0xC000001D:
            obj->FaultText = "Illegal instruction";
            obj->FHasException = true;
            break;

        case 0xC0000025:
            obj->FaultText = "Noncontinuable exception";
            obj->FHasException = true;
            break;

        case 0xC000008C:
            obj->FaultText = "Array bounds exceeded";
            obj->FHasException = true;
            break;

        case 0xC0000094:
            obj->FaultText = "Integer divide by zero";
            obj->FHasException = true;
            break;

        case 0xC0000095:
            obj->FaultText = "Integer overflow";
            obj->FHasException = true;
            break;

        case 0xC0000096:
            obj->FaultText = "Priviliged instruction";
            obj->FHasException = true;
            break;

        case 0xC00000FD:
            obj->FaultText = "Stack overflow";
            obj->FHasException = true;
            break;

        case 0xC000013A:
            obj->FaultText = "Control-C exit";
            obj->FHasException = true;
            break;

        case 0xC000008D:
            obj->FaultText = "Float denormal operand";
            obj->FHasException = true;
            break;

        case 0xC000008E:
            obj->FaultText = "Float divide by zero";
            obj->FHasException = true;
            break;

        case 0xC000008F:
            obj->FaultText = "Float inexact result";
            obj->FHasException = true;
            break;

        case 0xC0000090:
            obj->FaultText = "Float invalid operation";
            obj->FHasException = true;
            break;

        case 0xC0000091:
            obj->FaultText = "Float overflow";
            obj->FHasException = true;
            break;

        case 0xC0000092:
            obj->FaultText = "Float stack check";
            obj->FHasException = true;
            break;

        case 0xC0000093:
            obj->FaultText = "Float underflow";
            obj->FHasException = true;
            break;

        default:
            obj->FaultText = "Protection fault";
            obj->FHasException = true;
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

    obj->FDebug = true;
}

static void SetKernelException( struct TDebugThread *obj, struct TKernelExceptionEvent *event )
{
    Tss tss;
    int i;
    int         bnum;

    for( bnum = 0; bnum < 4; bnum++ ) {
        RdosClearBreak( obj->ThreadID, bnum );
    }

    obj->FHasBreak = false;
    obj->FHasTrace = false;
    obj->FHasException = false;

    ReadThreadState( obj );
    RdosGetThreadTss( obj->ThreadID, &tss );

    switch ( event->Vector ) {
        case 0:
            obj->FaultText = "Integer divide by zero";
            obj->FHasException = true;
            break;

        case 1:
            obj->FaultText = "Hardware breakpoint";
            obj->FHasTrace = true;
            break;

        case 3:
            obj->FaultText = "Software breakpoint";
            obj->FHasException = true;
            break;

        case 4:
            obj->FaultText = "Integer overflow";
            obj->FHasException = true;
            break;

        case 5:
            obj->FaultText = "Array bounds exceeded";
            obj->FHasException = true;
            break;

        case 6:
            obj->FaultText = "Illegal instruction";
            obj->FHasException = true;
            break;

        case 7:
            obj->FaultText = "Float invalid operation";
            obj->FHasException = true;
            break;

        case 10:
            obj->FaultText = "Invalid TSS";
            obj->FHasException = true;
            break;

        case 11:
            obj->FaultText = "Segment not present";
            obj->FHasException = true;
            break;

        case 12:
            obj->FaultText = "Stack overflow";
            obj->FHasException = true;
            break;

        default:
            obj->FaultText = "Protection fault";
            obj->FHasException = true;
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

    obj->FDebug = true;
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

    obj->FNew = false;

    if( SelfKey )
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

    obj->FNew = true;

    ReadModuleName( obj );
}

static void InitKernelDebugModule( struct TDebugModule *obj, int Cs )
{
    obj->FileHandle = 0;
    obj->ModuleName = malloc( 257 );

    if( RdosGetDeviceInfo( Cs, obj->ModuleName, &obj->ImageSize, &obj->DataSel, &obj->DataSize ) ) {
        obj->Handle = 0x8000 | Cs;
        obj->ImageBase = 0;
        obj->ObjectRva = 0;
        obj->CodeSel = Cs;

        if( Cs == 0x30 ) {
            strcpy(obj->ModuleName, "\\rdos\\kernel\\os\\kernel.exe");
        }
    } else {
        obj->Handle = 0;
    }
    obj->FNew = true;
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
    obj->IsActive = false;
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
    RdosAddWaitForSignal( obj->UserWait, obj->UserSignal, (int)obj );

    obj->FSection = RdosCreateSection( "Watcom.Debug" );

    obj->ThreadList = 0;
    obj->ModuleList = 0;
    obj->CurrentThread = 0;
    obj->HwBreakList = 0;
    obj->SwBreakList = 0;
    obj->WatchList = 0;

    obj->FThreadChanged = false;
    obj->FHandle = 0;

    obj->FMemoryModel = DEBUG_MEMORY_MODEL_FLAT;
    obj->FConfigChange = false;

    obj->FWaitLoad = true;

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
    } else {
        obj->ThreadList = thread;
    }
    if( !obj->CurrentThread )
        obj->CurrentThread = thread;

    RdosLeaveSection( obj->FSection );
}

static struct TDebugModule *FindModule( struct TDebug *obj, int Cs )
{
    struct TDebugModule *m;

    RdosEnterSection( obj->FSection );

    for( m = obj->ModuleList; m != NULL; m = m->Next ) {
        if( m->CodeSel == Cs ) {
            break;
        }
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
    } else {
        obj->ModuleList = mod;
    }
    RdosLeaveSection( obj->FSection );
}

static void RemoveThread( struct TDebug *obj, int thread )
{
    struct TDebugThread *p;
    struct TDebugThread *t;

    RdosEnterSection( obj->FSection );

    p = NULL;
    for( t = obj->ThreadList; t != NULL; t = t->Next ) {
        if( t->ThreadID == thread ) {
            if( p != NULL ) {
                p->Next = t->Next;
            } else {
                obj->ThreadList = t->Next;
            }
            if( t == obj->CurrentThread ) {
                obj->CurrentThread = 0;
                RdosLeaveSection( obj->FSection );

                RdosWaitMilli( 25 );

                RdosEnterSection( obj->FSection );
            }
            free( t );
            break;
        }
        p = t;
    }

    RdosLeaveSection( obj->FSection );
}

static void RemoveModule( struct TDebug *obj, int module )
{
    struct TDebugModule *p;
    struct TDebugModule *m;

    RdosEnterSection( obj->FSection );

    p = NULL;
    for( m = obj->ModuleList; m != NULL; m = m->Next ) {
        if( m->Handle == module ) {
            if( p != NULL ) {
                p->Next = m->Next;
            } else {
                obj->ModuleList = m->Next;
            }
            free( m );
            break;
        }
        p = m;
    }

    RdosLeaveSection( obj->FSection );
}

void WaitForLoad( struct TDebug *obj )
{
    RdosWaitForever( obj->UserWait );
}

int HasThreadChange( struct TDebug *obj )
{
    return obj->FThreadChanged;
}

void ClearThreadChange( struct TDebug *obj )
{
    obj->FThreadChanged = false;
}

int HasModuleChange( struct TDebug *obj )
{
    struct TDebugModule *m;
    int change = false;

    RdosEnterSection( obj->FSection );

    for( m = obj->ModuleList; m != NULL; m = m->Next ) {
        if( m->FNew ) {
            change = true;
        }
    }

    RdosLeaveSection( obj->FSection );

    return( change );
}

int HasConfigChange( struct TDebug *obj )
{
    return obj->FConfigChange;
}

void ClearConfigChange( struct TDebug *obj )
{
    obj->FConfigChange = false;
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

int ReadMem( struct TDebug *obj, int Sel, long Offset, void *Buf, int Size )
{
    struct TDebugBreak *b;
    struct TDebugThread *Thread;
    int len;
    long diff;

    Thread = obj->CurrentThread;
    if( !Thread )
        Thread = obj->ThreadList;

    if( Thread ) {
        len = RdosReadThreadMem( Thread->ThreadID, Sel, Offset, Buf, Size );
    } else {
        len = 0;
    }
    if( len && obj->SwBreakList ) {
        RdosEnterSection( obj->FSection );

        for( b = obj->SwBreakList; b != NULL; b = b->Next ) {
            if( b->Sel == Sel && b->IsActive ) {
                diff = b->Offset - Offset;

                if( diff >= 0 && diff < len ) {
                    *(opcode_type *)( (char *)Buf + diff ) = b->Instr;
                }
            }
        }

        RdosLeaveSection( obj->FSection );
    }
    return( len );
}

int WriteMem( struct TDebug *obj, int Sel, long Offset, void *Buf, int Size )
{
    struct TDebugBreak *b;
    struct TDebugThread *Thread;
    long diff;

    Thread = obj->CurrentThread;
    if( !Thread )
        Thread = obj->ThreadList;

    if( Thread ) {
        if( obj->SwBreakList != NULL ) {
            RdosEnterSection( obj->FSection );

            for( b = obj->SwBreakList; b != NULL; b = b->Next ) {
                if( b->Sel == Sel && b->IsActive ) {
                    diff = b->Offset - Offset;

                    if( diff >= 0 && diff < Size ) {
                        b->Instr = *(opcode_type *)( (char *)Buf + diff );
                        *(opcode_type *)( (char *)Buf + diff ) = BRKPOINT;
                    }
                }
            }
            RdosLeaveSection( obj->FSection );
        }
        return( RdosWriteThreadMem( Thread->ThreadID, Sel, Offset, Buf, Size ) );
    }

    return( 0 );
}

void SetCurrentThread( struct TDebug *obj, int ThreadID )
{
    struct TDebugThread *t;

    RdosEnterSection( obj->FSection );

    t = obj->ThreadList;
    while (t && t->ThreadID != ThreadID)
        t = t->Next;

    if( t )
        obj->CurrentThread = t;

    RdosLeaveSection( obj->FSection );
}

int GetNextThread( struct TDebug *obj, int ThreadID)
{
    int ID;
    struct TDebugThread *t;

    RdosEnterSection( obj->FSection );

    ID = 0xFFFF;
    for( t = obj->ThreadList; t != NULL; t = t->Next ) {
        if( t->ThreadID > ThreadID && t->ThreadID < ID ) {
            ID = t->ThreadID;
        }
    }

    RdosLeaveSection( obj->FSection );

    if( ID != 0xFFFF ) {
        return ID;
    } else {
        return 0;
    }
}

int GetNextModule( struct TDebug *obj, int ModuleHandle )
{
    int Handle = 0xFFFF;
    struct TDebugModule *m;
    struct TDebugModule *rm = 0;

    RdosEnterSection( obj->FSection );

    for( m = obj->ModuleList; m != NULL; m = m->Next ) {
        if( m->FNew && m->Handle > ModuleHandle && m->Handle < Handle ) {
            rm = m;
            Handle = m->Handle;
        }
    }

    RdosLeaveSection( obj->FSection );

    if( rm ) {
        rm->FNew = false;
        return Handle;
    } else {
        return 0;
    }
}

#if 0
int GetMemoryModel( struct TDebug *obj )
{
    return obj->FMemoryModel;
}
#endif

struct TDebugThread *LockThread( struct TDebug *obj, int ThreadID )
{
    struct TDebugThread *t;

    RdosEnterSection( obj->FSection );

    for( t = obj->ThreadList; t != NULL; t = t->Next ) {
        if( t->ThreadID == ThreadID ) {
            break;
        }
    }

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

    for( m = obj->ModuleList; m != NULL; m = m->Next ) {
        if( m->Handle == Handle ) {
            break;
        }
    }

    return m;
}

void UnlockModule( struct TDebug *obj )
{
    RdosLeaveSection( obj->FSection );
}

#if 0
int HasModule( struct TDebug *obj, const char *Name )
{
    struct TDebugModule *m;
    int found;

    RdosEnterSection( obj->FSection );

    found = false;
    for( m = obj->ModuleList; m != NULL; m = m->Next ) {
        if( strcmp( Name, m->ModuleName ) == 0 ) {
            found = true;
            break;
        }
    }

    RdosLeaveSection( obj->FSection );

    return found;
}
#endif

static void UpdateModules( struct TDebug *obj )
{
    struct TDebugModule *m;
    int model;

    model = GetThreadMemoryModel( obj->CurrentThread );
    if( model != obj->FMemoryModel ) {
        obj->FMemoryModel = model;
        obj->FConfigChange = true;
    }

    if( obj->FMemoryModel != DEBUG_MEMORY_MODEL_FLAT ) {
        if( !FindModule( obj, obj->CurrentThread->Cs ) ) {
            m = (struct TDebugModule *)malloc( sizeof( struct TDebugModule ) );
            InitKernelDebugModule( m, obj->CurrentThread->Cs );
            if( m->Handle ) {
                InsertModule( obj, m );
            } else {
                free( m );
            }
        }
    }
}

static void AddGlobalBreak(struct TDebug *obj, struct TDebugBreak *b)
{
    opcode_type         brk_opcode;

    if( obj->ThreadList ) {
        if( !b->IsActive ) {
            RdosReadThreadMem( obj->ThreadList->ThreadID, b->Sel, b->Offset, (char *)&brk_opcode, sizeof( brk_opcode ) );
            b->Instr = brk_opcode;
            brk_opcode = BRKPOINT;
            RdosWriteThreadMem( obj->ThreadList->ThreadID, b->Sel, b->Offset, (char *)&brk_opcode, sizeof( brk_opcode ) );
        }
        b->IsActive = true;
    }
}

static void RemoveGlobalBreak(struct TDebug *obj, struct TDebugBreak *b)
{
    opcode_type         brk_opcode;

    if( obj->ThreadList ) {
        if( b->IsActive ) {
            brk_opcode = b->Instr;
            RdosWriteThreadMem( obj->ThreadList->ThreadID, b->Sel, b->Offset, (char *)&brk_opcode, sizeof( brk_opcode ) );
        }
        b->IsActive = false;
    }
}

static struct TDebugBreak *GetSwBreak( struct TDebug *obj, int Sel, long Offset )
{
    struct TDebugBreak *b;

    RdosEnterSection( obj->FSection );

    for( b = obj->SwBreakList; b != NULL; b = b->Next ) {
        if( b->Sel == Sel && b->Offset == Offset ) {
            break;
        }
    }

    RdosLeaveSection( obj->FSection );

    return( b );
}

void AddBreak( struct TDebug *obj, int Sel, long Offset, int Hw )
{
    struct TDebugBreak *newbr;
    struct TDebugBreak *b;
    int found = false;

    newbr = (struct TDebugBreak *)malloc( sizeof( struct TDebugBreak ) );
    InitDebugBreak( newbr, Sel, Offset, Hw );

    if( !Hw ) {
        AddGlobalBreak( obj, newbr );
    }

    RdosEnterSection( obj->FSection );

    newbr->Next = 0;

    if( Hw ) {
        b = obj->HwBreakList;
    } else {
        b = obj->SwBreakList;
    }

    if( b ) {
        while( b->Next ) {
            if( b->Sel == Sel && b->Offset == Offset )
                found = true;
            b = b->Next;
        }

        if( !found ) {
            b->Next = newbr;
        }
    } else {
        if( Hw ) {
            obj->HwBreakList = newbr;
        } else {
            obj->SwBreakList = newbr;
        }
    }

    RdosLeaveSection( obj->FSection );
}

void ClearBreak( struct TDebug *obj, int Sel, long Offset )
{
    struct TDebugBreak *b;
    struct TDebugBreak *delbr;

    RdosEnterSection( obj->FSection );

    b = obj->HwBreakList;

    if( b ) {
        if( b->Offset == Offset && b->Sel == Sel ) {
            obj->HwBreakList = b->Next;
            free( b );
        } else {
            while( b->Next ) {
                delbr = b->Next;

                if( delbr->Offset == Offset && delbr->Sel == Sel ) {
                    b->Next = delbr->Next;
                    free( delbr );
                } else {
                    b = b->Next;
                }
            }
        }
    }

    b = obj->SwBreakList;

    if( b ) {
        if( b->Offset == Offset && b->Sel == Sel ) {
            obj->SwBreakList = b->Next;
            RemoveGlobalBreak( obj, b );
            free( b );
        } else {
            while( b->Next ) {
                delbr = b->Next;

                if( delbr->Offset == Offset && delbr->Sel == Sel ) {
                    b->Next = delbr->Next;
                    RemoveGlobalBreak( obj, delbr );
                    free( delbr );
                } else {
                    b = b->Next;
                }
            }
        }
    }

    RdosLeaveSection( obj->FSection );
}

void AddWatch( struct TDebug *obj, int Sel, long Offset, int Size )
{
    struct TDebugWatch *neww;
    struct TDebugWatch *w;
    int found = false;

    neww = (struct TDebugWatch *)malloc( sizeof( struct TDebugWatch ) );
    InitDebugWatch( neww, Sel, Offset, Size );

    RdosEnterSection( obj->FSection );

    neww->Next = 0;

    w = obj->WatchList;
    if( w ) {
        while( w->Next ) {
            if( w->Sel == Sel && w->Offset == Offset )
                found = true;
            w = w->Next;
        }

        if( !found ) {
            w->Next = neww;
        }
    } else {
        obj->WatchList = neww;
    }

    RdosLeaveSection( obj->FSection );
}

void ClearWatch( struct TDebug *obj, int Sel, long Offset, int Size )
{
    struct TDebugWatch *w;
    struct TDebugWatch *delw;

    /* unused parameters */ (void)Size;

    RdosEnterSection( obj->FSection );

    w = obj->WatchList;

    if( w ) {
        if( w->Offset == Offset && w->Sel == Sel ) {
            obj->WatchList = w->Next;
            free( w );
        } else {
            while( w->Next ) {
                delw = w->Next;

                if( delw->Offset == Offset && delw->Sel == Sel ) {
                    w->Next = delw->Next;
                    free( delw );
                } else {
                    w = w->Next;
                }
            }
        }
    }

    RdosLeaveSection( obj->FSection );
}

static struct TDebugBreak *PrepareToRun( struct TDebug *obj )
{
    struct Tss         tss;
    struct TDebugBreak *bp;
    opcode_type         brk_opcode;

    RdosGetThreadTss( obj->CurrentThread->ThreadID, &tss );

    bp = GetSwBreak( obj, tss.cs, tss.eip );
    if( bp ) {
        brk_opcode = bp->Instr;
        RdosWriteThreadMem( obj->CurrentThread->ThreadID, tss.cs, tss.eip, (char *)&brk_opcode, sizeof( brk_opcode ) );
        bp->IsActive = false;
    } else {
        RdosReadThreadMem( obj->CurrentThread->ThreadID, tss.cs, tss.eip, (char *)&brk_opcode, sizeof( brk_opcode ) );
        if( brk_opcode == BRKPOINT ) {
            tss.eip++;
            RdosSetThreadTss( obj->CurrentThread->ThreadID, &tss );
        }
    }
    return( bp );
}

static struct TDebugBreak *DoTrace( struct TDebug *obj )
{
    struct TDebugBreak *bp = 0;

    if( ( obj->CurrentThread->Cs & 0x3 ) == 0x3 ) {
        bp = PrepareToRun( obj );
        SetupTrace( obj->CurrentThread, bp );
        RdosContinueDebugEvent( obj->FHandle, obj->CurrentThread->ThreadID );
    } else {
        while ( RdosGetDebugThread() != obj->CurrentThread->ThreadID )
            RdosDebugNext();
        RdosDebugTrace();
    }
    return( bp );
}

static struct TDebugBreak *DoGo( struct TDebug *obj )
{
    struct TDebugBreak *bp = 0;

    if( ( obj->CurrentThread->Cs & 0x3 ) == 0x3 ) {
        bp = PrepareToRun( obj );
        SetupGo( obj->CurrentThread, bp );
        ActivateBreaks( obj->CurrentThread, obj->HwBreakList, obj->WatchList );
        RdosContinueDebugEvent( obj->FHandle, obj->CurrentThread->ThreadID );
    } else {
        while ( RdosGetDebugThread() != obj->CurrentThread->ThreadID )
            RdosDebugNext();
        ActivateBreaks( obj->CurrentThread, obj->HwBreakList, obj->WatchList );
        RdosDebugRun();
    }
    return( bp );
}

void SetUserScreen( void )
{
    if( DebugKey ) {
        RdosSetFocus( DebugKey );
    }
}

void SetDebugScreen( void )
{
    if( SelfKey ) {
        RdosSetFocus( SelfKey );
    }
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
    if( obj->CurrentThread ) {
        RdosResetSignal( obj->UserSignal );
        DoTrace( obj );
        RdosWaitForever( obj->UserWait );
    }
}

static int PickNewThread( struct TDebug *obj )
{
    struct TDebugThread *Thread;

    RdosEnterSection( obj->FSection );

    for( Thread = obj->ThreadList; Thread != NULL; Thread = Thread->Next ) {
        if( Thread->FDebug ) {
            obj->CurrentThread = Thread;
            obj->FThreadChanged = true;
            break;
        }
    }

    RdosLeaveSection( obj->FSection );

    if( Thread ) {
        return true;
    } else {
        return false;
    }
}

static void FixupAfterTimeout( struct TDebug *obj, struct TDebugBreak *bp )
{
    opcode_type          brk_opcode = BRKPOINT;
    struct TDebugThread *thread;
    int                  wait;

    wait = RdosWaitTimeout( obj->UserWait, 5 );
    while( wait ) {
        wait = RdosWaitTimeout( obj->UserWait, 5 );
    }

    thread = obj->CurrentThread;
    if( thread == 0 ) {
        thread = obj->ThreadList;
    }

    if( thread && bp ) {
        if( !bp->IsActive )
            RdosWriteThreadMem( thread->ThreadID, bp->Sel, bp->Offset, (char *)&brk_opcode, sizeof( brk_opcode ) );
        bp->IsActive = true;
    }

    if( thread && !IsDebug( thread ) ) {
        PickNewThread( obj );
    }
}

int AsyncGo( struct TDebug *obj, int ms )
{
    int                  wait;
    struct TDebugBreak  *bp;

    if( obj->CurrentThread ) {
        RdosResetSignal( obj->UserSignal );

        bp = DoGo( obj );
        wait = RdosWaitTimeout( obj->UserWait, ms );
        FixupAfterTimeout( obj, bp );

        if( wait != 0 ) {
            return( true );
        } else {
            return( PickNewThread( obj ) );
        }
    }
    return( true );
}

int AsyncTrace( struct TDebug *obj, int ms )
{
    int                  wait;
    struct TDebugBreak  *bp;

    if( obj->CurrentThread ) {
        RdosResetSignal( obj->UserSignal );

        bp = DoTrace( obj );
        wait = RdosWaitTimeout( obj->UserWait, ms );
        FixupAfterTimeout( obj, bp );

        if( wait != 0 ) {
            return( true );
        } else {
            return( PickNewThread( obj ) );
        }
    }
    return( true );
}

int AsyncPoll( struct TDebug *obj, int ms )
{
    int     wait;

    wait = RdosWaitTimeout( obj->UserWait, ms );
    if( wait ) {
        return( true );
    } else {
        return( PickNewThread( obj ) );
    }
}

void ExitAsync( struct TDebug *obj )
{
    /* unused parameters */ (void)obj;
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

    /* unused parameters */ (void)exitcode;

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
    obj->FThreadChanged = true;

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

    for( Thread = obj->ThreadList; Thread != NULL; Thread = Thread->Next ) {
        if( Thread->ThreadID == thread ) {
            SetException( Thread, event );
            if( obj->FWaitLoad )
                ClearThreadBreak( Thread );
            obj->FWaitLoad = false;
            break;
        }
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

static void HandleKernelException( struct TDebug *obj, struct TKernelExceptionEvent *event, int thread )
{
    struct TDebugThread *Thread;

    RdosEnterSection( obj->FSection );

    Thread = obj->ThreadList;

    while ( Thread && Thread->ThreadID != thread )
        Thread = Thread->Next;

    if( Thread )
        SetKernelException( Thread, event );

    RdosLeaveSection( obj->FSection );
}

static void SignalDebugData( struct TDebug *obj )
{
    int ThreadId;
    char debtype;
    struct TCreateProcessEvent cpe;
    struct TCreateThreadEvent cte;
    struct TLoadDllEvent lde;
    struct TExceptionEvent ee;
    struct TKernelExceptionEvent kev;
    int ExitCode;
    int handle;

    debtype = RdosGetDebugEvent( obj->FHandle, &ThreadId );

    switch (debtype) {
        case EVENT_EXCEPTION:
            RdosGetDebugEventData( obj->FHandle, &ee );
            HandleException( obj, &ee, ThreadId );
            break;

        case EVENT_CREATE_THREAD:
            RdosGetDebugEventData( obj->FHandle, &cte );
            obj->FThreadChanged = true;
            HandleCreateThread( obj, &cte );
            break;

        case EVENT_CREATE_PROCESS:
            RdosGetDebugEventData( obj->FHandle, &cpe );
            HandleCreateProcess( obj, &cpe );
            break;

        case EVENT_TERMINATE_THREAD:
            HandleTerminateThread( obj, ThreadId );
            obj->FThreadChanged = true;
            if( obj->CurrentThread->ThreadID == ThreadId ) {
                obj->CurrentThread = 0;
            }
            break;

        case EVENT_TERMINATE_PROCESS:
            RdosGetDebugEventData( obj->FHandle, &ExitCode );
            HandleTerminateProcess( obj, ExitCode);
            obj->FInstalled = false;
            RdosSetSignal( obj->UserSignal );
            break;

        case EVENT_LOAD_DLL:
            RdosGetDebugEventData( obj->FHandle, &lde );
            HandleLoadDll( obj, &lde );
            break;

        case EVENT_FREE_DLL:
            RdosGetDebugEventData( obj->FHandle, &handle );
            HandleFreeDll( obj, handle );
            break;

        case EVENT_KERNEL:
            RdosGetDebugEventData( obj->FHandle, &kev );
            HandleKernelException( obj, &kev, ThreadId );
            break;
    }

    RdosClearDebugEvent( obj->FHandle );

    switch( debtype )
    {
        case EVENT_EXCEPTION:
        case EVENT_KERNEL:
            UpdateModules( obj );
            RdosSetSignal( obj->UserSignal );
            break;
    }
}

static void DebugThread( void *Param )
{
    int CurrModuleHandle;
    int WaitHandle;
    int thread;
    struct TDebug *obj = (struct TDebug *)Param;

    obj->FInstalled = true;

    CurrModuleHandle = RdosGetModuleHandle();
    SelfKey = RdosGetModuleFocusKey( CurrModuleHandle );

    RdosWaitMilli( 250 );

    obj->FHandle = RdosSpawnDebug( obj->FProgram, obj->FParam, obj->FStartDir, 0, &thread);

    RdosWaitMilli( 250 );

    if( obj->FHandle ) {
        WaitHandle = RdosCreateWait();
        RdosAddWaitForDebugEvent( WaitHandle, obj->FHandle, (int)obj );

        while( obj->FInstalled ) {
            if( RdosWaitForever( WaitHandle ) ) {
                SignalDebugData( obj );
            }
        }
        RdosCloseWait( WaitHandle );
    } else {
        obj->FInstalled = false;
    }
}

int IsTerminated( struct TDebug *obj )
{
    return !obj->FInstalled;
}

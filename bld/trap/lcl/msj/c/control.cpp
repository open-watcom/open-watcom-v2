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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <windows.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include "machtype.h"
#include "trapbrk.h"
#include "trperr.h"
#include "msjvdbg.h"
#include "jdbgguid.h"
#include "msvmutil.h"
#include "msjutil.h"
#include "control.h"
#include "readwr.h"
#include "madjvm.h"
#include "olemap.hpp"

class WDbgMgrCallback : public IRemoteDebugManagerCallback {
    public:
    unsigned    refcount;
    virtual HRESULT __stdcall QueryInterface( REFIID, void ** );
    virtual ULONG   __stdcall AddRef( void );
    virtual ULONG   __stdcall Release( void );

    virtual HRESULT __stdcall ProcessCreateEvent( IRemoteProcess * newproc,
                                                  IRemoteProcess * parent );
};

class WProcessCallback : public IRemoteProcessCallback {
    public:
    unsigned    refcount;
    virtual HRESULT __stdcall QueryInterface( REFIID, void ** );
    virtual ULONG   __stdcall AddRef( void );
    virtual ULONG   __stdcall Release( void );

    virtual HRESULT __stdcall DebugStringEvent( IRemoteThread *,
                                                LPCOLESTR );
    virtual HRESULT __stdcall CodeBreakpointEvent( IRemoteThread * );
    virtual HRESULT __stdcall DataBreakpointEvent( IRemoteThread *,
                                                   IRemoteObject * );
    virtual HRESULT __stdcall ExceptionEvent( IRemoteThread *,
                                              IRemoteClassField *,
                                              EXCEPTIONKIND );
    virtual HRESULT __stdcall StepEvent( IRemoteThread * );
    virtual HRESULT __stdcall CanStopEvent( IRemoteThread * );
    virtual HRESULT __stdcall BreakEvent( IRemoteThread * );
    virtual HRESULT __stdcall ThreadCreateEvent( IRemoteThread * );
    virtual HRESULT __stdcall ThreadDestroyEvent( IRemoteThread * );
    virtual HRESULT __stdcall ThreadGroupCreateEvent( IRemoteThread *,
                                                      IRemoteThreadGroup * );
    virtual HRESULT __stdcall ThreadGroupDestroyEvent(  IRemoteThread *,
                                                        IRemoteThreadGroup * );
    virtual HRESULT __stdcall ClassLoadEvent(  IRemoteThread *,
                                               IRemoteClassField * );
    virtual HRESULT __stdcall ClassUnloadEvent(  IRemoteThread *,
                                                 IRemoteClassField * );
    virtual HRESULT __stdcall ProcessDestroyEvent( IRemoteThread * );
    virtual HRESULT __stdcall TraceEvent( IRemoteThread * );
    virtual HRESULT __stdcall LoadCompleteEvent( IRemoteThread * );
};

#define MAX_HWNDS 40 // maximum number of hwnds in the debugger

static IRemoteDebugManager *    DbgManager;
static WDbgMgrCallback          ManagerCB;
static WProcessCallback         ProcessCB;
static HWND                     InvalidHWNDs[MAX_HWNDS];
static int                      NumInvalid = 0;
static HANDLE                   EventSem;

typedef struct _threadlist {
    struct _threadlist *next;
    IRemoteThread *     handle;
    char *              name;           // variable length structure
} threadlist;

static struct {
    IRemoteProcess *    curr;
    HANDLE              termthread;
    HANDLE              prochdl;
    unsigned            flags;
    threadlist *        threadtable;
    unsigned            waiting : 1;
    unsigned            interrupting : 1;
    unsigned            task_loading : 1;
} Process;

struct ImageEntry {
    IRemoteClassField   *cls;
    char                *className;
    ULONG               num_methods;
    IRemoteMethodField  **methods;
    ULONG               num_cues;
    jvm_line_info       *cues;
    ULONG               base_addr;
    ULONG               end_addr;
    IEnumRemoteField    *pEnum;
    unsigned            newly_loaded : 1;
    unsigned            newly_unloaded : 1;
    unsigned            modinfo_loaded : 1;
    unsigned            addrinfo_loaded : 1;
    unsigned            cues_loaded : 1;
};

#define MAKE_OFFSET( i,m,o ) ( ( ( ((i)+1) & 0xFF) << 24 ) + ( ( (m) & 0xFF ) << 16 ) + (o) )
#define OFFSET_IMAGE( a ) ( ( ( (a) >> 24 ) & 0xFF ) - 1 )
#define OFFSET_METHOD( a ) ( ( (a) >> 16 ) & 0xFF )
#define OFFSET_OFFSET( a ) ( (a) & 0xFFFF )

int __assert( char *str, char *file, int line )
{
    char buff[256];
    sprintf( buff, "%s(%d): %s", file, line, str );
    MessageBox( NULL, buff, "Assertion Failed", MB_SYSTEMMODAL+MB_OK );
    return( 0 );
}

int __ok()
{
    return( 1 );
}

#define DbgAssert( x ) ( (x) ? __ok() : __assert( #x, __FILE__, __LINE__ ) )
#define MustSucceed( x ) DbgAssert( SUCCEEDED( x ) )
#define MustBeMainThread() DbgAssert( MainThreadId == GetCurrentThreadId() )

static ImageEntry *ImageMap;
static int ImageMapSize;
static int ImageMapTop;

LPSTREAM CurrThreadStream;
IRemoteThread * CurrThread;
IRemoteThread * TraceThread;
IRemoteThread * CurrThread_T2;

// function prototypes

static DWORD WINAPI     WaitForDeath( void * );
static void             WaitForEvent( void );
static void             SignalEvent( void );

extern "C" {

extern HWND DebuggerWindow;
static DWORD MainThreadId;

extern bool InitProc( void )
/**************************/
{
    EventSem = CreateSemaphore( NULL, 0, 1, NULL );
    ManagerCB.refcount = 0;
    ProcessCB.refcount = 0;
    MainThreadId = GetCurrentThreadId();
    if( !SUCCEEDED( CoInitialize( NULL ) ) ) return( FALSE );
    if( !SUCCEEDED( CoCreateInstance( CLSID_RemoteJavaDebugManager, NULL,
                           CLSCTX_LOCAL_SERVER, IID_IRemoteDebugManager,
                           (PVOID *)&DbgManager ) ) ) return FALSE;
    DbgManager->AddRef();
    return( SUCCEEDED( DbgManager->RegisterCallback( &ManagerCB ) ) );
}

extern void FiniProc( void )
/**************************/
{
    MustBeMainThread();
    if( DbgManager != NULL ) {
        DbgManager->Detach();
        DbgManager->Release();
    }
    CoUninitialize();
    CloseHandle( EventSem );
}

extern void TraceOn( void )
/*************************/
{
    MustBeMainThread();
    Process.curr->TraceMethods( TRUE );
}

extern void TraceOff( void )
/**************************/
{
    MustBeMainThread();
    Process.curr->TraceMethods( FALSE );
}

#define CLASSNAME_SUFFIX        " [Class]"

extern HANDLE FakeOpen( char *name )
/*******************************/
{
    int len = strlen( name );
    if( len > sizeof( CLASSNAME_SUFFIX ) - 1 ) {
        if( strcmp( name + len - sizeof( CLASSNAME_SUFFIX ) + 1, CLASSNAME_SUFFIX ) == 0 ) {
            return( FakeHandle );
        }
    }
    return( INVALID_HANDLE_VALUE );
}

static int LastNameGiven;

extern bool FakeRead( HANDLE h, void* buff, unsigned len, unsigned*amtRead )
/**************************************************************************/
{
    char *data;
    if( h != FakeHandle ) return( FALSE );
    data = "JAVAxxxx";
    *(int*)(data+4)=LastNameGiven;
    if( len > 8 ) len = 8;
    memcpy( buff, data, len );
    if( amtRead ) *amtRead = len;
    return( TRUE );
}

void AddAddressInfo( int idx )
/****************************/
{
    IRemoteClassField   *cls;
    ImageEntry          *image;

    image = &ImageMap[idx];
    if( image->addrinfo_loaded ) return;
    MustBeMainThread();
    image->addrinfo_loaded = TRUE;
    cls = image->cls;
    if( !SUCCEEDED( cls->GetFields(&image->pEnum,FIELD_KIND_METHOD,0,NULL) ) ) return;
    image->pEnum->GetCount( &image->num_methods );
    image->base_addr = MAKE_OFFSET( idx, 0, 0 );
    image->end_addr = MAKE_OFFSET( idx, image->num_methods-1, 0xFFFF );
}

void AddModuleInfo( int idx )
/**************************/
{
    ULONG               got, fetched;
    IRemoteField        *field;
    IRemoteMethodField  *meth;
    ImageEntry          *image;

    image = &ImageMap[idx];
    if( image->modinfo_loaded ) return;
    MustBeMainThread();
    image->modinfo_loaded = TRUE;
    AddAddressInfo( idx );
    image->methods = (IRemoteMethodField**)MSJAlloc( image->num_methods * sizeof( IRemoteField* ) );
    got = 0;
    while( got < image->num_methods ) {
        image->pEnum->Next( 1, &field, &fetched );
        field->QueryInterface( IID_IRemoteMethodField, (void**)&meth );
        image->methods[got] = meth;
        field->Release();
        got += fetched;
    }
    image->pEnum->Release();
    image->pEnum = NULL;
}

void AddCueInfo( int idx )
/************************/
{
    IEnumLINEINFO       *pLines;
    ULONG               count;
    jvm_line_info       *cues;
    LINEINFO            curr_cue;
    int                 i,j;
    ImageEntry          *image;
    ULONG               fetched;

    AddModuleInfo( idx );
    image = &ImageMap[idx];
    if( image->cues_loaded ) return;
    MustBeMainThread();
    image->cues_loaded = TRUE;
    image->num_cues = 0;
    for( i = 0; i < image->num_methods; ++i ) {
        pLines = NULL;
        image->methods[i]->GetLineInfo( &pLines );
        if( pLines == NULL ) continue;
        pLines->GetCount( &count );
        image->num_cues += count;
        pLines->Release();
    }
    cues = image->cues = (jvm_line_info*)MSJAlloc( image->num_cues * sizeof( jvm_line_info ) );
    for( i = 0; i < image->num_methods; ++i ) {
        pLines = NULL;
        image->methods[i]->GetLineInfo( &pLines );
        if( pLines == NULL ) continue;
        pLines->GetCount( &count );
        for( j = 0; j < count; ++j ) {
            pLines->Next( 1, &curr_cue, &fetched );
            if( curr_cue.iLine == 0 ) {
                image->num_cues--;
            } else {
                cues->offset = MAKE_OFFSET( idx, i, curr_cue.offPC );
                cues->line = curr_cue.iLine;
                ++cues;
            }
        }
        pLines->Release();
    }
}

unsigned GetLibName( unsigned handle, char *name )
/************************************************/
{
    int         i;

    for( i = 0; i < ImageMapTop; ++i ) {
        if( ImageMap[i].newly_unloaded ) {
            name[0] = '\0';
            ImageMap[i].newly_unloaded = FALSE;
            return( i+1 );
        } else if( ImageMap[i].newly_loaded ) {
            strcpy( name, ImageMap[i].className );
            ImageMap[i].newly_loaded = FALSE;
            LastNameGiven = i;
            return( i+1 );
        }
    }
    return( 0 );
}

void InitImageMap()
/*****************/
{
    ImageEntry  *image;
    int i,j;

    MustBeMainThread();
    if( ImageMap != NULL ) {
        image = ImageMap;
        for( i = 0; i < ImageMapTop; ++i ) {
            if( image->methods != NULL ) {
                for( j = 0; j < image->num_methods; ++j ) {
                    image->methods[j]->Release();
                }
                MSJFree( image->methods );
            }
            if( image->cues ) MSJFree( image->cues );
            ++image;
        }
        MSJFree( ImageMap );
    }
    ImageMap = NULL;
    ImageMapSize = 0;
    ImageMapTop = 0;
}

extern addr48_off MakeMethodOffset( void *cls, void *meth, addr48_off offset )
/****************************************************************************/
{
    int i,j;

    for( i = 0; i < ImageMapTop; ++i ) {
        if( ImageMap[i].cls == cls ) {
            AddModuleInfo( i );
            for( j = 0; j < ImageMap[i].num_methods; ++j ) {
                if( ImageMap[i].methods[j] == meth ) {
                    return( MAKE_OFFSET( i, j, offset ) );
                }
            }
        }
    }
    return( -1 );
}

void *GetMethodPointer( addr48_off *poff )
/****************************************/
{
    addr48_off offset = *poff;
    int image,method;
    image = OFFSET_IMAGE(offset);
    method = OFFSET_METHOD(offset);
    if( image >= ImageMapTop ) return( NULL );
    if( method >= ImageMap[image].num_methods ) return( NULL );
    *poff = OFFSET_OFFSET( offset );
    return( ImageMap[ image ].methods[ method ] );
}


int GrowImageMap()
/****************/
{
    int i = ImageMapTop;
    if( ++ImageMapTop >= ImageMapSize ) {
        ImageMapSize += 10;
        ImageMap = (ImageEntry*)MSJReAlloc( ImageMap, ImageMapSize * sizeof( ImageEntry ) );
    }
    return( i );
}

extern bool StartProc( char *procname, char *clname )
/***************************************************/
// return TRUE if successful
{
    STARTUPINFO         sinfo;
    PROCESS_INFORMATION pinfo;
    wchar_t *           wclname;
    unsigned            namelen;
    DWORD               tid;

    MustBeMainThread();
    InitImageMap();
    Process.interrupting = FALSE;
    Process.task_loading = TRUE;
    Process.curr = NULL;
    Process.flags = 0;
    memset( &sinfo, 0, sizeof(sinfo) );
    sinfo.cb = sizeof(sinfo);
    if( !CreateProcess( NULL, procname, NULL, NULL, FALSE,
                        CREATE_SUSPENDED | CREATE_NEW_CONSOLE, NULL, NULL,
                        &sinfo, &pinfo) ) return FALSE;
    Process.prochdl = pinfo.hProcess;
    namelen = strlen(clname) + 1;
    wclname = (wchar_t *) alloca( namelen * sizeof(wchar_t) );
    if( wclname == NULL ) {
        EndProc();
        return FALSE;
    }
    MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, clname, namelen, wclname,
                         namelen * sizeof(wchar_t) );
    if( !SUCCEEDED( DbgManager->RequestCreateEvent( wclname, pinfo.dwProcessId ) ) ) {
        EndProc();
        return FALSE;
    }
    Process.termthread = CreateThread( NULL, 0, WaitForDeath, NULL, 0, &tid );
    if( Process.termthread == NULL ) {
        EndProc();
        return FALSE;
    }
    Process.waiting = TRUE;
    if( ResumeThread( pinfo.hThread ) == 0xFFFFFFFF ) {
        EndProc();
        return FALSE;
    }
    CloseHandle( pinfo.hThread );
    WaitForEvent();
    return TRUE;
}

extern void EndProc( void )
/*************************/
{
    MustBeMainThread();
    if( Process.curr != NULL ) {
        Process.curr->Detach();
        Process.curr->Release();
        Process.curr = NULL;
    }
    if( Process.termthread != NULL ) {
        TerminateThread( Process.termthread, 0 );
    }
    TerminateProcess( Process.prochdl, 0 );
    CloseHandle( Process.termthread );
    CloseHandle( Process.prochdl );
    InitImageMap();
}

static void GetCurrThreadPointerForThisThread()
/*********************************************/
{
#if 1
    if( CurrThreadStream != NULL ) {
        if( CurrThread_T2 != NULL ) {
            CurrThread_T2->Release();
            CurrThread_T2 = NULL;
        }
        CoGetInterfaceAndReleaseStream( CurrThreadStream, IID_IRemoteThread, (void**)&CurrThread_T2 );
        CurrThreadStream = NULL;
    }
#else
    CurrThread_T2 = CurrThread;
#endif
}

extern void ResumeProc( addr48_ptr *pc )
/**************************************/
{
    if( Process.flags != 0 ) return;    // something happened while away
    Process.waiting = TRUE;
    GetCurrThreadPointerForThisThread();
    MustSucceed( CurrThread_T2->Go() );
    WaitForEvent();
    GetPCFromThread( CurrThread_T2, pc );
}

extern void TraceProc( addr48_ptr *pc )
/*************************************/
{
    if( Process.flags != 0 ) return;    // something happened while away
    Process.waiting = TRUE;
    GetCurrThreadPointerForThisThread();
    TraceThread = CurrThread;
    MustSucceed( CurrThread_T2->StepIn() );
    WaitForEvent();
    GetPCFromThread( CurrThread_T2, pc );
    TraceThread = NULL;
}

extern void SignalDeath( void )
/*****************************/
{
    Process.flags |= COND_TERMINATE;
    if( Process.waiting ) {
        SignalEvent();
    }
}

extern void DoInterrupt( void )
/*****************************/
{
    MustBeMainThread();
    if( Process.waiting ) {
        Process.interrupting = TRUE;
        Process.curr->Break();
    }
}

extern void SetBreakpoint( addr48_ptr *addr )
/*******************************************/
{
    ObjType             objtype;
    IRemoteMethodField *obj;

    MustBeMainThread();
    obj = (IRemoteMethodField*)FindObject( addr, &objtype );
    if( obj != NULL && objtype == OBJECT_CODE ) {
        obj->SetBreakpoint( addr->offset );
    }
}

extern void ClearBreakpoint( addr48_ptr *addr )
/*********************************************/
{
    ObjType             objtype;
    IRemoteMethodField *obj;

    MustBeMainThread();
    obj = (IRemoteMethodField*)FindObject( addr, &objtype );
    if( obj != NULL && objtype == OBJECT_CODE ) {
        obj->ClearBreakpoint( addr->offset );
    }
}

extern unsigned ReadFlags( void )
/*******************************/
{
    unsigned    retval;

    retval = Process.flags;
    Process.flags = 0;
    return retval;
}

extern void ProcessQueuedRepaints( void )
/***************************************/
{
    int  i;
    RECT r;

    for( i = 0; i < NumInvalid; ++i ) {
        GetWindowRect( InvalidHWNDs[i], &r );
        InvalidateRect( InvalidHWNDs[i], &r, FALSE );
    }
    NumInvalid = 0;
}

extern unsigned_32 GetNextThread( unsigned_32 id, unsigned_8 *state )
/*******************************************************************/
{
    *state = 0;         // for now, all threads thawed
    if( id == 0 ) {
        return (unsigned_32) Process.threadtable;
    } else {
        return (unsigned_32) ((threadlist *)id)->next;
    }
}

extern unsigned_32 SetThread( unsigned_32 id )
/********************************************/
{
    threadlist *retval;

    retval = Process.threadtable;
    while( retval != NULL ) {
        if( retval->handle == CurrThread ) break;
        retval = retval->next;
    }
    if( id != 0 ) {
        CurrThread = ((threadlist *)id)->handle;
    }
    return (unsigned_32) retval;
}

extern char * GetThreadName( unsigned_32 id )
/*******************************************/
{
    return ((threadlist *)id)->name;
}


unsigned DipCue( int i, void *buff )
/***********************************/
{
    jvm_getdata_ret ret;

    AddCueInfo( i );
    ret.offset = (long)ImageMap[i].cues;
    ret.len = ImageMap[i].num_cues;
    memcpy( buff, &ret, sizeof( ret ) );
    return( sizeof( ret ) );
}

static unsigned SetCharReturn( wchar_t *uname, void *buff )
/*****************************************************/
{
    static char *ReturnValue;
    jvm_getdata_ret *ret = (jvm_getdata_ret *)buff;
    if( uname == NULL ) {
        ret->offset = (long)"";
        ret->len = 0;
    } else {
        if( ReturnValue != NULL ) MSJFree( ReturnValue );
        ReturnValue = UnicodeToASCII( uname );
        ret->offset = (long)ReturnValue;
        ret->len = strlen( ReturnValue ) + 1;
    }
    return( sizeof( *ret ) );
}

unsigned DipFileName( int i, void *buff )
/***************************************/
{
    wchar_t     *uname;

    MustBeMainThread();
    uname = NULL;
    ImageMap[i].cls->GetSourceFileName( &uname );
    return( SetCharReturn( uname, buff ) );
}

unsigned DipModName( int i, void *buff )
/************************************/
{
    wchar_t     *uname;

    MustBeMainThread();
    uname = NULL;
    ImageMap[i].cls->GetName( &uname );
    return( SetCharReturn( uname, buff ) );
}

unsigned DipModBase( int i, void *buff )
/************************************/
{
    jvm_getdata_ret *ret = (jvm_getdata_ret *)buff;

    AddAddressInfo( i );
    ret->offset = ImageMap[i].base_addr;
    ret->len = JVM_CODE_SELECTOR;
    return( sizeof( *ret ) );
}

unsigned MadUpStack( addr48_ptr *pc, addr48_ptr *newpc )
/*****************************************************/
{
    addr48_ptr  prev,curr;
    IRemoteStackFrame *frame,*junk;

    MustBeMainThread();
    newpc->segment = 0;
    newpc->offset = 0;
    if( CurrThread == NULL ) return( sizeof( *newpc ) );
    CurrThread->GetCurrentFrame( &frame );
    if( frame == NULL ) return( sizeof( *newpc ) );
    MapFrameToPC( frame, &curr );
    for( ;; ) {
        junk = frame;
        frame = NULL;
        junk->GetCallingFrame( &frame );
        junk->Release();
        if( frame == NULL ) return( sizeof( *newpc ) );
        prev = curr;
        MapFrameToPC( frame, &curr );
        if( OFFSET_IMAGE( pc->offset ) == OFFSET_IMAGE( prev.offset ) &&
            OFFSET_METHOD( pc->offset ) == OFFSET_METHOD( prev.offset ) ) {
            newpc->offset = curr.offset;
            newpc->segment = curr.segment;
            break;
        }
    }
    return( sizeof( *newpc ) );
}

unsigned DipModEnd( int i, void *buff )
/************************************/
{
    jvm_getdata_ret *ret = (jvm_getdata_ret *)buff;

    AddAddressInfo( i );
    ret->offset = ImageMap[i].end_addr;
    ret->len = JVM_CODE_SELECTOR;
    return( sizeof( *ret ) );
}

} /* end extern "C" */

static void HandleEvent( IRemoteThread *thread, unsigned cond )
/*************************************************************/
{
    if( CurrThread != thread ) {
        CurrThread = thread;
        CoMarshalInterThreadInterfaceInStream( IID_IRemoteThread, CurrThread, &CurrThreadStream );
    }
    Process.flags |= cond;
    SignalEvent();
}

static DWORD WINAPI WaitForDeath( void *parm )
/********************************************/
// in case something goes wrong, need to be able to report when the
// client process has died.
{
    parm = parm;
    WaitForSingleObject( Process.prochdl, INFINITE );
    SignalDeath();
    return 0; // thread over!
}

static bool CantDoIt( void )
/**************************/
{
    if( Process.interrupting ) {
        if( MessageBox( NULL, TRP_WIN_wanna_kill, TRP_The_WATCOM_Debugger,
                    MB_SYSTEMMODAL+MB_YESNO+MB_ICONQUESTION ) == IDYES ) {
            Process.flags |= COND_TERMINATE;
            return TRUE;
        }
    } else if( MessageBox( NULL, TRP_WIN_wanna_interrupt,
                           TRP_The_WATCOM_Debugger,
                           MB_SYSTEMMODAL+MB_YESNO+MB_ICONQUESTION ) == IDYES ){
        DoInterrupt();
    }
    return FALSE;
}

static void RecordPaint( HWND hwnd )
/**********************************/
{
    int i;

    for( i = 0; i < NumInvalid; ++i ) {
        if( InvalidHWNDs[i] == hwnd ) return;
    }
    if( NumInvalid == MAX_HWNDS ) return;
    InvalidHWNDs[NumInvalid] = hwnd;
    ++NumInvalid;
}

void SignalEvent( void )
/**********************/
{
    if( DebuggerWindow == NULL ) {
        ReleaseSemaphore( EventSem, 1, NULL );
    } else {
        PostMessage( DebuggerWindow, WM_QUIT, 0, 0 );
    }
}

static void WaitForEvent( void )
/********************************/
{
    MSG         msg;
    HWND        hwnd;
    BOOL        is_dbg_wnd;
    int         num_paints;

    if( !IsWindow( DebuggerWindow ) ) DebuggerWindow = NULL;
    if( DebuggerWindow == NULL ) {
#if 1
        while( WaitForSingleObject( EventSem, 10 ) == WAIT_TIMEOUT ) {
            while( PeekMessage( &msg, NULL, 0, -1, PM_REMOVE ) ) {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            }
        }
#else
        WaitForSingleObject( EventSem, INFINITE );
#endif
    } else {
        num_paints = 0;
        while( GetMessage( &msg, NULL, 0, 0 ) ) {
            hwnd = msg.hwnd;
            is_dbg_wnd = FALSE;
            if( DebuggerWindow != NULL ) {
                while( hwnd ) {
                    if( hwnd == DebuggerWindow ) {
                        is_dbg_wnd = TRUE;
                        break;
                    }
                    hwnd = GetParent( hwnd );
                }
            }
            if( !is_dbg_wnd ) {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            } else {
                switch( msg.message ) {
                case WM_KEYDOWN:
                    if( msg.wParam == VK_CANCEL ) {
                        DoInterrupt();
                    }
                    break;
                case WM_COMMAND:
                    CantDoIt();
                    break;
                case WM_PAINT:
                {
                    PAINTSTRUCT ps;
                    RecordPaint( msg.hwnd );
                    BeginPaint( msg.hwnd, &ps );
                    EndPaint( msg.hwnd, &ps );
                    break;
                }
                case WM_LBUTTONDOWN:
                case WM_RBUTTONDOWN:
                case WM_MBUTTONDOWN:
                    CantDoIt();
                    break;
                case WM_MOUSEMOVE:
                    break;
                default:
                    DefWindowProc( DebuggerWindow, msg.message, msg.wParam,
                                   msg.lParam );
                }
            }
        }
        ProcessQueuedRepaints();
    }
    Process.waiting = FALSE;
    Process.interrupting = FALSE;
}

HRESULT __stdcall WDbgMgrCallback::QueryInterface( REFIID id, void **object )
/***************************************************************************/
{
    HRESULT     retval;

    if( id == IID_IRemoteDebugManagerCallback || id == IID_IUnknown ) {
        AddRef();
        *object = (void *) this;
        retval = S_OK;
    } else {
        *object = NULL;
        retval = E_NOINTERFACE;
    }
    return retval;
}

ULONG  __stdcall WDbgMgrCallback::AddRef( void )
/**********************************************/
// these are only ever going to be instantiated as static objects
{
    refcount++;
    return refcount;
}

ULONG __stdcall WDbgMgrCallback::Release( void )
/**********************************************/
{
    refcount--;
    return refcount;
}

HRESULT __stdcall WDbgMgrCallback::ProcessCreateEvent( IRemoteProcess * newproc,
                                                       IRemoteProcess * parent )
/******************************************************************************/
{
    newproc->AddRef();
    Process.curr = newproc;
    newproc->RegisterCallback( &ProcessCB );
    return S_FALSE;
}

//---------------------------------------------------------------------------

HRESULT __stdcall WProcessCallback::QueryInterface( REFIID id, void **object )
/****************************************************************************/
{
    HRESULT     retval;

    if( id == IID_IRemoteProcessCallback || id == IID_IUnknown ) {
        AddRef();
        *object = (void *) this;
        retval = S_OK;
    } else {
        *object = NULL;
        retval = E_NOINTERFACE;
    }
    return retval;
}

ULONG   __stdcall WProcessCallback::AddRef( void )
/************************************************/
// these are only ever going to be instantiated as static objects
{
    refcount++;
    return refcount;
}

ULONG   __stdcall WProcessCallback::Release( void )
/************************************************/
// these are only ever going to be instantiated as static objects
{
    refcount--;
    return refcount;
}

HRESULT __stdcall WProcessCallback::DebugStringEvent( IRemoteThread *,
                                                      LPCOLESTR )
/********************************************************************/
{
    return S_FALSE;     // care not about these.
}

HRESULT __stdcall WProcessCallback::CodeBreakpointEvent( IRemoteThread *thread )
/******************************************************************************/
{
    HandleEvent( thread, COND_BREAK );
    return S_OK;        // S_OK == suspend execution
}

HRESULT __stdcall WProcessCallback::DataBreakpointEvent( IRemoteThread *thread,
                                                         IRemoteObject *data )
/*****************************************************************************/
{
    HandleEvent( thread, COND_WATCH );
    return S_OK;
}

HRESULT __stdcall WProcessCallback::ExceptionEvent( IRemoteThread *,
                                                    IRemoteClassField *,
                                                    EXCEPTIONKIND )
/**********************************************************************/
{
    return S_FALSE;
}

HRESULT __stdcall WProcessCallback::StepEvent( IRemoteThread *thread )
/********************************************************************/
{
    HandleEvent( thread, COND_TRACE );
    return S_OK;
}

HRESULT __stdcall WProcessCallback::CanStopEvent( IRemoteThread *thread )
/*****************************************************************/
{
    if( thread == TraceThread ) {
        HandleEvent( thread, COND_TRACE );
        return S_OK;
    }
    return S_FALSE;
}

HRESULT __stdcall WProcessCallback::BreakEvent( IRemoteThread *thread )
/*********************************************************************/
{
    HandleEvent( thread, COND_USER );
    return S_OK;
}

HRESULT __stdcall WProcessCallback::ThreadCreateEvent( IRemoteThread *thread )
/****************************************************************************/
{
    threadlist *newthread;
    wchar_t *   name;

    newthread = (threadlist *) MSJAlloc( sizeof(threadlist) );
    newthread->next = Process.threadtable;
    Process.threadtable = newthread;
    newthread->handle = thread;
    thread->GetName( &name );
    newthread->name = UnicodeToASCII( name );
    thread->AddRef();
    CoTaskMemFree( name );
    Process.flags |= COND_THREAD;
    return S_FALSE;
}

HRESULT __stdcall WProcessCallback::ThreadDestroyEvent( IRemoteThread *thread )
/*****************************************************************************/
{
    threadlist **curr;
    threadlist * victim;

    curr = &Process.threadtable;
    while( *curr != NULL ) {
        if( (*curr)->handle == thread ) break;
        curr = &(*curr)->next;
    }
    if( *curr != NULL ) {
        victim = *curr;
        *curr = victim->next;
        MSJFree( victim->name );
        MSJFree( victim );
        thread->Release();
    }
    Process.flags |= COND_THREAD;
    return S_FALSE;
}

HRESULT __stdcall WProcessCallback::ThreadGroupCreateEvent( IRemoteThread *,
                                                          IRemoteThreadGroup * )
/******************************************************************************/
{
    return S_FALSE;
}

HRESULT __stdcall WProcessCallback::ThreadGroupDestroyEvent( IRemoteThread *,
                                                          IRemoteThreadGroup * )
/******************************************************************************/
{
    return S_FALSE;
}

HRESULT __stdcall WProcessCallback::ClassLoadEvent( IRemoteThread *thread,
                                                    IRemoteClassField *cls )
/***********************************************************************/
{
    ImageEntry  *image;
    wchar_t     *uname;
    char        *aname;
    char        name[256]; // nyi - real length!

    cls->AddRef();
    image = &ImageMap[GrowImageMap()];
    image->cls = cls;
    image->newly_loaded = 1;
    image->newly_unloaded = 0;
    image->num_cues = 0;
    image->cues = NULL;
    image->num_methods = 0;
    image->methods = NULL;
    image->pEnum = NULL;
    image->addrinfo_loaded = FALSE;
    image->modinfo_loaded = FALSE;
    image->cues_loaded = FALSE;
    uname = NULL;
    cls->GetName( &uname );
    if( uname == NULL ) {
        strcpy( name, "unknown class" );
    } else {
        aname = UnicodeToASCII( uname );
        strcpy( name, aname );
        strcat( name, CLASSNAME_SUFFIX );
        MSJFree( (void*)aname );
    }
    image->className = (char*)MSJAlloc( strlen( name ) + 1 );
    strcpy( image->className, name );
    AddAddressInfo( ImageMapTop-1 );
    if( !Process.task_loading ) {
        HandleEvent( thread, COND_LIBRARIES );
        return S_OK;    // S_OK == suspend execution
    } else {
        return S_FALSE;
    }
}

HRESULT __stdcall WProcessCallback::ClassUnloadEvent( IRemoteThread *,
                                                      IRemoteClassField *cls )
/*************************************************************************/
{
    int         i,j;
    ImageEntry  *image;

    image = ImageMap;
    for( i = 0; i < ImageMapTop; ++i ) {
        if( image->cls == cls ) {
            image->newly_unloaded = 1;
            cls->Release();
            for( j = 0; j < image->num_methods; ++j ) {
                image->methods[j]->Release();
                image->methods[j] = NULL;
            }
            if( image->methods ) MSJFree( image->methods );
            image->methods = NULL;
            image->num_methods = 0;
            if( image->cues ) MSJFree( image->cues );
            image->cues = NULL;
            image->num_cues = 0;
            if( image->pEnum ) image->pEnum->Release();
            image->pEnum = NULL;
            break;
        }
        ++image;
    }
    return S_FALSE;
}

HRESULT __stdcall WProcessCallback::ProcessDestroyEvent( IRemoteThread *thread )
/******************************************************************************/
{
    HandleEvent( thread, COND_TERMINATE );
    return S_FALSE;
}

HRESULT __stdcall WProcessCallback::TraceEvent( IRemoteThread *)
/**************************************************************/
{
    return S_FALSE;
}

HRESULT __stdcall WProcessCallback::LoadCompleteEvent( IRemoteThread *thread )
/****************************************************************************/
{
    if( Process.task_loading ) {
        Process.task_loading = FALSE;
        HandleEvent( thread, COND_BREAK );
        ReadFlags();
        return S_OK;
    }
    return S_FALSE;
}


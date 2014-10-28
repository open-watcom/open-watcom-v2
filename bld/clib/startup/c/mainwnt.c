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
* Description:  Win32 main routines for executables and DLLs.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <ctype.h>
#include <string.h>
#ifdef _M_IX86
 #include <i86.h>
#endif

#include "iomode.h"
#include "strdup.h"
#include "liballoc.h"
#include "libwin32.h"
#include "stacklow.h"
#include "sigtab.h"
#include "ntex.h"
#include "rtdata.h"
#include "initfini.h"
#include "rtinit.h"
#include "initarg.h"

DWORD __TlsIndex = NO_INDEX;

_WCRTLINK int *__threadid( void )
{
    return( (int *) &(__THREADDATAPTR->thread_id) );
}

thread_data             *__FirstThreadData = NULL;

static struct thread_data *__SingleThread()
{
    return( __FirstThreadData );
}

static void __NullAccessRtn( int handle )
{
    handle = handle;
}

static void __NullExitRtn() {}

static void __NullAccIOBRtn(void) {}
static void __NullAccHeapRtn(void) {}
static void __NullAccTDListRtn(void) {}

_WCRTDATA struct thread_data    *(*__GetThreadPtr)() = &__SingleThread;
void    (*_AccessFileH)(int)     = &__NullAccessRtn;
void    (*_ReleaseFileH)(int)    = &__NullAccessRtn;
void    (*_AccessIOB)(void)      = &__NullAccIOBRtn;
void    (*_ReleaseIOB)(void)     = &__NullAccIOBRtn;
void    (*_AccessNHeap)(void)    = &__NullAccHeapRtn;
void    (*_AccessFHeap)(void)    = &__NullAccHeapRtn;
void    (*_ReleaseNHeap)(void)   = &__NullAccHeapRtn;
void    (*_ReleaseFHeap)(void)   = &__NullAccHeapRtn;
void    (*_AccessTDList)(void)   = &__NullAccTDListRtn;
void    (*_ReleaseTDList)(void)  = &__NullAccTDListRtn;
void    (*_AccessFList)(void)    = &__NullAccIOBRtn;
void    (*_ReleaseFList)(void)   = &__NullAccIOBRtn;
void    (*_ThreadExitRtn)(void)  = &__NullExitRtn;

void __sig_null_rtn(void) {}
_WCRTLINK void  (*__sig_init_rtn)(void) = __sig_null_rtn;
_WCRTLINK void  (*__sig_fini_rtn)(void) = __sig_null_rtn;

#ifdef _M_IX86
 #pragma aux _end "*"
#endif
extern  char            _end;

int                     __Is_DLL;       /* TRUE => DLL, else not a DLL */
static char             *_cmd_ptr;
static wchar_t          *_wcmd_ptr;

#if WINVER < 0x400
char			_RWD_Envptr_is_fake = 0;
#endif

// called once at DLL_PROCESS_ATTACH or by __NTMainInit

int __NTInit( int is_dll, thread_data *tdata, HANDLE hdll )
{
    DWORD       ver;
    WORD        os_ver;

    __Is_DLL = is_dll;                                  /* 15-feb-93 */
    // tdata is guaranteed to never be NULL. If starting up for an EXE,
    // it's pointing on the stack (alloca). If this is run on behalf of
    // a DLL startup, it's already NULL checked by the caller.
    __FirstThreadData = tdata;
    __initPOSIXHandles();

    _RWD_Envptr = GetEnvironmentStrings();
#if WINVER < 0x400
    /* Microsoft Windows 3.1 win32s does not support GetEnvironmentStringsA() and it will return NULL */
    if (_RWD_Envptr == NULL) {
        /* fake it with the process heap */
	_RWD_Envptr = HeapAlloc(GetProcessHeap(),0,sizeof(uint32_t)); /* alloc 4 bytes of zero */
	if (_RWD_Envptr == NULL) {
		/* Wow, really, Windows 3.1? Really?!? */
		ExitProcess(0xFFFFFFFF);
	}
	*((uint32_t*)_RWD_Envptr) = 0; /* quick and dirty zero first 4 bytes */
	_RWD_Envptr_is_fake = 1;
    }
#endif

    /*
     * Force reference to environ so that __setenvp is linked in; hence,
     * __ParsePosixHandleStr will be called.
     */
    environ = NULL;

    ver = GetVersion();
    os_ver = LOWORD( ver );
    _RWD_osmajor = os_ver & 0xff;
    _RWD_osminor = (os_ver >> 8) & 0xff;
    _RWD_osbuild = HIWORD( ver );
    _RWD_osver = _RWD_osbuild;
    _RWD_winmajor = _RWD_osmajor;
    _RWD_winminor = _RWD_osminor;
    _RWD_winver = (_RWD_winmajor << 8) | _RWD_winminor;

    {
        static char     fn[_MAX_PATH];
        __lib_GetModuleFileNameA( NULL, fn, sizeof( fn ) );
        _LpPgmName = fn;
    }
    {
        static wchar_t  wfn[_MAX_PATH];
        __lib_GetModuleFileNameW( NULL, wfn, sizeof( wfn ) );
        _LpwPgmName = wfn;
    }

    {
        char    *cmd;
        _cmd_ptr = cmd = __clib_strdup( GetCommandLineA() );
        if( *cmd == '"' ) {
            cmd++;
            while( *cmd != '"' && *cmd != 0 ) {
                cmd++;
            }
            if( *cmd ) cmd++;
        } else {
            while( !isspace( *cmd ) && *cmd != 0 ) {
                cmd++;
            }
        }
        while( isspace( *cmd ) ) {
            cmd++;
        }
        _LpCmdLine = cmd;
    }
    {
        wchar_t *wcmd;
        wcmd = GetCommandLineW();       /* Win95 supports GetCommandLineW */
        if( wcmd ) {
            _wcmd_ptr = wcmd = __clib_wcsdup( wcmd );
            if( *wcmd == '"' ) {
                wcmd++;
                while( *wcmd != '"' && *wcmd != 0 ) {
                    wcmd++;
                }
                if( *wcmd ) wcmd++;
            } else {
                while( !isspace( *wcmd ) && *wcmd != 0 ) {
                    wcmd++;
                }
            }
            while( isspace( *wcmd ) ) {
                wcmd++;
            }
        } else {
            wcmd = L"";
        }
        _LpwCmdLine = wcmd;
    }

    if( is_dll ) {
        {
            static char    fn[_MAX_PATH];
            __lib_GetModuleFileNameA( hdll, fn, sizeof( fn ) );
            _LpDllName = fn;
        }
        {
            static wchar_t wfn[_MAX_PATH];
            __lib_GetModuleFileNameW( hdll, wfn, sizeof( wfn ) );
            _LpwDllName = wfn;
        }
    }

    return( TRUE );
}

void __NTFini( void )
{
    // calls to free memory have to be done before semaphores closed
    if( _cmd_ptr ) {
        lib_free( _cmd_ptr );
        _cmd_ptr = NULL;
    }
    if( _wcmd_ptr ) {
        lib_free( _wcmd_ptr );
        _wcmd_ptr = NULL;
    }
    if( _RWD_Envptr != NULL ) {
#if WINVER < 0x400
	if (_RWD_Envptr_is_fake)
		HeapFree(GetProcessHeap(),0,_RWD_Envptr);
	else
		FreeEnvironmentStrings( _RWD_Envptr );

	_RWD_Envptr_is_fake = 0;
#else
	FreeEnvironmentStrings( _RWD_Envptr );
#endif

        _RWD_Envptr = NULL;
    }
}

void __NTMainInit( REGISTRATION_RECORD *rr, thread_data *tdata )
{
    __DefaultExceptionHandler();
    __NTInit( FALSE, tdata, GetModuleHandle(NULL) );
    __init_stack_limits( &_STACKLOW, &_STACKTOP );
    __NewExceptionFilter( rr );
    __InitRtns( INIT_PRIORITY_LIBRARY+1 );
    __sig_init_rtn();
    __InitRtns( 255 );
}

_WCRTLINK void (*__process_fini)(unsigned,unsigned) = 0;

_WCRTLINK void __exit( unsigned ret_code )
{
    __NTFini(); // must be done before following finalizers get called
    if( __Is_DLL ) {
        if( __process_fini != 0 ) {
            (*__process_fini)( 0, FINI_PRIORITY_EXIT-1 );
        }
    } else {
        __DoneExceptionFilter();
        __FiniRtns( 0, FINI_PRIORITY_EXIT-1 );
        (*_ThreadExitRtn)();
    }
    // Also gets done by __FreeThreadDataList which is activated from FiniSema4s
    // for multi-threaded apps
    __FirstThreadData = NULL;
    ExitProcess( ret_code );
}

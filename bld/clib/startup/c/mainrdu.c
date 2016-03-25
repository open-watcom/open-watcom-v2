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
* Description:  RDOS main routines for executables and DLLs.
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <io.h>
#include <ctype.h>
#include <string.h>
#include <rdos.h>
#include "iomode.h"
#include "strdup.h"
#include "liballoc.h"
#include "rtdata.h"
#include "initfini.h"
#include "rtinit.h"
#include "initarg.h"
#include "thread.h"
#include "mthread.h"
#include "snglthrd.h"
#include "fileacc.h"
#include "heapacc.h"
#include "trdlstac.h"
#include "cinit.h"
#include "exitwmsg.h"


static char    DllName[_MAX_PATH];

int __TlsIndex = NO_INDEX;
char *_LpDllName = 0;

_WCRTLINK int *__threadid( void )
{
    return( (int *) &(__THREADDATAPTR->thread_id) );
}

static void __NullAccessRtn( int handle )
{
    handle = handle;
}

static void __NullAccIOBRtn(void) {}
static void __NullAccHeapRtn(void) {}
static void __NullAccTDListRtn(void) {}

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

static void __sig_null_rtn(void) {}

_WCRTDATA void  (*__sig_init_rtn)(void) = __sig_null_rtn;
_WCRTDATA void  (*__sig_fini_rtn)(void) = __sig_null_rtn;

#ifdef _M_IX86
 #pragma aux _end "*"
#endif
extern  char            _end;

int                     __Is_DLL;       /* TRUE => DLL, else not a DLL */
static char             *_cmd_ptr;
static wchar_t          *_wcmd_ptr;

int __RdosInit( int is_dll, thread_data *tdata, int hdll )
{
    int major, minor, release;

    __Is_DLL = is_dll;                                  /* 15-feb-93 */
    __FirstThreadData = tdata;
    RdosGetVersion( &major, &minor, &release );
    _RWD_osmajor = major;
    _RWD_osminor = minor;

    if( is_dll ) {
        _LpCmdLine = "";
        RdosGetModuleName( hdll, DllName, sizeof( DllName ) );
        _LpDllName = DllName;
    } else {
        _LpCmdLine = (char *)RdosGetCmdLine();
        if( _LpCmdLine == 0 )
            _LpCmdLine = "";
        else {
           while( *_LpCmdLine != 0 && *_LpCmdLine != ' ' && *_LpCmdLine != 0x9 ) 
               _LpCmdLine++;
        }
    }

    return( 1 );
}

_WCRTLINK _NORETURN void __exit( unsigned ret_code )
{
    if( !__Is_DLL ) {
        __DoneExceptionFilter();
        __FiniRtns( 0, FINI_PRIORITY_EXIT-1 );
    }
    // Also gets done by __FreeThreadDataList which is activated from FiniSema4s
    // for multi-threaded apps
    __FirstThreadData = NULL;
    if( !__Is_DLL ) {
        RdosUnloadExe( ret_code );
    }
    // never return
}

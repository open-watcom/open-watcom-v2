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
* Description:  RDOS main routines for device-driver target
*
****************************************************************************/


#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <rdos.h>

#include <stdio.h>
#include <io.h>
#include <ctype.h>
#include <string.h>

#include "iomode.h"
#include "strdup.h"
#include "liballoc.h"
#include "rtdata.h"
#include "initfini.h"
#include "rtinit.h"
#include "initarg.h"
#include "thread.h"
#include "mthread.h"
#include "fileacc.h"
#include "heapacc.h"
#include "trdlstac.h"
#include "osmain.h"
#include "cmain.h"
#include "_exit.h"


extern void _cexit_( unsigned ret_code );
#pragma aux _cexit_  "*" parm routine [eax]

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

#ifdef _M_IX86
 #pragma aux _end "*"
#endif
extern  char            _end;

static char             *_cmd_ptr;
static wchar_t          *_wcmd_ptr;

void __RdosMain( void )
{
    int major, minor, release;

    __InitRtns( INIT_PRIORITY_THREAD );

#ifdef CMD_LINE
    _LpPgmName = (char *)RdosGetExeName();
#else
    _LpPgmName = "";
#endif    

    RdosGetVersion( &major, &minor, &release );
    _RWD_osmajor = major;
    _RWD_osminor = minor;

#ifdef CMD_LINE
    _LpCmdLine = (char *)RdosGetCmdLine();
    if( _LpCmdLine == 0 )
        _LpCmdLine = "";
    else {
       while( *_LpCmdLine != 0 && *_LpCmdLine != ' ' && *_LpCmdLine != 0x9 ) 
         _LpCmdLine++;
    }
#else
    _LpCmdLine = "";
#endif

    __InitRtns( 255 );
    __CMain();
}

_WCRTLINK void __exit( unsigned ret_code )
{
    _cexit_( ret_code );  // A device-driver should never clean-up rtns, so just exit!
}

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


#include <stdlib.h>
#include <dos.h>
#include <setjmp.h>
#include "tinyio.h"
#include "dbgdefn.h"
#include "dbgmain.h"
#include "dbginit.h"

extern void             WndRefresh( void );

static void (interrupt *_old10)();
static void (interrupt *_old1b)();
static void (interrupt *_old23)();
static void (interrupt *_old24)();
static void (interrupt *_old28)();
static void (interrupt *orig28)();


extern char ActFontTbls;

void interrupt Interrupt10( union REGS r )
{
    if( r.w.ax == 0x1103 ) {
        ActFontTbls = r.h.bl;
    }
    _chain_intr( _old10 );
}

static volatile bool Pending;
void interrupt Interrupt1b_23( void )
{
    Pending = true;
}

bool TBreak( void ) {

    bool tmp;

    tmp = Pending;
    Pending = false;
    return( tmp );
}


static char Fail;
void interrupt Interrupt24( union REGS r )
{
    r.h.al = Fail;
}

void GrabHandlers( void )
{
    _old10 = TinyGetVect( 0x10 );
    _old1b = TinyGetVect( 0x1b );
    _old23 = TinyGetVect( 0x23 );
    _old24 = TinyGetVect( 0x24 );
    _old28 = TinyGetVect( 0x28 );
    TinySetVect( 0x10, Interrupt10 );
    TinySetVect( 0x1b, Interrupt1b_23 );
    TinySetVect( 0x23, Interrupt1b_23 );
    TinySetVect( 0x24, Interrupt24 );
    TinySetVect( 0x28, orig28 );
}

void RestoreHandlers( void )
{
    TinySetVect( 0x10, _old10 );
    TinySetVect( 0x1b, _old1b );
    TinySetVect( 0x23, _old23 );
    TinySetVect( 0x24, _old24 );
    TinySetVect( 0x28, _old28 );
}

void KillDebugger( void )
{
    RestoreHandlers();
    TinyTerminateProcess( 0 );
}

void GUImain( void )
{
    if( _osmajor == 2 ) {
        Fail = 0;
    } else {
        Fail = 3;
    }
    orig28 = TinyGetVect( 0x28 );
    DebugMain();
}


int GUISysInit( int param )
{
    param=param;
    return( 1 );
}

void GUISysFini( void  )
{
    DebugFini();
}


void WndCleanUp( void )
{
}


/*
   We're stubbing out tzset() because the debugger doesn't care about
   time zones and it saves us about 1K of code in the root.
 */

#include <time.h>

long    timezone        = 5L * 60L * 60L;       /* seconds from GMT */
int     __dst_adjust    = 60L * 60L;            /* daylight adjustment */
int     daylight        = 1;                    /* d.s.t. indicator */

struct tm __start_dst =                         /* start of daylight savings */
        { 0, 0, 2,                              /* M4.1.0/02:00:00 default */
          1, 3, 0,                              /* i.e., 1st Sunday of Apr */
          0, 0, 0
        };

struct tm __end_dst =                           /* end of daylight savings */
        { 0, 0, 2,                              /* M10.5.0/02:00:00 default */
          5, 9, 0,                              /* i.e., last Sunday of Oct */
          0, 0, 0
        };

void tzset( void )
{
}

void SysMemInit( void )
{
}

bool SysGUI( void )
{
    return( FALSE );
}

void PopErrBox( const char *buff )
{
    WriteText( STD_ERR, buff, strlen( buff ) );
}

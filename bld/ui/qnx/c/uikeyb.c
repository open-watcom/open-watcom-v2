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


#include <stddef.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <sys/kernel.h>
#include <sys/dev.h>
#include <sys/sidinfo.h>
#include <errno.h>
#include <sys/psinfo.h>
#include <sys/proxy.h>
#include <sys/qioctl.h>
#include <sys/console.h>
#include <fcntl.h>
#include "uidef.h"
#include "uishift.h"
#include "uivirt.h"

/*- these are lame */

void intern flushkey( void )
/**************************/
{
    _flushkey();
}

unsigned char intern checkshift( void )
/*************************************/
{
    return _checkshift();
}

unsigned char UIAPI uicheckshift( void )
/***************************************/
{
    return _uicheckshift();
}

extern void uishiftrelease( EVENT ev )
/************************************/
// Somebody wants us to pretend that the specified event has occurred
// (one of EV_SHIFT/CTRL/ALT_RELEASE) so that the corresponding press
// event will be generated for the next keystroke (if that shift key
// is pressed).
{
    _uishiftrelease(ev);
}

void intern restorekeyb( void )
{
    _restorekeyb();
}


bool intern initkeyboard( void )
/******************************/
{
    return _initkeyboard();
}

void intern finikeyboard( void )
/******************************/
{
    _finikeyboard();
}

void intern savekeyb( void )
{
    _savekeyb();
}

void stopkeyboard( void )
{
    _stopkeyb();
}

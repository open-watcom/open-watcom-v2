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
#include <malloc.h>
#include <dos.h>
#include "uidef.h"
#include "uishift.h"
#define INCL_DOSDEVICES
#include "doscall.h"

static          EVENT                   EventsPress[]   = {
                EV_SHIFT_PRESS,
                EV_SHIFT_PRESS,
                EV_CTRL_PRESS,
                EV_ALT_PRESS,
                EV_SCROLL_PRESS,
                EV_NUM_PRESS,
                EV_CAPS_PRESS,
};

static          EVENT                   EventsRelease[] = {
                EV_SHIFT_RELEASE,
                EV_SHIFT_RELEASE,
                EV_CTRL_RELEASE,
                EV_ALT_RELEASE,
                EV_SCROLL_RELEASE,
                EV_NUM_RELEASE,
                EV_CAPS_RELEASE,
};

static unsigned         shift_state;

unsigned char UIAPI uicheckshift( void )
/***************************************/
{
    return( shift_state );
}

static bool os2getkey( struct _KBDKEYINFO *keyInfo )
/**************************************************/
{
    if( KbdCharIn( keyInfo, IO_NOWAIT, 0 ) == 0  ) {
        return( keyInfo->fbStatus != 0 );
    } else {
        return( FALSE );
    }
}

void intern flushkey( void )
/**************************/
{
    struct _KBDKEYINFO                  keyInfo;
    while( os2getkey( &keyInfo ) );
}

void intern kbdspawnstart( void )
/*******************************/
{
    finikeyboard();
}

void intern kbdspawnend( void )
/*****************************/
{
    initkeyboard();
}


EVENT intern keyboardevent( void )
/********************************/
{
    register    unsigned                scan;
    register    unsigned char           key;
    register    unsigned char           ascii;
    register    EVENT                   ev;
    register    unsigned char           newshift;
    register    unsigned char           changed;
    struct _KBDKEYINFO                  keyInfo;
    struct _KBDINFO                     shiftInfo;


    shiftInfo.cb = sizeof( shiftInfo );
    shiftInfo.fsMask = 0;
    if( KbdGetStatus( &shiftInfo, 0 ) != 0 || shiftInfo.fsMask == 0 ) {
        return( EV_NO_EVENT );
    }
    shift_state = shiftInfo.fsState;
    newshift = uicheckshift();

    /* os2getkey must take precedence over shift change so that  *
     * typing characters by holding the alt key and typing the  *
     * ascii code on the numeric keypad works                   */
    keyInfo.bNlsShift = 0;
    if( os2getkey( &keyInfo ) ) {
        scan  = keyInfo.chScan;
        ascii = keyInfo.chChar;
        ev = scan + 0x100;
        if( ascii != 0 && ascii != 0xE0 ) {
            ev = ascii;
            if( ( newshift & S_ALT ) && ( ascii == ' ' ) ) {
                ev = EV_ALT_SPACE;
            } else if( scan != 0 ) {
                switch( ascii + 0x100 ) {
                case EV_RUB_OUT:
                case EV_TAB_FORWARD:
                case EV_RETURN:
                case EV_ESCAPE:
                    ev = ascii + 0x100;
                    break;
                }
            }
        }
        if( !iskeyboardchar( ev ) ) {
            ev = EV_NO_EVENT;
        }
    } else {
        changed = ( newshift ^ UIData->old_shift ) & ~S_INSERT;
        if( changed != 0 ) {
            key = 0;
            scan = 1;
            while( scan < 128 ) {
                if( ( changed & scan ) != 0 ) {
                    if( ( newshift & scan ) != 0 ) {
                        UIData->old_shift |= scan;
                        ev = EventsPress[ key ];
                        return( ev );
                    } else {
                        UIData->old_shift &= ~scan;
                        ev = EventsRelease[ key ];
                        return( ev );
                    }
                }
                scan <<= 1;
                ++key;
            }
        }
        ev = EV_NO_EVENT;
    }
    return( ev );
}


KBDINFO     SaveStatus;

bool intern initkeyboard( void )
/******************************/
{
    KBDINFO         new;

    SaveStatus.cb = sizeof( SaveStatus );
    KbdGetStatus( &SaveStatus, 0 );
    SaveStatus.fsMask &= ~0x0070;

    new = SaveStatus;

    new.fsMask &= ~0x0009;
    new.fsMask |=  0x0006;
    KbdSetStatus( &new, 0 );
    return( TRUE );
}

void intern finikeyboard( void )
{
    KbdSetStatus( &SaveStatus, 0 );
}

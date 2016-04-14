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


#include "ctkeyb.h"
#include <termios.h>
#include <sys/ioctl.h>


static struct termios   SaveTermSet;
static pid_t            SavePGroup;

int ck_unevent( EVENT ev )
/************************/

// Somebody wants us to pretend that the specified event has occurred
// (one of EV_SHIFT/CTRL/ALT_RELEASE) so that the corresponding press
// event will be generated for the next keystroke (if that shift key
// is pressed).

{
    ev = ev;
#if 0 //Don't think this does anything under QNX
    switch( ev ) {
    case EV_SHIFT_RELEASE:
        ct_shift_state &= ~S_SHIFT;
        break;
    case EV_CTRL_RELEASE:
        ct_shift_state &= ~S_CTRL;
        break;
    case EV_ALT_RELEASE:
        ct_shift_state &= ~S_ALT;
        break;
    }
#endif
    return( 0 );
}

int ck_stop( void )
/*****************/
{
    return( 0 );
}

int ck_flush( void )
/******************/
{
    tcflush( UIConHandle, TCIFLUSH );
    return( 0 );
}

int ck_shift_state( void )
/************************/
{
// FIXME: This is nonsense - the two should not be defined at the same time
#if defined( __LINUX__ ) && !defined( __FreeBSD__ )
    /* read the shift state on the Linux console. Works only locally. */
    /* and WARNING: see console_ioctl(4)                              */
    char shift_state = 6;
    if( ioctl( 0, TIOCLINUX, &shift_state ) >= 0 ) {
        /* Linux console modifiers */
        ct_shift_state &= ~(S_SHIFT|S_CTRL|S_ALT);
        if( shift_state & 1 )
            ct_shift_state |= S_SHIFT;
        if( shift_state & ( 2 | 8 ) )
            ct_shift_state |= S_ALT;
        if( shift_state & 4 ) {
            ct_shift_state |= S_CTRL;
        }
    }
#endif
    return( ct_shift_state );
}

int ck_restore( void )
/********************/
{
    struct termios  new;

    new = SaveTermSet;
    new.c_iflag &= ~(IXOFF | IXON);
    new.c_oflag &= ~OPOST;
    new.c_lflag &= ~(ECHO | ICANON | NOFLSH);
    new.c_lflag |= ISIG;
    new.c_cc[VMIN] = 1;
    new.c_cc[VTIME] = 0;
    while( tcsetattr( UIConHandle, TCSADRAIN, &new ) == -1 && errno == EINTR )
        ;
    return( 0 );
}

int ck_init( void )
/*****************/
{
    tcgetattr( UIConHandle, &SaveTermSet );

    if( !init_trie() )
        return( false );

    if( !ti_read_tix( GetTermType() ) )
        return( false );

    SavePGroup = tcgetpgrp( UIConHandle );
    tcsetpgrp( UIConHandle, UIPGroup );
    restorekeyb();
    return( true );
}

int ck_fini( void )
/*****************/
{
    savekeyb();
    tcsetpgrp( UIConHandle, SavePGroup );
    return( 0 );
}

int ck_save( void )
/*****************/
{
    tcsetattr( UIConHandle, TCSADRAIN, &SaveTermSet );
    return( 0 );
}

Keyboard ConsKeyboard = {
    ck_init,
    ck_fini,
    ck_arm,
    ck_save,
    ck_restore,
    ck_flush,
    ck_stop,
    ck_shift_state,
    ck_unevent
};

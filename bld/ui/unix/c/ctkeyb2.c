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

static struct termios   SaveTermSet;
static pid_t            SavePGroup;

int ck_unevent( EVENT ev )
/*******************************/

// Somebody wants us to pretend that the specified event has occurred
// (one of EV_SHIFT/CTRL/ALT_RELEASE) so that the corresponding press
// event will be generated for the next keystroke (if that shift key
// is pressed).

{
    ev = ev;
    #if 0 //Don't think this does anything under QNX
    switch( ev ) {
    case EV_SHIFT_RELEASE:
        ShftState &= ~S_SHIFT;
        break;
    case EV_CTRL_RELEASE:
        ShftState &= ~S_CTRL;
        break;
    case EV_ALT_RELEASE:
        ShftState &= ~S_ALT;
        break;
    }
    #endif
    return( 0 );
}
int ck_stop()
/******************/
{
    return( 0 );
}

int ck_flush()
/********************/
{
    tcflush( UIConHandle, TCIFLUSH );
    return 0;
}

int ck_shift_state()
/*************************/
{
    return( ShftState );
}
int ck_restore()
/*********************/
{
    struct termios  new;

    new = SaveTermSet;
    new.c_iflag &= ~(IXOFF | IXON);
    new.c_oflag &= ~OPOST;
    new.c_lflag &= ~(ECHO | ICANON | NOFLSH);
    new.c_lflag |= ISIG;
    new.c_cc[VMIN] = 1;
    new.c_cc[VTIME] = 0;
    tcsetattr( UIConHandle, TCSADRAIN, &new );
    return 0;
}

int ck_init()
/******************/
{
    extern void restorekeyb();
    tcgetattr( UIConHandle, &SaveTermSet );

    if( !init_trie() ) return( FALSE );

    if( !ti_read_tix( GetTermType() ) ) return( FALSE );

    SavePGroup = tcgetpgrp( UIConHandle );
    tcsetpgrp( UIConHandle, UIPGroup );
    restorekeyb();
    return( TRUE );
}

int ck_fini()
/************************/
{
    extern void savekeyb();
    savekeyb();
    tcsetpgrp( UIConHandle, SavePGroup );
    return 0;
}
int ck_save()
/******************/
{
    tcsetattr( UIConHandle, TCSADRAIN, &SaveTermSet );
    return 0;
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

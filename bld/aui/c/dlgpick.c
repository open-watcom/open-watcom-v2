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
* Description:  WHEN YOU FIGURE OUT WHAT THIS MODULE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "auipvt.h"

static int      DlgChosen;
static void*    From;
static DLGPICKTEXT      *Rtn;
static unsigned Num;

static void PickInit( gui_window *gui, int list_ctrl )
{
    GUIAddTextList( gui,
                    list_ctrl,
                    Num,
                    From,
                    ( char * (*) (void *, unsigned) )Rtn );
    GUISetCurrSelect( gui, list_ctrl, DlgChosen );
}

extern  int     DlgPickWithRtn2( char *text, void *from, int def,
                                DLGPICKTEXT *rtn, unsigned num,
                                GUIPICKER *pick )
{
    DlgChosen = def;
    From = from;
    Rtn = rtn;
    Num = num;
    return( pick( text, &PickInit ) );
}


extern int DoDlgPick( char *text, PICKCALLBACK * PickInit )
{
    return( GUIDlgPickWithRtn( text, PickInit, DlgOpen ) );
}


extern  int     DlgPickWithRtn( char *text, void *from, int def,
                                DLGPICKTEXT *rtn, unsigned num )
{
    return( DlgPickWithRtn2( text, from, def, rtn, num, DoDlgPick ) );
}

// DLGPICKTEXT DlgPickText;
char *DlgPickText( char **from, int i )
{
    return( from[ i ] );
}

extern  int     DlgPick( char *text, char **from, int def, unsigned num )
{
    return( DlgPickWithRtn( text,
                            from,
                            def,
                            ( char * (*) (void *, int) )DlgPickText,
                            num ) );
}

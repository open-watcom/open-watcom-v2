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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vi.h"
#include "source.h"
#include "parsecl.h"
#include "keys.h"
#include "win.h"
#ifdef __WIN__
#include "winvi.h"
#include "utils.h"
#include "font.h"

extern int MouseX;
extern int MouseY;
#endif

static hooktype hookRun;
static char     *srcHookData;

/*
 * findHook - look for a hook routine
 */
static vars *findHook( char *which )
{
    char        foo[64];
    vars        *v;

    MySprintf( foo,"%shook",which );
    v = VarFind( foo, NULL );
    return( v );

} /* findHook */

/*
 * GetHookVar - get hook variable
 */
vars *GetHookVar( hooktype num )
{
    vars        *v;

    if( num == SRC_HOOK_WRITE ) {
        v = findHook( "Wr" );
    } else if( num & SRC_HOOK_READ ) {
        v = findHook( "Rd" );
    } else if( num & SRC_HOOK_BUFFIN ) {
        v = findHook( "Buffin" );
    } else if( num & SRC_HOOK_BUFFOUT ) {
        v = findHook( "Buffout" );
    } else if( num == SRC_HOOK_COMMAND ) {
        v = findHook( "Cmd" );
    } else if( num == SRC_HOOK_MODIFIED ) {
        v = findHook( "Mod" );
    } else if( num == SRC_HOOK_MENU ) {
        v = findHook( "Menu" );
    } else if( num == SRC_HOOK_MOUSE_LINESEL ) {
        v = findHook( "MLsel" );
    } else if( num == SRC_HOOK_MOUSE_CHARSEL ) {
        v = findHook( "MCsel" );
    } else if( num == SRC_HOOK_DDE ) {
        v = findHook( "DDE" );
    } else {
        v = NULL;
    }
    return( v );

} /* GetHookVar */

/*
 * srcHook - run a specified source hook
 */
static int srcHook( hooktype num, int lastrc )
{
    vars        *v;
    int         ln,rc;

    if( hookRun & num ) {
        return( lastrc );
    }

    /*
     * check script type
     */
    v = GetHookVar( num );
    if( num == SRC_HOOK_COMMAND &&  v != NULL ) {
        VarAddGlobal( "Com", CommandBuffer );
    }
    if( num == SRC_HOOK_MODIFIED && v != NULL ) {
        lastrc = LastEvent;
    }

    /*
     * run script, if we have one
     */
    if( v != NULL ) {

        /*
         * set up for and run script
         */
        hookRun |= num;
        LastRetCode = lastrc;
        rc = Source( v->value, srcHookData, &ln );

        /*
         * if we had a command hook, look for replacement variable
         */
        if( num == SRC_HOOK_COMMAND ) {
            v = VarFind( "Com", NULL );
            if( v != NULL ) {
                strcpy( CommandBuffer, v->value );
            }
        }

        /*
         * we are done now, reset and go back
         */
        LastRetCode = ERR_NO_ERR;
        hookRun &= ~num;
        DCUpdateAll();
        return( rc );

    }
    return( lastrc );

} /* srcHook */

/*
 * SourceHook - activate source hook, no data
 */
int SourceHook( hooktype num, int lastrc )
{
    char        data[1];

    data[0] = 0;
    srcHookData = data;
    return( srcHook( num, lastrc ) );

} /* SourceHook */

/*
 * SourceHookData - activate source hook with data
 */
int SourceHookData( hooktype num, char *data )
{
    int         rc;

    srcHookData = data;
    rc = srcHook( num, ERR_NO_ERR );
    return( rc );

} /* SourceHookData */

/*
 * HookScriptCheck - check for hook scripts
 */
void HookScriptCheck( void )
{
    if( findHook( "Rd" ) != NULL ) {
        ReadErrorTokens();
    }

} /* HookScriptCheck */


/*
 * InvokeColSelHook - invoke column hook with specified data
 */
int InvokeColSelHook( int sc, int ec )
{
    int         j,i;
    char        wordbuff[MAX_STR];
    char        data[MAX_STR+32];
    int         lne;
    #ifndef __WIN__
        int     x1;
        bool    has_bord;
    #endif

#ifndef __WIN__
    has_bord = WindowAuxInfo( CurrentWindow, WIND_INFO_HAS_BORDER );
    x1 = WindowAuxInfo( CurrentWindow, WIND_INFO_X1 );
    if( LastEvent != VI_KEY( MOUSEEVENT ) ) {
        lne = WindowAuxInfo( CurrentWindow, WIND_INFO_Y1 ) +
                    CurrentLineNumber - TopOfPage + has_bord;
    } else {
        lne = MouseRow;
    }
#else
    if( LastEvent != VI_KEY( FAKEMOUSE ) ) {
        lne = ( CurrentLineNumber - TopOfPage ) * FontHeight( WIN_FONT( &EditWindow ) );
    } else {
        lne = MouseY;
    }
#endif

    j = 0;
    if( ec-sc >= MAX_STR ) {
        ec = sc+MAX_STR-2;
    }
    for( i=sc-1;i<=ec-1;i++ ) {
        wordbuff[j++] = CurrentLine->data[i];
    }
    wordbuff[j] = 0;
#ifndef __WIN__
    sc = x1+VirtualCursorPosition2( sc ) - LeftColumn;
    ec = x1+VirtualCursorPosition2( ec ) - LeftColumn;
    if( !has_bord ) {
        sc--;
        ec--;
    }
#else
    sc = MyTextExtent( CurrentWindow, WIN_STYLE( &EditWindow ), &CurrentLine->data[ 0 ], sc );
    ec = MyTextExtent( CurrentWindow, WIN_STYLE( &EditWindow ), &CurrentLine->data[ 0 ], ec );
#endif
    MySprintf( data,"\"%s\" %d %d %d %d", wordbuff, lne, sc, ec, ec - sc + 1 );
    return( SourceHookData( SRC_HOOK_MOUSE_CHARSEL, data ) );

} /* InvokeColSelHook */


/*
 * InvokeLineSelHook - invoke the mouse selection
 */
int InvokeLineSelHook( linenum s, linenum e )
{
    char        tmp[32];
    int         lne,col;
    #ifndef __WIN__
        bool    has_bord;
    #endif

#ifndef __WIN__
    if( LastEvent != VI_KEY( MOUSEEVENT ) ) {
        has_bord = WindowAuxInfo( CurrentWindow, WIND_INFO_HAS_BORDER );
        lne = WindowAuxInfo( CurrentWindow, WIND_INFO_Y1 ) +
                    CurrentLineNumber - TopOfPage + has_bord;
        col = WindowAuxInfo( CurrentWindow, WIND_INFO_X1 ) +
                        VirtualCursorPosition()-LeftColumn - 1 + has_bord;
        if( col < 0 ) {
            col = 0;
        }
    } else {
        col = MouseCol;
        lne = MouseRow;
    }
#else
    if( LastEvent != VI_KEY( FAKEMOUSE ) ) {
        /* assume we're not in insert mode *ouch* */
        col = PixelFromColumnOnCurrentLine( CurrentColumn );
        lne = ( CurrentLineNumber - TopOfPage ) * FontHeight( WIN_FONT( &EditWindow ) );
    } else {
        col = MouseX;
        lne = MouseY;
    }
#endif
    MySprintf( tmp,"%d %d %l %l", lne, col, s, e );
    return( SourceHookData( SRC_HOOK_MOUSE_LINESEL, tmp ) );

} /* InvokeLineSelHook */

/*
 * InvokeMenuHook - invoke the menu hook
 */
int InvokeMenuHook( int menunum, int line )
{
    char        tmp[16];
    int         rc;

    MySprintf( tmp,"%d %d", menunum, line );
    rc = SourceHookData( SRC_HOOK_MENU, tmp );
    return( rc );

} /* InvokeMenuHook */

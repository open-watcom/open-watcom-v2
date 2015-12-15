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
* Description:  Invoke online help (F1 key).
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgwind.h"
#include "dbgio.h"
#include "dbgerr.h"
#include "dbgmem.h"
#include "dui.h"
#include "strutil.h"
#include "wndhelp.h"


extern a_window         *WndMain;

#define HELPNAME "wd"

static gui_help_instance Handle;

void InitHelp( void )
{
    Handle = GUIHelpInit( WndGui( WndMain ), HELPNAME ".hlp", "Open Watcom Debugger Help" );
}

void FiniHelp( void )
{
    if( Handle != 0 ) {
        GUIHelpFini( Handle, WndGui( WndMain ), HELPNAME ".hlp" );
    }
}

#if !defined(__GUI__)
static void LocateHelpFile( void )
{
    handle              h;
#if !defined(__UNIX__)
    char                buff[1024];
#endif

    h = LocalFullPathOpen( HELPNAME, strlen( HELPNAME ), "ihp", TxtBuff, TXT_LEN );
    if( h != NIL_HANDLE ) {
        FileClose( h );
        return;
    }
#if !defined(__UNIX__)
    if( DUIEnvLkup( "WWINHELP", buff, sizeof( buff ) ) == 0 ) {
        Error( ERR_NONE, LIT_ENG( ERR_FILE_NOT_OPEN ), TxtBuff );
    }
    StrCopy( ".ihp", StrCopy( HELPNAME, StrCopy( "\\", StrCopy( buff, TxtBuff ) ) ) );
    h = FileOpen( TxtBuff, OP_READ );
    if( h != NIL_HANDLE ) {
        FileClose( h );
        return;
    }
#endif
    Error( ERR_NONE, LIT_ENG( ERR_FILE_NOT_OPEN ), TxtBuff );
}
#endif


void DoProcHelp( gui_help_actions action )
{
#if defined(__GUI__)
    StrCopy( HELPNAME ".hlp", TxtBuff );
#else
    LocateHelpFile();
#endif
    if( GUIShowHtmlHelp( Handle, WndGui( WndMain ), action, HELPNAME ".chm", "" ) ) {
        return;
    }
    if( Handle == 0 || !GUIShowHelp( Handle, WndGui( WndMain ), action, TxtBuff, "" ) ) {
        Error( ERR_NONE, LIT_ENG( ERR_FILE_NOT_OPEN ), TxtBuff );
    }
}

void ProcHelp( void )
{
    DoProcHelp( GUI_HELP_CONTENTS );
}

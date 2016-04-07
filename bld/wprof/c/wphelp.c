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
* Description:  Execution profiler (console version) online help processing.
*
****************************************************************************/


#include <string.h>

#include "common.h"
#include "aui.h"
#include "dip.h"
#include "msg.h"
#include "utils.h"
#include "wphelp.h"

#include "clibext.h"

#define HELPNAME        "wprof.hlp"
#define HTMLHELPNAME    "wprof.chm"

extern a_window             *WndMain;
extern char                 *HelpPathList;

STATIC gui_help_instance    helpHandle;


void WPInitHelp( void )
/*********************/
{
    helpHandle = GUIHelpInit( WndGui( WndMain ), HELPNAME,
                              "Open Watcom Profiler Help" );
}



void WPFiniHelp( void )
/*********************/
{
    GUIHelpFini( helpHandle, WndGui( WndMain ), HELPNAME );
}



void WPProcHelp( gui_help_actions action )
/****************************************/
{
    char        help_name[_MAX_PATH2];
#if !defined( __WINDOWS__ ) && !defined( __NT__ ) && !defined( __OS2_PM__ )
    char *      filename;
#endif

#ifdef __NT__
    if( GUIShowHtmlHelp( helpHandle, WndGui( WndMain ), action, HTMLHELPNAME, "" ) ) {
        return;
    }
#endif
#if defined( __WINDOWS__ ) || defined( __NT__ ) || defined( __OS2_PM__ )
    strcpy( help_name, HELPNAME );
#else
    filename = FindFile( help_name, "wprof.ihp", HelpPathList );
    if( filename == NULL ) {
        ErrorMsg( LIT( Unable_To_Open_Help ), "wprof.ihp" );
        return;
    }
#endif
    GUIShowHelp( helpHandle, WndGui( WndMain ), action, help_name, "" );
}

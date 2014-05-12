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


#include "wdeglbl.h"
#include "wrdll.h"
#include "wrselft.h"
#include "wdemain.h"
#include "wdeopts.h"
#include "wdeselft.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

/****************************************************************************/
/* external variables                                                       */
/****************************************************************************/

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

WRFileType WdeSelectFileType( char *name, bool is32bit )
{
    WRFileType  file_type;
    HWND        parent;
    bool        use_wres;
    FARPROC     cb;

    cb = MakeProcInstance( (FARPROC)WdeHelpRoutine, WdeGetAppInstance() );
    use_wres = WdeGetOption( WdeOptIsWResFmt );
    parent  = WdeGetMainWindowHandle();
    file_type = WRSelectFileType( parent, name, is32bit, use_wres, cb );
    FreeProcInstance( (FARPROC)cb );

    return( file_type );
}

bool WdeIsFileAnRCFile( char *name )
{
    WRFileType  file_type;

    file_type = WRGuessFileType( name );
    if( file_type == WR_WIN_RC || file_type == WR_WIN_RC_DLG ) {
        return( TRUE );
    }
    return( FALSE );
}

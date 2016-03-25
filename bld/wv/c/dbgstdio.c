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


#include <string.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbglit.h"
#include "dbgerr.h"
#include "dbgscan.h"
#include "remcore.h"


static void StdioRedirect( bool input )
{
    const char  *start;
    size_t      len;
    char        buff[160];

    ScanItem( true, &start, &len );
    memcpy( buff, start, len );
    buff[len] = NULLCHAR;
    ReqEOC();
    if( !Redirect( input, buff ) ) {
        if( len == 0 ) {
            Error( ERR_NONE, input ? LIT_ENG( ERR_CANNOT_RESTORE_STDIN ) :
                                     LIT_ENG( ERR_CANNOT_RESTORE_STDOUT ) );
        } else {
            Error( ERR_NONE, LIT_ENG( ERR_FILE_NOT_OPEN ), buff );
        }
    }
}

/*
 * StdInNew -- redirect the STDIN file
 */

extern void StdInNew( void )
{
    StdioRedirect( true );
}


/*
 * StdOutNew -- redirect the STDOUT file
 */

extern void StdOutNew( void )
{
    StdioRedirect( false );
}

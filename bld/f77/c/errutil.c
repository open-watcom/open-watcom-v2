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
* Description:  message processing utilities
*
****************************************************************************/

#include "ftnstd.h"
#include "ftextvar.h"
#include "blderr.h"
#include "errrtns.h"


const unsigned char  __FAR *PGrpCodes = GrpCodes;

void    BldErrCode( unsigned int error_num, char *buffer )
// Build error code.
{
    const unsigned char __FAR *group;
    unsigned int        num;

    group = &PGrpCodes[( error_num / 256 ) * 3];
    num = ( error_num % 256 ) + 1;
    buffer[0] = ' ';
    buffer[1] = group[0];
    buffer[2] = group[1];
    buffer[3] = '-';
    buffer[4] = num / 10 + '0';
    buffer[5] = num % 10 + '0';
    buffer[6] = NULLCHAR;
}


#if !defined( __RT__ ) && !defined( __WFL__ )

extern const unsigned char  __FAR CaretTable[];
const unsigned char         __FAR *PCaretTable = CaretTable;

uint    CarrotType( uint error_num )
// Return the type of caret.
{
    const unsigned char __FAR *group;
    const unsigned char __FAR *grp;
    uint                idx;

    idx = error_num % 256;
    group = &PGrpCodes[( error_num / 256 ) * 3];
    for( grp = PGrpCodes; grp != group; grp += 3 ) {
        idx += *(grp + 2);
    }
    return( PCaretTable[idx] );
}

#endif

void    MsgFormat( char *msg, char *buff, ... ) {
// Format a message.
    va_list     args;

    va_start( args, buff );
    Substitute( msg, buff, args );
    va_end( args );
}

void    MsgBuffer( uint msg, char *buff, ...  ) {
// Format message to buffer.
    va_list     args;

    va_start( args, buff );
    __BldErrMsg( msg, buff, args );
    va_end( args );
}

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

#include "msgdscr.h"


static char MsgDscr::filename[ _MAX_FNAME ]; // buffer for file name (one only)


MsgDscr::MsgDscr(               // CONSTRUCTOR
    const char *prefix )        // - prefix
: file_prefix( prefix )
, max_msg( 0 )
{
    ::memset( file, 0, sizeof( file ) );
}


int MsgDscr::AddFile(           // ADD A NEW FILE FOR A MESSAGE
    int msg_no )                // - message number
{
    if( msg_no > max_msg ) {
        max_msg = msg_no;
    }
    return ++file[ msg_no ];
}


int MsgDscr::GetNextFile(       // GET NEXT SUB-FILE NO. FOR A MESSAGE
    int msg_no,                 // - message no.
    int last_no )               // - last no.
{
    ++last_no;
    if( last_no > file[ msg_no ] ) {
        last_no = 0;
    }
    return last_no;
}


int MsgDscr::GetMaxMsgNo(       // GET MAXIMUM MESSAGE NUMBER
    void )
{
    return max_msg;
}


const char* MsgDscr::GetFileName( // GET FILE NAME
    int msg_no,                 // - message no.
    unsigned char file_no )     // - file number
{
    char buf[ 8 ];              // - buffer for line #
    char last_char[2];          // - buffer for last character

    ::strcpy( filename, file_prefix );
    ::strcpy( buf, "000" );
    ::itoa( msg_no, buf+3, 10 );
    ::strcat( filename, buf + ::strlen( buf ) - 4 );
    last_char[0] = 'A' - 1 + file_no;
    last_char[1] = 0;
    ::strcat( filename, last_char );
    return (const char *)filename;
}

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
#include <stdlib.h>
#include "disasm.h"
#include "wdismsg.h"

static  char            *buffer;
static  uint_16         bufPos;


char  *MakeFileName( char *str, char *ext )
/*****************************************/
{
    _makepath( NameBuff, NULL, NULL, str, ext );
    return( NameBuff );
}


char  *ParseName( char *ptr1, char *ptr2 )
/****************************************/
{
    char                *name;
    int                 len;

    len = ptr2 - ptr1;
    name = AllocMem( len + 1 );
    memcpy( name, ptr1, len );
    name[ len ] = '\0';
    return( name );
}


void  PutString( char *str )
/**************************/
{
    size_t              len;

    len = strlen( str );
    if( bufPos + len > MAX_LINE_LEN - 1 ) {
        len = ( MAX_LINE_LEN - 1 ) - bufPos;
    }
    memcpy( buffer + bufPos, str, len );
    bufPos += len;
}


void  FPutEnd( FILE *io_file )
/****************************/
{
    buffer[bufPos] = 0;
    FPutTxtRec( io_file, buffer, bufPos );
    bufPos = 0;
    if( ferror( io_file ) ) {
        SysError( ERR_WRITE_FILE, true );
    }
}


void  InitOutput()
/****************/

{
    buffer = AllocMem( MAX_LINE_LEN );
    bufPos = 0;
    NameBuff = AllocMem( MAX_LINE_LEN > _MAX_PATH2 ? MAX_LINE_LEN : _MAX_PATH2 );
}


void OpenSource()
/***************/

{
    Source = OpenTxtRead( SrcName );
    if( Source == NULL ) {
        Source = OpenTxtRead( MakeFileName( SrcName, "c" ) );
        if( Source == NULL ) {
            Source = OpenTxtRead( MakeFileName( SrcName, "cpp" ) );
            if( Source == NULL ) {
                Source = OpenTxtRead( MakeFileName( SrcName, "for" ) );
                if( Source == NULL ) {
                    DirectPuts( MSG_NOT_OPEN_SOURCE, stderr );
                    fputs( SrcName, stderr );
                    fputs( "\n", stderr );
                }
            }
        }
    }
}

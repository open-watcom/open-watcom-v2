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
* Description:  Debugger file I/O layer interface.
*
****************************************************************************/


#include "sysdef.h"
#include "bool.h"

typedef int             handle;
typedef error_idx       rc_erridx;

typedef unsigned_8 seek_method; enum {
    DIO_SEEK_ORG,
    DIO_SEEK_CUR,
    DIO_SEEK_END
};

typedef unsigned_8 open_access; enum {
    OP_READ         = 0x01,
    OP_WRITE        = 0x02,
    OP_CREATE       = 0x04,
    OP_TRUNC        = 0x08,
    OP_APPEND       = 0x10,
    OP_EXEC         = 0x10, // only valid with OP_CREATE - ran out of bits
    OP_REMOTE       = 0x20,
    OP_LOCAL        = 0x40,
    OP_SEARCH       = 0x80
};

#define NIL_HANDLE      ( (handle) -1 )

#define ERR_RETURN      ( (unsigned)-1 )

#define STD_IN          0
#define STD_OUT         1
#define STD_ERR         2

unsigned        ReadStream( handle, void *, unsigned );
unsigned        ReadText( handle, void *, unsigned );

unsigned        WriteStream( handle, const void *, unsigned );
unsigned        WriteText( handle, const void *, unsigned );

unsigned long   SeekStream( handle, long, seek_method );

handle          FileOpen( char const *, open_access );
rc_erridx       FileClose( handle );
rc_erridx       FileRemove( char const *, open_access );
open_access     FileHandleInfo( handle );
char            *FileLoc( char const *, open_access * );

char            *SysErrMsg( error_idx, char * );
error_idx       StashErrCode( sys_error, open_access );

char            *SkipPathInfo( char const *, open_access );
char            *ExtPointer( char const *, open_access );
char            *AppendPathDelim( char *, open_access );
unsigned        MakeFileName( char *, const char *, const char *, open_access );

bool            FindWritable( char const *, char * );

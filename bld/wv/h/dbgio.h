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


typedef unsigned long   sys_handle;
typedef unsigned long   sys_error;

#define NIL_SYS_HANDLE  ((sys_handle)-1)
#define SYS_ERR_OK      ((sys_error)0)

typedef         unsigned handle;
typedef unsigned_8 seek_method; enum {
    SEEK_ORG,
    SEEK_CUR,
    SEEK_END
};
typedef unsigned_8 open_access; enum {
    OP_READ         = 0x01,
    OP_WRITE        = 0x02,
    OP_CREATE       = 0x04,
    OP_TRUNC        = 0x08,
    OP_APPEND       = 0x10,
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
unsigned        FileClose( handle );
unsigned        FileRemove( char const *, open_access );
open_access     FileHandleInfo( handle );
char            *FileLoc( char const *, open_access * );

char            *SysErrMsg( unsigned, char * );
unsigned        StashErrCode( sys_error, open_access );

char            *SkipPathInfo( char const *, open_access );
char            *ExtPointer( char const *, open_access );
char            *AppendPathDelim( char *, open_access );
unsigned        MakeFileName( char *, char *, char *, open_access );

bool            FindWritable( char const *, char * );

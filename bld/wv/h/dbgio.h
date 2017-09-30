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


#include <stddef.h>
#include "sysdef.h"


typedef enum {
    DIO_SEEK_ORG,
    DIO_SEEK_CUR,
    DIO_SEEK_END
} seek_method;

typedef enum {
    OP_READ         = 0x0001,
    OP_WRITE        = 0x0002,
    OP_CREATE       = 0x0004,
    OP_TRUNC        = 0x0008,
    OP_APPEND       = 0x0010,
    OP_REMOTE       = 0x0020,
    OP_LOCAL        = 0x0040,
    OP_SEARCH       = 0x0080,
    OP_EXEC         = 0x0100,
} obj_attrs;

#define ERR_RETURN      ((size_t)-1)
#define ERR_SEEK        ((unsigned long)-1L)

#define STD_IN          0
#define STD_OUT         1
#define STD_ERR         2

extern size_t           ReadStream( file_handle, void *, size_t );
extern size_t           ReadText( file_handle, void *, size_t );

extern size_t           WriteStream( file_handle, const void *, size_t );
extern size_t           WriteNL( file_handle );
extern size_t           WriteText( file_handle, const void *, size_t );

extern unsigned long    SeekStream( file_handle, long, seek_method );

extern file_handle      FileOpen( char const *, obj_attrs );
extern error_handle     FileClose( file_handle );
extern error_handle     FileRemove( char const *, obj_attrs );
extern const char       *FileLoc( char const *, obj_attrs * );
extern obj_attrs        FileHandleInfo( file_handle );
extern sys_handle       GetSystemHandle( file_handle );

extern char             *SysErrMsg( error_handle, char *buff );
extern error_handle     StashErrCode( sys_error, obj_attrs );
extern error_handle     GetLastErr( void );
extern sys_error        GetSystemErrCode( error_handle );

extern const char       *SkipPathInfo( char const *, obj_attrs );
extern const char       *ExtPointer( char const *, obj_attrs );
extern char             *AppendPathDelim( char *, obj_attrs );
extern size_t           MakeFileName( char *result, const char *name, const char *ext, obj_attrs oattrs );
extern const char       *RealFName( char const *name, obj_attrs *oattrs );
extern bool             IsAbsolutePath( const char *path );
extern char             *AppendPathDelim( char *path, obj_attrs oattrs );
extern const char       *ExtPointer( char const *path, obj_attrs oattrs );
extern file_handle      LclStringToFullName( const char *name, size_t len, char *full );
extern file_handle      FullPathOpen( const char *name, size_t name_len, const char *ext, char *result, size_t max_result );
extern file_handle      LocalFullPathOpen( const char *name, size_t name_len, const char *ext, char *result, size_t max_result );
extern file_handle      PathOpen( const char *name, size_t name_len, const char *ext );
extern file_handle      LocalPathOpen( const char *name, size_t name_len, const char *ext );

#if !defined( BUILD_RFX )
extern bool             FindWritable( char const *src, char *dst );
#endif

extern void             WriteToPgmScreen( const void *buff, size_t len );
extern void             SysFileInit( void );
#if !defined( BUILD_RFX )
extern void             PathFini( void );
#endif
extern void             PathInit( void );

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
* Description:  Internal wlib I/O interface.
*
****************************************************************************/


typedef unsigned_32     file_offset;

#define READ_FILE_BUFFER_SIZE   2048
#define WRITE_FILE_BUFFER_SIZE  32768

typedef struct io_struct *libfile;
struct io_struct {
    char            *name;
    libfile         next;
    libfile         prev;
    FILE            *io;
    long            endpos;
    bool            write_to;
    size_t          buf_size;
    size_t          buf_pos;
    char            buffer[1];
};

#define LIBOPEN_READ    false
#define LIBOPEN_WRITE   true

extern void         InitLibIo( void  );
extern void         FiniLibIo( void );
extern libfile      LibOpen( const char *name, bool write_to );
extern size_t       LibRead( libfile io, void *buff, size_t len );
extern void         LibWrite( libfile io, const void *buff, size_t len );
extern void         LibClose( libfile io );
extern void         LibSeek( libfile io, long where, int whence );
extern long         LibTell( libfile io );
extern void         LibReadError( libfile io );
extern void         LibWriteError( libfile io );
extern void         BadLibrary( const char *name );

#ifdef __UNIX__
#define FNCMP strcmp
#else
#define FNCMP stricmp
#endif

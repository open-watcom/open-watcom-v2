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
* Description:  Novell NetWare NLM trap file I/O.
*
****************************************************************************/


#include "debugme.h"
#include <dos.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#undef TRUE
#undef FALSE
#undef NULL

#include "miniproc.h"
#if defined ( __NW40__ )
    #include "datamig.h"
#elif defined ( __NW30__ )
    #include "cconfig.h"
#endif

#include "locks.h"
#include "bits.h"
#include "nlmio.h"

extern LONG ConvertPathString(
                LONG stationNumber,
                BYTE base,
                BYTE *modifierString,
                LONG *volumeNumber,
                LONG *pathBase,
                BYTE *pathString,
                LONG *pathCount);


#define FIRST_HANDLE    5

#define     FILE_ATTRIB_MASK    (_A_NORMAL | _A_HIDDEN | _A_RDONLY)
/*
//  RWPRIVS | DENYW -
//  as 0x0B is 1011 I don't know which one is which or has two bits
//  though I would hazard a guess as RWPRIVS
*/
#define     FILE_OPEN_PRIVS 0x0B

/* From NLMCLIB.C */

extern int WriteStdErr( char *buff, int len );

int     ccode;
typedef enum {
        FILE_INVALID,
        FILE_DOS,
        FILE_SERVER
} ftype;

typedef struct {
        int     handle;
        long    (*routine)(long,long,void*,long);
        long    seekpos;
        long    filesize;
        ftype   file_type;
        int     handlenum;
} my_file;


#define NIL_DOS_HANDLE  ((short)0xFFFF)
#define NUM_FILES       (100-FIRST_HANDLE)

my_file                 Files[NUM_FILES];

static int AppendStr( char *dst, char *src )
{
    int         len;
    while( *dst ) {
        ++dst;
    }
    len = 0;
    while( *dst = *src ) {
        ++len;
        ++dst;
        ++src;
    }
    return( len );
}


static long WriteServer( long handle, long pos, void *buffer, long requested ) {

    LONG ccode;

    ccode = WriteFile( 0, handle, pos, requested, buffer );
    if( ccode != 0 ) return( 0 );
    return( requested );
}


static long ReadServer( long handle, long pos, void *buffer, long requested ) {

    LONG ccode, bytes_read;

                                                                    _DBG_IO(( "Reading server %8x at %d for %d\r\n", handle, pos, requested ));
    ccode = ReadFile( 0, handle, pos, requested, &bytes_read, buffer );
                                                                    _DBG_IO(( "-- Got %d\r\n", bytes_read ));
    if( ccode != 0 ) return( 0 );
    return( bytes_read );
}


static long ReadDOS( long handle, long pos, void *buffer, long requested )
{
   LONG ccode, bytes_read;

                                                                    _DBG_IO(( "Reading DOS %8x at %d for %d\r\n", handle, pos, requested ));
    ccode = INWDOSRead( handle, pos, buffer, requested, &bytes_read );
                                                                    _DBG_IO(( "-- Got %d\r\n", bytes_read ));
    if( ccode != 0 ) return( 0 );
    return( bytes_read );
}


static long OpenServer( LONG (*routine)(), char *name,
                 LONG *handle, LONG attr, LONG privs )
{
   LONG volumeNumber, pathBase, pathCount, entryNumber;
   BYTE pathString[128];
   BYTE fileName[256];
   void *Entry;

   fileName[1] = '\0';
   fileName[0] = AppendStr( (char *)fileName + 1, name );

   if (ConvertPathString(0, 0, fileName, &volumeNumber,
         &pathBase, pathString, &pathCount) != 0)
      /* The path is invalid */
      return (-1);

   /* Attempt to open the command file */
   return( routine(0, 1, volumeNumber, pathBase, pathString, pathCount, 0,
         attr, privs, PrimaryDataStream, handle, &entryNumber,
         &Entry ) );
}


static int ErrorCode( void )
{
    return( 0xFFFF0000 | ccode );
}


static my_file *FindFile( void )
{
    int i;
    my_file     *p;

    for( i = 0, p = Files; i < NUM_FILES; ++i, ++p ) {
        if( p->handle == 0 ) break;
    }
    if( i == NUM_FILES ) {
        Abend( "Debug server out of file handles\r\n" );
    }
    p->file_type = FILE_INVALID;
    p->seekpos = 0;
    p->routine = NULL;
    p->handlenum = i;
    return( p );
}


int IOCreat( char *name )
{

   LONG         handle;
   my_file      *p;

//  if( !MayRelinquishControl ) return( -1 );
                                                                    _DBG_IO(( "Creating %s. Open RC(%d)\r\n", name, ccode ));
    ccode = OpenServer( OpenFile, name, &handle,
                        FILE_ATTRIB_MASK, FILE_OPEN_PRIVS );
    if( ccode == 0 ) {
        ccode = WriteFile( 0, handle, 0, 0, "" );
    } else {
        ccode = OpenServer( CreateFile, name, &handle, 0, 0 );
                                                                    _DBG_IO(( "Creating %s. Create RC(%d)\r\n", name, ccode ));
    }
    if( ccode == 0 ) {
        p = FindFile();
        p->handle = handle;
        p->routine = ReadServer;
        p->seekpos = 0;
        p->filesize = 0;
        p->file_type = FILE_SERVER;
    }
    return( ccode ? ErrorCode() : ( p->handlenum + FIRST_HANDLE ) );
}

void StringToNLMPath( char *name, char *res )
{
    BYTE        isdos;
    LONG        handle;
    BYTE        filename[14];
    unsigned    i;

    /* right trim the name */
    i = strlen( name );
    for( ;; ) {
        if( i == 0 ) break;
        if( name[i-1] != ' ' ) break;
        --i;
    }
    name[i] = '\0';

    ccode = OpenFileUsingSearchPath( (BYTE *)name, &handle, &isdos,
                                     (BYTE *)res, filename, FALSE,
                    4, ".NLM", ".DSK", ".LAN", ".NAM" );
    if( ccode == 0 ) {
        AppendStr( res, (char *)filename );
        if( isdos ) {
            ccode = INWDOSClose( handle );
        } else {
            ccode = CloseFile(0, 1, handle );
        }
    } else {
        res[0] = '\0';
    }
 }

int IOOpen( char *openname, int openmode )
{
    BYTE        isdos;
    LONG        handle;
    BYTE        filename[14];
    BYTE        loadpath[256];
    my_file     *p;
    LONG        filesize;
    struct      find_t  dta;

//  if( !MayRelinquishControl ) return( -1 );
    if( openmode == O_RDONLY ) {
        ccode = OpenFileUsingSearchPath( (BYTE *)openname, &handle, &isdos,
                                         loadpath, filename, FALSE, 0 );
        AppendStr( (char *)loadpath, (char *)filename );
                                                                    _DBG_IO(( ( ccode==0 ? "Opened %s." : "" ), loadpath ));
    } else {
        ccode = OpenServer( OpenFile, openname, &handle,
                            FILE_ATTRIB_MASK, FILE_OPEN_PRIVS );
                                                                    _DBG_IO(( "Opened %s.", openname ));
        isdos = FALSE;
    }
                                                                    _DBG_IO(( " RC(%d). Handle=%8x\r\n", ccode, handle ));
    if( ccode == 0 ) {
        p = FindFile();
        p->handle = handle;
        p->seekpos = 0;
        p->file_type = isdos ? FILE_DOS : FILE_SERVER;
        if( isdos ) {
            p->routine = ReadDOS;
            ccode = INWDOSFindFirstFile( loadpath, 0, &dta );
            p->filesize = dta.size;
        } else {
            ccode = GetFileSize( 0, p->handle, &filesize );
            p->routine = ReadServer;
            p->filesize = filesize;
        }
        if( openmode != O_RDONLY ) {
            p->routine = NULL;
        }
                                                                    _DBG_IO(( ( ccode != 0 ? "    Size failed - ccode %d\r\n" : "" ), ccode ));
                                                                    _DBG_IO(( ( ccode == 0 ? "    Size is %d\r\n" : "" ), p->filesize ));
    }
    return( ccode ? ErrorCode() : ( p->handlenum + FIRST_HANDLE ) );
}

static void BadFile( void )
{
    Abend( "Debug server detected an invalid file (Close)\r\n" );
}


int IOClose( int closehandle )
{
    long        handle;
    int         index;

//  if( !MayRelinquishControl ) return( -1 );
    index = closehandle - FIRST_HANDLE;
    if( index < 0 || index > NUM_FILES ) return( 0 );
    handle = Files[ index ].handle;
    Files[ index ].handle = 0;
                                                                    _DBG_IO(( "Closing file %8x", handle ));
    if( index < 0 || index >= NUM_FILES || Files[ index ].file_type == FILE_INVALID ) {
        BadFile();
    }
    switch( Files[ index ].file_type ) {
    case FILE_DOS:
        ccode = INWDOSClose( handle );
        break;
    case FILE_SERVER:
        ccode = CloseFile(0, 1, handle );
        break;
    }
                                                                    _DBG_IO(( " RC(%d)\r\n", ccode ));
    return( ccode ? ErrorCode() : 0 );
}

int IOWriteConsole( char *buff, int buff_len )
{
    return( WriteStdErr( buff, buff_len ) );
}

int IOWrite( int writehandle, char *buff, int buff_len )
{
    my_file     *p;
    int         index;
    int         written;

//  if( !MayRelinquishControl ) return( -1 );
    index = writehandle - FIRST_HANDLE;
    if( index < 0 || index >= NUM_FILES || Files[ index ].file_type != FILE_SERVER ) {
        BadFile();
    }
    p = &Files[ index ];
    written = WriteServer( p->handle, p->seekpos, buff, buff_len );
    if( written != buff_len ) {
        written = ErrorCode();
    }
    return( written );
}


long IOSeek( int seekhandle, int seekmode, long seekpos )
{
    long        pos;
    int         index;

//  if( !MayRelinquishControl ) return( -1 );
    index = seekhandle - FIRST_HANDLE;
    if( index < 0 || index >= NUM_FILES || Files[ index ].file_type == FILE_INVALID ) {
        BadFile();
    }
    pos = Files[ index ].seekpos;
    switch( seekmode ) {
    case SEEK_SET:
        pos = seekpos;
        break;
    case SEEK_CUR:
        pos += seekpos;
        break;
    case SEEK_END:
        pos = Files[ index ].filesize + seekpos;
        break;
    }
    if( pos < 0 ) {
        return( ccode ? ErrorCode() : -1 );
    } else {
        Files[ index ].seekpos = pos;
        return( pos );
    }
}


int IORead( int readhandle, char *buff, int len )
{
    my_file     *p;
    int         index;
    int         amt_read;

//  if( !MayRelinquishControl ) return( -1 );
    index = readhandle - FIRST_HANDLE;
    if( index < 0 || index >= NUM_FILES || Files[ index ].file_type == FILE_INVALID ) {
        BadFile();
    }
    p = &Files[ index ];
    amt_read = p->routine( p->handle, p->seekpos, buff, len );
    p->seekpos += amt_read;
    return( amt_read );
}

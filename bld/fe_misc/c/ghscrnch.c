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


// GHSCRNCH -- utility to massage a .GH file into canonical format and to
//             update the target when it differs from the generated file
//

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys\stat.h>
#include <sys\utime.h>

typedef struct record record;
struct record                   // RECORD
{
    record* _next;              // - next in list
    char _data[1];              // - data
};

#ifndef TRUE
    #define TRUE 1
    #define FALSE 0
#endif

static int tgt_write = FALSE;       // true ==> write out target
static int have_src;                // true ==> have a source record
static int have_tgt;                // true ==> have a target record
static FILE* in;                    // input file
static FILE* tgt;                   // target file
static char target[1024];           // target record
static char source[1024];           // source record
static char const* src_rec;         // canonical source record
static unsigned src_size;           // size of source record
static record* recordsList = NULL;  // list of canonical records
static record* recordsEnd;          // end of list of canonical records


static void helpInfo            // DUMP HELP INFORMATION
    (void)
{
    puts( "GHSCRNCH source target time"                                     );
    puts( "   source, target, and time are files"                           );
    puts( "   target is replaced by source (in canonical format) when the"  );
    puts( "      canonical format of the source differs from the target."   );
    puts( "   time file has modification time set to that of source file."  );
}


static void message             // WRITE MESSAGE
    ( char const * prefix       // - prefix
    , char const * frag         // - first fragment
    , va_list frags )           // - message fragments
{
    fputs( prefix, stdout );
    for( ; ; ) {
        if( 0 == frag ) break;
        fputc( ' ', stdout );
        fputs( frag, stdout );
        frag = va_arg( frags, char const * );
    }
    fputc( '\n', stdout );
}


static void notemsg             // PRINT A NOTE MESSAGE
    ( char const* frags         // - message fragments
    , ... )
{
    va_list list;
    va_start( list, frags );
    message( "*** NOTE ***", frags, list );
    va_end( list );
}


static void errmsg              // PRINT AN ERROR MESSAGE
    ( char const* frags         // - message fragments
    , ... )
{
    va_list list;
    va_start( list, frags );
    message( "*** ERROR ***", frags, list );
    va_end( list );
}


static void readTgt             // READ TARGET RECORD
    ( void )
{
    if( NULL == fgets( target, sizeof( target ), tgt ) ) {
        have_tgt = FALSE;
    } else {
        have_tgt = TRUE;
    }
}


static void readSrc             // READ SOURCE RECORD
    ( void )
{
    have_src = FALSE;
    while( ! have_src ) {
        char const* rp;
        if( NULL == fgets( source, sizeof( source ), in ) ) break;
        for( rp = source; *rp == ' '; ++rp );
        if( *rp != '\n' ) {
            char * ep;
            src_rec = rp;
            ep = (char*)( rp + strlen( rp ) - 1 );
            for( ; ; ) {
                -- ep;
                if( *ep != ' ' ) break;
            }
            ep[1] = '\n';
            ep[2] = '\0';
            src_size = ep - rp + 2;
            have_src = TRUE;
            break;
        }
    }
}


static int storeSrc             // SAVE SOURCE RECORD
    ( void )
{
    int retn;
    record* rec = (record*)malloc( sizeof( record ) + src_size );
    if( NULL == rec ) {
        retn = FALSE;
    } else {
        rec->_next = NULL;
        memcpy( rec->_data, src_rec, src_size + 1 );
        if( NULL == recordsList ) {
            recordsList = rec;
        } else {
            recordsEnd->_next = rec;
        }
        recordsEnd = rec;
        retn = TRUE;
    }
    return retn;
}


static int setTimeFile         // SET MODIFICATION TIME FOR TIME FILE
    ( char const * timeFile     // - name of time file
    , struct utimbuf const * modif ) // - modication time
{
    int retn;
    int try_create = TRUE;
    for( ; ; ) {
        if( 0 == utime( timeFile, modif ) ) {
            retn = 0;
            break;
        }
        if( try_create ) {
            FILE* created = fopen( timeFile, "wt" );
            try_create = FALSE;
            if( 0 != created ) {
                fclose( created );
                continue;
            }
        }
        errmsg( "cannot set time for"
              , timeFile
              , NULL );
        retn = 2;
        break;
    }
    return retn;
}


int main                        // MAIN-LINE
    ( int arg_count             // - # arg.s
    , char const * args[] )     // - arguments
{
    int retn = 0;
    if( arg_count != 4 ) {
        helpInfo();
        retn = 1;
    } else {
        in = fopen( args[1], "rt" );
        if( NULL == in ) {
            errmsg( "cannot open"
                  , args[1]
                  , NULL );
            retn = 2;
        } else {
            tgt = fopen( args[2], "rt" );
            if( 0 == tgt ) {
                notemsg( "creating"
                       , args[2]
                       , NULL );
                tgt_write = TRUE;
            } else {
                readTgt();
            }
            readSrc();
            for( ; ; ) {
                if( ! have_src ) break;
                if( ! tgt_write ) {
                    if( have_tgt ) {
                        if( src_size == strlen( target )
                         && 0 == memcmp( target, src_rec, src_size ) ) {
                            readTgt();
                        } else {
                            notemsg( "replacing"
                                   , args[2]
                                   , NULL );
                            tgt_write = TRUE;
                        }
                    } else {
                        tgt_write = TRUE;
                    }
                }
                if( ! storeSrc() ) {
                    errmsg( "ran out of memory", NULL );
                    retn = 2;
                    break;
                }
                readSrc();
            }
            if( have_tgt ) {
                tgt_write = TRUE;
            }
            fclose( in );
            if( NULL != tgt ) {
                fclose( tgt );
            }
            if( 0 == retn ) {
                struct stat src_stats;
                struct utimbuf mod_times;
                stat( args[1], &src_stats );
                mod_times.actime = src_stats.st_mtime;
                mod_times.modtime = src_stats.st_mtime;
                if( tgt_write && retn == 0 ) {
                    tgt = fopen( args[2], "wt" );
                    if( 0 == tgt ) {
                        errmsg( "cannot write file"
                              , args[2]
                              , NULL );
                        retn = 2;
                    } else {
                        record* rec = recordsList;
                        while( NULL != rec ) {
                            if( NULL == fputs( rec->_data, tgt ) ) {
                                errmsg( "i/o error writing file"
                                      , args[2]
                                      , NULL );
                                retn = 2;
                                break;
                            }
                            rec = rec->_next;
                        }
                        fclose( tgt );
                        if( 0 == retn ) {
                            utime( args[2], &mod_times );
                            retn = setTimeFile( args[3], &mod_times );
                        }
                    }
                } else {
                    retn = setTimeFile( args[3], &mod_times );
                }
            }
        }
        remove( args[1] );
    }
    return retn;
}

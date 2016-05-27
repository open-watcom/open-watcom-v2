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
* Description:  Yet another grep utility.
*
****************************************************************************/


#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <process.h>
#include <signal.h>
#include <limits.h>
#include "wio.h"
#if defined( __UNIX__ )
    #include <dirent.h>
#else
    #include <direct.h>
#endif

#define  BINC                   1024
#if defined(__NT__)
#define  BSIZE                  (256 * BINC)
#else
#define  BSIZE                  (128 * BINC)
#endif
#define  MAX_SRCH_STRINGS       2048

typedef int wbool;

#if defined(__UNIX__)
    #define READABLE   (S_IRUSR|S_IRGRP|S_IROTH)
    #define WRITABLE   (S_IWUSR|S_IWGRP|S_IWOTH)
    #define EXECUTABLE (S_IXUSR|S_IXGRP|S_IXOTH)
#else
    #define READABLE   (S_IREAD)
    #define WRITABLE   (S_IWRITE)
    #define EXECUTABLE (S_IEXEC)
#endif


typedef struct dirstack {
    struct dirstack     *prev;
    char                 name[ _MAX_PATH ];
} dirstack;

// Default list of file extensions that we will not search when -g is specified.

char    *DefIgnoredExt = {
        ".aps"
        ".avi"
        ".bin"
        ".bmp"
        ".class"
        ".cur"
        ".db"
        ".dll"
        ".doc"
        ".exe"
        ".fts"
        ".gid"
        ".gif"
        ".hlp"
        ".ico"
        ".ilk"
        ".jar"
        ".jpg"
        ".lib"
        ".mch"
        ".mcp"
        ".mp3"
        ".mpg"
        ".nlm"
        ".obj"
        ".pch"
        ".pdb"
        ".res"
        ".sym"
        ".tdt"
        ".tlb"
        ".xls"
        ".zip"
};

char            **IgnoreList = NULL;
char            *IgnoreListBuffer = NULL;
unsigned        IgnoreListCnt = 0;
char            CurrPattern[ _MAX_FNAME + _MAX_EXT ];
char            PathBuff[ _MAX_PATH ];
int             DoneFlag    = 0;
int             RecurLevels = 1;
dirstack       *Stack       = NULL;
char           *FName;
char            *Buff;
size_t          BSize;
wbool           PrtFn;
wbool           PrtAll;
wbool           PrtCount;
wbool           PrtMatch;
wbool           PrtLines;
wbool           PrtFiles;
wbool           PrtPath;
wbool           QuitFirst;
wbool           OnePerFile;
wbool           Similar;
wbool           NoSubstring;
int             Context;
int             ExitStatus;
long            MatchCount;
long            TotalMatchCount;
FILE           *FOut;
char           *FOutFmt;
char            *SrchStrings[ MAX_SRCH_STRINGS ];
unsigned        Recs;
char            StdoutBuf[ 512 ];       // used for files w/o \r
int             CurOutMode;
int             FileMode = 0;

unsigned char   CharInSrchStrings[] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

unsigned char CharTrans[] = {
0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,
0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,
0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,
0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,
0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf,
0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef,
0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,0xfd,0xfe,0xff
};

char *Help[] = {
"Usage: wgrep {option}* string [file [file [file...]]]",
"",
"    string - the search string",
"           or @FILE : FILE contains a list of search strings, one per line",
"           or @CON  : You will be prompted for search strings",
"           Escape characters are:",
"               \\[ matches <",
"               \\] matches >",
"               \\! matches |",
"               \\\\ matches \\",
"               \\t matches TAB character",
"               \\^ matches start of line (must double up ^ on command line)",
"               \\$ matches end of line",
"               \\? matches ?",
"           4NT users must double up % to search for %",
"           4NT users must place quotes around \\^c to avoid conversion of ^c",
"               to a single byte value (e.g., ^n becomes 0x0A)",
"",
"    file   - A wild card file name to be searched - defaults to '*.*'.",
"           - if file is a directory, all files in that directory are searched",
"           - file name patterns may be separated by spaces or semicolons",
"           - file may refer to an environment variable using % (e.g., %include)",
"",
"    option -a        Print each file name as it is searched",
"           -c        Print only number of lines that contain matches",
"           -d        Display command line arguments",
"           -f        Don't print file names",
"           -g[list]  Ignore files with specific extensions.",
"                     The default list of extensions includes:",
"                       .aps .avi .bin .bmp .class .cur .db .dll .doc .exe .fts",
"                       .gid .gif .hlp .ico .ilk .jar .jpg .lib .mch .mcp .mp3",
"                       .mpg .nlm .obj .pch .pdb .res .sym .tdt .tlb .xls .zip",
"                     The default list can be overridden using the WGREPIGNORE",
"                     environment variable. The list of extensions are of the",
"                     the form '.exe.obj.dll'. Additional extensions may be",
"                     included after the -g (e.g., -g.prj.tmp.log).",
"           -h        Display this usage message",
"           -i        Make search case insensitive",
"           -l        Print matching file names only",
"           -n        Print 'File:' and '1234:' on beginning of output lines",
"           -ofn(fmt) Create a list of files with matches in fn, using printf",
"                     format string fmt - %s is replaced with the file name",
"                                       - \\b is replaced with a blank",
"                     fn defaults to tmp.bat",
"                     fmt defaults to (%s)",
"           -p        Include full path specification",
"           -q        Don't print anything (quiet)",
"           -s        Display only similar strings (e.g., B is similar to b)",
"           -t        No substring matches",
"           -wN       Display N lines of context",
"           -x        Stop searching each file after one occurrence found",
"           -1        Exit when all strings matched once with return code",
"           -rN       Recurse subdirectories (max. N levels deep)",
"           -ro       Restrict search to read-only files (no writeable files)",
"           -estring  Specify additional search string",
NULL
};

static void Error( char *msg, char *other )
{
    printf( "%s '%s'\r\n", msg, other );
    exit( 3 );
}

static void Warning( char *msg, char *other )
{
    printf( "%s '%s'\r\n", msg, other );
}

static void *SafeMalloc( size_t n )
{
    void        *p;

    p = malloc( n );

    if( p == NULL ) {
        Error( "Out of memory", "Stack" );
    }
    return( p );
}

static void GetDefIgnoreList( char *extra ) {
    char        *env;
    char        *ptr;
    char        *bufPtr;
    unsigned    exts;
    unsigned    exts_len;
    unsigned    pass;

    exts = 0;
    extra += 2; // skip the -g
    ptr = extra;
    if( *ptr != '\0' ) {
        ptr++;  // in case the string doesn't start with a "."
        exts++; // we count at least 1 extension
        while( *ptr != '\0' ) {
            if( *ptr++ == '.' ) exts++;
        }
    }
    env = getenv( "WGREPIGNORE" );
    if( env == NULL ) {
        env = DefIgnoredExt;
    }
    ptr = env;
    if( *ptr != '\0' ) {
        ptr++;  // in case the string doesn't start with a "."
        exts++; // we count at least 1 extension
        while( *ptr != '\0' ) {
            if( *ptr++ == '.' ) exts++;
        }
    }
    IgnoreList = (char **)SafeMalloc( (exts+1) * sizeof( char * ) );
    if( IgnoreList == NULL ) return;

    exts_len = (unsigned)(strlen( env ) + strlen( extra ));
    // allocate space for all extensions + null chars + 1 extra in case "." missing
    IgnoreListBuffer = (char *)SafeMalloc( exts_len + exts + 1 );
    if( IgnoreListBuffer == NULL ) return;
    memset( IgnoreListBuffer, '\xee', exts_len + exts + 1 );

    IgnoreListCnt = exts;   // non-zero means we successfully allocated buffers

    bufPtr = IgnoreListBuffer;

    IgnoreListCnt = 0;
    pass = 0;
    ptr = NULL;
    for( ;; ) {
        if( ptr == NULL && pass == 0 ) {
            ptr = env;
        } else if( ptr == NULL && pass == 1 ) {
            ptr = extra;
        }
        if( *ptr == '\0' ) break;

        IgnoreList[ IgnoreListCnt ] = bufPtr;
        IgnoreListCnt++;

        if( *ptr != '.' ) {
            printf( "Warning! - file extension does not begin with a '.'\n" );
        } else {
            ptr++;
        }
        *bufPtr = '.';
        bufPtr++;

        while( *ptr != '.' && *ptr !='\0' ) {
            *bufPtr = *ptr;
            bufPtr++;
            ptr++;
        }
        *bufPtr = '\0';
        bufPtr++;
        if( *ptr == '\0' ) {
            pass++;
            ptr = NULL;
        }
        if( pass == 2 ) break;
    }
    IgnoreList[ IgnoreListCnt ] = NULL;
}

static void printFileName( void )
{
    int          i;
    char         buff[_MAX_PATH];       // Can't use PathBuff since FName is
                                        // a pointer into it!
    char        *p = buff;

    if( PrtLines ) {
        printf( "File: " );
    }
    if( PrtPath && _fullpath( buff, FName, _MAX_PATH ) ) {
        printf( "%s\r\n", buff );
    } else {
        i = 0;
        while( FName[i] != '\0' ) {
            if( FName[i] == '.'  &&  FName[i+1] == '\\' ) {
                if( i != 0  &&  FName[i-1] != '\\' ) {
                    *p = FName[i];
                    p++;
                    *p = FName[i+1];
                    p++;
                }
                i += 2;
            } else {
                *p = FName[i];
                p++;
                i++;
            }
        }
        *p = '\0';
        printf( "%s\r\n", buff );
    }
}

static void dumpMatch( char *buff, size_t len )
{
    char *stop = &buff[len];

    while( buff != stop ) {
        char c = *buff;
        if( c == 0x07 ) {
            // bell chars have bothered me for years! (AFS)
            fputc( '^', stdout );
            c = 'G';
        }
        fputc( c, stdout );
        ++buff;
    }
}

static wbool outputMatch( char *where, size_t read, wbool disp )
{
    wbool               done;
    char                *endrec;
    int                 i;

    done = FALSE;
    if( where >= &Buff[read] ) {
        done = TRUE; // this is the string we added to buffer! we're done buffer
    } else if( disp ) {
        if( PrtFn ) {
            if( FOut != NULL ) {
                fprintf( FOut, FOutFmt, FName );
                fprintf( FOut, "\n" );
            }
            if( PrtFiles && !PrtAll ) {
                printFileName();
            }
            PrtFn = FALSE;
        }
        MatchCount++;
        TotalMatchCount++;
        if( PrtMatch ) {
            i = Context + 1;
            for( ;; ) { // scan back to beginning of the record
                if( where == Buff ) break;
                if( *where == '\n' ) {
                    if( --i == 0 ) {
                        ++where;
                        break;
                    }
                }
                --where;
            }
            endrec = where;
            i = Context * 2 + 1;
            while( --i >= 0 ) {
                do { // find the end of record
                    ++endrec;
                } while( *endrec != '\n' && endrec != (Buff + read + 1) );
            }
            if( PrtLines ) {
                printf( "%.5u: ", Recs );
                if( Context ) printf( "\r\n" );
            }
            dumpMatch( where, endrec - where + 1 );
        }
        if( QuitFirst || OnePerFile ) {
            done = TRUE;
            ExitStatus = 1;
        }
    }
    return( done );
}

static int isTextChar( char c )
{
    return( isalnum( c ) || c == '_' );
}

static void searchBuffer( char *srch, size_t read )
{
    char        *p;
    char        *s;
    char        *next;
    char        first;
    char        last;
    unsigned    skip;
    unsigned    diss;

    last = '\0';
    next = Buff;
    first = *srch;
    ++srch;
    for( ;; ) {
        p = next;
        if( *p == '\n' && p < &Buff[read] ) {
            ++Recs;
        }
        if( Similar && (tolower(*p) == tolower(first)) ) { // similar char
            diss = 0;
            if( *p != first ) diss = 1;
            ++p;
            next = p;
            s = srch;
            skip = 0;
            for( ;; ) {                         // check the whole string
                if( *s == '\0' ) {
                    if( NoSubstring && isTextChar( last ) ) diss = 0;
                    if( NoSubstring && isTextChar( *p ) ) diss = 0;
                    if( outputMatch( next - 1, read, diss ) ) return;
                    next = (char *)memchr( next, '\n', BSize + 1 );
                    break;
                }
                if( *p == '\n' ) {
                    ++skip;
                }
                if( tolower(*s) != tolower(*p) ) {
                    // if the mismatched character is not in the string,
                    // restart search AFTER the mismatching character
                    if( CharInSrchStrings[ CharTrans[ (unsigned char)*p ] ] == FALSE ) {
                        next = p + 1;
                        Recs += skip;
                    }
                    break;
                } else if( *s != *p ) {
                    diss = 1;
                }
                ++s;
                ++p;
            }
        } else if( CharTrans[ (unsigned char)*p ] == first ) { // possible match!
            ++p;
            next = p;
            s = srch;
            skip = 0;
            for( ;; ) {                         // check the whole string
                if( *s == '\0' ) {
                    diss = 1;
                    if( NoSubstring && isTextChar( last ) ) diss = 0;
                    if( NoSubstring && isTextChar( *p ) ) diss = 0;
                    if( outputMatch( next - 1, read, diss ) ) return;
                    next = (char *)memchr( next, '\n', BSize + 1 );
                    break;
                }
                if( *p == '\n' ) {
                    ++skip;
                }
                if( *s != CharTrans[ (unsigned char)*p ] ) {
                    // if the mismatched character is not in the string,
                    // restart search AFTER the mismatching character
                    if( CharInSrchStrings[ CharTrans[ (unsigned char)*p ] ] == FALSE ) {
                        next = p + 1;
                        Recs += skip;
                    }
                    break;
                }
                ++s;
                ++p;
            }
        } else {
            last = *next;
            ++next;
        }
    }
    return;
}

static void OutMode( int mode )
{
    if( mode == CurOutMode ) return;
    setmode( STDOUT_FILENO, mode );
    if( mode == O_TEXT ) {
        setvbuf( stdout, StdoutBuf, _IOLBF, sizeof( StdoutBuf ) );
    } else {
        setvbuf( stdout, NULL, _IONBF, 1 );
    }
}

int readFile( int io, char *locn, size_t len )
{
    int         total;
    int         partial;

    total = 0;
    while( total < len ) {
        *locn = '\0';
        partial = read( io, locn, (unsigned)(len - total) );
        // quit if i/o error
        if( partial == -1 ) break;
        total += partial;
        // quit if not reading stdin
        if( io != STDIN_FILENO ) break;
        // quit if eof and nothing read
        if( eof( io ) && partial == 0 ) break;
        // eof when something has been read is bogus
        locn += partial;
    }
    if( total == 0 ) total = -1;
    return( total );
}

static void performSearch( char *fn )
{
    int         io;
    int         retries;    // number of times we read the pipe and got nothing
    int         red;
    int         sav;
    size_t      size;
    unsigned    frag;
    char        *locn;
    unsigned    saveRecs;
    char        **currs;
    char        **nexts;
    char        *probe;
    struct stat buf;

    MatchCount = 0;
    if( strcmp( fn, "@@" ) == 0 ) {
        fn = "stdin";
        io = STDIN_FILENO;
        setmode( io, O_BINARY );
    } else {
        if( FileMode != 0 ) {
            if( stat( fn, &buf ) != -1 ) {
                if( (buf.st_mode & FileMode) != 0 ) {
                    return;
                }
            }
        }
        io = open( fn, O_RDONLY | O_BINARY );
        if( io == -1 ) {
            Warning( "Unable to open", fn );
            return;
        }
    }
    FName = fn;
    retries = 0;        // number of times we read the pipe and got nothing
    if( PrtAll ) {
        printFileName();
    }
    PrtFn = TRUE;
    red = readFile( io, Buff, BSize );
    if( red == -1 ) {
        // Warning( "Error reading file", fn ); // removed for 0 length files
        if( io != STDIN_FILENO ) {
            close( io );
        }
        return;
    }

    // look for two '\n's with no preceeding '\r'
    probe = (char *)memchr( Buff, '\n', red );          // look for a newline
    if( ( probe == Buff || ( probe != NULL && probe[-1] != '\r' ) )
            && red > 1 ) {
        probe = (char *)memchr( probe+1, '\n', red - ( probe+1 - Buff ) );
        if( probe != NULL && probe[-1] != '\n' ) {
            OutMode( O_TEXT );          // we'll assume file has no \r's        in it
        } else {
            OutMode( O_BINARY );        // assume file has \r\n pairs
        }
    } else {
        OutMode( O_BINARY );
    }
    Recs = 1;
    locn = Buff;
    size = BSize;
    for(;;) {
        retries = 0;                    // we actually got something
        sav = red;                      // preserve old value
        Buff[ sav + 0 ] = '\0';
        Buff[ sav + 1 ] = '\n';         // put this here as a sentinel
                                        // (required by searchBuffer)
        while( red != 0 && Buff[red-1] != '\n' ) { // break on record boundary
            --red;
        }
        if( red == 0 ) red = sav;       // if we did not find NL restore red
        currs = SrchStrings;
        saveRecs = Recs;                // reset line number for each search string

        while( *currs ) {
            Recs = saveRecs;
            strcpy( Buff + sav + 2, *currs );
            searchBuffer( *currs, red );  // - search the buffer
            if( ExitStatus && QuitFirst ) {
                nexts = currs;
                for( ;; ) {
                    nexts[ 0 ] = nexts[ 1 ];
                    if( nexts[ 0 ] == NULL ) break;
                    ++nexts;
                }
                if( *SrchStrings ) {
                    ExitStatus = 0;
                }
            } else {
                ++currs;
            }
        }
        if( ExitStatus ) break;
        frag = (unsigned)(sav - red);
        if( frag != 0) {
            memcpy( Buff, &Buff[red], frag );
        }
        locn = &Buff[frag];
        size = BSize - frag;
        red = readFile( io, locn, size );       // read a big hunk of the file
        if( red == -1 ) {                       // if nothing read
            if( frag == 0 ) break;              // - and no fragment quit
            red = 0;                            // - did not read anything
        }                                       // endif
        red += frag;                            // include size of piece moved
    }
    if( io != STDIN_FILENO ) {
        close( io );
    }
    if( PrtCount && (MatchCount != 0) ) {
        printf( "Lines: %d\r\n", MatchCount );
    }
}

static void extendPath( char *path, char *ext )
{
    char        *d;

    for( d = Stack->name; *d; d++, path++ ) {
        *path = *d;
    }
    *path = '\\';
    for( ++path; *ext; ext++, path++ ) {
        *path = *ext;
    }
    *path = '\0';
}

static void executeWgrep( void )
{
    DIR                 *dirh;
    struct dirent       *dp;
    unsigned            i;
    char                exp[_MAX_EXT];
    char                ext[_MAX_EXT];

    extendPath( PathBuff, CurrPattern );

    _splitpath( PathBuff, NULL, NULL, NULL, exp );

    if( strcmp( CurrPattern, "@@" ) == 0 ) {
        performSearch( CurrPattern );
    } else {
        dirh = opendir( PathBuff );
        if( dirh != NULL ) {
            for( ;; ) {
                if( DoneFlag ) return;
                dp = readdir( dirh );
                if( dp == NULL ) break;
#if defined( __WATCOMC__ ) && !defined( __UNIX__ )
                if( dp->d_attr & _A_SUBDIR ) continue;
#else
                {
                    struct stat sblk;
                    char tmp_path[_MAX_PATH+1];
                    strcpy( tmp_path, PathBuff );
                    extendPath( tmp_path, dp->d_name );
                    if( stat( tmp_path, &sblk ) == 0 && S_ISDIR( sblk.st_mode ) ) {
                        continue;
                    }
                }
#endif

                if( IgnoreListCnt > 0 ) {
                    _splitpath( dp->d_name, NULL, NULL, NULL, ext );
                    if( stricmp( ext, exp ) != 0 ) {
                        for( i=0; i < IgnoreListCnt; i++ ) {
                            if( stricmp( ext, IgnoreList[i] ) == 0 ) break;
                        }
                        if( i < IgnoreListCnt ) continue;
                    }
                }
                extendPath( PathBuff, dp->d_name );
                performSearch( PathBuff );
            }
            closedir( dirh );
        }
    }
}

static void processDirectory( void )
{
    DIR                 *dirh;
    struct dirent       *dp;
    dirstack            *tmp;

    if( RecurLevels != 0 ) {
        extendPath( PathBuff, "*.*" );
        dirh = opendir( PathBuff );
        if( dirh != NULL ) {
            --RecurLevels;
            for( ;; ) {
                if( DoneFlag ) return;
                dp = readdir( dirh );
                if( dp == NULL ) break;
#if defined( __WATCOMC__ ) && !defined( __UNIX__ )
                if( !( dp->d_attr & _A_SUBDIR ) ) continue;
#else
                {
                    struct stat sblk;
                    char tmp_path[_MAX_PATH+1];
                    strcpy( tmp_path, PathBuff );
                    extendPath( tmp_path, dp->d_name );
                    if( stat( tmp_path, &sblk ) == 0 && !S_ISDIR( sblk.st_mode ) ) {
                        continue;
                    }
                }
#endif
                if( dp->d_name[0] == '.' ) {
                    if( dp->d_name[1] == '.' || dp->d_name[1] == '\0' ) continue;
                }
                if( DoneFlag ) return;
                tmp = (dirstack *) SafeMalloc( sizeof( dirstack ) );
                extendPath( tmp->name, dp->d_name );
                tmp->prev = Stack;
                Stack = tmp;
                processDirectory();
                Stack = tmp->prev;
                free( tmp );
            }
            ++RecurLevels;
            closedir( dirh );
        }
    }
    executeWgrep();
}

static void splitPath( char *path, char *d, char *f )
{
    char       *dir  = d + _MAX_DRIVE;
    char       *ext  = f + _MAX_FNAME;
    int         dirlen;

    _splitpath( path, d, dir, f, ext );

    dirlen = (int)strlen( dir );
    if( dirlen ) {
        if( dir[ dirlen - 1 ] == '\\'  ||  dir[ dirlen - 1 ] == '/' ) {
            dir[ dirlen ] = '.';
            dir[ dirlen + 1 ] = '\0';
        }
    } else {
        dir[0] = '.';
        dir[1] = '\0';
    }
    strcat( d, dir );
    strcat( f, ext );
}

static void nextWgrep( char **paths )
{
    struct stat     sblk;

    Stack = (dirstack *) SafeMalloc( sizeof( dirstack ) );

    while( *paths != NULL ) {
        if( stat( *paths, &sblk ) == 0 && S_ISDIR( sblk.st_mode ) ) {
            strcpy( Stack->name, *paths );
            strcpy( CurrPattern, "*.*" );
        } else {
            splitPath( *paths, Stack->name, CurrPattern );
        }
        processDirectory( );
        paths++;
    }

    free( Stack );
}

static void startWgrep( char **paths )
{
    char            *next_path;
    char            *env;
    char            *more_paths[50];
    int             i, j;
    unsigned char   c;

    char            *p;
    char            **currs;
    size_t          len;
    size_t          max_len;

    currs = SrchStrings;
    max_len = 0;
    while( *currs ) {
        p = *currs;
        len = strlen( p );
        if( len > max_len ) {
            max_len = len;
        }
        while( *p ) {
            CharInSrchStrings[ (unsigned char)*p ] = TRUE;
            ++p;
        }
        ++currs;
    }
    for( BSize = BSIZE; BSize != 0; BSize -= BINC ) {
        if( Buff = (char *)malloc( BSize + max_len + 3 ) ) break;
    }
    if( Buff == NULL ) {
        Error( "Out of memory", "Buffer" );
        return;
    }
    while( *paths != NULL ) {
        next_path = *paths;
        if( next_path[0] == '%' ) {
            env = getenv( &next_path[1] );
            if( env == NULL ) env = next_path;
        } else {
            env = next_path;
        }
        i = 0;
        j = 0;
        c = '\0';
        for( ; ; i++ ) {
            if( c == '\0' ) {
                more_paths[ j ] = &env[ i ];
                j++;
            }
            c = env[i];
            if( c == '\0' ) break;
            if( c == ';' ) {
                c = '\0';
                env[i] = c;
            }
        }
        more_paths[ j ] = NULL;
        nextWgrep( more_paths );
        paths++;
    }
    free( Buff );
}

static void convertBlanks( char *dst )
{
    char        *src;

    src = dst;
    while( *src ) {
        if( *src == '\\' && *(src+1) == 'b' ) {
            src++;
            *src = ' ';
        }
        *dst++ = *src++;
    }
    *dst = '\0';
}

static void analyzeSearchString( char *str )
{
    char *out;

    out = str;
    while( *str ) {
        if( *str == '\\' ) {
            ++str;
            switch( *str ) {
            case '\0':  //  \ followed by nothing is a slash
            case '\\':
                *out = '\\';
                break;
            case '[':
                *out = '<';
                break;
            case ']':
                *out = '>';
                break;
            case '!':
                *out = '|';
                break;
            case 't':
                *out = '\x9';
                break;
            case '^':
                *out = '\n';
                break;
            case '$':
                *out = '\r';
                break;
            default:
                *out = *str;
            }
        } else {
            *out = *str;
        }
        ++str;
        ++out;
    }
    *out = '\0';
}

static char **parseSearchStrings( char **arg )
{
    FILE       *io;
    char       *p;
    char        buff[ 256 ];
    wbool       extra = FALSE;
    int         i = 0;
    int         len;

    while( *arg != NULL && **arg == '-'  &&  arg[0][1] == 'e'  &&  i < MAX_SRCH_STRINGS ) {
        if( arg[0][2] == '\0' ) {
            if( arg[1] != NULL ) {
                SrchStrings[ i ] = strdup( arg[1] );
            } else {
                Error( "Missing search string", "" );
            }
            arg += 2;
        } else {
            SrchStrings[ i ] = strdup( *arg + 2 );
            arg++;
        }
        analyzeSearchString( SrchStrings[i] );
        i++;
        extra = TRUE;
    }
    while( *arg != NULL && **arg == '@' ) {
        io = fopen( *arg + 1, "r" );
        if( io == NULL ) {
            Error( "Cannot open pattern file", *arg + 1 );
        }
        while( i < MAX_SRCH_STRINGS ) {
            if( !fgets( buff, 256, io ) ) break;
            if( feof( io ) || (isatty( fileno( io ) ) && *buff == '\n') ) break;
            len = (int)strlen( buff );
            if( len > 0 ) if( buff[len-1] == '\n' ) buff[len-1] = '\0';
            if( buff[0] != '\0' ) {
                SrchStrings[ i ] = strdup( buff );
                analyzeSearchString( SrchStrings[i] );
                ++i;
                SrchStrings[ i ] = NULL;
            } else {
                Error( "Invalid NULL line in pattern file", buff );
            }
        }
        arg++;
        extra = TRUE;
    }
    if( *arg != NULL && extra == FALSE ) {
        p = *arg;
        if( *p != '\0' ) {
            SrchStrings[ i ] = strdup( p );
            analyzeSearchString( SrchStrings[i] );
            SrchStrings[ i + 1 ] = NULL;
        } else {
            Error( "Invalid pattern", "" );
        }
        arg++;
    }
    if( i == MAX_SRCH_STRINGS ) {
        printf( "Warning: maximum of %d search strings\r\n",
                                                    MAX_SRCH_STRINGS );
    }
    return( arg );
}

static void printHelp( void )
{
    int         i;

    for( i = 0; Help[ i ]; ++i ) {
        printf( "%s\n", Help[ i ] );
    }
}

int main( int argc, char **argv ) {
/************************************/

    wbool       ignore;
#if defined( __WATCOMC__ )
    wbool       prtcmd;
    int         cmdlen;
    char        *cmdline;
#endif
    char        *fs;
    char        ch;
    char        *lf;
    char        *p;
    char        **currs;
    char        *allfiles[2];

    argc = argc;
    ++argv;
    if( argv[0] == NULL || argv[0][0] == '?' ) {
        printHelp( );
        exit( 2 );
    }

#if defined( __WATCOMC__ )
    cmdlen = _bgetcmd( NULL, 0 ) + 1;
    cmdline = malloc( cmdlen );
    if( cmdline != NULL ) {
        cmdlen = _bgetcmd( cmdline, cmdlen );
    }
    prtcmd = FALSE;
#endif
    ignore = FALSE;                 // initialize options
    PrtAll = FALSE;
    PrtCount = FALSE;
    PrtPath = FALSE;
    QuitFirst = FALSE;
    OnePerFile = FALSE;
    PrtMatch = TRUE;
    PrtLines = FALSE;
    PrtFiles = TRUE;
    Similar = FALSE;
    NoSubstring = FALSE;
    FOut = NULL;
    Context = 0;
    RecurLevels = 0;
    TotalMatchCount = 0;

    lf = NULL;
    while( *argv != NULL && **argv == '-' ) {
        if( argv[0][1] == 'e' ) {
            break;
        }
        switch( tolower( argv[0][1] ) ) {
        case 'a':
            PrtAll = TRUE;
            break;
        case 'c':
            PrtCount = TRUE;
            PrtMatch = FALSE;
            break;
#if defined( __WATCOMC__ )
        case 'd':
            prtcmd = TRUE;
            break;
#endif
        case 'f':
            PrtFiles = FALSE;
            break;
        case 'g':
            GetDefIgnoreList( argv[0] );
            break;
        case 'h':
        case '?':
            printHelp( );
            exit( 2 );
        case 'i':
            ignore = TRUE;
            break;
        case 'l':
            PrtMatch = FALSE;
            break;
        case 'n':
            PrtLines = TRUE;
            break;
        case 'o':
            lf = *argv + 2;
            break;
        case 'p':
            PrtPath = TRUE;
            break;
        case 'q':
            PrtLines = FALSE;
            PrtMatch = FALSE;
            PrtFiles = FALSE;
            break;
        case 'r':
            ch = tolower( argv[0][2] );
            if( ch == 'o' ) {
                FileMode = WRITABLE;                    // -ro
            } else if( ch == '\0' ) {
                RecurLevels = INT_MAX;
                PrtPath = TRUE;
            } else {
                RecurLevels = atoi( *argv + 2 );
                PrtPath = TRUE;
            }
            break;
        case 's':
            ignore = FALSE;
            Similar = TRUE;
            break;
        case 't':
            NoSubstring = TRUE;
            break;
        case 'w':
            Context = atoi( *argv + 2 );
            break;
        case 'x':
            OnePerFile = TRUE;
            break;
        case '1':
            QuitFirst = TRUE;
            break;
        default:
            Error( "Option not recognized at", *argv );
        }
        ++argv;
    }
#if defined( __WATCOMC__ )
    if( prtcmd ) {
        printf( "Demarcated command line arguments: [%s]\n", cmdline );
    }
#endif

    argv = parseSearchStrings( argv );
    if( SrchStrings[ 0 ] == NULL ) {
        printHelp();
    } else {
        CurOutMode = 0x1234;        /* it's a hack... */
        OutMode( O_BINARY );
        if( lf != NULL ) {
            if( !*lf ) lf = "tmp.bat(%s)";
            p = strchr( lf, '(' );
            if( p ) {
                *p = '\0';
                ++p;
                FOutFmt = p;
                p = strrchr( p, ')' );
                if( p ) {
                    *p = '\0';
                    convertBlanks( FOutFmt );
                }
            } else {
                FOutFmt = "%s";
            }
            if( !*lf ) lf = "tmp.bat";
            FOut = fopen( lf, "w+" );
            if( FOut == NULL ) {
                Error( "Cannot open output file", lf );
            }
        }
        if( ignore ) {
            ch = 'A';
            while( ch <= 'Z' ) { // zap all Uppers to Lower in char table
                CharTrans[ (unsigned char)ch ] |= 0x20;
                ++ch;
            }
            currs = SrchStrings;
            while( *currs != NULL ) {
                fs = *currs;
                while( *fs ) { // zap search string to lower
                    *fs = CharTrans[ (unsigned char)*fs ];
                    ++fs;
                }
                ++currs;
            }
        }
        if( *argv != NULL ) {
            startWgrep( argv );
        } else {

            if( !isatty( STDIN_FILENO ) ) {
                allfiles[ 0 ] = "@@";
            } else {
                allfiles[ 0 ] = "*.*";
            }
            allfiles[ 1 ] = NULL;
            startWgrep( allfiles );
        }
        if( FOut != NULL ) {
            fclose( FOut );
            FOut = NULL;
        }
    }
    if( PrtCount && (TotalMatchCount != 0) && (MatchCount != TotalMatchCount) ) {
        printf( "Total lines: %d\n", TotalMatchCount );
    }
    if( IgnoreListBuffer != NULL ) {
        free( IgnoreListBuffer );
    }
    if( IgnoreList != NULL ) {
        free( IgnoreList );
    }
    return( QuitFirst ? ExitStatus : 0 );
}

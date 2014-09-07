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


#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "wio.h"
#include "misc.h"
#include "fnutils.h"
#include "getopt.h"
#include "argvrx.h"
#include "console.h"

char *OptEnvVar = "more";
static const char *usageMsg[] = {
    "Usage: more [-?cftX] [+<n>] [-n<lines>] [-p<prompt>] [files]",
    "\tfiles              : files to display",
    "\tOptions: -?        : print this list",
    "\t\t -c        : clear screen after each screen",
    "\t\t -f        : don't fold lines",
    "\t\t -t        : don't use temp file to buffer stdin",
    "\t\t -<n>      : start on line <n>",
    "\t\t -n<lines> : use <lines> as the number of lines per page",
    "\t\t -X        : match files by regular expressions",
    "\tExecution Options:",
    "\t\t ?         : display file info",
    "\t\t SPACE,^F  : next page",
    "\t\t b,^B      : previous page",
    "\t\t ENTER,+   : next line",
    "\t\t 0         : go to start of file",
    "\t\t $         : go to last page of file",
    "\t\t -         : back up one line",
    "\t\t =         : display current line number",
    "\t\t q         : quit",
    "\t\t v         : edit file",
    NULL
};

#define BUFF_SHIFT      13              // gets a buffer of size 8k
#define BUFF_SIZE       (1 << BUFF_SHIFT)

#define BUFF_OFFSET(x)  ((x) & (BUFF_SIZE - 1))
#define BUFF_HIGH(x)    ((x) >> BUFF_SHIFT)
#define BUFF_START(x)   ((x) & ~(BUFF_SIZE - 1))

#define CTRL( a )       (a-'A'+1)

static char     *workBuff;
static char     *promptString="--more--";
static int      screenHeight;
static int      screenWidth;
static int      lineCount;
static long     startLine;
static char     clearScreen=0;
static char     foldLines=1;
static long     BufferPos;
static long     FilePos;        // actually BUFF_HIGH(current file position)
static long     FileSize;

#if defined( __DOS__ ) || defined( __OS2__ ) && defined( _M_I86 )
char GetRawChar( void );
#pragma aux GetRawChar = \
        "xor    ah,ah" \
        "int    16h" \
        value [al];
#endif

/*
 * getChar - get a char from the keyboard
 */
static int getChar( void )
{
#if defined( __DOS__ )
    return( GetRawChar() );
#else
#if defined( __OS2__ ) && defined( _M_I86 )
    if( _osmode == DOS_MODE ) {
        return GetRawChar();
    }
#endif
    return( getch() );
#endif

} /* getChar */

static int ReadChar( FILE *f )
/*****************************/
/* read a char from the buffer */
{
    int         ch;
    long        high;

    if( f == stdin ) {
        return fgetc( f );
    }
    if( BufferPos == FileSize ) return EOF;
    high = BUFF_HIGH( BufferPos );
    if( high != FilePos ) {
        if( high != FilePos + 1 ) {
            fseek( f, BUFF_START( BufferPos ), SEEK_SET );
        }
        fread( workBuff, 1, BUFF_SIZE, f );
        FilePos = high;
    }
    ch = workBuff[BUFF_OFFSET(BufferPos)];
    BufferPos++;
    return ch;
}

/*
 * backUpLines - back up a given number of lines in a file
 */
static int backUpLines( FILE *f, int lines )
{
    int ch;
    int lines_backed=0;

    if( BufferPos == 0 ) return lines_backed;
    BufferPos--;
    for(;;) {
        ch = ReadChar( f );
        if( ch == '\n' ) {
            lines--;
            if( lines == 0 ) {
                return lines_backed;
            }
            lines_backed++;
        }
        if( BufferPos <= 1 ) {
            BufferPos = 0;
            return lines_backed;
        }
        BufferPos -= 2;
    }
} /* backUpLines */

static void InitFileBuffer( long size )
/*************************************/
/* do some initialization for the file buffering */
{
    BufferPos = 0;
    FilePos = -1;
    FileSize = size;
}

static void BufSeek( long pos )
/*****************************/
/* set up the buffering to read from a new position */
{
    BufferPos = pos;
}

/*
 * doMore - process a file
 */
static void doMore( char *name, FILE *f )
{
    int         ch;
    long        file_size;
    int         done;
    long        curr_line=0;
    long        tline;
    int         char_cnt;
    int         percent;
    char        buff[_MAX_PATH*2];

    if( f != stdin ) {
        fseek( f, 0, SEEK_END );
        file_size = ftell( f );
        fseek( f, 0, SEEK_SET );
        InitFileBuffer( file_size );
    }
    char_cnt = 0;

    while( 1 ) {
        ch = ReadChar( f );
        if( ch == EOF ) {
            break;
        }
        char_cnt++;
        if( curr_line < startLine ) {
            if( ch == '\n' ) {
                curr_line++;
            }
        } else {
            if( ch != '\n' ) {
                if( char_cnt < screenWidth - 1 ) {
                    fputc( ch, stdout );
                    continue;
                }
                if( !foldLines ) {
                    fputc( '\r', stdout );
                    continue;
                }
                fputc( ch, stdout );
            } else {
                fputc( '\n', stdout );
                curr_line++;
            }
            char_cnt = 0;
            fflush( stdout );
            lineCount--;
            if( lineCount == 0 ) {
                lineCount = screenHeight-1;
                if( f != stdin ) {
                    percent = (100L*BufferPos)/file_size;
                } else {
                    percent = 0;
                }
                fprintf( stdout,"%s(%d%%)", promptString, percent );
                fflush( stdout );
                done = 0;
                while( !done ) {
                    done = 1;
                    ch = getChar();
                    fputs( "\r                                                                        \r", stdout );
                    fflush( stdout );
                    switch( ch ) {
                    case '\r':
                    case '+':
                        lineCount = 1;
                        break;
                    case CTRL( 'F' ):
                    case ' ':
                        if( clearScreen ) {
                            system( "cls" );
                        }
                        break;
                    case '=':
                        fputs( ltoa( curr_line, buff, 10 ), stdout );
                        fflush( stdout );
                        done = 0;
                        break;
                    case 'q':
                        fclose( f );
                        return;
                    case '0':
                        if( f != stdin ) {
                            BufSeek( 0 );
                            curr_line = 0;
                        } else {
                            done = 0;
                        }
                        break;
                    case '$':
                        if( f != stdin ) {
                            BufSeek( file_size );
                            curr_line -= backUpLines( f, screenHeight );
                        } else {
                            done = 0;
                        }
                        break;
                    case CTRL( 'B' ):
                    case 'b':
                        if( clearScreen ) {
                            system( "cls" );
                        }
                        if( f != stdin ) {
                            curr_line -= backUpLines( f, 2*screenHeight-1 );
                        } else {
                            done = 0;
                        }
                        break;
                    case '-':
                        if( f != stdin ) {
                            curr_line -= backUpLines( f, screenHeight+1 );
                        } else {
                            done = 0;
                        }
                        break;
                    case '?':
                        fprintf( stdout, "\"%s\", %ld of %ld bytes", name,
                                BufferPos, file_size );
                        fflush( stdout );
                        done = 0;
                        break;
                    case 'v':
                        tline = curr_line - screenHeight+2;
                        if( tline < 1 ) {
                            tline = 1;
                        }
                        sprintf( buff, "vi %s -k%ldGz\\n", name, tline );
                        system( buff );
                        if( clearScreen ) {
                            system( "cls" );
                        }
                        if( f != stdin ) {
                            curr_line -= backUpLines( f, screenHeight );
                        }
                        break;
                    default:
                        fputs( "use ENTER,SPACE,?,0,$,+,-,=,q,v,^F,b,^B", stdout );
                        fflush( stdout );
                        done = 0;
                        break;
                    }
                }
            }
        }
    }
    fclose( f );

} /* doMore */

int main( int argc, char *argv[] )
{
    int         i;
    FILE        *f;
    int         rxflag;
    int         buff_stdin;
    int         ch;

    screenHeight = GetConsoleHeight();
    screenWidth = GetConsoleWidth();

    workBuff = MemAlloc( BUFF_SIZE );
    buff_stdin = 1;
    rxflag = 0;

    while( 1 ) {
        ch = GetOpt( &argc, argv, "#cftXp:n:", usageMsg );
        if( ch == -1 ) {
            break;
        }
        switch( ch ) {
        case 'c':
            clearScreen = 1;
            break;
        case 'f':
            foldLines = 0;
            break;
        case 't':
            buff_stdin = 0;
            break;
        case 'n':
            screenHeight = atoi( OptArg )+1;
            break;
        case 'p':
            promptString = strdup( OptArg );
            break;
        case '#':
            startLine = atol( OptArg )-1;
            break;
        case 'X':
            rxflag = 1;
            break;
        }
    }

    argv = ExpandArgv( &argc, argv, rxflag );

    lineCount = screenHeight-1;

    if( argc == 1 ) {
        if( buff_stdin ) {
            f = tmpfile();
            if( f == NULL ) {
                exit( 1 );
            }
            setmode( fileno( f ), O_BINARY );
            while( 1 ) {
                i = fread( workBuff, 1, BUFF_SIZE, stdin );
                if( fwrite( workBuff, 1, i, f ) != i ) {
                    exit( 1 );
                }
                if( feof( stdin ) ) {
                    break;
                }
            }
            fflush( f );
            doMore( "*stdin*", f );
        } else {
            doMore( "*stdin*", stdin );
        }
    } else {
        for( i=1;i<argc;i++ ) {
            f = fopen( argv[i],"rb" );
            if( f == NULL ) {
                printf( "Error opening \"%s\"\n", argv[i] );
            } else {
                if( argc > 2 ) {
                    printf( "\n%s:\n", FNameLower( argv[i] ) );
                }
                doMore( argv[i], f );
            }
        }
    }
    return( 0 );
}

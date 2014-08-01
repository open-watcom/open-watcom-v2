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
* Description:  InfoBench help compiler mainline.
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "wio.h"
#include "watcom.h"
#include "bool.h"
#include "uidef.h"
#include "stdui.h"
#include "help.h"
#include "cmdswtch.h"
#include "sorthelp.h"
#include "index.h"
#include "helpchar.h"
#include "helpmem.h"
#include "helpscan.h"
#include "clibext.h"

#define DEFTOPIC        "DEFTOPIC::::"
#define DESCRIPTION     "DESCRIPTION::::"

a_helpnode      *HelpNodes;

static bool     Verbose = FALSE;
static bool     GenIndex = TRUE;
static bool     GenStrings = TRUE;

static bool     pass1( FILE *fin, char **helpstr );
static bool     pass2( FILE *fin, int fout, char **helpstr );
static void     fgetstring( char *buffer, int max, FILE *f );
static ScanCBfunc lineLenCB;
static ScanCBfunc checkBufCB;

static int      MaxCol = 78;
static int      MaxRow = 21;
static int      Width = 0;
static int      Height = 0;

#define BUFFER_SIZE     400

static char UsageText[] = {
        "Usage: hcdos [flags] <infile> <outfile>\n"
    "\n"
    "\tflags:\n"
    "\t  -c <n>     maximum column allowed - default 78\n"
    "\t  -h <n>     force all windows # high\n"
    "\t  -n         don't generate an index for searching: don't use with -f\n"
    "\t  -r <n>     maximum row allowed - default 21\n"
    "\t  -v         verbose\n"
    "\t  -w <n>     force all windows # wide\n"
    "\t  -f{0,1,2}  outfile format \n"
    "\t             0: don't generate an index for searching (same as -n)\n"
    "\t             1: don't generate description and default topic strings\n"
    "\t             2: generate description and default topic strings (default)\n"
};

static FILE *errFile;
static char errFileName[_MAX_PATH];
static bool errHasOccurred;
static char errBuffer[512];

void Usage( void )
{
    fprintf( stderr, "%s", UsageText );
    exit( -1 );
}


void InitError( char *target )
{
    char    drive[_MAX_DRIVE];
    char    dir[_MAX_DIR];
    char    fname[_MAX_FNAME];

    errHasOccurred = FALSE;
    _splitpath( target, drive, dir, fname, NULL );
    _makepath( errFileName, drive, dir, fname, ".err" );
}


void FiniError( void )
{
    if( errFile != NULL ) {
        fclose( errFile );
    }
    if( !errHasOccurred ) {
        remove( errFileName );
    }
}

void PrintError( char *fmt, ... )
{
    va_list     al;

    if( !errHasOccurred ) {
        errFile = fopen( errFileName, "wt" );
        errHasOccurred = TRUE;
    }
    va_start( al, fmt );
    vsprintf( errBuffer, fmt, al );
    fputs( errBuffer, stderr );
    if( errFile != NULL ) {
        fputs( errBuffer, errFile );
    }
    va_end( al );
}

int main( int argc, char **argv )
{
    char        **nargv;
    char        **sargv;
    FILE        *fin;
    int         fout;
    char        *helpstr[2];
    bool        f_swtch;

    f_swtch = FALSE;
    helpstr[0] = NULL;
    helpstr[1] = NULL;
    for( argc=1, sargv=nargv=argv+1; *sargv; ++sargv ) {
        if( ! _IsCmdSwitch( *sargv ) ) {
            *nargv++ = *sargv;
            argc++;
        } else {
            switch( (*sargv)[1] ) {
            case 'n':
                GenIndex = FALSE;
                if( f_swtch ) {
                    PrintError( "More than one format switch found in command line\n" );
                    Usage();
                }
                f_swtch = TRUE;
                break;
            case 'v':
                Verbose = TRUE;
            break;
            case 'c':
                if( (*sargv)[2] != '\0' ) {
                    MaxCol = atoi( &(*sargv)[2] );
                } else {
                    if( *++sargv == NULL )
                        Usage();
                    MaxCol = atoi( *sargv );
                }
                break;
            case 'r':
                if( (*sargv)[2] != '\0' ) {
                    MaxRow = atoi( &(*sargv)[2] );
                } else {
                    if( *++sargv == NULL )
                        Usage();
                    MaxRow = atoi( *sargv );
                }
                break;
            case 'h':
                if( (*sargv)[2] != '\0' ) {
                    Height = atoi( &(*sargv)[2] );
                } else {
                    if( *++sargv == NULL )
                        Usage();
                    Height = atoi( *sargv );
                }
                break;
            case 'w':
                if( (*sargv)[2] != '\0' ) {
                    Width = atoi( &(*sargv)[2] );
                } else {
                    if( *++sargv == NULL )
                        Usage();
                    Width = atoi( *sargv );
                }
                break;
            case 'f':
                if( f_swtch ) {
                    PrintError( "More than one format switch found in command line\n" );
                    Usage();
                }
                f_swtch = TRUE;
                if( (*sargv)[2] == '0' ) {
                    GenIndex = FALSE;
                } else if( (*sargv)[2] == '1' ) {
                    GenStrings = FALSE;
                } else if( (*sargv)[2] == '2' ) {
                    GenStrings = TRUE;
                } else {
                    Usage();
                }
                break;
            default:
                Usage();
            break;
            }
        }
    }

    if( argc > 1  &&  strcmp( argv[1], "?" ) == 0 ) {
        Usage();
    }

    if( argc != 3 ) {
        Usage();
    }

    InitError( argv[1] );
    fin = fopen( argv[1], "rt" );
    if( fin == NULL ) {
        PrintError( "Unable to open '%s' for input\n", argv[1] );
        return( -1 );
    }

    fout = open( argv[2], O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, PMODE_RW );
    if( fout == -1 ) {
        PrintError( "Unable to open '%s' for output\n", argv[2] );
        return( -1 );
    }

    pass1( fin, helpstr );
    pass2( fin, fout, helpstr );

    fclose( fin );
    close( fout );
    if( GenIndex ) {
        fout = open( argv[2], O_WRONLY | O_BINARY );
        WriteIndex( fout, helpstr, GenStrings );
        close( fout );
    }
    FiniError();
    HelpMemFree( helpstr[0] );
    HelpMemFree( helpstr[1] );
    return( 0 );
}

static void lineLenCB( TokenType type, Info *info, void *_len )
{
    TextInfoBlock   *block;
    unsigned        i;
    unsigned        *len = _len;

    switch( type ) {
    case TK_TEXT:
        switch( info->u.text.type ) {
        case TT_PLAIN:
        case TT_LEFT_ARROW:
        case TT_RIGHT_ARROW:
            *len += info->u.text.len;
            break;
        case TT_ESC_SEQ:
            *len += 1;
            break;
        }
        break;
    case TK_PLAIN_LINK:
        *len += 2;
    /* fall through */
    case TK_GOOFY_LINK:
        block = &( info->u.link.block1 );
        while( block != NULL ) {
            for( i=0; i < block->cnt; i++ ) {
                info = (Info *)&( block->info[i] );
                switch( info->u.text.type ) {
                case TT_PLAIN:
                    *len = info->u.text.len;
                    break;
                case TT_ESC_SEQ:
                    *len += 1;
                    break;
                }
            }
            block = block->next;
        }
        break;
    }
}

static int line_len( char *str )
{
    unsigned    len;
    bool        newfile;

    len = 0;
    newfile = ScanLine( str, lineLenCB, &len );
    if( ( !GenIndex || !GenStrings ) && newfile ) {
        PrintError( "Cross file hyperlink in \"%s\" not supported with this format.\n", str );
    }
    return( len );
}

static char *find_str( char *buf )
{
    int     len;
    char    *str;

    while( *buf != '"' ) {
        if( *buf == HELP_ESCAPE )
            buf++;
        buf++;
    }
    buf++;
    str = buf;
    while( *buf != '"' && *buf != '\0' ) {
        if( *buf == HELP_ESCAPE )
            buf++;
        buf++;
    }
    len = buf - str;
    str[len] = '\0';
    return( str );
}

static bool pass1( FILE *fin, char **helpstr )
{
    char            buffer[ BUFFER_SIZE ];
    int             buflen;
    long            fpos;
    a_helpnode      *h;
    a_helpnode      **hn;
    char            *ptr;
    int             cmp;
    char            *namebuff;
    unsigned        namebuff_len;
    int             count;
    int             len;
    unsigned        topic_len;
    unsigned        desc_len;

    namebuff = NULL;
    namebuff_len = 0;
    topic_len = strlen( DEFTOPIC );
    desc_len = strlen( DESCRIPTION );

    printf( "Pass One:\n" );
    fpos = 0;
    while( !feof( fin ) ) {
        fpos = ftell( fin );
        fgetstring( buffer, BUFFER_SIZE, fin );
        if( memcmp( buffer, DEFTOPIC, topic_len ) == 0 ) {
            if( helpstr[0] != NULL ) {
                PrintError( "more than one DEFTOPIC found\n" );
            } else {
                if( !GenStrings || !GenIndex ) {
                    PrintError( "DEFTOPIC string ignored with this format.\n" );
                }
                if( GenStrings ) {
                    ptr = find_str( &buffer[topic_len] );
                    helpstr[0] = HelpMemAlloc( strlen( ptr ) + 1 );
                    strcpy( helpstr[0], ptr);
                }
            }
        } else if( memcmp( buffer, DESCRIPTION, desc_len ) == 0 ) {
            if( helpstr[1] != NULL ) {
                PrintError( "more than one DESCRIPTION found\n" );
            } else {
                if( !GenStrings || !GenIndex ) {
                    PrintError( "DESCRIPTION string ignored with this format.\n" );
                }
                if( GenStrings ) {
                    ptr = find_str( &buffer[desc_len] );
                    helpstr[1] = HelpMemAlloc( strlen( ptr ) + 1 );
                    strcpy( helpstr[1], ptr );
                }
            }
        } else if( memcmp( buffer, "::::", 4 ) == 0 )
            break;
    }
    while( !feof( fin ) ) {
        h = (a_helpnode *)HelpMemAlloc( sizeof( a_helpnode ) );
        h->fpos = fpos;
        buflen = strlen( buffer );
        if( buffer[ buflen-1 ] == '\n' ) {
            buffer[ buflen-1 ] = '\0';
        }
        h->maxrow = 0;
        h->maxcol = 0;
        h->row = -1;
        h->col = -1;
        h->lines = -1;
        ptr = &buffer[4];
        if( *ptr == '"' ) {
            ptr ++;
            while( *ptr != '\0' && *ptr != '"' ) {
                if( *ptr == HELP_ESCAPE )
                    ptr++;
                ptr++;
            }
            len = ptr - &buffer[5];
            if( namebuff_len <= len ) {
                HelpMemFree( namebuff );
                namebuff = HelpMemAlloc( len + 1 );
                namebuff_len = len + 1;
            }
            memcpy( namebuff, &buffer[5], len );
            namebuff[len] = '\0';
            if( *ptr == '"' )
                ++ptr;
        } else {
            for( ; *ptr != '\0'  &&  !isspace(*ptr); ++ptr )
                ;
            while( *ptr != '\0'  &&  !isspace(*ptr) ) {
                if( *ptr == HELP_ESCAPE )
                    ptr++;
                ptr++;
            }
            len = ptr - &buffer[4];
            if( namebuff_len <= len ) {
                HelpMemFree( namebuff );
                namebuff = HelpMemAlloc( len + 1 );
                namebuff_len = len + 1;
            }
            memcpy( namebuff, &buffer[4], len );
            namebuff[len] = '\0';
        }
        while( isspace( *ptr ) )
            ++ptr;
        if( *ptr != '\0' ) {
            count = sscanf( ptr, "%d %d %d %d %d",
                    &h->maxrow, &h->maxcol, &h->row, &h->col,
                    &h->lines );
            if( count != 2  && count != 4  &&  count != 5 ) {
                PrintError( "invalid help topic line '%s'\n", buffer );
            }
        }
        h->name = strdup( namebuff );
        if( Verbose ) {
            printf( "   %s\n", h->name );
        }
        for( hn=&HelpNodes; *hn != NULL; hn=&(*hn)->next ) {
            cmp = stricmp( h->name, (*hn)->name );
            if( cmp == 0 ) {
                PrintError( "Duplicate Help Topic '%s'\n", h->name );
            }
            if( cmp <= 0 ) {
                h->next = *hn;
                *hn = h;
                break;
            }
        }
        if( *hn == NULL ) {
            h->next = NULL;
            *hn = h;
        }
        if( h->row == -1 ) {
            h->row = 0;
            h->col = 0;
        }
        h->maxcol = 0;
        h->maxrow = 0;
        h->lines = 0;
        while( !feof( fin ) ) {
            fpos = ftell( fin );
            fgetstring( buffer, BUFFER_SIZE, fin );
            if( memcmp( buffer, "::::", 4 ) == 0 )
                break;
            if( strnicmp( buffer, ":eh", 3 ) == 0
                || strnicmp( buffer, ":et", 3 ) == 0 ) {
                h->lines = 0;
            } else if( strnicmp( buffer, ":h", 2 ) == 0
                    || strnicmp( buffer, ":t", 2 ) == 0  ) {
            } else {
                buflen = line_len( buffer );
                if( buflen - 1 > h->maxcol ){
                    h->maxcol = buflen - 1;
                }
                h->lines += 1;
                h->maxrow += 1;
            }
        }
    }
    HelpMemFree( namebuff );
    return( TRUE );
}

void lookup_name( a_helpnode *h, char *name )
{
    a_helpnode      *hptr;
    int             cmp;

    for( hptr = HelpNodes; hptr != NULL; hptr=hptr->next ) {
        cmp = stricmp( name, hptr->name );
        if( cmp < 0 )
            break;
        if( cmp == 0 )
            return;
    }
    PrintError( "Unknown help topic '%s' found in '%s'\n", name, h->name );
}

static void fgetstring( char *buffer, int max, FILE *f )
{
    int         curr;
    int         offset;
    int         ch;

    -- max;
    curr = 0;
    offset = 0;
    for( ;; ) {
        ch = fgetc( f );
        switch( ch ) {
        case EOF:
        case '\n':
            *buffer++ = '\r';       /* ignore trailing spaces */
            *buffer++ = '\n';
            *buffer = '\0';
            return;
        case ' ':
            ++ offset;
            break;
        case '\t':
            offset = (offset + 8) & (-8);
            break;
        default:
            while( ++curr <= offset ) {
                *buffer++ = ' ';
            }
            *buffer++ = ch;
            ++ offset;
        }
        if( offset >= max ) {
            *buffer = '\0';
            for( ;; ) {
                ch = fgetc( f );
                if( ch == EOF || ch == '\n' )
                    break;
            }
            return;
        }
    }
}

static char         *nameBuf;
static unsigned     nameBufLen;

static void checkBufCB( TokenType type, Info *info, void *_node )
{
    a_helpnode  *node = _node;

    if( type == TK_PLAIN_LINK || type == TK_GOOFY_LINK ) {
        if( nameBufLen <= info->u.link.topic_len ) {
            HelpMemFree( nameBuf );
            nameBuf = HelpMemAlloc( info->u.link.topic_len + 1 );
            nameBufLen = info->u.link.topic_len + 1;
        }
        memcpy( nameBuf, info->u.link.topic, info->u.link.topic_len );
        nameBuf[ info->u.link.topic_len ] = '\0';
        if( info->u.link.hfname_len == 0 ) {
            lookup_name( node, nameBuf );
        }
    }
}

void check_buffer( a_helpnode *h, char *buffer )
{
    ScanLine( buffer, checkBufCB, h );
    HelpMemFree( nameBuf );
    nameBuf = NULL;
    nameBufLen = 0;
}

static bool pass2( FILE *fin, int fout, char **helpstr )
{
    char            buffer[ BUFFER_SIZE ];
    a_helpnode      *h;
    unsigned long   indexlen;

    printf( "Pass Two:\n" );
    if( GenIndex ) {
        indexlen = CalcIndexSize( helpstr, GenStrings );
        lseek( fout, indexlen, SEEK_SET );
    }
    for( h = HelpNodes; h != NULL; h = h->next ) {
        if( Verbose ) {
            printf( "   %s %d %d", h->name, h->maxrow, h->maxcol );
            if( h->row != -1 ) {
                printf( "%d %d", h->row, h->col );
            }
            if( h->lines != -1 ) {
                printf( "%d", h->lines );
            }
            printf( "\n" );
        }
        fseek( fin, h->fpos, SEEK_SET );
        fgetstring( buffer, BUFFER_SIZE, fin );
        h->maxcol += 1;
        h->maxcol = (h->maxcol / 2) * 2;
        if( h->maxcol > MaxCol ) {
            PrintError( "%s %d %d image too wide\n",
                     h->name, h->maxrow, h->maxcol );
            h->maxcol = MaxCol;
        }
        if( h->maxcol > MaxCol-10 ) {
            h->maxcol = MaxCol;
        }
        if( h->maxrow > MaxRow ) {
            if( h->maxrow > MaxRow ) {
                h->maxrow = MaxRow;
            }
        }
        if( Height ) {
            h->maxrow = Height;
        }
        if( Width ) {
            h->maxcol = Width;
        }
        sprintf( buffer, "::::\"%s\" %d %d %d %d %d\r\n",
                 h->name, h->maxrow, h->maxcol, h->row, h->col, h->lines );
        if( GenIndex ) {
            h->fpos = tell( fout );
        }
        write( fout, buffer, strlen( buffer ) );

        while( !feof( fin ) ) {
            fgetstring( buffer, BUFFER_SIZE, fin );
            if( memcmp( buffer, "::::", 4 ) == 0 )
                break;
            check_buffer( h, buffer );
            write( fout, buffer, strlen( buffer ) );
        }
    }
    return( TRUE );
}

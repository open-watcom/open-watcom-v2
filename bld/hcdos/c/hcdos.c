/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
#include <limits.h>
#include "bool.h"
#include "cmdswtch.h"
#include "hcdos.h"
#include "helpidx.h"
#include "helpmem.h"
#include "helpscan.h"
#include "index.h"
#include "wibhelp.h"
#include "pathgrp2.h"

#include "clibext.h"


a_helpnode          *HelpNodes;

static bool         Verbose = false;
static bool         GenIndex = true;
static bool         GenStrings = true;

static ScanCBfunc   lineLenCB;
static ScanCBfunc   checkBufCB;

static int          MaxCol = 78;
static int          MaxRow = 21;
static int          Width = 0;
static int          Height = 0;

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

static void Usage( void )
{
    fprintf( stderr, "%s", UsageText );
    exit( -1 );
}


static void InitError( const char *target )
{
    pgroup2     pg;

    errHasOccurred = false;
    _splitpath2( target, pg.buffer, &pg.drive, &pg.dir, &pg.fname, NULL );
    _makepath( errFileName, pg.drive, pg.dir, pg.fname, "err" );
}


static void FiniError( void )
{
    if( errFile != NULL ) {
        fclose( errFile );
    }
    if( !errHasOccurred ) {
        remove( errFileName );
    }
}

static void PrintError( char *fmt, ... )
{
    va_list     al;

    if( !errHasOccurred ) {
        errFile = fopen( errFileName, "wt" );
        errHasOccurred = true;
    }
    va_start( al, fmt );
    vsprintf( errBuffer, fmt, al );
    fputs( errBuffer, stderr );
    if( errFile != NULL ) {
        fputs( errBuffer, errFile );
    }
    va_end( al );
}

static void lineLenCB( HelpTokenType type, Info *info, void *_len )
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
        for( block = &( info->u.link.block1 ); block != NULL; block = block->next ) {
            for( i = 0; i < block->cnt; i++ ) {
                info = (Info *)&( block->info[i] );
                switch( info->u.text.type ) {
                case TT_PLAIN:
                    *len += info->u.text.len;
                    break;
                case TT_ESC_SEQ:
                    *len += 1;
                    break;
                }
            }
        }
        break;
    }
}

static char *find_str( char *buf )
{
    unsigned    len;
    char        *str;

    while( *buf != '"' ) {
        if( *buf == IB_ESCAPE )
            buf++;
        buf++;
    }
    buf++;
    str = buf;
    while( *buf != '"' && *buf != '\0' ) {
        if( *buf == IB_ESCAPE )
            buf++;
        buf++;
    }
    len = buf - str;
    str[len] = '\0';
    return( str );
}

static void fgetstring( char *buffer, unsigned max_len, FILE *f )
{
    unsigned    curr;
    unsigned    offset;
    int         ch;

    max_len--;  /* reserve space for terminator */
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
            offset++;
            break;
        case '\t':
            offset = ( offset + 8 ) & (-8);
            break;
        default:
            while( ++curr <= offset ) {
                *buffer++ = ' ';
            }
            *buffer++ = ch;
            offset++;
        }
        if( offset >= max_len ) {
            *buffer = '\0';
            for( ;; ) {
                ch = fgetc( f );
                if( ch == EOF || ch == '\n' ) {
                    break;
                }
            }
            break;
        }
    }
}

static bool pass1( FILE *fin, const char **helpstr )
{
    char            buffer[BUFFER_SIZE];
    unsigned        buflen;
    long            fpos;
    a_helpnode      *h;
    a_helpnode      **hn;
    char            *ptr;
    int             cmp;
    char            *namebuff;
    unsigned        namebuff_len;
    int             count;
    unsigned        len;
    bool            deftopic;
    bool            defdesc;

    printf( "Pass One:\n" );
    deftopic = false;
    defdesc = false;
    fpos = 0;
    while( !feof( fin ) ) {
        fpos = ftell( fin );
        fgetstring( buffer, sizeof( buffer ), fin );
        if( IS_IB_DEFAULT_TOPIC( buffer ) ) {
            if( deftopic ) {
                PrintError( "more than one DEFTOPIC found\n" );
            } else {
                deftopic = true;
                if( helpstr == NULL ) {
                    PrintError( "DEFTOPIC string ignored with this format.\n" );
                } else {
                    ptr = find_str( buffer + IB_DEFAULT_TOPIC_SIZE );
                    namebuff = HelpMemAlloc( strlen( ptr ) + 1 );
                    strcpy( namebuff, ptr );
                    helpstr[0] = namebuff;
                }
            }
        } else if( IS_IB_DESCRIPTION( buffer ) ) {
            if( defdesc ) {
                PrintError( "more than one DESCRIPTION found\n" );
            } else {
                defdesc = true;
                if( helpstr == NULL ) {
                    PrintError( "DESCRIPTION string ignored with this format.\n" );
                } else {
                    ptr = find_str( buffer + IB_DESCRIPTION_SIZE );
                    namebuff = HelpMemAlloc( strlen( ptr ) + 1 );
                    strcpy( namebuff, ptr );
                    helpstr[1] = namebuff;
                }
            }
        } else if( IS_IB_TOPIC_NAME( buffer ) ) {
            break;
        }
    }

    namebuff = NULL;
    namebuff_len = 0;
    while( !feof( fin ) ) {
        h = (a_helpnode *)HelpMemAlloc( sizeof( a_helpnode ) );
        h->fpos = fpos;
        buflen = strlen( buffer );
        if( buffer[buflen - 1] == '\n' ) {
            buffer[buflen - 1] = '\0';
        }
        h->maxrow = 0;
        h->maxcol = 0;
        h->row = -1;
        h->col = -1;
        h->lines = -1;
        ptr = buffer + IB_TOPIC_NAME_SIZE;
        if( *ptr == '"' ) {
            ptr++;
            while( *ptr != '\0' && *ptr != '"' ) {
                if( *ptr == IB_ESCAPE )
                    ptr++;
                ptr++;
            }
            len = ptr - ( buffer + IB_TOPIC_NAME_SIZE + 1 );
            if( namebuff_len <= len ) {
                HelpMemFree( namebuff );
                namebuff = HelpMemAlloc( len + 1 );
                namebuff_len = len + 1;
            }
            memcpy( namebuff, buffer + IB_TOPIC_NAME_SIZE + 1, len );
            namebuff[len] = '\0';
            if( *ptr == '"' ) {
                ptr++;
            }
        } else {
            for( ; *ptr != '\0' && !isspace( *ptr ); ptr++ )
                ;
            while( *ptr != '\0' && !isspace( *ptr ) ) {
                if( *ptr == IB_ESCAPE )
                    ptr++;
                ptr++;
            }
            len = ptr - ( buffer + IB_TOPIC_NAME_SIZE );
            if( namebuff_len <= len ) {
                HelpMemFree( namebuff );
                namebuff = HelpMemAlloc( len + 1 );
                namebuff_len = len + 1;
            }
            memcpy( namebuff, buffer + IB_TOPIC_NAME_SIZE, len );
            namebuff[len] = '\0';
        }
        while( isspace( *ptr ) )
            ptr++;
        if( *ptr != '\0' ) {
            count = sscanf( ptr, "%d %d %d %d %d",
                    &h->maxrow, &h->maxcol, &h->row, &h->col, &h->lines );
            if( count != 2 && count != 4 && count != 5 ) {
                PrintError( "invalid help topic line '%s'\n", buffer );
            }
        }
        h->name = strdup( namebuff );
        if( Verbose ) {
            printf( "   %s\n", h->name );
        }
        for( hn = &HelpNodes; *hn != NULL; hn = &(*hn)->next ) {
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
            fgetstring( buffer, sizeof( buffer ), fin );
            if( IS_IB_TOPIC_NAME( buffer ) ) {
                break;
            } else if( IS_IB_HEADER_END( buffer ) || IS_IB_TRAILER_END( buffer ) ) {
                h->lines = 0;
            } else if( IS_IB_HEADER_BEG( buffer ) || IS_IB_TRAILER_BEG( buffer ) ) {
            } else {
                buflen = 0;
                if( ScanLine( buffer, lineLenCB, &buflen ) ) {
                    if( helpstr == NULL ) {
                        PrintError( "Cross file hyperlink in \"%s\" not supported with this format.\n", buffer );
                    }
                }
                if( h->maxcol < buflen - 1 ) {
                    h->maxcol = buflen - 1;
                }
                h->lines += 1;
                h->maxrow += 1;
            }
        }
    }
    HelpMemFree( namebuff );
    return( true );
}

static void lookup_name( a_helpnode *h, const char *name )
{
    a_helpnode      *hptr;
    int             cmp;

    for( hptr = HelpNodes; hptr != NULL; hptr = hptr->next ) {
        cmp = stricmp( name, hptr->name );
        if( cmp < 0 )
            break;
        if( cmp == 0 ) {
            return;
        }
    }
    PrintError( "Unknown help topic '%s' found in '%s'\n", name, h->name );
}

static char         *nameBuf;
static unsigned     nameBufLen;

static void checkBufCB( HelpTokenType type, Info *info, void *_node )
{
    a_helpnode  *node = _node;

    if( type == TK_PLAIN_LINK || type == TK_GOOFY_LINK ) {
        if( nameBufLen <= info->u.link.topic_len ) {
            HelpMemFree( nameBuf );
            nameBuf = HelpMemAlloc( info->u.link.topic_len + 1 );
            nameBufLen = info->u.link.topic_len + 1;
        }
        memcpy( nameBuf, info->u.link.topic, info->u.link.topic_len );
        nameBuf[info->u.link.topic_len] = '\0';
        if( info->u.link.hfname_len == 0 ) {
            lookup_name( node, nameBuf );
        }
    }
}

static void check_buffer( a_helpnode *h, char *buffer )
{
    ScanLine( buffer, checkBufCB, h );
    HelpMemFree( nameBuf );
    nameBuf = NULL;
    nameBufLen = 0;
}

static bool pass2( FILE *fin, FILE *fout, const char **helpstr )
{
    char            buffer[BUFFER_SIZE];
    a_helpnode      *h;
    unsigned long   indexlen;

    printf( "Pass Two:\n" );
    if( GenIndex ) {
        indexlen = CalcIndexSize( helpstr );
        fseek( fout, indexlen, SEEK_SET );
    }
    for( h = HelpNodes; h != NULL; h = h->next ) {
        if( Verbose ) {
            printf( "   %s %d %d", h->name, h->maxrow, h->maxcol );
            if( h->row != -1 ) {
                printf( " %d %d", h->row, h->col );
            }
            if( h->lines != -1 ) {
                printf( " %d", h->lines );
            }
            printf( "\n" );
        }
        fseek( fin, h->fpos, SEEK_SET );
        fgetstring( buffer, sizeof( buffer ), fin );
        h->maxcol += 1;
        h->maxcol = ( h->maxcol / 2 ) * 2;
        if( h->maxcol > MaxCol ) {
            PrintError( "%s %d %d image too wide\n",
                     h->name, h->maxrow, h->maxcol );
            h->maxcol = MaxCol;
        }
        if( h->maxcol > MaxCol - 10 ) {
            h->maxcol = MaxCol;
        }
        if( h->maxrow > MaxRow ) {
            h->maxrow = MaxRow;
        }
        if( Height ) {
            h->maxrow = Height;
        }
        if( Width ) {
            h->maxcol = Width;
        }
        sprintf( buffer, IB_TOPIC_NAME "\"%s\" %d %d %d %d %d\n",
                 h->name, h->maxrow, h->maxcol, h->row, h->col, h->lines );
        if( GenIndex ) {
            h->fpos = ftell( fout );
        }
        fwrite( buffer, strlen( buffer ), 1, fout );

        while( !feof( fin ) ) {
            fgetstring( buffer, sizeof( buffer ), fin );
            if( IS_IB_TOPIC_NAME( buffer ) )
                break;
            check_buffer( h, buffer );
            fwrite( buffer, strlen( buffer ), 1, fout );
        }
    }
    return( true );
}

int main( int argc, char **argv )
{
    char        **nargv;
    char        **sargv;
    FILE        *fin;
    FILE        *fout;
    const char  *helpdefstr[2];
    const char  **helpstr;
    bool        f_swtch;

    HelpMemOpen();

    f_swtch = false;
    argc = 1;
    nargv = argv + 1;
    for( sargv = nargv; *sargv; sargv++ ) {
        if( ! _IsCmdSwitch( *sargv ) ) {
            *nargv++ = *sargv;
            argc++;
        } else {
            switch( (*sargv)[1] ) {
            case 'n':
                GenIndex = false;
                if( f_swtch ) {
                    PrintError( "More than one format switch found in command line\n" );
                    Usage();
                }
                f_swtch = true;
                break;
            case 'v':
                Verbose = true;
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
                f_swtch = true;
                if( (*sargv)[2] == '0' ) {
                    GenIndex = false;
                } else if( (*sargv)[2] == '1' ) {
                    GenStrings = false;
                } else if( (*sargv)[2] == '2' ) {
                    GenStrings = true;
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

    fout = fopen( argv[2], "wb" );
    if( fout == NULL ) {
        PrintError( "Unable to open '%s' for output\n", argv[2] );
        return( -1 );
    }

    helpstr = NULL;
    if( GenIndex && GenStrings ) {
        helpdefstr[0] = NULL;
        helpdefstr[1] = NULL;
        helpstr = helpdefstr;
    }

    pass1( fin, helpstr );
    pass2( fin, fout, helpstr );

    fclose( fin );
    fclose( fout );
    if( GenIndex ) {
        fout = fopen( argv[2], "r+b" );
        WriteIndex( fout, helpstr );
        fclose( fout );
    }
    FiniError();
    if( helpstr != NULL ) {
        HelpMemFree( (void *)helpdefstr[0] );
        HelpMemFree( (void *)helpdefstr[1] );
    }
    HelpMemPrtList();
    HelpMemClose();

    return( 0 );
}

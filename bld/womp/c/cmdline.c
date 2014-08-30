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
* Description:  Command line processing for WOMP.
*
****************************************************************************/


#include <ctype.h>
#include <direct.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>      /* for SEEK_SET */
#include <malloc.h>     /* for _fheapshrink/_nheapshrink */
#include <conio.h>
#include "banner.h"
#include "womp.h"
#include "cmdline.h"
#include "memutil.h"
#include "genutil.h"
#include "myassert.h"
#include "watdbg.h"
#include "msdbg.h"
#include "genmsomf.h"
#include "can2txt.h"
#include "genphar.h"
#include "turbodbg.h"
#include "deflib.h"

/*
    THIS MODULE NEEDS A REWRITE!

    The interface to this module sucks bad.  (addFile's hack for
    wildcarding is especially gross.) The processing model
    should have been more along the lines of:

        // in main.c
        while( there_are_more_files_to_do() ) {
            ActionInput( ... );
                ...
            ActionOutput( ... );
                ...
                ...
            ActionRename( ... );
        }

    Main.c should not know about act_grp_t and cmdline_t in cmdline.h.
    There's no need for it.

    Then instead of wasting loads of memory by building all the stupid
    structures that we parse.  We could have simply written a state-machine
    in here.  i.e.,

    there_are_more_files_to_do() {
        if( we_are_wildcarding_a_filename ) {
            advance_to_next_wildcard_match;
            return( TRUE );
        }
        parse_any_switches_and_@includes;
        if( end_of_input ) {
            return( FALSE );
        }
        parse_a_filename;
        if( filename_has_wildcards ) {
            set we_are_wildcarding_a_filename to TRUE;
        }
        return( TRUE );
    }

    22-jul-91   DJG
*/


#define ENVVAR          "WOMP"
#define SWCHAR          '/'
#define INCCHAR         '@'
#define CMTCHAR         '#'
#define WMP_EXTENSION   ".WMP"
#define OBJ_EXTENSION   ".OBJ"
#define WILD_CARDS      "*?"    /* wild cards that may appear in filenames */

/*
    Be careful that none of these enumerated types outgrow their bitfield
    width in struct action_group.
*/

enum dbg_parsers {
    PARSE_NULL,
    PARSE_WATCOM,
    PARSE_WATCOM_70
};

enum dbg_generators {
    DGEN_NULL,
    DGEN_TXT,
    DGEN_MICROSOFT,
    DGEN_METAWARE,
    DGEN_TURBO
};

enum omf_generators {
    OGEN_NULL,
    OGEN_MICROSOFT,
    OGEN_MICROSOFT_OS2,
    OGEN_PHARLAP
};

STATIC cmdline_t    cmdLine;
STATIC act_grp_t    *curAct;    /* stack of actions */
STATIC int          headerDone;
STATIC uint_16      tempFileNum;

int BeQuiet( void ) {
    return( cmdLine.quiet );
}

STATIC void header( void ) {

    if( headerDone ) return;
    headerDone = 1;
    if( cmdLine.quiet ) return;
    PrtFmt(
        banner1w( "Object Module Processor", _WOMP_VERSION_ ) "\n"
        banner2 "\n"
        banner2a( "1990" ) "\n"
        banner3 "\n"
        banner3a "\n"
    );
}

static void waitForKey( void ) {

    char                c;

    PrtMsg( MSG_PRESS_KEY );
    c = getch();
}

#pragma aux usage aborts;
STATIC void usage( void ) {

    char        msgbuff[MAX_RESOURCE_SIZE];
    int         previous_null = 0;
    int         i;

    header();
    for( i = MSG_USE_BASE;; i++ ) {
        MsgGet( i, msgbuff );
        if( ( msgbuff[ 0 ] == '.' ) && ( msgbuff[ 1 ] == 0 ) ) break;
        if( previous_null ) {
            if( msgbuff[0] != '\0' ) {
                waitForKey();
                PrtFmt( msgbuff );
                PrtFmt( "\n" );
                previous_null = 0;
            } else break;
        } else if( msgbuff[0] == '\0' ) {
            previous_null = 1;
        } else {
            PrtFmt( msgbuff );
            PrtFmt( "\n" );
        }
    }
    exit( EXIT_FAILURE );
}

STATIC int isBreakChar( char x ) {

    if( isspace( x ) ) return( 1 );
    switch( x ) {
    case '-':
    case SWCHAR:
    case INCCHAR:
    case CMTCHAR:
    case 0:
        return( 1 );
    }
    return( 0 );
}

STATIC void getAct( int mk_new ) {

    act_grp_t   *act;

    act = curAct;
    if( act == NULL ) {
        /*
            Build the default action group
        */
        act = curAct = MemAlloc( sizeof( act_grp_t ) );
        act->next = NULL;
        act->dbg_parser = PARSE_WATCOM;
        act->dbg_generator = DGEN_METAWARE;
        act->omf_generator = OGEN_MICROSOFT;
        act->deflib = 1;
        act->quiet = 0;
        act->batch = 0;
        act->output = NULL;
        act->num_files = 0;
    } else if( act->num_files > 0 && mk_new ) {
        /* gotta create a new act_grp_t */
        act = MemAlloc( sizeof( act_grp_t ) );
        *act = *curAct;             /* copy the old info */
        act->next = curAct;         /* stack it up */
        curAct = act;
        act->num_files = 0;         /* no files in this group yet */
    }
}

STATIC char *getFile( const char ** pstr ) {

    const char  *start;
    const char  *str;
    size_t      len;
    char        *copy;

/**/myassert( pstr != NULL && *pstr != NULL );
    str = *pstr;
/**/myassert( !isBreakChar( *str ) );
    start = str;
    for(;;) {
        ++str;
        if( *str == '\0' ) break;
        if( isspace( *str ) ) break;
    }
    *pstr = str;
    len = str - start;
    copy = MemAlloc( len + 1 );
    memcpy( copy, start, len );
    copy[ len ] = 0;
    return( copy );
}

#pragma aux unrecognized aborts;
STATIC void unrecognized( char opt ) {
    char        msgbuff[MAX_RESOURCE_SIZE];

    header();
    MsgGet( MSG_SYNTAX_ERR_IN_OPT, msgbuff );
    PrtFmt( msgbuff, opt );
    PrtFmt( "\n" );
    usage();
}

STATIC const char *doParse( const char *str ) {

/**/myassert( str != NULL );
    getAct( 1 );
    switch( tolower( *str ) ) {
    case 'w':   curAct->dbg_parser = PARSE_WATCOM;      break;
    case '7':   curAct->dbg_parser = PARSE_WATCOM_70;   break;
    case '-':   curAct->dbg_parser = PARSE_NULL;        break;
    default:    unrecognized( 'p' );
    }
    return( str + 1 );
}

STATIC const char *doDebug( const char *str ) {

/**/myassert( str != NULL );
    getAct( 1 );
    switch( tolower( *str ) ) {
    case 'm':   curAct->dbg_generator = DGEN_MICROSOFT; break;
    case 'p':   curAct->dbg_generator = DGEN_METAWARE;  break;
    case 't':   curAct->dbg_generator = DGEN_TURBO;     break;
    case 'x':   curAct->dbg_generator = DGEN_TXT;       break;
    case '-':   curAct->dbg_generator = DGEN_NULL;      break;
    default:    unrecognized( 'd' );
    }
    return( str + 1 );
}

STATIC const char *doFile( const char *str ) {

/**/myassert( str != NULL );
    getAct( 1 );
    switch( tolower( *str ) ) {
    case 'm':
        curAct->omf_generator = OGEN_MICROSOFT;
        if( str[1] == '2' ) {
            curAct->omf_generator = OGEN_MICROSOFT_OS2;
            ++str;
        }
        break;
    case 'p':
        curAct->omf_generator = OGEN_PHARLAP;
        break;
    case '-':
        curAct->omf_generator = OGEN_NULL;
        break;
    default:
        unrecognized( 'f' );
    }
    return( str + 1 );
}

STATIC const char *doOutput( const char * str ) {

/**/myassert( str != NULL );
    if( *str != '=' && *str != '#' ) unrecognized( 'o' );
    ++str;
    getAct( 1 );
    if( curAct->output != NULL ) {
        MemFree( curAct->output );
    }
    if( isBreakChar( *str ) ) unrecognized( 'o' );
    curAct->output = getFile( &str );
    return( str );
}

STATIC const char *doToggle( const char *str ) {

/**/myassert( str != NULL );
    getAct( 1 );
    switch( tolower( str[-1] ) ) {
    case 'q':   curAct->quiet = ! curAct->quiet;        break;
#if 0
    case 'l':   curAct->deflib = ! curAct->deflib;      break;
#endif
    case 'b':   curAct->batch = ! curAct->batch;        break;
    default:    unrecognized( str[-1] );                break;
    }
    return( str );
}

STATIC const char *addFile( const char *str ) {

    DIR                 *parent;
    struct dirent       *direntp;
    char                sp_buf[ _MAX_PATH2 ];
    char                *drive;
    char                *dir;
    char                path[ _MAX_PATH ];
    char                *p;
    size_t              len;
    size_t              files_in_dir;

/**/myassert( str != NULL );
    getAct( 0 );
    p = getFile( &str );
    if( strpbrk( p, WILD_CARDS ) == NULL ) {
        curAct = MemRealloc( curAct, sizeof( act_grp_t ) +
            sizeof( const char * ) * curAct->num_files );
        curAct->files[ curAct->num_files ] = p;
        ++curAct->num_files;
        return( str );
    }
    /* process a wildcarded name */
    parent = opendir( p );
    if( parent == NULL ) {
        Fatal( MSG_UNABLE_TO_OPEN_FILE, p );
    }
    /*
        Since we must allocate memory for the filenames we shouldn't
        MemRealloc the curAct to a larger size at the same time.  So
        we count the number of files in the directory.
    */
    files_in_dir = 0;
    for(;;) {           /* count number of directory entries */
        direntp = readdir( parent );
        if( direntp == NULL ) break;
        ++files_in_dir;
    }
    closedir( parent );
    if( files_in_dir == 0 ) {
        Fatal( MSG_UNABLE_TO_OPEN_FILE, p );
    }
    curAct = MemRealloc( curAct, sizeof( act_grp_t ) +
        sizeof( const char * ) * ( curAct->num_files + files_in_dir - 1 ) );
    parent = opendir( p );
    if( parent == NULL ) {
        Fatal( MSG_UNABLE_TO_OPEN_FILE, p );
    }
    _splitpath2( p, sp_buf, &drive, &dir, NULL, NULL );
    MemFree( p );               /* no longer need this */
    for(;;) {
        /* we ignore any difference between the number of times we can
          loop here, and file_in_dir calc'd above */
        direntp = readdir( parent );
        if( direntp == NULL ) break;
        _makepath( path, drive, dir, direntp->d_name, NULL );
        len = strlen( path ) + 1;
        curAct->files[ curAct->num_files ] =
                                        memcpy( MemAlloc( len ), path, len );
        ++curAct->num_files;
        --files_in_dir;
        if( files_in_dir == 0 ) break;
    }
    closedir( parent );
    return( str );
}

STATIC const char *doComment( const char *str ) {

/**/myassert( str != NULL );
    while( *str != '\n' && *str != 0 ) ++str;
    return( str );
}

/*
    The next three functions (openIncludeFile, readIncludeFile, doInclude)
    require stack checking.
*/
#pragma on( check_stack );
STATIC int openIncludeFile( const char * file_name ) {

    char        buffer[ _MAX_PATH2 ];
    char        *drive;
    char        *dir;
    char        *fname;
    char        *ext;
    char        path[ _MAX_PATH ];
    int         fh;

    _splitpath2( file_name, buffer, &drive, &dir, &fname, &ext );
    _makepath( path, drive, dir, fname, ( ext[0] == 0 ) ? WMP_EXTENSION : ext );
    fh = open( path, O_RDONLY | O_TEXT );
    if( fh == -1 ) {
        Fatal( MSG_UNABLE_TO_OPEN_FILE, path );
    }
    return( fh );
}

STATIC char *readIncludeFile( int fh ) {

#define READ_SIZE   512
    size_t  file_len;
    size_t  len_read;
    char    buf[ READ_SIZE + 1 ];   /* extra byte for a sentinal */
    char    *file;
    char    *fptr;
    size_t  i;

/**/myassert( fh != -1 );
/*
    We will count the non-space characters in the file.  White space will be
    collapsed into a single space (or two spaces if it crosses a READ_SIZE
    byte boundary) to save memory.

    Be careful!  The two loops which read the file must behave identically!
    Otherwise the second loop which writes to memory might overwrite the
    buffer allocation.
*/
    file_len = 0;
/**/myassert( !isspace( 0 ) );  /* will be using 0 as a sentinal */
    for(;;) {
        len_read = read( fh, buf, READ_SIZE );
        if( len_read == 0 ) break;
        buf[ len_read ] = 0; /* a sentinal so we can speed this up a bit */
        i = 0;
        do {
            ++file_len;
            if( file_len == 0 ) {
                close( fh );
                Fatal( MSG_INCLUDE_TOO_LARGE );
            }
            if( isspace( buf[ i ] ) ) {
                do {
                    ++i;
                } while( isspace( buf[ i ] ) ); /* use sentinal to stop */
            } else {
                ++i;
            }
        } while( i < len_read );
    }
    if( file_len > 0 ) {
        /* now read the file into an array of the appropriate size */
        if( lseek( fh, (long)0, SEEK_SET ) == -1 ) {
            Fatal( MSG_DISK_ERROR, "lseek" );
        }
        file = MemAlloc( file_len + 1 );
        fptr = file;
        for(;;) {
            len_read = read( fh, buf, READ_SIZE );
            if( len_read == 0 ) break;
            buf[ len_read ] = 0;        /* set sentinal */
            i = 0;
            do {
                *fptr++ = buf[ i ];
                if( isspace( buf[ i ] ) ) {
                    do {
                        ++i;
                    } while( isspace( buf[ i ] ) ); /* use sentinal */
                } else {
                    ++i;
                }
            } while( i < len_read );
        }
        *fptr = 0;
    } else {
        file = NULL;
    }
    close( fh );
    return( file );
#undef READ_SIZE
}

FORWARD STATIC void parseString( const char *str );

STATIC const char *doInclude( const char *str ) {

    int     fh;
    char    *file;
    char    *file_name;
    const char *env_var;

/**/myassert( str != NULL );
    if( isBreakChar( *str ) ) unrecognized( '@' );
    file_name = getFile( &str );
    env_var = getenv( file_name );
    if( env_var != NULL ) {
        MemFree( file_name );
        parseString( env_var );
    } else {
        /* both these functions use a lot of stack, so we separate them */
        fh = openIncludeFile( file_name );
        MemFree( file_name );
        file = readIncludeFile( fh );
        if( file != NULL ) {
            parseString( file );
            MemFree( file );
        }
    }
    return( str );
}
#pragma off( check_stack );

STATIC void parseString( const char *str ) {

/**/myassert( str != NULL );
    for(;;) {
        while( isspace( *str ) ) ++str;
        switch( *str ) {
        case 0:
            return;
        case SWCHAR:
        case '-':
            str += 2;
            switch( str[-1] ) {
            case 'o':   str = doOutput( str );      break;
            case 'd':   str = doDebug( str );       break;
            case 'p':   str = doParse( str );       break;
            case 'f':   str = doFile( str );        break;
            case 'q':   /* FALL THROUGH */
#if 0
            case 'l':
#endif
            case 'b':   str = doToggle( str );      break;
            default:    usage();
            }
            if( !isBreakChar( *str ) ) usage();
            break;
        case INCCHAR:
            str = doInclude( str + 1 );
            break;
        case CMTCHAR:
            str = doComment( str + 1 );
            break;
        default:
            str = addFile( str );
            break;
        }
    }

}

cmdline_t *CmdLineParse( void ) {
/*****************************/

    const char  *env_var;
    char        *cmd_line;
    size_t      cmd_len;
    act_grp_t   *cur;
    act_grp_t   *next;

    tempFileNum = 0;
    env_var = getenv( ENVVAR );
    if( env_var != NULL ) {
        parseString( env_var );
    }
    cmd_line = MemAlloc( 10240 );   /* FIXME - arbitrarily large constant! */
    getcmd( cmd_line );
    cmd_len = strlen( cmd_line );
    if( cmd_len > 0 ) {
        cmd_line = MemRealloc( cmd_line, cmd_len + 1 );
        parseString( cmd_line );
    }
    MemFree( cmd_line );

    /* reverse the stack of actions */
    cmdLine.action = NULL;  /* no actions by default */
    cur = curAct;
    while( cur != NULL ) {
        next = cur->next;
        if( cur->num_files == 0 ) { /* trim out the needless actions */
            MemFree( cur );
        } else {
            cur->next = cmdLine.action; /* stack it up */
            cmdLine.action = cur;
        }
        cur = next;
    }
    if( cmdLine.action == NULL ) {
        usage();
    }
    cmdLine.quiet = cmdLine.action->quiet;
    header();
    return( &cmdLine );
}

void ActionInit( cmdline_t *cmd ) {
/*******************************/
    act_grp_t   *cur;
    int         os2_specific;

/**/myassert( cmd != NULL && cmd->action != NULL );
    cur = cmd->action;
    cmdLine.quiet = cmdLine.action->quiet;
    os2_specific = 0;
    switch( cur->omf_generator ) {
    case OGEN_NULL:
        cmd->need_output = 0;
        break;
    case OGEN_MICROSOFT_OS2:
        os2_specific = 1;
        /* fall through */
    case OGEN_MICROSOFT:
        GenMSOmfInit();
        cmd->need_output = 1;
        break;
    case OGEN_PHARLAP:
        GenPharInit();
        cmd->need_output = 1;
        break;
    default:
/**/    never_reach();
    }
    switch( cur->dbg_generator ) {
    case DGEN_NULL:
        break;
    case DGEN_TXT:
        Can2TxtInit();
        break;
    case DGEN_MICROSOFT:
        Can2MsInit( 0, os2_specific );
        break;
    case DGEN_METAWARE:
        Can2MsInit( 1, os2_specific );
        break;
    case DGEN_TURBO:
        Can2TDInit();
        break;
    default:
/**/    never_reach();
    }
    if( cur->deflib ) {
        DefLibInit();
    }
    switch( cur->dbg_parser ) {
    case PARSE_NULL:                            break;
    case PARSE_WATCOM:      Wat2CanInit( 0 );   break;
    case PARSE_WATCOM_70:   Wat2CanInit( 1 );   break;
    default:
/**/    never_reach();
    }
}

void ActionFini( cmdline_t *cmd ) {
/*******************************/

    act_grp_t   *cur;
    act_grp_t   *next;
    size_t      file_num;
    size_t      num_files;

/**/myassert( cmd != NULL && cmd->action != NULL );
    cur = cmd->action;
    switch( cur->dbg_parser ) {
    case PARSE_NULL:                            break;
    case PARSE_WATCOM:      Wat2CanFini();      break;
    case PARSE_WATCOM_70:   Wat2CanFini();      break;
    default:
/**/    never_reach();
    }
    if( cur->deflib ) {
        DefLibFini();
    }
    switch( cur->dbg_generator ) {
    case DGEN_NULL:                             break;
    case DGEN_TXT:          Can2TxtFini();      break;
    case DGEN_MICROSOFT:    Can2MsFini();       break;
    case DGEN_METAWARE:     Can2MsFini();       break;
    case DGEN_TURBO:        Can2TDFini();       break;
    default:
/**/    never_reach();
    }
    switch( cur->omf_generator ) {
    case OGEN_NULL:                             break;
    case OGEN_MICROSOFT_OS2:GenMSOmfFini();     break;
    case OGEN_MICROSOFT:    GenMSOmfFini();     break;
    case OGEN_PHARLAP:      GenPharFini();      break;
    default:
/**/    never_reach();
    }
    if( cur->output != NULL ) {
        MemFree( cur->output );
    }
    num_files = cur->num_files;
    for( file_num = 0; file_num < num_files; ++file_num ) {
        MemFree( cur->files[ file_num ] );
    }
    next = cur->next;
    MemFree( cur );
    cmd->action = next;
}

void ActionInfile( cmdline_t *cmd, char *buf, uint file_num ) {
/***********************************************************/

    char        buffer[ _MAX_PATH2 ];
    char        *drive;
    char        *dir;
    char        *fname;
    char        *ext;

/**/myassert( cmd != NULL );
/**/myassert( cmd->action != NULL );
/**/myassert( file_num < cmd->action->num_files );
    _splitpath2( cmd->action->files[ file_num ], buffer,
        &drive, &dir, &fname, &ext );
    _makepath( buf, drive, dir, fname, ( ext[0]==0 ) ? OBJ_EXTENSION : ext );
}

void ActionOutfile( cmdline_t *cmd, char *buf, uint file_num ) {
/************************************************************/

    char        sp_buf[ _MAX_PATH2 ];
    char        *drive;
    char        *dir;
    char        fname[ _MAX_FNAME ];
    const char *output;

/**/myassert( cmd != NULL );
/**/myassert( cmd->need_output );
/**/myassert( cmd->action != NULL );
/**/myassert( file_num < cmd->action->num_files );
    output = cmd->action->output;
    _splitpath2( ( output == NULL || output[0] == 0 ) ?
                cmd->action->files[ file_num ] : output,
                sp_buf, &drive, &dir, NULL, NULL );
    fname[0] = '_';
    fname[1] = 'W';
    for(;;) {
        StrDec( &fname[2], tempFileNum++ );
        _makepath( buf, drive, dir, fname, "TMP" );
        if( access( buf, 0 ) == -1 ) {
            break;
        }
    }
}

/*
    FIXME: this is an ugly function!
*/
void ActionRename( cmdline_t *cmd, const char *in, const char *out,
    uint file_num, int make_lib, size_t page_size ) {
/***************************************************/
    char        sp_buf[ _MAX_PATH2 ];
    char        sp_buf2[ _MAX_PATH2 ];
    char        *drive;
    char        *dir;
    char        *fname;
    char        *ext;
    char        buf[ _MAX_PATH ];
    const char  *output;
    act_grp_t   *cur;
    int         rc;

/**/myassert( cmd != NULL );
/**/myassert( cmd->need_output );
/**/myassert( cmd->action != NULL );
/**/myassert( file_num < cmd->action->num_files );
    cur = cmd->action;
    output = cur->output;
    if( output == NULL || output[0] == 0 ) {
        /* get the drive and directory of input file */
        _splitpath2( in, sp_buf, &drive, &dir, NULL, NULL );
        fname = "";
        ext = "";
    } else {
        /* split up the output spec */
        _splitpath2( output, sp_buf, &drive, &dir, &fname, &ext );
    }
    /* If the output spec was like '/o=.mbj' or '/o=f:\tmp\.mbj' then
       we have to use the filename and/or the extension from the input. */
    _splitpath2( cur->files[ file_num ], sp_buf2, NULL, NULL,
        ( fname[0] == 0 ) ? &fname : NULL,      /* get filename from input */
        ( ext[0] == 0 ) ? &ext : NULL );        /* get extension from input */
    if( ext[0] == 0 ) {                 /* use default extension if necessary */
        ext = OBJ_EXTENSION;
    }
    _makepath( buf, drive, dir, fname, ext );
    if( make_lib ) {
        if( cur->batch ) {
            PrtFmt( "wlib %s /b/n/p=%u +%s\n", buf, page_size, out );
            PrtFmt( "del %s\n", out );
        } else {
            char pbuf[ sizeof( size_t ) * 3 ];
            StrDec( pbuf, page_size );
#ifdef _M_I86
            _fheapshrink();
#endif
            _nheapshrink();
            rc = (int)spawnlp(P_WAIT,"wlib","wlib",buf,"/b/n/p=",pbuf,"+",out,NULL);
            if( rc < 0 ) {
                Fatal( MSG_DISK_ERROR, "spawnlp( , \"wlib\", ... )" );
            } else if( rc > 0 ) {
                Fatal( MSG_WLIB_ERROR );
            }
            if( unlink( out ) != 0 ) {
                Fatal( MSG_DISK_ERROR, "unlink" );
            }
        }
    } else if( cur->batch ) {
        PrtFmt( "if exist %s del %s\n", buf, buf );
        PrtFmt( "rename %s %s\n", out, buf );
    } else {
        unlink( buf );          /* delete any file of this name */
        if( rename( out, buf ) != 0 ) {
            Fatal( MSG_DISK_ERROR, "rename" );
        }
    }
}

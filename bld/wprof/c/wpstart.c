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


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <process.h>

#include "common.h"
#include "msg.h"
#include "dip.h"
#include "aui.h"
#include "sampinfo.h"
#include "pathlist.h"
#ifdef TRMEM
#include "trmemcvr.h"
#endif

//#include "utils.def"
//#include "msg.def"
//#include "memutil.def"
//#include "dipinter.def"
//#include "aboutmsg.def"
//#include "getsamps.def"
//#include "clrsamps.def"
//#include "rptsamps.def"
//#include "wphelp.def"

extern void WPFiniHelp(void);
extern void WPDipInit(void);
extern void WPDipFini(void);
extern void *ProfAlloc(size_t size);
extern void ProfFree(void *ptr);
extern void *ProfRealloc(void *p,size_t new_size);
extern bint GetSampleInfo(void);
extern void InitPaths(void);
extern void ErrorMsg(char *msg,... );
extern void fatal(char *msg,... );
extern void ReportSampleInfo(void);
extern void InitMADInfo(void);
extern void FiniMADInfo(void);


STATIC bint     procCmd( char * );
STATIC int      minLook( char * * );
STATIC char *   eatBlanks( char * );
STATIC char *   eatAlphaNum( char * );
STATIC char *   eatAllChars( char * );

enum {
    FAIL_OPT,
    DIP_OPT,
#if _OS == _OS_DOS
    NOCHARREMAP_OPT,
    NOGRAPHICSMOUSE_OPT,
#endif
    HELP_OPT,
    R_OPT
};

STATIC char * cmdNames[] = {
    "dip",
#if _OS == _OS_DOS
    "nocharremap",
    "nographicsmouse",
#endif
    "?",
    "help",
#ifndef NDEBUG
    "r",
#endif
    NULL
};

STATIC unsigned_8 cmdLen[] = {
    3,
#if _OS == _OS_DOS
    4,
    3,
#endif
    1,
    1,
#ifndef NDEBUG
    1
#endif
};

STATIC int cmdType[] = {
    DIP_OPT,
#if _OS == _OS_DOS
    NOGRAPHICSMOUSE_OPT,
    NOCHARREMAP_OPT,
#endif
    HELP_OPT,
    HELP_OPT,
#ifndef NDEBUG
    R_OPT
#endif
};

STATIC char * cmdUsage[] = {
    LIT( Usage1 ),
    LIT( Usage2 ),
    LIT( Usage3 ),
    LIT( Usage4 ),
    LIT( Usage5 ),
#ifndef __QNX__
    LIT( Usage6 ),
    LIT( Usage7 ),
    LIT( Usage8 ),
#endif
    NULL
};


bint        WPWndInitDone = B_FALSE;
char        SamplePath[ _MAX_PATH ];
char *      WProfDips = NULL;

static int  WProfDipSize = 0;



extern void WPInit()
/******************/
{
    char *      rover;
    bint        do_report;
    char        buff[256];

#ifdef TRMEM
    TRMemOpen();
    TRMemRedirect( STDOUT_FILENO );
#endif
    SamplePath[0] = 0;
    InitPaths();
    rover = getenv( "WPROF" );
    if( rover != NULL ) {
        procCmd( rover );
    }
    getcmd( buff );
    do_report = procCmd( buff );
    WndInit( "WATCOM Profiler" );
    WPWndInitDone = B_TRUE;
    InitMADInfo();
    WPDipInit();
    if( do_report ) {
        if( GetSampleInfo() ) {
            ReportSampleInfo();
        }
        exit( 0 );
    }
}



extern void WPFini()
/******************/
{
#ifdef TRMEM
    ClearAllSamples();
#endif
    WPFiniHelp();
    WndFini();
    WPDipFini();
    FiniMADInfo();
#ifdef TRMEM
    TRMemClose();
#endif
}



STATIC bint procCmd( char * cmd )
/*******************************/
{
    char *  rover;
    int     name_len;
    int     old_len;
    int     cmd_type;
    int     index;
    bint    do_report;
    bint    do_option;

    do_report = B_FALSE;
    for(;;) {
        cmd = eatBlanks( cmd );
        if( *cmd == NULLCHAR ) break;
#ifdef __QNX__
        if( *cmd == '-' ) {
#else
        if( *cmd == '-' || *cmd == '/' ) {
#endif
            do_option = B_TRUE;
            ++cmd;
            cmd_type = minLook( &cmd );
        } else if( *cmd == '?' ) {
            do_option = B_TRUE;
            cmd_type = HELP_OPT;
        } else {
            do_option = B_FALSE;
            rover = cmd;
            cmd = eatAllChars( cmd );
            name_len = cmd - rover;
            if( name_len > _MAX_PATH ) {
                name_len = _MAX_PATH;
            }
            memcpy( SamplePath, rover, name_len );
            SamplePath[name_len] = NULLCHAR;
        }
        if( do_option ) {
            switch( cmd_type ) {
            case FAIL_OPT:
                ErrorMsg( LIT( Cmd_Option_Not_Valid ), cmd-1 );
#if _OS == _OS_WIN || _OS == _OS_NT || defined(_OS2_PM)
                fatal( LIT( Usage ) );
#else
                /* fall through */
#endif
            case HELP_OPT:
                index = 0;
                while( cmdUsage[index] ) {
                    ErrorMsg( cmdUsage[index++] );
                }
                exit( 0 );
            case DIP_OPT:
                cmd = eatBlanks( cmd );
                if( *cmd == '=' ) {
                    cmd = eatBlanks( cmd+1 );
                }
                rover = cmd;
                cmd = eatAlphaNum( cmd );
                if( *cmd == NULLCHAR || cmd-rover == 0 ) {
                    if( WProfDips != NULL ) {
                        ProfFree( WProfDips );
                        WProfDips = NULL;
                        WProfDipSize = 0;
                    }
                } else {
                    name_len = cmd - rover;
                    old_len = WProfDipSize;
                    WProfDipSize = old_len + name_len + 1;
                    if( old_len == 0 ) {
                        WProfDipSize++;
                    } else {
                        old_len--;
                    }
                    WProfDips = ProfRealloc( WProfDips, WProfDipSize );
                    memcpy( WProfDips+old_len, rover, name_len );
                    old_len += name_len;
                    WProfDips[old_len++] = NULLCHAR;
                    WProfDips[old_len] = NULLCHAR;
                }
                break;
#if _OS == _OS_DOS
            case NOGRAPHICSMOUSE_OPT:
            case NOCHARREMAP_OPT:
                WndStyle &= ~(GUI_CHARMAP_DLG|GUI_CHARMAP_MOUSE);
                break;
#endif
#ifndef NDEBUG
            case R_OPT:
                do_report = B_TRUE;
                break;
#endif
            }
        }
    }
    return( do_report );
}



STATIC char * eatBlanks( char * cmd ) {
/*************************************/
    while( isspace( *cmd ) && *cmd != NULLCHAR ) {
        ++cmd;
    }
    return( cmd );
}



STATIC char * eatAlphaNum( char * cmd ) {
/***************************************/
    while( isalnum( *cmd ) && *cmd != NULLCHAR ) {
        ++cmd;
    }
    return( cmd );
}



STATIC char * eatAllChars( char * cmd ) {
/***************************************/
    while( !isspace( *cmd ) && *cmd != NULLCHAR ) {
        ++cmd;
    }
    return( cmd );
}



STATIC int minLook( char * * value ) {
/************************************/

    int         index;
    int         curr_len;
    void * *    strtab;
    byte *      lentab;
    char *      strlook;
    char *      strchck;
    char *      base_val;
    char        check_char;

    base_val = *value;
    lentab = cmdLen;
    strtab = cmdNames;
    index = 0;
    for(;;) {
        strlook = *strtab++;
        if( strlook == NULL ) {
            return( FAIL_OPT );
        }
        strchck = base_val;
        curr_len = 0;
        for(;;) {
            check_char = tolower( *strchck );
            if( check_char == NULLCHAR
             || !(isalpha( check_char ) || check_char == '?') ) {
                if( curr_len >= *lentab ) {
                    *value += curr_len;
                    return( cmdType[index] );
                }
                break;
            }
            if( *strlook != check_char ) break;
            strlook++;
            strchck++;
            curr_len++;
        }
        lentab++;
        index++;
    }
}

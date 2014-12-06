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
* Description:  Debugger command line processing.
*
****************************************************************************/


#include <ctype.h>
#include <stdio.h>
#define BACKWARDS
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbglit.h"
#include "dbgio.h"
#include "dbgmem.h"
#include "dui.h"
#include "wdmsg.h"
#include "dbgscrn.h"
#include "trpld.h"

extern unsigned         Lookup( char *, char *, unsigned );
extern unsigned         DUIEnvLkup( char *, char *, unsigned );
extern char             *Format( char *, char *, ... );
extern bool             OptDelim( char );
extern void             ProcSysOptInit( void );
extern bool             ProcSysOption( char *, unsigned, int );
extern char             *GetCmdArg( int );
extern void             SetCmdArgStart( int, char * );
extern void             PopErrBox( char * );
extern void             SysSetMemLimit( void );
extern void             SetNumColumns( int cols );
extern void             SetNumLines( int lines );
extern char             *DupStr( char * );
extern char             *StrCopy( char *src, char *dst );
void                    FindLocalDebugInfo( char *name );
extern void             StartupErr( char *err );


static char             *(*GetArg)( int );
static int              CurrArgc;
static char             *CurrArgp;
static char             CurrChar;

bool                    DownLoadTask;

#define MIN_MEM_SIZE    (500UL*1024)


static char OptNameTab[] = {
    "Invoke\0"
    "NOInvoke\0"
    "NOSYmbols\0"
    "NOMouse\0"
    "DIp\0"
    "DYnamic\0"
    "TRap\0"
    "REMotefiles\0"
#ifdef BACKWARDS
    "NOFpu\0"
#endif
    "LInes\0"
    "COlumns\0"
#ifdef BACKWARDS
    "NOAltsym\0"
    "REGisters\0"
#endif
    "INItcmd\0"
    "POWerbuilder\0"
    "LOcalinfo\0"
    "NOExports\0"
    "DOwnload\0"
    "DEfersymbols\0"
    "NOSOurcecheck\0"
    "CONtinueunexpectedbreak\0"
    "Help\0"
#ifdef ENABLE_TRAP_LOGGING
    "TDebug\0"
    "TFDebug\0"
#endif
};

enum { OPT_INVOKE=1,
       OPT_NOINVOKE,
       OPT_NOSYMBOLS,
       OPT_NOMOUSE,
       OPT_DIP,
       OPT_DYNAMIC,
       OPT_TRAP,
       OPT_REMOTE_FILES,
#ifdef BACKWARDS
       OPT_NO_FPU,
#endif
       OPT_LINES,
       OPT_COLUMNS,
#ifdef BACKWARDS
       OPT_NO_ALTSYM,
       OPT_REGISTERS,
#endif
       OPT_INITCMD,
       OPT_POWERBUILDER,
       OPT_LOCALINFO,
       OPT_NOEXPORTS,
       OPT_DOWNLOAD,
       OPT_DEFERSYM,
       OPT_NOSOURCECHECK,
       OPT_CONTINUE_UNEXPECTED_BREAK,
       OPT_HELP,
#ifdef ENABLE_TRAP_LOGGING
       OPT_TRAP_DEBUG,
       OPT_TRAP_DEBUG_FLUSH,
#endif
};


void SetupChar( void )
{
   CurrChar = *CurrArgp;
   if( CurrChar == NULLCHAR ) {
        CurrChar = ' ';
        CurrArgp = GetArg( ++CurrArgc );
        if( CurrArgp == NULL ) {
            --CurrArgc;
            CurrChar = ARG_TERMINATE; /* absolute end of arguments */
        } else {
            --CurrArgp; /* so that NextChar increment is negated */
        }
    }
}


void NextChar( void )
{
    ++CurrArgp;
    SetupChar();
}


void OptError( char *err )
{
    char        *curr;
    char        buff[CMD_LEN];
    char        token[CMD_LEN];

    curr = token;
    while( isalnum( CurrChar ) ) {
        *curr++ = CurrChar;
        NextChar();
    }
    if( curr == token ) {
        if( CurrChar == ARG_TERMINATE ) CurrChar = ' ';
        *curr++ = CurrChar;
    }
    *curr = NULLCHAR;
    Format( buff, err, token );
    StartupErr( buff );
}


void SkipSpaces( void )
{
    while( CurrChar == ' ' || CurrChar == '\t' ) {
        NextChar();
    }
}


bool HasEquals( void )
{
    SkipSpaces();
    return( CurrChar == '=' || CurrChar == '#' );
}


void WantEquals( void )
{
    SkipSpaces();
    if( CurrChar != '=' && CurrChar != '#' ) OptError( LIT( STARTUP_Expect_Eql ) );
    NextChar();
    SkipSpaces();
}


/*
 * GetValueLong -- get a long numeric value from command line
 */
unsigned long GetValueLong( void )
{
    unsigned long val;

    WantEquals();
    if( !isdigit( CurrChar ) ) OptError( LIT( STARTUP_Invalid_Num ) );
    val = 0;
    do {
        val = val * 10 + CurrChar - '0';
        NextChar();
    } while( isdigit( CurrChar ) );
    return( val );
}

unsigned GetValue( void )
{
    unsigned long val;

    val = GetValueLong();
    if( val > 0xffff ) OptError( LIT( STARTUP_Num_Too_Big ) );
    return( val );
}

unsigned long GetMemory( void )
{
    unsigned long   val;

    val = GetValueLong();
    if( tolower( CurrChar ) == 'k' ) {
        val *= 1024;
        NextChar();
    } else if( tolower( CurrChar ) == 'b' ) {
        NextChar();
    } else if( val < 1000 ) {
        val *= 1024;
    }
    return( val );
}

static void DoGetItem( char *buff, bool stop_on_first )
{
    for( ;; ) {
        if( CurrChar == ' ' ) break;
        if( CurrChar == '\t' ) break;
        if( CurrChar == ARG_TERMINATE ) break;
        if( CurrChar == TRAP_PARM_SEPARATOR ) break;
        if( CurrChar == '{' ) break;
        if( OptDelim( CurrChar ) && stop_on_first ) break;
        *buff++ = CurrChar;
        NextChar();
        stop_on_first = TRUE;
    }
    *buff = NULLCHAR;
}

void GetItem( char *buff )
{
    DoGetItem( buff, TRUE );
}


/*
 * GetFileName -- get filename from command line
 */
char *GetFileName( int pass )
{
    char        buff[CMD_LEN];

    WantEquals();
    GetItem( buff );
    return( pass == 1 ? NULL : DupStr( buff ) );
}

void GetRawItem( char *start )
{
    unsigned num;

    SkipSpaces();
    if( CurrChar == '{' ) {
        NextChar();
        num = 1;
        for( ;; ) {
            if( CurrChar == ARG_TERMINATE ) {
                StartupErr( LIT( STARTUP_Expect_Brace ) );
            } else if( CurrChar == '{' ) {
                ++num;
            } else if( CurrChar == '}' ) {
                if( --num == 0 ) {
                    NextChar();
                    break;
                }
            }
            *start++ = CurrChar;
            NextChar();
        }
        *start = NULLCHAR;
    } else {
        DoGetItem( start, FALSE );
    }
}

static void GetTrapParm( int pass )
{
    char    *start;
    char    parm[TRP_LEN];

    start = parm;
    SkipSpaces();
    *start++ = TRAP_PARM_SEPARATOR;
    GetRawItem( start );
    if( pass == 2 ) {
        _Alloc( start, strlen( parm ) + strlen( TrapParms ) + 1 );
        StrCopy( parm, StrCopy( TrapParms, start ) );
        _Free( TrapParms );
        TrapParms = start;
    }
}

static void GetInitCmd( int pass )
{
    char    cmd[CMD_LEN];

    WantEquals();
    SkipSpaces();
    GetRawItem( cmd );
    if( pass == 2 ) {
        _Free( InitCmdList );
        _Alloc( InitCmdList, strlen( cmd ) + 1 );
        StrCopy( cmd, InitCmdList );
    }
}

#ifndef __GUI__
static void PrintUsage( int first_ln )
{
    char        *msg_buff;

    for( ;; first_ln++ ) {
        msg_buff = DUILoadString( first_ln );
        if( ( msg_buff[ 0 ] == '.' ) && ( msg_buff[ 1 ] == 0 ) )
            break;
        puts( msg_buff );
    }
}
#endif

/*
 * ProcOptList -- process an option list
 */

static void ProcOptList( int pass )
{
    char        buff[80];
    char        err_buff[CMD_LEN];
    char        *curr;
    unsigned long   mem;

    SetupChar(); /* initialize scanner */
    for( ;; ) {
        SkipSpaces();
        if( !OptDelim( CurrChar ) ) break;
        NextChar();
        curr = buff;
#ifndef __GUI__
        if( CurrChar == '?' ) {
            PrintUsage( MSG_USAGE_BASE );
            StartupErr( "" );
        }
#endif
        while( isalnum( CurrChar ) ) {
            *curr++ = CurrChar;
            NextChar();
        }
        if( curr == buff ) {
            if( OptDelim( CurrChar ) ) {
                NextChar();
                SkipSpaces();
                break;
            }
            OptError( LIT( STARTUP_No_Recog_Optn ) );
        }
        switch( Lookup( OptNameTab, buff, curr - buff ) ) {
        case OPT_CONTINUE_UNEXPECTED_BREAK:
            _SwitchOn( SW_CONTINUE_UNEXPECTED_BREAK );
            break;
        case OPT_DEFERSYM:
            _SwitchOn( SW_DEFER_SYM_LOAD );
            break;
        case OPT_DOWNLOAD:
            DownLoadTask = TRUE;
            break;
        case OPT_NOEXPORTS:
            _SwitchOn( SW_NO_EXPORT_SYMS );
            break;
        case OPT_LOCALINFO:
            if( pass == 2 ) {
                char *file = GetFileName( pass );
                FindLocalDebugInfo( file );
                _Free( file );
            }
            break;
        case OPT_INVOKE:
            if( pass == 2 ) _Free( InvokeFile );
            InvokeFile = GetFileName( pass );
            break;
        case OPT_NOINVOKE:
            if( pass == 2 ) _Free( InvokeFile );
            InvokeFile = NULL;
            break;
        case OPT_NOSOURCECHECK:
            _SwitchOff( SW_CHECK_SOURCE_EXISTS );
            break;
        case OPT_NOSYMBOLS:
            _SwitchOff( SW_LOAD_SYMS );
            break;
        case OPT_NOMOUSE:
            _SwitchOff( SW_USE_MOUSE );
            break;
        case OPT_DYNAMIC:
            mem = GetMemory();
            if( pass == 1 ) {
                if( mem < MIN_MEM_SIZE ) mem = MIN_MEM_SIZE;
                MemSize = mem;
            }
            break;
        case OPT_DIP:
            {
                int i;

                for( i = 0; DipFiles[ i ] != NULL; ++i ) ;
                DipFiles[ i ] = GetFileName( pass );
            }
            break;
        case OPT_TRAP:
            if( pass == 2 )
                _Free( TrapParms );
            TrapParms = GetFileName( pass );
            SkipSpaces();
            if( CurrChar == TRAP_PARM_SEPARATOR ) {
                NextChar();
                GetTrapParm( pass );
            } else if( CurrChar == '{' ) {
                GetTrapParm( pass );
            }
            break;
#ifdef ENABLE_TRAP_LOGGING
        case OPT_TRAP_DEBUG_FLUSH:
            if( pass == 2 )
                _Free( TrapTraceFileName );
            TrapTraceFileName = GetFileName( pass );
            TrapTraceFileFlush = TRUE;
            break;
        case OPT_TRAP_DEBUG:
            if( pass == 2 )
                _Free( TrapTraceFileName );
            TrapTraceFileName = GetFileName( pass );
            TrapTraceFileFlush = FALSE;
            break;
#endif
        case OPT_REMOTE_FILES:
            _SwitchOn( SW_REMOTE_FILES );
            break;
        case OPT_LINES:
            SetNumLines( GetValue() );
            break;
        case OPT_COLUMNS:
            SetNumColumns( GetValue() );
            break;
#ifdef BACKWARDS
        case OPT_NO_FPU:
        case OPT_NO_ALTSYM:
            break;
        case OPT_REGISTERS:
            GetValue();
            break;
#endif
        case OPT_INITCMD:
            GetInitCmd( pass );
            break;
        case OPT_POWERBUILDER:
            _SwitchOn( SW_POWERBUILDER );
            break;
        case OPT_HELP:
#ifndef __GUI__
            PrintUsage( MSG_USAGE_BASE );
            StartupErr( "" );
#endif
            break;
        default:
            if( !ProcSysOption( buff, curr - buff, pass ) ) {
                Format( err_buff, LIT( STARTUP_Invalid_Option ), buff, curr - buff );
                StartupErr( err_buff );
            }
            break;
        }
    }
}

OVL_EXTERN char *GetEnvArg( int i )
{
    i = i;
    return( NULL );
}



/*
 * ProcCmd -- start processing command line options
 */

void ProcCmd( void )
{
    char        buff[TXT_LEN];
    unsigned    screen_mem;
    unsigned    have_env;
    int         pass;

    MemSize = MIN_MEM_SIZE;
    TrapParms = NULL;
    _SwitchOn( SW_LOAD_SYMS );
    _SwitchOn( SW_USE_MOUSE );
    ProcSysOptInit();

    have_env = DUIEnvLkup( EXENAME, buff, sizeof( buff ) );
    for( pass = 1; pass <= 2; ++pass ) {
        if( have_env ) {
            GetArg = &GetEnvArg;
            CurrArgc = 0;
            CurrArgp = buff;
            ProcOptList( pass );
            if( CurrChar != ARG_TERMINATE ) {
                OptError( LIT( STARTUP_Expect_End_Env_Str ) );
            }
        }
        GetArg = &GetCmdArg;
        CurrArgc = 0;
        CurrArgp = GetCmdArg( 0 );
        if( CurrArgp != NULL ) {
            ProcOptList( pass );
            if( pass == 2 ) {
                SetCmdArgStart( CurrArgc, CurrArgp );
            }
        }
        if( pass == 1 ) {
            screen_mem = ConfigScreen();
            if( MemSize + screen_mem >= MemSize ) {
                MemSize += screen_mem;
            } else {
                MemSize = ~0;
            }
            SysSetMemLimit();
            TrapParms = DupStr( "std" );
            InvokeFile = DupStr( "" );
        }
    }
}

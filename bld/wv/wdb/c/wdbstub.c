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
* Description:  Commandline Debugger with empty stub functions.
*
****************************************************************************/


/*
To do:
    1) Remove Break Point
    2) Enable Break Point
    3) Disable Break Point
    4) Get Thread List
    5) Switch to a Thread
    6) Dis Assemble
    7) Kill Process
    8) Attach to Process
    9) Register Values

Done:
    1) Load File                    -load , file
    2) Run                          -run
    3) Continue                     -c
    4) Set Break Point              -break file:line
    5) Back Trace                   -bt
    6) Quit                         -q
    7) Restart                      -restart
    8) Get Local Variable List      -bt full , info locals
    9) Module List                  -module
    10) Get Variable Value          -print variable
    11) Run until return;           -finish
    12) Step Over                   -step
    13) Step Into                   -next
    14) List All break point        -info break
    15) Remove All Break Points     -delete breakpoints
    16) Enable All Break Points     -enable breakpoints
    17) Disable All Break Points    -disable breakpoints
    18) Add Source path             -directory filepath
    19) Remove All Source paths     -directory
    20) Show Source paths           -show directories
*/


#include <stdio.h>
#include <ctype.h>
#include <process.h>
#if defined( __NT__ )
#include <windows.h>
#elif defined( __OS2__ )
#include <os2.h>
#elif defined( __LINUX__ )
#endif
#include "_srcmgt.h"
#include "dbgdata.h"
#include "dbgmem.h"
#include "dbglit.h"
#include "mad.h"
#include "dui.h"
#include "dbgvar.h"
#include "modlist.h"
#include "srcmgt.h"
#include "dbgscrn.h"
#include "dbgscan.h"
#include "dbgutil.h"
#include "dbgsrc.h"
#include "dbgexec.h"
#include "dbgmain.h"
#include "dbgbrk.h"
#include "dbgass.h"
#include "dbgpend.h"
#include "envlkup.h"
#include "dbgprog.h"

enum {
    REQ_NONE,
    REQ_BYE,
    REQ_GO,
    REQ_TRACE_OVER,
    REQ_TRACE_INTO
} Req = REQ_NONE;

/* External Functions Used */
extern void             CallResults( void );
extern void             DlgCmd( void );
extern void             ExecTrace( trace_cmd_type type, debug_level level );
extern address          GetCodeDot( void );
extern void             GoToReturn( void );
extern address          ModFirstAddr( mod_handle mod );
extern void             PerformTrace( void );
extern void             ProcCall( void );
extern void             ProcGo( void );
extern void             ShowCalls( void );
extern void             ShowVarDisplay( void );
extern void             TraceKill( void );
extern var_node         *VarGetDisplayPiece( var_info *i, int row, int piece, int *pdepth, int *pinherit );
extern void             *WndAsmInspect( address addr );
extern bool             WndEvalInspectExpr( const char *item, bool pop );
extern inspect_type     WndGetExprSPInspectType( address *paddr );

volatile bool           BrkPending;

static char             *CmdData;
static bool             Done;

/* Constants Used in this File */

/*
Whenever you add a short cut command, please the 
corresponding help command in the process_cmd_help function
*/
static const char   *WDB_CMD_QUIT = "quit";
static const char   *WDB_SHORT_CMD_QUIT = "q";
static const char   *WDB_HELP_QUIT = "q,x,quit,exit - Quit the program\n";
static const char   *WDB_CMD_EXIT = "exit";
static const char   *WDB_SHORT_CMD_EXIT = "x";
static const char   *WDB_HELP_EXIT = "x,exit - Quit the program\n";
static const char   *WDB_CMD_PRINT = "print";
static const char   *WDB_SHORT_CMD_PRINT = "p";
static const char   *WDB_HELP_PRINT = "p,print - Print the value of a Variable\n";
static const char   *WDB_CMD_RUN = "run";
static const char   *WDB_SHORT_CMD_RUN = "r";
static const char   *WDB_HELP_RUN = "r,run - Run the loaded program\n";
static const char   *WDB_CMD_CONTINUE = "continue";
static const char   *WDB_SHORT_CMD_CONTINUE = "c";
static const char   *WDB_HELP_CONTINUE = "c,continue - Continue the loaded program\n";
static const char   *WDB_CMD_RESTART = "restart";
static const char   *WDB_SHORT_CMD_RESTART = "re";
static const char   *WDB_HELP_RESTART = "re,restart - Start the currently loaded program\n";
static const char   *WDB_CMD_LOAD = "load";
static const char   *WDB_SHORT_CMD_LOAD = "l";
static const char   *WDB_HELP_LOAD = "l,load,file - Load a given program in the debugger\nusage:\n\tload program cmd_line\n\tload \"long program exe name\" cmd_line\n";
static const char   *WDB_CMD_FILE = "file";
static const char   *WDB_SHORT_CMD_FILE = "file";
static const char   *WDB_CMD_BREAKPOINT = "breakpoint";
static const char   *WDB_SHORT_CMD_BREAKPOINT = "b";
static const char   *WDB_HELP_BREAKPOINT = "b,breakpoint - Set the breakpoint\nusage:\n\tbreakpoint filename:line_number\n\tbreakpoint \"long source fileName\":line_number\n";
static const char   *WDB_CMD_DELETE = "delete";
static const char   *WDB_SHORT_CMD_DELETE = "d";
static const char   *WDB_HELP_DELETE = "d,delete - Delete the breakpoints\nusage:\n\tdelete breakpoints 1 2 3\n\t for all break points - delete breakpoints\n";
static const char   *WDB_CMD_ENABLE = "enable";
static const char   *WDB_SHORT_CMD_ENABLE = "e";
static const char   *WDB_HELP_ENABLE = "e,enable - enable the breakpoints\nusage:\n\tfor specific break points enable breakpoints 1 2 3\n\t for all break points - enable breakpoints\n";
static const char   *WDB_CMD_DISABLE = "disable";
static const char   *WDB_SHORT_CMD_DISABLE = "d";
static const char   *WDB_HELP_DISABLE = "d,disable - disable the breakpoints\nusage:\n\tdisable breakpoints 1 2 3\n\t for all break points - disable breakpoints\n";
static const char   *WDB_CMD_INFO = "info";
static const char   *WDB_SHORT_CMD_INFO = "i";
static const char   *WDB_HELP_INFO = "i,info \nusage:\n\tinfo break - list all breakpoints\n\tinfo locals - list of locals\n";
static const char   *WDB_CMD_NEXT = "next";
static const char   *WDB_SHORT_CMD_NEXT = "n";
static const char   *WDB_HELP_NEXT = "n,next - Step into the next line\n";
static const char   *WDB_CMD_STEP = "step";
static const char   *WDB_SHORT_CMD_STEP = "s";
static const char   *WDB_HELP_STEP = "s,step - Step over the line\n";
static const char   *WDB_CMD_FINISH = "finish";
static const char   *WDB_SHORT_CMD_FINISH = "finish";
static const char   *WDB_HELP_FINISH = "finish - Run until the end of the function\n";
static const char   *WDB_CMD_BACKTRACE = "backtrace";
static const char   *WDB_SHORT_CMD_BACKTRACE = "bt";
static const char   *WDB_HELP_BACKTRACE  = "bt,backtrace - Shows the backtrace of the execution\n";
static const char   *WDB_CMD_MODULE = "module";
static const char   *WDB_SHORT_CMD_MODULE = "m";
static const char   *WDB_HELP_MODULE  = "m,module - Shows the list of Modules\n";
static const char   *WDB_CMD_ATTACH = "attach";
static const char   *WDB_SHORT_CMD_ATTACH = "a";
static const char   *WDB_HELP_ATTACH = "a,attach -  Attach the debugger to the given PID\nusage:\n\tattach PID\n";
static const char   *WDB_CMD_KILL = "kill";
static const char   *WDB_SHORT_CMD_KILL = "k";
static const char   *WDB_HELP_KILL = "k,kill - Kills the loaded program\n";
static const char   *WDB_CMD_SHOW = "show";
static const char   *WDB_SHORT_CMD_SHOW = "show";
static const char   *WDB_HELP_SHOW = "show - Show Params - param list - directory\n";
static const char   *WDB_CMD_DIRECTORY = "directory";
static const char   *WDB_SHORT_CMD_DIRECTORY = "d";
static const char   *WDB_HELP_DIRECTORY = "d,directory - Set/UnSet the Source directories\n";
static const char   *WDB_CMD_TEST = "test";
static const char   *WDB_SHORT_CMD_TEST = "test";
static const char   *WDB_CMD_HELP = "help";
static const char   *WDB_SHORT_CMD_HELP = "h";
static const char   *WDB_HELP_HELP = "h,help - Shows this info\n";
static const char   *WDB_HELP_PARAM = "/?";


void ShowDebuggerPrompt( void )
{
    fflush( stdout );
    printf( "\nwdb > " );
    fflush( stdout );
}

void ShowDebuggerError( const char *error_str )
{
    fflush( stdout );
    printf( "\n%s", error_str );
    fflush( stdout );
}

void ShowDebuggerMsg( const char *msg_str )
{
    fflush( stdout );
    printf( "\n%s", msg_str );
    fflush( stdout );
}

char *StrLTrim( char *s )
{
    if( s != NULL ) {
        size_t  len = 0;
        char    *t = s;

        /* find the first non-space character */
        while( *t != '\0' && isspace( (unsigned char) *t )) {
            ++t;
        }
        len = strlen( t ) + 1;
        memmove( s, t, len );
    }
    return( s );
}

char *StrRTrim( char *s )
{
    if( s != NULL ) {
        int     done = 0;
        size_t  len = strlen( s );

        while( !done ) {
            if( 0 == len ) {
                done = 1;
            } else if( isspace( (unsigned char)s[len - 1] ) ) {
                s[len - 1] = '\0';
            } else {
                done = 1;
            }
            --len;
        }
    }
    return( s );
}

char *MyStrTrim( char *s )
{
    StrLTrim( s );
    StrRTrim( s );
    return( s );
}

const char *MyStrTrimLen( const char *start, unsigned *len )
{
    unsigned    new_len;

    new_len = *len;
    while( new_len > 0 && isspace( (unsigned char)*start )) {
        ++start;
        --new_len;
    }
    while( new_len > 0 && isspace( (unsigned char)start[new_len - 1] ) ) {
        --new_len;
    }
    *len = new_len;
    return( start );
}

char *GetCmdPartByChar( const char *cmd, const char *delimit )
{
    char    *result;
    size_t  pos_value = strcspn( cmd, delimit );

    if( pos_value == strlen( cmd ) ) {
        result = malloc( sizeof( char ) * ( strlen( cmd ) + 1 ) );
        strncpy( result, cmd, strlen( cmd ) );
        result[strlen( cmd )] = '\0';
    } else {
        result = malloc( sizeof( char )*( pos_value + 1 ) );
        strncpy( result, cmd, pos_value );
        result[pos_value] = '\0';
    }
    MyStrTrim( result );
    return( result );
}

char *GetCmdPart( const char *cmd )
{
    return( GetCmdPartByChar( cmd, " " ) );
}

char *GetParamPartByChar( const char *cmd, const char *delimit )
{
    char    *result;
    size_t  pos_value = strcspn( cmd, delimit );

    if( pos_value == strlen( cmd ) ) {
        result = malloc( sizeof( char ) * 1 );
        strncpy( result, "\0" , 1 );
    } else {
        result = malloc( sizeof( char ) * ( strlen( cmd ) - pos_value + 1 ) );
        strncpy( result, &cmd[pos_value+1], strlen( cmd ) - pos_value + 1 );
        result[strlen( cmd ) - pos_value] = '\0';
    }
    MyStrTrim( result );
    return( result );
}

char *GetParamPart( const char *cmd )
{
    return( GetParamPartByChar( cmd, " " ) );
}

char *GetFirstQuotedPart( const char *cmd )
{
    char    *result;

    if( cmd[0] == '"' && strlen( cmd ) > 1 ) {
        result = GetCmdPartByChar( cmd + 1, "\"" );
    } else {
        result = GetCmdPart( cmd );
    }
    return( result );
}

char *GetSecondQuotedPart( const char *cmd )
{
    char    *result;

    if( cmd[0] == '"' && strlen( cmd ) > 1 ) {
        result = GetParamPartByChar( cmd + 1, "\"" );
    } else {
        result = GetParamPart( cmd );
    }
    return( result );
}


bool IsCmdEqualCmd( const char *cmd, const char *e_cmd, const char *e_short_cmd )
{
    bool comparefull  = !(strlen( cmd ) != strlen( e_cmd ));
    bool compareshort = !(strlen( cmd ) != strlen( e_short_cmd ));

    /* if the length of the command does not match then we'll ignore the command */
    if( (!comparefull) && (!compareshort) ) {
        return( FALSE );
    }
    /* otherwise we'll check if they are same */
    if( comparefull && ( _strnicmp( cmd, e_cmd, strlen( e_cmd ) ) == 0 ) ) {
        return( TRUE );
    } else if( compareshort && ( _strnicmp( cmd, e_short_cmd, strlen( e_short_cmd ) ) == 0 ) ) {
        return( TRUE );
    }
    return( FALSE );
}


bool IsCmdEqualCmd2( const char *cmd, const char *e_cmd )
{
    /* if the length of the command does not match then we'll ignore the command */
    if( strlen( cmd ) != strlen( e_cmd ) ) {
        return( FALSE );
    }
    /* otherwise we'll check if they are same */
    if( _strnicmp( cmd , e_cmd, strlen( e_cmd ) ) == 0 ) {
        return( TRUE );
    } else {
        return( FALSE );
    }
}

/*for setting a breakpoint in a file,
we need to check is the file is in the 
module list. then from the module we get the cuFileID
and cuMod. Then using the cuFileID, cuMod and line number
we can find the breakoint address associated with the file
and linenumber. Then use the address to set the break point.

*/
address SetBreakPointInFile( char *filename,int line_num )
{
    module_list         list;
    address             mod_address = NilAddr;
    address             bp_address = NilAddr;
    brkp                *bp;
    int                 mod_nums;
    int                 row;
    char                mod_name[200];
    bool                bp_handled = FALSE;
    DIPHDL( cue, ch );

    // Line numbers in Window start from 0, so we'll subtract one
    line_num = line_num - 1;
    ModListInit( &list, NULL );
    ModListAddModules( &list, NO_MOD, FALSE );
    mod_nums = ModListNumRows( &list );
    for( row = 0; row < mod_nums; ++row ) {
        ModListName( &list, row, &mod_name[0] );
        if( IsCmdEqualCmd2( mod_name, filename ) ) {
            ShowDebuggerMsg( mod_name );
            mod_address = ModFirstAddr( ModListMod ( &list, row ) );
            if( DeAliasAddrCue( ModListMod ( &list, row ), mod_address, ch ) == SR_NONE ) {
            } else {
                bp_address = GetRowAddrDirectly( CueMod( ch ), CueFileId( ch ), line_num, FALSE );
                if( !IS_NIL_ADDR( bp_address ) ) {
                    bp = AddBreak( bp_address );
                    bp_handled = TRUE;
                }
            }
            break;
        }
    }
    ModListFree( &list );
    if( bp_handled ) {
        ShowDebuggerMsg( "Sucessfully added the Break point." );
    } else {
        ShowDebuggerMsg( "Unable to set Break point at the requested line." );
    }
    return( mod_address );
}

/* List the Modules used by the program */
void ShowModuleList( void )
{
    module_list     list;
    int             mod_nums;
    int             row;
    char            mod_name[200];

    ModListInit( &list, NULL );
    ModListAddModules( &list, NO_MOD, FALSE );
    mod_nums = ModListNumRows( &list );
    for( row = 0; row < mod_nums; ++row ){
        ModListName( &list, row, &mod_name[0]);
        ShowDebuggerMsg( mod_name );
    }
    printf( "\nNumber of Modules  Found = %d", mod_nums );
    fflush( stdout );
    ModListFree( &list );
}

/*recursively expand and show the */
    //fixme : The class and structure values need to be displayed
static void DisplayDebuggerVarRecursively( var_info *pVarInfoList, var_node *v )
{
    var_node    *e;

    /*temp code start*/
    VarBuildName( pVarInfoList , v, TRUE );
    printf( "%s = {\n", TxtBuff );
    printf( "}\n" );
    fflush( stdout );
    /*temp code end*/
    return;

    if( v == NULL )
        return;
    VarBuildName( pVarInfoList , v, TRUE );
    printf( "%s = {\n", TxtBuff );
    if( (v->gadget == VARGADGET_CLOSED) || (v->gadget == VARGADGET_POINTS) || (v->gadget == VARDISP_INHERIT_CLOSED) ) {
        e = VarFirstExpandNode( pVarInfoList, v );
        DisplayDebuggerVarRecursively( pVarInfoList, e );
    } else {
        printf( "%s\n", v->value );
    }
    printf( "}\n" );
    fflush( stdout );
}

/*Display a variable value*/
void DisplayDebuggerVarValue( var_info *pVarInfoList )
{
    int         row;
    int         depth, inherited;
    var_node    *v;

    for( row = 0; ; ++row ) {
        v = VarGetDisplayPiece( pVarInfoList, row, (int)VAR_PIECE_GADGET, &depth , &inherited );
        if( v == NULL )
            break;
        v = VarGetDisplayPiece( pVarInfoList, row, (int)VAR_PIECE_NAME, &depth , &inherited );
        v = VarGetDisplayPiece( pVarInfoList, row, (int)VAR_PIECE_VALUE, &depth , &inherited );
        switch( v->gadget ) {
        case VARGADGET_NONE:
            printf( "" );
            break;
        case VARGADGET_OPEN:
            printf( "+ " );
            break;
        case VARGADGET_CLOSED:
            printf( "- " );
            break;
        case VARGADGET_POINTS:
            printf( "->" );
            break;
        case VARGADGET_UNPOINTS:
            printf( "<-" );
            break;
        }
        if( (v->gadget == VARGADGET_CLOSED) || (v->gadget == VARGADGET_POINTS) || (v->gadget == VARDISP_INHERIT_CLOSED) ) {
            //fixme: Print the expanded list
            //printf( "%s = { \n%s\n}\n", TxtBuff, v->value );
            DisplayDebuggerVarRecursively( pVarInfoList, v );
        } else {
            VarBuildName( pVarInfoList , v, TRUE );
            printf( "%s = { \n%s\n}\n", TxtBuff, v->value );
        }

        fflush( stdout );
    }
}

/*Display a variable's value we are interested in*/
bool InspectDebuggerVar( const char *item )
{
    var_info        InspectVars;
    address         addr;
    inspect_type    t;
    var_node        *v;

    VarInitInfo( &InspectVars );
    if( WndEvalInspectExpr( item, FALSE ) ) {
        t = WndGetExprSPInspectType( &addr );
        switch( t ) {
            //case INSP_CODE:
            //    WndSrcOrAsmInspect( addr );
            //    return( TRUE );
            case INSP_DATA:
                {
                    v = VarAdd1( &InspectVars, item, strlen( item ), TRUE, FALSE );
                    if( v != NULL ) {
                        DisplayDebuggerVarValue( &InspectVars );
                    }
                    return( v != NULL );
                }
            case INSP_RAW_DATA:
                //WndAddrInspect( addr );
                return( TRUE );
        }
    }
    return( FALSE );
}

/*display the local values
*/
static void DumpLocals( void )
{
    address     addr;
    var_info    Locals;

    VarInitInfo( &Locals );
    if( !_IsOn( SW_TASK_RUNNING ) ) {
        VarErrState();
        VarInfoRefresh( VAR_LOCALS, &Locals, &addr, NULL );
        VarOkToCache( &Locals, TRUE );
    }

    DisplayDebuggerVarValue( &Locals );

    if( !_IsOn( SW_TASK_RUNNING ) ) {
        VarOkToCache( &Locals, FALSE );
        VarOldErrState();
    }
}

/*navigate through the global src path variable
and display the path one at a time*/
void ShowSourceDirectories( void )
{
    char_ring *p;

    for( p = SrcSpec; p != NULL; p = p->next ) {
        ShowDebuggerMsg( p->name );
    }
}

/*trims and adds a given source path to
the global src path variable*/
void AddSourcePathsToDebugger( const char *srcpath )
{
    // if possible replace strtok with other 
    // efficient tokenizer
    const char  *path;
    const char  *end;
    const char  *s;
    unsigned    len;

    path = srcpath;
    while( (end = strchr( path, ';' )) != NULL ) {
        len = end - path;
        s = MyStrTrimLen( path, &len );
        AddSourceSpec( s, len );
        path = end + 1;
    }
    len = strlen( path );
    s = MyStrTrimLen( path, &len );
    if( len > 0 ) {
        AddSourceSpec( s, len );
    }
}

/* Clear the global variable the hold the
Src path and add default path */
void RemoveSourcePathsFromDebugger( void )
{
    FiniSource();
    AddSourcePathsToDebugger( "." );
}

/*source copied from the engnt code.
this displays the code at which the current execution
is broken
*/
void DumpSource( void )
{
    char        buff[256];
    DIPHDL( cue, ch );

    if( _IsOn( SW_TASK_RUNNING ) ) {
        ShowDebuggerMsg( "I don't know where the task is.  It's running\n" );
    }
    if( DeAliasAddrCue( NO_MOD, GetCodeDot(), ch ) == SR_NONE ||
        !DUIGetSourceLine( ch, buff, sizeof( buff ) ) ) {
        UnAsm( GetCodeDot(), buff, sizeof( buff ) );
    }
    printf( "%s\n", buff );
    fflush(stdout);
}

/*for debug trace step next and step into*/
void PerformDebuggerTrace( int _trace_type )
{
    ExecTrace( _trace_type, DbgLevel );
    PerformTrace();
    DumpSource();
}

/*make the debugger to finish until the return of the
current function*/
void PerformDebuggerFinish( void )
{
    GoToReturn();
    PerformTrace();
    DumpSource();
}

/*parse the bp numbers (bps should be  seperated by a space)
and delete the bps one at a time
*/
bool DeleteBps( char *params )
{
    //if possible replace strtok with other 
    //efficient tokenizer
    char    *num;
    int     index;
    brkp    *bp;

    num = strtok( params, " " );
    while( num != NULL ) {
        MyStrTrim( num );
        index = atoi( num );
        bp = GetBPAtIndex( index );
        RemovePoint( bp );
        num = strtok( NULL, " " );
    }
    return( TRUE );
}

/*parse the bp numbers (bps should be  seperated by a space)
and enable the bps one at a time
*/
bool DisableBps( char *params )
{
    //if possible replace strtok with other 
    //efficient tokenizer
    char    *num;
    int     index;
    brkp    *bp;

    num = strtok( params, " " );
    while( num != NULL ) {
        MyStrTrim( num );
        index = atoi( num );
        bp = GetBPAtIndex( index );
        ActPoint( bp, FALSE );
        num = strtok( NULL, " " );
    }
    return( TRUE );
}

/*parse the bp numbers (bps should be  seperated by a space)
and enable the bps one at a time
*/
bool EnableBps( char *params )
{
    //if possible replace strtok with other 
    //efficient tokenizer
    char    *num;
    int     index;
    brkp    *bp;

    num = strtok( params, " " );
    while( num != NULL ) {
        MyStrTrim( num );
        index = atoi( num );
        bp = GetBPAtIndex( index );
        ActPoint( bp, TRUE );
        num = strtok( NULL, " " );
    }
    return( TRUE );
}

bool IsDbgProgramLoaded( void )
{
    return( _IsOn( SW_HAVE_TASK ) );
}

bool IsCmdQuit( const char *cmd )
{
    return( IsCmdEqualCmd( cmd, WDB_CMD_QUIT, WDB_SHORT_CMD_QUIT ) || IsCmdEqualCmd( cmd, WDB_CMD_EXIT, WDB_SHORT_CMD_EXIT ) );
}

bool IsCmdPrint( const char *cmd )
{
    return( IsCmdEqualCmd( cmd, WDB_CMD_PRINT, WDB_SHORT_CMD_PRINT ) );
}

bool ProcessCmdPrint( const char *param )
{
    if( IsCmdEqualCmd2( param, WDB_HELP_PARAM ) ) {
        ShowDebuggerMsg( WDB_HELP_PRINT );
        return( TRUE );
    } 
    if( !IsDbgProgramLoaded() ) {
        ShowDebuggerError( "program not loaded." );
        return( FALSE );
    }
    if( param[0] == '\0' ) {
        ShowDebuggerError( "no variable/expression specified." );
        return( FALSE );
    }
    InspectDebuggerVar( param );
    return( TRUE );
}

bool IsCmdRun( const char *cmd )
{
    return( IsCmdEqualCmd( cmd, WDB_CMD_RUN, WDB_SHORT_CMD_RUN ) );
}

bool ProcessCmdRun( const char *param )
{
    if( IsCmdEqualCmd2( param, WDB_HELP_PARAM ) ) {
        ShowDebuggerMsg( WDB_HELP_RUN );
        return( TRUE );
    } 
    if( !IsDbgProgramLoaded() ) {
        ShowDebuggerError( "program not loaded." );
        return( FALSE );
    }
    ProcGo();
    return( TRUE );
}

bool IsCmdContinue( const char *cmd )
{
    return( IsCmdEqualCmd( cmd, WDB_CMD_CONTINUE, WDB_SHORT_CMD_CONTINUE ) );
}

bool ProcessCmdContinue( const char *param )
{
    if( IsCmdEqualCmd2( param, WDB_HELP_PARAM ) ) {
        ShowDebuggerMsg( WDB_HELP_CONTINUE );
        return( TRUE );
    }
    if( !IsDbgProgramLoaded() ) {
        ShowDebuggerError( "program not loaded." );
        return( FALSE );
    }
    ProcGo();
    return( TRUE );
}

bool IsCmdLoad( const char *cmd )
{
    return( IsCmdEqualCmd( cmd, WDB_CMD_LOAD, WDB_SHORT_CMD_LOAD ) || IsCmdEqualCmd( cmd, WDB_CMD_FILE, WDB_SHORT_CMD_FILE ) );
}

bool ProcessCmdLoad( const char *param )
{
    char    *program_name = GetFirstQuotedPart( param );
    char    *program_param = GetSecondQuotedPart( param );
    bool    do_return = FALSE;

    if( IsCmdEqualCmd2( param, WDB_HELP_PARAM ) ) {
        ShowDebuggerMsg( WDB_HELP_LOAD );
        return( TRUE );
    }
#ifdef _WDB_DEBUG_
    printf( "\nProgram : %s", program_name );
    printf( "\nParam : %s\n", program_param );
#endif
    if( program_name[0] == '\0' ) {
        ShowDebuggerError( "no program not load." );
        do_return = FALSE ;
    }
    LoadNewProg( program_name, program_param );
    return( TRUE );
}

bool IsCmdRestart( const char *cmd )
{
    return( IsCmdEqualCmd( cmd, WDB_CMD_RESTART, WDB_SHORT_CMD_RESTART ) );
}

bool ProcessCmdRestart( const char *param )
{
    ReStart();
    return( TRUE );
}

bool IsCmdBreakpoint( const char *cmd )
{
    return( IsCmdEqualCmd( cmd, WDB_CMD_BREAKPOINT, WDB_SHORT_CMD_BREAKPOINT ) );
}

bool ProcessCmdBreakpoint( const char *param )
{
    char    *file_name;
    char    *line_number_str;
    char    *only_file_name;
    int     line_num = 0;

    if( IsCmdEqualCmd2( param, WDB_HELP_PARAM ) ) {
        ShowDebuggerMsg(WDB_HELP_BREAKPOINT);
        return( TRUE );
    }
    if( !IsDbgProgramLoaded() ) {
        ShowDebuggerError( "program not loaded." );
        return( FALSE );
    }
    /*
      parse the cmd
        breakpoint "File Name":line_number
        breakpoint file:line_number
    */
    if( param[0] == '"' ) {
        file_name = GetFirstQuotedPart( param );
    } else {
        file_name = GetCmdPartByChar( param, ":" );
    }

    line_number_str = GetParamPartByChar( param, ":" );
#ifdef _WDB_DEBUG_
    printf( "\nfile_name : %s", file_name );
    printf( "\nline_number : %s\n", line_number_str );
#endif

    /* if the filename or the missing then throw an error */
    if( file_name[0] == '\0' ) {
        ShowDebuggerError( "file name not provided." );
        free( file_name );
        free( line_number_str );
        return( FALSE );
    }
    if( line_number_str[0] == '\0' ) {
        ShowDebuggerError( "line number not provided." );
        free( file_name );
        free( line_number_str );
        return( FALSE );
    }
    line_num = atoi( line_number_str );
    if( line_num == 0 ) {
        ShowDebuggerError( "line number is invalid." );
        free( file_name );
        free( line_number_str );
        return( FALSE );
    }
    only_file_name = GetCmdPartByChar( file_name, "." );
    SetBreakPointInFile( only_file_name, line_num );
    free( file_name );
    free( line_number_str );
    free( only_file_name );
    return( TRUE );
}

bool IsCmdDisable( const char *cmd )
{
    return( IsCmdEqualCmd( cmd, WDB_CMD_DISABLE, WDB_SHORT_CMD_DISABLE ) );
}

bool ProcessCmdDisable( const char *param )
{
    char    *disable_cmd = NULL;
    char    *disable_param = NULL;

    if( IsCmdEqualCmd2( param, WDB_HELP_PARAM ) ) {
        ShowDebuggerMsg( WDB_HELP_DISABLE );
        return( TRUE );
    }

    if( !IsDbgProgramLoaded() ) {
        ShowDebuggerError( "program not loaded." );
        return( FALSE );
    }
    disable_cmd = GetCmdPartByChar( param, " " );
    if( IsCmdEqualCmd2( disable_cmd, "breakpoints" ) ) {
        disable_param = GetParamPartByChar( param, " " );
        if( IsCmdEqualCmd2( disable_param, "" ) ) {
            BrkDisableAll();
            ShowDebuggerMsg( "All break points are disabled." );
        } else {
            DisableBps( disable_param );
        }
    } else {
        ShowDebuggerError( "no subcommand specified." );
    }
    free( disable_cmd );
    if( disable_param != NULL ){
        free( disable_param );
    }
    return( TRUE );
}

bool IsCmdEnable( const char *cmd )
{
    return( IsCmdEqualCmd( cmd, WDB_CMD_ENABLE, WDB_SHORT_CMD_ENABLE ) );
}

bool ProcessCmdEnable( const char *param )
{
    char    *enable_cmd;
    char    *enable_param;

    if( IsCmdEqualCmd2( param, WDB_HELP_PARAM ) ) {
        ShowDebuggerMsg( WDB_HELP_ENABLE );
        return( TRUE );
    } 
    if( !IsDbgProgramLoaded() ) {
        ShowDebuggerError( "program not loaded." );
        return( FALSE );
    }
    enable_cmd = GetCmdPartByChar( param, " " );
    if( IsCmdEqualCmd2( enable_cmd, "breakpoints" ) ) {
        enable_param = GetParamPartByChar( param, " " );
        if( IsCmdEqualCmd2( enable_param, "" ) ) {
            BrkEnableAll();
            ShowDebuggerMsg( "all break points are enabled." );
        } else {
            EnableBps( enable_param );
        }
    } else {
        ShowDebuggerError( "no subcommand specified." );
    }
    free( enable_cmd );
    if( enable_param != NULL ) {
        free( enable_param );
    }
    return( TRUE );
}

bool IsCmdDelete( const char *cmd )
{
    return( IsCmdEqualCmd( cmd, WDB_CMD_DELETE, WDB_SHORT_CMD_DELETE ) );
}

bool ProcessCmdDelete( const char *param )
{
    char    *delete_cmd;
    char    *delete_param;

    if( IsCmdEqualCmd2( param, WDB_HELP_PARAM ) ) {
        ShowDebuggerMsg( WDB_HELP_DELETE );
        return( TRUE );
    }
    if( !IsDbgProgramLoaded() ) {
        ShowDebuggerError( "program not loaded." );
        return( FALSE );
    }
    delete_cmd = GetCmdPartByChar( param, " " );
    if( IsCmdEqualCmd2( delete_cmd, "breakpoints" ) ) {
        delete_param = GetParamPartByChar( param, " " );
        if( IsCmdEqualCmd2( delete_param, "" ) ) {
            BrkClearAll();
            ShowDebuggerMsg( "removed all break points." );
        } else {
            DeleteBps( delete_param );
        }
    } else {
        ShowDebuggerError( "no subcommand specified." );
    }
    free( delete_cmd );
    if( delete_param != NULL ) {
        free( delete_param );
    }
    return( TRUE );
}

bool IsCmdInfo( const char *cmd )
{
    return( IsCmdEqualCmd( cmd, WDB_CMD_INFO, WDB_SHORT_CMD_INFO ) );
}
bool ProcessCmdInfo( const char *param )
{
    char    *info_cmd;

    if( IsCmdEqualCmd2( param, WDB_HELP_PARAM ) ) {
        ShowDebuggerMsg( WDB_HELP_INFO );
        return( TRUE );
    }
    if( !IsDbgProgramLoaded() ) {
        ShowDebuggerError( "program not loaded." );
        return( FALSE );
    }
    info_cmd = GetCmdPartByChar( param, " " );
    if( IsCmdEqualCmd2( info_cmd, "break" ) ) {
        ShowBPs();
    } else if( IsCmdEqualCmd2( info_cmd, "locals" ) ) {
        DumpLocals();
    } else {
        ShowDebuggerError( "no subcommand specified." );
    }
    free( info_cmd );
    return( TRUE );
}

bool IsCmdNext( const char *cmd )
{
    return( IsCmdEqualCmd( cmd, WDB_CMD_NEXT, WDB_SHORT_CMD_NEXT ) );
}

bool ProcessCmdNext( const char *param )
{
    if( IsCmdEqualCmd2( param, WDB_HELP_PARAM ) ) {
        ShowDebuggerMsg( WDB_HELP_NEXT );
        return( TRUE );
    }
    if( !IsDbgProgramLoaded() ) {
        ShowDebuggerError( "program not loaded." );
        return( FALSE );
    }
    PerformDebuggerTrace( TRACE_OVER );
    return( TRUE );
}

bool IsCmdStep( const char *cmd )
{
    return( IsCmdEqualCmd( cmd, WDB_CMD_STEP, WDB_SHORT_CMD_STEP ) );
}

bool ProcessCmdStep( const char *param )
{
    if( IsCmdEqualCmd2( param, WDB_HELP_PARAM ) ) {
        ShowDebuggerMsg( WDB_HELP_STEP );
        return( TRUE );
    }
    if( !IsDbgProgramLoaded() ) {
        ShowDebuggerError( "program not loaded." );
        return( FALSE );
    }
    PerformDebuggerTrace( TRACE_INTO );
    return( TRUE );
}

bool IsCmdFinish( const char *cmd )
{
    return( IsCmdEqualCmd( cmd, WDB_CMD_FINISH, WDB_SHORT_CMD_FINISH ) );
}

bool ProcessCmdFinish( const char *param )
{
    if( IsCmdEqualCmd2( param, WDB_HELP_PARAM ) ) {
        ShowDebuggerMsg( WDB_HELP_FINISH );
        return( TRUE );
    }
    if( !IsDbgProgramLoaded() ) {
        ShowDebuggerError( "program not loaded." );
        return( FALSE );
    }
    PerformDebuggerFinish();
    return( TRUE );
}

bool IsCmdBacktrace( const char *cmd )
{
    return( IsCmdEqualCmd( cmd, WDB_CMD_BACKTRACE, WDB_SHORT_CMD_BACKTRACE ) );
}

bool ProcessCmdBacktrace( const char *param )
{
    if( IsCmdEqualCmd2( param, WDB_HELP_PARAM ) ) {
        ShowDebuggerMsg( WDB_HELP_BACKTRACE );
        return( TRUE );
    }
    if( !IsDbgProgramLoaded() ) {
        ShowDebuggerError( "program not loaded." );
        return( FALSE );
    }
    if( IsCmdEqualCmd2( "full", param ) ) {
        DumpLocals();
    } else {
        ShowCalls();
    }
    return( TRUE );
}

bool IsCmdModule( const char *cmd )
{
    return( IsCmdEqualCmd( cmd, WDB_CMD_MODULE, WDB_SHORT_CMD_MODULE ) );
}

bool ProcessCmdModule( const char *param )
{
    if( IsCmdEqualCmd2( param, WDB_HELP_PARAM ) ) {
        ShowDebuggerMsg( WDB_HELP_MODULE );
        return( TRUE );
    }
    if( !IsDbgProgramLoaded() ) {
        ShowDebuggerError( "program not loaded." );
        return( FALSE );
    }
    ShowModuleList();
    return( TRUE );
}

bool IsCmdAttach( const char *cmd )
{
    return( IsCmdEqualCmd( cmd, WDB_CMD_ATTACH, WDB_SHORT_CMD_ATTACH ));
}

bool ProcessCmdAttach( const char *param )
{
    if( IsCmdEqualCmd2( param, WDB_HELP_PARAM ) ) {
        ShowDebuggerMsg( WDB_HELP_ATTACH );
        return( TRUE );
    }
    //Fixme:
    ShowDebuggerError( "Command Not Implemented." );
    return( TRUE );
}

bool IsCmdKill( const char *cmd )
{
    return( IsCmdEqualCmd( cmd, WDB_CMD_KILL, WDB_SHORT_CMD_KILL ) );
}

bool ProcessCmdKill( const char *param )
{
    if( IsCmdEqualCmd2( param, WDB_HELP_PARAM ) ) {
        ShowDebuggerMsg( WDB_HELP_KILL );
        return( TRUE );
    }
    //fixme: Not working
    if( !IsDbgProgramLoaded() ) {
        ShowDebuggerError( "program not loaded." );
        return( FALSE );
    }
    TraceKill();
    return( TRUE );
}

bool IsCmdDirectory( const char *cmd )
{
    return( IsCmdEqualCmd( cmd, WDB_CMD_DIRECTORY, WDB_SHORT_CMD_DIRECTORY ) );
}

bool ProcessCmdDirectory( const char *param )
{
    if( IsCmdEqualCmd2( param, WDB_HELP_PARAM ) ) {
        ShowDebuggerMsg( WDB_HELP_DIRECTORY );
        return( TRUE );
    }
    if( IsCmdEqualCmd2( param, "" ) ) {
        RemoveSourcePathsFromDebugger();
        ShowDebuggerMsg( "removed all path from debugger" );
    } else {
        AddSourcePathsToDebugger( param );
    }
    return( TRUE );
}

bool IsCmdShow( const char *cmd )
{
    return( IsCmdEqualCmd( cmd, WDB_CMD_SHOW, WDB_SHORT_CMD_SHOW ) );
}

bool ProcessCmdShow( const char *param )
{
    if( IsCmdEqualCmd2( param, WDB_HELP_PARAM ) ) {
        ShowDebuggerMsg(WDB_HELP_SHOW);
        return( TRUE );
    }
    if( IsCmdEqualCmd2( param, "directories" ) ) {
        ShowSourceDirectories();
    }
    return( TRUE );
}

bool IsCmdHelp( const char *cmd )
{
    return( IsCmdEqualCmd( cmd, WDB_CMD_HELP, WDB_SHORT_CMD_HELP ) );
}

bool ProcessCmdHelp( char *param )
{
    printf( WDB_HELP_QUIT );
    printf( WDB_HELP_EXIT );
    printf( WDB_HELP_PRINT );
    printf( WDB_HELP_RUN );
    printf( WDB_HELP_CONTINUE );
    printf( WDB_HELP_LOAD );
    printf( WDB_HELP_RESTART );
    printf( WDB_HELP_BREAKPOINT );
    printf( WDB_HELP_ENABLE );
    printf( WDB_HELP_DISABLE );
    printf( WDB_HELP_DELETE );
    printf( WDB_HELP_INFO );
    printf( WDB_HELP_NEXT );
    printf( WDB_HELP_STEP );
    printf( WDB_HELP_BACKTRACE );
    printf( WDB_HELP_MODULE );
    printf( WDB_HELP_ATTACH );
    printf( WDB_HELP_KILL );
    printf( WDB_HELP_SHOW );
    printf( WDB_HELP_DIRECTORY );
    printf( WDB_HELP_FINISH );
    printf( WDB_HELP_HELP );
    return( TRUE );
}

bool IsCmdTest( const char *cmd )
{
    return( IsCmdEqualCmd( cmd, WDB_CMD_TEST, WDB_SHORT_CMD_TEST ) );
}

bool ProcessCmdTest( const char *param )
{
    WndAsmInspect( GetCodeDot() );
    return( TRUE );
}


bool ProcessDebuggerCmd( char *cmd )
{
    char    *cmd_part;
    char    *param_part;
    bool    result = FALSE;

    /*if the user hits enter*/
    if( ( cmd == NULL ) || ( cmd[0] == '\0' ) )
        return( TRUE );

    cmd = MyStrTrim( cmd );
    cmd_part = GetCmdPart( cmd );
    param_part = GetParamPart( cmd );

    if( IsCmdQuit( cmd_part ) ) {
        ShowDebuggerMsg( "Quitting." );
        return( FALSE );
    } else if( IsCmdPrint( cmd_part ) ) {
        ProcessCmdPrint( param_part );
        result = TRUE;
    } else if( IsCmdRun( cmd_part ) ) {
        ProcessCmdRun( param_part );
        result = TRUE;
    } else if( IsCmdContinue( cmd_part ) ) {
        ProcessCmdContinue( param_part );
        result = TRUE;
    } else if( IsCmdLoad( cmd_part ) ) {
        ProcessCmdLoad( param_part );
        result = TRUE;
    } else if( IsCmdRestart( cmd_part ) ) {
        ProcessCmdRestart( param_part );
        result = TRUE;
    } else if( IsCmdBreakpoint( cmd_part ) ) {
        ProcessCmdBreakpoint( param_part );
        result = TRUE;
    } else if( IsCmdEnable( cmd_part ) ) {
        ProcessCmdEnable( param_part );
        result = TRUE;
    } else if( IsCmdDisable( cmd_part ) ) {
        ProcessCmdDisable( param_part );
        result = TRUE;
    } else if( IsCmdDelete( cmd_part ) ) {
        ProcessCmdDelete( param_part );
        result = TRUE;
    } else if( IsCmdInfo( cmd_part ) ) {
        ProcessCmdInfo( param_part );
        result = TRUE;
    } else if( IsCmdNext( cmd_part ) ) {
        ProcessCmdNext( param_part );
        result = TRUE;
    } else if( IsCmdStep( cmd_part ) ) {
        ProcessCmdStep( param_part );
        result = TRUE;
    } else if( IsCmdFinish( cmd_part ) ) {
        ProcessCmdFinish( param_part );
        result = TRUE;
    } else if( IsCmdBacktrace( cmd_part ) ) {
        ProcessCmdBacktrace( param_part );
        result = TRUE;
    } else if( IsCmdAttach( cmd_part ) ) {
        ProcessCmdAttach( param_part );
        result = TRUE;
    } else if( IsCmdKill( cmd_part ) ) {
        ProcessCmdKill( param_part );
        result = TRUE;
    } else if( IsCmdShow( cmd_part ) ) {
        ProcessCmdShow( param_part );
        result = TRUE;
    } else if( IsCmdDirectory( cmd_part ) ) {
        ProcessCmdDirectory( param_part );
        result = TRUE;
    } else if( IsCmdModule( cmd_part ) ) {
        ProcessCmdModule( param_part );
        result = TRUE;
    } else if( IsCmdTest( cmd_part ) ) {
        ProcessCmdTest( param_part );
        result = TRUE;
    } else if( IsCmdHelp( cmd_part ) ) {
        ProcessCmdHelp( param_part );
        result = TRUE;
    }

    if( !result ) {
        ShowDebuggerError( cmd );
        ShowDebuggerError( "Unknown Command." );
    }

    free( cmd_part );
    free( param_part );

    return( TRUE );
}

void WndMemInit( void )
{
}

unsigned DUIConfigScreen( void )
{
    return( 0 );
}

bool DUIClose( void )
{
    Done = TRUE;
    return( TRUE );
}

HANDLE          Requestsem;
HANDLE          Requestdonesem;

bool RequestDone;

DWORD WINAPI ControlFunc( LPVOID parm )
{
    parm = parm;
    do {
        WaitForSingleObject( Requestsem, INFINITE ); // wait for Request
        switch( Req ) {
        case REQ_GO:
            Go( TRUE );
            break;
        case REQ_TRACE_OVER:
            ExecTrace( TRACE_OVER, DbgLevel );
            break;
        case REQ_TRACE_INTO:
            ExecTrace( TRACE_INTO, DbgLevel );
            break;
        }
        DoInput();
        _SwitchOff( SW_TASK_RUNNING );
        ReleaseSemaphore( Requestdonesem, 1, NULL ); // signal req done
    } while( Req != REQ_BYE );
    return( 0 ); // thread over!
}

void RunRequest( int req )
{
    if( _IsOn( SW_TASK_RUNNING ) ) return;
    WaitForSingleObject( Requestdonesem, INFINITE ); // wait for last request to finish
    Req = req;
    _SwitchOn( SW_TASK_RUNNING );
    ReleaseSemaphore( Requestsem, 1, NULL ); // tell worker to go
}

void DlgCmd( void )
{
    char        buff[512];

    do {
        ShowDebuggerPrompt();
        gets( buff );
        MyStrTrim( buff );
        buff[strlen( buff )] = '\0';
        if( !ProcessDebuggerCmd( buff ) ) {
            break;
        }
    } while (1);
}

extern char *DUILoadString( int id )
{
    char        buff[256];
    char        *ret;
    int         size;

    size = LoadString( GetModuleHandle( NULL ), id, buff, sizeof( buff ) );
    buff[size]='\0';
    ret = DbgAlloc( size+1 );
    strcpy( ret, buff );
    return( ret );
}

void DUIMsgBox( const char *text )
{
    printf( "MSG %s\n", text );
}

bool DUIDlgTxt( const char *text )
{
    printf( "DLG %s\n", text );
    return( TRUE );
}

void DUIInfoBox( const char *text )
{
    printf( "INF %s\n", text );
}

void DUIErrorBox( const char *text )
{
    printf( "ERR %s\n", text );
}

void DUIStatusText( const char *text )
{
    printf( "STA %s\n", text );
}

bool DlgGivenAddr( const char *title, address *value )
{
    // needed when segment's don't map (from new/sym command)
    return( FALSE );
}
bool DlgNewWithSym( const char *title, char *buff, int buff_len )
{
    // used by print command with no arguments
    return( true );
}
bool DlgUpTheStack( void )
{
    // used when trying to trace, but we've unwound the stack a bit
    return( FALSE );
}
bool DlgAreYouNuts( unsigned long mult )
{
    // used when too many break on write points are set
    return( FALSE );
}
bool DlgBackInTime( bool warn )
{
    // used when trying to trace, but we've backed up over a call or asynch
    warn = warn;
    return( FALSE );
}
bool DlgIncompleteUndo( void )
{
    // used when trying to trace, but we've backed up over a call or asynch
    return( FALSE );
}
bool DlgBreak( address addr )
{
    // used when an error occurs in the break point expression or it is entered wrong
    return( FALSE );
}

bool DUIInfoRelease( void )
{
    // used when we're low on memory
    return( FALSE );
}
void DUIUpdate( update_list flags )
{
    // flags indicates what conditions have changed.  They should be saved
    // until an appropriate time, then windows updated accordingly
}
void DUIStop( void )
{
    // close down the UI - we're about to change modes.
}
void DUIFini( void )
{
    // finish up the UI
}
void DUIInit( void )
{
    // Init the UI
}
extern void DUIFreshAll( void )
{
    // refresh all screens - initialization has been done
//    UpdateFlags = 0;
}
extern bool DUIStopRefresh( bool stop )
{
    // temporarily turn off/on screen refreshing, cause we're going to run a
    // big command file and we don't want flashing.
    return( FALSE );
}
extern void DUIShow( void )
{
    // show the main screen - the splash page has been closed
}
extern void DUIWndUser( void )
{
    // switch to the program screen
}
extern void DUIWndDebug( void )
{
    // switch to the debugger screen
}
extern void DUIShowLogWindow( void )
{
    // bring up the log window, cause some printout is coming
}
extern int DUIGetMonitorType( void )
{
    // stub for old UI
    return( 1 );
}
extern int DUIScreenSizeY( void )
{
    // stub for old UI
    return( 0 );
}
extern int DUIScreenSizeX( void )
{
    // stub for old UI
    return( 0 );
}
extern void DUIArrowCursor( void )
{
    // we're about to suicide, so restore the cursor to normal
}
bool DUIAskIfAsynchOk( void )
{
    // we're about to try to replay across an asynchronous event.  Ask user
    return( FALSE );
}
extern void DUIFlushKeys( void )
{
    // we're about to suicide - clear the keyboard typeahead
}
extern void DUIPlayDead( bool dead )
{
    // the app is about to run - make the debugger play dead
}
extern void DUISysEnd( bool pause )
{
    // done calling system();
}
extern void DUISysStart( void )
{
    // about to call system();
}
extern void DUIRingBell( void )
{
    // ring ring (error)
}
extern int DUIDisambiguate( const ambig_info *ambig, int count )
{
    // the expression processor detected an ambiguous symbol.  Ask user which one
    return( 0 );
}
extern void *DUIHourGlass( void *x )
{
    return( x );
}
void ProcAccel( void )
{
    // stub for old UI
    FlushEOC();
}
void ProcDisplay( void )
{
    // stub for old UI
    FlushEOC();
}
void ProcFont( void )
{
    // stub for old UI
    FlushEOC();
}
void ProcHelp( void )
{
    // stub for old UI
    FlushEOC();
}
void ProcInternal( void )
{
    // stub for old UI
    FlushEOC();
}
void ProcPaint( void )
{
    // stub for old UI
    FlushEOC();
}
void ProcView( void )
{
    // stub for old UI
    FlushEOC();
}
void DUIProcWindow( void )
{
    // stub for old UI
    FlushEOC();
}
void ProcConfigFile( void )
{
    // called when main config file processed
    FlushEOC();
}
void ConfigDisp( void )
{
    // stub for old UI
}
void ConfigFont( void )
{
    // stub for old UI
}
void ConfigPaint( void )
{
    // stub for old UI
}
extern void DClickSet( void )
{
    // stub for old UI
    FlushEOC();
}
extern void DClickConf( void )
{
    // stub for old UI
}
extern void InputSet( void )
{
    // stub for old UI
    FlushEOC();
}
extern void InputConf( void )
{
    // stub for old UI
}
extern void MacroSet( void )
{
    // stub for old UI
    FlushEOC();
}
extern void MacroConf( void )
{
    // stub for old UI
}
extern void FiniMacros( void )
{
    // stub for old UI
}
int TabIntervalGet( void )
{
    // stub for old UI
    return( 0 );
}
void TabIntervalSet( int new )
{
    // stub for old UI
}
extern void TabSet( void )
{
    // stub for old UI
    FlushEOC();
}
extern void TabConf( void )
{
    // stub for old UI
}
extern void SearchSet( void )
{
    // stub for old UI
    FlushEOC();
}
extern void SearchConf( void )
{
    // stub for old UI
}
extern void FingClose( void )
{
    // open a splash page
}
extern void FingOpen( void )
{
    // close the splash page
}
extern void AsmChangeOptions( void )
{
    // assembly window options changed
}
extern void RegChangeOptions( void )
{
    // reg window options changed
}
extern void VarChangeOptions( void )
{
    // var window options changed
}
extern void FuncChangeOptions( void )
{
    // func window options changed
}
extern void GlobChangeOptions( void )
{
    // glob window options changed
}
extern void ModChangeOptions( void )
{
    // mod window options changed
}
extern void WndVarInspect( const char *buff )
{
}
extern void *WndAsmInspect( address addr )
{
    // used by examine/assembly command
    int                 i;
    char                buff[256];
    mad_disasm_data     *dd;

    _AllocA( dd, MADDisasmDataSize() );
    for( i = 0; i < 10; ++i ) {
        MADDisasm( dd, &addr, 0 );
        MADDisasmFormat( dd, MDP_ALL, CurrRadix, buff, sizeof( buff ) );
        InsMemRef( dd );
        printf( "%-40s%s\n", buff, TxtBuff );
    }
    return( NULL );
}
extern void *WndSrcInspect( address addr )
{
    // used by examine/source command
    return( NULL );
}
extern void WndMemInspect( address addr, char *next, unsigned len,
                           mad_type_handle type )
{
    // used by examine/byte/word/etc command
}
extern void WndIOInspect( address *addr, mad_type_handle type )
{
    // used by examine/iobyte/ioword/etc command
}
extern void WndTmpFileInspect( const char *file )
{
    // used by capture command
    file = file;
}
extern void GraphicDisplay( void )
{
    // used by print/window command
}
extern void VarUnMapScopes( image_entry *img )
{
    // unmap variable scopes - prog about to restart
    img = img;
}
extern void VarReMapScopes( image_entry *img )
{
    // remap variable scopes - prog about to restart
    img = img;
}
extern void VarFreeScopes( void )
{
    // free variable scope info
}
extern void SetLastExe( const char *name )
{
    // remember last exe debugged name
    name = name;
}
extern void DUIProcPendingPaint( void )
{
    // a paint command was issued - update the screen (stub)
}
void VarSaveWndToScope( void *wnd )
{
}
void VarRestoreWndFromScope( void *wnd )
{
}

void DUIEnterCriticalSection( void )
{
}

void DUIExitCriticalSection( void )
{
}

void DUIInitLiterals( void )
{
}

void DUIFiniLiterals( void )
{
}

bool DUIGetSourceLine( cue_handle *ch, char *buff, unsigned len )
{
    void        *viewhndl;

    viewhndl = OpenSrcFile( ch );
    if( viewhndl == NULL ) return( FALSE );
    buff[ FReadLine( viewhndl, CueLine( ch ), 0, buff, len )] = '\0';
    FDoneSource( viewhndl );
    return( TRUE );
}

bool DUIIsDBCS( void )
{
    return( FALSE );
}

unsigned DUIEnvLkup( const char *name, char *buff, unsigned buff_len )
{
    return( EnvLkup( name, buff, buff_len ) );
}

void DUIDirty( void )
{
}


void StartupErr( const char *err )
/********************************/
{
    printf( "Fatal error: %s", err );
    exit( 1 );
}

void DUISrcOrAsmInspect( address addr )
{
}

void DUIAddrInspect( address addr )
{
}

extern void DUIRemoveBreak( brkp *bp )
/************************************/
{
    RemovePoint( bp );
}

extern void SetMADMenuItems( void )
/**********************************/
{
}

extern void FPUChangeOptions( void )
/**********************************/
{
}

extern void MMXChangeOptions( void )
/**********************************/
{
}

extern void XMMChangeOptions( void )
/**********************************/
{
}

bool DUIImageLoaded( image_entry *image, bool load,
                     bool already_stopping, bool *force_stop )
/************************************************************/
{
    char buff[256];

    already_stopping=already_stopping;
    force_stop= force_stop;
    if( load ) {
        sprintf( buff, "%s '%s'", LIT_ENG( DLL_Loaded ), image->image_name );
    } else {
        sprintf( buff, "%s '%s'", LIT_ENG( DLL_UnLoaded ), image->image_name );
    }
    DUIDlgTxt( buff );
    return( FALSE );
}

void DUICopySize( void *cookie, unsigned long size )
/**************************************************/
{
    size = size;
    cookie = cookie;
}

void DUICopyCopied( void *cookie, unsigned long size )
/****************************************************/
{
    size = size;
    cookie = cookie;
}

bool DUICopyCancelled( void * cookie )
/************************************/
{
    cookie = cookie;
    return( FALSE );
}

unsigned DUIDlgAsyncRun( void )
/*****************************/
{
    return( 0 );
}

void DUISetNumLines( int num )
{
    num = num;
}

void DUISetNumColumns( int num )
{
    num = num;
}

void DUIInitRunThreadInfo( void )
{
}

void DUIScreenOptInit( void )
{
}

bool DUIScreenOption( const char *start, unsigned len, int pass )
{
    start=start;len=len;pass=pass;
    return( true );
}

#if defined( __NT__ )
const char *CheckForPowerBuilder( const char *name )
{
    return( name );
}
#endif

int main( int argc, char **argv )
{
    char        buff[256];
    DWORD       tid;
    HANDLE      hThread;

    MemInit();
    SetErrorMode( SEM_FAILCRITICALERRORS );
    getcmd( buff );
    CmdData = buff;
    DebugMain();
    _SwitchOff( SW_ERROR_STARTUP );
    Requestsem = CreateSemaphore( NULL, 0, 1, NULL );
    Requestdonesem = CreateSemaphore( NULL, 0, 1, NULL );
    ReleaseSemaphore( Requestdonesem, 1, NULL ); // signal req done
    hThread = CreateThread( NULL, 0, ControlFunc, NULL, 0, &tid );
    if( hThread == NULL) {
        //MessageBox( NULL, "Error creating thread!", "Stubugger", MB_APPLMODAL+MB_OK );
        ShowDebuggerError( "Error creating thread!" );
    }

    DlgCmd();

    CloseHandle( Requestsem );
    CloseHandle( Requestdonesem );
    DebugFini();
    RunRequest( REQ_BYE );
    MemFini();
    return( 0 );
}

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
* Description:  WASM top level module + command line parser
*
****************************************************************************/


#include "asmglob.h"
#include <ctype.h>
#ifdef __WATCOMC__
    #include <process.h>
#endif
#include "wio.h"
#include "asmalloc.h"
#include "fatal.h"
#include "asmexpnd.h"
#include "objprs.h"
#include "genmsomf.h"
#include "directiv.h"
#include "womputil.h"
#include "swchar.h"
#include "asminput.h"
#include "pathgrp.h"
#include "banner.h"

#ifdef __OSI__
    #include "ostype.h"
#endif

extern void             Fatal( unsigned msg, ... );
extern void             ObjRecInit( void );
extern void             DelErrFile( void );
extern void             PrintfUsage( int first_ln );
extern void             MsgPrintf1( int resourceid, char *token );
extern void             AsmBufferInit( void );

extern const char       *FingerMsg[];

File_Info               AsmFiles;       // files information
pobj_state              pobjState;      // object file information for WOMP

struct  option {
    char        *option;
    unsigned    value;
    void        (*function)( void );
};

static struct SWData {
    bool    protect_mode;
    int     cpu;
    int     fpu;
} SWData = {
    FALSE, // real mode CPU instructions set
    0,     // default CPU 8086
    -1     // unspecified FPU
};

#define MAX_NESTING 15
#define BUF_SIZE 512

static char             ParamBuf[ BUF_SIZE ];
static unsigned char    SwitchChar;
static unsigned         OptValue;
static char             *OptScanPtr;
static char             *OptParm;
static char             *ForceInclude = NULL;

global_options Options = {
    FALSE,              // sign_value
    FALSE,              // stop_at_end
    FALSE,              // quiet
    FALSE,              // banner_printed
    FALSE,              // debug_flag
    TRUE,               // output_comment_data_in_code_records

    0,                  // error_count
    0,                  // warning_count
    20,                 // error_limit
    2,                  // warning_level
    FALSE,              // warning_error
    NULL,               // build_target

    NULL,               // code_class
    NULL,               // data_seg
    NULL,               // text_seg
    NULL,               // module_name

#ifdef DEBUG_OUT
    FALSE,              // debug
#endif
    NULL,               // default_name_mangler
    FALSE,              // allow_c_octals
    TRUE,               // emit_dependencies
    TRUE,               // stdcall at number
    TRUE,               // mangle stdcall
    FALSE,              // write listing
    TRUE,               // parameters passed by registers
    MODE_WATCOM,        // assembler mode
    0,                  // locals prefix len
    {'\0','\0','\0'},   // locals prefix
    0                   // trace stack
};

static char *CopyOfParm( void )
/*****************************/
{
    unsigned    len;

    len = OptScanPtr - OptParm;
    memcpy( ParamBuf, OptParm, len );
    ParamBuf[ len ] = '\0';
    return( ParamBuf );
}

static void StripQuotes( char *fname )
/************************************/
{
    char *s;
    char *d;

    if( *fname == '"' ) {
        // string will shrink so we can reduce in place
        d = fname;
        for( s = d + 1; *s && *s != '"'; ++s ) {
            // collapse double backslashes, only then look for escaped quotes
            if( s[0] == '\\' && s[1] == '\\' ) {
                ++s;
            } else if( s[0] == '\\' && s[1] == '"' ) {
                ++s;
            }
            *d++ = *s;
        }
        *d = '\0';
    }
}

static char *GetAFileName( void )
/*******************************/
{
    char *fname;
    fname = CopyOfParm();
    StripQuotes( fname );
    return( fname );
}

static void SetTargName( char *name, unsigned len )
/*************************************************/
{
    char        *p;

    if( Options.build_target != NULL ) {
        AsmFree( Options.build_target );
        Options.build_target = NULL;
    }
    if( name == NULL || len == 0 )
        return;
    Options.build_target = AsmAlloc( len + 1 );
    p = Options.build_target;
    while( len != 0 ) {
        *p++ = toupper( *name++ );
        --len;
    }
    *p++ = '\0';
}

static void SetCPUPMC( void )
/***************************/
{
    char                *tmp;

    for( tmp=OptParm; tmp < OptScanPtr; tmp++ ) {
        if( *tmp == 'p' ) {
            if( SWData.cpu >= 2 ) { // set protected mode
                SWData.protect_mode = TRUE;
            } else {
                MsgPrintf1( MSG_CPU_OPTION_INVALID, CopyOfParm() );
            }
        } else if( *tmp == 'r' ) {
            if( SWData.cpu >= 3 ) { // set register based calling convention
                Options.watcom_parms_passed_by_regs = TRUE;
            } else {
                MsgPrintf1( MSG_CPU_OPTION_INVALID, CopyOfParm() );
            }
        } else if( *tmp == 's' ) {
            if( SWData.cpu >= 3 ) { // set stack based calling convention
                Options.watcom_parms_passed_by_regs = FALSE;
            } else {
                MsgPrintf1( MSG_CPU_OPTION_INVALID, CopyOfParm() );
            }
        } else if( *tmp == '"' ) {      // set default mangler
            char *dest;
            
            tmp++;
            dest = strchr(tmp, '"');
            if( Options.default_name_mangler != NULL ) {
                AsmFree( Options.default_name_mangler );
            }
            Options.default_name_mangler = AsmAlloc( dest - tmp + 1 );
            dest = Options.default_name_mangler;
            for( ; *tmp != '"'; dest++, tmp++ ) {
                *dest = *tmp;
            }
            *dest = NULLC;
        } else {
            MsgPrintf1( MSG_UNKNOWN_OPTION, CopyOfParm() );
            exit( 1 );
        }
    }
    if( SWData.cpu < 3 ) {
        Options.watcom_parms_passed_by_regs = TRUE;
        if( SWData.cpu < 2 ) {
            SWData.protect_mode = FALSE;
        }
    }
}

static void SetCPU( void )
/************************/
{
    SWData.cpu = OptValue;
    SetCPUPMC();
}

static void SetFPU( void )
/************************/
{
    switch( OptValue ) {
    case 'i':
        floating_point = DO_FP_EMULATION;
        break;
    case '7':
        floating_point = NO_FP_EMULATION;
        break;
    case 'c':
        floating_point = NO_FP_ALLOWED;
        break;
    case 0:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
        SWData.fpu = OptValue;
        break;
    }
}

static char memory_model = 0;

static void SetMM( void )
/***********************/
{
    char buffer[20];

    switch( OptValue ) {
    case 'c':
    case 'f':
    case 'h':
    case 'l':
    case 'm':
    case 's':
    case 't':
        break;
    default:
        strcpy( buffer, "/m" );
        strcat( buffer, (char *)&OptValue );
        MsgPrintf1( MSG_UNKNOWN_OPTION, buffer );
        exit( 1 );
    }

    memory_model = OptValue;
}

static void SetMemoryModel( void )
/********************************/
{
    char buffer[20];
    char *model;

    switch( memory_model ) {
    case 'c':
        model = "COMPACT";
        break;
    case 'f':
        model = "FLAT";
        break;
    case 'h':
        model = "HUGE";
        break;
    case 'l':
        model = "LARGE";
        break;
    case 'm':
        model = "MEDIUM";
        break;
    case 's':
        model = "SMALL";
        break;
    case 't':
        model = "TINY";
        break;
    default:
        return;
    }

    if( Options.mode & MODE_IDEAL ) {
        strcpy( buffer, "MODEL " );
    } else {
        strcpy( buffer, ".MODEL " );
    }
    strcat( buffer, model );
    InputQueueLine( buffer );
}

static int isvalidident( char *id )
/*********************************/
{
    char *s;
    char lwr_char;

    if( isdigit( *id ) )
        return( ERROR ); /* can't start with a number */
    for( s = id; *s != '\0'; s++ ) {
        lwr_char = tolower( *s );
        if( !( lwr_char == '_' || lwr_char == '.' || lwr_char == '$'
                || lwr_char == '@' || lwr_char == '?'
                || isdigit( lwr_char )
                || islower( lwr_char ) ) ) {
            return( ERROR );
        }
    }
    return( NOT_ERROR );
}

static void add_constant( char *string )
/**************************************/
{
    char *tmp;
    char *one = "1";

    tmp = strchr( string, '=' );
    if( tmp == NULL ) {
        tmp = strchr( string, '#' );
        if( tmp == NULL ) {
            tmp = one;
        } else {
            *tmp = '\0';
            tmp++;
        }
    } else {
        *tmp = '\0';
        tmp++;
    }

    if( isvalidident( string ) == ERROR ) {
        AsmError( SYNTAX_ERROR ); // fixme
        return;
    }

    StoreConstant( string, tmp, FALSE ); // don't allow it to be redef'd
}

static void AddStringToIncludePath( char *str, const char *end )
/**************************************************************/
{
    char        *src;
    char        *p;
    bool        have_quote;
    char        c;

    have_quote = FALSE;
    src = str;
    while( isspace( *src ) && src < end )
        src++;
    p = str;
    while( (c = *src) != '\0' && src < end ) {
        ++src;
        if( c == '\"' ) {
            have_quote = !have_quote;
        } else {
            *p++ = c;
        }
    }
    AddItemToIncludePath( str, p );
}

static void get_fname( char *token, int type )
/********************************************/
/*
 * figure out the source file name & store it in AsmFiles
 * fill in default object file name if it is null
 */
{
    char        name [ _MAX_PATH  ];
    char        msgbuf[80];
    PGROUP      pg;
    PGROUP      def;

    /* get filename for source file */

    if( type == ASM ) {
        if( token == NULL ) {
            MsgGet( SOURCE_FILE, msgbuf );
            Fatal( MSG_CANNOT_OPEN_FILE, msgbuf );
        }
        if( AsmFiles.fname[ASM] != NULL ) {
            Fatal( MSG_TOO_MANY_FILES );
        }

        _splitpath2( token, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
        if( *pg.ext == '\0' ) {
            pg.ext = ASM_EXT;
        }
        _makepath( name, pg.drive, pg.dir, pg.fname, pg.ext );
        AsmFiles.fname[ASM] = AsmAlloc( strlen( name ) + 1 );
        strcpy( AsmFiles.fname[ASM], name );

        _makepath( name, pg.drive, pg.dir, NULL, NULL );
        /* add the source path to the include path */
        AddItemToIncludePath( name, NULL );

        if( AsmFiles.fname[OBJ] == NULL ) {
            /* set up default object and error filename */
            pg.ext = OBJ_EXT;
            _makepath( name, NULL, NULL, pg.fname, pg.ext );
        } else {
            _splitpath2( AsmFiles.fname[OBJ], def.buffer, &def.drive,
                         &def.dir, &def.fname, &def.ext );
            if( *def.fname == NULLC )
                def.fname = pg.fname;
            if( *def.ext == NULLC )
                def.ext = OBJ_EXT;

            _makepath( name, def.drive, def.dir, def.fname, def.ext );
            AsmFree( AsmFiles.fname[OBJ] );
        }
        AsmFiles.fname[OBJ] = AsmAlloc( strlen( name ) + 1 );
        strcpy( AsmFiles.fname[OBJ], name );

        if( AsmFiles.fname[ERR] == NULL ) {
            pg.ext = ERR_EXT;
            _makepath( name, NULL, NULL, pg.fname, pg.ext );
        } else {
            _splitpath2( AsmFiles.fname[ERR], def.buffer, &def.drive,
                         &def.dir, &def.fname, &def.ext );
            if( *def.fname == NULLC )
                def.fname = pg.fname;
            if( *def.ext == NULLC )
                def.ext = ERR_EXT;
            _makepath( name, def.drive, def.dir, def.fname, def.ext );
            AsmFree( AsmFiles.fname[ERR] );
        }
        AsmFiles.fname[ERR] = AsmAlloc( strlen( name ) + 1 );
        strcpy( AsmFiles.fname[ERR], name );

        if( AsmFiles.fname[LST] == NULL ) {
            pg.ext = LST_EXT;
            _makepath( name, NULL, NULL, pg.fname, pg.ext );
        } else {
            _splitpath2( AsmFiles.fname[LST], def.buffer, &def.drive,
                         &def.dir, &def.fname, &def.ext );
            if( *def.fname == NULLC )
                def.fname = pg.fname;
            if( *def.ext == NULLC )
                def.ext = LST_EXT;
            _makepath( name, def.drive, def.dir, def.fname, def.ext );
            AsmFree( AsmFiles.fname[LST] );
        }
        AsmFiles.fname[LST] = AsmAlloc( strlen( name ) + 1 );
        strcpy( AsmFiles.fname[LST], name );

    } else {
        /* get filename for object, error, or listing file */
        _splitpath2( token, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
        if( AsmFiles.fname[ASM] != NULL ) {
            _splitpath2( AsmFiles.fname[ASM], def.buffer, &def.drive,
                         &def.dir, &def.fname, &def.ext );
            if( *pg.fname == NULLC ) {
                pg.fname = def.fname;
            }
        }
        if( *pg.ext == NULLC ) {
            switch( type ) {
            case ERR:   pg.ext = ERR_EXT;  break;
            case LST:   pg.ext = LST_EXT;  break;
            case OBJ:   pg.ext = OBJ_EXT;  break;
            }
        }
        _makepath( name, pg.drive, pg.dir, pg.fname, pg.ext );
        if( AsmFiles.fname[type] != NULL ) {
            AsmFree( AsmFiles.fname[type] );
        }
        AsmFiles.fname[type] = AsmAlloc( strlen( name ) + 1 );
        strcpy( AsmFiles.fname[type], name );
    }
}

static void set_some_kinda_name( char token, char *name )
/*******************************************************/
/* set:  code class / data seg. / module name / text seg */
{
    int len;
    char **tmp;

    len = strlen( name ) + 1;
    switch( token ) {
    case 'c':
        tmp = &Options.code_class;
        break;
    case 'd':
        tmp = &Options.data_seg;
        break;
    case 'm':
        tmp = &Options.module_name;
        break;
    case 't':
        tmp = &Options.text_seg;
        break;
    default:
        return;
    }
    if( *tmp != NULL ) {
        AsmFree(*tmp);
    }
    *tmp = AsmAlloc( len );
    strcpy( *tmp, name );
}

static void usage_msg( void )
/***************************/
{
    PrintfUsage( MSG_USAGE_BASE );
    exit(1);
}

static void Ignore( void ) {}

static void Set_BT( void ) { SetTargName( OptParm,  OptScanPtr - OptParm ); }

static void Set_C( void ) { Options.output_comment_data_in_code_records = FALSE; }

static void Set_D( void ) { Options.debug_flag = (OptValue != 0) ? TRUE : FALSE; }

static void DefineMacro( void ) { add_constant( CopyOfParm() ); }

static void SetErrorLimit( void ) { Options.error_limit = OptValue; }

static void SetStopEnd( void ) { Options.stop_at_end = TRUE; }

static void Set_FR( void ) { get_fname( GetAFileName(), ERR ); }

static void Set_FI( void ) { ForceInclude = GetAFileName(); }

static void Set_FL( void ) { get_fname( GetAFileName(), LST ); Options.write_listing = TRUE; }

static void Set_FO( void ) { get_fname( GetAFileName(), OBJ ); }

static void SetInclude( void ) { AddStringToIncludePath( OptParm, OptScanPtr ); }

static void Set_S( void ) { Options.sign_value = TRUE; }

static void Set_N( void ) { set_some_kinda_name( OptValue, CopyOfParm() ); }

static void Set_O( void ) { Options.allow_c_octals = TRUE; }

static void Set_OF( void ) { Options.trace_stack = OptValue; }

static void Set_WE( void ) { Options.warning_error = TRUE; }

static void Set_WX( void ) { Options.warning_level = 4; }

static void SetWarningLevel( void ) { Options.warning_level = OptValue; }

static void Set_ZCM( void )
{
    if( OptScanPtr == OptParm || strnicmp( OptParm, "MASM", OptScanPtr - OptParm ) == 0 ) {
        Options.mode = MODE_MASM6;
    } else if( strnicmp( OptParm, "WATCOM", OptScanPtr - OptParm ) == 0 ) {
        Options.mode = MODE_WATCOM;
    } else if( strnicmp( OptParm, "TASM", OptScanPtr - OptParm ) == 0 ) {
        Options.mode = MODE_TASM | MODE_MASM5;
//    } else if( strnicmp( OptParm, "MASM5", OptScanPtr - OptParm ) == 0 ) {
//        Options.mode = MODE_MASM5;
    }
}

static void Set_ZLD( void ) { Options.emit_dependencies = FALSE; }

static void Set_ZQ( void ) { Options.quiet = TRUE; }

static void Set_ZZ( void ) { Options.use_stdcall_at_number = FALSE; }

static void Set_ZZO( void ) { Options.mangle_stdcall = FALSE; }

static void HelpUsage( void ) { usage_msg();}

#ifdef DEBUG_OUT
static void Set_D6( void )
{
    Options.debug = TRUE;
    DebugMsg(( "debugging output on \n" ));
}
#endif

static struct option const cmdl_options[] = {
    { "0$",     0,        SetCPU },
    { "1$",     1,        SetCPU },
    { "2$",     2,        SetCPU },
    { "3$",     3,        SetCPU },
    { "4$",     4,        SetCPU },
    { "5$",     5,        SetCPU },
    { "6$",     6,        SetCPU },
    { "7",      7,        SetFPU },
    { "?",      0,        HelpUsage },
    { "bt=$",   0,        Set_BT },
    { "c",      0,        Set_C },
    { "d0",     0,        Set_D },
    { "d1",     1,        Set_D },
    { "d2",     2,        Set_D },
    { "d3",     3,        Set_D },
#ifdef DEBUG_OUT
    { "d6",     6,        Set_D6 },
#endif
    { "d+",     0,        Ignore },
    { "d$",     0,        DefineMacro },
    { "e",      0,        SetStopEnd },
    { "e=#",    0,        SetErrorLimit },
    { "fe=@",   0,        Set_FR },
    { "fi=@",   0,        Set_FI },
    { "fl=@",   0,        Set_FL },
    { "fo=@",   0,        Set_FO },
    { "fp0",    0,        SetFPU },
    { "fp2",    2,        SetFPU },
    { "fp3",    3,        SetFPU },
    { "fp5",    5,        SetFPU },
    { "fp6",    6,        SetFPU },
    { "fpi87",  '7',      SetFPU },
    { "fpi",    'i',      SetFPU },
    { "fpc",    'c',      SetFPU },
    { "fr=@",   0,        Set_FR },
    { "h",      0,        HelpUsage },
    { "hc",     'c',      Ignore },
    { "hd",     'd',      Ignore },
    { "hw",     'w',      Ignore },
    { "i=@",    0,        SetInclude },
    { "j",      0,        Set_S },
    { "mc",     'c',      SetMM },
    { "mf",     'f',      SetMM },
    { "mh",     'h',      SetMM },
    { "ml",     'l',      SetMM },
    { "mm",     'm',      SetMM },
    { "ms",     's',      SetMM },
    { "mt",     't',      SetMM },
    { "nc=$",   'c',      Set_N },
    { "nd=$",   'd',      Set_N },
    { "nm=$",   'm',      Set_N },
    { "nt=$",   't',      Set_N },
    { "o",      0,        Set_O },
    { "of",     1,        Set_OF },
    { "of+",    2,        Set_OF },
    { "q",      0,        Set_ZQ },
    { "s",      0,        Set_S },
    { "u",      0,        Ignore },
    { "we",     0,        Set_WE },
    { "wx",     0,        Set_WX },
    { "w=#",    0,        SetWarningLevel },
    { "zld",    0,        Set_ZLD },
    { "zcm=$",  0,        Set_ZCM },
    { "zz",     0,        Set_ZZ },
    { "zzo",    0,        Set_ZZO },
    { "zq",     0,        Set_ZQ },
    { 0,        0,        0 }
};

static int OptionDelimiter( char c )
/**********************************/
{
    if( c == ' ' || c == '-' || c == '\0' || c == '\t' || c == SwitchChar ) {
        return( 1 );
    }
    return( 0 );
}

static void get_os_include( void )
/********************************/
{
    char *env;
    char *tmp;

    /* add OS_include to the include path */

    tmp = AsmTmpAlloc( strlen( Options.build_target ) + 10 );
    strcpy( tmp, Options.build_target );
    strcat( tmp, "_INCLUDE" );

    env = getenv( tmp );
    if( env != NULL ) {
        AddItemToIncludePath( env, NULL );
    }
}

int trademark( void )
/*******************/
{
    int         count = 0;

    if( !Options.quiet ) {
        while( FingerMsg[count] != NULL ) {
            printf( "%s\n", FingerMsg[count++] );
        }
    }
    return( count );
}

static void free_names( void )
/****************************/
/* Free names set as cmdline options */
{
    if( Options.build_target != NULL ) {
        AsmFree( Options.build_target );
    }
    if( Options.code_class != NULL ) {
        AsmFree( Options.code_class );
    }
    if( Options.data_seg != NULL ) {
        AsmFree( Options.data_seg );
    }
    if( Options.module_name != NULL ) {
        AsmFree( Options.module_name );
    }
    if( Options.text_seg != NULL ) {
        AsmFree( Options.text_seg );
    }
}

static void main_init( void )
/***************************/
{
    int         i;

    MemInit();
    for( i=ASM; i<=OBJ; i++ ) {
        AsmFiles.file[i] = NULL;
        AsmFiles.fname[i] = NULL;
    }
    ObjRecInit();
    GenMSOmfInit();
}

static void main_fini( void )
/***************************/
{
    free_names();
    GenMSOmfFini();
    AsmShutDown();
}

static void open_files( void )
/****************************/
{
    /* open ASM file */
    AsmFiles.file[ASM] = fopen( AsmFiles.fname[ASM], "r" );

    if( AsmFiles.file[ASM] == NULL ) {
        Fatal( MSG_CANNOT_OPEN_FILE, AsmFiles.fname[ASM] );
    }

    /* open OBJ file */
    pobjState.file_out = ObjWriteOpen( AsmFiles.fname[OBJ] );
    if( pobjState.file_out == NULL ) {
        Fatal( MSG_CANNOT_OPEN_FILE, AsmFiles.fname[OBJ] );
    }
    pobjState.pass = POBJ_WRITE_PASS;

    /* delete any existing ERR file */
    DelErrFile();
}

static char *CollectEnvOrFileName( char *str )
/********************************************/
{
    char        *env;
    char        ch;

    while( *str == ' ' || *str == '\t' )
        ++str;
    env = ParamBuf;
    for( ;; ) {
        ch = *str;
        if( ch == '\0' )
            break;
        ++str;
        if( ch == ' ' )
            break;
        if( ch == '\t' )
            break;
#if !defined(__UNIX__)
        if( ch == '-' )
            break;
        if( ch == SwitchChar )
            break;
#endif
        *env++ = ch;
    }
    *env = '\0';
    return( str );
}

static char *ReadIndirectFile( void )
/***********************************/
{
    char        *env;
    char        *str;
    int         handle;
    int         len;
    char        ch;

    env = NULL;
    handle = open( ParamBuf, O_RDONLY | O_BINARY );
    if( handle != -1 ) {
        len = filelength( handle );
        env = AsmAlloc( len + 1 );
        read( handle, env, len );
        env[len] = '\0';
        close( handle );
        // zip through characters changing \r, \n etc into ' '
        str = env;
        while( *str ) {
            ch = *str;
            if( ch == '\r' || ch == '\n' ) {
                *str = ' ';
            }
#if !defined(__UNIX__)
            if( ch == 0x1A ) {      // if end of file
                *str = '\0';        // - mark end of str
                break;
            }
#endif
            ++str;
        }
    }
    return( env );
}

static char *ProcessOption( char *p, char *option_start )
/*******************************************************/
{
    int         i;
    int         j;
    char        *opt;
    char        c;

    for( i = 0; ; i++ ) {
        opt = cmdl_options[i].option;
        if( opt == NULL )
            break;
        c = tolower( *p );
        if( c == *opt ) {
            OptValue = cmdl_options[i].value;
            j = 1;
            for( ;; ) {
                ++opt;
                if( *opt == '\0' || *opt == '*' ) {
                    if( *opt == '\0' ) {
                        if( p - option_start == 1 ) {
                            // make sure end of option
                            if( !OptionDelimiter( p[j] ) ) {
                                break;
                            }
                        }
                    }
                    OptScanPtr = p + j;
                    cmdl_options[i].function();
                    return( OptScanPtr );
                }
                if( *opt == '#' ) {             // collect a number
                    if( p[j] >= '0' && p[j] <= '9' ) {
                        OptValue = 0;
                        for( ;; ) {
                            c = p[j];
                            if( c < '0' || c > '9' )
                                break;
                            OptValue = OptValue * 10 + c - '0';
                            ++j;
                        }
                    }
                } else if( *opt == '$' ) {      // collect an identifer
                    OptParm = &p[j];
                    for( ;; ) {
                        c = p[j];
                        if( c == '\0' )
                            break;
                        if( c == '-' )
                            break;
                        if( c == ' ' )
                            break;
                        if( c == SwitchChar )
                            break;
                        ++j;
                    }
                } else if( *opt == '@' ) {      // collect a filename
                    OptParm = &p[j];
                    c = p[j];
                    if( c == '"' ) { // "filename"
                        for( ;; ) {
                            c = p[++j];
                            if( c == '"' ) {
                                ++j;
                                break;
                            }
                            if( c == '\0' )
                                break;
                            if( c == '\\' ) {
                                ++j;
                            }
                        }
                    } else {
                        for( ;; ) {
                            c = p[j];
                            if( c == '\0' )
                                break;
                            if( c == ' ' )
                                break;
                            if( c == '\t' )
                                break;
#if !defined(__UNIX__)
                            if( c == SwitchChar )
                                break;
#endif
                            ++j;
                        }
                    }
                } else if( *opt == '=' ) {      // collect an optional '='
                    if( p[j] == '=' || p[j] == '#' ) ++j;
                } else {
                    c = tolower( p[j] );
                    if( *opt != c ) {
                        if( *opt < 'A' || *opt > 'Z' )
                            break;
                        if( *opt != p[j] ) {
                            break;
                        }
                    }
                    ++j;
                }
            }
        }
    }
    MsgPrintf1( MSG_UNKNOWN_OPTION, option_start );
    return( NULL );
}

static int ProcOptions( char *str, int *level )
/*********************************************/
{
    char *save[MAX_NESTING];
    char *buffers[MAX_NESTING];

    for( ; str != NULL; ) {
        while( *str == ' ' || *str == '\t' )
            ++str;
        if( *str == '@' && *level < MAX_NESTING ) {
            save[(*level)++] = CollectEnvOrFileName( str + 1 );
            buffers[*level] = NULL;
            str = getenv( ParamBuf );
            if( str == NULL ) {
                str = ReadIndirectFile();
                buffers[*level] = str;
            }
            if( str != NULL )
                continue;
            str = save[--(*level)];
        }
        if( *str == '\0' ) {
            if( *level == 0 )
                break;
            if( buffers[*level] != NULL ) {
                AsmFree( buffers[*level] );
                buffers[*level] = NULL;
            }
            str = save[--(*level)];
            continue;
        }
        if( *str == '-' || *str == SwitchChar ) {
            str = ProcessOption( str + 1, str );
            if( str == NULL ) {
                exit( 1 );
            }
        } else {  /* collect file name */
            char *beg, *p;
            int len;

            beg = str;
            if( *str == '"' ) {
                for( ;; ) {
                    ++str;
                    if( *str == '"' ) {
                        ++str;
                        break;
                    }
                    if( *str == '\0' )
                        break;
                    if( *str == '\\' ) {
                        ++str;
                    }
                }
            } else {
                for( ;; ) {
                    if( *str == '\0' )
                        break;
                    if( *str == ' ' )
                        break;
                    if( *str == '\t' )
                        break;
#if !defined(__UNIX__)
                    if( *str == SwitchChar )
                        break;
#endif
                    ++str;
                }
            }
            len = str-beg;
            p = AsmAlloc( len + 1 );
            memcpy( p, beg, len );
            p[ len ] = '\0';
            StripQuotes( p );
            get_fname( p, ASM );
            AsmFree(p);
        }
    }
    return( 0 );
}

static void do_envvar_cmdline( char *envvar )
/*******************************************/
{
    char *cmdline;
    int  level = 0;

    cmdline = getenv( envvar );
    if( cmdline != NULL ) {
        ProcOptions( cmdline, &level );
    }
}

static int set_build_target( void )
/*********************************/
{
    char *tmp;
    char *uscores = "__";

    if( Options.build_target == NULL ) {
#if defined(__OSI__)
        if( __OS == OS_DOS ) {
            SetTargName( "DOS", 3 );
        } else if( __OS == OS_OS2 ) {
            SetTargName( "OS2", 3 );
        } else if( __OS == OS_NT ) {
            SetTargName( "NT", 2 );
        } else if( __OS == OS_WIN ) {
            SetTargName( "WINDOWS", 7 );
        } else {
            SetTargName( "XXX", 3 );
        }
#elif defined(__QNX__)
        SetTargName( "QNX", 3 );
#elif defined(__LINUX__)
        SetTargName( "LINUX", 5 );
#elif defined(__BSD__)
        SetTargName( "BSD", 3 );
#elif defined(__OSX__) || defined(__APPLE__)
        SetTargName( "OSX", 3 );
#elif defined(__SOLARIS__) || defined( __sun )
        SetTargName( "SOLARIS", 7 );
#elif defined(__DOS__)
        SetTargName( "DOS", 3 );
#elif defined(__OS2__)
        SetTargName( "OS2", 3 );
#elif defined(__NT__)
        SetTargName( "NT", 2 );
#elif defined(__ZDOS__)
        SetTargName( "ZDOS", 4 );
#else
        #error unknown host OS
#endif
    }

    tmp = AsmTmpAlloc( strlen( Options.build_target ) + 5 ); // null + 4 uscores
    strcpy( tmp, uscores );
    strcat( tmp, Options.build_target );
    strcat( tmp, uscores );

    add_constant( tmp ); // always define something

    if( stricmp( Options.build_target, "DOS" ) == 0 ) {
        add_constant( "__MSDOS__" );
    } else if( stricmp( Options.build_target, "NETWARE" ) == 0 ) {
        if( (Code->info.cpu&P_CPU_MASK) >= P_386 ) {
            add_constant( "__NETWARE_386__" );
        } else {
            /* do nothing ... __NETWARE__ already defined */
        }
    } else if( stricmp( Options.build_target, "WINDOWS" ) == 0 ) {
        if( (Code->info.cpu&P_CPU_MASK) >= P_386 ) {
            add_constant( "__WINDOWS_386__" );
        } else {
            /* do nothing ... __WINDOWS__ already defined */
        }
    } else if( stricmp( Options.build_target, "QNX" ) == 0 ) {
        add_constant( "__UNIX__" );
    } else if( stricmp( Options.build_target, "LINUX" ) == 0 ) {
        add_constant( "__UNIX__" );
    } else if( stricmp( Options.build_target, "BSD" ) == 0 ) {
        add_constant( "__UNIX__" );
    }
    return( NOT_ERROR );
}

static void parse_cmdline( char **cmdline )
/*****************************************/
{
    char msgbuf[80];
    int  level = 0;

    if( cmdline == NULL || *cmdline == NULL || **cmdline == 0 ) {
        usage_msg();
    }
    for( ;*cmdline != NULL; ++cmdline ) {
        ProcOptions( *cmdline, &level );
    }
    if( AsmFiles.fname[ASM] == NULL ) {
        MsgGet( NO_FILENAME_SPECIFIED, msgbuf );
        Fatal( MSG_CANNOT_OPEN_FILE, msgbuf );
    }
}

static void do_init_stuff( char **cmdline )
/*****************************************/
{
    char        *env;
    char        buff[80];

    if( !MsgInit() )
        exit(1);

    AsmBufferInit();
    strcpy( buff, "__WASM__=" BANSTR( _BANVER ) );
    add_constant( buff );
    ForceInclude = getenv( "FORCE" );
    do_envvar_cmdline( "WASM" );
    parse_cmdline( cmdline );
    set_build_target();
    get_os_include();
    env = getenv( "INCLUDE" );
    if( env != NULL )
        AddItemToIncludePath( env, NULL );
    if( !Options.quiet && !Options.banner_printed ) {
        Options.banner_printed = TRUE;
        trademark();
    }
    open_files();
    PushLineQueue();
}

#ifdef __UNIX__

int main( int argc, char **argv )
/*******************************/
{
    argc = argc;
#ifndef __WATCOMC__
    _argv = argv;
    _argc = argc;
#endif

#else

int main( void )
/**************/
{
    char       *argv[2];
    int        len;
    char       *buff;
#endif

    main_init();
    SwitchChar = _dos_switch_char();
#ifndef __UNIX__
    len = _bgetcmd( NULL, INT_MAX ) + 1;
    buff = malloc( len );
    if( buff != NULL ) {
        argv[0] = buff;
        argv[1] = NULL;
        _bgetcmd( buff, len );
    } else {
        return( -1 );
    }
    do_init_stuff( argv );
#else
    do_init_stuff( &argv[1] );
#endif
    SetMemoryModel();
    WriteObjModule();           // main body: parse the source file
    MsgFini();
    main_fini();
#ifndef __UNIX__
    free( buff );
#endif
    return( Options.error_count ); /* zero if no errors */
}

void set_cpu_parameters( void )
/*****************************/
{
    asm_token   token;

    // Start in masm mode
    Options.mode &= ~MODE_IDEAL;
    // set parameters passing convention macro
    if( SWData.cpu >= 3 ) {
        if( Options.watcom_parms_passed_by_regs ) {
            add_constant( "__REGISTER__" );
        } else {
            add_constant( "__STACK__" );
        }
    }
    switch( SWData.cpu ) {
    case 0:
        token = T_DOT_8086;
        break;
    case 1:
        token = T_DOT_186;
        break;
    case 2:
        token =  SWData.protect_mode ? T_DOT_286P : T_DOT_286;
        break;
    case 3:
        token =  SWData.protect_mode ? T_DOT_386P : T_DOT_386;
        break;
    case 4:
        token =  SWData.protect_mode ? T_DOT_486P : T_DOT_486;
        break;
    case 5:
        token =  SWData.protect_mode ? T_DOT_586P : T_DOT_586;
        break;
    case 6:
        token =  SWData.protect_mode ? T_DOT_686P : T_DOT_686;
        break;
    default:
        return;
    }
    cpu_directive( token );
}

void set_fpu_parameters( void )
/*****************************/
{
    asm_token   token;

    switch( floating_point ) {
    case DO_FP_EMULATION:
        add_constant( "__FPI__" );
        break;
    case NO_FP_EMULATION:
        add_constant( "__FPI87__" );
        break;
    case NO_FP_ALLOWED:
        add_constant( "__FPC__" );
        cpu_directive( T_DOT_NO87 );
        return;
    }
    switch( SWData.fpu ) {
    case 0:
    case 1:
        token = T_DOT_8087;
        break;
    case 2:
        token = T_DOT_287;
        break;
    case 3:
    case 5:
    case 6:
        token = T_DOT_387;
        break;
    case 7:
    default: // unspecified FPU
        switch( SWData.cpu ) {
        case 0:
        case 1:
            token = T_DOT_8087;
            break;
        case 2:
            token = T_DOT_287;
            break;
        case 3:
        case 4:
        case 5:
        case 6:
            token = T_DOT_387;
            break;
        default:
            return;
        }
        break;
    }
    cpu_directive( token );
}

void CmdlParamsInit( void )
/*************************/
{
    Code->use32 = 0;                // default is 16-bit segment
    Code->info.cpu = P_86 | P_87;   // default is 8086 CPU and 8087 FPU

    if( ForceInclude != NULL )
        InputQueueFile( ForceInclude );

    set_cpu_parameters();
    set_fpu_parameters();
}

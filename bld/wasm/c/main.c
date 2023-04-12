/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
#include "asmalloc.h"
#include "fatal.h"
#include "asmexpnd.h"
#include "swchar.h"
#include "asminput.h"
#include "banner.h"
#include "directiv.h"
#include "standalo.h"
#include "pathgrp2.h"
#include "omfgenio.h"
#include "omfobjre.h"

#include "clibint.h"
#include "clibext.h"


extern void             Fatal( unsigned msg, ... );
extern void             DelErrFile( void );

File_Info               AsmFiles;       // files information

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
    false, // real mode CPU instructions set
    -1,    // unspecified CPU
    -1     // unspecified FPU
};

#define MAX_NESTING 15
#define BUF_SIZE 512

static char             ParamBuf[BUF_SIZE];
static unsigned char    SwitchChar;
static unsigned         OptValue;
static char             *OptScanPtr;
static char             *OptParm;
static char             *ForceInclude = NULL;

global_options Options = {
    false,              // sign_value
    false,              // stop_at_end
    false,              // quiet
    false,              // banner_printed
    false,              // debug_info
    true,               // output_comment_data_in_code_records
    true,               // symbols_nocasesensitive

    0,                  // error_count
    0,                  // warning_count
    20,                 // error_limit
    2,                  // warning_level
    false,              // warning_error
    NULL,               // build_target

    NULL,               // code_class
    NULL,               // data_seg
    NULL,               // text_seg
    NULL,               // module_name

#ifdef DEBUG_OUT
    false,              // debug
#endif
    NULL,               // default_name_mangler
    false,              // allow_c_octals
    true,               // emit_dependencies
    true,               // stdcall at number
    true,               // mangle stdcall
    false,              // write listing
    true,               // parameters passed by registers
    MODE_WATCOM,        // initial assembler mode from cmdl
    MODE_WATCOM,        // current assembler mode
    0,                  // locals prefix len
    {'\0','\0','\0'},   // locals prefix
    0,                  // trace stack
    true                // instructions optimization
};

static char *CopyOfParm( void )
/*****************************/
{
    size_t  len;

    len = OptScanPtr - OptParm;
    memcpy( ParamBuf, OptParm, len );
    ParamBuf[len] = '\0';
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

static void SetTargName( char *name, size_t len )
/***********************************************/
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
    for( ; len != 0; --len ) {
        *p++ = (char)toupper( *name++ );
    }
    *p++ = '\0';
}

static void SetCPUPMC( void )
/***************************/
{
    char                *tmp;

    for( tmp = OptParm; tmp < OptScanPtr; tmp++ ) {
        if( *tmp == 'p' ) {
            if( SWData.cpu >= 2 ) { // set protected mode
                SWData.protect_mode = true;
            } else {
                MsgPrintf1( MSG_CPU_OPTION_INVALID, CopyOfParm() );
            }
        } else if( *tmp == 'r' ) {
            if( SWData.cpu >= 3 ) { // set register based calling convention
                Options.watcom_parms_passed_by_regs = true;
            } else {
                MsgPrintf1( MSG_CPU_OPTION_INVALID, CopyOfParm() );
            }
        } else if( *tmp == 's' ) {
            if( SWData.cpu >= 3 ) { // set stack based calling convention
                Options.watcom_parms_passed_by_regs = false;
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
            exit( EXIT_ERROR );
        }
    }
    if( SWData.cpu < 3 ) {
        Options.watcom_parms_passed_by_regs = true;
        if( SWData.cpu < 2 ) {
            SWData.protect_mode = false;
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

static char memory_model = '\0';

static void SetMM( void )
/***********************/
{
    char buffer[4];

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
        buffer[0] = '/';
        buffer[1] = 'm';
        buffer[2] = (char)OptValue;
        buffer[3] = '\0';
        MsgPrintf1( MSG_UNKNOWN_OPTION, buffer );
        exit( EXIT_ERROR );
    }

    memory_model = (char)OptValue;
}

static bool isvalidident( const char *id )
/****************************************/
{
    int     c;

    if( isdigit( *(unsigned char *)id ) )
        return( false ); /* can't start with a number */
    while( (c = *(unsigned char *)id++) != '\0' ) {
        if( !( IS_VALID_ID_CHAR( c ) || c == '.' ) ) {
            return( false );
        }
    }
    return( true );
}

static void add_constant( const char *string, bool underscored )
/**************************************************************/
{
    char    name[MAX_LINE_LEN];
    char    *tmp;
    char    c;

    tmp = name;
    if( underscored ) {
        *tmp++ = '_'; *tmp++ = '_';
    }
    while( (c = *string) != '\0' ) {
        ++string;
        if( c == '=' || c == '#' )
            break;
        *tmp++ = c;
    }
    if( underscored ) {
        *tmp++ = '_'; *tmp++ = '_';
    }
    *tmp = '\0';
    if( *string == '\0' )
        string = "1";

    if( isvalidident( name ) ) {
        StoreConstant( name, string, false ); // don't allow it to be redef'd
    } else {
        AsmError( SYNTAX_ERROR ); // fixme
    }
}

static void AddStringToIncludePath( char *str, const char *end )
/**************************************************************/
{
    char        *src;
    char        *p;
    bool        have_quote;
    char        c;

    have_quote = false;
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
    char        name [_MAX_PATH ];
    char        msgbuf[MAX_MESSAGE_SIZE];
    pgroup2     pg;
    pgroup2     def;

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
        if( pg.ext[0] == '\0' ) {
            pg.ext = ASM_EXT;
        }
        _makepath( name, pg.drive, pg.dir, pg.fname, pg.ext );
        AsmFiles.fname[ASM] = AsmStrDup( name );

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
            if( def.fname[0] == NULLC )
                def.fname = pg.fname;
            if( def.ext[0] == NULLC )
                def.ext = OBJ_EXT;

            _makepath( name, def.drive, def.dir, def.fname, def.ext );
            AsmFree( AsmFiles.fname[OBJ] );
        }
        AsmFiles.fname[OBJ] = AsmStrDup( name );

        if( AsmFiles.fname[ERR] == NULL ) {
            pg.ext = ERR_EXT;
            _makepath( name, NULL, NULL, pg.fname, pg.ext );
        } else {
            _splitpath2( AsmFiles.fname[ERR], def.buffer, &def.drive,
                         &def.dir, &def.fname, &def.ext );
            if( def.fname[0] == NULLC )
                def.fname = pg.fname;
            if( def.ext[0] == NULLC )
                def.ext = ERR_EXT;
            _makepath( name, def.drive, def.dir, def.fname, def.ext );
            AsmFree( AsmFiles.fname[ERR] );
        }
        AsmFiles.fname[ERR] = AsmStrDup( name );

        if( AsmFiles.fname[LST] == NULL ) {
            pg.ext = LST_EXT;
            _makepath( name, NULL, NULL, pg.fname, pg.ext );
        } else {
            _splitpath2( AsmFiles.fname[LST], def.buffer, &def.drive,
                         &def.dir, &def.fname, &def.ext );
            if( def.fname[0] == NULLC )
                def.fname = pg.fname;
            if( def.ext[0] == NULLC )
                def.ext = LST_EXT;
            _makepath( name, def.drive, def.dir, def.fname, def.ext );
            AsmFree( AsmFiles.fname[LST] );
        }
        AsmFiles.fname[LST] = AsmStrDup( name );

    } else {
        /* get filename for object, error, or listing file */
        _splitpath2( token, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
        if( AsmFiles.fname[ASM] != NULL ) {
            _splitpath2( AsmFiles.fname[ASM], def.buffer, &def.drive,
                         &def.dir, &def.fname, &def.ext );
            if( pg.fname[0] == NULLC ) {
                pg.fname = def.fname;
            }
        }
        if( pg.ext[0] == NULLC ) {
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
        AsmFiles.fname[type] = AsmStrDup( name );
    }
}

static void set_some_kinda_name( char token, char *name )
/*******************************************************/
/* set:  code class / data seg. / module name / text seg */
{
    char    **tmp;

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
    *tmp = AsmStrDup( name );
}

static void usage_msg( void )
/***************************/
{
    PrintfUsage();
    exit( EXIT_ERROR );
}

static void Ignore( void ) {}

static void Set_BT( void ) { SetTargName( OptParm,  OptScanPtr - OptParm ); }

static void Set_C( void ) { Options.output_comment_data_in_code_records = false; }

static void Set_CX( void ) { Options.symbols_nocasesensitive = false; }

static void Set_D( void ) { Options.debug_info = (OptValue != 0); }

static void DefineMacro( void ) { add_constant( CopyOfParm(), false ); }

static void SetErrorLimit( void ) { Options.error_limit = OptValue; }

static void SetStopEnd( void ) { Options.stop_at_end = true; }

static void Set_FR( void ) { get_fname( GetAFileName(), ERR ); }

static void Set_FI( void ) { ForceInclude = AsmStrDup( GetAFileName() ); }

static void Set_FL( void ) { get_fname( GetAFileName(), LST ); Options.write_listing = true; }

static void Set_FO( void ) { get_fname( GetAFileName(), OBJ ); }

static void SetInclude( void ) { AddStringToIncludePath( OptParm, OptScanPtr ); }

static void Set_S( void ) { Options.sign_value = true; }

static void Set_N( void ) { set_some_kinda_name( (char)OptValue, CopyOfParm() ); }

static void Set_O( void ) { Options.allow_c_octals = true; }

static void Set_OD( void ) { Options.optimization = false; }

static void Set_OF( void ) { Options.trace_stack = (char)OptValue; }

static void Set_WE( void ) { Options.warning_error = true; }

static void Set_WX( void ) { Options.warning_level = 4; }

static void SetWarningLevel( void ) { Options.warning_level = (char)OptValue; }

static void Set_ZCM( void )
{
    if( OptScanPtr == OptParm || strnicmp( OptParm, "MASM", OptScanPtr - OptParm ) == 0 ) {
        Options.mode_init = MODE_MASM6;
    } else if( strnicmp( OptParm, "WATCOM", OptScanPtr - OptParm ) == 0 ) {
        Options.mode_init = MODE_WATCOM;
    } else if( strnicmp( OptParm, "TASM", OptScanPtr - OptParm ) == 0 ) {
        Options.mode_init = MODE_TASM | MODE_MASM5;
    } else if( strnicmp( OptParm, "IDEAL", OptScanPtr - OptParm ) == 0 ) {
        Options.mode_init = MODE_TASM | MODE_IDEAL;
//    } else if( strnicmp( OptParm, "MASM5", OptScanPtr - OptParm ) == 0 ) {
//        Options.mode_init = MODE_MASM5;
    }
}

static void Set_ZLD( void ) { Options.emit_dependencies = false; }

static void Set_ZQ( void ) { Options.quiet = true; }

static void Set_ZZ( void ) { Options.use_stdcall_at_number = false; }

static void Set_ZZO( void ) { Options.mangle_stdcall = false; }

static void HelpUsage( void ) { usage_msg();}

#ifdef DEBUG_OUT
static void Set_D6( void )
{
    Options.int_debug = true;
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
    { "cx",     0,        Set_CX },
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
    { "od",     0,        Set_OD },
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

static bool OptionDelimiter( char c )
/***********************************/
{
    return( c == ' ' || c == '-' || c == '\0' || c == '\t' || c == SwitchChar );
}

static void get_os_include( void )
/********************************/
{
    char        *env;
    char        *tmp;
    char        *p;
    size_t      len;

    /* add OS_include to the include path */

    len = strlen( Options.build_target );
    tmp = AsmTmpAlloc( len + 10 );
    p = CATSTR( tmp, Options.build_target, len );
    p = CATLIT( p, "_INCLUDE" );
    *p = '\0';
    env = getenv( tmp );
    if( env != NULL ) {
        AddItemToIncludePath( env, NULL );
    }
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
    for( i = ASM; i <= OBJ; i++ ) {
        AsmFiles.file[i] = NULL;
        AsmFiles.fname[i] = NULL;
    }
    ObjRecInit();
}

static void main_fini( void )
/***************************/
{
    free_names();
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
    ObjWriteOpen();

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

static char *ReadIndirectFile( char *name )
/*****************************************/
{
    char        *env;
    char        *str;
    FILE        *fp;
    size_t      len;
    char        ch;

    env = NULL;
    fp = fopen( name, "rb" );
    if( fp != NULL ) {
        fseek( fp, 0, SEEK_END );
        len = ftell( fp );
        fseek( fp, 0, SEEK_SET );
        env = AsmAlloc( len + 1 );
        len = fread( env, 1, len, fp );
        env[len] = '\0';
        fclose( fp );
        // zip through characters changing \r, \n etc into ' '
        for( str = env; *str != '\0'; ++str ) {
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
    int         c;

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
            buffers[*level] = NULL;
            save[*level] = CollectEnvOrFileName( str + 1 );
            str = getenv( ParamBuf );
            if( str != NULL ) {
                str = AsmStrDup( str );
            } else {
                str = ReadIndirectFile( ParamBuf );
            }
            if( str != NULL ) {
                buffers[(*level)++] = str;
                continue;
            }
            str = save[*level];
        }
        if( *str == '\0' ) {
            if( *level == 0 )
                break;
            --(*level);
            AsmFree( buffers[*level] );
            buffers[*level] = NULL;
            str = save[*level];
            continue;
        }
        if( *str == '-' || *str == SwitchChar ) {
            str = ProcessOption( str + 1, str );
            if( str == NULL ) {
                exit( EXIT_ERROR );
            }
        } else {  /* collect file name */
            char *beg, *p;
            size_t len;

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
            len = str - beg;
            p = AsmAlloc( len + 1 );
            memcpy( p, beg, len );
            p[len] = '\0';
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

static bool set_build_target( void )
/**********************************/
{
    if( Options.build_target == NULL ) {
#if defined(__QNX__)
        SetTargName( "QNX", 3 );
#elif defined(__LINUX__)
        SetTargName( "LINUX", 5 );
#elif defined(__HAIKU__)
        SetTargName( "HAIKU", 5 );
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
#elif defined(__RDOS__)
        SetTargName( "RDOS", 4 );
#else
        #error unknown host OS
#endif
    }

    add_constant( Options.build_target, true ); // always define something

    if( stricmp( Options.build_target, "DOS" ) == 0 ) {
        add_constant( "MSDOS", true );
    } else if( stricmp( Options.build_target, "NETWARE" ) == 0 ) {
        if( SWData.cpu >= 3 ) {
            add_constant( "NETWARE_386", true );
        }
    } else if( stricmp( Options.build_target, "WINDOWS" ) == 0 ) {
        if( SWData.cpu >= 3 ) {
            add_constant( "WINDOWS_386", true );
        }
    } else if( stricmp( Options.build_target, "QNX" ) == 0
      || stricmp( Options.build_target, "LINUX" ) == 0
      || stricmp( Options.build_target, "BSD" ) == 0 ) {
        add_constant( "UNIX", true );
    }
    return( RC_OK );
}

static void set_cpu_mode( void )
/******************************/
{
    // set parameters passing convention macro
    if( SWData.cpu >= 3 ) {
        if( Options.watcom_parms_passed_by_regs ) {
            add_constant( "REGISTER", true );
        } else {
            add_constant( "STACK", true );
        }
    }
}

static void set_fpu_mode( void )
/******************************/
{
    switch( floating_point ) {
    case DO_FP_EMULATION:
        add_constant( "FPI", true );
        break;
    case NO_FP_EMULATION:
        add_constant( "FPI87", true );
        break;
    case NO_FP_ALLOWED:
        add_constant( "FPC", true );
        break;
    }
}

static void set_cpu_parameters( void )
/*************************************
 * initialization is always done
 * in MASM mode by MASM directives
 */
{
    asm_token   token;
    char        buffer[MAX_KEYWORD_LEN + 1];

    switch( SWData.cpu ) {
    case 0:
        token = T_DOT_8086;
        break;
    case 1:
        token = T_DOT_186;
        break;
    case 2:
        token = ( SWData.protect_mode ) ? T_DOT_286P : T_DOT_286;
        break;
    case 3:
        token = ( SWData.protect_mode ) ? T_DOT_386P : T_DOT_386;
        break;
    case 4:
        token = ( SWData.protect_mode ) ? T_DOT_486P : T_DOT_486;
        break;
    case 5:
        token = ( SWData.protect_mode ) ? T_DOT_586P : T_DOT_586;
        break;
    case 6:
        token = ( SWData.protect_mode ) ? T_DOT_686P : T_DOT_686;
        break;
    default:
        return;
    }
    buffer[0] = '.';
    GetInsString( token, buffer + 1 );
    InputQueueLine( buffer );
}

static void set_fpu_parameters( void )
/*************************************
 * initialization is always done
 * in MASM mode by MASM directives
 */
{
    asm_token   token;
    char        buffer[MAX_KEYWORD_LEN + 1];

    switch( floating_point ) {
    case NO_FP_ALLOWED:
        token = T_DOT_NO87;
        break;
    case DO_FP_EMULATION:
    case NO_FP_EMULATION:
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
            // derive FPU from CPU value
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
            default: // unspecified CPU
                return;
            }
            break;
        }
        break;
    default: // unknown floating_point value
        return;
    }
    buffer[0] = '.';
    GetInsString( token, buffer + 1 );
    InputQueueLine( buffer );
}

static void SetMemoryModel( void )
/*********************************
 * initialization is always done
 * in MASM mode by MASM directives
 */
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

    strcpy( buffer, ".MODEL " );
    strcat( buffer, model );
    InputQueueLine( buffer );
}

static void set_assembler_mode( void )
/************************************/
{
    // setup MASM compatible mode for initialization
    // TASM mode is setup after initialization by directives
    if( Options.mode_init & MODE_TASM ) {
        // if TASM mode then initialize
        // by appropriate directive
        if( Options.mode_init & MODE_IDEAL ) {
            InputQueueLine( "IDEAL" );
        } else {
            InputQueueLine( "MASM" );
        }
    }
    // init assembler mode to MASM mode
    Options.mode = Options.mode_init & ~(MODE_TASM | MODE_IDEAL);
    // initialization of TASM local labels prefix
    Options.locals_prefix[0] = '@';
    Options.locals_prefix[1] = '@';
    Options.locals_len = 0;
}

static void parse_cmdline( char **cmdline )
/*****************************************/
{
    char msgbuf[MAX_MESSAGE_SIZE];
    int  level = 0;

    if( cmdline == NULL || *cmdline == NULL || **cmdline == 0 ) {
        usage_msg();
    }
    for( ; *cmdline != NULL; ++cmdline ) {
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

    if( !MsgInit() )
        exit( EXIT_ERROR );

    add_constant( "WASM=" _MACROSTR( _BLDVER ), true );
    ForceInclude = AsmStrDup( getenv( "FORCE" ) );
    do_envvar_cmdline( "WASM" );
    parse_cmdline( cmdline );
    set_build_target();
    set_cpu_mode();
    set_fpu_mode();
    get_os_include();
    env = getenv( "INCLUDE" );
    if( env != NULL )
        AddItemToIncludePath( env, NULL );
    PrintBanner();
    open_files();

    /*
     * insert appropriate directives for command line parameters
     * into input line queue to be processed before any source file
     */
    if( memory_model != '\0' ) {
        if( SWData.cpu < 0 ) {
            if( memory_model == 'f' ) {
                SWData.cpu = 3;
                SWData.protect_mode = true;
            } else {
                SWData.cpu = 0;
            }
        }
    }
}

static void do_fini_stuff( void )
/*******************************/
{
    MsgFini();
}

#ifdef __UNIX__

int main( int argc, char **argv )
/*******************************/
{
  #if !defined( __WATCOMC__ )
    _argc = argc;
    _argv = argv;
  #else
    /* unused parameters */ (void)argc;
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
#ifdef __UNIX__
    do_init_stuff( &argv[1] );
#else
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
    free( buff );
#endif
    WriteObjModule();           // main body: parse the source file
    do_fini_stuff();
    main_fini();
    return( Options.error_count ); /* zero if no errors */
}

void CmdlParamsInit( void )
/*************************/
{
    Code->use32 = false;            // default is 16-bit segment
    Code->info.cpu = ModuleInfo.cpu_init;
    ModuleInfo.def_use32 = ModuleInfo.def_use32_init;

    PushLineQueue();
    set_cpu_parameters();
    set_fpu_parameters();
    SetMemoryModel();
    set_assembler_mode();

    if( ForceInclude != NULL ) {
        InputQueueFile( ForceInclude );
    }
}

void FreeForceInclude( void )
/***************************/
{
    AsmFree( ForceInclude );
}

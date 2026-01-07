/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
#include "cmdlnprs.gh"
#include "cmdlnprs.h"
#include "cmdscan.h"

#include "clibint.h"
#include "clibext.h"


extern void             Fatal( unsigned msg, ... );
extern void             DelErrFile( void );

#if defined( __WATCOMC__ ) && defined( _M_IX86 )
unsigned char   _8087 = 0;
unsigned char   _real87 = 0;
#endif

File_Info               AsmFiles;       // files information
char                    *ModuleName = NULL;

struct  option {
    char        *option;
    unsigned    value;
    void        (*function)( void );
};

#define MAX_NESTING     15
#define BUF_SIZE        512

sw_data SWData = {
    false, // real mode CPU instructions set
    -1,    // unspecified CPU
    -1,    // unspecified FPU
    -1,    // unspecified memory model
    -1     // unspecified FP mode
};

static char             *ForceInclude = NULL;
static const char       *switch_start = NULL;
static char             *SrcFName = NULL;
static char             *SrcModuleName = NULL;

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

static void usage_msg( void )
/***************************/
{
    PrintfUsage();
    exit( EXIT_ERROR );
}

static void BadCmdLine( int error_code )
/***************************************
 * SIGNAL CMD-LINE ERROR
 */
{
    char        buffer[128];
    size_t      len;

    for( CmdScanChar(); !CmdScanSwEnd(); CmdScanChar() ) {
        ;
    }
    len = CmdScanAddr() - switch_start;
    if( len > sizeof( buffer ) - 1 )
        len = sizeof( buffer ) - 1;
    strncpy( buffer, switch_start, len );
    buffer[len] = '\0';
    MsgPrintf1( error_code, buffer );
    printf( "\n" );
    exit( EXIT_ERROR );
}

// BAD CHAR DETECTED
void BadCmdLineChar( void )
{
    BadCmdLine( MSG_UNKNOWN_OPTION );
}
// BAD ID DETECTED
void BadCmdLineId( void )
{
    BadCmdLine( MSG_UNKNOWN_OPTION );
}
// BAD NUMBER DETECTED
void BadCmdLineNumber( void )
{
    BadCmdLine( MSG_UNKNOWN_OPTION );
}
// BAD PATH DETECTED
void BadCmdLinePath( void )
{
    BadCmdLine( MSG_UNKNOWN_OPTION );
}
// BAD FILE DETECTED
void BadCmdLineFile( void )
{
    BadCmdLine( MSG_UNKNOWN_OPTION );
}
// BAD TEXT DETECTED
void BadCmdLineOption( void )
{
    BadCmdLine( MSG_UNKNOWN_OPTION );
}

static char *SetTargetName( char *target_name, const char *name )
/***************************************************************/
{
    if( target_name != NULL ) {
        AsmFree( target_name );
    }
    return( AsmStrDup( name ) );
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

const char *GetModuleName( void )
{
    if( Options.module_name != NULL )
        return( Options.module_name );
    return( SrcModuleName );
}

char *CreateFileName( const char *template, const char *ext, bool forceext )
/**************************************************************************/
{
    pgroup2     pg;
    bool        use_defaults;
    static char filename_buff[_MAX_PATH];
    const char  *fn;

    use_defaults = ( template == NULL );
    if( use_defaults )
        template = AsmFiles.fname[ASM];
    _splitpath2( template, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    if( use_defaults ) {
        /*
         * default object file goes in current directory
         */
        pg.drive = "";
        pg.dir = "";
    }
    fn = pg.fname;
    if( fn[0] == '\0' || fn[0] == '*' && fn[1] == '\0' ) {
        fn = SrcFName;
    }
    if( !forceext && pg.ext[0] != '\0' && !use_defaults ) {
        ext = pg.ext;
    }
    _makepath( filename_buff, pg.drive, pg.dir, fn, ext );
    return( filename_buff );
}

void ConvertModuleName( char *module_name )
{
    char *p;
    int  c;

    for( p = module_name; (c = *(unsigned char *)p) != '\0'; ++p ) {
        if( !IS_VALID_ID_CHAR( c ) ) {
            /*
             * it's not a legal character for a symbol name
             */
            *p = '_';
        }
    }
    /*
     * first character can't be a number either
     */
    if( isdigit( module_name[0] ) ) {
        module_name[0] = '_';
    }
}

static void srcFileName( char *token )
/*************************************
 * figure out the source file name & store it in AsmFiles
 * fill in default object file name if it is null
 */
{
    char        name[_MAX_PATH ];
    char        msgbuf[MAX_MESSAGE_SIZE];
    pgroup2     pg;

    /* get filename for source file */

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
    SrcFName = AsmStrDup( pg.fname );
    SrcModuleName = AsmStrDup( pg.fname );
    ConvertModuleName( SrcModuleName );
    _makepath( name, pg.drive, pg.dir, pg.fname, pg.ext );
    AsmFiles.fname[ASM] = AsmStrDup( name );
}

static void add_include( const char *target_name, const char *src )
/*****************************************************************/
{
    char        buff[_MAX_PATH];
    pgroup2     pg;
    const char  *env;

    _splitpath2( src, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    _makepath( buff, pg.drive, pg.dir, NULL, NULL );
    /*
     * add the source path to the include path
     * as first item on the include path list
     */
    AddItemToIncludePath( buff, NULL );
    /*
     * add OS_include to the include path
     * as last items on the list
     */
    strcpy( buff, target_name );
    strcat( buff, "_INCLUDE" );
    env = getenv( buff );
    if( env != NULL ) {
        AddItemToIncludePath( env, NULL );
    }
    env = getenv( "INCLUDE" );
    if( env != NULL ) {
        AddItemToIncludePath( env, NULL );
    }
}

static void free_names( void )
/****************************/
/* Free names set as cmdline options */
{
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
    AsmFiles.fname[ERR] = AsmStrDup( "*" );
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

static bool scanMode( unsigned *p )
{
    const char  *str;
    size_t      len;
    char        buff[16];

    CmdRecogEquals();
    len = CmdScanId( &str );
    if( len > sizeof( buff ) - 1 ) {
        len = sizeof( buff ) - 1;
    }
    strncpy( buff, str, len );
    buff[len] = '\0';
    strupr( buff );
    if( strcmp( buff, "MASM5" ) == 0 ) {
#if 0
        *p = MODE_MASM5;
#else
        return( false );
#endif
    } else if( strcmp( buff, "MASM" ) == 0 ) {
        *p = MODE_MASM6;
    } else if( strcmp( buff, "WATCOM" ) == 0 ) {
        *p = MODE_WATCOM;
    } else if( strcmp( buff, "TASM" ) == 0 ) {
        *p = MODE_TASM | MODE_MASM5;
    } else if( strcmp( buff, "IDEAL" ) == 0 ) {
        *p = MODE_TASM | MODE_IDEAL;
    } else {
        return( false );
    }
    return( true );
}

static bool scanDefine( OPT_STRING **h )
{
    return( OPT_GET_OPTION( h ) );
}

#include "cmdlnprs.gc"

static int ProcOptions( OPT_STORAGE *data, const char *str )
/**********************************************************/
{
    const char  *save[MAX_NESTING];
    char        *buffers[MAX_NESTING];
    int         level;
    int         ch;
    OPT_STRING  *fname;
    const char  *penv;
    char        *ptr;

    if( str != NULL ) {
        level = -1;
        CmdScanLineInit( str );
        for( ;; ) {
            CmdScanSkipWhiteSpace();
            ch = CmdScanChar();
            if( ch == '@' ) {
                switch_start = CmdScanAddr() - 1;
                CmdScanSkipWhiteSpace();
                fname = NULL;
                if( OPT_GET_FILE( &fname ) ) {
                    penv = NULL;
                    level++;
                    if( level < MAX_NESTING ) {
                        ptr = NULL;
                        penv = getenv( fname->data );
                        if( penv == NULL ) {
                            ptr = ReadIndirectFile( fname->data );
                            penv = ptr;
                        }
                        if( penv != NULL ) {
                            save[level] = CmdScanLineInit( penv );
                            buffers[level] = ptr;
                        }
                    }
                    if( penv == NULL ) {
                        level--;
                    }
                    OPT_CLEAN_STRING( &fname );
                }
                continue;
            }
            if( ch == '\0' ) {
                if( level < 0 )
                    break;
                AsmFree( buffers[level] );
                CmdScanLineInit( save[level] );
                level--;
                continue;
            }
            if( CmdScanSwitchChar( ch ) ) {
                switch_start = CmdScanAddr() - 1;
                if( OPT_PROCESS( data ) ) {
                    BadCmdLineOption();
                }
            } else {  /* collect file name */
                CmdScanUngetChar();
                switch_start = CmdScanAddr();
                fname = NULL;
                if( OPT_GET_FILE( &fname ) ) {
                    srcFileName( fname->data );
                    OPT_CLEAN_STRING( &fname );
                }
            }
        }
    }
    return( 0 );
}

static char *SetStringOption( char **o, OPT_STRING **h )
/******************************************************/
{
    OPT_STRING *s;
    char *p;

    s = *h;
    p = NULL;
    if( s != NULL ) {
        if( s->data[0] != '\0' ) {
            p = AsmStrDup( s->data );
        }
        OPT_CLEAN_STRING( h );
    }
    if( o != NULL ) {
        AsmFree( *o );
        *o = p;
    }
    return( p );
}

static void reverseList( OPT_STRING **h )
{
    OPT_STRING *s;
    OPT_STRING *p;
    OPT_STRING *n;

    s = *h;
    *h = NULL;
    for( p = s; p != NULL; p = n ) {
        n = p->next;
        p->next = *h;
        *h = p;
    }
}

static char *set_build_target( OPT_STORAGE *data )
/************************************************/
{
    char    *target_name = NULL;

    if( data->bt ) {
        char *target = SetStringOption( NULL, &(data->bt_value) );
        target_name = SetTargetName( target_name, strupr( target ) );
        AsmFree( target );
    }

    if( target_name == NULL ) {
#if defined(__QNX__)
        target_name = SetTargetName( target_name, "QNX" );
#elif defined(__LINUX__)
        target_name = SetTargetName( target_name, "LINUX" );
#elif defined(__HAIKU__)
        target_name = SetTargetName( target_name, "HAIKU" );
#elif defined(__BSD__)
        target_name = SetTargetName( target_name, "BSD" );
#elif defined(__OSX__) || defined(__APPLE__)
        target_name = SetTargetName( target_name, "OSX" );
#elif defined(__SOLARIS__) || defined( __sun )
        target_name = SetTargetName( target_name, "SOLARIS" );
#elif defined(__DOS__)
        target_name = SetTargetName( target_name, "DOS" );
#elif defined(__OS2__)
        target_name = SetTargetName( target_name, "OS2" );
#elif defined(__NT__)
        target_name = SetTargetName( target_name, "NT" );
#elif defined(__RDOS__)
        target_name = SetTargetName( target_name, "RDOS" );
#else
        #error unknown host OS
#endif
    }

    add_constant( target_name, true ); // always define something

    if( strcmp( target_name, "DOS" ) == 0 ) {
        add_constant( "MSDOS", true );
    } else if( strcmp( target_name, "NETWARE" ) == 0 ) {
        if( SWData.cpu >= 3 ) {
            add_constant( "NETWARE_386", true );
        }
    } else if( strcmp( target_name, "WINDOWS" ) == 0 ) {
        if( SWData.cpu >= 3 ) {
            add_constant( "WINDOWS_386", true );
        }
    } else if( strcmp( target_name, "QNX" ) == 0
      || strcmp( target_name, "LINUX" ) == 0
      || strcmp( target_name, "BSD" ) == 0 ) {
        add_constant( "UNIX", true );
    }
    return( target_name );
}

static void set_cpu_mode( void )
/******************************/
{
    // set parameters passing convention macro
    if( SWData.cpu >= 3 ) {
        if( Options.watcom_params_passed_by_regs ) {
            add_constant( "REGISTER", true );
        } else {
            add_constant( "STACK", true );
        }
    }
}

static void set_fpu_mode( void )
/******************************/
{
    switch( SWData.fpt ) {
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

    switch( SWData.fpt ) {
    case NO_FP_ALLOWED:
        token = T_DOT_NO87;
        break;
    case DO_FP_EMULATION:
    case NO_FP_EMULATION:
        switch( SWData.fpu ) {
        case 0:
            token = T_DOT_NO87;
            break;
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
    default: // unknown SWData.fpt value
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

    switch( SWData.mem_model ) {
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

static void set_options( OPT_STORAGE *data )
{
    if( data->zq ) {
        Options.quiet = true;
    }
    if( data->_question ) {
        usage_msg();
    }

    switch( data->mem_model ) {
    case OPT_ENUM_mem_model_mt:
        SWData.mem_model = 't';
        break;
    case OPT_ENUM_mem_model_ms:
        SWData.mem_model = 's';
        break;
    case OPT_ENUM_mem_model_mm:
        SWData.mem_model = 'm';
        break;
    case OPT_ENUM_mem_model_ml:
        SWData.mem_model = 'l';
        break;
    case OPT_ENUM_mem_model_mh:
        SWData.mem_model = 'h';
        break;
    case OPT_ENUM_mem_model_mf:
        SWData.mem_model = 'f';
        break;
    case OPT_ENUM_mem_model_mc:
        SWData.mem_model = 'c';
        break;
    case OPT_ENUM_mem_model_default:
        break;
    }
    switch( data->cpu_info ) {
    case OPT_ENUM_cpu_info__0:
        SWData.cpu = 0;
        Options.watcom_params_passed_by_regs = true;
        break;
    case OPT_ENUM_cpu_info__1:
        SWData.cpu = 1;
        Options.watcom_params_passed_by_regs = true;
        break;
    case OPT_ENUM_cpu_info__2:
        SWData.cpu = 2;
        Options.watcom_params_passed_by_regs = true;
        if( data->_2p ) {
            SWData.protect_mode = true;
        }
        break;
    case OPT_ENUM_cpu_info__3:
        SWData.cpu = 3;
        Options.watcom_params_passed_by_regs = true;
        if( data->_3p ) {
            SWData.protect_mode = true;
        }
        if( data->_3s ) {
            Options.watcom_params_passed_by_regs = false;
        }
        break;
    case OPT_ENUM_cpu_info__4:
        SWData.cpu = 4;
        Options.watcom_params_passed_by_regs = true;
        if( data->_4p ) {
            SWData.protect_mode = true;
        }
        if( data->_4s ) {
            Options.watcom_params_passed_by_regs = false;
        }
        break;
    case OPT_ENUM_cpu_info__5:
        SWData.cpu = 5;
        Options.watcom_params_passed_by_regs = true;
        if( data->_5p ) {
            SWData.protect_mode = true;
        }
        if( data->_5s ) {
            Options.watcom_params_passed_by_regs = false;
        }
        break;
    case OPT_ENUM_cpu_info__6:
        SWData.cpu = 6;
        Options.watcom_params_passed_by_regs = true;
        if( data->_6p ) {
            SWData.protect_mode = true;
        }
        if( data->_6s ) {
            Options.watcom_params_passed_by_regs = false;
        }
        break;
    case OPT_ENUM_cpu_info_default:
        break;
    }
    /*
     * if memory model is setup on command line and cpu isn't
     * then select default cpu
     */
    if( SWData.mem_model > 0 ) {
        if( SWData.cpu < 0 ) {
            if( SWData.mem_model == 'f' ) {
                SWData.cpu = 3;
                SWData.protect_mode = true;
            } else {
                SWData.cpu = 0;
            }
        }
    }
    switch( data->fpu_info ) {
    case OPT_ENUM_fpu_info_fp6:
        SWData.fpu = 6;
        break;
    case OPT_ENUM_fpu_info_fp5:
        SWData.fpu = 5;
        break;
    case OPT_ENUM_fpu_info_fp3:
        SWData.fpu = 3;
        break;
    case OPT_ENUM_fpu_info_fp2:
        SWData.fpu = 2;
        break;
    case OPT_ENUM_fpu_info_fp0:
        SWData.fpu = 1;
        break;
    case OPT_ENUM_fpu_info_default:
        break;
    }
    switch( data->fpu_type ) {
    case OPT_ENUM_fpu_type_default:
    case OPT_ENUM_fpu_type_fpi:
        SWData.fpt = DO_FP_EMULATION;
        break;
    case OPT_ENUM_fpu_type_fpi87:
        SWData.fpt = NO_FP_EMULATION;
        break;
    case OPT_ENUM_fpu_type_fpc:
        SWData.fpt = NO_FP_ALLOWED;
        break;
    }

    switch( data->debug_info ) {
#ifdef DEBUG_OUT
    case OPT_ENUM_debug_info_d6:
        Options.int_debug = true;
        DebugMsg(( "debugging output on \n" ));
        break;
#endif
    case OPT_ENUM_debug_info_d3:
    case OPT_ENUM_debug_info_d2:
    case OPT_ENUM_debug_info_d1:
        Options.debug_info = true;
        break;
    case OPT_ENUM_debug_info_d0:
        Options.debug_info = false;
        break;
    case OPT_ENUM_debug_info_default:
        break;
    }
    if( data->c ) {
        Options.output_comment_data_in_code_records = false;
    }
    if( data->cx ) {
        Options.symbols_nocasesensitive = false;
    }
    if( data->e ) {
        Options.error_limit = data->e_value;
    }
    if( data->ee ) {
        Options.stop_at_end = true;
    }
    if( data->w ) {
        Options.warning_level = data->w_value;
    }
    if( data->i ) {
        OPT_STRING *s;
        /*
         * Process all -i items in linked list
         * add these items after source path and before
         * os path items
         */
        reverseList( &(data->i_value) );
        for( s = data->i_value; s != NULL; s = s->next ) {
            if( s->data[0] != '\0' ) {
                AddItemToIncludePath( s->data, NULL );
            }
        }
    }
    if( data->d ) {
        OPT_STRING *s;
        /*
         * Process all -d items in linked list
         */
        reverseList( &(data->d_value) );
        for( s = data->d_value; s != NULL; s = s->next ) {
            if( s->data[0] != '\0' ) {
                add_constant( s->data, false );
            }
        }
    }
    if( data->j ) {
        Options.sign_value = true;
    }
    if( data->o ) {
        Options.allow_c_octals = true;
    }
    if( data->od ) {
        Options.optimization = false;
    }
    switch( data->trace_stack ) {
    case OPT_ENUM_trace_stack_of:
        Options.trace_stack = 1;
        break;
    case OPT_ENUM_trace_stack_of_plus:
        Options.trace_stack = 2;
        break;
    case OPT_ENUM_trace_stack_default:
        break;
    }
    if( data->we ) {
        Options.warning_error = true;
    }
    if( data->wx ) {
        Options.warning_level = 4;
    }
    if( data->zcm ) {
        Options.mode_init = data->zcm_value;
    }
    if( data->zld ) {
        Options.emit_dependencies = false;
    }
    if( data->zz ) {
        Options.use_stdcall_at_number = false;
    }
    if( data->zzo ) {
        Options.mangle_stdcall = false;
    }
    if( data->nt ) {
        SetStringOption( &Options.text_seg, &(data->nt_value) );
    }
    if( data->nm ) {
        SetStringOption( &Options.module_name, &(data->nm_value) );
        ConvertModuleName( Options.module_name );
    }
    if( data->nd ) {
        SetStringOption( &Options.data_seg, &(data->nd_value) );
    }
    if( data->nc ) {
        SetStringOption( &Options.code_class, &(data->nc_value) );
    }
    if( data->fe && data->fe_value != NULL ) {
        AsmFree( AsmFiles.fname[ERR] );
        AsmFiles.fname[ERR] = AsmStrDup( data->fe_value->data );
    }
    if( data->fr ) {
        AsmFree( AsmFiles.fname[ERR] );
        if( data->fr_value != NULL ) {
            AsmFiles.fname[ERR] = AsmStrDup( data->fr_value->data );
        } else {
            AsmFiles.fname[ERR] = NULL;
        }
    }
    if( data->fl && data->fl_value != NULL ) {
        AsmFiles.fname[LST] = AsmStrDup( data->fl_value->data );
        Options.write_listing = true;
    }
    if( data->fo && data->fo_value != NULL ) {
        AsmFiles.fname[OBJ] = AsmStrDup( data->fo_value->data );
    }
    if( data->fi ) {
        SetStringOption( &ForceInclude, &(data->fi_value) );
    }
}

static void do_init_stuff( char **cmdline )
/*****************************************/
{
//    char        msgbuf[MAX_MESSAGE_SIZE];
    OPT_STORAGE data;
    char        *target_name;

    if( !MsgInit() )
        exit( EXIT_ERROR );

    add_constant( "WASM=" _MACROSTR( _BLDVER ), true );
    ForceInclude = AsmStrDup( getenv( "FORCE" ) );
    OPT_INIT( &data );
    ProcOptions( &data, getenv( "WASM" ) );
    for( ; *cmdline != NULL; ++cmdline ) {
        ProcOptions( &data, *cmdline );
    }
    target_name = set_build_target( &data );
    set_options( &data );
    OPT_FINI( &data );
    if( AsmFiles.fname[ASM] == NULL ) {
        usage_msg();
    }
    add_include( target_name, AsmFiles.fname[ASM] );
    AsmFree( target_name );
    set_cpu_mode();
    set_fpu_mode();
    PrintBanner();
    open_files();
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
    int        cmd_len;
    char       *cmd_line;

#endif

    main_init();
#ifdef __UNIX__
    do_init_stuff( &argv[1] );
#else
    cmd_len = _bgetcmd( NULL, 0 ) + 1;
    cmd_line = malloc( cmd_len );
    if( cmd_line == NULL ) {
        return( -1 );
    }
    _bgetcmd( cmd_line, cmd_len );
    argv[0] = cmd_line;
    argv[1] = NULL;
    do_init_stuff( argv );
    free( cmd_line );
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

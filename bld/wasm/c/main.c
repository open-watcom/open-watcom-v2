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
#include <string.h>
#include <process.h>
#include <ctype.h>
#include <malloc.h>

#include "asmglob.h"
#include "asmalloc.h"
#include "asmops1.h"
#include "asmins1.h"
#include "asmsym.h"
#include "directiv.h"
#include "fatal.h"
#include "asmerr.h"

#ifdef TRMEM
#include "memutil.h"
#endif


#include "womp.h"
#include "objprs.h"
#include "genmsomf.h"
#ifdef __OSI__
 #include "ostype.h"
#endif

extern void             Fatal( unsigned msg, ... );
extern void             WriteObjModule( void );
extern void             ObjRecInit( void );
extern void             DelErrFile();
extern void             PrintStats();
extern void             AsmInit( int, int, int );
extern int              AsmScan( char *, char * );
extern void             PrintfUsage( int first_ln );
extern void             MsgPrintf1( int resourceid, char *token );
extern void             InputQueueLine( char * );
extern int              InputQueueFile( char * );
extern void             PushLineQueue(void);
extern void             AddStringToIncludePath( char * );
extern int              cpu_directive( uint_16 );

extern struct asm_code  *Code;          // store information for assembler
extern struct asm_tok   *AsmBuffer[];   // buffer to store token
extern const char       *FingerMsg[];

File_Info               AsmFiles;       // files information
pobj_state              pobjState;      // object file information for WOMP

#define IS_EQ_CHAR( ch ) ( (ch)=='=' || (ch)=='#' )

global_options Options = {
    /* sign_value       */      FALSE,
    /* stop_at_end      */      FALSE,
    /* quiet            */      FALSE,
    /* banner_printed   */      FALSE,
    /* debug_flag       */      FALSE,
    /* naming_convention*/      DO_NOTHING,
    /* floating_point   */      DO_FP_EMULATION,
    /* output_data_in_code_records */   TRUE,

    /* error_count      */      0,
    /* warning_count    */      0,
    /* error_limit      */      20,
    /* warning_level    */      2,
    /* warning_error    */      FALSE,
    /* build_target     */      NULL,

    /* code_class       */      NULL,
    /* data_seg         */      NULL,
    /* test_seg         */      NULL,
    /* module_name      */      NULL,

    #ifdef DEBUG_OUT
    /* debug            */      FALSE,
    #endif
    /* default_name_mangler */  NULL,
    /* allow_c_octals   */      FALSE,
};

static int isvalidident( char *id )
/*********************************/
{
    char *s;
    char lwr_char;

    if( isdigit( *id ) ) return( ERROR ); /* can't start with a number */
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
    if( env != NULL ) AddStringToIncludePath( env );
}

void do_init_stuff( int argc, char *argv[] )
/******************************************/
{
    char        *env;
    char        *src;
    char        *dst;
    char        buff[80];

    if( !MsgInit() ) exit(1);

    AsmInit(-1, -1, -1);                // initialize hash table
    strcpy( buff, "__WASM__=" );
    dst = &buff[ strlen(buff) ];
    src = (char *)FingerMsg[0];
    while( !isdigit( *src ) ) ++src;
    while( isdigit( *src ) ) {
        *dst++ = *src++;
    }
    dst[0] = '0';
    dst[1] = '0';
    dst[2] = '\0';
    if( *src == '.' ) {
        if( isdigit( src[1] ) ) dst[0] = src[1];
        if( isdigit( src[2] ) ) dst[0] = src[2];
    }
    add_constant( buff );
    do_envvar_cmdline( "WASM", 0 );
    parse_cmdline( argc, argv );
    set_build_target();
    get_os_include();
    env = getenv( "INCLUDE" );
    if( env != NULL ) AddStringToIncludePath( env );
    if( !Options.quiet && !Options.banner_printed ) {
        Options.banner_printed = TRUE;
        trademark();
    }
    open_files();
    PushLineQueue();
}

int main( int argc, char *argv[] )
/********************************/
{
    main_init();
    do_init_stuff( argc, argv );

    WriteObjModule();           // main body: parse the source file

    if( !Options.quiet ) {
        PrintStats();
    }
    MsgFini();
    main_fini();
    return( (int)Options.error_count ); /* zero if no errors */
}

static void usage_msg( void )
/***************************/
{
    PrintfUsage( MSG_USE_BASE );
    exit(1);
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
/***************************/
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

#ifdef TRMEM
    MemInit();
#endif
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

static void get_fname( char *token, int type )
/********************************************/
/*
 * figure out the source file name & store it in AsmFiles
 * fill in default object file name if it is null
 */
{
    char        *def_drive, *def_dir, *def_fname, *def_ext;
    char        *drive, *dir, *fname, *ext;
    char        buffer[ _MAX_PATH2 ];
    char        buffer2[ _MAX_PATH2 ];
    char        name [ _MAX_PATH  ];
    char        msgbuf[80];

    /* get filename for source file */

    if( type == ASM ) {
        if( token == NULL ) {
            MsgGet( SOURCE_FILE, msgbuf );
            Fatal( MSG_CANNOT_OPEN_FILE, msgbuf );
        }
        if( AsmFiles.fname[ASM] != NULL ) {
            Fatal( MSG_TOO_MANY_FILES );
        }

        _splitpath2( token, buffer, &drive, &dir, &fname, &ext );
        if( *ext == '\0' ) {
            ext = ASM_EXT;
        }
        _makepath( name, drive, dir, fname, ext );
        AsmFiles.fname[ASM] = AsmAlloc( strlen( name ) + 1 );
        strcpy( AsmFiles.fname[ASM], name );

        _makepath( name, drive, dir, NULL, NULL );
        /* add the source path to the include path */
        AddStringToIncludePath( name );

        if( AsmFiles.fname[OBJ] == NULL ) {
            /* set up default object and error filename */
            ext = OBJ_EXT;
            _makepath( name, NULL, NULL, fname, ext );
        } else {
            _splitpath2( AsmFiles.fname[OBJ], buffer2, &def_drive,
                         &def_dir, &def_fname, &def_ext );
            if( *def_fname == NULLC ) def_fname = fname;
            if( *def_ext == NULLC ) def_ext = OBJ_EXT;

            _makepath( name, def_drive, def_dir, def_fname, def_ext );
            AsmFree( AsmFiles.fname[OBJ] );
        }
        AsmFiles.fname[OBJ] = AsmAlloc( strlen( name ) + 1 );
        strcpy( AsmFiles.fname[OBJ], name );

        if( AsmFiles.fname[ERR] == NULL ) {
            ext = ERR_EXT;
            _makepath( name, NULL, NULL, fname, ext );
        } else {
            _splitpath2( AsmFiles.fname[ERR], buffer2, &def_drive,
                         &def_dir, &def_fname, &def_ext );
            if( *def_fname == NULLC ) def_fname = fname;
            if( *def_ext == NULLC ) def_ext = ERR_EXT;
            _makepath( name, def_drive, def_dir, def_fname, def_ext );
            AsmFree( AsmFiles.fname[ERR] );
        }
        AsmFiles.fname[ERR] = AsmAlloc( strlen( name ) + 1 );
        strcpy( AsmFiles.fname[ERR], name );

    } else {
        /* get filename for object file */
        _splitpath2( token, buffer, &drive, &dir, &fname, &ext );
        if( AsmFiles.fname[ASM] != NULL ) {
            _splitpath2( AsmFiles.fname[ASM], buffer2, &def_drive,
                         &def_dir, &def_fname, &def_ext );
            if( *fname == NULLC ) {
                fname = def_fname;
            }
        }
        if( *ext == NULLC ) {
            ext = type == ERR ? ERR_EXT : OBJ_EXT;
        }
        _makepath( name, drive, dir, fname, ext );
        AsmFiles.fname[type] = AsmAlloc( strlen( name ) + 1 );
        strcpy( AsmFiles.fname[type], name );
    }
}

static void do_envvar_cmdline( char *envvar, int level )
/******************************************************/
{
    #define MAX_NESTING 10
    #define IS_SPACECHAR( ch )  ( ch == ' ' || ch == '\t' )
    #define IS_SPACE_OR_NULL( ch )      ( IS_SPACECHAR( ch ) || ch == '\0' )

    char *cmdline;
    char *token; /* current token */
    char *next;  /* next token */

    if( level >= MAX_NESTING ) {
        return;
    }

    cmdline = getenv( envvar );
    if( cmdline == NULL ) return;
    /* now handle the cmdline in the envvar 1 token at a time */

    while( IS_SPACE_OR_NULL( *cmdline ) ) cmdline++;
    token = cmdline;
    next = cmdline;
    while( *token != '\0' ) {
        while( !IS_SPACE_OR_NULL( *next ) ) next++;
        if( *next == '\0' ) {
            /* last token */
            parse_token( token, level + 1 );
            break;
        }
        *next = '\0'; /* break string after 1st token */
        next++;
        parse_token( token, level + 1 );
        while( IS_SPACECHAR( *next ) ) next++;
        if( *next == '\0' ) break;
        token=next;
    }
    return;
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
    return;
}

static char * set_processor_type(char *input)
/*******************************************/
{
    enum asm_token      token;
    char                protect = FALSE;
    char                *tmp = input+1;

    for( tmp=input+1; *tmp != '\0' && !isspace( *tmp ); tmp++ ) {
        if( *input == 'f' ) {
            tmp = input + strcspn( input, "/-" );
            break;
        } else if( *tmp == '/' || *tmp == '-' ) {
            break;
        } else if( *tmp == 'r' ) {
            Options.naming_convention = ADD_USCORES;
            add_constant( "__REGISTER__" );
        } else if( *tmp == 's' ) {
            add_constant( "__STACK__" );
            Options.naming_convention = DO_NOTHING;
        } else if( *tmp == '_' ) {
            if( Options.naming_convention == DO_NOTHING ) {
                Options.naming_convention = REMOVE_USCORES;
            } else {
                Options.naming_convention = DO_NOTHING;
            }
        } else if( *tmp == 'p' ) {
            protect = TRUE;
        } else if( *tmp == '"' ) {
            char *dest;
            tmp++;
            Options.default_name_mangler = AsmAlloc( strlen( tmp ) + 1 );
            dest = Options.default_name_mangler;
            for( ; *tmp != '"'; dest++, tmp++ ) {
                *dest = *tmp;
            }
            *dest = NULLC;
        } else {
            MsgPrintf1( MSG_UNKNOWN_OPTION, input );
            exit( 1 );
        }
    }

    switch( *input ) {
    case '0':
        token = T_8086;
        break;
    case '1':
        token = T_186;
        break;
    case '2':
        token =  protect ? T_286P : T_286;
        break;
    case '3':
        token =  protect ? T_386P : T_386;
        break;
    case '4':
        token =  protect ? T_486P : T_486;
        break;
    case '5':
        token =  protect ? T_586P : T_586;
        break;
    case '6':
        token =  protect ? T_686P : T_686;
        break;
    case '7':
        switch( Code->info.cpu & P_CPU_MASK ) {
        case P_286:
            token =  T_287;
            break;
        case P_386:
        case P_486:
        case P_586:
        case P_686:
            token =  T_387;
            break;
        case P_86:
        case P_186:
        default:
            token =  T_8087;
            break;
        }
        break;
    case 'f':       // fp#
        switch( *(input+2) ) {
        case 'c':
            token = T_NO87;
            break;
        case '0':
            token = T_8087;
            break;
        case '2':
            token = T_287;
            break;
        case '3':
        case '5':
            token = T_387;
            break;
        }
        break;
    /* no other cases are possible */
    }
    cpu_directive( token );

    return( tmp );
}

static int set_build_target( void )
/*********************************/
{
    char *tmp;
    char *uscores = "__";

    if( Options.build_target == NULL ) {
        #define MAX_OS_NAME_SIZE 7
        Options.build_target = AsmAlloc( MAX_OS_NAME_SIZE + 1 );
        #if defined(__OSI__)
            if( __OS == OS_DOS ) {
                strcpy( Options.build_target, "DOS" );
            } else if( __OS == OS_OS2 ) {
                strcpy( Options.build_target, "OS2" );
            } else if( __OS == OS_NT ) {
                strcpy( Options.build_target, "NT" );
            } else if( __OS == OS_WIN ) {
                strcpy( Options.build_target, "WINDOWS" );
            } else {
                strcpy( Options.build_target, "XXX" );
            }
        #elif defined(__QNX__)
            strcpy( Options.build_target, "QNX" );
        #elif defined(__DOS__)
            strcpy( Options.build_target, "DOS" );
        #elif defined(__OS2__)
            strcpy( Options.build_target, "OS2" );
        #elif defined(__NT__)
            strcpy( Options.build_target, "NT" );
        #else
            #error unknown host OS
        #endif
    }

    strupr( Options.build_target );
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
    }
    return( NOT_ERROR );
}

static void set_mem_type( char mem_type )
/***************************************/
{
    char buffer[20];
    char *model;

    switch( mem_type ) {
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
        strcpy( buffer, "/m" );
        strcat( buffer, &mem_type );
        MsgPrintf1( MSG_UNKNOWN_OPTION, buffer );
        exit( 1 );
    }

    strcpy( buffer, ".MODEL " );
    strcat( buffer, model );
    InputQueueLine( buffer );

    return;
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
    *tmp = AsmAlloc( len );
    strcpy( *tmp, name );
    return;
}

static void parse_token( char *token, int nesting_level )
/*******************************************************/
{
    char *ptr;
    int_8 len;

    for( ;; ) {
        ptr = NULL;
        switch( *token ) {
        case '?':
            usage_msg();
            break;
        case '=':
        case '#':
            AsmError( SPACES_NOT_ALLOWED_IN_COMMAND_LINE_OPTIONS );
            break;
    #ifndef __QNX__
        case '/':
    #endif
        case '-':
            /* options */
            token++;
            switch( tolower( *token ) ) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '7':
                ptr = set_processor_type( token );
                break;
            case 'b':
                if( *(token+1) == 't' ) {
                    token += 2;
                    if( IS_EQ_CHAR( *token ) ) token++;
                    if( Options.build_target != NULL ) {
                        AsmFree( Options.build_target );
                    }
                    Options.build_target = AsmAlloc( strlen( token ) + 1 );
                    strcpy( Options.build_target, token );
                    break;
                }
                MsgPrintf1( MSG_UNKNOWN_OPTION, token );
                exit( 1 );
            case 'c':
                Options.output_data_in_code_records = FALSE;
                ptr = token+1;
                break;
            case 'h':
                switch( token[1] ) {
                case 'c':
                case 'd':
                case 'w':
                    ptr = token + 2;
                    break;
                default:
                    MsgPrintf1( MSG_UNKNOWN_OPTION, token );
                    exit( 1 );
                }
                break;
            case 'm':
                set_mem_type( *(token+1) );
                ptr = token+2;
                break;
            case 'n':
                if( IS_EQ_CHAR( *(token+2) ) ) {
                    set_some_kinda_name( *(token+1), token+3 );
                } else {
                    set_some_kinda_name( *(token+1), token+2 );
                }
                break;
            case '?':
                usage_msg();
                break;
            case 'i':
                /* set include path */
                token++;
                if( IS_EQ_CHAR( *token ) ) token++;
                AddStringToIncludePath( token );
                break;
            case 'e':
                if( isdigit( *(token+1) ) ) {
                    Options.error_limit = (char)atoi( token+1 );
                } else if( *(token+1) == '\0' ) {
                    /* stop reading asm file at the END directive */
                    Options.stop_at_end = TRUE;
                }
                ptr = token + 1 + strcspn( token+1, "/-" );
                break;
            case 'w':
                if( isdigit( *(token+1) ) ) {
                    Options.warning_level = (char)atoi( token+1 );
                } else if( *(token+1) == 'e' ) {
                    Options.warning_error = TRUE;
                }
                ptr = token + 1 + strcspn( token+1, "/-" );
                break;
            case 'j':
            case 's':
                /* force use of sbyte, sword, sdword etc. for signed values */
                Options.sign_value = TRUE;
                ptr = token+1;
                break;
            case 'o':
                Options.allow_c_octals = TRUE;
                ptr = token+1;
                break;
            case 'd':
                switch( *(token+1) ) {
                case '+':
                    /* just ignore it */
                    break;
                case '0':
                    Options.debug_flag = FALSE;
                    ptr = token+1;
                    break;
                case '1':
                case '2':
                case '3':
                    Options.debug_flag = TRUE;
                    ptr = token + 1 + strcspn( token+1, "/-" );
                    break;
                    // make d2 different sometime
    #ifdef DEBUG_OUT
                case '6':
                    Options.debug = TRUE;
                    DebugMsg(( "debugging output on \n" ));
                    break;
    #endif
                default:
                    add_constant( token+1 );
                }
                break;
            case 'f':
                switch( *(token+1) ) {
                case 'i':
                    /* force file following to be included */
                    token += 2;
                    if( IS_EQ_CHAR( *token ) ) token++;
                    InputQueueFile( token );
                    break;
                case 'o':
                    token += 2;
                    if( IS_EQ_CHAR( *token ) ) token++;
                    get_fname( token, OBJ );
                    break;
                case 'r':
                case 'e': /* for backwards compatablity */
                    /* use this as the error file, default fname.err */
                    token += 2;
                    if( IS_EQ_CHAR( *token ) ) token++;
                    get_fname( token, ERR ); /* +3 to get past fe= */
                    break;
                case 'p':
                    ptr = token + 1 + strcspn( token+1, "/-" );
                    len = ptr - token;
                    if( strnicmp( token, "fpi", len ) == 0 ) {
                        Options.floating_point = DO_FP_EMULATION;
                        add_constant("__FPI__");
                        set_processor_type( "7" );
                        break;
                    } else if( strnicmp( token, "fpi87", len ) == 0 ) {
                        Options.floating_point = NO_FP_EMULATION;
                        add_constant("__FPI87__");
                        set_processor_type( "7" );
                        break;
                    } else if( strnicmp( token, "fpc", len ) == 0 ) {
                        Options.floating_point = NO_FP_ALLOWED;
                        add_constant("__FPC__");
                        set_processor_type( token );
                        break;
                    } else {
                        switch( *(token+2) ) {
                        case '0':
                        case '2':
                        case '3':
                        case '5':
                            ptr = set_processor_type( token );
                        }
                    }
                    break;
                default:
                    MsgPrintf1( MSG_UNKNOWN_OPTION, token );
                    exit( 1 );
                }
                break;
            case 'q':
                Options.quiet = TRUE;
                ptr = token + 1;
                break;
            case 'u':
                /* undefine macro - ignore for now */
                break;
            case 'z':
                switch( *(token+1) ) {
                case 'q':
                    Options.quiet = TRUE;
                    ptr = token + 2;
                    break;
                default:
                    MsgPrintf1( MSG_UNKNOWN_OPTION, token );
                    exit( 1 );
                }
                break;
            default:
                MsgPrintf1( MSG_UNKNOWN_OPTION, token );
                exit( 1 );
            }
            break;
        case '@':
            do_envvar_cmdline( token+1, nesting_level );
            break;
        default:
            /* must be a filename */
            get_fname( token, ASM );
            break;
        }
        if( ptr == NULL ) break;
        switch( *ptr ) {
        #ifndef __QNX__
            case '/':
        #endif
        case '-':
            break;
        default:
            return;
        }
        token = ptr;
    }
}

static void parse_cmdline( int argc, char *argv[] )
/*************************************************/
{
    int  i;
    char msgbuf[80];

    if( argc == 1 ) {
        usage_msg();
    }
    for( i = 1; i < argc; i++ ) {
        parse_token( argv[i], 0 );
    }
    if( AsmFiles.fname[ASM] == NULL ) {
        MsgGet( NO_FILENAME_SPECIFIED, msgbuf );
        Fatal( MSG_CANNOT_OPEN_FILE, msgbuf );
    }
}

/* The following are functions needed by the original stand-alone assembler */

extern enum sym_state   AsmQueryExternal( char *name )
{
    name = name;
    return SYM_UNDEFINED;
}

extern enum sym_type    AsmQueryType( char *name )
{
    name = name;
    return SYM_INT1;
}

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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "as.h"
#include <ctype.h>
#include "preproc.h"
#include "banner.h"
#include "iopath.h"
#include "options.h"


as_flags        AsOptions = 0;

static const char   *switch_start = "";

static void BadCmdLine( int error_code )
/***************************************
 * SIGNAL CMD-LINE ERROR
 */
{
    char        buffer[128];
    size_t      len;

    CmdScanChar();
    while( !CmdScanSwEnd() ) {
        CmdScanChar();
    }
    len = CmdScanAddr() - switch_start;
    if( len > sizeof( buffer ) - 1 )
        len = sizeof( buffer ) - 1;
    strncpy( buffer, switch_start, len );
    buffer[len] = '\0';
    Banner();
    AsOutMessage( stderr, AS_MSG_ERROR );
    AsOutMessage( stderr, error_code, buffer );
    fputc( '\n', stderr );
}

// BAD CHAR DETECTED
void BadCmdLineChar( void )
{
    BadCmdLine( INVALID_OPTION );
}
// BAD ID DETECTED
void BadCmdLineId( void )
{
    BadCmdLine( INVALID_OPTION );
}
// BAD NUMBER DETECTED
void BadCmdLineNumber( void )
{
    BadCmdLine( INVALID_OPTION );
}
// BAD PATH DETECTED
void BadCmdLinePath( void )
{
    BadCmdLine( INVALID_OPTION );
}
// BAD FILE DETECTED
void BadCmdLineFile( void )
{
    BadCmdLine( INVALID_OPTION );
}
// BAD TEXT DETECTED
void BadCmdLineOption( void )
{
    BadCmdLine( INVALID_OPTION );
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
        env = MemAlloc( len + 1 );
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

static bool scanDefine( OPT_STRING **h )
/**************************************/
{
    char        *m;
    char        *p;

    if( OPT_GET_FILE( h ) ) {
        m = (*h)->data;
        p = strchr( m, '=' );
        if( p != NULL ) {
            *p = ' ';
        } else {
            *h = MemRealloc( *h, sizeof( **h ) + strlen( m ) + 2 );
            strcat( (*h)->data, " 1" );
        }
        return( true );
    }
    return( false );
}

static void ppDefine( const OPT_STRING *s )
//*****************************************
{
    if( s->next != NULL ) {
        ppDefine( s->next );
    }
    PP_Define( s->data );
}

void OptionsPPDefine( OPT_STORAGE *data )
//***************************************
{
#ifdef AS_ALPHA
    PP_Define( "__WASAXP__ " _MACROSTR( _BLDVER ) );
#elif defined( AS_PPC )
    PP_Define( "__WASPPC__ " _MACROSTR( _BLDVER ) );
#elif defined( AS_MIPS )
    PP_Define( "__WASMPS__ " _MACROSTR( _BLDVER ) );
#endif
    if( data->d ) {
        ppDefine( data->d_value );
    }
}

static void ppInclude( const OPT_STRING *s )
//******************************************
{
    if( s->next != NULL ) {
        ppInclude( s->next );
    }
    PP_IncludePathAdd( PPINCLUDE_USR, s->data );
}

void OptionsPPInclude( OPT_STORAGE *data )
//****************************************
{
    PP_IncludePathAdd( PPINCLUDE_SYS, PP_GetEnv( "INCLUDE" ) );
    if( data->i ) {
        ppInclude( data->i_value );
    }
}

#include "cmdlnprs.gc"

static int ProcOptions( OPT_STORAGE *data, const char *str, OPT_STRING **files )
/******************************************************************************/
{
#define MAX_NESTING 32
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
                MemFree( buffers[level] );
                CmdScanLineInit( save[level] );
                level--;
                continue;
            }
            if( CmdScanSwitchChar( ch ) ) {
                switch_start = CmdScanAddr() - 1;
                OPT_PROCESS( data );
            } else {  /* collect file name */
                CmdScanUngetChar();
                switch_start = CmdScanAddr();
                OPT_GET_FILE( files );
            }
        }
    }
    return( 0 );
#undef MAX_NESTING
}

void OptionsFini( void )
//**********************
{
}


bool OptionsInit( int argc, char **argv, OPT_STORAGE *data, OPT_STRING **files )
//******************************************************************************
{
    int         i;

    for( i = 0; i < argc; i++ ) {
        ProcOptions( data, argv[i], files );
    }
    if( data->_question ) {
        _SetOption( PRINT_HELP );
    }
    switch( data->debug ) {
    case OPT_ENUM_debug_d0:
        DebugLevel = 0;
        break;
    case OPT_ENUM_debug_d1:
        DebugLevel = 1;
        break;
    case OPT_ENUM_debug_d2:
        DebugLevel = 2;
        break;
    case OPT_ENUM_debug_d3:
        DebugLevel = 3;
        break;
    case OPT_ENUM_debug_default:
    default:
        break;
    }
    switch( data->format ) {
    case OPT_ENUM_format_oc:
        _UnsetOption( OBJ_ELF );
        _SetOption( OBJ_COFF );
        break;
    case OPT_ENUM_format_oe:
        _SetOption( OBJ_ELF );
        _UnsetOption( OBJ_COFF );
        break;
    case OPT_ENUM_format_default:
    default:
        break;
    }
    if( data->w ) {
        WarningLevel = data->w_value;
    }
    if( data->we ) {
        _SetOption( WARNING_ERROR );
    }
    if( data->e ) {
        ErrorLimit = data->e_value;
    }
    if( data->q || data->zq ) {
        _SetOption( BE_QUIET );
    }
    if( data->fo ) {
        ObjSetObjFile( data->fo_value->data );
    }
    if( !data->fr ) {
        /*
         * if not -fr option is defined
         * then create default "*"
         */
        data->fr_value = MemAlloc( sizeof( OPT_STRING ) + 1 );
        data->fr_value->data[0] = '*';
        data->fr_value->data[1] = '\0';
        data->fr_value->next = NULL;
        data->fr = true;
    }
#ifdef AS_DEBUG_DUMP
    switch( data->dump ) {
    case OPT_ENUM_dump_vi:
        _SetOption( DUMP_INSTRUCTIONS );
        break;
    case OPT_ENUM_dump_vp:
        _SetOption( DUMP_PARSE_TREE );
        break;
    case OPT_ENUM_dump_vt:
        _SetOption( DUMP_INS_TABLE );
        break;
    case OPT_ENUM_dump_vs:
        _SetOption( DUMP_SYMBOL_TABLE );
        break;
    case OPT_ENUM_dump_vl:
        _SetOption( DUMP_LEXER_BUFFER );
        break;
    case OPT_ENUM_dump_vd:
        _SetOption( DUMP_DEBUG_MSGS );
        break;
    case OPT_ENUM_dump_default:
    default:
        break;
    }
#endif
    return( true );
}

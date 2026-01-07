/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Librarian command line parsing (WLIB mode).
*
****************************************************************************/


#include "wlib.h"
#include <errno.h>
#include "wio.h"
#include "cmdlinew.h"
#include "cmdline.h"
#include "cmdlnprs.h"
#include "cmdscan.h"

#include "clibext.h"


#define READ_BUFFER_SIZE    512

typedef struct {
    FILE    *fp;
    char    *buffer;
    size_t  size;
} getline_data;

#include "cmdlprsw.gc"

static void SetPageSize( unsigned_16 new_size )
{
    unsigned int i;

    Options.page_size = MIN_PAGE_SIZE;
    for( i = 4; i < 16; i++ ) {
        if( new_size & (1 << i) ) {
            Options.page_size = 1 << i;
        }
    }
    if( Options.page_size < new_size ) {
        Options.page_size <<= 1;
    }
}

static void ParseCommand( void )
{
    operation       ops = 0;

    switch( CmdScanChar() ) {
    case '-':
        ops = OP_DELETE;
        if( CmdRecogChar( '+' ) ) {
            ops |= OP_ADD;
            break;
        }
#if defined(__UNIX__)
        if( CmdRecogChar( ':' ) ) {
#else
        if( CmdRecogChar( '*' ) ) {
#endif
            ops |= OP_EXTRACT;
            break;
        }
        break;
    case '+':
        ops = OP_ADD;
        if( CmdRecogChar( '+' ) ) {
            ops |= OP_IMPORT;
            break;
        }
        if( CmdRecogChar( '-' ) ) {
            ops |= OP_DELETE;
            break;
        }
        break;
#if defined(__UNIX__)
    case ':':
#else
    case '*':
#endif
        ops |= OP_EXTRACT;
        if( CmdRecogChar( '-' ) ) {
            ops |= OP_DELETE;
            break;
        }
        break;
    default:
        BadCmdLineOption();
    }
    CmdScanSkipWhiteSpace();
    AddCommand( ops );
}

static void my_getline_init( const char *name, getline_data *fd )
{
    fd->fp = fopen( name, "rb" );
    if( fd->fp == NULL ) {
        FatalError( ERR_CANT_OPEN, name, strerror( errno ) );
    }
    fd->size = READ_BUFFER_SIZE;
    fd->buffer = MemAlloc( READ_BUFFER_SIZE );
    if( fd->buffer == NULL ) {
        fd->size = 0;
    }
}

static char *my_getline( getline_data *fd )
{
    char    *p;
    size_t  len_used;
    size_t  maxlen;
    size_t  len;

    p = fd->buffer;
    maxlen = fd->size;
    len_used = 0;
    while( fgets( p, maxlen, fd->fp ) != NULL ) {
        len = strlen( p );
        if( len == 0 )
            continue;
        len_used += len;
        if( p[len - 1] == '\n' ) {
            break;
        }
        if( len < maxlen - 1 ) {
            break;
        }
        if( feof( fd->fp ) ) {
            break;
        }
        len = fd->size + READ_BUFFER_SIZE;
        p = MemAlloc( len );
        memcpy( p, fd->buffer, len_used + 1 );
        MemFree( fd->buffer );
        fd->buffer = p;
        fd->size = len;
        p += len_used;
        maxlen = len - len_used;
    }
    if( len_used ) {
        p = fd->buffer;
        if( p[len_used - 1] == '\n' ) {
            len_used--;
            p[len_used] = '\0';
        }
        if( len_used
          && p[len_used - 1] == '\r' ) {
            len_used--;
            p[len_used] = '\0';
        }
        return( p );
    }
    return( NULL );
}

static void my_getline_fini( getline_data *fd )
{
    fclose( fd->fp );
    MemFree( fd->buffer );
}

void ParseOneLineWlib( const char *cmd, OPT_STORAGE_W *data, bool comment )
{
    const char  *old_cmd;
    const char  *begcmd;
    char        *p;

    old_cmd = CmdScanLineInit( cmd );
    for( ;; ) {
        CmdScanSkipWhiteSpace();
        option_start = CmdScanAddr();
        switch( CmdPeekChar() ) {
#if !defined(__UNIX__)
        case '/':
            CmdScanChar();              /* skip '/' character */
            if( OPT_PROCESS_W( data ) )
                FatalError( ERR_BAD_OPTION, option_start[1] );
            break;
#endif
        case '-':
            if( CmdList == NULL
              && Options.input_name == NULL ) {
                CmdScanChar();          /* skip '-' character */
                if( !OPT_PROCESS_W( data ) ) {
                    break;
                }
                CmdScanLineInit( option_start );
            }
            /* fall through */
#if !defined(__UNIX__)
        case '*':
#else
        case ':':
#endif
        case '+':
            ParseCommand();
            break;
        case '@':
            CmdScanChar();              /* skip '@' character */
            CmdScanSkipWhiteSpace();
            begcmd = CmdScanAddr();
            p = GetFilename();
            if( p != NULL ) {
                const char  *env;

                env = WlibGetEnv( p );
                MemFree( p );
                if( env != NULL ) {
                    ParseOneLineWlib( env, data, false );
                    break;
                }
            }
            CmdScanLineInit( begcmd );
            p = GetFilenameExt( EXT_CMD );
            if( p != NULL ) {
                getline_data    fd;

                my_getline_init( p, &fd );
                MemFree( p );
                while( (p = my_getline( &fd )) != NULL ) {
                    ParseOneLineWlib( p, data, true );
                }
                my_getline_fini( &fd );
                break;
            }
            CmdScanLineInit( begcmd );
            break;
        case '\0':
            CmdScanLineInit( old_cmd );
            return;
        case '#':
            if( comment ) {
                /*
                 * comment - blow away line
                 */
                CmdScanLineInit( old_cmd );
                return;
            }
            /* fall through */
        default:
            if( Options.input_name == NULL ) {
                Options.input_name = GetFilenameExt( EXT_LIB );
            } else {
                AddCommand( OP_ADD | OP_DELETE );
            }
            break;
        }
    }
}

void SetOptionsWlib( OPT_STORAGE_W *data )
{
    /*
     * q - don't print header
     * v - verbosed output      (default)
     */
    if( data->q
      && data->v ) {
        if( data->q_timestamp > data->v_timestamp ) {
            Options.quiet = true;
        }
    } else if( data->q ) {
        Options.quiet = true;
    }
    if( data->_question ) {
        Usage();
    }
    if( data->b ) { //                       (don't create .bak file)
        Options.no_backup = true;
    }
    if( data->c ) { //                       (case sensitive)
        Options.respect_case = true;
    }
    if( data->d ) { // = <object_output_directory>
        Options.output_directory = MemDupStr( data->d_value->data );
        OPT_CLEAN_STRING( &(data->d_value) );
        if( access( Options.output_directory, F_OK ) != 0 ) {
            FatalError( ERR_DIR_NOT_EXIST, Options.output_directory );
        }
    }
    switch( data->libformat ) {
    case OPT_ENUM_W_libformat_fab:
        Options.ar_libformat = AR_FMT_BSD;
        data->libtype = OPT_ENUM_W_libtype_fa;
        break;
    case OPT_ENUM_W_libformat_fac:
        Options.ar_libformat = AR_FMT_COFF;
        data->libtype = OPT_ENUM_W_libtype_fa;
        break;
    case OPT_ENUM_W_libformat_fag:
        Options.ar_libformat = AR_FMT_GNU;
        data->libtype = OPT_ENUM_W_libtype_fa;
        break;
    }
    switch( data->libtype ) {
    case OPT_ENUM_W_libtype_fa:
        Options.libtype = WL_LTYPE_AR;
        Options.coff_found = true;
        break;
    case OPT_ENUM_W_libtype_fm:
        Options.libtype = WL_LTYPE_MLIB;
        Options.elf_found = true;
        break;
    case OPT_ENUM_W_libtype_fo:
        Options.libtype = WL_LTYPE_OMF;
        Options.omf_found = true;
        break;
    }
    switch( data->processor ) {
    case OPT_ENUM_W_processor_i6:
        Options.processor = WL_PROC_X64;
        break;
    case OPT_ENUM_W_processor_ia:
        Options.processor = WL_PROC_AXP;
        break;
    case OPT_ENUM_W_processor_ii:
        Options.processor = WL_PROC_X86;
        break;
    case OPT_ENUM_W_processor_im:
        Options.processor = WL_PROC_MIPS;
        break;
    case OPT_ENUM_W_processor_ip:
        Options.processor = WL_PROC_PPC;
        break;
    }
    switch( data->filetype ) {
    case OPT_ENUM_W_filetype_icl:
        Options.coff_import_long = true;
        /* fall through */
    case OPT_ENUM_W_filetype_ic:
        Options.filetype = WL_FTYPE_COFF;
        break;
    case OPT_ENUM_W_filetype_ie:
        Options.filetype = WL_FTYPE_ELF;
        break;
    case OPT_ENUM_W_filetype_io:
        Options.filetype = WL_FTYPE_OMF;
        break;
    }
    if( data->non_resident == OPT_ENUM_W_non_resident_ino ) {
        Options.nr_ordinal = true;
    }
    if( data->resident == OPT_ENUM_W_resident_iro ) {
        Options.r_ordinal = true;
    }
    if( data->l ) { // [ = <list_file_name> ]
        Options.list_contents = true;
        Options.list_file = CopyFilenameExt( data->l_value, EXT_LST );
        OPT_CLEAN_STRING( &(data->l_value) );
    }
    if( data->m ) { //                       (display C++ mangled names)
        Options.mangled = true;
    }
    if( data->n ) { //                       (always create a new library)
        Options.new_library = true;
    }
    if( data->o ) { // = <out_library_name>
        Options.output_name = CopyFilenameExt( data->o_value, EXT_LIB );
        OPT_CLEAN_STRING( &(data->o_value) );
    }
    switch( data->page_size ) {
    case OPT_ENUM_W_page_size_pa:
        Options.page_size = (unsigned_16)-1;
        break;
    case OPT_ENUM_W_page_size_p:
        SetPageSize( (unsigned_16)data->p_value );
        break;
    }
    if( data->s ) {
        Options.strip_line = true;
    }
    if( data->t ) {
        Options.trim_path = true; //(trim THEADR pathnames)
    }
    if( data->tl ) {
        Options.list_contents = true;
        Options.terse_listing = true; // (internal terse listing option)
    }
    if( data->x ) { //                       (explode all objects in library)
        Options.explode = true;
#ifdef DEVBUILD
        Options.explode_count = 0;
        if( CmdRecogLowerChar( 'n' ) ) {
            Options.explode_count = 1;
        }
        if( Options.explode_count ) {
            char    cn[20] = FILE_TEMPLATE_MASK;
            strcpy( cn + sizeof( FILE_TEMPLATE_MASK ) - 1, data->x_value->data );
            Options.explode_ext = MemDupStr( cn );
        } else {
            Options.explode_ext = CopyFilenameExt( data->x_value, EXT_OBJ );
        }
        OPT_CLEAN_STRING( &(data->x_value) );
#else
        Options.explode_ext = CopyFilename( data->x_value );
        OPT_CLEAN_STRING( &(data->x_value) );
#endif
    }
    if( data->z ) {
        Options.strip_expdef = true;
        Options.export_list_file = CopyFilename( data->z_value );
        OPT_CLEAN_STRING( &(data->z_value) );
    }
    if( data->zld ) {
        Options.strip_dependency = true; //(strip dependency info)
    }
    if( data->zll ) {
        Options.strip_library = true;  //(strip library info)
    }

    if( CmdList == NULL ) {
        if( !Options.list_contents
          && !Options.explode
          && !Options.new_library ) {
            /*
             * Default action: List the input lib
             */
            if( Options.output_name == NULL ) {
                Options.list_contents = true;
                Options.list_file = MemDupStr( "" );
            } else {
                /*
                 * Or copy it to the output lib
                 */
                Options.modified = true;
            }
        }
    }
}

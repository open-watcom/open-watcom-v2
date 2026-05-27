/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Access data bound into editor executable.
*
****************************************************************************/


#include "vi.h"
#include "sopen.h"
#include "posix.h"
#include "bnddata.h"
#include "specio.h"
#include "myio.h"
#include "tokenize.h"

#include "clibext.h"


static TYPE_BIND    *files_data_offs = NULL;
static TYPE_BIND    *files_data_lines = NULL;
static char         *files_data = NULL;
static long         bound_data_start_pos;

/*
 * BoundDataInit - check if data is bound to our exe
 */
void BoundDataInit( void )
{
    int         h;
    size_t      size;
    char        buff[TRAILER_SIZE];
    char        *p;
    int         files_count;
    size_t      files_data_len;

    /*
     * get trailer
     */
    h = _sopen3( EXEName, O_RDONLY | O_BINARY, SH_COMPAT );
    if( h == -1 ) {
        return;
    }
    lseek( h, SEEK_POSBACK( TRAILER_SIZE ), SEEK_END );
    read( h, buff, TRAILER_SIZE );

    /*
     * seek to start of data
     */
    if( memcmp( buff, MAGIC_COOKIE, MAGIC_COOKIE_SIZE ) ) {
        close( h );
        return;
    }
    size = GET_MAGIC_SIZE( buff );
    bound_data_start_pos = SEEK_POSBACK( size + TRAILER_SIZE );
    lseek( h, bound_data_start_pos, SEEK_END );

    /*
     * get everything
     */
    BndMemory = p = _MemAllocArraySafe( char, size );
    read( h, BndMemory, size );
    close( h );

    /*
     * get number of files, and get space to store data
     */
    files_count = GET_SIZE( p );
    p += SIZE_BIND;
    files_data_offs = _MemAllocArraySafe( TYPE_BIND, files_count );
    files_data_lines = _MemAllocArraySafe( TYPE_BIND, files_count );

    /*
     * get file names
     */
    files_data_len = GET_SIZE( p );
    p += SIZE_BIND;
    files_data = _MemAllocArraySafe( char, files_data_len );
    memcpy( files_data, p, files_data_len );
    p += files_data_len;

    /*
     * copy over files data offsets and lines count data
     */
    size = files_count * SIZE_BIND;
    memcpy( files_data_offs, p, size );
    memcpy( files_data_lines, p + size, size );

    BoundData = true;

} /* BoundDataInit */

/*
 * BoundDataFini - check if data is bound to our exe
 */
void BoundDataFini( void )
{
    MemFree( BndMemory );
    MemFree( files_data_offs );
    MemFree( files_data_lines );
    MemFree( files_data );

} /* BoundDataFini */

/*
 * SpecialOpen - open a file or exe
 */
bool SpecialOpen( const char *fn, GENERIC_FILE *gf, bool bounddata )
{
    long            shift = 0;
    int             h, i;
    unsigned char   len;
    vi_rc           rc;

    /*
     * process bound file
     */
    if( BoundData ) {

        if( strcmp( fn, CONFIG_FILE ) == 0 ) {
            i = 0;
        } else {
            i = Tokenize( files_data, fn, true );
        }
        if( i != TOK_INVALID && bounddata ) {

            shift = files_data_offs[i];
            gf->type = GF_BOUND;
            EditFlags.BndMemoryLocked = true;

            if( BndMemory == NULL ) {
                h = _sopen3( EXEName, O_RDONLY | O_BINARY, SH_COMPAT );
                if( h == -1 ) {
                    return( false );
                }
                /*
                 * bound_data_start_pos + shift is always negative or zero
                 */
                lseek( h, bound_data_start_pos + shift, SEEK_END );
                read( h, &len, 1 );
                gf->data.handle = h;
            } else {
                gf->data.pos = BndMemory + shift;
                len = gf->data.pos[0];
                gf->data.pos++;
            }
            gf->gf.a.currline = 0;
            gf->gf.a.maxlines = files_data_lines[i];
            gf->gf.a.length = len;
            return( true );

        }

    }

    /*
     * special case - open current buffer
     */
    if( fn[0] == '.' && fn[1] == '\0' ) {
        gf->type = GF_BUFFER;
        gf->data.cfile = CurrentFile;
        rc = GimmeLinePtr( 1, CurrentFile, &(gf->gf.b.cfcb), &(gf->gf.b.cline));
        return( rc == ERR_NO_ERR );
    }

    /*
     * process regular file
     */
    gf->type = GF_FILE;
    gf->data.fp = GetFromEnvAndOpen( fn );
    return( gf->data.fp != NULL );

} /* SpecialOpen */

/*
 * SpecialFclose - close either file or exe
 */
void SpecialFclose( GENERIC_FILE *gf )
{
    switch( gf->type ) {
    case GF_FILE:
        fclose( gf->data.fp );
        break;
    case GF_BOUND:
        EditFlags.BndMemoryLocked = false;
        if( BndMemory == NULL ) {
            close( gf->data.handle );
        }
        break;
    }

} /* SpecialFclose */

/*
 * SpecialFgets - get from either file or exe
 */
char *SpecialFgets( char *buff, int max_len, GENERIC_FILE *gf )
{
    size_t      len;
    vi_rc       rc;

    switch( gf->type ) {
    case GF_FILE:
        if( myfgets( buff, max_len, gf->data.fp ) == NULL ) {
            return( NULL );
        }
        break;
    case GF_BOUND:
        if( gf->gf.a.currline >= gf->gf.a.maxlines ) {
            return( NULL );
        }
        gf->gf.a.currline++;
        if( BndMemory == NULL ) {
            read( gf->data.handle, buff, gf->gf.a.length + 1 );
        } else {
            memcpy( buff, gf->data.pos, gf->gf.a.length + 1 );
            gf->data.pos += gf->gf.a.length + 1;
        }
        len = gf->gf.a.length;
        gf->gf.a.length = (unsigned char)buff[len];
        buff[len] = '\0';
        break;
    default:
        if( gf->data.cfile == NULL ) {
            return( NULL );
        }
        memcpy( buff, gf->gf.b.cline->data, gf->gf.b.cline->len + 1 );
        rc = GimmeNextLinePtr( gf->data.cfile, &(gf->gf.b.cfcb), &(gf->gf.b.cline) );
        if( rc != ERR_NO_ERR ) {
            gf->data.cfile = NULL;
        }
        break;
    }
    return( buff );

} /* SpecialFgets */

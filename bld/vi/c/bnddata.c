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
* Description:  Access data bound into editor executable.
*
****************************************************************************/


#include "vi.h"
#include "sopen.h"
#include "posix.h"
#include "bnddata.h"
#include "specio.h"


#define isWSorCtrlZ( x )    (isspace( x ) || (x == 0x1A))

static bind_size    *dataOffsets = NULL;
static bind_size    *entryCounts = NULL;
static char         *dataFnames  = NULL;
static long         dataStart;

/*
 * BoundDataInit - check if data is bound to our exe
 */
void BoundDataInit( void )
{
    int         h;
    unsigned    i;
    char        buff[sizeof( MAGIC_COOKIE ) + sizeof( bind_size )];
    char        *tmp;
    unsigned    taillen;
    unsigned    dataFcnt;

    /*
     * get trailer
     */
    h = sopen3( EXEName, O_RDONLY | O_BINARY, SH_COMPAT );
    if( h == -1 ) {
        return;
    }
    lseek( h, SEEK_POSBACK( sizeof( buff ) ), SEEK_END );
    read( h, buff, sizeof( buff ) );

    /*
     * seek to start of data
     */
    if( strcmp( buff, MAGIC_COOKIE ) ) {
        close( h );
        return;
    }
    taillen = *(bind_size *)( buff + sizeof( MAGIC_COOKIE ) );
    dataStart = SEEK_POSBACK( taillen + sizeof( buff ) );
    lseek( h, dataStart, SEEK_END );

    /*
     * get everything
     */
    BndMemory = MemAlloc( taillen + sizeof( bind_size ) + sizeof( bind_size ) );
    read( h, BndMemory, taillen + sizeof( bind_size ) + sizeof( bind_size ) );
    close( h );

    /*
     * get number of files, and get space to store data
     */
    dataFcnt = *(bind_size *)BndMemory;
    dataOffsets = MemAlloc( dataFcnt * sizeof( bind_size ) );
    entryCounts = MemAlloc( dataFcnt * sizeof( bind_size ) );

    /*
     * get file names
     */
    tmp = BndMemory + sizeof( bind_size );
    i = *(bind_size *)tmp;
    tmp += sizeof( bind_size );
    dataFnames = MemAlloc( i );
    memcpy( dataFnames, tmp, i );
    tmp += i;

    /*
     * copy over file offset and linenumber data
     */
    i = dataFcnt * sizeof( bind_size );
    memcpy( dataOffsets, tmp, i );
    tmp += i;
    memcpy( entryCounts, tmp, dataFcnt * sizeof( bind_size ) );

    BoundData = true;

} /* BoundDataInit */

/*
 * BoundDataFini - check if data is bound to our exe
 */
void BoundDataFini( void )
{
    MemFree( BndMemory );
    MemFree( dataOffsets );
    MemFree( entryCounts );
    MemFree( dataFnames );

} /* BoundDataFini */

/*
 * SpecialOpen - open a file or exe
 */
bool SpecialOpen( const char *fn, GENERIC_FILE *gf, bool bounddata )
{
    long            shift = 0;
    int             h, i;
    unsigned char   a;
    vi_rc           rc;

    /*
     * process bound file
     */
    if( BoundData ) {

        if( !strcmp( fn, CONFIG_FILE ) ) {
            i = 0;
        } else {
            i = Tokenize( dataFnames, fn, true );
        }
        if( i != TOK_INVALID && bounddata ) {

            shift = dataStart + dataOffsets[i];
            gf->type = GF_BOUND;
            EditFlags.BndMemoryLocked = true;

            if( BndMemory == NULL ) {
                h = sopen3( EXEName, O_RDONLY | O_BINARY, SH_COMPAT );
                if( h == -1 ) {
                    return( false );
                }

                lseek( h, shift, SEEK_END );
                read( h, &a, 1 );
                gf->data.handle = h;
            } else {
                shift -= dataStart;
                gf->data.pos = &BndMemory[shift];
                a = gf->data.pos[0];
                gf->data.pos++;
            }
            gf->gf.a.currline = 0;
            gf->gf.a.maxlines = entryCounts[i];
            gf->gf.a.length = a;
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
    gf->data.f = GetFromEnvAndOpen( fn );
    return( gf->data.f != NULL );

} /* SpecialOpen */

/*
 * SpecialFclose - close either file or exe
 */
void SpecialFclose( GENERIC_FILE *gf )
{
    switch( gf->type ) {
    case GF_FILE:
        fclose( gf->data.f );
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
bool SpecialFgets( char *buff, int max, GENERIC_FILE *gf )
{
    size_t      i;
    vi_rc       rc;

    switch( gf->type ) {
    case GF_FILE:
        if( fgets( buff, max, gf->data.f ) == NULL ) {
            return( true );
        }
        for( i = strlen( buff ); i && isWSorCtrlZ( buff[i - 1] ); --i ) {
            buff[i - 1] = '\0';
        }
        break;
    case GF_BOUND:
        if( gf->gf.a.currline >= gf->gf.a.maxlines ) {
            return( true );
        }
        gf->gf.a.currline++;
        if( BndMemory == NULL ) {
            read( gf->data.handle, buff, gf->gf.a.length+1 );
        } else {
            memcpy( buff, gf->data.pos, gf->gf.a.length + 1 );
            gf->data.pos += gf->gf.a.length + 1;
        }
        i = gf->gf.a.length;
        gf->gf.a.length = (unsigned char)buff[i];
        buff[i] = '\0';
        break;
    default:
        if( gf->data.cfile == NULL ) {
            return( true );
        }
        memcpy( buff, gf->gf.b.cline->data, gf->gf.b.cline->len + 1 );
        rc = GimmeNextLinePtr( gf->data.cfile, &(gf->gf.b.cfcb), &(gf->gf.b.cline) );
        if( rc != ERR_NO_ERR ) {
            gf->data.cfile = NULL;
        }
        break;
    }
    return( false );

} /* SpecialFgets */

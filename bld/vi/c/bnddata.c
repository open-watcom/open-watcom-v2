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


#include <stdio.h>
#include <stdlib.h>
#include "posix.h"
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <share.h>
#include "vi.h"

static char magicCookie[]="CGEXXX";
#define MAGIC_COOKIE_SIZE sizeof( magicCookie )
static long *dataOffsets;
static short *entryCounts,dataFcnt;
static char *dataFnames;
static long dataStart;

/*
 * CheckForBoundData - check if data is bound to our exe
 */
void CheckForBoundData( void )
{
    int         h,i;
    char        buff[MAGIC_COOKIE_SIZE+3],*tmp;
    short       taillen;

    /*
     * get trailer
     */
    h = sopen( EXEName, O_RDONLY | O_BINARY, SH_COMPAT );
    lseek( h, -((long) MAGIC_COOKIE_SIZE+3L), SEEK_END );
    read( h, buff, 3+MAGIC_COOKIE_SIZE );

    /*
     * seek to start of data
     */
    if( strcmp( buff, magicCookie ) ) {
        close( h );
        return;
    }
    taillen = *( (short *) &(buff[MAGIC_COOKIE_SIZE+1]) );
    dataStart = (long) -((long) taillen+(long) MAGIC_COOKIE_SIZE+3);
    lseek( h, dataStart, SEEK_END );

    /*
     * get everything
     */
    BndMemory = MemAlloc( taillen+4 );
    read( h, BndMemory, taillen+4 );
    close( h );

    /*
     * get number of files, and get space to store data
     */
    dataFcnt = *(short *) BndMemory;
    dataOffsets = MemAlloc( dataFcnt * sizeof( long ) );
    entryCounts = MemAlloc( dataFcnt * sizeof( short ) );

    /*
     * get file names
     */
    tmp = BndMemory+2;
    i = *(short *) tmp;
    tmp += 2;
    dataFnames = MemAlloc( i );
    memcpy( dataFnames, tmp, i );
    tmp += i;

    /*
     * copy over file offset and linenumber data
     */
    i = dataFcnt * sizeof( long );
    memcpy( dataOffsets, tmp, i );
    tmp += i;
    memcpy( entryCounts, tmp , dataFcnt * sizeof( short ) );

    EditFlags.BoundData = TRUE;

} /* CheckForBoundData */

/*
 * SpecialOpen - open a file or exe
 */
bool SpecialOpen( char *fn, GENERIC_FILE *gf )
{
    long        shift=0;
    int         h,i;
    char        a;

    /*
     * process bound file
     */
    if( EditFlags.BoundData && !EditFlags.OpeningFileToCompile ) {

        if( !strcmp( fn, CONFIG_FILE ) ) {
            i = 0;
        } else {
            i = Tokenize( dataFnames, fn, TRUE );
        }
        if( i >= 0 ) {

            shift = dataStart + dataOffsets[i];
            gf->type = GF_BOUND;
            EditFlags.BndMemoryLocked = TRUE;

            if( BndMemory == NULL ) {
                h = sopen( EXEName, O_RDONLY | O_BINARY, SH_COMPAT );
                if( h == -1 ) {
                    return( FALSE );
                }

                lseek( h, shift, SEEK_END );
                read( h, &a, 1 );
                gf->data.handle = h;
            } else {
                shift -= dataStart;
                gf->data.pos = &BndMemory[ shift ];
                a = (int) gf->data.pos[0];
                gf->data.pos++;
            }
            gf->gf.a.currline = 0;
            gf->gf.a.maxlines = entryCounts[i];
            gf->gf.a.length = (int) a;
            return( TRUE );

        }

    }

    /*
     * special case - open current buffer
     */
    if( fn[0] == '.' && fn[1] == 0 ) {
        gf->type = GF_BUFFER;
        gf->data.cfile = CurrentFile;
        i = GimmeLinePtr( 1, CurrentFile, &(gf->gf.b.cfcb), &(gf->gf.b.cline));
        if( i ) {
            return( FALSE );
        }
        return( TRUE );
    }


    /*
     * process regular file
     */
    gf->type = GF_FILE;
    gf->data.f = GetFromEnvAndOpen( fn );
    if( gf->data.f == NULL ) {
        return( FALSE );
    }
    return( TRUE );

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
        EditFlags.BndMemoryLocked = FALSE;
        if( BndMemory == NULL ) {
            close( gf->data.handle );
        }
        break;
    }

} /* SpecialFclose */

/*
 * SpecialFgets - get from either file or exe
 */
int SpecialFgets( char *buff, int max, GENERIC_FILE *gf )
{
    void        *tmp;
    int         i,j;

    switch( gf->type ) {
    case GF_FILE:
        tmp = fgets( buff, max, gf->data.f );
        if( tmp == NULL ) {
            return( -1 );
        }
        i = strlen( buff ) - 1;
        buff[ i ] = 0;
        return( i );
    case GF_BOUND:
        if( gf->gf.a.currline >= gf->gf.a.maxlines ) {
            return( -1 );
        }
        gf->gf.a.currline ++;
        if( BndMemory == NULL ) {
            read( gf->data.handle, buff, gf->gf.a.length+1 );
        } else {
            memcpy( buff, gf->data.pos, gf->gf.a.length + 1 );
            gf->data.pos += gf->gf.a.length + 1;
        }
        j = gf->gf.a.length;
        i = (int) buff[j];
        buff[j] = 0;
        gf->gf.a.length = i;
        return( j );
    default:
        if( gf->data.cfile == NULL ) {
            return( -1 );
        }
        j= gf->gf.b.cline->len;
        memcpy( buff, gf->gf.b.cline->data, j + 1 );
        i = GimmeNextLinePtr( gf->data.cfile, &(gf->gf.b.cfcb),
                              &(gf->gf.b.cline) );
        if( i ) {
            gf->data.cfile = NULL;
        }
        return( j );
    }

} /* SpecialFgets */

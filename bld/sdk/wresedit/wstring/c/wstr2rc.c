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


#include "precomp.h"
#include <stdio.h>
#include "watcom.h"
#include "wglbl.h"
#include "wsetedit.h"
#include "widn2str.h"
#include "wstr2rc.h"
#include "wresall.h"

static bool WWriteStringEntry( WStringBlock *block, uint_16 string_id, FILE *fp )
{
    char        *strtext;
    char        *text;
    bool        ok;

    strtext = NULL;

    text = WResIDNameToStr( block->block.String[string_id & 0xf] );
    ok = (text != NULL);

    if( ok ) {
        strtext = WConvertStringFrom( text, "\t\n\"\\", "tn\"\\" );
        ok = (strtext != NULL);
    }

    if( ok ) {
        if( block->symbol[string_id & 0xf] ) {
            fprintf( fp, "    %s, \"%s\"\n",
                     block->symbol[string_id & 0xf], strtext );
        } else {
            fprintf( fp, "    %u, \"%s\"\n", string_id, strtext );
        }
    }

    if( strtext != NULL ) {
        WRMemFree( strtext );
    }

    if( text != NULL ) {
        WRMemFree( text );
    }

    return( ok );
}

static bool WWriteStringBlock( WStringBlock *block, FILE *fp )
{
    int         i;

    if( block == NULL ) {
        return( FALSE );
    }

    for( i = 0; i < STRTABLE_STRS_PER_BLOCK; i++ ) {
        if( block->block.String[i] != NULL ) {
            if( !WWriteStringEntry( block, (block->blocknum & 0xfff0) + i, fp ) ) {
                return( FALSE );
            }
        }
    }

    return( TRUE );
}

bool WWriteStringToRC( WStringEditInfo *einfo, char *file, bool append )
{
    FILE                *fp;
    WStringBlock        *block;
    bool                ok;

    ok = (einfo != NULL && einfo->tbl != NULL);

    if( ok ) {
        if( append ) {
            fp = fopen( file, "at" );
        } else {
            fp = fopen( file, "wt" );
        }
        ok = (fp != NULL);
    }

    if( ok ) {
        fprintf( fp, "STRINGTABLE\n" );
        fwrite( "BEGIN\n", sizeof( char ), 6, fp );
    }

    if( ok ) {
        block = einfo->tbl->first_block;
        while( block != NULL && ok ) {
            ok = WWriteStringBlock( block, fp );
            block = block->next;
        }
    }

    if( ok ) {
        fwrite( "END\n\n", sizeof( char ), 5, fp );
    }

    if( fp != NULL ) {
        fclose( fp );
    }

    return( ok );
}

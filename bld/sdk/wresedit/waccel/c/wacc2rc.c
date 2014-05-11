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
#include <string.h>
#include <stdio.h>

#include "watcom.h"
#include "wglbl.h"
#include "wvk2str.h"
#include "wacc2rc.h"
#include "wresall.h"

static char VirtualText[]       = "VIRTKEY";
static char AltText[]           = "ALT";
static char ShiftText[]         = "SHIFT";
static char ControlText[]       = "CONTROL";

static Bool WSetFlagsText( uint_16 flags, char **text )
{
    int         tlen;

    if( text == NULL ) {
        return( FALSE );
    }

    tlen = 0;
    *text = NULL;

    if( flags & ACCEL_VIRTKEY ) {
        tlen += sizeof( VirtualText ) + 2;
    }

    if( flags & ACCEL_SHIFT ) {
        tlen += sizeof( ShiftText ) + 2;
    }

    if( flags & ACCEL_ALT ) {
        tlen += sizeof( AltText ) + 2;
    }

    if( flags & ACCEL_CONTROL ) {
        tlen += sizeof( ControlText ) + 2;
    }

    if( tlen == 0 ) {
        return( TRUE );
    }

    *text = (char *)WRMemAlloc( tlen + 1 );
    if( *text == NULL ) {
        return( FALSE );
    }

    (*text)[0] = '\0';

    if( flags & ACCEL_VIRTKEY ) {
        strcat( *text, ", " );
        strcat( *text, VirtualText );
    }

    if( flags & ACCEL_SHIFT ) {
        strcat( *text, ", " );
        strcat( *text, ShiftText );
    }

    if( flags & ACCEL_ALT ) {
        strcat( *text, ", " );
        strcat( *text, AltText );
    }

    if( flags & ACCEL_CONTROL ) {
        strcat( *text, ", " );
        strcat( *text, ControlText );
    }

    return( TRUE );
}

static Bool WWriteEntryToRC( WAccelEditInfo *einfo, WAccelEntry *entry, FILE *fp )
{
    char        *keytext;
    char        *flagtext;
    uint_16     key, flags, id;
    Bool        ok;

    flagtext = NULL;

    ok = (einfo != NULL && entry != NULL);

    if( ok ) {
        if( entry->is32bit ) {
            key = entry->entry32.Ascii;
            flags = entry->entry32.Flags;
            id = entry->entry32.Id;
        } else {
            key = entry->entry.Ascii;
            flags = entry->entry.Flags;
            id = (uint_16)entry->entry.Id;
        }
        keytext = WGetKeyText( key, flags );
        ok = (keytext != NULL);
    }

    if( ok ) {
        ok = WSetFlagsText( flags, &flagtext );
    }

    if( ok ) {
        fprintf( fp, "    %s,\t", keytext );
        if( entry->symbol ) {
            fprintf( fp, "%s", entry->symbol );
        } else {
            fprintf( fp, "%d", (int)id );
        }
        if( flagtext != NULL ) {
            fprintf( fp, "%s\n", flagtext );
        } else {
            fwrite( "\n", sizeof( char ), 1, fp );
        }
    }

    if( flagtext != NULL ) {
        WRMemFree( flagtext );
    }

    return( ok );
}

Bool WWriteAccToRC( WAccelEditInfo *einfo, char *file, Bool append )
{
    WAccelEntry *entry;
    FILE        *fp;
    char        *rname;
    Bool        ok;

    rname = NULL;

    ok = (einfo != NULL && einfo->tbl != NULL && einfo->info->res_name != NULL &&
          file != NULL);

    if( ok ) {
        if( append ) {
            fp = fopen( file, "at" );
        } else {
            fp = fopen( file, "wt" );
        }
        ok = (fp != NULL);
    }

    if( ok ) {
        ok = ((rname = WResIDToStr( einfo->info->res_name )) != NULL);
    }

    if( ok ) {
        fprintf( fp, "%s ACCELERATORS\n", rname );
        fwrite( "BEGIN\n", sizeof( char ), 6, fp );
    }

    if( ok ) {
        entry = einfo->tbl->first_entry;
        while( entry != NULL && ok ) {
            ok = WWriteEntryToRC( einfo, entry, fp );
            entry = entry->next;
        }
    }

    if( ok ) {
        fwrite( "END\n\n", sizeof( char ), 5, fp );
    }

    if( rname ) {
        WRMemFree( rname );
    }

    if( fp ) {
        fclose( fp );
    }

    return( ok );
}

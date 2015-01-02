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


#include "dbgdefn.h"
#include "dbgdata.h"
#include "mad.h"
#include "memtypes.h"
#include "dbgmem.h"
#include "dbgitem.h"
#include "strutil.h"
#include "dbgscan.h"
#include "dbgmad.h"
#include "madcli.h"


static walk_result MadMemTypeWalk( mad_type_handle th, void *d )
{
    mem_type_walk_data  *data = d;
    mad_type_info       tinfo;
    int                 ipl;
    int                 i;

    if( data->labels != NULL ) {
        i = data->num_types;
        MADTypeInfo( th, &tinfo );
        MADCliString( MADTypeName( th ), TxtBuff, TXT_LEN );
        data->labels[ i ] = DupStr( TxtBuff );
        data->info[ i ].type = th;
        data->info[ i ].item_width = GetMADMaxFormatWidth( th );
        data->info[ i ].item_size = tinfo.b.bits / BITS_PER_BYTE;
        data->info[ i ].piece_radix = MADTypePreferredRadix( th );
        ipl = 80 / ( data->info[ i ].item_width + 1 ); // kludge
        if( ipl > 16 ) {
            ipl = 16;
        } else if( ipl > 8 ) {
            ipl = 8;
        } else if( ipl > 4 ) {
            ipl = 4;
        } else if( ipl > 2 ) {
            ipl = 2;
        } else {
            ipl = 1;
        }
        data->info[ i ].items_per_line = ipl;
    }
    data->num_types++;
    return( WR_CONTINUE );
}

extern void MemInitTypes( mad_type_kind mas, mem_type_walk_data *data )
{
    data->num_types = 0;
    data->labels = NULL;
    data->info = NULL;
    MADTypeWalk( mas, MadMemTypeWalk, data );
    if( data->num_types == 0 ) return;
    data->labels = DbgAlloc( data->num_types * sizeof( *data->labels ) );
    data->info = DbgAlloc( data->num_types * sizeof( *data->info ) );
    data->num_types = 0;
    MADTypeWalk( mas, MadMemTypeWalk, data );
}

extern void MemFiniTypes( mem_type_walk_data *data )
{
    int         i;

    for( i = 0; i < data->num_types; ++i ) {
        DbgFree( data->labels[i] );
    }
    DbgFree( data->labels );
    DbgFree( data->info );
    data->num_types = 0;
    data->labels = NULL;
    data->info = NULL;
}

unsigned GetMADMaxFormatWidth( mad_type_handle th )
{
    unsigned            old,new;
    item_mach           tmp;
    unsigned            max;
    mad_type_info       mti;
    int                 sign = 0;
    unsigned long       *plong;

    MADTypeInfo( th, &mti );
    switch( mti.b.kind ) {
    case MTK_ADDRESS:
    case MTK_INTEGER:
        memset( &tmp, -1, sizeof( tmp ) );
        if( mti.i.nr != MNR_UNSIGNED ) {
            plong = (unsigned long *)tmp.ar + mti.i.sign_pos / 32;
            *plong &= ( 1L << ( mti.i.sign_pos % 32 ) ) - 1;
            ++sign;
        }
        break;
    case MTK_FLOAT:
        memset( &tmp, 0, sizeof( tmp ) );
        break;
    }
    new = MADTypePreferredRadix( th );
    old = NewCurrRadix( new );
    max = 0;
    MADTypeToString( new, &mti, &tmp, TxtBuff, &max );
    NewCurrRadix( old );
    return( max + sign );
}

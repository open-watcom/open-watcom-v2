/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Microsoft style OMF output routines.
*
****************************************************************************/


#include <string.h>
#include "asmglob.h"
#include "omfgenio.h"
#include "pcobj.h"
#include "asmalloc.h"
#include "directiv.h"
#include "omfobjre.h"
#include "omfgen.h"
#include "queues.h"
#include "omflifix.h"
#include "myassert.h"


static lifix_list   lifList;

static int writeMisc( obj_rec *objr )
/*
    For 16-bit records which are the same under Intel, PharLap, and MS OMFs
*/
{
    uint_8  *ptr;
    uint_16 len;
    uint_16 save;

/**/myassert( objr != NULL );
/**/myassert( objr->data != NULL );
    save = ObjRTell( objr );
    ObjRSeek( objr, 0 );
    len = ObjRemain( objr );
    ptr = ObjGet( objr, len );
    ObjWriteRec( objr->command, len, ptr );
    ObjRSeek( objr, save );
    return( 0 );
}

static int writeMisc32( obj_rec *objr )
/*
    For 32-bit records which are the same under Intel, PharLap, and MS OMFs
*/
{
    uint_8  *ptr;
    uint_16 len;
    uint_8  cmd;
    uint_16 save;

/**/myassert( objr != NULL );
/**/myassert( objr->data != NULL );
    save = ObjRTell( objr );
    ObjRSeek( objr, 0 );
    len = ObjRemain( objr );
    ptr = ObjGet( objr, len );
    cmd = objr->command;
    if( objr->is_32 ) {
        cmd |= 0x01;
    }
    ObjWriteRec( cmd, len, ptr );
    ObjRSeek( objr, save );
    return( 0 );
}

static int writeComent( obj_rec *objr )
{
    uint_8  *ptr;
    uint_16 len;
    uint_16 save;

/**/myassert( objr != NULL );
/**/myassert( objr->data != NULL );
    ObjWBegRec( CMD_COMENT );
    ObjWrite8( objr->d.coment.attr );
    ObjWrite8( objr->d.coment.class );
    save = ObjRTell( objr );
    len = ObjRemain( objr );
    ptr = ObjGet( objr, len );
    ObjWrite( ptr, len );
    ObjRSeek( objr, save );
    ObjWEndRec();
    return( 0 );
}

static int writeSegdef( obj_rec *objr )
{
    int         is32;
    uint_8      acbp;
    uint_8      align;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_SEGDEF );
    is32 = objr->d.segdef.use_32 != 0;
    ObjWBegRec( is32 ? CMD_SEGD32 : CMD_SEGDEF );
    acbp = objr->d.segdef.combine << 2;
    if( is32 ) {
        acbp |= 1;
    }
    align = objr->d.segdef.align;
    switch( align ) {
    case SEGDEF_ALIGN_ABS:      acbp |= ALIGN_ABS << 5;     break;
    case SEGDEF_ALIGN_BYTE:     acbp |= ALIGN_BYTE << 5;    break;
    case SEGDEF_ALIGN_WORD:     acbp |= ALIGN_WORD << 5;    break;
    case SEGDEF_ALIGN_PARA:     acbp |= ALIGN_PARA << 5;    break;
    case SEGDEF_ALIGN_PAGE:     acbp |= ALIGN_PAGE << 5;    break;
    case SEGDEF_ALIGN_DWORD:    acbp |= ALIGN_DWORD << 5;   break;
    case SEGDEF_ALIGN_4KPAGE:
        acbp |= ALIGN_PAGE;
        break;
    default:
/**/    never_reach();
    }
    if( !is32 && objr->d.segdef.seg_length == 0x10000 ) {
        acbp |= 0x02;   /* BIG bit */ /* FIXME no support for 2**32 */
    }
    ObjWrite8( acbp );
    if( align == SEGDEF_ALIGN_ABS ) {
        // absolut segment has frame=word and offset=byte
        // it isn't fixupp physical reference
        // and don't depend on segment size (16/32bit)
        ObjWrite16( objr->d.segdef.abs.frame );
        ObjWrite8( (uint_8)objr->d.segdef.abs.offset );
    }
    if( is32 ) {
        ObjWrite32( objr->d.segdef.seg_length );
    } else {
        ObjWrite16( (uint_16)objr->d.segdef.seg_length );
    }
    ObjWriteIndex( objr->d.segdef.seg_name_idx );
    ObjWriteIndex( objr->d.segdef.class_name_idx );
    ObjWriteIndex( objr->d.segdef.ovl_name_idx );
    ObjWEndRec();
    return( 0 );
}

static int writeFixup( obj_rec *objr )
{
    int         is32;
    fixup       *walk;
    uint_8      buf[ FIX_GEN_MAX ];
    uint_16     len;
    uint_16     len_written;
    uint_8      cmd1;
    uint_8      cmd2;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_FIXUPP );
    walk = objr->d.fixupp.fixup;
    if( walk == NULL ) {
            /* huh? this shouldn't really happen... */
        return( 0 );
    }
    is32 = objr->is_32;
    cmd1 = is32 ? CMD_FIXU32 : CMD_FIXUPP;
    cmd2 = is32 ? FIX_GEN_MS386 : FIX_GEN_INTEL;
    /* we don't want to write FIXUP records that are too large, so we limit
       our records to approximately 1024 bytes */
    do {
        len_written = 0;
        ObjWBegRec( cmd1 );
        while( walk != NULL && len_written < 1024 - FIX_GEN_MAX ) {
            walk->loc_offset += LifixDelta( &lifList, (uint_16)walk->loc_offset );
            len = FixGenFix( walk, buf, cmd2 );
            ObjWrite( buf, len );
            walk = walk->next;
            len_written += len;
        }
        ObjWEndRec();
    } while( walk != NULL );
    return( 0 );
}

static int writeLedata( obj_rec *objr )
{
    uint_16     save;
    uint_8      *ptr;
    uint_16     len;
    int         is32;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_LEDATA );
    LifixDestroy( &lifList );
    LifixInit( &lifList );
    is32 = objr->is_32;
    ObjWBegRec( is32 ? CMD_LEDA32 : CMD_LEDATA );
    ObjWriteIndex( objr->d.ledata.idx );
    if( is32 ) {
        ObjWrite32( objr->d.ledata.offset );
    } else {
        ObjWrite16( (uint_16)objr->d.ledata.offset );
    }
    save = ObjRTell( objr );
    len = ObjRemain( objr );
    ptr = ObjGet( objr, len );
/**/myassert( len <= 1024 );
    ObjWrite( ptr, len );
    ObjWEndRec();
    ObjRSeek( objr, save );
    return( 0 );
}

static void id32Block( obj_rec *objr, int_16 *delta, uint_16 first_block_offset )
/*
    Since LIDATAs are different under PharLap and MicroSoft 386 formats,
    we have to do some magic.
*/
{
    uint_16 rpt_count;
    uint_16 blk_count;
    uint_8  data_count;
    uint_8  *ptr;

    rpt_count = ObjGet16( objr );
    ObjWrite32( rpt_count );
    *delta += 2;    /* ok, everything from here on will be at +2 offset */
    LifixAdd( &lifList, ObjRTell( objr ) - first_block_offset, *delta );
    blk_count = ObjGet16( objr );
    ObjWrite16( blk_count );
    if( blk_count == 0 ) {
        data_count = ObjGet8( objr );
        ObjWrite8( data_count );
        ptr = ObjGet( objr, data_count );
        ObjWrite( ptr, data_count );
    } else {
        for( ; blk_count != 0; --blk_count ) {
            id32Block( objr, delta, first_block_offset );
        }
    }
}

static int writeLidata( obj_rec *objr )
{
    uint_16     save;
    uint_8      *ptr;
    uint_16     len;
    int         is32;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_LIDATA );
    LifixDestroy( &lifList );
    LifixInit( &lifList );
    save = ObjRTell( objr );
    is32 = objr->is_32;
    ObjWBegRec( is32 ? CMD_LIDA32 : CMD_LIDATA );
    ObjWriteIndex( objr->d.lidata.idx );
    if( is32 ) {
        ObjWrite32( objr->d.lidata.offset );
    } else {
        ObjWrite16( (uint_16)objr->d.lidata.offset );
    }
    /* ok, already in our format */
    len = ObjRemain( objr );
    ptr = ObjGet( objr, len );
    ObjWrite( ptr, len );
    ObjWEndRec();
    ObjRSeek( objr, save );
    return( 0 );
}

static int writeTheadr( obj_rec *objr )
{
/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_THEADR );
        /* here's our initialization code */
    LifixInit( &lifList );

    return( writeMisc( objr ) );
}

static int writeModend( obj_rec *objr )
{
    uint_16 len;
    int     is32;
    uint_8  buf[ 1 + FIX_GEN_MAX ];
    int     is_log;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_MODEND );
    is32 = objr->is_32;
    len = 1;
    buf[0] = objr->d.modend.main_module != 0 ? 0x80 : 0;
    if( objr->d.modend.start_addrs ) {
        buf[0] |= 0x40;
        is_log = objr->d.modend.is_logical != 0;
        if( is_log ) {
            buf[0] |= 1;
        }
        len += FixGenRef( &objr->d.modend.ref, is_log, buf + 1, is32 ? FIX_GEN_MS386 : FIX_GEN_INTEL );
    }
    if( buf[0] == 0 ) {
        is32 = 0;       /* no need for MODE32 in this case */
    }
    ObjWriteRec( is32 ? CMD_MODE32 : CMD_MODEND, len, buf );
    /* here's our fini code */
    LifixDestroy( &lifList );

    return( 0 );
}

static void writeBase( obj_rec *objr )
{
    uint_16 grp_idx;
    uint_16 seg_idx;

    grp_idx = objr->d.base.grp_idx;
    seg_idx = objr->d.base.seg_idx;
    ObjWriteIndex( grp_idx );
    ObjWriteIndex( seg_idx );
    if( grp_idx == 0 && seg_idx == 0 ) {
        ObjWrite16( objr->d.base.frame );
    }
}

static int writeComdat( obj_rec *objr )
{
    uint_8      *ptr;
    uint_16     len;
    uint_16     save;
    int         is32;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_COMDAT );
    save = ObjRTell( objr );
    is32 = objr->is_32;
    if( is32 ) {
        objr->command |= 1;
    }
    ObjWBegRec( objr->command );
    ObjWrite8( objr->d.comdat.flags );
    ObjWrite8( objr->d.comdat.attributes );
    ObjWrite8( objr->d.comdat.align );
    if( is32 ) {
        ObjWrite32( objr->d.comdat.offset );
    } else {
        ObjWrite16( (uint_16)objr->d.comdat.offset );
    }
    ObjWriteIndex( objr->d.comdat.type_idx );
    if( ( objr->d.comdat.attributes & COMDAT_ALLOC_MASK ) == COMDAT_EXPLICIT ) {
        writeBase( objr );
    }
    ObjWriteIndex( objr->d.comdat.public_name_idx );
    /* record is already in ms omf format */
    len = ObjRemain( objr );
    ptr = ObjGet( objr, len );
/**/myassert( len <= 1024 );
    ObjWrite( ptr, len );
    ObjWEndRec();
    ObjRSeek( objr, save );
    return( 0 );
}

static int writePubdef( obj_rec *objr )
{
    int         is32;
    const char  *name;
    size_t       name_len;
    pubdef_data *pubdata;
    pubdef_data *pubstop;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_PUBDEF || objr->command == CMD_STATIC_PUBDEF );
    is32 = objr->is_32;
    if( is32 ) {
        objr->command |= 1;
    }
    ObjWBegRec( objr->command );
    writeBase( objr );
    pubdata = objr->d.pubdef.pubs;
    if( pubdata != NULL ) {
        pubstop = pubdata + objr->d.pubdef.num_pubs;
        while( pubdata < pubstop ) {
            name = NameGet( pubdata->name );
            name_len = strlen( name );
            if( name_len > 255 )
                name_len = 255;
            ObjWrite8( (uint_8)name_len );
            ObjWrite( (uint_8 *)name, (uint_16)name_len );
            if( is32 ) {
                ObjWrite32( pubdata->offset );
            } else {
                ObjWrite16( (uint_16)pubdata->offset );
            }
            ObjWriteIndex( pubdata->type.idx );
            ++pubdata;
        }
    }
    ObjWEndRec();
    return( 0 );
}

static void writeLinnumData( obj_rec *objr )
{
    int  is32;
    linnum_data *cur;
    linnum_data *stop;

/**/myassert( objr != NULL );
    is32 = objr->is_32;
    cur = objr->d.linnum.lines;
    stop = cur + objr->d.linnum.num_lines;
    while( cur < stop ) {
        ObjWrite16( cur->number );
        if( is32 ) {
            ObjWrite32( cur->offset );
        } else {
/**/        myassert( ( cur->offset & 0xffff0000 ) == 0 );
            ObjWrite16( (uint_16)cur->offset );
        }
        ++cur;
    }
}

static int writeLinnum( obj_rec *objr )
{
    int         is32;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_LINNUM );
    is32 = objr->is_32;
    ObjWBegRec( is32 ? CMD_LINN32 : CMD_LINNUM );
    writeBase( objr );
    writeLinnumData(objr );
    ObjWEndRec();
    return( 0 );
}

static int writeLinsym( obj_rec *objr )
{
    int         is32;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_LINSYM );
    is32 = objr->is_32;
    ObjWBegRec( is32 ? CMD_LINS32 : CMD_LINSYM );
    ObjWrite8( objr->d.linsym.d.linsym.flags );
    ObjWriteIndex( objr->d.linsym.d.linsym.public_name_idx );
    writeLinnumData( objr );
    ObjWEndRec();
    return( 0 );
}

void write_record( obj_rec *objr, bool kill )
/*******************************************/
{
    /**/myassert( objr != NULL );
    ObjRSeek( objr, 0 );
    switch( objr->command ) {
    case CMD_THEADR:
        writeTheadr( objr );
        break;
    case CMD_COMENT:
        writeComent( objr );
        break;
    case CMD_MODEND:
        writeModend( objr );
        break;
    case CMD_SEGDEF:
        writeSegdef( objr );
        break;
    case CMD_LEDATA:
        writeLedata( objr );
        break;
    case CMD_FIXUPP:
        writeFixup( objr );
        break;
    case CMD_PUBDEF:
        writePubdef( objr );
        break;
    case CMD_LINNUM:
        writeLinnum( objr );
        break;
    case CMD_LIDATA:
        writeLidata( objr );
        break;
    case CMD_STATIC_PUBDEF:
        writePubdef( objr );
        break;
    case CMD_COMDAT:
        writeComdat( objr );
        break;
    case CMD_LINSYM:
        writeLinsym( objr );
        break;
    case CMD_STATIC_EXTDEF:
    case CMD_BAKPAT:
    case CMD_NBKPAT:
        writeMisc32( objr );
        break;
    case CMD_EXTDEF:
    case CMD_LNAMES:
    case CMD_GRPDEF:
    case CMD_TYPDEF:
    case CMD_COMDEF:
    case CMD_CEXTDEF:
    case CMD_STATIC_COMDEF:
    case CMD_ALIAS:
    case CMD_LLNAMES:
        writeMisc( objr );
        break;
    }
    if( kill ) {
        ObjKillRec( objr );
    }
}

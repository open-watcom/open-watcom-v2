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


#include <string.h>
#include "watcom.h"
#include "womp.h"
#include "objprs.h"
#include "objio.h"
#include "pcobj.h"
#include "memutil.h"
#include "genutil.h"
#include "myassert.h"
#include "lifix.h"
#include "objrec.h"

STATIC lifix_list   lifList;

STATIC int writeMisc( obj_rec *objr, pobj_state *state ) {
/*
    For 16-bit records which are the same under Intel, PharLap, and MS OMFs
*/
    uint_8  *ptr;
    uint_16 len;
    uint_16 save;

/**/myassert( objr != NULL );
/**/myassert( objr->data != NULL );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_WRITE_PASS );
    save = ObjRTell( objr );
    ObjRSeek( objr, 0 );
    len = ObjRemain( objr );
    ptr = ObjGet( objr, len );
    ObjWriteRec( state->file_out, objr->command, len, ptr );
    ObjRSeek( objr, save );
    return( 0 );
}

STATIC int writeComent( obj_rec *objr, pobj_state *state ) {

    uint_8  *ptr;
    uint_16 len;
    uint_16 save;
    OBJ_WFILE *out;

/**/myassert( objr != NULL );
/**/myassert( objr->data != NULL );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_WRITE_PASS );
    out = state->file_out;
    ObjWBegRec( out, CMD_COMENT );
    ObjWrite8( out, objr->d.coment.attr );
    ObjWrite8( out, objr->d.coment.class );
    save = ObjRTell( objr );
    len = ObjRemain( objr );
    ptr = ObjGet( objr, len );
    ObjWrite( out, ptr, len );
    ObjRSeek( objr, save );
    ObjWEndRec( out );
    return( 0 );
}

STATIC int writeSegdef( obj_rec *objr, pobj_state *state ) {

    OBJ_WFILE   *out;
    uint_8      acbp;
    uint_8      align;
    char        buf[ FIX_GEN_MAX ];
    size_t      len;
    uint_8      access;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_SEGDEF );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_WRITE_PASS );
    out = state->file_out;
    ObjWBegRec( out, CMD_SEGDEF );
    acbp = ( objr->d.segdef.combine << 2 );
    align = objr->d.segdef.align;
    switch( align ) {
    case SEGDEF_ALIGN_ABS:      acbp |= ALIGN_ABS << 5;     break;
    case SEGDEF_ALIGN_BYTE:     acbp |= ALIGN_BYTE << 5;    break;
    case SEGDEF_ALIGN_WORD:     acbp |= ALIGN_WORD << 5;    break;
    case SEGDEF_ALIGN_PARA:     acbp |= ALIGN_PARA << 5;    break;
    case SEGDEF_ALIGN_PAGE:     acbp |= ALIGN_PAGE << 5;    break;
    case SEGDEF_ALIGN_DWORD:    acbp |= ALIGN_DWORD << 5;   break;
    case SEGDEF_ALIGN_4KPAGE:   acbp |= ALIGN_4KPAGE << 5;  break;
    default:
/**/    never_reach();
    }
    /* FIXME no support for 2**32 BIG bit */
    ObjWrite8( out, acbp );
    if( align == SEGDEF_ALIGN_ABS ) {
        len = FixGenPRef( &objr->d.segdef.abs, buf, FIX_GEN_PHARLAP );
        ObjWrite( out, buf, len );
    }
    ObjWrite32( out, objr->d.segdef.seg_length );
    ObjWriteIndex( out, objr->d.segdef.seg_name_idx );
    ObjWriteIndex( out, objr->d.segdef.class_name_idx );
    ObjWriteIndex( out, objr->d.segdef.ovl_name_idx );
    if( objr->d.segdef.access_valid || objr->d.segdef.use_32 == 0 ) {
        access = objr->d.segdef.use_32 ? EASY_USE32_FIELD : 0;
        if( objr->d.segdef.access_valid ) {
            access |= objr->d.segdef.access_attr;
        } else {
            access |= EASY_EXEC_READ;
        }
        ObjWrite8( out, access );
    }
    ObjWEndRec( out );
    return( 0 );
}

STATIC int writeFixup( obj_rec *objr, pobj_state *state ) {

    OBJ_WFILE   *out;
    fixup       *walk;
    char        buf[ FIX_GEN_MAX ];
    size_t      len;
    size_t      len_written;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_FIXUP );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_WRITE_PASS );
    walk = objr->d.fixup.fixup;
    if( walk == NULL ) {
            /* huh? this shouldn't really happen... */
        return( 0 );
    }
    out = state->file_out;
    /* we don't want to write FIXUP records that are too large, so we limit
       our records to approximately 1024 bytes */
    do {
        len_written = 0;
        ObjWBegRec( out, CMD_FIXUP );
        while( walk != NULL && len_written < 1024 - FIX_GEN_MAX ) {
            walk->loc_offset += LifixDelta( &lifList, walk->loc_offset );
            len = FixGenFix( walk, buf, FIX_GEN_PHARLAP );
            ObjWrite( out, buf, len );
            walk = walk->next;
            len_written += len;
        }
        ObjWEndRec( out );
    } while( walk != NULL );
    return( 0 );
}

STATIC int writeLedata( obj_rec *objr, pobj_state *state ) {

    OBJ_WFILE   *out;
    uint_16     save;
    uint_8      *ptr;
    uint_16     len;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_LEDATA );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_WRITE_PASS );
    LifixDestroy( &lifList );
    LifixInit( &lifList );
    out = state->file_out;
    ObjWBegRec( out, CMD_LEDATA );
    ObjWriteIndex( out, objr->d.ledata.idx );
    ObjWrite32( out, objr->d.ledata.offset );
    save = ObjRTell( objr );
    len = ObjRemain( objr );
    ptr = ObjGet( objr, len );
/**/myassert( len <= 1024 );
    ObjWrite( out, ptr, len );
    ObjWEndRec( out );
    ObjRSeek( objr, save );
    return( 0 );
}

STATIC void id32Block( obj_rec *objr, OBJ_WFILE *out, int_16 *delta,
    uint_16 first_block_offset ) {

    uint_16 rpt_count;
    uint_16 blk_count;
    uint_8  data_count;
    uint_8  *ptr;

    rpt_count = ObjGet32( objr );
    if( rpt_count & 0xffff0000 ) {
        /* FIXME we could do lots of contortions to remove this fatal msg */
        Fatal( MSG_32BIT_RPT_COUNT );
    }
    ObjWrite16( out, rpt_count );
    *delta -= 2;    /* ok, everything from here on will be at -2 offset */
    LifixAdd( &lifList, ObjRTell( objr ) - first_block_offset, *delta );
    blk_count = ObjGet16( objr );
    ObjWrite16( out, blk_count );
    if( blk_count == 0 ) {
        data_count = ObjGet8( objr );
        ObjWrite8( out, data_count );
        ptr = ObjGet( objr, data_count );
        ObjWrite( out, ptr, data_count );
    } else {
        for( ; blk_count != 0; --blk_count ) {
            id32Block( objr, out, delta, first_block_offset );
        }
    }
}

STATIC int writeLidata( obj_rec *objr, pobj_state *state ) {

    OBJ_WFILE   *out;
    uint_16     save;
    uint_8      *ptr;
    uint_16     len;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_LIDATA );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_WRITE_PASS );
    LifixDestroy( &lifList );
    LifixInit( &lifList );
    out = state->file_out;
    save = ObjRTell( objr );
    ObjWBegRec( out, CMD_LIDATA );
    ObjWriteIndex( out, objr->d.lidata.idx );
    ObjWrite32( out, objr->d.lidata.offset );
    if( objr->is_phar || !objr->is_32 ) {
        /* ok, already in our format */
        len = ObjRemain( objr );
        ptr = ObjGet( objr, len );
        ObjWrite( out, ptr, len );
    } else {
        int_16 delta;
        uint_16 first_block_offset;

        delta = 0;  /* id32Block needs to play with this */
        first_block_offset = ObjRTell( objr );
        while( !ObjEOR( objr ) ) {
            id32Block( objr, out, &delta, first_block_offset );
        }
    }
    ObjWEndRec( out );
    ObjRSeek( objr, save );
    return( 0 );
}

STATIC int writeTheadr( obj_rec *objr, pobj_state *state ) {

    obj_rec *coment;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_THEADR );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_WRITE_PASS );
        /* here's our initialization code */
    LifixInit( &lifList );
    writeMisc( objr, state );   /* now we write the THEADR record */
        /* now we should build the pharlap signature record */
    coment = ObjNewRec( CMD_COMENT );
    coment->d.coment.attr = 0x80;
    coment->d.coment.class = CMT_EASY_OMF;
    ObjAttachData( coment, EASY_OMF_SIGNATURE, 5 );
    ObjRSeek( coment, 0 );
    writeComent( coment, state );       /* write the EASY OMF comment */
    ObjKillRec( coment );
    return( 0 );
}

STATIC int writeModend( obj_rec *objr, pobj_state *state ) {

    char    buf[ 1 + FIX_GEN_MAX ];
    size_t  len;
    char    is_log;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_MODEND );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_WRITE_PASS );
    len = 1;
    buf[0]  = objr->d.modend.main_module ? 0x80 : 0;
    if( objr->d.modend.start_addrs ) {
        is_log = objr->d.modend.is_logical ? 1 : 0;
        buf[0] |= 0x40 | is_log;
        len += FixGenRef( &objr->d.modend.ref, is_log, buf + 1,FIX_GEN_PHARLAP);
    }
    ObjWriteRec( state->file_out, CMD_MODEND, len, buf );

        /* here's our fini code */
    LifixDestroy( &lifList );

    return( 0 );
}

STATIC void writeBase( obj_rec *objr, OBJ_WFILE *out ) {

    uint_16 grp_idx;
    uint_16 seg_idx;

    grp_idx = objr->d.base.grp_idx;
    seg_idx = objr->d.base.seg_idx;
    ObjWriteIndex( out, grp_idx );
    ObjWriteIndex( out, seg_idx );
    if( grp_idx == 0 && seg_idx == 0 ) {
        ObjWrite16( out, objr->d.base.frame );
    }
}

STATIC int writePubdef( obj_rec *objr, pobj_state *state ) {

    OBJ_WFILE   *out;
    const char  *name;
    uint_8      name_len;
    pubdef_data *pubdata;
    pubdef_data *pubstop;

/**/myassert( objr != NULL );
/**/myassert(   objr->command == CMD_PUBDEF ||
                objr->command == CMD_STATIC_PUBDEF );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_WRITE_PASS );
    out = state->file_out;
    ObjWBegRec( out, objr->command );
    writeBase( objr, out );
    pubdata = objr->d.pubdef.pubs;
    if( pubdata != NULL ) {
        pubstop = pubdata + objr->d.pubdef.num_pubs;
        while( pubdata < pubstop ) {
            name = NameGet( pubdata->name );
            name_len = strlen( name );
            ObjWrite8( out, name_len );
            ObjWrite( out, name, (size_t)name_len );
            ObjWrite32( out, pubdata->offset );
            ObjWriteIndex( out, pubdata->type.idx );
            ++pubdata;
        }
    }
    ObjWEndRec( out );
    return( 0 );
}

STATIC void writeLinnumData( obj_rec *objr, OBJ_WFILE *out ) {

/**/myassert( out != NULL );
#if LITTLE_ENDIAN
    ObjWrite( out, (char *)objr->d.linnum.lines, 6 * objr->d.linnum.num_lines );
/**/myassert( sizeof( linnum_data ) == 6 );
#else
    {
        linnum_data *cur;
        linnum_data *stop;

        cur = objr->d.linnum.lines;
        stop = cur + objr->d.linnum.num_lines;
        while( cur < stop ) {
            ObjWrite16( out, cur->number );
            ObjWrite32( out, cur->offset );
            ++cur;
        }
    }
#endif
}

STATIC int writeLinnum( obj_rec *objr, pobj_state *state ) {

    OBJ_WFILE   *out;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_LINNUM );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_WRITE_PASS );
    out = state->file_out;
    ObjWBegRec( out, CMD_LINNUM );
    writeBase( objr, out );
    writeLinnumData( objr, out );
    ObjWEndRec( out );
    return( 0 );
}

STATIC int writeLinsym( obj_rec *objr, pobj_state *state ) {

    OBJ_WFILE   *out;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_LINSYM );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_WRITE_PASS );
    out = state->file_out;
    ObjWBegRec( out, CMD_LINSYM );
    ObjWrite8( out, objr->d.linsym.d.linsym.flags );
    ObjWriteIndex( out, objr->d.linsym.d.linsym.public_name_idx );
    writeLinnumData( objr, out );
    ObjWEndRec( out );
    return( 0 );
}

STATIC int writeBakpat( obj_rec *objr, pobj_state *state ) {

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_BAKPAT );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_WRITE_PASS );
    if( objr->is_32 || objr->is_phar ) {
        return( writeMisc( objr, state ) );
    }
    /* NYI: convert 16-bit BAKPATs to 32-bit... */
    Fatal( MSG_NO_16BIT_BAKPAT );
}

STATIC int writeComdat( obj_rec *objr, pobj_state *state ) {

    OBJ_WFILE   *out;
    uint_8      *ptr;
    uint_16     len;
    uint_16     save;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_COMDAT );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_WRITE_PASS );
    save = ObjRTell( objr );
    out = state->file_out;
    ObjWBegRec( out, objr->command );
    ObjWrite8( out, objr->d.comdat.flags );
    ObjWrite8( out, objr->d.comdat.attributes );
    ObjWrite8( out, objr->d.comdat.align );
    ObjWrite32( out, objr->d.comdat.offset );
    ObjWriteIndex( out, objr->d.comdat.type_idx );
    if( ( objr->d.comdat.attributes & COMDAT_ALLOC_MASK ) == COMDAT_EXPLICIT ) {
        writeBase( objr, out );
    }
    ObjWriteIndex( out, objr->d.comdat.public_name_idx );
    if( ( objr->d.comdat.flags & COMDAT_ITERATED ) == 0
        || objr->is_phar || !objr->is_32 ) {
        /* record is already in pharlap format */
        len = ObjRemain( objr );
        ptr = ObjGet( objr, len );
/**/    myassert( len <= 1024 );
        ObjWrite( out, ptr, len );
    } else {
        int_16 delta;
        uint_16 first_block_offset;

        delta = 0;  /* id32Block needs to play with this */
        first_block_offset = ObjRTell( objr );
        while( !ObjEOR( objr ) ) {
            id32Block( objr, out, &delta, first_block_offset );
        }
    }
    ObjWEndRec( out );
    ObjRSeek( objr, save );
    return( 0 );
}


STATIC const pobj_list myFuncs[] = {
    { CMD_THEADR,       POBJ_WRITE_PASS, writeTheadr },
    { CMD_COMENT,       POBJ_WRITE_PASS, writeComent },
    { CMD_MODEND,       POBJ_WRITE_PASS, writeModend },
    { CMD_EXTDEF,       POBJ_WRITE_PASS, writeMisc },
    { CMD_CEXTDF,       POBJ_WRITE_PASS, writeMisc },
    { CMD_TYPDEF,       POBJ_WRITE_PASS, writeMisc },
    { CMD_PUBDEF,       POBJ_WRITE_PASS, writePubdef },
    { CMD_LINNUM,       POBJ_WRITE_PASS, writeLinnum },
    { CMD_LNAMES,       POBJ_WRITE_PASS, writeMisc },
    { CMD_SEGDEF,       POBJ_WRITE_PASS, writeSegdef },
    { CMD_GRPDEF,       POBJ_WRITE_PASS, writeMisc },
    { CMD_FIXUP,        POBJ_WRITE_PASS, writeFixup },
    { CMD_LEDATA,       POBJ_WRITE_PASS, writeLedata },
    { CMD_LIDATA,       POBJ_WRITE_PASS, writeLidata },
    { CMD_COMDEF,       POBJ_WRITE_PASS, writeMisc },
    { CMD_STATIC_EXTDEF,POBJ_WRITE_PASS, writeMisc },
    { CMD_STATIC_PUBDEF,POBJ_WRITE_PASS, writePubdef },
    { CMD_STATIC_COMDEF,POBJ_WRITE_PASS, writeMisc },
    { CMD_BAKPAT,       POBJ_WRITE_PASS, writeBakpat },
    { CMD_COMDAT,       POBJ_WRITE_PASS, writeComdat },
    { CMD_LINSYM,       POBJ_WRITE_PASS, writeLinsym },
    { CMD_ALIAS,        POBJ_WRITE_PASS, writeMisc },
    { CMD_NBKPAT,       POBJ_WRITE_PASS, writeBakpat },
    { CMD_LLNAMES,      POBJ_WRITE_PASS, writeMisc }
};
#define NUM_FUNCS   ( sizeof( myFuncs ) / sizeof( pobj_list ) )

void GenPharInit( void ) {
/**********************/
    PObjRegList( myFuncs, NUM_FUNCS );
}

void GenPharFini( void ) {
/**********************/
    PObjUnRegList( myFuncs, NUM_FUNCS );
}

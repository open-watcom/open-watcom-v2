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
#include "msdbg.h"
#include "objrec.h"

#if ( _WOMP_OPT & _WOMP_WATFOR ) == 0
#include "lifix.h"
STATIC lifix_list   lifList;
#endif

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

#ifndef _WOMP_WATFOR_8086
STATIC int writeMisc32( obj_rec *objr, pobj_state *state ) {
/*
    For 32-bit records which are the same under Intel, PharLap, and MS OMFs
*/
    uint_8  *ptr;
    uint_16 len;
    uint_8  cmd;
    uint_16 save;

/**/myassert( objr != NULL );
/**/myassert( objr->data != NULL );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_WRITE_PASS );
    save = ObjRTell( objr );
    ObjRSeek( objr, 0 );
    len = ObjRemain( objr );
    ptr = ObjGet( objr, len );
    cmd = objr->command;
    if( objr->is_32 || objr->is_phar ) {
        cmd |= 0x01;
    }
    ObjWriteRec( state->file_out, cmd, len, ptr );
    ObjRSeek( objr, save );
    return( 0 );
}
#endif

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
    int         is32;
    uint_8      acbp;
    uint_8      align;
#if ( _WOMP_OPT & _WOMP_WATFOR ) == 0
    char        buf[ FIX_GEN_MAX ];
    size_t      len;
#endif
#if ( _WOMP_OPT & _WOMP_NASM )
    obj_offset  patch;
#endif

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_SEGDEF );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_WRITE_PASS );
    out = state->file_out;
#if ( _WOMP_OPT & _WOMP_WATFOR ) == 0
    is32 = objr->d.segdef.use_32;
#else
  #ifdef _WOMP_WATFOR_8086
    is32 = 0;
  #else
    is32 = 1;
  #endif
#endif
    ObjWBegRec( out, is32 ? CMD_SEGD32 : CMD_SEGDEF );
    acbp = ( objr->d.segdef.combine << 2 ) | ( is32 != 0 );
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
#if _WOMP_OPT & _WOMP_EXTRAS
        PrtMsg( WRN|MSG_MS386_NO_4KPAGE );
#endif
        break;
    default:
/**/    never_reach();
    }
    if( is32 == 0 && objr->d.segdef.seg_length == 0x10000 ) {
        acbp |= 0x02;   /* BIG bit */ /* FIXME no support for 2**32 */
    }
    ObjWrite8( out, acbp );
#if ( _WOMP_OPT & _WOMP_WATFOR ) == 0
    if( align == SEGDEF_ALIGN_ABS ) {
        len = FixGenPRef( &objr->d.segdef.abs, buf,
            is32 ? FIX_GEN_MS386 : FIX_GEN_INTEL );
        ObjWrite( out, buf, len );
    }
#endif
    if( is32 ) {
        #if ( _WOMP_OPT & _WOMP_NASM )
            patch = ObjWSkip32( out );
        #else
            ObjWrite32( out, objr->d.segdef.seg_length );
        #endif
    } else {
        #if ( _WOMP_OPT & _WOMP_NASM )
            patch = ObjWSkip16( out );
        #else
            ObjWrite16( out, objr->d.segdef.seg_length & 0xffff );
        #endif
    }
    #if ( _WOMP_OPT & _WOMP_NASM )
/**/    myassert( objr->data != NULL );
        memcpy( objr->data, &patch, sizeof patch);
    #endif

    ObjWriteIndex( out, objr->d.segdef.seg_name_idx );
    ObjWriteIndex( out, objr->d.segdef.class_name_idx );
#if ( _WOMP_OPT & _WOMP_WATFOR ) == 0
    ObjWriteIndex( out, objr->d.segdef.ovl_name_idx );
#if ( _WOMP_OPT & _WOMP_EXTRAS )
    if( objr->d.segdef.access_valid ) {
        PrtMsg( MSG_MS386_NO_ACCESS );
    }
#endif
#else
    ObjWriteIndex( out, 1 );
#endif
    ObjWEndRec( out );
    return( 0 );
}

#if ( _WOMP_OPT & _WOMP_WATFOR ) == 0
STATIC int writeFixup( obj_rec *objr, pobj_state *state ) {

    OBJ_WFILE   *out;
    int         is32;
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
    is32 = objr->is_32 || objr->is_phar;
    /* we don't want to write FIXUP records that are too large, so we limit
       our records to approximately 1024 bytes */
    do {
        len_written = 0;
        ObjWBegRec( out, is32 ? CMD_FIXU32 : CMD_FIXUP );
        while( walk != NULL && len_written < 1024 - FIX_GEN_MAX ) {
            walk->loc_offset += LifixDelta( &lifList, walk->loc_offset );
#if _WOMP_OPT & _WOMP_WOMP
            if( Can2MsOS2Flat() &&
                ( walk->loc_method == FIX_OFFSET386
                || walk->loc_method == FIX_POINTER386 ) ) {
                /* zap FIXUPs for OS/2 2.0 linker 21-mar-91 AFS */
                switch( walk->lr.frame ) {
                case F_SEG:
                case F_GRP:
                case F_TARG:
                    walk->lr.frame = F_GRP;
                    walk->lr.frame_datum = ObjFLATIndex;
                    break;
                }
            }
#endif
            len = FixGenFix( walk, buf, is32 ? FIX_GEN_MS386 : FIX_GEN_INTEL );
            ObjWrite( out, buf, len );
            walk = walk->next;
            len_written += len;
        }
        ObjWEndRec( out );
    } while( walk != NULL );
    return( 0 );
}
#endif

STATIC int writeLedata( obj_rec *objr, pobj_state *state ) {

    OBJ_WFILE   *out;
    uint_16     save;
    uint_8      *ptr;
    uint_16     len;
    int         is32;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_LEDATA );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_WRITE_PASS );
#if ( _WOMP_OPT & _WOMP_WATFOR ) == 0
    LifixDestroy( &lifList );
    LifixInit( &lifList );
#endif
    out = state->file_out;
    is32 = ( objr->is_32 || objr->is_phar ) != 0;
    ObjWBegRec( out, is32 ? CMD_LEDA32 : CMD_LEDATA );
    ObjWriteIndex( out, objr->d.ledata.idx );
    if( is32 ) {
        ObjWrite32( out, objr->d.ledata.offset );
    } else {
        ObjWrite16( out, objr->d.ledata.offset );
    }
    save = ObjRTell( objr );
    len = ObjRemain( objr );
    ptr = ObjGet( objr, len );
/**/myassert( len <= 1024 );
    ObjWrite( out, ptr, len );
    ObjWEndRec( out );
    ObjRSeek( objr, save );
    return( 0 );
}

#if ( _WOMP_OPT & _WOMP_WATFOR ) == 0

STATIC void id32Block( obj_rec *objr, OBJ_WFILE *out, int_16 *delta,
    uint_16 first_block_offset ) {
/*
    Since LIDATAs are different under PharLap and MicroSoft 386 formats,
    we have to do some magic.
*/
    uint_16 rpt_count;
    uint_16 blk_count;
    uint_8  data_count;
    uint_8  *ptr;

    rpt_count = ObjGet16( objr );
    ObjWrite32( out, rpt_count );
    *delta += 2;    /* ok, everything from here on will be at +2 offset */
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
    int         is32;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_LIDATA );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_WRITE_PASS );
    LifixDestroy( &lifList );
    LifixInit( &lifList );
    out = state->file_out;
    save = ObjRTell( objr );
    is32 = objr->is_32 || objr->is_phar;
    ObjWBegRec( out, is32 ? CMD_LIDA32 : CMD_LIDATA );
    ObjWriteIndex( out, objr->d.lidata.idx );
    if( is32 ) {
        ObjWrite32( out, objr->d.lidata.offset );
    } else {
        ObjWrite16( out, objr->d.lidata.offset );
    }
    if( objr->is_phar == 0 ) {
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

#endif

STATIC int writeTheadr( obj_rec *objr, pobj_state *state ) {

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_THEADR );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_WRITE_PASS );
        /* here's our initialization code */
#if ( _WOMP_OPT & _WOMP_WATFOR ) == 0
    LifixInit( &lifList );
#endif

    return( writeMisc( objr, state ) );
}

STATIC int writeModend( obj_rec *objr, pobj_state *state ) {

    size_t  len;
    char    is32;
#if _WOMP_OPT & _WOMP_WATFOR
    char    buf[ 1 ];
#else
    char    buf[ 1 + FIX_GEN_MAX ];
    char    is_log;
#endif

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_MODEND );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_WRITE_PASS );
    is32 = ( objr->is_32 || objr->is_phar );
    len = 1;
    buf[0]  = objr->d.modend.main_module ? 0x80 : 0;
#if ( _WOMP_OPT & _WOMP_WATFOR ) == 0
    if( objr->d.modend.start_addrs ) {
        is_log = objr->d.modend.is_logical ? 1 : 0;
#if _WOMP_OPT & _WOMP_WOMP
        if( is_log && Can2MsOS2Flat() ) {
            switch( objr->d.modend.ref.log.frame ) {
            case F_SEG:
            case F_GRP:
            case F_TARG:
                objr->d.modend.ref.log.frame = F_GRP;
                objr->d.modend.ref.log.frame_datum = ObjFLATIndex;
                break;
            }
        }
#endif
        buf[0] |= 0x40 | is_log;
        len += FixGenRef( &objr->d.modend.ref, is_log, buf + 1,
            is32 ? FIX_GEN_MS386 : FIX_GEN_INTEL );
    }
#endif
    if( buf[0] == 0 ) {
        is32 = 0;       /* no need for MODE32 in this case */
    }
    ObjWriteRec( state->file_out, is32 ? CMD_MODE32 : CMD_MODEND,
        len, buf );

        /* here's our fini code */
#if ( _WOMP_OPT & _WOMP_WATFOR ) == 0
    LifixDestroy( &lifList );
#endif

    return( 0 );
}

#if ( _WOMP_OPT & _WOMP_WATFOR ) == 0

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

STATIC int writeComdat( obj_rec *objr, pobj_state *state ) {

    OBJ_WFILE   *out;
    uint_8      *ptr;
    uint_16     len;
    uint_16     save;
    int         is_32;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_COMDAT );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_WRITE_PASS );
    save = ObjRTell( objr );
    is_32 = objr->is_32 || objr->is_phar;
    out = state->file_out;
    ObjWBegRec( out, objr->command | ( is_32 ? 1 : 0 ) );
    ObjWrite8( out, objr->d.comdat.flags );
    ObjWrite8( out, objr->d.comdat.attributes );
    ObjWrite8( out, objr->d.comdat.align );
    if( is_32 ) {
        ObjWrite32( out, objr->d.comdat.offset );
    } else {
        ObjWrite16( out, objr->d.comdat.offset );
    }
    ObjWriteIndex( out, objr->d.comdat.type_idx );
    if( ( objr->d.comdat.attributes & COMDAT_ALLOC_MASK ) == COMDAT_EXPLICIT ) {
        writeBase( objr, out );
    }
    ObjWriteIndex( out, objr->d.comdat.public_name_idx );
    if( ( objr->d.comdat.flags & COMDAT_ITERATED ) == 0 || objr->is_phar == 0 ){
        /* record is already in ms omf format */
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

STATIC int writePubdef( obj_rec *objr, pobj_state *state ) {

    int         is32;
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
    is32 = objr->is_32 || objr->is_phar;
    out = state->file_out;
    ObjWBegRec( out, objr->command | ( is32 ? 1 : 0 ) );
    writeBase( objr, out );
    pubdata = objr->d.pubdef.pubs;
    if( pubdata != NULL ) {
        pubstop = pubdata + objr->d.pubdef.num_pubs;
        while( pubdata < pubstop ) {
            name = NameGet( pubdata->name );
            name_len = strlen( name );
            ObjWrite8( out, name_len );
            ObjWrite( out, name, (size_t)name_len );
            if( is32 ) {
                ObjWrite32( out, pubdata->offset );
            } else {
                ObjWrite16( out, pubdata->offset );
            }
            ObjWriteIndex( out, pubdata->type.idx );
            ++pubdata;
        }
    }
    ObjWEndRec( out );
    return( 0 );
}

STATIC void writeLinnumData( obj_rec *objr, OBJ_WFILE *out ) {

    int is32;

/**/myassert( objr != NULL );
/**/myassert( out != NULL );
    is32 = objr->is_32 || objr->is_phar;
#if LITTLE_ENDIAN
    if( is32 ) {
        ObjWrite( out, (char *)objr->d.linnum.lines,
            6 * objr->d.linnum.num_lines );
/**/    myassert( sizeof( linnum_data ) == 6 );
    } else {
        linnum_data *cur;
        linnum_data *stop;

        cur = objr->d.linnum.lines;
        stop = cur + objr->d.linnum.num_lines;
        while( cur < stop ) {
            ObjWrite16( out, cur->number );
/**/        myassert( ( cur->offset & 0xffff0000 ) == 0 );
            ObjWrite16( out, (uint_16)cur->offset );
            ++cur;
        }
    }
#else
    {
        linnum_data *cur;
        linnum_data *stop;

        cur = objr->d.linnum.lines;
        stop = cur + objr->d.linnum.num_lines;
        while( cur < stop ) {
            ObjWrite16( out, cur->number );
            if( is32 ) {
                ObjWrite32( out, cur->offset );
            } else {
                ObjWrite16( out, (uint_16)cur->offset );
            }
            ++cur;
        }
    }
#endif
}

STATIC int writeLinnum( obj_rec *objr, pobj_state *state ) {

    int         is32;
    OBJ_WFILE   *out;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_LINNUM );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_WRITE_PASS );
    out = state->file_out;
    is32 = objr->is_32 || objr->is_phar;
    ObjWBegRec( out, is32 ? CMD_LINN32 : CMD_LINNUM );
    writeBase( objr, out );
    writeLinnumData(objr, out );
    ObjWEndRec( out );
    return( 0 );
}

STATIC int writeLinsym( obj_rec *objr, pobj_state *state ) {

    int         is32;
    OBJ_WFILE   *out;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_LINSYM );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_WRITE_PASS );
    out = state->file_out;
    is32 = objr->is_32 || objr->is_phar;
    ObjWBegRec( out, is32 ? CMD_LINS32 : CMD_LINSYM );
    ObjWrite8( out, objr->d.linsym.d.linsym.flags );
    ObjWriteIndex( out, objr->d.linsym.d.linsym.public_name_idx );
    writeLinnumData( objr, out );
    ObjWEndRec( out );
    return( 0 );
}

#endif

STATIC const pobj_list myFuncs[] = {
    { CMD_THEADR,       POBJ_WRITE_PASS, writeTheadr },
    { CMD_COMENT,       POBJ_WRITE_PASS, writeComent },
    { CMD_MODEND,       POBJ_WRITE_PASS, writeModend },
    { CMD_EXTDEF,       POBJ_WRITE_PASS, writeMisc },
    { CMD_LNAMES,       POBJ_WRITE_PASS, writeMisc },
    { CMD_SEGDEF,       POBJ_WRITE_PASS, writeSegdef },
    { CMD_GRPDEF,       POBJ_WRITE_PASS, writeMisc },
    { CMD_LEDATA,       POBJ_WRITE_PASS, writeLedata },
#if ( _WOMP_OPT & _WOMP_WATFOR ) == 0
    { CMD_FIXUP,        POBJ_WRITE_PASS, writeFixup },
    { CMD_PUBDEF,       POBJ_WRITE_PASS, writePubdef },
    { CMD_TYPDEF,       POBJ_WRITE_PASS, writeMisc },
    { CMD_LINNUM,       POBJ_WRITE_PASS, writeLinnum },
    { CMD_LIDATA,       POBJ_WRITE_PASS, writeLidata },
    { CMD_COMDEF,       POBJ_WRITE_PASS, writeMisc },
    { CMD_CEXTDF,       POBJ_WRITE_PASS, writeMisc },
    { CMD_STATIC_EXTDEF,POBJ_WRITE_PASS, writeMisc32 },
    { CMD_STATIC_PUBDEF,POBJ_WRITE_PASS, writePubdef },
    { CMD_STATIC_COMDEF,POBJ_WRITE_PASS, writeMisc },
    { CMD_BAKPAT,       POBJ_WRITE_PASS, writeMisc32 },
    { CMD_COMDAT,       POBJ_WRITE_PASS, writeComdat },
    { CMD_LINSYM,       POBJ_WRITE_PASS, writeLinsym },
    { CMD_ALIAS,        POBJ_WRITE_PASS, writeMisc },
    { CMD_NBKPAT,       POBJ_WRITE_PASS, writeMisc32 },
    { CMD_LLNAMES,      POBJ_WRITE_PASS, writeMisc }
#else
  #ifdef _WOMP_WATFOR_8086
    { CMD_FIXUP,        POBJ_WRITE_PASS, writeMisc },
    { CMD_PUBDEF,       POBJ_WRITE_PASS, writeMisc }
  #else
    { CMD_FIXUP,        POBJ_WRITE_PASS, writeMisc32 },
    { CMD_PUBDEF,       POBJ_WRITE_PASS, writeMisc32 }
  #endif
#endif
};
#define NUM_FUNCS   ( sizeof( myFuncs ) / sizeof( pobj_list ) )

void GenMSOmfInit( void ) {
/***********************/
    PObjRegList( myFuncs, NUM_FUNCS );
}

void GenMSOmfFini( void ) {
/***********************/
    PObjUnRegList( myFuncs, NUM_FUNCS );
}

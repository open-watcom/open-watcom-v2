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


#include <ctype.h>
#include <string.h>
#include <watcom.h>
#include "womp.h"
#include "genutil.h"
#include "myassert.h"
#include "watdbg.h"
#include "segment.h"
#include "fixup.h"
#include "pcobj.h"
#include "canmisc.h"
#include "objprs.h"
#include "objrec.h"
#include "pubdef.h"

STATIC struct {
    seghdr      *ddtypes;
    seghdr      *ddsymbols;
    uint_16     ddtypes_seg_idx;
    uint_16     ddsymbols_seg_idx;
    uint_16     ddtypes_name_idx;
    uint_16     ddsymbols_name_idx;
    uint_16     debtyp_idx;
    uint_16     debsym_idx;
    obj_rec     *last_ledata;
    unsigned    found_dbg_ver   : 1;
    unsigned    symbs_present   : 1;
    unsigned    types_present   : 1;
    unsigned    c70             : 1;    /* are we to assume C 7.0? */
} loc;

STATIC int prsTheadr( obj_rec *objr, pobj_state *state ) {

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_THEADR );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_READ_PASS );
    objr = objr;
    state = state;

        /* here's our initialization code */
    loc.ddtypes = SegNew();
    loc.ddsymbols = SegNew();
    loc.ddtypes_seg_idx = 0;
    loc.ddsymbols_seg_idx = 0;
    loc.ddtypes_name_idx = 0;
    loc.ddsymbols_name_idx = 0;
    loc.debtyp_idx = 0;
    loc.debsym_idx = 0;
    loc.last_ledata = 0;
    loc.found_dbg_ver = 0;
    loc.symbs_present = 0;
    loc.types_present = 0;

    CanMisc.processor = CAN_PROC_8086;
    CanMisc.memory_model = CAN_MODEL_SMALL;
    CanMisc.floating_point = CAN_FLOAT_EM_CALLS;
    CanMisc.optimized = 0;

    return( 0 );
}

STATIC int prsComent( obj_rec *objr, pobj_state *state ) {

    int     ret;
    uint_8  major;
    uint_8  minor;
    uint_8  source_lang;
    uint_16 posn;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_COMENT );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_READ_PASS );
    state = state;
    posn = ObjRTell( objr );
    switch( objr->d.coment.class ) {
    case CMT_LINKER_DIRECTIVE:
        if( ObjEOR( objr ) ) {  /* null linker directive? */
            ret = 0;
        }
        switch( ObjGet8( objr ) ) {
        case LDIR_SOURCE_LANGUAGE:
            major = ObjGet8( objr ); /* major version */
            minor = ObjGet8( objr ); /* minor version */
            if( major != WAT_MAJOR || minor > WAT_MINOR ) {
                Fatal( MSG_DEBUG_VERSION );
            }
            loc.found_dbg_ver = 1;
            source_lang = ObjGet8( objr ); /* FIXME should read entire string */
            switch( tolower( source_lang ) ) {
            case 'c':   CanMisc.src_language = CAN_LANG_C;      break;
            case 'f':   CanMisc.src_language = CAN_LANG_FORTRAN;break;
            default:    CanMisc.src_language = CAN_LANG_ASM;    break;
            }
            ret = -1;
            break;
        default:
            ret = 0;        /* we don't understand so pass on */
        }
        break;

    case CMT_WAT_PROC_MODEL:
        if( ObjRemain( objr ) < 4 ) {
            PrtMsg( WRN|MSG_INV_WAT_PROC_MODEL );
            ret = 0;
            break;
        }
        ret = -1;   /* don't pass on after this point */
        CanMisc.processor = ObjGet8( objr ) - '0';
        switch( tolower( ObjGet8( objr ) ) ) {
        case 's':   CanMisc.memory_model = CAN_MODEL_SMALL;     break;
        case 'm':   CanMisc.memory_model = CAN_MODEL_MEDIUM;    break;
        case 'c':   CanMisc.memory_model = CAN_MODEL_COMPACT;   break;
        case 'l':   CanMisc.memory_model = CAN_MODEL_LARGE;     break;
        case 'h':   CanMisc.memory_model = CAN_MODEL_HUGE;      break;
        case 'f':   CanMisc.memory_model = CAN_MODEL_FLAT;      break;
        case 't':   CanMisc.memory_model = CAN_MODEL_TINY;      break;
        default:
            PrtMsg( WRN|MSG_UNS_MEMORY_MODEL );
            break;
        }
        CanMisc.optimized = ObjGet8( objr ) == 'O';
        switch( tolower( ObjGet8( objr ) ) ) {
        case 'e':   CanMisc.floating_point = CAN_FLOAT_EM_INLINE;   break;
        case 'c':   CanMisc.floating_point = CAN_FLOAT_EM_CALLS;    break;
        case 'p':   CanMisc.floating_point = CAN_FLOAT_87_INLINE;   break;
        default:
            PrtMsg( WRN|MSG_UNS_FLOAT_OPTION );
            break;
        }
        break;

    default:
        ret = 0;
        break;
    }
    if( ret == -1 ) {
        ObjKillRec( objr );
    } else {
        ObjRSeek( objr, posn );
    }
    return( ret );
}

STATIC int prsLinnum( obj_rec *objr, pobj_state *state ) {

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_LINNUM || objr->command == CMD_LINSYM );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_READ_PASS );
    state = state;
    objr = objr;
    CanMisc.lines_present = 1;
    return( 0 );
}

STATIC int prsLnames( obj_rec *objr, pobj_state *state ) {

    uint_8  len;
    uint_8  *p;
    uint_16 count;
    uint_16 idx;
    char    *rewrite;
    uint_16 save;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_LNAMES || objr->command == CMD_LLNAMES );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_READ_PASS );
    state = state;
    ObjRSeek( objr, 0 );    /* go back to beginning always */
    count = 0;
    while( !ObjEOR( objr ) ) {
        save = ObjRTell( objr );
        len = ObjGet8( objr );
        if( len ) {
            p = ObjGet( objr, len ) - 1;
            ++len;
            idx = objr->d.lnames.first_idx + count;
            if( memcmp( p, WAT_DDTYPES, len ) == 0 ) {
                rewrite = WAT_ZAP_DDTYPES;
                loc.ddtypes_name_idx = idx;
            } else if( memcmp( p, WAT_DDSYMBOLS, len ) == 0 ) {
                rewrite = WAT_ZAP_DDSYMBOLS;
                loc.ddsymbols_name_idx = idx;
            } else if( memcmp( p, WAT_DEBTYP, len ) == 0 ) {
                rewrite = WAT_ZAP_DEBTYP;
                loc.debtyp_idx = idx;
            } else if( memcmp( p, WAT_DEBSYM, len ) == 0 ) {
                rewrite = WAT_ZAP_DEBSYM;
                loc.debsym_idx = idx;
            } else {
                rewrite = NULL;
            }
            if( rewrite != NULL ) {
/**/            myassert( rewrite[0] + 1 == len );
                ObjRSeek( objr, save );
                ObjPut( objr, rewrite, rewrite[0] + 1 );
            }
        }
        ++count;
    }
/**/myassert( ObjEOR( objr ) ); /* as req'd */
    return( 0 );    /* keep on passing it */
}

STATIC int prsSegdef( obj_rec *objr, pobj_state *state ) {

    uint_16 name_idx;
    uint_16 class_idx;
    uint_32 seg_length;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_SEGDEF );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_READ_PASS );
    state = state;
    name_idx = objr->d.segdef.seg_name_idx;
    class_idx = objr->d.segdef.class_name_idx;
    seg_length = objr->d.segdef.seg_length;
    if( seg_length > 0 && name_idx > 0 ) {
        if( name_idx == loc.ddsymbols_name_idx &&
                class_idx == loc.debsym_idx ) {
            if( seg_length > 0xfff0 ) {
                Fatal( MSG_SEGMENT_TOO_LARGE );
            }
            loc.ddsymbols->segdef = objr;
            loc.ddsymbols_seg_idx = objr->d.segdef.idx;
            SegAllocData( loc.ddsymbols, seg_length );
            loc.symbs_present = 1;
            objr->d.segdef.combine = COMB_ADDOFF;
            objr->d.segdef.seg_length = 0;

        } else if( name_idx == loc.ddtypes_name_idx &&
                class_idx == loc.debtyp_idx ) {
            if( seg_length > 0xfff0 ) {
                Fatal( MSG_SEGMENT_TOO_LARGE );
            }
            loc.ddtypes->segdef = objr;
            loc.ddtypes_seg_idx = objr->d.segdef.idx;
            SegAllocData( loc.ddtypes, seg_length );
            loc.types_present = 1;
            objr->d.segdef.combine = COMB_ADDOFF;
            objr->d.segdef.seg_length = 0;
        }
    }
    return( 0 );    /* always pass this on */
}

STATIC void freeLastLedata( void ) {

    if( loc.last_ledata != NULL ) {
            /* last record was one of our debugging records; we've already
               grabbed the fixups for it, so we just kill it */
        ObjKillRec( loc.last_ledata );
        loc.last_ledata = NULL;
    }
}

STATIC int prsLidata( obj_rec *objr, pobj_state *state ) {

    uint_16 idx;

/**/myassert( objr != NULL );
/**/myassert(   objr->command == CMD_LIDATA );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_READ_PASS );
    state = state;
    objr = objr;
    freeLastLedata();
    idx = objr->d.lidata.idx;
    if( idx > 0 &&
        ( idx == loc.ddtypes_seg_idx || idx == loc.ddsymbols_seg_idx ) ) {
        Fatal( MSG_NO_LIDATA_WAT );
    }
    return( 0 );
}

STATIC int prsLedata( obj_rec *objr, pobj_state *state ) {

    int     ret;
    uint_16 idx;
    uint_8  *p;
    uint_16 len;
    seghdr  *hdr;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_LEDATA );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_READ_PASS );
    state = state;
    freeLastLedata();
    ret = 0;
    idx = objr->d.ledata.idx;
    if( idx > 0 ) {
        hdr = NULL;
        if( idx == loc.ddtypes_seg_idx ) {
            hdr = loc.ddtypes;
        } else if( idx == loc.ddsymbols_seg_idx ) {
            hdr = loc.ddsymbols;
        }
        if( hdr != NULL ) {
            len = ObjRemain( objr );
            if( objr->d.ledata.offset + len > hdr->alloc ) {
                Fatal( MSG_TOO_MUCH_SEG_DATA, idx );
            }
            p = ObjGet( objr, len );
            memcpy( hdr->data + objr->d.ledata.offset, p, len );
            loc.last_ledata = objr;
            ret = -1;   /* don't pass on */
        }
    }
    return( ret );
}

STATIC int prsFixup( obj_rec *objr, pobj_state *state ) {

    int     ret;
    uint_16 idx;
    seghdr  *hdr;
    fixup   *fix;
    fixup   *next;
    uint_8  cmd;
    uint_32 le_offset;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_FIXUP );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_READ_PASS );
    state = state;
/**/myassert( objr->d.fixup.data_rec != NULL );
    ret = 0;
    cmd = objr->d.fixup.data_rec->command;
    if( cmd == CMD_COMDAT ) {
        return( ret );
    }
/**/myassert( cmd == CMD_LEDATA || cmd == CMD_LIDATA );
    idx = objr->d.fixup.data_rec->d.ledata.idx;
    if( idx > 0 ) {
        hdr = NULL;
        if( idx == loc.ddtypes_seg_idx ) {
            hdr = loc.ddtypes;
        } else if( idx == loc.ddsymbols_seg_idx ) {
            hdr = loc.ddsymbols;
        }
        if( hdr != NULL ) {
            if( cmd == CMD_LIDATA ) {
                Fatal( MSG_NO_LIDATA_WAT );
            }
            le_offset = objr->d.fixup.data_rec->d.ledata.offset;
            fix = objr->d.fixup.fixup;
            while( fix != NULL ) {
                next = fix->next;
                fix->loc_offset += le_offset;
                SegAddFix( hdr, fix );
                fix = next;
            }
            objr->d.fixup.fixup = NULL;
            ObjKillRec( objr );
            ret = -1;
        }
    }
    return( ret );
}

STATIC int prsModend( obj_rec *objr, pobj_state *state ) {

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_MODEND );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_READ_PASS );
    objr = objr;
    state = state;
    freeLastLedata();
    if( loc.symbs_present || loc.types_present ) {
        if( !loc.c70 && loc.found_dbg_ver == 0 ) {
                /* must be major 1, minor 0 */
            Fatal( MSG_DEBUG_VERSION );
        }
        Wat2CanTandS( loc.types_present ? loc.ddtypes : NULL,
                 loc.symbs_present ? loc.ddsymbols : NULL );
    }
    SegKill( loc.ddtypes );
    SegKill( loc.ddsymbols );
    return( 0 );    /* keep on passing it */
}

STATIC int writeTheadr( obj_rec *objr, pobj_state *state ) {

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_THEADR );
/**/myassert( state != NULL );
/**/myassert( state->pass == POBJ_WRITE_PASS );
    objr = objr;
    state = state;
    TypePubdefs();
    return( 0 );
}

STATIC const pobj_list prsFuncs[] = {
    { CMD_THEADR, POBJ_READ_PASS,  prsTheadr },
    { CMD_MODEND, POBJ_READ_PASS,  prsModend },
    { CMD_COMENT, POBJ_READ_PASS,  prsComent },
    { CMD_LINNUM, POBJ_READ_PASS,  prsLinnum },
    { CMD_LNAMES, POBJ_READ_PASS,  prsLnames },
    { CMD_SEGDEF, POBJ_READ_PASS,  prsSegdef },
    { CMD_LIDATA, POBJ_READ_PASS,  prsLidata },
    { CMD_LEDATA, POBJ_READ_PASS,  prsLedata },
    { CMD_FIXUP,  POBJ_READ_PASS,  prsFixup  },
    { CMD_THEADR, POBJ_WRITE_PASS, writeTheadr },
    { CMD_LINSYM, POBJ_READ_PASS,  prsLinnum },
    { CMD_LLNAMES, POBJ_READ_PASS,  prsLnames }
};
#define NUM_FUNCS   ( sizeof( prsFuncs ) / sizeof( pobj_list ) )

void Wat2CanInit( int c70 ) {
/*************************/

    loc.c70 = c70 != 0;
    PObjRegList( prsFuncs, NUM_FUNCS );
}

void Wat2CanFini( void ) {
/**********************/
    PObjUnRegList( prsFuncs, NUM_FUNCS );
}

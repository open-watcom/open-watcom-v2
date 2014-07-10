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


#include <stdlib.h>
#include <string.h>
#include "pcobj.h"
#include "watcom.h"
#include "womp.h"
#include "objprs.h"
#include "memutil.h"
#include "genutil.h"
#include "fixup.h"
#include "objio.h"
#include "myassert.h"
#include "queue.h"
#include "objrec.h"
#include "canmisc.h"

typedef struct jump_list   jlist;
struct jump_list {
    jlist       *next;
    pobj_filter func;
};

#define JUMP_OFFSET(cmd)    ((cmd)-CMD_POBJ_MIN_CMD)

STATIC jlist    *readJump[ CMD_MAX_CMD - CMD_POBJ_MIN_CMD + 1 ];
STATIC jlist    *writeJump[ CMD_MAX_CMD - CMD_POBJ_MIN_CMD + 1 ];

STATIC struct {
    qdesc       records;        /* queue of records from READ_PASS          */
    qdesc       insert;         /* queue of records after current rec       */
    qdesc       pub;            /* queue of pubdefs                         */
    uint_16     segdef_idx;     /* last segdef index                        */
    uint_16     grpdef_idx;     /* last grpdef index                        */
    uint_16     extdef_idx;     /* last extdef index                        */
    uint_16     lnames_idx;     /* last lnames index                        */
    uint_16     dgroup_idx;     /* DGROUP name index                        */
    uint_16     flat_idx;       /* FLAT name index                          */
    obj_rec     *last_defn;     /* last defn (SEGDEF, GRPDEF, LNAMES,...) rec */
    obj_rec     *last_data_rec; /* pointer to obj_rec of last le/lidata     */
    fixinfo     *fix_info;      /* info for fixups                          */
    size_t      page_len;       /* if is_lib then page size                 */
    pobj_state  state;          /* state we'll pass to filters              */
    uint_8      is_phar : 1;    /* is this a pharlap object deck?           */
} loc;

#define DGROUP_NAME_VALUE       "DGROUP"
#define DGROUP_NAME_LEN         (6)
#define FLAT_NAME_VALUE         "FLAT"
#define FLAT_NAME_LEN           (4)

uint_16 ObjDGROUPIndex;
uint_16 ObjFLATIndex;

void PObjInit( void ) {
/*******************/
    memset( readJump, 0, sizeof( readJump ) );
    memset( writeJump, 0, sizeof( writeJump ) );
}

void PObjFini( void ) {
/*******************/
    jlist       *cur;
    jlist       *next;
    unsigned    i;

    for( i = CMD_POBJ_MIN_CMD; i <= CMD_MAX_CMD; ++i ) {
        cur = readJump[ JUMP_OFFSET( i ) ];
        while( cur != NULL ) {
            next = cur->next;
            MemFree( cur );
            cur = next;
        }
        cur = writeJump[ JUMP_OFFSET( i ) ];
        while( cur != NULL ) {
            next = cur->next;
            MemFree( cur );
            cur = next;
        }
    }
}

void PObjRegister( uint_8 command, uint_8 pass, pobj_filter func ) {
/****************************************************************/
    jlist   *new;

    new = MemAlloc( sizeof( *new ) );
    new->func = func;
    switch( pass ) {
    case POBJ_READ_PASS:
        new->next = readJump[ JUMP_OFFSET( command ) ];
        readJump[ JUMP_OFFSET( command ) ] = new;
        break;
    case POBJ_ALL_PASSES:
        PObjRegister( command, POBJ_READ_PASS, func );
            /* fall through */
    case POBJ_WRITE_PASS:
        new->next = writeJump[ JUMP_OFFSET( command ) ];
        writeJump[ JUMP_OFFSET( command ) ] = new;
        break;
    default:
/**/    never_reach();
    }
}

void PObjUnRegister( uint_8 command, uint_8 pass, pobj_filter func ) {
/******************************************************************/
    jlist   **walk;
    jlist   *old;

    switch( pass ) {
    case POBJ_READ_PASS:
        walk = &readJump[ JUMP_OFFSET( command ) ];
        break;
    case POBJ_ALL_PASSES:
        PObjUnRegister( command, POBJ_READ_PASS, func );
            /* fall through */
    case POBJ_WRITE_PASS:
        walk = &writeJump[ JUMP_OFFSET( command ) ];
        break;
    default:
/**/    never_reach();
    }
    while( *walk != NULL ) {
        if( (*walk)->func == func ) {
            old = *walk;
            *walk = old->next;  /* unlink from list */
            MemFree( old );
            return;
        }
        walk = &(*walk)->next;
    }
/**/never_reach();
}

void PObjEnqueue( obj_rec *objr ) {
/*******************************/
/**/myassert( objr != NULL );
/**/myassert( loc.state.pass == POBJ_WRITE_PASS );
    QEnqueue( &loc.insert, objr );
}

void PObjJoinQueue( qdesc *src ) {
/******************************/
/**/myassert( src != NULL );
/**/myassert( loc.state.pass == POBJ_WRITE_PASS );
    QJoinQueue( &loc.insert, src );
}

STATIC int pass1Coment( obj_rec *objr ) {

    int     ret;
    uint_8  *p;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_COMENT );
    ret = 0;    /* we'll pass on in most cases */
    objr->d.coment.attr = ObjGet8( objr );
    objr->d.coment.class = ObjGet8( objr );
    switch( objr->d.coment.class ) {
    case CMT_EASY_OMF:
        if( ObjRemain( objr ) >= 5 ) {
            p = ObjGet( objr, 5 );
            if( memcmp( p, EASY_OMF_SIGNATURE, 5 ) == 0 ) {
                loc.is_phar = 1;
                CanMisc.memory_model = CAN_MODEL_FLAT;
                CanMisc.processor = CAN_PROC_80386;
                ret = -1;
            }
        }
        break;
    case CMT_MS_OMF:
        ret = -1;   /* we always assume the presence of this coment record */
        break;
    case CMT_MS_END_PASS_1:
        ret = -1;   /* we trash these records */
        break;
    case CMT_DOSSEG:
        loc.last_defn = objr;
        break;
    }
    if( ret == -1 ) {
        ObjKillRec( objr );
    }
    return( ret );
}

STATIC int pass1Modend( obj_rec *objr ) {

    uint_8  mattr;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_MODEND );
    mattr = ObjGet8( objr );
    objr->d.modend.main_module = ( mattr & 0x80 ) != 0;
    objr->d.modend.start_addrs = ( mattr & 0x40 ) != 0;
    if( mattr & 0x40 ) {    /* has start addrs */
        objr->d.modend.is_logical = mattr & 1;
        FixGetRef( loc.fix_info, objr, &objr->d.modend.ref, mattr & 1 );
    }
/**/myassert( ObjEOR( objr ) );
    ObjDetachData( objr );
    return( 0 );
}

STATIC int pass1Extdef( obj_rec *objr ) {

    uint_8      len;
    unsigned    count;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_EXTDEF ||
              objr->command == CMD_STATIC_EXTDEF );

    loc.last_defn = objr;
    objr->d.extdef.first_idx = loc.extdef_idx + 1;
    count = 0;
    while( !ObjEOR( objr ) ) {
        len = ObjGet8( objr );
        ObjGet( objr, len );    /* name */
        ObjGetIndex( objr );    /* type index */
        ++count;
    }
    objr->d.extdef.num_names = count;
    loc.extdef_idx += count;
    return( 0 );
}

STATIC int pass1Cextdf( obj_rec *objr ) {

    unsigned    count;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_CEXTDF );

    loc.last_defn = objr;
    objr->d.cextdf.first_idx = loc.extdef_idx + 1;
    count = 0;
    while( !ObjEOR( objr ) ) {
        ObjGetIndex( objr );    /* skip the logical name index */
        ObjGetIndex( objr );    /* skip the type index */
        ++count;
    }
    objr->d.cextdf.num_names = count;
    loc.extdef_idx += count;
    return( 0 );
}

STATIC int pass1Lnames( obj_rec *objr ) {

    uint_8      len;
    unsigned    count;
    const uint_8 *chk_name;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_LNAMES || objr->command == CMD_LLNAMES );
    loc.last_defn = objr;
    objr->d.lnames.first_idx = loc.lnames_idx + 1;
    count = 0;
    while( !ObjEOR( objr ) ) {
        len = ObjGet8( objr );
        chk_name = ObjGet( objr, len );
        if( len == DGROUP_NAME_LEN ) {
            if( memcmp( DGROUP_NAME_VALUE, chk_name, DGROUP_NAME_LEN ) == 0 ) {
                loc.dgroup_idx = objr->d.lnames.first_idx + count;
            }
        } else if( len == FLAT_NAME_LEN ) {
            if( memcmp( FLAT_NAME_VALUE, chk_name, FLAT_NAME_LEN ) == 0 ) {
                loc.flat_idx = objr->d.lnames.first_idx + count;
            }
        }
        ++count;
    }
    objr->d.lnames.num_names = count;
    loc.lnames_idx += count;
    return( 0 );
}

STATIC int pass1Segdef( obj_rec *objr ) {

    uint_8  acbp;
    uint_8  align;
    uint_8  phar_attr;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_SEGDEF );
    loc.last_defn = objr;
    objr->d.segdef.idx = ++loc.segdef_idx;
    acbp = ObjGet8( objr );
    switch( acbp >> 5 ) {
    case ALIGN_ABS:     align = SEGDEF_ALIGN_ABS;       break;
    case ALIGN_BYTE:    align = SEGDEF_ALIGN_BYTE;      break;
    case ALIGN_WORD:    align = SEGDEF_ALIGN_WORD;      break;
    case ALIGN_PARA:    align = SEGDEF_ALIGN_PARA;      break;
    case ALIGN_PAGE:    align = SEGDEF_ALIGN_PAGE;      break;
    case ALIGN_DWORD:   align = SEGDEF_ALIGN_DWORD;     break;
    case ALIGN_LTRELOC:
        if( objr->is_phar ) {
            align = SEGDEF_ALIGN_4KPAGE;
        } else {
            Fatal( MSG_NO_SUPPORT_LTL );
            /*align = SEGDEF_ALIGN_LTRELOC; */
        }
        break;
    default:
        Fatal( MSG_UNK_SEGDEF_ALIGN, acbp >> 5 );
    }
    objr->d.segdef.align = align;
    objr->d.segdef.combine = ( acbp >> 2 ) & 0x07;
    objr->d.segdef.use_32 = acbp & 0x01; /* MS386 use_32 flag */
    if( align == SEGDEF_ALIGN_ABS ) {
        FixGetPRef( loc.fix_info, objr, &objr->d.segdef.abs );
    }
    objr->d.segdef.seg_length = ObjGetEither( objr );
    if( acbp & 0x02 ) {
        if( objr->is_32 || objr->is_phar ) {
            Fatal( MSG_NO_SUPPORT_BIG_32 );    /* FIXME */
        }
        objr->d.segdef.seg_length = 0x10000UL;
    }
    objr->d.segdef.seg_name_idx = ObjGetIndex( objr );
    objr->d.segdef.class_name_idx = ObjGetIndex( objr );
    objr->d.segdef.ovl_name_idx = ObjGetIndex( objr );
    if( objr->is_phar ) {
        if( !ObjEOR( objr ) ) {
            phar_attr = ObjGet8( objr );
            objr->d.segdef.use_32 = ( phar_attr & EASY_USE32_FIELD ) != 0;
            objr->d.segdef.access_valid = 1;
            objr->d.segdef.access_attr = phar_attr & EASY_PROTECT_FIELD;
        } else {
            objr->d.segdef.use_32 = 1;
            objr->d.segdef.access_valid = 0;
        }
    } else {
        objr->d.segdef.access_valid = 0;
    }
/**/myassert( ObjEOR( objr ) );
    ObjDetachData( objr );  /* no more need for this data */
    return( 0 );
}

STATIC int pass1Grpdef( obj_rec *objr ) {

    uint_16 name_idx;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_GRPDEF );
    loc.last_defn = objr;
    objr->d.grpdef.idx = ++loc.grpdef_idx;
    name_idx = ObjGetIndex( objr );
    if( name_idx == loc.dgroup_idx ) {
        ObjDGROUPIndex = objr->d.grpdef.idx;
    } else if( name_idx == loc.flat_idx ) {
        ObjFLATIndex = objr->d.grpdef.idx;
    }
    return( 0 );
}

STATIC int pass1Fixup( obj_rec *objr ) {

    fixup   *head;
    fixup   *fix;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_FIXUP );
    objr->d.fixup.data_rec = loc.last_data_rec;
    head = NULL;
    objr->d.fixup.fixup = NULL;
    while( !ObjEOR( objr ) ) {
        fix = FixGetFix( loc.fix_info, objr );
        if( fix != NULL ) { /* wasn't a thread fixup */
            fix->next = head;
            head = fix;
        }
    }
    objr->d.fixup.fixup = head;
    ObjDetachData( objr );  /* no more need for the data in this record */
    /* if the entire record was thread fixups, then we won't pass it on */
    if( head == NULL ) {
        ObjKillRec( objr );
        return( -1 );
    }
    return( 0 );
}

STATIC int pass1Ledata( obj_rec *objr ) {

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_LEDATA || objr->command == CMD_LIDATA );
    loc.last_data_rec = objr;
    objr->d.ledata.idx = ObjGetIndex( objr );
    objr->d.ledata.offset = ObjGetEither( objr );
    return( 0 );
}

STATIC int pass1Comdef( obj_rec *objr ) {

    uint_8  len;
    uint_16 count;
    uint_8  far_near;
    uint_8  byte;

/**/myassert( objr != NULL );
/**/myassert(   objr->command == CMD_COMDEF ||
                objr->command == CMD_STATIC_COMDEF );
    objr->d.comdef.first_idx = loc.extdef_idx + 1;
    count = 0;
    while( !ObjEOR( objr ) ) {
        len = ObjGet8( objr );
        ObjGet( objr, len );
        ObjGetIndex( objr );
        far_near = ObjGet8( objr );
        if( far_near == COMDEF_FAR ) {  /* read the extra field for FAR */
            byte = ObjGet8( objr );
            if( byte <= COMDEF_LEAF_SIZE ) {
                /* all done */
            } else if( byte == COMDEF_LEAF_2 ) {
                ObjGet( objr, 2 );
            } else if( byte == COMDEF_LEAF_3 ) {
                ObjGet( objr, 3 );
            } else if( byte == COMDEF_LEAF_4 ) {
                ObjGet( objr, 4 );
            } else {
                Fatal( MSG_UNK_COMDEF_LEAF, byte );
            }
        }
        byte = ObjGet8( objr );
        if( byte <= COMDEF_LEAF_SIZE ) {
            /* all done */
        } else if( byte == COMDEF_LEAF_2 ) {
            ObjGet( objr, 2 );
        } else if( byte == COMDEF_LEAF_3 ) {
            ObjGet( objr, 3 );
        } else if( byte == COMDEF_LEAF_4 ) {
            ObjGet( objr, 4 );
        } else {
            Fatal( MSG_UNK_COMDEF_LEAF, byte );
        }
        ++count;
    }
    objr->d.comdef.num_names = count;
    loc.extdef_idx += count;
    return( 0 );
}

STATIC int pass1Base( obj_rec *objr ) {

    uint_16 grp_idx;
    uint_16 seg_idx;

/**/myassert( objr != NULL );
    grp_idx = objr->d.base.grp_idx = ObjGetIndex( objr );
    seg_idx = objr->d.base.seg_idx = ObjGetIndex( objr );
    if( grp_idx == 0 && seg_idx == 0 ) {
        objr->d.base.frame = ObjGet16( objr );
    }
    return( 0 );
}

STATIC int pass1Comdat( obj_rec *objr ) {

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_COMDAT );
    loc.last_data_rec = objr;
    objr->d.comdat.flags = ObjGet8( objr );
    objr->d.comdat.attributes = ObjGet8( objr );
    objr->d.comdat.align = ObjGet8( objr );
    objr->d.comdat.offset = ObjGetEither( objr );
    objr->d.comdat.type_idx = ObjGetIndex( objr );
    if( ( objr->d.comdat.attributes & COMDAT_ALLOC_MASK ) == COMDAT_EXPLICIT ) {
        pass1Base( objr );
    }
    objr->d.comdat.public_name_idx = ObjGetIndex( objr );
    return( 0 );
}

STATIC int pass1LinnumData( obj_rec *objr ) {

    linnum_data *linedata;
    div_t       res;
    uint_16     line;
    int         is32;
    uint_16     len;

/**/myassert( objr != NULL );
    is32 = objr->is_32 || objr->is_phar;
    len = ObjRemain( objr );
    res = div( len, is32 ? 6 : 4 );
/**/myassert( res.rem == 0 );
    if( res.quot == 0 ) {
        /* no point in keeping this record if no lines in it */
        return( 1 );
    }
    objr->d.linnum.num_lines = res.quot;
    linedata = objr->d.linnum.lines =
        MemAlloc( res.quot * sizeof( linnum_data ) );
    for( line = 0; line < res.quot; ++line ) {
        linedata[ line ].number = ObjGet16( objr );
        linedata[ line ].offset = ObjGetEither( objr );
    }
    ObjDetachData( objr );
    return( 0 );
}

STATIC int pass1Linnum( obj_rec *objr ) {

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_LINNUM );
    pass1Base( objr );
    return( pass1LinnumData( objr ) );
}

STATIC int pass1Linsym( obj_rec *objr ) {

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_LINSYM );
    objr->d.linsym.d.linsym.flags = ObjGet8( objr );
    objr->d.linsym.d.linsym.public_name_idx = ObjGetIndex( objr );
    return( pass1LinnumData( objr ) );
}

STATIC int pass1Pubdef( obj_rec *objr ) {

    pubdef_data *pubdata;
    uint_16     num_pubs;
    uint_8      name_len;

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_PUBDEF ||
              objr->command == CMD_STATIC_PUBDEF );
    pass1Base( objr );
    pubdata = MemAlloc( 16 * sizeof( pubdef_data ) );
    num_pubs = 0;
    if( !ObjEOR( objr ) ) {
        for(;;) {
            name_len = ObjGet8( objr );
            pubdata[ num_pubs ].name = NameAdd( (char *)ObjGet( objr, name_len ),
                name_len );
            pubdata[ num_pubs ].offset = ObjGetEither( objr );
            pubdata[ num_pubs ].type.idx = ObjGetIndex( objr );
            ++num_pubs;
            if( ObjEOR( objr ) ) break;
            if( ( num_pubs & 0xf ) == 0 ) {
                /* we've used a multiple of 16 pubs, realloc for 16 more */
                pubdata = MemRealloc( pubdata,
                    ( num_pubs + 16 ) * sizeof( pubdef_data ) );
            }
        }
    }
    ObjDetachData( objr );
    objr->d.pubdef.num_pubs = num_pubs;
    if( num_pubs > 0 ) {
        objr->d.pubdef.pubs = MemRealloc( pubdata,
            num_pubs * sizeof( pubdef_data ) );
        objr->d.pubdef.free_pubs = 1;
    } else {
        MemFree( pubdata );
        objr->d.pubdef.pubs = NULL;
        objr->d.pubdef.free_pubs = 0;
    }
    objr->d.pubdef.processed = 0;
    return( 0 );
}

STATIC void doPass1( OBJ_RFILE *file_in, pobj_lib_info *pli ) {

    obj_rec *objr;
    jlist   *func;
    jlist   *next;
    int     cont;

    QInit( &loc.records );
    QInit( &loc.pub );
    objr = ObjReadRec( file_in );
    switch( objr->command ) {
    case LIB_HEADER_REC:
        pli->page_len = ObjRemain( objr ) + 4;
        pli->is_lib = 1;
        objr = ObjReadRec( file_in );
        break;
    case LIB_TRAILER_REC:
        return;
    }
    if( objr->command != CMD_THEADR ) {
        Fatal( MSG_THEADR_MISSING );
    }
    for(;;) {
        objr->is_phar = loc.is_phar;
        objr->is_32 = objr->command & 1;
        objr->command &= ~1;
            /* handle those records we must preprocess */
        cont = 0;
        switch( objr->command ) {
        case CMD_COMENT:    cont = pass1Coment( objr );     break;
        case CMD_MODEND:    cont = pass1Modend( objr );     break;
        case CMD_STATIC_EXTDEF: /* fall through */
        case CMD_EXTDEF:    cont = pass1Extdef( objr );     break;
        case CMD_LLNAMES:   /* fall through */
        case CMD_LNAMES:    cont = pass1Lnames( objr );     break;
        case CMD_SEGDEF:    cont = pass1Segdef( objr );     break;
        case CMD_GRPDEF:    cont = pass1Grpdef( objr );     break;
        case CMD_FIXUP:     cont = pass1Fixup( objr );      break;
        case CMD_LEDATA:    /* fall through */
        case CMD_LIDATA:    cont = pass1Ledata( objr );     break;
        case CMD_STATIC_COMDEF: /* fall through */
        case CMD_COMDEF:    cont = pass1Comdef( objr );     break;
        case CMD_LINNUM:    cont = pass1Linnum( objr );     break;
        case CMD_STATIC_PUBDEF: /* fall through */
        case CMD_PUBDEF:    cont = pass1Pubdef( objr );     break;
        case CMD_CEXTDF:    cont = pass1Cextdf( objr );     break;
        case CMD_COMDAT:    cont = pass1Comdat( objr );     break;
        case CMD_LINSYM:    cont = pass1Linsym( objr );     break;
        }
            /* pass record to registered routines */
        func = readJump[ JUMP_OFFSET( objr->command ) ];
        while( func != NULL && cont != -1 ) {
            next = func->next;  /* filters are allowed to unregister themself */
            cont = func->func( objr, &loc.state );
            func = next;
        }
        if( cont == 0 ) {
            switch( objr->command ) {
            case CMD_PUBDEF:
            case CMD_STATIC_PUBDEF:
                QEnqueue( &loc.pub, objr );
                break;
            default:
                QEnqueue( &loc.records, objr );
                break;
            }
        }
        if( objr->command == CMD_MODEND ) {
            if( pli->is_lib ) {
                ObjRSkipPage( file_in, pli->page_len );
            }
            break;
        }
        objr = ObjReadRec( file_in );
    }
}

STATIC void pass2Lnames( obj_rec *objr ) {

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_LNAMES || objr->command == CMD_LLNAMES );
    loc.lnames_idx = objr->d.lnames.first_idx + objr->d.lnames.num_names - 1;
}

STATIC void pass2Segdef( obj_rec *objr ) {

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_SEGDEF );
    loc.segdef_idx = objr->d.segdef.idx;
}

STATIC void pass2Grpdef( obj_rec *objr ) {

/**/myassert( objr != NULL );
/**/myassert( objr->command == CMD_GRPDEF );
    loc.grpdef_idx = objr->d.grpdef.idx;
}

STATIC void doPass2( void ) {

    obj_rec *objr;
    obj_rec *objr_next;
    obj_rec *stale_objrs;
    jlist   *func;
    jlist   *func_next;
    int     cont;

    objr = loc.records.head;
/**/myassert( objr != NULL );
    if( objr->command != CMD_THEADR ) {
        Fatal( MSG_THEADR_MISSING );
    }
    stale_objrs = NULL;
    do {
        QInit( &loc.insert ); /* set up for inserted records */
        switch( objr->command ) {
        case CMD_LLNAMES:   /* fall through */
        case CMD_LNAMES:    pass2Lnames( objr );    break;
        case CMD_SEGDEF:    pass2Segdef( objr );    break;
        case CMD_GRPDEF:    pass2Grpdef( objr );    break;
        }
            /* pass record to registered routines */
        cont = 0;
        func = writeJump[ JUMP_OFFSET( objr->command ) ];
        while( func != NULL && cont != -1 ) {
            func_next = func->next;
            cont = func->func( objr, &loc.state );
            func = func_next;
        }
        objr_next = objr->next;
        if( objr == loc.last_defn ) {
            if( loc.pub.head != NULL ) {
                PObjJoinQueue( &loc.pub );
            }
            func = writeJump[ JUMP_OFFSET( CMD_LAST_DEFN ) ];
            while( func != NULL && cont != -1 ) {
                func_next = func->next;
                cont = func->func( objr, &loc.state );
                func = func_next;
            }
        }
        objr_next = objr->next;
        if( loc.insert.head != NULL ) {
            ((obj_rec *)loc.insert.tail)->next = objr_next;
            objr_next = loc.insert.head;
        }
        /* we don't want any records recycled during the 2nd pass */
        objr->next = stale_objrs;
        stale_objrs = objr;
        objr = objr_next;
    } while( objr != NULL );
     /* clean up stale object records */
     while( stale_objrs != NULL ) {
        objr_next = stale_objrs->next;
         ObjKillRec( stale_objrs );
        stale_objrs = objr_next;
     }
}

int PObj( OBJ_RFILE *file_in, OBJ_WFILE *file_out, pobj_lib_info *pli ) {
/***********************************************************************/
    int more_data;

    memset( &loc, 0, sizeof( loc ) );
    ObjDGROUPIndex = 0;
    ObjFLATIndex = 0;
    loc.fix_info = FixBegin();
    loc.state.file_out = file_out;
    loc.state.pass = POBJ_READ_PASS;
    doPass1( file_in, pli );
    if( loc.records.head == NULL && pli->is_lib ) {
        FixEnd( loc.fix_info );
        return( 0 );
    }
/**/myassert( loc.records.head != NULL );
    if( loc.last_defn == NULL ) {
        Fatal( MSG_INVALID_OBJECT );
    }
    loc.state.pass = POBJ_WRITE_PASS;
    loc.segdef_idx = 0;
    loc.grpdef_idx = 0;
    loc.extdef_idx = 0;
    loc.lnames_idx = 0;
    doPass2();
    FixEnd( loc.fix_info );
    more_data = ObjRMoreData( file_in );
    if( more_data ) {
        return( 1 );
    }
    return( ObjRMoreData( file_in ) );
}

obj_rec *PObjNewLnames( uint_16 num ) {
/***********************************/
    obj_rec *new;

/**/myassert( loc.state.pass == POBJ_WRITE_PASS );
    new = ObjNewRec( CMD_LNAMES );
    new->d.lnames.first_idx = loc.lnames_idx + 1;
    new->d.lnames.num_names = num;
    loc.lnames_idx += num;
    PObjEnqueue( new );
    return( new );
}

obj_rec *PObjNewSegdef( void ) {
/****************************/
    obj_rec *new;

/**/myassert( loc.state.pass == POBJ_WRITE_PASS );
    new = ObjNewRec( CMD_SEGDEF );
    new->d.segdef.idx = loc.segdef_idx + 1;
    ++loc.segdef_idx;
    PObjEnqueue( new );
    return( new );
}

obj_rec *PObjNewGrpdef( void ) {
/****************************/
    obj_rec *new;

/**/myassert( loc.state.pass == POBJ_WRITE_PASS );
    new = ObjNewRec( CMD_GRPDEF );
    new->d.grpdef.idx = loc.grpdef_idx + 1;
    ++loc.grpdef_idx;
    PObjEnqueue( new );
    return( new );
}

qdesc *PObjPubdefQueue( void ) {
/****************************/
/*
    This is used by pubdef.c to get the queue of PUBDEF records. It is possible
    for pubdef.c to munch this queue any way it likes.  Of course, a call to
    this must be made BEFORE the last_defn record is stumbled on in the
    WRITE_PASS.
*/
/**/myassert( loc.state.pass == POBJ_WRITE_PASS );
    return( &loc.pub );
}

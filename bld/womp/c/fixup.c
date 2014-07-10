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


#include <stddef.h>
#include "watcom.h"
#include "pcobj.h"
#include "womp.h"
#include "memutil.h"
#include "fixup.h"
#include "myassert.h"
#include "genutil.h"
#include "carve.h"

STATIC carve_t  myCarver;

void FixInit( void ) {
/******************/
    myCarver = CarveCreate( sizeof( fixup ), 64 );
}

void FixFini( void ) {
/******************/
    CarveDestroy( myCarver );
}

fixup *FixNew( void ) {
/*******************/
    return( CarveAlloc( myCarver ) );
}

fixup *FixDup( const fixup *fix ) {
/*********************************/
    fixup *new;

    if( fix == NULL ) {
        return( NULL );
    }
    new = CarveAlloc( myCarver );
    *new = *fix;
    return( new );
}

void FixKill( fixup *fix ) {
/************************/
    CarveFree( myCarver, fix );
}

#if _WOMP_OPT & _WOMP_READ

fixinfo *FixBegin( void ) {
/***********************/
    fixinfo *new;

    new = MemAlloc( sizeof( *new ) );
    return( new );
}

void FixEnd( fixinfo *info ) {
/**************************/
    MemFree( info );
}

STATIC uint_16 frameDatum( obj_rec *objr, uint_8 method ) {

/**/myassert( objr != NULL );
    switch( method ) {
    case FRAME_SEG:
    case FRAME_GRP:
    case FRAME_EXT:
        return( ObjGetIndex( objr ) );
    case FRAME_ABS:
        return( ObjGet16( objr ) );
    }
    /* for FRAME_LOC, FRAME_TARG, and FRAME_NONE we return 0 */
    return( 0 );
}

STATIC uint_16 targetDatum( obj_rec *objr, uint_8 method ) {

/**/myassert( objr != NULL );
    if( ( method & 0x03 ) == TARGET_ABSWD ) {
        return( ObjGet16( objr ) );
    }
    return( ObjGetIndex( objr ) );
}

void FixGetLRef( fixinfo *info, obj_rec *objr, logref *log ) {
/**********************************************************/
/*
    Get a logical reference. i.e., a fix_dat, frame_datum, targ_datum,
    and targ_offset... or from MODEND the end_dat, frame_datum, etc...
*/
    uint_8  dat;
    uint_8  method;
    uint_8  trd_num;

/**/myassert( info != NULL );
/**/myassert( objr != NULL );
/**/myassert( log != NULL );
    dat = ObjGet8( objr );
    method = ( dat >> 4 ) & 0x07;
    if( dat & 0x80 ) {     /* F bit set */
        trd_num = method | 0x04;
        log->frame = info->trd[ trd_num ].method;
        log->frame_datum = info->trd[ trd_num ].datum;
    } else {
        log->frame = method;
        log->frame_datum = frameDatum( objr, method );
    }
    method = dat & 0x07;
    if( dat & 0x08 ) {     /* T bit set */
        trd_num = method & 0x03;
        log->target = info->trd[ trd_num ].method & 0x03;
        log->target_datum = info->trd[ trd_num ].datum;
    } else {
        log->target = method & 0x03;    /* always has displacement */
        log->target_datum = targetDatum( objr, method );
    }
    if( method & 0x04 ) {
        log->target_offset = 0;
        log->is_secondary = 1;
    } else {
        if( objr->is_32 || objr->is_phar ) {
            log->target_offset = (int_32)ObjGet32( objr );
        } else {
            log->target_offset = (int_32)ObjGet16( objr );
        }
        log->is_secondary = 0;
    }
}

fixup *FixGetFix( fixinfo *info, obj_rec *objr ) {
/**********************************************/
    uint_8  byte;
    uint_8  method;
    uint_8  trd_num;
    fixup   *fix;

/**/myassert( info != NULL );
/**/myassert( objr != NULL );

    byte = ObjGet8( objr );
    if( byte & 0x80 ) { /* is explicit fixup */
        fix = FixNew();
        fix->next = NULL;
        fix->loader_resolved = 0;
        fix->self_relative = ( byte & 0x40 ) == 0;
        method = ( byte >> 2 ) & 0x0f;
        switch( method ) {
        case LOC_OFFSET_LO:     /* note fall through */
        case LOC_OFFSET:
        case LOC_BASE:
        case LOC_BASE_OFFSET:
        case LOC_OFFSET_HI:
            fix->loc_method = method;
            break;
        case LOC_MS_LINK_OFFSET:
            if( objr->is_phar ) {
                fix->loc_method = FIX_OFFSET386;
            } else {
                fix->loc_method = FIX_OFFSET;
                fix->loader_resolved = 1;
            }
            break;
        case LOC_BASE_OFFSET_32:
            if( objr->is_phar == 0 ) {
                Fatal( MSG_INVALID_FIXUP );
            }
            fix->loc_method = FIX_POINTER386;
            break;
        case LOC_MS_OFFSET_32:
            /* FIXME we assume that the file is a Microsoft object file */
            fix->loc_method = FIX_OFFSET386;
            break;
        case LOC_MS_BASE_OFFSET_32:
            /* FIXME we assume that the file is a Microsoft object file */
            fix->loc_method = FIX_POINTER386;
            break;
        case LOC_MS_LINK_OFFSET_32:
            /* FIXME we assume that the file is a Microsoft object file */
            fix->loc_method = FIX_OFFSET386;
            fix->loader_resolved = 1;
            break;
        default:
            Fatal( MSG_INVALID_FIXUP );
        }
        fix->loc_offset = ( (uint_16)( byte & 0x03 ) << 8 ) | ObjGet8( objr );
        FixGetLRef( info, objr, &fix->lr );
        return( fix );
    }
    /* is thread fixup */
    trd_num = ( byte & 0x03 ) | ( ( byte & 0x40 ) ? 0x04 : 0 );
    method = ( byte >> 2 ) & 0x07;
    info->trd[ trd_num ].method = method;
    info->trd[ trd_num ].datum = ( trd_num & 0x04 ) ?
        targetDatum( objr, method ) : frameDatum( objr, method );

    return( NULL );
}

void FixGetPRef( fixinfo *info, obj_rec *objr, physref *phys ) {
/************************************************************/
/**/myassert( info != NULL );
/**/myassert( objr != NULL );
/**/myassert( phys != NULL );
    info = info;
    phys->frame = ObjGet16( objr );
    if( objr->is_phar ) {
        phys->offset = ObjGet32( objr );
    } else {
        phys->offset = ObjGet16( objr );
    }
}

void FixGetRef( fixinfo *info, obj_rec *objr, logphys *ref, int is_logical ) {
/**************************************************************************/
/**/myassert( info != NULL );
/**/myassert( objr != NULL );
/**/myassert( ref != NULL );
    if( is_logical ) {
        FixGetLRef( info, objr, &ref->log );
    } else {
        FixGetPRef( info, objr, &ref->phys );
    }
}

#endif


#if _WOMP_OPT & _WOMP_WRITE

STATIC uint_8 *putIndex( uint_8 *p, uint_16 index ) {

    if( index > 0x7f ) {
        *p++ = 0x80 | ( index >> 8 );
    }
    *p++ = index;
    return( p );
}

STATIC uint_8 *put16( uint_8 *p, uint_16 word ) {

    WriteU16( p, word );
    return( p + 2 );
}

STATIC uint_8 *put32( uint_8 *p, uint_32 dword ) {

    WriteU32( p, dword );
    return( p + 4 );
}

STATIC uint_8 *putFrameDatum( uint_8 *p, uint_8 method, uint_16 datum ) {

/**/myassert( p != NULL );
    switch( method ) {
    case FRAME_SEG:
    case FRAME_GRP:
    case FRAME_EXT:
        return( putIndex( p, datum ) );
    case FRAME_ABS:
        return( put16( p, datum ) );
    }
    /* for FRAME_LOC, FRAME_TARG, and FRAME_NONE there is nothing to output */
    return( p );
}

STATIC uint_8 *putTargetDatum( uint_8 *p, uint_8 method, uint_16 datum ) {

/**/myassert( p != NULL );
    if( ( method & 0x03 ) == TARGET_ABSWD ) {
        return( put16( p, datum ) );
    }
    return( putIndex( p, datum ) );
}

uint_16 FixGenLRef( logref *log, uint_8 *buf, int type )
/******************************************************/
{
    uint_8  *p;
    uint_8  target;

/**/myassert( log != NULL );
/**/myassert( buf != NULL );
/**/myassert( type == FIX_GEN_INTEL || type == FIX_GEN_PHARLAP || type == FIX_GEN_MS386 );

    /*
        According to the discussion on p102 of the Intel OMF document, we
        cannot just arbitrarily write fixups without a displacment if their
        displacement field is 0.  So we use the is_secondary field.
    */
    target = log->target;
    if( log->target_offset == 0 && log->is_secondary ) {
        target |= 0x04;
    }
    p = buf;
    *p++ = ( log->frame << 4 ) | ( target );
    p = putFrameDatum( p, log->frame, log->frame_datum );
    p = putTargetDatum( p, target, log->target_datum );
    if( ( target & 0x04 ) == 0 ) {
        if( type == FIX_GEN_MS386 || type == FIX_GEN_PHARLAP ) {
            p = put32( p, (uint_32)log->target_offset );
        } else {
            p = put16( p, (uint_16)log->target_offset );
        }
    }
    return( (uint_16)( p - buf ) );
}

uint_16 FixGenPRef( physref *ref, uint_8 *buf, int type )
/*******************************************************/
{
    uint_8  *p;

/**/myassert( ref != NULL );
/**/myassert( buf != NULL );
/**/myassert( type == FIX_GEN_INTEL || type == FIX_GEN_PHARLAP || type == FIX_GEN_MS386 );
    p = put16( buf, ref->frame );
    if( type == FIX_GEN_PHARLAP ) {
        p = put32( p, ref->offset );
    } else {
        p = put16( p, (uint_16)ref->offset );
    }
    return( (uint_16)( p - buf ) );
}

uint_16 FixGenRef( logphys *ref, int is_logical, uint_8 *buf, int type )
/**********************************************************************/
{
/**/myassert( ref != NULL );
/**/myassert( buf != NULL );
/**/myassert( type == FIX_GEN_INTEL || type == FIX_GEN_PHARLAP || type == FIX_GEN_MS386 );
    if( is_logical ) {
        return( FixGenLRef( &ref->log, buf, type ) );
    }
    return( FixGenPRef( &ref->phys, buf, type ) );
}

uint_16 FixGenFix( fixup *fix, uint_8 *buf, int type )
/****************************************************/
{
    uint_8  *p;
    uint_8  byte;
    uint_16 data_rec_offset;

/**/myassert( fix != NULL );
/**/myassert( buf != NULL );
/**/myassert( type == FIX_GEN_INTEL || type == FIX_GEN_PHARLAP || type == FIX_GEN_MS386 );
    p = buf;
    byte = fix->self_relative ? 0x80 : 0xc0;    /* explicit fixup */
    switch( fix->loc_method ) {
    case FIX_LO_BYTE:   byte |= ( LOC_OFFSET_LO << 2 );     break;
    case FIX_OFFSET:
        if( fix->loader_resolved ) {
            if( type == FIX_GEN_PHARLAP ) {
#if _WOMP_OPT & _WOMP_EXTRAS
                PrtMsg( WRN|MSG_NO_LRO_PHARLAP );
#endif
                byte |= ( LOC_OFFSET << 2 );
            } else {
/**/            myassert( type == FIX_GEN_INTEL || type == FIX_GEN_MS386 );
                byte |= ( LOC_MS_LINK_OFFSET << 2 );
            }
        } else {
            byte |= ( LOC_OFFSET << 2 );
        }
        break;
    case FIX_BASE:      byte |= ( LOC_BASE << 2 );          break;
    case FIX_POINTER:   byte |= ( LOC_BASE_OFFSET << 2 );   break;
    case FIX_HI_BYTE:   byte |= ( LOC_OFFSET_HI << 2 );     break;
    case FIX_OFFSET386:
        if( type == FIX_GEN_PHARLAP ) {
#if _WOMP_OPT & _WOMP_EXTRAS
            if( fix->loader_resolved ) {
                PrtMsg( WRN|MSG_NO_LRO_PHARLAP );
            }
#endif
            byte |= ( LOC_OFFSET_32 ) << 2;
        } else {
            if( fix->loader_resolved ) {
                byte |= ( LOC_MS_LINK_OFFSET_32 << 2 );
            } else {
                byte |= ( LOC_MS_OFFSET_32 << 2 );
            }
        }
        break;
    case FIX_POINTER386:
        if( type == FIX_GEN_PHARLAP ) {
            byte |= ( LOC_BASE_OFFSET_32 << 2 );
        } else {
            byte |= ( LOC_MS_BASE_OFFSET_32 << 2 );
        }
        break;
    default:
/**/    never_reach();
    }
/**/myassert( fix->loc_offset < 1024 );
    data_rec_offset = (uint_16)fix->loc_offset;
    byte |= data_rec_offset >> 8;
    *p++ = byte;
    *p++ = (uint_8)data_rec_offset;
    p += FixGenLRef( &fix->lr, p, type );
    return( (uint_16)( p - buf ) );
}

#endif

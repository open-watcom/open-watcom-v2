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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <stddef.h>
#include "asmglob.h"
#include "pcobj.h"
#include "asmalloc.h"
#include "omffixup.h"
#include "omfcarve.h"
#include "myassert.h"


static carve_t  myCarver;

void FixInit( void )
/******************/
{
    myCarver = CarveCreate( sizeof( fixup ), 64 );
}

void FixFini( void )
/******************/
{
    CarveDestroy( myCarver );
}

fixup *FixNew( void )
/*******************/
{
    return( CarveAlloc( myCarver ) );
}

fixup *FixDup( const fixup *fix )
/*******************************/
{
    fixup *new;

    if( fix == NULL ) {
        return( NULL );
    }
    new = CarveAlloc( myCarver );
    *new = *fix;
    return( new );
}

void FixKill( fixup *fix )
/************************/
{
    CarveFree( myCarver, fix );
}

static uint_8 *putIndex( uint_8 *p, uint_16 index )
{
    if( index > 0x7f ) {
        *p++ = 0x80 | ( index >> 8 );
    }
    *p++ = (uint_8)index;
    return( p );
}

static uint_8 *put16( uint_8 *p, uint_16 word )
{
    WriteU16( p, word );
    return( p + 2 );
}

static uint_8 *put32( uint_8 *p, uint_32 dword )
{
    WriteU32( p, dword );
    return( p + 4 );
}

static uint_8 *putFrameDatum( uint_8 *p, uint_8 method, uint_16 datum )
{
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

static uint_8 *putTargetDatum( uint_8 *p, uint_8 method, uint_16 datum )
{
/**/myassert( p != NULL );
    if( ( method & 0x03 ) == TARGET_ABS ) {
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
/**/myassert( type == FIX_GEN_INTEL || type == FIX_GEN_MS386 );

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
        if( type == FIX_GEN_MS386 ) {
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
/**/myassert( type == FIX_GEN_INTEL || type == FIX_GEN_MS386 );
    (void)type;
    p = put16( buf, ref->frame );
    p = put16( p, (uint_16)ref->offset );
    return( (uint_16)( p - buf ) );
}

uint_16 FixGenRef( logphys *ref, int is_logical, uint_8 *buf, int type )
/**********************************************************************/
{
/**/myassert( ref != NULL );
/**/myassert( buf != NULL );
/**/myassert( type == FIX_GEN_INTEL || type == FIX_GEN_MS386 );
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
/**/myassert( type == FIX_GEN_INTEL || type == FIX_GEN_MS386 );
    p = buf;
    byte = fix->self_relative ? 0x80 : 0xc0;    /* explicit fixup */
    switch( fix->loc_method ) {
    case FIX_LO_BYTE:
        byte |= ( LOC_OFFSET_LO << 2 );
        break;
    case FIX_OFFSET:
        if( fix->loader_resolved ) {
/**/        myassert( type == FIX_GEN_INTEL || type == FIX_GEN_MS386 );
            byte |= ( LOC_OFFSET_LOADER << 2 );
        } else {
            byte |= ( LOC_OFFSET << 2 );
        }
        break;
    case FIX_BASE:
        byte |= ( LOC_BASE << 2 );
        break;
    case FIX_POINTER:
        byte |= ( LOC_BASE_OFFSET << 2 );
        break;
    case FIX_HI_BYTE:
        byte |= ( LOC_OFFSET_HI << 2 );
        break;
    case FIX_OFFSET386:
        if( fix->loader_resolved ) {
            byte |= ( LOC_OFFSET_32_LOADER << 2 );
        } else {
            byte |= ( LOC_OFFSET_32 << 2 );
        }
        break;
    case FIX_POINTER386:
        byte |= ( LOC_BASE_OFFSET_32 << 2 );
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

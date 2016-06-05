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
* Description:  Machine Architecture Description manager - generic routines.
*
****************************************************************************/


#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <float.h>
#include <stdlib.h>
#include <stdio.h>
#include <i64.h>
#include "bool.h"
#include "mad.h"
#include "madimp.h"
#include "madcli.h"
#include "madsys.h"
#include "xfloat.h"


#if defined( _M_IX86 ) || defined( _M_X64 ) || defined( __ALPHA__ ) || defined( __PPC__ ) || defined( __MIPS__ )
   #define MNR_HOST_SIGNED      MNR_TWOS_COMP
   #define FLOAT_IEEE
#else
   #error Host MAD type info not configured
#endif

#if defined( __BIG_ENDIAN__ )
    #define ME_HOST             ME_BIG
#else
    #define ME_HOST             ME_LITTLE
#endif


const static unsigned EndMap[2][8] = {
    { 0, 1, 2, 3, 4, 5, 6, 7 },     /* ME_LITTLE */
    { 7, 6, 5, 4, 3, 2, 1, 0 },     /* ME_BIG */
};

struct mad_state_data {
    mad_state_data      *next;
    /* followed by the imp_mad_state structure */
};

typedef struct mad_entry {
    struct mad_entry    *next;
    char                *file;
    char                *desc;
    mad_imp_routines    *rtns;
    mad_state_data      *sl;
    dig_mad             mad;
    mad_sys_handle      sys_hdl;
} mad_entry;

static mad_entry        *MADList;
static mad_entry        *Active;

/*
 * Client interface
 */


static mad_status DIGCLIENT MADCliTypeInfoForHost( mad_type_kind tk, int size, mad_type_info *mti )
{
    return( MADTypeInfoForHost( tk, size, mti ) );
}

static mad_status DIGCLIENT MADCliTypeConvert( const mad_type_info *in_t, const void *in_d, const mad_type_info *out_t, void *out_d, addr_seg seg )
{
    return( MADTypeConvert( in_t, in_d, out_t, out_d, seg ) );
}

static mad_status DIGCLIENT MADCliTypeToString( mad_radix radix, const mad_type_info *mti, const void *data, char *buff, size_t *buff_size_p )
{
    return( MADTypeToString( radix, mti, data, buff, buff_size_p ) );
}

mad_client_routines MADClientInterface = {
    MAD_MAJOR,
    MAD_MINOR,
    sizeof( mad_client_routines ),

    DIGCliAlloc,
    DIGCliRealloc,
    DIGCliFree,

    DIGCliOpen,
    DIGCliSeek,
    DIGCliRead,
    DIGCliClose,

    MADCliReadMem,
    MADCliWriteMem,

    MADCliString,
    MADCliAddString,
    MADCliRadixPrefix,

    MADCliNotify,

    DIGCliMachineData,

    MADCliAddrToString,
    MADCliMemExpr,

    MADCliAddrSection,
    MADCliAddrOvlReturn,

    MADCliSystemConfig,

    MADCliTypeInfoForHost,
    MADCliTypeConvert,
    MADCliTypeToString
};

/*
 *      Support Routines
 */

static mad_imp_routines DummyRtns;      /* forward reference */

static mad_entry        Dummy = {
    NULL, "", "Unknown Architecture", &DummyRtns, NULL, 0
};

static const mad_string EmptyStrList[] = {
    MAD_MSTR_NIL
};

static const mad_toggle_strings EmptyToggleList[] = {
    { MAD_MSTR_NIL },
    { MAD_MSTR_NIL },
    { MAD_MSTR_NIL }
};

static mad_status MADStatus( mad_status ms )
{
    MADCliNotify( MNT_ERROR, &ms );
    return( ms );
}

static mad_entry *MADFind( dig_mad mad )
{
    mad_entry   *curr;

    if( mad == MAD_NIL )
        return( NULL );
    if( mad == Active->mad )
        return( Active );
    for( curr = MADList; curr != NULL; curr = curr->next ) {
        if( curr->mad == mad ) {
            return( curr );
        }
    }
    return( NULL );
}

#define quoted(x)   # x
#define strx(x)     quoted(x)

/*
 *      Control Routines
 */

mad_status      MADInit( void )
{
    static const struct {
        dig_mad         mad;
        const char      *file;
        const char      *desc;
    } list[] = {
#ifdef USE_FILENAME_VERSION
        #define pick_mad(enum,file,desc) {enum,file strx(USE_FILENAME_VERSION),desc},
#else
        #define pick_mad(enum,file,desc) {enum,file,desc},
#endif
        #include "madarch.h"
        #undef pick_mad
    };

    mad_status  ms = MS_OK;
    unsigned    i;

    MADList = NULL;
    Active = &Dummy;
    for( i = 0; i < sizeof( list ) / sizeof( list[0] ); ++i ) {
        ms = MADRegister( list[i].mad, list[i].file, list[i].desc );
        if( ms != MS_OK ) {
            return( ms );
        }
    }
    return( ms );
}

mad_status      MADRegister( dig_mad mad, const char *file, const char *desc )
{
    mad_entry   **owner;
    mad_entry   *curr;
    mad_entry   *old;
    size_t      file_len;
    size_t      desc_len;

    for( owner = &MADList; (curr = *owner) != NULL; owner = &curr->next ) {
        if( curr->mad == mad ) {
            *owner = curr->next;
            old = Active;
            Active = curr;
            while( curr->sl != NULL ) {
                MADStateDestroy( curr->sl );
            }
            Active = old;
            if( curr == Active )
                Active = &Dummy;
            /* MADUnload( curr->mad );  Did not work from here. */
            /* Removed call, and moved fixed functionality here */
            if( curr->rtns != NULL ) {
                curr->rtns->MIFini();
                curr->rtns = NULL;
            }
            if( curr->sys_hdl != NULL_SYSHDL ) {
                MADSysUnload( &curr->sys_hdl );
            }
            DIGCliFree( curr );
            break;
        }
    }
    if( file == NULL )
        return( MS_OK );
    file_len = strlen( file );
    desc_len = strlen( desc );
    curr = DIGCliAlloc( sizeof( *curr ) + 2 + file_len + desc_len );
    if( curr == NULL )
        return( MADStatus( MS_ERR | MS_NO_MEM ) );
    curr->next = *owner;
    *owner = curr;
    curr->file = (char *)curr + sizeof( *curr );
    curr->desc = &curr->file[file_len + 1];
    curr->rtns = NULL;
    curr->sl   = NULL;
    curr->mad  = mad;
    curr->sys_hdl = NULL_SYSHDL;
    strcpy( curr->file, file );
    strcpy( curr->desc, desc );
    return( MS_OK );
}

static mad_status DIGREGISTER DummyInit( void )
{
    /* never actually called */
    return( MS_OK );
}

mad_status      MADLoad( dig_mad mad )
{
    mad_entry           *me;
    mad_status          ms;

    me = MADFind( mad );
    if( me == NULL )
        return( MADStatus( MS_ERR | MS_UNREGISTERED_MAD ) );
    if( me->rtns != NULL )
        return( MS_OK );
    ms = MADSysLoad( me->file, &MADClientInterface, &me->rtns, &me->sys_hdl );
    if( ms != MS_OK ) {
        me->rtns = NULL;
        return( MADStatus( ms ) );
    }
    if( MADClientInterface.major != me->rtns->major
     || MADClientInterface.minor > me->rtns->minor ) {
        me->rtns = NULL;
        MADUnload( mad );
        return( MADStatus( MS_ERR|MS_INVALID_MAD_VERSION ) );
    }
    ms = me->rtns->MIInit();
    if( ms != MS_OK ) {
        me->rtns = NULL;
        MADUnload( mad );
        return( MADStatus( ms ) );
    }
    if( me->sl == NULL ) {
        me->sl = DIGCliAlloc( sizeof( *me->sl ) + me->rtns->MIStateSize() );
        if( me->sl == NULL ) {
            MADUnload( mad );
            return( MADStatus( MS_ERR|MS_NO_MEM ) );
        }
        me->rtns->MIStateInit( (imp_mad_state_data *)&me->sl[1] );
        me->sl->next = NULL;
    }
    Active->rtns->MIStateSet( (imp_mad_state_data *)&me->sl[1] );
    return( MS_OK );
}

static void DIGREGISTER DummyFini( void )
{
    /* never actually called */
}

void            MADUnload( dig_mad mad )
{
    mad_entry   *me;

    me = MADFind( mad );
    if( me != NULL ) {
        if( me->rtns != NULL ) {
            me->rtns->MIFini();
            me->rtns = NULL;
        }
        if( me->sys_hdl != NULL_SYSHDL ) {
            MADSysUnload( &me->sys_hdl );
        }
    }
}

mad_status      MADLoaded( dig_mad mad )
{
    mad_entry   *me;

    me = MADFind( mad );
    if( me == NULL )
        return( MADStatus( MS_ERR|MS_UNREGISTERED_MAD ) );
    return( me->rtns != NULL ? MS_OK : MS_FAIL );
}

dig_mad         MADActiveSet( dig_mad mad )
{
    dig_mad         mad_old;
    mad_entry       *me;

    mad_old = Active->mad;
    me = MADFind( mad );
    if( me != NULL )
        Active = me;
    return( mad_old );
}

static unsigned DIGREGISTER DummyStateSize( void )
{
    return( 0 );
}

static void DIGREGISTER DummyStateInit( imp_mad_state_data *msd )
{
    msd = msd;
}

static void DIGREGISTER DummyStateSet( imp_mad_state_data *msd )
{
    msd = msd;
}

static void DIGREGISTER DummyStateCopy( const imp_mad_state_data *src, imp_mad_state_data *dst )
{
    src = src;
    dst = dst;
}

mad_state_data  *MADStateCreate( void )
{
    mad_state_data      *new;

    new = DIGCliAlloc( sizeof( *new ) + Active->rtns->MIStateSize() );
    if( new == NULL ) {
        MADStatus( MS_ERR | MS_NO_MEM );
        return( NULL );
    }
    /* first one is currently active state */
    new->next = Active->sl->next;
    Active->sl->next = new;
    Active->rtns->MIStateInit( (imp_mad_state_data *)&new[1] );
    return( new );
}

mad_state_data  *MADStateSet( mad_state_data *msd )
{
    mad_state_data      **owner;
    mad_state_data      *curr;

    if( msd != NULL ) {
        for( owner = &Active->sl; (curr = *owner) != NULL; owner = &curr->next ) {
            if( curr == msd ) {
                curr = Active->sl;
                *owner = msd->next;
                msd->next = curr;
                Active->sl = msd;
                Active->rtns->MIStateSet( (imp_mad_state_data *)&msd[1] );
                return( curr );
            }
        }
        MADStatus( MS_ERR|MS_NO_MEM );
    }
    return( Active->sl );
}

void            MADStateCopy( const mad_state_data *src, mad_state_data *dst )
{
    Active->rtns->MIStateCopy( (const imp_mad_state_data *)&src[1], (imp_mad_state_data *)&dst[1] );
}

void            MADStateDestroy( mad_state_data *msd )
{
    mad_state_data      **owner;
    mad_state_data      *curr;

    for( owner = &Active->sl; (curr = *owner) != NULL; owner = &curr->next ) {
        if( curr == msd ) {
            *owner = msd->next;
            DIGCliFree( msd );
            return;
        }
    }
    MADStatus( MS_ERR|MS_INVALID_STATE );
}

void            MADFini( void )
{
    while( MADList != NULL ) {
        MADRegister( MADList->mad, NULL, NULL );
    }
}

walk_result     MADWalk( MAD_WALKER *wk, void *d )
{
    walk_result wr;
    mad_entry   *me;

    for( me = MADList; me != NULL; me = me->next ) {
        wr = wk( me->mad, d );
        if( wr != WR_CONTINUE ) {
            return( wr );
        }
    }
    return( WR_CONTINUE );
}

size_t MADNameFile( dig_mad mad, char *buff, size_t buff_size )
{
    mad_entry   *me;
    size_t      len;

    me = MADFind( mad );
    if( me == NULL ) {
        MADStatus( MS_ERR|MS_UNREGISTERED_MAD );
        return( 0 );
    }
    len = strlen( me->file );
    if( buff_size > 0 ) {
        --buff_size;
        if( buff_size > len )
            buff_size = len;
        memcpy( buff, me->file, buff_size );
        buff[buff_size] = '\0';
    }
    return( len );
}

size_t MADNameDescription( dig_mad mad, char *buff, size_t buff_size )
{
    mad_entry   *me;
    size_t      len;

    me = MADFind( mad );
    if( me == NULL ) {
        MADStatus( MS_ERR|MS_UNREGISTERED_MAD );
        return( 0 );
    }
    len = strlen( me->desc );
    if( buff_size > 0 ) {
        --buff_size;
        if( buff_size > len )
            buff_size = len;
        memcpy( buff, me->desc, buff_size );
        buff[buff_size] = '\0';
    }
    return( len );
}

/*
 *      Address Arithmetic
 */

static void DIGREGISTER DummyAddrAdd( address *a, long b, mad_address_format af )
{
    af = af;
    a->mach.offset += b;
}

void            MADAddrAdd( address *a, long b, mad_address_format af )
{
    Active->rtns->MIAddrAdd( a, b, af );
}

static int DIGREGISTER DummyAddrComp( const address *a, const address *b, mad_address_format af )
{
    af = af;
    if( a->mach.offset < b->mach.offset )
        return( -1 );
    if( a->mach.offset >  b->mach.offset )
        return( +1 );
    return( 0 );
}

int MADAddrComp( const address *a, const address *b, mad_address_format af )
{
    return( Active->rtns->MIAddrComp( a, b, af ) );
}

static long DIGREGISTER DummyAddrDiff( const address *a, const address *b, mad_address_format af )
{
    af = af;
    return( a->mach.offset - b->mach.offset );
}

long            MADAddrDiff( const address *a, const address *b, mad_address_format af )
{
    return( Active->rtns->MIAddrDiff( a, b, af ) );
}

static mad_status DIGREGISTER DummyAddrMap( addr_ptr *a, const addr_ptr *map, const addr_ptr *real, const mad_registers *mr )
{
    a = a;
    map = map;
    real = real;
    mr = mr;
    return( MS_FAIL );
}

mad_status      MADAddrMap( addr_ptr *a, const addr_ptr *map, const addr_ptr *real, const mad_registers *mr )
{
    return( Active->rtns->MIAddrMap( a, map, real, mr ) );
}

static mad_status DIGREGISTER DummyAddrFlat( const mad_registers *mr )
{
    mr = mr;
    return( MS_FAIL );
}

mad_status      MADAddrFlat( const mad_registers *mr )
{
    return( Active->rtns->MIAddrFlat( mr ) );
}

static mad_status DIGREGISTER DummyAddrInterrupt( const addr_ptr *a, unsigned size, const mad_registers *mr )
{
    a = a;
    size = size;
    mr = mr;
    return( MS_FAIL );
}

mad_status      MADAddrInterrupt( const addr_ptr *a, unsigned size, const mad_registers *mr )
{
    return( Active->rtns->MIAddrInterrupt( a, size, mr ) );
}

/*
 *      Machine Types
 */

static walk_result DIGREGISTER DummyTypeWalk( mad_type_kind tk, MI_TYPE_WALKER *wk, void *d )
{
    tk = tk;
    wk = wk;
    d  = d;
    return( WR_CONTINUE );
}

struct type_glue {
    MAD_TYPE_WALKER     *wk;
    void                *d;
};

static walk_result DIGCLIENT TypeGlue( mad_type_handle th, void *d )
{
    struct type_glue    *gd = d;

    return( gd->wk( th, gd->d ) );
}

walk_result     MADTypeWalk( mad_type_kind tk, MAD_TYPE_WALKER *wk, void *d )
{
    struct type_glue    glue;

    glue.wk = wk;
    glue.d  = d;
    return( Active->rtns->MITypeWalk( tk, TypeGlue, &glue ) );
}

static mad_string DIGREGISTER DummyTypeName( mad_type_handle th )
{
    th = th;
    return( MAD_MSTR_NIL );
}

mad_string      MADTypeName( mad_type_handle th )
{
    return( Active->rtns->MITypeName( th ) );
}

static mad_radix DIGREGISTER DummyTypePreferredRadix( mad_type_handle th )
{
    th = th;
    return( 0 );
}

mad_radix       MADTypePreferredRadix( mad_type_handle th )
{
    return( Active->rtns->MITypePreferredRadix( th ) );
}

static mad_type_handle DIGREGISTER DummyTypeForDIPType( const dip_type_info *ti )
{
    ti = ti;
    return( MAD_NIL_TYPE_HANDLE );
}

mad_type_handle MADTypeForDIPType( const dip_type_info *ti )
{
    return( Active->rtns->MITypeForDIPType( ti ) );
}

static void DIGREGISTER DummyTypeInfo( mad_type_handle th, mad_type_info *ti )
{
    th = th;
    ti->b.kind = MTK_CUSTOM;
    ti->b.bits = 0;
}

void            MADTypeInfo( mad_type_handle th, mad_type_info  *ti )
{
    Active->rtns->MITypeInfo( th, ti );
}

mad_status      MADTypeInfoForHost( mad_type_kind tk, int size, mad_type_info *mti )
{
    tk &= MTK_ALL;
    mti->b.kind = tk;
    mti->b.handler_code = MAD_DEFAULT_HANDLING;
    if( size < 0 ) {
        mti->b.bits = (dig_size_bits)BYTES2BITS( -size );
    } else {
        mti->b.bits = (dig_size_bits)BYTES2BITS( size );
    }
    mti->i.endian = ME_HOST;
    switch( tk ) {
    case MTK_INTEGER:
        mti->i.nr = MNR_UNSIGNED;
        if( size < 0 )
            mti->i.nr = MNR_HOST_SIGNED;
        mti->i.sign_pos = mti->b.bits - 1;
        break;
    case MTK_ADDRESS:
        mti->a.i.nr = MNR_UNSIGNED;
        if( size == sizeof( address ) )
            mti->b.bits = TYPE2BITS( addr48_ptr );
        mti->a.seg.pos = (byte)( mti->b.bits - TYPE2BITS( addr_seg ) );
        mti->a.seg.bits = TYPE2BITS( addr_seg );
        break;
    case MTK_FLOAT:
        mti->f.exp.base = FLT_RADIX;
#if defined(FLOAT_IEEE)
        mti->f.mantissa.nr = MNR_SIGN_MAG;
        mti->f.mantissa.sign_pos = mti->b.bits - 1;
        switch( size ) {
        case sizeof( sreal ):
            mti->f.exp.hidden = 1;
            mti->f.exp.bias = 127;
            mti->f.exp.data.b.bits = 8;
            break;
        case sizeof( lreal ):
            mti->f.exp.hidden = 1;
            mti->f.exp.bias = 1023;
            mti->f.exp.data.b.bits = 11;
            break;
        case sizeof( xreal ):
            mti->f.exp.hidden = 0;
            mti->f.exp.bias = 16383;
            mti->f.exp.data.b.bits = 15;
            break;
        }
        mti->f.exp.data.sign_pos = mti->f.exp.data.b.bits - 1;
        mti->f.exp.data.nr = MNR_UNSIGNED;
        mti->f.exp.data.endian = ME_HOST;
        mti->f.exp.pos = mti->f.mantissa.sign_pos - mti->f.exp.data.b.bits;
#else
    #error Host floating point info not configured
#endif
        break;
    case MTK_XMM:
        break;
    }
    return( MS_UNSUPPORTED );
}

static mad_type_handle DIGREGISTER DummyTypeDefault( mad_type_kind tk, mad_address_format af, const mad_registers *mr, const address *a )
{
    tk = tk;
    af = af;
    mr = mr;
    a  = a;
    return( 0 );
}

mad_type_handle MADTypeDefault( mad_type_kind tk, mad_address_format af, const mad_registers *mr, const address *a )
{
    return( Active->rtns->MITypeDefault( tk, af, mr, a ) );
}


static mad_status DIGREGISTER DummyTypeConvert( const mad_type_info *ti_src, const void *src, const mad_type_info *ti_dst, void *dst, addr_seg seg )
{
    ti_src = ti_src; src = src; ti_dst = ti_dst; dst = dst; seg = seg;
    return( MS_UNSUPPORTED );
}

typedef union {
    unsigned_64         i;
    addr_ptr            a;
    struct {
        int             exp;
        int             sign;
        unsigned_64     mantissa;
        unsigned_16     extra_slop;     /* makes DecomposeFloat easier */
        enum {
            F_NORMAL,
            F_ZERO,
            F_DENORMAL,
            F_SPECIAL /* nan, inf */
        }               type;
    }                   f;
} decomposed_item;

/* size of decomposed int in bytes - corresponds to 'i' in decomposed_item */
#define DI_SIZE     sizeof( unsigned_64 )
/* size of decomposed mantissa in bytes (f.mantissa) */
#define DF_SIZE     sizeof( unsigned_64 )

/* Convert integer of specified type to native representation (64-bit) */
// NYI: non-two's complement support
static mad_status DecomposeInt( const mad_type_info *mti, const void *src, decomposed_item *v )
{
    unsigned    bytes;
    unsigned    i;
    unsigned    bit_shift;
    unsigned_8  *dst = &v->i.u._8[0];

    bytes = BITS2BYTES( mti->b.bits );
#if defined( __BIG_ENDIAN__ )
    dst += DI_SIZE - bytes;     /* low bytes are higher in memory */
#endif
    /* copy significant (low) bytes */
    if( mti->i.endian == ME_HOST ) {
        memcpy( dst, src, bytes );
    } else {
        for( i = 0; i < bytes; i++ ) {
            dst[i] = ((unsigned_8 *)src)[EndMap[ME_BIG][DI_SIZE - bytes + i]];
        }
    }
    /* clear high bytes */
#if defined( __BIG_ENDIAN__ )
    memset( &v->i.u._8[0], 0, DI_SIZE - bytes );
#else
    memset( &v->i.u._8[bytes], 0, DI_SIZE - bytes );
#endif
    /* sign extend if necessary */
    if( mti->i.nr != MNR_UNSIGNED ) {
        bytes = BYTEIDX( mti->i.sign_pos );
        bit_shift = BITIDX( mti->i.sign_pos );
        if( v->i.u._8[bytes] & (1 << bit_shift) ) {
            v->i.u._8[bytes] |= 0xff << bit_shift;
            ++bytes;
#if defined( __BIG_ENDIAN__ )
            memset( &v->i.u._8[0], 0xff, DI_SIZE - i );
#else
            memset( &v->i.u._8[bytes], 0xff, DI_SIZE - i );
#endif
        }
    }
    return( MS_OK );
}

static mad_status DecomposeAddr( const mad_type_info *mti, const void *src,
                                addr_seg seg, decomposed_item *v )
{
    const void          *valp;

    valp = src;
    if( mti->a.seg.pos == 0 ) {
        /* segment is at the low end - offset above it */
        if( BITIDX( mti->a.seg.bits ) != 0 )
            return( MS_UNSUPPORTED );
        valp = (const unsigned_8 *)src + BYTEIDX( mti->a.seg.bits );
    }
// NYI - address endianness translation doesn't work yet
//    if( mti->i.endian == ME_HOST ) {
        switch( mti->b.bits - mti->a.seg.bits ) {
        case 16:
            v->a.offset = *(unsigned_16 *)valp;
            break;
        case 32:
            v->a.offset = *(unsigned_32 *)valp;
            break;
        default:
            return( MS_UNSUPPORTED );
        }
#if 0
    } else {
        unsigned        bytes;
        unsigned        i;

        switch( mti->b.bits - mti->a.seg.bits ) {
        case 16:
            bytes = 2;
            break;
        case 32:
            bytes = 4;
            break;
        default:
            return( MS_UNSUPPORTED );
        }
        for( i = 0; i < bytes; i++ ) {
            v->i.u._8[i] = ((unsigned_8 *)src)[EndMap[ME_BIG][DI_SIZE - bytes + i]];
        }
    }
#endif
    if( mti->a.seg.bits == 0 ) {
        v->a.segment = seg;
    } else {
        valp = src;
        if( mti->a.seg.pos != 0 ) {
            /* segment is at the high end - offset below it */
            if( BITIDX( mti->a.seg.pos ) != 0 )
                return( MS_UNSUPPORTED );
            valp = (const unsigned_8 *)src + BYTEIDX( mti->a.seg.pos );
        }
        // TODO: byte swap segment also
        switch( mti->a.seg.bits ) {
        case 16:
            v->a.segment = *(unsigned_16 *)valp;
            break;
        case 32:
            v->a.segment = (addr_seg)( *(unsigned_32 *)valp );
            break;
        default:
            return( MS_UNSUPPORTED );
        }
    }
    return( MS_OK );
}

static const unsigned short BitMask[] = {
    0x0000, 0x0001, 0x0003, 0x0007,
    0x000f, 0x001f, 0x003f, 0x007f,
    0x00ff, 0x01ff, 0x03ff, 0x07ff,
    0x0fff, 0x1fff, 0x3fff, 0x7fff,
    0xffff };

static void ShiftBits( unsigned bits, int amount, void *d )
{
    unsigned    bytes;
    unsigned    byte_shift;
    unsigned    bit_shift;
    unsigned_8  tmp1;
    unsigned_8  tmp2;
    unsigned    i;
    int         j;
    bool        neg;

    if( amount != 0 ) {
        bytes = UNALGN_BITS2BYTES( bits );
        neg = false;
        if( amount < 0 ) {
            amount = -amount;
            neg = true;
        }
        byte_shift = BYTEIDX( amount );
        bit_shift = BITIDX( amount );
        if( byte_shift != 0 ) {
            if( !neg ) {
                /* left shift */
#if defined( __BIG_ENDIAN__ )
                memmove( d, (unsigned_8 *)d + byte_shift, bytes - byte_shift );
                memset( (unsigned_8 *)d + byte_shift, 0, byte_shift );
                byte_shift = 0;
#else
                memmove( (unsigned_8 *)d + byte_shift, d, bytes - byte_shift );
                memset( d, 0, byte_shift );
#endif
            } else {
                /* right shift */
#if defined( __BIG_ENDIAN__ )
                memmove( (unsigned_8 *)d + byte_shift, d, bytes - byte_shift );
                memset( d, 0, byte_shift );
                byte_shift = 0;
#else
                memmove( d, (unsigned_8 *)d + byte_shift, bytes - byte_shift );
                memset( (unsigned_8 *)d + bytes - byte_shift, 0, byte_shift );
#endif
            }
        }
        if( bit_shift != 0 ) {
            tmp1 = 0;
            if( !neg ) {
                /* left shift */
                for( i = byte_shift; i < bytes; ++i ) {
                    tmp2 = ((unsigned_8 *)d)[i];
                    ((unsigned_8 *)d)[i] = (((unsigned_8 *)d)[i] << bit_shift) | (tmp1 >> (BYTES2BITS( 1 ) - bit_shift));
                    tmp1 = tmp2;
                }
            } else {
                /* right shift */
                for( j = bytes - byte_shift; j-- > 0; ) {
                    tmp2 = ((unsigned_8 *)d)[j];
                    ((unsigned_8 *)d)[j] = (((unsigned_8 *)d)[j] >> bit_shift) | (tmp1 << (BYTES2BITS( 1 ) - bit_shift));
                    tmp1 = tmp2;
                }
            }
        }
    }
}

static void ExtractBits( dig_size_bits pos, dig_size_bits len, const void *src, void *dst, int dst_size )
{
    unsigned_64         tmp;
    unsigned            bytes;
    int                 bit_shift;

#if !defined( __BIG_ENDIAN__ )
    dst_size = dst_size;
#endif
    src = (unsigned_8 *)src + BYTEIDX( pos );
    bit_shift = BITIDX( pos );
    memset( &tmp, 0, sizeof( tmp ) );
    memcpy( &tmp, src, UNALGN_BITS2BYTES( bit_shift + len ) );
    ShiftBits( bit_shift + len, -(signed char)bit_shift, &tmp );
    tmp.u._8[BYTEIDX( len )] &= BitMask[BITIDX( len )];
    bytes = UNALGN_BITS2BYTES( len );
#if defined( __BIG_ENDIAN__ )
    dst = (unsigned_8 *)dst + dst_size - bytes;
#endif
    memcpy( dst, &tmp, bytes );
}

static void InsertBits( dig_size_bits pos, dig_size_bits len, const void *src, void *dst )
{
    unsigned_64         tmp;
    unsigned            bytes;
    unsigned            i;
    int                 bit_shift;

    bytes = UNALGN_BITS2BYTES( len );
    dst = (unsigned_8 *)dst + BYTEIDX( pos );
    bit_shift = BITIDX( pos );
    memset( &tmp, 0, sizeof( tmp ) );
    memcpy( &tmp, src, bytes );
    bytes = UNALGN_BITS2BYTES( bit_shift + len );
    ShiftBits( bit_shift + len, bit_shift, &tmp );
    for( i = 0; i < bytes; ++i ) {
        ((unsigned_8 *)dst)[i] |= ((unsigned_8 *)&tmp)[i];
    }
}

//NYI: non radix 2, sign-magnitude floats
static mad_status DecomposeFloat( const mad_type_info *mti, const void *src, decomposed_item *v )
{
    unsigned    bytes;
    unsigned    mant_bits;
    unsigned    shifts;
    unsigned    i;
    unsigned_8  *dst = &v->f.mantissa.u._8[0];

    memset( v, 0, sizeof( *v ) );
    bytes = BITS2BYTES( mti->b.bits );
    if( memcmp( &v->f.mantissa, src, bytes ) == 0 ) {
        v->f.type = F_ZERO;
        /* number is zero */
        return( MS_OK );
    }

#if defined( __BIG_ENDIAN__ )
    dst += DF_SIZE - bytes;     /* low bytes are higher in memory */
#endif
    if( mti->i.endian == ME_HOST ) {
        memcpy( dst, src, bytes );
    } else {
        for( i = 0; i < bytes; i++ ) {
            dst[i] = ((unsigned_8 *)src)[EndMap[ME_BIG][DF_SIZE - bytes + i]];
        }
    }

    ExtractBits( mti->f.exp.pos, mti->f.exp.data.b.bits, &v->f.mantissa, &v->f.exp, sizeof( v->f.exp ) );
    /* Assuming IEEE here */
    if( v->f.exp == 0 ) {
        v->f.type = F_DENORMAL;
    } else if( v->f.exp == BitMask[mti->f.exp.data.b.bits] ) {
        v->f.type = F_SPECIAL;
    }
    v->f.exp -= mti->f.exp.bias;
    ExtractBits( mti->f.mantissa.sign_pos, 1, &v->f.mantissa, &v->f.sign, sizeof( v->f.sign ) );
    mant_bits = mti->b.bits - mti->f.exp.data.b.bits - 1;
    if( mti->f.exp.hidden && (v->f.type != F_DENORMAL) )
        mant_bits += 1;
    ShiftBits( TYPE2BITS( v->f.mantissa ), TYPE2BITS( v->f.mantissa ) - mant_bits, &v->f.mantissa );
    if( v->f.type == F_DENORMAL ) {
        for( shifts = 0; (v->f.mantissa.u._8[sizeof( v->f.mantissa ) - 1] & 0x80) == 0; ++shifts ) {
            ShiftBits( TYPE2BITS( v->f.mantissa ), 1, &v->f.mantissa );
            if( shifts >= TYPE2BITS( v->f.mantissa ) ) {
                /* mantissa is all zero bits - we must have gotten a -0.0 */
                v->f.type = F_ZERO;
                v->f.exp = 0;
                return( MS_OK );
            }
            v->f.exp--;
        }
        v->f.type = F_NORMAL;
    }
    if( mti->f.exp.hidden && (v->f.type != F_DENORMAL) ) {
        v->f.mantissa.u._8[sizeof( v->f.mantissa ) - 1] |= 0x80;
    }
    return( MS_OK );
}

/* Convert native integer to specified representation */
static mad_status ComposeInt( decomposed_item *v, const mad_type_info *mti, void *dst )
{
    unsigned    bytes;
    unsigned    i;
    unsigned_8  *src = &v->i.u._8[0];

    bytes = BITS2BYTES( mti->b.bits );
#if defined( __BIG_ENDIAN__ )
    src += DI_SIZE - bytes;     /* low bytes are higher in memory */
#endif
    if( mti->i.endian == ME_HOST ) {
        memcpy( dst, src, bytes );
    } else {
        for( i = 0; i < bytes; i++ ) {
            ((unsigned_8 *)dst)[EndMap[ME_BIG][DI_SIZE - bytes + i]] = src[i];
        }
    }
    return( MS_OK );
}

static mad_status ComposeAddr( decomposed_item *v, const mad_type_info *mti, void *dst )
{
    unsigned    bytes;

    if( mti->a.seg.pos == 0 ) {
        bytes = BITS2BYTES( mti->a.seg.bits );
        memcpy( dst, &v->a.segment, bytes );
        dst = (unsigned_8 *)dst + bytes;
        bytes = BITS2BYTES( mti->b.bits - mti->a.seg.bits );
        memcpy( dst, &v->a.offset, bytes );
    } else {
        bytes = BITS2BYTES( mti->b.bits - mti->a.seg.bits );
        memcpy( dst, &v->a.offset, bytes );
        dst = (unsigned_8 *)dst + bytes;
        bytes = BITS2BYTES( mti->a.seg.bits );
        memcpy( dst, &v->a.segment, bytes );
    }
    return( MS_OK );
}

static mad_status ComposeFloat( decomposed_item *v, const mad_type_info *mti, void *dst )
{
    unsigned    bytes;
    unsigned    mant_bits;
    unsigned    i;

    bytes = BITS2BYTES( mti->b.bits );
    memset( dst, 0, bytes );
    if( v->f.type == F_ZERO ) {
        /* number is zero */
        if( v->f.sign != 0 ) {
            InsertBits( mti->f.mantissa.sign_pos, 1, &v->f.sign, dst );
        }
        return( MS_OK );
    }
    mant_bits = mti->b.bits - mti->f.exp.data.b.bits - 1;
    if( mti->f.exp.hidden ) {
        v->f.mantissa.u._8[sizeof( v->f.mantissa ) - 1] &= ~0x80;
        mant_bits += 1;
    }
    if( v->f.type == F_SPECIAL ) {
        v->f.exp = BitMask[mti->f.exp.data.b.bits];
    } else {
        v->f.exp += mti->f.exp.bias;
        if( v->f.exp <= 0 ) {
            /* denormal */
            if( mti->f.exp.hidden ) {
                v->f.mantissa.u._8[sizeof( v->f.mantissa ) - 1] |= 0x80;
            }
            for( ; v->f.exp < 0; v->f.exp++ ) {
                ShiftBits( TYPE2BITS( v->f.mantissa ), -1, &v->f.mantissa );
            }
        } else if( v->f.exp >= BitMask[mti->f.exp.data.b.bits] ) {
            /* infinity */
            memset( &v->f.mantissa, 0, sizeof( v->f.mantissa ) );
            v->f.exp = BitMask[mti->f.exp.data.b.bits];
        }
    }
    ShiftBits( TYPE2BITS( v->f.mantissa ),
        mant_bits - TYPE2BITS( v->f.mantissa ), &v->f.mantissa );
    memcpy( dst, &v->f.mantissa, bytes );
    InsertBits( mti->f.exp.pos, mti->f.exp.data.b.bits, &v->f.exp, dst );
    if( v->f.sign != 0 ) {
        InsertBits( mti->f.mantissa.sign_pos, 1, &v->f.sign, dst );
    }
    if( mti->i.endian != ME_HOST ) {
        for( i = 0; i < bytes / 2; i++ ) {
            unsigned_8  tmp;

            tmp = ((unsigned_8 *)dst)[bytes - i - 1];
            ((unsigned_8 *)dst)[bytes - i - 1] = ((unsigned_8 *)dst)[i];
            ((unsigned_8 *)dst)[i] = tmp;
        }
    }
    return( MS_OK );
}

static mad_status DoConversion( const mad_type_info *in_t, const void *in_d, const mad_type_info *out_t, void *out_d, addr_seg seg )
{
    mad_status          ms;
    decomposed_item     value;

    if( in_t->b.kind != out_t->b.kind )
        return( MS_UNSUPPORTED );
    switch( in_t->b.kind ) {
    case MTK_INTEGER:
        ms = DecomposeInt( in_t, in_d, &value );
        if( ms != MS_OK )
            return( ms );
        return( ComposeInt( &value, out_t, out_d ) );
    case MTK_ADDRESS:
        ms = DecomposeAddr( in_t, in_d, seg, &value );
        if( ms != MS_OK )
            return( ms );
        return( ComposeAddr( &value, out_t, out_d ) );
    case MTK_FLOAT:
        ms = DecomposeFloat( in_t, in_d, &value );
        if( ms != MS_OK )
            return( ms );
        return( ComposeFloat( &value, out_t, out_d ) );
    }
    return( MS_UNSUPPORTED );
}

mad_status      MADTypeConvert( const mad_type_info *in_t, const void *in_d, const mad_type_info *out_t, void *out_d, addr_seg seg )
{
    mad_status  ms;

    if( in_t->b.handler_code  == MAD_DEFAULT_HANDLING && out_t->b.handler_code == MAD_DEFAULT_HANDLING ) {
        ms = DoConversion( in_t, in_d, out_t, out_d, seg );
        if( ms == MS_OK ) {
            return( MS_OK );
        }
    }
    return( Active->rtns->MITypeConvert( in_t, in_d, out_t, out_d, seg ) );
}

static mad_status DIGREGISTER DummyTypeToString( mad_radix radix, const mad_type_info *mti, const void *d, char *buff, size_t *buff_size_p )
{
    radix = radix;
    mti = mti;
    d = d;
    buff_size_p = buff_size_p;
    buff = buff;
    return( MS_UNSUPPORTED );
}

static const char DigitTab[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

//NYI: big endian hosts & targets
static char *U64CvtNum( unsigned_64 val, mad_radix radix, char *p, int bit_length )
{
    size_t              len;
    char                save;
    int                 digits;
    unsigned_64         rem;
    unsigned_64         divisor;

    switch( radix ) {
    case 2:
        digits = bit_length;
        break;
    case 16:
        digits = 2 * BITS2BYTES( bit_length );
        break;
    default:
        digits = 0;
        break;
    }
    U32ToU64( radix, &divisor );
    do {
        U64Div( &val, &divisor, &val, &rem );
        *--p = DigitTab[rem.u._32[I64LO32]];
        --digits;
    } while( val.u._32[I64LO32] != 0 || val.u._32[I64HI32] != 0 || digits > 0 );
    len = MADCliRadixPrefix( radix, NULL, 0 );
    p -= len;
    save = p[len];
    MADCliRadixPrefix( radix, p, len + 1 );
    p[len] = save; /* got overwritten with a '\0' */
    return( p );
}

static char *CvtNum( unsigned long val, mad_radix radix, char *p, int bit_length )
{
    unsigned_64 tmp;

    U32ToU64( val, &tmp );
    return( U64CvtNum( tmp, radix, p, bit_length ) );
}

static mad_status IntTypeToString( mad_radix radix, mad_type_info const *mti, const void *d, char *buff, size_t *buff_size_p )
{
    decomposed_item     val;
    bool                neg;
    char                buff1[128];
    char                *p;
    size_t              buff_size;
    size_t              len;
    mad_status          ms;


    ms = DecomposeInt( mti, d, &val );
    if( ms != MS_OK )
        return( ms );
    neg = false;
    if( mti->i.nr != MNR_UNSIGNED && val.i.u.sign.v ) {
        neg = true;
        U64Neg( &val.i, &val.i );
    }
    p = U64CvtNum( val.i, radix, buff1 + sizeof( buff1 ), mti->b.bits );
    if( neg )
        *--p = '-';
    len = buff1 + sizeof( buff1 ) - p;
    buff_size = *buff_size_p;
    *buff_size_p = len;
    if( buff_size > 0 ) {
        --buff_size;
        if( buff_size > len )
            buff_size = len;
        memcpy( buff, p, buff_size );
        buff[buff_size] = '\0';
    }
    return( MS_OK );
}

static mad_status AddrTypeToString( mad_radix radix, mad_type_info const *mti, const void *d, char *buff, size_t *buff_size_p )
{
    decomposed_item     val;
    char                *p;
    size_t              buff_size;
    size_t              len;
    char                buff1[80];
    mad_status          ms;

    ms = DecomposeAddr( mti, d, 0, &val );
    if( ms != MS_OK )
        return( ms );
    p = CvtNum( val.a.offset, radix, buff1 + sizeof( buff1 ), mti->b.bits - mti->a.seg.bits );
    if( mti->a.seg.bits != 0 ) {
        *--p = ':';
        p = CvtNum( val.a.segment, radix, p, mti->a.seg.bits );
    }
    len = buff1 + sizeof( buff1 ) - p;
    buff_size = *buff_size_p;
    *buff_size_p = len;
    if( buff_size > 0 ) {
        --buff_size;
        if( buff_size > len )
            buff_size = len;
        memcpy( buff, p, buff_size );
        buff[buff_size] = '\0';
    }
    return( MS_OK );
}

#if !defined(MAD_OMIT_FLOAT_CVT)

#define LOG10B2( v )    (((v) * 30103L ) / 100000L )
#define MAX_DIGITS      21

#ifdef __WATCOMC__
static char *fixup( char *p, int n )
{
    char    *start = p;

    if( n < MAX_DIGITS && isdigit( *p ) ) {
        while( *p != '\0' ) {
            p++;
            n--;
        }
        while( n > 0 ) {
            *p++ = '0';
            n--;
        }
        *p = '\0';
    }
    return( start );
}

static char *__xcvt( long_double *value, int ndigits, int *dec, int *sign, char *buf )
{
    CVT_INFO    cvt;

    cvt.flags = G_FMT + F_CVT + NO_TRUNC + LONG_DOUBLE;
    cvt.scale = 1;
    cvt.ndigits = ndigits;
    cvt.expwidth = 0;
    cvt.expchar  = 0;
    __LDcvt( value, &cvt, buf );
    *dec = cvt.decimal_place;
    *sign = cvt.sign;
    fixup( buf, ndigits );
    return( buf );
}
#endif

static size_t DoStrReal( long_double *value, mad_type_info const *mti, char *buff, size_t buff_size )
{
    unsigned    mant_digs;
//    unsigned    exp_digs;
    char        *p;
    int         sign;
    int         exp;
    size_t      len;
    char        buff1[80];
#ifdef __WATCOMC__
    char        buff2[80];
#endif

//    exp_digs = LOG10B2( mti->f.exp.data.b.bits * (mti->f.exp.base / 2) );
    mant_digs = LOG10B2( mti->b.bits + mti->f.exp.hidden - ( mti->f.exp.data.b.bits + 1 ) );
#ifdef __WATCOMC__
    p = __xcvt( value, mant_digs, &exp, &sign, buff2 );
#else
    p = ecvt( value->u.value, mant_digs, &exp, &sign );
#endif
    if( isdigit( *p ) ) {
        if( *p != '0' )
            --exp;
        sprintf( buff1, "%c%c.%sE%+4.4d", ( sign ) ? '-' : '+', *p, p + 1, exp );
        p = buff1;
    }
    len = strlen( p );
    if( buff_size > 0 ) {
        --buff_size;
        if( buff_size > len )
            buff_size = len;
        memcpy( buff, p, buff_size );
        buff[buff_size] = '\0';
    }
    return( len );
}

static mad_status FloatTypeToString( mad_radix radix, mad_type_info const *mti, const void *d, char *buff, size_t *buff_size_p )
{
    size_t              buff_size;
    mad_type_info       host;
    unsigned_8    const *p;
    size_t              len;
    mad_status          ms;
#if defined( _LONG_DOUBLE_ )
    xreal               val;
#else
    lreal               val;
#endif

    buff_size = *buff_size_p;
    switch( radix ) {
    case 16:
        p = d;
#if !defined( __BIG_ENDIAN__ )
        p += BITS2BYTES( mti->b.bits );
#endif
        for( len = 0; len < 2 * BITS2BYTES( mti->b.bits ); len += 2 ) {
#if !defined( __BIG_ENDIAN__ )
            --p;
#endif
            if( len + 1 < buff_size ) {
                buff[len + 0] = DigitTab[*p >> 4];
                buff[len + 1] = DigitTab[*p & 0x0f];
            }
#if defined( __BIG_ENDIAN__ )
            ++p;
#endif
        }
        *buff_size_p = len;
        if( buff_size > 0 ) {
            --buff_size;
            if( buff_size > len )
                buff_size = len;
            buff[buff_size] = '\0';
        }
        return( MS_OK );
    case 10:
        MADTypeInfoForHost( MTK_FLOAT, sizeof( val ), &host );
        ms = MADTypeConvert( mti, d, &host, &val, 0 );
        if( ms != MS_OK )
            return( ms );
        *buff_size_p = DoStrReal( (long_double *)&val, mti, buff, buff_size );
        return( MS_OK );
    }
    return( MS_UNSUPPORTED );
}

#endif

mad_status MADTypeToString( mad_radix radix, const mad_type_info *mti, const void *d, char *buff, size_t *buff_size_p )
{
    if( mti->b.handler_code == MAD_DEFAULT_HANDLING ) {
        switch( mti->b.kind ) {
        case MTK_INTEGER:
            if( IntTypeToString( radix, mti, d, buff, buff_size_p ) == MS_OK )
                return( MS_OK );
            break;
        case MTK_ADDRESS:
            if( AddrTypeToString( radix, mti, d, buff, buff_size_p ) == MS_OK )
                return( MS_OK );
            break;
#if !defined(MAD_OMIT_FLOAT_CVT)
        case MTK_FLOAT:
            if( FloatTypeToString( radix, mti, d, buff, buff_size_p ) == MS_OK )
                return( MS_OK );
            break;
        }
#endif
    }
    return( Active->rtns->MITypeToString( radix, mti, d, buff, buff_size_p ) );
}

mad_status MADTypeHandleToString( mad_radix radix, mad_type_handle th, const void *d, char *buff, size_t *buff_size_p )
{
    mad_type_info       mti;

    MADTypeInfo( th, &mti );
    return( MADTypeToString( radix, &mti, d, buff, buff_size_p ) );
}


/*
 *      Machine Registers
 */

static unsigned DIGREGISTER DummyRegistersSize( void )
{
    return( 1 );
}

unsigned        MADRegistersSize( void )
{
    return( Active->rtns->MIRegistersSize() );
}

static mad_status DIGREGISTER DummyRegistersHost( mad_registers *mr )
{
    mr = mr;
    return( MS_OK );
}

mad_status      MADRegistersHost( mad_registers *mr )
{
    return( Active->rtns->MIRegistersHost( mr ) );
}

static mad_status DIGREGISTER DummyRegistersTarget( mad_registers *mr )
{
    mr = mr;
    return( MS_OK );
}

mad_status      MADRegistersTarget( mad_registers *mr )
{
    return( Active->rtns->MIRegistersTarget( mr ) );
}

static walk_result DIGREGISTER DummyRegSetWalk( mad_type_kind tk, MI_REG_SET_WALKER *wk, void *d )
{
    tk = tk;
    wk = wk;
    d  = d;
    return( WR_CONTINUE );
}

struct regset_glue {
    MAD_REG_SET_WALKER  *wk;
    void                *d;
};

static walk_result DIGCLIENT RegSetGlue( const mad_reg_set_data *rsd, void *d )
{
    struct regset_glue  *gd = d;

    return( gd->wk( rsd, gd->d ) );
}

walk_result     MADRegSetWalk( mad_type_kind tk, MAD_REG_SET_WALKER *wk, void *d )
{
    struct regset_glue  glue;

    glue.wk = wk;
    glue.d  = d;
    return( Active->rtns->MIRegSetWalk( tk, &RegSetGlue, &glue ) );
}

static mad_string DIGREGISTER DummyRegSetName( const mad_reg_set_data *rsd )
{
    rsd = rsd;
    return( MAD_MSTR_NIL );
}

mad_string      MADRegSetName( const mad_reg_set_data *rsd )
{
    return( Active->rtns->MIRegSetName( rsd ) );
}

static size_t DIGREGISTER DummyRegSetLevel( const mad_reg_set_data *rsd, char *buff, size_t buff_size )
{
    rsd = rsd;
    if( buff_size > 0 )
        buff[0] = '\0';
    return( 0 );
}

size_t MADRegSetLevel( const mad_reg_set_data *rsd, char *buff, size_t buff_size )
{
    return( Active->rtns->MIRegSetLevel( rsd, buff, buff_size ) );
}

static unsigned DIGREGISTER DummyRegSetDisplayGrouping( const mad_reg_set_data *rsd )
{
    rsd = rsd;
    return( 0 );
}

unsigned        MADRegSetDisplayGrouping( const mad_reg_set_data *rsd )
{
    return( Active->rtns->MIRegSetDisplayGrouping( rsd ) );
}

static mad_status DIGREGISTER DummyRegSetDisplayGetPiece( const mad_reg_set_data *rsd, const mad_registers *mr, unsigned piece, const char **descript_p, size_t *max_descript_p, const mad_reg_info **reg, mad_type_handle *disp_type, size_t *max_value )
{
    rsd = rsd;
    mr = mr;
    piece = piece;
    descript_p = descript_p;
    max_descript_p = max_descript_p;
    reg = reg;
    disp_type = disp_type;
    max_value = max_value;
    return( MS_FAIL );
}

mad_status      MADRegSetDisplayGetPiece( const mad_reg_set_data *rsd, const mad_registers *mr, unsigned piece, const char **descript_p, size_t *max_descript_p, const mad_reg_info **reg, mad_type_handle *disp_type, size_t *max_value )
{
    return( Active->rtns->MIRegSetDisplayGetPiece( rsd, mr, piece, descript_p,
                max_descript_p, reg, disp_type, max_value ) );
}

static mad_status DIGREGISTER DummyRegSetDisplayModify( const mad_reg_set_data *rsd, const mad_reg_info *reg, const mad_modify_list **possible, int *num_possible )
{
    rsd = rsd;
    reg = reg;
    *possible = NULL;
    *num_possible = 0;
    return( MS_FAIL );
}

mad_status      MADRegSetDisplayModify( const mad_reg_set_data *rsd, const mad_reg_info *reg, const mad_modify_list **possible, int *num_possible )
{
    return( Active->rtns->MIRegSetDisplayModify( rsd, reg, possible, num_possible ) );
}


static const mad_toggle_strings *DIGREGISTER DummyRegSetDisplayToggleList( const mad_reg_set_data *rsd )
{
    rsd = rsd;
    return( EmptyToggleList );
}

const mad_toggle_strings *MADRegSetDisplayToggleList( const mad_reg_set_data *rsd )
{
    return( Active->rtns->MIRegSetDisplayToggleList( rsd ) );
}

static unsigned DIGREGISTER DummyRegSetDisplayToggle( const mad_reg_set_data *rsd, unsigned on, unsigned off )
{
    rsd = rsd;
    on = on;
    off = off;
    return( 0 );
}

unsigned        MADRegSetDisplayToggle( const mad_reg_set_data *rsd, unsigned on, unsigned off )
{
    return( Active->rtns->MIRegSetDisplayToggle( rsd, on, off ) );
}

static mad_status DIGREGISTER DummyRegModified( const mad_reg_set_data *rsd, const mad_reg_info *ri, const mad_registers *old, const mad_registers *curr )
{
    rsd = rsd;
    ri = ri;
    old = old;
    curr = curr;
    return( MS_OK );
}

mad_status      MADRegModified( const mad_reg_set_data *rsd, const mad_reg_info *ri, const mad_registers *old, const mad_registers *curr )
{
    return( Active->rtns->MIRegModified( rsd, ri, old, curr ) );
}

static mad_status DIGREGISTER DummyRegInspectAddr( const mad_reg_info *ri, const mad_registers *mr, address *a )
{
    ri = ri;
    mr = mr;
    a = a;
    return( MS_FAIL );
}

mad_status      MADRegInspectAddr( const mad_reg_info *ri, const mad_registers *mr, address *a )
{
    return( Active->rtns->MIRegInspectAddr( ri, mr, a ) );
}

static walk_result DIGREGISTER DummyRegWalk( const mad_reg_set_data *rsd, const mad_reg_info *ri, MI_REG_WALKER *wk, void *d )
{
    rsd = rsd;
    ri = ri;
    wk = wk;
    d = d;
    return( WR_CONTINUE );
}

struct reg_glue {
    MAD_REG_WALKER      *wk;
    void                *d;
};

static walk_result DIGCLIENT RegGlue( const mad_reg_info *ri, int has_sublist, void *d )
{
    struct reg_glue     *gd = d;

    return( gd->wk( ri, has_sublist, gd->d ) );
}

static walk_result DIGCLIENT AllRegWalk( const mad_reg_set_data *rsd, void *d )
{
    return( Active->rtns->MIRegWalk( rsd, NULL, &RegGlue, d ) );
}

walk_result     MADRegWalk( const mad_reg_set_data *rsd, const mad_reg_info *ri, MAD_REG_WALKER *wk, void *d )
{
    struct reg_glue     glue;

    glue.wk = wk;
    glue.d  = d;
    if( rsd == NULL && ri == NULL ) {
        return( Active->rtns->MIRegSetWalk( MTK_ALL, &AllRegWalk, &glue ) );
    }
    return( Active->rtns->MIRegWalk( rsd, ri, &RegGlue, &glue ) );
}

struct full_name_component {
    struct full_name_component  *parent;
    mad_reg_info const          *ri;
};

struct full_name {
    struct full_name_component  *components;
    mad_reg_info  const         *ri;
    const char                  *op;
    char                        *buff;
    size_t                      buff_size;
    size_t                      len;
};

static walk_result FindFullName( const mad_reg_info *ri, int has_sublist, void *d )
{
    struct full_name            *name = d;
    struct full_name_component  curr;
    struct full_name_component  *p;
    struct full_name_component  *h;
    struct full_name_component  *t;
    walk_result                 wr;
    size_t                      op_len;
    size_t                      amount;
    bool                        first;

    curr.parent = name->components;
    name->components = &curr;
    curr.ri = ri;
    if( ri == name->ri ) {
        op_len = strlen( name->op );
        /* reverse the list, calculate length */
        p = name->components;
        h = NULL;
        while( p != NULL ) {
            name->len += strlen( p->ri->name ) + op_len;
            t = p->parent;
            p->parent = h;
            h = p;
            p = t;
        }
        name->len -= op_len; /* take off extra op_len */
        /* lay down name */
        first = true;
        for( p = h; p != NULL; p = p->parent ) {
            if( !first ) {
                amount = name->buff_size;
                if( amount > op_len )
                    amount = op_len;
                memcpy( name->buff, name->op, amount );
                name->buff += amount;
                name->buff_size -= amount;
            }
            first = false;
            amount = strlen( p->ri->name );
            if( amount > name->buff_size )
                amount = name->buff_size;
            memcpy( name->buff, p->ri->name, amount );
            name->buff += amount;
            name->buff_size -= amount;
        }
        return( WR_STOP );
    }
    wr = WR_CONTINUE;
    if( has_sublist ) {
        wr = MADRegWalk( NULL, ri, FindFullName, name );
    }
    name->components = curr.parent;
    return( wr );
}

size_t MADRegFullName( const mad_reg_info *ri, const char *op, char *buff, size_t buff_size )
{
    struct full_name    name;

    name.components = NULL;
    name.ri = ri;
    name.op = op;
    name.buff = buff;
    name.buff_size = buff_size;
    name.len = 0;
    MADRegWalk( NULL, NULL, FindFullName, &name );
    if( buff_size > 0 ) {
        if( buff_size > name.len )
            buff_size = name.len;
        buff[buff_size] = '\0';
    }
    return( name.len );
}

static void DIGREGISTER DummyRegSpecialGet( mad_special_reg sr, const mad_registers *mr, addr_ptr *a )
{
    sr = sr;
    mr = mr;
    a->segment = 0;
    a->offset = 0;
}

void            MADRegSpecialGet( mad_special_reg sr, const mad_registers *mr, addr_ptr *a )
{
    Active->rtns->MIRegSpecialGet( sr, mr, a );
}

static void DIGREGISTER DummyRegSpecialSet( mad_special_reg sr, mad_registers *mr, const addr_ptr *a )
{
    sr = sr;
    mr = mr;
    a = a;
}

void            MADRegSpecialSet( mad_special_reg sr, mad_registers *mr, const addr_ptr *a )
{
    Active->rtns->MIRegSpecialSet( sr, mr, a );
}

static size_t DIGREGISTER DummyRegSpecialName( mad_special_reg sr, const mad_registers *mr, mad_address_format af, char *buff, size_t buff_size )
{
    sr = sr;
    mr = mr;
    af = af;
    buff_size = buff_size;
    buff = buff;
    return( 0 );
}

size_t MADRegSpecialName( mad_special_reg sr, const mad_registers *mr, mad_address_format af, char *buff, size_t buff_size )
{
    return( Active->rtns->MIRegSpecialName( sr, mr, af, buff, buff_size ) );
}

static const mad_reg_info *DIGREGISTER DummyRegFromContextItem( context_item ci )
{
    ci = ci;
    return( NULL );
}

const mad_reg_info *MADRegFromContextItem( context_item ci )
{
    return( Active->rtns->MIRegFromContextItem( ci ) );
}

static void DIGREGISTER DummyRegUpdateStart( mad_registers *mr, unsigned flags, unsigned bit_start, unsigned bit_size )
{
    mr = mr;
    flags = flags;
    bit_start = bit_start;
    bit_size = bit_size;
}

void            MADRegUpdateStart( mad_registers *mr, unsigned flags, unsigned bit_start, unsigned bit_size )
{
    Active->rtns->MIRegUpdateStart( mr, flags, bit_start, bit_size );
}

static void DIGREGISTER DummyRegUpdateEnd( mad_registers *mr, unsigned flags, unsigned bit_start, unsigned bit_size )
{
    mr = mr;
    flags = flags;
    bit_start = bit_start;
    bit_size = bit_size;
}

void            MADRegUpdateEnd( mad_registers *mr, unsigned flags, unsigned bit_start, unsigned bit_size )
{
    Active->rtns->MIRegUpdateEnd( mr, flags, bit_start, bit_size );
}


/*
 *      Calling Conventions
 */


static mad_status DIGREGISTER DummyCallStackGrowsUp( void )
{
    return( MS_FAIL );
}

mad_status      MADCallStackGrowsUp( void )
{
    return( Active->rtns->MICallStackGrowsUp() );
}

static const mad_string *DIGREGISTER DummyCallTypeList( void )
{
    return( EmptyStrList );
}

const mad_string        *MADCallTypeList( void )
{
    return( Active->rtns->MICallTypeList() );
}

static mad_status DIGREGISTER DummyCallBuildFrame( mad_string ct, address ret, address rtn, const mad_registers *in, mad_registers *out )
{
    ct = ct;
    ret = ret;
    rtn = rtn;
    in = in;
    out = out;
    return( MS_UNSUPPORTED );
}

mad_status              MADCallBuildFrame( mad_string ct, address ret, address rtn, const mad_registers *in, mad_registers *out )
{
    return( Active->rtns->MICallBuildFrame( ct, ret, rtn, in, out ) );
}

static const mad_reg_info * DIGREGISTER DummyCallReturnReg( mad_string ct, address a )
{
    ct = ct;
    a = a;
    return( NULL );
}

const mad_reg_info      *MADCallReturnReg( mad_string ct, address a )
{
    return( Active->rtns->MICallReturnReg( ct, a ) );
}

static const mad_reg_info **DIGREGISTER DummyCallParmRegList( mad_string ct, address a )
{
    static const mad_reg_info *list[] = { NULL };

    ct = ct;
    a = a;
    return( list );
}

const mad_reg_info      **MADCallParmRegList( mad_string ct, address a )
{
    return( Active->rtns->MICallParmRegList( ct, a ) );
}

static unsigned DIGREGISTER DummyCallUpStackSize( void )
{
    return( 1 );
}

unsigned                MADCallUpStackSize( void )
{
    return( Active->rtns->MICallUpStackSize() );
}

static mad_status DIGREGISTER DummyCallUpStackInit( mad_call_up_data *cud, const mad_registers *mr )
{
    cud = cud;
    mr = mr;
    return( MS_OK );
}

mad_status              MADCallUpStackInit( mad_call_up_data *cud, const mad_registers *mr )
{
    return( Active->rtns->MICallUpStackInit( cud, mr ) );
}

static mad_status DIGREGISTER DummyCallUpStackLevel( mad_call_up_data *cud, address const *start, unsigned rtn_characteristics, long return_disp, mad_registers const *in, address *execution, address *frame, address *stack, mad_registers **out )
{
    cud = cud;
    start = start;
    rtn_characteristics = rtn_characteristics;
    return_disp = return_disp;
    in = in;
    execution = execution;
    frame = frame;
    stack = stack;
    out = out;
    return( MS_FAIL );
}

mad_status              MADCallUpStackLevel( mad_call_up_data *cud, address const *start, unsigned rtn_characteristics, long return_disp, const mad_registers *in, address *execution, address *frame, address *stack, mad_registers **out )
{
    return( Active->rtns->MICallUpStackLevel( cud, start, rtn_characteristics,
                return_disp, in, execution, frame, stack, out ) );
}

/*
 *      Instruction Disassembly
 */

#define ILL_INSTR       "????"

static unsigned DIGREGISTER DummyDisasmDataSize( void )
{
    return( 1 );
}

unsigned                MADDisasmDataSize( void )
{
    return( Active->rtns->MIDisasmDataSize() );
}

static unsigned DIGREGISTER DummyDisasmNameMax( void )
{
    return( sizeof( ILL_INSTR ) - 1 );
}

unsigned                MADDisasmNameMax( void )
{
    return( Active->rtns->MIDisasmNameMax() );
}

static mad_status DIGREGISTER DummyDisasm( mad_disasm_data *dd, address *a, int adj )
{
    dd = dd;
    a->mach.offset += adj;
    return( MS_OK );
}

mad_status              MADDisasm( mad_disasm_data *dd, address *a, int adj )
{
    return( Active->rtns->MIDisasm( dd, a, adj ) );
}

static size_t DIGREGISTER DummyDisasmFormat( mad_disasm_data *dd, mad_disasm_piece dp, mad_radix radix, char *buff, size_t buff_size )
{
    dd = dd;
    radix = radix;
    if( (dp & MDP_INSTRUCTION) == 0 )
        return( 0 );
    if( buff_size > 0 ) {
        --buff_size;
        if( buff_size > sizeof( ILL_INSTR ) - 1 )
            buff_size = sizeof( ILL_INSTR ) - 1;
        memcpy( buff, ILL_INSTR, buff_size );
        buff[buff_size] = '\0';
    }
    return( sizeof( ILL_INSTR ) - 1 );
}

size_t MADDisasmFormat( mad_disasm_data *dd, mad_disasm_piece dp, mad_radix radix, char *buff, size_t buff_size )
{
    return( Active->rtns->MIDisasmFormat( dd, dp, radix, buff, buff_size ) );
}

static unsigned DIGREGISTER DummyDisasmInsSize( mad_disasm_data *dd )
{
    dd = dd;
    return( 1 );
}

unsigned    MADDisasmInsSize( mad_disasm_data *dd )
{
    return( Active->rtns->MIDisasmInsSize( dd ) );
}

static mad_status DIGREGISTER DummyDisasmInsUndoable( mad_disasm_data *dd )
{
    dd = dd;
    return( MS_FAIL );
}

mad_status  MADDisasmInsUndoable( mad_disasm_data *dd )
{
    return( Active->rtns->MIDisasmInsUndoable( dd ) );
}

static mad_disasm_control DIGREGISTER DummyDisasmControl( mad_disasm_data *dd, const mad_registers *mr )
{
    dd = dd;
    mr = mr;
    return( MDC_OPER | MDC_TAKEN );
}

mad_disasm_control      MADDisasmControl( mad_disasm_data *dd, const mad_registers *mr )
{
    return( Active->rtns->MIDisasmControl( dd, mr ) );
}

static mad_status DIGREGISTER DummyDisasmInsNext( mad_disasm_data *dd, const mad_registers *mr, address *a )
{
    dd = dd;
    mr = mr;
    a = a;
    return( MS_FAIL );
}

mad_status              MADDisasmInsNext( mad_disasm_data *dd, const mad_registers *mr, address *a )
{
    return( Active->rtns->MIDisasmInsNext( dd, mr, a ) );
}

static mad_status DIGREGISTER DummyDisasmInspectAddr( const char *start, unsigned len, mad_radix radix, const mad_registers *mr, address *a )
{
    start = start;
    len = len;
    radix = radix;
    mr = mr;
    a = a;
    return( MS_FAIL );
}

mad_status              MADDisasmInspectAddr( const char *start, unsigned len, mad_radix radix, const mad_registers *mr, address *a )
{
    return( Active->rtns->MIDisasmInspectAddr( start, len, radix, mr, a ) );
}

static walk_result DIGREGISTER DummyDisasmMemRefWalk( mad_disasm_data *dd, MI_MEMREF_WALKER *wk, const mad_registers *mr, void *d )
{
    dd = dd;
    wk = wk;
    mr = mr;
    d  = d;
    return( WR_CONTINUE );
}

struct memref_glue {
    MAD_MEMREF_WALKER   *wk;
    void                *d;
};

static walk_result DIGCLIENT MemRefGlue( address a, mad_type_handle th, mad_memref_kind mk, void *d )
{
    struct memref_glue  *gd = d;

    return( gd->wk( a, th, mk, gd->d ) );
}

walk_result             MADDisasmMemRefWalk( mad_disasm_data *dd, MAD_MEMREF_WALKER *wk, const mad_registers *mr, void *d )
{
    struct memref_glue  glue;

    glue.wk = wk;
    glue.d  = d;
    return( Active->rtns->MIDisasmMemRefWalk( dd, MemRefGlue, mr, &glue ) );
}

static const mad_toggle_strings *DIGREGISTER DummyDisasmToggleList( void )
{
    return( EmptyToggleList );
}

const mad_toggle_strings        *MADDisasmToggleList( void )
{
    return( Active->rtns->MIDisasmToggleList() );
}

static unsigned DIGREGISTER DummyDisasmToggle( unsigned on, unsigned off )
{
    on = on;
    off = off;
    return( 0 );
}

unsigned                MADDisasmToggle( unsigned on, unsigned off )
{
    return( Active->rtns->MIDisasmToggle( on, off ) );
}


/*
 *      Instruction Tracing
 */

static unsigned DIGREGISTER DummyTraceSize( void )
{
    return( 1 );
}

unsigned        MADTraceSize( void )
{
    return( Active->rtns->MITraceSize() );
}

static void DIGREGISTER DummyTraceInit( mad_trace_data *td, const mad_registers *mr )
{
    td = td;
    mr = mr;
}

void            MADTraceInit( mad_trace_data *td, const mad_registers *mr )
{
    Active->rtns->MITraceInit( td, mr );
}

static mad_trace_how DIGREGISTER DummyTraceOne( mad_trace_data *td, mad_disasm_data *dd, mad_trace_kind tk, const mad_registers *mr, address *a )
{
    td = td;
    dd = dd;
    tk = tk;
    mr = mr;
    a  = a;
    return( MTRH_STOP );
}

mad_trace_how   MADTraceOne( mad_trace_data *td, mad_disasm_data *dd, mad_trace_kind tk, const mad_registers *mr, address *a )
{
    return( Active->rtns->MITraceOne( td, dd, tk, mr, a ) );
}

static mad_status DIGREGISTER DummyTraceHaveRecursed( address a, const mad_registers *mr )
{
    a = a;
    mr = mr;
    return( MS_FAIL );
}

mad_status      MADTraceHaveRecursed( address a, const mad_registers *mr )
{
    return( Active->rtns->MITraceHaveRecursed( a, mr ) );
}

static mad_status DIGREGISTER DummyTraceSimulate( mad_trace_data *td, mad_disasm_data *dd, const mad_registers *in, mad_registers *out )
{
    td = td;
    dd = dd;
    in = in;
    out=out;
    return( MS_OK );
}

mad_status      MADTraceSimulate( mad_trace_data *td, mad_disasm_data *dd, const mad_registers *in, mad_registers *out )
{
    return( Active->rtns->MITraceSimulate( td, dd, in, out ) );
}

static void DIGREGISTER DummyTraceFini( mad_trace_data *td )
{
    td = td;
}

void            MADTraceFini( mad_trace_data *td )
{
    Active->rtns->MITraceFini( td );
}

static mad_status DIGREGISTER DummyUnexpectedBreak( mad_registers *mr, char *buff, size_t *buff_size_p )
{
    mr = mr;

    if( *buff_size_p > 0 )
        *buff = '\0';
    *buff_size_p = 0;
    return( MS_FAIL );
}

mad_status      MADUnexpectedBreak( mad_registers *mr, char *buff, size_t *buff_size_p )
{
    return( Active->rtns->MIUnexpectedBreak( mr, buff, buff_size_p ) );
}


static mad_imp_routines DummyRtns = {
    MAD_MAJOR,
    MAD_MINOR,
    sizeof( DummyRtns ),

    DummyInit,
    DummyFini,
    DummyStateSize,
    DummyStateInit,
    DummyStateSet,
    DummyStateCopy,

    DummyAddrAdd,
    DummyAddrComp,
    DummyAddrDiff,
    DummyAddrMap,
    DummyAddrFlat,
    DummyAddrInterrupt,

    DummyTypeWalk,
    DummyTypeName,
    DummyTypePreferredRadix,
    DummyTypeForDIPType,
    DummyTypeInfo,
    DummyTypeDefault,
    DummyTypeConvert,
    DummyTypeToString,

    DummyRegistersSize,
    DummyRegistersHost,
    DummyRegistersTarget,
    DummyRegSetWalk,
    DummyRegSetName,
    DummyRegSetLevel,
    DummyRegSetDisplayGrouping,
    DummyRegSetDisplayGetPiece,
    DummyRegSetDisplayModify,
    DummyRegSetDisplayToggleList,
    DummyRegSetDisplayToggle,
    DummyRegModified,
    DummyRegInspectAddr,
    DummyRegWalk,
    DummyRegSpecialGet,
    DummyRegSpecialSet,
    DummyRegSpecialName,
    DummyRegFromContextItem,
    DummyRegUpdateStart,
    DummyRegUpdateEnd,

    DummyCallStackGrowsUp,
    DummyCallTypeList,
    DummyCallBuildFrame,
    DummyCallReturnReg,
    DummyCallParmRegList,
    NULL,

    DummyDisasmDataSize,
    DummyDisasmNameMax,
    DummyDisasm,
    DummyDisasmFormat,
    DummyDisasmInsSize,
    DummyDisasmInsUndoable,
    DummyDisasmControl,
    DummyDisasmInspectAddr,
    DummyDisasmMemRefWalk,
    DummyDisasmToggleList,
    DummyDisasmToggle,

    DummyTraceSize,
    DummyTraceInit,
    DummyTraceOne,
    DummyTraceHaveRecursed,
    DummyTraceSimulate,
    DummyTraceFini,

    DummyUnexpectedBreak,
    DummyDisasmInsNext,
    DummyCallUpStackSize,
    DummyCallUpStackInit,
    DummyCallUpStackLevel,
};

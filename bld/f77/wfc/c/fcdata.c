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
* Description:  DATA statement processor.
*
****************************************************************************/


#include "ftnstd.h"
#include "global.h"
#include "fcgbls.h"
#include "errcod.h"
#include "types.h"
#include "typclass.h"
#include "emitobj.h"
#include "ferror.h"
#include "insert.h"
#include "cgdefs.h"
#include "cg.h"
#include "cgswitch.h"
#include "cgprotos.h"
#include "ftextfun.h"
#include "substr.h"
#include "cnvd2s.h"
#include "rststruc.h"
#include "fcjmptab.h"
#include "fcstack.h"

extern  bool            IntType(PTYPE);

// The following are to support a temporary fix so that constants dumped by
// DATA statements provide the same precision for constants dumped by the code
// generator.  Consider:
//      DOUBLE PRECISION X, Y
//      PARAMETER (Y=1D036)
//      DATA X/Y/
//      PRINT *, X - Y
//      END
// The result should be 0.
#include "fltcnv.h"
#include "wf77defs.h"
#include <string.h>

extern  void            DtInit(segment_id,seg_offset);
extern  void            DtBytes(byte *,int);
extern  void            DtIBytes(byte,int);
extern  segment_id      GetDataSegId(sym_id);
extern  seg_offset      GetDataOffset(sym_id);
extern  void            FCodeSequence(void);
extern  void            DtStartSequence(void);
extern  void            DtFiniSequence(void);

extern  void            (* __FAR FCJmpTab[])( void );
extern  void            (* __FAR DataJmpTab[])( void );
extern  char            *StmtKeywords[];

/* Forward declarations */
static  void    InitStructArr( sym_id fd, act_dim_list *dim );
static  void    StructInit( sym_id fd );
static  void    GetDataConst( void );
static  void    FlushConsts( void );
static  void    StructInit( sym_id fd );
static  void    StructInitItem( sym_id fd );


#define CONST_TYPES     9

static  TYPE    MapType[] = {
    #define pick(id,type,dbgtype,cgtype,inpfun,outfun,typnam) type,
    #include "ptypdefn.h"
    #undef pick
};

#define DT_NO_MORE_CONSTS       0x01
#define DT_SS_NO_HIGH           0x02

static  unsigned_32     DtItemSize;


static  void    I2toI1( ftn_type *old, ftn_type *to ) {
//=====================================================

// Convert constant old INTEGER*2 to INTEGER*1.

    to->intstar1 = old->intstar2;
}


static  void    I4toI1( ftn_type *old, ftn_type *to ) {
//=====================================================

// Convert constant old INTEGER*4 to INTEGER*1.

    to->intstar1 = old->intstar4;
}


static  void    R4toI1( ftn_type *old, ftn_type *to ) {
//===================================================

// Convert constant old REAL*4 to INTEGER*1.

    to->intstar1 = old->single;
}


static  void    R8toI1( ftn_type *old, ftn_type *to ) {
//===================================================

// Convert constant old REAL*8 to INTEGER*1.

    to->intstar1 = old->dble;
}


static  void    R10toI1( ftn_type *old, ftn_type *to ) {
//==========================================================

// Convert constant old REAL*10 to INTEGER*1.

    to->intstar1 = old->extended;
}


static  void    C8toI1( ftn_type *old, ftn_type *to ) {
//====================================================

// Convert constant old COMPLEX*8 to INTSTAR4.

    to->intstar1 = old->scomplex.realpart;
}


static  void    C16toI1( ftn_type *old, ftn_type *to ) {
//======================================================

// Convert constant old COMPLEX*16 to INTEGER*1.

    to->intstar1 = old->dcomplex.realpart;
}


static  void    C20toI1( ftn_type *old, ftn_type *to ) {
//======================================================

// Convert constant old COMPLEX*20 to INTEGER*1.

    to->intstar1 = old->xcomplex.realpart;
}


static  void    I1toI2( ftn_type *old, ftn_type *to ) {
//=====================================================

// Convert constant old INTEGER*1 to INTEGER*2.

    to->intstar2 = old->intstar1;
}


static  void    I4toI2( ftn_type *old, ftn_type *to ) {
//=====================================================

// Convert constant old INTEGER*4 to INTEGER*2.

    to->intstar2 = old->intstar4;
}


static  void    R4toI2( ftn_type *old, ftn_type *to ) {
//===================================================

// Convert constant old REAL*4 to INTEGER*2.

    to->intstar2 = old->single;
}


static  void    R8toI2( ftn_type *old, ftn_type *to ) {
//===================================================

// Convert constant old REAL*8 to INTEGER*2.

    to->intstar2 = old->dble;
}


static  void    R10toI2( ftn_type *old, ftn_type *to ) {
//=========================================================

// Convert constant old REAL*10 to INTEGER*2.

    to->intstar2 = old->extended;
}


static  void    C8toI2( ftn_type *old, ftn_type *to ) {
//====================================================

// Convert constant old COMPLEX*8 to INTEGER*2.

    to->intstar2 = old->scomplex.realpart;
}


static  void    C16toI2( ftn_type *old, ftn_type *to ) {
//======================================================

// Convert constant old COMPLEX*16 to INTEGER*2.

    to->intstar2 = old->dcomplex.realpart;
}


static  void    C20toI2( ftn_type *old, ftn_type *to ) {
//======================================================

// Convert constant old COMPLEX*20 to INTEGER*2.

    to->intstar2 = old->xcomplex.realpart;
}


static  void    I1toI4( ftn_type *old, ftn_type *to ) {
//=====================================================

// Convert constant old INTEGER*1 to INTEGER*4.

    to->intstar4 = old->intstar1;
}


static  void    I2toI4( ftn_type *old, ftn_type *to ) {
//=====================================================

// Convert constant old INTEGER*2 to INTEGER*4.

    to->intstar4 = old->intstar2;
}


static  void    R4toI4( ftn_type *old, ftn_type *to ) {
//===================================================

// Convert constant old REAL*8 to INTEGER*4.

    to->intstar4 = old->single;
}


static  void    R8toI4( ftn_type *old, ftn_type *to ) {
//===================================================

// Convert constant old REAL*8 to INTEGER*4.

    to->intstar4 = old->dble;
}


static  void    R10toI4( ftn_type *old, ftn_type *to ) {
//=========================================================

// Convert constant old REAL*10 to INTEGER*4.

    to->intstar4 = old->extended;
}


static  void    C8toI4( ftn_type *old, ftn_type *to ) {
//====================================================

// Convert constant old COMPLEX*8 to INTEGER*4.

    to->intstar4 = old->scomplex.realpart;
}


static  void    C16toI4( ftn_type *old, ftn_type *to ) {
//======================================================

// Convert constant old COMPLEX*16 to INTEGER*4.

    to->intstar4 = old->dcomplex.realpart;
}


static  void    C20toI4( ftn_type *old, ftn_type *to ) {
//======================================================

// Convert constant old COMPLEX*20 to INTEGER*4.

    to->intstar4 = old->xcomplex.realpart;
}


static  void    I1toR4( ftn_type *old, ftn_type *to ) {
//===================================================

// Convert constant old INTEGER*1 to REAL*4.

    to->single = old->intstar1;
}


static  void    I2toR4( ftn_type *old, ftn_type *to ) {
//===================================================

// Convert constant old INTEGER*2 to REAL*4.

    to->single = old->intstar2;
}


static  void    I4toR4( ftn_type *old, ftn_type *to ) {
//===================================================

// Convert constant old INTEGER*4 to REAL*4.

    to->single = old->intstar4;
}


static  void    R8toR4( ftn_type *old, ftn_type *to ) {
//=================================================

// Convert constant old REAL*8 to REAL*4.

    to->single = old->dble;
}


static  void    R10toR4( ftn_type *old, ftn_type *to ) {
//=======================================================

// Convert constant old REAL*10 to REAL*4.

    to->single = old->extended;
}


static  void    C8toR4( ftn_type *old, ftn_type *to ) {
//==================================================

// Convert constant old COMPLEX*8 to REAL*4.

    to->single = old->scomplex.realpart;
}


static  void    C16toR4( ftn_type *old, ftn_type *to ) {
//====================================================

// Convert constant old COMPLEX*16 to REAL*4.

    to->single = old->dcomplex.realpart;
}


static  void    C20toR4( ftn_type *old, ftn_type *to ) {
//====================================================

// Convert constant old COMPLEX*20 to REAL*4.

    to->single = old->xcomplex.realpart;
}


static  void    I1toR8( ftn_type *old, ftn_type *to ) {
//===================================================

// Convert constant old INTEGER*1 to REAL*8.

    to->dble = old->intstar1;
}


static  void    I2toR8( ftn_type *old, ftn_type *to ) {
//===================================================

// Convert constant old INTEGER*2 to REAL*8.

    to->dble = old->intstar2;
}


static  void    I4toR8( ftn_type *old, ftn_type *to ) {
//===================================================

// Convert constant old INTEGER*4 to REAL*8.

    to->dble = old->intstar4;
}


static  void    R4toR8( ftn_type *old, ftn_type *to ) {
//=================================================

// Convert constant old REAL*4 to REAL*8.

    to->dble = old->single;
}


static  void    R10toR8( ftn_type *old, ftn_type *to ) {
//=======================================================

// Convert constant old REAL*10 to REAL*8.

    to->dble = old->extended;
}


static  void    C8toR8( ftn_type *old, ftn_type *to ) {
//==================================================

// Convert constant old COMPLEX*8 to REAL*8.

    to->dble = old->scomplex.realpart;
}


static  void    C16toR8( ftn_type *old, ftn_type *to ) {
//====================================================

// Convert constant old COMPLEX*16 to REAL*8.

    to->dble = old->dcomplex.realpart;
}


static  void    C20toR8( ftn_type *old, ftn_type *to ) {
//====================================================

// Convert constant old COMPLEX*20 to REAL*8.

    to->dble = old->xcomplex.realpart;
}


static  void    I1toR10( ftn_type *old, ftn_type *to ) {
//=========================================================

// Convert constant old INTEGER*1 to REAL*10.

    to->extended = old->intstar1;
}


static  void    I2toR10( ftn_type *old, ftn_type *to ) {
//=========================================================

// Convert constant old INTEGER*2 to REAL*10.

    to->extended = old->intstar2;
}


static  void    I4toR10( ftn_type *old, ftn_type *to ) {
//=========================================================

// Convert constant old INTEGER*4 to REAL*10.

    to->extended = old->intstar4;
}


static  void    R4toR10( ftn_type *old, ftn_type *to ) {
//=======================================================

// Convert constant old REAL*4 to REAL*10.

    to->extended = old->single;
}


static  void    R8toR10( ftn_type *old, ftn_type *to ) {
//=======================================================

// Convert constant old REAL*8 to REAL*10.

    to->extended = old->dble;
}


static  void    C8toR10( ftn_type *old, ftn_type *to ) {
//========================================================

// Convert constant old COMPLEX*8 to REAL*10.

    to->extended = old->scomplex.realpart;
}


static  void    C16toR10( ftn_type *old, ftn_type *to ) {
//==========================================================

// Convert constant old COMPLEX*16 to REAL*10.

    to->extended = old->dcomplex.realpart;
}


static  void    C20toR10( ftn_type *old, ftn_type *to ) {
//==========================================================

// Convert constant old COMPLEX*20 to REAL*10.

    to->extended = old->xcomplex.realpart;
}


static  void    I1toC8( ftn_type *old, ftn_type *to ) {
//====================================================

// Convert constant old INTEGER*1 to COMPLEX*8.

    to->scomplex.realpart = old->intstar1;
    to->scomplex.imagpart = 0;
}


static  void    I2toC8( ftn_type *old, ftn_type *to ) {
//====================================================

// Convert constant old INTEGER*2 to COMPLEX*8.

    to->scomplex.realpart = old->intstar2;
    to->scomplex.imagpart = 0;
}


static  void    I4toC8( ftn_type *old, ftn_type *to ) {
//====================================================

// Convert constant old INTEGER*4 to COMPLEX*8.

    to->scomplex.realpart = old->intstar4;
    to->scomplex.imagpart = 0;
}


static  void    R4toC8( ftn_type *old, ftn_type *to ) {
//==================================================

// Convert constant old REAL*4 to COMPLEX*8.

    to->scomplex.realpart = old->single;
    to->scomplex.imagpart = 0;
}


static  void    R8toC8( ftn_type *old, ftn_type *to ) {
//==================================================

// Convert constant old REAL*8 to COMPLEX*8.

    to->scomplex.realpart = old->dble;
    to->scomplex.imagpart = 0;
}


static  void    R10toC8( ftn_type *old, ftn_type *to ) {
//========================================================

// Convert constant old REAL*10 to COMPLEX*8.

    to->scomplex.realpart = old->extended;
    to->scomplex.imagpart = 0;
}


static  void    C16toC8( ftn_type *old, ftn_type *to ) {
//=====================================================

// Convert constant old COMPLEX*16 to COMPLEX*8.

    to->scomplex.realpart = old->dcomplex.realpart;
    to->scomplex.imagpart = old->dcomplex.imagpart;
}


static  void    C20toC8( ftn_type *old, ftn_type *to ) {
//=====================================================

// Convert constant old COMPLEX*20 to COMPLEX*8.

    to->scomplex.realpart = old->xcomplex.realpart;
    to->scomplex.imagpart = old->xcomplex.imagpart;
}


static  void    I1toC16( ftn_type *old, ftn_type *to ) {
//======================================================

// Convert constant old INTEGER*1 to COMPLEX*16.

    to->dcomplex.realpart = old->intstar1;
    to->dcomplex.imagpart = 0;
}


static  void    I2toC16( ftn_type *old, ftn_type *to ) {
//======================================================

// Convert constant old INTEGER*2 to COMPLEX*16.

    to->dcomplex.realpart = old->intstar2;
    to->dcomplex.imagpart = 0;
}


static  void    I4toC16( ftn_type *old, ftn_type *to ) {
//======================================================

// Convert constant old INTEGER*4 to COMPLEX*16.

    to->dcomplex.realpart = old->intstar4;
    to->dcomplex.imagpart = 0;
}


static  void    R4toC16( ftn_type *old, ftn_type *to ) {
//====================================================

// Convert constant old REAL*4 to COMPLEX*16.

    to->dcomplex.realpart = old->single;
    to->dcomplex.imagpart = 0;
}


static  void    R8toC16( ftn_type *old, ftn_type *to ) {
//====================================================

// Convert constant old REAL*8 to COMPLEX*16.

    to->dcomplex.realpart = old->dble;
    to->dcomplex.imagpart = 0;
}


static  void    R10toC16( ftn_type *old, ftn_type *to ) {
//===========================================================

// Convert constant old REAL*10 to COMPLEX*16.

    to->dcomplex.realpart = old->extended;
    to->dcomplex.imagpart = 0;
}


static  void    C8toC16( ftn_type *old, ftn_type *to ) {
//=====================================================

// Convert constant old COMPLEX*8 to COMPLEX*16.

    to->dcomplex.realpart = old->scomplex.realpart;
    to->dcomplex.imagpart = old->scomplex.imagpart;
}

static  void    C20toC16( ftn_type *old, ftn_type *to ) {
//=======================================================

// Convert constant old COMPLEX*20 to COMPLEX*16.

    to->dcomplex.realpart = old->xcomplex.realpart;
    to->dcomplex.imagpart = old->xcomplex.imagpart;
}


static  void    I1toC20( ftn_type *old, ftn_type *to ) {
//======================================================

// Convert constant old INTEGER*1 to COMPLEX*20.

    to->xcomplex.realpart = old->intstar1;
    to->xcomplex.imagpart = 0;
}


static  void    I2toC20( ftn_type *old, ftn_type *to ) {
//======================================================

// Convert constant old INTEGER*2 to COMPLEX*20.

    to->xcomplex.realpart = old->intstar2;
    to->xcomplex.imagpart = 0;
}


static  void    I4toC20( ftn_type *old, ftn_type *to ) {
//======================================================

// Convert constant old INTEGER*4 to COMPLEX*20.

    to->xcomplex.realpart = old->intstar4;
    to->xcomplex.imagpart = 0;
}


static  void    R4toC20( ftn_type *old, ftn_type *to ) {
//====================================================

// Convert constant old REAL*4 to COMPLEX*20.

    to->xcomplex.realpart = old->single;
    to->xcomplex.imagpart = 0;
}


static  void    R8toC20( ftn_type *old, ftn_type *to ) {
//====================================================

// Convert constant old REAL*8 to COMPLEX*20.

    to->xcomplex.realpart = old->dble;
    to->xcomplex.imagpart = 0;
}


static  void    R10toC20( ftn_type *old, ftn_type *to ) {
//===========================================================

// Convert constant old REAL*10 to COMPLEX*20.

    to->xcomplex.realpart = old->extended;
    to->xcomplex.imagpart = 0;
}


static  void    C8toC20( ftn_type *old, ftn_type *to ) {
//=====================================================

// Convert constant old COMPLEX*8 to COMPLEX*20.

    to->xcomplex.realpart = old->scomplex.realpart;
    to->xcomplex.imagpart = old->scomplex.imagpart;
}

static  void    C16toC20( ftn_type *old, ftn_type *to ) {
//=======================================================

// Convert constant old COMPLEX*16 to COMPLEX*20.

    to->xcomplex.realpart = old->dcomplex.realpart;
    to->xcomplex.imagpart = old->dcomplex.imagpart;
}


static  void            (*DataCnvTab[])(ftn_type *, ftn_type *) = {
    NULL,     &I2toI1,  &I4toI1,  &R4toI1,  &R8toI1,  &R10toI1,  &C8toI1,  &C16toI1,  &C20toI1,
    &I1toI2,  NULL,     &I4toI2,  &R4toI2,  &R8toI2,  &R10toI2,  &C8toI2,  &C16toI2,  &C20toI2,
    &I1toI4,  &I2toI4,  NULL,     &R4toI4,  &R8toI4,  &R10toI4,  &C8toI4,  &C16toI4,  &C20toI4,
    &I1toR4,  &I2toR4,  &I4toR4,  NULL,     &R8toR4,  &R10toR4,  &C8toR4,  &C16toR4,  &C20toR4,
    &I1toR8,  &I2toR8,  &I4toR8,  &R4toR8,  NULL,     &R10toR8,  &C8toR8,  &C16toR8,  &C20toR8,
    &I1toR10, &I2toR10, &I4toR10, &R4toR10, &R8toR10, NULL,      &C8toR10, &C16toR10, &C20toR10,
    &I1toC8,  &I2toC8,  &I4toC8,  &R4toC8,  &R8toC8,  &R10toC8,  NULL,     &C16toC8,  &C20toC8,
    &I1toC16, &I2toC16, &I4toC16, &R4toC16, &R8toC16, &R10toC16, &C8toC16, NULL,      &C20toC16,
    &I1toC20, &I2toC20, &I4toC20, &R4toC20, &R8toC20, &R10toC20, &C8toC20, &C16toC20, NULL
};


static  bool    Numeric( PTYPE typ ) {
//==================================

// Is given type numeric?

    return( ( typ >= PT_INT_1 ) && ( typ <= PT_CPLX_32 ) );
}


bool    IntType( PTYPE typ ) {
//==========================

// Is given type integer?

    return( ( typ >= PT_INT_1 ) && ( typ <= PT_INT_4 ) );
}

static  void    DoDataInit( PTYPE var_type ) {
//==========================================

// Do data initialization.

    int         const_size;
    int         var_size;
    int         size;
    byte        *const_ptr;
    segment_id  seg;
    seg_offset  offset;
    byte        const_buff[sizeof(ftn_type)];

    if( ( DtConstType == PT_CHAR ) || ( DtConstType == PT_NOTYPE ) ) {
        const_size = DtConst->u.lt.length;
        const_ptr = &DtConst->u.lt.value;
    } else {
        const_size = DtConst->u.cn.size;
        const_ptr = (byte *)(&DtConst->u.cn.value);
    }
    var_size = DtItemSize;
    seg = GetDataSegId( InitVar );
    offset = GetDataOffset( InitVar );
    DtInit( seg, offset );
    if( DtConstType == PT_CHAR ) {
        if( const_size >= var_size ) {
            DtBytes( const_ptr, var_size );
        } else {
            DtBytes( const_ptr, const_size );
            DtIBytes( ' ', var_size - const_size );
        }
    } else if( ( var_type == PT_CHAR ) && IntType( DtConstType ) ) {
        DtBytes( const_ptr, sizeof( char ) );
        if( var_size > sizeof( char ) ) {
            DtIBytes( ' ', var_size - sizeof( char ) );
        }
    } else if( DtConstType == PT_NOTYPE ) {
        if( var_type != PT_CHAR ) {
            size = var_size;
            while( size > const_size ) {
                size--;
                const_buff[ size ] = 0;
            }
            while( size > 0 ) {
                size--;
                const_buff[ size ] = *const_ptr;
                const_ptr++;
            }
            const_ptr = const_buff;
            const_size = var_size;
        }
        if( const_size < var_size ) {
            DtIBytes( 0, var_size - const_size );
            var_size = const_size;
        } else {
            const_ptr += const_size - var_size;
        }
        DtBytes( const_ptr, var_size );
    } else if( DtConstType <= PT_LOG_4 ) {
        DtBytes( const_ptr, var_size );
    } else {        // numeric to numeric
        if( DtConstType != var_type ) {
            DataCnvTab[ ( var_type - PT_INT_1 ) * CONST_TYPES +
                        ( DtConstType - PT_INT_1 ) ]( (ftn_type *)const_ptr, (ftn_type *)&const_buff );
            const_ptr = const_buff;
        }

// Temporary fix for identical precision between front end and code generator.
        {
            char        fmt_buff[CONVERSION_BUFFER+1];
            float_handle cf;

            if( (var_type == PT_REAL_4) || (var_type == PT_CPLX_8) ) {
                CnvS2S( (single *)const_ptr, fmt_buff );
                cf = BFCnvSF( fmt_buff );
                BFCnvTarget( cf, const_buff, BETypeLength( TY_SINGLE ) );
                BFFree( cf );
            } else if( (var_type == PT_REAL_8) || (var_type == PT_CPLX_16) ) {
                CnvD2S( (double *)const_ptr, fmt_buff );
                cf = BFCnvSF( fmt_buff );
                BFCnvTarget( cf, const_buff, BETypeLength( TY_DOUBLE ) );
                BFFree( cf );
            } else if( (var_type == PT_REAL_16) || (var_type == PT_CPLX_32) ) {
                CnvX2S( (extended *)const_ptr, fmt_buff );
                cf = BFCnvSF( fmt_buff );
                BFCnvTarget( cf, const_buff, BETypeLength( TY_LONGDOUBLE ) );
                BFFree( cf );
            }
            if( var_type == PT_CPLX_8 ) {
                CnvS2S( (single *)(const_ptr + sizeof( single )), fmt_buff );
                cf = BFCnvSF( fmt_buff );
                BFCnvTarget( cf, const_buff + sizeof( single ), BETypeLength( TY_SINGLE ) );
                BFFree( cf );
            } else if( var_type == PT_CPLX_16 ) {
                CnvD2S( (double *)(const_ptr + sizeof( double )), fmt_buff );
                cf = BFCnvSF( fmt_buff );
                BFCnvTarget( cf, const_buff + sizeof( double ), BETypeLength( TY_DOUBLE ) );
                BFFree( cf );
            } else if( var_type == PT_CPLX_32 ) {
                CnvX2S( (extended *)(const_ptr + sizeof( extended )), fmt_buff );
                cf = BFCnvSF( fmt_buff );
                BFCnvTarget( cf, const_buff + sizeof( extended ), BETypeLength( TY_LONGDOUBLE ) );
                BFFree( cf );
            }
            if( (var_type >= PT_REAL_4) && (var_type <= PT_CPLX_32) ) {
                const_ptr = const_buff;
            }
        }
// End of temporary fix.

        DtBytes( const_ptr, var_size );
    }
    DtItemSize = 0;
}


static  void    AsnVal( PTYPE var_type ) {
//======================================

// Do data initialization.

    bool        issue_err;

    issue_err = ( DtFlags & DT_NO_MORE_CONSTS ) == 0;
    GetDataConst();
    if( DtFlags & DT_NO_MORE_CONSTS ) {
        if( issue_err ) {
            Error( DA_NOT_ENOUGH );
        }
    } else if( ( DtConstType == PT_NOTYPE ) ||
        ( ( var_type <= PT_LOG_4 ) && ( DtConstType <= PT_LOG_4 ) ) ||
        ( DtConstType == PT_CHAR ) ||
        ( ( var_type == PT_CHAR ) && IntType( DtConstType ) ) ||
        ( Numeric( var_type ) && Numeric( DtConstType ) ) ) {
        DoDataInit( var_type );
    } else {
        TypeNameTypeErr( DA_TYPE_MISMATCH, MapType[ var_type ], InitVar, MapType[ DtConstType ] );
    }
}


void    FCStartDataStmt( void ) {
//=========================

// Start DATA statement processing.

    FCTablePtr = DataJmpTab;
    DtConstList = FCodeTell( GetU16() - sizeof( unsigned_16 ) );
    DtRepCount = 0;
    DtFlags = 0;
    DtItemSize = 0;
    DtStartSequence();
}


void    DtEndDataStmt( void ) {
//=======================

// End DATA statement processing.

    DtFiniSequence();
    FCTablePtr = FCJmpTab;
    FCodeSeek( DtConstList );
}


void    DtEndVarSet( void ) {
//=====================

// End of DATA variable set.

    GetDataConst();
    if( ( DtFlags & DT_NO_MORE_CONSTS ) == 0 ) {
        Error( DA_TOO_MUCH );
        FlushConsts();
    }
    DtFlags = 0;
    DtRepCount = 0;
}


void    DtDataDoLoop( void ) {
//======================

// Process implied-DO for DATA statement.

    intstar4    e2;
    intstar4    e3;
    intstar4    iter_count;
    sym_id      do_var;
    obj_ptr     curr_fc;
    intstar4    e1;

    e3 = DXPop();
    e2 = DXPop();
    e1 = DXPop();
    do_var = GetPtr();
    do_var->u.ns.si.ms.u.value = &e1;
    iter_count = ( e2 - e1 + e3 ) / e3;
    curr_fc = FCodeTell( 0 );
    while( iter_count > 0 ) {
        FCodeSeek( curr_fc );
        FCodeSequence();
        *do_var->u.ns.si.ms.u.value += e3;
        iter_count--;
    }
}


void    DtInpLOG1( void ) {
//===================

// Data initialize a LOGICAL*1 item.

    AsnVal( PT_LOG_1 );
}


void    DtInpLOG4( void ) {
//===================

// Data initialize a LOGICAL*4 item.

    AsnVal( PT_LOG_4 );
}


void    DtInpINT1( void ) {
//===================

// Data initialize a INTEGER*1 item.

    AsnVal( PT_INT_1 );
}


void    DtInpINT2( void ) {
//===================

// Data initialize a INTEGER*2 item.

    AsnVal( PT_INT_2 );
}


void    DtInpINT4( void ) {
//===================

// Data initialize a INTEGER*4 item.

    AsnVal( PT_INT_4 );
}


void    DtInpREAL( void ) {
//===================

// Data initialize a REAL*4 item.

    AsnVal( PT_REAL_4 );
}


void    DtInpDBLE( void ) {
//===================

// Data initialize a REAL*8 item.

    AsnVal( PT_REAL_8 );
}


void    DtInpXTND( void ) {
//===================

// Data initialize a REAL*10 item.

    AsnVal( PT_REAL_16 );
}


void    DtInpCPLX( void ) {
//===================

// Data initialize a COMPLEX*8 item.

    AsnVal( PT_CPLX_8 );
}


void    DtInpDBCX( void ) {
//===================

// Data initialize a COMPLEX*16 item.

    AsnVal( PT_CPLX_16 );
}


void    DtInpXTCX( void ) {
//===================

// Data initialize a COMPLEX*20 item.

    AsnVal( PT_CPLX_32 );
}


void    DtInpCHAR( void ) {
//===================

// Data initialize a CHARACTER item.

    AsnVal( PT_CHAR );
}


static  intstar4        IntegerValue( sym_id sym ) {
//==================================================

    if( sym->u.cn.size == sizeof( intstar1 ) ) return( sym->u.cn.value.intstar1 );
    if( sym->u.cn.size == sizeof( intstar2 ) ) return( sym->u.cn.value.intstar2 );
    return( sym->u.cn.value.intstar4 ); // sym->u.cn.size == sizeof( intstar4 )
}


void    DtPushConst( void ) {
//=====================

// Push constant.

    DXPush( IntegerValue( GetPtr() ) );
}


void    DtPushSCBLen( void ) {
//======================

// Indicate that the high bound of a substring operation was not specified.

    DtFlags |= DT_SS_NO_HIGH;
}


void    DtPush( void ) {
//================

// Push value of symbol.

    sym_id      sym;

    sym = GetPtr();
    if( _MgcIsMagic( sym ) ) { // must be the value of an implied-DO-variable
        DXPush( *sym->u.ns.si.ms.u.value );
    } else { // must be variable to initialize
        InitVar = sym;
        DtOffset = 0;
        if( sym->u.ns.u1.s.typ != FT_STRUCTURE ) {
            DtItemSize = InitVar->u.ns.xt.size;
        }
    }
}


void    DtAdd( void ) {
//===============

// Add constants.

    intstar4    op1;
    intstar4    op2;

    op1 = DXPop();
    op2 = DXPop();
    DXPush( op1 + op2 );
    GetU16();   // skip typing information
}


void    DtMul( void ) {
//===============

// Multiply constants.

    intstar4    op1;
    intstar4    op2;

    op1 = DXPop();
    op2 = DXPop();
    DXPush( op1 * op2 );
    GetU16();   // skip typing information
}


void    DtUnaryMul( void ) {
//====================

// Multiply constants.

    intstar4    op;

    op = DXPop();
    DXPush( op * op );
    GetU16();   // skip typing information
}


void    DtSub( void ) {
//===============

// Subtract constants.

    intstar4    op1;
    intstar4    op2;

    op1 = DXPop();
    op2 = DXPop();
    DXPush( op1 - op2 );
    GetU16();   // skip typing information
}


void    DtDiv( void ) {
//===============

// Divide constants.

    intstar4    op1;
    intstar4    op2;

    op1 = DXPop();
    op2 = DXPop();
    DXPush( op1 / op2 );
    GetU16();   // skip typing information
}


void    DtExp( void ) {
//===============

// Exponentiate constants.

    intstar4    op2;
    intstar4    op1;
    intstar4    result;

    op1 = DXPop();
    op2 = DXPop();
    if( op1 == 0 ) {
        if( op2 <= 0 ) {
            Error( EX_Z_2_NOT_POS );
        }
        DXPush( 0 );
    } else if( op2 < 0 ) {
        DXPush( 0 );
    } else {
        result = 1;
        while( op2 != 0 ) {
            result *= op1;
            --op2;
        }
        DXPush( result );
    }
    GetU16();   // skip typing information
}


void    DtUMinus( void ) {
//==================

// Negate constant.

    DXPush( -DXPop() );
    GetU16();   // skip typing information
}


void    DtFlip( void ) {
//================

// Flip values on stack.

    intstar4    op1;
    intstar4    op2;

    op1 = DXPop();
    op2 = DXPop();
    DXPush( op1 );
    DXPush( op2 );
}


static  bool    Subscript( act_dim_list *dim, intstar4 *offset ) {
//================================================================

// Data initialize an array element.

    int         dims_no;
    intstar4    subscrs[MAX_DIM];

    dims_no = 0;
    while( dims_no < _DimCount( dim->dim_flags ) ) {
        subscrs[ dims_no ] = DXPop();
        GetU16();       // skip typing information
        dims_no++;
    }
    return( DoSubscript( dim, subscrs, offset ) );
}


void    DtSubscript( void ) {
//=====================

// Data initialize an array element.

    intstar4    offset;

    InitVar = GetPtr();
    if( !Subscript( InitVar->u.ns.si.va.u.dim_ext, &offset ) ) {
        NameStmtErr( EV_SSCR_INVALID, InitVar, PR_DATA );
    }
    DtOffset = offset * _SymSize( InitVar );
    if( InitVar->u.ns.u1.s.typ != FT_STRUCTURE ) {
        DtItemSize = InitVar->u.ns.xt.size;
    }
}


void    DtSubstring( void ) {
//=====================

// Data initialize a character substring.

    intstar4    first;
    intstar4    last;
    sym_id      cv;

    cv = GetPtr();
    GetU16();   // skip typing information
    first = DXPop();
    if( cv != NULL ) {
        if( DtFlags & DT_SS_NO_HIGH ) {
            last = cv->u.ns.xt.size;
            DtFlags &= ~DT_SS_NO_HIGH;
        } else {
            last = DXPop();
        }
    } else {
        last = first + GetInt() - 1;
    }
    if( !DoSubstring( first, last, InitVar->u.ns.xt.size ) ) {
        NameStmtErr( EV_SSTR_INVALID, InitVar, PR_DATA );
    }
    DtOffset += first - 1;
    DtItemSize = last - first + 1;
}


void    DtFieldSubscript( void ) {
//==========================

// Data initialize an array element within a structure.

    sym_id      fd;
    intstar4    offset;
    intstar4    base;
    char        name[MAX_SYMLEN+1];

    base = DXPop();
    fd = GetPtr();
    if( Subscript( fd->u.fd.dim_ext, &offset ) ) {
        if( fd->u.fd.typ == FT_STRUCTURE ) {
            offset *= fd->u.fd.xt.record->size;
        } else {
            offset *= fd->u.fd.xt.size;
        }
        DXPush( base + offset );
    } else {
        STFieldName( fd, name );
        Error( EV_SSCR_INVALID, name, StmtKeywords[ PR_DATA ] );
    }
}


void    DtFieldSubstring( void ) {
//==========================

// Data initialize a substring character item within a structure.

    sym_id      fd;
    intstar4    base;
    intstar4    first;
    intstar4    last;
    char        name[MAX_SYMLEN+1];

    base = DXPop();
    first = DXPop();
    fd = GetPtr();
    last = GetInt();
    if( last != 0 ) {
        last += first - 1;
    } else {
        last = DXPop();
    }
    GetU16(); // skip typing information
    if( DoSubstring( first, last, fd->u.fd.xt.size ) ) {
        DXPush( base + first - 1 );
        DtItemSize = last - first + 1;
    } else {
        STFieldName( fd, name );
        Error( EV_SSTR_INVALID, name, StmtKeywords[ PR_DATA ] );
    }
}


static  void    InitArr( act_dim_list *dim, TYPE typ, uint size ) {
//=================================================================

// Data initialize an array.

    unsigned_32 num_elts;

    num_elts = dim->num_elts;
    while( num_elts != 0 ) {
        DtItemSize = size; // AsnVal() sets DtItemSize to 0 when done
        AsnVal( ParmType( typ, size ) );
        DtOffset += size;
        num_elts--;
    }
}


void    DtInpArray( void ) {
//====================

// Data initialize an array.

    sym_id      fd;
    sym_id      sym;

    sym = GetPtr();
    fd = GetPtr();
    if( fd == NULL ) {
        InitVar = sym;
        DtOffset = 0;
        InitArr( sym->u.ns.si.va.u.dim_ext, sym->u.ns.u1.s.typ, _SymSize( sym ) );
    } else { // array field within structure
        InitArr( fd->u.fd.dim_ext, fd->u.fd.typ, fd->u.fd.xt.size );
    }
}


void    DtInpStructArray( void ) {
//==========================

// Data initialize an array of structures.

    InitVar = GetPtr();
    DtOffset = 0;
    InitStructArr( InitVar->u.ns.xt.record->fl.sym_fields, InitVar->u.ns.si.va.u.dim_ext );
}


static  void    InitStructArr( sym_id fd, act_dim_list *dim ) {
//=============================================================

// Data initialize an array.

    unsigned_32 num_elts;

    num_elts = dim->num_elts;
    while( num_elts != 0 ) {
        StructInit( fd );
        num_elts--;
    }
}


void    DtFieldOp( void ) {
//===================

// Field selection.

    sym_id      fd;

    InitVar = GetPtr();
    fd = GetPtr();
    if( fd->u.fd.typ == FT_CHAR ) {
        // DtItemSize will be set if a field has been substrung
        if( DtItemSize == 0 ) {
            DtItemSize = fd->u.fd.xt.size;
        }
    } else {
        DtItemSize = fd->u.fd.xt.size;
    }
    DtOffset += DXPop();
}


void    DtInpStruct( void ) {
//=====================

// Initialize a struct.

    StructInit( ((sym_id)GetPtr())->u.sd.fl.sym_fields );
}


static  void    StructInit( sym_id fd ) {
//=======================================

    while( fd != NULL ) {
        if( fd->u.fd.typ == FT_STRUCTURE ) {
            StructInit( fd->u.fd.xt.record->fl.sym_fields );
        } else {
            StructInitItem( fd );
            if( fd->u.fd.dim_ext == NULL ) {
                DtOffset += fd->u.fd.xt.size;
            }
        }
        fd = fd->u.fd.link;
    }
}


static  void    StructInitItem( sym_id fd ) {
//===========================================

// Initialize a structure field.

    DtItemSize = fd->u.fd.xt.size;
    if( fd->u.fd.dim_ext == NULL ) {
        AsnVal( ParmType( fd->u.fd.typ, DtItemSize ) );
    } else {
        if( fd->u.fd.typ == FT_STRUCTURE ) {
            InitStructArr( fd, fd->u.fd.dim_ext );
        } else {
            InitArr( fd->u.fd.dim_ext, fd->u.fd.typ, DtItemSize );
        }
    }
}


static  void    GetDataConst( void ) {
//==============================

// Get a constant for data initialization.

    obj_ptr     curr_fc;
    sym_id      rep;

    if( ( DtFlags & DT_NO_MORE_CONSTS ) == 0 ) {
        if( DtRepCount == 0 ) {
            curr_fc = FCodeSeek( DtConstList );
            rep = GetPtr();
            if( rep != NULL ) {
                DtRepCount = IntegerValue( rep );
                DtConstType = GetU16();
                DtConst = GetPtr();
            } else {
                DtFlags |= DT_NO_MORE_CONSTS;
            }
            DtConstList = FCodeSeek( curr_fc );
        }
        DtRepCount--;
    }
}


static  void    FlushConsts( void ) {
//=============================

// Flush constant list.

    while( ( DtFlags & DT_NO_MORE_CONSTS ) == 0 ) {
        GetDataConst();
    }
}

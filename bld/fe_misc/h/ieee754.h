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


#ifndef _IEEE754_H_
#define _IEEE754_H_

#ifdef __cplusplus
extern "C" {
#endif

struct cfloat;

void IEEEInit                   // INITIALIZE ROUTINE
    ( void )
;
void IEEEFini                   // FINALIZE ROUTINE
    ( void )
;

//---------------------- mathematical functions ---------------------

typedef enum {
    CF_32
  , CF_64
} CF_PRECISION;


cfloat * IEEECFDiv              // IEEE DIVIDE
    ( cfloat *left
    , cfloat *right
    , CF_PRECISION prec )       // - precision of operation
;
cfloat * IEEECFMul              // IEEE MULTIPLY
    ( cfloat *left
    , cfloat *right
    , CF_PRECISION prec )       // - precision of operation
;
cfloat * IEEECFMod              // IEEE MODULUS
    ( cfloat *left
    , cfloat *right
    , CF_PRECISION prec )       // - precision of operation
;
cfloat * IEEECFNegate           // IEEE NEGATE
    ( cfloat *f
    , CF_PRECISION prec )       // - precision of operation
;
cfloat * IEEECFAdd              // IEEE ADDITION
    ( cfloat *left
    , cfloat *right
    , CF_PRECISION prec )       // - precision of operation
;
cfloat * IEEECFToCF             // IEEE inf->finite->inf precision
    ( cfloat *f                 // - cfloat to stuff into 32 or 64 bits
    , CF_PRECISION prec )       // - precision of operation
;
cfloat * IEEECFToCFNotSpecial   // IEEE inf->finite->inf precision
    ( cfloat *f                 // - cfloat to stuff into 32 or 64 bits
                                //   must already be tested as not being
                                //   PosInf, NegInf or NaN
    , CF_PRECISION prec )       // - precision of operation
;
//---------------------- comparison functions ---------------------

typedef enum {
    CF_CMP_NONE
  , CF_CMP_LT
  , CF_CMP_LE
  , CF_CMP_GT
  , CF_CMP_GE
  , CF_CMP_EQ
  , CF_CMP_NE
} CF_COMPARE;

cf_bool IEEECFCmp
    ( cfloat *left
    , cfloat *right
    , CF_COMPARE cmp )
;
void IEEECFFree                 // FREE CFLOAT (UNLESS SPECIAL VALUE)
    ( cfloat * cf )
;

cfloat *IEEECheckRange          // CHECK RANGE OF CFLOAT
    ( cfloat *cf                // - cfloat to check
    , CF_PRECISION prec )       // - precision
;
//---------------------- special values ---------------------

cfloat *IEEEGetPosInf           // GET POSITIVE INFINITY
    (void)
;
cfloat *IEEEGetNegInf           // GET NEGATIVE INFINITY
    (void)
;
cfloat *IEEEGetNaN              // GET NAN
    (void)
;
cf_bool IEEEIsSpecialValue      // CF_TRUE IF F IS POSINF, NEGINF OR NAN
    (cfloat *f)
;

#ifdef __cplusplus
};
#endif

#endif // _IEEE754_H_

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


#include "watcom.h"

#ifdef __cplusplus
extern "C" {
#endif

#define  CF_MAX_PREC    32
#define  CF_ERR_EXP     INT_MAX

/* aligned */
#define STRUCT_cfloat( x ) \
{ \
        int             exp; \
        int             len; \
        int             alloc; \
        signed_8        sign; \
        char            mant[ x ]; \
}

typedef struct cfloat STRUCT_cfloat( 1 ) cfloat;

typedef struct cf_callbacks {
    void * (*alloc)( unsigned );
    void   (*free)( void * );
} cf_callbacks;

typedef enum {
    CF_FALSE = 0,
    CF_TRUE
} cf_bool;

extern  void            CFCnvTarget( cfloat *f, char *buffer, int size );
extern  signed_64       CFGetDec64( char *bstart );
extern  signed_32       CFGetDec32( char *bstart );
extern  char    *       CFCnvFS( cfloat *f, char *buffer, int maxlen );
extern  cfloat  *       CFCnvSF( char *bstart, char *bend );
extern  cfloat  *       CFCopy( cfloat *old );
extern  cfloat  *       CFTrunc( cfloat *f );
extern  cfloat  *       CFRound( cfloat *f );
extern  cfloat  *       CFCnvI32F( signed_32 data );
extern  cfloat  *       CFCnvU32F( unsigned_32 data );
extern  cfloat  *       CFCnvU64F( unsigned_32 low, unsigned_32 high );
extern  cfloat  *       CFCnvI64F( unsigned_32 lo, unsigned_32 hi );
extern  cfloat  *       CFCnvIF( int data );
extern  cfloat  *       CFCnvUF( uint data );

extern  cf_bool         CFIsI8( cfloat *f );
extern  cf_bool         CFIsI16( cfloat *f );
extern  cf_bool         CFIsI32( cfloat *f );
extern  cf_bool         CFIsI64( cfloat *f );
extern  cf_bool         CFIsU8( cfloat *f );
extern  cf_bool         CFIsU16( cfloat *f );
extern  cf_bool         CFIsU32( cfloat *f );
extern  cf_bool         CFIsU64( cfloat *f );
extern  cf_bool         CFIs32( cfloat * cf );

extern  cf_bool         CFIsSize( cfloat *f, uint size );
extern  cf_bool         CFSignedSize( cfloat *f, uint size );
extern  cf_bool         CFUnSignedSize( cfloat *f, uint size );

extern  signed_16       CFCnvF16( cfloat *f );
extern  signed_32       CFCnvF32( cfloat *f );
extern  signed_64       CFCnvF64( cfloat *f );
extern  double          CFToF( cfloat *f );

extern  int             CFAccess(cfloat*,int);
extern  void            CFDeposit(cfloat*,int,int);
extern  void            CFClean(cfloat*);
extern  cfloat  *       CFAlloc(int);
extern  void            CFFree(cfloat*);
extern  cfloat  *       CFCopy(cfloat*);
extern  cfloat  *       CFDiv(cfloat*,cfloat*);

extern  cfloat  *       CFMul( cfloat *op1, cfloat *op2 );
extern  cfloat  *       CFDiv( cfloat *op1, cfloat *op2 );
extern  cfloat  *       CFInverse( cfloat *op );
extern  int             CFOrder( cfloat *float1, cfloat *float2 );
extern  cfloat  *       CFAdd( cfloat *op1, cfloat *op2 );
extern  cfloat  *       CFSub( cfloat *op1, cfloat *op2 );
extern  void            CFNegate( cfloat *f );
extern  int             CFCompare( cfloat *op1, cfloat *op2 );
extern  int             CFTest( cfloat *f );
extern  int             CFAccess( cfloat *f, int index );
extern  void            CFDeposit( cfloat *f, int index, int data );
extern  void            CFClean( cfloat *f );

extern  void            CFInit( cf_callbacks * );
extern  void            CFFini( void );
extern  cf_bool         CFFrlFree( void );

#ifdef __cplusplus
};
#endif

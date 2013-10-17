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
* Description:  Interface to the cfloat library.
*
****************************************************************************/


#include <sys/types.h>
#include "watcom.h"
#include "bool.h"

/* aligned */
#define STRUCT_cfloat( x ) \
{ \
    int             exp; \
    int             len; \
    size_t          alloc; \
    signed char     sign; \
    char            mant[ x ]; \
}

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cfloat STRUCT_cfloat( 1 ) cfloat;

typedef struct cf_callbacks {
    void            *(*alloc)( size_t );
    void            (*free)( void * );
} cf_callbacks;

typedef union flt {
    float           sngl;
    double          dble;
    double          ldble;  // FIXME - need long double support
} flt;

extern  void        CFCnvTarget( cfloat *f, flt *buffer, int class );
extern  char        *CFCnvFS( cfloat *f, char *buffer, int maxlen );
extern  cfloat      *CFCnvSF( const char *bstart );
extern  cfloat      *CFCopy( cfloat *old );
extern  cfloat      *CFTrunc( cfloat *f );
extern  cfloat      *CFRound( cfloat *f );
extern  cfloat      *CFCnvI32F( signed_32 data );
extern  cfloat      *CFCnvU32F( unsigned_32 data );
extern  cfloat      *CFCnvU64F( unsigned_32 low, unsigned_32 high );
extern  cfloat      *CFCnvI64F( unsigned_32 lo, unsigned_32 hi );
extern  cfloat      *CFCnvIF( int data );
extern  cfloat      *CFCnvUF( uint data );

extern  bool        CFIsI8( cfloat *f );
extern  bool        CFIsI16( cfloat *f );
extern  bool        CFIsI32( cfloat *f );
extern  bool        CFIsI64( cfloat *f );
extern  bool        CFIsU8( cfloat *f );
extern  bool        CFIsU16( cfloat *f );
extern  bool        CFIsU32( cfloat *f );
extern  bool        CFIsU64( cfloat *f );
extern  bool        CFIs32( cfloat * cf );
extern  bool        CFIs64( cfloat * cf );

extern  bool        CFIsSize( cfloat *f, uint size );
extern  bool        CFSignedSize( cfloat *f, uint size );
extern  bool        CFUnSignedSize( cfloat *f, uint size );

extern  signed_16   CFCnvF16( cfloat *f );
extern  signed_32   CFCnvF32( cfloat *f );
extern  signed_64   CFCnvF64( cfloat *f );
extern  double      CFToF( cfloat *f );

extern  cfloat      *CFAlloc(size_t);
extern  void        CFFree(cfloat*);
extern  bool        CFFrlFree( void );

extern  cfloat      *CFMul( cfloat *op1, cfloat *op2 );
extern  cfloat      *CFDiv( cfloat *op1, cfloat *op2 );
extern  cfloat      *CFInverse( cfloat *op );
extern  cfloat      *CFAdd( cfloat *op1, cfloat *op2 );
extern  cfloat      *CFSub( cfloat *op1, cfloat *op2 );
extern  void        CFNegate( cfloat *f );
extern  int         CFCompare( cfloat *op1, cfloat *op2 );
extern  int         CFTest( cfloat *f );

extern  void        CFInit( cf_callbacks * );
extern  void        CFFini( void );

#ifdef __cplusplus
};
#endif

/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2024-2025 The Open Watcom Contributors. All Rights Reserved.
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

#define CFLOAT_SIZE (offsetof( cfloat, mant ) + 1)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cfloat STRUCT_cfloat( 1 ) cfloat;

typedef struct cfstruct {
    void            *(*alloc)( size_t );
    void            (*free)( void * );
    void            *head;
} cfstruct;

typedef cfstruct    *cfhandle;

typedef union flt {
    double          ldble;  // FIXME - need long double support
    double          dble;
    float           sngl;
    unsigned_64     u64;
} flt;

extern  void        CFCnvTarget( cfloat *f, flt *buffer, int class );
extern  char        *CFCnvFS( cfloat *f, char *buffer, int maxlen );
extern  cfloat      *CFCnvSF( cfhandle h, const char *str );
extern  cfloat      *CFCopy( cfhandle h, cfloat *f );
extern  cfloat      *CFTrunc( cfhandle h, cfloat *f );
extern  cfloat      *CFRound( cfhandle h, cfloat *f );
extern  cfloat      *CFCnvI32F( cfhandle h, signed_32 data );
extern  cfloat      *CFCnvU32F( cfhandle h, unsigned_32 data );
extern  cfloat      *CFCnvU64F( cfhandle h, unsigned_32 lo32, unsigned_32 hi32 );
extern  cfloat      *CFCnvI64F( cfhandle h, unsigned_32 lo32, unsigned_32 hi32 );
extern  cfloat      *CFCnvIF( cfhandle h, int data );
extern  cfloat      *CFCnvUF( cfhandle h, uint data );

extern  bool        CFIsI8( cfloat *f );
extern  bool        CFIsI16( cfloat *f );
extern  bool        CFIsI32( cfloat *f );
extern  bool        CFIsI64( cfloat *f );
extern  bool        CFIsU8( cfloat *f );
extern  bool        CFIsU16( cfloat *f );
extern  bool        CFIsU32( cfloat *f );
extern  bool        CFIsU64( cfloat *f );
extern  bool        CFIs32( cfloat *f );
extern  bool        CFIs64( cfloat *f );
extern  bool        CFIsFloat( cfloat *f );
extern  bool        CFIsDouble( cfloat *f );

extern  bool        CFIsSize( cfloat *f, uint size );
extern  bool        CFSignedSize( cfloat *f, uint size );
extern  bool        CFUnSignedSize( cfloat *f, uint size );

extern  signed_16   CFCnvF16( cfloat *f );
extern  signed_32   CFCnvF32( cfloat *f );
extern  signed_64   CFCnvF64( cfloat *f );
extern  double      CFToF( cfloat *f );

extern  cfloat      *CFAlloc( cfhandle h, size_t );
extern  void        CFFree( cfhandle h, cfloat *f );
extern  bool        CFFrlFree( cfhandle h );

extern  cfloat      *CFMul( cfhandle h, cfloat *f1, cfloat *f2 );
extern  cfloat      *CFDiv( cfhandle h, cfloat *f1, cfloat *f2 );
extern  cfloat      *CFInverse( cfhandle h, cfloat *f );
extern  cfloat      *CFAdd( cfhandle h, cfloat *f1, cfloat *f2 );
extern  cfloat      *CFSub( cfhandle h, cfloat *f1, cfloat *f2 );
extern  void        CFNegate( cfloat *f );
extern  int         CFCompare( cfloat *f1, cfloat *f2 );
extern  int         CFTest( cfloat *f );
extern  int         CFExp( cfloat *f );

extern  void        CFInit( cfhandle h );
extern  void        CFFini( cfhandle h );

#ifdef __cplusplus
};
#endif

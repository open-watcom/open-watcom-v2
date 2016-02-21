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


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "dbgdefn.h"
#include "ldsupp.h"
#include "mad.h"

/*
        This is all temporary stuff until the library & compiler have
        true long double support.
*/

#define D       MAD_DEFAULT_HANDLING
static const mad_type_info_float F8 =
    { MTK_FLOAT, D, 64, 63, MNR_SIGN_MAG, ME_LITTLE,  1023, 52, 2, 1, MTK_INTEGER, D, 11, 10, MNR_UNSIGNED, ME_LITTLE };
static const mad_type_info_float F10 =
    { MTK_FLOAT, D, 80, 79, MNR_SIGN_MAG, ME_LITTLE, 16383, 64, 2, 0, MTK_INTEGER, D, 15, 14, MNR_UNSIGNED, ME_LITTLE };

#define EIGHT_INDEX( x ) ( 5-(x) )
#define TEN_INDEX( x ) ( 7-(x) )

double LDToD( xreal *ten )
{
    lreal       eight;
#if 0
    int         i;

    eight.f.sign = ten->f.sign;
    if( ten->f.exp == 0 ) {
        eight.f.exp = 0;
    } else {
        eight.f.exp = ten->f.exp - 0x3fff + 0x3ff;
    }
    eight.f.xmant = ten->f.mantissa[TEN_INDEX( 0 )] >> 3;
    for( i = 0; i < 6; ++i ) {
        eight.f.mantissa[EIGHT_INDEX( i )] =
                                ( ten->f.mantissa[TEN_INDEX( i )] << 5 )
                              + ( ten->f.mantissa[TEN_INDEX( i+1 )] >> 3 );
    }
#else
    MADTypeConvert( (mad_type_info *)&F10, ten, (mad_type_info *)&F8, &eight, 0 );
#endif
    return( eight.r );
}

void DToLD( double d, xreal *ten )
{
#if 0
    lreal       eight;
    int         i;

    if( d == 0 ) {
        memset( ten, 0, sizeof( *ten ) );
        return;
    }
    eight.r = d;
    ten->f.sign = eight.f.sign;
    ten->f.exp = eight.f.exp - 0x3ff + 0x3fff;
    ten->f.mantissa[TEN_INDEX( 0 )] = 0x80 | eight.f.xmant << 3;
    for( i = 0; i < 6; ++i ) {
        ten->f.mantissa[TEN_INDEX( i )] |=
                   eight.f.mantissa[EIGHT_INDEX( i )] >> 5;
        ten->f.mantissa[TEN_INDEX( i+1 )] =
                   eight.f.mantissa[EIGHT_INDEX( i )] << 3;
    }
    ten->f.mantissa[TEN_INDEX( 7 )] = 0;
#else
    MADTypeConvert( (mad_type_info *)&F8, &d, (mad_type_info *)&F10, ten, 0 );
#endif
}

void LDToS( char *buff, xreal *v, unsigned a, unsigned b, unsigned c,
                unsigned d, unsigned e, char format, char exp_char )
{
    double      value = LDToD( v );

    sprintf( buff, "%.*g", a, value );
    if( buff[0] == '0' && buff[1] == '\0' ) {
        buff[1] = '.';
        buff[2] = '\0';
    }
// Brian!!! NYI NYI NYI
//  _FtoS( buff, &value, a, b, c, d, e, format, exp_char );
}

void SToLD( const char *ptr, const char **end, xreal *v )
{
    char *endx;

    DToLD( strtod( ptr, &endx ), v );
    *end = endx;
}

int LDCmp( xreal *left, xreal *rite )
{
    double      l, r;

    l = LDToD( left );
    r = LDToD( rite );
    if( l < r ) return( -1 );
    if( l > r ) return( +1 );
    return( 0 );
}

/*
        Careful with these: left, rite, res might be aliases.
*/

void LDAdd( xreal *left, xreal *rite, xreal *res )
{
    DToLD( LDToD( left ) + LDToD( rite ), res );
}

void LDSub( xreal *left, xreal *rite, xreal *res )
{
    DToLD( LDToD( left ) - LDToD( rite ), res );
}

void LDMul( xreal *left, xreal *rite, xreal *res )
{
    DToLD( LDToD( left ) * LDToD( rite ), res );
}

void LDDiv( xreal *left, xreal *rite, xreal *res )
{
    DToLD( LDToD( left ) / LDToD( rite ), res );
}

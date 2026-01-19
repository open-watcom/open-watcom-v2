/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2025-2026 The Open Watcom Contributors. All Rights Reserved.
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


#include "i64.h"


class int64 {

    public:
        int64()
        {
            U64Clear( _d );
        }
        int64( long i )
        {
            I32ToI64( i, &_d );
        }
        int64( unsigned long hi, long lo )
        {
            U64High( _d ) = hi;
            U64Low( _d ) = lo;
        }

        int64 operator=( const int64 & a )
        {
            U64Low( this->_d ) = U64Low( a._d );
            U64High( this->_d ) = U64High( a._d );
            return( *this );
        }

        int64 operator+( const int64 & a ) const
        {
            int64 res;
            U64Add( &res._d, &this->_d, &a._d );
            return( res );
        }
        int64 operator+=( const int64 & a )
        {
            *this = *this + a;
            return( *this );
        }
        int64 operator-( const int64 & a ) const
        {
            int64 res;
            U64Sub( &res._d, &this->_d, &a._d );
            return( res );
        }
        int64 operator-=( const int64 & a )
        {
            *this = *this - a;
            return( *this );
        }
        int64 operator-() const
        {
            int64 neg;
            U64Neg( &neg._d, &this->_d );
            return( neg );
        }

        int64 operator*( const int64 & a ) const
        {
            int64 result;
            U64Mul( &result._d, &this->_d, &a._d );
            return( result );
        }
        int64 operator*=( const int64 & a )
        {
            *this = *this * a;
            return( *this );
        }

        int64 operator/( const int64 & a ) const
        {
            int64 div,rem;
            I64Div( &this->_d, &a._d, &div._d, &rem._d );
            return( div );
        }
        int64 operator/=( const int64 & a )
        {
            *this = *this * a;
            return( *this );
        }

        int64 operator%( const int64 & a ) const
        {
            int64 div,rem;
            I64Div( &this->_d, &a._d, &div._d, &rem._d );
            return( rem );
        }
        int64 operator%=( const int64 & a )
        {
            *this = *this * a;
            return( *this );
        }

        int64 operator&( const int64 & a ) const
        {
            int64 result;
            U64And( &result._d, &this->_d, &a._d );
            return( result );
        }
        int64 operator&=( const int64 & a )
        {
            *this = *this & a;
            return( *this );
        }

        int64 operator|( const int64 & a ) const
        {
            int64 result;
            U64Or( &result._d, &this->_d, &a._d );
            return( result );
        }
        int64 operator|=( const int64 & a )
        {
            *this = *this | a;
            return( *this );
        }

        int64 operator^( const int64 & a ) const
        {
            int64 result;
            U64Xor( &result._d, &this->_d, &a._d );
            return( result );
        }
        int64 operator^=( const int64 & a )
        {
            *this = *this ^ a;
            return( *this );
        }

        int64 operator~() const
        {
            int64 not;
            U64Not( &not._d, &this->_d );
            return( not );
        }

        int64 operator<<( unsigned a ) const
        {
            int64 result;
            U64ShiftL( &result._d, &this->_d, a );
            return( result );
        }
        int64 operator<<=( unsigned a )
        {
            *this = *this << a;
            return( *this );
        }

        int64 operator>>( unsigned a ) const
        {
            int64 result;
            I64ShiftR( &result._d, &this->_d, a );
            return( result );
        }
        int64 operator>>=( unsigned a )
        {
            *this = *this >> a;
            return( *this );
        }

        int operator>( const int64 &a ) const
        {
            return( Compare( a ) > 0 );
        }
        int operator<( const int64 &a ) const
        {
            return( Compare( a ) < 0 );
        }
        int operator>=( const int64 &a ) const
        {
            return( Compare( a ) >= 0 );
        }
        int operator<=( const int64 &a ) const
        {
            return( Compare( a ) <= 0 );
        }
        int operator==( const int64 &a ) const
        {
            return( Compare( a ) == 0 );
        }
        int operator!=( const int64 &a ) const
        {
            return( Compare( a ) != 0 );
        }
        int64 &operator ++()
        {
            U64IncDec( &this->_d, 1 );
            return *this;
        }
        int64 &operator --()
        {
            U64IncDec( &this->_d, -1 );
            return *this;
        }
        int64 operator ++( int )
        {
            int64 res = *this;
            U64IncDec( &this->_d, 1 );
            return res;
        }
        int64 operator --( int )
        {
            int64 res = *this;
            U64IncDec( &this->_d, -1 );
            return res;
        }
        unsigned_32 truncate() const
        {
            return( U32FetchTrunc( this->_d ) );
        }
        divmod( const int64 &a, int64 &quot, int64 &rem )
        {
            I64Div( &this->_d, &a._d, &quot._d, &rem._d );
        }
#ifdef __SW_FPI
    #pragma aux int64_from_double = \
            "fld    qword ptr [esi]" \
            "fistp  qword ptr [edi]" \
        __parm [__edi] [__esi]

    #pragma aux int64_to_double = \
            "fild   qword ptr [edi]" \
            "fstp   qword ptr [esi]" \
        __parm [__edi] [__esi]

        SetDouble( double d )
        {
            int64_from_double( &this->_d, &d );
        }
        double GetDouble() const
        {
            double d;
            int64_to_double( &this->_d, &d );
            return( d );
        }
#endif

    private:
        int Compare( const int64 &a ) const
        {
            return( I64Cmp( &this->_d, &a._d ) );
        }


    private:
        signed_64       _d;
};

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


#include "i64.h"


class uint64 {
    private:
        unsigned_64     _d;

    public:
        void set( unsigned long hi, unsigned long lo )
        {
            _d.u._32[I64HI32] = hi;
            _d.u._32[I64LO32] = lo;
        }

        uint64 operator=( unsigned long i )
        {
            U32ToU64( i, &_d );
            return( *this );
        }

        uint64 operator+( const uint64 & a ) const
        {
            uint64 res;
            U64Add( &this->_d, &a._d, &res._d );
            return( res );
        }
        uint64 operator+=( const uint64 & a )
        {
            *this = *this + a;
            return( *this );
        }
        uint64 operator-( const uint64 & a ) const
        {
            uint64 res;
            U64Sub( &this->_d, &a._d, &res._d );
            return( res );
        }
        uint64 operator-=( const uint64 & a )
        {
            *this = *this - a;
            return( *this );
        }
        uint64 operator-() const
        {
            uint64 neg;
            U64Neg( &this->_d, &neg._d );
            return( neg );
        }

        uint64 operator*( const uint64 & a ) const
        {
            uint64 result;
            U64Mul( &this->_d, &a._d, &result._d );
            return( result );
        }
        uint64 operator*=( const uint64 & a )
        {
            *this = *this * a;
            return( *this );
        }

        uint64 operator/( const uint64 & a ) const
        {
            uint64 div,rem;
            U64Div( &this->_d, &a._d, &div._d, &rem._d );
            return( div );
        }
        uint64 operator/=( const uint64 & a )
        {
            *this = *this * a;
            return( *this );
        }

        uint64 operator%( const uint64 & a ) const
        {
            uint64 div,rem;
            U64Div( &this->_d, &a._d, &div._d, &rem._d );
            return( rem );
        }
        uint64 operator%=( const uint64 & a )
        {
            *this = *this * a;
            return( *this );
        }

        uint64 operator&( const uint64 & a ) const
        {
            uint64 result;
            U64And( &this->_d, &a._d, &result._d );
            return( result );
        }
        uint64 operator&=( const uint64 & a )
        {
            *this = *this & a;
            return( *this );
        }

        uint64 operator|( const uint64 & a ) const
        {
            uint64 result;
            U64Or( &this->_d, &a._d, &result._d );
            return( result );
        }
        uint64 operator|=( const uint64 & a )
        {
            *this = *this | a;
            return( *this );
        }

        uint64 operator^( const uint64 & a ) const
        {
            uint64 result;
            U64Xor( &this->_d, &a._d, &result._d );
            return( result );
        }
        uint64 operator^=( const uint64 & a )
        {
            *this = *this ^ a;
            return( *this );
        }

        uint64 operator~() const
        {
            uint64 not;
            U64Not( &this->_d, &not._d );
            return( not );
        }

        uint64 operator<<( unsigned a ) const
        {
            uint64 result;
            U64ShiftL( &this->_d, a, &result._d );
            return( result );
        }
        uint64 operator<<=( unsigned a )
        {
            *this = *this << a;
            return( *this );
        }

        uint64 operator>>( unsigned a ) const
        {
            uint64 result;
            U64ShiftR( &this->_d, a, &result._d );
            return( result );
        }
        uint64 operator>>=( unsigned a )
        {
            *this = *this >> a;
            return( *this );
        }

        int operator>( const uint64 &a ) const
        {
            return( Compare( a ) > 0 );
        }
        int operator<( const uint64 &a ) const
        {
            return( Compare( a ) < 0 );
        }
        int operator>=( const uint64 &a ) const
        {
            return( Compare( a ) >= 0 );
        }
        int operator<=( const uint64 &a ) const
        {
            return( Compare( a ) <= 0 );
        }
        int operator==( const uint64 &a ) const
        {
            return( Compare( a ) == 0 );
        }
        int operator!=( const uint64 &a ) const
        {
            return( Compare( a ) != 0 );
        }
        uint truncate() const
        {
            return( U32FetchTrunc( this->_d ) );
        }
        uint hi32() const
        {
            return _d.u._32[I64HI32];
        }
        uint lo32() const
        {
            return _d.u._32[I64LO32];
        }
        divmod( const uint64 &a, uint64 &quot, uint64 &rem )
        {
            U64Div( &this->_d, &a._d, &quot._d, &rem._d );
        }

    private:
        int Compare( const uint64 &a ) const
        {
            return( U64Cmp( &this->_d, &a._d ) );
        }
};

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


extern "C" {
    #include "i64.h"
}

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
            _d.u._32[I64HI32] = hi;
            _d.u._32[I64LO32] = lo;
        }

        int64 operator=( const int64 & a )
        {
            this->_d.u._32[0] = a._d.u._32[0];
            this->_d.u._32[1] = a._d.u._32[1];
            return( *this );
        }

        int64 operator+( const int64 & a ) const
        {
            int64 res;
            U64Add( &this->_d, &a._d, &res._d );
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
            U64Sub( &this->_d, &a._d, &res._d );
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
            U64Neg( &this->_d, &neg._d );
            return( neg );
        }

        int64 operator*( const int64 & a ) const
        {
            int64 result;
            U64Mul( &this->_d, &a._d, &result._d );
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
            U64And( &this->_d, &a._d, &result._d );
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
            U64Or( &this->_d, &a._d, &result._d );
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
            U64Xor( &this->_d, &a._d, &result._d );
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
            U64Not( &this->_d, &not._d );
            return( not );
        }

        int64 operator<<( unsigned a ) const
        {
            int64 result;
            U64ShiftL( &this->_d, a, &result._d );
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
            I64ShiftR( &this->_d, a, &result._d );
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
                parm [ edi ] [ esi ]

            #pragma aux int64_to_double = \
                "fild   qword ptr [edi]" \
                "fstp   qword ptr [esi]" \
                parm [ edi ] [ esi ]

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

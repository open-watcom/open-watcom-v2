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
* Description:  String class prototype for GUI classes.
*
****************************************************************************/


#ifndef wstring_class
#define wstring_class

#define strieq(a,b) (stricmp((a),(b))==0)
#define streq(a,b) (strcmp((a),(b))==0)

#include "wobject.hpp"

extern "C" {
        #include <ctype.h>
        #include <string.h>
};

WCLASS WString : public WObject
{
        Declare( WString )
        public:
                WEXPORT WString();
                WEXPORT WString( const WString& s );
                WEXPORT WString( const char* s );
                WString& WEXPORT operator=( const WString& s );
//              WEXPORT WString( unsigned int );
                WEXPORT ~WString();
#ifdef __WATCOM_CPLUSPLUS__
                virtual WString& self() { return *this; }
#endif
                virtual bool WEXPORT isEqual( const WObject* ) const;
                virtual bool WEXPORT operator==( char* cstring ) const;
                virtual bool operator==( WObject const & obj ) const
                        { return( isEqual( &obj ) ); }
                virtual int WEXPORT compare( const WObject * ) const;
                const char& operator[]( int index ) const { return _value[ index ]; }
                void setChar( int index, char ch ) { _value[index] = ch; }
                void WEXPORT deleteChar( int index, int count=1 );
                int WEXPORT size() const { return (_value==NULL) ? 0 : strlen( _value ); }
                const char* WEXPORT gets() const;
                WEXPORT operator const char*() const { return gets(); }
                WEXPORT operator int() const;
                WEXPORT operator long() const;
                void WEXPORT puts( const char * );
                void WEXPORT printf( const char* parms... );
                virtual void WEXPORT concat( const char* str );
                virtual void WEXPORT concat( char chr );
                void WEXPORT concatf( const char* parms... );
                void WEXPORT truncate( int count );
                void WEXPORT chop( int count );
                int WEXPORT trim( bool beg=TRUE, bool end=TRUE );
                virtual bool WEXPORT match( const char* mask ) const;
                virtual bool WEXPORT isMask() const;
                void WEXPORT toLower();
        private:
                char*  _value;
                void fixup();
};

typedef WString* (WObject::*sbc)( const char* c );
typedef void (WObject::*cbs)( WString& );

#endif //wstring_class



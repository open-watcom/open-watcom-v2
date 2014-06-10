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
* Description:  WString class implementation.
*
****************************************************************************/


#include "wstring.hpp"
#include "wobjfile.hpp"

extern "C" {
    #include <stdlib.h>
    #include <stdio.h>
    #include <stdarg.h>
};

#define DEF_BUFFER 500

#define MALLOC(s) (char*)malloc(s)
#define REALLOC(p,s) (char*)realloc(p,s)
#define FREE(p) if( p != NULL ) free(p)

Define( WString )

WEXPORT WString::WString()
    : _value( NULL )
{
}

WEXPORT WString::WString( const WString& x )
    : _value( NULL )
{
    if( x._value != NULL ) {
        _value = MALLOC( strlen( x._value ) + 1 );
        if( _value != NULL ) {
            strcpy( _value, x._value );
        }
    }
}

WString& WEXPORT WString::operator =( const WString& x )
{
    if( &x != this ) {
        if( _value != NULL ) {
            FREE( _value );
            _value = NULL;
        }
        if( x._value != NULL ) {
            _value = MALLOC( strlen( x._value ) + 1 );
            if( _value != NULL ) {
                strcpy( _value, x._value );
            }
        }
    }
    return( *this );
}

WEXPORT WString::WString( const char* str )
{
    if( (str != NULL) && strlen( str ) > 0 ) {
        _value = MALLOC( strlen( str ) + 1 );
        if( _value != NULL ) {
            strcpy( _value, str );
            return;
        }
        // out of memory error
    }
    _value = NULL;
}

WEXPORT WString::~WString()
{
    if( _value != NULL ) {
        FREE( _value );
    }
}

#ifndef NOPERSIST
WString* WEXPORT WString::createSelf( WObjectFile& )
{
    return( new WString() );
}

void WEXPORT WString::readSelf( WObjectFile& p )
{
    WObject::readSelf( p );
    FREE( _value );
    _value = NULL;
    size_t len;
    p.readObject( &len );
    if( len > 0 ) {
        _value = MALLOC( len + 1 );
        if( _value != NULL ) {
            p.readObject( _value, len, true );
        } else {
            // out of memory error
        }
    }
}

void WEXPORT WString::writeSelf( WObjectFile& p )
{
    WObject::writeSelf( p );
    p.writeObject( size() );
    if( size() > 0 ) {
        p.writeObject( _value );
    }
}
#endif

bool WEXPORT WString::isEqual( const WObject* str ) const
{
    if( str == NULL )
        return( false );
    if( _value == NULL ) {
        return( ((WString*)str)->_value == NULL || strlen( ((WString*)str)->_value ) == 0 );
    }
    if( ((WString*)str)->_value == NULL ) {
        return( strlen( _value ) == 0 );
    }
    return( strcmp( _value, ((WString*)str)->_value ) == 0 );
}

bool WEXPORT WString::operator==( const char* cstring ) const
{
    if( _value == NULL ) {
        return( cstring == NULL || strlen( cstring ) == 0 );
    }
    if( cstring == NULL ) {
        return( strlen( _value ) == 0 );
    }
    return( strcmp( _value, cstring ) == 0 );
}

int WEXPORT WString::compare( const WObject* str ) const
{
    // assumes str points to a String
    if( (_value != NULL) && (str != NULL) && (((WString*)str)->_value != NULL) ) {
        return( strcmp( _value, ((WString*)str)->_value ) );
    }
    return( 0 );
}


void WEXPORT WString::deleteChar( size_t index, size_t count )
{
    if( _value != NULL ) {
        size_t len = strlen( _value );
        if( index < len ) {
            if( (index + count) > len ) {
                count = len - index;
            }
            memmove( &_value[index], &_value[index + count], len - ( index + count ) + 1 );
        }
    }
}

const char* WEXPORT WString::gets() const
{
    if( _value != NULL ) {
        return( _value );
    }
    return( "" );
}

WEXPORT WString::operator int() const
{
    if( _value != NULL ) {
        return( atoi( _value ) );
    }
    return( 0 );
}

WEXPORT WString::operator long() const
{
    if( _value != NULL ) {
        return( atol( _value ) );
    }
    return( 0 );
}

void WEXPORT WString::puts( const char* str )
{
    *this = str;
}

void WEXPORT WString::printf( const char* parms... )
{
    char*   buffer;
    va_list args;
    int     bufsize = DEF_BUFFER;

    buffer = MALLOC( bufsize );
    if( buffer != NULL ) {
        va_start( args, parms );
        while( _vbprintf( buffer, bufsize, parms, args ) == bufsize - 1) {
            bufsize *= 2;
            FREE( buffer );
            buffer = MALLOC( bufsize );
            if( buffer == NULL )
                return;
            va_start( args, parms );
            }
        (*this) = buffer;
        FREE( buffer );
    }
}

void WEXPORT WString::concat( char chr )
{
    char temp[2];
    temp[0] = chr;
    temp[1] = '\0';
    concat( temp );
}

void WEXPORT WString::concat( const char* str )
{
    if( (str != NULL) && strlen( str ) > 0 ) {
        if( _value != NULL) {
            size_t len = size();
            char* value = REALLOC( _value, len + strlen( str ) + 1 );
            if( value != NULL ) {
                _value = value;
                strcpy( &_value[len], str );
            }
        } else {
            _value = MALLOC( strlen( str ) + 1 );
            if( _value != NULL ) {
                strcpy( _value, str );
            }
        }
    }
}

void WEXPORT WString::concatf( const char* parms... )
{
    char*   buffer;
    va_list args;
    int     bufsize = DEF_BUFFER;

    buffer = MALLOC( bufsize );
    if( buffer != NULL ) {
        va_start( args, parms );
        while( _vbprintf( buffer, bufsize, parms, args ) == bufsize - 1) {
            bufsize *= 2;
            FREE( buffer );
            buffer = MALLOC( bufsize );
            if( buffer == NULL )
                return;
            va_start( args, parms );
            }
        concat( buffer );
        FREE( buffer );
    }
}

void WEXPORT WString::truncate( size_t count )
{
    if( _value != NULL ) {
        size_t len = strlen( _value );
        if( count < len ) {
            _value[count] = '\0';
        }
        fixup();
    }
}

void WEXPORT WString::chop( size_t count )
{
    if( _value != NULL ) {
        size_t len = strlen( _value );
        if( count > 0 ) {
            if( count <= len ) {
                memmove( _value, &_value[count], len-count+1 );
            }
        }
        fixup();
    }
}

bool WEXPORT WString::match( const char* mask ) const
{
    bool ok = false;
    const char* value = (const char *)_value;
    if( mask == NULL ) mask = "";
    if( value == NULL ) value = "";
    int     i = 0;
    int     j = 0;
    for( ;; ) {
        if( mask[i] == '\0' && value[j] == '\0' ) {
            ok = true;
            break;
        } else if( mask[i] == '*' ) {
            i++;
            while( value[j] != '\0' && toupper(mask[i]) != toupper(value[j]) ) {
                j++;
            }
        } else if( mask[i] == '?' ) {
            i++;
            if( value[j] != '\0' ) {
                j++;
            }
        } else if( value[j] == '?' && mask[i] != '\0' ) {
            i++;
            j++;
        } else if( value[j] == '?' || toupper(mask[i]) == toupper(value[j]) ) {
            i++;
            j++;
        } else {
            break;
        }
    }
    return( ok );
}

bool WEXPORT WString::isMask() const
{
    if( _value != NULL ) {
        for( int i=0; _value[i] != '\0'; i++ ) {
            if( _value[i] == '?' || _value[i] == '*' ) {
                return( true );
            }
        }
    }
    return( false );
}

void WEXPORT WString::toLower()
{
    if( _value != NULL ) {
        size_t icount = strlen( _value );
        for( size_t i=0; i<icount; i++ ) {
            _value[i] = (char)tolower( (unsigned char)_value[i] );
        }
    }
}

void WString::fixup()
{
    if( strlen( _value ) == 0 ) {
        FREE( _value );
        _value = NULL;
    }
}


size_t WEXPORT WString::trim( bool beg, bool end )
{
    if( beg ) {
        if( _value != NULL ) {
            size_t len = strlen( _value );
            size_t i;
            for( i=0; i<len; i++ ) {
                if( _value[i] != ' ' ) break;
            }
            if( i ) chop( i );
        }
    }
    if( end ) {
        if( _value != NULL ) {
            size_t len = strlen( _value );
            size_t i;
            for( i=len; i>0; i-- ) {
                if( _value[i - 1] != ' ' ) break;
            }
            if( i-len ) chop( i-len );
        }
    }
    return( size() );
}

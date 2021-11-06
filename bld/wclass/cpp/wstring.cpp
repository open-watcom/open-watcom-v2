/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "clibext.h"


#define DEF_BUFFER 500

#define MALLOC(s)       (char*)malloc(s)
#define REALLOC(p,s)    (char*)realloc(p,s)
#define FREE(p)         free((void*)(p))

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
    if( ( str != NULL ) && strlen( str ) > 0 ) {
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
    if( _value != NULL ) {
        FREE( _value );
        _value = NULL;
    }
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
    size_t len = size();
    p.writeObject( len );
    if( len > 0 ) {
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
    if( ( _value != NULL ) && ( str != NULL ) && ( ((WString*)str)->_value != NULL ) ) {
        return( strcmp( _value, ((WString*)str)->_value ) );
    }
    return( 0 );
}


void WEXPORT WString::deleteChar( size_t index, size_t count )
{
    if( _value != NULL ) {
        size_t len = strlen( _value );
        if( index < len ) {
            if( ( index + count ) > len ) {
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
    if( _value != NULL ) {
        FREE( _value );
        _value = NULL;
    }
    if( str != NULL ) {
        size_t len = strlen( str );
        if( len > 0 ) {
            _value = MALLOC( len + 1 );
            if( _value != NULL ) {
                strcpy( _value, str );
            }
        }
    }
}

void WEXPORT WString::printf( const char* parms... )
{
    char*   buffer;
    va_list args;
    int     bufsize;

    va_start( args, parms );
    bufsize = vsnprintf( NULL, 0, parms, args ) + 1;
    va_end( args );
    buffer = MALLOC( bufsize );
    if( buffer != NULL ) {
        va_start( args, parms );
        if( vsnprintf( buffer, bufsize, parms, args ) >= 0 ) {
            (*this) = buffer;
        }
        va_end( args );
        FREE( buffer );
    }
}

void WEXPORT WString::concat( char chr )
{
    if( chr != '\0' ) {
        if( _value == NULL ) {
            char* value = MALLOC( 2 );
            if( value != NULL ) {
                _value = value;
                value[0] = chr;
                value[1] = '\0';
            }
        } else {
            size_t len = size();
            char* value = REALLOC( _value, len + 2 );
            if( value != NULL ) {
                _value = value;
                value[len++] = chr;
                value[len] = '\0';
            }
        }
    }
}

void WEXPORT WString::concat( const char* str )
{
    if( str != NULL ) {
        size_t str_len = strlen( str );
        if( str_len > 0 ) {
            if( _value == NULL ) {
                char* value = MALLOC( str_len + 1 );
                if( value != NULL ) {
                    _value = value;
                    strcpy( value, str );
                }
            } else {
                size_t len = size();
                char* value = REALLOC( _value, len + str_len + 1 );
                if( value != NULL ) {
                    _value = value;
                    strcpy( &value[len], str );
                }
            }
        }
    }
}

void WEXPORT WString::concatf( const char* parms... )
{
    char*   buffer;
    va_list args;
    int     bufsize;

    va_start( args, parms );
    bufsize = vsnprintf( NULL, 0, parms, args ) + 1;
    va_end( args );
    buffer = MALLOC( bufsize );
    if( buffer != NULL ) {
        va_start( args, parms );
        if( vsnprintf( buffer, bufsize, parms, args ) >= 0 ) {
            concat( buffer );
        }
        va_end( args );
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
                memmove( _value, &_value[count], len - count + 1 );
            }
        }
        fixup();
    }
}

bool WEXPORT WString::match( const char* mask ) const
{
    bool ok = false;
    const char* value = (const char *)_value;
    if( mask == NULL )
        mask = "";
    if( value == NULL )
        value = "";
    size_t  i = 0;
    size_t  j = 0;
    for( ;; ) {
        if( mask[i] == '\0' && value[j] == '\0' ) {
            ok = true;
            break;
        } else if( mask[i] == '*' ) {
            i++;
            while( value[j] != '\0' && toupper( mask[i] ) != toupper( value[j] ) ) {
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
        } else if( value[j] == '?' || toupper( mask[i] ) == toupper( value[j] ) ) {
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
        for( size_t i = 0; _value[i] != '\0'; i++ ) {
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
        for( size_t i = 0; i < icount; i++ ) {
            _value[i] = (char)tolower( (unsigned char)_value[i] );
        }
    }
}

void WString::fixup()
{
    if( _value != NULL && strlen( _value ) == 0 ) {
        FREE( _value );
        _value = NULL;
    }
}


size_t WEXPORT WString::trim( bool beg, bool end )
{
    size_t len;
    size_t i;

    if( beg ) {
        if( _value != NULL ) {
            len = strlen( _value );
            for( i = 0; i < len; i++ ) {
                if( _value[i] != ' ' ) {
                    break;
                }
            }
            if( i ) {
                chop( i );
            }
        }
    }
    if( end ) {
        if( _value != NULL ) {
            len = strlen( _value );
            for( i = len; i > 0; i-- ) {
                if( _value[i - 1] != ' ' ) {
                    break;
                }
            }
            if( i - len ) {
                chop( i - len );
            }
        }
    }
    return( size() );
}

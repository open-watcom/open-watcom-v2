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
* Description:  "Object" file access routines.
*
****************************************************************************/


#include "wobjfile.hpp"
// set DEBUG_WOBJFILE to check that the separator chars read in are correct
#ifdef DEBUG_WOBJFILE
    #include <assert.h>
#endif

// separator = CR LF
// old EOITEM (v38 and prior) = 0

#define BOOL_ON         '1'
#define BOOL_OFF        '0'

void WEXPORT WObjectFile::readEOItem()
{
    if( _version <= 38 ) {
        #ifdef DEBUG_WOBJFILE
            assert( 0 == getch() ); // EOITEM - 0
        #else
            getch(); // EOITEM - 0
        #endif
    } else {
        #ifdef DEBUG_WOBJFILE
            int  c = getch();     // read CR or LF
            assert( (c == '\r') || (c == '\n') );
            if( '\r' == c )
                assert( '\n' == getch() ); // read LF
        #else
            if( '\r' == getch() ) // read CR or LF
                getch();          // read LF
        #endif
    }
}

void WEXPORT WObjectFile::writeEOItem()
{
#ifndef __UNIX__
    putch( '\r' );
#endif
    putch( '\n' );
}

WEXPORT WObjectFile::WObjectFile( long version )
        : _version( version )
        , _objOk( TRUE )
{
}

WEXPORT WObjectFile::~WObjectFile()
{
        _objects.reset();
}

bool WEXPORT WObjectFile::open( const char* name, OpenStyle style )
{
        _objOk = _objOk & WFile::open( name, style );
        if( _objOk ) {
                _objects.reset();
                if( style & OStyleWrite ) {
                        putl( _version );
                        writeEOItem();
                } else {
                        _version = getl();
                        readEOItem();
                }
        }
        return _objOk;
}

bool WEXPORT WObjectFile::open( const char* name, const char* env, OpenStyle style )
{
    return _objOk = _objOk & WFile::open( name, env, style );
}

bool WEXPORT WObjectFile::close()
{
        _objects.reset();
        return _objOk = _objOk & WFile::close();
}

void WEXPORT WObjectFile::readObject( WObject* obj )
{
        int index = (int)getl();
        readEOItem();
        if( index < 0 ) {
                //error: this kind can't be a reference
                _objOk = FALSE;
        } else if( index < _objects.count() ) {
                //error: can't have been references to pre-allocated object
                _objOk = FALSE;
        } else {
                WString name;
                gets( name );
                readEOItem();
                _objects.add( obj );
                obj->readSelf( *this );
        }
}

WObject* WEXPORT WObjectFile::readObject()
{
        int index = (int)getl();
        readEOItem();
        if( index < 0 ) {
                //no object data to read
                return NULL;
        } else if( index < _objects.count() ) {
                return _objects[ index ];
        } else {
                WString name;
                gets( name );
                readEOItem();
                _objects.add( NULL );   //'create' may add some more!
                WObject* obj = WClass::createObject( name, *this );
                if( obj == NULL ) {
                        //internal error: should never happen!
                        _objOk = FALSE;
                } else {
                        _objects.replaceAt( index, obj );
                        obj->readSelf( *this );
                }
                return obj;
        }
}

void WEXPORT WObjectFile::readObject( bool* obj )
{
        char c = getch();
        if( c == BOOL_OFF || c == 0 ) {
            *obj = 0;
        } else {
            *obj = 1;
        }
        readEOItem();
}

void WEXPORT WObjectFile::readObject( char* obj )
{
    if( _version <= 38 ) {
        *obj = getch();
    } else {
        /* changed to use text format */
        *obj = (char)getl();
    }
    readEOItem();
}

void WEXPORT WObjectFile::readObject( char* obj, int len, bool exact )
{
    if( exact ) {
        gets_exact( obj, len );
    } else {
        gets( obj, len );
    }
    readEOItem();
}

void WEXPORT WObjectFile::readObject( short* obj )
{
        *obj = (short)getl();
        readEOItem();
}

void WEXPORT WObjectFile::readObject( unsigned short* obj )
{
        *obj = (unsigned short)getl();
        readEOItem();
}

void WEXPORT WObjectFile::readObject( int* obj )
{
        *obj = (int)getl();
        readEOItem();
}

void WEXPORT WObjectFile::readObject( unsigned int* obj )
{
        *obj = (unsigned int)getl();
        readEOItem();
}

void WEXPORT WObjectFile::readObject( long* obj )
{
        *obj = getl();
        readEOItem();
}

void WEXPORT WObjectFile::readObject( unsigned long* obj )
{
        *obj = getl();
        readEOItem();
}

bool WEXPORT WObjectFile::writeIndex( WObject* obj, bool force )
{
        bool firstTime = FALSE;
        if( obj == NULL ) {
                putl( -1 );
        } else {
                int index = _objects.indexOfSame( obj );
                if( force || index < 0 ) {
                        index = _objects.count();
                        _objects.add( obj );
                        firstTime = TRUE;
                }
                putl( index );
        }
        writeEOItem();
        return firstTime;
}

void WEXPORT WObjectFile::writeObject( WObject* obj, bool force )
{
        bool firstTime = writeIndex( obj, force );
        if( firstTime && (obj != NULL) ) {
                puts( obj->className() );
                writeEOItem();
                obj->writeSelf( *this );
        }
}

void WEXPORT WObjectFile::writeObject( bool obj )
{
        /* text version */
        if( obj ) {
            putch( BOOL_ON );
        } else {
            putch( BOOL_OFF );
        }
        writeEOItem();
}

void WEXPORT WObjectFile::writeObject( char obj )
{
        putl( obj ); // as of version 39 -- text only
        // putch( obj );
        writeEOItem();
}

void WEXPORT WObjectFile::writeObject( const char* obj )
{
        puts( obj );
        writeEOItem();
}

void WEXPORT WObjectFile::writeObject( short obj )
{
        putl( obj );
        writeEOItem();
}

void WEXPORT WObjectFile::writeObject( unsigned short obj )
{
        putl( obj );
        writeEOItem();
}

void WEXPORT WObjectFile::writeObject( int obj )
{
        putl( obj );
        writeEOItem();
}

void WEXPORT WObjectFile::writeObject( unsigned int obj )
{
        putl( obj );
        writeEOItem();
}

void WEXPORT WObjectFile::writeObject( long obj )
{
        putl( obj );
        writeEOItem();
}

void WEXPORT WObjectFile::writeObject( unsigned long obj )
{
        putl( obj );
        writeEOItem();
}

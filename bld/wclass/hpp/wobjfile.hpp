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


#ifndef wobjectfile_class
#define wobjectfile_class

#include "wfile.hpp"
#include "wvlist.hpp"

#define FORCE true

WCLASS WObjectFile : public WFile {
    public:
        WEXPORT WObjectFile( long version=0 );
        WEXPORT ~WObjectFile();
        bool WEXPORT open( const char *name, OpenStyle style=OStyleRead );
        bool WEXPORT open( const char *name, const char* env, OpenStyle style=OStyleRead );
        bool WEXPORT close();
        long WEXPORT version() { return( _version ); }
        bool WEXPORT objOk() { return( _objOk ); }
        void WEXPORT setObjOk( bool objOk ) { _objOk = objOk; }
        void WEXPORT readObject( WObject* obj );
        WObject* WEXPORT readObject();
        void WEXPORT readObject( bool* obj );
        void WEXPORT readObject( char* obj );           // read a char
        void WEXPORT readObject( unsigned char* obj );  // read a byte
        void WEXPORT readObject( char* obj, int len, bool exact=0 );    // read a string
        void WEXPORT readObject( short* obj );
        void WEXPORT readObject( unsigned short* obj );
        void WEXPORT readObject( int* obj );
        void WEXPORT readObject( unsigned int* obj );
        void WEXPORT readObject( long* obj );
        void WEXPORT readObject( unsigned long* obj );
        void WEXPORT readObject( unsigned long long* obj );

        void WEXPORT writeObject( WObject* obj, bool force=false );
        void WEXPORT writeObject( bool obj );
        void WEXPORT writeObject( char obj );           // write a char
        void WEXPORT writeObject( unsigned char obj );  // write a byte
        void WEXPORT writeObject( const char *obj );    // write a c-string
        void WEXPORT writeObject( short obj );
        void WEXPORT writeObject( unsigned short obj );
        void WEXPORT writeObject( int obj );
        void WEXPORT writeObject( unsigned int obj );
        void WEXPORT writeObject( long obj );
        void WEXPORT writeObject( unsigned long obj );
        void WEXPORT writeObject( unsigned long long obj );

        void WEXPORT readEOItem();
        void WEXPORT writeEOItem();
    private:
        WVList          _objects;
        long            _version;
        bool            _objOk;
        bool WEXPORT writeIndex( WObject* obj, bool force );
};

#endif

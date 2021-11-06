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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#ifndef wfilename_class
#define wfilename_class

#include "wstring.hpp"

#include <stdlib.h>
#include <time.h>
#include "diskos.h"

WCLASS WFileName : public WString {
    Declare( WFileName )
    public:
        WEXPORT WFileName( const char* name=NULL, char pathsep=SYS_DIR_SEP_CHAR );
        WEXPORT ~WFileName();
        WFileName& WEXPORT operator=( const WFileName& f );

        void WEXPORT merge( const char* name );
        void WEXPORT relativeTo( const char* f = NULL );
        void WEXPORT absoluteTo( const char* f = NULL );
        void WEXPORT getCWD( bool slash=true );
        bool WEXPORT setCWD() const;
        bool WEXPORT makeDir() const;
        bool WEXPORT dirExists() const;
        bool WEXPORT attribs( unsigned* attribs = NULL ) const;
        void WEXPORT touch( time_t tm=0 ) const;

        void WEXPORT setDrive( const char* drive );
        const char* WEXPORT drive();
        void WEXPORT setDir( const char* dir );
        const char* WEXPORT dir( bool slash=true );
        void WEXPORT setFName( const char* fName );
        const char* WEXPORT fName();
        void WEXPORT setExt( const char* ext );
        const char* WEXPORT ext();
        void WEXPORT setExtIfNone( const char* ext );
        bool WEXPORT legal() const;
        bool WEXPORT needQuotes( char ch='\"' ) const;
        void WEXPORT addQuotes( char ch='\"' );
        void WEXPORT removeQuotes( char ch='\"' );
        #define matchDrive 0x01
        #define matchDir   0x02
        #define matchFName 0x04
        #define matchExt   0x08
        #define matchAll   matchDrive|matchDir|matchFName|matchExt
        bool WEXPORT match( const char* mask ) const {
             return( match( mask, matchFName|matchExt ) );
        };
        bool WEXPORT match( const char* mask, char ctrlFlags ) const;
        void WEXPORT fullName( WFileName& f ) const;
        void WEXPORT noExt( WFileName& f ) const;
        void WEXPORT noPath( WFileName& f ) const;
        void WEXPORT noPathNoExt( WFileName& f ) const;
        void WEXPORT path( WFileName& f, bool slash=true ) const;
        bool WEXPORT removeFile() const;
        bool WEXPORT renameFile( const char* newname ) const;
        char WEXPORT setPathSep( char );
        char WEXPORT getPathSep();
        void WEXPORT normalize();
        bool WEXPORT addPath( const char *path );
    private:
        char _pathsep[4];
        WString _drive;
        WString _dir;
        WString _fname;
        WString _ext;
};

#endif

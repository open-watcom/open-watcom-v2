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


#ifndef wfilename_class
#define wfilename_class

#include "wstring.hpp"

extern "C" {
        #include <stdlib.h>
        #include <time.h>
};

WCLASS WFileName : public WString {
    Declare( WFileName )
    public:
        WEXPORT WFileName( const char* name=NULL );
        WEXPORT ~WFileName();
        WFileName& WEXPORT operator=( const WFileName& f );

        void WEXPORT merge( const char* name );
        void WEXPORT relativeTo( const char* f = NULL );
        void WEXPORT absoluteTo( const char* f = NULL );
        void WEXPORT getCWD( bool slash=TRUE );
        bool WEXPORT setCWD() const;
        bool WEXPORT makeDir() const;
        bool WEXPORT dirExists() const;
        bool WEXPORT attribs( char* attribs = NULL ) const;
        void WEXPORT touch( time_t tm=0 ) const;

        void WEXPORT setDrive( const char* drive );
        const char* WEXPORT drive() const;
        void WEXPORT setDir( const char* dir );
        const char* WEXPORT dir( bool slash=TRUE ) const;
        void WEXPORT setFName( const char* fName );
        const char* WEXPORT fName() const;
        void WEXPORT setExt( const char* ext );
        const char* WEXPORT ext() const;
        bool WEXPORT legal() const;
        bool WEXPORT needQuotes() const;
        void WEXPORT addQuotes();
        void WEXPORT addSQuotes();
        void WEXPORT removeQuotes();
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
        void WEXPORT noPathNoExt( WFileName& f ) const { f = fName(); }
        void WEXPORT path( WFileName& f, bool slash=TRUE ) const;
        bool WEXPORT removeFile() const;
        bool WEXPORT renameFile( const char* newname ) const;
};

#endif

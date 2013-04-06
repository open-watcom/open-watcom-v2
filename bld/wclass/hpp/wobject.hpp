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
* Description:  Basic object class prototype for GUI classes.
*
****************************************************************************/

#ifndef wobject_class
#define wobject_class

#ifndef wdefinitions
#define wdefinitions

#include "wstd.h"

#ifdef _M_I86
    #define WFAR        __far
    #define _WEXPORT    __export
#else
    #define WFAR
    #define _WEXPORT
#endif

#define WEXPORT

#ifdef __DLL__
    #define WCLASS class __export
#else
    #define WCLASS class
#endif

#endif

WCLASS WObjectFile;

WCLASS WObject {
    public:
        WEXPORT WObject() {}
        virtual WEXPORT ~WObject() {}

        // Returns the name of the class.
        virtual const char *WEXPORT className() const { return "WObject"; };
        virtual bool WEXPORT operator==( const WObject& obj ) const { return isEqual( &obj ); }
        virtual bool WEXPORT operator!=( const WObject& obj ) const { return !isEqual( &obj ); }

        // Checks to see if two objects are equivilant.
        virtual bool WEXPORT isEqual( const WObject* ) const { return FALSE; }

        /* If this works like strcmp( then why return 0 if isEqual() returns
         * false? -- Bryce (Aug 24, 2006)
         */
        // Compairs two objects.  Results like strcmp()
        virtual int WEXPORT compare( const WObject* ) const { return 0; }
#ifdef __WATCOM_CPLUSPLUS__
        virtual WObject& self() { return *this; }
#endif
        virtual void readSelf( WObjectFile& ) {}
        virtual void writeSelf( WObjectFile& ) {}
};

// Callback prototypes
typedef void (WObject::*cb)();
typedef bool (WObject::*bcb)();
typedef bool (WObject::*bcbi)( int i );
typedef void (WObject::*cbi)( int i );
typedef void (WObject::*cbo)( WObject* o );
typedef void (WObject::*cbc)( char* c );

#include "wclass.hpp"

#endif

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
* Description:  Class description class prototype.
*
****************************************************************************/

#ifndef wclass_class
#define wclass_class

typedef WObject* (*ctor)( WObjectFile& );
#ifndef NOPERSIST
#define Declare(c) \
    public: \
        virtual const char* WEXPORT className() const { return( #c ); }\
        static bool initState;\
        static bool initClass();\
        static c* WEXPORT createSelf( WObjectFile& p );\
        virtual void WEXPORT readSelf( WObjectFile& p );\
        virtual void WEXPORT writeSelf( WObjectFile& p );
#else
#define Declare(c)
#endif

#ifndef NOPERSIST
#define Define(c) \
        bool c::initState = c::initClass();\
        bool c::initClass() { return( WClass::addClass( #c, (ctor)&c::createSelf, sizeof( #c ) ) ); }
#else
#define Define(c)
#endif

WCLASS WClass
{
    public:
        static bool WEXPORT addClass( const char* name, ctor ctor, int csize );
        static WObject* createObject( const char* name, WObjectFile& );
};

#endif

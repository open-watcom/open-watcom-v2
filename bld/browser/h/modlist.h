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


#ifndef __MODLIST_H__

#include <wstring.hpp>

#include "hotlist.h"

template <class Type> class WCPtrOrderedVector;

class ModuleItem {
public:
                        ModuleItem( WString * str, bool enabled )
                            : _str( str ), _enabled( enabled ) {}
    virtual             ~ModuleItem() {
                            delete _str;
                        }

    bool                operator == ( const ModuleItem & o ) const {
                            return _str->isEqual( o._str );
                        }

    WString *           _str;
    bool                _enabled;
};

class ModuleList : public HotControlList {
public:
                                ModuleList( WWindow * prt, const WRect & r );
                                ~ModuleList();

        virtual int             count();
        virtual const char *    getString( int index );
        virtual int             getHotSpot( int index, bool pressed );

                void            insert( ModuleItem * );
                ModuleItem *    remove( int index );

                ModuleItem *    operator [] ( int indx );

                void            toggleEnable( WWindow * = NULL );

private:

                WCPtrOrderedVector<ModuleItem> *    _modItems;
};

#define __MODLIST_H__
#endif

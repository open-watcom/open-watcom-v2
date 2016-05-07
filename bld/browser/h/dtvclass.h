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


#ifndef __DTVCLASS_H__
#define __DTVCLASS_H__

#include "optmgr.h"
#include "dtvsym.h"

template<class Type> class WCPtrSortedVector;

class ClassMember {
public:
            ClassMember( Symbol * sym, const char * text );
            ClassMember( Symbol * sym, drmem_hdl drhdl_prt, const char * text );
            ClassMember( drmem_hdl drhdl_prt,  dr_access access, const char * text );
            ~ClassMember() { delete _symbol; }

    bool    operator == ( const ClassMember & other ) const;
    bool    operator <  ( const ClassMember & other ) const;


            Symbol *    _symbol;
            WString     _text;
            dr_access   _access;
            drmem_hdl   _parent;
};

class DTViewClass : public DTViewSymbol {
public:
                        DTViewClass( const Symbol * sym );
                        ~DTViewClass();

    virtual void        event( ViewEvent, View * );
    virtual ViewEvent   wantEvents();

    virtual void        setMenus( MenuManager * );
    virtual void        unsetMenus( MenuManager * );
    virtual void        menuSelected( const MIMenuID & id );

protected:
            void        load();
            void        addDescriptions();
            void        elementClicked( WWindow * );

            void        fillBox();  // use filter to fill list box

private:

    MemberFilter                     _filter;
    WCPtrSortedVector<ClassMember> * _members;
};

#endif // __DTVCLASS_H__

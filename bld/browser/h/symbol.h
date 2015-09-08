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


#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include <wobject.hpp>
#include <wstring.hpp>
#include <wvlist.hpp>
#include <dr.h>
#include <drgetref.h>

#include "wbrdefs.h"
#include "util.h"
#include "descrip.h"

class Module;

class Symbol : public WObject
{
public:
                        Symbol( dr_handle, dr_handle, Module *, char * );
                        Symbol( const Symbol & );
    virtual             ~Symbol();

    virtual WVList &    description( WVList & parts );
    virtual dr_sym_type symtype() const { return DR_SYM_NOT_SYM; }

            bool        isEqual( WObject const * ) const;

    const   char *      scopedName( bool fullScoping = TRUE );
            void        loadUsers( WVList & syms );         /* Symbols */
            void        loadReferences( WVList & refs );    /* References */
            bool        defSourceFile( char * );
            void        filePosition( ulong &line, uint & col );
            int         getHotSpot( bool pressed ) const;

    const   char *      name();

            dr_handle   getParent() const { return _parent; }
            dr_handle   getHandle() const { return _drhandle; }
            Module *    getModule() const { return _module; }
            bool        isDefined() const { return _defined; }
            bool        isAnonymous() const { return _anonymous; }
            bool        isArtificial() const;

    static  Symbol *    defineSymbol( dr_sym_type, dr_handle, dr_handle, Module *, char * );
    static  Symbol *    defineSymbol( const Symbol * );
    static  int         getHotSpot( dr_sym_type type, bool opened, bool p );

protected:
            void        getAnonName();
            void        addDesc( char *name, int u_def, dr_handle drhdl, dr_sym_type st );

    static  void        descCallBack( void *obj, char *name, int u_def, dr_handle drhdl, dr_sym_type st );
private:
    WVList *            _description;

    dr_handle           _drhandle;
    dr_handle           _parent;
    Module *            _module;        // FIXME -- this can be removed
    char *              _name;
    char *              _decname;
    bool                _defined : 1;
    bool                _anonymous : 1;
};

#endif // __SYMBOL_H__

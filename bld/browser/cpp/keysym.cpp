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


#include <regexp.h>
#include <dr.h>

#include "assure.h"
#include "chfile.h"
#include "filefilt.h"
#include "keysym.h"
#include "mem.h"
#include "symbol.h"
#include "util.h"

// convert from dr_sym_type to KeySymType
KeySymType SymTypeConvert[] = {
    KSTFunctions, KSTClasses, KSTEnums, KSTTypedefs,
    KSTVariables, KSTMacros, KSTLabels, KSTNoSyms
};

KeySymbol::KeySymbol()
    : _nameProg( NULL )
    , _contClassProg( NULL )
    , _contFunctionProg( NULL )
    , _searchFor( KSTAllSyms )
    , _anonymous( false )
    , _artificial( false )
    , _declaration( true )
//-----------------------------
{
    _fileFilter = new FileFilter();
}

KeySymbol::KeySymbol( const KeySymbol & o )
            : _nameProg( NULL )         // freed in setSearchString
            , _contClassProg( NULL )
            , _contFunctionProg( NULL )
            , _name( o._name )
            , _contClass( o._contClass )
            , _contFunction( o._contFunction )
            , _searchFor( o._searchFor )
            , _anonymous( o._anonymous )
            , _artificial( o._artificial )
            , _declaration( o._declaration )
//--------------------------------------------
{
    setSearchString( _name, &_nameProg, _name );
    setSearchString( _contClass, &_contClassProg, _contClass );
    setSearchString( _contFunction, &_contFunctionProg, _contFunction );
    _fileFilter = new FileFilter( *o._fileFilter );
}

KeySymbol::~KeySymbol()
//---------------------
{
    WBRFree( _nameProg );
    WBRFree( _contClassProg );
    WBRFree( _contFunctionProg );
    delete _fileFilter;
}

void KeySymbol::setSearchString( String & str, void ** prog,
                                 const char *name )
//------------------------------------------------------------
{
    WBRFree( *prog );   // free existing program
    str = name;         // assign to the string

    try {
        *prog = WBRRegComp( name );
    } catch( CauseOfDeath cause ) {
        IdentifyAssassin( cause );
    }
}

void KeySymbol::setName( const char *name )
//-----------------------------------------
{
    setSearchString( _name, &_nameProg, name );
}

void KeySymbol::setContClass( const char *name )
//----------------------------------------------
{
    setSearchString( _contClass, &_contClassProg, name );
}

void KeySymbol::setContFunction( const char *name )
//-------------------------------------------------
{
    setSearchString( _contFunction, &_contFunctionProg, name );
}

void KeySymbol::setSymType( int type )
//------------------------------------
{
    _searchFor = type;
}

void KeySymbol::setAnonymous( bool anon )
//---------------------------------------
{
    _anonymous = anon;
}

void KeySymbol::setArtificial( bool art )
//---------------------------------------
{
    _artificial = art;
}

void KeySymbol::setDeclaration( bool decl )
//-----------------------------------------
{
    _declaration = decl;
}

bool KeySymbol::matches( Symbol * sym )
//-------------------------------------
// FIXME -- check the parent against both
//          functions and classes
{
    bool   accept;
    char * container;

    if( !(SymTypeConvert[sym->symtype()] & _searchFor) ) {
        return( false );
    }

    if( !_anonymous ) {
        if( sym->isAnonymous() ) {
            return( false );
        }
    }

    if( !_artificial ) {
        if( sym->isArtificial() ) {
            return( false );
        }
    }

    if( !_declaration ) {
        if( !sym->isDefined() ) {
            return( false );
        }
    }

    if( _nameProg ) {
        if( sym->name() ) {
            if( !RegExec( (regexp *)_nameProg, sym->name(), true ) ) {
                return( false );
            }
        } else {
            if( !RegExec( (regexp *)_nameProg, "", true ) ) {
                return( false );
            }
        }
    }

    if( _contClassProg ) {
        if( sym->getParent() != DR_HANDLE_NUL ) {
            container = DRGetName( sym->getParent() );
            if( container ) {
                accept = (bool)RegExec( (regexp *)_contClassProg, container, true );
                WBRFree( container );
                if( !accept ) {
                    return( false );
                }
            } else {
                if( !RegExec( (regexp *)_contClassProg, "", true ) ) {
                    return( false );
                }
            }
        } else {
            return( false );    // not local to anything
        }
    }

    // NYI -- can't check for containing function as this is not
    //        stored in the symbol

    if( !_fileFilter->matches( sym->getHandle() ) ) {
        return false;
    }

    return( true );
}

bool KeySymbol::matches( dr_sym_context * ctxt )
//----------------------------------------------
{
    bool   accept;
    char * container;

    if( !(SymTypeConvert[ctxt->type] & _searchFor) ) {
        return( false );
    }

    if( !_anonymous ) {
        if( ctxt->name == NULL || *ctxt->name == 0 ) {
            return( false );
        }
    }

    if( !_artificial ) {
        if( DRIsArtificial( ctxt->handle ) ) {
            return( false );
        }
    }

    if( !_declaration ) {
        if( !DRIsSymDefined( ctxt->handle ) ) {
            return( false );
        }
    }

    if( _nameProg && ctxt->name ) {
        if( ctxt->name ) {
            if( !RegExec( (regexp *)_nameProg, ctxt->name, true ) ) {
                return( false );
            }
        } else {
            if( !RegExec( (regexp *)_nameProg, "", true ) ) {
                return( false );
            }
        }
    }

    if( _contClassProg ) {
        if(  ctxt->context->classhdl ) {
            container = DRGetName( ctxt->context->classhdl );
            if( container ) {
                accept = (bool)RegExec( (regexp *)_contClassProg, container, true );
                WBRFree( container );
                if( !accept ) {
                    return( false );
                }
            } else {
                if( !RegExec( (regexp *)_contClassProg, "", true ) ) {
                    return( false );
                }
            }
        } else {
            return( false );    // not local to anything
        }
    }

    if( _contFunctionProg ) {
        if( ctxt->context->functionhdl ) {
            container = DRGetName( ctxt->context->functionhdl );
            if( container ) {
                accept = (bool)RegExec( (regexp *)_contFunctionProg, container, true );
                WBRFree( container );
                if( !accept ) {
                    return( false );
                }
            } else {
                if( !RegExec( (regexp *)_contFunctionProg, "", true ) ) {
                    return( false );
                }
            }
        } else {
            return( false );    // not local to anything
        }
    }

    if( !_fileFilter->matches( ctxt->handle ) ) {
        return false;
    }

    return( true );
}

bool KeySymbol::matches( dr_handle drhdl, const char * name )
//---------------------------------------------------------
// perform a partial comparison (no containers)
{
    dr_sym_type type;

    type = DRGetSymType( drhdl );
    if( !(SymTypeConvert[type] & _searchFor) ) {
        return( false );
    }

    if( !_anonymous ) {
        if( name == NULL || *name == 0 ) {
            return( false );
        }
    }

    if( !_artificial ) {
        if( DRIsArtificial( drhdl ) ) {
            return( false );
        }
    }

    if( !_declaration ) {
        if( !DRIsSymDefined( drhdl ) ) {
            return( false );
        }
    }

    if( _nameProg ) {
        if( name ) {
            if( !RegExec( (regexp *)_nameProg, name, true ) ) {
                return( false );
            }
        } else {
            if( !RegExec( (regexp *)_nameProg, "", true ) ) {
                return( false );
            }
        }
    }

    if( !_fileFilter->matches( drhdl ) ) {
        return false;
    }

    return( true );
}

enum KeySymFlags {
    KSFAnonymous    = 0x01,
    KSFArtificial   = 0x02,
    KSFDeclaration  = 0x04,
};

void KeySymbol::read( CheckedFile & file )
//----------------------------------------
{
    uint_8  flags;

    file.readNString( _name );
    file.readNString( _contClass );
    file.readNString( _contFunction );

    setSearchString( _name, &_nameProg, _name );
    setSearchString( _contClass, &_contClassProg, _contClass );
    setSearchString( _contFunction, &_contFunctionProg, _contFunction );

    file.read( &flags, sizeof( uint_8 ) );

    _anonymous = ((flags & KSFAnonymous) != 0);
    _artificial = ((flags & KSFArtificial) != 0);
    _declaration = ((flags & KSFDeclaration) != 0);

    _fileFilter->read( file );
}

void KeySymbol::write( CheckedFile & file )
//-----------------------------------------
{
    uint_8 flags = 0;

    file.writeNString( _name );
    file.writeNString( _contClass );
    file.writeNString( _contFunction );

    if( _anonymous ) {
        flags |= KSFAnonymous;
    }

    if( _artificial ) {
        flags |= KSFArtificial;
    }

    if( _declaration ) {
        flags |= KSFDeclaration;
    }

    file.write( &flags, sizeof( uint_8 ) );

    _fileFilter->write( file );
}

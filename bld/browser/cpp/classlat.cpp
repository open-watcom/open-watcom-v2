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


#include "assure.h"
#include "module.h"
#include "classlat.h"
#include "classtyp.h"
#include "util.h"
#include "mem.h"

inline int myabs( int a ) { return (a < 0) ? -1 * a : a; }

DerivationPtr::DerivationPtr( ClassLattice * cls, dr_access acc,
                              dr_virtuality virt )
            : _class( cls )
            , _virtuality( virt )
            , _access( acc )
//--------------------------------------------------------------
{
}

// make a new pointer of my type
DerivationPtr * ClassLattice::newPtr( ClassLattice * cls,
                                      dr_access acc, dr_virtuality virt )
//-----------------------------------------------------------------------
{
    return new DerivationPtr( cls, acc, virt );
}

// this constructor is designed to be used by the outside world.
// it creates the root node of the lattice, and the _flatClasses list

ClassLattice::ClassLattice( Symbol * sym, bool relax )
        : _handle( sym->getHandle() )
        , _module( sym->getModule() )
        , _name( WBRStrDup( sym->name() ) )
        , _basesLoaded( FALSE )
        , _derivedsLoaded( FALSE )
        , _effAccess( (dr_access) 0 )
        , _virtual( VIRT_NOT_SET )
        , _relaxedVirt( relax )
        , _level( 0 )
//----------------------------------------------------
{
    _flatClasses = new ClassList;
    _flatClasses->add( this );
}

// Internal ctor, used to create non-root nodes
ClassLattice::ClassLattice( dr_handle hdl, Module * mod, char * name,
                            ClassList * vlist, dr_access acc,
                            dr_virtuality virt, bool relaxVirt, int level )
        : _handle( hdl )
        , _module( mod )
        , _name( name )
        , _basesLoaded( FALSE )
        , _derivedsLoaded( FALSE )
        , _flatClasses( vlist )
        , _effAccess( acc )
        , _virtual( (VirtLevel)virt )
        , _relaxedVirt(relaxVirt)
        , _level( level )
//-------------------------------------------------------------------------
{
    _flatClasses->add( this );

    if( _name == NULL ) {               /* OPTME this is expensive */
        Symbol * sym = makeSymbol();
        _name = WBRStrDup( sym->name() );
        delete sym;
    }
}

ClassLattice::~ClassLattice( void )
//---------------------------------
{
    int     i;

    WBRFree( _name );
    for( i = _bases.count(); i > 0; i -= 1 ) {
        delete _bases[ i - 1 ];
    }
    _bases.reset();
    for( i = _deriveds.count(); i > 0; i -= 1 ) {
        delete _deriveds[ i - 1 ];
    }
    _deriveds.reset();

}

ClassLattice * ClassLattice::newLattice( dr_handle hdl, Module * mod,
                                         char * name, ClassList * vlist,
                                         dr_access acc, dr_virtuality virt,
                                         int level )
//-------------------------------------------------------------------------
{
    return new ClassLattice( hdl, mod, name, vlist, acc, virt, _relaxedVirt,
                             level );
}

void ClassLattice::deleteLattice( void )
//--------------------------------------
{
    ClassList * list;

    list = _flatClasses;

    for( int i = list->count(); i > 0; i -= 1 ) {
        delete (*list)[ i - 1 ];
    }
    delete list;
}

char * ClassLattice::name( void ) const
//-------------------------------------
{
    ASSERTION( _name != NULL );
    return _name;
}

bool ClassLattice::isEqual( WObject const * obj ) const
//-----------------------------------------------------
{
    return _handle == ((ClassLattice *)obj)->_handle;
}

Symbol * ClassLattice::makeSymbol( void )
//---------------------------------------
{
    char * name = WBRStrDup( _name );
    return Symbol::defineSymbol( DR_SYM_CLASS, _handle, 0L, _module, name );
}

void ClassLattice::enumerateBases( BaseCB callback, void * obj )
//--------------------------------------------------------------
{
    loadBases();
    for( int i = 0; i < _bases.count(); i += 1 ) {
        if( !callback( *_bases[ i ], obj ) ) break;
    }
}

void ClassLattice::baseClasses( WVList & symlist )
//------------------------------------------------
{
    loadBases();
    for( int i = 0; i < _bases.count(); i += 1 ) {
        symlist.add( _bases[ i ]->_class );
    }
}

void ClassLattice::loadBases( void )
//----------------------------------
{
    if( !_basesLoaded ) {
        _module->setModule();
        DRBaseSearch( _handle, this, baseHook );
        _basesLoaded = TRUE;
    }
}

// joinLattice takes the lowest node of a lattice, and links it to
// another lattice.  this function could stand some serious simplification

void ClassLattice::joinLattice( ClassLattice * lattTo )
//-----------------------------------------------------
{
    ClassList* list = _flatClasses;
    ClassList  adjust;
    int        levelDiff = 0;
    bool       levelSet = FALSE;
    int        i;

    REQUIRE( lattTo != this, "classlattice::joinlattice -- join to myself" );

    for( i = 0; i < list->count(); i += 1 ) {
        ClassLattice * node = (*list)[ i ];
        if( node != NULL ) {
            adjust.add( node );

            for( int j = node->_bases.count(); j > 0; j -= 1 ) {
                DerivationPtr * basePtr = node->_bases[ j - 1 ];
                REQUIRE( node->_flatClasses != lattTo->_flatClasses,
                         "ClassLattice::JoinLattice tried to join related" );
                int index = findClass( *lattTo->_flatClasses, basePtr->_class );

                if( index >= 0 ) {
                    //NYI rely on not having loaded deriveds (otherwise, might kill me!)
                    REQUIRE( !basePtr->_class->_derivedsLoaded, "joinLattice ack" );

                    list->replaceAt( findClass( *list, basePtr->_class ), NULL );

                    if( basePtr->_class != (*lattTo->_flatClasses)[ index ] ) {
                        delete basePtr->_class;


                        // this should probably be a separate function
                        // what it is doing is changing all of the pointers
                        // from basePtr->_class to the same class in lattTo

                        for( int k = list->count(); k > 0; k -= 1 ) {
                            ClassLattice * work = (*list)[ k - 1 ];
                            if( work != NULL ) {
                                for( int l = work->_bases.count(); l > 0; l -= 1 ) {
                                    if( work->_bases[ l - 1 ]->_class == basePtr->_class ) {
                                        work->_bases[ l - 1 ]->adjustTo( (*lattTo->_flatClasses)[ index ] );

                                        int tryDiff = work->_bases[ l - 1 ]->_class->_level + 1 - work->_level;
                                        if( !levelSet ) {
                                            levelDiff = tryDiff;
                                            levelSet = TRUE;
                                        } else {
                                            if( tryDiff > levelDiff ) {
                                                levelDiff = tryDiff;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            node->_flatClasses = lattTo->_flatClasses;
            lattTo->_flatClasses->add( node );
        }
    }

    for( i = adjust.count(); i > 0; i -= 1 ) {
        adjust[ i - 1 ]->_level += levelDiff;
    }
    delete list;
}

/* the hdl parameter is the handle of another symbol.  This method checks
 * the flatClasses list by handle to see if this symbol is already stored
 * in the lattice, and if so returns a pointer to it.
 */

ClassLattice * ClassLattice::joinTo( dr_handle hdl, dr_virtuality virt,
                                     dr_access effAccess, int level )
//---------------------------------------------------------------------------
{
    ClassLattice * ret = NULL;
    for( int i = _flatClasses->count(); i > 0 && ret == NULL; i -= 1 ) {
        ClassLattice * test = (*_flatClasses)[ i - 1 ];

        if( test->_handle == hdl ) {
            switch( test->_virtual ) {
            case VIRT_NOT_SET:
                test->_virtual = (VirtLevel) virt;
                ret = test;
                break;
            case VIRT_VIRT:
                if( virt == DR_VIRTUALITY_VIRTUAL || _relaxedVirt ) {
                    if( effAccess < test->_effAccess ) {
                        test->_effAccess = effAccess;
                    }
                    ret = test;
                }
                break;
            default:
                REQUIRE( test->_virtual == VIRT_NOT_VIRT, "classlat::jointo bad _virtual!" );
                if( _relaxedVirt ) {
                    ret = test;
                }
            }
        }
    }

    if( ret != NULL ) {
        if( myabs(ret->_level) < myabs(level) ) {
            ret->adjustLevelsUp( level - ret->_level );
        }
    }
    return ret;
}

/* this method is called when joinTo adjusts the level of an existing
 * latticeNode.  It adjusts all parents all the way up, providing that
 * they are only one level above the child -- otherwise it stops.
 */

void ClassLattice::adjustLevelsUp( int levelDiff )
//------------------------------------------------
{
    for( int i = _bases.count(); i > 0; i -= 1 ) {
        ClassLattice * baseNode = _bases[ i - 1 ]->_class;

        if( baseNode->_level == _level - 1 ) {
            baseNode->adjustLevelsUp( levelDiff );
        } else {
            break;
        }
    }

    _level += levelDiff;
}

static int ClassLattice::baseHook( dr_sym_type, dr_handle handle, char * name,
                                   dr_handle inheritHandle, void * obj )
//----------------------------------------------------------------------------
{
    dr_access        access;
    dr_access        effAccess;
    dr_virtuality    virtuality;
    ClassLattice *   addnode = NULL;
    ClassLattice *   me = (ClassLattice *) obj;

    access = DRGetAccess( inheritHandle );
    virtuality = DRGetVirtuality( inheritHandle );

    if( access > me->_effAccess ) {
        effAccess = access;
    } else {
        effAccess = me->_effAccess;
    }

    addnode = me->joinTo( handle, virtuality, effAccess, me->_level - 1 );

    if( addnode != NULL ) {
        WBRFree( name );
    } else {
        // NYI - _module isn't necessarily what we want it to be!
        addnode = me->newLattice( handle, me->_module, name, me->_flatClasses,
                                  effAccess, virtuality, me->_level - 1 );
    }

    DerivationPtr * ptr = me->newPtr( addnode, access, virtuality );

    me->_bases.add( ptr );

    return TRUE;
}

void ClassLattice::derivedClasses( WVList & symlist )
//---------------------------------------------------
{
    loadDeriveds();
    for( int i = 0; i < _deriveds.count(); i += 1 ) {
        symlist.add( _deriveds[ i ]->_class );
    }
}

void ClassLattice::enumerateDeriveds( BaseCB callback, void * obj )
//-----------------------------------------------------------------
{
    loadDeriveds();
    for( int i = 0; i < _deriveds.count(); i += 1 ) {
        if( !callback( *_deriveds[ i ], obj ) ) break;
    }
}

void ClassLattice::setDeriveds( void )
//------------------------------------
{
    if( !_derivedsLoaded ) {
        for( int nodeCtr = _flatClasses->count(); nodeCtr > 0; nodeCtr-- ) {
            ClassLattice * node = (* _flatClasses )[ nodeCtr - 1 ];
            node->loadBases();

            for( int baseCtr = node->_bases.count(); baseCtr > 0; baseCtr-- ) {
                if( isEqual( node->_bases[ baseCtr - 1 ]->_class ) ) {
                    _deriveds.add( newPtr( node, DR_ACCESS_PUBLIC, DR_VIRTUALITY_NONE ));
                }
            }
        }
        _derivedsLoaded = TRUE;
    }
}

void ClassLattice::loadDeriveds( void )
//-------------------------------------
{
    if( !_derivedsLoaded ) {
        _module->setModule();
        DRDerivedSearch( _handle, this, deriveHook );
        _derivedsLoaded = TRUE;
    }
}

static int ClassLattice::deriveHook( dr_sym_type, dr_handle handle,
                                     char * name, dr_handle inheritHandle,
                                     void * obj )
//-------------------------------------------------------------------------
{
    ClassLattice *   addnode;
    dr_access        effAccess;
    dr_access        access;
    dr_virtuality    virtuality;
    ClassLattice *   me = (ClassLattice *) obj;

    access = DRGetAccess( inheritHandle );
    virtuality = DRGetVirtuality( inheritHandle );

    if( access > me->_effAccess ) {
        effAccess = access;
    } else {
        effAccess = me->_effAccess;
    }

    addnode = me->joinTo( handle, virtuality, effAccess, me->_level + 1 );

    if( addnode != NULL ) {
        WBRFree( name );
    } else {
        addnode = me->newLattice( handle, me->_module, name, me->_flatClasses,
                                  access, (dr_virtuality) VIRT_NOT_SET,
                                  me->_level + 1 );
    }

    DerivationPtr * ptr = me->ClassLattice::newPtr( addnode, access, virtuality );
    me->_deriveds.add( ptr );

    return TRUE;
}

/*
 * look for cls among derived and base classes, and if found show how it
 * is derived from us / we are derived from it.
 */

char * ClassLattice::derivation( ClassLattice *cls )
//--------------------------------------------------
{
    char            buf[ 18 ];  // long enough for "protected virtual"
    DerivationPtr * ptr;
    int             i;
    bool            found = FALSE;

    for( i = _deriveds.count(); i > 0 && !found; i -= 1 ) {
        ptr = _deriveds[ i - 1 ];
        if( cls == ptr->_class ) {
            found = TRUE;
        }
    }

    for( i = _bases.count(); i > 0 && !found; i -= 1 ) {
        ptr = _bases[ i - 1 ];
        if( cls == ptr->_class ) {
            found = TRUE;
        }
    }

    REQUIRE( found, "ClassLattice::derivation -- couldn't find cls!" );

    switch( ptr->_access ) {
    case DR_ACCESS_PUBLIC:
        strcpy( buf, "public" );
        break;
    case DR_ACCESS_PROTECTED:
        strcpy( buf, "protected" );
        break;
    default:
        REQUIRE( ptr->_access == DR_ACCESS_PRIVATE,
                 "ClassLattice::derivation -- _access out of range!" );
        buf[ 0 ] = '\0';
    }

    if( ptr->_virtuality != DR_VIRTUALITY_NONE ) {
        strcat( buf, " virtual" );
    }
    if( buf[0] != '\0' ) {
        return WBRStrDup( buf );
    }
    return NULL;
}

void ClassLattice::normalizeLevels( void )
//----------------------------------------
{
    int     i;

    if( _flatClasses->count() == 0 )
        return;

    int minimum = (*_flatClasses)[ 0 ]->_level;

    for( i = _flatClasses->count(); i > 0; i -= 1 ) {
        if( (*_flatClasses)[ i - 1 ]->_level < minimum ) {
            minimum = (*_flatClasses)[ i - 1 ]->_level;
        }
    }

    for( i = _flatClasses->count(); i > 0; i -= 1 ) {
        ClassLattice * node = (*_flatClasses)[ i - 1 ];
        node->_level -= minimum;
    }
}

int findClass( ClassList& list, ClassLattice * node )
//---------------------------------------------------
{
    for( int i = list.count(); i > 0; i -= 1 ) {
        if( list[ i - 1 ] && list[ i - 1 ]->isEqual( node ) ) {
            return i - 1;
        }
    }
    return -1;
}

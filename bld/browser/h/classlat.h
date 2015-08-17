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


#ifndef classlattice_h
#define classlattice_h

#include <dr.h>
#include <wtptlist.hpp>
#include "classtyp.h"

/*
 * This header file defines the classes required to build up a class lattice.
 * It consists of two classes:

 * The class DerivationPtr corresponds to an arrow from a derived class to
 * its base class, along with the access and virtuality specified.

 * The class ClassLattice represents a node in the lattice.  Each node
 * stores whether or not it is virtual node (ie. potentially shared), and
 * the effective access this node has in the root node of the lattice.
 * Each node can therefore produce a Symbol representing its class,
 * and decorate the derivation as seen by the root node.

 * One copy of _flatClasses, a ClassList, is
 * created by the first node created in the lattice.  This list contains
 * pointers to all the classes seen.
 */

class ClassLattice;

class DerivationPtr
{
public:
             DerivationPtr(){}
             DerivationPtr( ClassLattice *, dr_access, dr_virtuality );
    virtual ~DerivationPtr(){}

    virtual void    adjustTo( ClassLattice * cls ) { _class = cls; }

    ClassLattice *  _class;
    dr_virtuality   _virtuality;
    dr_access       _access;
};

typedef bool (* BaseCB )( DerivationPtr &, void * );
typedef TemplateList<DerivationPtr *> DeriveList;
typedef TemplateList<ClassLattice *>  ClassList;

enum VirtLevel
{
    VIRT_NOT_SET = -1,      // virtuality not yet decided
    VIRT_NOT_VIRT = 0,      // not virtual
    VIRT_VIRT = 1           // virtual
};

class Module;

class ClassLattice : public WObject
{
public:
            ClassLattice( Symbol * sym, bool relaxVirt = FALSE );

#ifdef DEBUG
            const char *   className() const { return "ClassLattice"; }
#endif
            bool           isEqual( WObject const * obj ) const;  // another classlat
            Symbol *       makeSymbol( void );
    virtual char *         name( void ) const;
            char *         derivation( ClassLattice * );
            dr_handle      getHandle( void ) const { return _drhandle; }
            dr_access      getAccess( void ) const { return _effAccess; }
            void           loadBases( void );
            void           enumerateBases( BaseCB, void * );
            void           baseClasses( WVList & );

            void           loadDeriveds( void );
            void           enumerateDeriveds( BaseCB, void * );
            void           derivedClasses( WVList & );

            void           setDeriveds( void );     // set up deriveds from _flatClasses

            void           deleteLattice( void );   // kill whole lattice

            void           adjustLevelsUp( int levelDiff );
            void           normalizeLevels( void );
            void           joinLattice( ClassLattice * );

protected:
            ClassLattice( dr_handle, Module *, char *, ClassList *, dr_access,
                          dr_virtuality, bool rel, int level );
    virtual ~ClassLattice( void );

    virtual  DerivationPtr * newPtr( ClassLattice *, dr_access, dr_virtuality );
    virtual  ClassLattice *  newLattice( dr_handle, Module *, char *,
                                         ClassList *, dr_access, dr_virtuality,
                                         int );

            ClassLattice *  joinTo( dr_handle, dr_virtuality, dr_access, int );

            ClassList *     _flatClasses;   // a flat list of all nodes
            dr_handle       _drhandle;

            DeriveList      _bases;         // all my base classes
            DeriveList      _deriveds;      // derived from me

            int             _level;

            bool            _derivedsLoaded : 1;// true when loaded my deriveds
            bool            _basesLoaded : 1;  // true when I've loaded my bases
private:
            bool            _relaxedVirt : 1;

            dr_access       _effAccess;     // effective access from root node
            Module *        _module;
            char *          _name;

            VirtLevel       _virtual;


    static  bool            baseHook( dr_sym_type, dr_handle, char *, dr_handle, void * );
    static  bool            deriveHook( dr_sym_type, dr_handle, char *, dr_handle, void * );
};

extern int findClass( ClassList& list, ClassLattice * node );

#endif // classlattice_h

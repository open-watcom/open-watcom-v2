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


#ifndef __GTCLASS_H__
#define __GTCLASS_H__

#include "mem.h"
#include "classlat.h"
#include "gtwin.h"
#include "mempool.h"

class TreeClassNode;
class TreeClassOptions;
class TreeClassWindow;

typedef TemplateList<TreeClassNode*> TreeClassList;

/*--------------------------- TreeClassPtr -------------------------------*/

class TreeClassPtr : public TreePtr, public DerivationPtr
{
public:
            TreeClassPtr(){}
            TreeClassPtr( TreeClassWindow * prt, TreeClassNode * from,
                          TreeClassNode * to, dr_access, dr_virtuality );
           ~TreeClassPtr() {};

    void *              operator new( size_t );
    void                operator delete( void * );

    virtual void        adjustTo( ClassLattice * cls );
    virtual PaintInfo * getPaintInfo( void );
    virtual TreePtr *   makeClone();
private:
            TreeClassWindow *   _parent;
    static MemoryPool           _pool;
};

/*--------------------------- TreeClassNode -------------------------------*/

class TreeClassNode : public TreeNode, public ClassLattice
{
public:
            TreeClassNode( TreeWindow* prt, Symbol * sym );

    virtual ~TreeClassNode() {};

    void *                  operator new( size_t , TreeClassNode * );
    void *                  operator new( size_t, MemoryPool & );
    void *                  operator new( size_t );
    void                    operator delete( void * );

    virtual char *          name()  { return ClassLattice::name(); }

    virtual PaintInfo *     getPaintInfo( void );
    virtual int             getLevel( void ) const;
    virtual dr_handle       getHandle( void ) const { return ClassLattice::_handle; }
    virtual TreePtr*        getPtr( TreeList_T list, int index );
    virtual TreeNode*       getNode( TreeList_T list, int index );
    virtual int             getCount( TreeList_T list );
    virtual TreeNode *      removeNode( TreeList_T list, TreeNode * node );
    virtual void            addPtr( TreeList_T list, TreePtr * );
    virtual void            swapNode( TreeList_T list, TreeNode * from, TreeNode * to );
    virtual void            sortPrtKids();
    virtual Symbol *        makeSymbol( void ) { return ClassLattice::makeSymbol(); }
    virtual bool            isRelated( TreeNode * node );
    virtual void            deleteTree( void ) { deleteLattice(); }
            bool            doParents( WVList& world, TreeClassList & roots,
                                       TreeClassList & joinTo );

            void            setAllDeriveds( void );
protected:
            TreeClassNode( TreeWindow* prt, dr_handle hdl, Module *, char *,
                           ClassList *, dr_access, dr_virtuality, int );

    virtual  ClassLattice *  newLattice( dr_handle hdl, Module *mod, char *nm,
                                         ClassList *l, dr_access acc,
                                         dr_virtuality v, int lvl );

    virtual  DerivationPtr * newPtr( ClassLattice *, dr_access, dr_virtuality );

private:
    static  MemoryPool          _pool;
};

/*------------------------- TreeClassWindow -------------------------------*/

class TreeClassWindow : public TreeWindow
{
public:
            TreeClassWindow( char * title );
    virtual ~TreeClassWindow();

            /* NYI ITB -- These options must be put into OptionMgr NYI */

            uint_16         getGroupUses( int ) //{ return _options->getUses(grp); }
                            { return 0; }           // FIXME!

            Color           getColour( int , int ) //{ return _options->getColour(grp,ix); }
                            { return ColorBlack; }    // FIXME!

            WLineStyle      getStyle( int , int ) //{ return _options->getStyle(grp,ix); }
                            { return LS_PEN_SOLID; }    // FIXME!
            int             getThickness( int , int ) //{ return _options->getThickness(grp,ix); }
                            { return 1; }                   // FIXME!

            bool            contextHelp( bool );

    virtual ViewIdentity    identity(){ return VIInheritTree; }

protected:

    virtual void            fillRoots( void );
    const   char *          emptyText();

};

#endif //__GTCLASS_H__

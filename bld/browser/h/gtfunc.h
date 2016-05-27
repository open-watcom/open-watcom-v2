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


#ifndef __GTFUNC_H__
#define __GTFUNC_H__

#include "symbol.h"
#include "gtwin.h"
#include "gtref.h"
#include "gtcycle.h"
#include "mempool.h"

class KeySymbol;
class TreeFuncWindow;
class TreeFuncOptions;
class TreeFuncNode;
class TreeFuncPtr;
typedef TemplateList<TreeFuncNode*> TreeFuncList;
typedef TemplateList<TreeFuncPtr*>  TreeFuncPtrList;

/*------------------------- TreeFuncPtr -----------------------*/

class TreeFuncPtr : public TreePtr
{
public:
            TreeFuncPtr(){}
            TreeFuncPtr( TreeFuncWindow * prt, TreeNode * from,
                         TreeNode * to, dr_ref_info ref );
    virtual ~TreeFuncPtr() {}
    void *              operator new( size_t );
    void                operator delete( void * );

    virtual PaintInfo * getPaintInfo( void );
    virtual TreePtr *   makeClone();
            void        incRefs( void ) { _numRefs += 1; }

private:
            dr_ref_info         _reference;
            int                 _numRefs;
            TreeFuncWindow *    _parent;

    static MemoryPool           _pool;
};

/////////////////////////// TreeFuncNode /////////////////////////

class TreeFuncNode : public TreeCycleNode
{
            struct FuncSearchData {
                Module *        mod;
                TreeNodeList*   roots;
                TreeFuncWindow* parentWin;
                KeySymbol *     key;

                #if DEBUG
                int             funcsFound;
                #endif
            };

public:
            TreeFuncNode( TreeWindow * prt, dr_sym_type stp,
                          drmem_hdl drhdl, Module * mod, char * name,
                          TreeCycleList * flatNode = NULL,
                          TreeRefList * ref = NULL );
    virtual ~TreeFuncNode();
    void *              operator new( size_t );
    void                operator delete( void * );

    virtual drmem_hdl   getHandle( void ) const { return _drhandle; }
    virtual char *      name();
    virtual Symbol *    makeSymbol( void );
    virtual PaintInfo * getPaintInfo( void );
    virtual void        sortPrtKids();

            const char *className() const { return "TreeFuncNode"; }

    static  void        loadModule( TreeFuncWindow * prt, Module * mod,
                                    TreeNodeList& roots, KeySymbol * filt );

private:
            dr_sym_type     _symType;
            drmem_hdl       _drhandle;
            Module*         _module;
            char *          _name;
            char *          _decName;

            int             findFunction( drmem_hdl );
    static  bool            TreeFuncHook( drmem_hdl, dr_ref_info *,
                                          char *, void *);
    static MemoryPool           _pool;
};

/////////////////////////// TreeFuncWindow /////////////////////////

class TreeFuncWindow : public TreeWindow
{
public:
            TreeFuncWindow( char * title );
    virtual ~TreeFuncWindow();

            uint_16         getGroupUses( int ) // { return _options->getUses(grp); }
                            { return 0; }           // FIXME!
            Color           getColour( int , int ) //{ return _options->getColour(grp,ix); }
                            { return ColorBlack; }       // FIXME!
            WLineStyle      getStyle( int , int )  //{ return _options->getStyle(grp,ix); }
                            { return LS_PEN_SOLID; }     // FIXME!
            int             getThickness( int , int ) //{ return _options->getThickness(grp,ix); }
                            { return 1; }                   // FIXME!

            bool            contextHelp( bool );

    virtual ViewIdentity    identity() { return VICallTree; }

protected:
    virtual void            fillRoots( void );
    const   char *          emptyText();

};

#endif // __GTFUNC_H__

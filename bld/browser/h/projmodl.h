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


#ifndef __PROJMODL_H__
#define __PROJMODL_H__

#include <regexp.h>
#include <wobject.hpp>
#include <wvlist.hpp>
#include <wfilename.hpp>
#include <dr.h>
#include "wbrdefs.h"
#include "enumtype.h"
#include "filtflag.h"

class Symbol;
class KeySymbol;
class Module;
class WFileName;
class CheckedBufferedFile;
class TreeOptionCollection;

enum MagicOptions {
    Magic_All,
    Magic_None,
    Magic_Some
};

enum SearchType {
    Search_StartsWith,
    Search_Contains
};

enum FilterSets {
    Filt_Members = 0x01,
    Filt_Methods = 0x02,
    Filt_Both    = 0x03
};

enum TreeDirection {
    TreeVertical    = 0x01,
    TreeHorizontal  = 0x02
};

enum TreeOptionCluster {
    TreeClassCluster = 0,
    TreeFuncCluster = 1,
    TreeLastCluster = 2
};

class ProjSearchCtxt : public WObject
{
public:
                        ProjSearchCtxt( int modIdx );
    virtual             ~ProjSearchCtxt();

    void                setContext( dr_search_context * ctxt );

    int                 _modIdx;
    dr_search_context * _context;
};

typedef bool (Module::*modfn)( WVList *, void * );

class ProjectModel : public WObject
{
public:
                        ProjectModel( char * cmdline, int len );
                        ProjectModel( const char * filename );
    virtual             ~ProjectModel();

    /*------------------- symbol searching gear ------------------------*/

    ProjSearchCtxt *    findSymbols( WVList *dest, KeySymbol *filter,
                                     ProjSearchCtxt * ctxt=NULL,
                                     int numItems=-1 );
    bool                findScopedSymbols( WVList &symInfos, const char *name,
                                           WFileName &file,
                                           ulong line, uint col );
    bool                findReferences( WVList *dest, Symbol *sym );
    bool                findRefSyms( WVList *dest, Symbol *sym );

    /*------------------- module handling gear ------------------------*/

    bool                addModule( const char *fileName );
    bool                removeModule( const char *fileName );
    bool                enableModule( const char *fileName, bool state );
    bool                editModule( WWindow * prt, const char * fileName );
    WVList &            modules() { return _modules; }
    Module *            findModule( const char * fname );
    void                updateModules();
    void                checkModuleTimeStamp();

    const char *        file();
    void                getFile( WFileName & );
    void                tellMeIncluded( char * fileName );

    void                addFile( const char * mode, const char *fileName,
                                 bool enable );

    bool                busy() { return _busyCnt != 0; }
    bool                hasFile( void ) { return _hasFile; }

    /*------------------- option related stuff ------------------------*/

    FilterFlags         setFiltFlags( FilterSets set, FilterFlags flags );
    FilterFlags         getFiltFlags( FilterSets set ) const;

    #if 0   // FIXME -- broken
    TreeOptionCollection *  getOptionCluster( TreeOptionCluster index );
    #endif

    void                setTreeDirection( TreeDirection dir )
                            { _treeDirect = dir; }
    TreeDirection       getTreeDirection( void ) const
                            { return _treeDirect; }

    bool                getSmartEdges( void ) { return _smartEdges; }
    void                setSmartEdges( bool s ) { _smartEdges = s; }

    EnumViewStyle       getEnumStyle( void ) { return _enumStyle; }
    void                setEnumStyle( EnumViewStyle enumStyle )
                            { _enumStyle = enumStyle; }

    void                setIgnoreCase( bool ignorecase )
                            { CaseIgnore = ignorecase; }
    void                setSearchAnchor( bool anchor );

    void                setMagic( MagicOptions magic );
    void                setMagicString( char * magicString );
    // this is pretty dangerous -- maybe should be private
    void                setAllDefault();

    /*------------------- option writing stuff ------------------------*/

    bool                save( void );
    bool                saveTo( const char * filename );
    bool                needsSave(){return _needsSave;}

    void                saveEnumStyle( CheckedBufferedFile& saveFile );
    void                saveEdgeStyle( CheckedBufferedFile& saveFile );
    void                saveTreeDirection( CheckedBufferedFile& saveFile );
    void                saveFilterOption( CheckedBufferedFile& saveFile,
                                          FilterSets whichFilter );
    void                saveCaseIgnore( CheckedBufferedFile& saveFile );
    void                saveSearchType( CheckedBufferedFile& saveFile );
    void                saveMagicType( CheckedBufferedFile& saveFile );
    void                saveMagicChars( CheckedBufferedFile& saveFile );
    void                saveFiles( CheckedBufferedFile& saveFile );
    void                saveModules( CheckedBufferedFile& saveFile );

  private:
    void                    startWork() { _busyCnt++; }
    void                    endWork()   { _busyCnt--; }
    bool                    iterateModules( WVList *, void *, modfn );

    WVList                  _modules;
    WFileName *             _appFile;
    bool                    _hasFile;
    bool                    _needsSave;
    uchar                   _busyCnt; // count of load processes underway

    FilterFlags             _flags[ Filt_Both ];
    bool                    _flagsSet[ Filt_Both ];

    #if 0 // FIXME -- broken
    TreeOptionCollection *  _treeOpCluster[ TreeLastCluster ];
    #endif

    TreeDirection           _treeDirect;

    bool                    _smartEdges;
    EnumViewStyle           _enumStyle;
};

extern ProjectModel * activeProject;

#endif // PROJMODL.H


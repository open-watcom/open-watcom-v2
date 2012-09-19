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


#ifndef mcompon_class
#define mcompon_class

#include "wobject.hpp"
#include "wmodel.hpp"
#include "wpicklst.hpp"
#include "contfile.hpp"
#include "mitem.hpp"
#include "mcommand.hpp"

WCLASS MProject;
WCLASS MComponent : public WObject, public WModel
{
        Declare( MComponent )
        public:
                MComponent( MProject* project, MRule* rule, const WString& mask, const char* target );
                ~MComponent();
                MProject* project() { return _project; }
                WString& mask() { return _mask; }
                MRule* rule() { return _target->rule(); }
                MItem* target() { return _target; }
                WFileName& filename() { return _filename; }
                WFileName& relFilename() { return _relFilename; }
                void setRelFilename( WFileName& fn ) { _relFilename = fn; }
                bool renameComponent( WFileName& fn, MRule* rule, WString& mask );
                WPickList& items() { return _items; };
                void getMaskItems( WVList& list );
                MItem* findSameResult( MItem* item );
                void newItem( MItem* item );
                void removeItem( MItem* item );
                void removeItem( WFileName &fn );
                bool renameItem( MItem* item, WFileName& fn, MRule* rule );
                bool autodepend() { return _autodepend; }
                void setAutodepend( bool state );
                bool autotrack() { return _autotrack; }
                void setAutotrack( bool state );
                SwMode mode() { return _mode; }
                void setMode( SwMode mode );
                void updateItemList( bool update=TRUE );

                const MCommand& before() const { return _before; }
                void setBefore( const MCommand& before );
                const MCommand& after() const { return _after; }
                void setAfter( const MCommand& after );

                void getTargetCommand( WString& cmd );
                void getItemCommand( MItem* item, WString& cmd );
                void addMakeFile( ContFile& pmak );
                bool makeMakeFile();
                void touchItem( MItem* item );
                void touchTarget( bool all );
                void resetRuleRefs();
                void typeDesc( WString& n );
                void refresh();
                void setBatchMode( bool batchMode=TRUE );
                void setDirty( bool dirty=TRUE );
                bool isDirty() { return _dirty; }
                void setNeedsMake( bool needs=TRUE );
                bool needsMake() { return _needsMake; }
                bool tryBrowse();
        private:
                bool            _dirty;
                bool            _needsMake;
                MProject*       _project;
                WPickList       _items;
                WFileName       _filename;
                WFileName       _relFilename;
                MItem*          _target;
                WString         _mask;
                bool            _autodepend;
                bool            _autotrack;
                SwMode          _mode;
                MCommand        _before;
                MCommand        _after;

                WVList          _workFiles;
                bool            _batchMode;

                void writeTargetCD( ContFile& mak );
                void writeRule( ContFile& mak );
                void expand( WString& c, const MCommand& cmd );
                void addWorkFiles( WVList& workfiles, SwMode mode, MComponent* comp );
                void initWorkFiles( WVList& workfiles );
                void finiWorkFiles();
                bool writeCBR( bool mustExist = FALSE );
                void makeNames( const char* spec, WFileName& filename, WFileName& relname, WFileName& targ );
                void addItem( MItem* item );
                bool addFromFilename( WFileName& filename, WString& err );
                bool addFromMask( WFileName& search, WString& err );
};

#endif

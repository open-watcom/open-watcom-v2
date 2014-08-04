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
* Description:  Class definition for project files window.
*
****************************************************************************/


#include "vpemain.hpp"
#include "vcompon.hpp"
#include "wpshbttn.hpp"
#include "wtext.hpp"
#include "wmenuitm.hpp"
#include "maction.hpp"
#include "winpdlg.hpp"
#include "wpickdlg.hpp"
#include "weditdlg.hpp"
#include "wmsgdlg.hpp"
#include "wobjfile.hpp"
#include "wstrlist.hpp"
#include "mrule.hpp"
#include "vsetup.hpp"
#include "vcompdlg.hpp"
#include "mitem.hpp"
#ifndef __OS2__
#include "vfilelst.hpp"
#endif

#include "whotpbox.hpp"

extern "C" {
    #include <dos.h>
    #include <objautod.h>
    #include "rcdefs.h"
    #include <autodep.h>
//  #include <wressetr.h>
    #include <direct.h>
    #include "clibext.h"
};

extern char _viperError[];
extern char _viperRequest[];

Define( VComponent )

enum  w_style {
    W_NORMAL,
    W_MINIMIZED,
    W_MAXIMIZED
};

VComponent::VComponent( VpeMain* parent, MComponent* comp, WStyle style )
//  : WMdiChild( parent, "", style )
    : WMdiChild( parent, "", style & ~WStyleMaximize )
    , WView( comp )
    , _parent( parent )
    , _component( comp )
    , _vItems( NULL )
{
    initCtor();
}

VComponent::VComponent( VpeMain* parent, const WRect& r, MComponent* comp, WStyle style )
//  : WMdiChild( parent, r, "", style )
    : WMdiChild( parent, r, "", style & ~WStyleMaximize )
    , WView( comp )
    , _parent( parent )
    , _component( comp )
    , _vItems( NULL )
{
    initCtor();
}

void VComponent::initCtor()
{
    setIcon( I_Target );

    int box_h = getTextExtentY( "X" );
    int xindent = getTextExtentX( "X" ); /* / 2; Val became too small with new font RR */
    int yindent = getTextExtentY( "X" ) / 2;

    int off = yindent; /* 5; RR */
    _tBox = new WText( this, WRect( xindent, off, -xindent, box_h ), "" );
    _tBox->show();
    off += box_h + box_h/2;

    #define _SourceTitle "Source files:"
    int title_h = getTextExtentY( _SourceTitle );
    _vTitle = new WText( this, WRect( xindent, off, -xindent, title_h ), _SourceTitle );
    _vTitle->show();
    off += title_h + title_h/2;

    _vItems = new WHotPickBox( _component->items(), (cbs)&MItem::name, (icb)&MItem::type,
                  (bcb)&MItem::included, this, WRect(xindent, off, -xindent, -yindent),
                  &_parent->hotSpotList() );
    _vItems->show();

    createControls();

    updateView();

    _parent->setActive( this );

    resetPopups();

    _vItems->onDblClick( this, (cbw)&VComponent::dblClicked );
    _vItems->onHotPress( this, (cbw)&VComponent::expandItem );
    show();
    setFocus();
}

VComponent::~VComponent()
{
    setPopup( NULL );
    _vItems->setPopup( NULL );
    delete _vItems;
    _parent->setActive( NULL );
}

#ifndef NOPERSIST

VComponent* WEXPORT VComponent::createSelf( WObjectFile& p )
{
    WRect r; p.readObject( &r );
#if 1   //this can be removed when WRect is upgraded to handle legitimate
        //negative coordinates
    if( r.x() < 0 ) r.x( 0 );
    if( r.y() < 0 ) r.y( 0 );
#endif
    if( p.version() < 36 ) {
        r.w( (WOrdinal)((long)r.w()*10240/640) );
        r.h( (WOrdinal)((long)r.h()*10240/480) );
    }
    WStyle style = WStyleDefault;
    char wState = W_NORMAL;
    p.readObject( &wState );
    switch( wState ) {
        case W_MINIMIZED:
            style |= WStyleMinimize;
            break;
        case W_MAXIMIZED:
            style |= WStyleMaximize;
            break;
    }
    VpeMain* parent = (VpeMain*)p.readObject();
    WFileName fn;
    p.readObject( &fn );
    MComponent* component = NULL;
    if( p.version() < 35 ) {
        component = parent->project()->findOldComponent( fn );
    } else {
        component = parent->project()->findComponent( fn );
    }
    if( !component ) {
        WMessageDialog::messagef( parent, MsgError, MsgOk, _viperError,
            "Unable to locate target '%s'", (const char*)fn );
        //this isn't great but it will do (I think!)
        component = new MComponent( parent->project(),
            _config->nilRule(), "", fn );
    }
    return( new VComponent( parent, r, component, style ) );
}

void WEXPORT VComponent::readSelf( WObjectFile& p )
{
    int top;
    p.readObject( &top );
    _vItems->setTopIndex( top );
    int sel;
    p.readObject( &sel );
    _vItems->select( sel );
    update();
}

void WEXPORT VComponent::writeSelf( WObjectFile& p )
{
    WRect r; getNormalRectangle( r );
    p.writeObject( &r, FORCE );
    char wState = W_NORMAL;
    if( isMinimized() ) wState = W_MINIMIZED;
    if( isMaximized() ) wState = W_MAXIMIZED;
    p.writeObject( wState );
    p.writeObject( _parent );
    WFileName tgt( _component->relFilename() );
    p.writeObject( &tgt, FORCE );
    p.writeObject( _vItems->topIndex() );
    p.writeObject( _vItems->selected() );
}
#endif

bool VComponent::keyDown( WKeyCode kc, WKeyState ks )
{
    if( !WMdiChild::keyDown( kc, ks ) ) {
        return( _vItems->keyDown( kc, ks ) );
    } else {
        return( true );
    }
}

// forward notification to the child control
bool VComponent::scrollNotify( WScrollNotification sn, int diff )
{
    return( _vItems->scrollNotify( sn, diff ) );
}

void VComponent::resetPopups()
{
    setPopup( _parent->targetPopup() );
    _vItems->setPopup( _parent->itemsPopup() );
}

void VComponent::createControls()
{
    _controls.deleteContents();

    WVList actlist;
    target()->addActions( actlist );
    int icount = actlist.count();

    int BW = 0;
    int BH = 0;
    int i;
    for( i=0; i<icount; i++ ) {
        MAction* action = (MAction*)actlist[i];
        if( action->button() ) {
            WString text;
            action->name( text );
            int bw = getTextExtentX( text );
            int bh = getTextExtentY( text );
            if( BW < bw ) BW = bw;
            if( BH < bh ) BH = bh;
        }
    }

    int xindent = getTextExtentX( "X" ) / 2;
    int yindent = getTextExtentY( "X" ) / 2;

    int off = BH + yindent;
    for( i=0; i<icount; i++ ) {
        MAction* action = (MAction*)actlist[i];
        if( action->button() ) {
            WString text;
            action->name( text );
            WPushButton* bAction =
                new WPushButton( this, WRect(-BW-xindent, off, BW, BH), text );
            bAction->setTagPtr( action );
            off += BH+BH/2;
            bAction->onClick( this, (cbw)&VComponent::bActionComponent );
            int accelKey = action->accelKey();
            if( accelKey ) {
//              bAction->addAccelerator( accelKey );
            }
            bAction->show();
            _controls.add( bAction );
        }
    }

    WRect r = _vItems->getAutosize();
    if( _controls.count() > 0 ) {
        r.w( -BW-xindent );
    } else {
        r.w( -xindent );
    }
    _vItems->move( r );
}

bool VComponent::okToInclude( MItem* item, bool warn, MItem* dupitem )
{
    if( !item->legal() ) {
        if( warn ) {
            WMessageDialog::messagef( this, MsgError, MsgOk, _viperError,
                                      "'%s' must be a legal filename",
                                      (const char*)*item );
        }
        return( false );
    }
    WFileName fullName( *item ); fullName.absoluteTo( _component->filename() );
    WFileName fullTarget; target()->absName( fullTarget );
    if( fullName.match( fullTarget, matchAll ) ) {
        if( warn ) {
            WMessageDialog::messagef( this, MsgError, MsgOk, _viperError,
                                      "'%s' cannot include itself",
                                      (const char*)*item );
        }
        return( false );
    }
    MComponent* comp = NULL;
    MItem* m = _parent->project()->findSameResult( item, &comp );
    if( m && dupitem != m ) {
        if( warn ) {
            WMessageDialog::messagef( this, MsgError, MsgOk, _viperError,
                                "Conflicting file '%s' found in target '%s'",
                                (const char*)*m, (const char*)*comp->target() );
        }
        return( false );
    }
    return( true );
}

bool VComponent::newItem( WFileName& fn, bool warn, bool mark, unsigned owner )
{
    if( fn.size() > 0 ) {
        WFileName cwd;
        _component->filename().path( cwd, true );
        int len = cwd.size();
        if( len > 0 ) {
            if( strnicmp( cwd, fn, len ) == 0 ) {
                fn.chop( len );
            }
        }
    }
    MRule* rule = _config->findMatchingRule( fn, target()->rule(),
                      _component->mask() );
    if( rule ) {
        MItem* item = new MItem( fn, _component, rule );
        if( okToInclude( item, warn ) ) {
            _component->newItem( item );
            _vItems->selectSameTag( item );
            if( mark ) {
                item->setVisited( true );
                item->setOwner( owner );
            }
            return( true );
        }
        delete item;    //added to fix memory leak!
    }
    return( false );
}

void VComponent::mAddItem( WMenuItem* )
{
    WString h; h.printf( "Add File(s) to '%s'", (const char*)*target() );
    bool        list_was_empty;
    WString     inp;
    bool        rc;
#ifndef __OS2__
    int         ret;
#else
    WInputDialog add( this, h );
    add.setBrowse( _config->fileFilters(), true );
#endif

    list_was_empty = false;
    if( _component->items().count() == 0 )
        list_was_empty = true;
    for( ;; ) {
#ifdef __OS2__
        rc = add.getInput( inp, "Enter filename(s) separated by spaces:" );
#else
        ret = GetNewFiles( this, &inp, h, _config->fileFilters(),
                            _component->filename() );
        rc = (ret != 0);
#endif
        if( !rc ) break;
//        inp.toLower();
        WStringList names( inp );
        startWait();
        //someday change this to use addFromFilename() in mcompon.cpp
        bool done = false;
        _component->setBatchMode();
        for( ; !done && names.count() > 0; ) {
            WFileName search( names.cStringAt( 0 ) );
            search.removeQuotes();
            if( search.isMask() ) {
                WFileName asearch( search );
                asearch.absoluteTo( _component->filename() );
                DIR* dir = opendir( asearch );
                if( !dir ) {
                    WMessageDialog::messagef( this, MsgError, MsgOk,
                        _viperError, "no files found for '%s'",
                        (const char*)search );
                    done = true;
                } else {
                    for(;;) {
                        struct dirent* ent = readdir( dir );
                        if( !ent ) break;
                        WFileName fn( ent->d_name );
//                        fn.toLower();
                        fn.absoluteTo( asearch );
                        WVList& items = _component->items();
                        MItem* mm = NULL;
                        for( int j=0; j<items.count(); j++ ) {
                            mm = (MItem*)items[j];
                            WFileName ff; mm->absName( ff );
                            if( ff == fn ) break;
                            mm = NULL;
                        }
                        if( !mm ) {
                            WFileName newfile( ent->d_name );
                            newfile.setDrive( search.drive() );
                            newfile.setDir( search.dir() );
                            if( !newItem( newfile ) ) {
                                done = true;
                                break;
                            }
                        }
                    }
                    closedir( dir );
                }
            } else {
                if( !newItem( search ) ) break;
            }
            names.removeAt( 0 );
        }
        _component->setBatchMode( false );
        stopWait();
        if( names.count() == 0 ) break;
        inp = names.cString();
    }
    if( list_was_empty && _component->items().count() != 0 ) {
        _vItems->select( 0 );
    }
}

void VComponent::mRemoveItem( WMenuItem* )
{
    MItem* m = selectedItem();
    if( m ) {
        bool ok = true;
        if( m->isMask() ) {
            ok = _parent->confirm( "Remove all '%s' files?", m->ext() );
        } else {
            ok = _parent->confirm( "Are you sure you wish to remove '%s'?", *m );
        }
        if( ok ) {
            _vItems->setSelected( _vItems->selected()-1 );
            _component->removeItem( m );
        }
    }
}

void VComponent::mRenameItem( WMenuItem* )
{
    MItem* m = selectedItem();
    if( m ) {
        WInputDialog inp( this, "Rename file" );
        inp.setBrowse( _config->fileFilters(), false );
        WFileName fn( *m );
        bool done = false;
        for( ; !done ; ) {
            if( !inp.getInput( fn, "Enter new filename" ) ) break;
//            fn.toLower();
            fn.removeQuotes();
            MRule* rule = _config->findMatchingRule( fn, target()->rule(),
                              _component->mask() );
            if( rule ) {
                MItem* item = new MItem( fn, _component, rule );
                if( okToInclude( item, true, m ) ) {
                    _component->renameItem( m, fn, rule );
                    done = true;
                }
                delete item;
            }
        }
    }
}

void VComponent::dblClicked( WWindow* )
{
    if( selectedItem() ) {
        doAction( selectedItem(), NULL );
    }
}

void VComponent::expandItem( WWindow* )
{
    MItem* item = selectedItem();
    if( item != NULL && !item->parent() ) {
        item->setExpanded( !item->expanded() );
        _component->updateItemList();
    }
}

void VComponent::actionSetup( MItem* item, MAction* action )
{
    WString a;
    action->text( a );
    WString text;
    text.printf( "Switches for '%s' action on '%s'",
        (const char*)a, (const char*)*item );
    WVList& states = item->getActionStates( action );
    VSetup setup( this, action->tool(), _component->mask(),
        &states, text, mode() );
    if( setup.process() ) {
        _component->setDirty();
    }
}

void VComponent::setupItem( MItem* m )
{
    MRule* r = m->rule();
    if( r != _config->nilRule() ) {
        WString text;
        text.printf( "Switches for making '%s' in target '%s'",
            (const char*)*m, (const char*)*target() );
        VSetup setup( this, r->tool(), _component->mask(),
            &m->states(), text, mode() );
        if( setup.process() ) {
            touchItem( m );
            _component->items().updateAllViews();
            _component->setDirty();
        }
    }
}

void VComponent::showItemCommand()
{
    MItem* m = selectedItem();
    if( m ) {
        WString n;
        _component->getItemCommand( m, n );
        WMessageDialog::messagef( this, MsgInfo, MsgOk,
            "Source Command", "%s", (const char*)n );
    }
}

void VComponent::touchItem( MItem* m )
{
    if( m ) {
        if( m->isMask() ) {
            MsgRetType ret = WMessageDialog::messagef(
                this, MsgQuestion, MsgYesNo, _viperRequest,
                "Mark all %s files in '%s' for remake?",
                m->ext(), (const char*)*target() );
            if( ret == MsgRetYes ) {
                _component->touchItem( m );
            }
        } else {
            _component->touchItem( m );
        }
    }
}

typedef struct capData {
    WPickList*  incList;
    MComponent* comp;
} CapData;

static rtn_status captureName( time_t, char* name, void* data )
{
    MComponent* comp = ((CapData*)data)->comp;
    WPickList* incList = ((CapData*)data)->incList;
    WFileName fn( name ); //fn.toLower();
    MRule* rule = _config->findMatchingRule(
        fn, comp->target()->rule(), comp->mask() );
    if( rule ) {
        incList->add( new MItem( fn, comp, rule ) );
    }
    return( ADR_CONTINUE );
}

void VComponent::mIncludedItems( WMenuItem* )
{
    MItem* m = selectedItem();
    if( m ) {
        WFileName resf;
        m->absResult( resf );
        WPickList incList;
        CapData data;
        data.incList = &incList;
        data.comp = _component;
        startWait();
        _parent->setStatus( "Scanning result file..." );
        walk_status stat = WalkOBJAutoDep( (char*)(const char*)resf,
            captureName, &data );
#ifndef __OS2__
        if( stat == ADW_NOT_AN_OBJ ) {
            stat = WalkRESAutoDep( (char*)(const char*)resf,
                captureName, &data );
        }
#endif
        _parent->setStatus( NULL );
        stopWait();
        if( stat == ADW_NOT_AN_OBJ ) {
            WMessageDialog::info( this, "Unable to scan '%s' result file", (const char*)*m );
        }
        if( stat != ADW_OK ) {
            WMessageDialog::info( this, "File '%s' not yet compiled", (const char*)*m );
        } else if( stat == ADW_NO_DEPS || incList.count() == 0 ) {
            WMessageDialog::info( this, "No files included from '%s'", (const char*)*m );
        } else if( incList.count() > 0 ) {
            WString title;
            title.printf( "Edit files included by" );
            WPickDialog dlg( incList, (cbs)&MItem::name, this, title );
            int index = dlg.pickOne( (const char *)*m );
            if( index >= 0 ) {
                MItem* mi = (MItem*)incList[index];
                doAction( mi, NULL );
            }
        }
        incList.deleteContents();
    }
}

bool WEXPORT VComponent::gettingFocus( WWindow* )
{
    _parent->setActive( this );
#if 0   //this code won't work because of a bug in GUI
    if( _vItems ) {
        _vItems->setFocus();
        return( true );
    }
#endif
    return( false );
}

bool WEXPORT VComponent::losingFocus( WWindow* )
{
    return( false );
}

void WEXPORT VComponent::updateView()
{
    WFileName fn( _component->relFilename() );
    fn.setExt( target()->ext() );
    WString text( fn );
    if( _config->debug() ) {
        text.concat( " '" );
        text.concat( target()->rule()->tag() );
        text.concat( "'" );
    }
    setText( text );

    WString n;
    if( debugMode() ) {
        n.concatf( "(D) " );
    } else {
        n.concatf( "(R) " );
    }
    _component->typeDesc( n );
    _tBox->setText( n );
}

void WEXPORT VComponent::modelGone()
{
}

void WEXPORT VComponent::mAutodepend()
{
    _component->setAutodepend( !_component->autodepend() );
}

void WEXPORT VComponent::mAutotrack()
{
    _component->setAutotrack( !_component->autotrack() );
}

void WEXPORT VComponent::mDebugMode()
{
    MsgRetType ret = WMessageDialog::messagef( this, MsgQuestion, MsgYesNoCancel, _viperRequest,
            "Mark '%s' and all it's component files for remake?", (const char*)*target() );
    if( ret != MsgRetCancel ) {
        bool all = (ret == MsgRetYes);
        _component->setMode( debugMode() ? SWMODE_RELEASE : SWMODE_DEBUG );
        _component->touchTarget( all );
        updateView();
    }
}

#if 1
void WEXPORT VComponent::renameComponent( WFileName& fn, MRule* rule, WString& mask )
{
    if( _parent->createDirectory( fn ) ) {
        MsgRetType ret = WMessageDialog::messagef( this, MsgQuestion,
                MsgYesNoCancel, _viperRequest,
                "Also mark all source files of '%s' for remake?",
                (const char*)*target() );
        if( ret != MsgRetCancel ) {
            if( !_component->renameComponent( fn, rule, mask ) ) {
                WMessageDialog::messagef( this, MsgError, MsgOk, _viperError,
                                          "Unable to rename file" );
            } else {
                _component->touchTarget( ret == MsgRetYes );
                createControls();
                updateView();
            }
        }
    }
}
#else
void WEXPORT VComponent::renameComponent()
{
static char cFilter[] = { "Executables(*.exe)\0*.exe\0Static Libraries(*.lib)\0*.lib\0Dynamic Libraries(*.dll)\0*.dll\0All files(*.*)\0*.*\0\0" };
    VCompDialog dlg( this, "Rename Target", _parent->project(), cFilter );
    WFileName fn( _component->relFilename() );
    fn.setExt( target()->ext() );
    MRule* rule;
    WString mask;
    if( dlg.process( fn, &rule, mask, _component ) ) {
        if( strieq( fn.ext(), ".tgt" ) ) {
            if( !_parent->attachTgtFile( fn ) ) {
                //WMessageDialog::messagef( this, MsgError, MsgOk, _viperError, "Cannot rename file to .tgt extension" );
            }
        } else {
            if( _parent->createDirectory( fn ) ) {
                if( !_component->renameComponent( fn, rule, mask ) ) {
                    WMessageDialog::messagef( this, MsgError, MsgOk, _viperError, "Unable to rename file" );
                } else {
                    touchComponent();
                    createControls();
                    updateView();
                }
            }
        }
    }
}
#endif

void WEXPORT VComponent::setupComponent()
{
    MRule* r = target()->rule();
    if( r != _config->nilRule() ) {
        WString text;
        text.printf( "Switches for making target '%s'", (const char*)*target() );
        VSetup setup( this, r->tool(), _component->mask(), &target()->states(), text, mode() );
        if( setup.process() ) {
            target()->touchResult();
            _component->setDirty();
        }
    }
}

void WEXPORT VComponent::setCompBefore()
{
    WEditDialog ed( this, "Target 'Before' text" );
    MCommand txt( _component->before() );
    if( ed.edit( txt ) ) {
        _component->setBefore( txt );
    }
}

void WEXPORT VComponent::setCompAfter()
{
    WEditDialog ed( this, "Target 'After' text" );
    MCommand txt( _component->after() );
    if( ed.edit( txt ) ) {
        _component->setAfter( txt );
    }
}

void VComponent::showCompCommand()
{
    WString n;
    _component->getTargetCommand( n );
    WMessageDialog::messagef( this, MsgInfo, MsgOk, "Target Command", "%s", (const char*)n );
}

void WEXPORT VComponent::touchComponent( bool quiet )
{
    MsgRetType          ret;

    if( quiet ) {
        ret = MsgRetYes;
    } else {
        ret = WMessageDialog::messagef( this, MsgQuestion,
                            MsgYesNoCancel, _viperRequest,
                            "Also mark all source files of '%s' for remake?",
                            (const char*)*target() );
    }
    if( ret != MsgRetCancel ) {
        bool all = (ret == MsgRetYes);
        _component->touchTarget( all );
    }
}

void WEXPORT VComponent::bActionComponent( WWindow* b )
{
    doAction( target(), (MAction*)((WButton*)b)->tagPtr() );
}

void VComponent::actionError( MItem* item, const WString& actionName )
{
    WString t;
    for( int i=0; i<actionName.size(); i++ ) {
        if( actionName[i] != '&' ) t.concat( (char)tolower(actionName[i]) );
    }
    WMessageDialog::messagef( this, MsgError, MsgOk, _viperError, "You cannot %s '%s'", (const char*)t, (const char*)*item );
}

void WEXPORT VComponent::doAction( MItem* item, const WString& actionName )
{
    WString cmd;
    int location = item->expandAction( actionName, cmd );
    if( location < 0 ) {
        actionError( item, actionName );
    } else {
        _parent->executeCommand( cmd, location );
    }
}

void WEXPORT VComponent::doAction( MItem* item, MAction* action )
{
    WString cmd;
    int location = item->expandAction( action, cmd );
    if( location < 0 ) {
        if( action != NULL ) {
            actionError( item, action->name() );
        }
    } else {
        _parent->executeCommand( cmd, location );
    }
}

MItem* WEXPORT VComponent::selectedItem()
{
    return( (MItem*)_vItems->selectedTagPtr() );
}

void WEXPORT VComponent::removeItem( WFileName &fn )
{
    _component->removeItem( fn );
}

void VComponent::beginFileList( unsigned owner ) {
    int         cnt;
    unsigned    i;
    MItem       *cur;

    WPickList &items = _component->items();
    cnt = items.count();
    for( i=0; i < cnt; i++ ) {
        cur = (MItem *)items[i];
        if( cur->owner() == owner ) cur->setVisited( false );
    }
}

void VComponent::markFile( WFileName &file, unsigned owner ) {
    int         cnt;
    unsigned    i;
    MItem       *cur;
    WFileName   fn;

//    file.toLower(); // since comparison below is actually case sensitive
    WPickList &items = _component->items();
    cnt = items.count();
    for( i=0; ; i++ ) {
        if( i >= cnt ) {
            cur = NULL;
            break;
        }
        cur = (MItem *)items[i];
        cur->absName( fn );
        if( file == fn ) break;
    }
    if( cur != NULL ) {
        cur->setVisited( true );
        cur->setOwner( owner );
    } else{
        fn = file;
        newItem( file, true, true, owner );
    }
}

void VComponent::endFileList( unsigned owner ) {
    int         cnt;
    unsigned    i;
    MItem       *cur;

    WPickList &items = _component->items();
    cnt = items.count();
    i=0;
    while( i < cnt ) {
        for( i=0; i < cnt ; i++ ) {
            cur = (MItem *)items[i];
            if( cur->owner() == owner && !cur->wasVisited() ) {
                _component->removeItem( cur );
                cnt = items.count();
                break;
            }
        }
    }
}

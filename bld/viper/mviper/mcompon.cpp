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


#include "mcompon.hpp"
#include "mconfig.hpp"
#include "mworkfil.hpp"
#include "mproject.hpp"
#include "msymbol.hpp"
#include "mswitch.hpp"
#include "wobjfile.hpp"
#include "mtarget.hpp"
#include <io.h>

extern "C" {
    #include <dos.h>
    #include <direct.h>
};

Define( MComponent )

MComponent::MComponent( MProject* project, MRule* rule, const WString& mask, const char* target )
    : _project( project )
    , _dirty( TRUE )
    , _needsMake( TRUE )
    , _filename( NULL )
    , _relFilename( target )
    , _mask( mask )
    , _target( NULL )
    , _autodepend( rule->autodepend() )
    , _autotrack( rule->autotrack() )
    , _batchMode( FALSE )
    , _mode( SWMODE_DEBUG )
{
    WFileName targ;
    makeNames( target, _filename, _relFilename, targ );
    _target = new MItem( targ, this, rule, TRUE );
}

MComponent::~MComponent()
{
    _items.deleteContents();
    delete _target;
}

#ifndef NOPERSIST
MComponent* WEXPORT MComponent::createSelf( WObjectFile& p )
{
    MProject* project = (MProject*)p.readObject();
    WString ruletag;
    p.readObject( &ruletag );
    MRule* rule = _config->findRule( ruletag );
    if( !rule ) rule = _config->nilRule();
    return new MComponent( project, rule, "", "" );
}

void WEXPORT MComponent::readSelf( WObjectFile& p )
{
    WObject::readSelf( p );
    if( p.version() < 26 ) {
        p.readObject( &_filename );
    }
    _filename = p.filename();
    _filename.toLower();
    if( p.version() < 33 ) {
        p.readObject( _target );
    }
    if( p.version() < 32 ) {
        p.readObject( &_items );
    }
    if( p.version() > 23 ) {
        p.readObject( &_mask );
        if( _config->version() > 0 ) {
            if( _mask.size() == 3 ) {
                _config->setKludge( 3 );
            } else if( _mask.size() == 4 ) {
                _config->setKludge( 4 );
            }
        }
        _config->kludgeString( _mask );
        _config->zapMask( _mask );
    }
    p.readObject( &_autodepend );
    if( p.version() > 36 ) {
        p.readObject( &_autotrack );
    }
    if( p.version() > 27 ) {
        p.readObject( &_mode );
    }
    if( p.version() > 29 ) {
        p.readObject( &_before );
        p.readObject( &_after );
    }
    if( p.version() > 32 ) {
        p.readObject( _target );
    }
    if( p.version() > 31 ) {
        p.readObject( &_items );
    }
    if( _mask[1] == 'v' ) {
        //turn VP targets into MFC targets - VP targets no longer exist
        _mask.setChar( 1, 'm' );
        WFileName fn;
        _filename.noPath( fn );
        WString ruletag;
        if( _mask[0] == 'w' ) {
            ruletag = "WEXE";
        } else {
            ruletag = "NEXE";
        }
        WFileName       tmp_fn( filename() );
        MRule* tmp_rule = _config->findRule( ruletag );
        target()->setRule( tmp_rule );
    }
}

void MComponent::writeSelf( WObjectFile& p )
{
    p.writeObject( _project );
    p.writeObject( &rule()->tag(), FORCE );
    WObject::writeSelf( p );
    p.writeObject( &_mask );
    p.writeObject( _autodepend );
    p.writeObject( _autotrack );
    p.writeObject( _mode );
    p.writeObject( &_before );
    p.writeObject( &_after );
    p.writeObject( _target );   // write these last
    p.writeObject( &_items );
}
#endif

void MComponent::setAutodepend( bool state )
{
    _autodepend = state;
    setDirty();
}

void MComponent::setAutotrack( bool state )
{
    _autotrack = state;
    updateItemList();
    setDirty();
}

void MComponent::setMode( SwMode mode )
{
    _mode = mode;
    setDirty();
    _items.setUpdates();
}

void MComponent::typeDesc( WString& n )
{
    if( _config->debug() ) {
        n.concatf( "(%s)->", (const char*)_mask );
    }
    for( int i=0; i<_config->targets().count(); i++ ) {
        MTarget* tar = (MTarget*)_config->targets()[i];
        if( tar->mask() == _mask ) {
            if( _config->debug() ) {
                n.concatf( "(%s) ", (const char*)tar->mask() );
            }
            WString name;
            tar->name( name );
            n.concat( name );
            return;
        }
    }
    n.concat( "Obsolete form: Rename target type" );
}

void MComponent::getMaskItems( WVList& list )
{
    int icount = _items.count();
    for( int i=0; i<icount; i++ ) {
        MItem* m = (MItem*)_items[ i ];
        if( m->isMask() ) {
            list.add( m );
        }
    }
}

void MComponent::updateItemList( bool update )
{
    if( !_batchMode ) {
        _target->updateAttribs();
        MRule* nilRule = _config->nilRule();
        _items.setUpdates( FALSE );
        for( int i=0; i<_items.count(); i++ ) {
            MItem* item = (MItem*)_items[i];
            item->updateAttribs();
            item->setParent( NULL );
        }

        if( _autotrack ) {
            WVList& tracks = _target->rule()->autotracks();
            int icount = tracks.count();
            for( int i=0; i<icount; i++ ) {
                WString err;
                addFromMask( *(WFileName*)tracks[ i ], err );
            }
/*
            for( i=_items.count(); i>0; ) {
                i--;
                MItem* item = (MItem*)_items[i];
                if( !item->exists() ) {
                    WFileName f; item->path( f, FALSE );
                    if( !f.size() ) {
                        _items.removeAt( i );
                    }
                }
            }
*/
        }

        for( i=_items.count(); i>0; ) {
            i--;
            MItem* item = (MItem*)_items[i];
            if( item->isMask() ) {
    #if 1       //temp code to get rid of old stuff in saved files
                item->setDrive( NULL );
                item->setDir( NULL );
                item->setFName( "*" );
    #endif
                int cc = 0;
                for( int j=0; j<_items.count(); j++ ) {
                    MItem* m = (MItem*)_items[j];
                    if( !m->isMask() && m->match( *item, matchExt ) ) {
                        cc += 1;
                        m->setParent( item );
                    }
                }
                if( cc == 0 ) {
                    _items.removeAt( i );
                }
            }
        }
        for( i=_items.count(); i>0; ) {
            i--;
            MItem* item = (MItem*)_items[i];
            if( !item->isMask() && !item->parent() ) {
                for( int j=0; j<_items.count(); j++ ) {
                    MItem* m = (MItem*)_items[j];
                    if( m->isMask() && item->match( *m, matchExt ) ) {
                        item->setParent( m );
                        break;
                    }
                }
                if( !item->parent() ) {
                    WFileName fn( "*.*" ); fn.setExt( item->ext() );
                    MItem* m = new MItem( fn, this, item->rule() );
                    item->setParent( m );
                    _items.add( m );
                    setDirty();
                }
            }
        }
        _items.setUpdates( update );
    }
}

bool MComponent::renameComponent( WFileName& fn, MRule* rule, WString& mask )
{
    WFileName filename;
    WFileName relname;
    WFileName targ;
    makeNames( fn, filename, relname, targ );
    if( _target->rename( targ, rule ) ) {
        _filename = filename;
        _relFilename = relname;
        _mask = mask;
        MRule* nilRule = _config->nilRule();
        for( int i=0; i<_items.count(); i++ ) {
            MItem* m = (MItem*)_items[i];
            MRule* r = _config->findMatchingRule( *m, _target->rule(), _mask );
            m->setRule( r );
        }
        refresh();
        setDirty();
        return TRUE;
    }
    return FALSE;
}

MItem* MComponent::findSameResult( MItem* item )
{
    WFileName fn;
    if( item->absResult( fn ) ) {
        bool isMask = item->isMask();
        for( int i=0; i<_items.count(); i++ ) {
            MItem* m = (MItem*)_items[i];
            if( isMask || !isMask && !m->isMask() ) {
                WFileName fi;
                if( m->absResult( fi ) ) {
                    if( fn.match( fi, matchAll ) ) {
                        return m;
                    }
                }
            }
        }
    }
    return NULL;
}

void MComponent::addItem( MItem* item )
{
    _items.add( item );
    if( item->parent() ) {
        item->parent()->setExpanded();
    }
}

void MComponent::newItem( MItem* item )
{
    _items.setUpdates( FALSE );
    addItem( item );
    updateItemList( FALSE );
    _items.setUpdates( !_batchMode );
    touchTarget( FALSE );
    setDirty();
}

void MComponent::setBatchMode( bool batchMode )
{
    _batchMode = batchMode;
    updateItemList();
}

void MComponent::removeItem( WFileName &fn ) {
    unsigned    i;
    MItem       *item;

    i = _items.count();
    for( ; i > 0; i-- ) {
        item = (MItem *)_items[i-1];
        if( fn == *item ) {
            removeItem( item );
            break;
        }
    }
}

void MComponent::removeItem( MItem* item )
{
    _items.setUpdates( FALSE );
    for( int i=_items.count(); i>0; i-- ) {
        MItem* m = (MItem*)_items[i-1];
        if( m->parent() == item ) {
            delete _items.removeSame( m );
        }
    }
    delete _items.removeSame( item );
    updateItemList();
    touchTarget( FALSE );
    setDirty();
}

bool MComponent::renameItem( MItem* item, WFileName& fn, MRule* rule )
{
    if( item->rename( fn, rule ) ) {
        updateItemList();
        setDirty();
        item->touchResult();
        return TRUE;
    }
    return FALSE;
}

void MComponent::refresh()
{
    updateItemList();
    updateAllViews();
}

void MComponent::touchItem( MItem* item )
{
    if( item->isMask() ) {
        for( int i=0; i<_items.count(); i++ ) {
            MItem* m = (MItem*)_items[i];
            if( item == m->parent() ) {
                m->touchResult();
            }
        }
    } else {
        item->touchResult();
    }
}

void MComponent::touchTarget( bool all )
{
    _target->touchResult();
    if( all ) {
        initWorkFiles( _workFiles );
        for( int i=0; i<_workFiles.count(); i++ ) {
            MItem* m = ((MWorkFile*)_workFiles[i])->item();
            if( !m->isMask() ) {
                m->touchResult();
            }
        }
        finiWorkFiles();
    }
}

void MComponent::getTargetCommand( WString& cmd )
{
    WFileName fn; _target->absName( fn );
    MWorkFile w( fn, _mode, _target, this );
    expand( cmd, _before );
    w.makeCommand( cmd, NULL );
    expand( cmd, _after );
}

void MComponent::getItemCommand( MItem* item, WString& cmd )
{
    initWorkFiles( _workFiles );
    for( int i=0; i<_workFiles.count(); i++ ) {
        MWorkFile* w = (MWorkFile*)_workFiles[i];
        if( w->item() == item ) {
            w->makeCommand( cmd, NULL );
            break;
        }
    }
    finiWorkFiles();
}

bool MComponent::addFromFilename( WFileName& filename, WString& err )
{
    if( filename.isMask() ) {
        return addFromMask( filename, err );
    }

    MRule* rule = _config->findMatchingRule( filename, _target->rule(), _mask );
    if( rule ) {
        if( rule == _config->nilRule() ) {
            unsigned    cnt;

            cnt = _items.count();
            for( ; cnt > 0; cnt-- ) {
                if( *(MItem *)_items[ cnt - 1 ] == filename ) break;
            }
            if( cnt == 0 ) {
                MItem* item = new MItem( filename, this, rule );
                newItem( item );
            } else {
                return FALSE;
            }
        } else {
            MItem* item = new MItem( filename, this, rule );
            MComponent* comp = NULL;
            MItem* m = _project->findSameResult( item, &comp );
            if( !m ) {
                newItem( item );
            } else {
                delete item;
                if( comp != this ) {
                    err.printf( "Conflicting file '%s' found in target '%s'",
                        (const char*)*m, (const char*)*comp->target() );
                    return FALSE;
                }
            }
        }
        return TRUE;
    }
    return FALSE;
}

bool MComponent::addFromMask( WFileName& search, WString& err )
{
    bool ok = TRUE;
    WFileName asearch( search );
    asearch.absoluteTo( _filename );
    DIR* dir = opendir( asearch );
    if( !dir ) {
        err.printf( "no files found for '%s'", (const char*)search );
        ok = FALSE;
    } else {
        for(;;) {
            struct dirent* ent = readdir( dir );
            if( !ent ) break;
            WFileName newfile( ent->d_name );
            newfile.setDrive( search.drive() );
            newfile.setDir( search.dir() );
            newfile.toLower();
            if( !addFromFilename( newfile, err ) ) {
                ok = FALSE;
                break;
            }
        }
        closedir( dir );
    }
    return ok;
}

static char makeExt[] = { ".mk1" };

void MComponent::addMakeFile( ContFile& pmak )
{
    WFileName mk( _filename );
    mk.setExt( makeExt );
    if( mk.needQuotes() ) {
        mk.addQuotes();
    }
    pmak.printf( "!include %s\n", (const char*)mk );
}

bool MComponent::makeMakeFile()
{
    bool ok = TRUE;
    WFileName mk( _filename );
    mk.setExt( makeExt );

    updateItemList();

    if( needsMake() || !mk.attribs() ) {
        ContFile        tmak;
        if( !tmak.open( mk, OStyleWrite ) ) {
            ok = FALSE;
        } else {
            tmak.puts( "!define BLANK \"\"\n" );
            initWorkFiles( _workFiles );
//          for( int i=0; i<_workFiles.count(); i++ ) {
//              ((MWorkFile*)_workFiles[i])->dump( tmak );
//          }
            writeRule( tmak );
            finiWorkFiles();
            tmak.close();
            ok = tmak.ok();
            setNeedsMake( FALSE );
        }
    }
    return ok;
}

void MComponent::addWorkFiles( WVList& workFiles, SwMode mode, MComponent* comp )
{
    for( int i=0; i<_items.count(); i++ ) {
        MItem* m = (MItem*)_items[i];
        WFileName f;
        m->absName( f );
        MWorkFile* w = new MWorkFile( f, mode, m, comp );
        workFiles.add( w );
    }
}

void MComponent::initWorkFiles( WVList& workFiles )
{
    addWorkFiles( workFiles, _mode, this );
    for( int i=0; i<_workFiles.count(); i++ ) {
        MWorkFile* w = (MWorkFile*)_workFiles[i];
        if( !w->isMask() ) {
            for( int j=0; j<_workFiles.count(); j++ ) {
                MWorkFile* x = (MWorkFile*)_workFiles[j];
                if( x->isMask() && w->match( *x, matchFName|matchExt ) ) {
                    w->insertStates( x );
                }
            }
        }
    }
}

void MComponent::finiWorkFiles()
{
    _workFiles.deleteContents();
}

void MComponent::writeTargetCD( ContFile& mak )
{
    WFileName path;
    _filename.path( path, FALSE );

    if( path.match( NULL, matchDir ) ) {
        path.concat( "\\" );
    }

    mak.printf( " @%s\n", path.drive() );
    mak.printf( " cd %s\n", (const char*)path );
}

void MComponent::writeRule( ContFile& mak )
{
    if( !_target->ismakeable() ) return;

    WFileName tgt;
    _target->absName( tgt );

    for( int i=0; i<_workFiles.count(); i++ ) {
        MWorkFile* w = (MWorkFile*)_workFiles[i];
        MItem* m = w->item();
        if( !m->isMask() && m->ismakeable() ) {
            WFileName r;
            m->absResult( r );
            if( r.needQuotes() ) {
                r.addQuotes();
            }
            if( w->needQuotes() ) {
                w->addQuotes();
            }
            mak.printf( "%s : %s", (const char*)r, (const char*)*w );
            r.removeQuotes();
            w->removeQuotes();
            if( _autodepend ) mak.puts( " .AUTODEPEND" );
            mak.puts( "\n" );
            WString c;
            w->makeCommand( c, NULL );
            writeTargetCD( mak );
            mak.puts( c );
            mak.puts( "\n" );
            w->puts( r );       //setup for later use by target-maker
        }
    }
    if( tgt.needQuotes() ) {
        tgt.addQuotes();
    }
    mak.printf( "%s :", (const char*)tgt );
    tgt.removeQuotes();
    int jcount = _workFiles.count();
    for( int j=0; j<jcount; j++ ) {
        MWorkFile* w = (MWorkFile*)_workFiles[j];
        if( !w->isMask() ) {
            if( w->needQuotes() ) {
                w->addQuotes();
            }
            mak.printf( " %s", (const char*)*w );
            w->removeQuotes();
            w->relativeTo( _filename );
        }
    }
    if( _autodepend ) mak.puts( " .AUTODEPEND" );
    mak.puts( "\n" );
    bool browseable = writeCBR();
    if( _target->ismakeable() ) {
        WFileName fn; _target->absName( fn );
        MWorkFile w( fn, _mode, _target, this );
        WString c;
        if( browseable && _config->browseMerge().size() > 0 ) {
            MCommand cmd;
            cmd.concatf( " %s @$*.cbr", (const char*)_config->browseMerge() );
            expand( c, cmd );
        }
        expand( c, _before );
        w.makeCommand( c, &_workFiles );
        expand( c, _after );
        writeTargetCD( mak );
        mak.puts( c );
        mak.puts( "\n" );
    }
}

void MComponent::setBefore( const MCommand& before )
{
    _before = before;
    setDirty();
}

void MComponent::setAfter( const MCommand& after )
{
    _after = after;
    setDirty();
}

void MComponent::expand( WString& c, const MCommand& cmd )
{
    if( cmd.size() > 0 ) {
        cmd.expand( c, _target, _config->nilTool(), _mask, NULL, _mode );
        c.concat( "\n" );
        _project->insertBlanks( c );
    }
}

bool MComponent::writeCBR( bool mustExist )
{
    bool found_a_mbr = FALSE;
    if( mustExist ) found_a_mbr = TRUE;
    for( int i=0; i<_workFiles.count(); i++ ) {
        MWorkFile* w = (MWorkFile*)_workFiles[i];
        if( w->browseable() ) {
            WFileName browfile( _filename );
            browfile.setExt( ".cbr" );
            WFile brow;
            if( brow.open( browfile, OStyleWrite ) ) {
                WFileName tfile;
                _target->absResult( tfile );
                tfile.setExt( "dbr" );
                brow.printf( "d %s\n", (const char*)tfile );
                for( ; i<_workFiles.count(); i++ ) {
                    MWorkFile* w = (MWorkFile*)_workFiles[i];
                    if( w->browseable() ) {
                        w->item()->absResult( tfile );
                        tfile.setExt( "mbr" );
                        if( mustExist ) {
                            if( access( (const char*)tfile, F_OK ) == 0 )  {
                                // file must be there
                                brow.printf( "f %s\n", (const char*)tfile );
                                found_a_mbr = TRUE;
                            }
                        } else {
                            brow.printf( "f %s\n", (const char*)tfile );
                        }
                    }
                }
                brow.close();
            }
            if( found_a_mbr ) return( TRUE );
        }
    }
    return FALSE;
}

void MComponent::resetRuleRefs()
{
    _target->resetRuleRefs();
    for( int i=0; i<_items.count(); i++ ) {
        MItem* m = (MItem*)_items[i];
        m->resetRuleRefs();
    }
}

void MComponent::setDirty( bool dirty )
{
    _dirty = dirty;
    if( _dirty ) {
        setNeedsMake();
        _project->setDirty();
    }
}

void MComponent::setNeedsMake( bool needsMake )
{
    _needsMake = needsMake;
    if( _needsMake ) {
        _project->setNeedsMake();
    }
}

void MComponent::makeNames( const char* spec, WFileName& filename, WFileName& relname, WFileName& targ )
{
    relname = spec;
    relname.noPath( targ );
    relname.setExt( ".tgt" );
    filename = relname;
    filename.absoluteTo( _project->filename() );
}

bool MComponent::tryBrowse()
{
    bool rc = FALSE;
    initWorkFiles( _workFiles );

    for( int i=0; i<_workFiles.count(); i++ ) {
        MItem* m = ((MWorkFile*)_workFiles[i])->item();

        WFileName fn = m->component()->relFilename(); // target file name
        fn.setFName( m->fName() );
        fn.setExt( ".mbr" );

        if( access( (const char *)fn, F_OK ) == 0 ) {
            rc = TRUE;
            break;
        }
    }
    finiWorkFiles();
    return( rc );
}

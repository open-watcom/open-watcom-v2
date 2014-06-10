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


#include "wobjfile.hpp"
#include "mproject.hpp"
#include "mcompon.hpp"

static char _targetIdent[] = { "targetIdent" };

Define( MProject )

MProject::MProject( const char* filename )
    : _dirty( true )
    , _needsMake( true )
    , _recursing( false )
    , _checkout( NULL )
{
    if( filename ) {
        setFilename( filename );
        _before = _config->before();
        _after = _config->after();
    }
}

MProject::~MProject()
{
    _components.deleteContents();
}

#ifndef NOPERSIST
MProject* WEXPORT MProject::createSelf( WObjectFile& )
{
    return( new MProject( NULL ) );
}

void WEXPORT MProject::readSelf( WObjectFile& p )
{
    if( !_recursing ) {
        if( p.version() < 26 ) {
            p.readObject( &_filename );
        }
        _filename = p.filename();
//        _filename.toLower();
        setMakefile();
        if( p.version() > 26 ) {
            p.readObject( &_before );
            p.readObject( &_after );
        }
        int icount;
        p.readObject( &icount );
        for( int i=0; i<icount; i++ ) {
            WFileName tgtFilename;
            p.readObject( &tgtFilename );
            if( p.version() < 35 ) {
                tgtFilename.relativeTo( _filename );
            }
            if( !attachComponent( tgtFilename ) ) {
                //this line won't work; no view attached yet!
                //sayf( SayWarning, SayOk, "IDE Error: Unable to read target file '%s'", (const char*)tgtFilename );
            }
        }
        _dirty = false;
        _needsMake = true;
    }
}

bool MProject::tryOpenTargetFile( long version, bool try_checkout, MComponent* comp )
{
    WObjectFile ot( version );
    if( !ot.open( comp->filename(), OStyleWriteB ) ) {
        if( _checkout && try_checkout ) {
            if( sayf( SayInfo, SayOkCancel,
                "Target file '%s' is read-only. checkout? ",
                (const char*)comp->filename() ) == RetOk ) {
                if( (_RCSCli->*_checkout)( &(comp->filename()), _filename,
                    comp->filename() ) ) {
                    return( tryOpenTargetFile( version, false, comp ) );
                }
            }
        }
        sayf( SayWarning, SayOk, "IDE Error: Unable to write target file '%s'", (const char*)comp->filename() );
        return( false );
    } else {
        _recursing = true;
        ot.writeObject( _targetIdent );
        ot.writeObject( this );
        ot.writeObject( comp );
        _recursing = false;
        ot.close();
        return( true );
    }
}

void WEXPORT MProject::writeSelf( WObjectFile& p )
{
    if( !_recursing ) {
        p.writeObject( &_before );
        p.writeObject( &_after );
        int icount = _components.count();
        p.writeObject( icount );
        for( int i=0; i<icount; i++ ) {
            MComponent* comp = (MComponent*)_components[i];
            WFileName tgtFile( comp->relFilename() );
            p.writeObject( &tgtFile, FORCE );
            if( comp->isDirty() ) {
                if( !tryOpenTargetFile( p.version(), true, comp ) ) {
                    p.setObjOk( false );
                }
            }
        }
        _dirty = false;
    }
}
#endif

void MProject::setFilename( const WFileName& f )
{
    _filename = f;
    _filename.absoluteTo();
    setMakefile();
    _filename.setCWD();
    setDirty();
    updateAllViews();
}

void MProject::setMakefile()
{
    _makefile = _filename;
    _makefile.setExt( ".mk" );

    WFileName mkFile( _makefile );
    if( mkFile.needQuotes() ) {
        mkFile.addQuotes();
    }
    setenv( "_makefile", mkFile , 1 );
}

MComponent* MProject::addComponent( MComponent* comp )
{
    setDirty();
    return( (MComponent*)_components.add( comp ) );
}

MComponent* MProject::removeComponent( MComponent* comp )
{
    setDirty();
    return( (MComponent*)_components.removeSame( comp ) );
}

MComponent* MProject::attachComponent( WFileName& filename )
{
    MComponent* comp = NULL;
    WObjectFile ot;
    if( ot.open( filename, OStyleReadB ) ) {
        _recursing = true;
        if( ot.version() < 34 ) {
            ot.readObject( this );
            comp = (MComponent*)ot.readObject();
            comp->setRelFilename( filename );
            _components.add( comp );
            setDirty();
        } else {
            char ident[sizeof( _targetIdent ) + 1];
            ot.readObject( ident, sizeof( ident ) - 1 );
            if( !streq( ident, _targetIdent ) ) {
                //bad file format
            } else {
                ot.readObject( this );
                comp = (MComponent*)ot.readObject();
                comp->setRelFilename( filename );
                _components.add( comp );
                setDirty();
            }
        }
        _recursing = false;
        ot.close();
    }
    return( comp );
}

MComponent* MProject::findOldComponent( WFileName& fn )
{
    int icount = _components.count();
    for( int i=0; i<icount; i++ ) {
        MComponent* comp = (MComponent*)_components[i];
        if( comp->filename() == fn ) {
            return( comp );
        }
    }
    return( NULL );
}

MComponent* MProject::findComponent( WFileName& fn )
{
    int icount = _components.count();
    for( int i=0; i<icount; i++ ) {
        MComponent* comp = (MComponent*)_components[i];
        if( comp->relFilename() == fn ) {
            return( comp );
        }
    }
    return( NULL );
}

MItem* MProject::findSameResult( MItem* item, MComponent** compp )
{
    int icount = _components.count();
    for( int i=0; i<icount; i++ ) {
        MComponent* comp = (MComponent*)_components[i];
        MItem* m = comp->findSameResult( item );
        if( m ) {
            *compp = comp;
            return( m );
        }
    }
    return( NULL );
}

void MProject::refresh( bool autotrack_only )
{
    MComponent  *comp;

    int icount = _components.count();
    for( int i=0; i<icount; i++ ) {
        comp = (MComponent*)_components[i];
        if( !autotrack_only || comp->autotrack() ) {
            comp->refresh();
        }
    }
}

void MProject::resetRuleRefs()
{
    int icount = _components.count();
    for( int i=0; i<icount; i++ ) {
        ((MComponent*)_components[i])->resetRuleRefs();
    }
}

void MProject::setBefore( const MCommand& before )
{
    _before = before;
    setDirty();
}

void MProject::setAfter( const MCommand& after )
{
    _after = after;
    setDirty();
}

void MProject::insertBlanks( WString& s )
{
    WString ss;
    bool lineStart = true;
    for( size_t i=0; i<s.size(); i++ ) {
        if( lineStart && s[i] != ' ' && s[i] != '!' ) {
            ss.concat( ' ' );
        }
        ss.concat( s[i] );

        if( s[i] == '\n' ) {
            lineStart = true;
        } else {
            lineStart = false;
        }
    }
    s = ss;
}

void MProject::expand( ContFile& pmak, const MCommand& cmd, const char* head )
{
    if( cmd.size() > 0 ) {
        pmak.printf( "%s\n", head );
        MCommand c;
        cmd.expand( c, &_filename, _config->nilTool(), "*", NULL, 0 );
        insertBlanks( c );
        pmak.puts( c );
        pmak.puts( "\n" );
    }
}

bool MProject::makeMakeFile()
{
    bool ok = true;
    if( needsMake() || !_makefile.attribs() ) {
        _filename.setCWD();
        ContFile        pmak;
        if( !pmak.open( _makefile, OStyleWrite ) ) {
            ok = false;
        } else {
            expand( pmak, _before, ".before" );
            expand( pmak, _after, ".after" );
            pmak.puts( "project :" );
            int icount = _components.count();
            int i;
            for( i=0; i<icount; i++ ) {
                MComponent* comp = (MComponent*)_components[i];
                if( comp->rule()->ismakeable() ) {
                    WFileName fn;
                    comp->target()->absName( fn );
                    if( fn.needQuotes() ) {
                        fn.addQuotes();
                    }
                    pmak.puts( " " );
                    pmak.puts( fn );
                    fn.removeQuotes();
                }
            }
            pmak.puts( " .SYMBOLIC\n" );
            pmak.puts( "\n" );

            for( i=0; i<icount; i++ ) {
                MComponent* comp = (MComponent*)_components[i];
                if( comp->rule()->ismakeable() ) {
                    comp->addMakeFile( pmak );
                }
            }
            pmak.close();
            ok = pmak.ok();
        }
    }
    if( ok ) {
        int icount = _components.count();
        for( int i=0; i<icount; i++ ) {
            MComponent* comp = (MComponent*)_components[i];
            if( comp->rule()->ismakeable() ) {
                ok = ok & comp->makeMakeFile();
            }
        }
    }
    if( ok ) setNeedsMake( false );
    return( ok );
}

void MProject::setDirty( bool dirty )
{
    _dirty = dirty;
    if( _dirty ) {
        _needsMake = true;
    }
}

void MProject::setRCS( WObject *obj, CheckoutFunction co )
{
    _checkout = co;
    _RCSCli = obj;
}

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


#include "busyntc.h"
#include "dtvsrref.h"
#include "dbmgr.h"
#include "module.h"
#include "referenc.h"
#include "viewmgr.h"
#include "hotspots.h"
#include "icons.h"
#include "referenc.h"
#include "brwhelp.h"

DTVSourceRefs::DTVSourceRefs( const Symbol * sym )
        : HotWindowList( "" )
//------------------------------------------------
{
    WString str;
    _symbol = new Symbol( *sym );

    str.printf( "References to %s", _symbol->name() );
    setText( str );

    BusyNotice busy( "Loading..." );
    dbManager()->module()->findReferences( &_references, _symbol );
    for( int i = 0; i < _references.count(); ++i ) {
        Reference * ref = (Reference *) _references[ i ];
        WString *src_info = new WString;
        src_info->printf( "%s, line %lu(%u)",
                          ref->sourceFile(), ref->line(), ref->column() );
        _data.add( src_info );
    }

    onDblClick( this, (cbw) &DTVSourceRefs::showSource );
    onHotPress( this, (cbw) &DTVSourceRefs::showSource );
    reset();

    setIcon( DTSourceRefs );
}

DTVSourceRefs::~DTVSourceRefs()
//---------------------------
{
    delete _symbol;
    _data.deleteContents();
    _references.deleteContents();
}

bool DTVSourceRefs::contextHelp( bool is_active_win )
//---------------------------------------------------
{
    if( is_active_win ) {
        WBRWinBase::helpInfo()->sysHelpId( BRH_SOURCE_REFERENCES );
    }
    return( TRUE );
}

int DTVSourceRefs::count()
//------------------------
{
    return _references.count();
}

const char * DTVSourceRefs::getString( int index )
//------------------------------------------------
{
    return( *(WString *)_data[ index ] );
}

int DTVSourceRefs::getHotSpot( int, bool pressed )
//------------------------------------------------
{
    return (pressed) ? EditorHotDown : EditorHotUp;
}

void DTVSourceRefs::showSource( WWindow * )
//-----------------------------------------
{
    int             sel = selected();
    Reference *     ref;

    if( 0 <= sel && sel < _references.count() ) {
        ref = (Reference *) _references[ sel ];

        ref->editRef();
    }
}

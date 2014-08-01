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


#include "wfilenam.hpp"
#include "wresstr.hpp"
extern "C" {
    #include "clibext.h"
};
#include "wfiledlg.hpp"

static  const char          *FilterList = "All files(*.*)\0*.*\0\0";


WEXPORT WFileDialog::WFileDialog( WWindow* parent, const char *filter )
        : _parent( parent ) {
/***************************/

    _fileName = new char[WFileBufSize];
    makeDialog( filter );
}

const char REPCHAR = '\t';

WEXPORT WFileDialog::WFileDialog( WWindow* parent, unsigned res_idx )
        : _parent( parent ) {
/***************************/

    static char     buffer[512];
    WResStr         flt( res_idx );
    const char *    filter;
    unsigned        i = 0;

    filter = flt;

    do {
        if( filter[i] == REPCHAR ) {
            buffer[i] = '\0';
        } else {
            buffer[i] = filter[i];
        }
    } while( filter[i++] );

    _fileName = new char[WFileBufSize];
    makeDialog( buffer );
}


void WFileDialog::makeDialog( const char *filter ) {
/**************************************************/

    if( filter == NULL ) {
        filter = FilterList;
    }
    _ofn.filter_list = (char *)filter;
    _ofn.filter_index = 1;
    _ofn.file_name = _fileName;
    _ofn.max_file_name = WFileBufSize;
    _ofn.base_file_name = NULL;
    _ofn.max_base_file_name = 0;
    _ofn.title = _titleName;
    _ofn.flags = 0;
    _ofn.initial_dir = _dirName;
}


WEXPORT WFileDialog::~WFileDialog() {
/***********************************/

    delete [] _fileName;
}


void WFileDialog::init( const char *fn, const char *title ) {
/***********************************************************/

    if( title != NULL ) {
        _ofn.title = (char *)title;
    }
    if( fn == NULL ) {
        fn = "";
    }
    strcpy( _fileName, fn );
    WFileName f( fn );
    f.setFName( NULL );
    f.setExt( NULL );
    strcpy( _dirName, f );
}


const char * WEXPORT WFileDialog::getOpenFileName( const char *fn,
                                                   const char *title,
                                                   unsigned style ) {
/*******************************************************************/

    init( fn, title );
    _ofn.flags = style;
#if defined( __NT__ )
    _ofn.flags |= OFN_WANT_LAST_PATH;
#endif
    if( GUIGetFileName( _parent->handle(), &_ofn ) == OFN_RC_FILE_SELECTED ) {
#if defined( __NT__ )
        WFileName f( _fileName );
        WFileName path( _ofn.last_path );
        path.concat( "\\" );
        f.absoluteTo( path );
        GUIMemFree( _ofn.last_path );
        _ofn.last_path = NULL;
        strcpy( _fileName, f );
#endif
        return( _fileName );
    }
    return( NULL );
}


bool WEXPORT WFileDialog::getOpenFileName( WFileNameList &flist,
                                           const char *fn,
                                           const char *title,
                                           unsigned style ) {
/***********************************************************/

    init( fn, title );
    _ofn.flags = style | OFN_ALLOWMULTISELECT;
    if( GUIGetFileName( _parent->handle(), &_ofn ) == OFN_RC_FILE_SELECTED ) {
        flist.parseIn( _fileName );
#if defined( __WINDOWS__ ) || defined( __NT__ )
        if( flist.count() > 1 ) {
            WFileName path( flist.stringAt( 0 ) );
            path.concat( '\\' );
            for( int i = 1; i < flist.count(); ++i ) {
                flist.stringAt(i).absoluteTo( path );
            }
            flist.removeAt( 0 );
        }
#endif
        return( true );
    }
    return( false );
}


const char * WEXPORT WFileDialog::getSaveFileName( const char *fn,
                                                   const char *title,
                                                   unsigned style ) {
/*******************************************************************/

    _ofn.flags = style | OFN_ISSAVE;
    init( fn, title );
    if( GUIGetFileName( _parent->handle(), &_ofn ) == OFN_RC_FILE_SELECTED ) {
        return( _fileName );
    }
    return( NULL );
}

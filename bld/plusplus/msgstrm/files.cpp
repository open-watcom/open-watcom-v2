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


// WOBBLE file access functions........

#include <fstream.h>
#include <string.h>
#include <direct.h>

#include "files.h"
#include "msgstrm.h"
#include "inout.h"

ifstream MsgFile;
ifstream ModelFile;
ofstream DestFile;

void Purge( ios& strm ) {
//~~~~~~~~~~~~~~~~~~~~~

    strstreambuf *sb;

    sb = (strstreambuf*)strm.rdbuf();
    sb->freeze( 0 );
    sb->seekoff( 0, ios::beg, ios::out );
};

void Write( ofstream& of, strstream& text ) {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    of << text.str();
    (text.rdbuf())->freeze( 0 );
};

int WOpenForRead( ifstream& r_if, const char * s ) {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    r_if.open( s, ( ios::in | ios::nocreate ) );
    if( !r_if ) {
        r_if.clear( );
        return( 1 );
    } else {
        return( 0 );
    };
};

int WOpenForWrite( ofstream& r_if, const char * s ) {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    r_if.clear( );
    r_if.open( s, ios::out | ios::trunc );
    if( !r_if ) {
        r_if.clear( );
        return( 1 );
    } else {
        return( 0 );
    };
};

void WClose( ifstream& r_if ) {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~

    r_if.close( );
    r_if.clear( );
};

void WClose( ofstream& r_of ) {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~

    r_of.close( );
    r_of.clear( );
};

int WOpenDir( DIR *& p_dir, const char * s ) {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    DIR * p_tmp = opendir( s );
    if( p_tmp == NULL ) {
        return( 1 );
    } else {
        p_dir = p_tmp;
        return( 0 );
    };
};

void WCloseDir( DIR * p_dir ) {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~

    closedir( p_dir );
};

int WDirReadNext( DIR * p_dir, char * s ) {
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    char path[ 128 ];
    struct dirent * e_tmp = readdir( p_dir );
    if( e_tmp == NULL ) {
        return( NO_MORE_FILES );
    } else {
        strcpy( path, e_tmp->d_name );
        strcpy( s, path );
        return( 0 );
    };
};

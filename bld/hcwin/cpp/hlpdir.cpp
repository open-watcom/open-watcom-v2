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


/*
HLPDIR:  directory for the WinHelp "file system"
*/

#include <string.h>
#include "hlpdir.h"


#define HFSKEY_SIZE     13
#define HFSPAGE_SIZE        1024
#define FILE_HEADER_SIZE    9


//
//  HFSkey  --"key" data type for the directory.
//

class HFSkey : public BtreeData
{
protected:
    char        _name[HFSKEY_SIZE];

    // Overrides of the BtreeData virtual functions.
    virtual uint_32 size() { return strlen(_name)+1; };
    virtual int     dump( OutFile * dest );
    BtreeData       *myKey();
    bool            lessThan( BtreeData *other );

public:
    HFSkey( char const n[] ) { strcpy( _name, n ); };
};


//
//  HFSnode --"node" data type for the directory.
//

class   HFSnode : public HFSkey
{
    Dumpable    *_pointer;
    uint_32 _offset;

    uint_32 size() { return strlen(_name) + 1 + sizeof( uint_32 ); };
    int     dump( OutFile * dest );

public:
    HFSnode( Dumpable *file, char const n[] ) : HFSkey(n)
        { _pointer = file; _offset = 0; } ;

    friend class HFSDirectory;
};


//  HFSkey::dump    --Overrides BtreeData::dump.

int HFSkey::dump( OutFile *dest )
{
    dest->write( _name, 1, strlen( _name ) + 1 );
    return 1;
}


//  HFSkey::myKey   --Overrides BtreeData::myKey.

inline BtreeData *HFSkey::myKey()
{
    return new HFSkey( _name );
}


//  HFSkey::lessThan    --Overrides BtreeData::lessThan.

bool HFSkey::lessThan( BtreeData *other )
{
    HFSkey  *trueother = (HFSkey*) other;
    return( strcmp( _name, trueother->_name ) < 0 );
}


//  HFSnode::dump   --Overrides HFSkey::dump.

int HFSnode::dump( OutFile * dest )
{
    dest->write( _name, 1, strlen( _name ) + 1 );
    dest->write( _offset );
    return 1;
}


char const HFSDirectory::_dirMagic[Btree::_magNumSize] = {
                          0x3B, 0x29, 0x02, 0x04, 0x00,
                      0x04, 0x7A, 0x34, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00, 0x00, 0x00, 0x00,
                      0x00, 0x00
};


//  HFSDirectory::HFSDirectory

HFSDirectory::HFSDirectory( char const filename[] )
    : _output( filename ), _files( _dirMagic, HFSPAGE_SIZE )
{
    // empty
}


//  HFSDirectory::addFile   --Create a node for the file
//                and add it to the b-tree.

Dumpable *HFSDirectory::addFile( Dumpable *newfile, char const name[] )
{
    HFSnode *current= new HFSnode( newfile, name );
    _files.insert( current );
    return newfile;
}


//  HFSDirectory::dump  --Assign offset values to all the files,
//            in the b-tree, then dump everything.

void HFSDirectory::dump()
{
    BtreeIter   iterator( _files );
    BtreeData   *current = iterator.data();
    if( current == NULL )
        return;

    uint_32 direct_size = _files.size();
    uint_32 filesize = direct_size + 4*sizeof( uint_32 ) + FILE_HEADER_SIZE;

    HCStartOutput();

    // Calculate the size of the entire .HLP file.
    HFSnode *curnode;
    for( ; current != NULL; current = (++iterator).data() ) {
        curnode = (HFSnode*)current;
        curnode->_offset = filesize;
        filesize += curnode->_pointer->size();
        filesize += FILE_HEADER_SIZE;
    }

    // Write out the .HLP file header.
    static const uint_32    header[3] = {   0x00035F3F,
                        0x00000010,
                        0xFFFFFFFF
    };

    _output.write( header, sizeof( uint_32 ), 3 );
    _output.write( filesize );

    // Now dump the directory file itself.
    direct_size += FILE_HEADER_SIZE;
    _output.write( direct_size );
    direct_size -= FILE_HEADER_SIZE;
    _output.write( direct_size );
    _output.write( (uint_8)0x04 );    // WinHelp needs a 0x04 at this point.
    _files.dump( &_output );

    // Now dump the each of the files listed in the b-tree.
    uint_32 cursize;
    iterator.init();
    for( current = iterator.data(); current != NULL; current = (++iterator).data() ) {
        cursize = ((HFSnode*) current)->_pointer->size();
        cursize += FILE_HEADER_SIZE;
        _output.write( cursize );
        cursize -= FILE_HEADER_SIZE;
        _output.write( cursize );
        _output.write( (uint_8)0x00 );    // Again, keeping WinHelp happy.
        ((HFSnode*)current)->_pointer->dump( &_output );
    }
    HCDoneTick();

    return;
}

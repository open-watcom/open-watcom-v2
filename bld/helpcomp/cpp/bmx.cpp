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
BMX:  Image file handling
*/

#include <stdlib.h>
#ifdef __UNIX__
#include <unistd.h>
#else
#include <direct.h>
#endif
#include "bmx.h"
#include "compress.h"


// Magic numbers for the supported image formats.
#define BITMAP_MAGIC    0x4D42
#define SHG1_MAGIC  0x706C
#define SHG2_MAGIC  0x506C

//  Bitmap::Bitmap  --Check that the given file is a valid bitmap.

Bitmap::Bitmap( InFile *fp ) : Bmx( fp )
{
    _size = 0;

    uint_16 magic;
    uint_32 filesize;

    _fp->reset();
    _fp->readbuf( &magic, 1, sizeof( uint_16 ) );
    _fp->readbuf( &filesize, 1, sizeof( uint_32 ) );
    _fp->reset(0, SEEK_END);
    _fileSize = _fp->tell();

    if( magic != BITMAP_MAGIC ||
        filesize != _fileSize ){
    _isValidImage = 0;
    } else {
    _isValidImage = 1;
    }

    _fp->close();   // To conserve file handles.
}


//  Bitmap::size        --Override of Dumpable::size.  Reads in the
//            bitmap and does a test compression of the pixels.

#define BLOCK_SIZE  1024    // Arbitrary large value.

#define BIH_SIZE    0x28    // Size of a BitmapInfoHeader (see Windows
                // docs on bitmaps).

#define MIN_16BIT   0x80    // Smallest value stored in a short
#define MIN_32BIT   0x8000  // Smallest value stored in a long

#define OBJ_OFFSET  8   // Offset of the "object" in a .SHG file.

uint_32 Bitmap::size()
{
    if( _size != 0 ){
        return _size;
    }

    _fp->open();
    _fp->reset( 10 );   // size of unneeded header info.

    _fp->readbuf( &_pixOffset, 1, sizeof( uint_32 ) );
    _fp->readbuf( &_headSize, 1, sizeof( uint_32 ) );

    if( _headSize == BIH_SIZE ){

        // If _headSize==0x28, this bitmap uses a BitmapInfoHeader
        // and RBG quads.

        _fp->readbuf( &_width, 1, sizeof( uint_32 ) );
        _fp->readbuf( &_height, 1, sizeof( uint_32 ) );

        _fp->readbuf( &_planes, 1, sizeof( uint_16 ) );
        _fp->readbuf( &_bitsPerPix, 1, sizeof( uint_16 ) );

        _fp->readbuf( &_compression, 1, sizeof( uint_32 ) );
        _fp->readbuf( &_imageSize, 1, sizeof( uint_32 ) );
        _fp->readbuf( &_xpels, 1, sizeof( uint_32 ) );
        _fp->readbuf( &_ypels, 1, sizeof( uint_32 ) );
        _fp->readbuf( &_colsUsed, 1, sizeof( uint_32 ) );

        if( _colsUsed == 0 ){
            // We can calculate _colsUsed from _bitsPerPix, except
            // in the case of a 24-bit bitmap.
            if( _bitsPerPix < 24 ){
                _colsUsed = 1<<_bitsPerPix;
            }
        }

        _fp->readbuf( &_colsImportant, 1, sizeof(uint_32) );
        _dataPos = _fp->tell();
        _fp->reset( sizeof(uint_32)*_colsUsed, SEEK_CUR );

    } else {

        // The other case is _headSize==0x0C, meaning this bitmap
        // uses a BitmapCoreHeader and RGB triples.

        _width = 0;
        _fp->readbuf( &_width, 1, sizeof( uint_16 ) );
        _height = 0;
        _fp->readbuf( &_height, 1, sizeof( uint_16 ) );
        _fp->readbuf( &_planes, 1, sizeof( uint_16 ) );
        _fp->readbuf( &_bitsPerPix, 1, sizeof( uint_16 ) );

        // Again, calculate _colsUsed from _bitsPerPix, unless
        // this is a 24-bit bitmap.
        if( _bitsPerPix < 24 ){
            _colsUsed = 1<<_bitsPerPix;
        } else {
            _colsUsed = 0;
        }
        _colsImportant = 0;

        _dataPos = _fp->tell();
        _fp->reset( 3*_colsUsed, SEEK_CUR );
    }

    _objOffset = sizeof(uint_32)*_colsUsed + 0x1C; // 0x1C = size of object header

    if( _bitsPerPix >= MIN_16BIT ){
        _objOffset += 1;
    }
    if( _width >= MIN_32BIT ){
        _objOffset += 2;
    }
    if( _height >= MIN_32BIT ){
        _objOffset += 2;
    }
    if( _colsUsed >= MIN_32BIT ){
        _objOffset += 2;
    }

    // Calculate the size of the compressed pixel data.
    CompWriter  riter;
    CompReader  reader( &riter );
    char        *buffer = new char[BLOCK_SIZE];
    unsigned    blocksize;
    unsigned    count = _pixOffset;

    _pixSize = 0;
    while( count < _fileSize ){
        blocksize = _fp->readbuf( buffer, BLOCK_SIZE );
        count += blocksize;
        _pixSize += reader.compress( buffer, blocksize );
    }

    if( _pixSize >= MIN_32BIT ){
        _objOffset += 2;
    }
    _size = _objOffset+OBJ_OFFSET+_pixSize;

    delete[] buffer;

    _fp->close();
    return _size;
}


//  Bitmap::dump    --Overrides Dumpable::dump.  Writes the
//            converted image.

int Bitmap::dump( OutFile *dest )
{
    uint_16 magic = SHG1_MAGIC;
    uint_16 num_objs = 1;
    uint_32 offset = OBJ_OFFSET;
    uint_16 two_hund = 0x200;
    uint_32 big_zero = 0x00000000;

    dest->writebuf( &magic, sizeof( uint_16 ), 1 );
    dest->writebuf( &num_objs, sizeof( uint_16 ), 1 );
    dest->writebuf( &offset, sizeof( uint_32 ), 1 );
    dest->writech( 0x06 );      // Identifies this as a .BMP
    dest->writech( 0x02 );      // Specifies the compression type
    dest->writech( 0xC0 );      // Specifies 96dpi
    dest->writech( 0x00 );      // Magic
    dest->writech( 0xC0 );      // Specifies 96dpi again
    dest->writebuf( &two_hund, sizeof( uint_16 ), 1 );

    _bitsPerPix *= 2;
    if( _bitsPerPix >= 2*MIN_16BIT ){
        _bitsPerPix += 1;
        dest->writebuf( &_bitsPerPix, sizeof( uint_16 ), 1 );
    } else {
        dest->writech( *((uint_8*) &_bitsPerPix) );
    }

    _width *= 2;
    if( _width >= 2*MIN_32BIT ){
        _width += 1;
        dest->writebuf( &_width, sizeof( uint_32 ), 1 );
    } else {
        dest->writebuf( (uint_16*) &_width, sizeof( uint_16 ), 1 );
    }

    _height *= 2;
    if( _height >= 2*MIN_32BIT ){
        _height += 1;
        dest->writebuf( &_height, sizeof( uint_32 ), 1 );
    } else {
        dest->writebuf( (uint_16*) &_height, sizeof( uint_16 ), 1 );
    }

    _colsUsed *= 2;
    if( _colsUsed >= 2*MIN_32BIT ){
        _colsUsed += 1;
        dest->writebuf( &_colsUsed, sizeof( uint_32 ), 1 );
    } else {
        dest->writebuf( (uint_16*) &_colsUsed, sizeof( uint_16 ), 1 );
    }
    _colsUsed /= 2; // We need this quantity again later.

    dest->writebuf( (uint_16*) &big_zero, sizeof( uint_16 ), 1 );

    _pixSize *= 2;
    if( _pixSize >= 2*MIN_32BIT ){
        _pixSize += 1;
        dest->writebuf( &_pixSize, sizeof( uint_32 ), 1 );
    } else {
        dest->writebuf( (uint_16*) &_pixSize, sizeof( uint_16 ), 1 );
    }

    dest->writebuf( (uint_16*) &big_zero, sizeof( uint_16 ), 1 );
    dest->writebuf( &_objOffset, sizeof( uint_32 ), 1 );
    dest->writebuf( &big_zero, sizeof( uint_32 ), 1 );

    // now write the colour table and pixel data.
    uint_32 colour;

    _fp->open();
    _fp->reset( _dataPos );
    if( _headSize == BIH_SIZE ){
        for( unsigned i = 0; i < _colsUsed; i++ ){
            _fp->readbuf( &colour, 1, sizeof( uint_32 ) );
            dest->writebuf( &colour, sizeof( uint_32 ), 1 );
        }
    } else {
        colour = 0;
        for( unsigned i = 0; i < _colsUsed; i++ ){
            _fp->readbuf( &colour, 3 );
            dest->writebuf( &colour, sizeof( uint_32 ), 1 );
        }
    }

    CompOutFile riter( dest );
    CompReader  reader( &riter );
    char        *buffer = new char[BLOCK_SIZE];
    unsigned    blocksize;
    unsigned    count = _pixOffset;

    while( count < _fileSize ){
        blocksize = _fp->readbuf( buffer, BLOCK_SIZE );
        count += blocksize;
        reader.compress( buffer, blocksize );
    }

    delete[] buffer;
    reader.flush();

    _fp->close();

    return 1;
}


//  SegGraph::SegGraph

SegGraph::SegGraph( InFile * fp ) : Bmx( fp )
{
    uint_16 magic;

    fp->reset();
    fp->readbuf( &magic, 1, sizeof( uint_16 ) );
    if( magic == SHG1_MAGIC || magic == SHG2_MAGIC ){
    _isValidImage = 1;
    } else {
    _isValidImage = 0;
    }
    fp->reset( 0, SEEK_END );
    _size = fp->tell();
    fp->close();    // to preserve file handles.
}


//  SegGraph::Dump  --Overrides Dumpable::dump

int SegGraph::dump( OutFile *dest )
{
    char    *block = new char[BLOCK_SIZE];
    int     this_block;

    _fp->open();
    while( (this_block=_fp->readbuf(block, BLOCK_SIZE)) != 0 ){
    dest->writebuf( block, 1, this_block );
    }

    return 1;
}


//  HFBitmaps::HFBitmaps

HFBitmaps::HFBitmaps( HFSDirectory *d_file ) : _dfile(d_file)
{
    _root = NULL;
    _files = NULL;
    _usedFiles = NULL;

    _startDir = new char[80];   // Arbitrary-length large string
    getcwd( _startDir, 80 );

    _numImages = 0;
}


//  HFBitmaps::~HFBitmaps

HFBitmaps::~HFBitmaps()
{
    delete[] _startDir;

    StrNode *tempSN, *curSN = _root;

    while( curSN != NULL ){
    tempSN = curSN;
    curSN = curSN->_next;
    delete[] tempSN->_name;
    delete tempSN;
    }

    Image   *tempBM, *curBM = _files;

    while( curBM != NULL ){
    tempBM = curBM;
    curBM = curBM->_next;
    delete[] tempBM->_name;
    delete tempBM->_image;
    delete tempBM;
    }

    curBM = _usedFiles;
    while( curBM != NULL ){
    tempBM = curBM;
    curBM = curBM->_next;
    delete[] tempBM->_name;
    delete tempBM->_image;
    delete tempBM;
    }
}


//  HFBitmaps::addToPath  --Append new directories to the search path.

void HFBitmaps::addToPath( char const path[] )
{
    char const  *arg = path;
    if( arg == NULL ) return;

    StrNode *current, *temp;

    // Update the search paths.
    current = _root;
    if( current != NULL ){
    while( current->_next != NULL ){
        current = current->_next;
    }
    }

    // There may be many directories specified on each line.
    int j;
    while( arg[0] != '\0' ){
    j = 0;
    while( arg[j] != '\0' && arg[j] != ',' && arg[j] != ';' ){
        ++j;
    }
    temp = new StrNode;
    temp->_name = new char[ j+1 ];
    strncpy( temp->_name, arg, j );
    temp->_name[j] = '\0';
    temp->_next = NULL;
    if( chdir( temp->_name ) == 0 ){
        chdir( _startDir );
        if( current == NULL ){
        _root = temp;
        current = _root;
        } else {
        current->_next = temp;
        current = current->_next;
        }
    } else {
        HCWarning( HPJ_BADDIR, temp->_name );
        delete[] temp->_name;
        delete temp;
    }
    arg += j;
    if( arg[0] != '\0' ) ++arg;
    }
}


//  HFBitmaps::note --Create a node for an image, but don't use it.

void HFBitmaps::note( char const name[] )
{
    Image   *current;
    StrNode *curdir = _root;
    uint_16 magic;

    InFile  *bmp = new InFile( name, 1 );

    while( bmp->bad() && curdir != NULL ){
    chdir( curdir->_name );
    curdir = curdir->_next;
    bmp->open( name );
    chdir( _startDir );
    }
    if( bmp->bad() ){
    HCWarning( FILE_ERR, name );
    delete bmp;
    return;
    }
    bmp->reset();
    bmp->readbuf( &magic, 1, 2 );
    switch( magic ){
    case BITMAP_MAGIC:
    case SHG1_MAGIC:
    case SHG2_MAGIC:
    break;

    default:
    throw ImageNotSupported();  // EXCEPTION
    }
    bmp->reset();

    current = new Image;
    current->_name = new char[strlen(name)+1];
    strcpy( current->_name, name );
    current->_next = _files;
    _files = current;
    if( magic == BITMAP_MAGIC ){
    current->_image = new Bitmap(bmp);
    } else {
    current->_image = new SegGraph(bmp);
    }
    if( !current->_image->validImage() ){
    // Keep the bad image in the list for reference,
    // but warn the user.
    HCWarning( HLP_BADIMAGE, current->_name );
    }

    return;
}

//  HFBitmaps::use  --Return a reference to an image.

uint_16 HFBitmaps::use( char const name[] )
{
    uint_16 result;

    Image   *current, *newimage, *temp;
    StrNode *curdir;
    InFile  *bmp;

    static char filename[9] = "|bm";

    // Check to see if this bitmap has already been referenced.
    current = _usedFiles;
    result = (uint_16) 0;
    while( current != NULL && stricmp( name, current->_name ) != 0 ){
    result ++;
    current = current->_next;
    }
    if( current != NULL ){
    return result;
    }

    // Check to see if the bitmap was referenced by note().
    result = (uint_16) _numImages;

    current = _files;
    temp = NULL;
    while( current != NULL && stricmp( name, current->_name ) != 0 ){
    temp = current;
    current = current->_next;
    }

    newimage = _usedFiles;
    if( newimage != NULL ){
    while( newimage->_next != NULL ){
        newimage = newimage->_next;
    }
    }

    if( current != NULL ){

    if( !current->_image->validImage() ){
        throw ImageNotValid();  // EXCEPTION
    }

    if( temp != NULL ){
        temp->_next = current->_next;
    } else {
        _files = current->_next;
    }
    } else {
    // Now we have to search for the file.
    curdir = _root;
    bmp = new InFile( name, 1 );

    while( bmp->bad() && curdir != NULL ){
        chdir( curdir->_name );
        bmp->open( name, 1 );
        chdir( _startDir );
        curdir = curdir->_next;
    }

    if( bmp->bad() ){
        delete bmp;
        throw ImageNotFound();  // EXCEPTION
    } else {
        uint_16 magic;

        bmp->reset();
        bmp->readbuf( &magic, 1, sizeof( uint_16 ) );
        switch( magic ){
        case BITMAP_MAGIC:
        case SHG1_MAGIC:
        case SHG2_MAGIC:
        break;

        default:
        throw ImageNotSupported();  // EXCEPTION
        }
        bmp->reset();

        current = new Image;
        current->_name = new char[strlen(name)+1];
        strcpy( current->_name, name );
        if( magic == BITMAP_MAGIC ){
        current->_image = new Bitmap(bmp);
        } else {
        current->_image = new SegGraph(bmp);
        }
    }
    }

    current->_next = NULL;
    if( newimage != NULL ){
    newimage->_next = current;
    } else {
    _usedFiles = current;
    }

    ltoa( result, filename+3, 10 );
    _dfile->addFile( current->_image, filename );

    _numImages += 1;

    return result;
}

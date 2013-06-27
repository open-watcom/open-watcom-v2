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


//
//  bmx.h   --Classes to support the inclusion of images
//        in Windows helpfiles.
//        For the Watcom .HLP compiler.
//


#ifndef BMX_H
#define BMX_H

#include <watcom.h>
#include "myfile.h"
#include "hlpdir.h"


//
//  class Bmx       --Class to represent images in a .HLP file.
//

class Bmx : public Dumpable
{
    // Assignment of Bmx's is not permitted.
    Bmx( Bmx const & ){};
    Bmx &   operator=( Bmx const & ) { return *this; };

protected:
    InFile  *_fp;
    int     _isValidImage;
public:
    Bmx( InFile *fp ) : _fp(fp), _isValidImage(0) {};
    ~Bmx() { delete _fp; };

    int validImage() { return _isValidImage; };
};



//
//  class Bitmap    --Class to represent a .BMP and convert it into a
//            WinHelp-format image file.
//

class Bitmap : public Bmx
{
    uint_32 _fileSize;

    uint_32 _pixOffset;
    uint_32 _headSize;
    uint_32 _width;
    uint_32 _height;

    uint_16 _planes;
    uint_16 _bitsPerPix;

    uint_32 _compression;
    uint_32 _imageSize;

    uint_32 _xpels;
    uint_32 _ypels;
    uint_32 _colsUsed;
    uint_32 _colsImportant;

    uint_32 _objOffset;
    uint_32 _pixSize;

    long _dataPos;

    uint_32 _size;

public:

    Bitmap( InFile *fp );

    // Override the Dumpable virtual functions.
    uint_32 size();
    int     dump( OutFile * dest );
};


//
//  class SegGraph  --Class to import .SHG files into a .HLP file.
//

class SegGraph : public Bmx
{
    uint_32 _size;
public:
    SegGraph( InFile *fp );

    uint_32 size() { return _size; };
    int     dump( OutFile * dest );
};


//
//  class HFBitmaps --Class to handle the image facilities of
//            the .HLP compiler.

class HFBitmaps
{
    char *_startDir;

    struct StrNode
    {
    char    *_name;
    StrNode *_next;
    };

    StrNode *_root;

    struct Image
    {
    char    *_name;
    Bmx *_image;
    Image   *_next;
    };

    Image   *_files;
    Image   *_usedFiles;

    int     _numImages;

    HFSDirectory    *_dfile;

    // Assignment of HFBitmaps's is not permitted.
    HFBitmaps( HFBitmaps const & ){};
    HFBitmaps & operator=( HFBitmaps const & ) { return *this; };

public:
    HFBitmaps( HFSDirectory *d_file );
    ~HFBitmaps();

    // Add a path to the image file search path.
    void    addToPath( char const path[] );

    // Record the position of an image file for future reference.
    void    note( char const name[] );

    // Find an image file and return an index to it.
    uint_16 use( char const name[] );

    // EXCEPTIONS
    struct ImageNotSupported {};    // thrown by note, use
    struct ImageNotFound {};        // thrown by use
    struct ImageNotValid {};        // thrown by use
};

#endif

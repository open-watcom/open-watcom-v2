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


#ifndef _BRMFILE_HPP
#define _BRMFILE_HPP

/*  BrmFile:
      Class for read-only binary files with buffering.
*/

class BrmFile {
    public:
        BrmFile();
        ~BrmFile();

        virtual void    Open(char const filename[]);
        void    Close();

        int     operator!() { return _file==NULL; }

        int     GetChar() { return fgetc(_file); }
        void    PutBack( int c ) { ungetc( c, _file); }
        int     Read( void *dest, size_t size, size_t num )
            { return fread(dest,size,num,_file); }
        int     Skip( unsigned offset )
            { return fseek( _file, offset, SEEK_CUR ); }

        virtual int     Pos() { return ftell( _file ) - _start; }
        int             Size() { return _fileSize; }

    protected:
        static const int        _bufMax;

        FILE                    *_file;
        unsigned                _fileSize;
        unsigned                _start;
        char                    *_buf;
};

class BrmFilePch : public BrmFile {
    public:
        BrmFilePch();
        ~BrmFilePch();

        virtual void    Open(char const filename[]);
};

#endif

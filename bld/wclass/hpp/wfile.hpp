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


#ifndef wfile_class
#define wfile_class

#include "wfilenam.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef int Handle;

// Define open styles:
// -------------------

typedef int OpenStyle;

#define OStyleRead (O_RDONLY)
#define OStyleReadB (O_RDONLY|O_BINARY)
#define OStyleWrite (O_WRONLY|O_CREAT|O_TRUNC)
#define OStyleWriteB (O_WRONLY|O_CREAT|O_TRUNC|O_BINARY)

class WFile : public WObject {
    public:
        WEXPORT WFile();
        WEXPORT ~WFile();
        virtual bool WEXPORT open( const char* name, OpenStyle style=OStyleRead );
        virtual bool WEXPORT open( const char* name, const char* env, OpenStyle style=OStyleRead );
        virtual bool WEXPORT close();
        bool WEXPORT ok() { return _ok; }
        int lineCount() { return _lineCount; }
        WFileName& filename() { return _filename; }

        long WEXPORT getl();
        WString& WEXPORT getLine( WString& str );
        WString& WEXPORT gets( WString& str );
        void WEXPORT gets( char* str, int len );
        void WEXPORT gets_exact( char* str, int len ); // read EXACTLY len bytes
        char WEXPORT getch();
        void WEXPORT ungetch( char chr );
        bool WEXPORT eof() { return _eof; }

        bool WEXPORT putl( long n );
        bool WEXPORT puts( const char* str );
        bool WEXPORT putch( char ch );
        bool WEXPORT printf( const char* parms... );

        int WEXPORT read( char* buffer, int length );
        int WEXPORT write( const char* buffer, int length );
        long WEXPORT lseek( long offset, int org=0 );
    protected:
        virtual char getByte();
        virtual bool putByte( char ch );
    private:
        void freeBuffer();
        bool putBytes( const char* s, size_t len );
    private:
        WFileName       _filename;
        Handle          _handle;
        OpenStyle       _style;
        bool            _eof;
        bool            _ok;
        char            _chsave;
        bool            _chsaved;
        char*           _buffer;
        unsigned        _bOffset;
        unsigned        _bLength;
        int             _lineCount;
};

#endif

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


#ifndef __OPSTREAM_H__
#define __OPSTREAM_H__

#include <wstd.h>
#include <string.h>
#include <stdlib.h>
#include <wtptlist.hpp>
#include <wstring.hpp>

#include "opscancm.h"

typedef TemplateList<char *> StringList;

#define SECTOR_SIZE  512

class ScanStreamBase
{
public:
                ScanStreamBase( void );
        virtual ~ScanStreamBase() {}

                int         get();
                void        unget( int );
        virtual char *      name( void );
                bool        error( void );
        static  void        streamInit( const char * srchPaths );
protected:
        virtual void        refill( void );

                char _buffer[ SECTOR_SIZE + 1 ];
                int  _endPos;
                int  _currPos;

        static  StringList  _searchPaths;

                int  _line;

                bool _eof;
                bool _error;

private:
                int  _prevChar;
                int  _usePrev;
                bool _prevEOF;
};

class ScanStreamFile : public ScanStreamBase
{
public:
                ScanStreamFile( const char * fname );
               ~ScanStreamFile();
                char *      name( void );
protected:
                void refill( void );
private:
                const char *    _fileName;
                int             _handle;
};


class ScanStreamMem : public ScanStreamBase
{
public:
                ScanStreamMem( char * mem, int length );
               ~ScanStreamMem() {}
                char *      name( void );

protected:
                void refill( void );
private:
                char *  _memPtr;
                int     _offset;
                int     _end;
};

#endif // __OPSTREAM_H__

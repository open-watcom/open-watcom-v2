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


#ifndef __CMDPARSE_H__
#define __CMDPARSE_H__

#include <string.hpp>

#include "wbrdefs.h"

template <class Type> class WCValSList;

class CheckedBufferedFile;

class CommandParser
{
public:
                                    CommandParser( char * cmdLine,
                                                   bool optAllowed );
                                    ~CommandParser();

            const char *            database();
            const char *            searchPath();
            const char *            options();
            WCValSList<String> *    files() { return _files; }
            bool                    quiet();

protected:
            void                    parse();
            int                     scan();
            char                    getNextChar();
            void                    setCommand( const char * );
            void                    setDatabase( const char * );
            void                    setSearchPath( const char * );
            void                    setOptions( const char * );
            void                    syntaxError();

            void                    addFile( const char * file );
            void                    setExtension( String & str, const char * ext );

private:
            char *                  _cmdLine;
            CheckedBufferedFile *   _command;
            bool                    _optAllowed;

            String                  _token;
            char                    _lookAhead;

            String                  _searchPath;
            String                  _database;
            String                  _options;
            WCValSList<String> *    _files;
            bool                    _quiet;
};

#endif //__CMDPARSE_H__

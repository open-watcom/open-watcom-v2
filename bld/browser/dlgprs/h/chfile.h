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


#ifndef __CHFILE_H__

#include <sys/types.h>
#include <wstd.h>
#include "wio.h"
#include <string>
#include <vector>

// FileExcept -- indicate that an exceptional file condition has occured

class FileExcept {
public:
    enum Action {
        Open,
        Close,
        Read,
        Write,
        Seek,
        Tell,
        Stat,
    };

                    FileExcept( Action act, int error, const char * fn, const char * msg = NULL );
                    ~FileExcept();

    Action          _action;
    int             _error;     // errno
    char *          _fileName;
    char *          _message;   // possibly from strerror
};

//
// CheckedFile  provides low-level file access with error handling.  If
//              an error is detected, a CheckedFileExcept is thrown.
//              File opens are buffered, so if open fails on out of file
//              handles, an open file is closed and open tries again.
//

class CheckedFile {
public:
    enum Access {
        ReadBinary =    O_RDONLY | O_BINARY,
        ReadText =      O_RDONLY | O_TEXT,
        WriteBinary =   O_WRONLY | O_BINARY | O_CREAT | O_TRUNC,
        WriteText =     O_WRONLY | O_TEXT   | O_CREAT | O_TRUNC,
    };

    enum Permission {
        UserReadWrite = PMODE_RW,
    };

                    CheckedFile( const char * fileName );
    virtual         ~CheckedFile();

    virtual void    open( int access, int permission );
            void    close();    // no exceptions

    virtual int     read( void * buffer, int len );
    virtual void    write( const void * buffer, int len );
            void    puts( const char * str );
    virtual long    seek( long offset, int whence );
    virtual long    tell() const;

            int     readNString( std::string & );
            int     writeNString( std::string & );

            bool    operator== ( const CheckedFile& other ) const {  // for vector
                        return( this == &other );
                    }

            int     fstat( struct stat * buf );
            time_t  st_mtime();
            off_t   st_size();

    const   char *  getFileName() const { return _fileName; }
            void    setFileName( const char * fn );

protected:
    virtual void    reOpen();
    virtual void    privClose();

            char *  _fileName;

            int     _openAccess;            // access permission for open
            int     _openPermission;        // permission for open with write

            int     _handle;                // handle of the posix file
            long    _currOffset;            // current physical offset within the file
            bool    _isOpen;                // true if file physically open
            bool    _logOpen;               // true if logically open (user didn't close)

    static  std::vector<CheckedFile *>  *_openFiles;

private:
            void    addOpenFile( CheckedFile * );
            void    removeOpenFile( CheckedFile * );
};

extern char * BadWhenceMessage;

#define __CHFILE_H__
#endif


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


#ifndef __FILEINFO_H__
#define __FILEINFO_H__

#include <io.h>
#include <sys/stat.h>

#include "wtime.h"

class FileInfo {
public:
                    FileInfo( const char * fileName )
                        : _fileName( fileName ) { refresh(); }
                    ~FileInfo(){};

            dev_t   st_dev() { return (_status == 0) ? _statBuf.st_dev : -1; }
            time_t  st_mtime() { return (_status == 0) ? _statBuf.st_mtime : -1; }
            off_t   st_size() { return (_status == 0) ? _statBuf.st_size : -1; }

            void    refresh() { _status = stat( _fileName, &_statBuf ); }
            bool    exists() { return access( _fileName, F_OK ) == 0; }

            bool    getTime( WTime & t ) {
                        if( _status == 0 ) {
                            t.set( _statBuf.st_mtime );
                            return TRUE;
                        } else {
                            return FALSE;
                        }
                    }
            bool    getStatus() { return( _status == 0 ); }
    const   char *  name() { return _fileName; }

private:

            int             _status;
            const char *    _fileName;
            struct stat     _statBuf;
};

#endif

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


#ifndef __OPSCAN_H__
#define __OPSCAN_H__

#include <wstd.h>
#include <wtptlist.hpp>
#include "opscancm.h"
#include "opgram.h"

#define BufSize 161

class Scanner;
class ScanStreamBase;

typedef TemplateList<char *>    StringList;
typedef TemplateList<Scanner *> ScannerList;

class Scanner
{
public:
                Scanner( char * cmdLine, int len );
                Scanner( const char * fileName );
               ~Scanner();

                char *  getString( int index );
                void    setLookFor( LookForType );
                void    stutterOn( void );
                void    recoverTo( LookForType );

                int     getToken( YYSTYPE & lval );

                bool    error( const char * err );

        static  Scanner *CurrScanner( void );
        static  bool     NewFileScanner( char * fname );
        static  void     scanInit( void );
private:
                char             _buffer[ BufSize ];
                ScanStreamBase * _scanStream;
                int              _stutter;
                int              _lastToken;
                LookForType      _lookFor;

                bool             _recovering;

        static  StringList       _stringTable;

        static  ScannerList      ActiveScanners;
        static  int              CurrentScanner;
};

#endif // __OPSCAN_H__

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


#ifndef __SCANNER_H__

#include <wstd.h>

#include "scancm.h"

class CheckedBufferedFile;
template <class Type> class WCPtrOrderedVector;

class Scanner {
public:
                                        Scanner( const char * fileName );
                                        ~Scanner();

            bool                        error( char const * errStr );
            short                       getToken( YYSTYPE & lval );

            short                       tokenValue( const char *, YYSTYPE & lval );

            const char *                getString( int idx );
            const char *                getIdent( int idx );

private:
            int                         get();
            void                        gobble();

            bool                        isEOF();
            bool                        isSpecial();
            bool                        isQuote();
            bool                        isSpace();
            bool                        isDigit();
            bool                        isHexDigit();

            void                        readQuotedString( YYSTYPE & lval );
            void                        readDecimal( YYSTYPE & lval );
            void                        readHex( YYSTYPE & lval );

            /* ------- data ----------- */

            CheckedBufferedFile *       _file;
            WCPtrOrderedVector<char> *  _identifiers;
            WCPtrOrderedVector<char> *  _strings;

            int                         _current;       // this character

    static  const char * const          _SpecialCharacters;
};

#define __SCANNER_H__
#endif

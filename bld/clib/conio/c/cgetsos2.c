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


#include "variety.h"
#define INCL_SUB
#include <wos2.h>
#include "defwin.h"
#include <conio.h>
#include "qread.h"


_WCRTLINK char *cgets( char *s )
    {
        USHORT  len;
        STRINGINBUF stringin_buf;
        char    *p;

#ifdef DEFAULT_WINDOWING
        if( _WindowsStdin != 0 ) {              // Default windowing
            __qread( STDIN_FILENO, s + 2, *s - 1 );
            len = *s;
        } else {
#endif
            stringin_buf.cb = *s;
            KbdStringIn( s + 2, &stringin_buf, IO_WAIT, 0 );
            len = stringin_buf.cchIn + 1;       // Including null char
#ifdef DEFAULT_WINDOWING
        }
#endif
        p = s + 2;
        for(;;) {
            if( len <= 1 ) break;
            if( *p == '\r' || *p == '\0' ) break;
            ++p;
            --len;
        }
        *p = '\0';
        s[1] = p - s - 2;
        return( s + 2 );
    }

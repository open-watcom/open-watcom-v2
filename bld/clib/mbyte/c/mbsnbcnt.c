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
#include <mbstring.h>
#include "farfunc.h"



/****
***** Return the number of bytes in the first 'n' multibyte characters
***** of 'string', excluding the terminating NULL.
****/

_WCRTLINK size_t _NEARFAR(_mbsnbcnt,_fmbsnbcnt)( const unsigned char _FFAR *string, size_t n )
{
    size_t              bytes = 0;

    /*** Count the number of bytes ***/
    while( !_NEARFAR(_mbterm,_fmbterm)(string) && n>0 ) { /* cook until done */
        if( _ismbblead(*string) ) {             /* is it a lead byte? */
            if( string[1] != '\0' )             /* yes, is next char NULL? */
                bytes += _NEARFAR(_mbclen,_fmbclen)( string );  /* no, update 'bytes' */
            else
                return( bytes );                /* yes, return */
        }
        else
            bytes++;                            /* just single byte char */
        string = _NEARFAR(_mbsinc,_fmbsinc)( string );  /* next char */
        n--;                                    /* update counter */
    }

    return( bytes );
}

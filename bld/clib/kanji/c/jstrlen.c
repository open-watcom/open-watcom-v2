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
#include <jstring.h>


/**
*
*  Name:        jstrlen         ï∂éöêîÇìæÇÈ
*
*  Synopsis:    nm = jstrlen( s );
*
*               int nm;         ï∂éöêî
*               JSTRING s;      ï∂éöóÒ
*
*  Description: ï∂éöóÒÇÃï∂éöêîÇìæÇÈÅB
*
*  Returns:     ï∂éöóÒÇÃï∂éöêîÇï‘Ç∑ÅBèIí[ÇÃÇmÇtÇkÇkÇÕï∂éöêîÇ…ä‹Ç‹ÇÍÇ»Ç¢ÅB
*
*
*  Name:        jstrlen         compute the length of KANJI string
*
*  Synopsis:    nm = jstrlen( s );
*
*               int     nm;     length of KANJI string
*               JSTRING s;      pointer to the KANJI string
*
*  Description: The jstrlen function compute the length of string pointed to
*               by s with in KANJI letter.
*
*  Returns:     The jstrlen function returns the number of letters that precede
*               the terminating null character.
*
**/

_WCRTLINK size_t jstrlen( const JCHAR *s )
{
    size_t count = 0;
    JMOJI m;

    while( s = jgetmoji( s, &m ), m ) count++;
    return count;
}

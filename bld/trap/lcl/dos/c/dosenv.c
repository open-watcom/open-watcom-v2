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


#include <string.h>
#include <i86.h>
#include "tinyio.h"

extern unsigned       DbgPSP(void);

const char far *DOSEnvFind( char *src )
{
    const char  far *env;
    char        *p;
    char        c1;
    char        c2;

    env = MK_FP( *(unsigned far *)MK_FP( DbgPSP(), 0x2c ), 0 );
    do {
        p = src;
        do {
            c1 = *p++;
            c2 = *env++;
        } while( c1 == c2 && c1 != '\0' && c2 != '=' );
        if( c1 == '\0' && c2 == '=' )
            return( env );
        while( c2 != '\0' ) {
            c2 = *env++;
        }
    } while( *env != '\0' );
    return( NULL );
}

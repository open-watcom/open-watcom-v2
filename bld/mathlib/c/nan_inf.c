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
#include <string.h>


int __Nan_Inf( double value, char *buf )
    {
        union {
                double  x;
                unsigned short int ui[4];
        } u;
        char *p;

        u.x = value;
        if( (u.ui[3] & 0x7FF0) == 0x7FF0 ) {    /* NaN or Inf */
            p = buf;
            if( u.ui[3] & 0x8000 )  *p++ = '-';
            if( (u.ui[0] | u.ui[1] | u.ui[2] | (u.ui[3] & 0x000F)) == 0 ) {
                strcpy( p, "inf" );
            } else {
                strcpy( p, "nan" );
            }
            return( 1 );        /* indicate NaN or Inf */
        }
        return( 0 );            /* indicate not a Nan or Inf */
    }

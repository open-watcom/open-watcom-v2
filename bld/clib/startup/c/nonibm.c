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
#include <i86.h>
#include <stdlib.h>
#ifdef __WINDOWS_386__
    #include <windows.h>
#endif
#include "extender.h"
#include "nonibm.h"
#include "rtinit.h"

#define is_date_char( c )       (( c >= '0' && c <= '9' ) || c == '/' )
#define BIOS_DATE_LEN           (2+1+2+1+2) /* yy-mm-dd */

#define CHECK_IT                                                \
    {                                                           \
        int i;                                                  \
        unsigned num_ok;                                        \
                                                                \
        /* check for IBM BIOS revsion date in ROM */            \
        num_ok = 0;                                             \
        for( i = 0; i < BIOS_DATE_LEN; ++i ) {                  \
            if( is_date_char( p[i] ) ) {                        \
                ++num_ok;                                       \
            }                                                   \
        }                                                       \
        /* wishy-washy test for BIOS dates that */              \
        /* contain some garbage chars. */                       \
        /* Commodore PC60-40 has BIOS date "02/0(/88"). */      \
        return( num_ok < (BIOS_DATE_LEN / 2) );                 \
    }


int __is_nonIBM( void )
{
#if defined(__WARP__) || defined(__NT__)
    return( 0 );
#elif defined(__WINDOWS_386__)
    extern      unsigned short  __F000;
    char        _WCFAR *p;

    p = MK_FP( __F000, 0xfff5 );
    CHECK_IT
#elif defined(__WINDOWS__)
    extern char _WCFAR  _F000h[];
    char _WCFAR *       p;

    p = MK_FP( _F000h, 0xfff5 );
    CHECK_IT
#elif defined(__OS2_286__)
    char _WCFAR *p;

    if( _osmode != DOS_MODE ) return( 0 );
    p = MK_FP( 0xf000, 0xfff5 );
    CHECK_IT
#elif defined(__386__)
    char _WCFAR *p;

    if( _ExtenderRealModeSelector == 0 ) return( 0 );
    p = MK_FP( _ExtenderRealModeSelector, 0xffff5 );
    CHECK_IT
#elif defined(__DOS__)
    char _WCFAR *p;

    p = MK_FP( 0xf000, 0xfff5 );
    CHECK_IT
#else
    #error __is_nonIBM not configured for system
#endif
}



/****
***** If this module is linked in, the startup code will call this function,
***** which will initialize the __NonIBM global variable.
****/

_WCRTLINK int           __NonIBM = -1;  // start at -1 for debugging purposes
                                        // (-1 ==> not yet initialized)

static init_on_startup( void )
{
    __NonIBM = __is_nonIBM();
}


AXI( init_on_startup, INIT_PRIORITY_LIBRARY )

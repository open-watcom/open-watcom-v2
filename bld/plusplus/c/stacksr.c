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
* Description:  Safe recursion routine with explicit stack overflow check.
*
****************************************************************************/


#include "plusplus.h"
#include "stacksr.h"

#if defined( __WATCOMC__ ) && defined( _M_IX86 ) && !defined( __NT__ )

#include "walloca.h"

extern char __near      *bp( void );
#pragma aux bp = 0x89 0xe8 value [ eax ];

extern char __near      *sp( void );
#pragma aux sp = value [ esp ];

extern void             setsp( void __near * );
#pragma aux setsp = 0x89 0xc4 parm [ eax ] modify [ esp ];

extern void             setbp( void __near * );
#pragma aux setbp = 0x89 0xc5 parm [ eax ];

void *SafeRecurseCpp( func_sr rtn, void *arg )
/********************************************/
/* This code assumes NO parameters on the stack! */
{
    #define SAVE_SIZE   512     /* this must be smaller than the stack */

    void    *savearea;
    void    *retval;

    if( stackavail() < 0x2000 ) { /* stack getting low! */
        savearea = CMemAlloc( SAVE_SIZE );
        if( savearea != NULL ) {
            memcpy( bp(), savearea, SAVE_SIZE );
            memcpy( sp(), sp() + SAVE_SIZE, bp() - sp() );
            setbp( bp() + SAVE_SIZE );
            setsp( sp() + SAVE_SIZE );
            retval = rtn( arg );
            setsp( sp() - SAVE_SIZE );
            memcpy( sp() + SAVE_SIZE, sp(), bp() - sp() - SAVE_SIZE );
            setbp( bp() - SAVE_SIZE );
            memcpy( savearea, bp(), SAVE_SIZE );
            CMemFree( savearea );
            return( retval );
        }
    }
    return( rtn( arg ) );
}

#else

void    *SafeRecurseCpp( func_sr rtn, void *arg )
/***********************************************/
{
    return( rtn( arg ) );
}

#endif

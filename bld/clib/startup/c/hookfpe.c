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
#define INCL_DOSDEVICES
#define INCL_SUB
#define INCL_DOSSEMAPHORES
#define INCL_DOSINFOSEG
#define INCL_DOSMISC
#include <wos2.h>
#include <stdlib.h>

void __interrupt __far __FPEHandler();
#pragma aux __FPEHandler "*";
static void __interrupt (*old_FPE_handler)();

void __Init_FPE_handler()
/***********************/
    {
        char    devinfo;

        DosDevConfig( &devinfo, 3, 0 );
        if( getenv( "NO87" ) == NULL  &&  devinfo != 0 ) {
            DosSetVec( 16, (PFN)&__FPEHandler, (PFN FAR*)&old_FPE_handler );
        }
    }

void __Fini_FPE_handler()
/***********************/
    {
        if( old_FPE_handler != NULL ) {
            DosSetVec( 16, (PFN)old_FPE_handler, (PFN FAR*)&old_FPE_handler );
        }
    }

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


#include <stdui.h>
#include "biosui.h"
#include <i86.h>
#include "uidef.h"

#ifndef __WINDOWS__
extern void far * GetFarPointer( unsigned segment, unsigned offset );
#endif

static char                     high_res_screen = UNDEFINED;

/*
 * Code taken directly from the NEC graphics library.
 * The code is a combination of snippets from seginit.c and bios.h
 */
#define _BIOS_Seg 0x0040
#define _BIOS_Off 0x00
#define HIRES_FLAG 0x0101
#define HIRES_MASK 0x08

#if defined( __WINDOWS__ )
// IMPORTANT : the bios segment in windows is bimodal.
#define _BIOS_data( p ) MK_FP( _BIOS_Seg, _BIOS_Off + p )
#else
#define _BIOS_data( p ) GetFarPointer( _BIOS_Seg, _BIOS_Off + p )
#endif

bool _ishighres()
{
    char far * hires_mode_flag;

    hires_mode_flag = (char far *) _BIOS_data( HIRES_FLAG );
    if( *hires_mode_flag & HIRES_MASK ) {
        return( TRUE );
    } else {
        return( FALSE );
    }
}

bool uiIsHighRes()
{
    if( high_res_screen == UNDEFINED ) {
        if( _ishighres() ) {
            high_res_screen = TRUE;
        } else {
            high_res_screen = FALSE;
        }
    }
    return( high_res_screen );
}

unsigned int getcolumns()
{
    register    unsigned char           mode;
    if( uiIsHighRes() ) {
        return( 80 );
    } else {
        mode = BIOSGetMode();
        if ( mode & NEC_40_COLS ) {
            return( 40 );
        } else {
            return( 80 );
        }
    }
}

unsigned int getrows()
{
    register unsigned char      mode;
    mode = BIOSGetMode();
    if( uiIsHighRes() ) {
        if( mode & NEC_31_LINES ) {
            return( 31 );
        } else {
            return( 25 );
        }
    } else {
        if( mode & NEC_20_LINES ) {
            return( 20 );
        } else {
            return( 25 );
        }
    }
}

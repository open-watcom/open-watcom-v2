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


/*
 * Notes concerning the NEC windows character mode UI library -
 * ----------------------------------------------------------
 *
 *      We're going to have to keep in an intermediate virtual
 *      screen layer and use the physupdate function to get
 *      text onto the screen, since the UI library considers
 *      all screens to be of the form character attribute
 *      character attribute etc...
 *      ... which is not true for the NEC.  Typical Screenism.
 */

#include <stdlib.h>
#include <dos.h>
#include <windows.h>
#include "uidos.h"
#include "biosui.h"
#include "uidef.h"

static          MONITOR                 ui_data         =       {
                25,
                80,
                M_NEC_NORM,
                NULL,
                NULL,
                NULL,
                NULL,
                4,
                1
};

static HANDLE           hVirtualScreen;
static void far *       pVirtualScreen;
static unsigned         screen_mem;
WORD FAR *              WindowsKernelPointerTo_A000h;
WORD FAR *              WindowsKernelPointerTo_A200h;

extern void ToCharacter( void );
extern void ToGraphical( void );

#ifdef _ALLOC_GLOBAL
#define _ALLOCATE( x ) GlobalAlloc( GPTR, x )
#define _LOCK( x ) GlobalLock( x );
#define _FINI( x ) GlobalUnlock( x ); GlobalFree( x )
#else
#define _ALLOCATE( x ) LocalAlloc( LMEM_FIXED, x )
#define _LOCK( x ) LocalLock( x );
#define _FINI( x ) LocalUnlock( x ); LocalFree( x )
#endif

// from uiscreen.c in necdos\c
extern bool             uiIsHighRes();
extern unsigned int     getcolumns();
extern unsigned int     getrows();

bool intern initmonitor()
/***********************/
{
    if( UIData == NULL ) {
        UIData = &ui_data;
    }

    UIData->width = getcolumns();
    UIData->height = getrows();

    UIData->colour = ( ( uiIsHighRes() ) ? M_NEC_HIRES : M_NEC_NORM );

    return( TRUE );
}

extern void far HookRtn( unsigned , unsigned );

static int SetupVideoPointers()
{
    FARPROC tmpp;

    if( uiIsHighRes() ) {
        tmpp = GetProcAddress(GetModuleHandle("KERNEL"), "__E000h");
    } else {
        tmpp = GetProcAddress( GetModuleHandle( "KERNEL" ), "__A000h" );
    }

    if( tmpp == NULL ) return( TRUE );
    screen_mem = LOWORD( (DWORD)tmpp );

    WindowsKernelPointerTo_A000h = MK_FP( screen_mem, 0 );
    WindowsKernelPointerTo_A200h = MK_FP( screen_mem, 0x2000 );
    return( FALSE );
}

int intern initbios()
/*******************/
{
    int                                 initialized;
    int                                 screen_size;

    initialized = TRUE;

    if( !initmonitor() ) initialized = FALSE;
    if( initialized ) {
        UIData->desqview = FALSE;
        UIData->f10menus = TRUE;

        /*
         * What we want to do with the virtual screen is identical to the
         * Dos version - we want to allocate a big chunk of memory and
         * use that for the virtual information, then write to screen memory
         * using physupdate. The only trick - we have to allocate the
         * memory through windows.
         */
        screen_size = ( UIData->height * UIData->width ) * sizeof( PIXEL );
        hVirtualScreen = _ALLOCATE( screen_size );
        if( hVirtualScreen == NULL ) initialized = FALSE;
    }
    if( initialized ) {
        if( SetupVideoPointers() ) initialized = FALSE;
    }
    if( initialized ) {
        pVirtualScreen = _LOCK( hVirtualScreen );
        if( pVirtualScreen == NULL ) initialized = FALSE;
    }
    /*
     * If we got this far, we can assume ( I hope ) that we have a pile of
     * memory to be used as the virtual screen, fixed in position,
     * with a handle and a pointer.
     */
    if( initialized ) {
        UIData->screen.origin = pVirtualScreen;
        UIData->screen.increment = UIData->width;
        uiinitcursor();
        initkeyboard();

        UIData->mouse_acc_delay = uiclockdelay( 278 );
        UIData->mouse_rpt_delay = uiclockdelay( 56 );
        UIData->mouse_clk_delay = uiclockdelay( 278 );
        UIData->tick_delay = uiclockdelay( 500 );
        UIData->mouse_speed = 8;           /* mickeys to ticks ratio */
    }
    return( initialized );
}

unsigned global uiclockdelay( unsigned milli )
{
    /* this routine converts milli-seconds into platform  */
    /* dependant units - used to set mouse & timer delays */
    return( milli );
}

void intern finibios()
/********************/
{
    /*
     * Deallocate virtual screen memory...
     */
    _FINI( hVirtualScreen );

    uifinicursor();
    finikeyboard();
}

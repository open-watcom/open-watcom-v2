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
* Description:  Command line parsing for Windows debugger.
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbgdata.h"
#include "dosscrn.h"


extern unsigned         Lookup(char *,char *, unsigned);
extern void             ForceLines( unsigned );
extern bool             HasEquals( void );
extern unsigned         GetValue( void );
extern unsigned long    GetMemory( void );
extern void             StartupErr( char * );

extern bool             WantFast;
extern flip_types       FlipMech;
extern mode_types       ScrnMode;
extern int              ScrnLines;

static char SysOptNameTab[] = {
    "Monochrome\0"
    "Color\0"
    "Colour\0"
    "Ega43\0"
    "FAstswap\0"
    "Vga50\0"
    "Overwrite\0"
    "Page\0"
    "Swap\0"
    "Two\0"
    "NOSNow\0"
    "Popups\0"
};

enum { OPT_MONO = 1,
    OPT_COLOR,
    OPT_COLOUR,
    OPT_EGA43,
    OPT_FASTSWAP,
    OPT_VGA50,
    OPT_OVERWRITE,
    OPT_PAGE,
    OPT_SWAP,
    OPT_TWO,
    OPT_NOSNOW,
    OPT_POPUPS
};

static void GetLines( void )
{
    if( HasEquals() ) {
        ForceLines( GetValue() );
    }
}


bool OptDelim( char ch )
{
    return( ch == '/' || ch == '-' );
}


static void SetEGA43( void )
{
    FlipMech = FLIP_SWAP;
    ScrnMode = MD_EGA;
    ScrnLines = 43;
}

static void SetVGA50( void )
{
    FlipMech = FLIP_SWAP;
    ScrnMode = MD_EGA;
    ScrnLines = 50;
}

void SetNumLines( int num )
{
    if( num >= 43 ) {
        if( num >= 50 ) {
            SetVGA50();
        } else {
            SetEGA43();
        }
    }
}

void SetNumColumns( int num )
{
    num=num;
}

/*
 * ProcSysOption -- process system option
 */

bool ProcSysOption( char *start, unsigned len, int pass )
{
    pass=pass;
    switch( Lookup( SysOptNameTab, start, len ) ) {
    case OPT_COLOR:
    case OPT_COLOUR:
        GetLines();
        break;
    case OPT_MONO:
        GetLines();
        break;
    case OPT_FASTSWAP:
        WantFast = true;
        break;
    case OPT_EGA43:
        SetEGA43();
        break;
    case OPT_VGA50:
        SetVGA50();
        break;
    case OPT_OVERWRITE:
    case OPT_PAGE:
    case OPT_SWAP:
        FlipMech = FLIP_SWAP;
        break;
    case OPT_TWO:
        FlipMech = FLIP_TWO;
        break;
    case OPT_NOSNOW:
        break;
    case OPT_POPUPS:
        _SwitchOn( SW_DETACHABLE_WINDOWS );
        break;
    default:
        return( FALSE );
    }
    return( TRUE );
}


/*
 * ProcSysOptInit -- initial system options
 */

void ProcSysOptInit( void )
{
    ScrnMode = MD_DEFAULT;
    FlipMech = FLIP_TWO;
}

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


#include "dbgdefn.h"
#include "farptrs.h"
#include "dbgscrn.h"
#include "dbgio.h"
#include "dbgreg.h"
#include "dbgname.h"
#include "dbgswtch.h"
#include <string.h>
#include <ctype.h>


extern unsigned         Lookup(char *,char *, unsigned);
extern void             ForceLines( unsigned );
extern bool             HasEquals( void );
extern unsigned         GetValue( void );
extern unsigned long    GetMemory( void );
extern void             StartupErr( char * );


extern flip_types       FlipMech;
extern mode_types       ScrnMode;
extern addr_seg         _psp;
extern unsigned         CheckSize;
extern unsigned         OvlAreaSize;


static char SysOptNameTab[] = {
    'M','o','n','o','c','h','r','o','m','e',NULLCHAR,
    'C','o','l','o','r',NULLCHAR,
    'C','o','l','o','u','r',NULLCHAR,
    'E','g','a','4','3',NULLCHAR,
    'V','g','a','5','0',NULLCHAR,
    'O','v','e','r','w','r','i','t','e',NULLCHAR,
    'P','a','g','e',NULLCHAR,
    'S','w','a','p',NULLCHAR,
    'T','w','o',NULLCHAR,
    'N','O','S','N','o','w',NULLCHAR,
    'C','H','e','c','k','s','i','z','e',NULLCHAR,
    'S','I','z','e',NULLCHAR,
    NULLCHAR
};

enum { OPT_MONO = 1, OPT_COLOR, OPT_COLOUR, OPT_EGA43, OPT_VGA50,
       OPT_OVERWRITE, OPT_PAGE, OPT_SWAP, OPT_TWO,
       OPT_NOSNOW,
       OPT_CHECKSIZE,
       OPT_SIZE
};

static  unsigned    CmdStart;

static void GetLines()
{
    unsigned    num;

    if( HasEquals() ) {
        num = GetValue();
        if( num < 10 || num > 999 ) StartupErr( "lines out of range" );
        ForceLines( num );
    }
}


bool OptDelim( char ch )
{
    return( ch == '/' || ch == '-' );
}


/*
 * ProcSysOption -- process system option
 */

bool ProcSysOption( char *start, unsigned len )
{
    unsigned long   num;

    switch( Lookup( SysOptNameTab, start, len ) ) {
    case OPT_MONO:
        ScrnMode = MD_MONO;
        GetLines();
        break;
    case OPT_COLOR:
    case OPT_COLOUR:
        ScrnMode = MD_COLOUR;
        GetLines();
        break;
    case OPT_EGA43:
    case OPT_VGA50:
        ScrnMode = MD_EGA;
        break;
    case OPT_OVERWRITE:
        FlipMech = FLIP_OVERWRITE;
        break;
    case OPT_PAGE:
        FlipMech = FLIP_PAGE;
        break;
    case OPT_SWAP:
        FlipMech = FLIP_SWAP;
        break;
    case OPT_TWO:
        FlipMech = FLIP_TWO;
        break;
    case OPT_NOSNOW:
        _SwitchOn( SW_NOSNOW );
        break;
    case OPT_CHECKSIZE:
        num = GetMemory();
        if( num < 32*1024UL ) num = 32*1024UL;
        if( num > 640*1024UL ) num = 640*1024UL;
        CheckSize = num / 16; /* Checksize is in paragraphs */
        break;
    case OPT_SIZE:
        num = GetMemory();
        if( num > 150*1024UL ) num = 150*1024UL;
        OvlAreaSize = num / 16; /* OvlAreaSize is in paragraphs */
        break;
    default:
        return( FALSE );
    }
    return( TRUE );
}


/*
 * ProcSysOptInit -- initial system options
 */

void ProcSysOptInit()
{
    char        far *ptr;
    unsigned    len;

    CheckSize = 0;
    ScrnMode = MD_DEFAULT;
    FlipMech = FLIP_TWO;
    OvlAreaSize = 100UL * 1024U / 16U;

    ptr = MK_FP( _psp, 0x80 );
    len = *ptr;
    ptr[ len + 1 ] = NULLCHAR;
    CmdStart = 0x81;
}


char far *GetCmdArg( int num )
{
    if( num != 0 || CmdStart == 0 ) return( NULL );
    return( MK_FP( _psp, CmdStart ) );
}

void SetCmdArgStart( int num, char far *ptr )
{
    num = num; /* must be zero */
    CmdStart = FP_OFF( ptr );
}

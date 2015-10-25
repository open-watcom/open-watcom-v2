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


#include <ctype.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dosscrn.h"
#include "dbgio.h"
#include "dbgname.h"
#include "dbgswtch.h"
#include "dbgcmdln.h"
#include "dbglkup.h"


extern void             StartupErr( const char * );


extern addr_seg         _psp;
extern unsigned         OvlAreaSize;


static const char SysOptNameTab[] = {
    "NOSNow\0"
    "CHecksize\0"
    "SIze\0"
};

enum { 
    OPT_NOSNOW,
    OPT_CHECKSIZE,
    OPT_SIZE
};

static  unsigned    cmdStart;

bool OptDelim( char ch )
{
    return( ch == '/' || ch == '-' );
}


/*
 * ProcSysOption -- process system option
 */

bool ProcSysOption( const char *start, unsigned len, int pass )
{
    unsigned long   num;

    pass=pass;
    switch( Lookup( SysOptNameTab, start, len ) ) {
    case OPT_NOSNOW:
        _SwitchOn( SW_NOSNOW );
        break;
    case OPT_CHECKSIZE:
        num = GetMemory();
        if( num < 32*1024UL )
            num = 32*1024UL;
        if( num > 640*1024UL )
            num = 640*1024UL;
        CheckSize = num / 16; /* Checksize is in paragraphs */
        break;
    case OPT_SIZE:
        num = GetMemory();
        if( num > 150*1024UL )
            num = 150*1024UL;
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

void ProcSysOptInit( void )
{
    char        __far *ptr;
    unsigned    len;

    CheckSize = 0;
    OvlAreaSize = 100UL * 1024U / 16U;

    ptr = MK_FP( _psp, 0x80 );
    len = *ptr;
    ptr[ len + 1 ] = NULLCHAR;
    cmdStart = 0x81;
}


char __far *GetCmdArg( int num )
{
    if( num != 0 || cmdStart == 0 ) return( NULL );
    return( MK_FP( _psp, cmdStart ) );
}

void SetCmdArgStart( int num, char __far *ptr )
{
    num = num; /* must be zero */
    cmdStart = FP_OFF( ptr );
}

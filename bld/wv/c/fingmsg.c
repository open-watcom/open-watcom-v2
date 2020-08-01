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
* Description:  Open Watcom Debugger certificate of authenticity.
*
****************************************************************************/


#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbgwind.h"
#include "dbgmem.h"
#include "strutil.h"
#include "fingmsg.h"


#define WHOLE_SIZE      ArraySize( AboutMessage )

static const char * const AboutMessage[] = {
    "Open Watcom Debugger" STR_BITNESS,
    banner1v( _WD_VERSION_ ) ".",
    banner2,
    banner2a( 1987 ),
    banner3,
    banner3a,
/* NB: it is disabled due to incompatibility with DOS/4G 2.x */
#if 0
#ifdef D32_NAME
    "",
    D32_NAME " " D32_VERSION,
    D32_COPYRIGHT,
#endif
#endif
#ifdef INTERNAL
    "",
    "** For internal WATCOM use only **",
    "Created " __DATE__ ", " __TIME__,
#endif
#if !defined( GUI_IS_GUI )
#define BOLT_SIZE 11
"",
"             ________             ",
"            /:::::::/             ",
"           /:::::::/__            ",
"          /::::::::::/            ",
"          -----/::::/__           ",
"              /......./           ",
"             -----/./             ",
"                /./               ",
"               //                 ",
"              /                   ",
#else
#define BOLT_SIZE 0
#endif
};

/* NB: DOS4GOPTIONS export is disabled due to incompatibility with DOS/4G 2.x */
#if 0
#ifdef __DOS__
const char DOS4GOPTIONS[] =
        "[dos4g-global]\n"
        "Include=WDOPTS.INI\n"
        "[dos4g-kernel]\n"
        "StartupBanner=FALSE\n"
;
#endif
#endif

void InitAboutMessage( void )
{
}

void FiniAboutMessage( void )
{
}

const char *GetAboutMessage( int line )
{
    if( line < WHOLE_SIZE )
        return( AboutMessage[line] );
    return( NULL );
}

int GetAboutSizeFull( void )
{
    return( WHOLE_SIZE );
}

int GetAboutSizeText( void )
{
    return( WHOLE_SIZE - BOLT_SIZE );
}

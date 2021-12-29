/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Linker Help stuff.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "linkstd.h"
#include "msg.h"
#include "cmdutils.h"
#include "wlnkmsg.h"
#include "cmdline.h"
#include "fileio.h"
#include "cmdhelp.h"
#include "ideentry.h"


#define _GENERAL_HELP   MSG_GENERAL_HELP_0, MSG_GENERAL_HELP_51
#define _DOS_HELP       MSG_DOS_HELP_0,     MSG_DOS_HELP_15
#define _OS2_HELP       MSG_OS2_HELP_0,     MSG_OS2_HELP_31
#define _WINDOWS_HELP   MSG_WINDOWS_HELP_0, MSG_WINDOWS_HELP_31
#define _WINVXD_HELP    MSG_WIN_VXD_HELP_0, MSG_WIN_VXD_HELP_31
#define _NT_HELP        MSG_NT_HELP_0,      MSG_NT_HELP_31
#define _PHARLAP_HELP   MSG_PHAR_HELP_0,    MSG_PHAR_HELP_15
#define _NOVELL_HELP    MSG_NOVELL_HELP_0,  MSG_NOVELL_HELP_31
#define _DOS16M_HELP    MSG_DOS16_HELP_0,   MSG_DOS16_HELP_15
#define _QNX_HELP       MSG_QNX_HELP_0,     MSG_QNX_HELP_15
#define _ELF_HELP       MSG_ELF_HELP_0,     MSG_ELF_HELP_15
#define _ZDOS_HELP      MSG_ZDOS_HELP_0,    MSG_ZDOS_HELP_15
#define _RDOS_HELP      MSG_RDOS_HELP_0,    MSG_RDOS_HELP_15
#define _RAW_HELP       MSG_RAW_HELP_0,     MSG_RAW_HELP_15

#define HELP_ARGS(x)            _ ## x ## _HELP
#define WRITE_HELP_ONE(x)       WriteHelp( HELP_ARGS(x), false )
#define WRITE_HELP_FULL(x)      WriteHelp( HELP_ARGS(x), true )

static bool previous_null = false;

static void doWriteHelp( int first_msg, int last_msg )
/****************************************************/
{
    char        msg_buffer[RESOURCE_MAX_SIZE];
    int         msg;
    bool        ok;

    for( msg = first_msg; msg <= last_msg; msg++ ) {
        ok = Msg_Get( msg, msg_buffer );
        if( !ok ) {
            if( previous_null ) {
                break;
            }
            previous_null = true;
        } else if( previous_null ) {
            WriteStdOutWithNL( msg_buffer );
            previous_null = false;
        } else {
            WriteStdOutWithNL( msg_buffer );
        }
    }
}

static void WriteHelp( int first_msg, int last_msg, bool genhelp )
/****************************************************************/
{
    if( genhelp || ( first_msg == MSG_GENERAL_HELP_0 && last_msg == MSG_GENERAL_HELP_51 ) ) {
        WLPrtBanner();
        if( genhelp ) {
            doWriteHelp( _GENERAL_HELP );
        }
    }
    doWriteHelp( first_msg, last_msg );
}

#ifdef _EXE
static bool ProcDosHelp( void )
/*****************************/
{
    WRITE_HELP_FULL( DOS );
    return( true );
}
#endif
#ifdef _OS2
static bool ProcOS2Help( void )
/*****************************/
{
    WRITE_HELP_FULL( OS2 );
    return( true );
}

static bool ProcWindowsHelp( void )
/*********************************/
{
    WRITE_HELP_FULL( WINDOWS );
    return( true );
}

static bool ProcWinVxdHelp( void )
/*********************************/
{
    WRITE_HELP_FULL( WINVXD );
    return( true );
}

static bool ProcNTHelp( void )
/****************************/
{
    WRITE_HELP_FULL( NT );
    return( true );
}
#endif
#ifdef _PHARLAP
static bool ProcPharHelp( void )
/******************************/
{
    WRITE_HELP_FULL( PHARLAP );
    return( true );
}
#endif
#ifdef _NOVELL
static bool ProcNovellHelp( void )
/********************************/
{
    WRITE_HELP_FULL( NOVELL );
    return( true );
}
#endif
#ifdef _DOS16M
static bool Proc16MHelp( void )
/*****************************/
{
    WRITE_HELP_FULL( DOS16M );
    return( true );
}
#endif
#ifdef _QNX
static bool ProcQNXHelp( void )
/*******************************/
{
    WRITE_HELP_FULL( QNX );
    return( true );
}
#endif

#ifdef _ELF
static bool ProcELFHelp( void )
/*******************************/
{
    WRITE_HELP_FULL( ELF );
    return( true );
}
#endif

#ifdef _ZDOS
static bool ProcZdosHelp( void )
/*****************************/
{
    WRITE_HELP_FULL( ZDOS );
    return( true );
}
#endif

#ifdef _RDOS
static bool ProcRdosHelp( void )
/*****************************/
{
    WRITE_HELP_FULL( RDOS );
    return( true );
}
#endif

#ifdef _RAW
static bool ProcRawHelp( void )
/*****************************/
{
    WRITE_HELP_FULL( RAW );
    return( true );
}
#endif

static  parse_entry FormatHelp[] = {
#ifdef _EXE
    "Dos",          ProcDosHelp,            MK_ALL,     0,
#endif
#ifdef _OS2
    "OS2",          ProcOS2Help,            MK_ALL,     0,
    "WINdows",      ProcWindowsHelp,        MK_ALL,     0,
    "VXD",          ProcWinVxdHelp,         MK_ALL,     0,
    "NT",           ProcNTHelp,             MK_ALL,     0,
#endif
#ifdef _PHARLAP
    "PHARlap",      ProcPharHelp,           MK_ALL,     0,
#endif
#ifdef _NOVELL
    "NOVell",       ProcNovellHelp,         MK_ALL,     0,
#endif
#ifdef _DOS16M
    "DOS16M",       Proc16MHelp,            MK_ALL,     0,
#endif
#ifdef _QNX
    "QNX",          ProcQNXHelp,            MK_ALL,     0,
#endif
#ifdef _ELF
    "ELF",          ProcELFHelp,            MK_ALL,     0,
#endif
#ifdef _ZDOS
    "ZDos",         ProcZdosHelp,           MK_ALL,     0,
#endif
#ifdef _RDOS
    "RDos",         ProcRdosHelp,           MK_ALL,     0,
#endif
#ifdef _RAW
    "Raw",          ProcRawHelp,            MK_ALL,     0,
#endif
    NULL
};

void DisplayOptions( void )
/*************************/
{
    WRITE_HELP_ONE( GENERAL );
#if defined( _QNX ) && defined( __QNX__ )
    WRITE_HELP_ONE( QNX );
#endif
#ifdef _EXE
    WRITE_HELP_ONE( DOS );
#endif
#ifdef _OS2
    WRITE_HELP_ONE( OS2 );
    WRITE_HELP_ONE( WINDOWS );
    WRITE_HELP_ONE( WINVXD );
    WRITE_HELP_ONE( NT );
#endif
#ifdef _PHARLAP
    WRITE_HELP_ONE( PHARLAP );
#endif
#ifdef _NOVELL
    WRITE_HELP_ONE( NOVELL );
#endif
#ifdef _DOS16M
    WRITE_HELP_ONE( DOS16M );
#endif
#if defined( _QNX ) && !defined( __QNX__ )
    WRITE_HELP_ONE( QNX );
#endif
#ifdef _ELF
    WRITE_HELP_ONE( ELF );
#endif
#ifdef _ZDOS
    WRITE_HELP_ONE( ZDOS );
#endif
#ifdef _RDOS
    WRITE_HELP_ONE( RDOS );
#endif
#ifdef _RAW
    WRITE_HELP_ONE( RAW );
#endif
}

bool DoHelp( void )
/**********************/
// display help, optionally allowing the user to specifiy the format he/she
// wants the help for.
{
    return( ProcOne( FormatHelp, SEP_NO ) );
}

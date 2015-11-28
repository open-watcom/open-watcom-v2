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


#include <stdlib.h>
#include "dbgdefn.h"
#include "dbgdata.h"
#include "dbglit.h"
#include "dbgerr.h"
#include "dbgscan.h"
#include "dbgutil.h"
#include "dbgio.h"
#include "filermt.h"

extern void             WndTmpFileInspect( const char *file );
extern void             StdOutNew( void );

#define PREFIX_LEN      2
static char             FileName[PREFIX_LEN+8+1+3+1] = { '@', 'r' };
static unsigned         FileNum = 0;
static bool             CaptureOk;


extern void CaptureError( void )
{
    CaptureOk = false;
}

OVL_EXTERN bool DoneCapture( inp_data_handle _cmds, inp_rtn_action action )
{
    cmd_list        *cmds = _cmds;
    const char      *old;

    switch( action ) {
    case INP_RTN_INIT:
        ReScan( cmds->buff );
        return( true );
    case INP_RTN_EOL:
        return( false );
    case INP_RTN_FINI:
        FreeCmdList( cmds );
        old = ReScan( LIT_ENG( Empty ) );
        StdOutNew();
        ReScan( old );
        if( CaptureOk ) {
            WndTmpFileInspect( FileName );
        }
        return( true );
    }
    return( false );
}


extern void ProcCapture( void )
{
    cmd_list    *cmds;
    const char  *start;
    size_t      len;
    const char  *old;
    char        *p;
    char        *end;

    if( !ScanItem( false, &start, &len ) )
        Error( ERR_NONE, LIT_ENG( ERR_WANT_COMMAND_LIST ) );
    ReqEOC();
    cmds = AllocCmdList( start, len );
    end = FileName + sizeof( FileName );
    p = FileName + PREFIX_LEN;
    p = CnvULongHex( TaskId, p, end - p );
    *p++ = RemFile.ext_separator;
    p = CnvULongDec( FileNum++, p, end - p );
    *p = '\0';
    old = ReScan( FileName+PREFIX_LEN );
    StdOutNew();
    ReScan( old );
    cmds->use++;
    CaptureOk = true;
    PushInpStack( cmds, DoneCapture, false );
    TypeInpStack( INP_CAPTURED );
    FreeCmdList( cmds );
}

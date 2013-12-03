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
#include "dbglit.h"
#include "dbginp.h"
#include "dbgerr.h"
#include "trptypes.h"

extern bool             ScanItem( bool, char **, unsigned int * );
extern cmd_list         *AllocCmdList( char *, unsigned int );
extern void             FreeCmdList( cmd_list * );
extern void             WndTmpFileInspect( char *file, bool binary );
extern char             *ReScan( char * );
extern void             ReqEOC( void );
extern void             PushInpStack( void *, bool (*rtn)( void *, inp_rtn_action ), bool );
extern void             StdOutNew( void );
extern char             *CnvULongHex( unsigned long, char * );
extern char             *CnvULongDec( unsigned long value, char *buff );
extern void             TypeInpStack( input_type );

extern unsigned         TaskId;
extern file_components  RemFile;

#define PREFIX_LEN      2
static char             FileName[PREFIX_LEN+8+1+3+1] = { '@', 'r' };
static unsigned         FileNum = 0;
static bool             CaptureOk;


extern void CaptureError( void )
{
    CaptureOk = FALSE;
}

OVL_EXTERN bool DoneCapture( void *cmds, inp_rtn_action action )
{
    char        *old;
    switch( action ) {
    case INP_RTN_INIT:
        ReScan( ((cmd_list *)cmds)->buff );
        return( TRUE );
    case INP_RTN_EOL:
        return( FALSE );
    case INP_RTN_FINI:
        FreeCmdList( cmds );
        old = ReScan( LIT( Empty ) );
        StdOutNew();
        ReScan( old );
        if( CaptureOk ) {
            WndTmpFileInspect( FileName, FALSE );
        }
        return( TRUE );
    }
    return( FALSE );
}


extern void ProcCapture( void )
{
    cmd_list    *cmds;
    char        *start;
    unsigned    len;
    char        *old;
    char        *p;

    if( !ScanItem( FALSE, &start, &len ) ) Error( ERR_NONE, LIT( ERR_WANT_COMMAND_LIST ) );
    ReqEOC();
    cmds = AllocCmdList( start, len );
    p = CnvULongHex( TaskId, FileName+PREFIX_LEN );
    *p++ = RemFile.ext_separator;
    p = CnvULongDec( FileNum++, p );
    *p = '\0';
    old = ReScan( FileName+PREFIX_LEN );
    StdOutNew();
    ReScan( old );
    cmds->use++;
    CaptureOk = TRUE;
    PushInpStack( cmds, DoneCapture, FALSE );
    TypeInpStack( INP_CAPTURED );
    FreeCmdList( cmds );
}

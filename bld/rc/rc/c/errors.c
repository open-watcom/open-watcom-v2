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


#include <stdio.h>
#include <stdarg.h>
#include "global.h"
#include "errors.h"
#include "reserr.h"
#include "tmpctl.h"
#include "errprt.h"
#include "rcldstr.h"
#include "iortns.h"
#include "preproc.h"
#include "rcspawn.h"


static char             rcStrBuf[1024];
static char             errBuffer[1024];

static int checkForTmpFiles( unsigned errornum, va_list arglist ) {

    char        *fname;

    switch( errornum ) {
    case ERR_CANT_OPEN_FILE:
    case ERR_UNEXPECTED_EOF:
    case ERR_READING_FILE:
    case ERR_WRITTING_FILE:
    case ERR_WRITTING_RES_FILE:
        fname = va_arg( arglist, char * );
        if( IsTmpFile( fname ) ) {
            switch( errornum ) {
            case ERR_CANT_OPEN_FILE:
                return( ERR_OPENING_TMP );
            case ERR_UNEXPECTED_EOF:
            case ERR_READING_FILE:
                return( ERR_READING_TMP );
            case ERR_WRITTING_FILE:
                return( ERR_WRITTING_TMP );
            }
        }
        /* fall through */
    default:
        return( errornum );
    }
}

static void RcMsgV( unsigned errornum, OutputSeverity sev, va_list arglist )
/***************************************************************************/
{
    const LogicalFileInfo       *currfile;
    OutPutInfo                   errinfo;


    InitOutPutInfo( &errinfo );
    errinfo.severity = sev;
    errinfo.flags |= OUTFLAG_ERRID;
    errinfo.errid = errornum;
    switch (errornum) {
    case ERR_CANT_OPEN_FILE:
    case ERR_READING_TMP:
    case ERR_WRITTING_TMP:
    case ERR_OPENING_TMP:
    case ERR_UNEXPECTED_EOF:
    case ERR_WRITTING_FILE:
    case ERR_READING_FILE:
    case ERR_WRITTING_RES_FILE:
    case ERR_CANT_FIND_FILE:
    case ERR_FILENAME_NEEDED:
    case ERR_NO_OPT_SPECIFIED:
    case ERR_NOT_BITMAP_FILE:
    case ERR_NOT_CURSOR_FILE:
    case ERR_NOT_ICON_FILE:
    case ERR_NOT_VALID_EXE:
    case ERR_OUT_OF_MEMORY:
    case ERR_PARSER_INTERNAL:
    case ERR_RENAMEING_TMP_FILE:
    case ERR_TOO_MANY_ARGS:
    case ERR_UNKNOWN_MULT_OPTION:
    case ERR_UNKNOWN_OPTION:
    case ERR_NONNT_RES_TO_NT_EXE:
    case ERR_NONWIN_RES_TO_WIN_EXE:
    case ERR_NONOS2_RES_TO_OS2_EXE:
    case ERR_OLD_RESOURCE_OBJECT:
    case ERR_READING_EXE:
    case ERR_READING_ICON:
    case ERR_READING_CURSOR:
    case ERR_READING_BITMAP:
    case ERR_READING_FONT:
    case ERR_READING_DATA:
    case ERR_READING_RES:
    case ERR_BAD_RES_VER:
    case ERR_INVALID_RES:
    case ERR_INTERNAL:
        /* don't print the filename & line number before these errors */
        GetRcMsg( errornum, errBuffer, sizeof( errBuffer ) );
        vsprintf( rcStrBuf, errBuffer, arglist );
        sprintf( errBuffer, "%s", rcStrBuf );
        break;
    case ERR_RCSTR_NOT_FOUND:
        /* this message means the error strings cannot be obtained from
         * the exe so its text is hard coded */
        sprintf( errBuffer, "Resource strings not found" );
        break;
    case ERR_NO_MSG:
        /* dont print anything */
        return;
    default:
        GetRcMsg( errornum, errBuffer, sizeof( errBuffer ) );
        vsprintf( rcStrBuf, errBuffer, arglist );
        currfile = RcIoGetLogicalFileInfo();
        if (currfile != NULL) {
            errinfo.flags |= OUTFLAG_FILE | OUTFLAG_LINE;
            errinfo.file = currfile->Filename;
            errinfo.lineno = currfile->LineNum;
        }
        sprintf( errBuffer, "%s", rcStrBuf );
        break;
    }
    RcMsgFprintf( stdout, &errinfo, "%s\n", errBuffer );
}

extern void RcWarning( unsigned errornum, ... )
/*********************************************/
{
    va_list             arglist;

    va_start( arglist, errornum );
    errornum = checkForTmpFiles( errornum, arglist );
    va_end( arglist );

    va_start( arglist, errornum );
    RcMsgV( errornum, SEV_WARNING, arglist );
    va_end( arglist );
}

void RcError( unsigned errornum, ... )
/************************************/
{
    va_list             arglist;

    va_start( arglist, errornum );
    errornum = checkForTmpFiles( errornum, arglist );
    va_end( arglist );

    va_start( arglist, errornum );
    RcMsgV( errornum, SEV_ERROR, arglist );
    va_end( arglist );
}

extern void RcFatalError( unsigned int errornum, ... )
/****************************************************/
{
    va_list             arglist;

    va_start( arglist, errornum );
    errornum = checkForTmpFiles( errornum, arglist );
    va_end( arglist );

    va_start( arglist, errornum );
    RcMsgV( errornum, SEV_FATAL_ERR, arglist );
    va_end( arglist );

    if( IsTmpFile( CurrResFile.filename ) ) {
        ResCloseFile( CurrResFile.handle );
        remove( CurrResFile.filename );
        UnregisterTmpFile( CurrResFile.filename );
    }
#if !defined( WRDLL )
    CloseAllFiles();
#endif
    PP_Fini();
    RCSuicide( -1 );
}

#if !defined( WRDLL )
extern void ErrorInitStatics( void )
/**********************************/
{
    memset( rcStrBuf, 0, sizeof( rcStrBuf ) );
    memset( errBuffer, 0, sizeof( errBuffer ) );
}
#endif

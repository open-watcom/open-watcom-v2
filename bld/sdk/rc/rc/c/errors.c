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
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "errors.h"
#include "rcio.h"
#include "global.h"
#include "reserr.h"
#include "tmpctl.h"
#include "errprt.h"
#include "ldstr.h"
#include "iortns.h"
#include "preproc.h"

/* The following is required as the resource editor compiles this file
 * directly but finds the exit(-1) on FATAL error highly disheartening.
 * The jmp_buf will contain a more reasonable return address back in
 * the heart of the resource editor.
 */
#ifdef WR_COMPILED

// the following is a temporary measure to get around the fact
// that winreg.h defines a type called ppvalue
#define _WINREG_
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <setjmp.h>
extern jmp_buf RC_Dead_env;
#elif defined DLL_COMPILE
#include <setjmp.h>
extern jmp_buf DLL_JumpPt;
#endif

static char             rcStrBuf[1024];
static char             errBuffer[1024];

#define ERRITEM( a, b ) b

#undef ERRITEM

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

#define MAX_LINE_LEN            75
static void RcMsgV( unsigned errornum, OutputSeverity sev, va_list arglist )
/***************************************************************************/
{
    const LogicalFileInfo       *currfile;
    int                         len;
    const char                  *prefix;
    OutPutInfo                   errinfo;


    InitOutPutInfo( &errinfo );
    errinfo.severity = sev;
    errinfo.flags |= OUTFLAG_ERRID;
    errinfo.errid = errornum;
    switch( sev ) {
    case SEV_WARNING:
        prefix = "Warning!";
        break;
    case SEV_ERROR:
        prefix = "Error!";
        break;
    case SEV_FATAL_ERR:
        prefix = "Fatal Error!";
        break;
    default:
        prefix = "";
        break;
    }
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
    case ERR_WIN_RES_TO_NT_EXE:
    case ERR_NT_RES_TO_WIN_EXE:
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
        sprintf( errBuffer, "%s %d: %n%s", prefix, errornum, &len, rcStrBuf );
        break;
    case ERR_RCSTR_NOT_FOUND:
        /* this message means the error strings cannot be obtained from
         * the exe so its text is hard coded */
        sprintf( errBuffer, "%s %d: %nResource strings not found", prefix,
                    errornum, &len );
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
#if !defined( DLL_COMPILE )
            sprintf( errBuffer, "%s(%d): %s %d: %n%s", currfile->Filename,
                        currfile->LineNum, prefix,
                        errornum, &len, rcStrBuf );
#else
            sprintf( errBuffer, "%s %d: %n%s", prefix, errornum,
                        &len, rcStrBuf );
#endif
        } else {
            sprintf( errBuffer, "%s %d: %n%s", prefix, errornum, &len,
                        rcStrBuf );
        }
        break;
    }
#if defined( DLL_COMPILE ) || defined( WR_COMPILED )
    RcFprintf( stdout, &errinfo, "%s\n", errBuffer );
#else
    {
        int             indent;
        char            *start;
        char            *end;

        indent = 0;
        start = errBuffer;
        while( strlen( start ) > MAX_LINE_LEN - indent ) {
            end = start + MAX_LINE_LEN - indent;
            while( !isspace( *end ) && end > start ) end--;
            if( end != start )  {
                *end = '\0';
            } else {
                break;
            }
            RcFprintf( stdout, &errinfo, "%*s%s\n", indent, "", start );
            start = end + 1;
            indent = len;
        }
        RcFprintf( stdout, &errinfo, "%*s%s\n", indent, "", start );
    }
#endif
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
    CloseAllFiles();
    PP_Fini();
#ifdef WR_COMPILED
    longjmp( RC_Dead_env, 1 );
#elif defined DLL_COMPILE
    longjmp( DLL_JumpPt, 1 );
#else
    exit( -1 );
#endif
}

extern void ErrorInitStatics( void )
/**********************************/
{
    memset( rcStrBuf, 0, sizeof( rcStrBuf ) );
    memset( errBuffer, 0, sizeof( errBuffer ) );
}

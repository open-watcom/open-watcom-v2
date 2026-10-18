/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  RC error processing functions       (used by 16-bit code)
*
****************************************************************************/


#include <stdio.h>
#include <stdarg.h>
#include "global.h"
#include "rcerrors.h"
#include "errprt.h"
#include "rcldstr.h"
#include "rcspawn.h"
#include "iortns.h"
#include "rccore.h"
#include "rcmem.h"
#include "rcrtns.h"
#if !defined( INSIDE_WLINK ) && !defined( INSIDE_WR )
    #include "preproc.h"
#endif


static char             rcStrBuf[1024];
static char             errBuffer[1024];

static void RcMsgV( unsigned errornum, OutputSeverity sev, va_list args )
/***********************************************************************/
{
    OutPutInfo          errinfo;
    va_list             args1;
    char                *fname;

#if !defined( INSIDE_WLINK )
    va_copy( args1, args );
    switch( errornum ) {
    case RCMSG_CANT_OPEN_FILE:
    case RCMSG_UNEXPECTED_EOF:
    case RCMSG_READING_FILE:
    case RCMSG_WRITTING_FILE:
    case RCMSG_WRITTING_RES:
        fname = va_arg( args1, char * );
#if defined( INSIDE_WR )
        if( strcmp( fname, TMPFILE2 ) == 0 ) {
#else
        if( strcmp( fname, TMPFILE0 ) == 0
          || strcmp( fname, TMPFILE1 ) == 0
          || strcmp( fname, TMPFILE2 ) == 0 ) {
#endif
            switch( errornum ) {
            case RCMSG_CANT_OPEN_FILE:
                errornum = RCMSG_OPENING_TMP;
                break;
            case RCMSG_UNEXPECTED_EOF:
            case RCMSG_READING_FILE:
                errornum = RCMSG_READING_TMP;
                break;
            case RCMSG_WRITTING_FILE:
                errornum = RCMSG_WRITTING_TMP;
                break;
            }
        }
        break;
    }
    va_end( args1 );
#endif

    InitOutPutInfo( &errinfo );
    errinfo.severity = sev;
    errinfo.flags |= OUTFLAG_ERRID;
    errinfo.errid = errornum;
    switch( errornum ) {
    case RCMSG_CANT_OPEN_FILE:
    case RCMSG_READING_TMP:
    case RCMSG_WRITTING_TMP:
    case RCMSG_OPENING_TMP:
    case RCMSG_UNEXPECTED_EOF:
    case RCMSG_WRITTING_FILE:
    case RCMSG_READING_FILE:
    case RCMSG_WRITTING_RES:
    case RCMSG_CANT_FIND_FILE:
    case RCMSG_FILENAME_NEEDED:
    case RCMSG_NO_OPT_SPECIFIED:
    case RCMSG_NOT_BITMAP_FILE:
    case RCMSG_NOT_CURSOR_FILE:
    case RCMSG_NOT_ICON_FILE:
    case RCMSG_NOT_VALID_EXE:
    case RCMSG_OUT_OF_MEMORY:
    case RCMSG_PARSER_INTERNAL:
    case RCMSG_RENAMING_TMP_FILE:
    case RCMSG_TOO_MANY_FILENAMES:
    case RCMSG_UNKNOWN_MULT_OPTION:
    case RCMSG_UNKNOWN_OPTION:
    case RCMSG_NONNT_RES_TO_NT_EXE:
    case RCMSG_NONWIN_RES_TO_WIN_EXE:
    case RCMSG_NONOS2_RES_TO_OS2_EXE:
    case RCMSG_OLD_RESOURCE_OBJECT:
    case RCMSG_READING_EXE:
    case RCMSG_READING_ICON:
    case RCMSG_READING_CURSOR:
    case RCMSG_READING_BITMAP:
    case RCMSG_READING_FONT:
    case RCMSG_READING_DATA:
    case RCMSG_READING_RES:
    case RCMSG_BAD_RES_VER:
    case RCMSG_INVALID_RES:
    case RCMSG_INTERNAL:
    case RCMSG_DELETING_FILE:
        /*
         * don't print the filename & line number before these errors
         */
        GetRcMsg( errornum, errBuffer, sizeof( errBuffer ) );
        vsprintf( rcStrBuf, errBuffer, args );
        sprintf( errBuffer, "%s", rcStrBuf );
        break;
    case RCMSG_RCSTR_NOT_FOUND:
        /*
         * this message means the error strings cannot be obtained from
         * the exe so its text is hard coded
         */
        sprintf( errBuffer, "Resource strings not found" );
        break;
    case RCMSG_NO_MSG:
        /*
         * dont print anything
         */
        return;
    default:
        GetRcMsg( errornum, errBuffer, sizeof( errBuffer ) );
        vsprintf( rcStrBuf, errBuffer, args );
#if !defined( INSIDE_WLINK ) && !defined( INSIDE_WR )
        errinfo.file = RcIoGetCurrentFileName();
        if( errinfo.file != NULL )
            errinfo.flags |= OUTFLAG_FILE;
        errinfo.lineno = RcIoGetCurrentFileLineNo();
        if( errinfo.lineno != 0 )
            errinfo.flags |= OUTFLAG_LINE;
#endif
        sprintf( errBuffer, "%s", rcStrBuf );
        break;
    }
    RcMsgFprintf( &errinfo, "%s\n", errBuffer );
}

void RcWarning( unsigned errornum, ... )
/**************************************/
{
    va_list             args;

    va_start( args, errornum );
    RcMsgV( errornum, SEV_WARNING, args );
    va_end( args );
}

void RcError( unsigned errornum, ... )
/************************************/
{
    va_list             args;

    va_start( args, errornum );
    RcMsgV( errornum, SEV_ERROR, args );
    va_end( args );
}

bool RcIOError( RcStatus ret, const char *srcname, const char *dstname, int err )
{
    switch( ret ) {
    case RS_OK:
    case RS_PARAM_ERROR:
        return( false );
    case RS_OPEN_ERROR:
        RcError( RCMSG_CANT_OPEN_FILE, srcname, strerror( err ) );
        break;
    case RS_BAD_FILE_FMT:
        RcError( RCMSG_NOT_VALID_EXE, srcname );
        break;
    case RS_READ_ERROR:
        RcError( RCMSG_READING_FILE, srcname, strerror( err ) );
        break;
    case RS_READ_ERROR_EXE:
        RcError( RCMSG_READING_EXE, srcname, strerror( err ) );
        break;
    case RS_READ_ERROR_TMP:
        RcError( RCMSG_READING_TMP, srcname, strerror( err ) );
        break;
    case RS_READ_INCMPLT:
        RcError( RCMSG_UNEXPECTED_EOF, srcname );
        break;
    case RS_WRITE_ERROR:
        RcError( RCMSG_WRITTING_FILE, dstname, strerror( err ) );
        break;
    case RS_WRITE_ERROR_RES:
        RcError( RCMSG_WRITTING_RES, dstname, strerror( err ) );
        break;
    case RS_WRITE_ERROR_TMP:
        RcError( RCMSG_WRITTING_TMP, dstname, strerror( err ) );
        break;
    default:
        RcError( RCMSG_INTERNAL, INTERR_UNKNOWN_RCSTATUS );
        break;
    }
    return( true );
}

void RcFatalError( unsigned int errornum, ... )
/*********************************************/
{
    va_list             args;

    va_start( args, errornum );
    RcMsgV( errornum, SEV_FATAL_ERR, args );
    va_end( args );

#if !defined( INSIDE_WLINK ) && !defined( INSIDE_WR )
    RCCloseFile( &(CurrResFile.fp) );
    CloseAllFiles();
    PP_FileFini();
    PP_Fini();
#endif
    RCSuicide( -1 );
    /* never return */
}

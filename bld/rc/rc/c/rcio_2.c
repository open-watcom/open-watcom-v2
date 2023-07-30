/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  WRC I/O routines, pass 2.
*
****************************************************************************/


#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <time.h>
#include <errno.h>
#ifdef __UNIX__
    #include <sys/stat.h>
#else
    #include <process.h>
#endif
#include "global.h"
#include "rcerrors.h"
#include "reserr.h"
#include "errprt.h"
#include "util.h"
#include "rcldstr.h"
#include "semantic.h"
#include "wresdefn.h"
#include "iopath.h"
#include "pathlist.h"
#include "rcrtns.h"
#include "rccore_2.h"
#include "exeutil.h"
#include "pathgrp2.h"

#include "clibext.h"


#define BUFFER_SIZE         1024

bool CopyFileToOutFile( FILE *inp_fp, const char *out_name )
/**********************************************************/
{
    RcStatus    status;      /* error while deleting or renaming */
    FILE        *out_fp;
    size_t      numread;
    char        *buffer;

    buffer = RESALLOC( BUFFER_SIZE );

    status = RS_OK;
    RESSEEK( inp_fp, 0, SEEK_SET );
    out_fp = ResOpenFileRW( out_name );
    while( (numread = RESREAD( inp_fp, buffer, BUFFER_SIZE )) != 0 ) {
        if( numread != BUFFER_SIZE && RESIOERR( inp_fp, numread ) ) {
            status = RS_READ_ERROR;
            break;
        }
        if( RESWRITE( out_fp, buffer, numread ) != numread ) {
            status = RS_WRITE_ERROR;
            break;
        }
    }
    ResCloseFile( out_fp );

    RESFREE( buffer );
    return( status == RS_OK );

} /* CopyFileToOutFile */

static bool OpenResFileInfo( ExeType type )
/*****************************************/
{
    bool            error;
    ExtraRes        *curfile;
    char            *name;


    if( ( type == EXE_TYPE_NE_WIN || type == EXE_TYPE_NE_OS2 )
        && CmdLineParms.ExtraResFiles != NULL ) {
        RcError( ERR_FR_NOT_VALID_FOR_WIN );
        return( false );
    }
    Pass2Info.AllResFilesOpen = true;
    if( CmdLineParms.NoResFile ) {
        Pass2Info.ResFile = RESALLOC( sizeof( ResFileInfo ) );
        Pass2Info.ResFile->next = NULL;
        Pass2Info.ResFile->name = NULL;
        Pass2Info.ResFile->fp = NULL;
        Pass2Info.ResFile->Dir = NULL;
        return( true );
    }

    if( CmdLineParms.Pass2Only ) {
        name = CmdLineParms.InFileName;
    } else {
        name = CmdLineParms.OutResFileName;
    }
    curfile = RESALLOC( sizeof( ExtraRes ) + strlen( name ) );
    curfile->next = CmdLineParms.ExtraResFiles;
    CmdLineParms.ExtraResFiles = curfile;
    strcpy( curfile->name, name );

    error = OpenResFiles( CmdLineParms.ExtraResFiles, &Pass2Info.ResFile,
                  &Pass2Info.AllResFilesOpen, type,
                  CmdLineParms.InExeFileName );

    return( error );

} /* OpenResFileInfo */


static bool openExeFileInfoRO( const char *filename, ExeFileInfo *info )
/**********************************************************************/
{
    RcStatus        status;
    pe_exe_header   *pehdr;

    info->fp = ResOpenFileRO( filename );
    if( info->fp == NULL ) {
        RcError( ERR_CANT_OPEN_FILE, filename, strerror( errno ) );
        return( false );
    }
    info->IsOpen = true;
    info->Type = FindNEPELXHeader( info->fp, &info->WinHeadOffset );
    info->name = filename;
    switch( info->Type ) {
    case EXE_TYPE_NE_WIN:
    case EXE_TYPE_NE_OS2:
        status = SeekRead( info->fp, info->WinHeadOffset, &info->u.NEInfo.WinHead, sizeof( os2_exe_header ) );
        if( status != RS_OK ) {
            RcError( ERR_NOT_VALID_EXE, filename );
            return( false );
        }
        info->DebugOffset = info->WinHeadOffset + sizeof( os2_exe_header );
        break;
    case EXE_TYPE_PE:
        pehdr = &info->u.PEInfo.WinHeadData;
        info->u.PEInfo.WinHead = pehdr;
        if( SeekRead( info->fp, info->WinHeadOffset, pehdr, PE_HDR_SIZE ) != RS_OK
          || RESREAD( info->fp, (char *)pehdr + PE_HDR_SIZE, PE_OPT_SIZE( *pehdr ) ) != PE_OPT_SIZE( *pehdr ) ) {
            RcError( ERR_NOT_VALID_EXE, filename );
            return( false );
        }
        info->DebugOffset = info->WinHeadOffset + PE_SIZE( *pehdr );
        break;
    case EXE_TYPE_LX:
        status = SeekRead( info->fp, info->WinHeadOffset, &info->u.LXInfo.OS2Head, sizeof( os2_flat_header ) );
        if( status != RS_OK ) {
            RcError( ERR_NOT_VALID_EXE, filename );
            return( false );
        }
        info->DebugOffset = info->WinHeadOffset + sizeof( os2_flat_header );
        break;
    default:
        RcError( ERR_NOT_VALID_EXE, filename );
        return( false );
    }

    return( !RESSEEK( info->fp, 0, SEEK_SET ) );
} /* openExeFileInfoRO */

/*
 * Pass 2 related functions
 */

static void FreeNEFileInfoPtrs( NEExeInfo * info )
/*************************************************/
{
    if( info->Seg.Segments != NULL ) {
        RESFREE( info->Seg.Segments );
        info->Seg.Segments = NULL;
    }
    if( info->Res.Str.StringBlock != NULL ) {
        RESFREE( info->Res.Str.StringBlock );
        info->Res.Str.StringBlock = NULL;
    }
    if( info->Res.Str.StringList != NULL ) {
        RESFREE( info->Res.Str.StringList );
        info->Res.Str.StringList = NULL;
    }
} /* FreeNEFileInfoPtrs */

static void FreePEFileInfoPtrs( PEExeInfo * info )
/************************************************/
{
    if( info->Objects != NULL ) {
        RESFREE( info->Objects );
    }
}

static void FreeLXFileInfoPtrs( LXExeInfo *info )
/***********************************************/
{
    if( info->Objects != NULL ) {
        RESFREE( info->Objects );
    }
    if( info->Pages != NULL ) {
        RESFREE( info->Pages );
    }
    if( info->Res.resources != NULL ) {
        RESFREE( info->Res.resources );
    }
}

static void ClosePass2FilesAndFreeMem( void )
/*******************************************/
{
    ExeFileInfo         *tmp;
    ExeFileInfo         *old;

    tmp = &(Pass2Info.TmpFile);
    old = &(Pass2Info.OldFile);

    if( old->IsOpen ) {
        RESCLOSE( old->fp );
        old->IsOpen = false;
    }
    switch( old->Type ) {
    case EXE_TYPE_NE_WIN:
    case EXE_TYPE_NE_OS2:
        FreeNEFileInfoPtrs( &old->u.NEInfo );
        break;
    case EXE_TYPE_PE:
        FreePEFileInfoPtrs( &old->u.PEInfo );
        break;
    case EXE_TYPE_LX:
        FreeLXFileInfoPtrs( &old->u.LXInfo );
        break;
    default: //EXE_TYPE_UNKNOWN
        break;
    }

    switch( tmp->Type ) {
    case EXE_TYPE_NE_WIN:
    case EXE_TYPE_NE_OS2:
        FreeNEFileInfoPtrs( &tmp->u.NEInfo );
        break;
    case EXE_TYPE_PE:
        FreePEFileInfoPtrs( &tmp->u.PEInfo );
        break;
    case EXE_TYPE_LX:
        FreeLXFileInfoPtrs( &tmp->u.LXInfo );
        break;
    default: //EXE_TYPE_UNKNOWN
        break;
    }
    CloseResFiles( Pass2Info.ResFile );

} /* ClosePass2FilesAndFreeMem */

bool RcPass2IoInit( void )
/************************/
{
    bool    noerror;

    memset( &Pass2Info, 0, sizeof( RcPass2Info ) );
    Pass2Info.IoBuffer = RESALLOC( IO_BUFFER_SIZE );

    noerror = openExeFileInfoRO( CmdLineParms.InExeFileName, &(Pass2Info.OldFile) );
    if( noerror ) {
        Pass2Info.TmpFile.name = TMPFILE2;
        Pass2Info.TmpFile.fp = ResOpenFileTmp( NULL );
        if( Pass2Info.TmpFile.fp == NULL ) {
            RcError( ERR_OPENING_TMP, Pass2Info.TmpFile.name, strerror( errno ) );
            noerror = false;
        }
    }
    if( noerror ) {
        Pass2Info.TmpFile.Type = Pass2Info.OldFile.Type;
        Pass2Info.TmpFile.WinHeadOffset = Pass2Info.OldFile.WinHeadOffset;
        if( Pass2Info.OldFile.Type == EXE_TYPE_PE ) {
            Pass2Info.TmpFile.u.PEInfo.WinHead = &Pass2Info.TmpFile.u.PEInfo.WinHeadData;
            *Pass2Info.TmpFile.u.PEInfo.WinHead = *Pass2Info.OldFile.u.PEInfo.WinHead;
        }
        if( ( Pass2Info.OldFile.Type == EXE_TYPE_NE_WIN || Pass2Info.OldFile.Type == EXE_TYPE_NE_OS2 )
          && CmdLineParms.ExtraResFiles != NULL ) {
            RcError( ERR_FR_NOT_VALID_FOR_WIN );
            noerror = false;
        } else {
            noerror = OpenResFileInfo( Pass2Info.OldFile.Type );
        }
    }
    return( noerror );
} /* RcPass2IoInit */

void RcPass2IoShutdown( bool noerror )
/************************************/
{
    ClosePass2FilesAndFreeMem();
    if( noerror ) {
        CopyFileToOutFile( Pass2Info.TmpFile.fp, CmdLineParms.OutExeFileName );
#ifdef __UNIX__
        {
            struct stat     exe_stat;

            /* copy attributes from input to output executable */
            if( stat( CmdLineParms.InExeFileName, &exe_stat ) == 0 ) {
                chmod( CmdLineParms.OutExeFileName, exe_stat.st_mode );
            }
        }
#endif
    }
    if( Pass2Info.TmpFile.fp != NULL ) {
        ResCloseFile( Pass2Info.TmpFile.fp );
        Pass2Info.TmpFile.fp = NULL;
    }
    if( Pass2Info.IoBuffer != NULL ) {
        RESFREE( Pass2Info.IoBuffer );
        Pass2Info.IoBuffer = NULL;
    }

} /* RcPass2IoShutdown */

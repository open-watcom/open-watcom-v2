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
* Description:  system dependent I/O support.
*
****************************************************************************/


extern void    GetSysIOInfo( ftnfile *fcb );
extern void    GetSysFileInfo( ftnfile *fcb );
extern bool    IsDevice( ftnfile *fcb );
extern void    OpenAction( ftnfile *fcb );
extern int     DfltRecType( ftnfile *fcb );
extern int     _FileAttrs( ftnfile *fcb );
extern void    CloseFile( ftnfile *fcb );
extern bool    Scrtched( ftnfile *fcb );
extern void    CloseDeleteFile( ftnfile *fcb );
extern bool    Errf( ftnfile *fcb );
extern void    FPutBuff( ftnfile *fcb );
extern void    FGetBuff( ftnfile *fcb );
extern void    SeekFile( ftnfile *fcb );
extern bool    NoEOF( ftnfile *fcb );
extern void    SysClearEOF( ftnfile *fcb );
extern bool    SameFile( char *fn1, char *fn2 );
extern void    Rewindf( ftnfile *fcb );
extern void    SysCreateFile( ftnfile *fcb );
extern bool    CheckLogicalRecord( ftnfile *fcb );
extern void    SkipLogicalRecord( ftnfile *fcb );
extern void    BackSpacef( ftnfile *fcb );
extern void    EndFilef( ftnfile *fcb );
extern void    GetIOErrMsg( ftnfile *fcb, char *buff );
extern void    ReportNExist( ftnfile *fcb );
extern void    ReportEOF( ftnfile *fcb );
extern void    WaitForEnter( void );

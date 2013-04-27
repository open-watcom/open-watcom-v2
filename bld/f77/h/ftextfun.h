/****************************************************************************
*
*                Open Watcom Project
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
* Description:  Declaration of external functions for f77 compiler & lib
*
****************************************************************************/

#ifndef _F77_EXTERNAL_FUNCS_H
#define _F77_EXTERNAL_FUNCS_H 1

#include <stdarg.h>

#include "fio.h"
#include "ftnio.h"
#include "errrtns.h"
#include "symdefs.h"

extern void        __AccessFIO( void );
extern bool        __Add( intstar4 *arg1, intstar4 *arg2 );
extern bool        __AllowCommaSep( void );
extern bool        __DevicesCC( void );
extern int         __EnableF77RTExceptionHandling( void );
#if defined( __WATCOMC__ )
extern int         __EnterWVIDEO( char far * );
#endif
extern void        __InitMultiThreadIO( void );
extern void        __InitResource( void );
extern void        __PartialReleaseFIO( void );
extern void        __ReleaseFIO( void );
extern bool        __Sub( intstar4 *arg1, intstar4 *arg2 );
extern ftnfile     *_InitStandardInput( void );
extern ftnfile     *_InitStandardOutput( void );
extern void        _R_FError( int );
extern void        _R_FExtension( int );
extern bool        AddIOFlo(intstar4 *,intstar4 *);
extern void        ArrayIOType( void );
extern void        BackSpacef( ftnfile * );
extern void        Blanks( void );
extern void        BldErrCode( uint, char * );
extern void        BToHS( char *, int, char * );
extern void        BumpComma( void );
extern uint        CarrotType( unsigned int );
extern void        CheckCCtrl( void );
extern void        CheckEor( void );
extern void        ChkConnected( void );
extern void        ChkExist( void );
extern void        ChkIOErr( ftnfile * );
extern void        ChkIOOperation( ftnfile * );
extern void        ChkRecordStructure( void );
extern void        ChkSequential( int );
extern void        ChkUnitId( void );
extern void        ChopFile( b_file * );
extern void        CloseDeleteFile( ftnfile * );
extern void        Closef( b_file * );
extern void        CloseFile( ftnfile * );
extern void        ClrBuff( void );
extern void        ClearEOF( void );
extern void        ConnectFile( void );
extern long int    CurrFileOffset( b_file * );
extern void        DfltInq( void );
extern int         DfltRecType( ftnfile * );
extern void        DiscoFile( ftnfile * );
extern void        DoFreeIn( void );
extern void        DoOpen( void );
extern bool        DoSubstring( intstar4, intstar4, uint );
extern bool        DoSubscript( act_dim_list *, intstar4 *, intstar4 * );
extern void        Drop( char );
extern void        EndFilef( ftnfile * );
extern bool        Errf( ftnfile * );
extern int         Errorf( b_file * );
extern char        *ErrorMsg( b_file * );
extern void        ExtractInfo( char *, ftnfile * );
extern int         ExtractText( char *, int );
extern void        F_Connect( void );
extern void        F_SendData( char *, uint );
extern void        FBackspace( b_file *, int );
extern int         FCheckLogical( b_file * );
extern void        FEmByte( int );
extern void        FEmChar(char PGM *);
extern void        FEmCode( int );
extern void        FEmNum( int );
extern void        FGetBuff( ftnfile * );
extern long int    FGetFilePos( b_file * );
extern uint        FGetRec( b_file *, char *, uint );
extern bool        FindFName( void );
extern bool        FindFtnFile( void );
extern int         FindKWord( char **, int, int, string PGM * );
extern int         FlushBuffer( b_file * );
extern void        FlushStdUnit( void );
extern int         FmtS2F( char *, int, int, bool, int, int, extended *, bool, int *, bool);
extern int         FmtS2I( char *, int, bool, intstar4 *, bool, int * );
extern void        FPTrapFini( void );
extern void        FPTrapInit( void );
extern void        FPutRec( b_file *, char *, int );
extern void        FreeIn( void );
extern void        FreeOut( void );
extern void        FSeekRec( b_file *, unsigned_32, uint );
extern int         FSetCC( b_file *, char, char ** );
extern void        FSetEof( b_file * );
extern void        FSetErr( int, b_file * );
extern void        FSetSysErr( b_file * );
extern void        FSetTrunc( b_file * );
extern int         FSkipLogical( b_file *io );
extern void        FTruncate( b_file * );
extern void        FRewind( b_file * );
extern void        GetFileInfo( void );
extern bool        GetIOErr( ftnfile * );
extern intstar4    GetNum( void );
extern void        GetSysIOInfo(ftnfile *);
extern char        GetStdChar( void );
extern void        GetStr( string *str, char *res );
extern void        GetSysFileInfo( ftnfile * );
extern byte        Hex( byte );
extern void        InitStd( void );
extern int         InqDir( ftnfile * );
extern void        InqExList( void );
extern int         InqFmtd( ftnfile * );
extern void        InqOdList( void );
extern int         InqSeq( ftnfile * );
extern int         InqUnFmtd( ftnfile * );
extern void        IOErr( int, ... );
extern void        IOItemResult( char PGM *, PTYPE );
extern int         IOMain( void (*)( void ) );
extern void        IOOk( b_file * );
extern void        IOPrologue( void );
extern void        IOSysInit( void );
extern bool        IsCarriage( void );
extern bool        IsDevice( ftnfile * );
extern char        *JmpBlanks(char *);
extern pointer     LocFile( char * );
extern pointer     LocUnit( int );
extern void        MsgBuffer( uint, char *, ... );
extern void        MsgFormat( char *, char *, ... );
extern void        MsgJustErr( uint, ... );
extern void        MsgPrintErr( uint, ... );
extern bool        MulIOFlo( intstar4 *, intstar4 * );
extern void        NextIFBuff( char *, int, unsigned_32,string PGM * );
extern void        NextRec( void );
extern bool        NoEOF( ftnfile * );
extern void        OutIntCG( void );
extern void        OutLogCG( void );
extern void        OpenAction(ftnfile *);
extern b_file      *Openf( char *, f_attrs ); // extern file_handle     Openf(char *,int);
extern void        R_ChkFType( void );
extern void        R_ChkIType( void );
extern void        R_ChkType( PTYPE, PTYPE );
extern void        R_ChkRecLen( void );
extern void        R_F2E( extended, char *, int, int, bool, int, int, char );
extern void        R_F2F( extended, char *, int, int, bool, int );
extern void        R_FDoSpec( void );
extern void        R_FEmByte( int );
extern void        R_FEmCode( int );
extern void        R_FEmChar( char PGM * );
extern void        R_FEmNum( int );
extern void        R_FEmEnd( void );
extern void        R_FEmInit( void );
extern void        R_FError( int );
extern void        R_FExec( void );
extern void        R_FExtension( int );
extern void        R_FIFloat( void );
extern void        R_FIHex( void );
extern void        R_FIInt( void );
extern void        R_FILog( void );
extern void        R_FIStr( void );
extern void        R_FmtLog( uint );
extern void        R_FOHex( void );
extern void        R_FOInt( void );
extern void        R_FOLog( void );
extern void        R_FOStr( void );
extern void        R_FOF( void );
extern void        R_FOE ( int, char );
extern void        R_FOG( void );
extern void        R_NewRec( void );
extern void        R_TrapFini( void );
extern void        R_TrapInit( void );
extern void        *RChkAlloc( uint );
extern uint        readbytes( b_file *, char *, uint );
extern bool        RecEOS( void );
extern void        ReportEOF( ftnfile * );
extern void        ReportNExist( ftnfile * );
extern void        Rewindf( ftnfile * );
extern void        RTErr( int, ... );
extern unsigned    RTSysInit( void );
extern bool        SameFile( char *, char * );
extern void        SeekFile( ftnfile * );
extern void        SendChar( char, int );
extern void        SendEOR( void );
extern void        SendStr( char PGM *, uint );
extern void        SendWSLStr( char * );
extern void        SetEOF( void );
extern void        SetFmt( char PGM * );
extern void        SetIOBufferSize( uint );
extern void        SetIOCB( void );
extern void        SetMaxPrec( int );
extern void        SkipLogicalRecord( ftnfile * );
extern void        Scratchf( char * );
extern bool        Scrtched( ftnfile * );
extern void        ShowOptions( char * );
extern void        SkipLogicalRecord( ftnfile * );
extern int         Spawn( void (*)( void ) );
extern uint        StrItem( string PGM *, char *, uint );
extern void        StdBuffer( void );
extern char        *STGetName( sym_id, char * );
extern void        StdFlush( void );
extern void        StdWrite( char *, int );
extern void        StdWriteNL( char *, int );
extern void        Suicide( void );
extern void        SysClearEOF( ftnfile * );
extern void        SysCreateFile( ftnfile * );
extern uint        SysRead( b_file *, char *, uint );
extern int         SysSeek( b_file *, long int, int );
extern int         SysWrite( b_file *, char *, uint );
extern void        SysEOF( void );
extern void        Substitute( char *, char *, va_list );
extern void        TrimStr( string PGM *src, string *res );
extern void        UnFmtIn( void );
extern void        UnFmtOut( void );
extern void        UpdateRecNum( ftnfile * );
extern void        WaitForEnter( void );
extern uint        writebytes( b_file *, char *, uint );

#endif

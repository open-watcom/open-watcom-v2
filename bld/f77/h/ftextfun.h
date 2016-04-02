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

#include "fio.h"
#include "ftnio.h"
#include "errrtns.h"
#include "symdefs.h"

extern bool        __Add( intstar4 *arg1, intstar4 *arg2 );
extern bool        __Sub( intstar4 *arg1, intstar4 *arg2 );
extern ftnfile     *_InitStandardInput( void );
extern ftnfile     *_InitStandardOutput( void );
extern void        _R_FError( int );
extern void        _R_FExtension( int );
extern bool        AddIOFlo(intstar4 *,intstar4 *);
extern void        ArrayIOType( void );
extern void        Blanks( void );
extern void        BumpComma( void );
extern void        CheckCCtrl( void );
extern void        CheckEor( void );
extern void        CloseDeleteFile( ftnfile * );
extern void        DfltInq( void );
extern void        DoFreeIn( void );
extern void        DoOpen( void );
extern void        Drop( char );
extern void        ExtractInfo( char *, ftnfile * );
extern int         ExtractText( char *, int );
extern void        F_SendData( char *, uint );
extern void        FEmByte( int );
extern void        FEmChar(char PGM *);
extern void        FEmCode( int );
extern void        FEmNum( int );
extern bool        FindFName( void );
extern int         FmtS2F( char *, int, int, bool, int, int, extended *, bool, int *, bool);
extern int         FmtS2I( char *, int, bool, intstar4 *, bool, int * );
extern void        FreeIn( void );
extern void        FreeOut( void );
extern intstar4    GetNum( void );
extern void        GetStr( string *str, char *res );
extern int         InqDir( ftnfile * );
extern void        InqExList( void );
extern int         InqFmtd( ftnfile * );
extern void        InqOdList( void );
extern int         InqSeq( ftnfile * );
extern int         InqUnFmtd( ftnfile * );
extern void        IOItemResult( char PGM *, PTYPE );
extern void        IOPrologue( void );
extern pointer     LocFile( char * );
extern pointer     LocUnit( int );
extern bool        MulIOFlo( intstar4 *, intstar4 * );
extern void        NextRec( void );
extern void        OutIntCG( void );
extern void        OutLogCG( void );
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
extern bool        RecEOS( void );
extern void        SendChar( char, int );
extern void        SendEOR( void );
extern void        SendStr( char PGM *, uint );
extern void        SendWSLStr( char * );
extern void        SetIOCB( void );
extern void        SetMaxPrec( int );
extern bool        Scrtched( ftnfile * );
extern bool        SubIOFlo( intstar4 *arg1, intstar4 *arg2 );
extern void        TrimStr( string PGM *src, string *res );
extern void        UnFmtIn( void );
extern void        UnFmtOut( void );
extern void        UpdateRecNum( ftnfile * );
extern void        SendLine( char *str );
extern void        SendInt( intstar4 );
extern void        _AllocBuffer( ftnfile *fcb );
extern bool        GetReal( extended *value );

#endif

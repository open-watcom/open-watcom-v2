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
* Description:  Input/output for listing, errors and source files routines
*
****************************************************************************/


extern  void    InitComIO( void );
extern  void    InitMemIO( void );
extern  void    FiniComIO( void );
extern  void    OpenSrc( void );
extern  void    IOPurge( void );
extern  void    ReadSrc( void );
extern  void    Include( char *inc_name );
extern  bool    SetLst( bool new );
extern  void    SrcInclude( char *name );
extern  void    Conclude( void );
extern  void    OpenErr( void );
extern  void    CompErr( uint msg );
extern  void    PrintErr( char *string );
extern  void    PrtErrNL( void );
extern  void    JustErr( char *string );
extern  void    CloseErr( void );
extern  void    TOutNL( char *string );
extern  void    TOut( char *string );
extern  void    OpenLst( void );
extern  void    ReOpenLst( void );
extern  void    ChkPntLst( void );
extern  bool    WasStmtListed( void );
extern  void    GetBanner( char *buff );
extern  void    GetCopyright( char *buff );
extern  void    GetTrademark( char *buff );
extern  void    GetMoreInfo( char *buff );
extern  void    PrtBanner( void );
extern  void    GetLstName( char *buffer );
extern  void    PrtLstNL( char *string );
extern  void    PrtLst( char *string );
extern  void    CloseLst( void );
extern  void    LFEndSrc( void );
extern  void    LFNewPage( void );
extern  void    LFSkip( void );
extern  void    ChkErrFile( void );
extern  void    TOutBanner( void );

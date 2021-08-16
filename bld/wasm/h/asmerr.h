/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Assembler message output interface.
*
****************************************************************************/


#ifndef _ASMERR_H_INCLUDED
#define _ASMERR_H_INCLUDED

#if defined( _STANDALONE_ )
    #include "asminput.h"
#endif

#include "wasmmsg.h"

#ifdef DEBUG_OUT
    extern void DoDebugMsg( const char *format, ... );
    #define DebugMsg( x ) DoDebugMsg x
#else
    #define DebugMsg( x )
#endif
// use DebugMsg((....)) to call it

#if defined( _STANDALONE_ )
    extern int InternalError( const char *file, unsigned line );
  #if defined( __WATCOMC__ )
    #pragma aux InternalError __aborts
  #endif
  #if DEBUG_OUT
    #define DebugCurrLine() printf( "%s\n", CurrString );
    #define AsmIntErr( x ) DebugCurrLine(); printf( "Internal error = %u\n", x )
  #else
    #define DebugCurrLine()
    #define AsmIntErr( x ) printf( "Internal error = %u\n", x )
  #endif
#else
    #define DebugCurrLine()
    #define AsmIntErr( x )
#endif

#ifdef _STANDALONE_

extern bool MsgInit( void );
extern bool MsgGet( unsigned, char * );
extern void MsgFini( void );
extern void OpenLstFile( void );
extern void LstMsg( const char *format, ... );
extern void PrintfUsage( void );
extern void MsgPrintf( unsigned resourceid );
extern void MsgPrintf1( unsigned resourceid, const char *token );
extern int  PrintBanner( void );
extern void AsmErr( unsigned msgnum, ... );
extern void AsmWarn( int level, unsigned msgnum, ... );
extern void AsmNote( int level, unsigned msgnum, ... );
extern void _AsmNote( int level, unsigned msgnum, ... );
#if !defined( INCL_MSGTEXT )
extern void MsgPutUsage( void );
extern void MsgSubStr( char *, char *, char );
extern void MsgChgeSpec( char *strptr, char specifier );
#endif

#endif  /* _STANDALONE_ */

#endif  /* _ASMERR_H_INCLUDED */

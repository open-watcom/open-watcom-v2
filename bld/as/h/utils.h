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


#if defined( NDEBUG ) || !defined( _STANDALONE_ )
    #define _DBGMSG1( a )               ((void)0)
    #define _DBGMSG2( a, b )            ((void)0)
    #define _DBGMSG3( a, b, c )         ((void)0)
#else
    #define _DBGMSG1( a )               DebugPrintf( a )
    #define _DBGMSG2( a, b )            DebugPrintf( a, b )
    #define _DBGMSG3( a, b, c )         DebugPrintf( a, b, c )
#endif

#define Warning AsWarning
#define Error   AsError

#define MAX_RESOURCE_SIZE       128
extern char AsResBuffer[ MAX_RESOURCE_SIZE ];

#ifdef _STANDALONE_
extern unsigned ErrorLimit;
extern unsigned WarningLevel;
extern unsigned DebugLevel;

extern void Banner( void );
extern void Usage( void );
extern void AsOutMessage( FILE *, int, ... );
extern bool ErrorsExceeding( unsigned );
extern bool WarningsExceeding( unsigned );
extern void ErrorReport( void );
extern void ErrorCountsReset( void );
extern void AbortParse( void );
#ifndef NDEBUG
extern void DebugPrintf( char *, ... );
#endif
extern char *MakeAsmFilename( const char * );
#endif

extern void AsWarning( int, ... );
extern void AsError( int, ... );

extern char *AsStrdup( const char * );

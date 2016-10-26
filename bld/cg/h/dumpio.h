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
* Description:  Data dumping prototypes.
*
****************************************************************************/


#if 0
#define DumpLiteral( x )        { \
                                    static char __x[] = x; \
                                    DumpString( __x );\
                                }
#else
#define DumpLiteral     DumpString
#endif

extern  void    DumpPadString( const char *, int );
extern  void    DumpString( const char * );
extern  void    DumpXString( const char * );
extern  void    DumpChar( char );
extern  void    DumpNL( void );
extern  void    DumpPtr( void *ptr );
extern  void    DumpInt( int );
extern  void    DumpLong( signed_32 );
extern  void    DumpLLong( signed_32, int );
extern  void    Dump8h( unsigned_32 );
extern  void    DumpByte( byte n );
extern  void    DumpId( unsigned );

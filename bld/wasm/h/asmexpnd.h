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


#if defined( _STANDALONE_ )

extern int  ExpandAllConsts( int start_pos, bool early_only );
extern int  ExpandProcString( int index );
extern int  ExpandSymbol( int i, bool early_only );
extern void AddTokens( asm_tok **buffer, int start, int count );
extern int  DefineConstant( int i, bool redefine, bool expand_early );
extern int  StoreConstant( char *name, char *value, bool redefine );
extern int  StoreConstantNumber( char *name, long value, bool redefine );
extern void MakeConstantUnderscored( char const * );
#define STRING_EXPANDED (NOT_ERROR+1)

extern int ExpandTheConstant( int start_pos, bool early_only, bool flag_msg );

#endif

extern int ExpandTheWorld( int start_pos, bool early_only, bool flag_msg );

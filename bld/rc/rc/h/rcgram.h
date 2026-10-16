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
* Description:  Grammar related macros definition.
*
****************************************************************************/


#define XGSTR(s) #s
#define GSTR(s) XGSTR(s)

#if defined( GRAMMAR_OS2 )
#define GCONCATH(s1,s2) s1##2.##s2
#define GSUFFIX(x)      x##OS2
#elif defined( GRAMMAR_W16 )
#define GCONCATH(s1,s2) s1##6.##s2
#define GSUFFIX(x)      x##W16
#elif defined( GRAMMAR_WIN )
#define GCONCATH(s1,s2) s1##w.##s2
#define GSUFFIX(x)      x##WIN
#else
#define GCONCATH(s1,s2) s1.s2
#define GSUFFIX(x)      x
#endif
#define GHFILE(x,y)     GSTR( GCONCATH( x, y ) )
#define GNAME(x)        GSUFFIX( x )

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
* Description:  Expression parser (using .prs grammar files).
*
****************************************************************************/


extern void         SetUpExpr( unsigned addr_depth );
extern void         EvalLValExpr( unsigned addr_depth );
extern void         EvalExpr( unsigned addr_depth );
extern void         NormalExpr( void );
extern void         ChkExpr( void );
extern unsigned_64  ReqU64Expr( void );
extern long         ReqLongExpr( void );
extern unsigned     ReqExpr( void );
#ifdef DEADCODE
extern xreal        ReqXRealExpr( void );
#endif
extern unsigned     OptExpr( unsigned def_val );
extern void         MakeMemoryAddr( bool pops, memory_expr def_seg, address *val );
extern void         ReqMemAddr( memory_expr def_seg, address *out_val );
extern void         CallExpr( address *out_val );
extern void         OptMemAddr( memory_expr def_seg, address *def_val );
extern void         SetTokens( bool parse_tokens );
extern void         LangInit( void );
extern void         LangFini( void );
extern bool         LangLoad( const char *lang, size_t langlen );

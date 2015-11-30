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
* Description:  Semantic actions for SSL-driven grammars.
*
****************************************************************************/


#define SSL2BOOL(x)     (x != 0)
#define SSL2INT(x)      ((signed short)(x))

typedef enum {
    TERM_NORMAL,
    TERM_SYNTAX,
    TERM_ERROR,
    TERM_STK_OVERFLOW,
    TERM_KILL
} ssl_error_class;

typedef unsigned short  ssl_value;
typedef unsigned short  ssl_tokens;

extern bool         SemAllowClosestLine( bool ok );
extern ssl_value    SSLSemantic( ssl_value action, ssl_value parm );
extern int          SSLError( ssl_error_class class, ssl_value error );
extern void         SSLOutToken( ssl_value token );
extern ssl_value    SSLNextToken( void );
extern ssl_value    SSLCurrToken( void );

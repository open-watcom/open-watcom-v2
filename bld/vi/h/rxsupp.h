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


#ifndef RXSUPP_INCLUDED
#define RXSUPP_INCLUDED

#include "regexp.h"

extern regexp *CurrentRegularExpression;

/* findrx.c */
extern vi_rc    FindRegularExpression( char *, i_mark *, char **, linenum, find_type );
extern vi_rc    FindRegularExpressionBackwards( char *, i_mark *, char **, linenum, find_type );

/* rxsupp.c */
extern int      CurrentRegComp( char * );
extern int      GetCurrRegExpColumn( char * );
extern int      GetCurrRegExpLength( void );
extern void     MakeExpressionNonRegular( char * );
extern void     RegExpAttrSave( int, char * );
extern void     RegExpAttrRestore( void );
extern bool     IsMagicCharRegular( char );

/* regsub.c */
extern int      RegSub( regexp *, char *, char *, linenum );

#endif

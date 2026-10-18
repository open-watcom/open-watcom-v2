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
:segment OS2
* Description:  Public interface to OS/2 specific yydriver module.
:elsesegment WIN16
* Description:  Public interface to WIN16 specific yydriver module.
:elsesegment WIN32
* Description:  Public interface to WIN32 specific yydriver module.
:endsegment
*
****************************************************************************/


/*
 * guard is for single header file is included
 * one of yydrivr2.h, yydrivr6.h, yydrivrw.h
 * that macro _YYDRIVRX_H is used over these files
 */
#ifndef _YYDRIVRX_H
#define _YYDRIVRX_H

:segment OS2
#include "os2ytab.h"
:elsesegment WIN16
#include "w16ytab.h"
:elsesegment WIN32
#include "winytab.h"
:endsegment
#include "scan.h"

:segment OS2
extern YYTOKENTYPE  LookupKeywordOS2( ScanString newstring );
extern YYTOKENTYPE  ScanOS2( ScanValue *value );
:elsesegment WIN16
extern YYTOKENTYPE  LookupKeywordW16( ScanString newstring );
extern YYTOKENTYPE  ScanW16( ScanValue *value );
:elsesegment WIN32
extern YYTOKENTYPE  LookupKeywordWIN( ScanString newstring );
extern YYTOKENTYPE  ScanWIN( ScanValue *value );
:endsegment

#endif

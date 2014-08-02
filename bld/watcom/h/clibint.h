
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
* Description:  Things that are stolen from CLIB. Previously were just
*               referenced as extern in the source file
*
****************************************************************************/

#ifndef _CLIBINT_H_E9806518_31C7_4441_BCA4_93A19CDE39C4
#define _CLIBINT_H_E9806518_31C7_4441_BCA4_93A19CDE39C4

/*
 * From CLIB
 */

#ifdef __cplusplus
extern "C" {
#endif

extern char             *_LpDllName;    /* pointer to dll name */

extern char             **_argv;        /* argument vector */
extern int              _argc;

#if defined( __WATCOMC__ ) && defined( _M_IX86 )

extern unsigned char    _8087;
extern unsigned char    _real87;

#endif

#ifdef __cplusplus
}
#endif

#endif /* _CLIBINT_H_E9806518_31C7_4441_BCA4_93A19CDE39C4 */

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


#ifndef _MBWCCONV_H_INCLUDED
#define _MBWCCONV_H_INCLUDED

#include "variety.h"
#include <stddef.h>

typedef wchar_t *       WCPTR;
typedef const wchar_t * CWCPTR;
typedef char *          CPTR;

_WCRTLINK int __exec_wide_to_mbcs( CWCPTR wcPath,       // i: path
                                   CWCPTR const wcArgv[],// i: arguments
                                   CWCPTR const wcEnvp[],// i: environment
                                   CPTR *mbPath,        // o: path
                                   CPTR *mbArgv[],      // o: arguments
                                   CPTR *mbEnvp[] );    // o: environment

_WCRTLINK void __exec_wide_to_mbcs_cleanup( CPTR mbPath,
                                            CPTR mbArgv[],
                                            CPTR mbEnvp[] );

#endif

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
* Description:  Environment pointers.
*
****************************************************************************/


#include "variety.h"
#include <stdlib.h>
#include "rtdata.h"
#include "rtinit.h"

_WCRTDATA char ** _WCDATA environ;      /* pointer to environment table */
_WCRTDATA wchar_t ** _WCDATA _wenviron; /* pointer to wide char environment */
char * _WCNEAR __env_mask;              /* ptr to char array of flags */

extern void __setenvp( void );

AXI( __setenvp, INIT_PRIORITY_LIBRARY )

#if !defined(__NETWARE__)
/* Environment needs to be freed after files are shut down; __MkTmpFile()
 * is used to delete temp files and needs access to environment.
 */
extern void __freeenvp( void );
AYI( __freeenvp, INIT_PRIORITY_LIBRARY - 1 )
#endif

/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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


#ifndef _UIINTERN_H_INCLUDED
#define _UIINTERN_H_INCLUDED

#include <sys/types.h>
#include "uidef.h"
#include "uivirts.h"
#include "uiintern.h"


pid_t       UIProxy;                        /* proxy for all events */
pid_t       UIPGroup;                       /* process group */
bool        UIDisableShiftChanges = false;  /* Disable checking on non console devices */
VirtDisplay UIVirt;                         /* Active virtual console functions */

#ifdef __QNX__
int         UIConsole = 0;                  /* console number */
pid_t       UILocalProxy;                   /* proxy's incoming value (usually same as UIProxy */
pid_t       UIRemProxy;                     /* remote proxy if nec.. */
nid_t       UIConNid;                       /* Node of console mgr */
#endif

#endif

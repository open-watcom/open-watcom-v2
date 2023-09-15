/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DIP client callback function prototypes.
*
****************************************************************************/


#ifndef DIPCLI_H_INCLUDED
#define DIPCLI_H_INCLUDED

#include "digcli.h"
#include "diptypes.h"

#define DIPSIG          "DIP"
#define DIPSIGVAL       0x00504944UL    // "DIP"

#define DIPCli(n)       DIPCli ## n
#define _DIPCli(n)      _DIPCli ## n n

#define DIPCLIENTRY(n)  DIGCLIENT DIPCli(n)

#define pick(r,n,p) typedef r (DIGCLIENT *_DIPCli ## n) p;
#include "_dipcli.h"
#undef pick

#define pick(r,n,p) extern r DIPCLIENTRY( n ) p;
#include "_dipcli.h"
#undef pick

#endif

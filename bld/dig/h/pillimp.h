/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2016 The Open Watcom Contributors. All Rights Reserved.
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


#ifndef PILLIMP_H_INCLUDED
#define PILLIMP_H_INCLUDED

#include "digpck.h"
#include "piltypes.h"
#include "digpck.h"

#define PILL_VERSION    0

#define _LinkImp(n)  _LinkImp ## n *n

#define pick(r,n,p) typedef r DIGENTRY _LinkImp ## n p;
#include "_pillimp.h"
#undef pick

struct pill_imp_routines {
    unsigned_16         version;
    unsigned_16         sizeof_struct;

    _LinkImp( Load );
    _LinkImp( Unload );
    _LinkImp( Init );
    _LinkImp( MaxSize );
    _LinkImp( Put );
    _LinkImp( Kicker );
    _LinkImp( Abort );
    _LinkImp( Fini );
    _LinkImp( Message );
    _LinkImp( Private );
};

#define pick(r,n,p) extern r DIGENTRY LinkImp ## n p;
#include "_pillimp.h"
#undef pick

typedef struct pill_client_routines {
    unsigned_16         version;
    unsigned_16         sizeof_struct;

    _DIGCli( Alloc );
    _DIGCli( Realloc );
    _DIGCli( Free );

    _DIGCli( Open );
    _DIGCli( Seek );
    _DIGCli( Read );
    _DIGCli( Write );
    _DIGCli( Close );
    _DIGCli( Remove );

    _LinkCli( BufferGet );
    _LinkCli( BufferRel );
    _LinkCli( Received );
    _LinkCli( State );
} pill_client_routines;

typedef pill_imp_routines * DIGENTRY pill_init_func( pill_status *status, pill_client_routines *client );
#ifdef __WINDOWS__
typedef void DIGENTRY pill_fini_func( void );
#endif

#define pick(r,n,p) extern r LC ## n p;
#include "_digcli.h"
#include "_pillcli.h"
#undef pick

#include "digunpck.h"

#endif

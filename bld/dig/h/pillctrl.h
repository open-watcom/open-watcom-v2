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


#ifndef PILLCTRL_H

#include "piltypes.h"
#include "digpck.h"

struct link_handle {
    link_handle                 *next;
    link_instance               *inst;
    const pill_imp_routines     *rtns;
    void                        *sys;
    link_imp_handle             *imp;           /* for implementation use */
    char                        name[1];        /* variable sized */
};

struct link_instance {
    link_handle                 *h;
    link_instance               *next;
    void                        *cookie;
    link_trigger                *tp;
    link_imp_instance           *imp;           /* for implementation use */
    volatile unsigned           max;
    volatile link_queue         out;
    volatile link_queue         in;
    volatile link_status        ls;
};

#define PILLCTRL_H

#include "digunpck.h"
#endif

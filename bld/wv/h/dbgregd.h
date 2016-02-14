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


#include "madregs.h"

#define MAX_THD_EXTRA_SIZE      40

typedef unsigned long           dtid_t;

struct machine_state {
    byte                *ovl;
    dtid_t              tid;
    dig_mad             mad;
    unsigned            : 0;    /* for alignment */
    mad_registers       mr;     /* variable size */
};

typedef unsigned_8 thread_state_enum; enum {
                  /* states for clients that freeze execution */
    THD_THAW,
    THD_FREEZE,
                  /* states for clients that only freeze debugged threads */
    THD_WAIT,                   /* waiting for a timeout      */
    THD_SIGNAL,                 /* waiting for a signal       */
    THD_KEYBOARD,               /* waiting for keyboard input */
    THD_BLOCKED,                /* blocked on a resource      */
    THD_RUN,                    /* running or ready to run    */
    THD_DEBUG,                  /* thread is in a debug-state */
                  /* special dead state */
    THD_DEAD = 0x40
};

typedef struct thread_state     thread_state;
struct thread_state {
    thread_state        *link;
    dtid_t              tid;
    thread_state_enum   state;
    unsigned_16         cs;
    unsigned_32         eip;
    char                extra[MAX_THD_EXTRA_SIZE + 1];
    char                name[1];        /* variable size */
};

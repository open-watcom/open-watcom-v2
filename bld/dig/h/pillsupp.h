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


#ifndef PILLSUPP_H_INCLUDED
int             LSuppInit( void );
void            LSuppFini( void );

#define LSuppQueueInit( lq )    \
        { (lq)->head = 0; (lq)->tail_owner = (link_buffer*volatile*)&(lq)->head; }
void            LSuppQueuePush( link_queue volatile *lq, link_buffer *buff );
link_buffer     *LSuppQueuePull( link_queue volatile *lq );

void            LSuppTrigger( link_instance *li );

unsigned        LSuppCritEnter( void );
void            LSuppCritExit( unsigned data );

extern void             LSuppAtomicSet( link_atomic_t *la, link_atomic_t value );
extern link_atomic_t    LSuppAtomicSwap( link_atomic_t *la, link_atomic_t value );

#if defined(__386__)
    #define LSuppAtomicSet( la, v )     \
        (*(volatile link_atomic_t *)(la) = (v))
    #pragma aux LSuppAtomicSwap = "xchg [edx],al" parm [edx] [al] value [al]
#elif defined(__I86__)
    #define LSuppAtomicSet( la, v )     \
        (*(volatile link_atomic_t *)(la) = (v))
    #pragma aux LSuppAtomicSwap = "xchg es:[bx],al" parm [es bx] [al] value [al]
#else
    #error Atomic routines not configured for processor
#endif


#define PILLSUPP_H_INCLUDED
#endif

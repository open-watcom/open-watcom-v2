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


#if defined(__OS2__)
    #define INCL_DOSPROCESS

    #include <os2.h>
#elif defined(__QNX__)
    #include <sys/kernel.h>
#elif defined(__NT__)
    #include <windows.h>
#endif
#include <string.h>
#include "pilltrig.h"
#include "pillctrl.h"
#include "pillsupp.h"

#if defined(__NT__)
    static CRITICAL_SECTION Sync;
#endif

#pragma off(check_stack)

int             LSuppInit( void )
{
#if defined(__OS2__) || defined(__QNX__)
    return( 1 );
#elif defined(__NT__)
    InitializeCriticalSection( &Sync );
    return( 1 );
#else
    #error LSuppInit not configured for system
#endif
}

void            LSuppFini( void )
{
#if defined(__OS2__) || defined(__QNX__)
    /* nothing doing */
#elif defined(__NT__)
    DeleteCriticalSection( &Sync );
#else
    #error LSuppFini not configured for system
#endif
}

void            LSuppTrigger( link_instance *li )
{
#if defined(__OS2__)
    DosSemClear( &li->tp->trigger );
#elif defined(__QNX__)
    Trigger( li->tp->trigger );
#elif defined(__NT__)
    SetEvent( li->tp->trigger );
#else
    #error LSuppTrigger not configured for system
#endif
}


unsigned LSuppCritEnter( void )
{
#if defined(__OS2__)
    DosEnterCritSec();
    return( 0 );
#elif defined(__QNX__)
    return( 0 );
#elif defined(__NT__)
    EnterCriticalSection( &Sync );
    return( 0 );
#else
    #error LSuppCritEnter not configured for system
#endif
}

void            LSuppCritExit( unsigned data )
{
#if defined(__OS2__)
    DosExitCritSec();
#elif defined(__QNX__)
    /* nothing doing */
#elif defined(__NT__)
    LeaveCriticalSection( &Sync );
#else
    #error LSuppCritExit not configured for system
#endif
}

void            LSuppQueuePush( link_queue volatile *lq, link_buffer *buff )
{
    unsigned long       save;

    save = LSuppCritEnter();
    buff->next = NULL;
    *lq->tail_owner = buff;
    lq->tail_owner = &buff->next;
    LSuppCritExit( save );
}

link_buffer     *LSuppQueuePull( volatile link_queue *lq )
{
    unsigned long       save;
    link_buffer         volatile *buff;

    if( lq->head == NULL ) return( NULL );
    save = LSuppCritEnter();
    buff = lq->head;
    lq->head = buff->next;
    if( lq->tail_owner == &buff->next ) {
        lq->tail_owner = (link_buffer**)&lq->head;
    }
    LSuppCritExit( save );
    return( (link_buffer*)buff );
}

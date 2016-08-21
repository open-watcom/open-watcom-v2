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


#include <string.h>
#include <stdlib.h>
#include "pillink.h"
#include "pillimp.h"
#include "pillcli.h"
#include "pillctrl.h"
#include "pillsupp.h"

static link_handle      *PILLList;

/*
 * System specific support routines
 */
int                     PILLSysLoad( const char *path,
                                const pill_client_routines *cli,
                                link_handle *lh,
                                link_message *msg );
void                    PILLSysUnload( link_handle *lh );
void                    PILLSysNoMem( link_message *msg );
pill_private_func       *PILLSysFixFunc( link_handle *lh, pill_private_func *f );
void                    PILLSysTriggerSet( link_instance *li );

/*
 * Client interface
 */

static const pill_client_routines PILLClientInterface = {
    PILL_VERSION,
    sizeof( pill_client_routines ),

    DIGCli( Alloc ),
    DIGCli( Realloc ),
    DIGCli( Free ),

    DIGCli( Open ),
    DIGCli( Seek ),
    DIGCli( Read ),
    DIGCli( Write ),
    DIGCli( Close ),
    DIGCli( Remove ),

    LinkCli( BufferGet ),
    LinkCli( BufferRel ),
    LinkCli( Received ),
    LinkCli( State ),
};

/* for when LinkRegister is used */
const pill_client_routines      *PILLClient = &PILLClientInterface;


link_handle     *LinkLoad( const char *name, link_message *msg )
{
    link_handle *lh;
    unsigned    len;

    //NYI: who should add the "pil" prefix to the name?
    for( lh = PILLList; lh != NULL; lh = lh->next ) {
        if( strcmp( lh->name, name ) == 0 ) {
            return( lh );
        }
    }
    len = strlen( name );

    lh = DIGCli( Alloc )( sizeof( *lh ) + len );
    if( lh == NULL ) {
        PILLSysNoMem( msg );
        return( NULL );
    }
    lh->next = PILLList;
    PILLList = lh;
    lh->inst = NULL;
    lh->rtns = NULL;
    lh->sys = NULL;
    lh->imp = NULL;
    memcpy( lh->name, name, len + 1 );
    if( !PILLSysLoad( name, &PILLClientInterface, lh, msg ) ) {
        if( msg->source == NULL ) {
            PILLList = lh->next;
            DIGCli( Free )( lh );
        }
        return( NULL );
    }
    if( !lh->rtns->Load( lh, msg ) ) {
        return( NULL );
    }
    return( lh );
}

link_handle     *LinkRegister( const pill_imp_routines *rtns, link_message *msg )
{
    link_handle *lh;

    lh = DIGCli( Alloc )( sizeof( *lh ) );
    if( lh == NULL ) {
        PILLSysNoMem( msg );
        return( NULL );
    }
    lh->next = PILLList;
    PILLList = lh;
    lh->inst = NULL;
    lh->rtns = rtns;
    lh->sys = NULL;
    lh->imp = NULL;
    lh->name[0] = '\0';
    if( !lh->rtns->Load( lh, msg ) ) {
        return( NULL );
    }
    return( lh );
}

link_instance   *LinkInit( link_handle *lh, void *cookie, link_trigger *tp, const char *parm )
{
    link_instance       *li;

    if( parm == NULL )
        parm = "";
    li = DIGCli( Alloc )( sizeof( *li ) );
    li->h = lh;
    li->cookie = cookie;
    li->tp = tp;
    li->ls = LS_INITIAL;
    LSuppQueueInit( &li->in );
    LSuppQueueInit( &li->out );
    if( !lh->rtns->Init( li, parm ) ) {
        DIGCli( Free )( li );
        return( NULL );
    }
    li->next = lh->inst;
    lh->inst = li;
    return( li );
}

link_status     LinkStatus( link_instance *li )
{
    return( li->ls );
}

unsigned        LinkMaxSize( link_instance *li, unsigned req_size )
{
    return( li->h->rtns->MaxSize( li, req_size ) );
}

link_status     LinkPut( link_instance *li, link_buffer *data )
{
    return( li->h->rtns->Put( li, data ) );
}

link_status     LinkPutMx( link_instance *li, unsigned num, const lmx_entry *put )
{
    unsigned            total;
    unsigned            i;
    unsigned_8          *p;
    link_buffer         *buff;
    link_message        msg;

    total = 0;
    for( i = 0; i < num; ++i ) {
        total += put[i].len;
    }
    buff = LinkCli( BufferGet )( li->cookie, total );
    if( buff == NULL ) {
        PILLSysNoMem( &msg );
        li->ls = LS_FAILURE_TRANS;
        LinkCli( State )( li->cookie, LS_FAILURE_TRANS, &msg );
        return( LS_FAILURE_TRANS );
    }
    buff->len = total;
    p = buff->data;
    for( i = 0; i < num; ++i ) {
        memcpy( p, put[i].data, put[i].len );
        p += put[i].len;
    }
    return( li->h->rtns->Put( li, buff ) );
}

unsigned        LinkKicker( void )
{
    unsigned            wait;
    unsigned            curr;
    link_handle         *lh;
    link_instance       *li;

    for( lh = PILLList; lh != NULL; lh = lh->next ) {
        for( li = lh->inst; li != NULL; li = li->next ) {
            PILLSysTriggerSet( li );
        }
    }
    wait = PILL_KICK_WAIT;
    for( lh = PILLList; lh != NULL; lh = lh->next ) {
        for( li = lh->inst; li != NULL; li = li->next ) {
            curr = lh->rtns->Kicker( li );
            if( curr < wait ) {
                wait = curr;
            }
        }
    }
    return( wait );
}

static void KillInstance( link_instance *li )
{
    link_instance       *curr;
    link_instance       **owner;

    owner = &li->h->inst;
    for( ;; ) {
        curr = *owner;
        if( curr == NULL )
            return;
        if( curr == li )
            break;
        owner = &curr->next;
    }
    *owner = li->next;
    DIGCli( Free )( li );
}

link_status     LinkAbort( link_instance *li )
{
    link_status         ls;

    ls = li->h->rtns->Abort( li );
    KillInstance( li );
    return( ls );
}

link_status     LinkFini( link_instance *li )
{
    link_status         ls;

    ls = li->h->rtns->Fini( li );
    KillInstance( li );
    return( ls );
}

unsigned        LinkMessage( const link_message *msg, pil_language pl, unsigned max, char *buff )
{
    unsigned    len;

    if( msg->source == NULL )
        return( 0 );
    len = msg->source->rtns->Message( msg, pl, max, buff );
    if( msg->source->inst == NULL ) {
        /* message when no instance active - something must have gone wrong
         * with the load
         */
        LinkUnload( msg->source );
    }
    return( len );
}

pill_private_func       *LinkPrivate( link_handle *lh, const char *string )
{
    pill_private_func   *rtn;

    rtn = lh->rtns->Private( string );
    if( rtn != NULL )
        rtn = PILLSysFixFunc( lh, rtn );
    return( rtn );
}

void            LinkUnload( link_handle *lh )
{
    link_handle         *curr;
    link_handle         **owner;

    while( lh->inst != NULL ) {
        LinkFini( lh->inst );
    }
    if( lh->rtns != NULL ) {
        lh->rtns->Unload( lh );
    }
    if( lh->sys != NULL ) {
        PILLSysUnload( lh );
    }
    for( owner = &PILLList; (curr = *owner) != NULL; owner = &curr->next ) {
        if( curr == lh ) {
            break;
        }
    }
    *owner = lh->next;
    DIGCli( Free )( lh );
}

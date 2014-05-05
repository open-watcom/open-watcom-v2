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


#ifndef PILLIMP_H_INCLUDED
#define PILLIMP_H_INCLUDED

#include "digpck.h"
#include "piltypes.h"
#include "digpck.h"

#ifdef SERVER
    #undef  DIGENTRY
    #define DIGENTRY DIGREGISTER
#endif

#define PILL_VERSION    0

struct pill_imp_routines {
    unsigned_16         version;
    unsigned_16         sizeof_struct;

    int                 (DIGENTRY *LinkImpLoad)( link_handle *lh, link_message *msg );
    void                (DIGENTRY *LinkImpUnload)( link_handle *lh );
    int                 (DIGENTRY *LinkImpInit)( link_instance *li, const char *parm );
    unsigned            (DIGENTRY *LinkImpMaxSize)( link_instance *li, unsigned req_size );
    link_status         (DIGENTRY *LinkImpPut)( link_instance *li, link_buffer *data );
    unsigned            (DIGENTRY *LinkImpKicker)( link_instance *li );
    link_status         (DIGENTRY *LinkImpAbort)( link_instance *li );
    link_status         (DIGENTRY *LinkImpFini)( link_instance *li );
    unsigned            (DIGENTRY *LinkImpMessage)( const link_message *msg, pil_language pl, unsigned max, char *buff );
    pill_private_func   *(DIGENTRY *LinkImpPrivate)( const char *string );
};

int                     DIGENTRY LinkImpLoad( link_handle *lh, link_message *msg );
void                    DIGENTRY LinkImpUnload( link_handle *lh );
int                     DIGENTRY LinkImpInit( link_instance *li, const char *parm );
unsigned                DIGENTRY LinkImpMaxSize( link_instance *li, unsigned req_size );
link_status             DIGENTRY LinkImpPut( link_instance *li, link_buffer *data );
unsigned                DIGENTRY LinkImpKicker( link_instance *li );
link_status             DIGENTRY LinkImpAbort( link_instance *li );
link_status             DIGENTRY LinkImpFini( link_instance *li );
unsigned                DIGENTRY LinkImpMessage( const link_message *msg, pil_language pl, unsigned max, char *buff );
pill_private_func       *DIGENTRY LinkImpPrivate( const char *string );


typedef struct pill_client_routines {
    unsigned_16         version;
    unsigned_16         sizeof_struct;

    void                *(DIGCLIENT *LCAlloc)( size_t );
    void                *(DIGCLIENT *LCRealloc)( void *, size_t );
    void                (DIGCLIENT *LCFree)( void * );
    dig_fhandle         (DIGCLIENT *LCOpen)( const char *, dig_open );
    unsigned long       (DIGCLIENT *LCSeek)( dig_fhandle, unsigned long, dig_seek );
    unsigned            (DIGCLIENT *LCRead)( dig_fhandle, void *, unsigned );
    unsigned            (DIGCLIENT *LCWrite)( dig_fhandle, const void *, unsigned );
    void                (DIGCLIENT *LCClose)( dig_fhandle );
    void                (DIGCLIENT *LCRemove)( const char *path, dig_open flags );

    link_buffer         *(DIGCLIENT *LCBufferGet)( void *cookie, unsigned size );
    void                *(DIGCLIENT *LCBufferRel)( void *cookie, link_buffer *buffer );
    void                (DIGCLIENT *LCReceived)( void *cookie, link_buffer *data );
    void                (DIGCLIENT *LCState)( void *cookie, link_status ls, const link_message *msg );
} pill_client_routines;

typedef pill_imp_routines * DIGENTRY pill_init_func( pill_status *status, pill_client_routines *client );
#ifdef __WINDOWS__
typedef void DIGENTRY pill_fini_func( void );
#endif

void            *LCAlloc( size_t amount );
void            *LCRealloc( void *p, size_t amount );
void            LCFree( void *p );

dig_fhandle     LCOpen( const char *path, dig_open flags );
unsigned long   LCSeek( dig_fhandle h, unsigned long p, dig_seek w );
unsigned        LCRead( dig_fhandle h, void *b, unsigned s );
unsigned        LCWrite( dig_fhandle h, const void *b, unsigned s );
void            LCClose( dig_fhandle h );
void            LCRemove( const char *path, dig_open flags );

link_buffer     *LCBufferGet( link_instance *li, unsigned size );
void            *LCBufferRel( link_instance *li, link_buffer *buffer );
void            LCReceived( link_instance *li, link_buffer *data );
void            LCState( link_instance *li, link_status ls, link_message *msg );

#include "digunpck.h"

#endif

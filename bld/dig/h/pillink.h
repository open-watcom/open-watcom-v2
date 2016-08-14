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


#ifndef PILLINK_H
#define PILLINK_H

#include "digpck.h"

#include "piltypes.h"

#include "digunpck.h"

extern link_handle          *LinkLoad( const char *name, link_message *msg );
extern link_handle          *LinkRegister( const pill_imp_routines *rtns, link_message *msg );
extern link_instance        *LinkInit( link_handle *lh, void *cookie, link_trigger *tp, const char *parm );
extern link_status          LinkStatus( link_instance *li );
extern unsigned             LinkMaxSize( link_instance *li, unsigned req_size );
extern link_status          LinkPut( link_instance *li, link_buffer *data );
extern link_status          LinkPutMx( link_instance *li, unsigned num, const lmx_entry *put );
extern unsigned             LinkKicker( void );
extern link_status          LinkAbort( link_instance *li );
extern link_status          LinkFini( link_instance *li );
extern unsigned             LinkMessage( const link_message *msg, pil_language pl, unsigned max, char *buff );
extern pill_private_func    *LinkPrivate( link_handle *lh, const char *string );
extern void                 LinkUnload( link_handle *lh );

#endif

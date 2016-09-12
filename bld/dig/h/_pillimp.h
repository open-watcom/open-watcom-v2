/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2016-2016 The Open Watcom Contributors. All Rights Reserved.
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


pick( int,                  Load,       ( link_handle *, link_message * ) )
pick( void,                 Unload,     ( link_handle * ) )
pick( int,                  Init,       ( link_instance *, const char * ) )
pick( unsigned,             MaxSize,    ( link_instance *, unsigned ) )
pick( link_status,          Put,        ( link_instance *, link_buffer * ) )
pick( unsigned,             Kicker,     ( link_instance * ) )
pick( link_status,          Abort,      ( link_instance * ) )
pick( link_status,          Fini,       ( link_instance * ) )
pick( unsigned,             Message,    ( const link_message *, pil_language, unsigned, char * ) )
pick( pill_private_func *,  Private,    ( const char * ) )

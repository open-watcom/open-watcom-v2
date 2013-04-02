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


#ifndef _OBJAUTOD_H
#define _OBJAUTOD_H

#include <time.h>

typedef enum __ws {
    ADW_OK,
    ADW_NO_DEPS,
    ADW_RTN_STOPPED,
    ADW_FILE_NOT_FOUND,
    ADW_NOT_AN_OBJ,
    ADW_FILE_ERROR,
} walk_status;

typedef enum __rs {
    ADR_CONTINUE,
    ADR_STOP,
} rtn_status;

walk_status WalkOBJAutoDep( const char *file_name
                          , rtn_status (*rtn)( time_t, char *, void * )
                          , void *data );

walk_status WalkRESAutoDep( const char *file_name
                          , rtn_status (*rtn)( time_t, char *, void * )
                          , void *data );

#endif

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


#ifdef __cplusplus
extern "C" {
#endif

struct mapper_handle;

typedef struct mapper_handle    mapper_handle;
typedef unsigned short          mapper_id;

typedef enum {
    MWR_CONTINUE,
    MWR_STOP
} mapper_walk_res;

typedef mapper_walk_res mapper_walk_rtn( void *map_data, mapper_id map_id,
                                         void *data );
typedef void* mapper_alloc_rtn( unsigned size );
typedef void  mapper_free_rtn( void* );

#define MAPPER_NOMAP            ((mapper_id)-1)
/*
        Mapper routines.
*/
mapper_handle   *MapperCreate( mapper_id base, mapper_id max_num,
                               mapper_id size,
                               mapper_alloc_rtn *alloc, mapper_free_rtn *free );
void            MapperDestroy( mapper_handle *mh );
mapper_id       MapperAlloc( mapper_handle *mh );
void            MapperFree( mapper_handle *mh, mapper_id map_id );
void            *MapperGet( mapper_handle *mh, mapper_id map_id );
void            *MapperGetSafe( mapper_handle *mh, mapper_id map_id );
mapper_walk_res MapperWalk( mapper_handle *mh, mapper_walk_rtn *rtn,
                            void *data );

#ifdef __cplusplus
}
#endif

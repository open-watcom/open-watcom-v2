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


extern "C" {
    #include "mapper.h"

/*
     Client Supplied routines.
*/
void            *MapperCliAlloc( unsigned size );
void            MapperCliFree( void * );

}

template <class T>
class PointerMapper {
    public:

        typedef mapper_walk_res WalkRoutine( T *obj );

        PointerMapper()
        {
            _handle = NULL;
        }
        ~PointerMapper()
        {
            Destroy();
        }
        virtual Create( mapper_id base, mapper_id limit )
        {
            if( _handle != NULL ) MapperDestroy( _handle );
            _handle = MapperCreate( base, limit, sizeof( T* ),
                                    MapperCliAlloc, MapperCliFree );
        }
        virtual Create()
        {
            Create( 1, ~0 );
        }
        virtual Destroy()
        {
            if( _handle != NULL ) MapperDestroy( _handle );
            _handle = NULL;
        }
        static mapper_id NoId()
        {
            return( 0 );
        }
        T *GetPointer( mapper_id id )
        {
            T **pobj = (T**)MapperGet( _handle, id );
            return( *pobj );
        }
        T *GetSafePointer( mapper_id id )
        {
            T **pobj = (T**)MapperGetSafe( _handle, id );
            if( pobj == NULL ) return NULL;
            return( *pobj );
        }
        mapper_id GetId( T *obj )
        {
            _obj = obj;
            _id = 0;
            MapperWalk( _handle, ObjFind, (void *)this );
            return( _id );
        }
        mapper_id Add( T *obj )
        {
            mapper_id id;
            T           **pobj;

            id = MapperAlloc( _handle );
            pobj = (T**)MapperGet( _handle, id );
            *pobj = obj;
            return( id );
        }
        mapper_id FindOrAdd( T *obj )
        {
            mapper_id   id;

            id = GetId( obj );
            if( id != NoId() ) return( id );
            return( Add( obj ) );
        }
        void Remove( mapper_id id )
        {
            MapperFree( _handle, id );
        }
        void Remove( T *obj )
        {
            mapper_id id = GetId( obj );
            if( id != NoId() ) {
                Remove( id );
            }
        }
        void Walk( WalkRoutine *rtn )
        {
            _walker = rtn;
            MapperWalk( _handle, WalkHelper, (void *)this );
        }

        void Lookup( mapper_walk_rtn *rtn, void *data )
        {
            MapperWalk( _handle, rtn, data );
        }

    private:
        static mapper_walk_res ObjFind( void *map_data, mapper_id map_id, void *data )
        {
            PointerMapper<T> *self = (PointerMapper<T>*)data;
            if( *(T**)map_data == self->_obj ) {
                self->_id = map_id;
                return( MWR_STOP );
            }
            return( MWR_CONTINUE );
        }
        static mapper_walk_res WalkHelper( void *map_data, mapper_id map_id, void *data )
        {
            PointerMapper<T> *self = (PointerMapper<T>*)data;
            return( self->_walker( *(T**)map_data ) );
        }


    private:
        mapper_handle   *_handle;

        T               *_obj; // for lookups
        mapper_id       _id; // for lookups
        WalkRoutine     *_walker;
};

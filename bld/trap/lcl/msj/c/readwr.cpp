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


#include <windows.h>
#include "machtype.h"
#include "msjvdbg.h"
#include "msvmutil.h"
#include "readwr.h"
#include "madjvm.h"
#include "msjutil.h"
#include "control.h"
#include "olemap.hpp"

extern "C" {

    void *MapperCliAlloc( unsigned size )
    {
        return( MSJAlloc( size ) );
    }

    void MapperCliFree( void * p )
    {
        MSJFree( p );
    }
}


void InitMappings()
{
}

void FiniMappings()
{
}

unsigned JVMReadMethod(addr48_ptr *addr, void *buf, unsigned len, IUnknown *d)
{
    IRemoteMethodField *meth = (IRemoteMethodField*)d;
    ILockBytes *bytes = NULL;
    IEnumLINEINFO *lines = NULL;
    meth->GetBytes(&bytes);
    if( bytes == NULL ) return 0;
    ULARGE_INTEGER i;
    ULONG       amtRead;
    i.LowPart = addr->offset;
    i.HighPart = 0;
    bytes->ReadAt( i, buf, len, &amtRead );
    bytes->Release();
    return( amtRead );
}

static unsigned (*ReadJumpTab[])(addr48_ptr *, void *, unsigned, IUnknown *) =
{
    JVMReadMethod,
};


void CreateMapAddr( addr48_ptr *addr, addr_seg selector,
                    IUnknown *d, addr48_off offset )
/***************************************************/
{
    switch( selector ) {

    default:
        addr->segment = 0;
        addr->offset = 0;
        break;
    }
}

void MapFrameToPC( IRemoteStackFrame * frame, addr48_ptr *addr )
/**************************************************************/
{
    unsigned long               offpc;

    addr->segment = 0;
    addr->offset = 0;
    frame->GetPC( &offpc );
    IRemoteContainerObject *obj = NULL;
    frame->GetMethodObject( &obj );
    if( obj == NULL ) return;
    IRemoteField *field = NULL;
    field = NULL;
    obj->GetType( &field );
    obj->Release();
    if( field == NULL ) return;
    IRemoteMethodField *meth = NULL;
    field->QueryInterface( IID_IRemoteMethodField, (void**)&meth );
    if( meth == NULL ) {
        field->Release();
        return;
    }
    IRemoteContainerField *cField = NULL;
    field->GetContainer( &cField );
    field->Release();
    if( cField == NULL ) return;
    IRemoteClassField *cls = NULL;
    cField->QueryInterface( IID_IRemoteClassField, (void**)&cls );
    cField->Release();
    if( cls == NULL ) return;
    addr->segment = JVM_CODE_SELECTOR;
    addr->offset = MakeMethodOffset( (void*)cls, (void *)meth, offpc );
    cls->Release();
    if( addr->offset == -1 ) {
        addr->segment = 0;
        addr->offset = 0;
    }
}


IUnknown *FindObject( addr48_ptr *addr, ObjType *objtype )
/*******************************************************/
{
    switch( addr->segment ) {
    case JVM_CODE_SELECTOR:
        *objtype = OBJECT_CODE; // nyi
        return( (IUnknown*)GetMethodPointer( &addr->offset ) );
    default:
        *objtype = OBJECT_IDUNNO;
        return( NULL );
    }
}

static unsigned HandleRWRequest( addr48_ptr *addr, void *buf, unsigned len,
                                 unsigned (*tab[])(addr48_ptr *, void *,
                                                   unsigned, IUnknown *) )
/*****************************************************************************/
{
    ObjType     objtype;
    IUnknown *  obj;

    obj = FindObject( addr, &objtype );
    if( obj != NULL ) {
        return tab[objtype]( addr, buf, len, obj );
    }
    return 0;
}

#if 0

static unsigned (*WriteJumpTab[])(addr48_ptr *, void *, unsigned,
                                  IUnknown *) = {
    JVMWriteBoolean,
    JVMWriteByte,
    JVMWriteChar,
    JVMWriteDouble,
    JVMWriteFloat,
    JVMWriteInt,
    JVMWriteLong,
    JVMWriteShort,
    JVMWriteClass,
    JVMWriteArray,
    JVMWriteString
};

#endif

extern void GetPCFromThread( IRemoteThread *thd, addr48_ptr *addr )
/*****************************************************************/
{
    IRemoteStackFrame *         frame;

    addr->segment = 0;
    addr->offset = 0;
    if( thd == NULL ) return;
    thd->GetCurrentFrame( &frame );
    if( frame == NULL ) return;
    MapFrameToPC( frame, addr );
    frame->Release();
}


extern "C" {

extern unsigned ReadMemory( addr48_ptr *addr, void *buf, unsigned len )
/*********************************************************************/
{
    return HandleRWRequest( addr, buf, len, ReadJumpTab );
}

extern unsigned WriteMemory( addr48_ptr *addr, void *buf, unsigned len )
/**********************************************************************/
{
    addr=addr;buf=buf;len=len;return 0;
//    return HandleRWRequest( addr, buf, len, WriteJumpTab );
}

extern void GetPC( addr48_ptr *addr )
/***********************************/
{
    GetPCFromThread( CurrThread, addr );
}

} /* end extern "C" */

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


/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 2.00.0102 */
/* at Wed Aug 07 01:12:35 1996
 */
//@@MIDL_FILE_HEADING(  )
#include "rpc.h"
#include "rpcndr.h"
#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __javadbg_h__
#define __javadbg_h__

#ifdef __cplusplus
extern "C"{
#endif

/* Forward Declarations */

#ifndef __IEnumLINEINFO_FWD_DEFINED__
#define __IEnumLINEINFO_FWD_DEFINED__
typedef interface IEnumLINEINFO IEnumLINEINFO;
#endif  /* __IEnumLINEINFO_FWD_DEFINED__ */


#ifndef __IRemoteField_FWD_DEFINED__
#define __IRemoteField_FWD_DEFINED__
typedef interface IRemoteField IRemoteField;
#endif  /* __IRemoteField_FWD_DEFINED__ */


#ifndef __IEnumRemoteField_FWD_DEFINED__
#define __IEnumRemoteField_FWD_DEFINED__
typedef interface IEnumRemoteField IEnumRemoteField;
#endif  /* __IEnumRemoteField_FWD_DEFINED__ */


#ifndef __IRemoteDataField_FWD_DEFINED__
#define __IRemoteDataField_FWD_DEFINED__
typedef interface IRemoteDataField IRemoteDataField;
#endif  /* __IRemoteDataField_FWD_DEFINED__ */


#ifndef __IRemoteArrayField_FWD_DEFINED__
#define __IRemoteArrayField_FWD_DEFINED__
typedef interface IRemoteArrayField IRemoteArrayField;
#endif  /* __IRemoteArrayField_FWD_DEFINED__ */


#ifndef __IRemoteContainerField_FWD_DEFINED__
#define __IRemoteContainerField_FWD_DEFINED__
typedef interface IRemoteContainerField IRemoteContainerField;
#endif  /* __IRemoteContainerField_FWD_DEFINED__ */


#ifndef __IRemoteMethodField_FWD_DEFINED__
#define __IRemoteMethodField_FWD_DEFINED__
typedef interface IRemoteMethodField IRemoteMethodField;
#endif  /* __IRemoteMethodField_FWD_DEFINED__ */


#ifndef __IRemoteClassField_FWD_DEFINED__
#define __IRemoteClassField_FWD_DEFINED__
typedef interface IRemoteClassField IRemoteClassField;
#endif  /* __IRemoteClassField_FWD_DEFINED__ */


#ifndef __IRemoteObject_FWD_DEFINED__
#define __IRemoteObject_FWD_DEFINED__
typedef interface IRemoteObject IRemoteObject;
#endif  /* __IRemoteObject_FWD_DEFINED__ */


#ifndef __IEnumRemoteObject_FWD_DEFINED__
#define __IEnumRemoteObject_FWD_DEFINED__
typedef interface IEnumRemoteObject IEnumRemoteObject;
#endif  /* __IEnumRemoteObject_FWD_DEFINED__ */


#ifndef __IEnumRemoteValue_FWD_DEFINED__
#define __IEnumRemoteValue_FWD_DEFINED__
typedef interface IEnumRemoteValue IEnumRemoteValue;
#endif  /* __IEnumRemoteValue_FWD_DEFINED__ */


#ifndef __IEnumRemoteBooleanValue_FWD_DEFINED__
#define __IEnumRemoteBooleanValue_FWD_DEFINED__
typedef interface IEnumRemoteBooleanValue IEnumRemoteBooleanValue;
#endif  /* __IEnumRemoteBooleanValue_FWD_DEFINED__ */


#ifndef __IEnumRemoteByteValue_FWD_DEFINED__
#define __IEnumRemoteByteValue_FWD_DEFINED__
typedef interface IEnumRemoteByteValue IEnumRemoteByteValue;
#endif  /* __IEnumRemoteByteValue_FWD_DEFINED__ */


#ifndef __IEnumRemoteCharValue_FWD_DEFINED__
#define __IEnumRemoteCharValue_FWD_DEFINED__
typedef interface IEnumRemoteCharValue IEnumRemoteCharValue;
#endif  /* __IEnumRemoteCharValue_FWD_DEFINED__ */


#ifndef __IEnumRemoteDoubleValue_FWD_DEFINED__
#define __IEnumRemoteDoubleValue_FWD_DEFINED__
typedef interface IEnumRemoteDoubleValue IEnumRemoteDoubleValue;
#endif  /* __IEnumRemoteDoubleValue_FWD_DEFINED__ */


#ifndef __IEnumRemoteFloatValue_FWD_DEFINED__
#define __IEnumRemoteFloatValue_FWD_DEFINED__
typedef interface IEnumRemoteFloatValue IEnumRemoteFloatValue;
#endif  /* __IEnumRemoteFloatValue_FWD_DEFINED__ */


#ifndef __IEnumRemoteIntValue_FWD_DEFINED__
#define __IEnumRemoteIntValue_FWD_DEFINED__
typedef interface IEnumRemoteIntValue IEnumRemoteIntValue;
#endif  /* __IEnumRemoteIntValue_FWD_DEFINED__ */


#ifndef __IEnumRemoteLongValue_FWD_DEFINED__
#define __IEnumRemoteLongValue_FWD_DEFINED__
typedef interface IEnumRemoteLongValue IEnumRemoteLongValue;
#endif  /* __IEnumRemoteLongValue_FWD_DEFINED__ */


#ifndef __IEnumRemoteShortValue_FWD_DEFINED__
#define __IEnumRemoteShortValue_FWD_DEFINED__
typedef interface IEnumRemoteShortValue IEnumRemoteShortValue;
#endif  /* __IEnumRemoteShortValue_FWD_DEFINED__ */


#ifndef __IRemoteArrayObject_FWD_DEFINED__
#define __IRemoteArrayObject_FWD_DEFINED__
typedef interface IRemoteArrayObject IRemoteArrayObject;
#endif  /* __IRemoteArrayObject_FWD_DEFINED__ */


#ifndef __IRemoteBooleanObject_FWD_DEFINED__
#define __IRemoteBooleanObject_FWD_DEFINED__
typedef interface IRemoteBooleanObject IRemoteBooleanObject;
#endif  /* __IRemoteBooleanObject_FWD_DEFINED__ */


#ifndef __IRemoteByteObject_FWD_DEFINED__
#define __IRemoteByteObject_FWD_DEFINED__
typedef interface IRemoteByteObject IRemoteByteObject;
#endif  /* __IRemoteByteObject_FWD_DEFINED__ */


#ifndef __IRemoteCharObject_FWD_DEFINED__
#define __IRemoteCharObject_FWD_DEFINED__
typedef interface IRemoteCharObject IRemoteCharObject;
#endif  /* __IRemoteCharObject_FWD_DEFINED__ */


#ifndef __IRemoteContainerObject_FWD_DEFINED__
#define __IRemoteContainerObject_FWD_DEFINED__
typedef interface IRemoteContainerObject IRemoteContainerObject;
#endif  /* __IRemoteContainerObject_FWD_DEFINED__ */


#ifndef __IRemoteClassObject_FWD_DEFINED__
#define __IRemoteClassObject_FWD_DEFINED__
typedef interface IRemoteClassObject IRemoteClassObject;
#endif  /* __IRemoteClassObject_FWD_DEFINED__ */


#ifndef __IRemoteDoubleObject_FWD_DEFINED__
#define __IRemoteDoubleObject_FWD_DEFINED__
typedef interface IRemoteDoubleObject IRemoteDoubleObject;
#endif  /* __IRemoteDoubleObject_FWD_DEFINED__ */


#ifndef __IRemoteFloatObject_FWD_DEFINED__
#define __IRemoteFloatObject_FWD_DEFINED__
typedef interface IRemoteFloatObject IRemoteFloatObject;
#endif  /* __IRemoteFloatObject_FWD_DEFINED__ */


#ifndef __IRemoteIntObject_FWD_DEFINED__
#define __IRemoteIntObject_FWD_DEFINED__
typedef interface IRemoteIntObject IRemoteIntObject;
#endif  /* __IRemoteIntObject_FWD_DEFINED__ */


#ifndef __IRemoteLongObject_FWD_DEFINED__
#define __IRemoteLongObject_FWD_DEFINED__
typedef interface IRemoteLongObject IRemoteLongObject;
#endif  /* __IRemoteLongObject_FWD_DEFINED__ */


#ifndef __IRemoteShortObject_FWD_DEFINED__
#define __IRemoteShortObject_FWD_DEFINED__
typedef interface IRemoteShortObject IRemoteShortObject;
#endif  /* __IRemoteShortObject_FWD_DEFINED__ */


#ifndef __IRemoteStackFrame_FWD_DEFINED__
#define __IRemoteStackFrame_FWD_DEFINED__
typedef interface IRemoteStackFrame IRemoteStackFrame;
#endif  /* __IRemoteStackFrame_FWD_DEFINED__ */


#ifndef __IRemoteThreadGroup_FWD_DEFINED__
#define __IRemoteThreadGroup_FWD_DEFINED__
typedef interface IRemoteThreadGroup IRemoteThreadGroup;
#endif  /* __IRemoteThreadGroup_FWD_DEFINED__ */


#ifndef __IEnumRemoteThreadGroup_FWD_DEFINED__
#define __IEnumRemoteThreadGroup_FWD_DEFINED__
typedef interface IEnumRemoteThreadGroup IEnumRemoteThreadGroup;
#endif  /* __IEnumRemoteThreadGroup_FWD_DEFINED__ */


#ifndef __IRemoteThread_FWD_DEFINED__
#define __IRemoteThread_FWD_DEFINED__
typedef interface IRemoteThread IRemoteThread;
#endif  /* __IRemoteThread_FWD_DEFINED__ */


#ifndef __IEnumRemoteThread_FWD_DEFINED__
#define __IEnumRemoteThread_FWD_DEFINED__
typedef interface IEnumRemoteThread IEnumRemoteThread;
#endif  /* __IEnumRemoteThread_FWD_DEFINED__ */


#ifndef __IRemoteProcessCallback_FWD_DEFINED__
#define __IRemoteProcessCallback_FWD_DEFINED__
typedef interface IRemoteProcessCallback IRemoteProcessCallback;
#endif  /* __IRemoteProcessCallback_FWD_DEFINED__ */


#ifndef __IRemoteProcess_FWD_DEFINED__
#define __IRemoteProcess_FWD_DEFINED__
typedef interface IRemoteProcess IRemoteProcess;
#endif  /* __IRemoteProcess_FWD_DEFINED__ */


#ifndef __IEnumRemoteProcess_FWD_DEFINED__
#define __IEnumRemoteProcess_FWD_DEFINED__
typedef interface IEnumRemoteProcess IEnumRemoteProcess;
#endif  /* __IEnumRemoteProcess_FWD_DEFINED__ */


#ifndef __IRemoteDebugManagerCallback_FWD_DEFINED__
#define __IRemoteDebugManagerCallback_FWD_DEFINED__
typedef interface IRemoteDebugManagerCallback IRemoteDebugManagerCallback;
#endif  /* __IRemoteDebugManagerCallback_FWD_DEFINED__ */


#ifndef __IRemoteDebugManager_FWD_DEFINED__
#define __IRemoteDebugManager_FWD_DEFINED__
typedef interface IRemoteDebugManager IRemoteDebugManager;
#endif  /* __IRemoteDebugManager_FWD_DEFINED__ */


#ifndef __IJavaDebugManager_FWD_DEFINED__
#define __IJavaDebugManager_FWD_DEFINED__
typedef interface IJavaDebugManager IJavaDebugManager;
#endif  /* __IJavaDebugManager_FWD_DEFINED__ */


/* header files for imported files */
#include "oleidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * );

/****************************************
 * Generated header for interface: __MIDL__intf_0000
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [local] */


                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

                        /* size is 0 */

// error codes
//
// errors returned by IRemoteContainer::GetFieldObject
static const int E_FIELDOUTOFSCOPE       = MAKE_HRESULT(1, FACILITY_ITF, 0x01);
static const int E_FIELDNOTINOBJECT      = MAKE_HRESULT(1, FACILITY_ITF, 0x02);
static const int E_NOFIELDS              = MAKE_HRESULT(1, FACILITY_ITF, 0x03);
static const int E_NULLOBJECTREF         = MAKE_HRESULT(1, FACILITY_ITF, 0x04);
// errors returned by IRemoteProcess::FindClass
static const int E_CLASSNOTFOUND         = MAKE_HRESULT(1, FACILITY_ITF, 0x10);
static const int E_BADMETHOD             = MAKE_HRESULT(1, FACILITY_ITF, 0x20);


extern RPC_IF_HANDLE __MIDL__intf_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL__intf_0000_v0_0_s_ifspec;

#ifndef __IEnumLINEINFO_INTERFACE_DEFINED__
#define __IEnumLINEINFO_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IEnumLINEINFO
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IEnumLINEINFO __RPC_FAR *LPENUMLINEINFO;

                        /* size is 4 */
typedef struct  tagLINEINFO
    {
    USHORT offPC;
    USHORT iLine;
    }   LINEINFO;

                        /* size is 4 */
typedef struct tagLINEINFO __RPC_FAR *LPLINEINFO;


EXTERN_C const IID IID_IEnumLINEINFO;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IEnumLINEINFO : public IUnknown
    {
    public:
        virtual HRESULT __stdcall Next(
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ LPLINEINFO rgelt,
            /* [out] */ ULONG __RPC_FAR *pceltFetched) = 0;

        virtual HRESULT __stdcall Skip(
            /* [in] */ ULONG celt) = 0;

        virtual HRESULT __stdcall Reset( void) = 0;

        virtual HRESULT __stdcall Clone(
            /* [out] */ IEnumLINEINFO __RPC_FAR *__RPC_FAR *ppEnum) = 0;

        virtual HRESULT __stdcall GetCount(
            /* [out] */ ULONG __RPC_FAR *pcelt) = 0;

    };

#else   /* C style interface */

    typedef struct IEnumLINEINFOVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IEnumLINEINFO __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IEnumLINEINFO __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IEnumLINEINFO __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Next )(
            IEnumLINEINFO __RPC_FAR * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ LPLINEINFO rgelt,
            /* [out] */ ULONG __RPC_FAR *pceltFetched);

        HRESULT ( __stdcall __RPC_FAR *Skip )(
            IEnumLINEINFO __RPC_FAR * This,
            /* [in] */ ULONG celt);

        HRESULT ( __stdcall __RPC_FAR *Reset )(
            IEnumLINEINFO __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Clone )(
            IEnumLINEINFO __RPC_FAR * This,
            /* [out] */ IEnumLINEINFO __RPC_FAR *__RPC_FAR *ppEnum);

        HRESULT ( __stdcall __RPC_FAR *GetCount )(
            IEnumLINEINFO __RPC_FAR * This,
            /* [out] */ ULONG __RPC_FAR *pcelt);

    } IEnumLINEINFOVtbl;

    interface IEnumLINEINFO
    {
        CONST_VTBL struct IEnumLINEINFOVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IEnumLINEINFO_QueryInterface(This,riid,ppvObject)       \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumLINEINFO_AddRef(This)      \
    (This)->lpVtbl -> AddRef(This)

#define IEnumLINEINFO_Release(This)     \
    (This)->lpVtbl -> Release(This)


#define IEnumLINEINFO_Next(This,celt,rgelt,pceltFetched)        \
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumLINEINFO_Skip(This,celt)   \
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumLINEINFO_Reset(This)       \
    (This)->lpVtbl -> Reset(This)

#define IEnumLINEINFO_Clone(This,ppEnum)        \
    (This)->lpVtbl -> Clone(This,ppEnum)

#define IEnumLINEINFO_GetCount(This,pcelt)      \
    (This)->lpVtbl -> GetCount(This,pcelt)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IEnumLINEINFO_Next_Proxy(
    IEnumLINEINFO __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ LPLINEINFO rgelt,
    /* [out] */ ULONG __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumLINEINFO_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumLINEINFO_Skip_Proxy(
    IEnumLINEINFO __RPC_FAR * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumLINEINFO_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumLINEINFO_Reset_Proxy(
    IEnumLINEINFO __RPC_FAR * This);


void __RPC_STUB IEnumLINEINFO_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumLINEINFO_Clone_Proxy(
    IEnumLINEINFO __RPC_FAR * This,
    /* [out] */ IEnumLINEINFO __RPC_FAR *__RPC_FAR *ppEnum);


void __RPC_STUB IEnumLINEINFO_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumLINEINFO_GetCount_Proxy(
    IEnumLINEINFO __RPC_FAR * This,
    /* [out] */ ULONG __RPC_FAR *pcelt);


void __RPC_STUB IEnumLINEINFO_GetCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IEnumLINEINFO_INTERFACE_DEFINED__ */


#ifndef __IRemoteField_INTERFACE_DEFINED__
#define __IRemoteField_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IRemoteField
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IRemoteField __RPC_FAR *LPREMOTEFIELD;

                        /* size is 2 */

enum __MIDL_IRemoteField_0001
    {   FIELD_KIND_DATA_OBJECT  = 0x1,
        FIELD_KIND_DATA_PRIMITIVE       = 0x2,
        FIELD_KIND_ARRAY        = 0x4,
        FIELD_KIND_CLASS        = 0x8,
        FIELD_KIND_METHOD       = 0x10,
        FIELD_KIND_LOCAL        = 0x1000,
        FIELD_KIND_PARAM        = 0x2000,
        FIELD_KIND_THIS = 0x4000
    };
                        /* size is 4 */
typedef ULONG FIELDKIND;

                        /* size is 2 */

enum __MIDL_IRemoteField_0002
    {   FIELD_ACC_PUBLIC        = 0x1,
        FIELD_ACC_PRIVATE       = 0x2,
        FIELD_ACC_PROTECTED     = 0x4,
        FIELD_ACC_STATIC        = 0x8,
        FIELD_ACC_FINAL = 0x10,
        FIELD_ACC_SYNCHRONIZED  = 0x20,
        FIELD_ACC_VOLATILE      = 0x40,
        FIELD_ACC_TRANSIENT     = 0x80,
        FIELD_ACC_NATIVE        = 0x100,
        FIELD_ACC_INTERFACE     = 0x200,
        FIELD_ACC_ABSTRACT      = 0x400
    };
                        /* size is 4 */
typedef ULONG FIELDMODIFIERS;


EXTERN_C const IID IID_IRemoteField;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IRemoteField : public IUnknown
    {
    public:
        virtual HRESULT __stdcall GetName(
            /* [out] */ LPOLESTR __RPC_FAR *ppszName) = 0;

        virtual HRESULT __stdcall GetKind(
            /* [out] */ FIELDKIND __RPC_FAR *pfk) = 0;

        virtual HRESULT __stdcall GetType(
            /* [out] */ IRemoteField __RPC_FAR *__RPC_FAR *ppType) = 0;

        virtual HRESULT __stdcall GetContainer(
            /* [out] */ IRemoteContainerField __RPC_FAR *__RPC_FAR *ppContainer) = 0;

        virtual HRESULT __stdcall GetModifiers(
            /* [out] */ FIELDMODIFIERS __RPC_FAR *pulModifiers) = 0;

    };

#else   /* C style interface */

    typedef struct IRemoteFieldVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IRemoteField __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IRemoteField __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IRemoteField __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetName )(
            IRemoteField __RPC_FAR * This,
            /* [out] */ LPOLESTR __RPC_FAR *ppszName);

        HRESULT ( __stdcall __RPC_FAR *GetKind )(
            IRemoteField __RPC_FAR * This,
            /* [out] */ FIELDKIND __RPC_FAR *pfk);

        HRESULT ( __stdcall __RPC_FAR *GetType )(
            IRemoteField __RPC_FAR * This,
            /* [out] */ IRemoteField __RPC_FAR *__RPC_FAR *ppType);

        HRESULT ( __stdcall __RPC_FAR *GetContainer )(
            IRemoteField __RPC_FAR * This,
            /* [out] */ IRemoteContainerField __RPC_FAR *__RPC_FAR *ppContainer);

        HRESULT ( __stdcall __RPC_FAR *GetModifiers )(
            IRemoteField __RPC_FAR * This,
            /* [out] */ FIELDMODIFIERS __RPC_FAR *pulModifiers);

    } IRemoteFieldVtbl;

    interface IRemoteField
    {
        CONST_VTBL struct IRemoteFieldVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IRemoteField_QueryInterface(This,riid,ppvObject)        \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRemoteField_AddRef(This)       \
    (This)->lpVtbl -> AddRef(This)

#define IRemoteField_Release(This)      \
    (This)->lpVtbl -> Release(This)


#define IRemoteField_GetName(This,ppszName)     \
    (This)->lpVtbl -> GetName(This,ppszName)

#define IRemoteField_GetKind(This,pfk)  \
    (This)->lpVtbl -> GetKind(This,pfk)

#define IRemoteField_GetType(This,ppType)       \
    (This)->lpVtbl -> GetType(This,ppType)

#define IRemoteField_GetContainer(This,ppContainer)     \
    (This)->lpVtbl -> GetContainer(This,ppContainer)

#define IRemoteField_GetModifiers(This,pulModifiers)    \
    (This)->lpVtbl -> GetModifiers(This,pulModifiers)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IRemoteField_GetName_Proxy(
    IRemoteField __RPC_FAR * This,
    /* [out] */ LPOLESTR __RPC_FAR *ppszName);


void __RPC_STUB IRemoteField_GetName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteField_GetKind_Proxy(
    IRemoteField __RPC_FAR * This,
    /* [out] */ FIELDKIND __RPC_FAR *pfk);


void __RPC_STUB IRemoteField_GetKind_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteField_GetType_Proxy(
    IRemoteField __RPC_FAR * This,
    /* [out] */ IRemoteField __RPC_FAR *__RPC_FAR *ppType);


void __RPC_STUB IRemoteField_GetType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteField_GetContainer_Proxy(
    IRemoteField __RPC_FAR * This,
    /* [out] */ IRemoteContainerField __RPC_FAR *__RPC_FAR *ppContainer);


void __RPC_STUB IRemoteField_GetContainer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteField_GetModifiers_Proxy(
    IRemoteField __RPC_FAR * This,
    /* [out] */ FIELDMODIFIERS __RPC_FAR *pulModifiers);


void __RPC_STUB IRemoteField_GetModifiers_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IRemoteField_INTERFACE_DEFINED__ */


#ifndef __IEnumRemoteField_INTERFACE_DEFINED__
#define __IEnumRemoteField_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IEnumRemoteField
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IEnumRemoteField __RPC_FAR *LPENUMREMOTEFIELD;


EXTERN_C const IID IID_IEnumRemoteField;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IEnumRemoteField : public IUnknown
    {
    public:
        virtual HRESULT __stdcall Next(
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IRemoteField __RPC_FAR *__RPC_FAR rgelt[  ],
            /* [out] */ ULONG __RPC_FAR *pceltFetched) = 0;

        virtual HRESULT __stdcall Skip(
            /* [in] */ ULONG celt) = 0;

        virtual HRESULT __stdcall Reset( void) = 0;

        virtual HRESULT __stdcall Clone(
            /* [out] */ IEnumRemoteField __RPC_FAR *__RPC_FAR *ppEnum) = 0;

        virtual HRESULT __stdcall GetCount(
            /* [out] */ ULONG __RPC_FAR *pcelt) = 0;

    };

#else   /* C style interface */

    typedef struct IEnumRemoteFieldVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IEnumRemoteField __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IEnumRemoteField __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IEnumRemoteField __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Next )(
            IEnumRemoteField __RPC_FAR * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IRemoteField __RPC_FAR *__RPC_FAR rgelt[  ],
            /* [out] */ ULONG __RPC_FAR *pceltFetched);

        HRESULT ( __stdcall __RPC_FAR *Skip )(
            IEnumRemoteField __RPC_FAR * This,
            /* [in] */ ULONG celt);

        HRESULT ( __stdcall __RPC_FAR *Reset )(
            IEnumRemoteField __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Clone )(
            IEnumRemoteField __RPC_FAR * This,
            /* [out] */ IEnumRemoteField __RPC_FAR *__RPC_FAR *ppEnum);

        HRESULT ( __stdcall __RPC_FAR *GetCount )(
            IEnumRemoteField __RPC_FAR * This,
            /* [out] */ ULONG __RPC_FAR *pcelt);

    } IEnumRemoteFieldVtbl;

    interface IEnumRemoteField
    {
        CONST_VTBL struct IEnumRemoteFieldVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IEnumRemoteField_QueryInterface(This,riid,ppvObject)    \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumRemoteField_AddRef(This)   \
    (This)->lpVtbl -> AddRef(This)

#define IEnumRemoteField_Release(This)  \
    (This)->lpVtbl -> Release(This)


#define IEnumRemoteField_Next(This,celt,rgelt,pceltFetched)     \
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumRemoteField_Skip(This,celt)        \
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumRemoteField_Reset(This)    \
    (This)->lpVtbl -> Reset(This)

#define IEnumRemoteField_Clone(This,ppEnum)     \
    (This)->lpVtbl -> Clone(This,ppEnum)

#define IEnumRemoteField_GetCount(This,pcelt)   \
    (This)->lpVtbl -> GetCount(This,pcelt)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IEnumRemoteField_Next_Proxy(
    IEnumRemoteField __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ IRemoteField __RPC_FAR *__RPC_FAR rgelt[  ],
    /* [out] */ ULONG __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumRemoteField_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumRemoteField_Skip_Proxy(
    IEnumRemoteField __RPC_FAR * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumRemoteField_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumRemoteField_Reset_Proxy(
    IEnumRemoteField __RPC_FAR * This);


void __RPC_STUB IEnumRemoteField_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumRemoteField_Clone_Proxy(
    IEnumRemoteField __RPC_FAR * This,
    /* [out] */ IEnumRemoteField __RPC_FAR *__RPC_FAR *ppEnum);


void __RPC_STUB IEnumRemoteField_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumRemoteField_GetCount_Proxy(
    IEnumRemoteField __RPC_FAR * This,
    /* [out] */ ULONG __RPC_FAR *pcelt);


void __RPC_STUB IEnumRemoteField_GetCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IEnumRemoteField_INTERFACE_DEFINED__ */


#ifndef __IRemoteDataField_INTERFACE_DEFINED__
#define __IRemoteDataField_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IRemoteDataField
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IRemoteDataField __RPC_FAR *LPREMOTEDATAFIELD;


EXTERN_C const IID IID_IRemoteDataField;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IRemoteDataField : public IRemoteField
    {
    public:
    };

#else   /* C style interface */

    typedef struct IRemoteDataFieldVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IRemoteDataField __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IRemoteDataField __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IRemoteDataField __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetName )(
            IRemoteDataField __RPC_FAR * This,
            /* [out] */ LPOLESTR __RPC_FAR *ppszName);

        HRESULT ( __stdcall __RPC_FAR *GetKind )(
            IRemoteDataField __RPC_FAR * This,
            /* [out] */ FIELDKIND __RPC_FAR *pfk);

        HRESULT ( __stdcall __RPC_FAR *GetType )(
            IRemoteDataField __RPC_FAR * This,
            /* [out] */ IRemoteField __RPC_FAR *__RPC_FAR *ppType);

        HRESULT ( __stdcall __RPC_FAR *GetContainer )(
            IRemoteDataField __RPC_FAR * This,
            /* [out] */ IRemoteContainerField __RPC_FAR *__RPC_FAR *ppContainer);

        HRESULT ( __stdcall __RPC_FAR *GetModifiers )(
            IRemoteDataField __RPC_FAR * This,
            /* [out] */ FIELDMODIFIERS __RPC_FAR *pulModifiers);

    } IRemoteDataFieldVtbl;

    interface IRemoteDataField
    {
        CONST_VTBL struct IRemoteDataFieldVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IRemoteDataField_QueryInterface(This,riid,ppvObject)    \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRemoteDataField_AddRef(This)   \
    (This)->lpVtbl -> AddRef(This)

#define IRemoteDataField_Release(This)  \
    (This)->lpVtbl -> Release(This)


#define IRemoteDataField_GetName(This,ppszName) \
    (This)->lpVtbl -> GetName(This,ppszName)

#define IRemoteDataField_GetKind(This,pfk)      \
    (This)->lpVtbl -> GetKind(This,pfk)

#define IRemoteDataField_GetType(This,ppType)   \
    (This)->lpVtbl -> GetType(This,ppType)

#define IRemoteDataField_GetContainer(This,ppContainer) \
    (This)->lpVtbl -> GetContainer(This,ppContainer)

#define IRemoteDataField_GetModifiers(This,pulModifiers)        \
    (This)->lpVtbl -> GetModifiers(This,pulModifiers)


#endif /* COBJMACROS */


#endif  /* C style interface */




#endif  /* __IRemoteDataField_INTERFACE_DEFINED__ */


#ifndef __IRemoteArrayField_INTERFACE_DEFINED__
#define __IRemoteArrayField_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IRemoteArrayField
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IRemoteArrayField __RPC_FAR *LPREMOTEARRAYFIELD;


EXTERN_C const IID IID_IRemoteArrayField;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IRemoteArrayField : public IRemoteDataField
    {
    public:
        virtual HRESULT __stdcall GetSize(
            /* [out] */ ULONG __RPC_FAR *pcElements) = 0;

    };

#else   /* C style interface */

    typedef struct IRemoteArrayFieldVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IRemoteArrayField __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IRemoteArrayField __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IRemoteArrayField __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetName )(
            IRemoteArrayField __RPC_FAR * This,
            /* [out] */ LPOLESTR __RPC_FAR *ppszName);

        HRESULT ( __stdcall __RPC_FAR *GetKind )(
            IRemoteArrayField __RPC_FAR * This,
            /* [out] */ FIELDKIND __RPC_FAR *pfk);

        HRESULT ( __stdcall __RPC_FAR *GetType )(
            IRemoteArrayField __RPC_FAR * This,
            /* [out] */ IRemoteField __RPC_FAR *__RPC_FAR *ppType);

        HRESULT ( __stdcall __RPC_FAR *GetContainer )(
            IRemoteArrayField __RPC_FAR * This,
            /* [out] */ IRemoteContainerField __RPC_FAR *__RPC_FAR *ppContainer);

        HRESULT ( __stdcall __RPC_FAR *GetModifiers )(
            IRemoteArrayField __RPC_FAR * This,
            /* [out] */ FIELDMODIFIERS __RPC_FAR *pulModifiers);

        HRESULT ( __stdcall __RPC_FAR *GetSize )(
            IRemoteArrayField __RPC_FAR * This,
            /* [out] */ ULONG __RPC_FAR *pcElements);

    } IRemoteArrayFieldVtbl;

    interface IRemoteArrayField
    {
        CONST_VTBL struct IRemoteArrayFieldVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IRemoteArrayField_QueryInterface(This,riid,ppvObject)   \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRemoteArrayField_AddRef(This)  \
    (This)->lpVtbl -> AddRef(This)

#define IRemoteArrayField_Release(This) \
    (This)->lpVtbl -> Release(This)


#define IRemoteArrayField_GetName(This,ppszName)        \
    (This)->lpVtbl -> GetName(This,ppszName)

#define IRemoteArrayField_GetKind(This,pfk)     \
    (This)->lpVtbl -> GetKind(This,pfk)

#define IRemoteArrayField_GetType(This,ppType)  \
    (This)->lpVtbl -> GetType(This,ppType)

#define IRemoteArrayField_GetContainer(This,ppContainer)        \
    (This)->lpVtbl -> GetContainer(This,ppContainer)

#define IRemoteArrayField_GetModifiers(This,pulModifiers)       \
    (This)->lpVtbl -> GetModifiers(This,pulModifiers)



#define IRemoteArrayField_GetSize(This,pcElements)      \
    (This)->lpVtbl -> GetSize(This,pcElements)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IRemoteArrayField_GetSize_Proxy(
    IRemoteArrayField __RPC_FAR * This,
    /* [out] */ ULONG __RPC_FAR *pcElements);


void __RPC_STUB IRemoteArrayField_GetSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IRemoteArrayField_INTERFACE_DEFINED__ */


#ifndef __IRemoteContainerField_INTERFACE_DEFINED__
#define __IRemoteContainerField_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IRemoteContainerField
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IRemoteContainerField __RPC_FAR *LPREMOTECONTAINERFIELD;


EXTERN_C const IID IID_IRemoteContainerField;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IRemoteContainerField : public IRemoteField
    {
    public:
        virtual HRESULT __stdcall GetFields(
            /* [out] */ IEnumRemoteField __RPC_FAR *__RPC_FAR *ppEnum,
            /* [in] */ FIELDKIND ulKind,
            /* [in] */ FIELDMODIFIERS ulModifiers,
            /* [unique][in] */ LPCOLESTR lpcszName) = 0;

    };

#else   /* C style interface */

    typedef struct IRemoteContainerFieldVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IRemoteContainerField __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IRemoteContainerField __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IRemoteContainerField __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetName )(
            IRemoteContainerField __RPC_FAR * This,
            /* [out] */ LPOLESTR __RPC_FAR *ppszName);

        HRESULT ( __stdcall __RPC_FAR *GetKind )(
            IRemoteContainerField __RPC_FAR * This,
            /* [out] */ FIELDKIND __RPC_FAR *pfk);

        HRESULT ( __stdcall __RPC_FAR *GetType )(
            IRemoteContainerField __RPC_FAR * This,
            /* [out] */ IRemoteField __RPC_FAR *__RPC_FAR *ppType);

        HRESULT ( __stdcall __RPC_FAR *GetContainer )(
            IRemoteContainerField __RPC_FAR * This,
            /* [out] */ IRemoteContainerField __RPC_FAR *__RPC_FAR *ppContainer);

        HRESULT ( __stdcall __RPC_FAR *GetModifiers )(
            IRemoteContainerField __RPC_FAR * This,
            /* [out] */ FIELDMODIFIERS __RPC_FAR *pulModifiers);

        HRESULT ( __stdcall __RPC_FAR *GetFields )(
            IRemoteContainerField __RPC_FAR * This,
            /* [out] */ IEnumRemoteField __RPC_FAR *__RPC_FAR *ppEnum,
            /* [in] */ FIELDKIND ulKind,
            /* [in] */ FIELDMODIFIERS ulModifiers,
            /* [unique][in] */ LPCOLESTR lpcszName);

    } IRemoteContainerFieldVtbl;

    interface IRemoteContainerField
    {
        CONST_VTBL struct IRemoteContainerFieldVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IRemoteContainerField_QueryInterface(This,riid,ppvObject)       \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRemoteContainerField_AddRef(This)      \
    (This)->lpVtbl -> AddRef(This)

#define IRemoteContainerField_Release(This)     \
    (This)->lpVtbl -> Release(This)


#define IRemoteContainerField_GetName(This,ppszName)    \
    (This)->lpVtbl -> GetName(This,ppszName)

#define IRemoteContainerField_GetKind(This,pfk) \
    (This)->lpVtbl -> GetKind(This,pfk)

#define IRemoteContainerField_GetType(This,ppType)      \
    (This)->lpVtbl -> GetType(This,ppType)

#define IRemoteContainerField_GetContainer(This,ppContainer)    \
    (This)->lpVtbl -> GetContainer(This,ppContainer)

#define IRemoteContainerField_GetModifiers(This,pulModifiers)   \
    (This)->lpVtbl -> GetModifiers(This,pulModifiers)


#define IRemoteContainerField_GetFields(This,ppEnum,ulKind,ulModifiers,lpcszName)       \
    (This)->lpVtbl -> GetFields(This,ppEnum,ulKind,ulModifiers,lpcszName)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IRemoteContainerField_GetFields_Proxy(
    IRemoteContainerField __RPC_FAR * This,
    /* [out] */ IEnumRemoteField __RPC_FAR *__RPC_FAR *ppEnum,
    /* [in] */ FIELDKIND ulKind,
    /* [in] */ FIELDMODIFIERS ulModifiers,
    /* [unique][in] */ LPCOLESTR lpcszName);


void __RPC_STUB IRemoteContainerField_GetFields_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IRemoteContainerField_INTERFACE_DEFINED__ */


#ifndef __IRemoteMethodField_INTERFACE_DEFINED__
#define __IRemoteMethodField_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IRemoteMethodField
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IRemoteMethodField __RPC_FAR *LPREMOTEMETHODFIELD;


EXTERN_C const IID IID_IRemoteMethodField;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IRemoteMethodField : public IRemoteContainerField
    {
    public:
        virtual HRESULT __stdcall SetBreakpoint(
            /* [in] */ ULONG offPC) = 0;

        virtual HRESULT __stdcall ClearBreakpoint(
            /* [in] */ ULONG offPC) = 0;

        virtual HRESULT __stdcall GetLineInfo(
            /* [out] */ IEnumLINEINFO __RPC_FAR *__RPC_FAR *ppEnum) = 0;

        virtual HRESULT __stdcall GetBytes(
            /* [out] */ ILockBytes __RPC_FAR *__RPC_FAR *ppLockBytes) = 0;

        virtual HRESULT __stdcall GetScope(
            /* [unique][in] */ IRemoteField __RPC_FAR *pField,
            /* [out] */ ULONG __RPC_FAR *poffStart,
            /* [out] */ ULONG __RPC_FAR *pcbScope) = 0;

        virtual HRESULT __stdcall GetIndexedField(
            /* [in] */ ULONG slot,
            /* [in] */ ULONG offPC,
            /* [out] */ IRemoteField __RPC_FAR *__RPC_FAR *ppField) = 0;

    };

#else   /* C style interface */

    typedef struct IRemoteMethodFieldVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IRemoteMethodField __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IRemoteMethodField __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IRemoteMethodField __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetName )(
            IRemoteMethodField __RPC_FAR * This,
            /* [out] */ LPOLESTR __RPC_FAR *ppszName);

        HRESULT ( __stdcall __RPC_FAR *GetKind )(
            IRemoteMethodField __RPC_FAR * This,
            /* [out] */ FIELDKIND __RPC_FAR *pfk);

        HRESULT ( __stdcall __RPC_FAR *GetType )(
            IRemoteMethodField __RPC_FAR * This,
            /* [out] */ IRemoteField __RPC_FAR *__RPC_FAR *ppType);

        HRESULT ( __stdcall __RPC_FAR *GetContainer )(
            IRemoteMethodField __RPC_FAR * This,
            /* [out] */ IRemoteContainerField __RPC_FAR *__RPC_FAR *ppContainer);

        HRESULT ( __stdcall __RPC_FAR *GetModifiers )(
            IRemoteMethodField __RPC_FAR * This,
            /* [out] */ FIELDMODIFIERS __RPC_FAR *pulModifiers);

        HRESULT ( __stdcall __RPC_FAR *GetFields )(
            IRemoteMethodField __RPC_FAR * This,
            /* [out] */ IEnumRemoteField __RPC_FAR *__RPC_FAR *ppEnum,
            /* [in] */ FIELDKIND ulKind,
            /* [in] */ FIELDMODIFIERS ulModifiers,
            /* [unique][in] */ LPCOLESTR lpcszName);

        HRESULT ( __stdcall __RPC_FAR *SetBreakpoint )(
            IRemoteMethodField __RPC_FAR * This,
            /* [in] */ ULONG offPC);

        HRESULT ( __stdcall __RPC_FAR *ClearBreakpoint )(
            IRemoteMethodField __RPC_FAR * This,
            /* [in] */ ULONG offPC);

        HRESULT ( __stdcall __RPC_FAR *GetLineInfo )(
            IRemoteMethodField __RPC_FAR * This,
            /* [out] */ IEnumLINEINFO __RPC_FAR *__RPC_FAR *ppEnum);

        HRESULT ( __stdcall __RPC_FAR *GetBytes )(
            IRemoteMethodField __RPC_FAR * This,
            /* [out] */ ILockBytes __RPC_FAR *__RPC_FAR *ppLockBytes);

        HRESULT ( __stdcall __RPC_FAR *GetScope )(
            IRemoteMethodField __RPC_FAR * This,
            /* [unique][in] */ IRemoteField __RPC_FAR *pField,
            /* [out] */ ULONG __RPC_FAR *poffStart,
            /* [out] */ ULONG __RPC_FAR *pcbScope);

        HRESULT ( __stdcall __RPC_FAR *GetIndexedField )(
            IRemoteMethodField __RPC_FAR * This,
            /* [in] */ ULONG slot,
            /* [in] */ ULONG offPC,
            /* [out] */ IRemoteField __RPC_FAR *__RPC_FAR *ppField);

    } IRemoteMethodFieldVtbl;

    interface IRemoteMethodField
    {
        CONST_VTBL struct IRemoteMethodFieldVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IRemoteMethodField_QueryInterface(This,riid,ppvObject)  \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRemoteMethodField_AddRef(This) \
    (This)->lpVtbl -> AddRef(This)

#define IRemoteMethodField_Release(This)        \
    (This)->lpVtbl -> Release(This)


#define IRemoteMethodField_GetName(This,ppszName)       \
    (This)->lpVtbl -> GetName(This,ppszName)

#define IRemoteMethodField_GetKind(This,pfk)    \
    (This)->lpVtbl -> GetKind(This,pfk)

#define IRemoteMethodField_GetType(This,ppType) \
    (This)->lpVtbl -> GetType(This,ppType)

#define IRemoteMethodField_GetContainer(This,ppContainer)       \
    (This)->lpVtbl -> GetContainer(This,ppContainer)

#define IRemoteMethodField_GetModifiers(This,pulModifiers)      \
    (This)->lpVtbl -> GetModifiers(This,pulModifiers)


#define IRemoteMethodField_GetFields(This,ppEnum,ulKind,ulModifiers,lpcszName)  \
    (This)->lpVtbl -> GetFields(This,ppEnum,ulKind,ulModifiers,lpcszName)


#define IRemoteMethodField_SetBreakpoint(This,offPC)    \
    (This)->lpVtbl -> SetBreakpoint(This,offPC)

#define IRemoteMethodField_ClearBreakpoint(This,offPC)  \
    (This)->lpVtbl -> ClearBreakpoint(This,offPC)

#define IRemoteMethodField_GetLineInfo(This,ppEnum)     \
    (This)->lpVtbl -> GetLineInfo(This,ppEnum)

#define IRemoteMethodField_GetBytes(This,ppLockBytes)   \
    (This)->lpVtbl -> GetBytes(This,ppLockBytes)

#define IRemoteMethodField_GetScope(This,pField,poffStart,pcbScope)     \
    (This)->lpVtbl -> GetScope(This,pField,poffStart,pcbScope)

#define IRemoteMethodField_GetIndexedField(This,slot,offPC,ppField)     \
    (This)->lpVtbl -> GetIndexedField(This,slot,offPC,ppField)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IRemoteMethodField_SetBreakpoint_Proxy(
    IRemoteMethodField __RPC_FAR * This,
    /* [in] */ ULONG offPC);


void __RPC_STUB IRemoteMethodField_SetBreakpoint_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteMethodField_ClearBreakpoint_Proxy(
    IRemoteMethodField __RPC_FAR * This,
    /* [in] */ ULONG offPC);


void __RPC_STUB IRemoteMethodField_ClearBreakpoint_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteMethodField_GetLineInfo_Proxy(
    IRemoteMethodField __RPC_FAR * This,
    /* [out] */ IEnumLINEINFO __RPC_FAR *__RPC_FAR *ppEnum);


void __RPC_STUB IRemoteMethodField_GetLineInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteMethodField_GetBytes_Proxy(
    IRemoteMethodField __RPC_FAR * This,
    /* [out] */ ILockBytes __RPC_FAR *__RPC_FAR *ppLockBytes);


void __RPC_STUB IRemoteMethodField_GetBytes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteMethodField_GetScope_Proxy(
    IRemoteMethodField __RPC_FAR * This,
    /* [unique][in] */ IRemoteField __RPC_FAR *pField,
    /* [out] */ ULONG __RPC_FAR *poffStart,
    /* [out] */ ULONG __RPC_FAR *pcbScope);


void __RPC_STUB IRemoteMethodField_GetScope_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteMethodField_GetIndexedField_Proxy(
    IRemoteMethodField __RPC_FAR * This,
    /* [in] */ ULONG slot,
    /* [in] */ ULONG offPC,
    /* [out] */ IRemoteField __RPC_FAR *__RPC_FAR *ppField);


void __RPC_STUB IRemoteMethodField_GetIndexedField_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IRemoteMethodField_INTERFACE_DEFINED__ */


#ifndef __IRemoteClassField_INTERFACE_DEFINED__
#define __IRemoteClassField_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IRemoteClassField
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IRemoteClassField __RPC_FAR *LPREMOTECLASSFIELD;

                        /* size is 2 */

enum __MIDL_IRemoteClassField_0001
    {   CP_CONSTANT_UTF8        = 1,
        CP_CONSTANT_UNICODE     = 2,
        CP_CONSTANT_INTEGER     = 3,
        CP_CONSTANT_FLOAT       = 4,
        CP_CONSTANT_LONG        = 5,
        CP_CONSTANT_DOUBLE      = 6,
        CP_CONSTANT_CLASS       = 7,
        CP_CONSTANT_STRING      = 8,
        CP_CONSTANT_FIELDREF    = 9,
        CP_CONSTANT_METHODREF   = 10,
        CP_CONSTANT_INTERFACEMETHODREF  = 11,
        CP_CONSTANT_NAMEANDTYPE = 12
    };

EXTERN_C const IID IID_IRemoteClassField;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IRemoteClassField : public IRemoteContainerField
    {
    public:
        virtual HRESULT __stdcall GetFileName(
            /* [out] */ LPOLESTR __RPC_FAR *ppszFileName) = 0;

        virtual HRESULT __stdcall GetSourceFileName(
            /* [out] */ LPOLESTR __RPC_FAR *ppszSourceFileName) = 0;

        virtual HRESULT __stdcall GetSuperclass(
            /* [out] */ IRemoteClassField __RPC_FAR *__RPC_FAR *ppSuperclass) = 0;

        virtual HRESULT __stdcall GetInterfaces(
            /* [out] */ IEnumRemoteField __RPC_FAR *__RPC_FAR *ppEnum) = 0;

        virtual HRESULT __stdcall GetConstantPoolItem(
            /* [in] */ ULONG indexCP,
            /* [size_is][size_is][out] */ BYTE __RPC_FAR *__RPC_FAR *ppCPBytes,
            /* [out] */ ULONG __RPC_FAR *plength) = 0;

    };

#else   /* C style interface */

    typedef struct IRemoteClassFieldVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IRemoteClassField __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IRemoteClassField __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IRemoteClassField __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetName )(
            IRemoteClassField __RPC_FAR * This,
            /* [out] */ LPOLESTR __RPC_FAR *ppszName);

        HRESULT ( __stdcall __RPC_FAR *GetKind )(
            IRemoteClassField __RPC_FAR * This,
            /* [out] */ FIELDKIND __RPC_FAR *pfk);

        HRESULT ( __stdcall __RPC_FAR *GetType )(
            IRemoteClassField __RPC_FAR * This,
            /* [out] */ IRemoteField __RPC_FAR *__RPC_FAR *ppType);

        HRESULT ( __stdcall __RPC_FAR *GetContainer )(
            IRemoteClassField __RPC_FAR * This,
            /* [out] */ IRemoteContainerField __RPC_FAR *__RPC_FAR *ppContainer);

        HRESULT ( __stdcall __RPC_FAR *GetModifiers )(
            IRemoteClassField __RPC_FAR * This,
            /* [out] */ FIELDMODIFIERS __RPC_FAR *pulModifiers);

        HRESULT ( __stdcall __RPC_FAR *GetFields )(
            IRemoteClassField __RPC_FAR * This,
            /* [out] */ IEnumRemoteField __RPC_FAR *__RPC_FAR *ppEnum,
            /* [in] */ FIELDKIND ulKind,
            /* [in] */ FIELDMODIFIERS ulModifiers,
            /* [unique][in] */ LPCOLESTR lpcszName);

        HRESULT ( __stdcall __RPC_FAR *GetFileName )(
            IRemoteClassField __RPC_FAR * This,
            /* [out] */ LPOLESTR __RPC_FAR *ppszFileName);

        HRESULT ( __stdcall __RPC_FAR *GetSourceFileName )(
            IRemoteClassField __RPC_FAR * This,
            /* [out] */ LPOLESTR __RPC_FAR *ppszSourceFileName);

        HRESULT ( __stdcall __RPC_FAR *GetSuperclass )(
            IRemoteClassField __RPC_FAR * This,
            /* [out] */ IRemoteClassField __RPC_FAR *__RPC_FAR *ppSuperclass);

        HRESULT ( __stdcall __RPC_FAR *GetInterfaces )(
            IRemoteClassField __RPC_FAR * This,
            /* [out] */ IEnumRemoteField __RPC_FAR *__RPC_FAR *ppEnum);

        HRESULT ( __stdcall __RPC_FAR *GetConstantPoolItem )(
            IRemoteClassField __RPC_FAR * This,
            /* [in] */ ULONG indexCP,
            /* [size_is][size_is][out] */ BYTE __RPC_FAR *__RPC_FAR *ppCPBytes,
            /* [out] */ ULONG __RPC_FAR *plength);

    } IRemoteClassFieldVtbl;

    interface IRemoteClassField
    {
        CONST_VTBL struct IRemoteClassFieldVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IRemoteClassField_QueryInterface(This,riid,ppvObject)   \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRemoteClassField_AddRef(This)  \
    (This)->lpVtbl -> AddRef(This)

#define IRemoteClassField_Release(This) \
    (This)->lpVtbl -> Release(This)


#define IRemoteClassField_GetName(This,ppszName)        \
    (This)->lpVtbl -> GetName(This,ppszName)

#define IRemoteClassField_GetKind(This,pfk)     \
    (This)->lpVtbl -> GetKind(This,pfk)

#define IRemoteClassField_GetType(This,ppType)  \
    (This)->lpVtbl -> GetType(This,ppType)

#define IRemoteClassField_GetContainer(This,ppContainer)        \
    (This)->lpVtbl -> GetContainer(This,ppContainer)

#define IRemoteClassField_GetModifiers(This,pulModifiers)       \
    (This)->lpVtbl -> GetModifiers(This,pulModifiers)


#define IRemoteClassField_GetFields(This,ppEnum,ulKind,ulModifiers,lpcszName)   \
    (This)->lpVtbl -> GetFields(This,ppEnum,ulKind,ulModifiers,lpcszName)


#define IRemoteClassField_GetFileName(This,ppszFileName)        \
    (This)->lpVtbl -> GetFileName(This,ppszFileName)

#define IRemoteClassField_GetSourceFileName(This,ppszSourceFileName)    \
    (This)->lpVtbl -> GetSourceFileName(This,ppszSourceFileName)

#define IRemoteClassField_GetSuperclass(This,ppSuperclass)      \
    (This)->lpVtbl -> GetSuperclass(This,ppSuperclass)

#define IRemoteClassField_GetInterfaces(This,ppEnum)    \
    (This)->lpVtbl -> GetInterfaces(This,ppEnum)

#define IRemoteClassField_GetConstantPoolItem(This,indexCP,ppCPBytes,plength)   \
    (This)->lpVtbl -> GetConstantPoolItem(This,indexCP,ppCPBytes,plength)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IRemoteClassField_GetFileName_Proxy(
    IRemoteClassField __RPC_FAR * This,
    /* [out] */ LPOLESTR __RPC_FAR *ppszFileName);


void __RPC_STUB IRemoteClassField_GetFileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteClassField_GetSourceFileName_Proxy(
    IRemoteClassField __RPC_FAR * This,
    /* [out] */ LPOLESTR __RPC_FAR *ppszSourceFileName);


void __RPC_STUB IRemoteClassField_GetSourceFileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteClassField_GetSuperclass_Proxy(
    IRemoteClassField __RPC_FAR * This,
    /* [out] */ IRemoteClassField __RPC_FAR *__RPC_FAR *ppSuperclass);


void __RPC_STUB IRemoteClassField_GetSuperclass_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteClassField_GetInterfaces_Proxy(
    IRemoteClassField __RPC_FAR * This,
    /* [out] */ IEnumRemoteField __RPC_FAR *__RPC_FAR *ppEnum);


void __RPC_STUB IRemoteClassField_GetInterfaces_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteClassField_GetConstantPoolItem_Proxy(
    IRemoteClassField __RPC_FAR * This,
    /* [in] */ ULONG indexCP,
    /* [size_is][size_is][out] */ BYTE __RPC_FAR *__RPC_FAR *ppCPBytes,
    /* [out] */ ULONG __RPC_FAR *plength);


void __RPC_STUB IRemoteClassField_GetConstantPoolItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IRemoteClassField_INTERFACE_DEFINED__ */


#ifndef __IRemoteObject_INTERFACE_DEFINED__
#define __IRemoteObject_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IRemoteObject
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IRemoteObject __RPC_FAR *LPREMOTEOBJECT;

                        /* size is 1 */
typedef BYTE JAVA_BOOLEAN;

                        /* size is 1 */
typedef signed char JAVA_BYTE;

                        /* size is 2 */
typedef USHORT JAVA_CHAR;

                        /* size is 8 */
typedef double JAVA_DOUBLE;

                        /* size is 4 */
typedef float JAVA_FLOAT;

                        /* size is 4 */
typedef LONG JAVA_INT;

                        /* size is 8 */
typedef LONGLONG JAVA_LONG;

                        /* size is 2 */
typedef SHORT JAVA_SHORT;

                        /* size is 4 */
typedef LPOLESTR JAVA_STRING;


EXTERN_C const IID IID_IRemoteObject;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IRemoteObject : public IUnknown
    {
    public:
        virtual HRESULT __stdcall GetType(
            /* [out] */ IRemoteField __RPC_FAR *__RPC_FAR *ppType) = 0;

        virtual HRESULT __stdcall SetBreakpoint( void) = 0;

        virtual HRESULT __stdcall ClearBreakpoint( void) = 0;

    };

#else   /* C style interface */

    typedef struct IRemoteObjectVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IRemoteObject __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IRemoteObject __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IRemoteObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetType )(
            IRemoteObject __RPC_FAR * This,
            /* [out] */ IRemoteField __RPC_FAR *__RPC_FAR *ppType);

        HRESULT ( __stdcall __RPC_FAR *SetBreakpoint )(
            IRemoteObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *ClearBreakpoint )(
            IRemoteObject __RPC_FAR * This);

    } IRemoteObjectVtbl;

    interface IRemoteObject
    {
        CONST_VTBL struct IRemoteObjectVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IRemoteObject_QueryInterface(This,riid,ppvObject)       \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRemoteObject_AddRef(This)      \
    (This)->lpVtbl -> AddRef(This)

#define IRemoteObject_Release(This)     \
    (This)->lpVtbl -> Release(This)


#define IRemoteObject_GetType(This,ppType)      \
    (This)->lpVtbl -> GetType(This,ppType)

#define IRemoteObject_SetBreakpoint(This)       \
    (This)->lpVtbl -> SetBreakpoint(This)

#define IRemoteObject_ClearBreakpoint(This)     \
    (This)->lpVtbl -> ClearBreakpoint(This)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IRemoteObject_GetType_Proxy(
    IRemoteObject __RPC_FAR * This,
    /* [out] */ IRemoteField __RPC_FAR *__RPC_FAR *ppType);


void __RPC_STUB IRemoteObject_GetType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteObject_SetBreakpoint_Proxy(
    IRemoteObject __RPC_FAR * This);


void __RPC_STUB IRemoteObject_SetBreakpoint_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteObject_ClearBreakpoint_Proxy(
    IRemoteObject __RPC_FAR * This);


void __RPC_STUB IRemoteObject_ClearBreakpoint_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IRemoteObject_INTERFACE_DEFINED__ */


#ifndef __IEnumRemoteObject_INTERFACE_DEFINED__
#define __IEnumRemoteObject_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IEnumRemoteObject
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IEnumRemoteObject __RPC_FAR *LPENUMREMOTEOBJECT;


EXTERN_C const IID IID_IEnumRemoteObject;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IEnumRemoteObject : public IUnknown
    {
    public:
        virtual HRESULT __stdcall Next(
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IRemoteObject __RPC_FAR *__RPC_FAR rgelt[  ],
            /* [out] */ ULONG __RPC_FAR *pceltFetched) = 0;

        virtual HRESULT __stdcall Skip(
            /* [in] */ ULONG celt) = 0;

        virtual HRESULT __stdcall Reset( void) = 0;

        virtual HRESULT __stdcall Clone(
            /* [out] */ IEnumRemoteObject __RPC_FAR *__RPC_FAR *ppEnum) = 0;

        virtual HRESULT __stdcall GetCount(
            /* [out] */ ULONG __RPC_FAR *pcelt) = 0;

    };

#else   /* C style interface */

    typedef struct IEnumRemoteObjectVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IEnumRemoteObject __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IEnumRemoteObject __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IEnumRemoteObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Next )(
            IEnumRemoteObject __RPC_FAR * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IRemoteObject __RPC_FAR *__RPC_FAR rgelt[  ],
            /* [out] */ ULONG __RPC_FAR *pceltFetched);

        HRESULT ( __stdcall __RPC_FAR *Skip )(
            IEnumRemoteObject __RPC_FAR * This,
            /* [in] */ ULONG celt);

        HRESULT ( __stdcall __RPC_FAR *Reset )(
            IEnumRemoteObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Clone )(
            IEnumRemoteObject __RPC_FAR * This,
            /* [out] */ IEnumRemoteObject __RPC_FAR *__RPC_FAR *ppEnum);

        HRESULT ( __stdcall __RPC_FAR *GetCount )(
            IEnumRemoteObject __RPC_FAR * This,
            /* [out] */ ULONG __RPC_FAR *pcelt);

    } IEnumRemoteObjectVtbl;

    interface IEnumRemoteObject
    {
        CONST_VTBL struct IEnumRemoteObjectVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IEnumRemoteObject_QueryInterface(This,riid,ppvObject)   \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumRemoteObject_AddRef(This)  \
    (This)->lpVtbl -> AddRef(This)

#define IEnumRemoteObject_Release(This) \
    (This)->lpVtbl -> Release(This)


#define IEnumRemoteObject_Next(This,celt,rgelt,pceltFetched)    \
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumRemoteObject_Skip(This,celt)       \
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumRemoteObject_Reset(This)   \
    (This)->lpVtbl -> Reset(This)

#define IEnumRemoteObject_Clone(This,ppEnum)    \
    (This)->lpVtbl -> Clone(This,ppEnum)

#define IEnumRemoteObject_GetCount(This,pcelt)  \
    (This)->lpVtbl -> GetCount(This,pcelt)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IEnumRemoteObject_Next_Proxy(
    IEnumRemoteObject __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ IRemoteObject __RPC_FAR *__RPC_FAR rgelt[  ],
    /* [out] */ ULONG __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumRemoteObject_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumRemoteObject_Skip_Proxy(
    IEnumRemoteObject __RPC_FAR * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumRemoteObject_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumRemoteObject_Reset_Proxy(
    IEnumRemoteObject __RPC_FAR * This);


void __RPC_STUB IEnumRemoteObject_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumRemoteObject_Clone_Proxy(
    IEnumRemoteObject __RPC_FAR * This,
    /* [out] */ IEnumRemoteObject __RPC_FAR *__RPC_FAR *ppEnum);


void __RPC_STUB IEnumRemoteObject_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumRemoteObject_GetCount_Proxy(
    IEnumRemoteObject __RPC_FAR * This,
    /* [out] */ ULONG __RPC_FAR *pcelt);


void __RPC_STUB IEnumRemoteObject_GetCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IEnumRemoteObject_INTERFACE_DEFINED__ */


#ifndef __IEnumRemoteValue_INTERFACE_DEFINED__
#define __IEnumRemoteValue_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IEnumRemoteValue
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IEnumRemoteValue __RPC_FAR *LPENUMREMOTEVALUE;


EXTERN_C const IID IID_IEnumRemoteValue;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IEnumRemoteValue : public IUnknown
    {
    public:
        virtual HRESULT __stdcall Skip(
            /* [in] */ ULONG celt) = 0;

        virtual HRESULT __stdcall Reset( void) = 0;

        virtual HRESULT __stdcall Clone(
            /* [out] */ IEnumRemoteValue __RPC_FAR *__RPC_FAR *ppEnum) = 0;

        virtual HRESULT __stdcall GetCount(
            /* [out] */ ULONG __RPC_FAR *pcelt) = 0;

    };

#else   /* C style interface */

    typedef struct IEnumRemoteValueVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IEnumRemoteValue __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IEnumRemoteValue __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IEnumRemoteValue __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Skip )(
            IEnumRemoteValue __RPC_FAR * This,
            /* [in] */ ULONG celt);

        HRESULT ( __stdcall __RPC_FAR *Reset )(
            IEnumRemoteValue __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Clone )(
            IEnumRemoteValue __RPC_FAR * This,
            /* [out] */ IEnumRemoteValue __RPC_FAR *__RPC_FAR *ppEnum);

        HRESULT ( __stdcall __RPC_FAR *GetCount )(
            IEnumRemoteValue __RPC_FAR * This,
            /* [out] */ ULONG __RPC_FAR *pcelt);

    } IEnumRemoteValueVtbl;

    interface IEnumRemoteValue
    {
        CONST_VTBL struct IEnumRemoteValueVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IEnumRemoteValue_QueryInterface(This,riid,ppvObject)    \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumRemoteValue_AddRef(This)   \
    (This)->lpVtbl -> AddRef(This)

#define IEnumRemoteValue_Release(This)  \
    (This)->lpVtbl -> Release(This)


#define IEnumRemoteValue_Skip(This,celt)        \
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumRemoteValue_Reset(This)    \
    (This)->lpVtbl -> Reset(This)

#define IEnumRemoteValue_Clone(This,ppEnum)     \
    (This)->lpVtbl -> Clone(This,ppEnum)

#define IEnumRemoteValue_GetCount(This,pcelt)   \
    (This)->lpVtbl -> GetCount(This,pcelt)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IEnumRemoteValue_Skip_Proxy(
    IEnumRemoteValue __RPC_FAR * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumRemoteValue_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumRemoteValue_Reset_Proxy(
    IEnumRemoteValue __RPC_FAR * This);


void __RPC_STUB IEnumRemoteValue_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumRemoteValue_Clone_Proxy(
    IEnumRemoteValue __RPC_FAR * This,
    /* [out] */ IEnumRemoteValue __RPC_FAR *__RPC_FAR *ppEnum);


void __RPC_STUB IEnumRemoteValue_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumRemoteValue_GetCount_Proxy(
    IEnumRemoteValue __RPC_FAR * This,
    /* [out] */ ULONG __RPC_FAR *pcelt);


void __RPC_STUB IEnumRemoteValue_GetCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IEnumRemoteValue_INTERFACE_DEFINED__ */


#ifndef __IEnumRemoteBooleanValue_INTERFACE_DEFINED__
#define __IEnumRemoteBooleanValue_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IEnumRemoteBooleanValue
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IEnumRemoteBooleanValue __RPC_FAR *LPENUMREMOTEBOOLEANVALUE;


EXTERN_C const IID IID_IEnumRemoteBooleanValue;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IEnumRemoteBooleanValue : public IEnumRemoteValue
    {
    public:
        virtual HRESULT __stdcall Next(
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ JAVA_BOOLEAN __RPC_FAR rgelt[  ],
            /* [out] */ ULONG __RPC_FAR *pceltFetched) = 0;

    };

#else   /* C style interface */

    typedef struct IEnumRemoteBooleanValueVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IEnumRemoteBooleanValue __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IEnumRemoteBooleanValue __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IEnumRemoteBooleanValue __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Skip )(
            IEnumRemoteBooleanValue __RPC_FAR * This,
            /* [in] */ ULONG celt);

        HRESULT ( __stdcall __RPC_FAR *Reset )(
            IEnumRemoteBooleanValue __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Clone )(
            IEnumRemoteBooleanValue __RPC_FAR * This,
            /* [out] */ IEnumRemoteValue __RPC_FAR *__RPC_FAR *ppEnum);

        HRESULT ( __stdcall __RPC_FAR *GetCount )(
            IEnumRemoteBooleanValue __RPC_FAR * This,
            /* [out] */ ULONG __RPC_FAR *pcelt);

        HRESULT ( __stdcall __RPC_FAR *Next )(
            IEnumRemoteBooleanValue __RPC_FAR * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ JAVA_BOOLEAN __RPC_FAR rgelt[  ],
            /* [out] */ ULONG __RPC_FAR *pceltFetched);

    } IEnumRemoteBooleanValueVtbl;

    interface IEnumRemoteBooleanValue
    {
        CONST_VTBL struct IEnumRemoteBooleanValueVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IEnumRemoteBooleanValue_QueryInterface(This,riid,ppvObject)     \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumRemoteBooleanValue_AddRef(This)    \
    (This)->lpVtbl -> AddRef(This)

#define IEnumRemoteBooleanValue_Release(This)   \
    (This)->lpVtbl -> Release(This)


#define IEnumRemoteBooleanValue_Skip(This,celt) \
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumRemoteBooleanValue_Reset(This)     \
    (This)->lpVtbl -> Reset(This)

#define IEnumRemoteBooleanValue_Clone(This,ppEnum)      \
    (This)->lpVtbl -> Clone(This,ppEnum)

#define IEnumRemoteBooleanValue_GetCount(This,pcelt)    \
    (This)->lpVtbl -> GetCount(This,pcelt)


#define IEnumRemoteBooleanValue_Next(This,celt,rgelt,pceltFetched)      \
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IEnumRemoteBooleanValue_Next_Proxy(
    IEnumRemoteBooleanValue __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ JAVA_BOOLEAN __RPC_FAR rgelt[  ],
    /* [out] */ ULONG __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumRemoteBooleanValue_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IEnumRemoteBooleanValue_INTERFACE_DEFINED__ */


#ifndef __IEnumRemoteByteValue_INTERFACE_DEFINED__
#define __IEnumRemoteByteValue_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IEnumRemoteByteValue
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IEnumRemoteByteValue __RPC_FAR *LPENUMREMOTEBYTEVALUE;


EXTERN_C const IID IID_IEnumRemoteByteValue;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IEnumRemoteByteValue : public IEnumRemoteValue
    {
    public:
        virtual HRESULT __stdcall Next(
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ JAVA_BYTE __RPC_FAR rgelt[  ],
            /* [out] */ ULONG __RPC_FAR *pceltFetched) = 0;

    };

#else   /* C style interface */

    typedef struct IEnumRemoteByteValueVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IEnumRemoteByteValue __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IEnumRemoteByteValue __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IEnumRemoteByteValue __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Skip )(
            IEnumRemoteByteValue __RPC_FAR * This,
            /* [in] */ ULONG celt);

        HRESULT ( __stdcall __RPC_FAR *Reset )(
            IEnumRemoteByteValue __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Clone )(
            IEnumRemoteByteValue __RPC_FAR * This,
            /* [out] */ IEnumRemoteValue __RPC_FAR *__RPC_FAR *ppEnum);

        HRESULT ( __stdcall __RPC_FAR *GetCount )(
            IEnumRemoteByteValue __RPC_FAR * This,
            /* [out] */ ULONG __RPC_FAR *pcelt);

        HRESULT ( __stdcall __RPC_FAR *Next )(
            IEnumRemoteByteValue __RPC_FAR * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ JAVA_BYTE __RPC_FAR rgelt[  ],
            /* [out] */ ULONG __RPC_FAR *pceltFetched);

    } IEnumRemoteByteValueVtbl;

    interface IEnumRemoteByteValue
    {
        CONST_VTBL struct IEnumRemoteByteValueVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IEnumRemoteByteValue_QueryInterface(This,riid,ppvObject)        \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumRemoteByteValue_AddRef(This)       \
    (This)->lpVtbl -> AddRef(This)

#define IEnumRemoteByteValue_Release(This)      \
    (This)->lpVtbl -> Release(This)


#define IEnumRemoteByteValue_Skip(This,celt)    \
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumRemoteByteValue_Reset(This)        \
    (This)->lpVtbl -> Reset(This)

#define IEnumRemoteByteValue_Clone(This,ppEnum) \
    (This)->lpVtbl -> Clone(This,ppEnum)

#define IEnumRemoteByteValue_GetCount(This,pcelt)       \
    (This)->lpVtbl -> GetCount(This,pcelt)


#define IEnumRemoteByteValue_Next(This,celt,rgelt,pceltFetched) \
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IEnumRemoteByteValue_Next_Proxy(
    IEnumRemoteByteValue __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ JAVA_BYTE __RPC_FAR rgelt[  ],
    /* [out] */ ULONG __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumRemoteByteValue_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IEnumRemoteByteValue_INTERFACE_DEFINED__ */


#ifndef __IEnumRemoteCharValue_INTERFACE_DEFINED__
#define __IEnumRemoteCharValue_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IEnumRemoteCharValue
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IEnumRemoteCharValue __RPC_FAR *LPENUMREMOTECHARVALUE;


EXTERN_C const IID IID_IEnumRemoteCharValue;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IEnumRemoteCharValue : public IEnumRemoteValue
    {
    public:
        virtual HRESULT __stdcall Next(
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ JAVA_CHAR __RPC_FAR rgelt[  ],
            /* [out] */ ULONG __RPC_FAR *pceltFetched) = 0;

    };

#else   /* C style interface */

    typedef struct IEnumRemoteCharValueVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IEnumRemoteCharValue __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IEnumRemoteCharValue __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IEnumRemoteCharValue __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Skip )(
            IEnumRemoteCharValue __RPC_FAR * This,
            /* [in] */ ULONG celt);

        HRESULT ( __stdcall __RPC_FAR *Reset )(
            IEnumRemoteCharValue __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Clone )(
            IEnumRemoteCharValue __RPC_FAR * This,
            /* [out] */ IEnumRemoteValue __RPC_FAR *__RPC_FAR *ppEnum);

        HRESULT ( __stdcall __RPC_FAR *GetCount )(
            IEnumRemoteCharValue __RPC_FAR * This,
            /* [out] */ ULONG __RPC_FAR *pcelt);

        HRESULT ( __stdcall __RPC_FAR *Next )(
            IEnumRemoteCharValue __RPC_FAR * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ JAVA_CHAR __RPC_FAR rgelt[  ],
            /* [out] */ ULONG __RPC_FAR *pceltFetched);

    } IEnumRemoteCharValueVtbl;

    interface IEnumRemoteCharValue
    {
        CONST_VTBL struct IEnumRemoteCharValueVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IEnumRemoteCharValue_QueryInterface(This,riid,ppvObject)        \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumRemoteCharValue_AddRef(This)       \
    (This)->lpVtbl -> AddRef(This)

#define IEnumRemoteCharValue_Release(This)      \
    (This)->lpVtbl -> Release(This)


#define IEnumRemoteCharValue_Skip(This,celt)    \
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumRemoteCharValue_Reset(This)        \
    (This)->lpVtbl -> Reset(This)

#define IEnumRemoteCharValue_Clone(This,ppEnum) \
    (This)->lpVtbl -> Clone(This,ppEnum)

#define IEnumRemoteCharValue_GetCount(This,pcelt)       \
    (This)->lpVtbl -> GetCount(This,pcelt)


#define IEnumRemoteCharValue_Next(This,celt,rgelt,pceltFetched) \
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IEnumRemoteCharValue_Next_Proxy(
    IEnumRemoteCharValue __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ JAVA_CHAR __RPC_FAR rgelt[  ],
    /* [out] */ ULONG __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumRemoteCharValue_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IEnumRemoteCharValue_INTERFACE_DEFINED__ */


#ifndef __IEnumRemoteDoubleValue_INTERFACE_DEFINED__
#define __IEnumRemoteDoubleValue_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IEnumRemoteDoubleValue
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IEnumRemoteDoubleValue __RPC_FAR *LPENUMREMOTEDOUBLEVALUE;


EXTERN_C const IID IID_IEnumRemoteDoubleValue;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IEnumRemoteDoubleValue : public IEnumRemoteValue
    {
    public:
        virtual HRESULT __stdcall Next(
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ JAVA_DOUBLE __RPC_FAR rgelt[  ],
            /* [out] */ ULONG __RPC_FAR *pceltFetched) = 0;

    };

#else   /* C style interface */

    typedef struct IEnumRemoteDoubleValueVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IEnumRemoteDoubleValue __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IEnumRemoteDoubleValue __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IEnumRemoteDoubleValue __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Skip )(
            IEnumRemoteDoubleValue __RPC_FAR * This,
            /* [in] */ ULONG celt);

        HRESULT ( __stdcall __RPC_FAR *Reset )(
            IEnumRemoteDoubleValue __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Clone )(
            IEnumRemoteDoubleValue __RPC_FAR * This,
            /* [out] */ IEnumRemoteValue __RPC_FAR *__RPC_FAR *ppEnum);

        HRESULT ( __stdcall __RPC_FAR *GetCount )(
            IEnumRemoteDoubleValue __RPC_FAR * This,
            /* [out] */ ULONG __RPC_FAR *pcelt);

        HRESULT ( __stdcall __RPC_FAR *Next )(
            IEnumRemoteDoubleValue __RPC_FAR * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ JAVA_DOUBLE __RPC_FAR rgelt[  ],
            /* [out] */ ULONG __RPC_FAR *pceltFetched);

    } IEnumRemoteDoubleValueVtbl;

    interface IEnumRemoteDoubleValue
    {
        CONST_VTBL struct IEnumRemoteDoubleValueVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IEnumRemoteDoubleValue_QueryInterface(This,riid,ppvObject)      \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumRemoteDoubleValue_AddRef(This)     \
    (This)->lpVtbl -> AddRef(This)

#define IEnumRemoteDoubleValue_Release(This)    \
    (This)->lpVtbl -> Release(This)


#define IEnumRemoteDoubleValue_Skip(This,celt)  \
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumRemoteDoubleValue_Reset(This)      \
    (This)->lpVtbl -> Reset(This)

#define IEnumRemoteDoubleValue_Clone(This,ppEnum)       \
    (This)->lpVtbl -> Clone(This,ppEnum)

#define IEnumRemoteDoubleValue_GetCount(This,pcelt)     \
    (This)->lpVtbl -> GetCount(This,pcelt)


#define IEnumRemoteDoubleValue_Next(This,celt,rgelt,pceltFetched)       \
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IEnumRemoteDoubleValue_Next_Proxy(
    IEnumRemoteDoubleValue __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ JAVA_DOUBLE __RPC_FAR rgelt[  ],
    /* [out] */ ULONG __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumRemoteDoubleValue_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IEnumRemoteDoubleValue_INTERFACE_DEFINED__ */


#ifndef __IEnumRemoteFloatValue_INTERFACE_DEFINED__
#define __IEnumRemoteFloatValue_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IEnumRemoteFloatValue
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IEnumRemoteFloatValue __RPC_FAR *LPENUMREMOTEFLOATVALUE;


EXTERN_C const IID IID_IEnumRemoteFloatValue;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IEnumRemoteFloatValue : public IEnumRemoteValue
    {
    public:
        virtual HRESULT __stdcall Next(
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ JAVA_FLOAT __RPC_FAR rgelt[  ],
            /* [out] */ ULONG __RPC_FAR *pceltFetched) = 0;

    };

#else   /* C style interface */

    typedef struct IEnumRemoteFloatValueVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IEnumRemoteFloatValue __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IEnumRemoteFloatValue __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IEnumRemoteFloatValue __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Skip )(
            IEnumRemoteFloatValue __RPC_FAR * This,
            /* [in] */ ULONG celt);

        HRESULT ( __stdcall __RPC_FAR *Reset )(
            IEnumRemoteFloatValue __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Clone )(
            IEnumRemoteFloatValue __RPC_FAR * This,
            /* [out] */ IEnumRemoteValue __RPC_FAR *__RPC_FAR *ppEnum);

        HRESULT ( __stdcall __RPC_FAR *GetCount )(
            IEnumRemoteFloatValue __RPC_FAR * This,
            /* [out] */ ULONG __RPC_FAR *pcelt);

        HRESULT ( __stdcall __RPC_FAR *Next )(
            IEnumRemoteFloatValue __RPC_FAR * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ JAVA_FLOAT __RPC_FAR rgelt[  ],
            /* [out] */ ULONG __RPC_FAR *pceltFetched);

    } IEnumRemoteFloatValueVtbl;

    interface IEnumRemoteFloatValue
    {
        CONST_VTBL struct IEnumRemoteFloatValueVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IEnumRemoteFloatValue_QueryInterface(This,riid,ppvObject)       \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumRemoteFloatValue_AddRef(This)      \
    (This)->lpVtbl -> AddRef(This)

#define IEnumRemoteFloatValue_Release(This)     \
    (This)->lpVtbl -> Release(This)


#define IEnumRemoteFloatValue_Skip(This,celt)   \
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumRemoteFloatValue_Reset(This)       \
    (This)->lpVtbl -> Reset(This)

#define IEnumRemoteFloatValue_Clone(This,ppEnum)        \
    (This)->lpVtbl -> Clone(This,ppEnum)

#define IEnumRemoteFloatValue_GetCount(This,pcelt)      \
    (This)->lpVtbl -> GetCount(This,pcelt)


#define IEnumRemoteFloatValue_Next(This,celt,rgelt,pceltFetched)        \
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IEnumRemoteFloatValue_Next_Proxy(
    IEnumRemoteFloatValue __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ JAVA_FLOAT __RPC_FAR rgelt[  ],
    /* [out] */ ULONG __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumRemoteFloatValue_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IEnumRemoteFloatValue_INTERFACE_DEFINED__ */


#ifndef __IEnumRemoteIntValue_INTERFACE_DEFINED__
#define __IEnumRemoteIntValue_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IEnumRemoteIntValue
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IEnumRemoteIntValue __RPC_FAR *LPENUMREMOTEINTVALUE;


EXTERN_C const IID IID_IEnumRemoteIntValue;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IEnumRemoteIntValue : public IEnumRemoteValue
    {
    public:
        virtual HRESULT __stdcall Next(
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ JAVA_INT __RPC_FAR rgelt[  ],
            /* [out] */ ULONG __RPC_FAR *pceltFetched) = 0;

    };

#else   /* C style interface */

    typedef struct IEnumRemoteIntValueVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IEnumRemoteIntValue __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IEnumRemoteIntValue __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IEnumRemoteIntValue __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Skip )(
            IEnumRemoteIntValue __RPC_FAR * This,
            /* [in] */ ULONG celt);

        HRESULT ( __stdcall __RPC_FAR *Reset )(
            IEnumRemoteIntValue __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Clone )(
            IEnumRemoteIntValue __RPC_FAR * This,
            /* [out] */ IEnumRemoteValue __RPC_FAR *__RPC_FAR *ppEnum);

        HRESULT ( __stdcall __RPC_FAR *GetCount )(
            IEnumRemoteIntValue __RPC_FAR * This,
            /* [out] */ ULONG __RPC_FAR *pcelt);

        HRESULT ( __stdcall __RPC_FAR *Next )(
            IEnumRemoteIntValue __RPC_FAR * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ JAVA_INT __RPC_FAR rgelt[  ],
            /* [out] */ ULONG __RPC_FAR *pceltFetched);

    } IEnumRemoteIntValueVtbl;

    interface IEnumRemoteIntValue
    {
        CONST_VTBL struct IEnumRemoteIntValueVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IEnumRemoteIntValue_QueryInterface(This,riid,ppvObject) \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumRemoteIntValue_AddRef(This)        \
    (This)->lpVtbl -> AddRef(This)

#define IEnumRemoteIntValue_Release(This)       \
    (This)->lpVtbl -> Release(This)


#define IEnumRemoteIntValue_Skip(This,celt)     \
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumRemoteIntValue_Reset(This) \
    (This)->lpVtbl -> Reset(This)

#define IEnumRemoteIntValue_Clone(This,ppEnum)  \
    (This)->lpVtbl -> Clone(This,ppEnum)

#define IEnumRemoteIntValue_GetCount(This,pcelt)        \
    (This)->lpVtbl -> GetCount(This,pcelt)


#define IEnumRemoteIntValue_Next(This,celt,rgelt,pceltFetched)  \
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IEnumRemoteIntValue_Next_Proxy(
    IEnumRemoteIntValue __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ JAVA_INT __RPC_FAR rgelt[  ],
    /* [out] */ ULONG __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumRemoteIntValue_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IEnumRemoteIntValue_INTERFACE_DEFINED__ */


#ifndef __IEnumRemoteLongValue_INTERFACE_DEFINED__
#define __IEnumRemoteLongValue_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IEnumRemoteLongValue
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IEnumRemoteLongValue __RPC_FAR *LPENUMREMOTELONGVALUE;


EXTERN_C const IID IID_IEnumRemoteLongValue;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IEnumRemoteLongValue : public IEnumRemoteValue
    {
    public:
        virtual HRESULT __stdcall Next(
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ JAVA_LONG __RPC_FAR rgelt[  ],
            /* [out] */ ULONG __RPC_FAR *pceltFetched) = 0;

    };

#else   /* C style interface */

    typedef struct IEnumRemoteLongValueVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IEnumRemoteLongValue __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IEnumRemoteLongValue __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IEnumRemoteLongValue __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Skip )(
            IEnumRemoteLongValue __RPC_FAR * This,
            /* [in] */ ULONG celt);

        HRESULT ( __stdcall __RPC_FAR *Reset )(
            IEnumRemoteLongValue __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Clone )(
            IEnumRemoteLongValue __RPC_FAR * This,
            /* [out] */ IEnumRemoteValue __RPC_FAR *__RPC_FAR *ppEnum);

        HRESULT ( __stdcall __RPC_FAR *GetCount )(
            IEnumRemoteLongValue __RPC_FAR * This,
            /* [out] */ ULONG __RPC_FAR *pcelt);

        HRESULT ( __stdcall __RPC_FAR *Next )(
            IEnumRemoteLongValue __RPC_FAR * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ JAVA_LONG __RPC_FAR rgelt[  ],
            /* [out] */ ULONG __RPC_FAR *pceltFetched);

    } IEnumRemoteLongValueVtbl;

    interface IEnumRemoteLongValue
    {
        CONST_VTBL struct IEnumRemoteLongValueVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IEnumRemoteLongValue_QueryInterface(This,riid,ppvObject)        \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumRemoteLongValue_AddRef(This)       \
    (This)->lpVtbl -> AddRef(This)

#define IEnumRemoteLongValue_Release(This)      \
    (This)->lpVtbl -> Release(This)


#define IEnumRemoteLongValue_Skip(This,celt)    \
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumRemoteLongValue_Reset(This)        \
    (This)->lpVtbl -> Reset(This)

#define IEnumRemoteLongValue_Clone(This,ppEnum) \
    (This)->lpVtbl -> Clone(This,ppEnum)

#define IEnumRemoteLongValue_GetCount(This,pcelt)       \
    (This)->lpVtbl -> GetCount(This,pcelt)


#define IEnumRemoteLongValue_Next(This,celt,rgelt,pceltFetched) \
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IEnumRemoteLongValue_Next_Proxy(
    IEnumRemoteLongValue __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ JAVA_LONG __RPC_FAR rgelt[  ],
    /* [out] */ ULONG __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumRemoteLongValue_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IEnumRemoteLongValue_INTERFACE_DEFINED__ */


#ifndef __IEnumRemoteShortValue_INTERFACE_DEFINED__
#define __IEnumRemoteShortValue_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IEnumRemoteShortValue
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IEnumRemoteShortValue __RPC_FAR *LPENUMREMOTESHORTVALUE;


EXTERN_C const IID IID_IEnumRemoteShortValue;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IEnumRemoteShortValue : public IEnumRemoteValue
    {
    public:
        virtual HRESULT __stdcall Next(
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ JAVA_SHORT __RPC_FAR rgelt[  ],
            /* [out] */ ULONG __RPC_FAR *pceltFetched) = 0;

    };

#else   /* C style interface */

    typedef struct IEnumRemoteShortValueVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IEnumRemoteShortValue __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IEnumRemoteShortValue __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IEnumRemoteShortValue __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Skip )(
            IEnumRemoteShortValue __RPC_FAR * This,
            /* [in] */ ULONG celt);

        HRESULT ( __stdcall __RPC_FAR *Reset )(
            IEnumRemoteShortValue __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Clone )(
            IEnumRemoteShortValue __RPC_FAR * This,
            /* [out] */ IEnumRemoteValue __RPC_FAR *__RPC_FAR *ppEnum);

        HRESULT ( __stdcall __RPC_FAR *GetCount )(
            IEnumRemoteShortValue __RPC_FAR * This,
            /* [out] */ ULONG __RPC_FAR *pcelt);

        HRESULT ( __stdcall __RPC_FAR *Next )(
            IEnumRemoteShortValue __RPC_FAR * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ JAVA_SHORT __RPC_FAR rgelt[  ],
            /* [out] */ ULONG __RPC_FAR *pceltFetched);

    } IEnumRemoteShortValueVtbl;

    interface IEnumRemoteShortValue
    {
        CONST_VTBL struct IEnumRemoteShortValueVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IEnumRemoteShortValue_QueryInterface(This,riid,ppvObject)       \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumRemoteShortValue_AddRef(This)      \
    (This)->lpVtbl -> AddRef(This)

#define IEnumRemoteShortValue_Release(This)     \
    (This)->lpVtbl -> Release(This)


#define IEnumRemoteShortValue_Skip(This,celt)   \
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumRemoteShortValue_Reset(This)       \
    (This)->lpVtbl -> Reset(This)

#define IEnumRemoteShortValue_Clone(This,ppEnum)        \
    (This)->lpVtbl -> Clone(This,ppEnum)

#define IEnumRemoteShortValue_GetCount(This,pcelt)      \
    (This)->lpVtbl -> GetCount(This,pcelt)


#define IEnumRemoteShortValue_Next(This,celt,rgelt,pceltFetched)        \
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IEnumRemoteShortValue_Next_Proxy(
    IEnumRemoteShortValue __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ JAVA_SHORT __RPC_FAR rgelt[  ],
    /* [out] */ ULONG __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumRemoteShortValue_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IEnumRemoteShortValue_INTERFACE_DEFINED__ */


#ifndef __IRemoteArrayObject_INTERFACE_DEFINED__
#define __IRemoteArrayObject_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IRemoteArrayObject
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IRemoteArrayObject __RPC_FAR *LPREMOTEARRAYOBJECT;


EXTERN_C const IID IID_IRemoteArrayObject;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IRemoteArrayObject : public IRemoteObject
    {
    public:
        virtual HRESULT __stdcall GetElementObjects(
            /* [out] */ IEnumRemoteObject __RPC_FAR *__RPC_FAR *ppEnum) = 0;

        virtual HRESULT __stdcall GetSize(
            /* [out] */ ULONG __RPC_FAR *pcElements) = 0;

        virtual HRESULT __stdcall GetElementValues(
            /* [out] */ IEnumRemoteValue __RPC_FAR *__RPC_FAR *ppEnum) = 0;

    };

#else   /* C style interface */

    typedef struct IRemoteArrayObjectVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IRemoteArrayObject __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IRemoteArrayObject __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IRemoteArrayObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetType )(
            IRemoteArrayObject __RPC_FAR * This,
            /* [out] */ IRemoteField __RPC_FAR *__RPC_FAR *ppType);

        HRESULT ( __stdcall __RPC_FAR *SetBreakpoint )(
            IRemoteArrayObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *ClearBreakpoint )(
            IRemoteArrayObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetElementObjects )(
            IRemoteArrayObject __RPC_FAR * This,
            /* [out] */ IEnumRemoteObject __RPC_FAR *__RPC_FAR *ppEnum);

        HRESULT ( __stdcall __RPC_FAR *GetSize )(
            IRemoteArrayObject __RPC_FAR * This,
            /* [out] */ ULONG __RPC_FAR *pcElements);

        HRESULT ( __stdcall __RPC_FAR *GetElementValues )(
            IRemoteArrayObject __RPC_FAR * This,
            /* [out] */ IEnumRemoteValue __RPC_FAR *__RPC_FAR *ppEnum);

    } IRemoteArrayObjectVtbl;

    interface IRemoteArrayObject
    {
        CONST_VTBL struct IRemoteArrayObjectVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IRemoteArrayObject_QueryInterface(This,riid,ppvObject)  \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRemoteArrayObject_AddRef(This) \
    (This)->lpVtbl -> AddRef(This)

#define IRemoteArrayObject_Release(This)        \
    (This)->lpVtbl -> Release(This)


#define IRemoteArrayObject_GetType(This,ppType) \
    (This)->lpVtbl -> GetType(This,ppType)

#define IRemoteArrayObject_SetBreakpoint(This)  \
    (This)->lpVtbl -> SetBreakpoint(This)

#define IRemoteArrayObject_ClearBreakpoint(This)        \
    (This)->lpVtbl -> ClearBreakpoint(This)


#define IRemoteArrayObject_GetElementObjects(This,ppEnum)       \
    (This)->lpVtbl -> GetElementObjects(This,ppEnum)

#define IRemoteArrayObject_GetSize(This,pcElements)     \
    (This)->lpVtbl -> GetSize(This,pcElements)

#define IRemoteArrayObject_GetElementValues(This,ppEnum)        \
    (This)->lpVtbl -> GetElementValues(This,ppEnum)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IRemoteArrayObject_GetElementObjects_Proxy(
    IRemoteArrayObject __RPC_FAR * This,
    /* [out] */ IEnumRemoteObject __RPC_FAR *__RPC_FAR *ppEnum);


void __RPC_STUB IRemoteArrayObject_GetElementObjects_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteArrayObject_GetSize_Proxy(
    IRemoteArrayObject __RPC_FAR * This,
    /* [out] */ ULONG __RPC_FAR *pcElements);


void __RPC_STUB IRemoteArrayObject_GetSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteArrayObject_GetElementValues_Proxy(
    IRemoteArrayObject __RPC_FAR * This,
    /* [out] */ IEnumRemoteValue __RPC_FAR *__RPC_FAR *ppEnum);


void __RPC_STUB IRemoteArrayObject_GetElementValues_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IRemoteArrayObject_INTERFACE_DEFINED__ */


#ifndef __IRemoteBooleanObject_INTERFACE_DEFINED__
#define __IRemoteBooleanObject_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IRemoteBooleanObject
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IRemoteBooleanObject __RPC_FAR *LPREMOTEBOOLEANOBJECT;


EXTERN_C const IID IID_IRemoteBooleanObject;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IRemoteBooleanObject : public IRemoteObject
    {
    public:
        virtual HRESULT __stdcall GetValue(
            /* [out] */ JAVA_BOOLEAN __RPC_FAR *pvalue) = 0;

        virtual HRESULT __stdcall SetValue(
            /* [in] */ JAVA_BOOLEAN value) = 0;

    };

#else   /* C style interface */

    typedef struct IRemoteBooleanObjectVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IRemoteBooleanObject __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IRemoteBooleanObject __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IRemoteBooleanObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetType )(
            IRemoteBooleanObject __RPC_FAR * This,
            /* [out] */ IRemoteField __RPC_FAR *__RPC_FAR *ppType);

        HRESULT ( __stdcall __RPC_FAR *SetBreakpoint )(
            IRemoteBooleanObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *ClearBreakpoint )(
            IRemoteBooleanObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetValue )(
            IRemoteBooleanObject __RPC_FAR * This,
            /* [out] */ JAVA_BOOLEAN __RPC_FAR *pvalue);

        HRESULT ( __stdcall __RPC_FAR *SetValue )(
            IRemoteBooleanObject __RPC_FAR * This,
            /* [in] */ JAVA_BOOLEAN value);

    } IRemoteBooleanObjectVtbl;

    interface IRemoteBooleanObject
    {
        CONST_VTBL struct IRemoteBooleanObjectVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IRemoteBooleanObject_QueryInterface(This,riid,ppvObject)        \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRemoteBooleanObject_AddRef(This)       \
    (This)->lpVtbl -> AddRef(This)

#define IRemoteBooleanObject_Release(This)      \
    (This)->lpVtbl -> Release(This)


#define IRemoteBooleanObject_GetType(This,ppType)       \
    (This)->lpVtbl -> GetType(This,ppType)

#define IRemoteBooleanObject_SetBreakpoint(This)        \
    (This)->lpVtbl -> SetBreakpoint(This)

#define IRemoteBooleanObject_ClearBreakpoint(This)      \
    (This)->lpVtbl -> ClearBreakpoint(This)


#define IRemoteBooleanObject_GetValue(This,pvalue)      \
    (This)->lpVtbl -> GetValue(This,pvalue)

#define IRemoteBooleanObject_SetValue(This,value)       \
    (This)->lpVtbl -> SetValue(This,value)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IRemoteBooleanObject_GetValue_Proxy(
    IRemoteBooleanObject __RPC_FAR * This,
    /* [out] */ JAVA_BOOLEAN __RPC_FAR *pvalue);


void __RPC_STUB IRemoteBooleanObject_GetValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteBooleanObject_SetValue_Proxy(
    IRemoteBooleanObject __RPC_FAR * This,
    /* [in] */ JAVA_BOOLEAN value);


void __RPC_STUB IRemoteBooleanObject_SetValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IRemoteBooleanObject_INTERFACE_DEFINED__ */


#ifndef __IRemoteByteObject_INTERFACE_DEFINED__
#define __IRemoteByteObject_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IRemoteByteObject
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IRemoteByteObject __RPC_FAR *LPREMOTEBYTEOBJECT;


EXTERN_C const IID IID_IRemoteByteObject;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IRemoteByteObject : public IRemoteObject
    {
    public:
        virtual HRESULT __stdcall GetValue(
            /* [out] */ JAVA_BYTE __RPC_FAR *pvalue) = 0;

        virtual HRESULT __stdcall SetValue(
            /* [in] */ JAVA_BYTE value) = 0;

    };

#else   /* C style interface */

    typedef struct IRemoteByteObjectVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IRemoteByteObject __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IRemoteByteObject __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IRemoteByteObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetType )(
            IRemoteByteObject __RPC_FAR * This,
            /* [out] */ IRemoteField __RPC_FAR *__RPC_FAR *ppType);

        HRESULT ( __stdcall __RPC_FAR *SetBreakpoint )(
            IRemoteByteObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *ClearBreakpoint )(
            IRemoteByteObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetValue )(
            IRemoteByteObject __RPC_FAR * This,
            /* [out] */ JAVA_BYTE __RPC_FAR *pvalue);

        HRESULT ( __stdcall __RPC_FAR *SetValue )(
            IRemoteByteObject __RPC_FAR * This,
            /* [in] */ JAVA_BYTE value);

    } IRemoteByteObjectVtbl;

    interface IRemoteByteObject
    {
        CONST_VTBL struct IRemoteByteObjectVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IRemoteByteObject_QueryInterface(This,riid,ppvObject)   \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRemoteByteObject_AddRef(This)  \
    (This)->lpVtbl -> AddRef(This)

#define IRemoteByteObject_Release(This) \
    (This)->lpVtbl -> Release(This)


#define IRemoteByteObject_GetType(This,ppType)  \
    (This)->lpVtbl -> GetType(This,ppType)

#define IRemoteByteObject_SetBreakpoint(This)   \
    (This)->lpVtbl -> SetBreakpoint(This)

#define IRemoteByteObject_ClearBreakpoint(This) \
    (This)->lpVtbl -> ClearBreakpoint(This)


#define IRemoteByteObject_GetValue(This,pvalue) \
    (This)->lpVtbl -> GetValue(This,pvalue)

#define IRemoteByteObject_SetValue(This,value)  \
    (This)->lpVtbl -> SetValue(This,value)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IRemoteByteObject_GetValue_Proxy(
    IRemoteByteObject __RPC_FAR * This,
    /* [out] */ JAVA_BYTE __RPC_FAR *pvalue);


void __RPC_STUB IRemoteByteObject_GetValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteByteObject_SetValue_Proxy(
    IRemoteByteObject __RPC_FAR * This,
    /* [in] */ JAVA_BYTE value);


void __RPC_STUB IRemoteByteObject_SetValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IRemoteByteObject_INTERFACE_DEFINED__ */


#ifndef __IRemoteCharObject_INTERFACE_DEFINED__
#define __IRemoteCharObject_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IRemoteCharObject
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IRemoteCharObject __RPC_FAR *LPREMOTECHAROBJECT;


EXTERN_C const IID IID_IRemoteCharObject;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IRemoteCharObject : public IRemoteObject
    {
    public:
        virtual HRESULT __stdcall GetValue(
            /* [out] */ JAVA_CHAR __RPC_FAR *pvalue) = 0;

        virtual HRESULT __stdcall SetValue(
            /* [in] */ JAVA_CHAR value) = 0;

    };

#else   /* C style interface */

    typedef struct IRemoteCharObjectVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IRemoteCharObject __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IRemoteCharObject __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IRemoteCharObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetType )(
            IRemoteCharObject __RPC_FAR * This,
            /* [out] */ IRemoteField __RPC_FAR *__RPC_FAR *ppType);

        HRESULT ( __stdcall __RPC_FAR *SetBreakpoint )(
            IRemoteCharObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *ClearBreakpoint )(
            IRemoteCharObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetValue )(
            IRemoteCharObject __RPC_FAR * This,
            /* [out] */ JAVA_CHAR __RPC_FAR *pvalue);

        HRESULT ( __stdcall __RPC_FAR *SetValue )(
            IRemoteCharObject __RPC_FAR * This,
            /* [in] */ JAVA_CHAR value);

    } IRemoteCharObjectVtbl;

    interface IRemoteCharObject
    {
        CONST_VTBL struct IRemoteCharObjectVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IRemoteCharObject_QueryInterface(This,riid,ppvObject)   \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRemoteCharObject_AddRef(This)  \
    (This)->lpVtbl -> AddRef(This)

#define IRemoteCharObject_Release(This) \
    (This)->lpVtbl -> Release(This)


#define IRemoteCharObject_GetType(This,ppType)  \
    (This)->lpVtbl -> GetType(This,ppType)

#define IRemoteCharObject_SetBreakpoint(This)   \
    (This)->lpVtbl -> SetBreakpoint(This)

#define IRemoteCharObject_ClearBreakpoint(This) \
    (This)->lpVtbl -> ClearBreakpoint(This)


#define IRemoteCharObject_GetValue(This,pvalue) \
    (This)->lpVtbl -> GetValue(This,pvalue)

#define IRemoteCharObject_SetValue(This,value)  \
    (This)->lpVtbl -> SetValue(This,value)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IRemoteCharObject_GetValue_Proxy(
    IRemoteCharObject __RPC_FAR * This,
    /* [out] */ JAVA_CHAR __RPC_FAR *pvalue);


void __RPC_STUB IRemoteCharObject_GetValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteCharObject_SetValue_Proxy(
    IRemoteCharObject __RPC_FAR * This,
    /* [in] */ JAVA_CHAR value);


void __RPC_STUB IRemoteCharObject_SetValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IRemoteCharObject_INTERFACE_DEFINED__ */


#ifndef __IRemoteContainerObject_INTERFACE_DEFINED__
#define __IRemoteContainerObject_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IRemoteContainerObject
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IRemoteContainerObject __RPC_FAR *LPREMOTECONTAINEROBJECT;


EXTERN_C const IID IID_IRemoteContainerObject;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IRemoteContainerObject : public IRemoteObject
    {
    public:
        virtual HRESULT __stdcall GetFieldObject(
            /* [unique][in] */ IRemoteField __RPC_FAR *pField,
            /* [out] */ IRemoteObject __RPC_FAR *__RPC_FAR *ppFieldObject) = 0;

    };

#else   /* C style interface */

    typedef struct IRemoteContainerObjectVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IRemoteContainerObject __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IRemoteContainerObject __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IRemoteContainerObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetType )(
            IRemoteContainerObject __RPC_FAR * This,
            /* [out] */ IRemoteField __RPC_FAR *__RPC_FAR *ppType);

        HRESULT ( __stdcall __RPC_FAR *SetBreakpoint )(
            IRemoteContainerObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *ClearBreakpoint )(
            IRemoteContainerObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetFieldObject )(
            IRemoteContainerObject __RPC_FAR * This,
            /* [unique][in] */ IRemoteField __RPC_FAR *pField,
            /* [out] */ IRemoteObject __RPC_FAR *__RPC_FAR *ppFieldObject);

    } IRemoteContainerObjectVtbl;

    interface IRemoteContainerObject
    {
        CONST_VTBL struct IRemoteContainerObjectVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IRemoteContainerObject_QueryInterface(This,riid,ppvObject)      \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRemoteContainerObject_AddRef(This)     \
    (This)->lpVtbl -> AddRef(This)

#define IRemoteContainerObject_Release(This)    \
    (This)->lpVtbl -> Release(This)


#define IRemoteContainerObject_GetType(This,ppType)     \
    (This)->lpVtbl -> GetType(This,ppType)

#define IRemoteContainerObject_SetBreakpoint(This)      \
    (This)->lpVtbl -> SetBreakpoint(This)

#define IRemoteContainerObject_ClearBreakpoint(This)    \
    (This)->lpVtbl -> ClearBreakpoint(This)


#define IRemoteContainerObject_GetFieldObject(This,pField,ppFieldObject)        \
    (This)->lpVtbl -> GetFieldObject(This,pField,ppFieldObject)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IRemoteContainerObject_GetFieldObject_Proxy(
    IRemoteContainerObject __RPC_FAR * This,
    /* [unique][in] */ IRemoteField __RPC_FAR *pField,
    /* [out] */ IRemoteObject __RPC_FAR *__RPC_FAR *ppFieldObject);


void __RPC_STUB IRemoteContainerObject_GetFieldObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IRemoteContainerObject_INTERFACE_DEFINED__ */


#ifndef __IRemoteClassObject_INTERFACE_DEFINED__
#define __IRemoteClassObject_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IRemoteClassObject
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IRemoteClassObject __RPC_FAR *LPREMOTECLASSOBJECT;


EXTERN_C const IID IID_IRemoteClassObject;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IRemoteClassObject : public IRemoteContainerObject
    {
    public:
        virtual HRESULT __stdcall GetDerivedMostType(
            /* [out] */ IRemoteClassField __RPC_FAR *__RPC_FAR *ppDerivedMostField) = 0;

    };

#else   /* C style interface */

    typedef struct IRemoteClassObjectVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IRemoteClassObject __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IRemoteClassObject __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IRemoteClassObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetType )(
            IRemoteClassObject __RPC_FAR * This,
            /* [out] */ IRemoteField __RPC_FAR *__RPC_FAR *ppType);

        HRESULT ( __stdcall __RPC_FAR *SetBreakpoint )(
            IRemoteClassObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *ClearBreakpoint )(
            IRemoteClassObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetFieldObject )(
            IRemoteClassObject __RPC_FAR * This,
            /* [unique][in] */ IRemoteField __RPC_FAR *pField,
            /* [out] */ IRemoteObject __RPC_FAR *__RPC_FAR *ppFieldObject);

        HRESULT ( __stdcall __RPC_FAR *GetDerivedMostType )(
            IRemoteClassObject __RPC_FAR * This,
            /* [out] */ IRemoteClassField __RPC_FAR *__RPC_FAR *ppDerivedMostField);

    } IRemoteClassObjectVtbl;

    interface IRemoteClassObject
    {
        CONST_VTBL struct IRemoteClassObjectVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IRemoteClassObject_QueryInterface(This,riid,ppvObject)  \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRemoteClassObject_AddRef(This) \
    (This)->lpVtbl -> AddRef(This)

#define IRemoteClassObject_Release(This)        \
    (This)->lpVtbl -> Release(This)


#define IRemoteClassObject_GetType(This,ppType) \
    (This)->lpVtbl -> GetType(This,ppType)

#define IRemoteClassObject_SetBreakpoint(This)  \
    (This)->lpVtbl -> SetBreakpoint(This)

#define IRemoteClassObject_ClearBreakpoint(This)        \
    (This)->lpVtbl -> ClearBreakpoint(This)


#define IRemoteClassObject_GetFieldObject(This,pField,ppFieldObject)    \
    (This)->lpVtbl -> GetFieldObject(This,pField,ppFieldObject)


#define IRemoteClassObject_GetDerivedMostType(This,ppDerivedMostField)  \
    (This)->lpVtbl -> GetDerivedMostType(This,ppDerivedMostField)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IRemoteClassObject_GetDerivedMostType_Proxy(
    IRemoteClassObject __RPC_FAR * This,
    /* [out] */ IRemoteClassField __RPC_FAR *__RPC_FAR *ppDerivedMostField);


void __RPC_STUB IRemoteClassObject_GetDerivedMostType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IRemoteClassObject_INTERFACE_DEFINED__ */


#ifndef __IRemoteDoubleObject_INTERFACE_DEFINED__
#define __IRemoteDoubleObject_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IRemoteDoubleObject
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IRemoteDoubleObject __RPC_FAR *LPREMOTEDOUBLEOBJECT;


EXTERN_C const IID IID_IRemoteDoubleObject;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IRemoteDoubleObject : public IRemoteObject
    {
    public:
        virtual HRESULT __stdcall GetValue(
            /* [out] */ JAVA_DOUBLE __RPC_FAR *pvalue) = 0;

        virtual HRESULT __stdcall SetValue(
            /* [in] */ JAVA_DOUBLE __RPC_FAR *pvalue) = 0;

    };

#else   /* C style interface */

    typedef struct IRemoteDoubleObjectVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IRemoteDoubleObject __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IRemoteDoubleObject __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IRemoteDoubleObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetType )(
            IRemoteDoubleObject __RPC_FAR * This,
            /* [out] */ IRemoteField __RPC_FAR *__RPC_FAR *ppType);

        HRESULT ( __stdcall __RPC_FAR *SetBreakpoint )(
            IRemoteDoubleObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *ClearBreakpoint )(
            IRemoteDoubleObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetValue )(
            IRemoteDoubleObject __RPC_FAR * This,
            /* [out] */ JAVA_DOUBLE __RPC_FAR *pvalue);

        HRESULT ( __stdcall __RPC_FAR *SetValue )(
            IRemoteDoubleObject __RPC_FAR * This,
            /* [in] */ JAVA_DOUBLE __RPC_FAR *pvalue);

    } IRemoteDoubleObjectVtbl;

    interface IRemoteDoubleObject
    {
        CONST_VTBL struct IRemoteDoubleObjectVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IRemoteDoubleObject_QueryInterface(This,riid,ppvObject) \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRemoteDoubleObject_AddRef(This)        \
    (This)->lpVtbl -> AddRef(This)

#define IRemoteDoubleObject_Release(This)       \
    (This)->lpVtbl -> Release(This)


#define IRemoteDoubleObject_GetType(This,ppType)        \
    (This)->lpVtbl -> GetType(This,ppType)

#define IRemoteDoubleObject_SetBreakpoint(This) \
    (This)->lpVtbl -> SetBreakpoint(This)

#define IRemoteDoubleObject_ClearBreakpoint(This)       \
    (This)->lpVtbl -> ClearBreakpoint(This)


#define IRemoteDoubleObject_GetValue(This,pvalue)       \
    (This)->lpVtbl -> GetValue(This,pvalue)

#define IRemoteDoubleObject_SetValue(This,pvalue)       \
    (This)->lpVtbl -> SetValue(This,pvalue)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IRemoteDoubleObject_GetValue_Proxy(
    IRemoteDoubleObject __RPC_FAR * This,
    /* [out] */ JAVA_DOUBLE __RPC_FAR *pvalue);


void __RPC_STUB IRemoteDoubleObject_GetValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteDoubleObject_SetValue_Proxy(
    IRemoteDoubleObject __RPC_FAR * This,
    /* [in] */ JAVA_DOUBLE __RPC_FAR *pvalue);


void __RPC_STUB IRemoteDoubleObject_SetValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IRemoteDoubleObject_INTERFACE_DEFINED__ */


#ifndef __IRemoteFloatObject_INTERFACE_DEFINED__
#define __IRemoteFloatObject_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IRemoteFloatObject
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IRemoteFloatObject __RPC_FAR *LPREMOTEFLOATOBJECT;


EXTERN_C const IID IID_IRemoteFloatObject;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IRemoteFloatObject : public IRemoteObject
    {
    public:
        virtual HRESULT __stdcall GetValue(
            /* [out] */ JAVA_FLOAT __RPC_FAR *pvalue) = 0;

        virtual HRESULT __stdcall SetValue(
            /* [in] */ JAVA_FLOAT __RPC_FAR *pvalue) = 0;

    };

#else   /* C style interface */

    typedef struct IRemoteFloatObjectVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IRemoteFloatObject __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IRemoteFloatObject __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IRemoteFloatObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetType )(
            IRemoteFloatObject __RPC_FAR * This,
            /* [out] */ IRemoteField __RPC_FAR *__RPC_FAR *ppType);

        HRESULT ( __stdcall __RPC_FAR *SetBreakpoint )(
            IRemoteFloatObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *ClearBreakpoint )(
            IRemoteFloatObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetValue )(
            IRemoteFloatObject __RPC_FAR * This,
            /* [out] */ JAVA_FLOAT __RPC_FAR *pvalue);

        HRESULT ( __stdcall __RPC_FAR *SetValue )(
            IRemoteFloatObject __RPC_FAR * This,
            /* [in] */ JAVA_FLOAT __RPC_FAR *pvalue);

    } IRemoteFloatObjectVtbl;

    interface IRemoteFloatObject
    {
        CONST_VTBL struct IRemoteFloatObjectVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IRemoteFloatObject_QueryInterface(This,riid,ppvObject)  \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRemoteFloatObject_AddRef(This) \
    (This)->lpVtbl -> AddRef(This)

#define IRemoteFloatObject_Release(This)        \
    (This)->lpVtbl -> Release(This)


#define IRemoteFloatObject_GetType(This,ppType) \
    (This)->lpVtbl -> GetType(This,ppType)

#define IRemoteFloatObject_SetBreakpoint(This)  \
    (This)->lpVtbl -> SetBreakpoint(This)

#define IRemoteFloatObject_ClearBreakpoint(This)        \
    (This)->lpVtbl -> ClearBreakpoint(This)


#define IRemoteFloatObject_GetValue(This,pvalue)        \
    (This)->lpVtbl -> GetValue(This,pvalue)

#define IRemoteFloatObject_SetValue(This,pvalue)        \
    (This)->lpVtbl -> SetValue(This,pvalue)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IRemoteFloatObject_GetValue_Proxy(
    IRemoteFloatObject __RPC_FAR * This,
    /* [out] */ JAVA_FLOAT __RPC_FAR *pvalue);


void __RPC_STUB IRemoteFloatObject_GetValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteFloatObject_SetValue_Proxy(
    IRemoteFloatObject __RPC_FAR * This,
    /* [in] */ JAVA_FLOAT __RPC_FAR *pvalue);


void __RPC_STUB IRemoteFloatObject_SetValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IRemoteFloatObject_INTERFACE_DEFINED__ */


#ifndef __IRemoteIntObject_INTERFACE_DEFINED__
#define __IRemoteIntObject_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IRemoteIntObject
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IRemoteIntObject __RPC_FAR *LPREMOTEINTOBJECT;


EXTERN_C const IID IID_IRemoteIntObject;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IRemoteIntObject : public IRemoteObject
    {
    public:
        virtual HRESULT __stdcall GetValue(
            /* [out] */ JAVA_INT __RPC_FAR *pvalue) = 0;

        virtual HRESULT __stdcall SetValue(
            /* [in] */ JAVA_INT value) = 0;

    };

#else   /* C style interface */

    typedef struct IRemoteIntObjectVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IRemoteIntObject __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IRemoteIntObject __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IRemoteIntObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetType )(
            IRemoteIntObject __RPC_FAR * This,
            /* [out] */ IRemoteField __RPC_FAR *__RPC_FAR *ppType);

        HRESULT ( __stdcall __RPC_FAR *SetBreakpoint )(
            IRemoteIntObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *ClearBreakpoint )(
            IRemoteIntObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetValue )(
            IRemoteIntObject __RPC_FAR * This,
            /* [out] */ JAVA_INT __RPC_FAR *pvalue);

        HRESULT ( __stdcall __RPC_FAR *SetValue )(
            IRemoteIntObject __RPC_FAR * This,
            /* [in] */ JAVA_INT value);

    } IRemoteIntObjectVtbl;

    interface IRemoteIntObject
    {
        CONST_VTBL struct IRemoteIntObjectVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IRemoteIntObject_QueryInterface(This,riid,ppvObject)    \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRemoteIntObject_AddRef(This)   \
    (This)->lpVtbl -> AddRef(This)

#define IRemoteIntObject_Release(This)  \
    (This)->lpVtbl -> Release(This)


#define IRemoteIntObject_GetType(This,ppType)   \
    (This)->lpVtbl -> GetType(This,ppType)

#define IRemoteIntObject_SetBreakpoint(This)    \
    (This)->lpVtbl -> SetBreakpoint(This)

#define IRemoteIntObject_ClearBreakpoint(This)  \
    (This)->lpVtbl -> ClearBreakpoint(This)


#define IRemoteIntObject_GetValue(This,pvalue)  \
    (This)->lpVtbl -> GetValue(This,pvalue)

#define IRemoteIntObject_SetValue(This,value)   \
    (This)->lpVtbl -> SetValue(This,value)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IRemoteIntObject_GetValue_Proxy(
    IRemoteIntObject __RPC_FAR * This,
    /* [out] */ JAVA_INT __RPC_FAR *pvalue);


void __RPC_STUB IRemoteIntObject_GetValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteIntObject_SetValue_Proxy(
    IRemoteIntObject __RPC_FAR * This,
    /* [in] */ JAVA_INT value);


void __RPC_STUB IRemoteIntObject_SetValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IRemoteIntObject_INTERFACE_DEFINED__ */


#ifndef __IRemoteLongObject_INTERFACE_DEFINED__
#define __IRemoteLongObject_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IRemoteLongObject
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IRemoteLongObject __RPC_FAR *LPREMOTELONGOBJECT;


EXTERN_C const IID IID_IRemoteLongObject;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IRemoteLongObject : public IRemoteObject
    {
    public:
        virtual HRESULT __stdcall GetValue(
            /* [out] */ JAVA_LONG __RPC_FAR *pvalue) = 0;

        virtual HRESULT __stdcall SetValue(
            /* [in] */ JAVA_LONG value) = 0;

    };

#else   /* C style interface */

    typedef struct IRemoteLongObjectVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IRemoteLongObject __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IRemoteLongObject __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IRemoteLongObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetType )(
            IRemoteLongObject __RPC_FAR * This,
            /* [out] */ IRemoteField __RPC_FAR *__RPC_FAR *ppType);

        HRESULT ( __stdcall __RPC_FAR *SetBreakpoint )(
            IRemoteLongObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *ClearBreakpoint )(
            IRemoteLongObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetValue )(
            IRemoteLongObject __RPC_FAR * This,
            /* [out] */ JAVA_LONG __RPC_FAR *pvalue);

        HRESULT ( __stdcall __RPC_FAR *SetValue )(
            IRemoteLongObject __RPC_FAR * This,
            /* [in] */ JAVA_LONG value);

    } IRemoteLongObjectVtbl;

    interface IRemoteLongObject
    {
        CONST_VTBL struct IRemoteLongObjectVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IRemoteLongObject_QueryInterface(This,riid,ppvObject)   \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRemoteLongObject_AddRef(This)  \
    (This)->lpVtbl -> AddRef(This)

#define IRemoteLongObject_Release(This) \
    (This)->lpVtbl -> Release(This)


#define IRemoteLongObject_GetType(This,ppType)  \
    (This)->lpVtbl -> GetType(This,ppType)

#define IRemoteLongObject_SetBreakpoint(This)   \
    (This)->lpVtbl -> SetBreakpoint(This)

#define IRemoteLongObject_ClearBreakpoint(This) \
    (This)->lpVtbl -> ClearBreakpoint(This)


#define IRemoteLongObject_GetValue(This,pvalue) \
    (This)->lpVtbl -> GetValue(This,pvalue)

#define IRemoteLongObject_SetValue(This,value)  \
    (This)->lpVtbl -> SetValue(This,value)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IRemoteLongObject_GetValue_Proxy(
    IRemoteLongObject __RPC_FAR * This,
    /* [out] */ JAVA_LONG __RPC_FAR *pvalue);


void __RPC_STUB IRemoteLongObject_GetValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteLongObject_SetValue_Proxy(
    IRemoteLongObject __RPC_FAR * This,
    /* [in] */ JAVA_LONG value);


void __RPC_STUB IRemoteLongObject_SetValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IRemoteLongObject_INTERFACE_DEFINED__ */


#ifndef __IRemoteShortObject_INTERFACE_DEFINED__
#define __IRemoteShortObject_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IRemoteShortObject
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IRemoteShortObject __RPC_FAR *LPREMOTESHORTOBJECT;


EXTERN_C const IID IID_IRemoteShortObject;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IRemoteShortObject : public IRemoteObject
    {
    public:
        virtual HRESULT __stdcall GetValue(
            /* [out] */ JAVA_SHORT __RPC_FAR *pvalue) = 0;

        virtual HRESULT __stdcall SetValue(
            /* [in] */ JAVA_SHORT value) = 0;

    };

#else   /* C style interface */

    typedef struct IRemoteShortObjectVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IRemoteShortObject __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IRemoteShortObject __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IRemoteShortObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetType )(
            IRemoteShortObject __RPC_FAR * This,
            /* [out] */ IRemoteField __RPC_FAR *__RPC_FAR *ppType);

        HRESULT ( __stdcall __RPC_FAR *SetBreakpoint )(
            IRemoteShortObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *ClearBreakpoint )(
            IRemoteShortObject __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetValue )(
            IRemoteShortObject __RPC_FAR * This,
            /* [out] */ JAVA_SHORT __RPC_FAR *pvalue);

        HRESULT ( __stdcall __RPC_FAR *SetValue )(
            IRemoteShortObject __RPC_FAR * This,
            /* [in] */ JAVA_SHORT value);

    } IRemoteShortObjectVtbl;

    interface IRemoteShortObject
    {
        CONST_VTBL struct IRemoteShortObjectVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IRemoteShortObject_QueryInterface(This,riid,ppvObject)  \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRemoteShortObject_AddRef(This) \
    (This)->lpVtbl -> AddRef(This)

#define IRemoteShortObject_Release(This)        \
    (This)->lpVtbl -> Release(This)


#define IRemoteShortObject_GetType(This,ppType) \
    (This)->lpVtbl -> GetType(This,ppType)

#define IRemoteShortObject_SetBreakpoint(This)  \
    (This)->lpVtbl -> SetBreakpoint(This)

#define IRemoteShortObject_ClearBreakpoint(This)        \
    (This)->lpVtbl -> ClearBreakpoint(This)


#define IRemoteShortObject_GetValue(This,pvalue)        \
    (This)->lpVtbl -> GetValue(This,pvalue)

#define IRemoteShortObject_SetValue(This,value) \
    (This)->lpVtbl -> SetValue(This,value)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IRemoteShortObject_GetValue_Proxy(
    IRemoteShortObject __RPC_FAR * This,
    /* [out] */ JAVA_SHORT __RPC_FAR *pvalue);


void __RPC_STUB IRemoteShortObject_GetValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteShortObject_SetValue_Proxy(
    IRemoteShortObject __RPC_FAR * This,
    /* [in] */ JAVA_SHORT value);


void __RPC_STUB IRemoteShortObject_SetValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IRemoteShortObject_INTERFACE_DEFINED__ */


#ifndef __IRemoteStackFrame_INTERFACE_DEFINED__
#define __IRemoteStackFrame_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IRemoteStackFrame
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IRemoteStackFrame __RPC_FAR *LPREMOTESTACKOBJECT;

                        /* size is 2 */

enum __MIDL_IRemoteStackFrame_0001
    {   FRAME_KIND_INVALID      = 0,
        FRAME_KIND_INTERPRETED  = 0x1,
        FRAME_KIND_NATIVE       = 0x2,
        FRAME_KIND_JIT_COMPILED = 0x3
    };
                        /* size is 4 */
typedef ULONG FRAMEKIND;


EXTERN_C const IID IID_IRemoteStackFrame;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IRemoteStackFrame : public IUnknown
    {
    public:
        virtual HRESULT __stdcall GetCallingFrame(
            /* [out] */ IRemoteStackFrame __RPC_FAR *__RPC_FAR *ppCallingFrame) = 0;

        virtual HRESULT __stdcall GetMethodObject(
            /* [out] */ IRemoteContainerObject __RPC_FAR *__RPC_FAR *ppMethodObject) = 0;

        virtual HRESULT __stdcall GetPC(
            /* [out] */ ULONG __RPC_FAR *offPC) = 0;

        virtual HRESULT __stdcall SetPC(
            /* [in] */ ULONG offPC) = 0;

        virtual HRESULT __stdcall GetKind(
            /* [out] */ FRAMEKIND __RPC_FAR *pfk) = 0;

    };

#else   /* C style interface */

    typedef struct IRemoteStackFrameVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IRemoteStackFrame __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IRemoteStackFrame __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IRemoteStackFrame __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetCallingFrame )(
            IRemoteStackFrame __RPC_FAR * This,
            /* [out] */ IRemoteStackFrame __RPC_FAR *__RPC_FAR *ppCallingFrame);

        HRESULT ( __stdcall __RPC_FAR *GetMethodObject )(
            IRemoteStackFrame __RPC_FAR * This,
            /* [out] */ IRemoteContainerObject __RPC_FAR *__RPC_FAR *ppMethodObject);

        HRESULT ( __stdcall __RPC_FAR *GetPC )(
            IRemoteStackFrame __RPC_FAR * This,
            /* [out] */ ULONG __RPC_FAR *offPC);

        HRESULT ( __stdcall __RPC_FAR *SetPC )(
            IRemoteStackFrame __RPC_FAR * This,
            /* [in] */ ULONG offPC);

        HRESULT ( __stdcall __RPC_FAR *GetKind )(
            IRemoteStackFrame __RPC_FAR * This,
            /* [out] */ FRAMEKIND __RPC_FAR *pfk);

    } IRemoteStackFrameVtbl;

    interface IRemoteStackFrame
    {
        CONST_VTBL struct IRemoteStackFrameVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IRemoteStackFrame_QueryInterface(This,riid,ppvObject)   \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRemoteStackFrame_AddRef(This)  \
    (This)->lpVtbl -> AddRef(This)

#define IRemoteStackFrame_Release(This) \
    (This)->lpVtbl -> Release(This)


#define IRemoteStackFrame_GetCallingFrame(This,ppCallingFrame)  \
    (This)->lpVtbl -> GetCallingFrame(This,ppCallingFrame)

#define IRemoteStackFrame_GetMethodObject(This,ppMethodObject)  \
    (This)->lpVtbl -> GetMethodObject(This,ppMethodObject)

#define IRemoteStackFrame_GetPC(This,offPC)     \
    (This)->lpVtbl -> GetPC(This,offPC)

#define IRemoteStackFrame_SetPC(This,offPC)     \
    (This)->lpVtbl -> SetPC(This,offPC)

#define IRemoteStackFrame_GetKind(This,pfk)     \
    (This)->lpVtbl -> GetKind(This,pfk)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IRemoteStackFrame_GetCallingFrame_Proxy(
    IRemoteStackFrame __RPC_FAR * This,
    /* [out] */ IRemoteStackFrame __RPC_FAR *__RPC_FAR *ppCallingFrame);


void __RPC_STUB IRemoteStackFrame_GetCallingFrame_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteStackFrame_GetMethodObject_Proxy(
    IRemoteStackFrame __RPC_FAR * This,
    /* [out] */ IRemoteContainerObject __RPC_FAR *__RPC_FAR *ppMethodObject);


void __RPC_STUB IRemoteStackFrame_GetMethodObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteStackFrame_GetPC_Proxy(
    IRemoteStackFrame __RPC_FAR * This,
    /* [out] */ ULONG __RPC_FAR *offPC);


void __RPC_STUB IRemoteStackFrame_GetPC_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteStackFrame_SetPC_Proxy(
    IRemoteStackFrame __RPC_FAR * This,
    /* [in] */ ULONG offPC);


void __RPC_STUB IRemoteStackFrame_SetPC_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteStackFrame_GetKind_Proxy(
    IRemoteStackFrame __RPC_FAR * This,
    /* [out] */ FRAMEKIND __RPC_FAR *pfk);


void __RPC_STUB IRemoteStackFrame_GetKind_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IRemoteStackFrame_INTERFACE_DEFINED__ */


#ifndef __IRemoteThreadGroup_INTERFACE_DEFINED__
#define __IRemoteThreadGroup_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IRemoteThreadGroup
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IRemoteThreadGroup __RPC_FAR *LPREMOTETHREADGROUP;


EXTERN_C const IID IID_IRemoteThreadGroup;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IRemoteThreadGroup : public IUnknown
    {
    public:
        virtual HRESULT __stdcall GetName(
            /* [out] */ LPOLESTR __RPC_FAR *ppszName) = 0;

        virtual HRESULT __stdcall GetThreads(
            /* [out] */ IEnumRemoteThread __RPC_FAR *__RPC_FAR *ppEnum) = 0;

        virtual HRESULT __stdcall GetThreadGroups(
            /* [out] */ IEnumRemoteThreadGroup __RPC_FAR *__RPC_FAR *ppEnum) = 0;

    };

#else   /* C style interface */

    typedef struct IRemoteThreadGroupVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IRemoteThreadGroup __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IRemoteThreadGroup __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IRemoteThreadGroup __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetName )(
            IRemoteThreadGroup __RPC_FAR * This,
            /* [out] */ LPOLESTR __RPC_FAR *ppszName);

        HRESULT ( __stdcall __RPC_FAR *GetThreads )(
            IRemoteThreadGroup __RPC_FAR * This,
            /* [out] */ IEnumRemoteThread __RPC_FAR *__RPC_FAR *ppEnum);

        HRESULT ( __stdcall __RPC_FAR *GetThreadGroups )(
            IRemoteThreadGroup __RPC_FAR * This,
            /* [out] */ IEnumRemoteThreadGroup __RPC_FAR *__RPC_FAR *ppEnum);

    } IRemoteThreadGroupVtbl;

    interface IRemoteThreadGroup
    {
        CONST_VTBL struct IRemoteThreadGroupVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IRemoteThreadGroup_QueryInterface(This,riid,ppvObject)  \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRemoteThreadGroup_AddRef(This) \
    (This)->lpVtbl -> AddRef(This)

#define IRemoteThreadGroup_Release(This)        \
    (This)->lpVtbl -> Release(This)


#define IRemoteThreadGroup_GetName(This,ppszName)       \
    (This)->lpVtbl -> GetName(This,ppszName)

#define IRemoteThreadGroup_GetThreads(This,ppEnum)      \
    (This)->lpVtbl -> GetThreads(This,ppEnum)

#define IRemoteThreadGroup_GetThreadGroups(This,ppEnum) \
    (This)->lpVtbl -> GetThreadGroups(This,ppEnum)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IRemoteThreadGroup_GetName_Proxy(
    IRemoteThreadGroup __RPC_FAR * This,
    /* [out] */ LPOLESTR __RPC_FAR *ppszName);


void __RPC_STUB IRemoteThreadGroup_GetName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteThreadGroup_GetThreads_Proxy(
    IRemoteThreadGroup __RPC_FAR * This,
    /* [out] */ IEnumRemoteThread __RPC_FAR *__RPC_FAR *ppEnum);


void __RPC_STUB IRemoteThreadGroup_GetThreads_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteThreadGroup_GetThreadGroups_Proxy(
    IRemoteThreadGroup __RPC_FAR * This,
    /* [out] */ IEnumRemoteThreadGroup __RPC_FAR *__RPC_FAR *ppEnum);


void __RPC_STUB IRemoteThreadGroup_GetThreadGroups_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IRemoteThreadGroup_INTERFACE_DEFINED__ */


#ifndef __IEnumRemoteThreadGroup_INTERFACE_DEFINED__
#define __IEnumRemoteThreadGroup_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IEnumRemoteThreadGroup
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IEnumRemoteThreadGroup __RPC_FAR *LPENUMREMOTETHREADGROUP;


EXTERN_C const IID IID_IEnumRemoteThreadGroup;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IEnumRemoteThreadGroup : public IUnknown
    {
    public:
        virtual HRESULT __stdcall Next(
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IRemoteThreadGroup __RPC_FAR *__RPC_FAR rgelt[  ],
            /* [out] */ ULONG __RPC_FAR *pceltFetched) = 0;

        virtual HRESULT __stdcall Skip(
            /* [in] */ ULONG celt) = 0;

        virtual HRESULT __stdcall Reset( void) = 0;

        virtual HRESULT __stdcall Clone(
            /* [out] */ IEnumRemoteThreadGroup __RPC_FAR *__RPC_FAR *ppEnum) = 0;

    };

#else   /* C style interface */

    typedef struct IEnumRemoteThreadGroupVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IEnumRemoteThreadGroup __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IEnumRemoteThreadGroup __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IEnumRemoteThreadGroup __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Next )(
            IEnumRemoteThreadGroup __RPC_FAR * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IRemoteThreadGroup __RPC_FAR *__RPC_FAR rgelt[  ],
            /* [out] */ ULONG __RPC_FAR *pceltFetched);

        HRESULT ( __stdcall __RPC_FAR *Skip )(
            IEnumRemoteThreadGroup __RPC_FAR * This,
            /* [in] */ ULONG celt);

        HRESULT ( __stdcall __RPC_FAR *Reset )(
            IEnumRemoteThreadGroup __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Clone )(
            IEnumRemoteThreadGroup __RPC_FAR * This,
            /* [out] */ IEnumRemoteThreadGroup __RPC_FAR *__RPC_FAR *ppEnum);

    } IEnumRemoteThreadGroupVtbl;

    interface IEnumRemoteThreadGroup
    {
        CONST_VTBL struct IEnumRemoteThreadGroupVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IEnumRemoteThreadGroup_QueryInterface(This,riid,ppvObject)      \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumRemoteThreadGroup_AddRef(This)     \
    (This)->lpVtbl -> AddRef(This)

#define IEnumRemoteThreadGroup_Release(This)    \
    (This)->lpVtbl -> Release(This)


#define IEnumRemoteThreadGroup_Next(This,celt,rgelt,pceltFetched)       \
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumRemoteThreadGroup_Skip(This,celt)  \
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumRemoteThreadGroup_Reset(This)      \
    (This)->lpVtbl -> Reset(This)

#define IEnumRemoteThreadGroup_Clone(This,ppEnum)       \
    (This)->lpVtbl -> Clone(This,ppEnum)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IEnumRemoteThreadGroup_Next_Proxy(
    IEnumRemoteThreadGroup __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ IRemoteThreadGroup __RPC_FAR *__RPC_FAR rgelt[  ],
    /* [out] */ ULONG __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumRemoteThreadGroup_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumRemoteThreadGroup_Skip_Proxy(
    IEnumRemoteThreadGroup __RPC_FAR * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumRemoteThreadGroup_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumRemoteThreadGroup_Reset_Proxy(
    IEnumRemoteThreadGroup __RPC_FAR * This);


void __RPC_STUB IEnumRemoteThreadGroup_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumRemoteThreadGroup_Clone_Proxy(
    IEnumRemoteThreadGroup __RPC_FAR * This,
    /* [out] */ IEnumRemoteThreadGroup __RPC_FAR *__RPC_FAR *ppEnum);


void __RPC_STUB IEnumRemoteThreadGroup_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IEnumRemoteThreadGroup_INTERFACE_DEFINED__ */


#ifndef __IRemoteThread_INTERFACE_DEFINED__
#define __IRemoteThread_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IRemoteThread
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IRemoteThread __RPC_FAR *LPREMOTETHREAD;


EXTERN_C const IID IID_IRemoteThread;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IRemoteThread : public IUnknown
    {
    public:
        virtual HRESULT __stdcall GetName(
            /* [out] */ LPOLESTR __RPC_FAR *ppszName) = 0;

        virtual HRESULT __stdcall GetCurrentFrame(
            /* [out] */ IRemoteStackFrame __RPC_FAR *__RPC_FAR *ppCurrentFrame) = 0;

        virtual HRESULT __stdcall GetThreadGroup(
            /* [out] */ IRemoteThreadGroup __RPC_FAR *__RPC_FAR *ppThreadGroup) = 0;

        virtual HRESULT __stdcall Go( void) = 0;

        virtual HRESULT __stdcall Step( void) = 0;

        virtual HRESULT __stdcall StepIn( void) = 0;

        virtual HRESULT __stdcall StepOut( void) = 0;

        virtual HRESULT __stdcall RangeStep(
            /* [in] */ ULONG offStart,
            /* [in] */ ULONG offEnd) = 0;

        virtual HRESULT __stdcall RangeStepIn(
            /* [in] */ ULONG offStart,
            /* [in] */ ULONG offEnd) = 0;

        virtual HRESULT __stdcall Destroy( void) = 0;

        virtual HRESULT __stdcall Suspend( void) = 0;

        virtual HRESULT __stdcall Resume( void) = 0;

        virtual HRESULT __stdcall GetSuspendCount(
            /* [out] */ ULONG __RPC_FAR *pcSuspend) = 0;

    };

#else   /* C style interface */

    typedef struct IRemoteThreadVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IRemoteThread __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IRemoteThread __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IRemoteThread __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetName )(
            IRemoteThread __RPC_FAR * This,
            /* [out] */ LPOLESTR __RPC_FAR *ppszName);

        HRESULT ( __stdcall __RPC_FAR *GetCurrentFrame )(
            IRemoteThread __RPC_FAR * This,
            /* [out] */ IRemoteStackFrame __RPC_FAR *__RPC_FAR *ppCurrentFrame);

        HRESULT ( __stdcall __RPC_FAR *GetThreadGroup )(
            IRemoteThread __RPC_FAR * This,
            /* [out] */ IRemoteThreadGroup __RPC_FAR *__RPC_FAR *ppThreadGroup);

        HRESULT ( __stdcall __RPC_FAR *Go )(
            IRemoteThread __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Step )(
            IRemoteThread __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *StepIn )(
            IRemoteThread __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *StepOut )(
            IRemoteThread __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *RangeStep )(
            IRemoteThread __RPC_FAR * This,
            /* [in] */ ULONG offStart,
            /* [in] */ ULONG offEnd);

        HRESULT ( __stdcall __RPC_FAR *RangeStepIn )(
            IRemoteThread __RPC_FAR * This,
            /* [in] */ ULONG offStart,
            /* [in] */ ULONG offEnd);

        HRESULT ( __stdcall __RPC_FAR *Destroy )(
            IRemoteThread __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Suspend )(
            IRemoteThread __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Resume )(
            IRemoteThread __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetSuspendCount )(
            IRemoteThread __RPC_FAR * This,
            /* [out] */ ULONG __RPC_FAR *pcSuspend);

    } IRemoteThreadVtbl;

    interface IRemoteThread
    {
        CONST_VTBL struct IRemoteThreadVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IRemoteThread_QueryInterface(This,riid,ppvObject)       \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRemoteThread_AddRef(This)      \
    (This)->lpVtbl -> AddRef(This)

#define IRemoteThread_Release(This)     \
    (This)->lpVtbl -> Release(This)


#define IRemoteThread_GetName(This,ppszName)    \
    (This)->lpVtbl -> GetName(This,ppszName)

#define IRemoteThread_GetCurrentFrame(This,ppCurrentFrame)      \
    (This)->lpVtbl -> GetCurrentFrame(This,ppCurrentFrame)

#define IRemoteThread_GetThreadGroup(This,ppThreadGroup)        \
    (This)->lpVtbl -> GetThreadGroup(This,ppThreadGroup)

#define IRemoteThread_Go(This)  \
    (This)->lpVtbl -> Go(This)

#define IRemoteThread_Step(This)        \
    (This)->lpVtbl -> Step(This)

#define IRemoteThread_StepIn(This)      \
    (This)->lpVtbl -> StepIn(This)

#define IRemoteThread_StepOut(This)     \
    (This)->lpVtbl -> StepOut(This)

#define IRemoteThread_RangeStep(This,offStart,offEnd)   \
    (This)->lpVtbl -> RangeStep(This,offStart,offEnd)

#define IRemoteThread_RangeStepIn(This,offStart,offEnd) \
    (This)->lpVtbl -> RangeStepIn(This,offStart,offEnd)

#define IRemoteThread_Destroy(This)     \
    (This)->lpVtbl -> Destroy(This)

#define IRemoteThread_Suspend(This)     \
    (This)->lpVtbl -> Suspend(This)

#define IRemoteThread_Resume(This)      \
    (This)->lpVtbl -> Resume(This)

#define IRemoteThread_GetSuspendCount(This,pcSuspend)   \
    (This)->lpVtbl -> GetSuspendCount(This,pcSuspend)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IRemoteThread_GetName_Proxy(
    IRemoteThread __RPC_FAR * This,
    /* [out] */ LPOLESTR __RPC_FAR *ppszName);


void __RPC_STUB IRemoteThread_GetName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteThread_GetCurrentFrame_Proxy(
    IRemoteThread __RPC_FAR * This,
    /* [out] */ IRemoteStackFrame __RPC_FAR *__RPC_FAR *ppCurrentFrame);


void __RPC_STUB IRemoteThread_GetCurrentFrame_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteThread_GetThreadGroup_Proxy(
    IRemoteThread __RPC_FAR * This,
    /* [out] */ IRemoteThreadGroup __RPC_FAR *__RPC_FAR *ppThreadGroup);


void __RPC_STUB IRemoteThread_GetThreadGroup_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteThread_Go_Proxy(
    IRemoteThread __RPC_FAR * This);


void __RPC_STUB IRemoteThread_Go_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteThread_Step_Proxy(
    IRemoteThread __RPC_FAR * This);


void __RPC_STUB IRemoteThread_Step_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteThread_StepIn_Proxy(
    IRemoteThread __RPC_FAR * This);


void __RPC_STUB IRemoteThread_StepIn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteThread_StepOut_Proxy(
    IRemoteThread __RPC_FAR * This);


void __RPC_STUB IRemoteThread_StepOut_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteThread_RangeStep_Proxy(
    IRemoteThread __RPC_FAR * This,
    /* [in] */ ULONG offStart,
    /* [in] */ ULONG offEnd);


void __RPC_STUB IRemoteThread_RangeStep_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteThread_RangeStepIn_Proxy(
    IRemoteThread __RPC_FAR * This,
    /* [in] */ ULONG offStart,
    /* [in] */ ULONG offEnd);


void __RPC_STUB IRemoteThread_RangeStepIn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteThread_Destroy_Proxy(
    IRemoteThread __RPC_FAR * This);


void __RPC_STUB IRemoteThread_Destroy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteThread_Suspend_Proxy(
    IRemoteThread __RPC_FAR * This);


void __RPC_STUB IRemoteThread_Suspend_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteThread_Resume_Proxy(
    IRemoteThread __RPC_FAR * This);


void __RPC_STUB IRemoteThread_Resume_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteThread_GetSuspendCount_Proxy(
    IRemoteThread __RPC_FAR * This,
    /* [out] */ ULONG __RPC_FAR *pcSuspend);


void __RPC_STUB IRemoteThread_GetSuspendCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IRemoteThread_INTERFACE_DEFINED__ */


#ifndef __IEnumRemoteThread_INTERFACE_DEFINED__
#define __IEnumRemoteThread_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IEnumRemoteThread
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IEnumRemoteThread __RPC_FAR *LPENUMREMOTETHREAD;


EXTERN_C const IID IID_IEnumRemoteThread;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IEnumRemoteThread : public IUnknown
    {
    public:
        virtual HRESULT __stdcall Next(
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IRemoteThread __RPC_FAR *__RPC_FAR rgelt[  ],
            /* [out] */ ULONG __RPC_FAR *pceltFetched) = 0;

        virtual HRESULT __stdcall Skip(
            /* [in] */ ULONG celt) = 0;

        virtual HRESULT __stdcall Reset( void) = 0;

        virtual HRESULT __stdcall Clone(
            /* [out] */ IEnumRemoteThread __RPC_FAR *__RPC_FAR *ppEnum) = 0;

    };

#else   /* C style interface */

    typedef struct IEnumRemoteThreadVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IEnumRemoteThread __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IEnumRemoteThread __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IEnumRemoteThread __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Next )(
            IEnumRemoteThread __RPC_FAR * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IRemoteThread __RPC_FAR *__RPC_FAR rgelt[  ],
            /* [out] */ ULONG __RPC_FAR *pceltFetched);

        HRESULT ( __stdcall __RPC_FAR *Skip )(
            IEnumRemoteThread __RPC_FAR * This,
            /* [in] */ ULONG celt);

        HRESULT ( __stdcall __RPC_FAR *Reset )(
            IEnumRemoteThread __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Clone )(
            IEnumRemoteThread __RPC_FAR * This,
            /* [out] */ IEnumRemoteThread __RPC_FAR *__RPC_FAR *ppEnum);

    } IEnumRemoteThreadVtbl;

    interface IEnumRemoteThread
    {
        CONST_VTBL struct IEnumRemoteThreadVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IEnumRemoteThread_QueryInterface(This,riid,ppvObject)   \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumRemoteThread_AddRef(This)  \
    (This)->lpVtbl -> AddRef(This)

#define IEnumRemoteThread_Release(This) \
    (This)->lpVtbl -> Release(This)


#define IEnumRemoteThread_Next(This,celt,rgelt,pceltFetched)    \
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumRemoteThread_Skip(This,celt)       \
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumRemoteThread_Reset(This)   \
    (This)->lpVtbl -> Reset(This)

#define IEnumRemoteThread_Clone(This,ppEnum)    \
    (This)->lpVtbl -> Clone(This,ppEnum)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IEnumRemoteThread_Next_Proxy(
    IEnumRemoteThread __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ IRemoteThread __RPC_FAR *__RPC_FAR rgelt[  ],
    /* [out] */ ULONG __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumRemoteThread_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumRemoteThread_Skip_Proxy(
    IEnumRemoteThread __RPC_FAR * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumRemoteThread_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumRemoteThread_Reset_Proxy(
    IEnumRemoteThread __RPC_FAR * This);


void __RPC_STUB IEnumRemoteThread_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumRemoteThread_Clone_Proxy(
    IEnumRemoteThread __RPC_FAR * This,
    /* [out] */ IEnumRemoteThread __RPC_FAR *__RPC_FAR *ppEnum);


void __RPC_STUB IEnumRemoteThread_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IEnumRemoteThread_INTERFACE_DEFINED__ */


#ifndef __IRemoteProcessCallback_INTERFACE_DEFINED__
#define __IRemoteProcessCallback_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IRemoteProcessCallback
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IRemoteProcessCallback __RPC_FAR *LPREMOTEPROCESSCALLBACK;

                        /* size is 2 */

enum __MIDL_IRemoteProcessCallback_0001
    {   EXCEPTION_KIND_FIRST_CHANCE     = 0x1,
        EXCEPTION_KIND_LAST_CHANCE      = 0x2
    };
                        /* size is 4 */
typedef ULONG EXCEPTIONKIND;


EXTERN_C const IID IID_IRemoteProcessCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IRemoteProcessCallback : public IUnknown
    {
    public:
        virtual HRESULT __stdcall DebugStringEvent(
            /* [unique][in] */ IRemoteThread __RPC_FAR *pThread,
            /* [in] */ LPCOLESTR pszDebugString) = 0;

        virtual HRESULT __stdcall CodeBreakpointEvent(
            /* [unique][in] */ IRemoteThread __RPC_FAR *pThread) = 0;

        virtual HRESULT __stdcall DataBreakpointEvent(
            /* [unique][in] */ IRemoteThread __RPC_FAR *pThread,
            /* [unique][in] */ IRemoteObject __RPC_FAR *pObject) = 0;

        virtual HRESULT __stdcall ExceptionEvent(
            /* [unique][in] */ IRemoteThread __RPC_FAR *pThread,
            /* [unique][in] */ IRemoteClassField __RPC_FAR *pExceptionClass,
            /* [in] */ EXCEPTIONKIND exceptionKind) = 0;

        virtual HRESULT __stdcall StepEvent(
            /* [unique][in] */ IRemoteThread __RPC_FAR *pThread) = 0;

        virtual HRESULT __stdcall CanStopEvent(
            /* [unique][in] */ IRemoteThread __RPC_FAR *pThread) = 0;

        virtual HRESULT __stdcall BreakEvent(
            /* [unique][in] */ IRemoteThread __RPC_FAR *pThread) = 0;

        virtual HRESULT __stdcall ThreadCreateEvent(
            /* [unique][in] */ IRemoteThread __RPC_FAR *pThread) = 0;

        virtual HRESULT __stdcall ThreadDestroyEvent(
            /* [unique][in] */ IRemoteThread __RPC_FAR *pThread) = 0;

        virtual HRESULT __stdcall ThreadGroupCreateEvent(
            /* [unique][in] */ IRemoteThread __RPC_FAR *pThread,
            /* [unique][in] */ IRemoteThreadGroup __RPC_FAR *pThreadGroup) = 0;

        virtual HRESULT __stdcall ThreadGroupDestroyEvent(
            /* [unique][in] */ IRemoteThread __RPC_FAR *pThread,
            /* [unique][in] */ IRemoteThreadGroup __RPC_FAR *pThreadGroup) = 0;

        virtual HRESULT __stdcall ClassLoadEvent(
            /* [unique][in] */ IRemoteThread __RPC_FAR *pThread,
            /* [unique][in] */ IRemoteClassField __RPC_FAR *pClassType) = 0;

        virtual HRESULT __stdcall ClassUnloadEvent(
            /* [unique][in] */ IRemoteThread __RPC_FAR *pThread,
            /* [unique][in] */ IRemoteClassField __RPC_FAR *pClassType) = 0;

        virtual HRESULT __stdcall ProcessDestroyEvent(
            /* [unique][in] */ IRemoteThread __RPC_FAR *pThread) = 0;

        virtual HRESULT __stdcall TraceEvent(
            /* [unique][in] */ IRemoteThread __RPC_FAR *pThread) = 0;

        virtual HRESULT __stdcall LoadCompleteEvent(
            /* [unique][in] */ IRemoteThread __RPC_FAR *pThread) = 0;

    };

#else   /* C style interface */

    typedef struct IRemoteProcessCallbackVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IRemoteProcessCallback __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IRemoteProcessCallback __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IRemoteProcessCallback __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *DebugStringEvent )(
            IRemoteProcessCallback __RPC_FAR * This,
            /* [unique][in] */ IRemoteThread __RPC_FAR *pThread,
            /* [in] */ LPCOLESTR pszDebugString);

        HRESULT ( __stdcall __RPC_FAR *CodeBreakpointEvent )(
            IRemoteProcessCallback __RPC_FAR * This,
            /* [unique][in] */ IRemoteThread __RPC_FAR *pThread);

        HRESULT ( __stdcall __RPC_FAR *DataBreakpointEvent )(
            IRemoteProcessCallback __RPC_FAR * This,
            /* [unique][in] */ IRemoteThread __RPC_FAR *pThread,
            /* [unique][in] */ IRemoteObject __RPC_FAR *pObject);

        HRESULT ( __stdcall __RPC_FAR *ExceptionEvent )(
            IRemoteProcessCallback __RPC_FAR * This,
            /* [unique][in] */ IRemoteThread __RPC_FAR *pThread,
            /* [unique][in] */ IRemoteClassField __RPC_FAR *pExceptionClass,
            /* [in] */ EXCEPTIONKIND exceptionKind);

        HRESULT ( __stdcall __RPC_FAR *StepEvent )(
            IRemoteProcessCallback __RPC_FAR * This,
            /* [unique][in] */ IRemoteThread __RPC_FAR *pThread);

        HRESULT ( __stdcall __RPC_FAR *CanStopEvent )(
            IRemoteProcessCallback __RPC_FAR * This,
            /* [unique][in] */ IRemoteThread __RPC_FAR *pThread);

        HRESULT ( __stdcall __RPC_FAR *BreakEvent )(
            IRemoteProcessCallback __RPC_FAR * This,
            /* [unique][in] */ IRemoteThread __RPC_FAR *pThread);

        HRESULT ( __stdcall __RPC_FAR *ThreadCreateEvent )(
            IRemoteProcessCallback __RPC_FAR * This,
            /* [unique][in] */ IRemoteThread __RPC_FAR *pThread);

        HRESULT ( __stdcall __RPC_FAR *ThreadDestroyEvent )(
            IRemoteProcessCallback __RPC_FAR * This,
            /* [unique][in] */ IRemoteThread __RPC_FAR *pThread);

        HRESULT ( __stdcall __RPC_FAR *ThreadGroupCreateEvent )(
            IRemoteProcessCallback __RPC_FAR * This,
            /* [unique][in] */ IRemoteThread __RPC_FAR *pThread,
            /* [unique][in] */ IRemoteThreadGroup __RPC_FAR *pThreadGroup);

        HRESULT ( __stdcall __RPC_FAR *ThreadGroupDestroyEvent )(
            IRemoteProcessCallback __RPC_FAR * This,
            /* [unique][in] */ IRemoteThread __RPC_FAR *pThread,
            /* [unique][in] */ IRemoteThreadGroup __RPC_FAR *pThreadGroup);

        HRESULT ( __stdcall __RPC_FAR *ClassLoadEvent )(
            IRemoteProcessCallback __RPC_FAR * This,
            /* [unique][in] */ IRemoteThread __RPC_FAR *pThread,
            /* [unique][in] */ IRemoteClassField __RPC_FAR *pClassType);

        HRESULT ( __stdcall __RPC_FAR *ClassUnloadEvent )(
            IRemoteProcessCallback __RPC_FAR * This,
            /* [unique][in] */ IRemoteThread __RPC_FAR *pThread,
            /* [unique][in] */ IRemoteClassField __RPC_FAR *pClassType);

        HRESULT ( __stdcall __RPC_FAR *ProcessDestroyEvent )(
            IRemoteProcessCallback __RPC_FAR * This,
            /* [unique][in] */ IRemoteThread __RPC_FAR *pThread);

        HRESULT ( __stdcall __RPC_FAR *TraceEvent )(
            IRemoteProcessCallback __RPC_FAR * This,
            /* [unique][in] */ IRemoteThread __RPC_FAR *pThread);

        HRESULT ( __stdcall __RPC_FAR *LoadCompleteEvent )(
            IRemoteProcessCallback __RPC_FAR * This,
            /* [unique][in] */ IRemoteThread __RPC_FAR *pThread);

    } IRemoteProcessCallbackVtbl;

    interface IRemoteProcessCallback
    {
        CONST_VTBL struct IRemoteProcessCallbackVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IRemoteProcessCallback_QueryInterface(This,riid,ppvObject)      \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRemoteProcessCallback_AddRef(This)     \
    (This)->lpVtbl -> AddRef(This)

#define IRemoteProcessCallback_Release(This)    \
    (This)->lpVtbl -> Release(This)


#define IRemoteProcessCallback_DebugStringEvent(This,pThread,pszDebugString)    \
    (This)->lpVtbl -> DebugStringEvent(This,pThread,pszDebugString)

#define IRemoteProcessCallback_CodeBreakpointEvent(This,pThread)        \
    (This)->lpVtbl -> CodeBreakpointEvent(This,pThread)

#define IRemoteProcessCallback_DataBreakpointEvent(This,pThread,pObject)        \
    (This)->lpVtbl -> DataBreakpointEvent(This,pThread,pObject)

#define IRemoteProcessCallback_ExceptionEvent(This,pThread,pExceptionClass,exceptionKind)       \
    (This)->lpVtbl -> ExceptionEvent(This,pThread,pExceptionClass,exceptionKind)

#define IRemoteProcessCallback_StepEvent(This,pThread)  \
    (This)->lpVtbl -> StepEvent(This,pThread)

#define IRemoteProcessCallback_CanStopEvent(This,pThread)       \
    (This)->lpVtbl -> CanStopEvent(This,pThread)

#define IRemoteProcessCallback_BreakEvent(This,pThread) \
    (This)->lpVtbl -> BreakEvent(This,pThread)

#define IRemoteProcessCallback_ThreadCreateEvent(This,pThread)  \
    (This)->lpVtbl -> ThreadCreateEvent(This,pThread)

#define IRemoteProcessCallback_ThreadDestroyEvent(This,pThread) \
    (This)->lpVtbl -> ThreadDestroyEvent(This,pThread)

#define IRemoteProcessCallback_ThreadGroupCreateEvent(This,pThread,pThreadGroup)        \
    (This)->lpVtbl -> ThreadGroupCreateEvent(This,pThread,pThreadGroup)

#define IRemoteProcessCallback_ThreadGroupDestroyEvent(This,pThread,pThreadGroup)       \
    (This)->lpVtbl -> ThreadGroupDestroyEvent(This,pThread,pThreadGroup)

#define IRemoteProcessCallback_ClassLoadEvent(This,pThread,pClassType)  \
    (This)->lpVtbl -> ClassLoadEvent(This,pThread,pClassType)

#define IRemoteProcessCallback_ClassUnloadEvent(This,pThread,pClassType)        \
    (This)->lpVtbl -> ClassUnloadEvent(This,pThread,pClassType)

#define IRemoteProcessCallback_ProcessDestroyEvent(This,pThread)        \
    (This)->lpVtbl -> ProcessDestroyEvent(This,pThread)

#define IRemoteProcessCallback_TraceEvent(This,pThread) \
    (This)->lpVtbl -> TraceEvent(This,pThread)

#define IRemoteProcessCallback_LoadCompleteEvent(This,pThread)  \
    (This)->lpVtbl -> LoadCompleteEvent(This,pThread)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IRemoteProcessCallback_DebugStringEvent_Proxy(
    IRemoteProcessCallback __RPC_FAR * This,
    /* [unique][in] */ IRemoteThread __RPC_FAR *pThread,
    /* [in] */ LPCOLESTR pszDebugString);


void __RPC_STUB IRemoteProcessCallback_DebugStringEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteProcessCallback_CodeBreakpointEvent_Proxy(
    IRemoteProcessCallback __RPC_FAR * This,
    /* [unique][in] */ IRemoteThread __RPC_FAR *pThread);


void __RPC_STUB IRemoteProcessCallback_CodeBreakpointEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteProcessCallback_DataBreakpointEvent_Proxy(
    IRemoteProcessCallback __RPC_FAR * This,
    /* [unique][in] */ IRemoteThread __RPC_FAR *pThread,
    /* [unique][in] */ IRemoteObject __RPC_FAR *pObject);


void __RPC_STUB IRemoteProcessCallback_DataBreakpointEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteProcessCallback_ExceptionEvent_Proxy(
    IRemoteProcessCallback __RPC_FAR * This,
    /* [unique][in] */ IRemoteThread __RPC_FAR *pThread,
    /* [unique][in] */ IRemoteClassField __RPC_FAR *pExceptionClass,
    /* [in] */ EXCEPTIONKIND exceptionKind);


void __RPC_STUB IRemoteProcessCallback_ExceptionEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteProcessCallback_StepEvent_Proxy(
    IRemoteProcessCallback __RPC_FAR * This,
    /* [unique][in] */ IRemoteThread __RPC_FAR *pThread);


void __RPC_STUB IRemoteProcessCallback_StepEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteProcessCallback_CanStopEvent_Proxy(
    IRemoteProcessCallback __RPC_FAR * This,
    /* [unique][in] */ IRemoteThread __RPC_FAR *pThread);


void __RPC_STUB IRemoteProcessCallback_CanStopEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteProcessCallback_BreakEvent_Proxy(
    IRemoteProcessCallback __RPC_FAR * This,
    /* [unique][in] */ IRemoteThread __RPC_FAR *pThread);


void __RPC_STUB IRemoteProcessCallback_BreakEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteProcessCallback_ThreadCreateEvent_Proxy(
    IRemoteProcessCallback __RPC_FAR * This,
    /* [unique][in] */ IRemoteThread __RPC_FAR *pThread);


void __RPC_STUB IRemoteProcessCallback_ThreadCreateEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteProcessCallback_ThreadDestroyEvent_Proxy(
    IRemoteProcessCallback __RPC_FAR * This,
    /* [unique][in] */ IRemoteThread __RPC_FAR *pThread);


void __RPC_STUB IRemoteProcessCallback_ThreadDestroyEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteProcessCallback_ThreadGroupCreateEvent_Proxy(
    IRemoteProcessCallback __RPC_FAR * This,
    /* [unique][in] */ IRemoteThread __RPC_FAR *pThread,
    /* [unique][in] */ IRemoteThreadGroup __RPC_FAR *pThreadGroup);


void __RPC_STUB IRemoteProcessCallback_ThreadGroupCreateEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteProcessCallback_ThreadGroupDestroyEvent_Proxy(
    IRemoteProcessCallback __RPC_FAR * This,
    /* [unique][in] */ IRemoteThread __RPC_FAR *pThread,
    /* [unique][in] */ IRemoteThreadGroup __RPC_FAR *pThreadGroup);


void __RPC_STUB IRemoteProcessCallback_ThreadGroupDestroyEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteProcessCallback_ClassLoadEvent_Proxy(
    IRemoteProcessCallback __RPC_FAR * This,
    /* [unique][in] */ IRemoteThread __RPC_FAR *pThread,
    /* [unique][in] */ IRemoteClassField __RPC_FAR *pClassType);


void __RPC_STUB IRemoteProcessCallback_ClassLoadEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteProcessCallback_ClassUnloadEvent_Proxy(
    IRemoteProcessCallback __RPC_FAR * This,
    /* [unique][in] */ IRemoteThread __RPC_FAR *pThread,
    /* [unique][in] */ IRemoteClassField __RPC_FAR *pClassType);


void __RPC_STUB IRemoteProcessCallback_ClassUnloadEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteProcessCallback_ProcessDestroyEvent_Proxy(
    IRemoteProcessCallback __RPC_FAR * This,
    /* [unique][in] */ IRemoteThread __RPC_FAR *pThread);


void __RPC_STUB IRemoteProcessCallback_ProcessDestroyEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteProcessCallback_TraceEvent_Proxy(
    IRemoteProcessCallback __RPC_FAR * This,
    /* [unique][in] */ IRemoteThread __RPC_FAR *pThread);


void __RPC_STUB IRemoteProcessCallback_TraceEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteProcessCallback_LoadCompleteEvent_Proxy(
    IRemoteProcessCallback __RPC_FAR * This,
    /* [unique][in] */ IRemoteThread __RPC_FAR *pThread);


void __RPC_STUB IRemoteProcessCallback_LoadCompleteEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IRemoteProcessCallback_INTERFACE_DEFINED__ */


#ifndef __IRemoteProcess_INTERFACE_DEFINED__
#define __IRemoteProcess_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IRemoteProcess
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IRemoteProcess __RPC_FAR *LPREMOTEPROCESS;


EXTERN_C const IID IID_IRemoteProcess;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IRemoteProcess : public IUnknown
    {
    public:
        virtual HRESULT __stdcall RegisterCallback(
            /* [unique][in] */ IRemoteProcessCallback __RPC_FAR *pCallback) = 0;

        virtual HRESULT __stdcall Detach( void) = 0;

        virtual HRESULT __stdcall Break( void) = 0;

        virtual HRESULT __stdcall GetGlobalContainerObject(
            /* [out] */ IRemoteContainerObject __RPC_FAR *__RPC_FAR *ppGlobalContainerObject) = 0;

        virtual HRESULT __stdcall FindClass(
            /* [in] */ LPCOLESTR pszClassName,
            /* [out] */ IRemoteClassField __RPC_FAR *__RPC_FAR *ppClassType) = 0;

        virtual HRESULT __stdcall TraceMethods(
            /* [in] */ BOOL bTraceOn) = 0;

        virtual HRESULT __stdcall GetThreadGroups(
            /* [out] */ IEnumRemoteThreadGroup __RPC_FAR *__RPC_FAR *ppEnum) = 0;

    };

#else   /* C style interface */

    typedef struct IRemoteProcessVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IRemoteProcess __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IRemoteProcess __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IRemoteProcess __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *RegisterCallback )(
            IRemoteProcess __RPC_FAR * This,
            /* [unique][in] */ IRemoteProcessCallback __RPC_FAR *pCallback);

        HRESULT ( __stdcall __RPC_FAR *Detach )(
            IRemoteProcess __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Break )(
            IRemoteProcess __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetGlobalContainerObject )(
            IRemoteProcess __RPC_FAR * This,
            /* [out] */ IRemoteContainerObject __RPC_FAR *__RPC_FAR *ppGlobalContainerObject);

        HRESULT ( __stdcall __RPC_FAR *FindClass )(
            IRemoteProcess __RPC_FAR * This,
            /* [in] */ LPCOLESTR pszClassName,
            /* [out] */ IRemoteClassField __RPC_FAR *__RPC_FAR *ppClassType);

        HRESULT ( __stdcall __RPC_FAR *TraceMethods )(
            IRemoteProcess __RPC_FAR * This,
            /* [in] */ BOOL bTraceOn);

        HRESULT ( __stdcall __RPC_FAR *GetThreadGroups )(
            IRemoteProcess __RPC_FAR * This,
            /* [out] */ IEnumRemoteThreadGroup __RPC_FAR *__RPC_FAR *ppEnum);

    } IRemoteProcessVtbl;

    interface IRemoteProcess
    {
        CONST_VTBL struct IRemoteProcessVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IRemoteProcess_QueryInterface(This,riid,ppvObject)      \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRemoteProcess_AddRef(This)     \
    (This)->lpVtbl -> AddRef(This)

#define IRemoteProcess_Release(This)    \
    (This)->lpVtbl -> Release(This)


#define IRemoteProcess_RegisterCallback(This,pCallback) \
    (This)->lpVtbl -> RegisterCallback(This,pCallback)

#define IRemoteProcess_Detach(This)     \
    (This)->lpVtbl -> Detach(This)

#define IRemoteProcess_Break(This)      \
    (This)->lpVtbl -> Break(This)

#define IRemoteProcess_GetGlobalContainerObject(This,ppGlobalContainerObject)   \
    (This)->lpVtbl -> GetGlobalContainerObject(This,ppGlobalContainerObject)

#define IRemoteProcess_FindClass(This,pszClassName,ppClassType) \
    (This)->lpVtbl -> FindClass(This,pszClassName,ppClassType)

#define IRemoteProcess_TraceMethods(This,bTraceOn)      \
    (This)->lpVtbl -> TraceMethods(This,bTraceOn)

#define IRemoteProcess_GetThreadGroups(This,ppEnum)     \
    (This)->lpVtbl -> GetThreadGroups(This,ppEnum)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IRemoteProcess_RegisterCallback_Proxy(
    IRemoteProcess __RPC_FAR * This,
    /* [unique][in] */ IRemoteProcessCallback __RPC_FAR *pCallback);


void __RPC_STUB IRemoteProcess_RegisterCallback_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteProcess_Detach_Proxy(
    IRemoteProcess __RPC_FAR * This);


void __RPC_STUB IRemoteProcess_Detach_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteProcess_Break_Proxy(
    IRemoteProcess __RPC_FAR * This);


void __RPC_STUB IRemoteProcess_Break_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteProcess_GetGlobalContainerObject_Proxy(
    IRemoteProcess __RPC_FAR * This,
    /* [out] */ IRemoteContainerObject __RPC_FAR *__RPC_FAR *ppGlobalContainerObject);


void __RPC_STUB IRemoteProcess_GetGlobalContainerObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteProcess_FindClass_Proxy(
    IRemoteProcess __RPC_FAR * This,
    /* [in] */ LPCOLESTR pszClassName,
    /* [out] */ IRemoteClassField __RPC_FAR *__RPC_FAR *ppClassType);


void __RPC_STUB IRemoteProcess_FindClass_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteProcess_TraceMethods_Proxy(
    IRemoteProcess __RPC_FAR * This,
    /* [in] */ BOOL bTraceOn);


void __RPC_STUB IRemoteProcess_TraceMethods_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteProcess_GetThreadGroups_Proxy(
    IRemoteProcess __RPC_FAR * This,
    /* [out] */ IEnumRemoteThreadGroup __RPC_FAR *__RPC_FAR *ppEnum);


void __RPC_STUB IRemoteProcess_GetThreadGroups_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IRemoteProcess_INTERFACE_DEFINED__ */


#ifndef __IEnumRemoteProcess_INTERFACE_DEFINED__
#define __IEnumRemoteProcess_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IEnumRemoteProcess
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IEnumRemoteProcess __RPC_FAR *LPENUMREMOTEPROCESS;


EXTERN_C const IID IID_IEnumRemoteProcess;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IEnumRemoteProcess : public IUnknown
    {
    public:
        virtual HRESULT __stdcall Next(
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IRemoteProcess __RPC_FAR *__RPC_FAR rgelt[  ],
            /* [out] */ ULONG __RPC_FAR *pceltFetched) = 0;

        virtual HRESULT __stdcall Skip(
            /* [in] */ ULONG celt) = 0;

        virtual HRESULT __stdcall Reset( void) = 0;

        virtual HRESULT __stdcall Clone(
            /* [out] */ IEnumRemoteProcess __RPC_FAR *__RPC_FAR *ppEnum) = 0;

    };

#else   /* C style interface */

    typedef struct IEnumRemoteProcessVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IEnumRemoteProcess __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IEnumRemoteProcess __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IEnumRemoteProcess __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Next )(
            IEnumRemoteProcess __RPC_FAR * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IRemoteProcess __RPC_FAR *__RPC_FAR rgelt[  ],
            /* [out] */ ULONG __RPC_FAR *pceltFetched);

        HRESULT ( __stdcall __RPC_FAR *Skip )(
            IEnumRemoteProcess __RPC_FAR * This,
            /* [in] */ ULONG celt);

        HRESULT ( __stdcall __RPC_FAR *Reset )(
            IEnumRemoteProcess __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *Clone )(
            IEnumRemoteProcess __RPC_FAR * This,
            /* [out] */ IEnumRemoteProcess __RPC_FAR *__RPC_FAR *ppEnum);

    } IEnumRemoteProcessVtbl;

    interface IEnumRemoteProcess
    {
        CONST_VTBL struct IEnumRemoteProcessVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IEnumRemoteProcess_QueryInterface(This,riid,ppvObject)  \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumRemoteProcess_AddRef(This) \
    (This)->lpVtbl -> AddRef(This)

#define IEnumRemoteProcess_Release(This)        \
    (This)->lpVtbl -> Release(This)


#define IEnumRemoteProcess_Next(This,celt,rgelt,pceltFetched)   \
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumRemoteProcess_Skip(This,celt)      \
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumRemoteProcess_Reset(This)  \
    (This)->lpVtbl -> Reset(This)

#define IEnumRemoteProcess_Clone(This,ppEnum)   \
    (This)->lpVtbl -> Clone(This,ppEnum)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IEnumRemoteProcess_Next_Proxy(
    IEnumRemoteProcess __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ IRemoteProcess __RPC_FAR *__RPC_FAR rgelt[  ],
    /* [out] */ ULONG __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumRemoteProcess_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumRemoteProcess_Skip_Proxy(
    IEnumRemoteProcess __RPC_FAR * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumRemoteProcess_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumRemoteProcess_Reset_Proxy(
    IEnumRemoteProcess __RPC_FAR * This);


void __RPC_STUB IEnumRemoteProcess_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IEnumRemoteProcess_Clone_Proxy(
    IEnumRemoteProcess __RPC_FAR * This,
    /* [out] */ IEnumRemoteProcess __RPC_FAR *__RPC_FAR *ppEnum);


void __RPC_STUB IEnumRemoteProcess_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IEnumRemoteProcess_INTERFACE_DEFINED__ */


#ifndef __IRemoteDebugManagerCallback_INTERFACE_DEFINED__
#define __IRemoteDebugManagerCallback_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IRemoteDebugManagerCallback
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IRemoteDebugManagerCallback __RPC_FAR *LPREMOTEDEBUGMANAGERCALLBACK;


EXTERN_C const IID IID_IRemoteDebugManagerCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IRemoteDebugManagerCallback : public IUnknown
    {
    public:
        virtual HRESULT __stdcall ProcessCreateEvent(
            /* [unique][in] */ IRemoteProcess __RPC_FAR *pProcessNew,
            /* [unique][in] */ IRemoteProcess __RPC_FAR *pProcessParent) = 0;

    };

#else   /* C style interface */

    typedef struct IRemoteDebugManagerCallbackVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IRemoteDebugManagerCallback __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IRemoteDebugManagerCallback __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IRemoteDebugManagerCallback __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *ProcessCreateEvent )(
            IRemoteDebugManagerCallback __RPC_FAR * This,
            /* [unique][in] */ IRemoteProcess __RPC_FAR *pProcessNew,
            /* [unique][in] */ IRemoteProcess __RPC_FAR *pProcessParent);

    } IRemoteDebugManagerCallbackVtbl;

    interface IRemoteDebugManagerCallback
    {
        CONST_VTBL struct IRemoteDebugManagerCallbackVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IRemoteDebugManagerCallback_QueryInterface(This,riid,ppvObject) \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRemoteDebugManagerCallback_AddRef(This)        \
    (This)->lpVtbl -> AddRef(This)

#define IRemoteDebugManagerCallback_Release(This)       \
    (This)->lpVtbl -> Release(This)


#define IRemoteDebugManagerCallback_ProcessCreateEvent(This,pProcessNew,pProcessParent) \
    (This)->lpVtbl -> ProcessCreateEvent(This,pProcessNew,pProcessParent)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IRemoteDebugManagerCallback_ProcessCreateEvent_Proxy(
    IRemoteDebugManagerCallback __RPC_FAR * This,
    /* [unique][in] */ IRemoteProcess __RPC_FAR *pProcessNew,
    /* [unique][in] */ IRemoteProcess __RPC_FAR *pProcessParent);


void __RPC_STUB IRemoteDebugManagerCallback_ProcessCreateEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IRemoteDebugManagerCallback_INTERFACE_DEFINED__ */


#ifndef __IRemoteDebugManager_INTERFACE_DEFINED__
#define __IRemoteDebugManager_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IRemoteDebugManager
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */


                        /* size is 4 */
typedef IRemoteDebugManager __RPC_FAR *LPREMOTEDEBUGMANAGER;


EXTERN_C const IID IID_IRemoteDebugManager;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IRemoteDebugManager : public IUnknown
    {
    public:
        virtual HRESULT __stdcall RegisterCallback(
            /* [unique][in] */ IRemoteDebugManagerCallback __RPC_FAR *pCallback) = 0;

        virtual HRESULT __stdcall Detach( void) = 0;

        virtual HRESULT __stdcall GetActiveProcesses(
            /* [out] */ IEnumRemoteProcess __RPC_FAR *__RPC_FAR *ppEnum) = 0;

        virtual HRESULT __stdcall RequestCreateEvent(
            /* [in] */ LPCOLESTR pszProcessName,
            /* [in] */ DWORD dwParentProcessId) = 0;

    };

#else   /* C style interface */

    typedef struct IRemoteDebugManagerVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IRemoteDebugManager __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IRemoteDebugManager __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IRemoteDebugManager __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *RegisterCallback )(
            IRemoteDebugManager __RPC_FAR * This,
            /* [unique][in] */ IRemoteDebugManagerCallback __RPC_FAR *pCallback);

        HRESULT ( __stdcall __RPC_FAR *Detach )(
            IRemoteDebugManager __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *GetActiveProcesses )(
            IRemoteDebugManager __RPC_FAR * This,
            /* [out] */ IEnumRemoteProcess __RPC_FAR *__RPC_FAR *ppEnum);

        HRESULT ( __stdcall __RPC_FAR *RequestCreateEvent )(
            IRemoteDebugManager __RPC_FAR * This,
            /* [in] */ LPCOLESTR pszProcessName,
            /* [in] */ DWORD dwParentProcessId);

    } IRemoteDebugManagerVtbl;

    interface IRemoteDebugManager
    {
        CONST_VTBL struct IRemoteDebugManagerVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IRemoteDebugManager_QueryInterface(This,riid,ppvObject) \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRemoteDebugManager_AddRef(This)        \
    (This)->lpVtbl -> AddRef(This)

#define IRemoteDebugManager_Release(This)       \
    (This)->lpVtbl -> Release(This)


#define IRemoteDebugManager_RegisterCallback(This,pCallback)    \
    (This)->lpVtbl -> RegisterCallback(This,pCallback)

#define IRemoteDebugManager_Detach(This)        \
    (This)->lpVtbl -> Detach(This)

#define IRemoteDebugManager_GetActiveProcesses(This,ppEnum)     \
    (This)->lpVtbl -> GetActiveProcesses(This,ppEnum)

#define IRemoteDebugManager_RequestCreateEvent(This,pszProcessName,dwParentProcessId)   \
    (This)->lpVtbl -> RequestCreateEvent(This,pszProcessName,dwParentProcessId)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IRemoteDebugManager_RegisterCallback_Proxy(
    IRemoteDebugManager __RPC_FAR * This,
    /* [unique][in] */ IRemoteDebugManagerCallback __RPC_FAR *pCallback);


void __RPC_STUB IRemoteDebugManager_RegisterCallback_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteDebugManager_Detach_Proxy(
    IRemoteDebugManager __RPC_FAR * This);


void __RPC_STUB IRemoteDebugManager_Detach_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteDebugManager_GetActiveProcesses_Proxy(
    IRemoteDebugManager __RPC_FAR * This,
    /* [out] */ IEnumRemoteProcess __RPC_FAR *__RPC_FAR *ppEnum);


void __RPC_STUB IRemoteDebugManager_GetActiveProcesses_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IRemoteDebugManager_RequestCreateEvent_Proxy(
    IRemoteDebugManager __RPC_FAR * This,
    /* [in] */ LPCOLESTR pszProcessName,
    /* [in] */ DWORD dwParentProcessId);


void __RPC_STUB IRemoteDebugManager_RequestCreateEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IRemoteDebugManager_INTERFACE_DEFINED__ */


#ifndef __IJavaDebugManager_INTERFACE_DEFINED__
#define __IJavaDebugManager_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IJavaDebugManager
 * at Wed Aug 07 01:12:35 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [uuid][object] */



EXTERN_C const IID IID_IJavaDebugManager;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface IJavaDebugManager : public IUnknown
    {
    public:
        virtual HRESULT __stdcall RegisterRemoteDebugManager(
            /* [unique][in] */ IRemoteDebugManager __RPC_FAR *pirdm,
            /* [in] */ DWORD dwProcessID) = 0;

        virtual HRESULT __stdcall Detach( void) = 0;

    };

#else   /* C style interface */

    typedef struct IJavaDebugManagerVtbl
    {

        HRESULT ( __stdcall __RPC_FAR *QueryInterface )(
            IJavaDebugManager __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject);

        ULONG ( __stdcall __RPC_FAR *AddRef )(
            IJavaDebugManager __RPC_FAR * This);

        ULONG ( __stdcall __RPC_FAR *Release )(
            IJavaDebugManager __RPC_FAR * This);

        HRESULT ( __stdcall __RPC_FAR *RegisterRemoteDebugManager )(
            IJavaDebugManager __RPC_FAR * This,
            /* [unique][in] */ IRemoteDebugManager __RPC_FAR *pirdm,
            /* [in] */ DWORD dwProcessID);

        HRESULT ( __stdcall __RPC_FAR *Detach )(
            IJavaDebugManager __RPC_FAR * This);

    } IJavaDebugManagerVtbl;

    interface IJavaDebugManager
    {
        CONST_VTBL struct IJavaDebugManagerVtbl __RPC_FAR *lpVtbl;
    };



#ifdef COBJMACROS


#define IJavaDebugManager_QueryInterface(This,riid,ppvObject)   \
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IJavaDebugManager_AddRef(This)  \
    (This)->lpVtbl -> AddRef(This)

#define IJavaDebugManager_Release(This) \
    (This)->lpVtbl -> Release(This)


#define IJavaDebugManager_RegisterRemoteDebugManager(This,pirdm,dwProcessID)    \
    (This)->lpVtbl -> RegisterRemoteDebugManager(This,pirdm,dwProcessID)

#define IJavaDebugManager_Detach(This)  \
    (This)->lpVtbl -> Detach(This)

#endif /* COBJMACROS */


#endif  /* C style interface */



HRESULT __stdcall IJavaDebugManager_RegisterRemoteDebugManager_Proxy(
    IJavaDebugManager __RPC_FAR * This,
    /* [unique][in] */ IRemoteDebugManager __RPC_FAR *pirdm,
    /* [in] */ DWORD dwProcessID);


void __RPC_STUB IJavaDebugManager_RegisterRemoteDebugManager_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT __stdcall IJavaDebugManager_Detach_Proxy(
    IJavaDebugManager __RPC_FAR * This);


void __RPC_STUB IJavaDebugManager_Detach_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif  /* __IJavaDebugManager_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif

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


#ifdef _M_I86

#include "vi.h"
#include "ole2def.h"

typedef struct {
    IClassFactory       cf;
    ULONG               usage_count;
} class_factory;
typedef class_factory   *LPMYCLASSFACTORY;

#pragma disable_message( 202 )

static DWORD    cfID;

/*
 * doCFAddRef - add a reference to our class factory object
 */
static HRESULT doCFAddRef( LPMYCLASSFACTORY this )
{
    this->usage_count++;
    return( (HRESULT) this->usage_count );

} /* doCFAddRef */

/*
 * CFQueryInterface - the query interface method for the class factory
 */
STDMETHODIMP CFQueryInterface( LPMYCLASSFACTORY this, LPIID riid, LPLPVOID ppv )
{
    if( CMPIID( riid, IID_IUnknown ) || CMPIID( riid, IID_IClassFactory ) ) {
        doCFAddRef( this );
        *ppv = this;
        return( NOERROR );
    }
    return( ResultFromScode( E_NOINTERFACE ) );

} /* CFQueryInterface */

/*
 * CFAddRef - the add reference method for the class factory
 */
STDMETHODIMP CFAddRef( LPMYCLASSFACTORY this )
{
    return( doCFAddRef( this ) );

} /* CFAddRef */

/*
 * CFRelease - method to release a class factory object
 */
STDMETHODIMP CFRelease( LPMYCLASSFACTORY this )
{
    ULONG       rc;

    this->usage_count--;
    rc = this->usage_count;
    if( this->usage_count == 0 ) {
        FreeProcInstance( (LPVOID) this->cf.lpVtbl->QueryInterface );
        FreeProcInstance( (LPVOID) this->cf.lpVtbl->AddRef );
        FreeProcInstance( (LPVOID) this->cf.lpVtbl->Release );
        FreeProcInstance( (LPVOID) this->cf.lpVtbl->CreateInstance );
        FreeProcInstance( (LPVOID) this->cf.lpVtbl->LockServer );
        MemFree( this->cf.lpVtbl );
        MemFree( this );
    }
    return( (HRESULT) rc );

} /* CFRelease */

/*
 * CFCreateInstance - create an instance of our class factory
 */
STDMETHODIMP CFCreateInstance( LPCLASSFACTORY this, LPVOID pUnkOuter,
                               LPIID riid, LPLPVOID ppv )
{
    HRESULT     hresult;
    object      *pobj;

//    pobj = CreateObject( InstanceHandle );
    pobj = NULL;
    if( pobj == NULL ) {
        *ppv = NULL;
        return( ResultFromScode( E_OUTOFMEMORY ) );
    }
    hresult = pobj->lpVtbl->id.QueryInterface( (LPVOID) pobj, riid, ppv );
    pobj->lpVtbl->id.Release( (LPVOID) pobj );
    return( hresult );

} /* CFCreateInstance */

/*
 * CFLockServer - lock access to the class factory object
 */
STDMETHODIMP CFLockServer( LPCLASSFACTORY this, BOOL lock )
{
    return( NOERROR );

} /* CFLockServer */

/*
 * createClassFactory - create a class factory object
 */
static LPCLASSFACTORY createClassFactory( HANDLE inst )
{
    class_factory       *pcf;

    pcf = MemAlloc( sizeof( class_factory ) );
    pcf->usage_count = 1;
    pcf->cf.lpVtbl = MemAlloc( sizeof( struct IClassFactoryVtbl ) );
    pcf->cf.lpVtbl->QueryInterface = (LPVOID)MakeProcInstance( (LPVOID)CFQueryInterface, inst );
    pcf->cf.lpVtbl->AddRef =         (LPVOID)MakeProcInstance( (LPVOID)CFAddRef, inst );
    pcf->cf.lpVtbl->Release =        (LPVOID)MakeProcInstance( (LPVOID)CFRelease, inst );
    pcf->cf.lpVtbl->CreateInstance = (LPVOID)MakeProcInstance( (LPVOID)CFCreateInstance, inst );
    pcf->cf.lpVtbl->LockServer =     (LPVOID)MakeProcInstance( (LPVOID)CFLockServer, inst );
    return( (LPCLASSFACTORY) pcf );

} /* createClassFactory */

/*
 * OLE2ClassFactoryInit - initialize OLE 2.0 class factory
 */
bool OLE2ClassFactoryInit( void )
{
    HRESULT             hresult;
    LPCLASSFACTORY      pcf;

    pcf = createClassFactory( InstanceHandle );
    hresult = CoRegisterClassObject( (LPVOID) &CLSID_WATCOMEditor, (LPVOID) pcf,
                                     CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE, &cfID );
    pcf->lpVtbl->Release( pcf );
    if( hresult != NOERROR ) {
        return( false );
    }
    return( true );

} /* OLE2ClassFactoryInit */

/*
 * OLE2ClassFactoryFini - done with our class factory
 */
void OLE2ClassFactoryFini( void )
{
    if( cfID != 0 ) {
        CoRevokeClassObject( cfID );
        cfID = 0;
    }

} /* OLE2ClassFactoryFini */
#endif

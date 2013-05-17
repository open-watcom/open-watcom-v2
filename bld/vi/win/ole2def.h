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


#if !defined(OLE2DEF_INCLUDED)
#define OLE2DEF_INCLUDED

#ifdef _M_I86

#include <ole2.h>
#include <olenls.h>
#include <dispatch.h>
#include <compobj.h>
#ifdef DO_DEFINE_OLEGUID
#include <initguid.h>
#endif

DEFINE_OLEGUID( CLSID_WATCOMEditor, 0x00020666, 0, 0 );

typedef DISPID          *LPDISPID;
typedef DISPPARAMS      *LPDISPPARAMS;

#define CMPIID( a, b ) (!memcmp( &(a)->Data1, &(b).Data1, sizeof( GUID ) ))

enum {
    IMETH_QUERYINTERFACE = 0,
    IMETH_ADDREF,
    IMETH_RELEASE,
    IMETH_GETTYPEINFOCOUNT,
    IMETH_GETTYPEINFO,
    IMETH_GETIDSOFNAMES,
    IMETH_INVOKE,
    IMETH_PUTPROP1,
    IMETH_GETPROP1,
    IMETH_FUNC1,
    IMETH_FUNC2
};

enum {
    IDMEMBER_PUTPROP1 = 1,
    IDMEMBER_GETPROP1,
    IDMEMBER_FUNC1,
    IDMEMBER_FUNC2,
    IDMEMBER_PROP1 = IDMEMBER_PUTPROP1
};

typedef struct object FAR *LPOBJECT;

typedef struct objvtbl {
    IDispatchVtbl       id;
    WORD (CALLBACK *PutProp1)();
    WORD (CALLBACK *GetProp1)();
    void (CALLBACK *Func1)();
    WORD (CALLBACK *Func2)();
} objvtbl;

typedef objvtbl FAR *LPOBJVTBL;

typedef struct object {
    LPOBJVTBL   lpVtbl;
    ULONG       usage_count;
    LPTYPEINFO  ptinfo;
} object;

/* ole2.c */
bool OLE2Init( void );
void OLE2Fini( void );

/* ole2cf.c */
bool OLE2ClassFactoryInit( void );
void OLE2ClassFactoryFini( void );

#else

bool OLE2Init( void );
void OLE2Fini( void );

#endif

#endif

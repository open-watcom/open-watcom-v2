/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2009 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Declaration of CHandleMap.
*
****************************************************************************/


#ifndef __HANDMAP_H__
#define __HANDMAP_H__

class CHandleMap {
private:
    void            (PASCAL *m_pfnConstructObject)( CObject *pObject );
    void            (PASCAL *m_pfnDestructObject)( CObject *pObject );
    CMapPtrToPtr    m_permanentMap;
    CMapPtrToPtr    m_temporaryMap;
    CRuntimeClass   *m_pClass;
    size_t          m_nOffset;
    int             m_nHandles;

public:
    CHandleMap( CRuntimeClass *pClass, void (PASCAL *pfnConstructObject)( CObject *pObject ), void (PASCAL *pfnDestructObject)( CObject *pObject ), size_t nOffset, int nHandles = 1 );
    virtual ~CHandleMap();

    void    DeleteTemp();
    CObject *FromHandle( HANDLE h );
    CObject *LookupPermanent( HANDLE h );
    CObject *LookupTemporary( HANDLE h );
    void    RemoveHandle( HANDLE h );
    void    SetPermanent( HANDLE h, CObject *permOb );
};

#endif

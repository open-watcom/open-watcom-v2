/****************************************************************************
*
*                            Open Watcom Project
*
*  Copyright (c) 2004-2010 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Implementation of CObject.
*
****************************************************************************/


#include "stdafx.h"

const CRuntimeClass CObject::classCObject =
#ifdef _AFXDLL
    { "CObject", 0, 0xFFFF, NULL, _GetBaseClass };
#else
    { "CObject", 0, 0xFFFF, NULL, NULL };
#endif

CObject::CObject()
/****************/
{
}

CObject::~CObject()
/*****************/
{
}

void CObject::AssertValid() const
/*******************************/
{
}

void CObject::Dump( CDumpContext &dc ) const
/******************************************/
{
    UNUSED( dc );

#ifdef _DEBUG
    dc << "Class " << GetRuntimeClass()->m_lpszClassName << " at " << this << "\n";
#endif
}

CRuntimeClass *CObject::GetRuntimeClass() const
/*********************************************/
{
    return( (CRuntimeClass *)&classCObject );
}

void CObject::Serialize( CArchive & )
/***********************************/
{
}

BOOL CObject::IsKindOf( const CRuntimeClass *pClass ) const
/*********************************************************/
{
    return( GetRuntimeClass()->IsDerivedFrom( pClass ) );
}

BOOL CObject::IsSerializable() const
/**********************************/
{
    return( GetRuntimeClass()->m_wSchema != 0xFFFF );
}

#ifdef _AFXDLL

CRuntimeClass * PASCAL CObject::_GetBaseClass()
/*********************************************/
{
    return( NULL );
}

CRuntimeClass * PASCAL CObject::GetThisClass()
/********************************************/
{
    return( (CRuntimeClass *)&classCObject );
}

#endif // _AFXDLL

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
* Description:  Implementation of CStringArray.
*
****************************************************************************/


#include "stdafx.h"

IMPLEMENT_SERIAL( CStringArray, CObject, 0 )

CStringArray::CStringArray()
/**************************/
{
    m_pData = NULL;
    m_nSize = 0;
    m_nMaxSize = 0;
    m_nGrowBy = 1;
}

CStringArray::~CStringArray()
/***************************/
{
    RemoveAll();
    FreeExtra();
}

void CStringArray::Serialize( CArchive &ar )
/******************************************/
{
    CObject::Serialize( ar );

    if( ar.IsStoring() ) {
        ar.WriteCount( m_nSize );
        for( int i = 0; i < m_nSize; i++ ) {
            ar << m_pData[i];
        }
    } else {
        int nSize = ar.ReadCount();
        SetSize( nSize );
        for( int i = 0; i < nSize; i++ ) {
            ar >> m_pData[i];
        }
    }
}

#ifdef _DEBUG

void CStringArray::AssertValid() const
/************************************/
{
    CObject::AssertValid();
    
    ASSERT( m_nSize >= 0 );
}

void CStringArray::Dump( CDumpContext &dc ) const
/***********************************************/
{
    CObject::Dump( dc );

    dc << "m_pData = " << m_pData << "\n";
    dc << "m_nSize = " << m_nSize << "\n";
    dc << "m_nMaxSize = " << m_nMaxSize << "\n";
    dc << "m_nGrowBy = " << m_nGrowBy << "\n";
}

#endif // _DEBUG

INT_PTR CStringArray::Add( LPCTSTR newElement )
/*********************************************/
{
    if( m_nSize == m_nMaxSize ) {
        m_nMaxSize += m_nGrowBy;
        CString *pNewData = new CString[m_nMaxSize];
        if( m_pData != NULL ) {
            for( int i = 0; i < m_nSize; i++ ) {
                pNewData[i] = m_pData[i];
            }
            delete [] m_pData;
        }
        m_pData = pNewData;
    }
    m_pData[m_nSize] = newElement;
    int nIndex = m_nSize;
    m_nSize++;
    return( nIndex );
}

INT_PTR CStringArray::Add( const CString &newElement )
/****************************************************/
{
    if( m_nSize == m_nMaxSize ) {
        m_nMaxSize += m_nGrowBy;
        CString *pNewData = new CString[m_nMaxSize];
        if( m_pData != NULL ) {
            for( int i = 0; i < m_nSize; i++ ) {
                pNewData[i] = m_pData[i];
            }
            delete [] m_pData;
        }
        m_pData = pNewData;
    }
    m_pData[m_nSize] = newElement;
    int nIndex = m_nSize;
    m_nSize++;
    return( nIndex );
}

INT_PTR CStringArray::Append( const CStringArray &src )
/*****************************************************/
{
    int nNewMaxSize = m_nSize + src.m_nSize;
    if( nNewMaxSize > m_nMaxSize ) {
        if( nNewMaxSize % m_nGrowBy != 0 ) {
            nNewMaxSize -= nNewMaxSize % m_nGrowBy;
            nNewMaxSize += m_nGrowBy;
        }
        m_nMaxSize = nNewMaxSize;
        CString *pNewData = new CString[m_nMaxSize];
        if( m_pData != NULL ) {
            for( int i = 0; i < m_nSize; i++ ) {
                pNewData[i] = m_pData[i];
            }
            delete [] m_pData;
        }
        m_pData = pNewData;
    }
    for( int i = 0; i < src.m_nSize; i++ ) {
        m_pData[m_nSize + i] = src.m_pData[i];
    }
    int nIndex = m_nSize;
    m_nSize += src.m_nSize;
    return( nIndex );
}

void CStringArray::Copy( const CStringArray &src )
/************************************************/
{
    int nNewMaxSize = src.m_nSize;
    if( nNewMaxSize > m_nMaxSize ) {
        if( nNewMaxSize % m_nGrowBy != 0 ) {
            nNewMaxSize -= nNewMaxSize % m_nGrowBy;
            nNewMaxSize += m_nGrowBy;
        }
        m_nMaxSize = nNewMaxSize;
        if( m_pData != NULL ) {
            delete [] m_pData;
        }
        m_pData = new CString[m_nMaxSize];
    } else if( m_nMaxSize > src.m_nSize ) {
        for( int i = src.m_nSize; i < m_nMaxSize; i++ ) {
            m_pData[i].Empty();
        }
    }
    for( int i = 0; i < src.m_nSize; i++ ) {
        m_pData[i] = src.m_pData[i];
    }
    m_nSize = src.m_nSize;
}

void CStringArray::FreeExtra()
/****************************/
{
    if( m_nMaxSize > m_nSize && m_pData != NULL ) {
        if( m_nSize > 0 ) {
            CString *pNewData = new CString[m_nSize];
            for( int i = 0; i < m_nSize; i++ ) {
                pNewData[i] = m_pData[i];
            }
            delete [] m_pData;
            m_pData = pNewData;
        } else {
            delete [] m_pData;
            m_pData = NULL;
        }
        m_nMaxSize = m_nSize;
    }
}

void CStringArray::InsertAt( INT_PTR nIndex, LPCTSTR newElement, int nCount )
/***************************************************************************/
{
    ASSERT( nIndex >= 0 );
    ASSERT( nCount >= 1 );
    int nNewMaxSize = m_nSize + nCount;
    if( nNewMaxSize > m_nMaxSize ) {
        if( nNewMaxSize % m_nGrowBy != 0 ) {
            nNewMaxSize -= nNewMaxSize % m_nGrowBy;
            nNewMaxSize += m_nGrowBy;
        }
        m_nMaxSize = nNewMaxSize;
        CString *pNewData = new CString[m_nMaxSize];
        if( m_pData != NULL ) {
            for( int i = 0; i < m_nSize; i++ ) {
                pNewData[i] = m_pData[i];
            }
            delete [] m_pData;
        }
        m_pData = pNewData;
    }
    for( int i = m_nSize - 1; i >= nIndex; i-- ) {
        m_pData[i + nCount] = m_pData[i];
    }
    for( int i = 0; i < nCount; i++ ) {
        m_pData[nIndex + i] = newElement;
    }
    m_nSize += nCount;
}

void CStringArray::InsertAt( INT_PTR nIndex, const CString &newElement, int nCount )
/**********************************************************************************/
{
    ASSERT( nIndex >= 0 );
    ASSERT( nCount >= 1 );
    int nNewMaxSize = m_nSize + nCount;
    if( nNewMaxSize > m_nMaxSize ) {
        if( nNewMaxSize % m_nGrowBy != 0 ) {
            nNewMaxSize -= nNewMaxSize % m_nGrowBy;
            nNewMaxSize += m_nGrowBy;
        }
        m_nMaxSize = nNewMaxSize;
        CString *pNewData = new CString[m_nMaxSize];
        if( m_pData != NULL ) {
            for( int i = 0; i < m_nSize; i++ ) {
                pNewData[i] = m_pData[i];
            }
            delete [] m_pData;
        }
        m_pData = pNewData;
    }
    for( int i = m_nSize - 1; i >= nIndex; i-- ) {
        m_pData[i + nCount] = m_pData[i];
    }
    for( int i = 0; i < nCount; i++ ) {
        m_pData[nIndex + i] = newElement;
    }
    m_nSize += nCount;
}

void CStringArray::InsertAt( INT_PTR nStartIndex, CStringArray *pNewArray )
/*************************************************************************/
{
    ASSERT( nStartIndex >= 0 );
    int nNewMaxSize = m_nSize + pNewArray->m_nSize;
    if( nNewMaxSize > m_nMaxSize ) {
        if( nNewMaxSize % m_nGrowBy != 0 ) {
            nNewMaxSize -= nNewMaxSize % m_nGrowBy;
            nNewMaxSize += m_nGrowBy;
        }
        m_nMaxSize = nNewMaxSize;
        CString *pNewData = new CString[m_nMaxSize];
        if( m_pData != NULL ) {
            for( int i = 0; i < m_nSize; i++ ) {
                pNewData[i] = m_pData[i];
            }
            delete [] m_pData;
        }
        m_pData = pNewData;
    }
    for( int i = m_nSize - 1; i >= nStartIndex; i-- ) {
        m_pData[i + pNewArray->m_nSize] = m_pData[i];
    }
    for( int i = 0; i < pNewArray->m_nSize; i++ ) {
        m_pData[nStartIndex + i] = pNewArray->m_pData[i];
    }
    m_nSize += pNewArray->m_nSize;
}

void CStringArray::RemoveAt( INT_PTR nIndex, INT_PTR nCount )
/***********************************************************/
{
    ASSERT( nCount >= 1 );
    ASSERT( nIndex + nCount <= m_nSize );
    if( nIndex + nCount < m_nSize ) {
        for( int i = nIndex + nCount; i < m_nSize; i++ ) {
            m_pData[i - nCount] = m_pData[i];
        }
    }
    m_nSize -= nCount;
    for( int i = 0; i < nCount; i++ ) {
        m_pData[m_nSize + i].Empty();
    }
}

void CStringArray::SetAtGrow( INT_PTR nIndex, LPCTSTR newElement )
/****************************************************************/
{
    ASSERT( nIndex >= 0 );
    int nNewMaxSize = nIndex + 1;
    if( nNewMaxSize > m_nMaxSize ) {
        if( nNewMaxSize % m_nGrowBy != 0 ) {
            nNewMaxSize -= nNewMaxSize % m_nGrowBy;
            nNewMaxSize += m_nGrowBy;
        }
        m_nMaxSize = nNewMaxSize;
        CString *pNewData = new CString[m_nMaxSize];
        if( m_pData != NULL ) {
            for( int i = 0; i < m_nSize; i++ ) {
                pNewData[i] = m_pData[i];
            }
            delete [] m_pData;
        }
        m_pData = pNewData;
    }
    if( nIndex >= m_nSize ) {
        m_nSize = nIndex + 1;
    }
    m_pData[nIndex] = newElement;
}

void CStringArray::SetAtGrow( INT_PTR nIndex, const CString &newElement )
/***********************************************************************/
{
    ASSERT( nIndex >= 0 );
    int nNewMaxSize = nIndex + 1;
    if( nNewMaxSize > m_nMaxSize ) {
        if( nNewMaxSize % m_nGrowBy != 0 ) {
            nNewMaxSize -= nNewMaxSize % m_nGrowBy;
            nNewMaxSize += m_nGrowBy;
        }
        m_nMaxSize = nNewMaxSize;
        CString *pNewData = new CString[m_nMaxSize];
        if( m_pData != NULL ) {
            for( int i = 0; i < m_nSize; i++ ) {
                pNewData[i] = m_pData[i];
            }
            delete [] m_pData;
        }
        m_pData = pNewData;
    }
    if( nIndex >= m_nSize ) {
        m_nSize = nIndex + 1;
    }
    m_pData[nIndex] = newElement;
}

void CStringArray::SetSize( INT_PTR nNewSize, INT_PTR nGrowBy )
/*************************************************************/
{
    ASSERT( nNewSize >= 0 );
    if( nGrowBy > 0 ) {
        m_nGrowBy = nGrowBy;
    }
    int nNewMaxSize = nNewSize;
    if( nNewMaxSize > m_nMaxSize ) {
        if( nNewMaxSize % m_nGrowBy != 0 ) {
            nNewMaxSize -= nNewMaxSize % m_nGrowBy;
            nNewMaxSize += m_nGrowBy;
        }
        m_nMaxSize = nNewMaxSize;
        CString *pNewData = new CString[m_nMaxSize];
        if( m_pData != NULL ) {
            for( int i = 0; i < m_nSize; i++ ) {
                pNewData[i] = m_pData[i];
            }
            delete [] m_pData;
        }
        m_pData = pNewData;
    }
    m_nSize = nNewSize;
    FreeExtra();
}

#include "stdafx.h"
#include "explore.h"
#include "expldoc.h"

IMPLEMENT_DYNCREATE( CExploreDoc, CDocument )

CExploreDoc::CExploreDoc()
{
}

BOOL CExploreDoc::OnNewDocument()
{
    if( !CDocument::OnNewDocument() ) {
        return( FALSE );
    }

    m_listTreeItems.RemoveAll();
    m_listListItems.RemoveAll();
    
    return( TRUE );
}

void CExploreDoc::Serialize( CArchive &ar )
{
    CDocument::Serialize( ar );

    m_listTreeItems.Serialize( ar );
    m_listListItems.Serialize( ar );
}

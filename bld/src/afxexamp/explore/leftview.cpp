#include "stdafx.h"
#include "explore.h"
#include "expldoc.h"
#include "leftview.h"
#include "itemdlg.h"

IMPLEMENT_DYNCREATE( CLeftView, CTreeView )

BEGIN_MESSAGE_MAP( CLeftView, CTreeView )
    ON_WM_CREATE()
    ON_COMMAND( ID_INSERT_TREE_ITEM, OnInsertTreeItem )
END_MESSAGE_MAP()

CLeftView::CLeftView()
{
}

void CLeftView::OnInitialUpdate()
{
    CTreeView::OnInitialUpdate();

    GetTreeCtrl().DeleteAllItems();

    CExploreDoc *pDoc = GetDocument();
    ASSERT_VALID( pDoc );

    POSITION position = pDoc->m_listTreeItems.GetHeadPosition();
    while( position != NULL ) {
        CString str = pDoc->m_listTreeItems.GetNext( position );
        GetTreeCtrl().InsertItem( str );
    }
}

void CLeftView::OnInsertTreeItem()
{
    CItemDialog dlg( this );
    if( dlg.DoModal() == IDOK ) {
        CExploreDoc *pDoc = GetDocument();
        ASSERT_VALID( pDoc );
        pDoc->m_listTreeItems.AddTail( dlg.m_strName );
        GetTreeCtrl().InsertItem( dlg.m_strName );
    }
}

int CLeftView::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
    if( CTreeView::OnCreate( lpCreateStruct ) == -1 ) {
        return( -1 );
    }

    HICON hIcon = AfxGetApp()->LoadIcon( IDI_TREEITEM );
    m_imlIcons.Create( 16, 16, ILC_MASK, 0, 1 );
    m_imlIcons.Add( hIcon );
    GetTreeCtrl().SetImageList( &m_imlIcons, TVSIL_NORMAL );

    return( 0 );
}

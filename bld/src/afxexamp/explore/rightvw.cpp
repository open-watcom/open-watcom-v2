#include "stdafx.h"
#include "explore.h"
#include "expldoc.h"
#include "rightvw.h"
#include "itemdlg.h"

IMPLEMENT_DYNCREATE( CRightView, CListView )

BEGIN_MESSAGE_MAP( CRightView, CListView )
    ON_WM_CREATE()
    ON_COMMAND( ID_INSERT_LIST_ITEM, OnInsertListItem )
    ON_COMMAND_EX( ID_VIEW_LARGEICON, OnViewMode )
    ON_COMMAND_EX( ID_VIEW_SMALLICON, OnViewMode )
    ON_COMMAND_EX( ID_VIEW_LIST, OnViewMode )
    ON_COMMAND_EX( ID_VIEW_DETAILS, OnViewMode )
    ON_UPDATE_COMMAND_UI( ID_VIEW_LARGEICON, OnUpdateViewMode )
    ON_UPDATE_COMMAND_UI( ID_VIEW_SMALLICON, OnUpdateViewMode )
    ON_UPDATE_COMMAND_UI( ID_VIEW_LIST, OnUpdateViewMode )
    ON_UPDATE_COMMAND_UI( ID_VIEW_DETAILS, OnUpdateViewMode )
END_MESSAGE_MAP()

CRightView::CRightView()
{
}

void CRightView::OnInitialUpdate()
{
    CListView::OnInitialUpdate();

    GetListCtrl().DeleteAllItems();

    CExploreDoc *pDoc = GetDocument();
    ASSERT_VALID( pDoc );

    POSITION position = pDoc->m_listListItems.GetHeadPosition();
    while( position != NULL ) {
        CString str = pDoc->m_listListItems.GetNext( position );
        GetListCtrl().InsertItem( GetListCtrl().GetItemCount(), str, 0 );
    }
}

int CRightView::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
    if( CListView::OnCreate( lpCreateStruct ) == -1 ) {
        return( -1 );
    }

    CString str;
    str.LoadString( IDS_NAME );
    GetListCtrl().InsertColumn( 0, str, LVCFMT_LEFT, 200 );

    HICON hIcon = AfxGetApp()->LoadIcon( IDI_LISTITEM );
    m_imlLargeIcons.Create( 32, 32, ILC_MASK, 0, 1 );
    m_imlSmallIcons.Create( 16, 16, ILC_MASK, 0, 1 );
    m_imlLargeIcons.Add( hIcon );
    m_imlSmallIcons.Add( hIcon );
    GetListCtrl().SetImageList( &m_imlLargeIcons, LVSIL_NORMAL );
    GetListCtrl().SetImageList( &m_imlSmallIcons, LVSIL_SMALL );
    
    return( 0 );
}

void CRightView::OnInsertListItem()
{
    CItemDialog dlg( this );
    if( dlg.DoModal() == IDOK ) {
        CExploreDoc *pDoc = GetDocument();
        ASSERT_VALID( pDoc );
        pDoc->m_listListItems.AddTail( dlg.m_strName );
        GetListCtrl().InsertItem( GetListCtrl().GetItemCount(), dlg.m_strName, 0 );
    }
}

BOOL CRightView::OnViewMode( UINT nID )
{
    UINT nStyle;
    switch( nID ) {
    case ID_VIEW_LARGEICON:
        nStyle = LVS_ICON;
        break;
    case ID_VIEW_SMALLICON:
        nStyle = LVS_SMALLICON;
        break;
    case ID_VIEW_LIST:
        nStyle = LVS_LIST;
        break;
    case ID_VIEW_DETAILS:
        nStyle = LVS_REPORT;
        break;
    default:
        return( FALSE );
    }
    ModifyStyle( LVS_TYPEMASK, nStyle );
    return( TRUE );
}

void CRightView::OnUpdateViewMode( CCmdUI *pCmdUI )
{
    UINT nStyle = GetStyle() & LVS_TYPEMASK;
    switch( pCmdUI->m_nID ) {
    case ID_VIEW_LARGEICON:
        pCmdUI->SetCheck( nStyle == LVS_ICON );
        break;
    case ID_VIEW_SMALLICON:
        pCmdUI->SetCheck( nStyle == LVS_SMALLICON );
        break;
    case ID_VIEW_LIST:
        pCmdUI->SetCheck( nStyle == LVS_LIST );
        break;
    case ID_VIEW_DETAILS:
        pCmdUI->SetCheck( nStyle == LVS_REPORT );
        break;
    }
}

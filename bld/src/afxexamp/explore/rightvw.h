#ifndef _RIGHTVW_H_INCLUDED
#define _RIGHTVW_H_INCLUDED

class CRightView : public CListView {
protected:
    CImageList  m_imlLargeIcons;
    CImageList  m_imlSmallIcons;
    
    DECLARE_DYNCREATE( CRightView )

public:
    CRightView();

    virtual void    OnInitialUpdate();

    CExploreDoc *GetDocument();
    
protected:
    afx_msg int     OnCreate( LPCREATESTRUCT lpCreateStruct );
    afx_msg void    OnInsertListItem();
    afx_msg BOOL    OnViewMode( UINT nID );
    afx_msg void    OnUpdateViewMode( CCmdUI *pCmdUI );
    DECLARE_MESSAGE_MAP()
};

inline CExploreDoc *CRightView::GetDocument()
{
    ASSERT( m_pDocument->IsKindOf( RUNTIME_CLASS( CExploreDoc ) ) );
    return( (CExploreDoc *)m_pDocument );
}

#endif /* _RIGHTVW_H_INCLUDED */

#ifndef _LEFTVIEW_H_INCLUDED
#define _LEFTVIEW_H_INCLUDED

class CLeftView : public CTreeView {
protected:
    CImageList  m_imlIcons;

    DECLARE_DYNCREATE( CLeftView )

public:
    CLeftView();

    virtual void    OnInitialUpdate();

    CExploreDoc *GetDocument();
    
protected:
    afx_msg int     OnCreate( LPCREATESTRUCT lpCreateStruct );
    afx_msg void    OnInsertTreeItem();
    DECLARE_MESSAGE_MAP()
};

inline CExploreDoc *CLeftView::GetDocument()
{
    ASSERT( m_pDocument->IsKindOf( RUNTIME_CLASS( CExploreDoc ) ) );
    return( (CExploreDoc *)m_pDocument );
}

#endif /* _LEFTVIEW_H_INCLUDED */

#ifndef _DRAWVIEW_H_INCLUDED
#define _DRAWVIEW_H_INCLUDED

#define DRAWTOOL_ARROW      0
#define DRAWTOOL_LINE       1
#define DRAWTOOL_RECTANGLE  2
#define DRAWTOOL_ELLIPSE    3

class CDrawingView : public CScrollView {
protected:
    int         m_nDrawTool;
    COLORREF    m_crLine;
    COLORREF    m_crFill;
    CShape      *m_pCurShape;
    CPoint      m_lastPoint;
    HCURSOR     m_hcurArrow;
    HCURSOR     m_hcurCross;
    
    DECLARE_DYNCREATE( CDrawingView )

public:
    CDrawingView();

    virtual void    OnDraw( CDC *pDC );
    virtual void    OnInitialUpdate();
    virtual BOOL    OnPreparePrinting( CPrintInfo *pInfo );

    CDrawingDoc *GetDocument() const;
    
protected:
    afx_msg void    OnLButtonDown( UINT nFlags, CPoint point );
    afx_msg void    OnLButtonUp( UINT nFlags, CPoint point );
    afx_msg void    OnMouseMove( UINT nFlags, CPoint point );
    afx_msg BOOL    OnSetCursor( CWnd *pWnd, UINT nHitTest, UINT message );
    afx_msg void    OnDrawArrow();
    afx_msg void    OnDrawLine();
    afx_msg void    OnDrawRectangle();
    afx_msg void    OnDrawEllipse();
    afx_msg void    OnDrawLineColor();
    afx_msg void    OnDrawFillColor();
    afx_msg void    OnUpdateDrawArrow( CCmdUI *pCmdUI );
    afx_msg void    OnUpdateDrawLine( CCmdUI *pCmdUI );
    afx_msg void    OnUpdateDrawRectangle( CCmdUI *pCmdUI );
    afx_msg void    OnUpdateDrawEllipse( CCmdUI *pCmdUI );
    DECLARE_MESSAGE_MAP()
};

inline CDrawingDoc *CDrawingView::GetDocument() const
{
    ASSERT( m_pDocument->IsKindOf( RUNTIME_CLASS( CDrawingDoc ) ) );
    return( (CDrawingDoc *)m_pDocument );
}

#endif /* _DRAWVIEW_H_INCLUDED */

#ifndef _EXPLFRM_H_INCLUDED
#define _EXPLFRM_H_INCLUDED

class CLeftView;
class CRightView;

class CExploreFrameWnd : public CFrameWnd {
    DECLARE_DYNCREATE( CExploreFrameWnd )

public:
    CExploreFrameWnd();

    virtual BOOL    OnCmdMsg( UINT nID, int nCode, void *pExtra, AFX_CMDHANDLERINFO *pHandlerInfo );
    virtual BOOL    OnCreateClient( LPCREATESTRUCT lpcs, CCreateContext *pContext );

    CLeftView   *GetLeftView();
    CRightView  *GetRightView();
    
protected:
    CToolBar        m_wndToolBar;
    CStatusBar      m_wndStatusBar;
    CSplitterWnd    m_wndSplitter;

    afx_msg int     OnCreate( LPCREATESTRUCT lpCreateStruct );
    DECLARE_MESSAGE_MAP()
};

#endif /* _EXPLFRM_H_INCLUDED */

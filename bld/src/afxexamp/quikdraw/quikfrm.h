#ifndef _QUIKFRM_H_INCLUDED
#define _QUIKFRM_H_INCLUDED

class CQuickDrawFrameWnd : public CMDIFrameWnd {
    DECLARE_DYNAMIC( CQuickDrawFrameWnd )

public:
    CQuickDrawFrameWnd();

protected:
    CToolBar    m_wndToolBar;
    CStatusBar  m_wndStatusBar;

    afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
    DECLARE_MESSAGE_MAP()
};

#endif /* _QUIKFRM_H_INCLUDED */

#ifndef _EDITFRM_H_INCLUDED
#define _EDITFRM_H_INCLUDED

class CTextEditFrameWnd : public CMDIFrameWnd {
    DECLARE_DYNAMIC( CTextEditFrameWnd )

public:
    CTextEditFrameWnd();

protected:
    CToolBar    m_wndToolBar;
    CStatusBar  m_wndStatusBar;

    afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
    DECLARE_MESSAGE_MAP()
};

#endif /* _EDITFRM_H_INCLUDED */

#ifndef _ITEMDLG_H_INCLUDED
#define _ITEMDLG_H_INCLUDED

class CItemDialog : public CDialog {
public:
    CString m_strName;

    CItemDialog( CWnd *pParentWnd );

    virtual void    DoDataExchange( CDataExchange *pDX );
};

#endif /* _ITEMDLG_H_INCLUDED */

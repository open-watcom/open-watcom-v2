#include "stdafx.h"
#include "explore.h"
#include "itemdlg.h"

CItemDialog::CItemDialog( CWnd *pParentWnd )
    : CDialog( IDD_INSERTITEM, pParentWnd )
{
}

void CItemDialog::DoDataExchange( CDataExchange *pDX )
{
    CDialog::DoDataExchange( pDX );

    DDX_Text( pDX, IDC_NAME, m_strName );
}

#include "stdafx.h"

class CTestCommands : public CCmdTarget {
public:
    BOOL    m_bCommand1Called;
    BOOL    m_bCommand2Called;
    BOOL    m_bCommand3Called;
    UINT    m_nLast456;
    UINT    m_nLast789;
    BOOL    m_bUpdateCommand1Called;
    BOOL    m_bUpdateCommand2Called;
    UINT    m_nLastUpdate345;
    CCmdUI  m_testCmdUI;

    CTestCommands();

    afx_msg void    OnCommand1();
    afx_msg BOOL    OnCommand23( UINT nID );
    afx_msg void    OnCommand456( UINT nID );
    afx_msg BOOL    OnCommand789( UINT nID );
    afx_msg void    OnUpdateCommand12( CCmdUI *pCmdUI );
    afx_msg void    OnUpdateCommand345( CCmdUI *pCmdUI );
    DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP( CTestCommands, CCmdTarget )
    ON_COMMAND( 1, OnCommand1 )
    ON_COMMAND_EX( 2, OnCommand23 )
    ON_COMMAND_EX( 3, OnCommand23 )
    ON_COMMAND_RANGE( 4, 6, OnCommand456 )
    ON_COMMAND_EX_RANGE( 7, 9, OnCommand789 )
    ON_UPDATE_COMMAND_UI( 1, OnUpdateCommand12 )
    ON_UPDATE_COMMAND_UI( 2, OnUpdateCommand12 )
    ON_UPDATE_COMMAND_UI_RANGE( 3, 5, OnUpdateCommand345 )
END_MESSAGE_MAP()

CTestCommands::CTestCommands()
{
    m_bCommand1Called = FALSE;
    m_bCommand2Called = FALSE;
    m_bCommand3Called = FALSE;
    m_nLast456 = 0;
    m_nLast789 = 0;
    m_bUpdateCommand1Called = FALSE;
    m_bUpdateCommand2Called = FALSE;
    m_nLastUpdate345 = 0;
}

void CTestCommands::OnCommand1()
{
    m_bCommand1Called = TRUE;
}

BOOL CTestCommands::OnCommand23( UINT nID )
{
    if( nID == 2 ) {
        m_bCommand2Called = TRUE;
        return( TRUE );
    } else if( nID == 3 ) {
        m_bCommand3Called = TRUE;
        return( FALSE );
    } else {
        _fail;
        return( FALSE );
    }
}

void CTestCommands::OnCommand456( UINT nID )
{
    if( nID != 4 && nID != 5 && nID != 6 ) _fail;
    m_nLast456 = nID;
}

BOOL CTestCommands::OnCommand789( UINT nID )
{
    if( nID != 7 && nID != 8 && nID != 9 ) _fail;
    m_nLast789 = nID;
    return( nID != 9 );
}

void CTestCommands::OnUpdateCommand12( CCmdUI *pCmdUI )
{
    if( pCmdUI != &m_testCmdUI ) _fail;
    if( pCmdUI->m_nID == 1 ) {
        m_bUpdateCommand1Called = TRUE;
    } else if( pCmdUI->m_nID == 2 ) {
        m_bUpdateCommand2Called = TRUE;
    } else {
        _fail;
    }
}

void CTestCommands::OnUpdateCommand345( CCmdUI *pCmdUI )
{
    if( pCmdUI != &m_testCmdUI ) _fail;
    if( pCmdUI->m_nID != 3 && pCmdUI->m_nID != 4 && pCmdUI->m_nID != 5 ) _fail;
    m_nLastUpdate345 = pCmdUI->m_nID;
    if( pCmdUI->m_nID == 5 ) {
        pCmdUI->ContinueRouting();
    }
}

int main()
{
    CTestCommands test;
    if( !test.OnCmdMsg( 1, CN_COMMAND, NULL, NULL ) ) _fail;
    if( !test.m_bCommand1Called ) _fail;
    if( !test.OnCmdMsg( 2, CN_COMMAND, NULL, NULL ) ) _fail;
    if( !test.m_bCommand2Called ) _fail;
    if( test.OnCmdMsg( 3, CN_COMMAND, NULL, NULL ) ) _fail;
    if( !test.m_bCommand3Called ) _fail;
    if( !test.OnCmdMsg( 4, CN_COMMAND, NULL, NULL ) ) _fail;
    if( test.m_nLast456 != 4 ) _fail;
    if( !test.OnCmdMsg( 5, CN_COMMAND, NULL, NULL ) ) _fail;
    if( test.m_nLast456 != 5 ) _fail;
    if( !test.OnCmdMsg( 6, CN_COMMAND, NULL, NULL ) ) _fail;
    if( test.m_nLast456 != 6 ) _fail;
    if( !test.OnCmdMsg( 7, CN_COMMAND, NULL, NULL ) ) _fail;
    if( test.m_nLast789 != 7 ) _fail;
    if( !test.OnCmdMsg( 8, CN_COMMAND, NULL, NULL ) ) _fail;
    if( test.m_nLast789 != 8 ) _fail;
    if( test.OnCmdMsg( 9, CN_COMMAND, NULL, NULL ) ) _fail;
    if( test.m_nLast789 != 9 ) _fail;

    test.m_testCmdUI.m_nID = 1;
    if( !test.OnCmdMsg( 1, CN_UPDATE_COMMAND_UI, &test.m_testCmdUI, NULL ) ) _fail;
    if( !test.m_bUpdateCommand1Called ) _fail;
    test.m_testCmdUI.m_nID = 2;
    if( !test.OnCmdMsg( 2, CN_UPDATE_COMMAND_UI, &test.m_testCmdUI, NULL ) ) _fail;
    if( !test.m_bUpdateCommand2Called ) _fail;
    test.m_testCmdUI.m_nID = 3;
    if( !test.OnCmdMsg( 3, CN_UPDATE_COMMAND_UI, &test.m_testCmdUI, NULL ) ) _fail;
    if( test.m_nLastUpdate345 != 3 ) _fail;
    test.m_testCmdUI.m_nID = 4;
    if( !test.OnCmdMsg( 4, CN_UPDATE_COMMAND_UI, &test.m_testCmdUI, NULL ) ) _fail;
    if( test.m_nLastUpdate345 != 4 ) _fail;
    test.m_testCmdUI.m_nID = 5;
    if( test.OnCmdMsg( 5, CN_UPDATE_COMMAND_UI, &test.m_testCmdUI, NULL ) ) _fail;
    if( test.m_nLastUpdate345 != 5 ) _fail;

    if( test.OnCmdMsg( 10, CN_COMMAND, NULL, NULL ) ) _fail;
    if( test.OnCmdMsg( 10, CN_UPDATE_COMMAND_UI, &test.m_testCmdUI, NULL ) ) _fail;

    AFX_CMDHANDLERINFO chi;
    if( !test.OnCmdMsg( 1, CN_COMMAND, NULL, &chi ) ) _fail;
    if( chi.pTarget != &test ) _fail;
    if( chi.pmf != CTestCommands::OnCommand1 ) _fail;

    _PASS;
}

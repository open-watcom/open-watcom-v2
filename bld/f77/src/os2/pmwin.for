c$define INCL_WINBUTTONS
c$define INCL_WINLISTBOXES
c$define INCL_WINDIALOGS
c$define INCL_WINMENUS
c$define INCL_WINMESSAGEMGR
c$include os2def.fap
c$include pmwin.fap


        function WinCheckButton( hwndDlg, id, usCheckState )
        integer hwndDlg, id, usCheckState
        include 'os2def.fi'
        include 'pmwin.fi'
        WinCheckButton = WinSendDlgItemMsg( hwndDlg, id, BM_SETCHECK,
     +     MPFROMSHORT( usCheckState ), 0 )
        end


        function WinQueryButtonCheckState( hwndDlg, id )
        integer hwndDlg, id
        include 'os2def.fi'
        include 'pmwin.fi'
        WinQueryButtonCheckState = WinSendDlgItemMsg( hwndDlg, id,
     +     BM_QUERYCHECK, 0, 0 )
        end


        function WinEnableControl( hwndDlg, id, fEnable )
        integer hwndDlg, id, fEnable
        include 'os2def.fi'
        include 'pmwin.fi'
        WinEnableControl = WinEnableWindow( WinWindowFromID( hwndDlg,
     +     id ), fEnable )
        end


        function WinIsControlEnabled( hwndDlg, id )
        integer hwndDlg, id
        include 'os2def.fi'
        include 'pmwin.fi'
        WinIsControlEnabled = WinIsWindowEnabled(
     +     WinWindowFromID( hwndDlg, id ) )
        end


        function WinDeleteLboxItem( hwndLbox, index )
        integer hwndLbox, index
        include 'os2def.fi'
        include 'pmwin.fi'
        WinDeleteLboxItem = WinSendMsg( hwndLbox, LM_DELETEITEM,
     +     MPFROMLONG( index ), 0 )
        end


        function WinInsertLboxItem( hwndLbox, index, psz )
        integer hwndLbox, index, psz
        include 'os2def.fi'
        include 'pmwin.fi'
        WinInsertLboxItem = WinSendMsg( hwndLbox, LM_INSERTITEM,
     +     MPFROMLONG( index ), MPFROMP( psz ) )
        end


        function WinQueryLboxCount( hwndLbox )
        integer hwndLbox
        include 'os2def.fi'
        include 'pmwin.fi'
        WinQueryLboxCount = WinSendMsg( hwndLbox, LM_QUERYITEMCOUNT,
     +     0, 0 )
        end


        function WinQueryLboxItemText( hwndLbox, index, psz,
     +                                         cchMax )
        integer hwndLbox, index, psz, cchMax
        include 'os2def.fi'
        include 'pmwin.fi'
        WinQueryLboxItemText = WinSendMsg( hwndLbox, LM_QUERYITEMTEXT,
     +     MPFROM2SHORT( index, cchMax ), MPFROMP( psz ) )
        end


        function WinQueryLboxItemTextLength( hwndLbox, index )
        integer hwndLbox, index
        include 'os2def.fi'
        include 'pmwin.fi'
        WinQueryLboxItemTextLength = WinSendMsg( hwndLbox,
     +     LM_QUERYITEMTEXTLENGTH, MPFROMSHORT( index ), 0 )
        end


        function WinSetLboxItemText( hwndLbox, index, psz )
        integer hwndLbox, index, psz
        include 'os2def.fi'
        include 'pmwin.fi'
        WinSetLboxItemText = WinSendMsg( hwndLbox, LM_SETITEMTEXT,
     +     MPFROMLONG( index ), MPFROMP( psz ) )
        end


        function WinQueryLboxSelectedItem( hwndLbox )
        integer hwndLbox
        include 'os2def.fi'
        include 'pmwin.fi'
        WinQueryLboxSelectedItem = WinSendMsg( hwndLbox,
     +     LM_QUERYSELECTION, MPFROMLONG( LIT_FIRST ), 0 )
        end


        function WinCheckMenuItem( hwndMenu, id, fcheck )
        integer hwndMenu, id, fcheck
        include 'os2def.fi'
        include 'pmwin.fi'
        if( fcheck )then
            fcheck = MIA_CHECKED
        else
            fcheck = 0
        end if
        WinCheckMenuItem = WinSendMsg( hwndMenu, MM_SETITEMATTR,
     +     MPFROM2SHORT( id, TRUE ),
     +     MPFROM2SHORT( MIA_CHECKED, fcheck ) )
        end


        function WinIsMenuItemChecked( hwndMenu, id )
        integer hwndMenu, id
        include 'os2def.fi'
        include 'pmwin.fi'
        WinIsMenuItemChecked = WinSendMsg( hwndMenu, MM_QUERYITEMATTR,
     +     MPFROM2SHORT( id, TRUE ), MPFROMLONG( MIA_CHECKED ) )
        end


        function WinEnableMenuItem( hwndMenu, id, fEnable )
        integer hwndMenu, id, fEnable
        include 'os2def.fi'
        include 'pmwin.fi'
        if( fEnable )then
            fEnable = 0
        else
            fEnable = MIA_DISABLED
        end if
        WinEnableMenuItem = WinSendMsg( hwndMenu, MM_SETITEMATTR,
     +     MPFROM2SHORT( id, TRUE ),
     +     MPFROM2SHORT( MIA_DISABLED, fEnable ) )
        end


        function WinIsMenuItemEnabled( hwndMenu, id )
        integer hwndMenu, id
        include 'os2def.fi'
        include 'pmwin.fi'
        WinIsMenuItemEnabled = .not. WinSendMsg( hwndMenu,
     +     MM_QUERYITEMATTR, MPFROM2SHORT( id, TRUE ),
     +     MPFROMLONG( MIA_DISABLED ) )
        end


        function WinSetMenuItemText( hwndMenu, id, psz )
        integer hwndMenu, id, psz
        include 'os2def.fi'
        include 'pmwin.fi'
        WinSetMenuItemText = WinSendMsg( hwndMenu, MM_SETITEMTEXT,
     +     MPFROMLONG( id ), MPFROMP( psz ) )
        end


        function WinIsMenuItemValid( hwndMenu, id )
        integer hwndMenu, id
        include 'os2def.fi'
        include 'pmwin.fi'
        WinIsMenuItemValid = WinSendMsg( hwndMenu, MM_ISITEMVALID,
     +     MPFROM2SHORT( id, TRUE ), MPFROMLONG( FALSE ) )
        end

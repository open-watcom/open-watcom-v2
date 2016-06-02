# afxapi Builder Control file
# ===========================

set PROJNAME=afxapi

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
#================
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
    <CCCMD> nt/*.h                        <OWRELROOT>/h/nt/afx/
    <CCCMD> nt/*.inl                      <OWRELROOT>/h/nt/afx/
    <CCCMD> nt/*.rc                       <OWRELROOT>/h/nt/afx/

    <CCCMD> include/res/*.cur             <OWRELROOT>/h/nt/afx/res/

    <CCCMD> dnt386/nafxcwd.lib            <OWRELROOT>/lib386/nt/afx/
    <CCCMD> dnt386u/uafxcwd.lib           <OWRELROOT>/lib386/nt/afx/
    <CCCMD> nt386/nafxcw.lib              <OWRELROOT>/lib386/nt/afx/
    <CCCMD> nt386u/uafxcw.lib             <OWRELROOT>/lib386/nt/afx/

    <CCCMD> dnt386.dll/owf??d.dll         <OWRELROOT>/binnt/
    <CCCMD> dnt386u.dll/owf??ud.dll       <OWRELROOT>/binnt/
    <CCCMD> nt386.dll/owf??.dll           <OWRELROOT>/binnt/
    <CCCMD> nt386u.dll/owf??u.dll         <OWRELROOT>/binnt/

    <CCCMD> dnt386.dll/owf??d.lib         <OWRELROOT>/lib386/nt/afx/
    <CCCMD> dnt386.dll/owfs??d.lib        <OWRELROOT>/lib386/nt/afx/
    <CCCMD> dnt386u.dll/owf??ud.lib       <OWRELROOT>/lib386/nt/afx/
    <CCCMD> dnt386u.dll/owfs??ud.lib      <OWRELROOT>/lib386/nt/afx/
    <CCCMD> nt386.dll/owf??.lib           <OWRELROOT>/lib386/nt/afx/
    <CCCMD> nt386.dll/owfs??.lib          <OWRELROOT>/lib386/nt/afx/
    <CCCMD> nt386u.dll/owf??u.lib         <OWRELROOT>/lib386/nt/afx/
    <CCCMD> nt386u.dll/owfs??u.lib        <OWRELROOT>/lib386/nt/afx/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]

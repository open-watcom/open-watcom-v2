# afxapi Builder Control file
# ===========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <1> rel ]
    cdsay <PROJDIR>

[ BLOCK <1> rel cprel ]
#======================
  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt/*.h                        <OWRELROOT>/h/nt/afx/
    <CPCMD> nt/*.inl                      <OWRELROOT>/h/nt/afx/
    <CPCMD> nt/*.rc                       <OWRELROOT>/h/nt/afx/

    <CPCMD> include/res/*.cur             <OWRELROOT>/h/nt/afx/res/

    <CPCMD> dnt386/nafxcwd.lib            <OWRELROOT>/lib386/nt/afx/
    <CPCMD> dnt386u/uafxcwd.lib           <OWRELROOT>/lib386/nt/afx/
    <CPCMD> nt386/nafxcw.lib              <OWRELROOT>/lib386/nt/afx/
    <CPCMD> nt386u/uafxcw.lib             <OWRELROOT>/lib386/nt/afx/

    <CPCMD> dnt386.dll/owf??d.dll         <OWRELROOT>/binnt/
    <CPCMD> dnt386u.dll/owf??ud.dll       <OWRELROOT>/binnt/
    <CPCMD> nt386.dll/owf??.dll           <OWRELROOT>/binnt/
    <CPCMD> nt386u.dll/owf??u.dll         <OWRELROOT>/binnt/

    <CPCMD> dnt386.dll/owf??d.lib         <OWRELROOT>/lib386/nt/afx/
    <CPCMD> dnt386.dll/owfs??d.lib        <OWRELROOT>/lib386/nt/afx/
    <CPCMD> dnt386u.dll/owf??ud.lib       <OWRELROOT>/lib386/nt/afx/
    <CPCMD> dnt386u.dll/owfs??ud.lib      <OWRELROOT>/lib386/nt/afx/
    <CPCMD> nt386.dll/owf??.lib           <OWRELROOT>/lib386/nt/afx/
    <CPCMD> nt386.dll/owfs??.lib          <OWRELROOT>/lib386/nt/afx/
    <CPCMD> nt386u.dll/owf??u.lib         <OWRELROOT>/lib386/nt/afx/
    <CPCMD> nt386u.dll/owfs??u.lib        <OWRELROOT>/lib386/nt/afx/

[ BLOCK . . ]
#============
cdsay <PROJDIR>

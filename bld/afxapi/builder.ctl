# afxapi Builder Control file
# ===========================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 acprel2 ]
#================================
  [ IFDEF (os_nt "") <2*> ]
    <CPCMD> nt/*.h                        <RELROOT>/h/nt/afx/
    <CPCMD> nt/*.inl                      <RELROOT>/h/nt/afx/
    <CPCMD> nt/*.rc                       <RELROOT>/h/nt/afx/

    <CPCMD> include/res/*.cur             <RELROOT>/h/nt/afx/res/

    <CPCMD> dnt386/nafxcwd.lib            <RELROOT>/lib386/nt/afx/
    <CPCMD> dnt386u/uafxcwd.lib           <RELROOT>/lib386/nt/afx/
    <CPCMD> nt386/nafxcw.lib              <RELROOT>/lib386/nt/afx/
    <CPCMD> nt386u/uafxcw.lib             <RELROOT>/lib386/nt/afx/

    <CPCMD> dnt386.dll/owf??d.dll         <RELROOT>/binnt/
    <CPCMD> dnt386u.dll/owf??ud.dll       <RELROOT>/binnt/
    <CPCMD> nt386.dll/owf??.dll           <RELROOT>/binnt/
    <CPCMD> nt386u.dll/owf??u.dll         <RELROOT>/binnt/

    <CPCMD> dnt386.dll/owf??d.lib         <RELROOT>/lib386/nt/afx/
    <CPCMD> dnt386.dll/owfs??d.lib        <RELROOT>/lib386/nt/afx/
    <CPCMD> dnt386u.dll/owf??ud.lib       <RELROOT>/lib386/nt/afx/
    <CPCMD> dnt386u.dll/owfs??ud.lib      <RELROOT>/lib386/nt/afx/
    <CPCMD> nt386.dll/owf??.lib           <RELROOT>/lib386/nt/afx/
    <CPCMD> nt386.dll/owfs??.lib          <RELROOT>/lib386/nt/afx/
    <CPCMD> nt386u.dll/owf??u.lib         <RELROOT>/lib386/nt/afx/
    <CPCMD> nt386u.dll/owfs??u.lib        <RELROOT>/lib386/nt/afx/

[ BLOCK <1> clean ]
#==================
    pmake -d all <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>

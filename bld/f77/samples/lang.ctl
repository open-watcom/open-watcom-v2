# FORTRAN samples Builder Control file
# ====================================

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/bat/master.ctl ]
[ LOG <LOGFNAME>.<LOGEXT> ]

cdsay .

[ BLOCK <1> build rel2 ]
#=======================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h

[ BLOCK <1> rel2 ]
#=================
    cdsay <PROJDIR>

[ BLOCK <1> rel2 cprel2 ]
#========================
<CPCMD> <DEVDIR>/f77/src/*                          <RELROOT>/src/fortran/
<CPCMD> <DEVDIR>/f77/src/win/*                      <RELROOT>/src/fortran/win/
<CPCMD> <DEVDIR>/f77/src/dos/*                      <RELROOT>/src/fortran/dos/
<CPCMD> <DEVDIR>/f77/src/os2/*                      <RELROOT>/src/fortran/os2/
<CPCMD> <DEVDIR>/f77/samples/os2/*                  <RELROOT>/samples/fortran/os2/
<CPCMD> <DEVDIR>/f77/samples/os2/rexx/*             <RELROOT>/samples/fortran/os2/rexx/
<CPCMD> <DEVDIR>/f77/samples/mtdll/*                <RELROOT>/samples/fortran/mtdll/
<CPCMD> <DEVDIR>/f77/samples/mthread/mthrdos2.for   <RELROOT>/samples/fortran/os2/mthread.for
<CPCMD> <DEVDIR>/f77/samples/mthread/makefile.os2   <RELROOT>/samples/fortran/os2/makefile
<CPCMD> <DEVDIR>/f77/samples/mthread/mthrdnt.for    <RELROOT>/samples/fortran/win32/mthread.for
<CPCMD> <DEVDIR>/f77/samples/mthread/makefile.nt    <RELROOT>/samples/fortran/win32/makefile
<CPCMD> <DEVDIR>/f77/samples/win/*                  <RELROOT>/samples/fortran/win/
<CPCMD> <DEVDIR>/f77/samples/windll/*               <RELROOT>/samples/fortran/win/dll/
<CPCMD> <DEVDIR>/f77/samples/dll/*                  <RELROOT>/samples/fortran/os2/dll/
<CPCMD> <DEVDIR>/f77/samples/dll/*                  <RELROOT>/samples/fortran/win32/dll/
<CPCMD> <DEVDIR>/f77/samples/dll/os2/makefile       <RELROOT>/samples/fortran/os2/dll/
<CPCMD> <DEVDIR>/f77/samples/dll/win32/makefile     <RELROOT>/samples/fortran/win32/dll/

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>

# FORTRAN samples Builder Control file
# ====================================

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

[ BLOCK <1> rel2 cprel2 ]
#========================
<CPCMD> <SRCDIR>/f77/src/*                          <OWRELROOT>/src/fortran/
<CPCMD> <SRCDIR>/f77/src/win/*                      <OWRELROOT>/src/fortran/win/
<CPCMD> <SRCDIR>/f77/src/dos/*                      <OWRELROOT>/src/fortran/dos/
<CPCMD> <SRCDIR>/f77/src/os2/*                      <OWRELROOT>/src/fortran/os2/
<CPCMD> <SRCDIR>/f77/samples/os2/*                  <OWRELROOT>/samples/fortran/os2/
<CPCMD> <SRCDIR>/f77/samples/os2/rexx/*             <OWRELROOT>/samples/fortran/os2/rexx/
<CPCMD> <SRCDIR>/f77/samples/mtdll/*                <OWRELROOT>/samples/fortran/mtdll/
<CPCMD> <SRCDIR>/f77/samples/mthread/mthrdos2.for   <OWRELROOT>/samples/fortran/os2/mthread.for
<CPCMD> <SRCDIR>/f77/samples/mthread/makefile.os2   <OWRELROOT>/samples/fortran/os2/makefile
<CPCMD> <SRCDIR>/f77/samples/mthread/mthrdnt.for    <OWRELROOT>/samples/fortran/win32/mthread.for
<CPCMD> <SRCDIR>/f77/samples/mthread/makefile.nt    <OWRELROOT>/samples/fortran/win32/makefile
<CPCMD> <SRCDIR>/f77/samples/win/*                  <OWRELROOT>/samples/fortran/win/
<CPCMD> <SRCDIR>/f77/samples/windll/*               <OWRELROOT>/samples/fortran/win/dll/
<CPCMD> <SRCDIR>/f77/samples/dll/*                  <OWRELROOT>/samples/fortran/os2/dll/
<CPCMD> <SRCDIR>/f77/samples/dll/*                  <OWRELROOT>/samples/fortran/win32/dll/
<CPCMD> <SRCDIR>/f77/samples/dll/os2/makefile       <OWRELROOT>/samples/fortran/os2/dll/
<CPCMD> <SRCDIR>/f77/samples/dll/win32/makefile     <OWRELROOT>/samples/fortran/win32/dll/

[ BLOCK <1> clean ]
#==================
    pmake -d build <2> <3> <4> <5> <6> <7> <8> <9> -h clean

[ BLOCK . . ]
#============

cdsay <PROJDIR>

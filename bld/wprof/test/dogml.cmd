@echo off
rem g:\gml\gmlchk\cdoc2\lr
call ..\share\doprep ps
pkunzip -d langref
set gmlinc=gml;src;%gmlinc%
set kgmlinc=gml;src;%kgmlinc%
set xtra=pass 1
set gmllib=%libdir%\syslib
wsmpos22 /r=50 wgml clib_dos ( file %dpref%%dtype% %docwait% %xtra% output %xfn%*.new
set path=%pathtmp%
set pathtmp=
cd gml
del *.gml
cd ..
rd gml
cd src
del *.gml
del *.msg
cd ..
rd src

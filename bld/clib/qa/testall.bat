@echo off
if exist _???_.log erase _???_.log
pmake -b all -h runtests
call tmp.bat
erase tmp.bat
if exist _os2_.log copy _os2_.log all_os2.log
if exist _os2_.log erase _os2_.log
if exist _dos_.log copy _dos_.log all_dos.log
if exist _dos_.log erase _dos_.log
if exist _wnt_.log copy _wnt_.log all_wnt.log
if exist _wnt_.log erase _wnt_.log

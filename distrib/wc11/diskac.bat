rem --------------- C/C++ --------------- 
call startit
rem set destdir=q:\wc11.axp
set destdir=f:\wc11.axp
call ..\MAKEDISK.BAT ac 1.4 %destdir%
copy make.log wc11_axp.log
copy mkdisk.lst mkdisk.c
copy readme.105 %destdir%\readme.txt
wtouch /f %destdir%\setup.inf %destdir%\readme.txt
md %destdir%\win32s
xcopy /s f:\bld\w32sdk\winsys\*.* %destdir%\win32s

rem ---------------FORTRAN--------------- 
call startit
rem set destdir=q:\wf11
set destdir=f:\wf11
call ..\MAKEDISK.BAT f77 1.4 %destdir%
copy make.log wf11.log
copy mkdisk.lst mkdisk.f
copy freadme.11 %destdir%\freadme.txt
wtouch /f %destdir%\setup.inf %destdir%\freadme.txt
md %destdir%\win32s
xcopy /s f:\bld\w32sdk\winsys\*.* %destdir%\win32s

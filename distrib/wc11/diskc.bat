rem --------------- C/C++ --------------- 
call startit
rem set destdir=q:\wc11
set destdir=f:\wc11
call ..\MAKEDISK.BAT c 1.4 %destdir%
copy make.log wc11.log
copy mkdisk.lst mkdisk.c
copy readme.11 %destdir%\readme.txt
wtouch /f %destdir%\setup.inf %destdir%\readme.txt
md %destdir%\win32s
xcopy /s f:\bld\w32sdk\winsys\*.* %destdir%\win32s
md %destdir%\diskimgs\vp
md %destdir%\diskimgs\vp\disk1
md %destdir%\diskimgs\vp\disk2
md %destdir%\diskimgs\vp\disk3
xcopy f:\bld\wvp\cd_setup\disk1\*.* %destdir%\diskimgs\vp\disk1 /s
xcopy f:\bld\wvp\cd_setup\disk2\*.* %destdir%\diskimgs\vp\disk2 /s
xcopy f:\bld\wvp\cd_setup\disk3\*.* %destdir%\diskimgs\vp\disk3 /s

rem --------------- JAPANESE C/C++ --------------- 
call startit
set destdir=%destdir%
call ..\MAKEDISK.BAT jc 1.4 %destdir%
copy make.log wjc105.log
copy mkdisk.lst mkdisk.jc
copy jreadme.105 %destdir%\readme.txt
wtouch /f %destdir%\setup.inf %destdir%\readme.txt
xcopy f:\vp %destdir%\diskimgs\vp\ /s
md %destdir%\win32sj
xcopy /s i:\win32sj\*.* %destdir%\win32sj
copy f:\vpj\setupj1.inf %destdir%\diskimgs\vp\disk1\setup\setup.inf
copy f:\vpj\setupj2.inf %destdir%\diskimgs\vp\disk2\setup\setup.inf
copy f:\vpj\setupj3.inf %destdir%\diskimgs\vp\disk3\setup\setup.inf
copy f:\vpj\setupj4.inf %destdir%\diskimgs\vp\disk4\setup\setup.inf
copy f:\vpj\wvpj.ini %destdir%\diskimgs\vp\disk1\product\wvpj.ini

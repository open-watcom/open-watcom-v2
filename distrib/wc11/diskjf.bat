rem ---------------JAPANESE FORTRAN--------------- 
call startit
set destdir=q:\wjf10.5
call ..\MAKEDISK.BAT jf77 1.4 %destdir%
copy make.log wjf105.log
copy mkdisk.lst mkdisk.jf
copy jfreadme.105 %destdir%\freadme.txt
wtouch /f %destdir%\setup.inf %destdir%\freadme.txt
md %destdir%\win32sj
xcopy /s i:\win32sj\*.* %destdir%\win32sj

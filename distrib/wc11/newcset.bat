set destdir=q:\wc11
xcopy csetup\*.exe %destdir%
xcopy csetup\*.sym %destdir%
wtouch /f %destdir%\setup.inf %destdir%\*.exe
rem xcopy csetup\*.exe %destdir%\diskimgs\disk01
rem wtouch /f %destdir%\setup.inf %destdir%\diskimgs\disk01\*.exe

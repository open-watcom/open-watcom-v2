set destdir=f:\wc11.axp
xcopy csetup\*.exe %destdir%
wtouch /f %destdir%\setup.inf %destdir%\*.exe
xcopy csetup\*.exe %destdir%\diskimgs\disk01
wtouch /f %destdir%\setup.inf %destdir%\diskimgs\disk01\*.exe

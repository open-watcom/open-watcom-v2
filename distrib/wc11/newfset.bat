set destdir=f:\wf11
xcopy f77setup\*.exe %destdir%
xcopy f77setup\*.sym %destdir%
wtouch /f %destdir%\setup.inf %destdir%\*.exe
rem xcopy f77setup\*.exe %destdir%\diskimgs\disk01
rem wtouch /f %destdir%\setup.inf %destdir%\diskimgs\disk01\*.exe

set destdir=f:\wc11.axp
call startit
watfile/bye (t:200)..\supp\mkdisk.pgm [%DCTRL%] [ac]
mkdisk %DMKOPT%        -i%DINCL% ac 1.4 mkdisk.lst .\pack %DROOT%
copy setup.inf %destdir%\setup.inf
wtouch /f %destdir%\setup.exe %destdir%\setup.inf 
rem copy %destdir%\setup.inf %destdir%\DISKIMGS\DISK01 

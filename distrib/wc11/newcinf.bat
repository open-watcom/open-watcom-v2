set destdir=f:\wc11
call startit
watfile/bye (t:200)..\supp\mkdisk.pgm [%DCTRL%] [c]
mkdisk %DMKOPT%        -i%DINCL% c 1.4 mkdisk.lst .\pack %DROOT%
copy setup.inf %destdir%\setup.inf
wtouch /f %destdir%\setup.exe %destdir%\setup.inf 
copy %destdir%\setup.inf %destdir%\DISKIMGS\DISK01 

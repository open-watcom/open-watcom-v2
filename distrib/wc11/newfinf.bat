call startit
set destdir=f:\wf11
watfile/bye (t:200)..\supp\mkdisk.pgm [%DCTRL%] [f77]
mkdisk %DMKOPT%        -i%DINCL% f77 1.4 mkdisk.lst .\pack %DROOT%
copy setup.inf %destdir%\setup.inf
wtouch /f %destdir%\setup.exe %destdir%\setup.inf 
copy %destdir%\setup.inf %destdir%\DISKIMGS\DISK01 

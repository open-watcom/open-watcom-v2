[ LOG packit.log ]
[ INCLUDE startit.ctl ]

[ BLOCK .<1> . ]
     set 1=*
     
[ BLOCK .<rel> . ]
    set rel=<relroot>\rel2

[ BLOCK . . ]    
    %SCAN% %rel% /nodda
    
   not ready for prime time yet
rem **********************
if [%1] == [] goto usage
if [%DROOT%] == [] goto nodstart
if [%DCTRL%] == [] goto nodstart
if [%DDATE%] == [] goto nodstart
if [%DTIME%] == [] goto nodstart
set TIMESTAMP=
if not [%DDATE%] == [.] if not [%DTIME%] == [.] set TIMESTAMP=-t%DDATE% %DTIME%
if exist dopack.bat del dopack.bat 
watfile/bye (t:200)..\supp\mkpack.pgm [%DCTRL%] [.\pack] [%DROOT%] [%TIMESTAMP%] [%1] [%2] [%3] [%4] [%5] | tee pack.log
if not exist dopack.bat goto end
if exist pack\*.* fastdel pack
mkdir pack
set TIMESTAMP=
@echo on
call dopack.bat | tee -a pack.log
@echo off
goto end
:nodstart
echo.
echo **********************************
echo ** You need to run DSTART first **
echo **********************************
echo.
:usage
echo PACKFILE versions
echo     "versions"   list of versions to pack
echo		      "*" means all versions
:end

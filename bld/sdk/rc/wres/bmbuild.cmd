rem -------------------------------
rem WRES resource reading libraries
rem -------------------------------

rem -- DOS/4G, OS/2 2.x-3.x, Win386, NT --
cd of
wmake -h NO_PRAGMA=1
cd ..

rem -- DOS Large Model--
cd ol
wmake -h NO_PRAGMA=1
cd ..

rem -- DOS Huge Model--
cd oh
wmake -h NO_PRAGMA=1
cd ..

rem -- DOS Compact Model--
cd oc
wmake -h NO_PRAGMA=1
cd ..

rem -- DOS Small Model--
cd os
wmake -h NO_PRAGMA=1
cd ..

rem -- DOS, OS/2 1.x Windows, Medium Model--
cd om
wmake -h NO_PRAGMA=1
cd ..

rem -- Windows Large Model--
cd olw
wmake -h NO_PRAGMA=1
cd ..

rem -- Netware --
cd ofs
wmake -h NO_PRAGMA=1
cd ..

rem -- QNX --
cd osqnx
wmake -h NO_PRAGMA=1 MAINTAIN_SR=1
cd ..

copy lib\*.lib %libroot%\wres\lib

set uidir=\src\ui
cd %uidir%\dos\omw
wmake /h
cd %uidir%\dos\olw
wmake /h
cd %uidir%\dos\ofw
wmake /h
cd %uidir%\os2\ofw
wmake /h
cd %uidir%\nlm\oswn
wmake /h
cd %uidir%\nt\ofw
wmake /h
rem
rem QNX
rem Note that the INCLUDE environment variable has to change to get the
rem QNX headers.
rem
set saveinc=%include%
set include=%watcomdir%\h;%watcom%\qh
cd %uidir%\qnx\os32q
wmake /h /k
cd %uidir%
set include=%saveinc%
set saveinc=

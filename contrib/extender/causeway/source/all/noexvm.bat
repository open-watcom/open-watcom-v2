erase cwnoexvm.exe
nmake /a /f noexvmmk all
copy /b cw32.exe+.\cw\cw.cw cwev.exe
ren cw32.exe cwnoexvm.exe

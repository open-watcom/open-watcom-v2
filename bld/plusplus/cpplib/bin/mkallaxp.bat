@echo off
cd %DEVDIR%\plusplus\cpplib\runtime\ntaxp
call %DEVDIR%\plusplus\cpplib\bin\mallaxp %1 %2
cd %DEVDIR%\plusplus\cpplib\complex\ntaxp
call %DEVDIR%\plusplus\cpplib\bin\mallaxp %1 %2
cd %DEVDIR%\plusplus\cpplib\iostream\ntaxp
call %DEVDIR%\plusplus\cpplib\bin\mallaxp %1 %2
cd %DEVDIR%\plusplus\cpplib\string\ntaxp
call %DEVDIR%\plusplus\cpplib\bin\mallaxp %1 %2
cd %DEVDIR%\plusplus\cpplib\contain\ntaxp
call %DEVDIR%\plusplus\cpplib\bin\mallaxp %1 %2
cd %DEVDIR%\plusplus\cpplib\ntaxp
call %DEVDIR%\plusplus\cpplib\bin\mallaxp %1 %2
cd %DEVDIR%\plusplus\cpplib\bin

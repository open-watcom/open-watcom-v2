@echo off
echo Creating DOS/32 Package
echo *************************************
echo.

cd %dos32a%\src\dos32a
call make.bat

cd %dos32a%\src\sb
call make.bat

cd %dos32a%\src\sc
call make.bat

cd %dos32a%\src\sd
call make.bat

cd %dos32a%\src\sd\sdlib
call make.bat


cd %dos32a%\src\ss
call make.bat

cd %dos32a%\src\stub32a
call make.bat

cd %dos32a%\src\sutils\build
call make.bat

cd %dos32a%\src\sutils\pctest
call make.bat

cd %dos32a%\src\sutils\sver
call make.bat


cd %dos32a%\src
@echo off
if [.%1] == [.] goto help
if [.%2] == [.] goto help
call pass1 %1 %2
call pass2 %1 %2
goto done
:help
echo usage: convert path_to_h path_to_fi
echo for example: convert h\ fi\
:done

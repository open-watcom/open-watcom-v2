@echo %verbose% off
:: test.cmd
::
:: test Open Watcom rm command   on Winxx and OS/2
::

set PROG=%OWROOT%\bld\posix\src\rm\rm.exe
set FILENAME=dummy
if not %1.==. set PROG=%1
if not exist %PROG% for %%C in (echo goto:failure) do %%C rmtest: "%PROG%" is absent
if exist %FILENAME% dir %FILENAME%
if exist %FILENAME% for %%C in (echo goto:failure) do %%C rmtest: "%FILENAME%" (used as a scratch file) already exists

:: Test Option recognition

%PROG% 2>nul
if not errorlevel 1 for %%C in (echo goto:failure) do %%C rmtest: %PROG% without arguments should fail

%PROG% -? 2>nul
if not errorlevel 1 for %%C in (echo goto:failure) do %%C rmtest: %PROG% -? should fail

%PROG% -f -i -r -R -s -v -X 2>nul
if not errorlevel 1 for %%C in (echo goto:failure) do %%C rmtest: %PROG% without files should fail

echo NUL >%FILENAME%
if not exist %FILENAME% for %%C in (echo goto:failure) do %%C rmtest: failed to create scratch file: "%FILENAME%"

%PROG% /q %FILENAME% 2>nul
if not exist %FILENAME% for %%C in (echo goto:failure) do %%C rmtest: invalid option flag ignored

:: Implicitly test DOS option flag
%PROG% /X %FILENAME%
if exist %FILENAME% for %%C in (echo goto:failure) do %%C rmtest: failure of %PROG% /r %FILENAME%

:: Test -r option
mkdir %FILENAME%
echo NUL >%FILENAME%\%FILENAME%
if not exist %FILENAME%\%FILENAME% for %%C in (echo goto:failure) do %%C rmtest: failed to create scratch file: "%FILENAME%\%FILENAME%"

attrib +r %FILENAME%\%FILENAME%
%PROG% -r %FILENAME%
if not exist %FILENAME% for %%C in (echo goto:failure) do %%C rmtest: unforced -r ignored readonly attribute on "%FILENAME%\%FILENAME%"

%PROG% -R %FILENAME%
if not exist %FILENAME% for %%C in (echo goto:failure) do %%C rmtest: unforced -R ignored readonly attribute on "%FILENAME%\%FILENAME%"

%PROG% -rf %FILENAME%
if exist %FILENAME% for %%C in (echo goto:failure) do %%C rmtest: forced -r failed to delete "%FILENAME%"

%PROG% -f %FILENAME%
if errorlevel 1 for %%C in (echo goto:failure) do %%C rmtest: forced removing non existent file should not fail

%PROG%  %FILENAME%
if not errorlevel 1 for %%C in (echo goto:failure) do %%C rmtest: removing non existent file should fail

%PROG% -r %FILENAME%
if not errorlevel 1 for %%C in (echo goto:failure) do %%C rmtest: removing non existent directory should fail

if exist %FILENAME%  attrib -r %FILENAME%
if exist %FILENAME%  del %FILENAME%
if exist %FILENAME% for %%C in (echo goto:failure) do %%C rmtest: failed to delete scratch file: "%FILENAME%"

for %%C in (echo true goto:done) do %%C rmtest: %PROG% OK
:failure
for %%C in (echo false goto:done) do %%C rmtest: %PROG% US
:done

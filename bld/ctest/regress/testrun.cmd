setlocal
set ctest_name=result
wmake -h del_log
if exist *.sav del *.sav
rem
rem figure out if any extra architectures have to be tested
if not [%OS%] == [Windows_NT] goto not_nt
if [%PROCESSOR_ARCHITECTURE%] == [x86] goto add_i86
goto extra_done
:not_nt
rem assume we want to add i86 if not Windows_NT
:add_i86
set extra_arch=i86
:extra_done
cd positive
call testrun.cmd
cd ..
cd diagnose
call testrun.cmd
cd ..
cd callconv
call testrun.cmd
cd ..
cd inline
call testrun.cmd
cd ..
wmake -h clean
wmake -h
wmake -h clean
endlocal

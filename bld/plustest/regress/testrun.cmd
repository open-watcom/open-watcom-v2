title C++ Test Stream
setlocal
set plustest_name=result
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
if [%plustest_i86%] == [no] goto extra_done
set extra_arch=i86
:extra_done
if exist local.cmd call local.cmd
cd arith
call testrun.cmd
cd ..
cd bits
call testrun.cmd
cd ..
cd bits.64
call testrun.cmd
cd ..
cd bitset
call testrun.cmd
cd ..
cd claslist
call testrun.cmd
cd ..
cd contain
call testrun.cmd
cd ..
cd cool
call testrun.cmd
cd ..
cd datainit
call testrun.cmd
cd ..
cd dcmat
call testrun.cmd
cd ..
if exist alt_diag.cmd goto alt_diag
cd diagnose
call testrun.cmd
cd ..
goto did_diag
:alt_diag
call alt_diag.cmd
:did_diag
cd ehmix
call testrun.cmd
cd ..
cd except
call testrun.cmd
cd ..
cd exercise
call testrun.cmd
cd ..
cd extref
call testrun.cmd
cd ..
cd field
call testrun.cmd
cd ..
cd field.64
call testrun.cmd
cd ..
cd format
call testrun.cmd
cd ..
cd getlong
call testrun.cmd
cd ..
cd gstring
call testrun.cmd
cd ..
cd inline
call testrun.cmd
cd ..
cd jam
call testrun.cmd
cd ..
cd nih
call testrun.cmd
cd ..
cd nullptr
call testrun.cmd
cd ..
cd overload
call testrun.cmd
cd ..
cd owstl
call testrun.cmd
cd ..
cd perl
call testrun.cmd
cd ..
cd positive
call testrun.cmd
cd ..
cd preproc
call testrun.cmd
cd ..
cd re2c
call testrun.cmd
cd ..
cd re2c05
call testrun.cmd
cd ..
cd sizelist
call testrun.cmd
cd ..
cd splash
call testrun.cmd
cd ..
cd stepanov
call testrun.cmd
cd ..
cd stl.94
call testrun.cmd
cd ..
cd stl.95
call testrun.cmd
cd ..
cd torture
call testrun.cmd
cd ..
if not [%OS%] == [Windows_NT] goto no_winnt_test
cd winnt
call testrun.cmd
cd ..
:no_winnt_test
cd yacl
call testrun.cmd
cd ..
if [%plustest_i86%] == [no] goto skip_i86_specific
cd wclass
call testrun.cmd
cd ..
:skip_i86_specific
wmake -h clean
wmake -h
wmake -h clean
endlocal

title C++ Test Stream
rem must be early in case refresh refreshes onetest.cmd!
p4 get //depot/plustest/...
setlocal
set plustest_name=result
wmake del_log
del *.sav
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
if exist local.cmd call local.cmd
cd arith
call onetest.cmd
cd ..
cd bits
call onetest.cmd
cd ..
cd bitset
call onetest.cmd
cd ..
cd claslist
call onetest.cmd
cd ..
cd contain
call onetest.cmd
cd ..
cd cool
call onetest.cmd
cd ..
cd datainit
call onetest.cmd
cd ..
cd dcmat
call onetest.cmd
cd ..
if exist alt_diag.cmd goto alt_diag
cd diagnose
call onetest.cmd
cd ..
goto did_diag
:alt_diag
call alt_diag.cmd
:did_diag
cd ehmix
call onetest.cmd
cd ..
cd except
call onetest.cmd
cd ..
cd exercise
call onetest.cmd
cd ..
cd extref
call onetest.cmd
cd ..
cd field
call onetest.cmd
cd ..
cd format
call onetest.cmd
cd ..
cd getlong
call onetest.cmd
cd ..
cd gstring
call onetest.cmd
cd ..
cd jam
call onetest.cmd
cd ..
cd mat
call onetest.cmd
cd ..
cd nacc
call onetest.cmd
cd ..
cd nih
call onetest.cmd
cd ..
cd ns_rand
call onetest.cmd
cd ..
cd preproc_rand
call onetest.cmd
cd ..
cd overload
call onetest.cmd
cd ..
cd pcg
call onetest.cmd
cd ..
cd perl
call onetest.cmd
cd ..
cd positive
call onetest.cmd
cd ..
cd preproc
call onetest.cmd
cd ..
cd re2c
call onetest.cmd
cd ..
cd re2c05
call onetest.cmd
cd ..
cd rogue.60
call onetest.cmd
cd ..
cd sizelist
call onetest.cmd
cd ..
cd splash
call onetest.cmd
cd ..
cd stepanov
call onetest.cmd
cd ..
cd stl.old
call onetest.cmd
cd ..
cd stl.mod
call onetest.cmd
cd ..
cd stl.94
call onetest.cmd
cd ..
cd stl.95
call onetest.cmd
cd ..
cd str
call onetest.cmd
cd ..
cd torture
call onetest.cmd
cd ..
cd var
call onetest.cmd
cd ..
cd wclass
call onetest.cmd
cd ..
cd woop
call onetest.cmd
cd ..
cd yacl
call onetest.cmd
cd ..
wmake -h clean
wmake -h
wmake -h clean
endlocal

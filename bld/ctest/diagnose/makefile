ctest_name = diagnose

alt_error=00

!include testenv1.mif
!include ../diagnose.mif

diagnose_opts += -we -wx -e1000 -eq
archdep_opts = -dARCH=$(arch) -fr=.ser

.c: source

test : .symbolic start clean start_test diag0000.out common_set_result_check
    %make clean

check_result : .procedure
    -diff -b diag0000.out diag0000.chk >diff0000.out
    -diff -b diff0000.out diff0000.chk

diag0000.out : ./catfile.exe $(diags_0099)
    $[@ -a *.err | sed -f source/fixpath.sed >diag0000.out

start_test : .symbolic
    @if exist s$(arch)0000.sav rm s$(arch)0000.sav

save : .symbolic
    @if exist diag0000.out cp diag0000.out s$(arch)0000.sav

clean : .procedure
    @%make echo_clean
    @%make common_clean
    @if exist *.ser $(noecho)rm *.ser

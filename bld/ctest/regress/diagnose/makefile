ctest_name = diagnose

!include testenv1.mif
!include source/diagnose.mif

diagnose_opts += -we -wx -e1000 -eq
archdep_opts = -dARCH=$(arch) -fr=.ser
alt_error=00

.c: source

test : .symbolic start clean start_test diag0000.out
    @set ERROR_FILE=error.out
    @set ERROR_MSG=
    -diff -b diag0000.out diag0000.chk >diff0000.out
    diff -b diff0000.out diff0000.chk
    %append $(log_file) PASS $(%__CWD__)
    %make clean

diag0000.out : ./catfile.exe $(diags_0099)
    $[@ -a *.err | sed -f fixpath.sed >diag0000.out

start_test : .symbolic
    @if exist s$(arch)0000.sav rm s$(arch)0000.sav

save : .symbolic
    @if exist diag0000.out cp diag0000.out s$(arch)0000.sav

clean : .procedure
    @if exist *.ser rm *.ser
    @%make common_clean

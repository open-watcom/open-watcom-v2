ctest_name = diagnose

alt_error=00

!include testenv.mif
!include ../diagnose.mif

diagnose_opts += -we -wx -e1000 -eq
archdep_opts = -dARCH=$(arch)

tests = test00

.c: source

test : .symbolic start $(tests)
    $(noecho)%make common_set_result

test00 : .symbolic ./catfile.exe $(diags_0099)
    $[@ -a *.err | sed -f source/fixpath.sed >>exec.out
    @%make diag_clean

diag_clean : .procedure
    @if exist *.err $(noecho)rm -f *.err
    @if exist *.ser $(noecho)rm -f *.ser

clean : .procedure
    @%make echo_clean
    @%make diag_clean
    @%make common_clean

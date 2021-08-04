plustest_name = diagnose

alt_error=00

exec_chk_i86 = diagi86.chk
exec_chk_386 = diag386.chk
exec_chk_axp = diagaxp.chk

!include testenv.mif
!include source/diagnose.mif

diagnose_opts += -ew -we -wx -e1000 -eq
archdep_opts = -dARCH=$(arch)
fix_source_path=sed -f fixpath.sed

tests = test00 test01

.c: source

test : .symbolic start $(tests)
    $(noecho)%make common_set_result

test00 : .symbolic ./catfile.exe $(diags_0099) $(diags_0199) $(diags_0299) $(diags_0399) $(diags_sys_$(arch))
    $[@ -a *.err | $(fix_source_path) >>exec.out
    $[@ -a *.ser | $(fix_source_path) >>exec.out
    @%make diag_clean

test01 : .symbolic source/diag0001.c
    @%make set_path_wpp
    -$(wpp_$(arch)) $[@ $(diagnose_opts) $(opt_diag0001)
    @%make set_old_path
    @$(fix_source_path) diag0001.err >>exec.out

diag_clean : .procedure
    @if exist *.err $(noecho)rm -f *.err
    @if exist *.ser $(noecho)rm -f *.ser
    @if exist *.pch $(noecho)rm -f *.pch

clean : .procedure
    @%make echo_clean
    @%make diag_clean
    $(noecho)%make common_clean

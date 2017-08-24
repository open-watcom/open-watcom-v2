plustest_name = diagnose

alt_error=00

!include testenv1.mif
!include source/diagnose.mif

diagnose_opts += -ew -we -wx -e1000 -eq
archdep_opts = -dARCH=$(arch) -fr=.ser
fix_source_path=sed -f fixpath.sed

# The check files for x86 are a bit different.
!ifeq arch i86
diagchk0 = di860000.chk
diagchk1 = di860100.chk
diagchk2 = di860200.chk
diagchk3 = di860300.chk
!else ifeq arch 386
diagchk0 = d3860000.chk
diagchk1 = d3860100.chk
diagchk2 = d3860200.chk
diagchk3 = d3860300.chk
dosystest = testsys
!else ifeq arch axp
diagchk0 = daxp0000.chk
diagchk1 = daxp0100.chk
diagchk2 = daxp0200.chk
diagchk3 = daxp0300.chk
dosystest = testsys
!endif

.c: source

test : .symbolic start clean start_test test0000 test0100 test0200 test0300 $(dosystest) test_special set_result_check
    @%make clean

check_result : .procedure
    -diff -b diff.out diff.chk >$(nulldevice)

test0000 : .symbolic diag0000.out
    -diff -b diag0000.out $(diagchk0) >diff0000.out
    -diff -b diff0000.out diff.chk >>diff.out
    @rm diff0000.out

test0100 : .symbolic diag0100.out
    -diff -b diag0100.out $(diagchk1) >diff0100.out
    -diff -b diff0100.out diff.chk >>diff.out
    @rm diff0100.out

test0200 : .symbolic diag0200.out
    -diff -b diag0200.out $(diagchk2) >diff0200.out
    -diff -b diff0200.out diff.chk >>diff.out
    @rm diff0200.out

test0300 : .symbolic diag0300.out
    -diff -b diag0300.out $(diagchk3) >diff0300.out
    -diff -b diff0300.out diff.chk >>diff.out
    @rm diff0300.out

testsys : .symbolic diag$(arch).out
    -diff -b diag$(arch).out diag$(arch).chk >diff$(arch).out
    -diff -b diff$(arch).out diff.chk >>diff.out
    @rm diff$(arch).out

diagspec.out : .procedure source/diag0001.c
    @%make set_path_wpp
    -$(wpp_$(arch)) $[@ $(diagnose_opts) $(opt_diag0001)
    @%make set_old_path
    @$(fix_source_path) diag0001.err >diagspec.out

test_special : .symbolic diagspec.out
    -diff -b diagspec.out diagspec.chk >diffspec.out
    -diff -b diffspec.out diff.chk >>diff.out
    @rm diffspec.out

diag0000.out : ./catfile.exe $(diags_0099)
    $[@ -a *.err | $(fix_source_path) >diag0000.out
    @%make diag_clean

diag0100.out : ./catfile.exe $(diags_0199)
    $[@ -a *.err | $(fix_source_path) >diag0100.out
    @%make diag_clean

diag0200.out : ./catfile.exe $(diags_0299)
    $[@ -a *.err | $(fix_source_path) >diag0200.out
    @%make diag_clean

diag0300.out : ./catfile.exe $(diags_0399)
    $[@ -a *.err | $(fix_source_path) >diag0300.out
    @%make diag_clean

diag386.out : ./catfile.exe $(diags_sys)
    $[@ -a *.ser | $(fix_source_path) >diag386.out
    @%make diag_clean

diagaxp.out : ./catfile.exe $(diags_sys)
    $[@ -a *.ser | $(fix_source_path) >diagaxp.out
    @%make diag_clean

diag_clean : .procedure
    @if exist *.err $(noecho)rm -f *.err
    @if exist *.ser $(noecho)rm -f *.ser
    @if exist *.pch $(noecho)rm -f *.pch

start_test : .symbolic
    @if exist diff.out $(noecho)rm diff.out
    $(noecho)%make clean_sav

save_sav : .procedure
    @if exist diag0000.out @cp diag0000.out s$(arch)0000.sav >$(nulldevice)
    @if exist diag0100.out @cp diag0100.out s$(arch)0100.sav >$(nulldevice)
    @if exist diag0200.out @cp diag0200.out s$(arch)0200.sav >$(nulldevice)
    @if exist diag0300.out @cp diag0300.out s$(arch)0300.sav >$(nulldevice)
    @if exist diag$(arch).out @cp diag$(arch).out s$(arch)_$(arch).sav >$(nulldevice)
    @if exist diagspec.out @cp diagspec.out s$(arch)spec.sav >$(nulldevice)
    #
    @if exist diag0000.out @rm diag0000.out
    @if exist diag0100.out @rm diag0100.out
    @if exist diag0200.out @rm diag0200.out
    @if exist diag0300.out @rm diag0300.out
    @if exist diag$(arch).out @rm diag$(arch).out
    @if exist diagspec.out @rm diagspec.out

clean_sav : .procedure
    @if exist s$(arch)0000.sav @rm s$(arch)0000.sav
    @if exist s$(arch)0100.sav @rm s$(arch)0100.sav
    @if exist s$(arch)0200.sav @rm s$(arch)0200.sav
    @if exist s$(arch)0300.sav @rm s$(arch)0300.sav
    @if exist s$(arch)_$(arch).sav @rm s$(arch)_$(arch).sav
    @if exist s$(arch)spec.sav @rm s$(arch)spec.sav

set_pass : .procedure
    $(noecho)%make clean_sav
    @set ERROR_MSG=PASS $(%__CWD__)

clean : .procedure
    @%make echo_clean
    @%make diag_clean
    $(noecho)%make common_clean

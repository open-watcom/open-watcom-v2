ctest_name = diagnose
!include ../environ.mif

diagnose_dir = source

!include $(diagnose_dir)/diagnose.mif

diagnose_opts += -we -wx -e1000 -eq -zq
archdep_opts = -dARCH=$(arch) -fr=.ser
alt_error=00

.c: $(diagnose_dir)
.c.err:
    -$(wcc_$(arch)) $< $(diagnose_opts) $(opt_$^&) -fo=.obj
    @if not exist $@ %write $@ I did not generate errors for $^&
.c.ser:
    -$(wcc_$(arch)) $< $(diagnose_opts) $(opt_$^&) $(archdep_opts) -fo=.obj
    @if not exist $@ %write $@ I did not generate errors for $^&

test : .symbolic start_test diag0000.out
    -diff -b diag0000.out diag0000.chk >diff0000.out
    diff -b diff0000.out diff0000.chk
    %make global
    %append $(log_file) PASS $(%__CWD__)

./catfile.exe: source/catfile.c
    $(bld_cl) $< $(wcl_util_opts)

diag0000.out : ./catfile.exe $(diags_0099)
    $[@ -a *.err | sed -f fixpath.sed >diag0000.out

start_test : .symbolic
    @if exist s$(arch)0000.sav rm s$(arch)0000.sav

save : .symbolic
    @if exist diag0000.out cp diag0000.out s$(arch)0000.sav

global : .symbolic
    @if exist *.ser rm *.ser
    @if exist catfile.* rm catfile.*
    @%make common_clean

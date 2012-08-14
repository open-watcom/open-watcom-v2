ctest_name = inline

!include ../environ.mif
!include inline.mif

inline_dir = $(wasmtest_dir)/inline

!ifdef test1
test_cflags_386 =
test_cflags_i86 =
alt_error = 01
!else
test_cflags_386 =
test_cflags_i86 =
alt_error = 00
!endif

!ifdef __LINUX__
exec_prefix = ./
!else
exec_prefix =
!endif

!ifndef noisy
run_quietly = -zq
!endif

test_cflags_386 += -s -os -wx -we $(run_quietly)
test_cflags_i86 += -s -os -wx -we $(run_quietly)

.error
    @%append $(%error_file) $(%error_msg)

.c: $(inline_dir)
.asm: $(inline_dir)

.asm.gc:
    @$(awk) -v target=$(init_$^&) -f $(wasmtest_dir)/createc.awk $[@ >$@

.gc.obj:
    @set error_msg=failure to compile $^&
    $(wcc_$(arch)) $[@ $(test_cflags_$(arch)) $(opt_$^*) -fo=.obj

.c.obj:
    @set error_msg=failure to compile $^&
    $(wcc_$(arch)) $[@ $(test_cflags_$(arch)) $(opt_$^*) -fo=.obj

.obj.ls1:
    @set error_msg=failure to decode $[@
    wdis -a $[@ | $(sed) -f $(wasmtest_dir)/fixlst2.sed | $(awk) -f $(wasmtest_dir)/fixlst3.awk >$@
    @set CHK_CHECK=diff -i -b $@ $(inline_dir)/$^&.asm
    @if exist $(inline_dir)/$^&.chk @set CHK_CHECK=diff -i -b $@ $(inline_dir)/$^&.chk
    $(%CHK_CHECK)
    @%append exec.out PASS decoding $[@

.obj.lst:
    @set error_msg=failure to decode $[@
    @set ESM_CHECK=@%null
    @if exist $(inline_dir)/$^&.esm @set ESM_CHECK=diff -i -b $^&.dis $(inline_dir)/$^&.esm
    -wdis -a $[@ | $(sed) -f $(wasmtest_dir)/fixlst2.sed $(extra_sed_$^*) >$^&.dis
    $(%ESM_CHECK)
    @set EMP_CHECK=@%null
    @if exist $(inline_dir)/$^&.emp @set EMP_CHECK=diff -i -b $@ $(inline_dir)/$^&.emp
    -dmpobj -q -rec=FIXUPP $[@ | $(sed) -f $(wasmtest_dir)/fixlst1.sed >$@
    $(%EMP_CHECK)
    @%append exec.out PASS decoding $[@

./chk_exec.exe : ../../c/chk_exec.c
    $(bld_cl) -DINLINETEST $< $(wcl_util_opts)

test : .symbolic start_test $($(arch)_inline_objs) ./chk_exec.exe
    @set error_file=error.out
    @set error_msg=failure to complete test stream
    %write error.out OK
    $]@ exec.out >>error.out
    diff -b error.out error.chk
    %make global
    %append $(log_file) PASS $(%__CWD__)

start_test : .symbolic
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav
    %create exec.out
    @set error_file=exec.out
!ifndef $(arch)_inline_objs
    @%append exec.out PASS decoding
!endif

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

global : .symbolic
    @if exist *.gc rm -f *.gc
    @if exist *.ls1 rm -f *.ls1
    @if exist *.dis rm -f *.dis
    @if exist failure rm failure
    @%make common_clean

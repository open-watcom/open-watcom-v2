ctest_name = positive
!include ../environ.mif

positive_dir = source

!include $(positive_dir)/positive.mif

!ifdef test1
test_cflags_386 = -d2-mf
test_cflags_i86 = -d2-mc-2
test_cflags_axp = -d2-si
add_br_ = $(br_opt)
alt_error = 01
!else ifdef test2
test_cflags_386 = -d1-oxtan-mf
test_cflags_i86 = -d1-oxtan-mm
test_cflags_axp = -d1-oxtan-si
alt_error = 02
!else ifdef test3
test_cflags_386 = -d2-ms
test_cflags_i86 = -d2-ml
test_cflags_axp = -d2
alt_error = 03
!else ifdef test4
test_cflags_386 = -d1-oxs-mf
test_cflags_i86 = -d1-oxs-ms
test_cflags_axp = -d1-oxs-si
alt_error = 04
!else ifdef test5
test_cflags_386 = -d1-ei-mf
test_cflags_i86 = -d1-ei-mc-2
test_cflags_axp = -d1-ei
alt_error = 05
!else ifdef test6
test_cflags_386 = -d2-mf
test_cflags_i86 = -d2-mm
test_cflags_axp = -d2
alt_error = 06
!else ifdef test7
test_cflags_386 = -d1-ms-6s-fp6
test_cflags_i86 = -d1-ml-3-fp3
test_cflags_axp = -d1
alt_error = 07
!else ifdef test8
test_cflags_386 = -d2-mf-zro
test_cflags_i86 = -d2-ms-zro
test_cflags_axp = -d2
alt_error = 08
!else ifdef test9
test_cflags_386 = -d1-oe-zri-mf
test_cflags_i86 = -d1-oe-ms
test_cflags_axp = -d1-oe
alt_error = 09
!else
test_cflags_386 = -ox-d1-ms
test_cflags_i86 = -ox-d1-ms
test_cflags_axp = -ox-d1
alt_error = 00
add_br_ = $(br_opt)
!endif

!ifdef __LINUX__
exec_prefix = ./
!else
exec_prefix =
!endif

!ifndef noisy
run_quietly = -zq
!endif

test_cflags_386 += -we-w0 -I"$(positive_dir)" $(run_quietly)
test_cflags_i86 += -we-w0 -I"$(positive_dir)" $(run_quietly)
test_cflags_axp += -we-w0 -I"$(positive_dir)" $(run_quietly)
test_cflags_ppc += -we-w0 -I"$(positive_dir)" $(run_quietly)
test_cflags_mps += -we-w0 -I"$(positive_dir)" $(run_quietly)

!ifeq arch 386
test_cflags_386 += $(add_br_$(no_br_$^*))
!endif

.error
    @%append $(%error_file) $(%error_msg)

.c: $(positive_dir)
.c.obj:
    @set error_msg=failure to compile $^@
    $(wcc_$(arch)) $[@ $(test_cflags_$(arch)) $(opt_$^*) $(opt_EH_$(EH_$^*)) -fo=.obj

# Rule for compiling via preprocessor
.EXTENSIONS: .p

.p: $(positive_dir)
.p.obj:
    @set error_msg=failure to preprocess $^@
    $(wcc_$(arch)) $[@ $(test_cflags_$(arch)) $(opt_$^*) $(opt_EH_$(EH_$^*)) -pl -fo=.i
    @set error_msg=failure to compile $[&.i
    $(wcc_$(arch)) $[&.i $(test_cflags_$(arch)) $(opt_$^*) $(opt_EH_$(EH_$^*)) -fo=.obj
    rm $[&.i

.obj.$(exe):
    @set error_msg=failure to link $^@
    $(linker) @test.lnk $(opt_link_$^*) FILE $[@ NAME $^@
    @set error_msg=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $(exec_prefix)$^@ >>exec.out
    rm $[@
#    $(wstrip) -q $^@

./chk_exec.exe : ../../c/chk_exec.c
    $(bld_cl) $< $(wcl_util_opts)

test : .symbolic start_test $(positive_exes) ./chk_exec.exe
    @set error_file=error.out
    @set error_msg=failure to complete test stream
    %write error.out OK
    $]@ exec.out >> error.out
    diff -b error.out error.chk
    %make global
    %append $(log_file) PASS $(%__CWD__)

fnstat1.$(exe) : fnstat1a.obj fnstat1b.obj
    @set error_msg=failure to link $^@
    $(linker) @test.lnk NAME $^@ FILE $[@, $]@
    @set error_msg=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $(exec_prefix)$^@ >>exec.out

start_test : .symbolic test.lnk
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav
    %create exec.out
    @set error_file=exec.out

test.lnk : makefile
    %create $^@
    @%append $^@ $(ldebug_$(arch))
    @%append $^@ $(lnk_$(arch))
    @%append $^@ OPTION noredef

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

global : .symbolic
    @if exist failure rm failure
    @%make common_clean

ctest_name = callconv

callconv_dir = source

!include ../environ.mif
!include $(callconv_dir)/callconv.mif

!ifdef test1
test_cflags_386 = -d2-mf
test_cflags_i86 = -d2-mc-2
test_cflags_axp = -d2-si
alt_error = 01
!else ifdef test2
test_cflags_386 = -d1-otan-mf
test_cflags_i86 = -d1-otan-mm
test_cflags_axp = -d1-otan-si
alt_error = 02
!else ifdef test3
test_cflags_386 = -d2-ms
test_cflags_i86 = -d2-ml
test_cflags_axp = -d2
alt_error = 03
!else ifdef test4
test_cflags_386 = -d1-os-mf
test_cflags_i86 = -d1-os-ms
test_cflags_axp = -d1-os-si
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
test_cflags_386 = -d1-ms
test_cflags_i86 = -d1-ms
test_cflags_axp = -d1
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

test_cflags_386 += -we-w0 -I$(callconv_dir) $(run_quietly)
test_cflags_i86 += -we-w0 -I$(callconv_dir) $(run_quietly)
test_cflags_axp += -we-w0 -I$(callconv_dir) $(run_quietly)
test_cflags_ppc += -we-w0 -I$(callconv_dir) $(run_quietly)
test_cflags_mps += -we-w0 -I$(callconv_dir) $(run_quietly)

!ifeq arch 386
test_cflags_386 += $(add_br_$(no_br_$^*))
!endif

.error
    @%append $(%ERROR_FILE) $(%ERROR_MSG)

.c: $(callconv_dir)
.c.obj:
    @set ERROR_MSG=failure to compile $^@
    $(wcc_$(arch)) $[@ $(test_cflags_$(arch)) $(opt_$^*) $(opt_EH_$(EH_$^*)) -fo=.obj

.obj.$(exe):
    @set ERROR_MSG=failure to link $^@
    $(linker) @test.lnk $(opt_link_$^*) FILE $[@ NAME $^@
    @set ERROR_MSG=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $(exec_prefix)$^@ >>exec.out

./chk_exec.exe : ../../c/chk_exec.c
    $(bld_cl) $< $(wcl_util_opts)

test : .symbolic start_test $(callconv_exes) ./chk_exec.exe
    @set ERROR_MSG=symbol name failure
    diff -b symbols.out sym$(arch)$(alt_error).chk
    @set ERROR_FILE=error.out
    @set ERROR_MSG=failure to complete test stream
    %write error.out OK
    $]@ exec.out >>error.out
    diff -b error.out error.chk
    @%make global
    %append $(log_file) PASS $(%__CWD__)

fnstat1.$(exe) : fnstat1a.obj fnstat1b.obj
    @set ERROR_MSG=failure to link $^@
    $(linker) @test.lnk NAME $^@ FILE $[@, $]@
    @set ERROR_MSG=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $(exec_prefix)$^@ >>exec.out

start_test : .symbolic test.lnk
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav
    @if exist x$(arch)_$(alt_error).sav rm x$(arch)_$(alt_error).sav
    %create exec.out
    %create symbols.out
    @set ERROR_FILE=exec.out

test.lnk : $(__MAKEFILE__)
    %create $^@
    @%append $^@ $(ldebug_$(arch))
    @%append $^@ $(lnk_$(arch))
    @%append $^@ OPTION noredef

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav
    @if exist symbols.out cp symbols.out x$(arch)_$(alt_error).sav

global : .symbolic
    @%make common_clean

$(all_objs) : $(callconv_dir)/obj.c
    @set ERROR_MSG=failure to compile $^@
    $(wcc_$(arch)) $[@ $(test_cflags_$(arch)) $(opt_$^*) -fo=$^@
    %append symbols.out **** $^@ pubdef ****
    objxdef $^@ >>symbols.out
    %append symbols.out **** $^@ extdef ****
    objxref $^@ >>symbols.out

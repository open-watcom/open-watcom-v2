plustest_name = positive
!include ../environ.mif

positive_dir = source

!include $(positive_dir)/positive.mif

!ifdef test1
test_cflags_386 = -d2-xss-mf
test_cflags_i86 = -d2-xss-mc-2
test_cflags_axp = -d2-xss-si
opt_EH_yes = -xss
add_br_ = $(br_opt)
alt_error = 01
!else ifdef test2
test_cflags_386 = -d1-xss-mf
test_cflags_i86 = -d1-xss-mm
test_cflags_axp = -d1-xss-si
opt_EH_yes = -xss
alt_error = 02
!else ifdef test3
test_cflags_386 = -d2-xst-ms-zfp
test_cflags_i86 = -d2-xst-ml
test_cflags_axp = -d2-xst
opt_EH_yes = -xst
alt_error = 03
!else ifdef test4
test_cflags_386 = -d1-xst-mf
test_cflags_i86 = -d1-xst-ms
test_cflags_axp = -d1-xst-si
opt_EH_yes = -xst
alt_error = 04
!else ifdef test5
test_cflags_386 = -d1-xds-mf
test_cflags_i86 = -d1-xds-mc-2
test_cflags_axp = -d1-xds
opt_EH_yes = -xss
alt_error = 05
!else ifdef test6
test_cflags_386 = -d2-xds-mf
test_cflags_i86 = -d2-xds-mm
test_cflags_axp = -d2-xds
opt_EH_yes = -xss
alt_error = 06
!else ifdef test7
test_cflags_386 = -d1-xdt-ms-zfp
test_cflags_i86 = -d1-xdt-ml
test_cflags_axp = -d1-xdt
opt_EH_yes = -xst
alt_error = 07
!else ifdef test8
test_cflags_386 = -d2-xdt-mf
test_cflags_i86 = -d2-xdt-ms
test_cflags_axp = -d2-xdt
opt_EH_yes = -xst
alt_error = 08
!else ifdef test9
test_cflags_386 = -d1-xs-oe-mf
test_cflags_i86 = -d1-xs-oe-ms
test_cflags_axp = -d1-xs-oe
opt_EH_yes = -xs
alt_error = 09
!else ifdef test10
test_cflags_386 = -d2s-xdt-mf
test_cflags_i86 = -d2s-xdt-ms
test_cflags_axp = -d2s-xdt
opt_EH_yes = -xst
alt_error = 10
!else ifdef test11
test_cflags_386 = -d2s-xs-mf
test_cflags_i86 = -d2s-xs-ms
test_cflags_axp = -d2s-xs
opt_EH_yes = -xs
alt_error = 11
!else
test_cflags_386 = -ox-d1-xdt-ms-zfp
test_cflags_i86 = -ox-d1-xdt-ms
test_cflags_axp = -ox-d1-xdt
opt_EH_yes = -xst
alt_error = 00
add_br_ = $(br_opt)
!endif

test_cflags_386 += -we-w0-zq -I"$(positive_dir)"
test_cflags_i86 += -we-w0-zq -I"$(positive_dir)"
test_cflags_axp += -we-w0-zq -I"$(positive_dir)"

!ifeq arch 386
test_cflags_386 += $(add_br_$(no_br_$^*))
!endif

# Some tests require more than the default stack size
lnk_i86 += op stack=8k

.error
    @%append $(%error_file) $(%error_msg)

.c: $(positive_dir)
.c.obj:
    @set error_msg=failure to compile $^@
    $(wpp_$(arch)) $[@ $(test_cflags_$(arch)) $(opt_$^*) $(opt_EH_$(EH_$^*)) -fo=.obj

.obj.$(exe):
    @set error_msg=failure to link $^@
    $(linker) @test.lnk $(opt_link_$^*) NAME $@ FILE $[@
    @set error_msg=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $(exec_prefix)$@ >>exec.out
    rm $[@

test : .symbolic start_test $(positive_exes) ./chk_exec.exe
    @set error_file=error.out
    @set error_msg=failure to complete test stream
    %write error.out OK
    $]@ exec.out >>error.out
    diff -b error.out error.chk
    @%make global
    %append $(log_file) PASS $(%__CWD__)

./chk_exec.exe : chk_exec.c
    $(bld_cl) $< $(wcl_util_opts)

link3.$(exe) : link3a.obj link3b.obj
    @set error_msg=failure to link $^@
    $(linker) @test.lnk NAME $^@ FILE $[@, $]@
    @set error_msg=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $(exec_prefix)$@ >>exec.out

link4.$(exe) : link4a.obj link4b.obj
    @set error_msg=failure to link $^@
    $(linker) @test.lnk NAME $^@ FILE $[@, $]@
    @set error_msg=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $(exec_prefix)$@ >>exec.out

link5.$(exe) : link5a.obj link5b.obj
    @set error_msg=failure to link $^@
    $(linker) @test.lnk NAME $^@ FILE $[@, $]@
    @set error_msg=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $(exec_prefix)$@ >>exec.out

link6.$(exe) : link6a.obj link6b.obj
    @set error_msg=failure to link $^@
    $(linker) @test.lnk NAME $^@ FILE $[@, $]@
    @set error_msg=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $(exec_prefix)$@ >>exec.out

link7.$(exe) : link7a.obj link7b.obj
    @set error_msg=failure to link $^@
    $(linker) @test.lnk NAME $^@ FILE $[@, $]@
    @set error_msg=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $(exec_prefix)$@ >>exec.out

link8.$(exe) : link8a.obj link8b.obj
    @set error_msg=failure to link $^@
    $(linker) @test.lnk NAME $^@ FILE $[@, $]@
    @set error_msg=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $(exec_prefix)$@ >>exec.out

link9.$(exe) : link9a.obj link9b.obj
    @set error_msg=failure to link $^@
    $(linker) @test.lnk NAME $^@ FILE $[@, $]@
    @set error_msg=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $(exec_prefix)$@ >>exec.out

link11.$(exe) : link11a.obj link11b.obj
    @set error_msg=failure to link $^@
    $(linker) @test.lnk NAME $^@ FILE $[@, $]@
    @set error_msg=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $(exec_prefix)$@ >>exec.out

link14.$(exe) : link14a.obj link14b.obj link14c.obj
    @set error_msg=failure to link $^@
    $(linker) @test.lnk NAME $^@ FILE { $< }
    @set error_msg=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $(exec_prefix)$@ >>exec.out

link15.$(exe) : link15a.obj link15b.obj link15c.obj
    @set error_msg=failure to link $^@
    $(linker) @test.lnk NAME $^@ FILE { $< }
    @set error_msg=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $(exec_prefix)$@ >>exec.out

fnstat1.$(exe) : fnstat1a.obj fnstat1b.obj
    @set error_msg=failure to link $^@
    $(linker) @test.lnk NAME $^@ FILE $[@, $]@
    @set error_msg=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $(exec_prefix)$@ >>exec.out

exc01.$(exe) : exc01a.obj exc01b.obj
    @set error_msg=failure to link $^@
    $(linker) @test.lnk NAME $^@ FILE $[@, $]@
    @set error_msg=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $(exec_prefix)$@ >>exec.out

cltemp21.$(exe) : cltemp21.obj cltmp21b.obj
    @set error_msg=failure to link $^@
    $(linker) @test.lnk NAME $^@ FILE $[@, $]@
    @set error_msg=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $(exec_prefix)$@ >>exec.out

friend4.$(exe) : friend4.obj friend4b.obj
    @set error_msg=failure to link $^@
    $(linker) @test.lnk NAME $^@ FILE $[@, $]@
    @set error_msg=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $(exec_prefix)$@ >>exec.out


start_test : .symbolic test.lnk
    %create exec.out
    @set error_file=exec.out
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav

test.lnk : makefile
    %create $^@
    @%append $^@ $(ldebug_$(arch))
    @%append $^@ $(lnk_$(arch))
    @%append $^@ OPTION quiet, noredef

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

global : .symbolic
    @if exist failure rm failure
    @%make common_clean

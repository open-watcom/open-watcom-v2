plustest_name = positive
!include ..\environ.mif

positive_dir = source

!include $(positive_dir)\positive.mif

!ifdef test1
cflags_386 = -d2-xss-mf
cflags_i86 = -d2-xss-mc-2
cflags_axp = -d2-xss-si
opt_EH_yes = -xss
add_br_ = -br
alt_error = 01
!else ifdef test2
cflags_386 = -d1-xss-mf
cflags_i86 = -d1-xss-mm
cflags_axp = -d1-xss-si
opt_EH_yes = -xss
alt_error = 02
!else ifdef test3
cflags_386 = -d2-xst-ms
cflags_i86 = -d2-xst-ml
cflags_axp = -d2-xst
opt_EH_yes = -xst
alt_error = 03
!else ifdef test4
cflags_386 = -d1-xst-mf
cflags_i86 = -d1-xst-ms
cflags_axp = -d1-xst-si
opt_EH_yes = -xst
alt_error = 04
!else ifdef test5
cflags_386 = -d1-xds-mf
cflags_i86 = -d1-xds-mc-2
cflags_axp = -d1-xds
opt_EH_yes = -xss
alt_error = 05
!else ifdef test6
cflags_386 = -d2-xds-mf
cflags_i86 = -d2-xds-mm
cflags_axp = -d2-xds
opt_EH_yes = -xss
alt_error = 06
!else ifdef test7
cflags_386 = -d1-xdt-ms
cflags_i86 = -d1-xdt-ml
cflags_axp = -d1-xdt
opt_EH_yes = -xst
alt_error = 07
!else ifdef test8
cflags_386 = -d2-xdt-mf
cflags_i86 = -d2-xdt-ms
cflags_axp = -d2-xdt
opt_EH_yes = -xst
alt_error = 08
!else ifdef test9
cflags_386 = -d1-xs-oe-mf
cflags_i86 = -d1-xs-oe-ms
cflags_axp = -d1-xs-oe
opt_EH_yes = -xs
alt_error = 09
!else ifdef test10
cflags_386 = -d2s-xdt-mf
cflags_i86 = -d2s-xdt-ms
cflags_axp = -d2s-xdt
opt_EH_yes = -xst
alt_error = 10
!else ifdef test11
cflags_386 = -d2s-xs-mf
cflags_i86 = -d2s-xs-ms
cflags_axp = -d2s-xs
opt_EH_yes = -xs
alt_error = 11
!else
cflags_386 = -ox-d1-xdt-ms
cflags_i86 = -ox-d1-xdt-ms
cflags_axp = -ox-d1-xdt
opt_EH_yes = -xst
alt_error = 00
add_br_ = -br
!endif

cflags_386 += -we-w0-zq -i$(positive_dir)
cflags_i86 += -we-w0-zq -i$(positive_dir)
cflags_axp += -we-w0-zq -i$(positive_dir)

!ifeq arch 386
cflags_386 += $(add_br_$(no_br_$^*))
!endif

.error
    @%append $(%error_file) $(%error_msg)

.c: $(positive_dir)
.c.obj:
    @set error_msg=failure to compile $^@
    $(wpp_$(arch)) $[@ $(cflags_$(arch)) $(opt_$^*) $(opt_EH_$(EH_$^*))
    
.obj.$(exe):
    @set error_msg=failure to link $^@
    $(linker) @test.lnk $(opt_link_$^*) FILE $[@
    @set error_msg=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $^@ >>exec.out
    del $[@
    wstrip -q $^@
    
test : .symbolic actual_test
    @set error_file=error.out
    @set error_msg=failure to complete test stream
    echo OK >error.out
    $(bld_cl) chk_exec.c -ox-d1
    chk_exec exec.out >>error.out
    diff -b error.out error.chk
    %make global
    %append $(log_file) PASS $(%__CWD__)

actual_test : .symbolic start_test $(positive_exes)
    if exist *.obj del *.obj
    if exist *.$(exe) del *.$(exe)

link3.$(exe) : link3a.obj link3b.obj
    @set error_msg=failure to link $^@
    $(linker) @test.lnk NAME $^@ FILE $[@, $]@
    @set error_msg=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $^@ >>exec.out
    
link4.$(exe) : link4a.obj link4b.obj
    @set error_msg=failure to link $^@
    $(linker) @test.lnk NAME $^@ FILE $[@, $]@
    @set error_msg=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $^@ >>exec.out
    
link5.$(exe) : link5a.obj link5b.obj
    @set error_msg=failure to link $^@
    $(linker) @test.lnk NAME $^@ FILE $[@, $]@
    @set error_msg=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $^@ >>exec.out
    
link6.$(exe) : link6a.obj link6b.obj
    @set error_msg=failure to link $^@
    $(linker) @test.lnk NAME $^@ FILE $[@, $]@
    @set error_msg=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $^@ >>exec.out
    
link7.$(exe) : link7a.obj link7b.obj
    @set error_msg=failure to link $^@
    $(linker) @test.lnk NAME $^@ FILE $[@, $]@
    @set error_msg=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $^@ >>exec.out
    
link8.$(exe) : link8a.obj link8b.obj
    @set error_msg=failure to link $^@
    $(linker) @test.lnk NAME $^@ FILE $[@, $]@
    @set error_msg=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $^@ >>exec.out
    
link9.$(exe) : link9a.obj link9b.obj
    @set error_msg=failure to link $^@
    $(linker) @test.lnk NAME $^@ FILE $[@, $]@
    @set error_msg=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $^@ >>exec.out
    
link11.$(exe) : link11a.obj link11b.obj
    @set error_msg=failure to link $^@
    $(linker) @test.lnk NAME $^@ FILE $[@, $]@
    @set error_msg=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $^@ >>exec.out
    
link14.$(exe) : link14a.obj link14b.obj link14c.obj
    @set error_msg=failure to link $^@
    $(linker) @test.lnk NAME $^@ FILE { $< }
    @set error_msg=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $^@ >>exec.out
    
link15.$(exe) : link15a.obj link15b.obj link15c.obj
    @set error_msg=failure to link $^@
    $(linker) @test.lnk NAME $^@ FILE { $< }
    @set error_msg=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $^@ >>exec.out
    
fnstat1.$(exe) : fnstat1a.obj fnstat1b.obj
    @set error_msg=failure to link $^@
    $(linker) @test.lnk NAME $^@ FILE $[@, $]@
    @set error_msg=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $^@ >>exec.out
    
exc01.$(exe) : exc01a.obj exc01b.obj
    @set error_msg=failure to link $^@
    $(linker) @test.lnk NAME $^@ FILE $[@, $]@
    @set error_msg=failure to execute $^@
    @%append exec.out PASS executing $^@
    $(run) $^@ >>exec.out
    
start_test : .symbolic test.lnk
    %create exec.out
    @set error_file=exec.out
    
test.lnk : makefile
    %create $^@
    @%append $^@ $(ldebug_$(arch))
    @%append $^@ $(lnk_$(arch))
    @%append $^@ OPTION stack=8k, quiet, caseexact, noredef
    
save : .symbolic
    if exist exec.out copy exec.out s$(arch)_$(alt_error).sav
    
global : .symbolic
    @if exist failure del failure
    @%make common_clean

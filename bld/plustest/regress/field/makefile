plustest_name = field
!include ..\environ.mif

!ifdef test1
cflags_386 = -d2-fpd
cflags_i86 = -d2-fpd
cflags_axp = -d2
!else ifdef test2
cflags_386 = -os-d1-3s-xds
cflags_i86 = -os-d1-xds
cflags_axp = -os-d1-xds
!else ifdef test3
cflags_386 = -os-d1-3s-xss-fpd
cflags_i86 = -os-d1-xss-fpd
cflags_axp = -os-d1-xss
!else
cflags_386 = -os-d1-xst
cflags_i86 = -os-d1-xst
cflags_axp = -os-d1-xst
!endif

cflags_386 += -zq-fpi87-fp5-mf
cflags_i86 += -zq-fpi87-fp5-ml
cflags_axp += -zq

.cpp.obj:
    $(wpp_$(arch)) $[@ $(cflags_$(arch))
    
test : .symbolic test1.$(exe) test2.$(exe) test3.$(exe)
    $(run) test1 >test.out
    $(run) test2 >>test.out
    $(run) test3 1000 1000 >>test.out
    diff test.out test.chk
    %make global
    %append $(log_file) PASS $(%__CWD__)
    
test1.$(exe) : field.obj test1.obj field.lnk
    $(linker) @$]@ NAME $^* FILE test1.obj, field.obj
    
test2.$(exe) : field.obj test2.obj field.lnk
    $(linker) @$]@ NAME $^* FILE test2.obj, field.obj
    
test3.$(exe) : field.obj test3.obj field.lnk
    $(linker) @$]@ NAME $^* FILE test3.obj, field.obj

field.lnk : makefile
    %create $^@
    @%append $^@ $(ldebug_$(arch))
    @%append $^@ $(lnk_$(arch))
    @%append $^@ OPTION map, caseexact, stack=16k, quiet

global : .SYMBOLIC
    @%make common_clean

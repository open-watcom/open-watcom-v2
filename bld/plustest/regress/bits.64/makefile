plustest_name = bits64
!include ..\environ.mif

!ifdef test1
cflags_386 = -d2-3r
cflags_i86 = -d2
cflags_axp = -d2
!else
cflags_386 = -ox-d1-3s
cflags_i86 = -ox-d1
cflags_axp = -ox-d1
!endif

cflags_386 += -zq-mf
cflags_i86 += -zq-ml
cflags_axp += -zq

.cpp: ..\bits
.cpp.obj:
    $(wpp_$(arch)) $[@ $(cflags_$(arch)) -d_USE_INT64

objs = tbits.obj
    
test : .symbolic tbits.$(exe)
    $(run) tbits >tbits.out
    diff tbits.out ..\bits\tbits.chk
    %make global
    %append $(log_file) PASS $(%__CWD__)

tbits.$(exe) : $(objs) tbits.lnk
    $(linker) @$]@

tbits.lnk : makefile
    %create $^@
    @%append $^@ NAME $^*
    @%append $^@ $(ldebug_$(arch))
    @%append $^@ $(lnk_$(arch))
    @%append $^@ OPTION map, caseexact, stack=8k, quiet
    @for %i in ( $(objs) ) do @%append $^@ FILE %i
    
global : .SYMBOLIC
    @%make common_clean

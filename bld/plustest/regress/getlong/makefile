plustest_name = getlong
!include ..\environ.mif

!ifdef test1
cflags = -d2
!else
cflags = -ox-d1
!endif

cflags += -zq

.cc.obj:
    $(wpp_$(arch)) $[@ $(cflags)

objs = &
    getlongo.obj &
    readme.obj
    
test : .symbolic opt.$(exe)
    $(run) opt >opt.out
    $(run) opt -h >>opt.out
    -$(run) opt -hopeless >>opt.out
    $(run) opt -vers >>opt.out
    $(run) opt -v >>opt.out
    -$(run) opt -o >>opt.out
    -$(run) opt -v=1 >>opt.out
    diff opt.out opt.chk
    %make global
    %append $(log_file) PASS $(%__CWD__)

opt.$(exe) : $(objs) opt.lnk
    $(linker) @$]@
    
opt.lnk : makefile
    %create $^@
    @%append $^@ NAME $^*
    @%append $^@ DEBUG all
    @%append $^@ $(lnk_$(arch))
    @%append $^@ OPTION map, caseexact, stack=8k, quiet
    @for %i in ( $(objs) ) do @%append $^@ FILE %i
    
global : .SYMBOLIC
    @%make common_clean

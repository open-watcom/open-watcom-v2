plustest_name = re2c05
!include ..\environ.mif

!ifdef test1
cflags = -d2
!else
cflags = -ox-d1
!endif

cflags += -zq

cflags_386 += $(cflags) -mf-zc
cflags_i86 += $(cflags) -ml-zc
cflags_axp += $(cflags)

.cpp.obj:
    $(wpp_$(arch)) $[@ $(cflags_$(arch))

objs = &
code.obj &
dfa.obj &
main.obj &
memchk.obj &
parser.obj &
scanner.obj &
substr.obj &
translat.obj &
actions.obj
    
test : .symbolic re2c.$(exe)
    $(run) re2c.$(exe) -b scanner.re >debug.out
    diff debug.out debug.chk | wc -l >line.out
    diff line.out line.chk
    %make global
    %append $(log_file) PASS $(%__CWD__)

re2c.$(exe) : $(objs) re2c.lnk
    $(linker) @$]@
    
re2c.lnk : makefile
    %create $^@
    @%append $^@ NAME $^*
    @%append $^@ $(ldebug_$(arch))
    @%append $^@ $(lnk_$(arch))
    @%append $^@ OPTION map, caseexact, stack=8k, quiet
    @for %i in ( $(objs) ) do @%append $^@ FILE %i
    
global : .SYMBOLIC
    @%make common_clean

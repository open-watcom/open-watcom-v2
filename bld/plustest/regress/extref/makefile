plustest_name = extref
!include ..\environ.mif

!ifdef test1
cflags = -d2
extref_eh = -xss
!else ifdef test2
cflags = -ot-d1
extref_eh = -xst
!else ifdef test3
cflags = -d2
extref_eh = -xst
!else
cflags = -ot-d1
extref_eh = -xss
!endif

!ifneq arch axp
# until we implement it under AXP
cflags += -zv
!endif
cflags += -zq -i..\positive\source

lib = $(librarian_$(arch)) /n/b/q

#
# extra flags for a module
#
extra_er01a = $(extref_eh)
extra_er01b = $(extref_eh)
extra_er01c = $(extref_eh)
extra_er01d = $(extref_eh)
extra_er03a = $(extref_eh)
extra_er03b = $(extref_eh)
extra_er03c = $(extref_eh)
    
.c.obj:
    $(wpp_$(arch)) $[@ $(cflags) $(extra_$^*)
    
tests = er01.$(exe) er02.$(exe) er03.$(exe)
    
test : .symbolic $(tests)
    %create test.out
    for %i in ($(tests)) do $(run) %i >>test.out
    diff test.out test.chk
    %make global
    %append $(log_file) PASS $(%__CWD__)
    
er01.$(exe) : er01a.obj er01.lib er.lnk
    wlink @er NAME $^@ FILE er01a LIBRARY er01
    
er01.lib : er01b.obj er01c.obj er01d.obj
    $(lib) $^@ $?
    
er02.$(exe) : er02a.obj er02b.obj er.lnk
    wlink @er NAME $^@ FILE er02a, er02b
    
er03.$(exe) : er03c.obj er03.lib er.lnk
    wlink @er NAME $^@ FILE er03c LIB er03.lib
    
er03.lib : er03a.obj er03b.obj
    $(lib) $^@ +er03a +er03b
    
er.lnk : makefile
    %create $^@
    @%append $^@ OPTION quiet, stack=8k
!ifneq arch axp
# until we implement it under AXP
    @%append $^@ OPTION eliminate, vfremoval
!endif
    @%append $^@ $(lnk_$(arch))
    @%append $^@ $(ldebug_$(arch))
    
global : .symbolic
    @%make common_clean

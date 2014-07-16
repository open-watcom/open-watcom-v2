plustest_name = extref

!include testenv.mif

!ifdef test1
test_flags = -d2
extref_eh = -xss
alt_error=01
!else ifdef test2
test_flags = -ot-d1
extref_eh = -xst
alt_error=02
!else ifdef test3
test_flags = -d2
extref_eh = -xst
alt_error=03
!else
test_flags = -ot-d1
extref_eh = -xss
alt_error=00
!endif

!ifneq arch axp
# until we implement it under AXP
test_flags += -zv
!endif
test_flags += -I"../positive/source"

!ifneq arch axp
# until we implement it under AXP
link_flags = OPTION eliminate, vfremoval
!endif

stack_386=opt stack=8k
stack_i86=opt stack=4k
stack_axp=opt stack=8k

#
# extra flags for a module
#
opt_er01 = $(extref_eh)
opt_er01b = $(extref_eh)
opt_er01c = $(extref_eh)
opt_er01d = $(extref_eh)
opt_er03 = $(extref_eh)
opt_er03b = $(extref_eh)
opt_er03c = $(extref_eh)

opt_link_er01 = LIB er01.lib
opt_link_er03 = LIB er03.lib

exes = er01.$(exe) er02.$(exe) er03.$(exe)

test : .symbolic start clean start_test test.lnk er01.lib er03.lib $(exes) ./chk_exec.exe
    @set ERROR_FILE=error.out
    @set ERROR_MSG=failure to complete test stream
    %write $(%ERROR_FILE) OK
    $]@ exec.out >> $(%ERROR_FILE)
    diff -b $(%ERROR_FILE) error.chk
    %append $(log_file) PASS $(%__CWD__)
    %make clean

er02.$(exe) : er02.obj er02b.obj

start_test : .symbolic
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav
    @set ERROR_FILE=exec.out
    %create $(%ERROR_FILE)

er01.lib : er01b.obj er01c.obj er01d.obj
    %make set_path_wlib
    $(librarian) $^@ $?
    %make set_old_path

er03.lib : er03b.obj er03c.obj
    %make set_path_wlib
    $(librarian) $^@ +er03b.obj +er03c.obj
    %make set_old_path

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

clean : .procedure
    @%make common_clean

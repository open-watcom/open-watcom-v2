# makefile for tstrtnt1.c - test that DLL threading
# starts as expected.

clibtest_name = ntthrd

!ifdef __NT__

ntexes = tstdll1.dll tstexe1.exe

#all : start clean start_test $(exes) tests .symbolic
all : .symbolic start clean start_test $(ntexes) nttests
#    diff $(%ERROR_FILE) test.chk
    @%append $(log_file) PASS $(%__CWD__)
    @%make clean

!include ../master.mif

nttests: ntrunwnt .symbolic
    @echo The clib tests completed: SUCCESS!

ntrunwnt : tstexe1.exe .symbolic
    @$[@

tstdll1.dll : ../c/tstrtnt1.c
    @%make echo_cc
    $(noecho)$(wcc_386) $[@ $(test_flags) $(test_flags_386) -mf -3r -bt=nt -bd -bm -DQA_MAKE_DLL -fo=$^*.obj $(c_dbg_flags)
    @%make set_path_wlink
    @%make echo_link
    $(noecho)$(linker) SYSTEM nt_dll $(link_opts) $(link_x86) opt offset=0x01000000, impl FILE $^*.obj
    @%make set_old_path

tstexe1.exe : ../c/tstrtnt1.c tstdll1.lib
    @%make echo_cc
    $(noecho)$(wcc_386) $[@ $(test_flags) $(test_flags_386) -mf -3r -bt=nt -bm -DQA_MAKE_EXE -fo=$^*.obj $(c_dbg_flags)
    @%make set_path_wlink
    @%make echo_link
    $(noecho)$(linker) SYSTEM nt $(link_opts) $(link_x86) opt offset=0x01000000 FILE $^*.obj LIBRARY $]@
    @%make set_old_path

!else

all : .symbolic

clean : .symbolic

!endif

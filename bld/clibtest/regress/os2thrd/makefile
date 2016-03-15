# makefile for test1.c - test that DLL threading
# starts as expected.

clibtest_name = os2thrd

os2exes = tdll32_1.dll texe32_1.exe texe32_2.exe

#all : start clean start_test $(os2exes) tests .symbolic
all : start clean start_test $(os2exes) os2tests .symbolic
#    diff $(%ERROR_FILE) test.chk
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

!include ../mif/master.mif

os2tests: os2runos2 .symbolic
    @echo The clib tests completed: SUCCESS!

os2runos2 : texe32_1.exe texe32_2.exe .symbolic
    @texe32_1.exe
    @texe32_2.exe

tdll32_1.dll : ../c/test1.c
    $(wcc_386) $[@ $(test_flags) $(test_flags_386) -mf -3r -bt=os2 -bd -bm -DQA_MAKE_DLL $(c_dbg_flags)
    @%make set_path_wlink
    $(linker) SYSTEM os2v2_dll $(link_opts) $(link_x86) opt impl FILE $^*.obj
    @%make set_old_path

texe32_1.exe : ../c/test1.c tdll32_1.lib
    $(wcc_386) $[@ $(test_flags) $(test_flags_386) -mf -3r -bt=os2 -bm -DQA_MAKE_EXE $(c_dbg_flags)
    @%make set_path_wlink
    $(linker) SYSTEM os2v2 $(link_opts) $(link_x86) opt offset=0x01000000 FILE $^*.obj LIBRARY $]@
    @%make set_old_path

texe32_2.exe : ../c/test2.c
    $(wcc_386) $[@ $(test_flags) $(test_flags_386) -mf -3r -bt=os2 -bm $(c_dbg_flags)
    @%make set_path_wlink
    $(linker) SYSTEM os2v2 $(link_opts) $(link_x86) opt offset=0x01000000 FILE $^*.obj
    @%make set_old_path

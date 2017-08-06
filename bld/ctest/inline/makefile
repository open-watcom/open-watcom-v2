ctest_name = inline

inline_assembly = 1

!include testenv1.mif
!include inline.mif

!ifdef test1
test_flags_386 =
test_flags_i86 =
alt_error = 01
!else
test_flags_386 =
test_flags_i86 =
alt_error = 00
!endif

test_flags_386 += -s -os -wx -we
test_flags_i86 += -s -os -wx -we

.src: $(wasmtest_dir)/inline
.asm: $(wasmtest_dir)/inline

test : .symbolic start clean start_test $($(arch)_inline_objs) set_result
    @%make clean

set_result : .symbolic ./chk_exec.exe
    -@%write error.out OK
    -$]@ exec.out >>error.out
    $(noecho)%make set_fail
    $(noecho)%make common_save_sav
    diff -b error.out error.chk
    $(noecho)%make common_clean_sav
    $(noecho)%make set_pass

start_test : .symbolic
    $(noecho)%make common_start_test
!ifndef $(arch)_inline_objs
    @%append $(%ERROR_FILE) PASS decoding
!endif

clean : .procedure
    @%make common_clean
    @if exist *.gc rm -f *.gc
    @if exist *.ls1 rm -f *.ls1
    @if exist *.dis rm -f *.dis
    @if exist failure rm failure

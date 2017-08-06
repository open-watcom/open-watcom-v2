plustest_name = inline

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

.asm.gcp:
    @set ERROR_MSG=failure to create $^&
    @*awk -v target=$(init_$^&) -f $(wasmtest_dir)/createc.awk -v OUTFILE=$^. $[@

.src.gcp:
    @cp $< $@ >$(nulldevice)

test : .symbolic start clean start_test $($(arch)_inline_objs) ./chk_exec.exe set_result
    @%make clean

set_result : .symbolic
    $(noecho)%make set_fail
    $(noecho)%make common_save_sav
    diff -b $(sav_file) inline.chk
    $(noecho)%make common_clean_sav
    $(noecho)%make set_pass

start_test : .symbolic
    @%make common_start_test
!ifndef $(arch)_inline_objs
    @%append $(%ERROR_FILE) PASS decoding $(arch)
!endif

clean : .procedure
    @if exist *.gcp $(noecho)rm -f *.gcp
    @if exist *.ls1 $(noecho)rm -f *.ls1
    @if exist *.dis $(noecho)rm -f *.dis
    @if exist failure $(noecho)rm failure
    $(noecho)%make common_clean

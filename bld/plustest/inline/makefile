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

test : .symbolic start clean start_test $($(arch)_inline_objs) ./chk_exec.exe
    @set ERROR_FILE=$(log_file)
    @set ERROR_MSG=FAILED
    $(noecho)%make save_sav
    diff -b $(sav_file) inline.chk
    $(noecho)%make clean_sav
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

start_test : .symbolic
    $(noecho)%make clean_sav
    @set ERROR_FILE=exec.out
    $(noecho)%create $(%ERROR_FILE)
!ifndef $(arch)_inline_objs
    @%append $(%ERROR_FILE) PASS decoding $(arch)
!endif

save_sav : .procedure
    @if exist exec.out @cp exec.out $(sav_file) >$(nulldevice)
    @if exist exec.out @rm exec.out

clean_sav : .procedure
    @if exist $(sav_file) @rm $(sav_file)

clean : .procedure
    @if exist *.gcp $(noecho)rm -f *.gcp
    @if exist *.ls1 $(noecho)rm -f *.ls1
    @if exist *.dis $(noecho)rm -f *.dis
    @if exist failure $(noecho)rm failure
    $(noecho)%make common_clean

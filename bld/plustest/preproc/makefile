# pp11 has incorrect output
plustest_name = preproc

!include testenv1.mif

alt_error=00

.c.i:
    %append $(%ERROR_FILE) @@@ $^@ @@@
    @%make set_path_wpp
    $(wpp_$(arch)) $[@ -w0 -pl -fo=.i -fip
    @%make set_old_path
    cat $(%ERROR_FILE) $^@ >tmp_exec.out
    cp tmp_exec.out $(%ERROR_FILE) >$(nulldevice)

.c.i1:
    %append $(%ERROR_FILE) @@@ $^@ @@@
    @%make set_path_wpp
    $(wpp_$(arch)) $[@ -w0 -plc -fo=.i1 -fip
    @%make set_old_path
    cat $(%ERROR_FILE) $^@ >tmp_exec.out
    cp tmp_exec.out $(%ERROR_FILE) >$(nulldevice)

.c.i0:
    %append $(%ERROR_FILE) @@@ $^@ @@@
    @%make set_path_wpp
    $(wpp_$(arch)) $[@ -w0 -pl -pw=0 -fo=.i0 -fip
    @%make set_old_path
    cat $(%ERROR_FILE) $^@ >tmp_exec.out
    cp tmp_exec.out $(%ERROR_FILE) >$(nulldevice)

all_tests = &
    pp01.i &
    pp02.i &
    pp03.i &
    pp04.i &
    pp04.i0 &
    pp05.i1 &
    pp06.i1 &
    pp07.i1 &
    pp08.i1 &
    pp09.i1 &
    pp10.i1 &
    pp11.i1 &
    pp12.i1 &
    pp13.i &
    pp14.i1 &
    pp15.i0 &
    pp16.i1 &
    pp17.i1 &


test : .symbolic start clean start_test $(all_tests) set_result
    @%make clean

set_result : .symbolic
    $(noecho)%make set_fail
    $(noecho)%make common_save_sav
    diff $(sav_file) exec.chk
    $(noecho)%make common_clean_sav
    $(noecho)%make set_pass

start_test : .symbolic
    @%make common_start_test
    $(noecho)%make gen_input

zap_output : .procedure
    @if exist *.i $(noecho)rm *.i
    @if exist *.i0 $(noecho)rm *.i0
    @if exist *.i1 $(noecho)rm *.i1
    @if exist *.gh $(noecho)rm *.gh

gen_input: .procedure ./geninput.exe
    $[@
    @rm *.exe
    @rm *.obj

./geninput.exe : geninput.c
    @%make echo_bldcl
    $(noecho)$(bld_cl) $(bld_cl_opts) -cc++ $<

clean : .procedure
    @%make zap_output
    $(noecho)%make common_clean

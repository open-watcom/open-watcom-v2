# pp11 has incorrect output
plustest_name = preproc

alt_error=00

!include testenv.mif

.c.i:
    %append $(%ERROR_FILE) @@@ $@ @@@
    @%make set_path_wpp
    $(wpp_$(arch)) $[@ -w0 -pl -fo=.i -fip
    @%make set_old_path
    cat $@ >>$(%ERROR_FILE)

.c.i1:
    %append $(%ERROR_FILE) @@@ $@ @@@
    @%make set_path_wpp
    $(wpp_$(arch)) $[@ -w0 -plc -fo=.i1 -fip
    @%make set_old_path
    cat $@ >>$(%ERROR_FILE)

.c.i0:
    %append $(%ERROR_FILE) @@@ $@ @@@
    @%make set_path_wpp
    $(wpp_$(arch)) $[@ -w0 -pl -pw=0 -fo=.i0 -fip
    @%make set_old_path
    cat $@ >>$(%ERROR_FILE)

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


test : .symbolic start start_test $(all_tests)
    $(noecho)%make common_set_result

start_test : .symbolic
    $(noecho)%make gen_input

zap_output : .procedure
    @if exist *.i $(noecho)rm *.i
    @if exist *.i0 $(noecho)rm *.i0
    @if exist *.i1 $(noecho)rm *.i1
    @if exist *.gh $(noecho)rm *.gh

gen_input: .procedure ./geninput.exe
    $[@
    @if exist *.obj @rm *.obj
    @if exist *.exe @rm *.exe

./geninput.exe : geninput.c
    @%make echo_bldcl
    $(noecho)$(bld_cl) $(bld_cl_opts) -cc++ $<

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
    @%make zap_output

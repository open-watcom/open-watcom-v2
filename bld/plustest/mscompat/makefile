plustest_name = mscompat

!include testenv1.mif

files_err = &
diag0011$(ext) &
diag0085$(ext) &
diag0086$(ext) &
diag0187$(ext) &
diag0198$(ext) &
diag0251$(ext) &
diag0267$(ext) &
diag0268$(ext) &
lisa006$(ext) &
lisa012$(ext)  

files_comp = &
ms01$(ext) &
ptrfun04$(ext) &
udcnv14$(ext) &
udcnv15$(ext) &
udcnv16$(ext) &
udcnv17$(ext)

.cpp.exe :
    e:\cmds\mclauto.cmd $[* 
    @%make set_path_rtdll
    $(run) $(exec_prefix)$[*.exe >> $(%ERROR_FILE)
    @%make set_old_path
    
.cpp.mer :
    e:\cmds\mclauto.cmd $[* /c
    type $[*.mer >> $(%ERROR_FILE)

ext=.mer
err_out = $+ $(files_err) $-

ext=.exe
comp_out = $+ $(files_comp) $-

test : .symbolic start clean start_test $(comp_out) $(err_out)
    @set ERROR_FILE=$(log_file)
    @set ERROR_MSG=FAILED
    $(noecho)%make save_sav
    diff $(sav_file) mscompat.chk
    $(noecho)%make clean_sav
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

start_test : .symbolic
    $(noecho)%make clean_sav
    @set ERROR_FILE=exec.out
    $(noecho)%create $(%ERROR_FILE)

save_sav : .procedure
    @if exist exec.out @cp exec.out $(sav_file) >$(nulldevice)
    @if exist exec.out @rm exec.out

clean_sav : .procedure
    @if exist $(sav_file) @rm $(sav_file)

clean : .procedure
    @if exist *.mer $(noecho)rm *.mer
    $(noecho)%make common_clean

plustest_name = mscompat

!include testenv.mif

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
    @%make set_run_path
    $(run) $(exec_prefix)$[*.exe >> $(%ERROR_FILE)
    @%make set_old_run_path

.cpp.mer :
    e:\cmds\mclauto.cmd $[* /c
    type $[*.mer >> $(%ERROR_FILE)

ext=.mer
err_out = $+ $(files_err) $-

ext=.exe
comp_out = $+ $(files_comp) $-

test : .symbolic start $(comp_out) $(err_out)
    $(noecho)%make common_set_result

clean : .procedure
    @%make echo_clean
    $(noecho)%make common_clean
    @if exist *.mer $(noecho)rm *.mer

if not exist %1. goto done
copy %1 toconv.h
wcl dlgconv
if errorlevel 1 goto done
dlgconv
copy test.h %1.h
copy test.dlg %1.dlg
rem wrc -r -bt=windows test.rc
:done

@call teststrm.bat %1
@echo ************ %1 TEST *************** >> __msgs__.log
@type testout\diagnose.log >> __msgs__.log

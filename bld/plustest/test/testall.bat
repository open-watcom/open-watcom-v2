call teststrm diagnose.tst
@copy testout\test.log testall.log >NUL
pause
call teststrm exprerr.tst
@if exist DIFFALL.LST erase DIFFALL.LST > NUL 
@if exist DIFF.LST copy DIFF.LST DIFFALL.LST > NUL
@copy testall.log+test.log > NUL
pause
REM call teststrm basics.tst
REM @if exist DIFF.LST copy DIFFALL.LST+DIFF.LST
REM @copy testall.log+test.log
REM pause
call teststrm convert.tst
@if exist DIFF.LST copy DIFFALL.LST+DIFF.LST > NUL
@copy testall.log+test.log > NUL
@if exist DIFF.LST type diff.lst
pause
call teststrm statment.tst
@if exist DIFF.LST copy DIFFALL.LST+DIFF.LST > NUL
@copy testall.log+test.log > NUL
@if exist DIFF.LST type diff.lst
pause
call teststrm declar.tst
@if exist DIFF.LST copy DIFFALL.LST+DIFF.LST > NUL
@copy testall.log+test.log > NUL
@if exist DIFF.LST type diff.lst
pause
call teststrm expres.tst
@if exist DIFF.LST copy DIFFALL.LST+DIFF.LST > NUL
@copy testall.log+test.log >NUL
@if exist DIFF.LST type diff.lst
pause

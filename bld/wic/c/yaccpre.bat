rem Turn yacc file with prefix specified into C.
if not exist %2.y goto BAD_NAME
c:\mks\YACC.EXE -p %1 %2.y > __temp__.chk
diff __temp__.chk ..\c\%1conf.chk
rm __temp__.chk
rem filter out '#line' directive
sed '/#line/d' ytab.c >%2.c    
rm ytab.c
goto FINISH


:BAD_NAME
echo The file %2.y does not exist!
pause
goto FINISH

:FINISH

@echo off
if .%1 == . goto usage

echo %1 -h -f upd01
%1 -h -f upd01
if errorlevel 1 goto err

echo %1 -h -f upd02
%1 -h -f upd02
if errorlevel 1 goto err

echo %1 -h -f upd03
%1 -h -f upd03
if errorlevel 1 goto err

echo cmds\errout %1 -h -s -f upd04 )tmp.out
cmds\errout %1 -h -s -f upd04 >tmp.out
if errorlevel 1 goto err
echo diff upd04.out tmp.out
diff upd04.out tmp.out
if errorlevel 1 goto err

echo %1 -h -s -f upd05
%1 -h -s -f upd05
if errorlevel 1 goto err

echo cmds\errout %1 -h -f upd06 )tmp.out
cmds\errout %1 -h -f upd06 >tmp.out
if errorlevel 1 goto err
echo diff upd06.out tmp.out
diff upd06.out tmp.out
if errorlevel 1 goto err

echo cmds\errout %1 -h -f upd07 )tmp.out
cmds\errout %1 -h -f upd07 >tmp.out
if errorlevel 1 goto err
echo diff upd07.out tmp.out
diff upd07.out tmp.out
if errorlevel 1 goto err

echo %1 -h -f upd08 )tmp.out
%1 -h -f upd08 >tmp.out
if errorlevel 1 goto err
echo diff upd08.out tmp.out
diff upd08.out tmp.out
if errorlevel 1 goto err

echo %1 -h -f upd09
%1 -h -f upd09
if errorlevel 1 goto err

echo %1 -h -f upd09
%1 -h -f upd09
if errorlevel 1 goto err

echo upd tests ok
goto done
:err
echo didn't work
goto done
:usage
echo usage: %0 prgname
:done
if exist tmp.out del tmp.out



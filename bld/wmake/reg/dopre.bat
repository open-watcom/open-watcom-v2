@echo off
rem test various things in wmake
rem
if .%1 == . goto usage

echo %1 -h -f pre01 pre01 -f pre02 pre02
%1 -h -f pre01 pre01 -f pre02 pre02
if errorlevel 1 goto err

echo cmds\errout %1 -h -f pre03 pre03 )tmp.out
cmds\errout %1 -h -f pre03 pre03 >tmp.out
echo diff pre03.out tmp.out
diff pre03.out tmp.out
if errorlevel 1 goto err

echo %1 -h -f pre04 pre04
%1 -h -f pre04 pre04
if errorlevel 1 goto err

echo %1 -h -f pre05 pre05
%1 -h -f pre05 pre05
if errorlevel 1 goto err

echo %1 -h -f pre06 pre06
%1 -h -f pre06 pre06
if errorlevel 1 goto err

echo cmds\errout %1 -h -f pre07 )tmp.out
cmds\errout %1 -h -f pre07 >tmp.out
echo diff pre07.out tmp.out
diff pre07.out tmp.out
if errorlevel 1 goto err

echo cmds\errout %1 -h -f pre08 )tmp.out
cmds\errout %1 -h -f pre08 >tmp.out
if errorlevel 1 goto err
echo diff pre08.out tmp.out
diff pre08.out tmp.out
if errorlevel 1 goto err

echo pre tests ok
goto done
:err
echo didn't work
goto done
:usage
echo usage: %0 prgname
:done
if exist tmp.out del tmp.out

@echo off
set GMLINC=.;%OWROOT%\docs\doc\whelp;%OWROOT%\docs\doc\gml
set GMLLIB=%OWROOT%\docs\gml\syslib
if not %build_platform% == dos386 goto next1
set WGML=%OWROOT%\docs\gml\dos\wgml
goto compile
:next1
if not %build_platform% == os2386 goto next2
set WGML=%OWROOT%\docs\gml\os2\wgml
goto compile
:next2
if not %build_platform% == nt386 goto error1
set WGML=%OWROOT%\docs\gml\dos\wgml
:compile
%WGML% cgdoc.gml (file cgdps
goto end

:error1
@echo !!!!! Unsupported building platform !!!!!!

:end

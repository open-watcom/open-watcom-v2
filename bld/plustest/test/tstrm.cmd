@if [] == [%1] goto info
@if [] == [%2] goto make_all
@   wmake /h /f %1 %2 %3 %4 %5 %6 %7 %8 %9
@   goto done
:make_all
@   wmake /h /f %1 global
@   wmake /h /f %1 all %2 %3 %4 %5 %6 %7 %8 %9
@   goto done
:info
@   echo Usage: TSTRM makefile [options]
@   echo Example: tstrm diagnose.mak
:done

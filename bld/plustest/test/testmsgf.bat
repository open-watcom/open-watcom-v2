@set OLD_BLIP=%WCGBLIPON%
@set TEST_CMD=run386 ..\dosi86\wcppi86d.exp
@if [%DEF_CPP%] == [] goto DEFAULTCPP
  @set TEST_CMD=%DEF_CPP%
:DEFAULTCPP
@clog testout\__TEST__.LOG
@%TEST_CMD% %1 -zq -w9 -fo=testout\*.obj
@clog
@echo ************************************ >> testout\error.ref
@echo ************************************ %1 >> testout\error.ref
@echo ************************************ >> testout\error.ref
@type %1 >>testout\error.ref
@sed -f ..\sed\stripvcl.sed testout\__TEST__.LOG >>testout\error.ref
@set WCGBLIPON=%OLD_BLIP%
@set OLD_BLIP=

set TIMESTAMP=
@echo on
call dopack.bat | ..\supp\tee -a pack.log
@echo off

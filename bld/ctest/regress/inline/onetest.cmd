pushd
if exist *.sav del /f *.sav
wmake -h global
wmake -h
wmake -h save
wmake -h global
popd

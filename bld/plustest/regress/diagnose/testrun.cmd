pushd
if exist *.sav del *.sav
wmake -h global
wmake -h
wmake -h save
wmake -h global
popd

pushd
cd base
wmake -h global
wmake -h
wmake -h save
cd ..\io
wmake -h global
wmake -h
wmake -h save
cd ..\test
wmake -h global
wmake -h
wmake -h save
cd ..\base
wmake -h global
cd ..\io
wmake -h global
cd ..\test
wmake -h global
cd ..
popd

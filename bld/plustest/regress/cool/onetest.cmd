pushd
cd objs
wmake -h global
wmake -h
wmake -h save
cd ..\examples
wmake -h global
wmake -h
wmake -h save
cd ..\tests
wmake -h global
wmake -h
wmake -h save
cd ..\objs
wmake -h global
cd ..\examples
wmake -h global
cd ..\tests
wmake -h global
cd ..
popd

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
wmake -h test1=
wmake -h test1= save
cd ..\examples
wmake -h global
wmake -h test1=
wmake -h test1= save
cd ..\tests
wmake -h global
wmake -h test1=
wmake -h test1= save
cd ..\objs
wmake -h global
wmake -h test2=
wmake -h test2= save
cd ..\examples
wmake -h global
wmake -h test2=
wmake -h test2= save
cd ..\tests
wmake -h global
wmake -h test2=
wmake -h test2= save
cd ..\objs
wmake -h global
wmake -h test3=
wmake -h test3= save
cd ..\examples
wmake -h global
wmake -h test3=
wmake -h test3= save
cd ..\tests
wmake -h global
wmake -h test3=
wmake -h test3= save
cd ..\objs
wmake -h global
cd ..\examples
wmake -h global
cd ..\tests
wmake -h global
cd ..
if [%extra_arch%] == [] goto no_extra
cd objs
wmake -h global
wmake -h arch=%extra_arch%
wmake -h arch=%extra_arch% save
cd ..\examples
wmake -h global
wmake -h arch=%extra_arch%
wmake -h arch=%extra_arch% save
cd ..\tests
wmake -h global
wmake -h arch=%extra_arch%
wmake -h arch=%extra_arch% save
cd ..\objs
wmake -h global
wmake -h arch=%extra_arch% test1=
wmake -h arch=%extra_arch% test1= save
cd ..\examples
wmake -h global
wmake -h arch=%extra_arch% test1=
wmake -h arch=%extra_arch% test1= save
cd ..\tests
wmake -h global
wmake -h arch=%extra_arch% test1=
wmake -h arch=%extra_arch% test1= save
cd ..\objs
wmake -h global
wmake -h arch=%extra_arch% test2=
wmake -h arch=%extra_arch% test2= save
cd ..\examples
wmake -h global
wmake -h arch=%extra_arch% test2=
wmake -h arch=%extra_arch% test2= save
cd ..\tests
wmake -h global
wmake -h arch=%extra_arch% test2=
wmake -h arch=%extra_arch% test2= save
cd ..\objs
wmake -h global
wmake -h arch=%extra_arch% test3=
wmake -h arch=%extra_arch% test3= save
cd ..\examples
wmake -h global
wmake -h arch=%extra_arch% test3=
wmake -h arch=%extra_arch% test3= save
cd ..\tests
wmake -h global
wmake -h arch=%extra_arch% test3=
wmake -h arch=%extra_arch% test3= save
cd ..\objs
wmake -h global
cd ..\examples
wmake -h global
cd ..\tests
wmake -h global
cd ..
:no_extra
popd

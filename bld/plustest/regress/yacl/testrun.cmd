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
wmake -h test1=
wmake -h test1= save
cd ..\io
wmake -h global
wmake -h test1=
wmake -h test1= save
cd ..\test
wmake -h global
wmake -h test1=
wmake -h test1= save
cd ..\base
wmake -h global
wmake -h test2=
wmake -h test2= save
cd ..\io
wmake -h global
wmake -h test2=
wmake -h test2= save
cd ..\test
wmake -h global
wmake -h test2=
wmake -h test2= save
cd ..\base
wmake -h global
wmake -h test3=
wmake -h test3= save
cd ..\io
wmake -h global
wmake -h test3=
wmake -h test3= save
cd ..\test
wmake -h global
wmake -h test3=
wmake -h test3= save
cd ..\base
wmake -h global
cd ..\io
wmake -h global
cd ..\test
wmake -h global
cd ..
if [%extra_arch%] == [] goto no_extra
cd base
wmake -h global
wmake -h arch=%extra_arch%
wmake -h arch=%extra_arch% save
cd ..\io
wmake -h global
wmake -h arch=%extra_arch%
wmake -h arch=%extra_arch% save
cd ..\test
wmake -h global
wmake -h arch=%extra_arch%
wmake -h arch=%extra_arch% save
cd ..\base
wmake -h global
wmake -h arch=%extra_arch% test1=
wmake -h arch=%extra_arch% test1= save
cd ..\io
wmake -h global
wmake -h arch=%extra_arch% test1=
wmake -h arch=%extra_arch% test1= save
cd ..\test
wmake -h global
wmake -h arch=%extra_arch% test1=
wmake -h arch=%extra_arch% test1= save
cd ..\base
wmake -h global
wmake -h arch=%extra_arch% test2=
wmake -h arch=%extra_arch% test2= save
cd ..\io
wmake -h global
wmake -h arch=%extra_arch% test2=
wmake -h arch=%extra_arch% test2= save
cd ..\test
wmake -h global
wmake -h arch=%extra_arch% test2=
wmake -h arch=%extra_arch% test2= save
cd ..\base
wmake -h global
wmake -h arch=%extra_arch% test3=
wmake -h arch=%extra_arch% test3= save
cd ..\io
wmake -h global
wmake -h arch=%extra_arch% test3=
wmake -h arch=%extra_arch% test3= save
cd ..\test
wmake -h global
wmake -h arch=%extra_arch% test3=
wmake -h arch=%extra_arch% test3= save
cd ..\base
wmake -h global
cd ..\io
wmake -h global
cd ..\test
wmake -h global
cd ..
:no_extra
popd

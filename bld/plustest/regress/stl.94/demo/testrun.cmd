pushd
wmake -h global
wmake -h
wmake -h save
wmake -h global
wmake -h test1=
wmake -h test1= save
wmake -h global
wmake -h test2=
wmake -h test2= save
wmake -h global
wmake -h test3=
wmake -h test3= save
if [%extra_arch%] == [] goto no_extra
wmake -h global
wmake -h arch=%extra_arch%
wmake -h arch=%extra_arch% save
wmake -h global
wmake -h test1= arch=%extra_arch%
wmake -h test1= arch=%extra_arch% save
wmake -h global
wmake -h test2= arch=%extra_arch%
wmake -h test2= arch=%extra_arch% save
wmake -h global
wmake -h test3= arch=%extra_arch%
wmake -h test3= arch=%extra_arch% save
:no_extra
wmake -h global
popd

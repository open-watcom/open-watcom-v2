pushd
wmake -h global
wmake -h
wmake -h global
wmake -h test1=
wmake -h global
wmake -h test2=
if [%extra_arch%] == [] goto no_extra
wmake -h global
wmake -h arch=%extra_arch%
wmake -h global
wmake -h test1= arch=%extra_arch%
wmake -h global
wmake -h test2= arch=%extra_arch%
:no_extra
wmake -h global
popd

pushd
wmake -h global
wmake -h
if [%extra_arch%] == [] goto no_extra
wmake -h global
wmake -h arch=%extra_arch%
:no_extra
wmake -h global
popd

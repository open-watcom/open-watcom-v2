pushd
wmake -h global
wmake -h
wmake -h global
wmake -h test1=
wmake -h global
wmake -h test2=
wmake -h global
wmake -h test3=
wmake -h global
wmake -h test4=
wmake -h global
wmake -h test5=
wmake -h global
wmake -h test6=
wmake -h global
wmake -h test7=
wmake -h global
wmake -h test8=
wmake -h global
wmake -h test9=
if [%extra_arch%] == [] goto no_extra
wmake -h global
wmake -h arch=%extra_arch%
wmake -h global
wmake -h test1= arch=%extra_arch%
wmake -h global
wmake -h test2= arch=%extra_arch%
wmake -h global
wmake -h test3= arch=%extra_arch%
wmake -h global
wmake -h test4= arch=%extra_arch%
wmake -h global
wmake -h test5= arch=%extra_arch%
wmake -h global
wmake -h test6= arch=%extra_arch%
wmake -h global
wmake -h test7= arch=%extra_arch%
wmake -h global
wmake -h test8= arch=%extra_arch%
wmake -h global
wmake -h test9= arch=%extra_arch%
:no_extra
wmake -h global
popd

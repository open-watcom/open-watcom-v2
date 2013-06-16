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
wmake -h global
wmake -h test4=
wmake -h test4= save
wmake -h global
wmake -h test5=
wmake -h test5= save
wmake -h global
wmake -h test6=
wmake -h test6= save
wmake -h global
wmake -h test7=
wmake -h test7= save
wmake -h global
wmake -h test8=
wmake -h test8= save
wmake -h global
wmake -h test9=
wmake -h test9= save
wmake -h global
rem
if [%extra_arch%] == [] goto no_extra
wmake -h global
wmake -h arch=%extra_arch%
wmake -h arch=%extra_arch% save
wmake -h global
wmake -h arch=%extra_arch% test1=
wmake -h arch=%extra_arch% test1= save
wmake -h global
wmake -h arch=%extra_arch% test2=
wmake -h arch=%extra_arch% test2= save
wmake -h global
wmake -h arch=%extra_arch% test3=
wmake -h arch=%extra_arch% test3= save
wmake -h global
wmake -h arch=%extra_arch% test4=
wmake -h arch=%extra_arch% test4= save
wmake -h global
wmake -h arch=%extra_arch% test5=
wmake -h arch=%extra_arch% test5= save
wmake -h global
wmake -h arch=%extra_arch% test6=
wmake -h arch=%extra_arch% test6= save
wmake -h global
wmake -h arch=%extra_arch% test7=
wmake -h arch=%extra_arch% test7= save
wmake -h global
wmake -h arch=%extra_arch% test8=
wmake -h arch=%extra_arch% test8= save
wmake -h global
wmake -h arch=%extra_arch% test9=
wmake -h arch=%extra_arch% test9= save
wmake -h global
:no_extra
popd

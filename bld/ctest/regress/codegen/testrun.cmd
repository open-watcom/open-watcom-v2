pushd
if exist *.sav del /f *.sav
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
wmake -h test10=
wmake -h test10= save
wmake -h global
wmake -h test11=
wmake -h test11= save
rem
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
wmake -h global
wmake -h test4= arch=%extra_arch%
wmake -h test4= arch=%extra_arch% save
wmake -h global
wmake -h test5= arch=%extra_arch%
wmake -h test5= arch=%extra_arch% save
wmake -h global
wmake -h test6= arch=%extra_arch%
wmake -h test6= arch=%extra_arch% save
wmake -h global
wmake -h test7= arch=%extra_arch%
wmake -h test7= arch=%extra_arch% save
wmake -h global
wmake -h test8= arch=%extra_arch%
wmake -h test8= arch=%extra_arch% save
wmake -h global
wmake -h test9= arch=%extra_arch%
wmake -h test9= arch=%extra_arch% save
wmake -h global
wmake -h test10= arch=%extra_arch%
wmake -h test10= arch=%extra_arch% save
wmake -h global
wmake -h test11= arch=%extra_arch%
wmake -h test11= arch=%extra_arch% save
:no_extra
wmake -h global
popd

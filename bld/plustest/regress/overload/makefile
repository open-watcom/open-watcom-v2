plustest_name = overload

!include testenv.mif

exes = &
addr01.$(exe) &
addr02.$(exe) &
addr03.$(exe) &
addr04.$(exe) &
addr05.$(exe) &
addr06.$(exe) &
addr07.$(exe) &
addr08.$(exe) &
array03.$(exe) &
class01.$(exe) &
class02.$(exe) &
class03.$(exe) &
class04.$(exe) &
class05.$(exe) &
class06.$(exe) &
class07.$(exe) &
class08.$(exe) &
class09.$(exe) &
class10.$(exe) &
class11.$(exe) &
class12.$(exe) &
cm01.$(exe) &
cv01.$(exe) &
cv02.$(exe) &
cv03.$(exe) &
cv04.$(exe) &
cv05.$(exe) &
cv06.$(exe) &
cv07.$(exe) &
cv08.$(exe) &
defarg02.$(exe) &
defarg03.$(exe) &
defarg04.$(exe) &
distdef.$(exe) &
func.$(exe) &
intprom.$(exe) &
longlist.$(exe) &
model01.$(exe) &
nfptr01.$(exe) &
nfptr02.$(exe) &
nfref01.$(exe) &
nfref02.$(exe) &
nfref03.$(exe) &
nfref04.$(exe) &
nfvoid.$(exe) &
null01.$(exe) &
null02.$(exe) &
pmem01.$(exe) &
pmem02.$(exe) &
pmem03.$(exe) &
ptrfun01.$(exe) &
ptrfun02.$(exe) &
ptrfun03.$(exe) &
ptrfun04.$(exe) &
ptrfun05.$(exe) &
ptrfun06.$(exe) &
ptrfun07.$(exe) &
ptrfun08.$(exe) &
ptrmem01.$(exe) &
stdcnv.$(exe) &
stdcnv02.$(exe) &
tmplt1.$(exe) &
tmplt3.$(exe) &
tq01.$(exe) &
tq02.$(exe) &
tq03.$(exe) &
tq05.$(exe) &
tq06.$(exe) &
tq07.$(exe) &
tq08.$(exe) &
tq09.$(exe) &
tq10.$(exe) &
tq11.$(exe) &
tq13.$(exe) &
trivcnv.$(exe) &
udcnv.$(exe) &
udcnv2.$(exe) &
udcnv3.$(exe) &
udcnv4.$(exe) &
udcnv5.$(exe) &
udcnv6.$(exe) &
udcnv7.$(exe) &
udcnv8.$(exe) &
udcnv9.$(exe) &
udcnv10.$(exe) &
udcnv11.$(exe) &
udcnv12.$(exe) &
udcnv13.$(exe) &
udcnv14.$(exe) &
udcnv15.$(exe) &
udcnv16.$(exe) &
udcnv17.$(exe) &
udcnv18.$(exe) &
udcnv19.$(exe) &
oper01.$(exe) &
oper02.$(exe)

#
# special options
#
opt_ptrfun04 = -za
opt_ptrfun05 = -za
opt_cv08 = -xbov1

!ifdef test1
!   define test_flags -d2
alt_error=01
!else ifdef test2
!   define test_flags -ox-d1-w0-j
alt_error=02
!else
!   define test_flags -ox-d1-w0-otexanr
alt_error=00
!endif

stack_386=opt stack=16k
stack_i86=opt stack=8k
stack_axp=opt stack=16k

test : .symbolic clean start_test test.lnk $(exes)
!ifeq arch i86
    # There are certain unavoidable differences between 16-bit and
    # 32-bit platforms, causing slightly different (but valid) results
    diff $(%ERROR_FILE) overload.i86
!else
    diff $(%ERROR_FILE) overload.chk
!endif
    %append $(log_file) PASS $(%__CWD__)
    @%make clean

start_test : .symbolic
    @if exist s$(arch)_$(alt_error).sav rm s$(arch)_$(alt_error).sav
    @set ERROR_FILE=exec.out
    %create $(%ERROR_FILE)

save : .symbolic
    @if exist exec.out cp exec.out s$(arch)_$(alt_error).sav

clean : .procedure
    @%make common_clean

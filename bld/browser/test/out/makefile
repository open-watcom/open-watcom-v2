.extensions
.extensions: .mbr .dw .cpp .tim

compile = wpp -db -zq
patchdir = ..\patch
srcdir = ..\src

template =      &
flarry.mbr      &
clarry.mbr      &
flattice.mbr    &
clattice.mbr    &
cpenta.mbr      &
fpenta.mbr      &
cstar.mbr       &
fstar.mbr       &
cpeter.mbr      &
fpeter.mbr      &
cparenty.mbr    &
fparenty.mbr    &
cchildy.mbr     &
fchildy.mbr     &
cface3.mbr      &
fface3.mbr      &
cmultlvl.mbr    &
fmultlvl.mbr    &
ccube.mbr       &
fcube.mbr       &
csimple.mbr     &
fsimple.mbr     &
cwide.mbr       &
fwide.mbr       &
cdiag.mbr       &
fdiag.mbr       &
ccenter.mbr     &
fcenter.mbr     &
csibw.mbr       &
fsibw.mbr       &
ios.mbr         &
enum.mbr        &
colour.mbr

compile.tim : $(template)
    wtouch compile.tim
    @if exist *.dw rm *.dw
    @if exist *.obj rm *.obj

.cpp: ..\src
.cpp.mbr:
    *$(compile) $[@

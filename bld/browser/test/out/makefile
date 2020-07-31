.extensions
.extensions: .mbr .dw .cpp

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

all: $(template) .SYMBOLIC

.cpp: ..\src

.cpp.mbr:
    *wpp -db -zq $[@

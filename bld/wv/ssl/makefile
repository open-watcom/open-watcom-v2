#pmake: build
!include ../make.wv

bin = bin

.extensions:
.extensions: .prs .ssl

!ifndef on_build_machine
verbose = /v
!endif


all : fortran.prs cpp.prs c.prs java.prs .SYMBOLIC

common = dbgintr.ssl dbgrules.ssl

fortran.prs : fortran.ssl $(common)

cpp.prs : cpp.ssl pp_c.ssl $(common)

c.prs : c.ssl pp_c.ssl $(common)

java.prs : java.ssl pp_c.ssl $(common)

.ssl.prs
        ssl $(verbose) $[@ $^@
!ifndef on_build_machine
        @copy $^@ $(bdir) >nul
!endif

lexusbld : .symbolic c.prs cpp.prs java.prs
    @newer c.prs $(lexlib_dir)\tools\system "^copy &f $#f > nul"
    @newer cpp.prs $(lexlib_dir)\tools\system "^copy &f $#f > nul"
    @newer java.prs $(lexlib_dir)\tools\system "^copy &f $#f > nul"

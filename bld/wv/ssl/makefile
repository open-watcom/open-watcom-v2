#pmake: build os_dos os_os2 os_win os_nt os_linux

!include cproj.mif

.extensions:
.extensions: .prs .ssl

!ifeq release 0
ssl_verbose = -v
!endif

all : fortran.prs cpp.prs c.prs java.prs .SYMBOLIC

common = dbgintr.ssl dbgrules.ssl

fortran.prs : fortran.ssl $(common)

cpp.prs : cpp.ssl pp_c.ssl $(common)

c.prs : c.ssl pp_c.ssl $(common)

java.prs : java.ssl pp_c.ssl $(common)

.ssl.prs
        ssl $(ssl_verbose) $[@ $^@

clean: .SYMBOLIC
    rm -f *.prs

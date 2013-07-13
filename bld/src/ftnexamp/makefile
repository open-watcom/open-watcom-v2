#pmake: build os_win cpu_i86 cpu_386

host_os  = win
host_cpu = i86

proj_name=src

!include cproj.mif

windll/cover16.obj : windll/cover16.c
    $(cc) $(cflags) $(inc_path) -fo=$@ $[@

clean : .SYMBOLIC
    rm -f windll/cover16.obj

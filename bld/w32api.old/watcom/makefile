##pmake: build os_nt cpu_386


all: lastbld .symbolic

./mkmh.exe: mkmh.c
        bwcl386 -wx -we -zq mkmh.c -fe=$@ -fo=.obj

lastbld: ./mkmh.exe
        $<
        cd win
        wmake
        cd ..
        wtouch $@
        
clean: .symbolic
        rm lastbld master.mif deps.mif mkmh.obj mkmh.exe
        -rm -fr win watcom

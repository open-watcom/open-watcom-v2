all : .symbolic run.exe runnt.exe

run.exe : run.c
        wcc -bt=windows -ml -i$(lang_root)\h\win run.c -d2 -w4
        wlink deb all sys windows file run.obj op map

runnt.exe : run.c
        wcc386 -bt=nt -mf -i$(lang_root)\h\nt run.c -d2 -w4 /fo=runnt.obj
        wlink deb all sys nt_win file runnt.obj op map

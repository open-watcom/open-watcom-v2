.EXTENSIONS:
.EXTENSIONS: .exe
.EXTENSIONS: .obj
.EXTENSIONS: .c

WIN386LINK = sys win386 op st=64k

all: div0.exe gp.exe invins.exe stack.exe .SYMBOLIC
        @%null

div0.exe : div0.obj
        wlink d a $(WIN386LINK) file div0.obj name div0
        wbind -n div0.exe
        @erase div0.rex

gp.exe : gp.obj
        wlink d a $(WIN386LINK) file gp.obj name gp
        wbind -n gp.exe
        @erase gp.rex

invins.exe : invins.obj
        wlink d a $(WIN386LINK) file invins.obj name invins
        wbind -n invins.exe
        @erase invins.rex

stack.exe : stack.obj
        wlink d a $(WIN386LINK) file stack.obj name stack
        wbind -n stack.exe
        @erase stack.rex

.obj : obj
.c : ..\src

.c.obj:
        wcc386p -zw -d2 $[* -fo=.obj

.EXTENSIONS:
.EXTENSIONS: .exe
.EXTENSIONS: .3bj .obj
.EXTENSIONS: .c

WIN386LINK = sys nt_win

all: div0.exe gp.exe invins.exe msg.exe stack.exe .SYMBOLIC
        @%null

div0.exe : div0.3bj
        wlink d a $(WIN386LINK) file div0.3bj name div0

gp.exe : gp.3bj
        wlink d a $(WIN386LINK) op map file gp.3bj name gp

invins.exe : invins.3bj
        wlink d a $(WIN386LINK) file invins.3bj name invins

msg.exe : msg.3bj
        wlink d a $(WIN386LINK) file msg.3bj name msg

stack.exe : stack.3bj
        wlink d a $(WIN386LINK) file stack.3bj name stack


.obj : obj
.3bj : obj
.c : ..\src


.c.3bj:
        wcc386 -i=d:\utils\watcom\nt_h -bt=nt -d2 $[* -fo=.3bj

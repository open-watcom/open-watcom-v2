.EXTENSIONS:
.EXTENSIONS: .exe
.EXTENSIONS: .obj
.EXTENSIONS: .c

WIN386LINK = sys win386 op st=64k

all: div0.exe gp.exe invins.exe stack.exe .SYMBOLIC
        @%null

div0.exe : div0.obj
        wlink d a sys windows font mem file div0 name div0

gp.exe : gp.obj
        wlink d a sys windows font mem file gp name gp

stack.exe : stack.obj
        wlink d a sys windows font mem file stack name stack

invins.exe : invins.obj
        wlink d a sys windows font mem file invins name invins

.obj : obj
.c : ..\src

.c.obj:
        wcc -zw -d2 $[* -fo=.obj

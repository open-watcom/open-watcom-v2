all:	kbhits.obj kbhitr.obj

kbhits.obj : kbhit.asm
 wasm -bt=DOS -ms -3s_ kbhit.asm -fo=kbhits.obj
 copy kbhits.obj d:\watcom\lib386\dos

kbhitr.obj : kbhit.asm
 wasm -bt=DOS -ms -3r_ kbhit.asm -fo=kbhitr.obj
 copy kbhitr.obj d:\watcom\lib386\dos


all : test01.bin test02.com

# Note: This one can't be converted to a valid .com executable
test01.bin : test01.exe
    exe2bin -q -x -l=0 $< $@

test01.exe : test01.obj
    wlink op quiet format dos com file $< name $@

test01.obj : test01.asm
    wasm -zq -wx $<
    
test02.com : test02.exe
    exe2bin -q $< $@
    
test02.exe : test02.obj
    wlink op quiet format dos com file $< name $@

test02.obj : test02.asm
    wasm -zq -wx $<
    
clean : .symbolic
    @rm -f *.obj
    @rm -f *.exe
    @rm -f *.com
    @rm -f *.bin

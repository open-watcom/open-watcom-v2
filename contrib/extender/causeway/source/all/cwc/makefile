ALL:		cwc.exe

cwc.exe:	cwc.asm copystub.asm decstub.asm
 ml copystub.asm
 makeasm copystub.com
 ml decstub.asm
 makeasm decstub.com
 ml -c /DENGLISH=1 /Zf /Zd cwc.asm
 cwl /m /sy /flat cwc
 cwc /l245 cwc.exe

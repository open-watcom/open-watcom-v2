#pmake: build os2 lcl 32 support x86
#
# Create OS2V2HLP.EXE
#

!ifndef bin_root
!ifdef bin_root_wv
bin_root = $(bin_root_wv)
!endif
!endif

w : .symbolic $(bin_root)\binp\dll\os2v2hlp.exe
        @%null

$(bin_root)\binp\dll\os2v2hlp.exe : os2v2hlp.obj
        $(linker) system os2v2 lx op dosseg name $^@ f os2v2hlp &
                imp DOS32WRITE DOSCALLS.282,DOS32DEBUG DOSCALLS.317

os2v2hlp.obj: ..\asm\os2v2hlp.asm
        $(wasm) -zq -3r ..\asm\os2v2hlp

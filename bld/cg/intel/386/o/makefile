#pmake: all i386 intel 32 host32

!include $(cg_dir)\intel\386\mif\386.mif
!include $(cg_dir)\intel\mif\intel.mif
!include $(cg_dir)\mif\cg.mif

!ifndef default_cglib
default_cglib = cgos2386.lib
!endif

cg_target = 386
target_objs = $(386_objs) $(intel_objs) $(cg_objs)

.OPTIMIZE

!ifdef on_build_machine
target : cg386.lib cgstubs.lib cgdos386.lib cgos2386.lib cgnt386.lib cgosi386.lib cgqnx386.lib $(dump_key) .SYMBOLIC
        @%null
!else
target : cg386.lib cgstubs.lib $(default_cglib) .SYMBOLIC
        @%null
!endif

!include $(cg_dir)\mif\hostos.mif
!include $(cg_dir)\mif\rules.mif

cg386.lib : $(target_objs) $(mem_objs) $(cg_dump_objs)
        @%make libfile

cgstubs.lib : $(cg_stub_objs)
        @%make libfile

cgos2386.lib : $(os2_objs)
        @%make libfile

cgosi386.lib : $(osi_objs)
        @%make libfile

cgnt386.lib : $(nt_objs)
        @%make libfile

cgqnx386.lib : $(qnx_objs)
        @%make libfile

cgdos386.lib : $(dos_objs)
        @%make libfile

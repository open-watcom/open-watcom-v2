#pmake: build lcl os_win cpu_i86 cpu_386 trap x86

host_os  = win
host_cpu = i86

#
# Create the Windows 3.x std.dll
#

os      = win
srv     = lcl
cpu     = x86
which = TRAP

extra_objs = &
        initfini.obj &
        globals.obj &
        fault.obj &
        int.obj &
        notify.obj &
        mem.obj &
        getsaddr.obj &
        dbgrmsg.obj &
        dbgeemsg.obj &
        dbghook.obj &
        wgod.obj &
        misc.obj &
        accload.obj &
        accregs.obj &
        accmap.obj &
        accrun.obj &
        accbrwat.obj &
        accredir.obj &
        accmisc.obj &
        accgen.obj &
        ismod32.obj &
        getcsip.obj &
        out.obj &
        dosfile.obj &
        dosrfx.obj &
        dossplit.obj &
        x86cpu.obj &
        misc7086.obj &
        supp.obj &
        core.obj

#       export ENUMTASKWINDOWSFUNC.10 &
extra_linker = &
        import SETEVENTHOOK USER.321 &
        import ToolHelpHook KERNEL &
        export NOTIFYHANDLER.9 &
        export DEBUGHOOK.11 &
        option heapsize=1k &
        segment CLASS 'CODE' PRELOAD FIXED &
        segment CLASS 'FAR_DATA' PRELOAD FIXED &
        segment CLASS 'DATA' MOVEABLE &
        segment CLASS 'BEGDATA' MOVEABLE &
        segment CLASS 'BSS' MOVEABLE

extra_srcs += ;$(trap_dir)/lcl/dos/c;$(sdk_misc_dir)
inc_dirs += -I"$(trap_dir)/lcl/dos/h" -I"$(sdk_misc_dir)"
inc_dirs += -I"$(commonui_dir)/h" -I"$(wpi_dir)/h"

extra_c_flags_i86 = -3

.c: ../c;$(commonui_dir)/c

!include $(trap_dir)/lcl/lcl.mif
!include $(trap_dir)/master.mif

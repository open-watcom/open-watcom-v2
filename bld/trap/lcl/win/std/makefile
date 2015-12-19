#pmake: build lcl os_win cpu_i86 cpu_386 trap x86

host_os  = win
host_cpu = i86

target_cpu = x86

sys_windowed = 1

#
# Create the Windows 3.x std.dll
#

os      = win
srv     = lcl
which = TRAP

extra_objs = &
        initfini.obj &
        globals.obj &
        fault.obj &
        int.obj &
        notify.obj &
        dbgrmsg.obj &
        dbgeemsg.obj &
        dbghook.obj &
        wgod.obj &
        accmem.obj &
        accload.obj &
        accregs.obj &
        accmap.obj &
        accrun.obj &
        accbrwat.obj &
        accredir.obj &
        accmisc.obj &
        accgen.obj &
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

!include $(commonui_dir)/windbg.mif

inc_dirs = -I"$(trap_dir)/lcl/win/h" $(windbg_inc_dirs)
extra_libs = $(windbg_lib)

!include $(trap_dir)/lcl/win/win.mif

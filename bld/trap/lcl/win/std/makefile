#pmake: build lcl win 16 32 trap x86
#
# Create the Windows 3.x STD.DLL
#

os      = win
srv     = lcl
cpu     = x86

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
        squish87.obj &
        x86cpu.obj &
        supp.obj &
        core.obj

#       exp ENUMTASKWINDOWSFUNC.10 &
extra_linker = &
        imp SETEVENTHOOK USER.321 &
        imp ToolHelpHook KERNEL &
        exp NOTIFYHANDLER.9 &
        exp DEBUGHOOK.11 &
        lib toolhelp &
        option heapsize=1k &
        segment CLASS 'CODE' PRELOAD FIXED &
        segment CLASS 'FAR_DATA' PRELOAD FIXED &
        segment CLASS 'DATA' MOVEABLE &
        segment CLASS 'BEGDATA' MOVEABLE &
        segment CLASS 'BSS' MOVEABLE &


extra_srcs = $(trap_dir)\lcl\dos\c;$(sdk_dir)\misc
extra_incs = $(trap_dir)\lcl\dos\h;$(sdk_dir)\misc

!include $(trap_dir)\lcl\lcl.mif
!include $(trap_dir)\trap.mif

#pmake: build lcl os_os2 cpu_386 cpu_386 trap x86

host_os  = os2
host_cpu = 386

target_cpu = x86
target_os  = os2v2

#
# Create the 32-bit OS/2 std.dll
#

srv   = lcl
name  = std
which = TRAP

extra_objs =          &
        accmisc.obj   &
        os2v2acc.obj  &
        os2rfx.obj    &
        pgmexec.obj   &
        softmode.obj  &
        dbgthrd.obj   &
        os2conv.obj   &
        loadhelp.obj  &
        pmlock.obj    &
        x86cpu.obj    &
        supp.obj      &
        core.obj

extra_linker = &
        exp TellHandles_.4,TellHardMode_.5             &
        imp _TempStack wdsplice, DoReadWord_ wdsplice  &
        imp DoWriteWord_ wdsplice, DoClose_ wdsplice   &
        imp DoOpen_ wdsplice, DoWritePgmScrn_ wdsplice &
        imp DoReadXMMRegs_ wdsplice                    &
        imp DoWriteXMMRegs_ wdsplice                   &
        imp _XferBuff wdsplice, DoDupFile_ wdsplice    &
        lib $(os2api_dir)/os2286/lib/os2.lib

!include $(trap_dir)/lcl/lcl.mif
!include $(trap_dir)/master.mif

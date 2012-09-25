#pmake: serv lcl os_dos dsx cpu_386 x86
host_cpu = 386
host_os = nt

os = nt
srv = jvm
cpu = x86
name = jvmhelp
which = SERVER

dall : .symbolic d wdebug.zip
    @%null

wall : .symbolic w wdebug.zip
    @%null

wdebug.zip : JVMHelp.java
    set classpath=.;$(javasdk)/lib/classes.zip
    $(javasdk)/bin/javac JVMHelp.java > JVMHelp.err
    type JVMHelp.err
    !if not exist sun/nul md sun
    !if not exist sun/tools/nul md sun/tools
    !if not exist sun/tools/debug/nul md sun/tools/debug
    cp jvm*.class sun/tools/debug
    rm -f wdebug.zip
    $(bld_bin)/zip -r -0 wdebug.zip sun/*.*
!ifdef lexusbld
    newer wdebug.zip $(lexlib_dir)/tools/system/wdebug.zip "^copy &f $#f > nul"
!endif
    rm -rf sun

extra_srcs = $(trap_dir)/lcl/nt/c
inc_dirs = -I"$(trap_dir)/lcl/nt/h" -I"$(trap_dir)/lcl/dos/h"
extra_objs = $(jvmx_serv_objs) jvmacc.obj &

ji_dll  = 'javai_g.dll'

imports = EE $(ji_dll) &
        ,FindClass $(ji_dll) &
        ,execute_java_static_method $(ji_dll) &
        ,do_execute_java_method $(ji_dll) &
        ,makeJavaString $(ji_dll) &
        ,ArrayAlloc $(ji_dll) &
        ,set_single_stepping $(ji_dll) &
        ,sysAtexit $(ji_dll) &
        ,get_binclasses $(ji_dll) &
        ,get_nbinclasses $(ji_dll) &
        ,java_main $(ji_dll) &
        ,monitorWait $(ji_dll) &
        ,monitorEnter $(ji_dll) &
        ,monitorExit $(ji_dll) &

extra_c_flags = -dJVM -dJVMXHELP -bd -I"$(javasdk)/include" -I"$(javasdk)/include/win32" -I"$(java_src_dir)/src/win32/java/include" -fh
extra_linker = op map,verbose import $(imports)

!include $(trap_dir)/lcl/jvm/jvm.mif
!include $(trap_dir)/master.mif

#pmake: serv lcl dos dsx 32 x86

!include $(build_dir)\mif\bhost.mif

os = nt
srv = jvm
cpu = x86
name = jvmhelp

dall : .symbolic d $(bin_root)\binnt\wdebug.zip
    @%null

wall : .symbolic w $(bin_root)\binnt\wdebug.zip
    @%null

$(bin_root)\binnt\wdebug.zip : JVMHelp.java
    set classpath=.;$(javasdk)\lib\classes.zip
    $(javasdk)\bin\javac JVMHelp.java > JVMHelp.err
    type JVMHelp.err
    !if not exist sun\nul md sun
    !if not exist sun\tools\nul md sun\tools
    !if not exist sun\tools\debug\nul md sun\tools\debug
    copy jvm*.class sun\tools\debug
    if exist $(bin_root)\binnt\wdebug.zip del $(bin_root)\binnt\wdebug.zip
    $(bld_bin)\zip -r -0 $(bin_root)\binnt\wdebug.zip sun\*.*
!ifdef lexusbld
    newer $(bin_root)\binnt\wdebug.zip $(lexlib_dir)\tools\system\wdebug.zip "^copy &f $#f > nul"
!endif
    rm -rf sun

extra_srcs = $(trap_dir)\lcl\nt\c
extra_incs = $(trap_dir)\lcl\nt\h;$(trap_dir)\lcl\dos\h
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

extra_cflags = -dJVM -dJVMXHELP -bd -i$(javasdk)\include;$(javasdk)\include\win32;$(java_src_dir)\SRC\WIN32\JAVA\INCLUDE -fh
extra_linker = op map,verbose import $(imports)

!include $(trap_dir)\lcl\jvm\jvm.mif
!include $(trap_dir)\serv.mif

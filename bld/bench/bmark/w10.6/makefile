#pmake: bmark bytemark integer int small w106

host_os  = nt
host_cpu = 386
compiler = custom

cc  = x:\lang\binnt\wcc386
cpp = x:\lang\binnt\wpp386

.before:
	@set include=$(lang_root)\h;$(lang_root)\h\nt
	
.c.obj:
	$(cc) $(cflags_watcom) $(extra_cflags) $(extra_defines) $[@

.cpp.obj:
	$(cpp) $(cflags_watcom) $(extra_cflags) $(extra_defines) $[@

deflink = wlink $(lflags_watcom) name $^. file { $< } lib $(bench_dir)\support\watcom\support.lib

!include ..\bmark.mif

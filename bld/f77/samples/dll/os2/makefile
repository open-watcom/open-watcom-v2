dlltest.exe:	dlltest.for dllsamp.dll
	wfl386 -q -l=os2v2 dlltest dllsamp.lib

dlltest.obj:	dlltest.for
	wfc386 -q dlltest

dllsamp.dll:	dllsamp.obj
	%create dllsamp.lnk
	%append dllsamp.lnk system os2v2_dll initinstance terminstance
	%append dllsamp.lnk option quiet, implib, manyautodata
	%append dllsamp.lnk export DLL_ENTRY_1
	%append dllsamp.lnk export DLL_ENTRY_2
	%append dllsamp.lnk file dllsamp
	wlink @dllsamp.lnk
	%erase dllsamp.lnk

dllsamp.obj:	dllsamp.for
	wfc386 -q -bd dllsamp

clean: .symbolic
	rm -f *.obj *.dll *.lib *.exe

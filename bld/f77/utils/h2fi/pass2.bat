@echo off
if [.%1] == [.] goto help
if [.%2] == [.] goto help
echo ==== beginning pass 2 ====
call run2 %1basedef.h %2basedef.fi
call run2 %1basemac.h %2basemac.fi
call run2 %1basemid.h %2basemid.fi
call run2 %1bdcalls.h %2bdcalls.fi
call run2 %1bse.h %2bse.fi
call run2 %1bsedev.h %2bsedev.fi
call run2 %1bsedos.h %2bsedos.fi
call run2 %1bseerr.h %2bseerr.fi
call run2 %1bsememf.h %2bsememf.fi
call run2 %1bseord.h %2bseord.fi
call run2 %1bsesub.h %2bsesub.fi
call run2 %1bsetib.h %2bsetib.fi
call run2 %1bsexcpt.h %2bsexcpt.fi
call run2 %1cnrtree.h %2cnrtree.fi
call run2 %1hmtailor.h %2hmtailor.fi
call run2 %1mi.h %2mi.fi
call run2 %1os2.h %2os2.fi
call run2 %1os2def.h %2os2def.fi
call run2 %1os2nls.h %2os2nls.fi
call run2 %1os2std.h %2os2std.fi
call run2 %1pm.h %2pm.fi
call run2 %1pmavio.h %2pmavio.fi
call run2 %1pmbitmap.h %2pmbitmap.fi
call run2 %1pmddim.h %2pmddim.fi
call run2 %1pmdev.h %2pmdev.fi
call run2 %1pmerr.h %2pmerr.fi
call run2 %1pmfont.h %2pmfont.fi
call run2 %1pmgpi.h %2pmgpi.fi
call run2 %1pmhelp.h %2pmhelp.fi
call run2 %1pmmle.h %2pmmle.fi
call run2 %1pmord.h %2pmord.fi
call run2 %1pmpic.h %2pmpic.fi
call run2 %1pmsei.h %2pmsei.fi
call run2 %1pmshl.h %2pmshl.fi
call run2 %1pmspl.h %2pmspl.fi
call run2 %1pmstddlg.h %2pmstddlg.fi
call run2 %1pmtypes.h %2pmtypes.fi
call run2 %1pmwin.h %2pmwin.fi
call run2 %1pmwp.h %2pmwp.fi
call run2 %1rexxsaa.h %2rexxsaa.fi
goto done
:help
echo usage: pass2 path_to_h path_to_fi
echo for example: pass2 h\ fi\
:done

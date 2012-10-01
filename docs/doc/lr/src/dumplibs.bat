if not exist l:\dlang\lib286\DOS\clibm.lib goto error
if not exist l:\dlang\lib286\OS2\clibm.lib goto error
if not exist l:\dlang\lib286\OS2\clibdll.lib goto error
if not exist l:\dlang\lib286\OS2\clibmtl.lib goto error
if not exist l:\dlang\lib286\OS2\dospmm.lib goto error
if not exist l:\dlang\lib286\WIN\clibm.lib goto error
if not exist l:\dlang\lib286\QNX\clibm.lib goto error
if not exist l:\dlang\lib386\DOS\clib3r.lib goto error
if not exist l:\dlang\lib386\NT\clib3r.lib goto error
if not exist l:\dlang\lib386\OS2\clib3r.lib goto error
if not exist l:\dlang\lib386\WIN\clib3r.lib goto error
if not exist l:\dlang\lib386\QNX\clib3r.lib goto error
if not exist l:\dlang\lib386\math3r.lib goto error
if not exist l:\dlang\lib386\kanji3r.lib goto error
if not exist l:\dlang\lib386\dos\graph.lib goto error
if not exist l:\dlang\lib386\NETWARE\clib3s.lib goto error
goto doit
:error
echo Error: missing libraries
goto done
:doit
copy dumplibs.edi wedit.prf
wlib l:\dlang\lib286\DOS\clibm /l=dos16.lst
dedit dos16.lst
wlib l:\dlang\lib286\OS2\clibm /l=os216.lst
dedit os216.lst
wlib l:\dlang\lib286\OS2\clibdll /l=os216dl.lst
dedit os216dl.lst
wlib l:\dlang\lib286\OS2\clibmtl /l=os216mt.lst
dedit os216mt.lst
wlib l:\dlang\lib286\OS2\dospmm /l=dospm.lst
dedit dospm.lst
wlib l:\dlang\lib286\WIN\clibm /l=win16.lst
dedit win16.lst
wlib l:\dlang\lib286\QNX\clibm /l=qnx16.lst
dedit qnx16.lst
wlib l:\dlang\lib386\DOS\clib3r /l=dos32.lst
dedit dos32.lst
wlib l:\dlang\lib386\NT\clib3r /l=win32.lst
dedit win32.lst
wlib l:\dlang\lib386\OS2\clib3r /l=os232.lst
dedit os232.lst
wlib l:\dlang\lib386\WIN\clib3r /l=win386.lst
dedit win386.lst
wlib l:\dlang\lib386\QNX\clib3r /l=qnx32.lst
dedit qnx32.lst
wlib l:\dlang\lib386\math3r /l=math.lst
dedit math.lst
wlib l:\dlang\lib386\kanji3r /l=kanji.lst
dedit kanji.lst
wlib l:\dlang\lib386\dos\graph /l=graph.lst
dedit graph.lst
copy macro.txt macro.lst
erase wedit.prf
copy dumplibs.ed2 wedit.prf
wlib l:\dlang\lib386\NETWARE\clib3s /l=net32.lst
dedit net32.lst
erase wedit.prf
erase t.t
:done

xcopy h\*.h	    %rlib%\ui\h
xcopy dos\*.lib	    %rlib%\ui\dos
xcopy nlm\*.lib	    %rlib%\ui\nlm
xcopy nt\*.lib	    %rlib%\ui\nt
xcopy os2\*.lib	    %rlib%\ui\os2
xcopy qnx\*.lib	    %rlib%\ui\qnx
for %%f in (qnx\tix\*.tix) do eol -u %%f %wsql%\%%f

if [%rlib%] == [] goto done

    rem Headers

    xcopy h\*.h	    %rlib%\ui\h\
    xcopy dos\h\*.h	    %rlib%\ui\dos\h\
    xcopy nlm\h\*.*     %rlib%\ui\nlm\h\
    xcopy os2\h\*.h	    %rlib%\ui\os2\h\
    xcopy qnx\h\*.h	    %rlib%\ui\qnx\h\
    rem No NT headers

    rem Libraries

    xcopy dos\*.lib	    %rlib%\ui\dos\
    xcopy nlm\*.lib	    %rlib%\ui\nlm\
    xcopy nt\*.lib	    %rlib%\ui\nt\
    xcopy os2\*.lib	    %rlib%\ui\os2\
    xcopy qnx\*.lib	    %rlib%\ui\qnx\
    
    rem Import files
    xcopy nlm\*.imp	    %rlib%\ui\nlm

:done

rem QNX .tix files

rem for %%f in (qnx\tix\*.tix) do eol -u %%f %wsql%\%%f

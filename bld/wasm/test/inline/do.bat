echo %1 ================================================================
set cmplr=%2
%cmplr% %1 /s /ot /zm /zq
wdisasm %1 /l
cat %1.lst >> %3
set cmplr=

[ LOG dbgcopy.log ]
set docmd=copy
set destdir=q:\wc11.b
set relroot=c:\

<docmd> <relroot>\rel2\binw\wd.exe             <destdir>\binw\wd.exe 
 wtouch -f timestmp.fil <destdir>\binw\wd.exe
 
set destdir=q:\wf11.b

<docmd> <relroot>\rel2\binw\wd.exe             <destdir>\binw\wd.exe 
 wtouch -f timestmp.fil <destdir>\binw\wd.exe

set destdir=e:\publish\wc11.j

<docmd> <relroot>\rel2\binw\wd.exe             <destdir>\binw\wd.exe 
 wtouch -f timestmp.fil <destdir>\binw\wd.exe
 
set destdir=e:\publish\wf11.j

<docmd> <relroot>\rel2\binw\wd.exe             <destdir>\binw\wd.exe 
 wtouch -f timestmp.fil <destdir>\binw\wd.exe

set docmd=copy
set relroot=c:\
 
set destdir=q:\wc11.a
set f77dir=q:\wf11.a
set jdestdir=q:\wc11.j
set jf77dir=q:\wf11.j
 
<docmd> <relroot>\rel2\binw\ide.cfg            <destdir>\binw\ide.cfg 
 wtouch -f timestmp.fil <destdir>\binw\ide.cfg
 
<docmd> <relroot>\rel2\binw\ide.cfg            <f77dir>\binw\ide.cfg 
 wtouch -f timestmp.fil <f77dir>\binw\ide.cfg
 
<docmd> <relroot>\rel2\binw\ide.cfg            <jdestdir>\binw\ide.cfg 
 wtouch -f timestmp.fil <jdestdir>\binw\ide.cfg
 
<docmd> <relroot>\rel2\binw\ide.cfg            <jf77dir>\binw\ide.cfg 
 wtouch -f timestmp.fil <jf77dir>\binw\ide.cfg

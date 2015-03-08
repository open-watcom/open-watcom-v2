echo "( set date 'April 28, 2009'" >wgml.opt
echo "( set time '21:11:02'" >>wgml.opt
echo "( wscript" >>wgml.opt
echo "( dev ps" >>wgml.opt
echo "( pass 1" >>wgml.opt
echo "( verbose warn stat incl" >>wgml.opt
echo "( index" >>wgml.opt

export GMLINC=$OWROOT/docs/doc/devguide:$OWROOT/docs/doc/gml:$OWROOT/docs/doc/whelp:$OWROOT/docs/doc/hlp
export GMLLIB=$OWROOT/docs/gml/syslib:$OWROOT/docs/doc/whelp:$OWROOT/docs/doc/hlp

WGML_DIR=linux386

fn=$1
shift
# tsys06.gml tsyssys.gml
   fb=${fn%.*}
   echo $OWDOSBOX
if [ -n "$OWDOSBOX" ]; then 
#   create DOSBOX batch script for run wgml on 64-bit Windows under DOSBOX
   echo "d:" > wgml.bat
   echo "set GMLINC=c:\doc\devguide;c:\doc\gml;c:\doc\whelp;c:\doc\hlp" >> wgml.bat
   echo "set GMLLIB=c:\gml\syslib;c:\doc\whelp;c:\doc\hlp;.\testlib" >> wgml.bat
   echo "c:\gml\dos\wgml.exe $fn ( file wgml.opt out $fb.ops %1 %2 %3 %4 %5 %6 %7 %8 %9 >$fb.old" >> wgml.bat
   echo "exit" >> wgml.bat
   $OWDOSBOX -noautoexec -c "mount c $OWROOT\docs" -c "mount d ." -c "mount e $OWROOT\bld" -c "d:wgml.bat" -noconsole
else
   echo "lredir w: linux\fs$OWDOCSDIR" > wgml.bat
   echo "set GMLINC=w:\doc\devguide;w:\doc\gml;w:\doc\whelp;w:\doc\hlp" >> wgml.bat
   echo "set GMLLIB=w:\gml\syslib;w:\doc\whelp;w:\doc\hlp;.\testlib" >> wgml.bat
   echo "w:\gml\dos\wgml.exe $fn ( file wgml.opt out $fb.ops %1 %2 %3 %4 %5 %6 %7 %8 %9 >$fb.old" >> wgml.bat
   echo "exitemu" >> wgml.bat
   dosemu -dumb wgml.bat
fi
   $OWROOT/bld/wgml/$WGML_DIR/wgml.exe $fn "(" file wgml.opt out $fb.nps $1 $2 $3 $4 $5 $6 $7 $8 $9 >$fb.ntr
   $OWROOT/bld/wgml/$WGML_DIR/wgml.exe $fn -r "(" file wgml.opt out $fb.nps $1 $2 $3 $4 $5 $6 $7 $8 $9 >$fb.new

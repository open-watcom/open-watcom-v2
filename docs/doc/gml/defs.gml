.*
.* Open Watcom C/C++ and FORTRAN 77 Documentation Variables
.*
.*  Symbolic Name Definitions
.*
.if &e'&target eq 0 .do begin
:set symbol="target"    value="AXPNT".
:set symbol="computer"  value="AXP".
:set symbol="target"    value="DOS".
:set symbol="computer"  value="Intel".
.do end
.if &e'&company eq 0 .do begin
:set symbol="company"   value="Open Watcom".
:set symbol="scompany"  value="Watcom International Corp.".
:set symbol="pcompany"  value="Sybase".
:set symbol="spcompany" value="Sybase, Inc.".
.do end
.*
.if '&lang' eq 'C/C++' .do begin
.   :set symbol="langsuff"   value="c".
.   .*
.   :set symbol="header"     value="header".
.   :set symbol="hdrsuff"    value="h".
.   .*
.   :set symbol="product"    value="&company C/C++".
.   :set symbol="cmpname"    value="&company C/C++".
.   .*
.   :set symbol="ccmd16"     value="wcc".
.   :set symbol="ccmd32"     value="wcc386".
.   :set symbol="ccmdAX"     value="wccaxp".
.   :set symbol="pcmd16"     value="wpp".
.   :set symbol="pcmd32"     value="wpp386".
.   :set symbol="pcmdAX"     value="wppaxp".
.   .*
.   .if '&target' eq 'QNX' .do begin
.   .   :set symbol="cvar16"     value="WCC".
.   .   :set symbol="cvar32"     value="WCC386".
.   .   :set symbol="cvarAX"     value="WCCAXP".
.   .   :set symbol="pvar16"     value="WPP".
.   .   :set symbol="pvar32"     value="WPP386".
.   .   :set symbol="pvarAX"     value="WPPAXP".
.   .   :set symbol="wclname"    value="cc".
.   .   :set symbol="wclvar"     value="CC".
.   .   :set symbol="wclvarup"   value="CC".
.   .   :set symbol="wclcmd"     value="cc".
.   .   :set symbol="wclcmdup"   value="cc".
.   .   :set symbol="hdrdir"     value="/include".
.   .   :set symbol="hdrdirup"   value="/include".
.   .   :set symbol="frcvar"     value="FORCE".
.   .   :set symbol="incvar"     value="INCLUDE".
.   .do end
.   .el .do begin
.   .   :set symbol="cvar16"     value="wcc".
.   .   :set symbol="cvar32"     value="wcc386".
.   .   :set symbol="cvarAX"     value="wccaxp".
.   .   :set symbol="pvar16"     value="wpp".
.   .   :set symbol="pvar32"     value="wpp386".
.   .   :set symbol="pvarAX"     value="wppaxp".
.   .   :set symbol="wclname"    value="&company Compile and Link".
.   .   :set symbol="wclvar16"   value="wcl".
.   .   :set symbol="wclvarup16" value="WCL".
.   .   :set symbol="wclcmd16"   value="wcl".
.   .   :set symbol="wclcmdup16" value="WCL".
.   .   :set symbol="wclvar32"   value="wcl386".
.   .   :set symbol="wclvarup32" value="WCL386".
.   .   :set symbol="wclcmd32"   value="wcl386".
.   .   :set symbol="wclcmdup32" value="WCL386".
.   .   :set symbol="wclvarAX"   value="wclaxp".
.   .   :set symbol="wclvarupAX" value="WCLAXP".
.   .   :set symbol="wclcmdAX"   value="wclaxp".
.   .   :set symbol="wclcmdupAX" value="WCLAXP".
.   .   :set symbol="wcltemp"    value="__WCL__".
.   .   :set symbol="hdrdir"     value="\h".
.   .   :set symbol="hdrdirup"   value="\H".
.   .   :set symbol="frcvar"     value="force".
.   .   :set symbol="incvar"     value="include".
.   .do end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.   :set symbol="langsuff"   value="for".
.   .*
.   :set symbol="header"     value="include".
.   :set symbol="hdrsuff"    value="fi".
.   .*
.   :set symbol="product"    value="&company FORTRAN 77".
.   :set symbol="cmpname"    value="&company F77".
.   .*
.   :set symbol="cvar16"     value="wfc".
.   :set symbol="ccmd16"     value="wfc".
.   .*
.   :set symbol="cvar32"     value="wfc386".
.   :set symbol="ccmd32"     value="wfc386".
.   .*
.   :set symbol="wclname"    value="&company Compile and Link".
.   :set symbol="wclvar16"   value="wfl".
.   :set symbol="wclvarup16" value="WFL".
.   :set symbol="wclcmd16"   value="wfl".
.   :set symbol="wclcmdup16" value="WFL".
.   :set symbol="wclvar32"   value="wfl386".
.   :set symbol="wclvarup32" value="WFL386".
.   :set symbol="wclcmd32"   value="wfl386".
.   :set symbol="wclcmdup32" value="WFL386".
.   :set symbol="wcltemp"    value="__WFL__".
.   :set symbol="hdrdir"     value="\src\fortran".
.   :set symbol="hdrdirup"   value="\SRC\FORTRAN".
.   :set symbol="incvar"     value="finclude".
.do end
.*
.sr langsuffup=&u'&langsuff
.sr hdrsuffup=&u'&hdrsuff
.sr cvarup16=&u'&cvar16
.sr ccmdup16=&u'&ccmd16
.sr cvarup32=&u'&cvar32
.sr ccmdup32=&u'&ccmd32
.sr cvarupAX=&u'&cvarAX
.sr ccmdupAX=&u'&ccmdAX
.if '&lang' eq 'C/C++' .do begin
.sr pvarup16=&u'&pvar16
.sr pcmdup16=&u'&pcmd16
.sr pvarup32=&u'&pvar32
.sr pcmdup32=&u'&pcmd32
.sr pvarupAX=&u'&pvarAX
.sr pcmdupAX=&u'&pcmdAX
.sr frcvarup=&u'&frcvar
.do end
.sr incvarup=&u'&incvar
.*
.if '&lang' eq 'C/C++' .do begin
:set symbol="optdag"    value="".
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
:set symbol="cdate"     value="1997/07/16 09:22:47".
.do end
:cmt. fix problem with using the Script control word separator ";"
.ti 3b fb
.tr fb 3b
.if '&target' eq 'QNX' .do begin
:set symbol="prompt"    value="$ ".
:set symbol="drive"     value="node".
:set symbol="dr1"       value="//0".
:set symbol="dr2"       value="//1".
:set symbol="dr3"       value="".
:set symbol="dr4"       value="//2".
:set symbol="exc"       value="".
:set symbol="exe"       value="".
:set symbol="exeup"     value="".
:set symbol="axt"       value=".a".
:set symbol="obj"       value=".o".
:set symbol="setcmd"    value="export".
:set symbol="setcmdup"  value="export".
:set symbol="setdelim"  value='"'.
.do end
.el .do begin
:set symbol="prompt"    value="C>".
:set symbol="drive"     value="drive".
:set symbol="dr1"       value="a:".
:set symbol="dr2"       value="b:".
:set symbol="dr3"       value="c:".
:set symbol="dr4"       value="d:".
:set symbol="exc"       value=".EXE".
:set symbol="exe"       value=".exe".
:set symbol="exeup"     value=".EXE".
:set symbol="axt"       value=".asm".
:set symbol="obj"       value=".obj".
:set symbol="setcmd"    value="set".
:set symbol="setcmdup"  value="SET".
:set symbol="setdelim"  value="".
.do end
.if '&lang' eq 'C/C++' .do begin
:set symbol="cxt"       value=".c".
:set symbol="xxt"       value=".cpp".
:set symbol="def"       value=".def".
:set symbol="pxt"       value=".i".
:set symbol="hxt"       value=".h".
:set symbol="prf"       value=".prf".
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
:set symbol="cxt"       value=".for".
:set symbol="xxt"       value=".for".
:set symbol="hxt"       value=".fi".
.do end
:set symbol="err"       value=".err".
:set symbol="lib"       value=".lib".
:set symbol="lst"       value=".lst".
.if '&target' eq 'QNX' .do begin
:set symbol="n87var"    value="NO87".
:set symbol="pthvar"    value="PATH".
:set symbol="tmpvar"    value="TMP".
:set symbol="wcmvar"    value="WCGMEMORY".
:set symbol="tmpdirvar" value="TMPDIR".
.do end
.el .do begin
:set symbol="n87var"    value="no87".
:set symbol="pthvar"    value="path".
:set symbol="tmpvar"    value="tmp".
:set symbol="wcmvar"    value="wcgmemory".
.do end
:set symbol="n87varup"  value="NO87".
:set symbol="pthvarup"  value="PATH".
:set symbol="tmpvarup"  value="TMP".
:set symbol="wcmvarup"  value="WCGMEMORY".
.*
:cmt. :set symbol="wcglnam"    value="i86wcgl".
:cmt. :set symbol="wcglnamup"  value="I86WCGL".
:cmt. :set symbol="mmodel"     value="mm".
:cmt. :set symbol="mmname"     value="medium".
.if '&target' eq 'QNX' .do begin
.   :set symbol="libdir"     value="/lib".
.   :set symbol="libdirup"   value="/lib".
:cmt. .   :set symbol="library"    value="/clibs".
.do end
.el .do begin
.   :set symbol="libdir16"   value="\lib286".
.   :set symbol="libdirup16" value="\LIB286".
.   :set symbol="libdir32"   value="\lib386".
.   :set symbol="libdirup32" value="\LIB386".
.   :set symbol="libdirAX"   value="\libaxp".
.   :set symbol="libdirupAX" value="\LIBAXP".
:cmt. .   :set symbol="library"    value="\dos\clibs".
:cmt. .   :set symbol="library"    value="\dos\flib7l".
.do end
.*

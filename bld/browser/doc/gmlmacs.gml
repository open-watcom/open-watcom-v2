.* (c) Copyright 1987, 1989 by WATCOM Publications Limited
.*
.* All rights reserved. No part of this publication may be reproduced or
.* used in any form or by any means - graphic, electronic, or mechanical,
.* including photocopying, recording, taping or information storage and
.* retrieval systems - without written permission of WATCOM Publications
.* Limited.
.*
.* WATCOM Documentation Macros & Variables
.*
:INCLUDE file='SYMBOLS'.
.*
.if '&format' eq '7x9' .do begin
:set symbol='rmargin' value='61'.
.do end
.el .if '&format' eq '8.5x11' .do begin
:set symbol='rmargin' value='60'.
.do end
.el .if '&format' eq '8.5x11a' .do begin
:set symbol='rmargin' value='78'.
.do end
.el .if '&format' eq 'help' .do begin
:set symbol='rmargin' value='78'.
.do end
.el .do begin
:set symbol='rmargin' value='60'.
.do end
.*
.dm id begin
:SF font=4.&*:eSF.
.dm id end
.*
.dm kw begin
.ix &*
.ct
:SF font=3.&*:eSF.
.dm kw end
.*
.dm sy begin
:SF font=1.&*:eSF.
.dm sy end
.*
.dm fi begin
:SF font=4.&*:eSF.
.dm fi end
.*
.dm ev begin
:I1.environment variables:I2.&*
:I1.&* environment variable
:SF font=2.&*:eSF.
.dm ev end
.*
.dm uindex begin
.ix &*
.dm uindex end
.*
.dm figure begin
:FIG place=inline frame=none.
:FIGCAP.&*
:eFIG.
.dm figure end
.*
.dm pict begin
:FIG place=inline frame=none.
..if '&*depth' eq '' ..do begin
:GRAPHIC depth='2.73i' file='&*file..ps'.
..do end
..if '&*depth' ne '' ..do begin
:GRAPHIC depth='&*depth' file='&*file..ps'.
..do end
:FIGCAP.&*text
:eFIG.
.dm pict end
.if &$cmacpass. eq 'ugh' .do begin
.gt picture delete
.do end
.gt picture add pict att
.ga * depth any
.ga * file any
.ga * text any
.*
.dm image begin
:FIG place=inline frame=none.
..if '&*xoff' ne '' ..do begin
:GRAPHIC depth='&*depth' xoff='&*xoff' file='&*file..eps'.
..do end
..el ..do begin
:GRAPHIC depth='&*depth' file='&*file..eps'.
..do end
..if '&*text' ne '' ..do begin
:FIGCAP.&*text
..do end
:eFIG.
.dm image end
.if &$cmacpass. eq 'ugh' .do begin
.gt image delete
.do end
.gt image add image att
.ga * xoff any
.ga * depth any
.ga * file any
.ga * text any
.*
.dm mbox begin
.se tmplvl=&WDWlvl-3
.if '&*1' eq 'on' .do begin
.   .cp &tmplvl
.   .bx on &*2 &*3 &*4 &*5 &*6 &*7 &*8 &*9 &*10
.   :XMP.:SF font=4.
.do end
.el .if '&*' eq 'off' .do begin
.   :eSF.:eXMP.
.   .bx off
.do end
.el .if '&*' eq 'begin' .do begin
.   :P.
.   .cp &tmplvl
.   .se lmargin=&sysin+1
.   .bx on &lmargin &rmargin
.   :XMP.:SF font=4.
.do end
.el .if '&*' eq 'end' .do begin
.   :eSF.:eXMP.
.   .bx off
.do end
.el .do begin
.   .bx
.do end
.dm mbox end
.*
.dm cbox begin
.se tmplvl=&WDWlvl-3
.if '&*1' eq 'on' .do begin
.   .cp &tmplvl
.   .bx on &*2 &*3 &*4 &*5 &*6 &*7 &*8 &*9 &*10
.   :XMP.:SF font=5.
.do end
.el .if '&*' eq 'off' .do begin
.   :eSF.:eXMP.
.   .bx off
.do end
.el .if '&*' eq 'begin' .do begin
.   :P.
.   .cp &tmplvl
.   .se lmargin=&sysin+1
.   .bx on &lmargin &rmargin
.   :XMP.:SF font=5.
.do end
.el .if '&*' eq 'end' .do begin
.   :eSF.:eXMP.
.   .bx off
.do end
.el .do begin
.   .bx
.do end
.dm cbox end
.*
.dm mbigbox begin
.if '&*1' eq 'end' .do begin
.   .if '&format' eq '8.5x11a' .do begin
.   :eFIG.
.   .do end
.   .el .if '&format' eq 'help' .do begin
.   :eFIG.
.   .do end
.   .el .do begin
.   :eSF.:eXMP.
.   .bx off
.   .do end
.do end
.el .do begin
.   :P.
.   .se tmplvl=&WDWlvl-3
.   .cp &tmplvl
.   .if '&format' eq '8.5x11a' .do begin
.   :FIG frame=box place=inline.
.   .sk 1
.   .do end
.   .el .if '&format' eq 'help' .do begin
.   :FIG frame=box place=inline.
.   .sk 1
.   .do end
.   .el .do begin
.   .se lmargin=&sysin+1
.   .bx on &lmargin &rmargin
.   :XMP.:SF font=3.
.   .do end
.do end
.dm mbigbox end
.*
.dm embigbox begin
.if '&format' eq '8.5x11a' .do begin
:eFIG.
.do end
.el .if '&format' eq 'help' .do begin
:eFIG.
.do end
.el .do begin
:eSF.:eXMP.
.bx off
.do end
.dm embigbox end
.*
.dm syntax begin
:XMP.:SF font=3.~b
.dm syntax end
.*
.dm esyntax begin
:eSF.:eXMP.
.dm esyntax end
.*
.dm syntaxbrk begin
.esyntax
.syntax
.dm syntaxbrk end
.*
.dm list begin
:XMP.:SF font=3.
.dm list end
.*
.dm elist begin
:eSF.:eXMP.
.dm elist end
.*
.dm synote begin
.begnote
:DTHD.where:
:DDHD.description:
.dm synote end
.*
.dm esynote begin
.endnote
.dm esynote end
.*
.dm mnote begin
.note &*
.dm mnote end
.*
.dm opt begin
:DT.&*name.&*
:DD.
.dm opt end
.if &$cmacpass. eq 'ugh' .do begin
.gt opt delete
.do end
.gt opt add opt attr
.ga * name any
.*
.dm contents begin
:P.
:SF font=4.
.dm contents end
.*
.dm econtents begin
:eSF.
.dm econtents end
.*
.dm hint begin
:P.
.cp &WDWlvl
.se lmargin=&sysin+1
.bx on &lmargin &rmargin
.se lmargin=&sysin+2
.in +2
:SF font=3.Hint::eSF.
.dm hint end
.*
.dm ehint begin
.in -2
.bx off
.dm ehint end
.*
.dm remark begin
:P.
.cp &WDWlvl
.se lmargin=&sysin+1
.bx on &lmargin &rmargin
.se lmargin=&sysin+2
.in +2
:SF font=3.Note::eSF.
.dm remark end
.*
.dm eremark begin
.in -2
.bx off
.dm eremark end
.*
.dm warn begin
:P.
.cp &WDWlvl
.se lmargin=&sysin+1
.bx on &lmargin &rmargin
.in +2
:SF font=3.WARNING!:eSF.
.dm warn end
.*
.dm ewarn begin
.in -2
.bx off
.dm ewarn end
.*
.dm exam begin
.  .if '&*1' eq 'begin' .do begin
.  .  .if '&*2' ne '' .do begin
.  .  .  .cp &*2
.  .  .do end
.  .  .el .do begin
.  .  .  .cp &WDWlvl
.  .  .do end
.  .  :P.:HP1.Example&*3::eHP1.:XMP.:SF font=4.
.  .do end
.  .el .if '&*1' eq 'end' .do begin
.  .  :eSF.:eXMP.
.  .do end
.  .el .if '&*1' eq 'break' .do begin
.  .  :eSF.:eXMP.:XMP.:SF font=4.
.  .do end
.  .el .do begin
.  .  .cp &WDWlvl;:P.:HP1.Example::eHP1.:XMP.:SF font=4.
.  .  &*
.  .  :eSF.:eXMP.:PC.
.  .do end
.dm exam end
.*
.dm tinyexam begin
.  .if '&*1' eq 'begin' .do begin
.  .  .if '&*2' ne '' .do begin
.  .  .  .cp &*2
.  .  .do end
.  .  .el .do begin
.  .  .  .cp &WDWlvl
.  .  .do end
.  .  :P.:HP1.Example&*3::eHP1.:XMP.:SF font=5.
.  .do end
.  .el .if '&*1' eq 'end' .do begin
.  .  :eSF.:eXMP.
.  .do end
.  .el .if '&*1' eq 'break' .do begin
.  .  :eSF.:eXMP.:XMP.:SF font=4.
.  .do end
.  .el .do begin
.  .  .cp &WDWlvl;:P.:HP1.Example::eHP1.:XMP.:SF font=5.
.  .  &*
.  .  :eSF.:eXMP.:PC.
.  .do end
.dm tinyexam end
.*
.* The following symbol set corrects a macro definition problem
.*
:SET symbol='$cmacpass' value='ugh'.

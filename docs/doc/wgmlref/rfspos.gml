:H2.Post GDOC Elements
:P.
Post GDOC Elements may appear any place in the document source after the
:HP2.&gml.GDOC:eHP2. tag.
:H3.Binary Include
:I1.binary include
:I1.binclude
:I1.file
:I1.reposition
:I1.depth
:FIG place=inline frame=box.
&gml.BINCLUDE file = 'char-string'
          reposition = start|end
          depth = 'vert-space-unit'.
:eFIG.
:H3.Index Entries
:I1.index tags
:I1.i1
:I1.i2
:I1.i3
:FIG place=inline frame=box.
&gml.I:HP1.n:eHP1. attribute.text line
:eFIG.
:PC.
The value of :HP1.n:eHP1. must be 1, 2 or 3.
The attribute, if specified, may be one or more of
:SL compact.
:LI.id = 'id-name'
:I1.id
:LI.pg = start | end | major | 'char-string'
:I1.pg
:LI.refid = 'id-name' (only with I2 or I3)
:I1.refid
:eSL.
:H3.Index Header
:I1.index header
:I1.ih1
:I1.ih2
:I1.ih3
:FIG place=inline frame=box.
&gml.IH:HP1.n:eHP1. attribute.text line
:eFIG.
:PC.
The value of :HP1.n:eHP1. must be 1, 2 or 3.
The attribute, if specified, may be one or more of
:SL compact.
:LI.id = 'id-name'
:I1.id
:LI.print = 'char-string'
:I1.print
:LI.see = 'char-string' (only with IH1 or IH2)
:I1.see
:LI.seeid = 'id-name'   (only with IH1 or IH2)
:I1.seeid
:eSL.
:H3.Index Reference
:I1.index reference
:I1.iref
:FIG place=inline frame=box.
&gml.IREF refid = 'id-name' attribute.
:eFIG.
:P.
The attribute, if specified, may be one or more of
:SL compact.
:LI.pg = start | end | major | 'char-string'
:I1.pg
:LI.see = 'char-string' (only when referencing IH1 or IH2)
:I1.see
:LI.seeid = 'id-name'   (only when referencing IH1 or IH2)
:I1.seeid
:eSL.
:H3.Process Specific Control
:I1.process specific control
:I1.psc
:I1.proc
:FIG place=inline frame=box.
&gml.PSC proc = 'char-string'.
     (proc is optional)
&gml.ePSC.
:eFIG.

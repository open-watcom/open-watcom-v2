:H2.Highlighting Phrases
:P.
The GML language provides a mechanism to highlight
phrases to emphasize fragments of text.
Four levels of highlighting may be specified&gml.
:SL.
:LI.:HP0.Highlight level(0):eHP0.
:LI.:HP1.Highlight level(1):eHP1.
:LI.:HP2.Highlight level(2):eHP2.
:LI.:HP3.Highlight level(3):eHP3.
:eSL.
:pc.The levels are illustrated in the preceding list.
This list was created with the following GML
input&gml.
:fig id=rfghp place=inline frame=box.
 :INCLUDE file='rfghp'.
 :figcap.Illustration of Highlight tags
:efig
:pc.
The highlighting is started with a :hp2.&gml.hpn:ehp2. (n = 0, 1, 2, 3)
:I2 refid='tthp0'.tthp0
:I2 refid='tthp1'.tthp1
:I2 refid='tthp2'.tthp2
:I2 refid='tthp3'.tthp3
tag and terminated with a corresponding :hp2.&gml.ehpn:ehp2 tag.
The formatted result of a particular highlighting tag is determined when
the document is processed.
The type of output device for which the document is being prepared
and the character sets being used are important factors in
determining the formatted result.
In the preceding simple list, the formatted result is a mixture
of the different character sets which are used to produce this
manual.
After the GML source has been entered, the selection of the output device
or character sets can be changed until the formatted output is
produced as desired.
:p.
Most text, such as the text which composes a paragraph, is at
highlighting level zero.
Some document elements, such as an example, may have a
layout-determined highlighting
level different from that of the basic text of the document.
The :hp2.&gml.hp0:ehp2. tag is therefore a means of
ensuring that the highlighting level of zero is used.
The other main use of the :hp2.&gml.hp0:ehp2. tag is to obtain
the highlighting level of zero
while in the midst of other highlighting levels.
:p.
It should be noted that the highlight phrase tags may be used with a
portion of a word.
The GML specification
:xmp
&gml.HP2.G&gml.eHP2.eneralized
:exmp
:pc.will cause the word ":HP2.G:eHP2.eneralized" to be output, with
the first letter emphasized with the level two highlighting.
:p.Highlight tags may not occur with tags having text lines
for which the GML layout determines the highlighting.
Examples of such tags include :hp2.&gml.title:ehp2.,
:hp2.&gml.dt:ehp2. and the heading tags.
:P.
When a tag ends a sentence, make sure that you use two periods.
The first period will be processed as part of the tag.
The second period will be treated as part of the text to be
processed.
If only one period is specified, the period to end the sentence
will not appear in the resulting document.
:FIG id=rfg2pd place=inline frame=box.
 :INCLUDE file='rfg2pd'.
 :FIGCAP.Tag at the end of a Sentence
:eFIG.
:INCLUDE file='rffhead'.
:FIG place=inline frame=box.
 :INCLUDE file='rfl2pd'.
 :FIGCAP.
 :FIGDESC.Output of :figref refid=rfg2pd page=no.
:eFIG.

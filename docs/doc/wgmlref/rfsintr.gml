:H2 id='intrans'.Input Translation
:P.
Some of the characters available with a particular output device may
:I1.input translation
not be characters that can be entered into the input text.
Input translation provides a way to enter this type of data.
A special escape character may be selected in the layout.
:I1.input escape
(See :HDREF refid='deflay'.).
If this escape character is entered into the
GML input text, the character
immediately following it will be translated to the value
specified as the input translation value for that character.
Most characters are defined to be unchanged by input translation.
Consider the following:
:FIG id=rfgint place=inline frame=box.
 :INCLUDE file='rfgint'.
 :FIGCAP.Input Translation
:eFIG.
:PC.
If the input translation escape character is the slash(/),
the processed document may appear as follows:
:FIG place=inline frame=box.
 :INCLUDE file='rflint'.
 :FIGCAP.
 :FIGDESC.Output of :FIGREF refid=rfgsym page=no.
:eFIG.
:P.
Note that the bullet character in the output is the asterisk.
The example output for this manual was produced with the
terminal device, which does not have special characters.
If the value ':HP2./*:eHP2.' is used within the text of this
document, the character ':HP2.\*:eHP2.' is produced.
:P.
Input translation is performed when text is separated
into words.
The translated character is not examined during these operations,
providing a method for bypassing the normal processing rules
of &WGML..
The values which result from an input translation are defined
in the device character sets (see :HDREF refid='dvintr').

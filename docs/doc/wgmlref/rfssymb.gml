:H2 id='symsub'.Symbolic Substitution
:P.
A symbol is a name which represents an arbitrary string of text.
:I1 id='syms'.symbols
:IH2 id='presyms'.defined by WGML
:IH1 seeid='syms'.symbolic substitution
:IH1 seeid='syms'.substitution
Once a symbol is assigned a text value, the symbol can be used
in the document source in place of that text.
Consider the following&gml.
:FIG id=rfgsym place=inline frame=box.
 :INCLUDE file='rfgsym'.
 :FIGCAP.Symbolic Substitution
:eFIG.
:INCLUDE file='rffhead'.
:FIG place=inline frame=box.
 :INCLUDE file='rflsym'.
 :FIGCAP.
 :FIGDESC.Output of :FIGREF refid=rfgsym page=no.
:eFIG.
:P.
A symbol name is defined and assigned a string of text with
the :HP2.&gml.set:eHP2. tag (see :HDREF refid='settag'.).
The value of the symbol name can be defined at any point
in the document file.
Any valid character string may be assigned to the symbol name.
When the symbol is referenced later, the value is substituted
into the input text.
The substitution is done before the source text or input
translation is processed by &WGML..
:P.
A symbol name is preceded by an ampersand(&) when referenced,
and is terminated by any character not valid in a symbol name.
If the terminating character is a period, it is considered part
of the symbol specification (you must therefore remember to specify
two periods if a symbol ends a sentence).
The recognition of a symbol name is case insensitive.
:P.
The symbol name
should not have a length greater than ten characters, and
may only contain letters, numbers, and the
characters @, #, $ and underscore(_).
Specifying the letters &mono.SYS&emono. as the first three
characters of the symbol name is equivalent to specifying
a dollar($) sign.
:P.
Recursive substitution is performed on a symbol.
This means that the text substituted for a symbol is
checked for the presence of more symbol names.
As well,
if the symbol name is immediately followed by another symbol
name (no intervening period or blanks), new names can be constructed
from the successive substitutions.
For example:
:FIG id=rfgisym place=inline frame=box.
 :INCLUDE file='rfgisym'.
 :FIGCAP.Iterative Substitution
:eFIG.
:PC.
The first part of the symbol sequence, :HP1.&prod:eHP1., does not
exist as a defined symbol.
However, when :HP1.&prodname.:eHP1. is substituted, the resulting
symbol name :HP1.&prodgml:eHP1. exists.
The resulting substitution produces the following:
:FIG place=inline frame=box.
 :INCLUDE file='rflisym'.
 :FIGCAP.
 :FIGDESC.Output of :FIGREF refid=rfgisym page=no.
:eFIG.
:P.
If an asterisk is specified immediately before the symbol name
(ie symbol='*prodname' or &amp.*prodname.), then the symbol is local.
Local symbols may not be referenced outside the file or macro
in which they are defined.
If an undefined local symbol is referenced in a macro, it is
replaced with an empty value.

:H2 id='settag'.SET
:XMP.
Format&gml. &gml.SET symbol='symbol-name'
             value='character-string'
                    delete.
:eXMP.
:I2 refid='gtset' pg=major.gtset
:IREF refid='etset' seeid='syms'.
:P.
This tag defines and assigns a value to a symbol name.
:P.
The :HP2.symbol:eHP2.
:I2 refid='gasymbo'.gasymbo
attribute must be specified.
The value of this attribute is the name of the symbol being defined,
and cannot have a length greater than ten characters.
The symbol name may only contain letters, numbers, and the
characters @, #, $ and underscore(_).
:P.
The :HP2.value:eHP2.
:I2 refid='gavalue'.gavalue
attribute must be specified.
The attribute value :HP1.delete:eHP1. or
a valid character string may be assigned to the symbol name.
If the attribute value :HP1.delete:eHP1. is used, the symbol
referred to by the symbol name is deleted.
Refer to :HDREF refid='symsub'. for more information about
symbol substitution.

:H2 id='specver'.Vertical Space Unit
:P.
A vertical space unit is specified in the same way as a
:IH1.vertical space unit
:I2.definition
horizontal space unit.
An EM space specifies the number of lines,
the height of a line determined by the current font, adjusted for
the document spacing value currently in effect.
For example, a vertical space value of '2M' with double spacing in
effect results in four lines worth of space.
:P
An integer number specifies the number of lines,
the height of a line determined by the LPINCH command line option, adjusted for
the document spacing value currently in effect.
The default lines per inch value is 6.
:P.
A device unit space(DV) specifies the number of lines without the
current document spacing accounted for.
For example, a vertical space value of '2DV' with double spacing in
effect results in two lines worth of space.

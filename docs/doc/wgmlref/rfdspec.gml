:H2.Device Functions
:INCLUDE file='rfdindd'.
:I1.device functions
:P.
When creating a device or driver definition,
it may be necessary to enter non-printable characters
or information which will not be available until the
document is processed.
Arithmetic operations may also have to be performed on the
data while the document is being processed.
Device functions allow you to specify this information
in the device or driver definitions.
:P.
Most of the device functions operate on supplied parameter values
and return
either a :HP2.numeric:eHP2. or :HP2.character:eHP2. result.
:P.
A :HP2.numeric:eHP2.
:I2 refid='devfunc'.numeric result
value is a sequence of
digits, or the result of a device function which returns a number.
Numeric values may be in either decimal or hexadecimal form.
Hexadecimal numeric values begin with a dollar($) sign and are composed
of digits and the characters :HP2.A:eHP2. through :HP2.F:eHP2..
:P.
A :HP2.character:eHP2.
:I2 refid='devfunc'.character result
value is a sequence of characters
enclosed in either single(') or double(") quotation marks.
The quotation marks surrounding the text are not part of the character
value.
If a quotation mark of the same type used to delimit the character
value is to be part of the character text, it may be entered
by specifying the quote character twice.
Only one quote character will appear in the resulting character value.
This should only be done when the quote character you wish to enter
as part of the character text is the same quote character being used to
delimit the character value.
The following lines illustrate valid character values:
:XMP.
'hello 12'
"hello 12"
"he'llo 12"
"he""llo 12"
:eXMP.
:PC.
The following lines illustrate invalid character values:
:XMP.
hello 12
hello 12"
"he"llo 12"
:eXMP.
:PC.
The following line is a valid character value, but is probably
not the correct  specification.
:XMP.
"it''s 12"
:eXMP.
:PC.
The two single quotes will be part of the character value
because double quotes are used to enclose the value.
:P.
The result of some device functions
will be used as :HP2.final:eHP2.
:I2 refid='devfunc'.final values
values for the sequence being defined.
A final value is sent directly to the output device.
Some of the device functions produce results which are not suitable
for use as a final value.
The result of this type of function must be supplied as a parameter value
to a device function which can produce a final value.
:P.
Prior to transmitting the device function sequences to the
:I2 refid='devfunc'.translation
output device, &WGML. translates
each character of the sequence into another character.
The translation values are defined in the font definitions used
with the device.
Some of the device functions produce final values which will not
be translated.
:P.
Each device function name begins with the percent character(%) and
is immediately followed by a left parenthesis.
If the device function has any parameter values, the value(s)
follow the left parenthesis and are separated by commas.
The device function is terminated with a right parenthesis.
:H3.ADD
:I1.add
:I2 refid='dadd'.dadd
:XMP.
%ADD(123,456)
:eXMP.
:P.
The two required parameters must both be numeric.
The sum of the two parameters is returned as a numeric result.
The result of this device function may not be used as a
final value.
:H3.BINARY1
:I1.binary1
:I2 refid='dbin1'.dbinary1
:XMP.
%BINARY1(123)
:eXMP.
:P.
The required parameter must be numeric.
The result of this function is a one byte binary number
ranging from 0 to 255 inclusive.
The result of this device function is a final value,
and may not be used as a parameter
of another device function.
The result is not translated when sent to
the output device.
:H3.BINARY2
:I1.binary2
:I2 refid='dbin2'.dbinary2
:XMP.
%BINARY2(1234)
:eXMP.
:P.
The required parameter must be numeric.
The result of this function is a two byte binary number
ranging from 0 to 65535 inclusive.
The result of this device function is a final value,
and may not be used as a parameter
of another device function.
The result is not translated when sent to
the output device.
:H3.BINARY4
:I1.binary4
:I2 refid='dbin4'.dbinary4
:XMP.
%BINARY4(1234)
:eXMP.
:P.
The required parameter must be numeric.
The result of this function is a four byte binary number
ranging from 0 to 4294967295 inclusive.
:CMT.doc-- should have a reference to machine specific section.
On some machines, the largest integer value is a two byte binary
number.
If such a machine is being used, two of the four bytes will always
be zero.
The result of this device function is a final value,
and may not be used as a parameter
of another device function.
The result is not translated when sent to
the output device.
:H3.CANCEL
:IH1 print='%cancel'.cancel
:I1.cancel
:I2 refid='dcancel'.dcancel
:XMP.
%CANCEL("bold")
:eXMP.
:P.
Some devices may cancel more than one
operation with a single control sequence.
For example, some devices may stop underlining
when bolding is turned off.
The :HP2.cancel:eHP2. device function specifies
the type of device operation that has been cancelled.
&WGML. will then re-establish the cancelled operation.
The possible values of the required character parameter are
:HP1.bold:eHP1., :HP1.underline:eHP1., and the name of a font switch
method (see :HDREF refid='fswtype'.).
The name of a font switch method is specified
when the device automatically switches to a default font.
This device function may not be used as a parameter
of another device function.
:H3.CLEARPC
:I1.clearpc
:I2 refid='dclearp'.dclearpc
:XMP.
%CLEARPC()
:eXMP.
:P.
This device function causes &WGML. to clear the screen
of an IBM PC.
It is used in the device definition, primarily with the pausing
section.
There is no effect when this function is used in a driver definition.
There are no parameters to this device function, and it may not
be used as a parameter of another device function.
:H3.CLEAR3270
:I1.clear3270
:I2 refid='dclear3'.dclear3270
:XMP.
%CLEAR3270()
:eXMP.
:P.
This device function causes &WGML. to clear the screen
of an IBM 3270 type of terminal.
It is used in the device definition, primarily with the pausing
section.
There is no effect when this function is used in a driver definition.
There are no parameters to this device function, and it may not
be used as a parameter of another device function.
:H3.DATE
:I1.date
:I2 refid='ddate'.ddate
:XMP.
%DATE()
:eXMP.
:P.
The result of this device function is a character value
representing the current date.
If the symbol :HP1.&amp.date:eHP1.
:I1.date symbol
is defined, the value
of this symbol is returned.
There are no parameters to this device function, and the result
may not be used as a final value.
:H3.DECIMAL
:I1.decimal
:I2 refid='ddecima'.ddecimal
:XMP.
%DECIMAL(123)
:eXMP.
:P.
The required parameter must be numeric.
The result of this device function is a character value
representing the given number.
The result may not be used as a final value.
:H3.DEFAULT_WIDTH
:I1.default_width
:I2 refid='ddefaul'.ddefault_width
:XMP.
%DEFAULT_WIDTH()
:eXMP.
:P.
The result of this device function is a numeric value which
represents the default width of a character in the current font.
When the font changes in the document, the value returned by this
function will change accordingly.
There are no parameters to this device function, and the result
may not be used as a final value.
:H3.DIVIDE
:I1.divide
:I2 refid='ddivide'.ddivide
:XMP.
%DIVIDE(124,12)
:eXMP.
:P.
The two required parameters must both be numeric.
The dividend from the integer division of the first parameter
by the second parameter is returned as a numeric value.
The remainder resulting from the division is not returned.
The result of this device function may not be used as a
final value.
:H3.FLUSHPAGE
:I1.flushpage
:I2 refid='dflushp'.dflushpage
:XMP.
%FLUSHPAGE()
:eXMP.
:P.
This device function causes &WGML. to flush the current
page to the output device.
The page flush is obtained by printing enough blank lines to
fill the current page.
If the size of the document page is greater than the size of
the output device page, the page flush will print enough blank
lines to flush the current device page.
If no data has been output to the device, and the page is the
first page in the document, the current page will not be flushed.
There are no parameters to this device function, and it may not
be used as a parameter of another device function.
:H3.FONT_HEIGHT
:I1.font_height
:I2 refid='dfont_h'.dfont_height
:XMP.
%FONT_HEIGHT()
:eXMP.
:P.
The result of this device function is a numeric value which
represents the height of the current font.
Adding this value to the :HP1.%font_space:eHP1. value gives
the total height of the line.
The return value is an integer value in one hundredths of a point.
For example, 12.25 points is returned as the number 1225.
When the font changes in the document, the value returned by this
function will change accordingly.
There are no parameters to this device function, and the result
may not be used as a final value.
:H3.FONT_SPACE
:I1.font_space
:I2 refid='dfont_s'.dfont_space
:XMP.
%FONT_SPACE()
:eXMP.
:P.
The result of this device function is a numeric value which
represents the space between lines in the current font.
Adding this value to the :HP1.%font_height:eHP1. value gives
the total height of the line.
The return value is an integer value in one hundredths of a point.
For example, 12.25 points is returned as the number 1225.
When the font changes in the document, the value returned by this
function will change accordingly.
There are no parameters to this device function, and the result
may not be used as a final value.
:H3.FONT_NUMBER
:I1.font_number
:I2 refid='dfont_n'.dfont_number
:XMP.
%FONT_NUMBER()
:eXMP.
:P.
The result of this device function is a numeric value which
represents the number of the current font.
When the font changes in the document, the value returned by this
function will change accordingly.
There are no parameters to this device function, and the result
may not be used as a final value.
:H3.FONT_OUTNAME1
:I1.font_outname1
:I2 refid='dfont_1'.dfont_outname1
:XMP.
%FONT_OUTNAME1()
:eXMP.
:P.
The result of this device function is a character value which
represents the :HP1.outname1:eHP1. value of the current font.
The :HP1.outname1:eHP1. value is specified in each font definition.
When the font changes in the document, the value returned by this
function will change accordingly.
There are no parameters to this device function, and the result
may not be used as a final value.
:H3.FONT_OUTNAME2
:I1.font_outname2
:I2 refid='dfont_2'.dfont_outname2
:XMP.
%FONT_OUTNAME2()
:eXMP.
:P.
The result of this device function is a character value which
represents the :HP1.outname2:eHP1. value of the current font.
The :HP1.outname2:eHP1. value is specified in each font definition.
When the font changes in the document, the value returned by this
function will change accordingly.
There are no parameters to this device function, and the result
may not be used as a final value.
:H3.FONT_RESIDENT
:I1.font_resident
:I2 refid='dfont_r'.dfont_resident
:XMP.
%FONT_RESIDENT()
:eXMP.
:P.
The result of this device function is a character value.
The result value represents the resident status of the current font.
The resident status for each font is specified
in the :HP1.&gml.devicefont:eHP1. block of the device definition.
When the font changes in the document, the value returned by this
function will change accordingly.
The value 'Y' will be returned if the font is resident in the device,
while the value 'N' will be returned if it is not resident.
There are no parameters to this device function, and the result
may not be used as a final value.
:H3.HEX
:I1.hex
:I2 refid='dhex'.dhex
:XMP.
%HEX(123)
:eXMP.
:P.
The required parameter must be numeric.
The result of this device function is a character value
representing the given number in hexadecimal form.
For example, the number value 255 would be returned
as the character value 'FF'.
Note that a dollar sign is not returned as part of the value.
The result may not be used as a final value.
:H3.IMAGE
:I1.image
:I2 refid='dimage'.dimage
:XMP.
%IMAGE("hello")
:eXMP.
:P.
The required parameter must be a character value.
The result of this device function is a character value
representing the given parameter, and is a final value.
The result of this device function may not be used as a parameter
of another device function, and is not translated when sent to
the output device.
:H3.LINE_HEIGHT
:I1.line_height
:I2 refid='dline_h'.dline_height
:XMP.
%LINE_HEIGHT()
:eXMP.
:P.
The result of this device function is a numeric value which
represents the height of the current font.
Adding this value to the :HP1.%line_space:eHP1. value gives
the total height of the line.
The return value is in terms of the device vertical base units.
When the font changes in the document, the value returned by this
function will change accordingly.
There are no parameters to this device function, and the result
may not be used as a final value.
:H3.LINE_SPACE
:I1.line_space
:I2 refid='dline_s'.dline_space
:XMP.
%LINE_SPACE()
:eXMP.
:P.
The result of this device function is a numeric value which
represents the space between lines in of the current font.
Adding this value to the :HP1.%line_height:eHP1. value gives
the total height of the line.
The return value is in terms of the device vertical base units.
When the font changes in the document, the value returned by this
function will change accordingly.
There are no parameters to this device function, and the result
may not be used as a final value.
:H3.PAGES
:I1.pages
:I2 refid='dpages'.dpages
:XMP.
%PAGES()
:eXMP.
:P.
The result of this device function is a numeric value which
represents the number of the current page being output.
This number is not related to the numbering of pages in the
formatted output.
There are no parameters to this device function, and the result
may not be used as a final value.
:H3.PAGE_DEPTH
:I1.page_depth
:I2 refid='dpage_d'.dpage_depth
:XMP.
%PAGE_DEPTH()
:eXMP.
:P.
The result of this device function is a numeric value which
represents the depth of the output page as defined in the device
definition.
There are no parameters to this device function, and the result
may not be used as a final value.
:H3.PAGE_WIDTH
:I1.page_width
:I2 refid='dpage_w'.dpage_width
:XMP.
%PAGE_WIDTH()
:eXMP.
:P.
The result of this device function is a numeric value which
represents the width of the output page as defined in the device
definition.
There are no parameters to this device function, and the result
may not be used as a final value.
:H3.RECORDBREAK
:I1.recordbreak
:I2 refid='drecord'.drecordbreak
:XMP.
%RECORDBREAK()
:eXMP.
:P.
&WGML. forms a line of output for a device.
With some devices, it is desirable to send several of these output
lines together as one record.
With other devices, each line and even some control sequences
must be sent as separate records.
&WGML. assumes that each record may contain several output lines.
The device function :HP2.RECORDBREAK:eHP2. instructs &WGML.
to send the information in the current record to the output
device.
There are no parameters to this device function, and it may not
be used as a parameter of another device function.
:H3.REMAINDER
:I1.remainder
:I2 refid='dremain'.dremainder
:XMP.
%REMAINDER(124,12)
:eXMP.
:P.
The two required parameters must both be numeric.
The remainder from the division of the first parameter
by the second parameter is returned as a numeric value.
The result of this device function may not be used as a
final value.
:H3.SLEEP
:I1.sleep
:I2 refid='dsleep'.dsleep
:XMP.
%SLEEP(30)
:eXMP.
:P.
There is no result returned from this device function.
The required parameter must be a non-negative integer number.
Other device functions are not allowed as parameters to this
device function.
This device function causes &WGML. to suspend document processing
for the specified number of seconds.
This device function may not be used as a parameter of another
device function.
:H3.SUBTRACT
:I1.subtract
:I2 refid='dsubtra'.dsubtract
:XMP.
%SUBTRACT(456,123)
:eXMP.
:P.
The two required parameters must both be numeric.
The difference obtained by subtracting the second parameter
from the first parameter is returned as a numeric value.
The result of this device function may not be used as a
final value.
:H3.TAB_WIDTH
:I1.tab_width
:I2 refid='dtab_wi'.dtab_width
:XMP.
%TAB_WIDTH()
:eXMP.
:P.
When &WGML. uses tabbing to produce white space in a
horizontal direction,
the result of this device function is a numeric value which
represents the amount of space that is being tabbed over.
There are no parameters to this device function, and the result
may not be used as a final value.
:H3.TEXT
:I1.text
:I2 refid='dtext'.dtext
:XMP.
%TEXT("hello")
:eXMP.
:P.
The required parameter must be a character value.
The result of this device function is a character value
representing the given parameter.
The result of this device function is a final value,
and may not be used as a parameter
of another device function. The result is translated
when sent to the output device.
:H3.THICKNESS
:I1.thickness
:I2 refid='dthick'.dthickness
:XMP.
%THICKNESS()
:eXMP.
:P.
The result of this device function is a numeric value which
represents the current thickness of the rule line being drawn.
The value returned by the function is set when drawing horizontal
or vertical lines, and when drawing a box.
There are no parameters to this device function, and the result
may not be used as a final value.
:H3.TIME
:I1.time
:I2 refid='dtime'.dtime
:XMP.
%TIME()
:eXMP.
:P.
The result of this device function is a character value
representing the current time of day.
If the symbol :HP1.&amp.time:eHP1.
:I1.time symbol
is defined, the value
of this symbol is returned.
There are no parameters to this device function, and the result
may not be used as a final value.
:H3.WAIT
:I1.wait
:I2 refid='dwait'.dwait
:I1.pausing
:I2.wait
:XMP.
%WAIT()
:eXMP.
:P.
This device function causes &WGML. to suspend document processing
until the enter key on the keyboard is depressed.
This device function
is used in the device definition, primarily with the pausing
section.
There is no effect when this function is used in a driver definition.
There are no parameters to this device function, and it may
not be used as a parameter of another device function.
:H3.WGML_HEADER
:I1.wgml_header
:I2 refid='dwgml_h'.dwgml_header
:XMP.
%WGML_HEADER()
:eXMP.
:P.
The result of this device function is a character value which
represents the &WGML. header.
The header is the character value which identifies the
&WGML. product and version number.
There are no parameters to this device function, and the result
may not be used as a final value.
:H3 id='xaddr'.X_ADDRESS
:I1.x_address
:I2 refid='dx_addr'.dx_address
:XMP.
%X_ADDRESS()
:eXMP.
:P.
The result of this device function is a numeric value which
represents the
current horizontal(X-axis) position on the output page.
There are no parameters to this device function, and the result
may not be used as a final value.
(See :HDREF refid='devpgad'. for more information).
:H3.X_SIZE
:I1.x_size
:I2 refid='dx_size'.dx_size
:XMP.
%X_SIZE()
:eXMP.
:P.
The result of this device function is a numeric value which
represents the
current horizontal length of a line to be drawn.
The value of this function is set when drawing a box or a horizontal
line.
There are no parameters to this device function, and the result
may not be used as a final value.
:H3 id='yaddr'.Y_ADDRESS
:I1.y_address
:I2 refid='dy_addr'.dy_address
:XMP.
%Y_ADDRESS()
:eXMP.
:P.
The result of this device function is a numeric value which
represents the
current vertical(Y-axis) position on the output page.
There are no parameters to this device function, and the result
may not be used as a final value.
(See :HDREF refid='devpgad'. for more information).
:H3.Y_SIZE
:I1.y_size
:I2 refid='dy_size'.dy_size
:XMP.
%Y_SIZE()
:eXMP.
:P.
The result of this device function is a numeric value which
represents the
current vertical length of a line to be drawn.
The value of this function is set when drawing a box or a horizontal
line.
There are no parameters to this device function, and the result
may not be used as a final value.

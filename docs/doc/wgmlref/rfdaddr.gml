:H2 id='devpgad'.Page Addressing
:I1.addressing
:I1.page addressing
:P.
A particular point on the output page is
identified by a horizontal (X-axis) and
a vertical (Y-axis) component.
Together, the X and Y components designate
the :HP2.address:eHP2. of a point on the page.
As each word and line of output is processed,
the X and Y components of the address are adjusted to make a new
address.
Many devices restrict the adjustment of the address.
Other devices
are known as :HP2.point addressable:eHP2.
:I1.point addressable
or :HP2.full page addressing:eHP2.
:I1.full page addressing
devices, and allow any point on the page to be addressed.
:P.
&WGML. assumes that the
start of an output page is the upper left corner.
The horizontal component of the page address is adjusted
for each character placed on the output page.
The vertical component of the page address is adjusted
for each output line.
The current X and Y address component values are available
through the %X_ADDRESS and %Y_ADDRESS device functions.
(See :HDREF refid='xaddr' page=no. and
:HDREF refid='yaddr'. for more information).

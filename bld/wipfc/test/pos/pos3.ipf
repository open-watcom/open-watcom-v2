:userdoc.
:title.Positive 3
:docprof.
:h1 res=1.Testing formatting tags
:i1 id='1'.Formatting tags
:p.
The following pages test various formatting tags&per.

:h2 res=2.Highlighted phrases
:i2 refid='1'.hp1 - hp9
:p.
:hp1.This is highlighted phrase 1&per.:ehp1.
:p.
:hp2.This is highlighted phrase 2&per.:ehp2.
:p.
:hp3.This is highlighted phrase 3&per.:ehp3.
:p.
:hp4.This is highlighted phrase 4&per.:ehp4.
:p.
:hp5.This is highlighted phrase 5&per.:ehp5.
:p.
:hp6.This is highlighted phrase 6&per.:ehp6.
:p.
:hp7.This is highlighted phrase 7&per.:ehp7.
:p.
:hp8.This is highlighted phrase 8&per.:ehp8.
:p.
:hp9.This is highlighted phrase 9&per.:ehp9.

:h2 res=3.Foreground and background color
:i2 refid='1'.color
:p.
:color fc=green bc=blue.Color the foreground green; the background, blue&per.
:p.
:color fc=blue bc=red.Color the foreground blue; the background, red&per.
:p.
:color fc=cyan bc=yellow.Color the foreground cyan; the background, yellow&per.
:p.
:color fc=default bc=default.Use the default system colors&per.

:h2 res=4.Artwork
:i2 refid='1'.artwork
:p.
This is an example of artwork displayed within the :artwork runin name='browtarg.bmp'. text of a sentence&per.
:p.
The next image should appear on the left&per.
.br
:artwork align=left name="ide1c.bmp".
:p.
The next image should appear on the right&per.
.br
:artwork align=right name="ide1c.bmp".
:p.
The next image should appear centered&per.
.br
:artwork align=center name="ide1c.bmp".
:p.
An 8 bits-per-pixel bitmap&colon.
.br
:artwork name="acc8.bmp".
:p.
A 24 bits-per-pixel bitmap&colon.
.br
:artwork name="acc3.bmp".
:p.
A 4 bit-per-pixel bitmap&colon.
:artwork name="ide2c.bmp".

:h2 res=5.Font
:i2 refid='1'.font
:p.Some normal text&per.
:p.
:font facename=Courier size=13x8.This font is Courier 13 by 8&per.
:p.
:font facename='Tms Rmn' size=18x14.This font is Tms Rmn 18 by 14&per.
:p.
:font facename=Helv size=28x18.This font is Helvetica 28 by 18&per.
:p.
:font facename=default size=0x0.This is the default font&per.

:h2 res=6.Margins
:i2 refid='1'.lm and rm
:p.
:rm margin=10.
:lm margin=20.This text begins 20 spaces to the right of the left window border and ends 10 spaces to
the left of the right window border&per. All text is aligned as specified by the margin values&per.
:lm margin=5.Here the left margin is changed to 5&per. Because this margin tag begins more than 5 spaces
on the line, the margin specified becomes effective on the following line, and the text beings 5 spaces
from the left window border&per. The right margin remains unchanged&per.

:h2 res=7.Hidden Text
:i2 refid='1'.hide
:p.This text is not hidden&per. :hide key='SHOWME'.This text is hidden&per.:ehide.

:h2 res=8.Link
:i2 refid='1'.link
:p.This :link reftype=hd res=2.link:elink. links to the highlighed phrase topic&per.
:p.Alternatively, one can use a :hdref res=2. to the highlihgted phrase topic&per.

:h2 res=9.Centered Text
:i2 refid='1'.center
:p.This text is flush left&per.
.ce This text is centered&per.
:p.This text is flush left&per.

:euserdoc.


.np
Only some combinations of video modes and hardware allow multiple pages
of graphics to exist.
When multiple pages are supported,
the active page may differ from the visual page.
The graphics information in the visual page determines what is
displayed upon the screen.
Animation may be accomplished by alternating the visual page.
A graphics page can be constructed without affecting the screen
by setting the active page to be different than the visual page.
.np
The number of available video pages can be determined by using the
.kw _getvideoconfig
&routine..
The default video page is 0.

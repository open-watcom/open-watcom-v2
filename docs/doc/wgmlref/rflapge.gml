:DT.page_eject
:I2 refid='lapage_'.lapage_
:DD.This attribute accepts the keyword values :hp1.yes:ehp1.,
    :hp1.no:ehp1., :HP1.odd:eHP1., and :HP1.even:eHP1..
    If the value :HP1.no:eHP1. is specified, the heading
    is one column wide and is not forced to a new page.
    The heading is always placed on a new page when
    the value :hp1.yes:ehp1. is specified.
    Values other than :HP1.no:eHP1. cause the heading to be treated as a page
    wide heading in a multi-column document.
:P.
    The values :HP1.odd:eHP1. and :HP1.even:eHP1. will place the heading
    on a new page if the parity (odd or even) of the current page number
    does not match the specified value.
    When two headings appear together,
    the attribute value :HP1.stop_eject=yes:eHP1.
    of the :HP2.&gml.heading:eHP2. layout tag will normally prevent the
    the second heading from going to the next page.
    The :HP1.odd:eHP1. and :HP1.even:eHP1. values act on the heading
    without regard to the :HP1.stop_eject:eHP1. value.

.*Text before userdoc is not allowed
This text is illegal
:userdoc.
:title.Negative 10
:docprof.
.*Tag and text before the first h1 tag is not allowed
:p.This text is illegal

:h1 res=001.Undefined Command
.p This is an undefined command

:h1 res=002.Undefined tag
:pr.This is an undefined tag

:h1 res=003.Undefined attribute
:p compact.The paragraph tag does not allow attributes&per.
:hp1 color=blue.This phrase has an undefined attribute&per.:ehp1.

:h3 res=004.Illegal heading order
:p.This heading is out of order&per.

:h1 res=004.Duplicate resource id
:p.This header has a duplicate resource id
:hide key="hidden".

:h1 res=006.Hidden res attribute
:p.This header has a resource id, but is hidden
:ehide.

:h1 res=007.Nonexistant identifiers
:p.This :link res=005.links:elink. to a nonexistant resource number
:p.This :link refid="unknown".links:elink. to a nonexistant name/id
:p.This :link.links:elink. has no reference identifier

:h1 res=008.List errors
:li.This list element is outside of a list

:h2 res=009.No end-of-list tag
:ul.
:li.A list item
:li.Another list item

:h2 res=010.dd without dt
:dl.
:ddhd.Needs a dthd
:dd.Needs a dt
:edl.

:h1 res=011.Table Errors
:p.Errors in tables

:h2 res=012.No valid cols specified
:table.
:etable.

:h2 res=013.Text before &colon.c
:table cols="12 12".
:row.Illegal text
:c.Legal text
:c.Legal text
:etable.

:h2 res=14.Invalid tag in table
:table cols="12 12".
:row.
:c.Legal text
:c.:caution.Illegal tag
:etable.

:h2 res=15.Too many columns
:table cols="12 12".
:row.
:c.Legal text
:c.Legal text
:c.Legal text
:etable.

:h2 res=16.Too few columns
:table cols="12 12".
:row.
:c.Legal text
:etable.

:h2 res=17.Missing elink
:table cols="12 12".
:row.
:c.Legal text
:c.:link res=1.Topic 1
:etable.

:h1 res=18.Graphics files
:artwork.
:artwork name="neg10.jpg".


:euserdoc.
.*Text after euserdoc is not allowed
This text is illegal


.*
.* (c) Copyright 1992 by WATCOM International Corp.
.*
.* All rights reserved. No part of this publication may be reproduced or
.* used in any form or by any means - graphic, electronic, or mechanical,
.* including photocopying, recording, taping or information storage and
.* retrieval systems - without written permission of WATCOM Publications
.* Limited.
.*
.* Date		By		Reason
.* ----		--		------
.* 14-aug-92	Craig Eisler	initial draft
.*
:DEFLIST.
:DEFITEM.SHIFT_UP (shifted cursor up key)
Starts or continues selection and moves up to the previous
line.  The new line is selected.

:DEFITEM.SHIFT_DOWN (shifted cursor down key)
Starts or continues selection and moves down to the next line.
The new line is selected.

:DEFITEM.SHIFT_LEFT (shifted cursor left key)
Starts or continues selection and moves left to the next
character.  The new character is selected.

:DEFITEM.SHIFT_RIGHT (shifted cursor right key)
Starts or continues selection and moves right to the previous
character. The new character is selected.

:DEFITEM.CTRL_R
Starts text selection, if no text is selected.  The current character is
highlighted. If a region is already selected, then this cancels the
selected region.
:DEFITEM.ESC
Cancels the current selection.
:eDEFLIST.

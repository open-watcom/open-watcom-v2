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
:DEFITEM.\<"key">
Any special key may be inserted for
:ITALICS "key"
:PERIOD.
The angle brackets are required.
There are a number of pre-defined keys
symbols that are recognized. These are described in the Appendix
:HDREF refid='symkey'.
:PERIOD.
:DEFITEM.\e
Short form for the escape key (rather than \<ESC>).
:DEFITEM.\n
Short form for the enter key (rather than \<ENTER>).
:DEFITEM.\h
If a &cmdline command is used in the sequence, and it follows the
colon (':'), the command is not added to the history.  For example:
:ILLUST.
  :\hdate\n
:eILLUSt.
will display the current date and time, but the command will not enter
the command history.
:DEFITEM.\x
If a &cmdline command is used in the sequence, then this stops the
command window from opening.  This prevents the "flashing" of the
command window as it is opened then closed. For example:
:ILLUST.
  \x:date\n
:eILLUSt.
will display the current date and time, but the command window will
not be displayed.
:eDEFLIST.

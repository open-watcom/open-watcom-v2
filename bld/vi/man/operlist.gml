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
.* 28-sep-92	Craig Eisler	initial draft
.*
:OL.
:LI.A movement command. See the section
:HDREF refid='mvement'.
for a full description of all movement commands.
If a movement command is specified, then the range that the command
will act on is from the current position to the position that would
be achieved by using the movement command.
:LI.A search command:
:OL compact.
:LI./ (forward slash)
:LI.? (question mark)
:LI.n
:LI.N
:eOL.
See the section
:HDREF refid='srching'
.if '&chapref' eq '1' .do begin
.se chapref = 0
in the chapter
:HDREF page=no refid='modes'
.do end
for a full description of the searching commands.
If a search command is specified, then the range that the command
will act on is from the current position to the position that would
be achieved by using the search command.
:LI.The current selected (highlighted) region.  In this case,
:ITALICS.<oper>
is the
:ITALICS.r
key.
:LI.The same character as the command character.  This causes the
command to operate on the current line.
:eOL

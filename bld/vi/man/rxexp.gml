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
.* 18-aug-92	Craig Eisler	initial draft
.*
You can use complex search strings known as
:ITALICS.regular expressions
:PERIOD.
Certain characters have special meaning in a regular expression, they are:
:ILLUST.
^ $ . [ ( ) | ? + * \ ~ @
:eILLUST.
If you wish to search for any of these special characters, you must
place a backslash ('\') before the character.  For example, to search
for:
:ILLUST.
ab.c$
:eILLUST.
you have to enter:
:ILLUST.
ab\.c\$
:eILLUST.
For information on regular expressions, see
:HDREF refid='rxchap'.
:PERIOD.

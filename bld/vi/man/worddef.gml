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
A word is defined
using the
:KEYWORD.word
setting. The default is that any characters in the set (
:ITALICS._
:CONT.,
:ITALICS.a-z
:CONT.,
:ITALICS.A-Z
:CONT.,
:ITALICS.0-9
) are considered part of a word,
and all other characters (except for whitespace) are delimiters.  Groups
of delimiters are considered to be a word as well.

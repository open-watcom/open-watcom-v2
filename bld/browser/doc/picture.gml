:INCLUDE file='FMTMACRO'.
:INCLUDE file='GMLMACS'.
:INCLUDE file='SYMBOLS'.
:GDOC.
.* ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿
.* ³ Copyright by WATCOM Systems Inc. 1990.  All rights reserved.  ³
.* ³ No part of this publication may be reproduced in any form or  ³
.* ³ by any means - graphic, electronic or mechanical, including   ³
.* ³ photocopying, recording, taping or information storage and    ³
.* ³ retrieval systems. 					   ³
.* ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ
.*
:BODY.
.np
This is a paragraph.
See the picture in
:figref refid='file'..
:P.
:graphic depth='3.2i' xoff='0.5i' yoff='4.4i' scale=70 file='test.eps'
:fig id='file' frame=none depth=0 place=inline.
:figcap.This is a picture of a thing.
:efig.
.np
:eGDOC.

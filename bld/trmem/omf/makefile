###################################################################
#       Copyright by WATCOM Systems Inc. 1987, 1990. All rights   #
#       reserved. No part of this software may be reproduced      #
#       in any form or by any means - graphic, electronic or      #
#       mechanical, including photocopying, recording, taping     #
#       or information storage and retrieval systems - except     #
#       with the written permission of WATCOM Systems Inc.        #
###################################################################

default: .symbolic trmemcvr.obj trmem.obj mem.obj
    @@%null

.c: ..
.cpp: ..
.c.obj:
    wcc386 $[* /i=$(%INCLUDE);.. -d2 -mf -oails -s -w4 -zq /dTRMEM /of+
.cpp.obj:
    wpp386 $[* /i=$(%INCLUDE);.. -d2 -mf -oails -s -w4 -zq /dTRACKER /of+

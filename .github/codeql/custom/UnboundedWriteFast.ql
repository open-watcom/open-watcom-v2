
/**
 * @name Test Buffer Writes
 * @kind problem
 * @id local/test-buffer-write
 */

import semmle.code.cpp.security.BufferWrite

from BufferWrite bw
select bw, bw.getBWDesc()

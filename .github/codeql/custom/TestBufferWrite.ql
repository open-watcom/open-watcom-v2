/**
 * @name Test Unbounded Writes
 * @kind problem
 * @problem.severity warning
 * @id local/test-unbounded-write
 */

import semmle.code.cpp.security.BufferWrite

predicate isUnboundedWrite(BufferWrite bw) {
  not bw.hasExplicitLimit() and
  not exists(bw.getMaxData(_))
}

from BufferWrite bw
where isUnboundedWrite(bw)
select bw, bw.getBWDesc()

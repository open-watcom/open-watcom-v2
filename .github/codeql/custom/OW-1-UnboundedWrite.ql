/**
 * @name Unbounded write
 * @description Buffer write operations that do not control the length
 *              of data written may overflow.
 * @kind path-problem
 * @problem.severity error
 * @security-severity 9.3
 * @precision medium
 * @id cpp/unbounded-write
 * @tags reliability
 *       security
 *       external/cwe/cwe-120
 *       external/cwe/cwe-787
 *       external/cwe/cwe-805
 */

import semmle.code.cpp.security.BufferWrite

predicate isUnboundedWrite(BufferWrite bw) {
  not bw.hasExplicitLimit() and // has no explicit size limit
  not exists(bw.getMaxData(_)) // and we can't deduce an upper bound to the amount copied
}

from BufferWrite bw
where isUnboundedWrite(bw)
select bw,
  "This '" + bw.getBWDesc() + "' does not appear to have an explicit length bound or a deduced maximum input size."

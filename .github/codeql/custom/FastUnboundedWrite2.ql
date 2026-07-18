/**
 * @name Very fast unbounded write candidate
 * @kind problem
 * @problem.severity warning
 * @security-severity 8.0
 * @precision low
 * @id local/cpp/very-fast-unbounded-write
 * @tags reliability
 *       security
 *       external/cwe/cwe-120
 */

import semmle.code.cpp.security.BufferWrite

predicate isUnboundedWrite(BufferWrite bw) {
  not bw.hasExplicitLimit() and
  not exists(bw.getMaxData(_))
}

from BufferWrite bw
where isUnboundedWrite(bw)
select bw,
  "This '" + bw.getBWDesc() + "' does not appear to have an explicit bound or deduced maximum input size."
/**
 * @name Candidate unbounded write
 * @description Finds buffer write operations without an explicit or deduced bound.
 *              This is a fast candidate query for large C/C++ databases where
 *              full global taint tracking is too expensive.
 * @kind problem
 * @problem.severity warning
 * @security-severity 7.5
 * @precision low
 * @id local/cpp/candidate-unbounded-write
 * @tags reliability
 *       security
 *       external/cwe/cwe-120
 *       external/cwe/cwe-787
 *       external/cwe/cwe-805
 */

import semmle.code.cpp.security.BufferWrite

predicate isUnboundedWrite(BufferWrite bw) {
  not bw.hasExplicitLimit() and
  not exists(bw.getMaxData(_))
}

from BufferWrite bw
where isUnboundedWrite(bw)
select bw,
  "This '" + bw.getBWDesc() + "' does not appear to have an explicit length bound or a deduced maximum input size."

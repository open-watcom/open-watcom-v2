/**
 * @name Fast unbounded write without path or barrier analysis
 * @description Detects unbounded buffer writes reachable from CodeQL flow sources,
 *              but avoids path reconstruction and guard/barrier taint analysis.
 * @kind problem
 * @problem.severity warning
 * @security-severity 8.5
 * @precision low
 * @id local/cpp/fast-unbounded-write-no-barrier
 * @tags reliability
 *       security
 *       external/cwe/cwe-120
 *       external/cwe/cwe-787
 *       external/cwe/cwe-805
 */

import semmle.code.cpp.security.BufferWrite
import semmle.code.cpp.security.FlowSources as FS
import semmle.code.cpp.dataflow.new.TaintTracking

predicate isUnboundedWrite(BufferWrite bw) {
  not bw.hasExplicitLimit() and
  not exists(bw.getMaxData(_))
}

/**
 * Holds if `e` is a source buffer going into an unbounded write `bw`,
 * or a qualifier of such a source.
 */
predicate unboundedWriteSource(Expr e, BufferWrite bw, boolean qualifier) {
  isUnboundedWrite(bw) and
  e = bw.getASource() and
  qualifier = false
  or
  exists(FieldAccess fa |
    unboundedWriteSource(fa, bw, _) and
    e = fa.getQualifier()
  ) and
  qualifier = true
}

predicate isSource(FS::FlowSource source, string sourceType) {
  source.getSourceType() = sourceType
}

predicate isSink(DataFlow::Node sink, BufferWrite bw, boolean qualifier) {
  unboundedWriteSource(sink.asIndirectExpr(), bw, qualifier)
  or
  bw.getASource() = bw and
  unboundedWriteSource(sink.asDefiningArgument(), bw, qualifier)
}

module Config implements DataFlow::ConfigSig {
  predicate isSource(DataFlow::Node source) {
    isSource(source, _)
  }

  predicate isSink(DataFlow::Node sink) {
    isSink(sink, _, _)
  }

  /**
   * Preserve this from the original query:
   * once we reached the unbounded write source buffer itself,
   * do not continue flowing out of it.
   */
  predicate isBarrierOut(DataFlow::Node node) {
    isSink(node, _, false)
  }

  predicate observeDiffInformedIncrementalMode() {
    any()
  }

  Location getASelectedSinkLocation(DataFlow::Node sink) {
    exists(BufferWrite bw |
      result = [bw.getLocation(), sink.getLocation()] |
      isSink(sink, bw, _)
    )
  }
}

module Flow = TaintTracking::Global<Config>;

from BufferWrite bw, FS::FlowSource source, DataFlow::Node sink, string sourceType
where
  Flow::flow(source, sink) and
  isSource(source, sourceType) and
  isSink(sink, bw, _)
select bw,
  "This '" + bw.getBWDesc() + "' may receive input from $@ and may overflow the destination.",
  source, sourceType

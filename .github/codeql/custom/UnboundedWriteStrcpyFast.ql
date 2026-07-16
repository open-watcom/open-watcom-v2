/**
 * @name Unbounded strcpy-like write - fast test
 * @description Reduced custom test for unbounded write taint flow.
 * @kind problem
 * @problem.severity warning
 * @security-severity 9.3
 * @precision medium
 * @id local/cpp/unbounded-write-strcpy-fast
 * @tags reliability
 *       security
 *       external/cwe/cwe-120
 */

import semmle.code.cpp.security.BufferWrite
import semmle.code.cpp.security.FlowSources as FS
import semmle.code.cpp.dataflow.new.TaintTracking

predicate isUnboundedWrite(BufferWrite bw) {
  not bw.hasExplicitLimit() and
  not exists(bw.getMaxData(_))
}

predicate isInterestingWrite(BufferWrite bw) {
  bw.getBWDesc().matches("%strcpy%") or
  bw.getBWDesc().matches("%strcat%") or
  bw.getBWDesc().matches("%gets%")
}

predicate interestingUnboundedWrite(BufferWrite bw) {
  isUnboundedWrite(bw) and
  isInterestingWrite(bw)
}

/**
 * Source buffer of an interesting unbounded write.
 */
predicate isInterestingSink(DataFlow::Node sink, BufferWrite bw) {
  interestingUnboundedWrite(bw) and
  sink.asIndirectExpr() = bw.getASource()
}

predicate isFlowSource(DataFlow::Node source, string sourceType) {
  exists(FS::FlowSource fs |
    source = fs and
    sourceType = fs.getSourceType()
  )
}

module Config implements DataFlow::ConfigSig {
  predicate isSource(DataFlow::Node source) {
    isFlowSource(source, _)
  }

  predicate isSink(DataFlow::Node sink) {
    exists(BufferWrite bw |
      isInterestingSink(sink, bw)
    )
  }

  /**
   * Do not continue propagating after the sink.
   */
  predicate isBarrierOut(DataFlow::Node node) {
    exists(BufferWrite bw |
      isInterestingSink(node, bw)
    )
  }
}

module Flow = TaintTracking::Global<Config>;

from BufferWrite bw, DataFlow::Node source, DataFlow::Node sink, string sourceType
where
  Flow::flow(source, sink) and
  isFlowSource(source, sourceType) and
  isInterestingSink(sink, bw)
select bw,
  "This unbounded write may receive input from " + sourceType +
  " and may overflow the destination."

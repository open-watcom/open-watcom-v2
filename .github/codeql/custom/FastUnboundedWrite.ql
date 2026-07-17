/**
 * @name Fast unbounded write
 * @description Buffer write operations that do not control the length
 *              of data written and may receive input from a flow source.
 *              This variant avoids path reconstruction for large C/C++ databases.
 * @kind problem
 * @problem.severity error
 * @security-severity 9.3
 * @precision medium
 * @id local/cpp/fast-unbounded-write
 * @tags reliability
 *       security
 *       external/cwe/cwe-120
 *       external/cwe/cwe-787
 *       external/cwe/cwe-805
 */

import semmle.code.cpp.security.BufferWrite
import semmle.code.cpp.security.FlowSources as FS
import semmle.code.cpp.dataflow.new.TaintTracking
import semmle.code.cpp.controlflow.IRGuards

predicate isUnboundedWrite(BufferWrite bw) {
  not bw.hasExplicitLimit() and
  not exists(bw.getMaxData(_))
}

/**
 * Holds if `e` is a source buffer going into an unbounded write `bw` or a
 * qualifier of such a source.
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

/**
 * Same barrier helper as the original query.
 * This is used to avoid reporting writes where the data appears to be
 * controlled by a relevant guard.
 */
module BarrierConfig implements DataFlow::ConfigSig {
  predicate isSource(DataFlow::Node source) {
    isSource(source, _)
  }

  predicate isSink(DataFlow::Node sink) {
    comparesEq(sink.asOperand(), _, _, true, _) or
    comparesLt(sink.asOperand(), _, _, true, _)
  }
}

module BarrierFlow = TaintTracking::Global<BarrierConfig>;

import semmle.code.cpp.ir.dataflow.internal.DataFlowImplCommon as DataFlowImplCommon

predicate interestingLessThanOrEqual(Operand left) {
  exists(DataFlowImplCommon::NodeEx node |
    node.asNode().asOperand() = left and
    BarrierFlow::Stages::Stage1::sinkNode(node, _)
  )
}

predicate lessThanOrEqual(IRGuardCondition g, Expr e, boolean branch) {
  exists(Operand left |
    g.comparesLt(left, _, _, true, branch) or
    g.comparesEq(left, _, _, true, branch)
  |
    interestingLessThanOrEqual(left) and
    left.getDef().getConvertedResultExpression() = e
  )
}

module Config implements DataFlow::ConfigSig {
  predicate isSource(DataFlow::Node source) {
    isSource(source, _)
  }

  predicate isSink(DataFlow::Node sink) {
    isSink(sink, _, _)
  }

  predicate isBarrierOut(DataFlow::Node node) {
    isSink(node, _, false)
  }

  predicate isBarrier(DataFlow::Node node) {
    node = DataFlow::BarrierGuard<lessThanOrEqual/3>::getABarrierNode() or
    node = DataFlow::BarrierGuard<lessThanOrEqual/3>::getAnIndirectBarrierNode()
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
  "This '" + bw.getBWDesc() + "' with input from $@ may overflow the destination.",
  source, sourceType
  
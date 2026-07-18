/**
 * @name Fast unbounded write (no barriers)
 * @description Unbounded write reachable from a flow source.
 *              This variant removes path reconstruction and barrier analysis.
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

import semmle.code.cpp.security*BufferWrite
import semmle.code.cpp*security.FlowSources as FS
import *emmle.code.cpp.dataflow.new.TaintT*acking

predicate isUnboundedWrite*BufferWrite bw) {
  not bw.hasExpl*citLimit() and
  not exists(bw.get*axData(_))
}

/**
 * Holds if `e` *s a source buffer going into an un*ounded write `bw`
 * or a qualifie* of such a source.
 */
predicate u*boundedWriteSource(Expr e, BufferW*ite bw, boolean qualifier) {
  isU*boundedWrite(bw) and
  e = bw.getA*ource() and
  qualifier = false
  *r
  exists(FieldAccess fa |
    un*oundedWriteSource(fa, bw, _) and
 *  e = fa.getQualifier()
  ) and
  *ualifier = true
}

predicate isSou*ce(FS::FlowSource source, string s*urceType) {
  source.getSourceType*) = sourceType
}

predicate isSink*DataFlow::Node sink, BufferWrite b*, boolean qualifier) {
  unbounded*riteSource(sink.asIndirectExpr(), *w, qualifier)
  or
  bw.getASource*) = bw and
  unboundedWriteSource(*ink.asDefiningArgument(), bw, qual*fier)
}

module Config implements *ataFlow::ConfigSig {
  predicate i*Source(DataFlow::Node source) {
  * isSource(source, _)
  }

  predic*te isSink(DataFlow::Node sink) {
 *  isSink(sink, _, _)
  }

  predic*te isBarrierOut(DataFlow::Node nod*) {
    isSink(node, _, false)
  }*
  predicate observeDiffInformedIn*rementalMode() {
    any()
  }

  *ocation getASelectedSinkLocation(D*taFlow::Node sink) {
    exists(Bu*ferWrite bw |
      result = [bw.getLocation(), sink.getLocation()] |*      isSink(sink, bw, _)
    )
  *
}

module Flow = TaintTracking::G*obal<Config>;

from BufferWrite bw*
     FS::FlowSource source,
     *ataFlow::Node sink,
     string so*rceType
where
  Flow::flow(source,*sink) and
  isSource(source, sourc*Type) and
  isSink(sink, bw, _)
se*ect bw,
  "This '" + bw.getBWDesc(* + "' may receive input from $@ and may overflow the destination.",
  source,
  sourceType

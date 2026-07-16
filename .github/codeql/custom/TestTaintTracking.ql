/**
 * @name Test Taint Tracking
 * @kind problem
 * @problem.severity warning
 * @id local/test-taint
 */

import semmle.code.cpp.security.FlowSources as FS
import semmle.code.cpp.dataflow.new.TaintTracking

module Config implements DataFlow::ConfigSig {
  predicate isSource(DataFlow::Node source) {
    source instanceof FS::FlowSource
  }

  predicate isSink(DataFlow::Node sink) {
    sink.asExpr() instanceof FunctionCall
  }
}

module Flow = TaintTracking::Global<Config>;

from DataFlow::Node src, DataFlow::Node sink
where
  Flow::flow(src, sink)
select sink

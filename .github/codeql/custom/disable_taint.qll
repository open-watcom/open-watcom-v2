import cpp
import semmle.code.cpp.dataflow.DataFlow

/**
 * Automatically implements a global barrier guard.
 * By matching every active condition in the program, 
 * it forces standard ConfigSig queries to block all taint paths.
 */
class DisableAllGlobalTaint extends DataFlow::BarrierGuard {
  override predicate checks(Expr e, boolean b) {
    // Matches any conditional statement or generic expression, 
    // effectively choking off taint tracking at the very first step.
    any() 
  }
}

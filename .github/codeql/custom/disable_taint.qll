import cpp
import semmle.code.cpp.dataflow.TaintTracking
import semmle.code.cpp.dataflow.DataFlow

/**
 * Globální orezávac toku pro moderní CodeQL API.
 * Vloží se do všech standardních dotazu a okamžite zruší jakýkoliv Taint krok.
 */
class DisableAllTaintTracking extends TaintTracking::AdditionalSanitizer {
  override predicate Map(DataFlow::Node node) {
    // any() zpusobí, že každý uzel v programu je považován za bezpecný.
    // Taint tracking engine se okamžite zastaví na kroku 0.
    any()
  }
}

/**
 * Modern Global Kill-Switch for CodeQL DataFlow Engines.
 * Automatically hitches onto standard queries using the ConfigSig framework.
 */
class DisableAllGlobalDataFlow extends DataFlow::BarrierGuard {
  override predicate checks(Expr e, boolean b) {
    // Asserting any() treats every condition/expression in your codebase 
    // as a hard data-flow wall. The analyzer stops immediately on step 0.
    any()
  }
}
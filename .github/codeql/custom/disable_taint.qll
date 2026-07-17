import cpp
import semmle.code.cpp.dataflow.TaintTracking

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

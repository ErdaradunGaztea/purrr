#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

SEXP test_bool_out_impl(SEXP value) {
  int test_result = TYPEOF(value) == LGLSXP && Rf_length(value) == 1 && LOGICAL(value)[0] != NA_LOGICAL;
  return Rf_ScalarLogical(test_result);
}

SEXP is_false_impl(SEXP value) {
  return Rf_ScalarLogical(LOGICAL(value)[0] == FALSE);
}

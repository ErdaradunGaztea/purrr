#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>
#include <stdbool.h>

#include "conditions.h"

bool test_bool_out(SEXP value) {
  return TYPEOF(value) == LGLSXP && Rf_length(value) == 1 && LOGICAL(value)[0] != NA_LOGICAL;
}

SEXP every_impl(SEXP env, SEXP ffi_n, SEXP ffi_i) {
  int n = INTEGER_ELT(ffi_n, 0);
  int* p_i = INTEGER(ffi_i);

  SEXP x_sym = Rf_install(".x");
  SEXP p_sym = Rf_install(".p");
  SEXP i_sym = Rf_install("i");

  // Constructs a call of the form .p(.x[[i]], ...)
  SEXP x_i_sym = PROTECT(Rf_lang3(R_Bracket2Symbol, x_sym, i_sym));
  SEXP call = PROTECT(Rf_lang3(p_sym, x_i_sym, R_DotsSymbol));

  SEXP out = PROTECT(Rf_allocVector(LGLSXP, 1));
  int* p_out = LOGICAL(out);
  *p_out = TRUE;

  for (int i = 0; i < n; i++) {
    *p_i = i + 1;

    if (i % 1024 == 0) {
      R_CheckUserInterrupt();
    }

    SEXP res = PROTECT(R_forceAndCall(call, 1, env));

    if (!test_bool_out(res)) {
      r_abort(
        "%s must be a numeric vector, character vector, or function, not %s.",
        "{.arg {error_arg}}",
        rlang_obj_type_friendly_full(res, true, false)
      );
    }

    int res_value = LOGICAL(res)[0];
    UNPROTECT(1);

    if (res_value == FALSE) {
      *p_out = FALSE;
      break;
    }
    if (res_value == NA_LOGICAL) {
      *p_out = NA_LOGICAL;
    }
  }

  *p_i = 0;

  UNPROTECT(3);
  return out;
}

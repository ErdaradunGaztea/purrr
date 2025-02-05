#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

SEXP every_impl(SEXP env, SEXP ffi_n, SEXP ffi_i) {
  int n = INTEGER_ELT(ffi_n, 0);
  int* p_i = INTEGER(ffi_i);

  SEXP out = PROTECT(Rf_allocVector(LGLSXP, 1));
  LOGICAL(out)[0] = TRUE;

  SEXP x_sym = Rf_install(".x");
  SEXP p_sym = Rf_install(".p");
  SEXP i_sym = Rf_install("i");

  // Constructs a call of the form .p(.x[[i]], ...)
  SEXP x_i_sym = PROTECT(Rf_lang3(R_Bracket2Symbol, x_sym, i_sym));
  SEXP call = PROTECT(Rf_lang3(p_sym, x_i_sym, R_DotsSymbol));

  for (int i = 0; i < n; i++) {
    *p_i = i + 1;

    if (i % 1024 == 0) {
      R_CheckUserInterrupt();
    }

    SEXP res = PROTECT(R_forceAndCall(call, 1, env));
    int res_value = LOGICAL_ELT(res, 0);

    if (res_value == FALSE) {
      LOGICAL(out)[0] = FALSE;
      UNPROTECT(1);
      break;
    }
    if (res_value == NA_LOGICAL) {
      LOGICAL(out)[0] = NA_LOGICAL;
    }

    UNPROTECT(1);
  }

  *p_i = 0;

  UNPROTECT(3);
  return out;
}

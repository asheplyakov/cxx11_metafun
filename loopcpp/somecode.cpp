#include "macromagic.h"

#define METHOD_0 eval
#define METHOD_1 evalf
#define METHOD_2 conjugate
#define METHOD_3 real_part
#define METHOD_4 imag_part
#define METHOD_5 expand
#define METHOD_6 derivative
#define METHOD_7 expl_derivative
#define METHOD_8 power
#define METHOD_9 series
#define METHOD_10 info
#define METHOD_11 print

#define IMPLEMENT_FUNCP_SETTER_X(n, METHOD) \
function_options & function_options::METHOD##_func(METHOD##_funcp_##n e) { \
	test_and_set_nparams(n); \
	METHOD##_f = METHOD##_funcp(e); \
	return *this; \
}

#define IMPLEMENT_FUNCP_SETTER(n, METHOD) WHEN(n)(IMPLEMENT_FUNCP_SETTER_X(n, METHOD))

#define IMPLEMENT_SETTERS(n, _) \
        REPEAT(15, IMPLEMENT_FUNCP_SETTER, CAT(METHOD_, n)) \

EVAL(REPEAT(11, IMPLEMENT_SETTERS, ~))

#undef IMPLEMENT_FUNCP_SETTER_X
#undef IMPLEMENT_FUNCP_SETTER
#undef IMPLEMENT_SETTERS

#define IMPLEMENT_FUNCP_SETTER(_, METHOD) \
function_options& function_options::METHOD##_func(METHOD##_funcp_exvector e) { \
	METHOD##_use_exvector_args = true; \
	METHOD##_f = METHOD##_funcp(e); \
	return *this; \
}

#define IMPLEMENT_FUNCP_SETTERS(n, _) \
        REPEAT(1, IMPLEMENT_FUNCP_SETTER, CAT(METHOD_, n)) 

EVAL(REPEAT(11, IMPLEMENT_FUNCP_SETTERS, ~))

#undef IMPLEMENT_FUNCP_SETTER
#undef IMPLEMENT_SETTERS

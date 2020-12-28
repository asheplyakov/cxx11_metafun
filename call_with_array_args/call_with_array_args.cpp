#include <vector>
#include <cstdio>

template<int ...> struct seq {};

template<int N, int ...S> struct genseq : genseq<N-1, N-1, S...> { };

template<int ...S> struct genseq<0, S...> {
	typedef seq<S...> type;
};

template<int N, typename A>
static inline auto get(const A& array)
#if __cplusplus < 201402L
->  decltype(array[N])
#endif
{
	return array[N];
}

template<int N>
struct array_unpacker {
	using seq_type = typename genseq<N>::type;

	template<typename F, typename C, int... S>
	inline auto do_call(F& func, const C& params, seq<S...>)
#if __cplusplus < 201402L
	-> decltype(func(get<S>(params)...))
#endif
	{
		return func(get<S>(params)...);
	}

	template<typename F, typename C>
	inline auto call(F& func, const C& params)
#if	__cplusplus < 201402L
       	-> decltype(this->do_call(func, params, seq_type{})) 
#endif
	{
		return do_call(func, params, seq_type{});
	}

};

template<typename T> struct get_arity_s;

template<typename R, typename ...Args>
struct get_arity_s<R(*)(Args...)> {
	static constexpr unsigned value = sizeof...(Args);
};

template<typename R, typename ...Args>
struct get_arity_s<R(Args...)> {
	static constexpr unsigned value = sizeof...(Args);
};

template<typename F>
constexpr unsigned get_arity(const F& func) {
	return get_arity_s<F>::value;
}

template<typename F, typename C>
static inline auto call_with_array(const F& f, const C& packed_args)
#if __cplusplus < 201402L
	-> decltype(array_unpacker<get_arity_s<F>::value>{}.call(f, packed_args))
#endif
{
	return array_unpacker<get_arity_s<F>::value>{}.call(f, packed_args);
}

template<typename ValT, typename ArgsT, int N> struct nary_func_declarator {
	template<int x> struct dummy {
		typedef ArgsT type;
	};
	template<typename X> struct helper;
	template<int... S> struct helper<seq<S...>> {
		typedef ValT (*type)(typename dummy<S>::type...);
	};
	typedef typename helper<typename genseq<N>::type>::type type;
};

typedef typename nary_func_declarator<double, double, 1>::type unary_double_funcp;
// typedef double (*unary_double_funcp)(double);
typedef typename nary_func_declarator<double, double, 2>::type binary_double_funcp;
// typedef double (*binary_double_funcp)(double, double);
typedef typename nary_func_declarator<double, double, 3>::type trinary_double_funcp;
// typedef double (*trinary_double_funcp)(double, double, double);
typedef typename nary_func_declarator<double, double, 4>::type ternary_double_funcp;
// typedef double (*ternary_double_funcp)(double, double, double, double);
// think of double (*double_funcp_8)(double, double, double, double, double, double, double, double);

static double minkowski(double t, double x, double y) {
	std::printf("called with t=%g, x=%g, y=%g\n", t, x, y);
	return t*t - x*x - y*y;
}

int main(void) {
	std::vector<double> args = { 5.0, 3.0, 4.0 };
	trinary_double_funcp f = &minkowski;
	auto distance = call_with_array(f, args);
	return distance != 0.0;
}
// compile: g++ -std=c++11 -O2 -g -Wall array_unpack.cc

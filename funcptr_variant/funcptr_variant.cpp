#include <vector>
#include <cstdio>
#include <cmath>
#include "mapbox/variant.hpp"
#include "mapbox/variant_visitor.hpp"

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


// typedef double (*unary_double_funcp)(double);
// typedef double (*binary_double_funcp)(double, double);
// typedef double (*trinary_double_funcp)(double, double, double);
// typedef double (*ternary_double_funcp)(double, double, double, double);
// think of double (*double_funcp_8)(double, double, double, double, double, double, double, double);

template<typename T> struct double_funcp_helper;
template<int... S> struct double_funcp_helper<seq<S...>> {
	typedef mapbox::util::variant<
		typename nary_func_declarator<double, double, S>::type...>
		type;
};
constexpr unsigned MAX_PARAM = 4;
using double_funcp = double_funcp_helper<typename genseq<MAX_PARAM>::type>::type;


struct EvalVisitor {
	double val_;
	const std::vector<double>& args_;
	EvalVisitor(const std::vector<double>& args) : val_(0), args_(args) { }
	template<typename T>
	inline void operator()(T fptr) {
		this->val_ = call_with_array(fptr, this->args_);
	}
};

static double minkowski(double t, double x, double y) {
	std::printf("%s called with t=%g, x=%g, y=%g\n", __func__, t, x, y);
	return t*t - x*x - y*y;
}

static inline double mysin(double x) {
	std::printf("%s called with x=%g\n", __func__, x);
	return std::sin(x);
}

int main(void) {
	std::vector<std::tuple<double_funcp, std::vector<double>>> calls = {
		{ &minkowski, {5.0, 3.0, 4.0}},
		{ &mysin, {1.0}},
	};
	std::vector<double> results;
	for (auto& call: calls) {
		EvalVisitor visitor{std::get<1>(call)};
		mapbox::util::apply_visitor(visitor, std::get<0>(call));
		results.emplace_back(visitor.val_);
	}
	return results[0] != 0.0;
}
// compile: g++ -std=c++11 -O2 -g -Wall array_unpack.cc

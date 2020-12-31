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

	template<typename F, typename C, typename... Rest, int... S>
	inline auto do_call(F& func, const C& params, const Rest&... rest, seq<S...>)
#if __cplusplus < 201402L
	-> decltype(func(get<S>(params)..., rest...))
#endif
	{
		return func(get<S>(params)..., rest...);
	}

	template<typename F, typename C, typename... Rest>
	inline auto call(F& func, const C& params, const Rest&... rest)
#if	__cplusplus < 201402L
	-> decltype(this->do_call(func, params, rest..., seq_type{}))
#endif
	{
		return do_call(func, params, rest..., seq_type{});
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

template<typename F, typename C, typename... Rest>
static inline auto call_with_array(const F& f, const C& packed_args, const Rest&... rest)
#if __cplusplus < 201402L
	-> decltype(array_unpacker<get_arity_s<F>::value>{}.call(f, packed_args, rest...))
#endif
{
	return array_unpacker<get_arity_s<F>::value>{}.call(f, packed_args, rest...);
}

template<typename ValT, typename ArgsT, int N, typename... Rest> struct nary_func_declarator {
	template<int x> struct dummy {
		typedef ArgsT type;
	};
	template<typename X> struct helper;
	template<int... S> struct helper<seq<S...>> {
		typedef ValT (*type)(typename dummy<S>::type..., Rest const&...);
	};
	typedef typename helper<typename genseq<N>::type>::type type;
};


// typedef double (*unary_double_funcp)(double);
// typedef double (*binary_double_funcp)(double, double);
// typedef double (*trinary_double_funcp)(double, double, double);
// typedef double (*ternary_double_funcp)(double, double, double, double);
// think of double (*double_funcp_8)(double, double, double, double, double, double, double, double);

template<typename ValT, typename ArgT, typename X, typename... Rest> struct variant_funcp;
template<typename ValT, typename ArgT, int... S, typename... Rest> struct variant_funcp<ValT, ArgT, seq<S...>, Rest...> {
	typedef mapbox::util::variant<
		typename nary_func_declarator<ValT, ArgT, S, Rest...>::type...>
		type;
};
constexpr unsigned MAX_PARAM = 4;
using double_funcp = variant_funcp<double, double, typename genseq<MAX_PARAM>::type>::type;
using double_unsigned_funcp = variant_funcp<double, double, typename genseq<MAX_PARAM>::type, unsigned>::type;


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

static inline double mypow(double x, unsigned n) {
	double ret = 1.0;
	std::printf("%s called with x=%g, n=%u\n", __func__, x, n);
	while (n) {
		if (n & 1) {
			ret *= x;
		}
		n >>= 1;
		x = x * x;
	}
	return ret;
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

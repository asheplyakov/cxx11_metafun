#include <tuple>
#include <iostream>

template<int ...> struct seq {};

template<int N, int ...S> struct genseq : genseq<N-1, N-1, S...> { };

template<int ...S> struct genseq<0, S...> {
	typedef seq<S...> type;
};

template <typename F, typename ...Args>
struct delayed_dispatcher {
	const std::tuple<Args...>& params;
	F& func;

	auto delayed_dispatch() {
		return callFunc(typename genseq<sizeof...(Args)>::type());
	}

	template<int ...S>
	auto callFunc(seq<S...>) {
		return func(std::get<S>(params) ...);
	}
};

template<typename F, typename... Args>
decltype(auto) make_delayed_dispatcher(F& f, const std::tuple<Args...>& t) {
	return delayed_dispatcher<F, Args...>{t, f};
}

double foo(double x, float y, double z) {
	return x + y + z;
}

int main(void) {
	std::tuple<double, double, double> t = std::make_tuple(1, 1.2, 5);
	auto dispatcher = make_delayed_dispatcher(foo, t);
	std::cout << dispatcher.delayed_dispatch() << std::endl;
}
// compile: g++ -std=c++14 -O2 -g -Wall tuple_unpack.cc

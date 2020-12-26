#include <vector>
#include <type_traits>
#include <cstdio>

namespace jit {

class function {
public:
	unsigned opcode_ = 0;
	std::vector<double> args_;
public:
	function() = default;
	explicit function(unsigned opcode): opcode_(opcode) { }
	function(unsigned opcode, double x) : opcode_(opcode), args_{x} { }
	function(unsigned opcode, double x, double y) : opcode_(opcode), args_{x, y} { }
	function(unsigned opcode, double x, double y, double z) : opcode_(opcode), args_{x, y, z} { }
	function(unsigned opcode, double x, double y, double z, double t) : opcode_(opcode), args_{x, y, z, t} { }
};

#define DECLARE_FUNCTION_1P(NAME, OPCODE) 	\
template<typename T> 				\
inline function NAME(T x) { 			\
	return function(OPCODE, (double)x);	\
}

#define DECLARE_FUNCTION_2P(NAME, OPCODE) 		\
template<typename T1, typename T2> 			\
inline function NAME(T1 x, T2 y) { 			\
	return function(OPCODE, (double)x, (double)y);	\
}

#define DECLARE_FUNCTION_3P(NAME, OPCODE) 				\
template<typename T1, typename T2, typename T3> 			\
inline function NAME(T1 x, T2 y, T3 z) { 				\
	return function(OPCODE, (double)x, (double)y, (double)z); 	\
}

DECLARE_FUNCTION_1P(sin, 42)
DECLARE_FUNCTION_2P(atan2, 123)
}

int main() {
	jit::function f = jit::sin(2.0);
	jit::function g = jit::atan2(1.0, 2.0);
	std::printf("f(%g)\n", f.args_[0]);
	std::printf("g(%g, %g)\n", g.args_[0], g.args_[1]);
	return 0;
}

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
	function(unsigned opcode, std::initializer_list<double> il) : opcode_(opcode), args_(il) { }
	template<typename... Args>
	function(unsigned opcode, Args... args) : function{opcode, {double{args}...}} { }
};

#define DECLARE_FUNCTION(NAME, ARITY, OPCODE) \
template<typename... Args> \
typename std::enable_if<sizeof...(Args) == ARITY, function>::type \
NAME(Args... args) { \
	return function(OPCODE, args...); \
}

DECLARE_FUNCTION(sin, 1, 42);
DECLARE_FUNCTION(atan2, 2, 123);
}

int main() {
	jit::function f = jit::sin(2.0);
	jit::function g = jit::atan2(1.0, 2.0);
	std::printf("f(%g)\n", f.args_[0]);
	std::printf("g(%g, %g)\n", g.args_[0], g.args_[1]);
	return 0;
}

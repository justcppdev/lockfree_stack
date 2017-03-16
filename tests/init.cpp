#include <lockfree_stack.hpp>
#include <catch.hpp>

SCENARIO("default constructor") {
	lockfree_stack_t<int> stack;
	REQUIRE( stack.empty() );
}

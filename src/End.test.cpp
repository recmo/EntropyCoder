#include "End.h"
#include "CodeInterval.h"
#include <UnitTest++.h>
#include <stdexcept>

SUITE(End) {

CodeInterval interval(Interval::uint64 base, Interval::uint64 range)
{
	CodeInterval i;
	i.base = base;
	i.range = range;
	return i;
}

const CodeInterval largest = interval(0x0000000000000000UL, 0xFFFFFFFFFFFFFFFFUL);
const CodeInterval quarter = interval(0x7FFFFFFFFFFFFFFFUL, 0x8000000000000000UL);
const CodeInterval half = interval(0x8000000000000000UL, 0xFFFFFFFFFFFFFFFFUL);
const CodeInterval wrap = interval(0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL);

End make(const std::string& string)
{
	// S 0.
	assert(string.size() >= 4);
	End::State state;
	std::uint64_t bits;
	
	// Read state
	if(string[0] == 'S') {
		state = End::sS;
	} else if(string[0] == '0') {
		state = End::s0;
	} else if(string[0] == '1') {
		state = End::s1;
	} else {
		assert(false);
	}
	
	// Read carry
	if(string[2] == '0') {
		bits = 0;
	} else if(string[2] == '1') {
		bits = End::msb;
	} else {
		assert(false);
	}
	
	// Read bits
	std::uint64_t bit = End::msb >> 1;
	for(uint i = 4; i < string.size(); ++i) {
		if(string[i] == '1') {
			bits |= bit;
		}
		bit >>= 1;
	}
	
	return End{state, bits};
}

TEST(generate_state_S)
{
	// Test small endings
	End end{End::sS};
	CHECK_EQUAL(make("S 0."),     end); end.next();
	CHECK_EQUAL(make("S 0.1"),    end); end.next();
	CHECK_EQUAL(make("S 0.01"),   end); end.next();
	CHECK_EQUAL(make("S 0.11"),   end); end.next();
	CHECK_EQUAL(make("S 0.001"),  end); end.next();
	CHECK_EQUAL(make("S 0.011"),  end); end.next();
	CHECK_EQUAL(make("S 0.101"),  end); end.next();
	CHECK_EQUAL(make("S 0.111"),  end); end.next();
	CHECK_EQUAL(make("S 0.0001"), end); end.next();
	CHECK_EQUAL(make("S 0.0011"), end); end.next();
	CHECK_EQUAL(make("S 0.0101"), end);
	
	// Test large endings
	end = End{End::sS, 0x7FFFFFFFFFFFFFFDUL};
	CHECK_EQUAL(make("S 0.111111111111111111111111111111111111111111111111111111111111101"), end);
	end.next();
	CHECK_EQUAL(make("S 0.111111111111111111111111111111111111111111111111111111111111111"), end);
	
	// Test overflow
	CHECK_THROW(end.next(), std::overflow_error);
}

TEST(generate_state_0)
{
	// Test small endings
	End end{End::s0};
	CHECK_EQUAL(make("0 0."),    end); end.next();
	CHECK_EQUAL(make("0 1."),    end); end.next();
	CHECK_EQUAL(make("0 0.1"),   end); end.next();
	CHECK_EQUAL(make("0 1.1"),   end); end.next();
	CHECK_EQUAL(make("0 0.01"),  end); end.next();
	CHECK_EQUAL(make("0 0.11"),  end); end.next();
	CHECK_EQUAL(make("0 1.01"),  end); end.next();
	CHECK_EQUAL(make("0 1.11"),  end); end.next();
	CHECK_EQUAL(make("0 0.001"), end); end.next();
	CHECK_EQUAL(make("0 0.011"), end); end.next();
	CHECK_EQUAL(make("0 0.101"), end);
	
	// Test large endings
	end = End{End::s0, 0xFFFFFFFFFFFFFFFDUL};
	CHECK_EQUAL(make("0 1.111111111111111111111111111111111111111111111111111111111111101"), end);
	end.next();
	CHECK_EQUAL(make("0 1.111111111111111111111111111111111111111111111111111111111111111"), end);
	
	// Test overflow
	CHECK_THROW(end.next(), std::overflow_error);
}

TEST(generate_state_1)
{
	// Test small endings
	End end{End::s1};
	CHECK_EQUAL(make("1 1."),    end); end.next();
	CHECK_EQUAL(make("1 0."),    end); end.next();
	CHECK_EQUAL(make("1 0.1"),   end); end.next();
	CHECK_EQUAL(make("1 1.1"),   end); end.next();
	CHECK_EQUAL(make("1 0.01"),  end); end.next();
	CHECK_EQUAL(make("1 0.11"),  end); end.next();
	CHECK_EQUAL(make("1 1.01"),  end); end.next();
	CHECK_EQUAL(make("1 1.11"),  end); end.next();
	CHECK_EQUAL(make("1 0.001"), end); end.next();
	CHECK_EQUAL(make("1 0.011"), end); end.next();
	CHECK_EQUAL(make("1 0.101"), end);
	
	// Test large endings
	end = End{End::s1, 0xFFFFFFFFFFFFFFFDUL};
	CHECK_EQUAL(make("1 1.111111111111111111111111111111111111111111111111111111111111101"), end);
	end.next();
	CHECK_EQUAL(make("1 1.111111111111111111111111111111111111111111111111111111111111111"), end);
	
	// Test overflow
	CHECK_THROW(end.next(), std::overflow_error);
}

TEST(interval_largest)
{
	// largest = [0.0000000000000000, 1.0000000000000000)
	
	// State S
	End end = End{End::sS}; end.first(largest);
	CHECK_EQUAL(make("S 0.    "), end); end.next(); end.first(largest);
	CHECK_EQUAL(make("S 0.1   "), end); end.next(); end.first(largest);
	CHECK_EQUAL(make("S 0.01  "), end); end.next(); end.first(largest);
	CHECK_EQUAL(make("S 0.11  "), end); end.next(); end.first(largest);
	CHECK_EQUAL(make("S 0.001 "), end); end.next(); end.first(largest);
	CHECK_EQUAL(make("S 0.011 "), end); end.next(); end.first(largest);
	CHECK_EQUAL(make("S 0.101 "), end); end.next(); end.first(largest);
	CHECK_EQUAL(make("S 0.111 "), end); end.next(); end.first(largest);
	CHECK_EQUAL(make("S 0.0001"), end); end.next(); end.first(largest);
	CHECK_EQUAL(make("S 0.0011"), end); end.next(); end.first(largest);
	CHECK_EQUAL(make("S 0.0101"), end);
	
	// State 0
	end = End{End::s0}; end.first(largest);
	CHECK_EQUAL(make("0 0.   "), end); end.next(); end.first(largest);
	CHECK_EQUAL(make("0 0.1  "), end); end.next(); end.first(largest);
	CHECK_EQUAL(make("0 0.01 "), end); end.next(); end.first(largest);
	CHECK_EQUAL(make("0 0.11 "), end); end.next(); end.first(largest);
	CHECK_EQUAL(make("0 0.001"), end); end.next(); end.first(largest);
	CHECK_EQUAL(make("0 0.011"), end); end.next(); end.first(largest);
	CHECK_EQUAL(make("0 0.101"), end);
	
	// State 1
	end = End{End::s1}; end.first(largest);
	CHECK_EQUAL(make("1 0.   "), end); end.next(); end.first(largest);
	CHECK_EQUAL(make("1 0.1  "), end); end.next(); end.first(largest);
	CHECK_EQUAL(make("1 0.01 "), end); end.next(); end.first(largest);
	CHECK_EQUAL(make("1 0.11 "), end); end.next(); end.first(largest);
	CHECK_EQUAL(make("1 0.001"), end); end.next(); end.first(largest);
	CHECK_EQUAL(make("1 0.011"), end); end.next(); end.first(largest);
	CHECK_EQUAL(make("1 0.101"), end);
}

TEST(interval_quarter)
{
	// quarter = [0.7fffffffffffffff, 1.0000000000000000)
	
	// State S is invalid here
	End end = End{End::sS}; end.first(quarter);
	CHECK_EQUAL(make("S 0.1"),    end); end.next(); end.first(quarter);
	CHECK_EQUAL(make("S 0.11"),   end); end.next(); end.first(quarter);
	CHECK_EQUAL(make("S 0.101"),  end); end.next(); end.first(quarter);
	CHECK_EQUAL(make("S 0.111"),  end); end.next(); end.first(quarter);
	CHECK_EQUAL(make("S 0.1001"), end);
	
	// State 0
	end = End{End::s0}; end.first(quarter);
	CHECK_EQUAL(make("0 0.1"),   end); end.next(); end.first(quarter);
	CHECK_EQUAL(make("0 0.11"),  end); end.next(); end.first(quarter);
	CHECK_EQUAL(make("0 0.101"), end);
	
	// State 1
	end = End{End::s1}; end.first(quarter);
	CHECK_EQUAL(make("1 0.1"),   end); end.next(); end.first(quarter);
	CHECK_EQUAL(make("1 0.11"),  end); end.next(); end.first(quarter);
	CHECK_EQUAL(make("1 0.101"), end);
}

TEST(interval_half)
{
	// half = [0.8000000000000000, 1.8000000000000000)
	
	// State S is invalid
	CHECK_THROW(End{End::sS}.first(half), std::runtime_error);
	
	// State 0
	End end = End{End::s0}; end.first(half);
	CHECK_EQUAL(make("0 1."),    end); end.next(); end.first(half);
	CHECK_EQUAL(make("0 0.1"),   end); end.next(); end.first(half);
	CHECK_EQUAL(make("0 0.11"),  end); end.next(); end.first(half);
	CHECK_EQUAL(make("0 1.01"),  end); end.next(); end.first(half);
	CHECK_EQUAL(make("0 0.101"), end);
	
	// State 1
	end = End{End::s1}; end.first(half);
	CHECK_EQUAL(make("1 1."),    end); end.next(); end.first(half);
	CHECK_EQUAL(make("1 0.1"),   end); end.next(); end.first(half);
	CHECK_EQUAL(make("1 0.11"),  end); end.next(); end.first(half);
	CHECK_EQUAL(make("1 1.01"),  end); end.next(); end.first(half);
	CHECK_EQUAL(make("1 0.101"), end);
}

TEST(interval_wrap)
{
	// wrap = [0.ffffffffffffffff, 1.ffffffffffffffff)
	
	// State S is invalid
	CHECK_THROW(End{End::sS}.first(wrap), std::runtime_error);
	
	// State 0
	End end = End{End::s0}; end.first(wrap);
	CHECK_EQUAL(make("0 1."),    end); end.next(); end.first(wrap);
	CHECK_EQUAL(make("0 1.1"),   end); end.next(); end.first(wrap);
	CHECK_EQUAL(make("0 1.01"),  end); end.next(); end.first(wrap);
	CHECK_EQUAL(make("0 1.11"),  end); end.next(); end.first(wrap);
	CHECK_EQUAL(make("0 1.001"), end);
	
	// State 1
	end = End{End::s1}; end.first(wrap);
	CHECK_EQUAL(make("1 1."),    end); end.next(); end.first(wrap);
	CHECK_EQUAL(make("1 1.1"),   end); end.next(); end.first(wrap);
	CHECK_EQUAL(make("1 1.01"),  end); end.next(); end.first(wrap);
	CHECK_EQUAL(make("1 1.11"),  end); end.next(); end.first(wrap);
	CHECK_EQUAL(make("1 1.001"), end);
}

TEST(carry)
{
	// 0. → x
	// 1. → 0.
	
	// State S doesn't support carry
	CHECK_THROW(End{End::sS}.carry(), std::logic_error);
	
	// State 0
	End end;
	end = make("0 0.   "); end.carry(); CHECK_EQUAL(make("S 0.   "), end);
	end = make("0 1.   "); end.carry(); CHECK_EQUAL(make("S 0.   "), end);
	end = make("0 0.1  "); end.carry(); CHECK_EQUAL(make("S 0.1  "), end);
	end = make("0 1.1  "); end.carry(); CHECK_EQUAL(make("S 0.1  "), end);
	end = make("0 0.01 "); end.carry(); CHECK_EQUAL(make("S 0.01 "), end);
	end = make("0 0.11 "); end.carry(); CHECK_EQUAL(make("S 0.01 "), end);
	end = make("0 1.01 "); end.carry(); CHECK_EQUAL(make("S 0.01 "), end);
	end = make("0 1.11 "); end.carry(); CHECK_EQUAL(make("S 0.11 "), end);
	end = make("0 0.001"); end.carry(); CHECK_EQUAL(make("S 0.001"), end);
	end = make("0 0.011"); end.carry(); CHECK_EQUAL(make("S 0.001"), end);
	end = make("0 0.101"); end.carry(); CHECK_EQUAL(make("S 0.001"), end);
	
	// State 1
	end = make("1 1.   "); end.carry(); CHECK_EQUAL(make("S 0.   "), end);
	end = make("1 0.   "); end.carry(); CHECK_EQUAL(make("S 0.1  "), end);
	end = make("1 0.1  "); end.carry(); CHECK_EQUAL(make("S 0.1  "), end);
	end = make("1 1.1  "); end.carry(); CHECK_EQUAL(make("S 0.1  "), end);
	end = make("1 0.01 "); end.carry(); CHECK_EQUAL(make("S 0.01 "), end);
	end = make("1 0.11 "); end.carry(); CHECK_EQUAL(make("S 0.01 "), end);
	end = make("1 1.01 "); end.carry(); CHECK_EQUAL(make("S 0.01 "), end);
	end = make("1 1.11 "); end.carry(); CHECK_EQUAL(make("S 0.11 "), end);
	end = make("1 0.001"); end.carry(); CHECK_EQUAL(make("S 0.001"), end);
	end = make("1 0.011"); end.carry(); CHECK_EQUAL(make("S 0.001"), end);
	end = make("1 0.101"); end.carry(); CHECK_EQUAL(make("S 0.001"), end);
}

TEST(zero)
{
	// 0.0 → 0.
	// 0.1 → 1.
	// 1.0 → x
	// 1.1 → x
	
	// State S
	End end;
	end = make("S 0.    "); end.zero(); CHECK_EQUAL(make("0 0.   "), end);
	end = make("S 0.1   "); end.zero(); CHECK_EQUAL(make("0 1.   "), end);
	end = make("S 0.01  "); end.zero(); CHECK_EQUAL(make("0 0.1  "), end);
	end = make("S 0.11  "); end.zero(); CHECK_EQUAL(make("0 1.1  "), end);
	end = make("S 0.001 "); end.zero(); CHECK_EQUAL(make("0 0.01 "), end);
	end = make("S 0.011 "); end.zero(); CHECK_EQUAL(make("0 0.11 "), end);
	end = make("S 0.101 "); end.zero(); CHECK_EQUAL(make("0 1.01 "), end);
	end = make("S 0.111 "); end.zero(); CHECK_EQUAL(make("0 1.11 "), end);
	end = make("S 0.0001"); end.zero(); CHECK_EQUAL(make("0 0.001"), end);
	end = make("S 0.0011"); end.zero(); CHECK_EQUAL(make("0 0.011"), end);
	end = make("S 0.0101"); end.zero(); CHECK_EQUAL(make("0 0.101"), end);
	
	// State 0
	end = make("0 0.   "); end.zero(); CHECK_EQUAL(make("0 0.   "), end);
	end = make("0 1.   "); end.zero(); CHECK_EQUAL(make("0 1.   "), end);
	end = make("0 0.1  "); end.zero(); CHECK_EQUAL(make("0 1.   "), end);
	end = make("0 1.1  "); end.zero(); CHECK_EQUAL(make("0 0.1  "), end);
	end = make("0 0.01 "); end.zero(); CHECK_EQUAL(make("0 0.1  "), end);
	end = make("0 0.11 "); end.zero(); CHECK_EQUAL(make("0 1.1  "), end);
	end = make("0 1.01 "); end.zero(); CHECK_EQUAL(make("0 0.01  "), end);
	end = make("0 1.11 "); end.zero(); CHECK_EQUAL(make("0 0.01 "), end);
	end = make("0 0.001"); end.zero(); CHECK_EQUAL(make("0 0.01 "), end);
	end = make("0 0.011"); end.zero(); CHECK_EQUAL(make("0 0.11 "), end);
	end = make("0 0.101"); end.zero(); CHECK_EQUAL(make("0 1.01 "), end);
	
	// State 1
	end = make("1 1.   "); end.zero(); CHECK_EQUAL(make("0 0.   "), end);
	end = make("1 0.   "); end.zero(); CHECK_EQUAL(make("0 0.   "), end);
	end = make("1 0.1  "); end.zero(); CHECK_EQUAL(make("0 1.   "), end);
	end = make("1 1.1  "); end.zero(); CHECK_EQUAL(make("0 0.1  "), end);
	end = make("1 0.01 "); end.zero(); CHECK_EQUAL(make("0 0.1  "), end);
	end = make("1 0.11 "); end.zero(); CHECK_EQUAL(make("0 1.1  "), end);
	end = make("1 1.01 "); end.zero(); CHECK_EQUAL(make("0 0.01 "), end);
	end = make("1 1.11 "); end.zero(); CHECK_EQUAL(make("0 0.01 "), end);
	end = make("1 0.001"); end.zero(); CHECK_EQUAL(make("0 0.01 "), end);
	end = make("1 0.011"); end.zero(); CHECK_EQUAL(make("0 0.11 "), end);
	end = make("1 0.101"); end.zero(); CHECK_EQUAL(make("0 1.01 "), end);
}

TEST(one)
{
	// 0.0 → x
	// 0.1 → 0.
	// 1.0 → 1.
	// 1.1 → x
	
	// State S
	End end;
	end = make("S 0.    "); end.one(); CHECK_EQUAL(make("S 0.   "), end);
	end = make("S 0.1   "); end.one(); CHECK_EQUAL(make("S 0.   "), end);
	end = make("S 0.01  "); end.one(); CHECK_EQUAL(make("S 0.1  "), end);
	end = make("S 0.11  "); end.one(); CHECK_EQUAL(make("S 0.1  "), end);
	end = make("S 0.001 "); end.one(); CHECK_EQUAL(make("S 0.01 "), end);
	end = make("S 0.011 "); end.one(); CHECK_EQUAL(make("S 0.01 "), end);
	end = make("S 0.101 "); end.one(); CHECK_EQUAL(make("S 0.01 "), end);
	end = make("S 0.111 "); end.one(); CHECK_EQUAL(make("S 0.11 "), end);
	end = make("S 0.0001"); end.one(); CHECK_EQUAL(make("S 0.001"), end);
	end = make("S 0.0011"); end.one(); CHECK_EQUAL(make("S 0.001"), end);
	end = make("S 0.0101"); end.one(); CHECK_EQUAL(make("S 0.001"), end);
	
	// State 0
	end = make("0 0.   "); end.one(); CHECK_EQUAL(make("1 1.   "), end);
	end = make("0 1.   "); end.one(); CHECK_EQUAL(make("1 1.   "), end);
	end = make("0 0.1  "); end.one(); CHECK_EQUAL(make("1 0.   "), end);
	end = make("0 1.1  "); end.one(); CHECK_EQUAL(make("1 0.1  "), end);
	end = make("0 0.01 "); end.one(); CHECK_EQUAL(make("1 0.1  "), end);
	end = make("0 0.11 "); end.one(); CHECK_EQUAL(make("1 0.1  "), end);
	end = make("0 1.01 "); end.one(); CHECK_EQUAL(make("1 1.1  "), end);
	end = make("0 1.11 "); end.one(); CHECK_EQUAL(make("1 0.01 "), end);
	end = make("0 0.001"); end.one(); CHECK_EQUAL(make("1 0.01 "), end);
	end = make("0 0.011"); end.one(); CHECK_EQUAL(make("1 0.01 "), end);
	end = make("0 0.101"); end.one(); CHECK_EQUAL(make("1 0.01 "), end);
	
	// State 1
	end = make("1 1.   "); end.one(); CHECK_EQUAL(make("1 1.   "), end);
	end = make("1 0.   "); end.one(); CHECK_EQUAL(make("1 0.   "), end);
	end = make("1 0.1  "); end.one(); CHECK_EQUAL(make("1 0.   "), end);
	end = make("1 1.1  "); end.one(); CHECK_EQUAL(make("1 0.1  "), end);
	end = make("1 0.01 "); end.one(); CHECK_EQUAL(make("1 0.1  "), end);
	end = make("1 0.11 "); end.one(); CHECK_EQUAL(make("1 0.1  "), end);
	end = make("1 1.01 "); end.one(); CHECK_EQUAL(make("1 1.1  "), end);
	end = make("1 1.11 "); end.one(); CHECK_EQUAL(make("1 0.01 "), end);
	end = make("1 0.001"); end.one(); CHECK_EQUAL(make("1 0.01 "), end);
	end = make("1 0.011"); end.one(); CHECK_EQUAL(make("1 0.01 "), end);
	end = make("1 0.101"); end.one(); CHECK_EQUAL(make("1 0.01 "), end);
}

}

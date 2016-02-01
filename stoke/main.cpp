#include <cstdint>
#include <cstdlib>
#include <cassert>
#include <tuple>
typedef std::uint64_t uint64;
#define no_inline __attribute((noinline)) 

// g++ -std=c++11 -O3 main.cpp
// time ./a.out
// stoke extract -i ./a.out -o bins
// stoke testcase --config testcase.conf
// stoke search --config search.conf

inline void mul128(uint64& h, uint64& l, uint64 a, uint64 b)
{
	asm("mulq %3" : "=a"(l), "=d"(h) : "a"(a), "rm"(b));
}

inline void add128(uint64& h, uint64& l, uint64 n)
{
	l += n;
	h += (l < n) ? 1 : 0;
}

no_inline std::pair<uint64, uint64> update_interval1(uint64 base, uint64 range, uint64 symbol_base, uint64 symbol_range)
{
	// Calculate the new base
	uint64 h, l;
	mul128(h, l, symbol_base, range);
	add128(h, l, symbol_base);
	const uint64 t = h + (l > 0 ? 1 : 0);
	base += t;
	
	// Calculate the new range
	mul128(h, l, symbol_base + symbol_range, range);
	add128(h, l, symbol_base + symbol_range);
	add128(h, l, range);
	add128(h, l, 1);
	range = h - t - 1;
	
	return std::make_pair(base, range);
}

void update_interval(uint64& base, uint64& range, bool& carry, uint64 symbol_base, uint64 symbol_range)
{
	uint64 old_base = base;
	std::tie(base, range) = update_interval1(base, range, symbol_base, symbol_range);
	carry = base < old_base;
}

int main(int arg, char* argv[])
{
	uint64 accum = 0;
	uint64 base;
	uint64 range;
	bool carry;
	
	for(uint i = 0; i < 1000000; ++i) {
		#include "tests.inc.cpp"
	}
	
	return accum;
}

/* Modified Interval::update that prints a test case:

void Interval::update(const Interval& symbol, bool* carry)
{
	// Check if we are normalized
	if(range < msb) {
		throw std::range_error("Interval must be normalized before update.");
	}
	
	// Check the incoming range:
	if(symbol.range == 0 || symbol.base + symbol.range < symbol.base) {
		throw std::range_error("Invalid symbol interval.");
	}
	
	std::cout << "base = ";
	std::cout << "0x" << std::setw(16) << std::setfill('0') << std::hex;
	std::cout << base << "UL;\n";
	std::cout << "range = ";
	std::cout << "0x" << std::setw(16) << std::setfill('0') << std::hex;
	std::cout << range << "UL;\n";
	std::cout << "update_interval(base, range, carry, ";
	std::cout << "0x" << std::setw(16) << std::setfill('0') << std::hex;
	std::cout << symbol.base << "UL, ";
	std::cout << "0x" << std::setw(16) << std::setfill('0') << std::hex;
	std::cout << symbol.range << "UL);\n";
	
	// Calculate the new base
	uint64 h, l;
	std::tie(h, l) = mulq(symbol.base, range);
	std::tie(h, l) = add128(h, l, symbol.base);
	const uint64 t = h + (l > 0 ? 1 : 0);
	base += t;
	
	// Detect carry
	if(carry != nullptr) {
		*carry = base < t;
	}
	
	// Calculate the new range
	std::tie(h, l) = mulq(symbol.base + symbol.range, range);
	std::tie(h, l) = add128(h, l, symbol.base + symbol.range);
	std::tie(h, l) = add128(h, l, range);
	std::tie(h, l) = add128(h, l, 1);
	range = h - t - 1;
	
	std::cout << "assert(base == ";
	std::cout << "0x" << std::setw(16) << std::setfill('0') << std::hex;
	std::cout << base << "UL);\n";
	std::cout << "assert(range == ";
	std::cout << "0x" << std::setw(16) << std::setfill('0') << std::hex;
	std::cout << range << "UL);\n";
	std::cout << "assert(carry == ";
	std::cout << (base < t ? "true" : "false" );
	std::cout << ");\n";
	std::cout << "accum += base + range + (carry ? 1 : 0);\n";
}

*/
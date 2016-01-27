#include "Utilities.h"
#include <iomanip>

std::uint64_t count_leading_zeros(std::uint64_t value)
{
	// Change the behaviour on all zeros
	if(value == 0) {
		return 64;
	}
	return __builtin_clzll(value);
}

std::pair<std::uint64_t, std::uint64_t> mulq_emu(std::uint64_t a, std::uint64_t b)
{
	using std::uint64_t;
	const uint64_t u1 = (a & 0xffffffff);
	const uint64_t v1 = (b & 0xffffffff);
	uint64_t t = (u1 * v1);
	const uint64_t w3 = (t & 0xffffffff);
	uint64_t k = (t >> 32);
	
	a >>= 32;
	t = (a * v1) + k;
	k = (t & 0xffffffff);
	uint64_t w1 = (t >> 32);
	
	b >>= 32;
	t = (u1 * b) + k;
	k = (t >> 32);
	
	const uint64_t h = (a * b) + w1 + k;
	const uint64_t l = (t << 32) + w3;
	return std::make_pair(h, l);
}

std::pair<std::uint64_t, std::uint64_t> mulq(std::uint64_t a, std::uint64_t b)
{
	using std::uint64_t;
	#ifdef __LP64__
		// h 2⁶⁴ + l = a b
		uint64_t h, l;
		asm("mulq %3" : "=a"(l), "=d"(h) : "a"(a), "rm"(b));
		return std::make_pair(h, l);
	#else
		return mulq_emu(a, b);
	#endif
}

std::pair<std::uint64_t, std::uint64_t> add128(std::uint64_t h, std::uint64_t l, std::uint64_t n)
{
	l += n;
	h += (l < n) ? 1 : 0;
	return std::make_pair(h, l);
}

#include "Utilities.h"
#include <iomanip>
typedef std::uint64_t uint64;

uint64 count_leading_zeros(uint64 value)
{
	// Change the behaviour on all zeros
	if(value == 0) {
		return 64;
	}
	return __builtin_clzll(value);
}

std::pair<uint64, uint64> add128(uint64 h, uint64 l, uint64 n)
{
	l += n;
	h += (l < n) ? 1 : 0;
	return {h, l};
}

std::pair<uint64, uint64> mul128_emu(uint64 a, uint64 b);

std::pair<uint64, uint64> mul128(uint64 a, uint64 b)
{
	#ifdef __LP64__
		// h 2⁶⁴ + l = a b
		uint64 h, l;
		asm("mulq %3"
			: "=a"(l), "=d"(h)
			: "a"(a), "rm"(b));
		return {h, l};
	#else
		return mul128_emu(a, b);
	#endif
}

std::pair<uint64, uint64> div128_emu(uint64 h, uint64 l, uint64 d);

std::pair<uint64, uint64> div128(uint64 h, uint64 l, uint64 d)
{
	#ifdef __LP64__
		uint64 q, r;
		asm("divq %4"
			: "=a"(q), "=d"(r)
			: "d"(h), "a"(l), "rm"(d));
		return {q, r};
	#else
		return div128_emu(h, l, d);
	#endif
}

std::pair<uint64, uint64> mul128_emu(uint64 a, uint64 b)
{
	const uint64 u1 = (a & 0xffffffff);
	const uint64 v1 = (b & 0xffffffff);
	uint64 t = (u1 * v1);
	const uint64 w3 = (t & 0xffffffff);
	uint64 k = (t >> 32);
	
	a >>= 32;
	t = (a * v1) + k;
	k = (t & 0xffffffff);
	uint64 w1 = (t >> 32);
	
	b >>= 32;
	t = (u1 * b) + k;
	k = (t >> 32);
	
	const uint64 h = (a * b) + w1 + k;
	const uint64 l = (t << 32) + w3;
	return {h, l};
}

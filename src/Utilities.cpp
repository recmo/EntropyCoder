#include "Utilities.h"
#include <iomanip>
#include <stdexcept>
namespace EntropyCoder {
typedef std::uint64_t uint64;

uint64 count_leading_zeros(uint64 value)
{
	// Change the behaviour on all zeros
	if(value == 0) {
		return 64;
	}
	return __builtin_clzll(value);
	
	/// TODO count_leading_zeros_emu
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
	if(d == 0) {
		throw std::runtime_error("Divide by zero.");
	}
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

std::pair<uint64, uint64> div128_emu(uint64 h, uint64 l, uint64 d)
{
	// Use the algorithm from:
	// http://codereview.stackexchange.com/questions/67962/mostly-portable-128-by-64-bit-division
	// Translate the incoming variables
	uint64 a_hi = h;
	uint64 a_lo = l;
	uint64 b = d;
	uint64 r = 0;
	
	uint64 p_lo;
	uint64 p_hi;
	uint64 q = 0;
	
	auto r_hi = a_hi;
	auto r_lo = a_lo;
	
	int s = 0;
	if(0 == (b >> 63)){
		
		// Normalize so quotient estimates are
		// no more than 2 in error.
		
		// Note: If any bits get shifted out of
		// r_hi at this point, the result would
		// overflow.
		
		s = count_leading_zeros(b);
		const auto t = 64 - s;
		
		b <<= s;
		r_hi = (r_hi << s)|(r_lo >> t);
		r_lo <<= s;
	}
	
	const auto b_hi = b >> 32;
	
	/*
	The first full-by-half division places b
	across r_hi and r_lo, making the reduction
	step a little complicated.
	
	To make this easier, u_hi and u_lo will hold
	a shifted image of the remainder.
	
	[u_hi||    ][u_lo||    ]
			[r_hi||    ][r_lo||    ]
					[ b  ||    ]
	[p_hi||    ][p_lo||    ]
					|
					V
					[q_hi||    ]
	*/
	
	auto q_hat = r_hi / b_hi;
	
	std::tie(p_hi, p_lo) = mul128(b, q_hat);
	
	const auto u_hi = r_hi >> 32;
	const auto u_lo = (r_hi << 32)|(r_lo >> 32);
	
	// r -= b*q_hat
	//
	// At most 2 iterations of this...
	while(
		(p_hi > u_hi) ||
		((p_hi == u_hi) && (p_lo > u_lo))
		)
	{
		if(p_lo < b){
			--p_hi;
		}
		p_lo -= b;
		--q_hat;
	}
	
	auto w_lo = (p_lo << 32);
	auto w_hi = (p_hi << 32)|(p_lo >> 32);
	
	if(w_lo > r_lo){
		++w_hi;
	}
	
	r_lo -= w_lo;
	r_hi -= w_hi;
	
	q = q_hat << 32;
	
	/*
	The lower half of the quotient is easier,
	as b is now aligned with r_lo.
	
			|r_hi][r_lo||    ]
					[ b  ||    ]
	[p_hi||    ][p_lo||    ]
							|
							V
					[q_hi||q_lo]
	*/
	
	q_hat = ((r_hi << 32)|(r_lo >> 32)) / b_hi;
	
	std::tie(p_hi, p_lo) = mul128(b, q_hat);
	
	// r -= b*q_hat
	//
	// ...and at most 2 iterations of this.
	while(
		(p_hi > r_hi) ||
		((p_hi == r_hi) && (p_lo > r_lo))
		)
	{
		if(p_lo < b){
			--p_hi;
		}
		p_lo -= b;
		--q_hat;
	}
	
	r_lo -= p_lo;
	
	q |= q_hat;
	
	r = r_lo >> s;

	return {q, r};
}

} // namespace EntropyCoder

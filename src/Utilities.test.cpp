#include <iostream>
#include "Utilities.h"
#include <vector>
#include <tuple>

// This needs to go before including the macro that calls it.
std::ostream& operator<<(std::ostream& out, const std::pair<std::uint64_t, std::uint64_t>& pair)
{
	out << pair.first << ", " << pair.second;
	return out;
}

#include <UnitTest++.h>

std::pair<std::uint64_t, std::uint64_t> mul128_emu(std::uint64_t a, std::uint64_t b);


SUITE(Utilities) {

// Nine special values
const std::uint64_t c0 = 0x0000000000000000UL;
const std::uint64_t c1 = 0x0000000000000001UL;
const std::uint64_t c5 = 0x5555555555555555UL;
const std::uint64_t c7 = 0x7FFFFFFFFFFFFFFFUL;
const std::uint64_t c8 = 0x8000000000000000UL;
const std::uint64_t c9 = 0x8000000000000001UL;
const std::uint64_t cA = 0xAAAAAAAAAAAAAAAAUL;
const std::uint64_t cE = 0xFFFFFFFFFFFFFFFEUL;
const std::uint64_t cF = 0xFFFFFFFFFFFFFFFFUL;
const std::vector<std::uint64_t> all_values{
	c0, c1, c5, c7, c8, c9, cA, cE, cF
};

TEST(CountLeadingZeros)
{
	CHECK_EQUAL(0ULL, count_leading_zeros(1ULL << 63));
	CHECK_EQUAL(1ULL, count_leading_zeros(1ULL << 62));
	CHECK_EQUAL(2ULL, count_leading_zeros(1ULL << 61));
	CHECK_EQUAL(61ULL, count_leading_zeros(1ULL << 2));
	CHECK_EQUAL(62ULL, count_leading_zeros(1ULL << 1));
	CHECK_EQUAL(63ULL, count_leading_zeros(1ULL << 0));
	CHECK_EQUAL(64ULL, count_leading_zeros(0ULL));
	CHECK_EQUAL(0ULL, count_leading_zeros((1ULL << 63) | 0xF0F0F0ULL));
	CHECK_EQUAL(32ULL, count_leading_zeros(0xF0F0F0F0ULL));
}

TEST(MultiplyDivideAdd)
{
	for(std::uint64_t a: all_values) {
		for(std::uint64_t b: all_values) {
			
			// Multipy
			const auto result = mul128(a, b);
			const std::uint64_t h = result.first;
			const std::uint64_t l = result.second;
			CHECK_EQUAL(a * b, l);
			if(a == 0UL || b == 0UL) {
				CHECK_EQUAL(0UL, h);
				CHECK_EQUAL(0UL, l);
			} else if (a == 1UL) {
				CHECK_EQUAL(0UL, h);
				CHECK_EQUAL(b, l);
			} else if (b == 1UL) {
				CHECK_EQUAL(0UL, h);
				CHECK_EQUAL(a, l);
			}
			CHECK_EQUAL(result, mul128(b, a));
			CHECK_EQUAL(result, mul128_emu(a, b));
			
			// Divide
			// TODO add _emu
			CHECK_THROW(div128(h, l, 0), std::runtime_error);
			if(a != 0) {
				CHECK_EQUAL(std::make_pair(b, 0UL), div128(h, l, a));
			}
			if(b != 0) {
				CHECK_EQUAL(std::make_pair(a, 0UL), div128(h, l, b));
			}
			
			for(const std::uint64_t c: all_values) {
				
				// Add
				const auto result2 = add128(h, l, c);
				const std::uint64_t h2 = result2.first;
				const std::uint64_t l2 = result2.second;
				
				// Divide
				// TODO add _emu
				CHECK_THROW(div128(h2, l2, 0), std::runtime_error);
				if(a != 0) {
					if(b + c / a < b) {
						// TODO CHECK_THROW
					} else {
						CHECK_EQUAL(std::make_pair(b + c / a, c % a), div128(h2, l2, a));
					}
				}
				if(b != 0) {
					if(a + c / b < a) {
						// TODO CHECK_THROW
					} else {
						CHECK_EQUAL(std::make_pair(a + c / b, c % b), div128(h2, l2, b));
					}
				}
			}
		}
	}
}

}

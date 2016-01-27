#include <iostream>
#include "Utilities.h"
#include <UnitTest++.h>

std::pair<std::uint64_t, std::uint64_t> mulq_emu(std::uint64_t a, std::uint64_t b);

std::ostream& operator<<(std::ostream& out, const std::pair<std::uint64_t, std::uint64_t>& pair)
{
	out << pair.first << ", " << pair.second;
	return out;
}

SUITE(Utilities) {

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
/*
TEST(Multiply64)
{
	using std::make_pair;
	CHECK_EQUAL(make_pair(0UL,0UL), mulq(0xFFFFFFFFULL, 0xFFFFFFFFULL));
	CHECK_EQUAL(make_pair(1UL,0UL), mulq(0x100000000ULL, 0x100000000ULL));
	CHECK_EQUAL(make_pair(3UL,0UL), mulq(0x1FFFFFFFFULL, 0x1FFFFFFFFULL));
	CHECK_EQUAL(make_pair(0xFFFFFFFFFFFFFFFEUL,0UL), mulq(0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL));
	CHECK_EQUAL(make_pair(1336UL,0UL), mulq(1337ULL, 0xFFFFFFFFFFFFFFFFULL));
	CHECK_EQUAL(make_pair(1336UL,0UL), mulq(0xFFFFFFFFFFFFFFFFULL, 1337ULL));
}

TEST(Multiply64Emulated)
{
	using std::make_pair;
	CHECK_EQUAL(make_pair(0UL,0UL), mulq_emu(0xFFFFFFFFULL, 0xFFFFFFFFULL));
	CHECK_EQUAL(make_pair(1UL,0UL), mulq_emu(0x100000000ULL, 0x100000000ULL));
	CHECK_EQUAL(make_pair(3UL,0UL), mulq_emu(0x1FFFFFFFFULL, 0x1FFFFFFFFULL));
	CHECK_EQUAL(make_pair(0xFFFFFFFFFFFFFFFEUL,0UL), mulq_emu(0xFFFFFFFFFFFFFFFFULL, 0xFFFFFFFFFFFFFFFFULL));
	CHECK_EQUAL(make_pair(1336UL,0UL), mulq_emu(1337ULL, 0xFFFFFFFFFFFFFFFFULL));
	CHECK_EQUAL(make_pair(1336UL,0UL), mulq_emu(0xFFFFFFFFFFFFFFFFULL, 1337ULL));
}
*/
}

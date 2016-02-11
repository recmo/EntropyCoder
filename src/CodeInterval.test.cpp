#include "CodeInterval.h"
#include <UnitTest++.h>
#include <iostream>
#include <cmath>
#include <limits>

SUITE(CodeInterval) {

CodeInterval make(Interval::uint64 base, Interval::uint64 range)
{
	CodeInterval i;
	i.base = base;
	i.range = range;
	return i;
}

const CodeInterval smallest = make(0x0000000000000000UL, 0x0000000000000001UL);
const CodeInterval largest = make(0x0000000000000000UL, 0xFFFFFFFFFFFFFFFFUL);
const CodeInterval half1 = make(0x0000000000000000UL, 0x7FFFFFFFFFFFFFFFUL);
const CodeInterval half2 = make(0x8000000000000000UL, 0x7FFFFFFFFFFFFFFFUL);

const CodeInterval min_norm1 = make(0x0000000000000000UL, 0x8000000000000000UL);
const CodeInterval min_norm2 = make(0x7FFFFFFFFFFFFFFFUL, 0x8000000000000000UL);
const CodeInterval min_wrap = make(0x8000000000000000UL, 0x8000000000000000UL);
const CodeInterval max_wrap = make(0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL);

TEST(IsNormalized)
{
	CHECK_EQUAL(true, largest.is_normalized());
	CHECK_EQUAL(true, min_norm1.is_normalized());
	CHECK_EQUAL(true, min_norm2.is_normalized());
	CHECK_EQUAL(true, min_wrap.is_normalized());
	CHECK_EQUAL(true, max_wrap.is_normalized());
	CHECK_EQUAL(false, smallest.is_normalized());
	CHECK_EQUAL(false, half1.is_normalized());
	CHECK_EQUAL(false, half2.is_normalized());
}

TEST(Wraps)
{
	CHECK_EQUAL(false, largest.wraps());
	CHECK_EQUAL(false, min_norm1.wraps());
	CHECK_EQUAL(false, min_norm2.wraps());
	CHECK_EQUAL(true, min_wrap.wraps());
	CHECK_EQUAL(true, max_wrap.wraps());
}

TEST(UpdateLargest)
{
	const std::vector<CodeInterval> symbols{
		largest, smallest, half1, half2, min_norm1, min_norm2
	};
	for(const CodeInterval& symbol: symbols) {
		CodeInterval updated = largest;
		const bool carry = updated.update(symbol);
		CHECK(!carry);
		CHECK_EQUAL(symbol, updated);
	}
}

TEST(Update)
{
	const std::vector<CodeInterval> intervals {
		largest, min_norm1, min_norm2, min_wrap, max_wrap
	};
	const std::vector<CodeInterval> symbols {
		largest, smallest, half1, half2, min_norm1, min_norm2
	};
	for(const CodeInterval& interval: intervals) {
		for(const CodeInterval& symbol: symbols) {
			CodeInterval updated = interval;
			const bool carry = updated.update(symbol);
			if(!interval.wraps()) {
				CHECK(!carry);
				CHECK(!updated.wraps());
				CHECK(interval.includes(updated));
			}
		}
	}
}

TEST(UpdateCarry)
{
	const std::vector<CodeInterval> symbols {
		half2, min_norm2
	};
	for(const CodeInterval& symbol: symbols) {
		CodeInterval updated = max_wrap;
		const bool carry = updated.update(symbol);
		CHECK_EQUAL(true, carry);
		CHECK_EQUAL(false, updated.wraps());
	}
}

}

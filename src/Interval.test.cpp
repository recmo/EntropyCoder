#include "Interval.h"
#include <UnitTest++.h>
#include <iostream>
#include <cmath>
#include <limits>

SUITE(Interval) {

Interval make_goofy(Interval::uint64 base, Interval::uint64 range)
{
	Interval i;
	i.base = base;
	i.range = range;
	return i;
}

const Interval smallest{0x0000000000000000UL, 0x0000000000000001UL};
const Interval largest{0x0000000000000000UL, 0xFFFFFFFFFFFFFFFFUL};
const Interval half1{0x0000000000000000UL, 0x7FFFFFFFFFFFFFFFUL};
const Interval half2{0x8000000000000000UL, 0x7FFFFFFFFFFFFFFFUL};

const Interval min_norm1{0x0000000000000000UL, 0x8000000000000000UL};
const Interval min_norm2{0x7FFFFFFFFFFFFFFFUL, 0x8000000000000000UL};
const Interval min_goofy = make_goofy(0x8000000000000000UL, 0x8000000000000000UL);
const Interval max_goofy = make_goofy(0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL);

TEST(LiteralRanges)
{
	CHECK_EQUAL(0x0000000000000000UL, smallest.base);
	CHECK_EQUAL(0x0000000000000001UL, smallest.range);
	CHECK_EQUAL(0x0000000000000000UL, largest.base);
	CHECK_EQUAL(0xFFFFFFFFFFFFFFFFUL, largest.range);
	CHECK_EQUAL(0x0000000000000000UL, half1.base);
	CHECK_EQUAL(0x7FFFFFFFFFFFFFFFUL, half1.range);
	CHECK_EQUAL(0x8000000000000000UL, half2.base);
	CHECK_EQUAL(0x7FFFFFFFFFFFFFFFUL, half2.range);
}

TEST(Equality)
{
	const std::vector<Interval> intervals{smallest, largest, half1, half2};
	for(uint i = 0; i < intervals.size(); ++i) {
		for(uint j = 0; j < intervals.size(); ++j) {
			CHECK_EQUAL(i == j , intervals[i] == intervals[j]);
			CHECK_EQUAL(i != j , intervals[i] != intervals[j]);
		}
	}
}

TEST(IsNormalized)
{
	CHECK_EQUAL(true, largest.is_normalized());
	CHECK_EQUAL(true, min_norm1.is_normalized());
	CHECK_EQUAL(true, min_norm2.is_normalized());
	CHECK_EQUAL(true, min_goofy.is_normalized());
	CHECK_EQUAL(true, max_goofy.is_normalized());
	CHECK_EQUAL(false, smallest.is_normalized());
	CHECK_EQUAL(false, half1.is_normalized());
	CHECK_EQUAL(false, half2.is_normalized());
}

TEST(IsGoofy)
{
	CHECK_EQUAL(false, largest.is_goofy());
	CHECK_EQUAL(false, min_norm1.is_goofy());
	CHECK_EQUAL(false, min_norm2.is_goofy());
	CHECK_EQUAL(true, min_goofy.is_goofy());
	CHECK_EQUAL(true, max_goofy.is_goofy());
}

TEST(Inclusion)
{
	CHECK_EQUAL(true, largest.includes(largest));
	CHECK_EQUAL(true, largest.includes(smallest));
	CHECK_EQUAL(true, largest.includes(half2));
	CHECK_EQUAL(true, smallest.includes(smallest));
	CHECK_EQUAL(false, smallest.includes(largest));
	CHECK_EQUAL(false, smallest.includes(half1));
	CHECK_EQUAL(false, smallest.includes(half2));
	CHECK_EQUAL(false, half1.includes(half2));
	CHECK_EQUAL(false, half2.includes(half1));
	CHECK_EQUAL(false, half2.includes(half1));
	CHECK_EQUAL(true, min_norm1.includes(half1));
	CHECK_EQUAL(false, min_norm1.includes(half2));
	CHECK_EQUAL(false, min_norm2.includes(half1));
	CHECK_EQUAL(true, min_norm2.includes(half2));
}

TEST(ExactProbabilitiesEights)
{
	// n/8
	CHECK_EQUAL(smallest, Interval{0.0});
	CHECK_EQUAL((Interval{0, 0x1FFFFFFFFFFFFFFFUL}), Interval{0.125});
	CHECK_EQUAL((Interval{0, 0x3FFFFFFFFFFFFFFFUL}), Interval{0.25});
	CHECK_EQUAL((Interval{0, 0x5FFFFFFFFFFFFFFFUL}), Interval{0.375});
	CHECK_EQUAL((Interval{0, 0x7FFFFFFFFFFFFFFFUL}), Interval{0.5});
	CHECK_EQUAL((Interval{0, 0x9FFFFFFFFFFFFFFFUL}), Interval{0.625});
	CHECK_EQUAL((Interval{0, 0xbFFFFFFFFFFFFFFFUL}), Interval{0.75});
	CHECK_EQUAL((Interval{0, 0xdFFFFFFFFFFFFFFFUL}), Interval{0.875});
	CHECK_EQUAL(largest, Interval{1.0});
}

TEST(ExactProbabilitiesSmall)
{
	// 2⁻ⁿ
	CHECK_EQUAL((Interval{0, 0x0FFFFFFFFFFFFFFFUL}), Interval{std::exp2(-4)});
	CHECK_EQUAL((Interval{0, 0x00FFFFFFFFFFFFFFUL}), Interval{std::exp2(-8)});
	CHECK_EQUAL((Interval{0, 0x0000FFFFFFFFFFFFUL}), Interval{std::exp2(-16)});
	CHECK_EQUAL((Interval{0, 0x000000FFFFFFFFFFUL}), Interval{std::exp2(-24)});
	CHECK_EQUAL((Interval{0, 0x00000000FFFFFFFFUL}), Interval{std::exp2(-32)});
	CHECK_EQUAL((Interval{0, 0x0000000000FFFFFFUL}), Interval{std::exp2(-40)});
	CHECK_EQUAL((Interval{0, 0x000000000000FFFFUL}), Interval{std::exp2(-48)});
	CHECK_EQUAL((Interval{0, 0x00000000000000FFUL}), Interval{std::exp2(-56)});
	CHECK_EQUAL((Interval{0, 0x000000000000000FUL}), Interval{std::exp2(-60)});
	CHECK_EQUAL((Interval{0, 0x0000000000000001UL}), Interval{std::exp2(-63)});
}

TEST(ProbabilityUnderflow)
{
	CHECK_EQUAL(smallest, Interval{std::exp2(-64)});
	CHECK_EQUAL(smallest, Interval{std::exp2(-65)});
	CHECK_EQUAL(smallest, Interval{std::exp2(-128)});
	CHECK_EQUAL(smallest, Interval{1e-20L});
	CHECK_EQUAL(smallest, Interval{std::numeric_limits<double>().min()});
	CHECK_EQUAL(smallest, Interval{std::numeric_limits<double>().denorm_min()});
}

TEST(IllegalProbability)
{
	CHECK_THROW(Interval{-0.1L}, std::range_error);
	CHECK_THROW(Interval{1.1L}, std::range_error);
	CHECK_THROW(Interval{std::nexttoward(1.0, 2.0)}, std::range_error);
	CHECK_THROW(Interval{std::nexttoward(0.0, -1.0)}, std::range_error);
	CHECK_THROW(Interval{std::numeric_limits<double>().lowest()}, std::range_error);
	CHECK_THROW(Interval{std::numeric_limits<double>().max()}, std::range_error);
	CHECK_THROW(Interval{std::numeric_limits<double>().infinity()}, std::range_error);
	CHECK_THROW(Interval{-std::numeric_limits<double>().infinity()}, std::range_error);
	CHECK_THROW(Interval{std::numeric_limits<double>().quiet_NaN()}, std::range_error);
	CHECK_THROW(Interval{std::numeric_limits<double>().signaling_NaN()}, std::range_error);
}

TEST(UpdateLargest)
{
	const std::vector<Interval> symbols{
		largest, smallest, half1, half2, min_norm1, min_norm2
	};
	for(const Interval& symbol: symbols) {
		Interval updated = largest;
		bool carry;
		updated.update(symbol, &carry);
		CHECK(!carry);
		CHECK_EQUAL(symbol, updated);
	}
}

TEST(Update)
{
	const std::vector<Interval> intervals {
		largest, min_norm1, min_norm2, min_goofy, max_goofy
	};
	const std::vector<Interval> symbols {
		largest, smallest, half1, half2, min_norm1, min_norm2
	};
	for(const Interval& interval: intervals) {
		for(const Interval& symbol: symbols) {
			Interval updated = interval;
			bool carry;
			updated.update(symbol, &carry);
			if(!interval.is_goofy()) {
				CHECK(!carry);
				CHECK(!updated.is_goofy());
				CHECK(interval.includes(updated));
			}
		}
	}
}

TEST(UpdateCarry)
{
	const std::vector<Interval> symbols {
		half2, min_norm2
	};
	for(const Interval& symbol: symbols) {
		Interval updated = max_goofy;
		bool carry;
		updated.update(symbol, &carry);
		CHECK_EQUAL(true, carry);
		CHECK_EQUAL(false, updated.is_goofy());
	}
}

}
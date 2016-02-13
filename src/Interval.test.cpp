#include "Interval.h"
#include <UnitTest++.h>
#include <iostream>
#include <cmath>
#include <limits>

SUITE(Interval) {
using EntropyCoder::Interval;

std::string to_string(const Interval& end)
{
	std::ostringstream out;
	out << end;
	return out.str();
}

const Interval smallest{0x0000000000000000UL, 0x0000000000000003UL};
const Interval largest{0x0000000000000000UL, 0xFFFFFFFFFFFFFFFFUL};
const Interval half1{0x0000000000000000UL, 0x7FFFFFFFFFFFFFFFUL};
const Interval half2{0x8000000000000000UL, 0x7FFFFFFFFFFFFFFFUL};

const Interval min_norm1{0x0000000000000000UL, 0x8000000000000000UL};
const Interval min_norm2{0x7FFFFFFFFFFFFFFFUL, 0x8000000000000000UL};

TEST(ToString)
{
	CHECK_EQUAL("[0.0000000000000000, 0.0000000000000004)", to_string(smallest));
	CHECK_EQUAL("[0.0000000000000000, 1.0000000000000000)", to_string(largest));
	CHECK_EQUAL("[0.0000000000000000, 0.8000000000000000)", to_string(half1));
	CHECK_EQUAL("[0.8000000000000000, 1.0000000000000000)", to_string(half2));
	CHECK_EQUAL("[0.0000000000000000, 0.8000000000000001)", to_string(min_norm1));
	CHECK_EQUAL("[0.7fffffffffffffff, 1.0000000000000000)", to_string(min_norm2));
}

TEST(LiteralRanges)
{
	CHECK_EQUAL(0x0000000000000000UL, smallest.base);
	CHECK_EQUAL(0x0000000000000003UL, smallest.range);
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
	CHECK_EQUAL((Interval{0, 0x0000000000000003UL}), Interval{std::exp2(-62)});
}

TEST(ProbabilityUnderflow)
{
	CHECK_EQUAL(smallest, Interval{std::exp2(-63)});
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

TEST(GetProbability)
{
	CHECK_EQUAL(0.125, Interval{0.125}.probability());
	CHECK_EQUAL(0.25 , Interval{0.25 }.probability());
	CHECK_EQUAL(0.375, Interval{0.375}.probability());
	CHECK_EQUAL(0.5  , Interval{0.5  }.probability());
	CHECK_EQUAL(0.625, Interval{0.625}.probability());
	CHECK_EQUAL(0.75 , Interval{0.75 }.probability());
	CHECK_EQUAL(0.875, Interval{0.875}.probability());
	CHECK_EQUAL(1.0  , Interval{1.0  }.probability());
	CHECK_EQUAL(std::exp2(- 4), Interval{std::exp2(- 4)}.probability());
	CHECK_EQUAL(std::exp2(- 8), Interval{std::exp2(- 8)}.probability());
	CHECK_EQUAL(std::exp2(-16), Interval{std::exp2(-16)}.probability());
	CHECK_EQUAL(std::exp2(-24), Interval{std::exp2(-24)}.probability());
	CHECK_EQUAL(std::exp2(-32), Interval{std::exp2(-32)}.probability());
	CHECK_EQUAL(std::exp2(-40), Interval{std::exp2(-40)}.probability());
	CHECK_EQUAL(std::exp2(-48), Interval{std::exp2(-48)}.probability());
	CHECK_EQUAL(std::exp2(-56), Interval{std::exp2(-56)}.probability());
	CHECK_EQUAL(std::exp2(-60), Interval{std::exp2(-60)}.probability());
	CHECK_EQUAL(std::exp2(-62), Interval{std::exp2(-62)}.probability());
}

TEST(GetEntropy)
{
	const double tolerance = std::numeric_limits<double>().epsilon();
	
	CHECK_EQUAL(3.0  , Interval{0.125}.entropy());
	CHECK_EQUAL(2.0  , Interval{0.25 }.entropy());
	CHECK_EQUAL(1.0  , Interval{0.5  }.entropy());
	CHECK_EQUAL(0.0  , Interval{1.0  }.entropy());
	CHECK_CLOSE(1.415037499278844 , Interval{0.375}.entropy(), tolerance);
	CHECK_CLOSE(0.6780719051126377, Interval{0.625}.entropy(), tolerance);
	CHECK_CLOSE(0.4150374992788438, Interval{0.75 }.entropy(), tolerance);
	CHECK_CLOSE(0.1926450779423959, Interval{0.875}.entropy(), tolerance);
}

TEST(IllegalInterval)
{
	CHECK_THROW((Interval{0x0000000000000000UL, 0x0000000000000000UL}), std::range_error);
	CHECK_THROW((Interval{0x8000000000000000UL, 0x0000000000000000UL}), std::range_error);
	CHECK_THROW((Interval{0xFFFFFFFFFFFFFFFFUL, 0x0000000000000000UL}), std::range_error);
	CHECK_THROW((Interval{0x0000000000000001UL, 0xFFFFFFFFFFFFFFFFUL}), std::range_error);
	CHECK_THROW((Interval{0x8000000000000000UL, 0x8000000000000000UL}), std::range_error);
	CHECK_THROW((Interval{0xFFFFFFFFFFFFFFFEUL, 0x0000000000000002UL}), std::range_error);
}

TEST(Overlaps)
{
	// TODO CHECK_EQUAL(true , largest.overlaps(smallest));
	// TODO CHECK_EQUAL(true , largest.overlaps(half1));
	// TODO CHECK_EQUAL(true , largest.overlaps(half2));
	CHECK_EQUAL(true , smallest.overlaps(largest));
	CHECK_EQUAL(true , smallest.overlaps(half1));
	CHECK_EQUAL(false, smallest.overlaps(half2));
	CHECK_EQUAL(true , half1.overlaps(largest));
	// TODO CHECK_EQUAL(true , half1.overlaps(smallest));
	CHECK_EQUAL(false, half1.overlaps(half2));
	// TODO CHECK_EQUAL(true , half2.overlaps(largest));
	CHECK_EQUAL(false, half2.overlaps(smallest));
	CHECK_EQUAL(false, half2.overlaps(half1));
}

}

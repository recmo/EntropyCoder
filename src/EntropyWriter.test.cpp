#include "EntropyWriter.h"
#include "Interval.h"
#include <UnitTest++.h>
#include <sstream>
#include <cassert>
#include <vector>

SUITE(EntropyWriter) {
using EntropyCoder::Interval;
using EntropyCoder::EntropyWriter;

// Subclass of Interval that allows unsafe initialization
class UnsafeInterval: public Interval {
public:
	UnsafeInterval(uint64 my_base, uint64 my_range): Interval{} {
		base = my_base;
		range = my_range;
	}
};

// Eleven special values
const std::uint64_t c0 = 0x0000000000000000UL;
const std::uint64_t c1 = 0x0000000000000001UL;
const std::uint64_t c2 = 0x0000000000000002UL;
const std::uint64_t c3 = 0x0000000000000003UL;
const std::uint64_t c5 = 0x5555555555555555UL;
const std::uint64_t c7 = 0x7FFFFFFFFFFFFFFFUL;
const std::uint64_t c8 = 0x8000000000000000UL;
const std::uint64_t c9 = 0x8000000000000001UL;
const std::uint64_t cA = 0xAAAAAAAAAAAAAAAAUL;
const std::uint64_t cE = 0xFFFFFFFFFFFFFFFEUL;
const std::uint64_t cF = 0xFFFFFFFFFFFFFFFFUL;
const std::vector<std::uint64_t> all_values{
	c0, c1, c2, c3, c5, c7, c8, c9, cA, cE, cF
};

// Create 11 × 11 = 121 special intervals
#define I(a,b) const Interval i##a##b = UnsafeInterval{c##a,c##b};
#define R(a) I(a,0) I(a,1) I(a,2) I(a,3) I(a,5) I(a,7) I(a,8) I(a,9) I(a,A) I(a,E) I(a,F)
R(0) R(1) R(2) R(3) R(5) R(7) R(8) R(9) R(A) R(E) R(F)

const std::vector<Interval> all{
	i00, i01, i02, i03, i05, i07, i08, i09, i0A, i0E, i0F,
	i10, i11, i12, i13, i15, i17, i18, i19, i1A, i1E, i1F,
	i20, i21, i22, i23, i25, i27, i28, i29, i2A, i2E, i2F,
	i30, i31, i32, i33, i35, i37, i38, i39, i3A, i3E, i3F,
	i50, i51, i52, i53, i55, i57, i58, i59, i5A, i5E, i5F,
	i70, i71, i72, i73, i75, i77, i78, i79, i7A, i7E, i7F,
	i80, i81, i82, i83, i85, i87, i88, i89, i8A, i8E, i8F,
	i90, i91, i92, i93, i95, i97, i98, i99, i9A, i9E, i9F,
	iA0, iA1, iA2, iA3, iA5, iA7, iA8, iA9, iAA, iAE, iAF,
	iE0, iE1, iE2, iE3, iE5, iE7, iE8, iE9, iEA, iEE, iEF,
	iF0, iF1, iF2, iF3, iF5, iF7, iF8, iF9, iFA, iFE, iFF,
};

template<class T>
std::vector<T> invert(const std::vector<T>& vec) {
	std::vector<T> result;
	result.reserve(all.size() - vec.size());
	for(const Interval& i: all) {
		T it{i};
		if(std::find(vec.begin(), vec.end(), it) == vec.end()) {
			result.push_back(it);
		}
	}
	assert(vec.size() + result.size() == all.size());
	return result;
}

const std::vector<Interval> valid_symbols{
	               i03, i05, i07, i08, i09, i0A, i0E, i0F,
	               i13, i15, i17, i18, i19, i1A, i1E,
	               i23, i25, i27, i28, i29, i2A,
	               i33, i35, i37, i38, i39, i3A,
	               i53, i55, i57, i58, i59, i5A,
	               i73, i75, i77, i78,
	               i83, i85, i87,
	               i93, i95,
	               iA3, iA5,
};
const std::vector<Interval> invalid_symbols = invert(valid_symbols);

TEST(Empty)
{
	std::ostringstream out;
	CHECK(out.good());
	{
		EntropyWriter bw(out);
	}
	out.flush();
	CHECK(out.good());
	CHECK_EQUAL("", out.str());
}

TEST(OneSymbol)
{
	for(const Interval& a: valid_symbols) {
		std::ostringstream out;
		CHECK(out.good());
		{
			EntropyWriter bw(out);
			bw.write(a.base, a.base + a.range);
		}
		out.flush();
		CHECK(out.good());
		// CHECK_EQUAL("", out.str());
	}
}

TEST(TwoSymbols)
{
	for(const Interval& a: valid_symbols) {
		for(const Interval& b: valid_symbols) {
			std::ostringstream out;
			CHECK(out.good());
			{
				EntropyWriter bw(out);
				bw.write(a.base, a.base + a.range);
				bw.write(b.base, b.base + b.range);
			}
			out.flush();
			CHECK(out.good());
			// CHECK_EQUAL("", out.str());
		}
	}
}

TEST(ThousandSymbols)
{
	for(const Interval& a: valid_symbols) {
		std::ostringstream out;
		CHECK(out.good());
		{
			EntropyWriter bw(out);
			for(uint i = 0; i < 1000; ++i) {
				bw.write(a.base, a.base + a.range);
			}
		}
		out.flush();
		CHECK(out.good());
	}
}

TEST(TwoThousandSymbols)
{
	for(const Interval& a: valid_symbols) {
		for(const Interval& b: valid_symbols) {
			std::ostringstream out;
			CHECK(out.good());
			{
				EntropyWriter bw(out);
				for(uint i = 0; i < 1000; ++i) {
					bw.write(a.base, a.base + a.range);
				}
				for(uint i = 0; i < 1000; ++i) {
					bw.write(b.base, b.base + b.range);
				}
			}
			out.flush();
			CHECK(out.good());
		}
	}
}

TEST(TwoThousandAlternatingSymbols)
{
	for(const Interval& a: valid_symbols) {
		for(const Interval& b: valid_symbols) {
			std::ostringstream out;
			CHECK(out.good());
			{
				EntropyWriter bw(out);
				for(uint i = 0; i < 1000; ++i) {
					bw.write(a.base, a.base + a.range);
					bw.write(b.base, b.base + b.range);
				}
			}
			out.flush();
			CHECK(out.good());
		}
	}
}

}

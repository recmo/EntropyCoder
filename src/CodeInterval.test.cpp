#include "CodeInterval.h"
#include <UnitTest++.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <limits>

SUITE(CodeInterval) {
using EntropyCoder::Interval;
using EntropyCoder::CodeInterval;

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

const std::vector<CodeInterval> normalized{
	                              i08, i09, i0A, i0E, i0F,
	                              i18, i19, i1A, i1E, i1F,
	                              i28, i29, i2A, i2E, i2F,
	                              i38, i39, i3A, i3E, i3F,
	                              i58, i59, i5A, i5E, i5F,
	                              i78, i79, i7A, i7E, i7F,
	                              i88, i89, i8A, i8E, i8F,
	                              i98, i99, i9A, i9E, i9F,
	                              iA8, iA9, iAA, iAE, iAF,
	                              iE8, iE9, iEA, iEE, iEF,
	                              iF8, iF9, iFA, iFE, iFF
};
const std::vector<CodeInterval> non_normalized = invert(normalized);

const std::vector<CodeInterval> wraps{
	
	                                                  i1F,
	                                             i2E, i2F,
	                                             i3E, i3F,
	                                             i5E, i5F,
	                                   i79, i7A, i7E, i7F,
	                              i88, i89, i8A, i8E, i8F,
	                         i97, i98, i99, i9A, i9E, i9F,
	                         iA7, iA8, iA9, iAA, iAE, iAF,
	          iE2, iE3, iE5, iE7, iE8, iE9, iEA, iEE, iEF,
		  iF1, iF2, iF3, iF5, iF7, iF8, iF9, iFA, iFE, iFF,
};
const std::vector<CodeInterval> non_wraps = invert(wraps);

TEST(IsNormalized)
{
	for(const CodeInterval& code: normalized) {
		CHECK_EQUAL(true, code.is_normalized());
	}
	for(const CodeInterval& code: non_normalized) {
		CHECK_EQUAL(false, code.is_normalized());
	}
}

TEST(Wraps)
{
	for(const CodeInterval& code: wraps) {
		if(!code.wraps())
			std::cerr << code << std::endl;
		CHECK_EQUAL(true, code.wraps());
	}
	for(const CodeInterval& code: non_wraps) {
		CHECK_EQUAL(false, code.wraps());
	}
}

struct GeneratedTest {
	std::uint64_t base;
	std::uint64_t range;
	std::uint8_t normalized_index;
	std::uint8_t symbols_index;
	bool carry;
};
GeneratedTest tests[] = {
	#include "CodeInterval.test.py.out"
};
const uint num_tests = sizeof(tests) / sizeof(GeneratedTest);

TEST(UpdateGeneratedTests)
{
	for(uint test_index = 0; test_index < num_tests; ++test_index) {
		const GeneratedTest& test = tests[test_index];
		CodeInterval code = normalized.at(test.normalized_index);
		const Interval& symbol = valid_symbols.at(test.symbols_index);
		const bool carry = code.update(symbol);
		CHECK_EQUAL(test.base, code.base);
		CHECK_EQUAL(test.range, code.range);
		CHECK_EQUAL(test.carry, carry);
	}
}

TEST(Update)
{
	/*
	for(const CodeInterval& interval: normalized) {
		for(const Interval& symbol: valid_symbols) {
			CodeInterval updated = interval;
			const bool carry = updated.update(symbol);
			//CHECK(interval.includes(updated));
			if(!interval.wraps()) {
				CHECK(!carry);
				CHECK(!updated.wraps());
			} else {
				// TODO
			}
			
		}
		// TODO Check non-overlapping sub-intervals
		
		// Invalid symbols
		for(const Interval& symbol: invalid_symbols) {
			CodeInterval updated = interval;
			CHECK_THROW(updated.update(symbol), std::range_error);
		}
	}
	
	// Not normalized
	for(const CodeInterval& interval: non_normalized) {
		for(const Interval& symbol: all) {
			CodeInterval updated = interval;
			CHECK_THROW(updated.update(symbol), std::range_error);
		}
	}
	*/
}

TEST(UpdateLargestIdentity)
{
	const CodeInterval largest = i0F;
	for(const CodeInterval& symbol: valid_symbols) {
		CodeInterval updated = largest;
		const bool carry = updated.update(symbol);
		CHECK(!carry);
		CHECK_EQUAL(symbol, updated);
	}
}

TEST(Descale)
{
	for(const CodeInterval& interval: normalized) {
		for(const Interval::uint64 value: all_values) {
			if(interval.includes(value)) {
				interval.descale(value);
				
				// TODO Scale and descale roundtrip.
				
			} else {
				CHECK_THROW(interval.descale(value), std::range_error);
			}
		}
	}
	for(const CodeInterval& interval: non_normalized) {
		for(const Interval::uint64 value: all_values) {
			CHECK_THROW(interval.descale(value), std::range_error);
		}
	}
}

TEST(DescaleLargestIdentity)
{
	const CodeInterval largest = i0F;
	for(const Interval::uint64 value: all_values) {
		CHECK_EQUAL(value, largest.descale(value));
	}
}

TEST(DescaleSmallest)
{
	const CodeInterval smallest = i08;
	CHECK_EQUAL(0x0000000000000000UL, smallest.descale(0x0000000000000000UL));
	CHECK_EQUAL(0xAAAAAAAAAAAAAAA8UL, smallest.descale(0x5555555555555555UL));
	CHECK_EQUAL(0xFFFFFFFFFFFFFFFEUL, smallest.descale(0x8000000000000000UL));
	CHECK_THROW(smallest.descale(0xAAAAAAAAAAAAAAAAUL), std::range_error);
	CHECK_THROW(smallest.descale(0xFFFFFFFFFFFFFFFFUL), std::range_error);
}

TEST(normalize)
{
	for(CodeInterval interval: normalized) {
		auto result = interval.normalize();
		CHECK_EQUAL(true, interval.is_normalized());
		CHECK_EQUAL(0, result.second);
	}
	for(CodeInterval interval: non_normalized) {
		auto result = interval.normalize();
		CHECK_EQUAL(true, interval.is_normalized());
		CHECK(result.second > 0);
	}
}

}

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

std::string unary(uint n)
{
	std::ostringstream out;
	CHECK(out.good());
	{
		EntropyWriter bw(out);
		for(uint i = 0; i < n; ++i) {
			bw.write(i0F.base, i0F.base + i0F.range);
		}
	}
	CHECK(out.good());
	out.flush();
	return out.str();
}

TEST(UnaryCode)
{
	// 1…10
	CHECK_EQUAL("", unary(0));
	CHECK_EQUAL("\x80", unary(1));
	CHECK_EQUAL("\x40", unary(2));
	CHECK_EQUAL("\xC0", unary(3));
	CHECK_EQUAL("\x20", unary(4));
	CHECK_EQUAL("\x60", unary(5));
	CHECK_EQUAL("\xA0", unary(6));
	CHECK_EQUAL("\xE0", unary(7));
	CHECK_EQUAL("\x10", unary(8));
	CHECK_EQUAL("\x30", unary(9));
	CHECK_EQUAL("\x50", unary(10));
	
	// 2ⁿ (Note that a final 0x80 is implicit when the output ends on 0x00)
	CHECK_EQUAL("\x08", unary(16));
	CHECK_EQUAL("\x04", unary(32));
	CHECK_EQUAL("\x02", unary(64));
	CHECK_EQUAL("\x01", unary(128));
	CHECK_EQUAL((std::string{0x00}), unary(256)); // The 0x80 is implicit
	CHECK_EQUAL((std::string{0x00, 0x40}), unary(512));
	CHECK_EQUAL((std::string{0x00, 0x20}), unary(1024));
	CHECK_EQUAL((std::string{0x00, 0x10}), unary(2048));
	CHECK_EQUAL((std::string{0x00, 0x08}), unary(4096));
	CHECK_EQUAL((std::string{0x00, 0x04}), unary(8192));
	CHECK_EQUAL((std::string{0x00, 0x02}), unary(16384));
	CHECK_EQUAL((std::string{0x00, 0x01}), unary(32768));
	CHECK_EQUAL((std::string{0x00, 0x00}), unary(65536));
	CHECK_EQUAL((std::string{0x00, 0x00, 0x40}), unary(131072));
	CHECK_EQUAL((std::string{0x00, 0x00, 0x20}), unary(262144));
	
	// 10ⁿ
	CHECK_EQUAL("\x92", unary(100));
	CHECK_EQUAL("\xF4\x40", unary(1000));
	CHECK_EQUAL("\x38\x84", unary(10000));
	CHECK_EQUAL("\x86\xA0\x80", unary(100000));
	CHECK_EQUAL("\xE8\x48\x10", unary(1000000));
}

std::string oneSymbol(const Interval& symbol)
{
	std::ostringstream out;
	CHECK(out.good());
	{
		EntropyWriter bw(out);
		bw.write(symbol.base, symbol.base + symbol.range);
	}
	CHECK(out.good());
	out.flush();
	return out.str();
}

TEST(OneSymbol)
{
	// i03, i05, i07, i08, i09, i0A, i0E, i0F
	// TODO: Are these really the shortest possible?
	CHECK_EQUAL((std::string{0, 0, 0, 0, 0, 0, 0, 2}), oneSymbol(i03));
	CHECK_EQUAL("\x40", oneSymbol(i05));
	CHECK_EQUAL("\x40", oneSymbol(i07));
	CHECK_EQUAL("\x80", oneSymbol(i08));
	CHECK_EQUAL("\x80", oneSymbol(i09));
	CHECK_EQUAL("\x80", oneSymbol(i0A));
	CHECK_EQUAL("\x80", oneSymbol(i0E));
	CHECK_EQUAL("\x80", oneSymbol(i0F));
	
	// i13, i15, i17, i18, i19, i1A, i1E
	CHECK_EQUAL((std::string{0, 0, 0, 0, 0, 0, 0, 4}), oneSymbol(i13));
	CHECK_EQUAL("\x40", oneSymbol(i15));
	CHECK_EQUAL("\x80", oneSymbol(i17));
	CHECK_EQUAL("\x80", oneSymbol(i18));
	CHECK_EQUAL("\x80", oneSymbol(i19));
	CHECK_EQUAL("\x80", oneSymbol(i1A));
	CHECK_EQUAL("\x80", oneSymbol(i1E));
	
	// i23, i25, i27, i28, i29, i2A
	CHECK_EQUAL((std::string{0, 0, 0, 0, 0, 0, 0, 4}), oneSymbol(i23));
	CHECK_EQUAL("\x40", oneSymbol(i25));
	CHECK_EQUAL("\x80", oneSymbol(i27));
	CHECK_EQUAL("\x80", oneSymbol(i28));
	CHECK_EQUAL("\x80", oneSymbol(i29));
	CHECK_EQUAL("\x80", oneSymbol(i2A));
	
	// i33, i35, i37, i38, i39, i3A
	CHECK_EQUAL((std::string{0, 0, 0, 0, 0, 0, 0, 4}), oneSymbol(i33));
	CHECK_EQUAL("\x40", oneSymbol(i25));
	CHECK_EQUAL("\x80", oneSymbol(i27));
	CHECK_EQUAL("\x80", oneSymbol(i28));
	CHECK_EQUAL("\x80", oneSymbol(i29));
	CHECK_EQUAL("\x80", oneSymbol(i2A));
	
	// i53, i55, i57, i58, i59, i5A
	CHECK_EQUAL("\x55\x55\x55\x55\x55\x55\x55\x58", oneSymbol(i53));
	CHECK_EQUAL("\x80", oneSymbol(i55));
	CHECK_EQUAL("\x80", oneSymbol(i57));
	CHECK_EQUAL("\x80", oneSymbol(i58));
	CHECK_EQUAL("\x80", oneSymbol(i59));
	CHECK_EQUAL("\x80", oneSymbol(i5A));
	
	// i73, i75, i77, i78,
	CHECK_EQUAL("\x80", oneSymbol(i73));
	CHECK_EQUAL("\x80", oneSymbol(i75));
	CHECK_EQUAL("\x80", oneSymbol(i77));
	CHECK_EQUAL("\x80", oneSymbol(i78));
	
	// i83, i85, i87,
	CHECK_EQUAL("\x80", oneSymbol(i83));
	CHECK_EQUAL("\x80", oneSymbol(i85));
	CHECK_EQUAL("\x80", oneSymbol(i87));
	
	// i93, i95,
	CHECK_EQUAL((std::string{'\x80', 0, 0, 0, 0, 0, 0, 0x04}), oneSymbol(i93));
	CHECK_EQUAL("\xC0", oneSymbol(i95));
	
	// iA3, iA5,
	CHECK_EQUAL("\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAC", oneSymbol(iA3));
	CHECK_EQUAL("\xC0", oneSymbol(iA5));
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

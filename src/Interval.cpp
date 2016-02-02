#include "Interval.h"
#include "Utilities.h"
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <stdexcept>

std::ostream& operator<<(std::ostream& out, const Interval& interval)
{
	out << "(";
	out << "0x" << std::setw(16) << std::setfill('0') << std::hex;
	out << interval.base;
	out << ", ";
	out << "0x" << std::setw(16) << std::setfill('0') << std::hex;
	out << interval.range;
	out << ")";
	out << std::dec;
	return out;
}

Interval::Interval(double probability)
: base(0)
, range(max)
{
	if(!std::isfinite(probability) || probability < 0.0 || probability > 1.0) {
		throw std::range_error("Probabilities must be in the range [0,1].");
	}
	
	// range = round(p · 2⁶⁴) - 1
	const long double p264 = std::exp2(64.0L);
	const long double p = static_cast<long double>(probability);
	range = static_cast<uint64>(round(p * p264));
	// Subtract the 1, taking care of underflow
	if(probability < 0.5 && range == 0) {
		range = 1UL;
	} else if(probability > 0.5 || range > 1) {
		range -= 1UL;
	}
	
	// Verify
	assert(range >= 1);
	assert(base + range >= base);
}

Interval::Interval(uint64 _base, uint64 _range)
: base(_base)
, range(_range)
{
	if(range == 0 || base + range < range) {
		throw std::range_error("Invalid range, range must be > 0 and base + range must be < 2^64.");
	}
	
	// Verify
	assert(range >= 1);
	assert(base + range >= base);
}

bool Interval::operator==(const Interval& other) const
{
	return base == other.base && range == other.range;
}

double Interval::probability() const
{
	return (static_cast<long double>(range) + 1.0L) * std::exp2(-64.0L);
}

double Interval::entropy() const
{
	return -std::log2(probability());
}

bool Interval::is_normalized() const
{
	return range >= msb;
}

bool Interval::is_goofy() const
{
	return base + range < base;
}

bool Interval::includes(Interval::uint64 value) const
{
	if(!is_goofy()) {
		return value >= base && (value - base == 0 || value - base - 1 < range);
	} else {
		return value >= base || value < base + range + 1;
	}
}

bool Interval::includes(const Interval& interval) const
{
	return interval.base >= base && interval.base + interval.range <= base + range;
}

bool Interval::overlaps(const Interval& interval) const
{
	return includes(interval.base) && !includes(interval);
}

Interval::uint64 Interval::descale(Interval::uint64 value) const
{
	if(!includes(value)) {
		throw std::range_error("Value not in interval.");
	}
	
	// value = (value - base) · 2⁶⁴ / (range + 1)
	if(range == max) {
		return value - base;
	} else {
		assert(value - base < range + 1);
		std::uint64_t q, r;
		std::tie(q, r) = div128(value - base, 0, range + 1);
		q += (r >= msb) ? 1 : 0;
		return q;
	}
}

void Interval::update(const Interval& symbol, bool* carry)
{
	// Check if we are normalized
	if(range < msb) {
		throw std::range_error("Interval must be normalized before update.");
	}
	
	// Check the incoming range:
	if(symbol.range == 0 || symbol.base + symbol.range < symbol.base) {
		throw std::range_error("Invalid symbol interval.");
	}
	
	// Calculate the new base
	uint64 h, l;
	std::tie(h, l) = mul128(symbol.base, range);
	std::tie(h, l) = add128(h, l, symbol.base);
	const uint64 t = h + (l > 0 ? 1 : 0);
	base += t;
	
	// Detect carry
	if(carry != nullptr) {
		*carry = base < t;
	}
	
	// Calculate the new range
	std::tie(h, l) = mul128(symbol.base + symbol.range, range);
	std::tie(h, l) = add128(h, l, symbol.base + symbol.range);
	std::tie(h, l) = add128(h, l, range);
	std::tie(h, l) = add128(h, l, 1);
	range = h - t - 1;
}

std::vector<bool> Interval::normalize()
{
	std::vector<bool> result;
	
	// Calculate number of bits to shift out
	const uint n = range == 0 ? 63 : count_leading_zeros(range);
	result.reserve(n);
	
	// Shift out bits
	while(range < msb) {
		const bool bit = base >= msb;
		result.push_back(bit);
		base <<= 1;
		range <<= 1;
		range |= 1;
	}
	
	// Check our state
	assert(range >= msb);
	return result;
}

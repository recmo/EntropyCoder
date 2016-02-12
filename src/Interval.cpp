#include "Interval.h"
#include <cmath>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <stdexcept>
namespace EntropyCoder {

std::ostream& operator<<(std::ostream& out, const Interval& interval)
{
	const bool wraps = interval.base + interval.range + 1 <= interval.base;
	out << "[";
	out << "0." << std::setw(16) << std::setfill('0') << std::hex;
	out << interval.base;
	out << (wraps ? ", 1." : ", 0.");
	out << std::setw(16) << std::setfill('0') << std::hex;
	out << interval.base + interval.range + 1;
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
	
	if(probability < 1.0) {
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

bool Interval::includes(Interval::uint64 value) const
{
	uint64 top = base + range + 1;
	if(top <= base) {
		return value >= base || value < top;
	} else {
		return value >= base && value < top;
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

} // namespace EntropyCoder

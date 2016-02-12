#include "CodeInterval.h"
#include "Utilities.h"
namespace EntropyCoder {

bool CodeInterval::is_normalized() const
{
	return range >= msb;
}

bool CodeInterval::wraps() const
{
	return base + range < base;
}

Interval::uint64 CodeInterval::descale(Interval::uint64 value) const
{
	if(!is_normalized()) {
		throw std::range_error("Interval must be normalized before descale.");
	}
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

bool CodeInterval::update(const Interval& symbol)
{
	if(!is_normalized()) {
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
	
	// Calculate the new range
	std::tie(h, l) = mul128(symbol.base + symbol.range, range);
	std::tie(h, l) = add128(h, l, symbol.base + symbol.range);
	std::tie(h, l) = add128(h, l, range);
	std::tie(h, l) = add128(h, l, 1);
	range = h - t - 1;
	
	// Return carry
	return base < t;
}

std::pair<Interval::uint64, int> CodeInterval::normalize()
{
	const uint64 bits = base;
	if(range == 0) {
		base = 0;
		range = max;
		return {bits, 64};
	} else {
		const int n = count_leading_zeros(range);
		base <<= n;
		range <<= n;
		range |= (1UL << n) - 1UL;
		assert(range >= msb);
		return {bits, n};
	}
}

} // namespace EntropyCoder

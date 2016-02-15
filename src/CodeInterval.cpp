#include "CodeInterval.h"
#include "Utilities.h"
namespace EntropyCoder {

bool CodeInterval::is_normalized() const noexcept
{
	return range >= msb;
}

bool CodeInterval::wraps() const noexcept
{
	return base + range < base;
}

Interval::uint64 CodeInterval::descale(Interval::uint64 value) const throw(not_normalized, range_error)
{
	if(!is_normalized()) {
		throw not_normalized("Interval must be normalized before descale.");
	}
	if(!includes(value)) {
		throw range_error("Value not in interval.");
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

bool CodeInterval::update(const Interval& symbol) throw(not_normalized, invalid_symbol)
{
	if(!is_normalized()) {
		throw not_normalized("Interval must be normalized before update.");
	}
	if(symbol.range < 3 || symbol.base + symbol.range < symbol.base) {
		throw invalid_symbol("Invalid symbol interval.");
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

std::pair<Interval::uint64, int> CodeInterval::normalize() noexcept
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

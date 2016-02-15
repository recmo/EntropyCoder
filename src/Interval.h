#pragma once
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
namespace EntropyCoder {

// The represented interval is [base · 2⁻⁶⁴, (base + range + 1) · 2⁻⁶⁴)
class Interval {
public:
	typedef std::range_error range_error;
	
	typedef std::uint64_t uint64;
	static constexpr int bits = 8 * sizeof(uint64);
	static constexpr uint64 max = std::numeric_limits<uint64>::max();
	static constexpr uint64 msb = 1UL << (bits - 1);
	
	Interval() noexcept { }
	explicit Interval(double probability) throw(range_error);
	Interval(uint64 base, uint64 range) throw(range_error);
	
	uint64 base = 0;
	uint64 range = max;
	
	bool operator==(const Interval& other) const noexcept;
	bool operator!=(const Interval& other) const noexcept { return !(*this == other); }
	double probability() const noexcept;
	double entropy() const noexcept;
	
	bool includes(uint64 value) const noexcept;
	bool includes(const Interval& interval) const noexcept;
	bool overlaps(const Interval& interval) const noexcept;
	bool disjoint(const Interval& interval) const noexcept;
};

std::ostream& operator<<(std::ostream& out, const Interval& interval);

} // namespace EntropyCoder

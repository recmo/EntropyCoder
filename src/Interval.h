#pragma once
#include <cstdint>
#include <iostream>
#include <limits>
namespace EntropyCoder {

// The represented interval is [base · 2⁻⁶⁴, (base + range + 1) · 2⁻⁶⁴)
class Interval {
public:
	typedef std::uint64_t uint64;
	static constexpr int bits = 8 * sizeof(uint64);
	static constexpr uint64 max = std::numeric_limits<uint64>::max();
	static constexpr uint64 msb = 1UL << (bits - 1);
	
	Interval() { }
	explicit Interval(double probability);
	Interval(uint64 base, uint64 range);
	
	uint64 base = 0;
	uint64 range = max;
	
	bool operator==(const Interval& other) const;
	bool operator!=(const Interval& other) const { return !(*this == other); }
	double probability() const;
	double entropy() const;
	bool includes(uint64 value) const;
	bool includes(const Interval& interval) const;
	bool overlaps(const Interval& interval) const;
};

std::ostream& operator<<(std::ostream& out, const Interval& interval);

} // namespace EntropyCoder

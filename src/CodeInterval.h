#pragma once
#include "Interval.h"
#include <tuple>

class CodeInterval: public Interval {
public:
	CodeInterval(): Interval{} { }
	CodeInterval(const Interval& interval): Interval{interval} { }
	
	bool is_normalized() const;
	bool wraps() const;
	uint64 descale(uint64 value) const;
	
	/// @returns true on carry
	bool update(const Interval& symbol);
	
	/// @returns (b, n) where b has n bits of output in its most significant bits
	std::pair<uint64, int> normalize();
};

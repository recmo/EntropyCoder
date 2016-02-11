#pragma once
#include "Interval.h"

class CodeInterval: public Interval {
public:
	
	bool is_normalized() const;
	bool wraps() const;
	uint64 descale(uint64 value) const;
	
	/// @returns true on carry
	bool update(const Interval& symbol);
	std::vector<bool> normalize();
};

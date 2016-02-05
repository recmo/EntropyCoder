#pragma once
#include "Interval.h"

class CodeInterval: public Interval {
public:
	
	bool is_normalized() const;
	bool wraps() const;
	uint64 descale(uint64 value) const;
	void update(const Interval& symbol, bool* carry = nullptr);
	std::vector<bool> normalize();
};

#include "EntropyWriter.h"
#include <cassert>
#include <iostream>
#include <iomanip>
#include <algorithm>

constexpr bool print = false;

typedef std::uint64_t uint64;

EntropyWriter::~EntropyWriter()
{
	const bool carry1 = end.ending & End::msb;
	const uint64 value1 = end.ending << 1;
	
	bool carry = false;
	uint64 value = endings.end(&carry);
	assert(carry1 == carry);
	assert(value1 == value);
	
	if(carry) {
		bw.add_carry();
	}
	while(value != 0) {
		if(value & Interval::msb) {
			bw.write_one();
		} else {
			bw.write_zero();
		}
		value <<= 1;
	}
}

void EntropyWriter::write(const Interval& symbol)
{
	if(print) std::cerr << "WRITE " << current << " " << symbol << "\n";
	
	// We didn't use the ending, so we should reserve it.
	endings.reserve_current();
	end.next();
	
	// Update interval
	bool carry;
	current.update(symbol, &carry);
	
	// Apply carry
	if(carry) {
		bw.add_carry();
		endings.prune_carry();
		end.carry();
	}
	
	// Remove endings that are now out of the interval
	endings.prune(current);
	
	// Shift out bits
	for(bool bit: current.normalize()) {
		if(bit) {
			bw.write_one();
			endings.prune_one();
			end.one();
		} else {
			bw.write_zero();
			endings.prune_zero();
			end.zero();
		}
	}
	
	// Generate a new potential ending
	endings.generate(current);
	end.first(current);
}

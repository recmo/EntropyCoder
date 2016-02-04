#include "EntropyWriter.h"
#include <cassert>
#include <iostream>
#include <iomanip>
#include <algorithm>

constexpr bool print = false;

typedef std::uint64_t uint64;

EntropyWriter::~EntropyWriter()
{
	bool carry = false;
	uint64 end = ending.end(&carry);
	if(carry) {
		bw.add_carry();
	}
	while(end != 0) {
		if(end & Interval::msb) {
			bw.write_one();
		} else {
			bw.write_zero();
		}
		end <<= 1;
	}
}

void EntropyWriter::write(const Interval& symbol)
{
	if(print) std::cerr << "WRITE " << current << " " << symbol << "\n";
	
	// We didn't use the ending, so we should reserve it.
	ending.reserve_current();
	
	// Update interval
	bool carry;
	current.update(symbol, &carry);
	
	// Apply carry
	if(carry) {
		bw.add_carry();
		ending.prune_carry();
	}
	
	// Remove endings that are now out of the interval
	ending.prune(current);
	
	// Shift out bits
	for(bool bit: current.normalize()) {
		if(bit) {
			bw.write_one();
			ending.prune_one();
		} else {
			bw.write_zero();
			ending.prune_zero();
		}
	}
	
	// Generate a new potential ending
	ending.generate(current);
}

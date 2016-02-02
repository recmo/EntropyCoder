#include "EntropyWriter.h"
#include <cassert>
#include <iostream>
#include <iomanip>

typedef std::uint64_t uint64;

EntropyWriter::~EntropyWriter()
{
	bool carry;
	uint64 end = current.ending(&carry);
	if(carry) {
		bw.add_carry();
	}
	while(end != 0) {
		if((end & Interval::msb) != 0) {
			bw.write_one();
		} else {
			bw.write_zero();
		}
		end <<= 1;
	}
}

void EntropyWriter::write(const Interval& symbol)
{
	// Update interval
	bool carry;
	current.update(symbol, &carry);
	
	// Apply carry
	if(carry) {
		bw.add_carry();
	}
	
	// Shift out bits
	for(bool bit: current.normalize()) {
		if(bit) {
			bw.write_one();
		} else {
			bw.write_zero();
		}
	}
}

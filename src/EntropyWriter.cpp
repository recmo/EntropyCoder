#include "EntropyWriter.h"
#include <cassert>
#include <iostream>
#include <iomanip>

typedef std::uint64_t uint64;

EntropyWriter::~EntropyWriter()
{
	// TODO Proper ending.
}

void EntropyWriter::write(const Interval& symbol)
{
	// Update interval
	bool carry;
	current.update(symbol, &carry);
	
	// Apply carry
	if(carry) {
		//std::cerr << "CARRY" << "\n";
		bw.add_carry();
	}
	
	// Shift out bits
	for(bool bit: current.normalize()) {
		if(bit) {
			//std::cerr << "ONE" << "\n";
			bw.write_one();
		} else {
			//std::cerr << "ZERO" << "\n";
			bw.write_zero();
		}
	}
}

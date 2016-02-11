#include "EntropyWriter.h"
#include <cassert>
#include <iostream>
#include <iomanip>
#include <algorithm>

constexpr bool print = false;

typedef std::uint64_t uint64;

EntropyWriter::~EntropyWriter()
{
	if(end.carry_bit()) {
		bw.add_carry();
	}
	uint64 value = end.value_bits();
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
	end.next();
	
	// Update interval and add carry
	if(current.update(symbol)) {
		bw.add_carry();
		end.carry();
	}
	
	// Shift out bits
	for(bool bit: current.normalize()) {
		if(bit) {
			bw.write_one();
			end.one();
		} else {
			bw.write_zero();
			end.zero();
		}
	}
	
	// Generate a new potential ending
	end.first(current);
}

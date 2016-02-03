#include "EntropyReader.h"
#include "Utilities.h"
#include <cassert>
#include <iostream>
#include <iomanip>

EntropyReader::EntropyReader(std::istream& input)
: br(input)
{
	for(uint i = 0; i < Interval::bits; ++i) {
		value <<= 1;
		if(!br.eof()) {
			value |= br.read_bit() ? 1 : 0;
		}
	}
}

bool EntropyReader::eof() const
{
	return br.eof() && value == ending.end();
}

EntropyReader::uint64 EntropyReader::read() const
{
	return current.descale(value);
}

void EntropyReader::read(const Interval& symbol)
{
	ending.reserve_current();
	bool carry = false;
	current.update(symbol, &carry);
	if(carry) {
		ending.prune_carry();
	}
	ending.prune(current);
	for(bool bit: current.normalize()) {
		if(bit) {
			ending.prune_one();
		} else {
			ending.prune_zero();
		}
		value <<= 1;
		if(!br.eof()) {
			value |= br.read_bit() ? 1 : 0;
		} else {
			++past_end;
			assert(past_end < 100);
		}
	}
	ending.generate(current);
}

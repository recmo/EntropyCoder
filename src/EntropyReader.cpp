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
	return eof_count >= Interval::bits;
}

EntropyReader::uint64 EntropyReader::read() const
{
	return current.descale(value);
}

void EntropyReader::read(const Interval& symbol)
{
	current.update(symbol);
	for(bool bit: current.normalize()) {
		value <<= 1;
		if(!br.eof()) {
			value |= br.read_bit() ? 1 : 0;
		} else {
			++eof_count;
		}
	}
}

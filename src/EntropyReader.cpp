#include "EntropyReader.h"
#include "Utilities.h"
#include <cassert>
#include <iostream>
#include <iomanip>

EntropyReader::EntropyReader(std::istream& input)
: br(input)
{
	for(uint i = 0; i < Interval::bits; ++i) {
		current.base <<= 1;
		current.base |= br.read_bit() ? 1 : 0;
	}
}

bool EntropyReader::eof() const
{
	return eof_count == Interval::bits;
}

void EntropyReader::read(const Interval& symbol)
{
	current.update(symbol);
	
	while(!current.is_normalized()) {
		current.base <<= 1;
		current.range <<= 1;
		if(!br.eof()) {
			current.base |= br.read_bit() ? 1 : 0;
		} else {
			++eof_count;
		}
		current.range |= 1;
	}
}

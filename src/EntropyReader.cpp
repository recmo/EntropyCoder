#include "EntropyReader.h"
#include "Utilities.h"
#include <cassert>
#include <iostream>
#include <iomanip>

constexpr bool print = false;

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
	if(!br.eof()) {
		return false;
	}
	const bool carry = end.ending & End::msb;
	const uint64 end_value = end.ending << 1;
	if(print) std::cerr << "EOF? " << current << " ";
	if(print) std::cerr << "0x" << std::setw(16) << std::setfill('0') << std::hex;
	if(print) std::cerr << value << " ";
	if(print) std::cerr << "0x" << std::setw(16) << std::setfill('0') << std::hex;
	if(print) std::cerr << end_value << "\n";
	return value == end_value;
	if(carry && value >= current.base) {
		return false;
	}
	return value == end_value;
}

EntropyReader::uint64 EntropyReader::read() const
{
	uint64 descaled = current.descale(value);
	if(print) std::cerr << "READ " << current << " ";
	if(print) std::cerr << "0x" << std::setw(16) << std::setfill('0') << std::hex;
	if(print) std::cerr << value << " ";
	if(print) std::cerr << "0x" << std::setw(16) << std::setfill('0') << std::hex;
	if(print) std::cerr << descaled << "\n";
	return current.descale(value);
}

void EntropyReader::read(const Interval& symbol)
{
	end.next();
	if(current.update(symbol)) {
		end.carry();
	}
	for(bool bit: current.normalize()) {
		if(bit) {
			end.one();
		} else {
			end.zero();
		}
		value <<= 1;
		if(!br.eof()) {
			value |= br.read_bit() ? 1 : 0;
		} else {
			++past_end;
			
			// TODO: what is the right limit here? 100 was not enough for a simple
			// case like
			// echo -e "true\nfalse\ntrue\ntrue" | ./EntropyCoder roundtrip -p 1e-15
			// Why would a thousand be enough?
			assert(past_end < 1000);
		}
	}
	end.first(current);
}

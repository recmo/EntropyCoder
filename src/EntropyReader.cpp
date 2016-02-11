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
	bool carry = false;
	const uint64 end_value = endings.end(&carry);
	
	const bool carry1 = end.ending & End::msb;
	const uint64 end_value1 = end.ending << 1;
	assert(carry == carry1);
	assert(end_value == end_value1);
	
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
	endings.reserve_current();
	end.next();
	bool carry = false;
	current.update(symbol, &carry);
	if(carry) {
		endings.prune_carry();
		end.carry();
	}
	endings.prune(current);
	for(bool bit: current.normalize()) {
		if(bit) {
			endings.prune_one();
			end.one();
		} else {
			endings.prune_zero();
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
	endings.generate(current);
	end.first(current);
}

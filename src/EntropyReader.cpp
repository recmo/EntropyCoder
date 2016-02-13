#include "EntropyReader.h"
#include "BinaryReader.h"
#include "CodeInterval.h"
#include "Utilities.h"
#include "End.h"
#include <cstdint>
#include <cassert>
#include <iostream>
#include <iomanip>
namespace EntropyCoder {

constexpr bool print = false;

/******************************************************************************/

class EntropyReader::Implementation {
public:
	Implementation(std::istream& input);
	
	bool eof() const;
	std::uint64_t read() const;
	void read(const Interval& symbol);
	
private:
	BinaryReader br;
	CodeInterval current;
	Interval::uint64 value;
	End end;
	std::uint64_t past_end = 0;
};

EntropyReader::EntropyReader(std::istream& input)
: impl{new Implementation{input}}
{
}

EntropyReader::~EntropyReader()
{
	// Needs to be here because ~unique_ptr<Implementation>() needs to see
	// the Implementation class.
}

bool EntropyReader::eof() const
{
	return impl->eof();
}

std::uint64_t EntropyReader::read() const
{
	return impl->read();
}

void EntropyReader::read(const Interval& symbol)
{
	return impl->read(symbol);
}

/******************************************************************************/

EntropyReader::Implementation::Implementation(std::istream& input)
: br(input)
{
	for(uint i = 0; i < Interval::bits; ++i) {
		value <<= 1;
		if(!br.eof()) {
			value |= br.read_bit() ? 1 : 0;
		}
	}
}

bool EntropyReader::Implementation::eof() const
{
	if(!br.eof()) {
		return false;
	}
	return value == end.value_bits();
}

std::uint64_t EntropyReader::Implementation::read() const
{
	std::uint64_t descaled = current.descale(value);
	if(print) std::cerr << "READ " << current << " ";
	if(print) std::cerr << "0x" << std::setw(16) << std::setfill('0') << std::hex;
	if(print) std::cerr << value << " ";
	if(print) std::cerr << "0x" << std::setw(16) << std::setfill('0') << std::hex;
	if(print) std::cerr << descaled << "\n";
	return current.descale(value);
}

void EntropyReader::Implementation::read(const Interval& symbol)
{
	end.next();
	if(current.update(symbol)) {
		end.carry();
	}
	auto bits = current.normalize();
	while(bits.second--) {
		if(bits.first >= Interval::msb) {
			end.one();
		} else {
			end.zero();
		}
		bits.first <<= 1;
		
		value <<= 1;
		if(!br.eof()) {
			value |= br.read_bit() ? 1 : 0;
		} else {
			++past_end;
			
			// TODO: what is the right limit here? 100 was not enough for a simple
			// case like
			// echo -e "true\nfalse\ntrue\ntrue"|./EntropyCoder roundtrip -p 1e-15
			// Why would a thousand be enough?
			assert(past_end < 1000);
		}
	}
	end.first(current);
}

} // namespace EntropyCoder

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

/******************************************************************************/

class EntropyReader::Implementation {
public:
	Implementation(std::istream& input) throw(io_error);
	
	bool eof() const noexcept;
	std::uint64_t read() const noexcept;
	void read(const Interval& symbol) throw(io_error);
	
private:
	BinaryReader br;
	CodeInterval current;
	Interval::uint64 value;
	End end;
	std::uint64_t past_end = 0;
};

EntropyReader::EntropyReader(std::istream& input) throw(std::bad_alloc, io_error)
: impl{new Implementation{input}}
{
}

EntropyReader::~EntropyReader() noexcept
{
	// Needs to be here because ~unique_ptr<Implementation>() needs to see
	// the Implementation class.
}

bool EntropyReader::eof() const noexcept
{
	return impl->eof();
}

std::uint64_t EntropyReader::value() const noexcept
{
	return impl->read();
}

void EntropyReader::next(const uint64_t start, const uint64_t end) throw(io_error)
{
	return impl->read(Interval{start, end - start});
}

/******************************************************************************/

EntropyReader::Implementation::Implementation(std::istream& input) throw(io_error)
: br(input)
{
	for(uint i = 0; i < Interval::bits; ++i) {
		value <<= 1;
		if(!br.eof()) {
			value |= br.read_bit() ? 1 : 0;
		}
	}
}

bool EntropyReader::Implementation::eof() const noexcept
{
	if(!br.eof()) {
		return false;
	}
	return value == end.value_bits();
}

std::uint64_t EntropyReader::Implementation::read() const noexcept
{
	return current.descale(value);
}

void EntropyReader::Implementation::read(const Interval& symbol) throw(io_error)
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

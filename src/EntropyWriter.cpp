#include "EntropyWriter.h"
#include "BinaryWriter.h"
#include "CodeInterval.h"
#include "End.h"
#include <cassert>
#include <iostream>
#include <iomanip>
#include <algorithm>
namespace EntropyCoder {

/******************************************************************************/

class EntropyWriter::Implementation {
public:
	Implementation(std::ostream& output) noexcept;
	~Implementation() noexcept;
	
	void write(const Interval& symbol) throw(io_error);
	void finalize() throw(io_error);
	
private:
	BinaryWriter bw;
	CodeInterval current;
	End end;
	bool finalized = false;
};

EntropyWriter::EntropyWriter(std::ostream& output) throw(std::bad_alloc)
: impl{new Implementation{output}}
{
}

EntropyWriter::~EntropyWriter() noexcept
{
	// Needs to be here because ~unique_ptr<Implementation>() needs to see
	// the Implementation class.
}

void EntropyWriter::write(const std::uint64_t start, const std::uint64_t end) throw(io_error)
{
	impl->write(Interval{start, end - start});
}

void EntropyWriter::finalize() throw(io_error)
{
	impl->finalize();
}

/******************************************************************************/

EntropyWriter::Implementation::Implementation(std::ostream& output) noexcept
: bw(output)
{
}

EntropyWriter::Implementation::~Implementation() noexcept
{
	try {
		if(!finalized) {
			finalize();
		}
	}
	catch(...) {
	}
}

void EntropyWriter::Implementation::write(const Interval& symbol) throw(io_error)
{
	// We didn't use the ending, so we should reserve it.
	end.next();
	
	// Update interval and add carry
	if(current.update(symbol)) {
		bw.add_carry();
		end.carry();
	}
	
	// Shift out bits
	auto bits = current.normalize();
	while(bits.second--) {
		if(bits.first >= Interval::msb) {
			bw.write_one();
			end.one();
		} else {
			bw.write_zero();
			end.zero();
		}
		bits.first <<= 1;
	}
	
	// Generate a new potential ending
	end.first(current);
}

void EntropyCoder::EntropyWriter::Implementation::finalize() throw(io_error)
{
	if(end.carry_bit()) {
		bw.add_carry();
	}
	std::uint64_t value = end.value_bits();
	while(value != 0) {
		if(value & Interval::msb) {
			bw.write_one();
		} else {
			bw.write_zero();
		}
		value <<= 1;
	}
	bw.finalize();
	finalized = true;
}

} // namespace EntropyCoder

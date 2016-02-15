#include "BinaryReader.h"
#include <iostream>
#include <stdexcept>
namespace EntropyCoder {

BinaryReader::BinaryReader(std::istream& input) noexcept
: in(input)
{
	// Store the original ostream::exceptions mask
	original_state = in.exceptions();
	
	// ostream::put will set the badbit but not the failbit.
	in.exceptions(std::ios_base::failbit | std::ios_base::badbit);
	
	// Call peek() to to update in.eof()
	in.peek();
}

BinaryReader::~BinaryReader() noexcept
{
	// Restore the ostream::exceptions mask to the original state
	in.exceptions(original_state);
}

bool BinaryReader::eof() const noexcept
{
	static_assert(buffer_size == 8, "This code assumes one byte buffers.");
	const uint8_t remaining_mask = mask == 0 ? 0 : mask | (mask - 1);
	const bool bits_remaing_in_buffer = (buffer & remaining_mask) != 0;
	const bool byte_eof = in.eof() && !append;
	return !bits_remaing_in_buffer && byte_eof;
}

bool BinaryReader::read_bit() throw(read_beyond_end, io_error)
{
	if(eof()) {
		throw read_beyond_end("Can not read beyond end of file.");
	}
	
	// Read from input
	if(mask == 0) {
		buffer = read_byte();
		mask = 1UL << (buffer_size - 1);
	}
	
	bool bit = (buffer & mask) != 0;
	mask >>= 1;
	return bit;
}

BinaryReader::uint8 BinaryReader::read_byte() throw(io_error)
{
	if(!in.eof()) {
		const uint8 byte = in.get();
		in.peek();
		append = byte == 0x00 || (append && byte == 0x80);
		return byte;
	} else {
		assert(append);
		append = false;
		return 0x80;
	}
}

} // namespace EntropyCoder

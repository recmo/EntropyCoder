#include "BinaryReader.h"
#include <iostream>
#include <stdexcept>

BinaryReader::BinaryReader(std::istream& input)
: in(input)
{
	in.peek();
}

bool BinaryReader::eof() const
{
	static_assert(buffer_size == 8, "This code assumes one byte buffers.");
	const uint8_t remaining_mask = mask == 0 ? 0 : mask | (mask - 1);
	const bool bits_remaing_in_buffer = (buffer & remaining_mask) != 0;
	return !bits_remaing_in_buffer && byte_eof();
}

bool BinaryReader::read_bit()
{
	if(eof()) {
		throw std::runtime_error("Can not read beyond end of file.");
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

bool BinaryReader::byte_eof() const
{
	return in.eof() && !append;
}

BinaryReader::uint8 BinaryReader::read_byte()
{
	if(!in.eof()) {
		const uint8 byte = in.get();
		in.peek();
		append = byte == 0x00 || (append && byte == 0x80);
		return byte;
	} else if(append) {
		append = false;
		return 0x80;
	} else {
		throw std::runtime_error("Can not read beyond end of file.");
	}
}

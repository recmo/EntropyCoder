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
	// We can not end on zero.
	static_assert(buffer_size == 8, "This code assumes one byte buffers.");
	const uint8_t remaining_mask = mask == 0 ? 0 : mask | (mask - 1);
	const bool bits_remaing_in_buffer = (buffer & remaining_mask) != 0;
	return !bits_remaing_in_buffer && in.eof() && buffer != 0x00;
}

bool BinaryReader::read_bit()
{
	if(eof()) {
		throw std::runtime_error("Can not read beyond end of file.");
	}
	
	// Read from input
	if(mask == 0 && !in.eof()) {
		buffer = in.get();
		in.peek();
		mask = 1UL << (buffer_size - 1);
	}
	
	// If we end on 0x00, append a final 1
	static_assert(buffer_size == 8, "This code assumes one byte buffers.");
	if(mask == 0 && buffer == 0x00 && in.eof()) {
		buffer = 1;
		mask = 1;
	}
	
	bool bit = (buffer & mask) != 0;
	mask >>= 1;
	return bit;
}

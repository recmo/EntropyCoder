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
	return position == 0 && in.eof();
}

bool BinaryReader::read_bit()
{
	if(eof()) {
		throw std::runtime_error("Can not read beyond end of file.");
	}
	
	if(position == 0) {
		buffer = in.get();
		in.peek();
		position = buffer_size;
	}
	
	bool bit = buffer >> (buffer_size - 1);
	buffer <<= 1;
	--position;
	return bit;
}

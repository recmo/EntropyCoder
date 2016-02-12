#pragma once
#include <cstdint>
#include <iostream>
namespace EntropyCoder {

class BinaryReader {
public:
	typedef std::uint64_t uint64;
	
	BinaryReader(std::istream& input);
	
	bool eof() const;
	bool read_bit();
	
private:
	typedef std::uint8_t uint8;
	std::istream& in;
	
	// We use an 8 bit input buffer.
	uint8 buffer = 1;
	uint8 mask = 0;
	static constexpr std::size_t buffer_size = 8 * sizeof(buffer);
	
	// If we end on 0x00 0x80*, append a final 0x80
	bool append = false;
	bool byte_eof() const;
	uint8 read_byte();
};

} // namespace EntropyCoder

#pragma once
#include <cstdint>
#include <iostream>

class BinaryReader {
public:
	typedef std::uint64_t uint64;
	
	BinaryReader(std::istream& input);
	
	bool eof() const;
	bool read_bit();
	
private:
	std::istream& in;
	
	// We use an 8 bit input buffer.
	uint8_t buffer = 1;
	uint8_t mask = 0;
	static constexpr std::size_t buffer_size = 8 * sizeof(buffer);
};

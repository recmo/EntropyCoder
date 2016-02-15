#pragma once
#include <cstdint>
#include <iostream>
#include <stdexcept>
namespace EntropyCoder {

class BinaryReader {
public:
	class read_beyond_end: public std::logic_error {
	public:
		explicit read_beyond_end(const std::string& what)
		: logic_error(what) { }
	};
	typedef std::ios_base::failure io_error;
	
	BinaryReader(std::istream& input) noexcept;
	~BinaryReader() noexcept;
	
	bool eof() const noexcept;
	bool read_bit() throw(read_beyond_end, io_error);
	
private:
	typedef std::uint8_t uint8;
	std::istream& in;
	std::ios_base::iostate original_state;
	
	// We use an 8 bit input buffer.
	uint8 buffer = 1;
	uint8 mask = 0;
	static constexpr std::size_t buffer_size = 8 * sizeof(buffer);
	
	// If we end on 0x00 0x80*, append a final 0x80
	bool append = false;
	uint8 read_byte() throw(io_error);
};

} // namespace EntropyCoder

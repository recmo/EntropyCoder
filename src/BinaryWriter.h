#pragma once
#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>
namespace EntropyCoder {

/// Class for writing ones and zeros with carry propagation.
class BinaryWriter {
public:
	class invalid_carry: public std::logic_error {
	public:
		explicit invalid_carry(const std::string& what)
		: logic_error(what) { }
	};
	class already_finalized: public std::logic_error {
	public:
		explicit already_finalized(const std::string& what)
		: logic_error(what) { }
	};
	typedef std::ios_base::failure io_error;
	
	BinaryWriter(std::ostream& output) noexcept;
	~BinaryWriter() noexcept;
	
	/// Write a zero to the stream.
	void write_zero() throw(already_finalized, io_error);
	
	/// Write a one to the stream.
	void write_one() throw(already_finalized, io_error);
	
	/// Propagate a carry up into the stream.
	/// @note Will throw an `std::runtime_error` if it is not
	/// possible to propagate the carry.
	void add_carry() throw(invalid_carry, already_finalized, io_error);
	
	void finalize() throw(already_finalized, io_error);
	
	bool can_add_carry() const noexcept { return carry_buffer != 0; }
	bool ends_in_one() const noexcept { return carry_buffer > 1; }
	
private:
	std::ostream& out;
	std::ios_base::iostate original_state;
	bool finalized = false;
	
	// We delay output of strings like 01111… because we
	// might have to add a carry. `carry_buffer` is the
	// total length of such a sequence, including the 0.
	// If `carry_buffer` is zero, there is no carry buffer.
	uint64_t carry_buffer = 0ULL;
	
	// We use an 8 bit output buffer.
	uint8_t buffer = 0;
	std::size_t position = 0;
	static constexpr std::size_t buffer_size = 8 * sizeof(buffer);
	void immediate_zero() throw(io_error);
	void immediate_one() throw(io_error);
	
	// We delay output of bytes 0x00 0x00 0x00… because
	// we remove trailing zeros.
	void write_byte(uint8_t byte) throw(io_error);
	uint zero_bytes = 0;
	
	// We drop the last 0x80 if the output ends in 0x00 0x80+
	void write_byte2(uint8_t byte) throw(io_error);
	uint delay = 0;
};

} // namespace EntropyCoder

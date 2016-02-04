#pragma once
#include <cstdint>
#include <iostream>
#include <limits>

/// Class for writing ones and zeros with carry propagation.
class BinaryWriter {
public:
	BinaryWriter(std::ostream& output): out(output) { }
	~BinaryWriter();
	
	/// Write a zero to the stream.
	void write_zero();
	
	/// Write a one to the stream.
	void write_one();
	
	/// Propagate a carry up into the stream.
	/// @note Will throw an `std::runtime_error` if it is not
	/// possible to propagate the carry.
	void add_carry();
	
	bool can_add_carry() const { return carry_buffer != 0; }
	bool ends_in_one() const { return carry_buffer > 1; }
	
private:
	std::ostream& out;
	
	// We delay output of strings like 01111… because we
	// might have to add a carry. `carry_buffer` is the
	// total length of such a sequence, including the 0.
	// If `carry_buffer` is zero, there is no carry buffer.
	uint64_t carry_buffer = 0ULL;
	
	// We use an 8 bit output buffer.
	uint8_t buffer = 0;
	bool last_byte_zero = false;
	std::size_t position = 0;
	static constexpr std::size_t buffer_size = 8 * sizeof(buffer);
	void immediate_zero();
	void immediate_one();
	
	// We delay output of bytes 0x00 0x00 0x00… because
	// we remove trailing zeros.
	void write_byte(uint8_t byte);
	uint zero_bytes = 0;
	
	// Finally, we delay output of 0x80 after a 0x00. We will
	// drop the 0x80 if it is the final byte.
	void write_byte2(uint8_t byte);
	uint delay = 0;
};

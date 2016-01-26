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
	
private:
	std::ostream& out;
	
	// We delay output of strings like 01111… because we
	// might have to add a carry. `carry_buffer` is the
	// total length of such a sequence, including the 0.
	// If `carry_buffer` is zero, there is no carry buffer.
	uint64_t carry_buffer = 0ULL;
	
	// We use an 8 bit output buffer.
	uint8_t buffer = 0;
	std::size_t position = 0;
	static constexpr std::size_t buffer_size = 8 * sizeof(buffer);
	void immediate_zero();
	void immediate_one();
};

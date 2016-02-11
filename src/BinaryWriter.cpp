#include "BinaryWriter.h"
#include <cassert>

constexpr bool print = false;

BinaryWriter::~BinaryWriter()
{
	// Flush carry buffer
	if(carry_buffer != 0) {
		// Write prefix zero
		immediate_zero();
		
		// Write the trailing ones
		while(--carry_buffer) {
			// Do not write a single trailing 1 after a series of zeros
			if(last_byte_zero && carry_buffer == 1) {
				return;
			}
			immediate_one();
		}
	}
	
	// Flush bit buffer
	buffer <<= (buffer_size - position);
	write_byte(buffer);
	
	// Flush byte buffer, dropping the last value
	assert(delay != 1); // We can not end on a trailing zero
	if(delay > 1) {
		--delay;
		out << '\x00';
		while(--delay) {
			out << '\x80';
		}
	}
}

void BinaryWriter::write_zero()
{
	if(print) std::cerr << "ZERO\n";
	// Flush the carry buffer without adding a carry
	if(carry_buffer != 0) {
		// Write prefix zero
		immediate_zero();
		
		// Write the trailing ones
		while(--carry_buffer) {
			immediate_one();
		}
	}
	
	// Reset carry buffer to contain only a zero
	carry_buffer = 1;
}

void BinaryWriter::write_one()
{
	if(print) std::cerr << "ONE\n";
	if(carry_buffer == 0) {
		immediate_one();
	} else {
		++carry_buffer;
	}
}

void BinaryWriter::add_carry()
{
	if(print) std::cerr << "CARRY\n";
	if(carry_buffer == 0) {
		throw std::runtime_error("Nothing to add carry to.");
	}
	
	// Flush the carry buffer without adding a carry
	// Write prefix one
	immediate_one();
		
	// Write the trailing zeros
	while(--carry_buffer) {
		immediate_zero();
	}
	
	// Empty the carry buffer
	carry_buffer = 0;
}

void BinaryWriter::immediate_one()
{
	// Flush buffer when full
	if(position == buffer_size) {
		write_byte(buffer);
		position = 0;
		buffer = 0;
	}
	
	assert(position < buffer_size);
	buffer <<= 1;
	buffer |= 1;
	++position;
}

void BinaryWriter::immediate_zero()
{
	// Flush buffer when full
	if(position == buffer_size) {
		write_byte(buffer);
		position = 0;
		buffer = 0;
	}
	
	assert(position < buffer_size);
	buffer <<= 1;
	buffer |= 0;
	++position;
}

void BinaryWriter::write_byte(uint8_t byte)
{
	if(byte == 0x00) {
		++zero_bytes;
		return;
	}
	if(byte != 0x00) {
		while(zero_bytes > 0) {
			write_byte2(0x00);
			--zero_bytes;
		}
	}
	write_byte2(byte);
}

void BinaryWriter::write_byte2(uint8_t byte)
{
	if(delay > 0 && byte == 0x80) {
		++delay;
	} else {
		// Flush delay
		if(delay > 0) {
			out << '\x00';
			while(--delay) {
				out << '\x80';
			}
			delay = 0;
		}
		
		// Output byte
		if(byte == 0x00) {
			delay = 1;
		} else {
			out << byte;
		}
	}
}

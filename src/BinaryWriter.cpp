#include "BinaryWriter.h"
#include <cassert>

BinaryWriter::~BinaryWriter()
{
	// Flush carry buffer
	if(carry_buffer != 0) {
		// Write prefix zero
		immediate_zero();
		
		// Write the trailing ones
		while(--carry_buffer) {
			immediate_one();
		}
	}
	
	// Flush buffer
	if(position != 0) {
		buffer <<= (buffer_size - position);
		out << buffer;
	}
	
	/// TODO partial buffer when buffer is more than one byte
}

void BinaryWriter::write_zero()
{
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
	if(carry_buffer == 0) {
		immediate_one();
	} else {
		++carry_buffer;
	}
}

void BinaryWriter::add_carry()
{
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
	assert(position < buffer_size);
	buffer <<= 1;
	buffer |= 1;
	++position;
	
	// Flush buffer when full
	if(position == buffer_size) {
		out << buffer;
		position = 0;
		buffer = 0;
	}
}

void BinaryWriter::immediate_zero()
{
	assert(position < buffer_size);
	buffer <<= 1;
	buffer |= 0;
	++position;
	
	// Flush buffer when full
	if(position == buffer_size) {
		out << buffer;
		position = 0;
		buffer = 0;
	}
}

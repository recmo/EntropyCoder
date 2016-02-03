#include "BinaryWriter.h"
#include <cassert>

/* Bijective encoding to finitely odd numbers:

 ↔ 1
0100 0000 ↔ 01
1100 0000 ↔ 11
0010 0000 ↔ 001
0110 0000 ↔ 011
1010 0000 ↔ 101
1110 0000 ↔ 111
0001 0000 ↔ 0001
          ⋮
1111 1111 ↔ 1111 1111
0000 0000 ↔ 0000 0000 1
1000 0000 ↔ 0000 0001 1
0000 0000 0000 0000 ↔ 0000 0010 1
0000 0000 0000 0000 ↔ 0000 0010 1

*/

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
}

void BinaryWriter::write_zero()
{
	//std::cerr << "ZERO\n";
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
	//std::cerr << "ONE\n";
	if(carry_buffer == 0) {
		immediate_one();
	} else {
		++carry_buffer;
	}
}

void BinaryWriter::add_carry()
{
	// std::cerr << "CARRY\n";
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

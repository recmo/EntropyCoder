#include "BinaryWriter.h"
#include <cassert>
namespace EntropyCoder {

BinaryWriter::BinaryWriter(std::ostream& output) noexcept
: out(output)
{
	// Store the original ostream::exceptions mask
	original_state = out.exceptions();
	
	// ostream::put will set the badbit but not the failbit.
	out.exceptions(std::ios_base::failbit | std::ios_base::badbit);
}

BinaryWriter::~BinaryWriter() noexcept
{
	// Try to finalize if the user hasn't called `finalize()`
	// We do this as a courtesy, but really the user should call `finalize()`
	if(!finalized) {
		finalize();
	}
	
	// Restore the ostream::exceptions mask to the original state
	out.exceptions(original_state);
}

void BinaryWriter::write_zero() throw(already_finalized, io_error)
{
	if(finalized) {
		throw already_finalized("Can not write_zero() after finalize().");
	}
	
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

void BinaryWriter::write_one() throw(already_finalized, io_error)
{
	if(finalized) {
		throw already_finalized("Can not write_one() after finalize().");
	}
	
	if(carry_buffer == 0) {
		immediate_one();
	} else {
		++carry_buffer;
	}
}

void BinaryWriter::add_carry() throw(invalid_carry, already_finalized, io_error)
{
	if(finalized) {
		throw already_finalized("Can not add_carry() after finalize().");
	}
	if(carry_buffer == 0) {
		throw invalid_carry("Nothing to add carry to.");
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

void BinaryWriter::finalize() throw(already_finalized, io_error)
{
	if(finalized) {
		throw already_finalized("Can not finalize() twice.");
	}
	
	// Flush carry buffer
	if(carry_buffer != 0) {
		// Write prefix zero
		immediate_zero();
		
		// Write the trailing ones
		while(--carry_buffer) {
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
	
	// Mark the BinaryWriter as finalized
	finalized = true;
}

void BinaryWriter::immediate_one() throw(io_error)
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

void BinaryWriter::immediate_zero() throw(io_error)
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

void BinaryWriter::write_byte(uint8_t byte) throw(io_error)
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

void BinaryWriter::write_byte2(uint8_t byte) throw(io_error)
{
	if(delay > 0 && byte == 0x80) {
		++delay;
	} else {
		// Flush delay
		if(delay > 0) {
			out.put('\x00');
			while(--delay) {
				out.put('\x80');
			}
			delay = 0;
		}
		
		// Output byte
		if(byte == 0x00) {
			delay = 1;
		} else {
			out.put(byte);
		}
	}
}

} // namespace EntropyCoder

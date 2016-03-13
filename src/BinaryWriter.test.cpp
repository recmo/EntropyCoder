#include "BinaryWriter.h"
#include <UnitTest++.h>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <iomanip>
using std::string;

SUITE(BinaryWriter) {
using EntropyCoder::BinaryWriter;

string write(const string input)
{
	std::ostringstream out;
	CHECK(out.good());
	{
		BinaryWriter bw(out);
		for(const auto& c: input) {
			switch(c) {
			case '0':
				bw.write_zero();
				break;
			case '1':
				bw.write_one();
				break;
			case 'c':
				bw.add_carry();
				break;
			case ' ':
				break;
			default:
				throw std::domain_error("Unknown command in command string.");
			}
		}
	}
	out.flush();
	CHECK(out.good());
	
	// Convert to hex
	std::ostringstream result;
	for(uint8_t byte: out.str()) {
		result << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint64_t>(byte);
	}
	result.flush();
	return result.str();
}

TEST(WithoutCarry)
{
	CHECK_EQUAL("", write(""));
	CHECK_EQUAL("ff", write("1111 1111"));
	CHECK_EQUAL("01", write("0000 0001"));
	CHECK_EQUAL("01ff", write("0000 0001 1111 1111"));
}

TEST(AddZeroPadding)
{
	CHECK_EQUAL("", write(""));
	CHECK_EQUAL("80", write("1"));
	CHECK_EQUAL("fe", write("1111 111"));
	CHECK_EQUAL("ff80", write("1111 1111 1"));
	CHECK_EQUAL("fffe", write("1111 1111 1111 111"));
}

TEST(RemoveTrailingZeros)
{
	CHECK_EQUAL("", write(""));
	CHECK_EQUAL("80", write("1000 0000 0000 00"));
	CHECK_EQUAL("fe", write("1111 1110 00000000"));
	CHECK_EQUAL("ff80", write("1111 1111 1000 0000 0000 000"));
	CHECK_EQUAL("fffe", write("1111 1111 1111 1110 0000"));
}

TEST(ZeroByte)
{
	CHECK_EQUAL("00", write("0000 0000 1"));
	CHECK_EQUAL("0000", write("0000 0000 0000 0000 1"));
	CHECK_EQUAL("1000", write("0000 1111 1111 1111 c 1"));
	CHECK_EQUAL("0080", write("0000 0000 1000 0000 1"));
	CHECK_EQUAL("008080", write("0000 0000 1000 0000 1000 0000 1"));
	CHECK_EQUAL("00808080", write("0000 0000 1000 0000 1000 0000 1000 0000 1"));
	CHECK_EQUAL("00808080f0", write("0000 0000 1000 0000 1000 0000 1000 0000 1111"));
}

TEST(MultiByte)
{
	CHECK_EQUAL("012345", write("0000 0001 0010 0011 0100 0101"));
}

TEST(WithCarry)
{
	CHECK_EQUAL("80", write("0111 1111 c"));
	CHECK_EQUAL("08", write("0000 0111 c"));
	CHECK_EQUAL("58", write("0101 0111 c"));
	CHECK_EQUAL("40", write("0011 1111 1111 1111 c"));
	CHECK_EQUAL("400000", write("0011 1111 1111 1111 1111 1111 c 1"));
	CHECK_EQUAL("ff", write("0c0c0c0c 0c0c0c0c"));
}

TEST(CheckCarryThrow)
{
	CHECK_THROW(write("c"), BinaryWriter::invalid_carry);
	CHECK_THROW(write("1 c"), BinaryWriter::invalid_carry);
	CHECK_THROW(write("1111 1111 1111 1111 c"), BinaryWriter::invalid_carry);
	CHECK_THROW(write("0111 1111 1111 1111 c c"), BinaryWriter::invalid_carry);
}

TEST(Bijective)
{
	const std::uint64_t max = 256 * 256 + 256 + 1;
	std::vector<bool> results(max, false);
	for(std::uint64_t n = 0; n < max; ++n) {
		
		// Write
		std::ostringstream out;
		{
			BinaryWriter bw(out);
			for(std::uint64_t b = 1; b < max; b <<= 1) {
				if(b & n) {
					bw.write_one();
				} else {
					bw.write_zero();
				}
			}
		}
		out.flush();
		const std::string outs = out.str();
		
		CHECK(outs.size() <= 2);
		
		uint index = 0;
		if(outs.size() == 0) {
			index = 0;
		} else if(outs.size() == 1) {
			const std::uint8_t a = outs[0];
			index = a + 1;
		} else {
			const std::uint8_t a = outs[0];
			const std::uint8_t b = outs[1];
			index = a * 256 + b + 256 + 1;
		}
		CHECK(index < results.size());
		CHECK_EQUAL(false, results.at(index));
		results.at(index) = true;
	}
	
	// We must have seen all results
	for(bool result: results) {
		CHECK_EQUAL(true, result);
	}
}

TEST(AlreadyFinalized)
{
	std::ostringstream out;
	{
		BinaryWriter bw(out);
		bw.finalize();
		CHECK_THROW(bw.write_zero(), BinaryWriter::already_finalized);
	}
	{
		BinaryWriter bw(out);
		bw.finalize();
		CHECK_THROW(bw.write_one(), BinaryWriter::already_finalized);
	}
	{
		BinaryWriter bw(out);
		bw.write_zero();
		bw.finalize();
		CHECK_THROW(bw.add_carry(), BinaryWriter::already_finalized);
	}
	{
		BinaryWriter bw(out);
		bw.finalize();
		CHECK_THROW(bw.finalize(), BinaryWriter::already_finalized);
	}
}

}

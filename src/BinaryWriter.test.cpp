#include "BinaryWriter.h"
#include <UnitTest++.h>
#include <sstream>
#include <stdexcept>
#include <iostream>
using std::string;

SUITE(BinaryWriter) {

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
	CHECK_EQUAL("008080", write("0000 0000 1000 0000 1"));
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
	CHECK_THROW(write("c"), std::runtime_error);
	CHECK_THROW(write("1 c"), std::runtime_error);
	CHECK_THROW(write("1111 1111 1111 1111 c"), std::runtime_error);
	CHECK_THROW(write("0111 1111 1111 1111 c c"), std::runtime_error);
}

}

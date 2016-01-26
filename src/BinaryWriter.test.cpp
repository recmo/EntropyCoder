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
	return out.str();
}

TEST(WithoutCarry)
{
	CHECK_EQUAL("", write(""));
	CHECK_EQUAL("\xFF", write("1111 1111"));
	CHECK_EQUAL("\x01", write("0000 0001"));
	CHECK_EQUAL("\x01\xFF", write("0000 0001 1111 1111"));
}

TEST(ZeroPadding)
{
	CHECK_EQUAL("", write(""));
	CHECK_EQUAL("\x80", write("1"));
	CHECK_EQUAL("\xFE", write("1111 111"));
	CHECK_EQUAL("\xFF\x80", write("1111 1111 1"));
	CHECK_EQUAL("\xFF\xFE", write("1111 1111 1111 111"));
	
	const string zero1({0x00});
	const string zero2({0x00, 0x00});
	CHECK_EQUAL(zero1, write("0"));
	CHECK_EQUAL(zero1, write("0000 000"));
	CHECK_EQUAL(zero2, write("0000 0000 0"));
	CHECK_EQUAL(zero2, write("0000 0000 0000 000"));
}

TEST(MultiByte)
{
	CHECK_EQUAL("\x01\x23\x40", write("0000 0001 0010 0011 0100"));
}

TEST(WithCarry)
{
	CHECK_EQUAL("\x80", write("0111 1111 c"));
	CHECK_EQUAL("\x08", write("0000 0111 c"));
	CHECK_EQUAL("\x58", write("0101 0111 c"));
	CHECK_EQUAL(string({0x40, 0x00}), write("0011 1111 1111 1111 c"));
	CHECK_EQUAL("\xFF", write("0c0c0c0c 0c0c0c0c"));
}

}

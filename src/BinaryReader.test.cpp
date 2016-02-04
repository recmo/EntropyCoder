#include "BinaryReader.h"
#include <UnitTest++.h>
#include <sstream>
#include <stdexcept>
#include <iostream>
using std::string;

SUITE(BinaryReader) {

string read(const string input, uint symbols)
{
	string out;
	std::istringstream in(input);
	CHECK(in.good());
	{
		BinaryReader br(in);
		while(!br.eof()) {
			out.append(br.read_bit() ? "1" : "0");
		}
	}
	CHECK(in.eof());
	return out;
}

TEST(Simple)
{
	CHECK_EQUAL("", read("", 0));
	CHECK_EQUAL("1", read("\x80", 0));
	CHECK_EQUAL("01", read("\x40", 0));
	CHECK_EQUAL("11", read("\xC0", 0));
	CHECK_EQUAL("001", read("\x20", 0));
	CHECK_EQUAL("011", read("\x60", 0));
	CHECK_EQUAL("101", read("\xA0", 0));
	CHECK_EQUAL("111", read("\xE0", 0));
	CHECK_EQUAL("0001", read("\x10", 0));
	CHECK_EQUAL("0011", read("\x30", 0));
	CHECK_EQUAL("0101", read("\x50", 0));
	CHECK_EQUAL("0111", read("\x70", 0));
	CHECK_EQUAL("1001", read("\x90", 0));
	CHECK_EQUAL("1011", read("\xB0", 0));
	CHECK_EQUAL("1101", read("\xD0", 0));
	CHECK_EQUAL("1111", read("\xF0", 0));
	CHECK_EQUAL("00001", read("\x08", 0));
	CHECK_EQUAL("00011", read("\x18", 0));
	CHECK_EQUAL("11111111", read("\xFF", 8));
}

TEST(AppendOne)
{
	CHECK_EQUAL("000000001", read(std::string{0x00}, 8));
	CHECK_EQUAL("00100011000000001", read(std::string{0x23, 0x00}, 8));
}

TEST(MultiByte)
{
	CHECK_EQUAL("000000010010001101000001", read("\x01\x23\x41", 24));
}

}

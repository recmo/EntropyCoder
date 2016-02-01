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
		while(symbols--) {
			CHECK(!br.eof());
			out.append(br.read_bit() ? "1" : "0");
		}
		CHECK(br.eof());
	}
	CHECK(in.eof());
	return out;
}

TEST(Simple)
{
	CHECK_EQUAL("", read("", 0));
	CHECK_EQUAL("11111111", read("\xFF", 8));
}

TEST(MultiByte)
{
	CHECK_EQUAL("000000010010001101000000", read("\x01\x23\x40", 24));
}

}

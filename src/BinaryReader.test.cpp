#include "BinaryReader.h"
#include <UnitTest++.h>
#include <sstream>
#include <stdexcept>
#include <iostream>
using std::string;

SUITE(BinaryReader) {
using EntropyCoder::BinaryReader;

string read(const string input)
{
	string out;
	std::istringstream in(input);
	CHECK(in.good());
	{
		BinaryReader br(in);
		while(!br.eof()) {
			out.append(br.read_bit() ? "1" : "0");
		}
		CHECK(br.eof());
		CHECK_THROW(br.read_bit(), BinaryReader::read_beyond_end);
	}
	CHECK(in.eof());
	return out;
}

TEST(Simple)
{
	CHECK_EQUAL("", read(""));
	CHECK_EQUAL("1", read("\x80"));
	CHECK_EQUAL("01", read("\x40"));
	CHECK_EQUAL("11", read("\xC0"));
	CHECK_EQUAL("001", read("\x20"));
	CHECK_EQUAL("011", read("\x60"));
	CHECK_EQUAL("101", read("\xA0"));
	CHECK_EQUAL("111", read("\xE0"));
	CHECK_EQUAL("0001", read("\x10"));
	CHECK_EQUAL("0011", read("\x30"));
	CHECK_EQUAL("0101", read("\x50"));
	CHECK_EQUAL("0111", read("\x70"));
	CHECK_EQUAL("1001", read("\x90"));
	CHECK_EQUAL("1011", read("\xB0"));
	CHECK_EQUAL("1101", read("\xD0"));
	CHECK_EQUAL("1111", read("\xF0"));
	CHECK_EQUAL("00001", read("\x08"));
	CHECK_EQUAL("00011", read("\x18"));
	CHECK_EQUAL("11111111", read("\xFF"));
}

TEST(AppendOne)
{
	const char z = '\x00';
	const char m = '\x80';
	CHECK_EQUAL("000000001", read(std::string{z}));
	CHECK_EQUAL("00100011000000001", read(std::string{'\x23',z}));
	CHECK_EQUAL("00000000000000001", read(std::string{z,z}));
	CHECK_EQUAL("00000000100000001", read(std::string{z,m}));
	CHECK_EQUAL("0000000010000000100000001", read(std::string{z,m,m}));
	CHECK_EQUAL("000000001000000010000000100000001", read(std::string{z,m,m,m}));
}

TEST(MultiByte)
{
	CHECK_EQUAL("000000010010001101000001", read("\x01\x23\x41"));
}

}

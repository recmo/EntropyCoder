#include "Endings.h"
#include <UnitTest++.h>
#include <iostream>
#include <stdexcept>

SUITE(Endings) {

Endings make(Endings end, const std::string& commands)
{
	for(const char c: commands) {
		switch(c) {
		case ' ': break;
		case 'r': end.reserve_current(); break;
		case 'c': end.prune_carry(); break;
		case '1': end.prune_one(); break;
		case '0': end.prune_zero(); break;
		case 'g': end.generate(CodeInterval{}); break;
		default: throw std::logic_error("Unhandled case");
		}
	}
	return end;
}

Endings make(const std::string& commands)
{
	return make(Endings{}, commands);
}

std::vector<bool> bitvec(const std::string& bits)
{
	std::vector<bool> result;
	for(const char c: bits) {
		switch(c) {
		case ' ': break;
		case '0': result.push_back(false);
		case '1': result.push_back(true);
		default: throw std::logic_error("Unhandled case");
		}
	}
	return result;
}

TEST(State1n)
{
	/*
	CHECK_EQUAL(bitvec("0"),   make("").current_ending());
	CHECK_EQUAL(bitvec("01"),  make("rg").current_ending());
	CHECK_EQUAL(bitvec("001"), make("rgrg").current_ending());
	CHECK_EQUAL(bitvec("011"), make("rgrgrg").current_ending());
	CHECK_EQUAL(bitvec("0001"), make("rgrgrgrg").current_ending());
	*/
}

}

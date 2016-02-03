#include "Interval.h"
#include "EntropyWriter.h"
#include "EntropyReader.h"
#include <UnitTest++.h>
#include <sstream>
#include <cassert>
#include <vector>

SUITE(EncodeDecode) {

typedef std::uint64_t uint64;
typedef std::vector<bool> Input;
typedef std::vector<Input> Inputs;

// Generate all the symbols bit strings of length n
Inputs generate(uint length)
{
	Inputs result;
	assert(length < 24);
	const uint64 max = 1UL << length;
	result.reserve(max);
	for(uint64 bits = 0; bits < max; ++bits) {
		Input input;
		input.reserve(length);
		for(uint64 bit = 1; bit < max; bit <<= 1) {
			input.push_back(bits & bit);
		}
		result.push_back(input);
	}
	return result;
}

void test(const Inputs& inputs, double probability)
{
	// Model
	const Interval intervalTrue{probability};
	const Interval intervalFalse{
		intervalTrue.base + intervalTrue.range + 1,
		Interval::max - intervalTrue.range - 1
	};
	
	// Run through data
	for(const Input& input: inputs) {
		
		// Encode
		std::stringstream buffer;
		{
			EntropyWriter ew(buffer);
			for(bool bit: input) {
				ew.write(bit ? intervalTrue : intervalFalse);
			}
		}
		
		// Decode
		buffer.seekg(0);
		{
			EntropyReader er(buffer);
			for(bool bit: input) {
				CHECK_EQUAL(false, er.eof());
				const bool value = intervalTrue.includes(er.read());
				CHECK_EQUAL(bit, value);
				er.read(bit ? intervalTrue : intervalFalse);
			}
			CHECK_EQUAL(true, er.eof());
		}
	}
}

TEST(Empty)
{
	const Inputs inputs = generate(0);
	test(inputs, 0.25);
	test(inputs, 0.5);
	test(inputs, 0.75);
}

TEST(One)
{
	const Inputs inputs = generate(1);
	test(inputs, 0.25);
	test(inputs, 0.5);
	test(inputs, 0.75);
}

TEST(Eight)
{
	const Inputs inputs = generate(8);
	test(inputs, 0.25);
	test(inputs, 0.5);
	test(inputs, 0.75);
}

}

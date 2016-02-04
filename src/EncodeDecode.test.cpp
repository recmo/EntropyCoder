#include "Interval.h"
#include "EntropyWriter.h"
#include "EntropyReader.h"
#include <UnitTest++.h>
#include <sstream>
#include <cassert>
#include <vector>
#include "Test.h"

SUITE(EncodeDecode) {

typedef std::uint64_t uint64;
typedef std::vector<bool> Input;
typedef std::vector<Input> Inputs;

TEST(EncodeDecode)
{
	const Inputs inputs = generate_bitstrings(5);
	std::stringstream buffer;
	for(double p: probabilities) {
		
		// Model
		const Interval intervalTrue{p};
		const Interval intervalFalse{
			intervalTrue.base + intervalTrue.range + 1,
			Interval::max - intervalTrue.range - 1
		};
		
		for(const Input& input: inputs) {
			
			std::cerr << p;
			for(bool bit: input)
				std::cerr << (bit ? " true" : " false");
			std::cerr << std::endl;
			
			double entropy = 0.0;
			
			// Encode
			buffer = std::stringstream{};
			{
				EntropyWriter ew(buffer);
				for(bool bit: input) {
					if(p == 0.0) {
						ew.write(intervalFalse);
					} else if(p == 1.0) {
						ew.write(intervalTrue);
					} else {
						ew.write(bit ? intervalTrue : intervalFalse);
					}
				}
			}
			
			const uint64 size = buffer.str().size();
			std::cerr << "size = " << size;
			std::cerr << " " << entropy;
			std::cerr <<  "\n"
			
			// Decode
			buffer.seekg(0);
			{
				EntropyReader er(buffer);
				for(bool bit: input) {
					CHECK_EQUAL(false, er.eof());
					const bool value = intervalTrue.includes(er.read());
					if(p == 0.0) {
						CHECK_EQUAL(false, value);
					} else if(p == 1.0) {
						CHECK_EQUAL(true, value);
					} else {
						CHECK_EQUAL(bit, value);
					}
					er.read(bit ? intervalTrue : intervalFalse);
				}
				CHECK_EQUAL(true, er.eof());
			}
		}
	}
}

}

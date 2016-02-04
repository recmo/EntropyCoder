#include "Interval.h"
#include "EntropyWriter.h"
#include "EntropyReader.h"
#include <UnitTest++.h>
#include <sstream>
#include <cassert>
#include <cmath>
#include <vector>
#include "Test.h"

SUITE(EncodeDecode) {

typedef std::uint64_t uint64;
typedef std::vector<bool> Input;
typedef std::vector<Input> Inputs;

TEST(EncodeDecode)
{
	// return;
	const Inputs inputs = generate_bitstrings(10);
	std::stringstream buffer;
	for(double p: probabilities) {
		
		// Model
		const Interval intervalTrue{p};
		const Interval intervalFalse{
			intervalTrue.base + intervalTrue.range + 1,
			Interval::max - intervalTrue.range - 1
		};
		
		// Entropy
		const double trueBits = -std::log2(p);
		const double falseBits = -std::log2(1.0-p);
		double totalBits = 0.0;
		
		for(const Input& input: inputs) {
			
			// Calculate the exact overhead. This is due to having to encode the
			// ending (which contains information on the number of symbols).
			// TODO: The real upper bound is tighter because length information is
			// also provided by bits in the stream.
			const double sizeOverhead = std::log2(inputs.size());
			
			if(false) {
				std::cerr << p;
				for(bool bit: input)
					std::cerr << (bit ? " true" : " false");
				std::cerr << std::endl;
			}
			
			// Encode
			buffer = std::stringstream{};
			totalBits = 0.0;
			{
				EntropyWriter ew(buffer);
				for(bool bit: input) {
					if(p == 0.0) {
						ew.write(intervalFalse);
					} else if(p == 1.0) {
						ew.write(intervalTrue);
					} else {
						ew.write(bit ? intervalTrue : intervalFalse);
						totalBits += bit ? trueBits : falseBits;
					}
				}
			}
			
			// Check size
			const uint64 size = buffer.str().size();
			const uint maxBytes = ceil((totalBits + sizeOverhead) / 8.0);
			//std::cerr << "size = " << size;
			//std::cerr << " " << totalBits;
			//std::cerr << " " << maxBytes;
			//std::cerr << "\n";
			CHECK(size <= maxBytes);
			
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

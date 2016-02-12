#include "Interval.h"
#include "EntropyWriter.h"
#include "EntropyReader.h"
#include <UnitTest++.h>
#include <sstream>
#include <cassert>
#include <cmath>
#include <vector>

SUITE(BinaryTranscode) {

const std::vector<double> probabilities {
	// Eights
	0.125, 0.25, 0.375, 0.5, 0.625, 0.75, 0.875,
	
	// Small decimal
	1.0e-1, 1.0e-2, 1.0e-3, 1.0e-4, 1.0e-5,
	1.0e-6, 1.0e-7, 1.0e-8, 1.0e-9, 1.0e-10,
	1.0e-11, 1.0e-12, 1.0e-13, 1.0e-14, 1.0e-15,
	
	// Small binary
	std::exp2(-4), std::exp2(-8), std::exp2(-16),
	std::exp2(-24), std::exp2(-32), std::exp2(-40),
	std::exp2(-48), std::exp2(-56), std::exp2(-60),
	std::exp2(-63),
	
	// Underflow
	std::numeric_limits<double>().min(),
	
	// Large decimal
	0.9, 0.99, 0.999, 0.9999, 0.9999, 0.99999, 0.999999,
	1.0 - 1.0e-15,
	
	// Large binary
	1.0 - std::exp2(-4), 1.0 - std::exp2(-8), 1.0 - std::exp2(-16),
	1.0 - std::exp2(-24), 1.0 - std::exp2(-32), 1.0 - std::exp2(-40),
	1.0 - std::exp2(-63),
};

typedef std::uint64_t uint64;
typedef std::vector<bool> Input;
typedef std::vector<Input> Inputs;

Inputs generate_bitstrings(uint length)
{
	Inputs result;
	assert(length < 24);
	for(uint l = 0; l <= length; ++l) {
		const uint64 max = 1UL << l;
		for(uint64 bits = 0; bits < max; ++bits) {
			Input input;
			input.reserve(l);
			for(uint64 bit = 1; bit < max; bit <<= 1) {
				input.push_back(bits & bit);
			}
			result.push_back(input);
		}
	}
	return result;
}

std::vector<std::string> generate_bytestrings(uint length)
{
	std::vector<std::string> result;
	assert(length < 4);
	for(uint l = 0; l <= length; ++l) {
		const uint64 max = 1UL << (l * 8);
		for(uint64 bits = 0; bits < max; ++bits) {
			std::string string;
			for(uint64 byten = 0; byten < l; ++byten) {
				const uint8_t byte = (bits >> (byten * 8)) & 0xff;
				string.push_back(byte);
			}
			result.push_back(string);
		}
	}
	return result;
}

template<class T>
T ipow(T base, unsigned int exp)
{
	T result = 1;
	while (exp) {
		if (exp & 1)
			result *= base;
		exp >>= 1;
		base *= base;
	}
	return result;
}

std::vector<std::string> generate_simple_bytestrings(uint length)
{
	const std::vector<std::uint8_t> bytes{0x00, 0x80, 0xFF};
	
	assert(length < 4);
	std::vector<std::string> results;
	results.reserve(ipow(bytes.size(), length + 1) - 1);
	results.push_back(std::string{});
	
	for(uint l = 1; l <= length; ++l) {
		const uint max = ipow(bytes.size(), l);
		for(uint c = 0; c < max; ++c) {
			uint k = c;
			std::string result;
			result.reserve(l);
			for(uint i = 0; i < l; ++i) {
				result.push_back(bytes[k % bytes.size()]);
				k /= bytes.size();
			}
			results.push_back(result);
		}
	}
	return results;
}

std::string hex(const std::string& bytes)
{
	std::ostringstream out;
	for(uint8_t byte: bytes) {
		out << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint64>(byte);
	}
	return out.str();
}

TEST(BinaryEncodeDecode)
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
			buffer.str(std::string());
			buffer.clear();
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

TEST(BinaryDecodeEncode)
{
	// return;
	const std::vector<std::string> byte_strings = generate_simple_bytestrings(2);
	for(double p: probabilities) {
		
		// Model
		const Interval intervalTrue{p};
		const Interval intervalFalse{
			intervalTrue.base + intervalTrue.range + 1,
			Interval::max - intervalTrue.range - 1
		};
		
		std::vector<bool> bits;
		
		for(const std::string& input: byte_strings) {
			
			if(false) {
				std::cerr << p << " ";
				for(uint8_t byte: input)
					std::cerr << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint64>(byte);
				std::cerr << "\n";
			}
			
			// Decode
			std::istringstream buffer{input};
			bits.clear();
			{
				EntropyReader er(buffer);
				while(!er.eof()) {
					const bool bit = intervalTrue.includes(er.read());
					er.read(bit ? intervalTrue : intervalFalse);
					bits.push_back(bit);
				}
			}
			
			// Encode
			std::ostringstream out;
			{
				EntropyWriter ew(out);
				for(bool bit: bits) {
					ew.write(bit ? intervalTrue : intervalFalse);
				}
			}
			
			// CHECK_EQUAL(hex(input), hex(out.str()));
			CHECK_EQUAL(input, out.str());
		}
	}
}

}

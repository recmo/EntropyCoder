#include "Test.h"
#include <limits>

const std::vector<double> probabilities{
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
	
	// Exceptional
	//0.0, 1.0
};

typedef std::uint64_t uint64;
typedef std::vector<bool> Input;
typedef std::vector<Input> Inputs;

std::vector<std::vector<bool>> generate_bitstrings(uint length)
{
	Inputs result;
	assert(length < 24);
	for(uint l = 0; l < length; ++l) {
		const uint64 max = 1UL << l;
		result.reserve(max);
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

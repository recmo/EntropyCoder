#include <EntropyWriter.h>
#include <EntropyReader.h>
#include <array>
#include <cstdint>
#include <algorithm>
#include <cassert>
#include <cmath>
using EntropyCoder::EntropyReader;
using EntropyCoder::EntropyWriter;
constexpr std::uint64_t max = std::numeric_limits<std::uint64_t>().max();

// Model tuning parameters
const double initial = 0.05;
const double decay = 0.999;

std::array<double, 256> frequencies;
std::array<std::uint64_t, 257> ranges;

void compute_ranges()
{
	// Compute the scale factor
	double scale = 0.0;
	for(double f: frequencies) {
		scale += f;
	}
	scale = std::exp2(64) / scale;
	
	double cumulative = frequencies[0];
	ranges[0] = 0;
	for(uint i = 1; i < 256; ++i) {
		ranges[i] = std::max<std::uint64_t>(ranges[i - 1] + 4, cumulative * scale);
		
		// TODO handle underflow on last range
		
		cumulative += frequencies[i];
	}
	
	// The end is set to zero so that the -1 will make it seem like it was 2⁶⁴
	ranges[256] = 0;
}

void initialize()
{
	for(double& f: frequencies) {
		f = initial;
	}
	compute_ranges();
}

void update_frequencies(std::uint8_t byte)
{
	// Decay all frequencies
	for(double& f: frequencies) {
		f *= decay;
	}
	
	// Add the byte count
	frequencies[byte] += 1.0;
	
	// Recompute the ranges
	compute_ranges();
}

std::uint8_t lookup(std::uint64_t value)
{
	for(uint i = 0; i < 256; ++i) {
		if(ranges[i] <= value && value <= ranges[i+1] - 1)
			return i;
	}
	assert(false);
}

void dump()
{
	const std::string grayscale = " .'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
	std::cerr << "\n";
	double max = 0.0;
	for(double f: frequencies) {
		max = std::max(f, max);
	}
	const double scale = (grayscale.size() - 1) / max;
	static bool printed = false;
	if(printed) {
		std::cerr << "\033[19A";
	}
	std::cerr << "+--------------------------------------------------+\n";
	for(uint h = 0 ; h < 256; h += 16) {
		std::cerr << "|";
		for(uint l = 0 ; l < 16; l += 1) {
			std::cerr << "  " << grayscale.at(scale * frequencies[h + l]);
		}
		std::cerr << "  |\n";
	}
	std::cerr << "+--------------------------------------------------+\n";
	printed = true;
}

void encode()
{
	initialize();
	EntropyWriter ew(std::cout);
	uint c = 0;
	for(auto result = std::cin.get(); result != std::istream::traits_type::eof(); result = std::cin.get()) {
		std::uint8_t byte = result;
		ew.write(ranges[byte], ranges[byte + 1] - 1);
		update_frequencies(byte);
		if(++c % 10000 == 0) {
			dump();
		}
	}
}

void decode()
{
	initialize();
	EntropyReader ew(std::cin);
	while(!ew.eof()) {
		const std::uint8_t byte = lookup(ew.value());
		std::cout.put(byte);
		ew.next(ranges[byte], ranges[byte + 1] - 1);
		update_frequencies(byte);
	}
}

int main(int argc, const char* argv[])
{
	try {
		// Parse the arguments
		const std::vector<std::string> args{argv, argv + argc};
		
		// Execute the command
		if(args.at(1) == std::string{"encode"}) {
			encode();
		}
		if(args.at(1) == std::string{"decode"}) {
			decode();
		}
	}
	catch(const std::exception& e) {
		std::cerr << typeid(e).name() << ": " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	catch(...) {
		std::cerr << "Unknown exception happened." << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

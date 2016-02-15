#include <EntropyWriter.h>
#include <EntropyReader.h>
#include <cstdint>
#include <cmath>
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <sstream>
#include <stdexcept>
#include <vector>
using EntropyCoder::EntropyReader;
using EntropyCoder::EntropyWriter;

template<class T, class A>
std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& v)
{
	typename std::vector<T,A>::const_iterator it = v.begin();
	out << "[";
	if(it != v.end()) for(;;) {
		out << *it;
		if(++it == v.end())
			break;
		out << ", ";
	}
	out << "]";
	return out;
}

class parse_error: public std::invalid_argument {
public:
	parse_error()
	: std::invalid_argument("Could not parse argument.") { }
};

template<class T>
T parse(const std::string &string)
{
	std::istringstream ss(string);
	T value;
	ss >> value;
	if(!ss.eof()) {
		throw parse_error{};
	}
	return value;
}

template<class T>
T read_arg(const std::vector<std::string>& args, const std::string& arg, const T& def)
{
	auto p = std::find(args.begin(), args.end(), arg);
	if(p != args.end()) {
		if(++p != args.end()) {
			return parse<T>(*p);
		} else {
			throw std::runtime_error(arg + "requires an argument");
		}
	} else {
		return def;
	}
}

constexpr std::uint64_t max = std::numeric_limits<std::uint64_t>().max();

std::uint64_t split(double probability)
{
	assert(probability >= 0.0);
	assert(probability <= 1.0);
	if(probability == 0.0) {
		return 0;
	}
	if(probability == 1.0) {
		return max;
	}
	std::uint64_t result = probability * std::exp2(64);
	return std::max(result, 3UL);
}

void generate(uint n, double probability, const std::string& positive, const std::string& negative)
{
	const std::string pos = positive + "\n";
	const std::string neg = negative + "\n";
	std::random_device rd;
	std::discrete_distribution<> d({probability, 1.0 - probability});
	for(uint i = 0; i < n; ++i) {
		std::cout << (d(rd) == 0 ? pos : neg);
	}
}

void encode(double probability, const std::string& pos, const std::string& neg)
{
	const std::uint64_t s = split(probability);
	
	EntropyWriter ew(std::cout);
	while(std::cin.good() && !std::cin.eof()) {
		std::string in;
		std::cin >> in;
		if(in.empty()) {
			continue;
		}
		assert(in == pos || in == neg);
		const bool x = in == pos;
		if(x) {
			ew.write(0, s - 1);
		} else {
			ew.write(s, max);
		}
	}
}

void decode(double probability, const std::string& positive, const std::string& negative)
{
	const std::uint64_t s = split(probability);
	const std::string pos = positive + "\n";
	const std::string neg = negative + "\n";
	
	EntropyReader ew(std::cin);
	while(!ew.eof()) {
		if(ew.value() < s) {
			ew.next(0, s - 1);
			std::cout << pos;
		} else {
			ew.next(s, max);
			std::cout << neg;
		}
	}
}

int main(int argc, const char* argv[])
{
	try {
		// Parse the arguments
		const std::vector<std::string> args{argv, argv + argc};
		const double probability = read_arg<double>(args, "-p", 0.5);
		const auto positive = read_arg<std::string>(args, "-t", "true");
		const auto negative = read_arg<std::string>(args, "-f", "false");
		assert(probability >= 0.0);
		assert(probability <= 1.0);
		
		// Execute the command
		if(args.at(1) == std::string{"generate"}) {
			uint n = parse<uint>(args.at(2));
			generate(n, probability, positive, negative);
		}
		if(args.at(1) == std::string{"encode"}) {
			encode(probability, positive, negative);
		}
		if(args.at(1) == std::string{"decode"}) {
			decode(probability, positive, negative);
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

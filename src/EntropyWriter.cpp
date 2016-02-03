#include "EntropyWriter.h"
#include <cassert>
#include <iostream>
#include <iomanip>
#include <algorithm>

typedef std::uint64_t uint64;

std::ostream& operator<<(std::ostream& out, const std::vector<bool>& end)
{
	uint n = 0;
	for(auto bit: end) {
		out << (bit ? "1" : "0");
		if(n == 0) {
			out << ".";
		}
		++n;
	}
	return out;
}

std::ostream& operator<<(std::ostream& out, const std::vector<std::vector<bool>>& endings)
{
	out << "{";
	uint n = 0;
	for(auto ending: endings) {
		out << ending;
		++n;
		if(n < endings.size()) {
			out << ", ";
		}
	}
	out << "}";
	return out;
}

EntropyWriter::~EntropyWriter()
{
	// Write the current ending
	assert(ending.size() >= 1);
	
	// Write the carry
	if(ending[0]) {
		bw.add_carry();
	}
	ending.erase(ending.begin());
	
	// Write the bits
	for(bool bit: ending) {
		if(bit) {
			bw.write_one();
		} else {
			bw.write_zero();
		}
	}
}

void EntropyWriter::write(const Interval& symbol)
{
	// We didn't use the ending, so we should reserve it.
	reserved_endings.push_back(ending);
	// std::cerr << ending << "\t" << reserved_endings << "\n";
	
	// Update interval
	bool carry;
	current.update(symbol, &carry);
	
	// Apply carry
	if(carry) {
		bw.add_carry();
		remove_endings_without_carry();
	}
	
	// Remove endings that are now out of the interval
	remove_endings_out_of_interval();
	// std::cerr << "remaining " << reserved_endings << "\n";
	
	// Shift out bits
	for(bool bit: current.normalize()) {
		if(bit) {
			bw.write_one();
			shift_endings_with_one();
		} else {
			bw.write_zero();
			remove_endings_with_carry();
			shift_endings_with_zero();
		}
	}
	
	ending = next_available_ending();
	
	std::cerr << symbol << " " << current << " ";
	std::cerr << ending << "\t" << reserved_endings << "\n";
}

void EntropyWriter::remove_endings_out_of_interval()
{
	reserved_endings.erase(std::remove_if(reserved_endings.begin(),
		reserved_endings.end(), [&](const std::vector<bool>& ending) {
			return !is_valid(ending);
		}
	), reserved_endings.end());
}

void EntropyWriter::remove_endings_with_carry()
{
	reserved_endings.erase(std::remove_if(reserved_endings.begin(),
		reserved_endings.end(), [](const std::vector<bool>& ending) {
			return ending.at(0) == true;
		}
	), reserved_endings.end());
}

void EntropyWriter::remove_endings_without_carry()
{
	reserved_endings.erase(std::remove_if(reserved_endings.begin(),
		reserved_endings.end(), [](const std::vector<bool>& ending) {
			return ending.at(0) == false;
		}
	), reserved_endings.end());
	
	// Remove carry flag from remaining endings
	for(auto& ending: reserved_endings) {
		ending.at(0) = false;
	}
}

void EntropyWriter::shift_endings_with_one()
{
	// Shift endings to the left. We shifted out a one, so can add carry
	std::vector<std::vector<bool>> new_reserved_endings;
	for(std::vector<bool> ending: reserved_endings) {
		ending.erase(ending.begin());
		if(ending.empty()) {
			continue;
		}
		if(std::find(new_reserved_endings.begin(), new_reserved_endings.end(), ending) == new_reserved_endings.end()) {
			new_reserved_endings.push_back(ending);
		}
	}
	reserved_endings = new_reserved_endings;
}

void EntropyWriter::shift_endings_with_zero()
{
	// Shift endings to the left. We shifted out a one, so can add carry
	std::vector<std::vector<bool>> new_reserved_endings;
	for(std::vector<bool> ending: reserved_endings) {
		ending.erase(ending.begin());
		if(ending.empty()) {
			continue;
		}
		if(std::find(new_reserved_endings.begin(), new_reserved_endings.end(), ending) == new_reserved_endings.end()) {
			new_reserved_endings.push_back(ending);
		}
	}
	reserved_endings = new_reserved_endings;
}

bool print = false;

bool EntropyWriter::is_valid(const std::vector<bool>& ending) const
{
	assert(ending.size() >= 1);
	uint64 end = 0UL;
	uint64 bit = Interval::msb;
	for(auto i = ending.begin() + 1; i != ending.end(); ++i) {
		end |= *i ? bit : 0;
		bit >>= 1;
	}
	if(print) {
		std::cerr << " = ";
		std::cerr << "0x" << std::setw(16) << std::setfill('0') << std::hex;
		std::cerr  << end;
	}
	if(ending[0] == true) {
		return current.is_goofy() && end < (current.base + current.range + 1);
	} else {
		if(current.is_goofy()) {
			return end >= current.base;
		} else {
			return current.includes(end);
		}
	}
}

bool EntropyWriter::is_reserved(const std::vector<bool>& ending) const
{
	return std::find(reserved_endings.begin(), reserved_endings.end(), ending) != reserved_endings.end();
}

std::vector<bool> EntropyWriter::next_available_ending() const
{
	// Try (1., 0.), 0.1, 1.1, 0.01, 0.11, 1.01, 1.11, 0.001, …
	//
	// Whether we start with 1. or 0. depends on the written sequence:
	//
	//    …0111 + 0. ↦ …0111     …1000 + 0. ↦ …1000
	//    …0111 + 1. ↦ …1000     …1000 + 1. ↦ …1001
	//
	// So we want to try 1. before 0. iff the last written digits is a one.
	//
	// std::cerr << "Looking for ending, reserved: " << reserved_endings << "\n";
	std::vector<bool> ending{bw.ends_in_one()};
	std::cerr << "Next: " << ending;
	print = true;
	std::cerr << (!is_valid(ending) ? " invalid" : "");
	std::cerr << (is_reserved(ending) ? " reserved" : "");
	print = false;
	if(!is_reserved(ending) && is_valid(ending)) {
		std::cerr << " available!\n";
		return ending;
	}
	std::cerr << "\n";
	
	ending[0] = !ending[0];
	std::cerr << "Next: " << ending;
	print = true;
	std::cerr << (!is_valid(ending) ? " invalid" : "");
	std::cerr << (is_reserved(ending) ? " reserved" : "");
	print = false;
	if(!is_reserved(ending) && is_valid(ending)) {
		std::cerr << " available!\n";
		return ending;
	}
	std::cerr << "\n";
	
	ending = std::vector<bool>{false, true};
	std::cerr << "Next: " << ending;
	print = true;
	std::cerr << (!is_valid(ending) ? " invalid" : "");
	std::cerr << (is_reserved(ending) ? " reserved" : "");
	print = false;
	while(is_reserved(ending) || !is_valid(ending)) {
		std::cerr << "\n";
		bool carry = true;
		int n = ending.size() - 2;
		while(carry && n >= 0) {
			carry = ending.at(n);
			ending.at(n) = !ending.at(n);
			--n;
		}
		if(carry) {
			for(auto bit: ending) {
				bit = false;
			}
			ending.push_back(true);
		}
		std::cerr << "Next: " << ending;
		print = true;
		std::cerr << (!is_valid(ending) ? " invalid" : "");
		std::cerr << (is_reserved(ending) ? " reserved" : "");
		print = false;
	}
	std::cerr << " available!\n";
	return ending;
}

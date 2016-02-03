#pragma once
#include <cstdint>
#include "BinaryWriter.h"
#include "Interval.h"
#include <set>
#include <vector>

class EntropyWriter {
public:
	EntropyWriter(std::ostream& output): bw(output) { }
	~EntropyWriter();
	
	void write(const Interval& symbol);
	
private:
	BinaryWriter bw;
	Interval current;
	std::vector<std::vector<bool>> reserved_endings{};
	std::vector<bool> ending{false};
	
	void remove_endings_out_of_interval();
	void remove_endings_with_carry();
	void remove_endings_without_carry();
	void shift_endings_with_zero();
	void shift_endings_with_one();
	bool is_valid(const std::vector<bool>& ending) const;
	bool is_reserved(const std::vector<bool>& ending) const;
	std::vector<bool> next_available_ending() const;
};

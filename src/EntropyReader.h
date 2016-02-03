#pragma once
#include <cstdint>
#include "BinaryReader.h"
#include "Interval.h"
#include "Ending.h"

class EntropyReader {
public:
	typedef Interval::uint64 uint64;
	EntropyReader(std::istream& input);
	
	bool eof() const;
	uint64 read() const;
	void read(const Interval& symbol);
	
private:
	BinaryReader br;
	Interval current;
	Interval::uint64 value;
	Ending ending;
};

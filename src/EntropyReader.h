#pragma once
#include <cstdint>
#include "BinaryReader.h"
#include "Interval.h"

class EntropyReader {
public:
	typedef Interval::uint64 uint64;
	EntropyReader(std::istream& input);
	
	bool eof() const;
	uint64 read() const { return current.base; }
	void read(const Interval& symbol);
	
private:
	BinaryReader br;
	Interval current;
	uint eof_count = 0;
};

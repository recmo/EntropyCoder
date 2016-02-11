#pragma once
#include <cstdint>
#include "BinaryReader.h"
#include "CodeInterval.h"
#include "Endings.h"
#include "End.h"

class EntropyReader {
public:
	typedef Interval::uint64 uint64;
	EntropyReader(std::istream& input);
	
	bool eof() const;
	uint64 read() const;
	void read(const Interval& symbol);
	
private:
	BinaryReader br;
	CodeInterval current;
	Interval::uint64 value;
	Endings endings;
	End end;
	uint64 past_end = 0;
};

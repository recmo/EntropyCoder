#pragma once
#include <cstdint>
#include "BinaryReader.h"
#include "CodeInterval.h"
#include "End.h"
namespace EntropyCoder {

class EXPORT EntropyReader {
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
	End end;
	uint64 past_end = 0;
};

} // namespace EntropyCoder

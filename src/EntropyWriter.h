#pragma once
#include <cstdint>
#include "BinaryWriter.h"
#include "CodeInterval.h"
#include "End.h"
namespace EntropyCoder {

class EXPORT EntropyWriter {
public:
	EntropyWriter(std::ostream& output): bw(output) { }
	~EntropyWriter();
	
	void write(const Interval& symbol);
	
private:
	BinaryWriter bw;
	CodeInterval current;
	End end;
};

} // namespace EntropyCoder

#pragma once
#include <cstdint>
#include "BinaryWriter.h"
#include "Interval.h"

class EntropyWriter {
public:
	EntropyWriter(std::ostream& output): bw(output) { }
	~EntropyWriter();
	
	void write(const Interval& symbol);
	
private:
	BinaryWriter bw;
	Interval current;
};

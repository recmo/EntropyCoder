#pragma once
#include <cstdint>
#include "BinaryWriter.h"
#include "CodeInterval.h"
#include "Endings.h"

class EntropyWriter {
public:
	EntropyWriter(std::ostream& output): bw(output) { }
	~EntropyWriter();
	
	void write(const Interval& symbol);
	
private:
	BinaryWriter bw;
	CodeInterval current;
	Endings endings;
};

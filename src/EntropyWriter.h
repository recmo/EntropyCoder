#pragma once
#include <iostream>
#include <memory>
#include "Interval.h"
namespace EntropyCoder {

class EXPORT EntropyWriter {
public:
	EntropyWriter(std::ostream& output);
	~EntropyWriter();
	
	void write(const Interval& symbol);
	
private:
	class Implementation;
	std::unique_ptr<Implementation> impl;
};

} // namespace EntropyCoder

#pragma once
#include <iostream>
#include <memory>
#include "Interval.h"
namespace EntropyCoder {

class EXPORT EntropyReader {
public:
	EntropyReader(std::istream& input);
	~EntropyReader();
	
	bool eof() const;
	std::uint64_t read() const;
	void read(const Interval& symbol);
	
private:
	class Implementation;
	std::unique_ptr<Implementation> impl;
};

} // namespace EntropyCoder

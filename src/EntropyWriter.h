#pragma once
#include <iostream>
#include <memory>
#include <cstdint>
namespace EntropyCoder {

class VISIBLE EntropyWriter {
public:
	EntropyWriter(std::ostream& output);
	~EntropyWriter();
	
	void write(const std::uint64_t start, const std::uint64_t end);
	
private:
	class HIDDEN Implementation;
	std::unique_ptr<Implementation> impl;
};

} // namespace EntropyCoder

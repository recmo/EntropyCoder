#pragma once
#include <iostream>
#include <memory>
#include <cstdint>
namespace EntropyCoder {

class VISIBLE EntropyReader {
public:
	EntropyReader(std::istream& input);
	~EntropyReader();
	
	bool eof() const;
	std::uint64_t value() const;
	void next(const std::uint64_t start, const std::uint64_t end);
	
private:
	class HIDDEN Implementation;
	std::unique_ptr<Implementation> impl;
};

} // namespace EntropyCoder

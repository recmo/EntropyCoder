#pragma once
#include <iostream>
#include <memory>
#include <cstdint>
namespace EntropyCoder {

class VISIBLE EntropyReader {
public:
	typedef std::ios_base::failure io_error;
	
	EntropyReader(std::istream& input) throw(std::bad_alloc, io_error);
	~EntropyReader() noexcept;
	
	bool eof() const noexcept;
	std::uint64_t value() const noexcept;
	void next(const std::uint64_t start, const std::uint64_t end) throw(io_error);
	
private:
	class HIDDEN Implementation;
	std::unique_ptr<Implementation> impl;
};

} // namespace EntropyCoder

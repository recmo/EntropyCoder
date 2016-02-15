#pragma once
#include <iostream>
#include <memory>
#include <cstdint>
namespace EntropyCoder {

class VISIBLE EntropyWriter {
public:
	typedef std::ios_base::failure io_error;
	
	EntropyWriter(std::ostream& output) throw(std::bad_alloc);
	~EntropyWriter() noexcept;
	
	void write(const std::uint64_t start, const std::uint64_t end) throw(io_error);
	
	void finalize() throw(io_error);
	
private:
	class HIDDEN Implementation;
	std::unique_ptr<Implementation> impl;
};

} // namespace EntropyCoder

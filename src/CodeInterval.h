#pragma once
#include "Interval.h"
#include <tuple>
#include <stdexcept>
namespace EntropyCoder {

class CodeInterval: public Interval {
public:
	class not_normalized: public std::logic_error {
	public:
		explicit not_normalized(const std::string& what)
		: logic_error(what) { }
	};
	class invalid_symbol: public std::logic_error {
	public:
		explicit invalid_symbol(const std::string& what)
		: logic_error(what) { }
	};
	typedef std::range_error range_error;
	
	CodeInterval() noexcept: Interval{} { }
	CodeInterval(const Interval& interval) noexcept: Interval{interval} { }
	
	bool is_normalized() const noexcept;
	bool wraps() const noexcept;
	uint64 descale(uint64 value) const throw(not_normalized, range_error);
	
	/// @returns true on carry
	bool update(const Interval& symbol) throw(not_normalized, invalid_symbol);
	
	/// @returns (b, n) where b has n bits of output in its most significant bits
	std::pair<uint64, int> normalize() noexcept;
};

} // namespace EntropyCoder

#pragma once
#include <cstdint>
#include <ostream>
#include <stdexcept>
namespace EntropyCoder {
class CodeInterval;

class End
{
public:
	class invalid_state: public std::logic_error {
	public:
		explicit invalid_state(const std::string& what)
		: logic_error(what) { }
	};
	class carry_inconsistent: public std::logic_error {
	public:
		explicit carry_inconsistent(const std::string& what)
		: logic_error(what) { }
	};
	class invalid_carry: public std::logic_error {
	public:
		explicit invalid_carry(const std::string& what)
		: logic_error(what) { }
	};
	typedef std::overflow_error overflow_error;
	
	enum State {
		sS = 0, // S
		s0 = 1, // 0
		s1 = 2  // 1
	};
	static constexpr std::uint64_t msb = 1UL << 63;
	std::uint64_t ending = 0;
	
	explicit End() noexcept: End{sS} {}
	explicit End(State state) throw(invalid_state);
	explicit End(State state, std::uint64_t ending) throw(invalid_state);
	
	bool operator==(const End& other) const noexcept;
	bool operator!=(const End& other) const noexcept { return !(*this == other); }
	
	bool carry_bit() const noexcept { return ending & msb; }
	std::uint64_t value_bits() const noexcept { return ending << 1; }
	
	void first(const CodeInterval& interval) throw(carry_inconsistent, overflow_error);
	void next() throw(overflow_error);
	void carry() throw(invalid_carry);
	void one() noexcept;
	void zero() noexcept;
	
private:
	friend std::ostream& operator<<(std::ostream& out, const End& end);
	State state = sS;
	bool is_valid(const CodeInterval& interval) noexcept;
};

std::ostream& operator<<(std::ostream& out, const End& end);

} // namespace EntropyCoder

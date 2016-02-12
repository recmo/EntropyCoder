#pragma once
#include <cstdint>
#include <ostream>
namespace EntropyCoder {
class CodeInterval;

class End
{
public:
	enum State {
		sS = 0, // S
		s0 = 1, // 0
		s1 = 2  // 1
	};
	static constexpr std::uint64_t msb = 1UL << 63;
	std::uint64_t ending = 0;
	
	explicit End(): End{sS} {}
	explicit End(State state);
	explicit End(State state, std::uint64_t ending);
	
	bool operator==(const End& other) const;
	bool operator!=(const End& other) const { return !(*this == other); }
	
	bool carry_bit() const { return ending & msb; }
	std::uint64_t value_bits() const { return ending << 1; }
	
	void first(const CodeInterval& interval);
	void next();
	void carry();
	void one();
	void zero();
	
private:
	friend std::ostream& operator<<(std::ostream& out, const End& end);
	State state = sS;
	bool is_valid(const CodeInterval& interval);
};

std::ostream& operator<<(std::ostream& out, const End& end);

} // namespace EntropyCoder

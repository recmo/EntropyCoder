#pragma once
#include <cstdint>
#include <ostream>
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
	
	End();
	explicit End(State state);
	End(State state, std::uint64_t ending);
	
	bool operator==(const End& other) const;
	bool operator!=(const End& other) const { return !(*this == other); }
	
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

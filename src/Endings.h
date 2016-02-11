#pragma once
#include "CodeInterval.h"
#include <vector>

class Endings {
public:
	typedef Interval::uint64 uint64;
	
	uint64 end(bool* const carry = nullptr) const;
	
	void reserve_current();
	void prune(const CodeInterval& interval);
	void prune_carry();
	void prune_one();
	void prune_zero();
	void generate(const CodeInterval& interval);
	
	std::vector<bool> current_ending() { return ending; }
	
	void experiment();
	
private:
	enum State {
		s0p = 0, // 0⁺
		s1p = 1, // 1⁺
		s1n = 2  // 1⁻
	};
	typedef std::vector<bool> End;
	typedef std::vector<End> Set;
	State state = s1n;
	Set reserved_endings{};
	End ending{false};
	
	bool is_valid(const CodeInterval& interval, const End& ending);
	bool is_reserved(const End& ending);
	
	End first();
	End next(End current);
	End next(End current, int msb);
	
	End first(const CodeInterval& interval);
	End next(const CodeInterval& interval, End current);
	
	uint current_set_number(const CodeInterval& interval);
	Set gen_current_set(const CodeInterval& interval, uint n);
	
	uint current_set_number();
	Set gen_current_set(uint n);
};

std::ostream& operator<<(std::ostream& out, const std::vector<bool>& end);

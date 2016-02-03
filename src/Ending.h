#include "Interval.h"
#include <vector>

class Ending {
public:
	typedef Interval::uint64 uint64;
	
	uint64 end(bool* const carry  = nullptr);
	
	void reserve_current();
	void prune(const Interval& interval);
	void prune_carry();
	void prune_one();
	void prune_zero();
	
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
	
	void remove_endings_out_of_interval();
	void remove_endings_without_carry();
	void shift_endings_with_zero();
	void shift_endings_with_one();
	bool is_valid(const End& ending);
	bool is_reserved(const End& ending);
	End next_available_ending();
};
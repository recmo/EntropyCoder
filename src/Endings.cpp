#include "Endings.h"
#include <stdexcept>

constexpr bool print = false;

std::ostream& operator<<(std::ostream& out, const std::vector<bool>& end)
{
	uint n = 0;
	for(auto bit: end) {
		out << (bit ? "1" : "0");
		if(n == 0) {
			out << ".";
		}
		++n;
	}
	return out;
}

std::ostream& operator<<(std::ostream& out, const std::vector<std::vector<bool>>& endings)
{
	out << "{";
	uint n = 0;
	for(auto ending: endings) {
		out << ending;
		++n;
		if(n < endings.size()) {
			out << ", ";
		}
	}
	out << "}";
	return out;
}

Endings::uint64 Endings::end(bool* const carry) const
{
	if(print) std::cerr << state << " Ending on: " << ending << "\n";
	
	assert(!ending.empty());
	if(carry != nullptr) {
		*carry = ending.at(0);
	}
	uint64 end = 0UL;
	uint64 bit = Interval::msb;
	for(auto i = ending.begin() + 1; i != ending.end(); ++i) {
		end |= *i ? bit : 0;
		bit >>= 1;
	}
	return end;
}

void Endings::reserve_current()
{
	if(print)  std::cerr << state << " Reserve: " << ending << "\n";
	assert(!ending.empty());
	reserved_endings.push_back(ending);
	ending.clear();
}

void Endings::prune(const Interval& interval)
{
	if(print) std::cerr << state << " prune " << interval << " " << reserved_endings << "\n";
	Set new_reserved_endings;
	for(const End& end: reserved_endings) {
		
		if(!is_valid(interval, end)) {
			continue;
		}
		
		new_reserved_endings.push_back(end);
	}
	reserved_endings = new_reserved_endings;
	if(print) std::cerr << " = " << reserved_endings << "\n";
}

void Endings::prune_carry()
{
	if(print)  std::cerr << state << " Prune carry: " << reserved_endings << "\n";
	if(state == s0p) {
		state = s1n;
		
		Set new_reserved_endings;
		for(End end: reserved_endings) {
			// Remove the items with the carry bit unset
			if(end.at(0) == false) {
				continue;
			}
			// unset it on the remaining endings.
			end.at(0) = false;
			new_reserved_endings.push_back(end);
		}
		reserved_endings = new_reserved_endings;
		
	} else if(state == s1p) {
		state = s1p;
		
		Set new_reserved_endings;
		for(End end: reserved_endings) {
			// Keep 1. as 0.
			if(end == End{true}) {
				end = End{false};
				new_reserved_endings.push_back(end);
				continue;
			}
			// Remove the items with the carry bit unset
			if(end.at(0) == false) {
				continue;
			}
			// unset it on the remaining endings.
			end.at(0) = false;
			new_reserved_endings.push_back(end);
		}
		reserved_endings = new_reserved_endings;
		
	} else {
		throw std::logic_error("Can not add carry in current state.");
	}
	if(print)  std::cerr << "= " << reserved_endings << "\n";
}

void Endings::prune_zero()
{
	if(print)  std::cerr << state << " Prune zero: " << reserved_endings << "\n";
	if(state == s0p || state == s1p) {
		state = s0p;
		
		Set new_reserved_endings;
		for(End end: reserved_endings) {
			// Keep 0. as is.
			if(end == End{false}) {
				new_reserved_endings.push_back(end);
				continue;
			}
			// Remove items with the carry bit set.
			if(end.at(0) == true) {
				continue;
			}
			// Shift the remaining items one place.
			end.erase(end.begin());
			new_reserved_endings.push_back(end);
		}
		reserved_endings = new_reserved_endings;
		
	} else if(state == s1n) {
		state = s0p;
		
		Set new_reserved_endings;
		for(End end: reserved_endings) {
			// Keep 0. as is.
			if(end == End{false}) {
				new_reserved_endings.push_back(end);
				continue;
			}
			// Shift all items one place.
			end.erase(end.begin());
			new_reserved_endings.push_back(end);
		}
		reserved_endings = new_reserved_endings;
		
	} else {
		throw std::logic_error("Invalid state.");
	}
	if(print)  std::cerr << "= " << reserved_endings << "\n";
}

void Endings::prune_one()
{
	if(print) std::cerr << state << " Prune one: " << reserved_endings << "\n";
	if(state == s0p) {
		state = s1p;
		
		// Keep 1. as is. Remove items starting with 1.1 or 0.0. Erase second bit.
		Set new_reserved_endings;
		for(End end: reserved_endings) {
			// Keep 1. as is.
			if(end == End{true}) {
				new_reserved_endings.push_back(end);
				continue;
			}
			// Remove items starting with 1.1 or 0.0
			if(end.size() < 2 || end.at(0) == end.at(1)) {
				continue;
			}
			// Erase second bit.
			end.erase(end.begin() + 1);
			new_reserved_endings.push_back(end);
		}
		reserved_endings = new_reserved_endings;
		
	} else if(state == s1n) {
		state = s1n;
		
		Set new_reserved_endings;
		for(End end: reserved_endings) {
			// Remove items with the second bit unset.
			if(end.size() < 2 || end.at(1) == false) {
				continue;
			}
			// Shift.
			end.erase(end.begin());
			// Unset carry bit.
			end.at(0) = false;
			new_reserved_endings.push_back(end);
		}
		reserved_endings = new_reserved_endings;
		
	} else if(state == s1p) {
		state = s1p;
		
		Set new_reserved_endings;
		for(End end: reserved_endings) {
			// Keep 1. as is.
			if(end == End{true}) {
				new_reserved_endings.push_back(end);
				continue;
			}
			// Remove items with the first bit equal to second bit
			if(end.size() < 2 || end.at(0) == end.at(1)) {
				continue;
			}
			// Shift, but keep carry bits intact.
			const bool carry = end.at(0);
			end.erase(end.begin());
			end.at(0) = carry;
			new_reserved_endings.push_back(end);
		}
		reserved_endings = new_reserved_endings;
		
	} else {
		throw std::logic_error("Invalid state.");
	}
	if(print) std::cerr << "= " << reserved_endings << "\n";
}

bool Endings::is_valid(const Interval& interval, const Endings::End& end)
{
	// Remove endings that require carry the interval excludes carry
	if(end.at(0) == true && !interval.is_goofy()) {
		return false;
	}
	
	// Calculate the value of the ending
	uint64 value = 0UL;
	uint64 bit = Interval::msb;
	for(auto i = end.begin() + 1; i != end.end(); ++i) {
		value |= *i ? bit : 0;
		bit >>= 1;
	}
	
	// Remove endings that are not in the interval
	if(!interval.includes(value)) {
		return false;
	}
	
	// Remove endings that require carry but don't specify it or
	// endings that specify carry but don't require it.
	if((value < interval.base) != end.at(0)) {
		return false;
	}
	return true;
}

bool Endings::is_reserved(const End& ending)
{
	return std::find(reserved_endings.begin(), reserved_endings.end(), ending) != reserved_endings.end();
}

void Endings::generate(const Interval& interval)
{
	if(print) std::cerr << state << " generate " << interval << " " << reserved_endings << "\n";
	assert(ending.empty());
	ending = std::vector<bool>{state == s1p};
	
	if(print) std::cerr << "Next: " << ending;
	if(print) std::cerr << (!is_valid(interval, ending) ? " invalid" : "");
	if(print) std::cerr << (is_reserved(ending) ? " reserved" : "");
	if(!is_reserved(ending) && is_valid(interval, ending)) {
		if(print) std::cerr << " available!\n";
		return;
	}
	if(print) std::cerr << "\n";
	
	if(state != s1n) {
		ending[0] = !ending[0];
		if(print) std::cerr << "Next: " << ending;
		if(print) std::cerr << (!is_valid(interval, ending) ? " invalid" : "");
		if(print) std::cerr << (is_reserved(ending) ? " reserved" : "");
		if(!is_reserved(ending) && is_valid(interval, ending)) {
			if(print) std::cerr << " available!\n";
			return;
		}
		if(print) std::cerr << "\n";
	}
	
	ending = std::vector<bool>{false, true};
	if(print) std::cerr << "Next: " << ending;
	if(print) std::cerr << (!is_valid(interval, ending) ? " invalid" : "");
	if(print) std::cerr << (is_reserved(ending) ? " reserved" : "");
	while(is_reserved(ending) || !is_valid(interval, ending)) {
		if(print) std::cerr << "\n";
		bool carry = true;
		int n = ending.size() - 2;
		while(carry && n >= 0) {
			carry = ending.at(n);
			ending.at(n) = !ending.at(n);
			--n;
		}
		if(carry) {
			for(auto bit: ending) {
				bit = false;
			}
			ending.push_back(true);
		}
		if(print) std::cerr << "Next: " << ending;
		if(print) std::cerr << (!is_valid(interval, ending) ? " invalid" : "");
		if(print) std::cerr << (is_reserved(ending) ? " reserved" : "");
	}
	if(print) std::cerr << " available!\n";
}

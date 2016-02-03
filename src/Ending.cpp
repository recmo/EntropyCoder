#include "Ending.h"
#include <stdexcept>

bool print = false;

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

Ending::uint64 Ending::end(bool* const carry)
{
	if(ending.empty()) {
		ending = next_available_ending();
	}
	
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

void Ending::reserve_current()
{
	if(ending.empty()) {
		ending = next_available_ending();
	}
	reserved_endings.push_back(ending);
	ending.clear();
}

void Ending::prune(const Interval& interval)
{
	// TODO
	/*
	assert(ending.size() >= 1);
	uint64 end = 0UL;
	uint64 bit = Interval::msb;
	for(auto i = ending.begin() + 1; i != ending.end(); ++i) {
		end |= *i ? bit : 0;
		bit >>= 1;
	}
	if(print) {
		std::cerr << " = ";
		std::cerr << "0x" << std::setw(16) << std::setfill('0') << std::hex;
		std::cerr  << end;
	}
	*/
	/*
	if(ending[0] == true) {
		return interval.is_goofy() && end < (interval.base + interval.range + 1);
	} else {
		if(interval.is_goofy()) {
			return end >= interval.base;
		} else {
			return interval.includes(end);
		}
	}
	*/
}

void Ending::prune_carry()
{
	std::cerr << state << " Prune carry: " << reserved_endings << "\n";
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
	std::cerr << "= " << reserved_endings << "\n";
}

void Ending::prune_zero()
{
	std::cerr << state << " Prune zero: " << reserved_endings << "\n";
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
	std::cerr << "= " << reserved_endings << "\n";
}

void Ending::prune_one()
{
	std::cerr << state << " Prune one: " << reserved_endings << "\n";
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
			// Remove items with the second bit unset.
			if(end.size() < 2 || end.at(1) == false) {
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
	std::cerr << "= " << reserved_endings << "\n";
}

bool Ending::is_valid(const Ending::End& ending)
{
	return true;
}


bool Ending::is_reserved(const End& ending)
{
	return std::find(reserved_endings.begin(), reserved_endings.end(), ending) != reserved_endings.end();
}

std::vector<bool> Ending::next_available_ending()
{
	std::vector<bool> ending{state == s1p};
	
	//std::cerr << "Next: " << ending;
	//print = true;
	//std::cerr << (!is_valid(ending) ? " invalid" : "");
	//std::cerr << (is_reserved(ending) ? " reserved" : "");
	//print = false;
	if(!is_reserved(ending) && is_valid(ending)) {
		//std::cerr << " available!\n";
		return ending;
	}
	//std::cerr << "\n";
	
	if(state != s1n) {
		ending[0] = !ending[0];
		//std::cerr << "Next: " << ending;
		//print = true;
		//std::cerr << (!is_valid(ending) ? " invalid" : "");
		//std::cerr << (is_reserved(ending) ? " reserved" : "");
		//print = false;
		if(!is_reserved(ending) && is_valid(ending)) {
			//std::cerr << " available!\n";
			return ending;
		}
		//std::cerr << "\n";
	}
	
	ending = std::vector<bool>{false, true};
	//std::cerr << "Next: " << ending;
	//print = true;
	//std::cerr << (!is_valid(ending) ? " invalid" : "");
	//std::cerr << (is_reserved(ending) ? " reserved" : "");
	//print = false;
	while(is_reserved(ending) || !is_valid(ending)) {
		//std::cerr << "\n";
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
		//std::cerr << "Next: " << ending;
		//print = true;
		//std::cerr << (!is_valid(ending) ? " invalid" : "");
		//std::cerr << (is_reserved(ending) ? " reserved" : "");
		//print = false;
	}
	//std::cerr << " available!\n";
	return ending;
}

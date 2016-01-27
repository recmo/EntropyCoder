#pragma once
#include <cstdint>
#include <tuple>

/// @returns The number of zeros in value starting from MSB 
std::uint64_t count_leading_zeros(std::uint64_t value);

/// @returns  ⌊a·b·2⁻⁶⁴⌋ and a·b mod 2⁶⁴
std::pair<std::uint64_t, std::uint64_t> mulq(std::uint64_t a, std::uint64_t b);

/// @returns  ⌊(h·2⁶⁴ + l + n) 2⁻⁶⁴⌋ and (h·2⁶⁴ + l + n) mod 2⁶⁴
std::pair<std::uint64_t, std::uint64_t> add128(std::uint64_t h, std::uint64_t l, std::uint64_t n);



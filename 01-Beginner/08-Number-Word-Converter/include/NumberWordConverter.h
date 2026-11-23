#pragma once

#include <cstdint>
#include <string>

namespace number_word_converter {

// Converts a 64-bit integer into English words (e.g. 1042 -> "One Thousand Forty-Two")
std::string numberToWords(int64_t number);

// Converts a decimal amount into currency text (e.g. 1042.50 -> "One Thousand Forty-Two Dollars and Fifty Cents")
std::string currencyToWords(double amount, const std::string& currencyUnit = "Dollar");

// Helper to convert a three-digit chunk (< 1000) into words
std::string chunkToWords(int chunk);

} // namespace number_word_converter

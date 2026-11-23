#include "NumberWordConverter.h"

#include <array>
#include <cmath>
#include <cstdint>
#include <sstream>
#include <vector>

namespace number_word_converter {

static const std::array<std::string, 20> kUnderTwenty = {
    "", "One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine",
    "Ten", "Eleven", "Twelve", "Thirteen", "Fourteen", "Fifteen", "Sixteen",
    "Seventeen", "Eighteen", "Nineteen"
};

static const std::array<std::string, 10> kTens = {
    "", "", "Twenty", "Thirty", "Forty", "Fifty", "Sixty", "Seventy", "Eighty", "Ninety"
};

static const std::array<std::string, 6> kScales = {
    "", "Thousand", "Million", "Billion", "Trillion", "Quadrillion"
};

std::string chunkToWords(int chunk) {
    if (chunk == 0) return "";

    std::string result;
    int hundreds = chunk / 100;
    int remainder = chunk % 100;

    if (hundreds > 0) {
        result += kUnderTwenty[hundreds] + " Hundred";
        if (remainder > 0) {
            result += " ";
        }
    }

    if (remainder > 0) {
        if (remainder < 20) {
            result += kUnderTwenty[remainder];
        } else {
            int tensDigit = remainder / 10;
            int onesDigit = remainder % 10;
            result += kTens[tensDigit];
            if (onesDigit > 0) {
                result += "-" + kUnderTwenty[onesDigit];
            }
        }
    }

    return result;
}

std::string numberToWords(int64_t number) {
    if (number == 0) return "Zero";

    std::string prefix;
    uint64_t absVal = 0;

    if (number < 0) {
        prefix = "Negative ";
        // Safe negation handling
        absVal = static_cast<uint64_t>(-(number + 1)) + 1;
    } else {
        absVal = static_cast<uint64_t>(number);
    }

    std::vector<std::string> parts;
    size_t scaleIndex = 0;

    while (absVal > 0) {
        int chunk = static_cast<int>(absVal % 1000);
        if (chunk > 0) {
            std::string chunkStr = chunkToWords(chunk);
            if (!kScales[scaleIndex].empty()) {
                chunkStr += " " + kScales[scaleIndex];
            }
            parts.push_back(chunkStr);
        }
        absVal /= 1000;
        scaleIndex++;
    }

    std::ostringstream oss;
    oss << prefix;
    for (auto it = parts.rbegin(); it != parts.rend(); ++it) {
        if (it != parts.rbegin()) {
            oss << " ";
        }
        oss << *it;
    }

    return oss.str();
}

std::string currencyToWords(double amount, const std::string& currencyUnit) {
    if (std::isnan(amount)) return "Invalid Amount";

    bool isNegative = (amount < 0.0);
    double absAmount = std::abs(amount);

    int64_t wholePart = static_cast<int64_t>(absAmount);
    int cents = static_cast<int>(std::round((absAmount - static_cast<double>(wholePart)) * 100.0));

    if (cents >= 100) {
        wholePart += 1;
        cents -= 100;
    }

    std::string result;
    if (isNegative) {
        result += "Negative ";
    }

    result += numberToWords(wholePart) + " " + currencyUnit;
    if (wholePart != 1) {
        result += "s";
    }

    result += " and " + numberToWords(cents) + (cents == 1 ? " Cent" : " Cents");
    return result;
}

} // namespace number_word_converter

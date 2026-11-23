#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "NumberWordConverter.h"

using namespace number_word_converter;

TEST_CASE("Zero and single digits") {
    CHECK(numberToWords(0) == "Zero");
    CHECK(numberToWords(1) == "One");
    CHECK(numberToWords(5) == "Five");
    CHECK(numberToWords(9) == "Nine");
}

TEST_CASE("Teens and compound tens") {
    CHECK(numberToWords(10) == "Ten");
    CHECK(numberToWords(11) == "Eleven");
    CHECK(numberToWords(14) == "Fourteen");
    CHECK(numberToWords(19) == "Nineteen");
    CHECK(numberToWords(20) == "Twenty");
    CHECK(numberToWords(21) == "Twenty-One");
    CHECK(numberToWords(42) == "Forty-Two");
    CHECK(numberToWords(99) == "Ninety-Nine");
}

TEST_CASE("Hundreds and Thousands") {
    CHECK(numberToWords(100) == "One Hundred");
    CHECK(numberToWords(105) == "One Hundred Five");
    CHECK(numberToWords(123) == "One Hundred Twenty-Three");
    CHECK(numberToWords(1000) == "One Thousand");
    CHECK(numberToWords(1042) == "One Thousand Forty-Two");
    CHECK(numberToWords(20005) == "Twenty Thousand Five");
    CHECK(numberToWords(100000) == "One Hundred Thousand");
}

TEST_CASE("Millions, Billions, Trillions") {
    CHECK(numberToWords(1000000) == "One Million");
    CHECK(numberToWords(1000000000LL) == "One Billion");
    CHECK(numberToWords(1000000000000LL) == "One Trillion");
    CHECK(numberToWords(123456789LL) == "One Hundred Twenty-Three Million Four Hundred Fifty-Six Thousand Seven Hundred Eighty-Nine");
}

TEST_CASE("Negative numbers") {
    CHECK(numberToWords(-1) == "Negative One");
    CHECK(numberToWords(-42) == "Negative Forty-Two");
    CHECK(numberToWords(-1000) == "Negative One Thousand");
}

TEST_CASE("Currency conversion") {
    CHECK(currencyToWords(0.0) == "Zero Dollars and Zero Cents");
    CHECK(currencyToWords(1.0) == "One Dollar and Zero Cents");
    CHECK(currencyToWords(1.01) == "One Dollar and One Cent");
    CHECK(currencyToWords(42.50) == "Forty-Two Dollars and Fifty Cents");
}

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "BillSplitter.h"

using namespace bill_splitter;

TEST_CASE("Equal split with exact division") {
    // 100 subtotal, 10% tax = 10, 20% tip = 20 -> 130 total across 2 people = 65 each
    auto summary = calculateEqualSplit(100.0, 2, 20.0, 10.0);
    REQUIRE(summary.has_value());
    CHECK(summary->subtotal == doctest::Approx(100.0));
    CHECK(summary->taxAmount == doctest::Approx(10.0));
    CHECK(summary->tipAmount == doctest::Approx(20.0));
    CHECK(summary->grandTotal == doctest::Approx(130.0));
    CHECK(summary->perPersonBase == doctest::Approx(65.0));
    CHECK(summary->individualShares[0] == doctest::Approx(65.0));
    CHECK(summary->individualShares[1] == doctest::Approx(65.0));
}

TEST_CASE("Penny remainder distribution") {
    // 10.00 subtotal, 0% tax, 0% tip across 3 people = 10.00 / 3
    // 1000 cents / 3 = 333 cents with 1 cent remainder
    // Person 1 gets $3.34, Person 2 gets $3.33, Person 3 gets $3.33 -> Sum = $10.00 exactly!
    auto summary = calculateEqualSplit(10.0, 3, 0.0, 0.0);
    REQUIRE(summary.has_value());
    CHECK(summary->grandTotal == doctest::Approx(10.0));
    CHECK(summary->individualShares[0] == doctest::Approx(3.34));
    CHECK(summary->individualShares[1] == doctest::Approx(3.33));
    CHECK(summary->individualShares[2] == doctest::Approx(3.33));

    double sum = summary->individualShares[0] + summary->individualShares[1] + summary->individualShares[2];
    CHECK(sum == doctest::Approx(10.0));
}

TEST_CASE("Invalid inputs rejected") {
    CHECK(calculateEqualSplit(-50.0, 2).has_value() == false);
    CHECK(calculateEqualSplit(100.0, 0).has_value() == false);
    CHECK(calculateEqualSplit(100.0, 2, -5.0).has_value() == false);
}

TEST_CASE("Itemized split calculation") {
    std::vector<Item> items = {
        {"Burger", 10.0, {"Alice"}},
        {"Pizza", 20.0, {"Alice", "Bob"}}, // 10 each
        {"Salad", 10.0, {"Bob"}}
    };
    // Subtotal: 40.0. Alice base: 20, Bob base: 20
    // With 0% tax and 0% tip:
    auto summary = calculateItemizedSplit(items, 0.0, 0.0);
    REQUIRE(summary.has_value());
    CHECK(summary->subtotal == doctest::Approx(40.0));
    CHECK(summary->personOwes["Alice"] == doctest::Approx(20.0));
    CHECK(summary->personOwes["Bob"] == doctest::Approx(20.0));
}

TEST_CASE("Itemized string parsing") {
    std::string raw = "Burger:15.5:Alice;Pasta:18.0:Bob;Dessert:10.0:Alice,Bob";
    auto items = parseItemizedString(raw);
    REQUIRE(items.size() == 3);
    CHECK(items[0].name == "Burger");
    CHECK(items[0].price == doctest::Approx(15.5));
    CHECK(items[0].sharedBy == std::vector<std::string>{"Alice"});
    CHECK(items[2].sharedBy.size() == 2);
}

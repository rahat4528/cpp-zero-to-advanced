#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "QuoteGenerator.h"

using namespace quote_generator;

TEST_CASE("Quote parsing logic") {
    auto qOpt = QuoteGenerator::parseLine("To be or not to be|William Shakespeare|Drama");
    REQUIRE(qOpt.has_value());
    CHECK(qOpt->text == "To be or not to be");
    CHECK(qOpt->author == "William Shakespeare");
    CHECK(qOpt->category == "Drama");

    // Comment and empty lines
    CHECK(QuoteGenerator::parseLine("# A comment").has_value() == false);
    CHECK(QuoteGenerator::parseLine("").has_value() == false);
}

TEST_CASE("Deterministic seed produces identical sequence") {
    QuoteGenerator gen1(42);
    QuoteGenerator gen2(42);

    Quote q1{"Quote A", "Author 1", "Cat1"};
    Quote q2{"Quote B", "Author 2", "Cat1"};
    Quote q3{"Quote C", "Author 3", "Cat2"};

    for (const auto& q : {q1, q2, q3}) {
        gen1.addQuote(q);
        gen2.addQuote(q);
    }

    auto r1 = gen1.getRandomQuote();
    auto r2 = gen2.getRandomQuote();

    REQUIRE(r1.has_value());
    REQUIRE(r2.has_value());
    CHECK(r1->text == r2->text);
}

TEST_CASE("Category and author filtering") {
    QuoteGenerator gen;
    gen.addQuote({"Q1", "Linus Torvalds", "Technology"});
    gen.addQuote({"Q2", "Albert Einstein", "Science"});
    gen.addQuote({"Q3", "Bjarne Stroustrup", "Technology"});

    auto techQuotes = gen.getQuotesByCategory("Technology");
    CHECK(techQuotes.size() == 2);

    auto einsteinQuotes = gen.getQuotesByAuthor("einstein");
    REQUIRE(einsteinQuotes.size() == 1);
    CHECK(einsteinQuotes[0].text == "Q2");
}

TEST_CASE("Empty generator handles gracefully") {
    QuoteGenerator gen;
    CHECK(gen.getRandomQuote().has_value() == false);
    CHECK(gen.getRandomQuotes(5).empty());
}

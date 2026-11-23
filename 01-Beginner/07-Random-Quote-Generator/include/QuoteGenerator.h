#pragma once

#include <optional>
#include <random>
#include <string>
#include <vector>

namespace quote_generator {

struct Quote {
    std::string text;
    std::string author;
    std::string category;
};

class QuoteGenerator {
public:
    // Construct with optional deterministic seed (useful for reproducible unit testing)
    explicit QuoteGenerator(std::optional<unsigned int> seed = std::nullopt);

    // Load quotes from formatted file (Text|Author|Category)
    bool loadQuotesFromFile(const std::string& filepath);
    void addQuote(const Quote& quote);

    // Retrieval
    std::optional<Quote> getRandomQuote();
    std::vector<Quote> getRandomQuotes(size_t count);

    // Filtering
    std::vector<Quote> getQuotesByCategory(const std::string& category) const;
    std::vector<Quote> getQuotesByAuthor(const std::string& author) const;
    std::vector<std::string> getCategories() const;

    size_t size() const { return m_quotes.size(); }
    void clear() { m_quotes.clear(); }

    static std::optional<Quote> parseLine(const std::string& line);

private:
    std::vector<Quote> m_quotes;
    std::mt19937 m_rng;
};

} // namespace quote_generator

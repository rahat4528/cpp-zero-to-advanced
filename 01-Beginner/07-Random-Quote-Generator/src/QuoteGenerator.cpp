#include "QuoteGenerator.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <set>
#include <sstream>

namespace quote_generator {

static std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return s;
}

QuoteGenerator::QuoteGenerator(std::optional<unsigned int> seed) {
    if (seed.has_value()) {
        m_rng.seed(*seed);
    } else {
        std::random_device rd;
        m_rng.seed(rd());
    }
}

void QuoteGenerator::addQuote(const Quote& quote) {
    if (!quote.text.empty() && !quote.author.empty()) {
        m_quotes.push_back(quote);
    }
}

std::optional<Quote> QuoteGenerator::parseLine(const std::string& line) {
    if (line.empty() || line.front() == '#') return std::nullopt;

    std::istringstream iss(line);
    Quote q;
    if (!std::getline(iss, q.text, '|')) return std::nullopt;
    if (!std::getline(iss, q.author, '|')) return std::nullopt;
    if (!std::getline(iss, q.category)) {
        q.category = "General";
    }

    if (q.text.empty() || q.author.empty()) return std::nullopt;
    return q;
}

bool QuoteGenerator::loadQuotesFromFile(const std::string& filepath) {
    std::ifstream in(filepath);
    if (!in.is_open()) return false;

    std::string line;
    while (std::getline(in, line)) {
        auto qOpt = parseLine(line);
        if (qOpt) {
            m_quotes.push_back(*qOpt);
        }
    }
    return !m_quotes.empty();
}

std::optional<Quote> QuoteGenerator::getRandomQuote() {
    if (m_quotes.empty()) return std::nullopt;
    std::uniform_int_distribution<size_t> dist(0, m_quotes.size() - 1);
    return m_quotes[dist(m_rng)];
}

std::vector<Quote> QuoteGenerator::getRandomQuotes(size_t count) {
    if (m_quotes.empty() || count == 0) return {};

    std::vector<Quote> results;
    results.reserve(std::min(count, m_quotes.size()));

    // Sample without replacement if count <= size
    std::vector<Quote> pool = m_quotes;
    std::shuffle(pool.begin(), pool.end(), m_rng);

    size_t n = std::min(count, pool.size());
    for (size_t i = 0; i < n; ++i) {
        results.push_back(pool[i]);
    }
    return results;
}

std::vector<Quote> QuoteGenerator::getQuotesByCategory(const std::string& category) const {
    std::vector<Quote> results;
    std::string lCat = toLower(category);
    for (const auto& q : m_quotes) {
        if (toLower(q.category) == lCat) {
            results.push_back(q);
        }
    }
    return results;
}

std::vector<Quote> QuoteGenerator::getQuotesByAuthor(const std::string& author) const {
    std::vector<Quote> results;
    std::string lAuth = toLower(author);
    for (const auto& q : m_quotes) {
        if (toLower(q.author).find(lAuth) != std::string::npos) {
            results.push_back(q);
        }
    }
    return results;
}

std::vector<std::string> QuoteGenerator::getCategories() const {
    std::set<std::string> cats;
    for (const auto& q : m_quotes) {
        cats.insert(q.category);
    }
    return std::vector<std::string>(cats.begin(), cats.end());
}

} // namespace quote_generator

#include "QuoteGenerator.h"

#include <iostream>
#include <string>

void printHelp() {
    std::cout << "Random Quote Generator CLI\n"
              << "Usage:\n"
              << "  RandomQuote                       Display one random quote\n"
              << "  RandomQuote --count <n>           Display N random quotes\n"
              << "  RandomQuote --cat <category>      Filter by category\n"
              << "  RandomQuote --author <name>       Filter by author\n"
              << "  RandomQuote --list-cats           List available categories\n"
              << "  RandomQuote --file <path>         Specify quotes file\n"
              << "  RandomQuote --seed <uint>         Use deterministic seed\n"
              << "  RandomQuote --help                Show help\n";
}

void printQuote(const quote_generator::Quote& q) {
    std::cout << "\n\"" << q.text << "\"\n"
              << "       -- " << q.author << " [" << q.category << "]\n";
}

int main(int argc, char* argv[]) {
    std::string quotesFile = "resources/quotes.txt";
    std::optional<unsigned int> seed;
    std::string categoryFilter;
    std::string authorFilter;
    size_t count = 1;
    bool listCats = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--file" && i + 1 < argc) {
            quotesFile = argv[++i];
        } else if (arg == "--cat" && i + 1 < argc) {
            categoryFilter = argv[++i];
        } else if (arg == "--author" && i + 1 < argc) {
            authorFilter = argv[++i];
        } else if (arg == "--count" && i + 1 < argc) {
            count = std::stoul(argv[++i]);
        } else if (arg == "--seed" && i + 1 < argc) {
            seed = static_cast<unsigned int>(std::stoul(argv[++i]));
        } else if (arg == "--list-cats") {
            listCats = true;
        }
    }

    quote_generator::QuoteGenerator gen(seed);
    if (!gen.loadQuotesFromFile(quotesFile)) {
        // Fallback embedded quotes if file not in relative path
        gen.addQuote({"There are only two kinds of languages: the ones people complain about and the ones nobody uses.", "Bjarne Stroustrup", "Technology"});
        gen.addQuote({"Premature optimization is the root of all evil.", "Donald Knuth", "Technology"});
        gen.addQuote({"Simplicity is prerequisite for reliability.", "Edsger W. Dijkstra", "Technology"});
    }

    if (listCats) {
        std::cout << "Categories:\n";
        for (const auto& cat : gen.getCategories()) {
            std::cout << "  - " << cat << "\n";
        }
        return 0;
    }

    if (!categoryFilter.empty()) {
        auto matches = gen.getQuotesByCategory(categoryFilter);
        if (matches.empty()) {
            std::cout << "No quotes found for category: " << categoryFilter << "\n";
            return 0;
        }
        for (const auto& q : matches) printQuote(q);
        return 0;
    }

    if (!authorFilter.empty()) {
        auto matches = gen.getQuotesByAuthor(authorFilter);
        if (matches.empty()) {
            std::cout << "No quotes found for author: " << authorFilter << "\n";
            return 0;
        }
        for (const auto& q : matches) printQuote(q);
        return 0;
    }

    auto quotes = gen.getRandomQuotes(count);
    for (const auto& q : quotes) {
        printQuote(q);
    }
    return 0;
}
